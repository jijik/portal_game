// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexCoordinates.h"

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
bool S_HexCoordinates::Valid()
{
	return s != InvalidHexCoordinate && t != InvalidHexCoordinate && z != InvalidHexCoordinate;
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
