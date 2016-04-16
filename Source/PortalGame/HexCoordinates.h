// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PortalUtils.h"

using HexCoordinate = int;

static const	HexCoordinate InvalidHexCoordinate = std::numeric_limits<HexCoordinate>::max();

struct PORTALGAME_API S_HexCoordinates
{
	HexCoordinate s, t, z;

	S_HexCoordinates();
	S_HexCoordinates(HexCoordinate _x, HexCoordinate _y);
	S_HexCoordinates(HexCoordinate _x, HexCoordinate _y, HexCoordinate _z);

	bool operator==(const S_HexCoordinates& rhs) const;

	bool Valid();

	friend S_HexCoordinates operator+(const S_HexCoordinates&, const S_HexCoordinates&);

};

S_HexCoordinates operator+(const S_HexCoordinates& lhs, const S_HexCoordinates& rhs);

namespace std
{
	template<>
	struct hash<S_HexCoordinates>
	{
		std::size_t operator()(const S_HexCoordinates& rhs) const
		{
			std::size_t hash = 0;
			hash_combine(hash, rhs.s);
			hash_combine(hash, rhs.t);
			hash_combine(hash, rhs.z);
			return hash;
		}
	};
}