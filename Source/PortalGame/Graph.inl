#include <algorithm>

template <typename T> T& ensure_ref(T& t) { return t; }
template <typename T> T& ensure_ref(T* t) { return *t; }

//=============================================================================================================

template <typename NODE, typename EDGE>
C_Graph<NODE, EDGE>::C_Graph(bool digaph)
	:	m_digraph(digaph)
	,	m_nextNodeIndex(0)
{
	check(digaph && "If you van to use it as nondirected graph, you must revide some code, eg. AddEdge");
}

//=============================================================================================================

template <typename NODE, typename EDGE>
void C_Graph<NODE, EDGE>::Clear()
{
	m_nodes.clear();
	m_nodeEdges.clear();
}

//=============================================================================================================

template <typename NODE, typename EDGE>
T_GraphIndex C_Graph<NODE, EDGE>::AddNode(NODE & node)
{
	T_GraphIndex i = (unsigned)m_nodes.size();
	T_GraphIndex ret = i;

	if (i > 0 && (m_nodes.back() == nullptr || ensure_ref(m_nodes.back()).GetIndex() == INVALID_INDEX))
	{
		unsigned t = i-1;
		while (t > 0 && (m_nodes.back() == nullptr || ensure_ref(m_nodes[t - 1]).GetIndex() == INVALID_INDEX))
		{
			--t;
		}
		m_nodes[t] = node;
		ensure_ref(m_nodes[t]).SetIndex(t);
		ret = t;
	}
	else
	{
		ensure_ref(node).SetIndex(i);
		m_nodes.push_back(node);
		m_nodeEdges.resize(i+1);
	}

	return ret;
}

//=============================================================================================================

// 	template <typename NODE, typename EDGE>
// 	void C_Graph<NODE, EDGE>::Connect(T_GraphIndex from, T_GraphIndex to, float cost)
// 	{
// 		EDGE e(from, to, cost);
// 		AddEdge(e);
// 	}

//=============================================================================================================

template <typename NODE, typename EDGE>
void C_Graph<NODE, EDGE>::Disconnect(T_GraphIndex from, T_GraphIndex to)
{
	RemoveEdge(from, to);
}

//=============================================================================================================

template <typename NODE, typename EDGE>
bool C_Graph<NODE, EDGE>::IsNodePresent(T_GraphIndex index) const
{
	return (index < m_nodeEdges.size() &&
		index >= 0 &&
		m_nodes[index] &&
		ensure_ref(m_nodes[index]).GetIndex() != INVALID_INDEX);
}

//=============================================================================================================

template <typename NODE, typename EDGE>
void C_Graph<NODE, EDGE>::AddEdge(EDGE & edge)
{
	T_GraphIndex size = (T_GraphIndex)m_nodes.size();
	check(ensure_ref(edge).From() < size && ensure_ref(edge).To() < size && ensure_ref(edge).From() >= 0 && ensure_ref(edge).To() >= 0 && "Connecting nodes with wrong indexes");

	//if from and to are not nulls and not invalid indexes
	//todo rewrite
	if (m_nodes[ensure_ref(edge).From()] && 
		(ensure_ref(m_nodes[ensure_ref(edge).From()]).GetIndex() != INVALID_INDEX) 
		&&
			m_nodes[ensure_ref(edge).To()] &&
		(ensure_ref(m_nodes[ensure_ref(edge).To()]).GetIndex() != INVALID_INDEX))
	{
		if(IsUniqueEdge(edge))
			ensure_ref(m_nodeEdges[ensure_ref(edge).From()]).push_back(edge);

// 			if(!m_digraph)
// 			{
// 				EDGE newEdge(edge);
// 				newEdge.SetTo(edge.From());
// 				newEdge.SetFrom(edge.To());
// 				if(IsUniqueEdge(newEdge))
// 				{
// 					m_nodeEdges[edge.To()].push_back(newEdge);
// 				}
//			}

	}
}

//=============================================================================================================

template <typename NODE, typename EDGE>
const typename C_Graph<NODE, EDGE>::T_Edges & C_Graph<NODE, EDGE>::GetAllEdgesFrom(T_GraphIndex from) const
{
	check(IsNodePresent(from) && "Invalid node");
	return m_nodeEdges[from];
}

//=============================================================================================================

