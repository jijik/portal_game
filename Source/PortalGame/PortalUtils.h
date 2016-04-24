// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <array>
#include <ostream>
#include <fstream>
#include "Runtime/Slate/Public/Widgets/Notifications/SNotificationList.h"
#include "Runtime/Slate/Public/Framework/Notifications/NotificationManager.h"

//========================================================================
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

//========================================================================
template <typename T>
using Array6 = std::array<T, 6>;

//========================================================================
#define Cont(container) std::begin(container), std::end(container)

//========================================================================
extern class AHexEditorActor* gHexEditor;

//========================================================================
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text)

//========================================================================
template<typename T>
std::ostream& binary_write(std::ostream& stream, const T& value) {
	return stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template<typename T>
std::istream & binary_read(std::istream& stream, T& value) {
	return stream.read(reinterpret_cast<char*>(&value), sizeof(T));
}

//========================================================================