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
#include "Dude.h"
#include "DudeActions.h"

#include <unordered_set>
#include <string>
#include <sstream>

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
	T_Graph::NodeIterator nodeIterator(m_InitialState.m_Graph);
	auto* node = nodeIterator.begin();
	while (!nodeIterator.end())
	{
		auto fromIndex = node->GetIndex();
		auto fromPos = m_InitialState.m_Graph.GetNode(fromIndex)->Position;

		fromPos.Z += GetTileType(node->TileActor) == 0 ? 30 : 90;

		DrawDebugString(gHexGame->GetWorld(), fromPos, FString::FromInt(fromIndex), 0, FColor::White, dt);

		T_Graph::EdgeIterator edgeIterator(m_InitialState.m_Graph, fromIndex);
		auto* edge = edgeIterator.begin();
		while (!edgeIterator.end())
		{
			auto toIndex = edge->To();
			auto toPos = m_InitialState.m_Graph.GetNode(toIndex)->Position;

			toPos.Z += GetTileType(m_InitialState.m_Graph.GetNode(toIndex)->TileActor) == 0 ? 30 : 90;

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
	hexGrid.GetElement({0,0,0})->GraphIndex = INVALID_INDEX;

	m_InitialState.Clear();

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
			auto newIndex = m_InitialState.m_Graph.AddNode(newNode);
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
		m_InitialState.m_Graph.AddEdge(edge1);
		m_InitialState.m_Graph.AddEdge(edge2);
	}

	//barriers
	m_InitialState.m_Barriers.reserve(gHexEditor->m_AllBarriers.size());
	for (auto* b : gHexEditor->m_AllBarriers)
	{
		auto pair = b->GetNeighbors();

		m_InitialState.m_Barriers.emplace_back();
		m_InitialState.m_Barriers.back().m_Neighbors.first.neighbor = pair.first.neighbor;
		m_InitialState.m_Barriers.back().m_Neighbors.first.slotAtNeighbor = pair.first.slotAtNeighbor;
		m_InitialState.m_Barriers.back().m_Neighbors.second.neighbor = pair.second.neighbor;
		m_InitialState.m_Barriers.back().m_Neighbors.second.slotAtNeighbor = pair.second.slotAtNeighbor;
		m_InitialState.m_Barriers.back().m_Id = b->GetId();
		m_InitialState.m_Barriers.back().walkEnabledRefCount = 1; //to be lowered in next command
		m_InitialState.m_Barriers.back().enableWalk(false, m_InitialState.m_Graph);
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
		return m_InitialState.m_Graph.GetNode(element->GraphIndex);
	};

	m_InitialState.m_Finish = new C_AIFinish;
	auto* node = GetGraphNode(gHexEditor->m_Finish);
	node->AIElements.push_back(m_InitialState.m_Finish);
	m_InitialState.m_Finish->m_CurrentIndex = node->GetIndex();
	m_InitialState.m_Finish->m_RealFinish = gHexEditor->m_Finish;

	m_InitialState.m_Platforms.reserve(gHexEditor->m_AllPlatforms.size());
	for (auto* platform : gHexEditor->m_AllPlatforms)
	{
		auto targetId = platform->GetTarget()->GetId();
		auto it = std::find_if(Cont(m_InitialState.m_Barriers), [&](auto& bar) { return bar.m_Id == targetId; });
		check(it != m_InitialState.m_Barriers.end());

		m_InitialState.m_Platforms.emplace_back();
		auto& p = m_InitialState.m_Platforms.back();
		p.m_Target = &*it;
		auto* tile = GetGraphNode(platform);
		tile->AIElements.push_back(&p);
		p.m_CurrentIndex = tile->GetIndex();
		p.m_RealPlatform = platform;
	}

	m_InitialState.m_Cubes.reserve(gHexEditor->m_AllCompanions.size());
	for (auto* cube : gHexEditor->m_AllCompanions)
	{
		m_InitialState.m_Cubes.emplace_back();
		auto& c = m_InitialState.m_Cubes.back();
		auto* node = GetGraphNode(cube);
		node->AIElements.push_back(&c);
		c.m_CurrentIndex = node->GetIndex();
		c.m_RealCube = cube;
	}

	m_InitialState.m_ActorPos = gHexEditor->GraphIndex;
}

