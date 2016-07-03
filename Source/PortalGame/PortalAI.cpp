#include "PortalGame.h"
#include "PortalAI.h"
#include "PortalUtils.h"
#include "HexEditorActor.h"
#include "FinishActor.h"
#include "PlatformActor.h"
#include "CompanionActor.h"
#include "BarrierActor.h"
#include "HexGame.h"
#include "GraphUtils.h"
#include "GraphSearchAStar.h"

#include <unordered_set>

//========================================================================
auto GetTileType = [](auto* hexTile)
{
	auto* mesh = hexTile->GetStaticMeshComponent()->StaticMesh;
	auto& meshes = gHexEditor->AvailableTiles;
	int i = 0;
	for (; i < meshes.Num(); ++i)
	{
		if (mesh == meshes[i])
		{
			break;
		}
	}
	check(i < meshes.Num());
	return i;
};

//========================================================================
void C_PortalAI::Update(float dt)
{
	DebugDrawGraph(dt);
}

//========================================================================
void C_PortalAI::DebugDrawGraph(float dt)
{
	T_Graph::NodeIterator nodeIterator(m_Graph);
	auto* node = nodeIterator.begin();
	while (!nodeIterator.end())
	{
		auto fromIndex = node->GetIndex();
		auto fromPos = m_Graph.GetNode(fromIndex)->Position;

		fromPos.Z += GetTileType(node->TileActor) == 0 ? 30 : 90;

		DrawDebugString(gHexGame->GetWorld(), fromPos, FString::FromInt(fromIndex), 0, FColor::White, dt);

		T_Graph::EdgeIterator edgeIterator(m_Graph, fromIndex);
		auto* edge = edgeIterator.begin();
		while (!edgeIterator.end())
		{
			auto toIndex = edge->To();
			auto toPos = m_Graph.GetNode(toIndex)->Position;

			toPos.Z += GetTileType(m_Graph.GetNode(toIndex)->TileActor) == 0 ? 30 : 90;

			DrawDebugLine(gHexGame->GetWorld(), fromPos, toPos, edge->m_Enabled ? FColor::Green : FColor::Red, false, dt*3);

			edge = edgeIterator.next();
		}

		node = nodeIterator.next();
	}
}

