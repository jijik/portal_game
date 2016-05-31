#pragma once

#include "PortalGame.h"
#include "GraphUtils.h"

class AHexTileActor;

class C_GraphNode
{
public:
	C_GraphNode();
	explicit C_GraphNode(T_GraphIndex index);
	virtual ~C_GraphNode();

	T_GraphIndex	GetIndex() const;
	void			SetIndex(T_GraphIndex index);

	void			ResetIndex();

	FVector		Position;
	AHexTileActor* TileActor = nullptr;

protected:
	T_GraphIndex m_index;
};