//========================================================================
void C_PortalAI::Solve()
{
	std::vector<C_AICube*> cubeElements;
	std::vector<C_AIPlatform*> platformElements;
	bool finished = false;

	auto getElements = [&](const S_State& state)
	{
		finished = false;
		cubeElements.clear();
		platformElements.clear();
		std::vector<T_GraphIndex> openlist;

		using AStar = C_GraphSearchAStar<T_Graph, Heuristic_Dijkstra>;
	
		std::function<bool(AStar::edge_const_t&)> traversePred = [](AStar::edge_const_t& edge) { return edge->m_Enabled; };

		AStar search(state.m_Graph, state.m_ActorPos, INVALID_INDEX, false,
									traversePred,
									AStar::NoCallback,
									&openlist);

		for (auto nodeId : openlist)
		{
			auto& elms = state.m_Graph.GetNode(nodeId)->AIElements;
			for (auto* e : elms)
			{
				if (e->GetType() == C_AIElement::cube)
				{
					cubeElements.push_back(static_cast<C_AICube*>(e));
				}
				else if (e->GetType() == C_AIElement::platform)
				{
					platformElements.push_back(static_cast<C_AIPlatform*>(e));
				}
				else if (e->GetType() == C_AIElement::finish)
				{
					finished = true;
				}
			}
		}
	};

	unsigned currId = 0;
	std::vector<S_State> visited;
	visited.reserve(10000);
	visited.push_back(m_InitialState);
	m_InitialState.CloneTo(visited.back());

	std::vector<std::pair<ACompanionActor*, APlatformActor*>> actions;
	actions.resize(10000);

	while (currId < visited.size())
	{
		auto& state = visited[currId];

		getElements(state);
		if (finished)
		{
			std::vector<std::pair<ACompanionActor*, APlatformActor*>> finalActions;
			unsigned stateId = currId;
			while (stateId != 0)
			{
				finalActions.push_back(actions[stateId]);
				stateId = visited[stateId].previousStateId;
			}
			std::reverse(finalActions.begin(), finalActions.end());
			auto& dude = *gHexGame->Dude;
			dude.ClearActionQueue();
			for (auto a : finalActions)
			{
				dude.PushAction(*new C_DudeWait(dude, 0.1f), false);
				auto* gotoAction1 = new C_DudeMoveTo(dude);
				gotoAction1->target = a.first->GetActorLocation();
				auto* pickupAction = new C_DudePick(dude);
				pickupAction->companion = a.first;
				auto* gotoAction2 = new C_DudeMoveTo(dude);
				gotoAction2->target = a.second->GetActorLocation();
				auto* dropAction = new C_DudeDrop(dude);
				dude.PushAction(*gotoAction1, false);
				dude.PushAction(*pickupAction, false);
				dude.PushAction(*new C_DudeWait(dude, 1.0f), false);
				dude.PushAction(*gotoAction2, false);
				dude.PushAction(*dropAction, false);
			}
			dude.PushAction(*new C_DudeWait(dude, 0.1f), false);
			auto* gotoFinish = new C_DudeMoveTo(dude);
			gotoFinish->target = m_InitialState.m_Finish->m_RealFinish->GetActorLocation();
			dude.PushAction(*gotoFinish, false);
			dude.StartActions();
			break;
		}

		for (auto* cube : cubeElements)
		{
			for (auto* platform : platformElements)
			{
				if (/*cube->m_PlacedOn == nullptr && */platform->m_On == false) //free cube and platform
				{
					check(visited.size() < 10000);
					visited.push_back(state);
					auto& newState = visited.back();
					state.CloneTo(newState);

					auto* p = state.Get<C_AIPlatform*>(platform);
					auto* c = state.Get<C_AICube*>(cube);

					auto* currentCompanionActor = c->m_RealCube;
					auto* currentPlatform = p->m_RealPlatform;

					bool wasPlacedOnSomething;
					c->PickUp(newState.m_Graph, wasPlacedOnSomething);
					newState.m_ActorPos = c->m_CurrentIndex;

					//is it still reachable? barrier could closed due to pickup
					//if (wasPlacedOnSomething)
					{
						using AStarRechable = C_GraphSearchAStar<T_Graph, Heuristic_Euclid>;
						std::function<bool(AStarRechable::edge_const_t&)> traversePred = [](AStarRechable::edge_const_t& edge) { return edge->m_Enabled; };
						AStarRechable search(newState.m_Graph, newState.m_ActorPos, p->m_CurrentIndex, false, traversePred);
						if (!search.IsPathExists())
						{
							visited.pop_back();
							continue;
						}
					}

					c->Place(newState.m_Graph, *p);
					newState.m_ActorPos = p->m_CurrentIndex;

					newState.previousStateId = currId;

					auto it = std::find(visited.begin(), visited.end() - 1, newState);
					if (it != visited.end() - 1)
					{
						visited.pop_back();
					}
					else
					{
						actions[visited.size() - 1] = { currentCompanionActor, currentPlatform };
					}
				}
			}
		}

		++currId;
	}

	std::for_each(Cont(visited), [](auto& state) {state.m_Graph.DeleteAll(); });
}

//========================================================================
void C_AIPlatform::activate(bool b, T_Graph& graph)
{
	m_On = b;
	if (m_Target)
	{
		m_Target->enableWalk(b, graph);
	}
}

//========================================================================
void C_AIBarrier::enableWalk(bool b, T_Graph& graph)
{
	if (b && walkEnabledRefCount > 0)
	{
		++walkEnabledRefCount;
		return;
	}

	if (!b && walkEnabledRefCount > 1)
	{
		--walkEnabledRefCount;
		return;
	}

	if (!b && walkEnabledRefCount == 0)
	{
		check(false); //somebody enable enabled barrier
		return;
	}

	if (b) ++walkEnabledRefCount;
	if (!b) --walkEnabledRefCount;

	if (m_Neighbors.first.neighbor->GraphIndex != INVALID_INDEX &&
		m_Neighbors.second.neighbor &&
		m_Neighbors.second.neighbor->GraphIndex != INVALID_INDEX)
	{
		graph.GetEdge(m_Neighbors.first.neighbor->GraphIndex, m_Neighbors.second.neighbor->GraphIndex)->m_Enabled = b;
		graph.GetEdge(m_Neighbors.second.neighbor->GraphIndex, m_Neighbors.first.neighbor->GraphIndex)->m_Enabled = b;
	}
}
