// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "PortalUtils.h"
#include "FinishActor.generated.h"

UCLASS()
class PORTALGAME_API AFinishActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	AFinishActor();

	void	Tick(float DeltaSeconds);

	void	Save(std::ofstream& file);
	void	Load(std::ifstream& file);
};
