#pragma once

template <typename graph_type, typename heuristic_type>
class C_GraphSearchAStar
{
public:
	typedef typename graph_type::edge_type edge_t;
	typedef typename graph_type::edge_const_type edge_const_t;

	typedef std::function<bool(edge_const_t &)> TraversableEdgePredicate;
	typedef std::function<void(edge_const_t &)> AddingEdgeCallback;

	static TraversableEdgePredicate		NoPredicate;
	static AddingEdgeCallback					NoCallback;

	C_GraphSearchAStar(	const graph_type & g, 
											T_GraphIndex startNode, 
											T_GraphIndex dstNode, 
											bool sliced = false, 
											TraversableEdgePredicate & func = NoPredicate, 
											AddingEdgeCallback & addingCb = NoCallback,
											std::vector<T_GraphIndex> * openNodes = 0);
		
		
		
		
	~C_GraphSearchAStar();

	T_GraphIndex		GetPath(std::vector<edge_const_t> & toFill) const; //fill path and returns id of first node in path. If there is only one NODE, no edges are added, node Index is returned
	bool						IsPathExists() const;

	void						SetCurrentAgentType(unsigned);
	
	FindPathResult	CalculateSlice(int iterations);

private:
	FindPathResult	Search(int iterations = std::numeric_limits<int>::max());

	const graph_type &						m_g;

	IndexedPriorityQLow<float> &	m_pq;

	std::vector<float>						m_GCosts;
	std::vector<float>						m_FCosts;
	std::vector<edge_const_t>			m_SearchFrontier;
	std::vector<edge_const_t>			m_SpanningPathTree; //shortest path tree

	std::vector<T_GraphIndex>			m_route;

	T_GraphIndex									m_startNode;
	T_GraphIndex									m_dstNode;
	bool													m_pathFound;
			
	TraversableEdgePredicate			m_traversableEdgePredicate;
	AddingEdgeCallback						m_addingEdgeCB;

	C_GraphSearchAStar						operator=(const C_GraphSearchAStar &);

	unsigned											m_iterations;

	std::vector<T_GraphIndex> *		m_openlist;
};

#include "GraphSearchAStar.inl"
