// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BarrierActor.generated.h"

UCLASS()
class PORTALGAME_API ABarrierActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	ABarrierActor();

	void				Init();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
