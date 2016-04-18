// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "HexEditorHUD.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAME_API AHexEditorHUD : public AHUD
{
	GENERATED_BODY()
public:
	AHexEditorHUD();
	virtual void DrawHUD() override;

	UMaterial* m_CursorMat;
};
