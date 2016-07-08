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
struct C_AIElement 
{
	virtual ~C_AIElement() = default;
	enum type { barrier, finish, platform, cube };
	const char* typeStr(type t) { const char* arr[] = { "bar", "fin", "pf", "cb" }; return arr[(unsigned)t]; }
	const char* typeStr() { return typeStr(GetType()); }
	virtual type GetType() = 0;
	T_GraphIndex m_CurrentIndex = INVALID_INDEX;

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
	bool on = true;
	virtual type GetType() { return barrier; }
	unsigned m_Id;
	void enable(bool b, T_Graph& graph);
	bool operator==(const C_AIBarrier& rhs)
	{
		return C_AIElement::operator==(rhs) && m_Neighbors == rhs.m_Neighbors && on == rhs.on && m_Id == rhs.m_Id;
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
		return C_AIElement::operator==(rhs) && m_Target == rhs.m_Target && m_On == rhs.m_On;
	}
	bool operator!=(const C_AIPlatform& rhs) { return !operator==(rhs); }
};

//========================================================================
struct C_AICube : public C_AIElement
{
	virtual type GetType() { return cube; }
	C_AIPlatform* m_PlacedOn = nullptr;
	
	bool operator==(const C_AICube& rhs)
	{
		return C_AIElement::operator==(rhs) && m_PlacedOn == rhs.m_PlacedOn;
	}
	bool operator!=(const C_AICube& rhs) { return !operator==(rhs); }
};

//========================================================================
class C_PortalAI
{
public:
	C_PortalAI() {}

	void	Update(float dt);
	void	DebugDrawGraph(float dt);

	void	Generate();

	void	Solve();

	struct S_State
	{
		T_Graph m_Graph;
		T_GraphIndex								m_ActorPos;
		std::vector<C_AIBarrier>		m_Barriers;
		std::vector<C_AICube*>			m_Cubes;
		std::vector<C_AIPlatform*>	m_Platforms;

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
				if (*m_Cubes[i] != *rhs.m_Cubes[i]) return false;
			}
			for (unsigned i = 0; i < m_Platforms.size(); ++i)
			{
				if (*m_Platforms[i] != *rhs.m_Platforms[i]) return false;
			}
			return true;
		}
	};

	S_State m_InitialState;
};