template <typename NODE, typename EDGE>
const NODE & C_Graph<NODE, EDGE>::GetNode(T_GraphIndex index) const
{
	check(IsNodePresent(index) && "Invalid node");
	return m_nodes[index];
}

//=============================================================================================================

template <typename NODE, typename EDGE>
NODE & C_Graph<NODE, EDGE>::GetNode(T_GraphIndex index)
{
	check(IsNodePresent(index) && "Invalid node");
	return m_nodes[index];
}

//=============================================================================================================

template <typename NODE, typename EDGE>
EDGE C_Graph<NODE,EDGE>::GetEdge(T_GraphIndex from, T_GraphIndex to)
{
	check(IsNodePresent(from) && "Invalid node");
	check(IsNodePresent(to) && "Invalid node");

	T_Edges e = m_nodeEdges[from];
	for (auto* edge : e)
	{
		if (edge->To() == to)
		{
			return edge;
		}
	}
	return nullptr;
}

//=============================================================================================================

template <typename NODE, typename EDGE>
void C_Graph<NODE, EDGE>::RemoveNode(T_GraphIndex index)
{
	check(IsNodePresent(index) && "Invalid node");
	ensure_ref(m_nodes[index]).SetIndex(INVALID_INDEX);
	if(!m_digraph)
	{
		RemoveNodeNonDirected(index);
	}
	else
	{
		CullInvalidEdges();
	}

	m_nodes[index] = nullptr;
}

//=============================================================================================================

template <typename NODE, typename EDGE>
void C_Graph<NODE, EDGE>::RemoveNodeAndDontCullEdges(T_GraphIndex index)
{
	check(IsNodePresent(index) && "Invalid node");
	ensure_ref(m_nodes[index]).SetIndex(INVALID_INDEX);

	m_nodes[index] = nullptr;
}

//=============================================================================================================

template <typename NODE, typename EDGE>
void C_Graph<NODE, EDGE>::RemoveNodeWithNoEdges()
{
	for (unsigned i = 0; i < m_nodes.size(); ++i)
	{
		if (m_nodeEdges[i].empty()/* && m_nodes[i] && m_nodes[i]->GetIndex() != INVALID_INDEX*/)
		{
			m_nodes[i] = nullptr;
				
			//RemoveNodeAndDontCullEdges(i);
		}
	}
}
		
//=============================================================================================================

template <typename NODE, typename EDGE>
void C_Graph<NODE, EDGE>::RemoveNodeNonDirected(T_GraphIndex index) 
{
	T_Edges & listConnected = m_nodeEdges[index];
	for(auto iter = listConnected.begin(); iter != listConnected.end(); ++iter)
	{
		EDGE & startDuplicate = ensure_ref(*iter);
		T_Edges & otherSide = m_nodeEdges[startDuplicate.To()];
		for(auto iiter = otherSide.begin(); iiter != otherSide.end(); ++iiter)
		{
			if ((ensure_ref(*iiter).To()) == index)
			{
				iiter = m_nodeEdges[startDuplicate.To()].erase(iiter);
				break;
			}
		}
	}

	m_nodeEdges[index].clear();
}

//=============================================================================================================

template <typename NODE, typename EDGE>
void C_Graph<NODE, EDGE>::CullInvalidEdges()
{
	for(auto currEdgeList = m_nodeEdges.begin(); currEdgeList != m_nodeEdges.end(); ++currEdgeList)
	{
		T_Edges & edges = ensure_ref(*currEdgeList);
		edges.erase(std::remove_if(edges.begin(), edges.end(), [&](EDGE & e)
		{
			//EDGE & e = ensure_ref(ee);
			return (m_nodes[ensure_ref(e).From()] == nullptr ||
							ensure_ref(m_nodes[ensure_ref(e).From()]).GetIndex() == INVALID_INDEX ||
							m_nodes[ensure_ref(e).To()] == nullptr ||
							ensure_ref(m_nodes[ensure_ref(e).To()]).GetIndex() == INVALID_INDEX);
		}), edges.end());
	}
}

//=============================================================================================================

template <typename NODE, typename EDGE>
void C_Graph<NODE, EDGE>::RemoveEdge(T_GraphIndex from, T_GraphIndex to)
{
	auto & list = m_nodeEdges[from];
	for(auto iter = list.cbegin(); iter != list.cend(); ++iter)
	{
		if (ensure_ref((*iter)).To() == to)
		{
			list.erase(iter);
			return;
		}
	}
}

