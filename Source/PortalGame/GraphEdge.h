#pragma once

#include "GraphUtils.h"

class C_GraphEdge
{
public:
	C_GraphEdge();
	C_GraphEdge(T_GraphIndex from, T_GraphIndex to);
	C_GraphEdge(T_GraphIndex from, T_GraphIndex to, float cost);
	virtual ~C_GraphEdge();

	virtual bool	operator==(const C_GraphEdge & rhs);
	virtual bool	operator!=(const C_GraphEdge & rhs);

	T_GraphIndex	From() const;
	void					SetFrom(T_GraphIndex from);
	T_GraphIndex	To() const;
	void					SetTo(T_GraphIndex to);
	float					GetCost() const;
	void					SetCost(float cost);

protected:
	T_GraphIndex	m_fromIndex;
	T_GraphIndex	m_toIndex;

	float	m_cost;

};


#include "GraphEdge.inl"
