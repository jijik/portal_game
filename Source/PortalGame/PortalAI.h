#pragma once

#include "Graph.h"
#include "GraphNode.h"
#include "GraphEdge.h"
#include "PortalAI.h"
#include "HexCoordinates.h"
#include "HexEditorActor.h"
#include "HexTileActor.h"

//========================================================================
using T_Graph = C_Graph<C_GraphNode*, C_GraphEdge*>;

//========================================================================
template <typename T>
bool elementEquals(T* t1, T* t2)
{
	if (t1 == nullptr && t2 == nullptr) return true;
	if (t1 == nullptr || t2 == nullptr) return false;
	return *t1 == *t2;
}

struct C_AIElement 
{
	virtual ~C_AIElement() = default;
	enum type { barrier, finish, platform, cube };
	const char* typeStr(type t) { const char* arr[] = { "bar", "fin", "pf", "cb" }; return arr[(unsigned)t]; }
	const char* typeStr() { return typeStr(GetType()); }
	virtual type GetType() = 0;
	T_GraphIndex m_CurrentIndex = INVALID_INDEX;

	void Place(T_Graph& graph, T_GraphIndex where)
	{
		check(m_CurrentIndex != INVALID_INDEX);
		graph.GetNode(where)->AIElements.push_back(this);
		m_CurrentIndex = where;
	}

	bool operator==(const C_AIElement& rhs)
	{
		return m_CurrentIndex == rhs.m_CurrentIndex;
	}
	bool operator!=(const C_AIElement& rhs) { return !operator==(rhs); }
};

//========================================================================
struct C_AIFinish : public C_AIElement
{
	virtual type GetType() { return finish; }
};

//========================================================================
struct C_AIBarrier : public C_AIElement
{
	struct S_NeighborInfo 
	{
		AHexTileActor* neighbor; HexDir slotAtNeighbor; 
		bool operator==(const S_NeighborInfo& rhs) const { return neighbor == rhs.neighbor && slotAtNeighbor == rhs.slotAtNeighbor; }
	};
	std::pair<S_NeighborInfo, S_NeighborInfo> m_Neighbors; //first is always valid
	int walkEnabledRefCount = 0;
	virtual type GetType() { return barrier; }
	unsigned m_Id;
	void enableWalk(bool b, T_Graph& graph);
	bool operator==(const C_AIBarrier& rhs)
	{
		return C_AIElement::operator==(rhs) && m_Neighbors == rhs.m_Neighbors && walkEnabledRefCount == rhs.walkEnabledRefCount && m_Id == rhs.m_Id;
	}
	bool operator!=(const C_AIBarrier& rhs) { return !operator==(rhs); }
};

//========================================================================
struct C_AIPlatform : public C_AIElement
{
	virtual type GetType() { return platform; }
	C_AIBarrier*	m_Target = nullptr;
	bool m_On = false;
	void activate(bool b, T_Graph& graph);
	bool operator==(const C_AIPlatform& rhs)
	{
		return C_AIElement::operator==(rhs) && elementEquals(m_Target, rhs.m_Target) && m_On == rhs.m_On;
	}
	bool operator!=(const C_AIPlatform& rhs) { return !operator==(rhs); }
};

//========================================================================
struct C_AICube : public C_AIElement
{
	virtual type GetType() { return cube; }
	
	void PickUp(T_Graph& graph, bool& wasPlacedOnSomething)
	{
		check(m_CurrentIndex != INVALID_INDEX);
		erase(graph.GetNode(m_CurrentIndex)->AIElements, this);
		wasPlacedOnSomething = m_PlacedOn != nullptr;
		if (wasPlacedOnSomething)
		{
			m_PlacedOn->activate(false, graph);
		}
		m_PlacedOn = nullptr;
	}

	void Place(T_Graph& graph, C_AIPlatform& p)
	{
		m_PlacedOn = &p;
		C_AIElement::Place(graph, p.m_CurrentIndex);
		p.activate(true, graph);
	}

	bool operator==(const C_AICube& rhs)
	{
		return C_AIElement::operator==(rhs) && elementEquals(m_PlacedOn, rhs.m_PlacedOn);
	}
	bool operator!=(const C_AICube& rhs) { return !operator==(rhs); }

	C_AIPlatform* m_PlacedOn = nullptr;
};

//========================================================================
class C_PortalAI
{
public:
	C_PortalAI() {}
	~C_PortalAI() { m_InitialState.m_Graph.DeleteAll(); }

	void	Update(float dt);
	void	DebugDrawGraph(float dt);

	void	Generate();

	void	Solve();

	struct S_State
	{
		C_AIFinish* m_Finish;
		T_Graph m_Graph;
		T_GraphIndex							m_ActorPos;
		std::vector<C_AIBarrier>	m_Barriers;
		std::vector<C_AICube>			m_Cubes;
		std::vector<C_AIPlatform>	m_Platforms;

		std::unordered_map<void*, void*> mapper;
		void CloneTo(S_State& toFill)
		{
			mapper.clear();

			mapper[nullptr] = nullptr;
			mapper[m_Finish] = m_Finish;
			for (unsigned i = 0; i < m_Barriers.size(); ++i) { mapper[&m_Barriers[i]] = &toFill.m_Barriers[i]; }
			for (unsigned i = 0; i < m_Cubes.size(); ++i) { mapper[&m_Cubes[i]] = &toFill.m_Cubes[i]; }
			for (unsigned i = 0; i < m_Platforms.size(); ++i) { mapper[&m_Platforms[i]] = &toFill.m_Platforms[i]; }

			m_Graph.CloneTo(toFill.m_Graph);

			for (unsigned i = 0; i < m_Cubes.size(); ++i)
			{
				auto* toMap = m_Cubes[i].m_PlacedOn;
				toFill.m_Cubes[i].m_PlacedOn = static_cast<C_AIPlatform*>(mapper[toMap]);
			}

			for (unsigned i = 0; i < m_Platforms.size(); ++i)
			{
				auto* toMap = m_Platforms[i].m_Target;
				toFill.m_Platforms[i].m_Target = static_cast<C_AIBarrier*>(mapper[toMap]);
			}

			T_Graph::NodeIterator nodeIter(m_Graph);
			T_Graph::NodeIterator toFillNodeIter(toFill.m_Graph);

			auto* node = nodeIter.begin();
			auto* toFillNode = toFillNodeIter.begin();

			while (!nodeIter.end())
			{
				for (unsigned i = 0; i < node->AIElements.size(); ++i)
				{
					toFillNode->AIElements[i] = static_cast<C_AIElement*>(mapper[node->AIElements[i]]);
				}
				node = nodeIter.next();
				toFillNode = toFillNodeIter.next();
			}
		}

		template <typename T>
		T Get(void* ptr)
		{
			return static_cast<T>(mapper[ptr]);
		}

		int	previousStateId = -1;
		
		bool operator!=(const S_State& rhs)
		{
			return !operator==(rhs);
		}

		bool operator==(const S_State& rhs)
		{
			if (m_ActorPos != rhs.m_ActorPos) return false;
			for (unsigned i = 0; i < m_Barriers.size(); ++i)
			{
				if (m_Barriers[i] != rhs.m_Barriers[i]) return false;
			}
			for (unsigned i = 0; i < m_Cubes.size(); ++i)
			{
				if (m_Cubes[i] != rhs.m_Cubes[i]) return false;
			}
			for (unsigned i = 0; i < m_Platforms.size(); ++i)
			{
				if (m_Platforms[i] != rhs.m_Platforms[i]) return false;
			}
			return true;
		}
	};

	S_State m_InitialState;
};
