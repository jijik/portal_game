// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PortalUtils.h"

using HexCoordinate = int;

static const	HexCoordinate InvalidHexCoordinate = std::numeric_limits<HexCoordinate>::max();

struct PORTALGAME_API S_HexCoordinates
{
	HexCoordinate s, t;

	S_HexCoordinates() : S_HexCoordinates(InvalidHexCoordinate, InvalidHexCoordinate) {}
	S_HexCoordinates(HexCoordinate _x, HexCoordinate _y) : s(_x), t(_y) {}

	bool operator==(const S_HexCoordinates& rhs) const
	{
		return rhs.s == s && rhs.t == t;
	}

	bool Valid() { return s != InvalidHexCoordinate && t != InvalidHexCoordinate; }

	friend S_HexCoordinates operator+(const S_HexCoordinates&, const S_HexCoordinates&);

};

inline S_HexCoordinates operator+(const S_HexCoordinates& lhs, const S_HexCoordinates& rhs)
{
	return S_HexCoordinates{ lhs.s + rhs.s, lhs.t + rhs.t };
}

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
			return hash;
		}
	};
}