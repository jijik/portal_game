#pragma once

#include "Graph.h"
#include "GraphNode.h"
#include "GraphEdge.h"
#include "HexCoordinates.h"

//========================================================================
struct C_AIElement 
{
	virtual ~C_AIElement() = default;
	enum type { barrier, finish, platform, cube };
	virtual type GetType() = 0;
};

//========================================================================
struct C_AIFinish : public C_AIElement
{
	virtual type GetType() { return finish; }
};

//========================================================================
struct C_AIBarrier : public C_AIElement
{
	struct S_NeighborInfo { AHexTileActor* neighbor; HexDir slotAtNeighbor; };
	std::pair<S_NeighborInfo, S_NeighborInfo> m_Neighbors; //first is always valid
	bool on = true;
	virtual type GetType() { return barrier; }
};

//========================================================================
struct C_AIPlatform : public C_AIElement
{
	virtual type GetType() { return platform; }
	C_AIBarrier*	m_Target = nullptr;
	bool m_On = false;
};

//========================================================================
struct C_AICube : public C_AIElement
{
	virtual type GetType() { return cube; }
};

//========================================================================
class C_PortalAI
{
public:
	C_PortalAI() {}

	using T_Graph = C_Graph<C_GraphNode*, C_GraphEdge*>;

	void	Update(float dt);
	void	DebugDrawGraph(float dt);

	void	Generate();

private:
	T_Graph m_Graph;

	C_AIFinish	m_Finish;
	std::vector<C_AIBarrier>	m_Barriers;
	std::vector<C_AIPlatform> m_Platforms;
	std::vector<C_AICube> m_Cubes;
};
