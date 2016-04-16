// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HexTileActor.generated.h"

UCLASS()
class PORTALGAME_API AHexTileActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	void			Init(const struct S_HexCoordinates& coordinates);
	void			SetSelectedMaterial(bool);
	void			CycleModel();
	void			RotateModel();

private:
	unsigned	m_CurrentModelId = 0;
};
