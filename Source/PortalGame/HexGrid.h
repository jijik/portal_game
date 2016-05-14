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
	static const Array12<S_HexCoordinates> HorizontalNextNeighborIndexes;
	static unsigned GetComplementaryNeighborIndex(unsigned id);

	void		Init();
	void		InsertElement(const S_HexCoordinates& coordinates, const T& element);
	T				GetElement(const S_HexCoordinates& coordinates);
	void		RemoveElement(const S_HexCoordinates& coordinates);
	void		FillHorizontalNeighbors(const S_HexCoordinates& origin, Array6<S_HexCoordinates>& toFill);
	void		SetTileRadius(float tileRadius);
	FVector	GetPosition(const S_HexCoordinates& coordinates);
	S_HexCoordinates	GetCoordinates(const FVector& position);
	FVector	GetPositionBetweenTiles(const S_HexCoordinates& c1, const S_HexCoordinates& c2);

	using T_Storage = std::unordered_map<S_HexCoordinates, T>;
	const T_Storage& GetStorage();

private:
	T_Storage					m_GridMap;
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
const Array12<S_HexCoordinates> C_HexGrid<T>::HorizontalNextNeighborIndexes = {S_HexCoordinates(2, 0),
																																							S_HexCoordinates(2,-1),
																																							S_HexCoordinates(2,-2),
																																							S_HexCoordinates(1,-2),
																																							S_HexCoordinates(0,-2),
																																							S_HexCoordinates(-1, -1),
																																							S_HexCoordinates(-2, 0),
																																							S_HexCoordinates(-2, 1),
																																							S_HexCoordinates(-2, 2),
																																							S_HexCoordinates(-1, 2),
																																							S_HexCoordinates(0, 2),
																																							S_HexCoordinates(1, 1)};

//========================================================================
template <typename T>
unsigned C_HexGrid<T>::GetComplementaryNeighborIndex(unsigned id)
{
	return (id + 3) % 6;
}

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
S_HexCoordinates C_HexGrid<T>::GetCoordinates(const FVector& pos)
{
	S_HexCoordinates ret(0,0,0);

	const auto r = m_TileRadius;
	const auto d = m_BoundaryDistance;

	// assume rectangular tiles (with height of 3/2 radius), starting at pointy top
	// having two top corners belonging to the other tiles 

	float t = (pos.X - r) / (-1.5f * r);
	ret.t = FMath::FloorToInt(t);

	float s = (pos.Y + d - (ret.t * d)) / (2 * d);
	ret.s = FMath::FloorToInt(s);

	//calculate if the point lies within corners (map top part of rectangle tile to [0-1]x[0-1]

	float garbage;
	float a = modf(t, &garbage);
	float b = modf(s, &garbage);

	if (a < 0) a += 1;
	if (b < 0) b += 1;

	a *= 3;

	if (1.0f - a > 2.0f * b)
	{
		ret.t -= 1;
	}

	if (a < 2.0f * (b - 0.5f))
	{
		ret.t -= 1;
		ret.s += 1;
	}

	ret.z = pos.Z / m_TileRadius;

	return ret;
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
template <typename T>
const typename C_HexGrid<T>::T_Storage& C_HexGrid<T>::GetStorage()
{
	return m_GridMap;
}


//========================================================================
