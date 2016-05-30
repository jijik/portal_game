#pragma once

#include "Graph.h"
#include "GraphNode.h"
#include "GraphEdge.h"

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
};