//=============================================================================================================

template <typename NODE, typename EDGE>
inline unsigned C_Graph<NODE, EDGE>::GetNodeCountSlots() const
{
	return (unsigned)m_nodes.size();
}

//=============================================================================================================

template <typename NODE, typename EDGE>
inline unsigned C_Graph<NODE, EDGE>::GetNodeCountReal() const
{
	check(false && "this is expensive, if you want to use this, have counter and keep its state when removing/adding");
	// TODO: Unable to use static_assert because of Clang
	//static_assert(false, "this is expensive, if you want to use this, have counter and keep its state when removing/adding");
	return count_if(m_nodes, [](NODE & n) { n && n->GetIndex() != INVALID_INDEX; });
}

//=============================================================================================================

//is directed graph (A->B != B->A)
template <typename NODE, typename EDGE>
bool C_Graph<NODE, EDGE>::IsDigraph() const
{
	return m_digraph;
}

//=============================================================================================================

template <typename NODE, typename EDGE>
bool C_Graph<NODE, EDGE>::IsUniqueEdge(EDGE & edge) const
{
	auto & list = m_nodeEdges[ensure_ref(edge).From()];
	for(auto iter = list.cbegin(); iter != list.cend(); ++iter)
	{
		if (ensure_ref(*iter).To() == ensure_ref(edge).To())
			return false;
	}
	return true;
}

//=============================================================================================================

template <typename NODE, typename EDGE>
float C_Graph <NODE, EDGE>::DistEq(T_GraphIndex from, T_GraphIndex to)
{
	return (ensure_ref(GetNode(from)).position - ensure_ref(GetNode(to)).position).GetLength();
}

//=============================================================================================================
template <typename NODE, typename EDGE>
void C_Graph<NODE, EDGE>::CloneTo(C_Graph<NODE, EDGE>& to)
{
	to.m_nextNodeIndex = m_nextNodeIndex;
	to.m_digraph = m_digraph;

	to.m_nodes.clear();
	for (unsigned i = 0; i < m_nodes.size(); ++i)
	{
		auto* newNode = new std::remove_pointer<NODE>::type;
		*newNode = *m_nodes[i];
		to.m_nodes.push_back(newNode);
	}

	for (unsigned i = 0; i < m_nodeEdges.size(); ++i)
	{
		to.m_nodeEdges[i].clear();
		for (unsigned j = 0; j < m_nodeEdges[i].size(); ++j)
		{
			auto* edge = new std::remove_pointer<EDGE>::type;
			*edge = *m_nodeEdges[i][j];
			to.m_nodeEdges[i].push_back(edge);
		}
	}
}

//=============================================================================================================
template <typename NODE, typename EDGE>
void C_Graph<NODE, EDGE>::DeleteAll()
{
	for (unsigned i = 0; i < m_nodes.size(); ++i)
	{
		delete m_nodes[i];
	}
	for (unsigned i = 0; i < m_nodeEdges.size(); ++i)
	{
		for (unsigned j = 0; j < m_nodeEdges[i].size(); ++j)
		{
			delete m_nodeEdges[i][j];
		}
	}
}

//=============================================================================================================



//////////////////////////////////////////////////////////////////////////
//Inner classes (iterators)
//
// Using:
//
//     graph_type::NodeIterator n(g);
//     node_type * i = n.begin();
//     while(!n.end())
//     {
//         i->something();
//         i = n.next();
//     }
//////////////////////////////////////////////////////////////////////////
	
template <typename NODE, typename EDGE>
class C_Graph<NODE, EDGE>::NodeIterator
{
public:
	NodeIterator(C_Graph<NODE, EDGE> & graph)
		:	m_graph(graph)
		,	iter(m_graph.m_nodes.begin())
	{

	}

	//=============================================================================================================

	NODE begin()
	{
		iter = m_graph.m_nodes.begin();

		if (end())
		{
			return 0;
		}

		getNextValidIndex();

		return end() ? 0 : (*iter);
	}

	//=============================================================================================================

	NODE next()
	{
		++iter;
		if (end())
		{
			return 0;
		}

		getNextValidIndex();

		return end() ? 0 : (*iter);
	}

