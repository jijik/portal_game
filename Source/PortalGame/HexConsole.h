// Fill out your copyright notice in the Description page of Project Settings.

#include "Internationalization/Text.h"
#include "Containers/UnrealString.h"

#include "HexEditorActor.h"
#include "PortalAI.h"
#include "PortalUtils.h"

#pragma once

//========================================================================
static void Hex_SaveMap(const TArray< FString >& params)
{
	if (params.Num())
	{
		gHexEditor->SaveMap(params[0]);
	}
	else
	{
		print(TEXT("invalid map name"));
	}
}

//========================================================================
static void Hex_LoadMap(const TArray< FString >& params)
{
	if (params.Num())
	{
		gHexEditor->LoadMap(params[0]);
	}
	else
	{
		print(TEXT("invalid map name"));
	}
}

//========================================================================
static void Hex_Solve(const TArray< FString >& params)
{
	gHexEditor->m_PortalAI->Solve();
}

//========================================================================
namespace HexConsole
{
	static FAutoConsoleCommand c01(TEXT("Hex_SaveMap"), TEXT("mapname"), FConsoleCommandWithArgsDelegate::CreateStatic(&Hex_SaveMap));
	static FAutoConsoleCommand c02(TEXT("Hex_LoadMap"), TEXT("mapname"), FConsoleCommandWithArgsDelegate::CreateStatic(&Hex_LoadMap));
	static FAutoConsoleCommand c03(TEXT("Hex_Solve"), TEXT(""), FConsoleCommandWithArgsDelegate::CreateStatic(&Hex_Solve));
};
