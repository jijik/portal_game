// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HexCoordinates.h"
#include "UnrealMathUtility.h"
#include <unordered_map>

//  AXIAL COORDINATES [x,y]
//  
//          +0,-2  +1,-2  +2,-2
//      -1,-1  +0,-1  +1,-1  +2,-1
//  -2,+0  -1,+0  +0,+0  +1,+0  +2,+0 
//      -2,+1  -1,+1  +0,+1  +1,+1
//          -2,+2  -1,+2  +0,+2
//  
//  

template <typename T>
class PORTALGAME_API C_HexGrid
{
public:
	static const Array6<S_HexCoordinates> HorizontalNeighborIndexes;

	void		Init();
	void		InsertElement(const S_HexCoordinates& coordinates, const T& element);
	T				GetElement(const S_HexCoordinates& coordinates);
	void		RemoveElement(const S_HexCoordinates& coordinates);
	void		FillHorizontalNeighbors(const S_HexCoordinates& origin, Array6<S_HexCoordinates>& toFill);
	void		SetTileRadius(float tileRadius);
	FVector	GetPosition(const S_HexCoordinates& coordinates);
	FVector	GetPositionBetweenTiles(const S_HexCoordinates& c1, const S_HexCoordinates& c2);

private:
	std::unordered_map<S_HexCoordinates, T>		m_GridMap;
	float							m_TileRadius;
	float							m_BoundaryDistance;
	S_HexCoordinates	m_SelectedTile;
};

//========================================================================
//========================================================================
template <typename T>
const Array6<S_HexCoordinates> C_HexGrid<T>::HorizontalNeighborIndexes = {	S_HexCoordinates(1, 0),
																																						S_HexCoordinates(1,-1),
																																						S_HexCoordinates(0,-1),
																																						S_HexCoordinates(-1,0),
																																						S_HexCoordinates(-1,1),
																																						S_HexCoordinates(0, 1) };

//========================================================================
template <typename T>
void C_HexGrid<T>::Init()
{
	
}

//========================================================================
template <typename T>
T C_HexGrid<T>::GetElement(const S_HexCoordinates& coordinates)
{
	auto it = m_GridMap.find(coordinates);
	if (it == m_GridMap.end())
	{
		return nullptr;
	}
	return it->second;
}

//========================================================================
template <typename T>
void C_HexGrid<T>::RemoveElement(const S_HexCoordinates& coordinates)
{
	auto it = m_GridMap.find(coordinates);
	check(it != m_GridMap.end());
	m_GridMap.erase(it);
}

//========================================================================
template <typename T>
void C_HexGrid<T>::InsertElement(const S_HexCoordinates& coordinates, const T& element)
{
	m_GridMap[coordinates] = element;
}

//========================================================================
template <typename T>
void C_HexGrid<T>::FillHorizontalNeighbors(const S_HexCoordinates& origin, Array6<S_HexCoordinates>& toFill)
{
	for (unsigned i = 0; i < 6; ++i)
	{
		toFill[i] = origin + HorizontalNeighborIndexes[i];
	}
}

//========================================================================
template <typename T>
FVector C_HexGrid<T>::GetPosition(const S_HexCoordinates& coordinates)
{
	auto x = coordinates.t * 1.5 * -m_TileRadius;
	auto y = coordinates.s * 2.0 * m_BoundaryDistance + coordinates.t * m_BoundaryDistance;
	auto z = coordinates.z * m_TileRadius;
	return FVector(x, y, z);
}

//========================================================================
template <typename T>
FVector C_HexGrid<T>::GetPositionBetweenTiles(const S_HexCoordinates& c1, const S_HexCoordinates& c2)
{
	auto p1 = GetPosition(c1);
	auto p2 = GetPosition(c2);
	return p1 + 0.5 * (p2 - p1);
}

//========================================================================
template <typename T>
void C_HexGrid<T>::SetTileRadius(float tileRadius)
{
	m_TileRadius = tileRadius;
	m_BoundaryDistance = sqrtf(FMath::Square(m_TileRadius) - FMath::Square(0.5*m_TileRadius));
}

//========================================================================
