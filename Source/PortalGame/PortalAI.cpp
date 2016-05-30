#include "PortalGame.h"
#include "PortalAI.h"
#include "PortalUtils.h"
#include "HexEditorActor.h"
#include "HexGame.h"

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

		fromPos.Z += 30.f;

		T_Graph::EdgeIterator edgeIterator(m_Graph, fromIndex);
		auto* edge = edgeIterator.begin();
		while (!edgeIterator.end())
		{
			auto toIndex = edge->To();
			auto toPos = m_Graph.GetNode(toIndex)->Position;

			toPos.Z += 30.f;

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

	for (auto& pair : storage)
	{
		auto hexCoords = pair.first;
		auto* hexTileActor = pair.second;

		if (hexTileActor->GraphIndex == INVALID_INDEX)
		{
			auto* newNode = new C_GraphNode;
			auto newIndex = m_Graph.AddNode(newNode);
			newNode->SetIndex(newIndex);
			hexTileActor->GraphIndex = newIndex;
			newNode->Position = hexGrid.GetPosition(hexCoords);
		}
		else
		{

		}

		for (auto n : hexGrid.HorizontalNeighborIndexes)
		{
			auto neighborCoords = hexCoords + n;
			auto* neighbor = hexGrid.GetElement(neighborCoords);
			if (neighbor && neighbor->GraphIndex != INVALID_INDEX)
			{
				auto* edge1 = new C_GraphEdge(hexTileActor->GraphIndex, neighbor->GraphIndex);
				auto* edge2 = new C_GraphEdge(neighbor->GraphIndex, hexTileActor->GraphIndex);

				m_Graph.AddEdge(edge1);
				m_Graph.AddEdge(edge2);
			}
		}

	}
}