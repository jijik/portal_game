// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <array>
#include <functional>
#include <ostream>
#include <fstream>
#include "Runtime/Slate/Public/Widgets/Notifications/SNotificationList.h"
#include "Runtime/Slate/Public/Framework/Notifications/NotificationManager.h"

//========================================================================
extern class AHexEditorActor* gHexEditor;
extern class AHexGame* gHexGame;

//========================================================================
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

//========================================================================
namespace std
{
	template<typename T1, typename T2>
	struct hash < pair<T1, T2> >
	{
		inline size_t operator()(const pair<T1, T2> & v) const
		{
			size_t seed = 0;
			hash_combine(seed, v.first);
			hash_combine(seed, v.second);
			return seed;
		}
	};
}

//========================================================================
template <typename T>
using Array6 = std::array<T, 6>;
template <typename T>
using Array12 = std::array<T, 12>;

//========================================================================
#define Cont(container) std::begin(container), std::end(container)

//========================================================================
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text)
#define print_frame(text, dt) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, dt, FColor::White,text)

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
template <typename DesiredType, typename HitFunc>
void	Raycast(AActor* actor, HitFunc hitFunc, std::function<void()> noHitFunc = []() {})
{
	auto* pc = actor->GetWorld()->GetFirstPlayerController();
	FHitResult TraceResult(ForceInit);
	pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Camera), false, TraceResult);
	auto* resultActor = Cast<DesiredType>(TraceResult.GetActor());
	if (resultActor)
	{
		hitFunc(resultActor, TraceResult);
	}
	else
	{
		noHitFunc();
	}
}