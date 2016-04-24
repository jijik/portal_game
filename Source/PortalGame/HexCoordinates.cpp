// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexCoordinates.h"
#include "HexGrid.h"

//========================================================================
S_HexCoordinates::S_HexCoordinates() 
	: S_HexCoordinates(InvalidHexCoordinate, InvalidHexCoordinate, InvalidHexCoordinate)
{

}

//========================================================================
S_HexCoordinates::S_HexCoordinates(HexCoordinate _x, HexCoordinate _y)
	: s(_x)
	, t(_y)
	, z(0)
{

}

//========================================================================
S_HexCoordinates::S_HexCoordinates(HexCoordinate _x, HexCoordinate _y, HexCoordinate _z)
	: s(_x)
	, t(_y)
	, z(_z)
{

}

//========================================================================
HexDir S_HexCoordinates::ToHexDir() const
{
	auto& indexes = C_HexGrid<void*>::HorizontalNeighborIndexes;
	auto it = std::find(Cont(indexes), *this);
	if (it == indexes.end())
	{
		return InvalidHexDir;
	}
	return it - indexes.begin();
}

//========================================================================
bool S_HexCoordinates::Valid()
{
	return s != InvalidHexCoordinate && t != InvalidHexCoordinate && z != InvalidHexCoordinate;
}

//========================================================================
bool S_HexCoordinates::IsHorizontalDir() const 
{
	return z == 0;
}

//========================================================================
bool S_HexCoordinates::IsZero() const
{
	return s == t && t == z && z == 0;
}

//========================================================================
void S_HexCoordinates::Save(std::ofstream& stream)
{
	stream << s << t << z;
}

//========================================================================
bool S_HexCoordinates::operator==(const S_HexCoordinates& rhs) const
{
	return rhs.s == s && rhs.t == t && rhs.z == z;
}

//========================================================================
S_HexCoordinates operator+(const S_HexCoordinates& lhs, const S_HexCoordinates& rhs)
{
	return S_HexCoordinates{ lhs.s + rhs.s, lhs.t + rhs.t, lhs.z + rhs.z };
}

//========================================================================
S_HexCoordinates operator+(const S_HexCoordinates& lhs, HexDir dir)
{
	auto relativeCoords = C_HexGrid<void*>::HorizontalNeighborIndexes[dir];
	return lhs + relativeCoords;
}
