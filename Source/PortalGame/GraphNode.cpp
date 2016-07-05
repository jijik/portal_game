
#include "PortalGame.h"
#include "PortalUtils.h"
#include "PortalAI.h"
#include "GraphNode.h"

//=============================================================================================================

C_GraphNode::C_GraphNode()
	:m_index(INVALID_INDEX)
{

}

//=============================================================================================================

C_GraphNode::C_GraphNode(T_GraphIndex index)
	:m_index(index)
{

}

//=============================================================================================================

C_GraphNode::~C_GraphNode()
{

}

//=============================================================================================================

T_GraphIndex C_GraphNode::GetIndex() const
{
	return m_index;
}

//=============================================================================================================

void C_GraphNode::SetIndex(T_GraphIndex index)
{
	m_index = index;
}

//=============================================================================================================

void C_GraphNode::ResetIndex()
{
	m_index = INVALID_INDEX;
}

//=============================================================================================================

