#include "PortalGame.h"
#include "PortalAI.h"
#include "PortalUtils.h"
#include "HexEditorActor.h"
#include "HexGame.h"

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

		T_Graph::EdgeIterator edgeIterator(m_Graph, fromIndex);
		auto* edge = edgeIterator.begin();
		while (!edgeIterator.end())
		{
			auto toIndex = edge->To();
			auto toPos = m_Graph.GetNode(toIndex)->Position;

			toPos.Z += GetTileType(m_Graph.GetNode(toIndex)->TileActor) == 0 ? 30 : 90;

			DrawDebugLine(gHexGame->GetWorld(), fromPos, toPos, FColor::Blue);

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
}
