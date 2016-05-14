// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "PortalUtils.h"
#include "TurretActor.generated.h"

UCLASS()
class PORTALGAME_API ATurretActor : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	ATurretActor();
	virtual void		Tick(float DeltaSeconds) override;

	void	Save(std::ofstream& file);
	void	Load(std::ifstream& file);
	
};