	//=============================================================================================================

	bool end()
	{
		return iter == m_graph.m_nodes.end();
	}

	//=============================================================================================================

private:

	void getNextValidIndex()
	{
		while (*iter == nullptr || (*iter)->GetIndex() == INVALID_INDEX)
		{
			++iter;
			if (end())
			{
				break;
			}
		}
	}

	C_Graph<NODE, EDGE> & m_graph;

	NodeIterator & operator=(const NodeIterator &);

	typename std::vector<NODE>::iterator iter;
};

//=============================================================================================================
//=============================================================================================================
//=============================================================================================================

template <typename NODE, typename EDGE>
class C_Graph<NODE, EDGE>::ConstNodeIterator
{
public:
	ConstNodeIterator(const C_Graph<NODE, EDGE> & graph)
		:	m_graph(graph)
		,	iter(m_graph.m_nodes.begin())
	{

	}

	//=============================================================================================================

	const NODE begin()
	{
		iter = m_graph.m_nodes.begin();

		if (end())
		{
			return 0;
		}

		getNextValidIndex();
			
		return end() ? 0 : (*iter);
	}

	//=============================================================================================================

	const NODE next()
	{
		++iter;

		if (end())
		{
			return 0;
		}

		getNextValidIndex();

		return end() ? 0 : (*iter);
	}

	//=============================================================================================================

		bool end()
		{
			return iter == m_graph.m_nodes.end();
		}

		//=============================================================================================================

private:
	void getNextValidIndex()
	{
		while(*iter == nullptr || (*iter)->GetIndex() == INVALID_INDEX)
		{
			++iter;

			if (end())
			{
				break;
			}
		}
	}

	const C_Graph<NODE, EDGE> & m_graph;

	ConstNodeIterator & operator=(const ConstNodeIterator &);

	typename std::vector<NODE>::const_iterator iter;
};

//=============================================================================================================
//=============================================================================================================
//=============================================================================================================

template <typename NODE, typename EDGE>
class C_Graph<NODE, EDGE>::EdgeIterator
{
public:
	EdgeIterator(C_Graph<NODE, EDGE> & graph, T_GraphIndex fromNode)
		:	m_graph(graph)
		,	m_node(fromNode)
		,	iter(m_graph.m_nodeEdges[fromNode].begin())
	{

	}

	//=============================================================================================================

	EDGE begin()
	{
		iter = m_graph.m_nodeEdges[m_node].begin();

		if (end())
		{
			return 0;
		}

		return end() ? 0 : (*iter);
	}

	//=============================================================================================================

	EDGE next()
	{
		++iter;

		return end() ? 0 : (*iter);
	}

	//=============================================================================================================

	bool end()
	{
		return iter == m_graph.m_nodeEdges[m_node].end();
	}

	//=============================================================================================================

private:
	C_Graph<NODE, EDGE> & m_graph;

	typename std::vector<EDGE>::iterator iter;

	EdgeIterator & operator=(const EdgeIterator &);

	T_GraphIndex m_node;
};

//=============================================================================================================
//=============================================================================================================
//=============================================================================================================

template <typename NODE, typename EDGE>
class C_Graph<NODE, EDGE>::ConstEdgeIterator
{
public:
	ConstEdgeIterator(const C_Graph<NODE, EDGE> & graph, T_GraphIndex fromNode)
		:	m_graph(graph)
		,	m_node(fromNode)
		,	iter(m_graph.m_nodeEdges[fromNode].begin())
	{

	}

	//=============================================================================================================

	const EDGE begin()
	{
		iter = m_graph.m_nodeEdges[m_node].begin();
		return end() ? 0 : (*iter);
	}

	//=============================================================================================================

	const EDGE next()
	{
		++iter;
			
		return end() ? 0 : (*iter);
	}

	//=============================================================================================================

	bool end()
	{
		return iter == m_graph.m_nodeEdges[m_node].end();
	}

	//=============================================================================================================

private:
	const C_Graph<NODE, EDGE> & m_graph;
		
	typename std::vector<EDGE>::const_iterator iter;

	ConstEdgeIterator & operator=(const ConstEdgeIterator &);

	T_GraphIndex m_node;
};

//=============================================================================================================
//=============================================================================================================
//=============================================================================================================



