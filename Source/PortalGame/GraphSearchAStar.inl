#include <queue>
#include "GraphUtils.h"

//=============================================================================================================
template <typename graph_type, typename heuristic_type>
typename C_GraphSearchAStar<graph_type, heuristic_type>::AddingEdgeCallback
	C_GraphSearchAStar<graph_type, heuristic_type>::NoCallback = [](const edge_t &){};

//=============================================================================================================
template <typename graph_type, typename heuristic_type>
typename C_GraphSearchAStar<graph_type, heuristic_type>::TraversableEdgePredicate
	C_GraphSearchAStar<graph_type, heuristic_type>::NoPredicate = [](const edge_t &){ return true; };

//=============================================================================================================
template <typename graph_type, typename heuristic_type>
C_GraphSearchAStar<graph_type, heuristic_type>::C_GraphSearchAStar(	const graph_type & g,
																																		T_GraphIndex startNode,
																																		T_GraphIndex dstNode, 
																																		bool sliced, 
																																		TraversableEdgePredicate & pred, 
																																		AddingEdgeCallback & addingCb,
																																		std::vector<T_GraphIndex> * openNodes)
	:	m_g(g)
	,	m_startNode(startNode)
	,	m_dstNode(dstNode)
	,	m_pathFound(false)
	,	m_FCosts(g.GetNodeCountSlots(), 0.0f)
	,	m_GCosts(g.GetNodeCountSlots(), 0.0f)
	,	m_SearchFrontier(g.GetNodeCountSlots(), 0)
	,	m_SpanningPathTree(g.GetNodeCountSlots(), 0)
	,	m_route(g.GetNodeCountSlots(), NO_PARRENT)
	,	m_traversableEdgePredicate(pred)
	, m_addingEdgeCB(addingCb)
	, m_openlist(openNodes)
	, m_pq(*new IndexedPriorityQLow<float>(m_FCosts, m_g.GetNodeCountSlots()))
{
	//put the source node on the queue
	m_pq.insert(m_startNode);

	if (! sliced)
	{
		m_pathFound = Search() == FindPathResult::Success;
	}
}

//=============================================================================================================
template <typename graph_type, typename heuristic_type>
C_GraphSearchAStar<graph_type, heuristic_type>::~C_GraphSearchAStar()
{
	delete &m_pq;
}

//=============================================================================================================
template <typename graph_type, typename heuristic_type>
FindPathResult C_GraphSearchAStar<graph_type, heuristic_type>::CalculateSlice(int iterations)
{
	return Search(iterations);
}

//=============================================================================================================
template <typename graph_type, typename heuristic_type>
FindPathResult C_GraphSearchAStar<graph_type, heuristic_type>::Search(int iterations)
{
	//while the queue is not empty
	while (!m_pq.empty() && iterations--> 0)
	{
		//get cheapest node from the queue
		T_GraphIndex ncn = (T_GraphIndex)m_pq.Pop();

		if (m_openlist)
		{ 
			m_openlist->push_back(ncn);
		}

		//move this node from the frontier to the spanning tree
		m_SpanningPathTree[ncn] = m_SearchFrontier[ncn];

		//if the target has been found exit
		if (ncn == m_dstNode)
		{
			m_pathFound = true;
			return FindPathResult::Success;
		}

		//test all neighb.
		typename graph_type::ConstEdgeIterator ConstEdgeItr(m_g, ncn);

		for (	edge_t pE=ConstEdgeItr.begin();
					!ConstEdgeItr.end(); 
					pE=ConstEdgeItr.next())
		{
			//check if edge can be traversed
			bool traversable = m_traversableEdgePredicate(pE);
			if (! traversable)
 			{
 				continue;
 			}

			//heuristic from this to the target
			float HCost = heuristic_type::Calculate(m_g, m_dstNode, pE->To()); 

			float GCost = m_GCosts[ncn] + pE->GetCost();

			//if not in sf, add it
			if (m_SearchFrontier[pE->To()] == 0 && pE->To() != m_startNode)
			{
				m_FCosts[pE->To()] = GCost + HCost;
				m_GCosts[pE->To()] = GCost;

				m_pq.insert(pE->To());

				m_SearchFrontier[pE->To()] = pE;

				m_addingEdgeCB(pE);
			}
			else if ((GCost < m_GCosts[pE->To()]) && (m_SpanningPathTree[pE->To()]==0))
			{
				//relax
				m_FCosts[pE->To()] = GCost + HCost;
				m_GCosts[pE->To()] = GCost;

				m_pq.ChangePriority(pE->To());

				m_SearchFrontier[pE->To()] = pE;

				m_addingEdgeCB(pE);
			}
		}
	}

	if (m_pq.empty())
	{
		return FindPathResult::Failed;	//queue empty
	}
	else
	{
		return FindPathResult::NotDone;	//queue not empty, no more iterations
	}
}

//=============================================================================================================
template <typename graph_type, typename heuristic_type>
bool C_GraphSearchAStar<graph_type, heuristic_type>::IsPathExists() const
{
	return m_pathFound;
}

//=============================================================================================================
template <typename graph_type, typename heuristic_type>
T_GraphIndex C_GraphSearchAStar<graph_type, heuristic_type>::GetPath(std::vector<edge_const_t> & list) const
{
	list.clear();

	if(!m_pathFound || m_dstNode == INVALID_INDEX)
		return INVALID_INDEX;

	if(m_startNode == m_dstNode)
		return m_startNode;

	T_GraphIndex back = m_dstNode;

	edge_t edge = m_SpanningPathTree[back];
	T_GraphIndex from = ensure_ref(edge).From();

	while(from != m_startNode)
	{
		list.push_back(edge);
		back = from;
		edge = m_SpanningPathTree[back];
		from = ensure_ref(edge).From();
	}
	list.push_back(edge);
		
	std::reverse(list.begin(), list.end());

	return m_startNode;
}
