// Copyright (c) 2014 WarhorseStudios
// author: Martin Stys

#pragma once

#ifndef _WH_AI_XGEN_GRAPH_h__
#define _WH_AI_XGEN_GRAPH_h__

#include <XGenAIModule/Navigation/Paths/GraphUtils.h>

#include <vector>

namespace wh {
namespace xgenaimodule {
namespace navigation {

	template<class t> struct constify { typedef t type; };
	template<class t> struct constify<t&> { typedef const t& type; };

	//designed for sparse graphs
	template <typename NODE, typename EDGE>
	class C_Graph
	{
	public:	
		typedef std::vector<NODE>			T_Nodes;
		typedef std::vector<EDGE>			T_Edges;
		typedef std::vector<T_Edges>	T_EdgesVector;

		typedef NODE node_type;
		typedef EDGE edge_type;
		typedef typename constify<EDGE>::type edge_const_type;

	public:
		C_Graph(bool digraph);

		T_GraphIndex			AddNode(NODE & node);
		void							AddEdge(EDGE & edge);
		NODE &						GetNode(T_GraphIndex index);
		EDGE *						GetEdge(T_GraphIndex from, T_GraphIndex to);
		const NODE &			GetNode(T_GraphIndex index) const;
		const EDGE &			GetEdge(T_GraphIndex from, T_GraphIndex to) const;
		void							RemoveNode(T_GraphIndex index);
		void							RemoveNodeAndDontCullEdges(T_GraphIndex index);
		void							RemoveEdge(T_GraphIndex from, T_GraphIndex to);
		unsigned					GetNodeCountReal() const;
		unsigned					GetNodeCountSlots() const;
		//void							Connect(T_GraphIndex from, T_GraphIndex to, float const = COST_DEFAULT);
		void							Disconnect(T_GraphIndex from, T_GraphIndex to);

		const T_Edges &		GetAllEdgesFrom(T_GraphIndex from) const;

		bool							IsDigraph() const;
		bool							IsNodePresent(T_GraphIndex index) const;
		bool							IsEdgePresent(T_GraphIndex from, T_GraphIndex to) const;

		void							Clear();
		float							DistEq(T_GraphIndex from, T_GraphIndex to);

		void							CullInvalidEdges();
		void							RemoveNodeWithNoEdges();

		static_assert(std::is_pointer<NODE>::value, "You are using non pointer type!");
		static_assert(std::is_pointer<EDGE>::value, "You are using non pointer type!");
		//if you getting this error, and you want to use graph with non-pointer type, you must revise
		//iterators, search and other ...

		class NodeIterator;
		friend class NodeIterator;

		class ConstNodeIterator;
		friend class ConstNodeIterator;

		class EdgeIterator;
		friend class EdgeIterator;

		class ConstEdgeIterator;
		friend class ConstEdgeIterator;

	protected:
		T_Nodes				m_nodes;
		T_EdgesVector	m_nodeEdges;

		T_GraphIndex	m_nextNodeIndex;

		bool					m_digraph;

	private:
		bool			IsUniqueEdge(EDGE & edge) const;
		void			RemoveNodeNonDirected(T_GraphIndex index);
	};

}}} //namespaces 

#include <XGenAIModule/Navigation/Paths/Graph.inl>

#endif // Graph_h__