//========================================================================
void C_PortalAI::Generate()
{
	auto& hexGrid = gHexEditor->GetHexGrid();

	auto& storage = hexGrid.GetStorage();

	//create nodes
	for (auto& pair : storage)
	{
		auto hexCoords = pair.first;
		auto* hexTileActor = pair.second;

		auto type = GetTileType(hexTileActor);
		if(type == 2) continue;

		if (hexTileActor->GraphIndex == INVALID_INDEX)
		{
			auto* newNode = new C_GraphNode;
			auto newIndex = m_Graph.AddNode(newNode);
			newNode->SetIndex(newIndex);
			hexTileActor->GraphIndex = newIndex;
			newNode->Position = hexGrid.GetPosition(hexCoords);
			newNode->TileActor = hexTileActor;
		}
	}

	//create edges
	std::unordered_set<std::pair<T_GraphIndex, T_GraphIndex>> toConnect; //lower first

	auto insert = [&](auto* t1, auto* t2) 
	{
		auto p = std::make_pair(t1->GraphIndex, t2->GraphIndex);  
		if (p.first > p.second)
		{
			std::swap(p.first, p.second);
		}
		toConnect.insert(p);
	};

	for (auto& pair : storage)
	{
		auto hexCoords = pair.first;
		auto* hexTileActor = pair.second;

		if (hexTileActor->GraphIndex == INVALID_INDEX) continue;

		auto type = GetTileType(hexTileActor);

		std::vector<S_HexCoordinates> neighbors(hexGrid.HorizontalNeighborIndexes.begin(), hexGrid.HorizontalNeighborIndexes.end());
		std::transform(hexGrid.HorizontalNeighborIndexes.begin(), hexGrid.HorizontalNeighborIndexes.end(),
			std::back_inserter(neighbors), [](auto& c) { return c + S_HexCoordinates(0, 0, 1); });

		for (auto n : neighbors)
		{
			bool z = n.z != 0;
			auto neighborCoords = hexCoords + n;
			auto* neighbor = hexGrid.GetElement(neighborCoords);

			if (!neighbor || neighbor->GraphIndex == INVALID_INDEX)
			{
				continue;
			}

			auto ntype = GetTileType(neighbor);
			if (type == 0 && ntype == 0 && !z)
			{
				insert(hexTileActor, neighbor);
			}
			else if (type == 0 && ntype == 1)
			{
				int rot = (neighbor->GetActorRotation().Yaw + 10.0f) / 60.0f; //safe offset
				if (hexGrid.HorizontalNeighborIndexes[6-rot] == n)
				{
					insert(hexTileActor, neighbor);
				}
			}
			else if (type == 1 && ntype == 0 && z)
			{
				int rot = (hexTileActor->GetActorRotation().Yaw + 10.0f) / 60.0f; //safe offset
				if (hexGrid.HorizontalNeighborIndexes[6-rot] == S_HexCoordinates(n.s, n.t))
				{
					insert(hexTileActor, neighbor);
				}
			}
		}
	}

	for (auto connectpair : toConnect)
	{
		auto* edge1 = new C_GraphEdge(connectpair.first, connectpair.second);
		auto* edge2 = new C_GraphEdge(connectpair.second, connectpair.first);
		m_Graph.AddEdge(edge1);
		m_Graph.AddEdge(edge2);
	}

	//barriers
	for (auto* b : gHexEditor->m_AllBarriers)
	{
		auto pair = b->GetNeighbors();
		auto from = pair.first.neighbor->GraphIndex;
		if (from != INVALID_INDEX && pair.second.neighbor && pair.second.neighbor->GraphIndex != INVALID_INDEX)
		{
			m_Graph.GetEdge(from, pair.second.neighbor->GraphIndex)->m_Enabled = false;
			m_Graph.GetEdge(pair.second.neighbor->GraphIndex, from)->m_Enabled = false;
		}

		m_Barriers.emplace_back();
		m_Barriers.back().on = true;
		m_Barriers.back().m_Neighbors.first.neighbor = pair.first.neighbor;
		m_Barriers.back().m_Neighbors.first.slotAtNeighbor = pair.first.slotAtNeighbor;
		m_Barriers.back().m_Neighbors.second.neighbor = pair.second.neighbor;
		m_Barriers.back().m_Neighbors.second.slotAtNeighbor = pair.second.slotAtNeighbor;
		m_Barriers.back().m_Id = b->GetId();
	}
	
	if (!gHexEditor->m_Finish)
	{
		print_frame("NO AI GENERATED, FINISH IS NOT PLACED", 3.0f);
		return;
	}

	auto GetGraphNode = [&](auto* actor) 
	{
		auto pos = actor->GetActorLocation();
		auto* element = hexGrid.GetElement(hexGrid.GetCoordinates(pos));
		return m_Graph.GetNode(element->GraphIndex);
	};

	auto* fin = new C_AIFinish;
	auto* node = GetGraphNode(gHexEditor->m_Finish);
	node->AIElements.push_back(fin);
	fin->m_CurrentIndex = node->GetIndex();

	for (auto* platform : gHexEditor->m_AllPlatforms)
	{
		auto targetId = platform->GetTarget()->GetId();
		auto it = std::find_if(Cont(m_Barriers), [&](auto& bar) { return bar.m_Id == targetId; });
		check(it != m_Barriers.end());

		auto* p = new C_AIPlatform;
		p->m_Target = &*it;
		auto* tile = GetGraphNode(platform);
		tile->AIElements.push_back(p);
		m_Platforms.push_back(p);
		p->m_CurrentIndex = tile->GetIndex();
	}

	for (auto* cube : gHexEditor->m_AllCompanions)
	{
		auto* c = new C_AICube;
		auto* node = GetGraphNode(cube);
		node->AIElements.push_back(c);
		m_Cubes.push_back(c);
		c->m_CurrentIndex = node->GetIndex();
	}

	m_ActorPos = gHexEditor->GraphIndex;
}

//========================================================================
void C_PortalAI::Solve()
{
	std::vector<T_GraphIndex> openlist;

	using AStar = C_GraphSearchAStar<T_Graph, Heuristic_Dijkstra>;
	
	std::function<bool(AStar::edge_const_t&)> traversePred = [](AStar::edge_const_t& edge) { return edge->m_Enabled; };

	AStar search(	m_Graph, m_ActorPos, INVALID_INDEX, false,
								traversePred,
								AStar::NoCallback,
								&openlist);

	std::vector<C_AIElement*> elements;
	for (auto nodeId : openlist)
	{
		auto& elms = m_Graph.GetNode(nodeId)->AIElements;
		for (auto* e : elms)
		{
			elements.push_back(e);
		}
	}

	elements.size();
}

//========================================================================