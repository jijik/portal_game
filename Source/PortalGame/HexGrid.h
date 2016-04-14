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
	static const Array6<S_HexCoordinates> NeighborIndexes;

	void		Init();
	void		InsertElement(const S_HexCoordinates& coordinates, const T& element);
	T				GetElement(const S_HexCoordinates& coordinates);
	void		FillNeighbors(const S_HexCoordinates& origin, Array6<S_HexCoordinates>& toFill);
	void		SetTileRadius(float tileRadius);
	FVector	GetPosition(const S_HexCoordinates& coordinates);

private:
	std::unordered_map<S_HexCoordinates, T>		m_GridMap;
	float							m_TileRadius;
	float							m_BoundaryDistance;
	S_HexCoordinates	m_SelectedTile;
};

//========================================================================
//========================================================================
template <typename T>
const Array6<S_HexCoordinates> C_HexGrid<T>::NeighborIndexes = { { 1,0 },{ 1,-1 },{ 0,-1 },{ 0,-1 },{ -1,0 },{ -1,1 } };

//========================================================================
template <typename T>
void C_HexGrid<T>::Init()
{
	
}

//========================================================================
template <typename T>
T C_HexGrid<T>::GetElement(const S_HexCoordinates& coordinates)
{
	return m_GridMap[coordinates];
}

//========================================================================
template <typename T>
void C_HexGrid<T>::InsertElement(const S_HexCoordinates& coordinates, const T& element)
{
	m_GridMap[coordinates] = element;
}

//========================================================================
template <typename T>
void C_HexGrid<T>::FillNeighbors(const S_HexCoordinates& origin, Array6<S_HexCoordinates>& toFill)
{
	for (unsigned i = 0; i < 6; ++i)
	{
		toFill[i] = origin + NeighborIndexes[i];
	}
}

//========================================================================
template <typename T>
FVector C_HexGrid<T>::GetPosition(const S_HexCoordinates& coordinates)
{
	auto x = coordinates.t * 1.5 * -m_TileRadius;
	auto y = coordinates.s * 2.0 * m_BoundaryDistance + coordinates.t * m_BoundaryDistance;
	return FVector(x, y, 0.0);
}

//========================================================================
template <typename T>
void C_HexGrid<T>::SetTileRadius(float tileRadius)
{
	m_TileRadius = tileRadius;
	m_BoundaryDistance = sqrtf(FMath::Square(m_TileRadius) - FMath::Square(0.5*m_TileRadius));
}

//========================================================================
