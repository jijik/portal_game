
#include "PortalGame.h"
#include "GraphEdge.h"

//=============================================================================================================

C_GraphEdge::C_GraphEdge():
	m_fromIndex(INVALID_INDEX),
	m_toIndex(INVALID_INDEX),
	m_cost(COST_DEFAULT)
{

}

//=============================================================================================================

C_GraphEdge::C_GraphEdge( T_GraphIndex from, T_GraphIndex to ):
	m_fromIndex(from),
	m_toIndex(to),
	m_cost(COST_DEFAULT)
{

}

//=============================================================================================================

C_GraphEdge::C_GraphEdge( T_GraphIndex from, T_GraphIndex to, float cost ):
	m_fromIndex(from),
	m_toIndex(to),
	m_cost(cost)
{

}

//=============================================================================================================

C_GraphEdge::~C_GraphEdge()
{

}

//=============================================================================================================

bool C_GraphEdge::operator==( const C_GraphEdge & rhs )
{
	return m_toIndex == rhs.m_toIndex && m_fromIndex == rhs.m_fromIndex && m_cost == rhs.m_cost;
}

//=============================================================================================================

bool C_GraphEdge::operator!=( const C_GraphEdge & rhs )
{
	return !(*this).operator==(rhs);
}

//=============================================================================================================
