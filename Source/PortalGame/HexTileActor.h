// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HexTileActor.generated.h"

UCLASS()
class PORTALGAME_API AHexTileActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexTileActor();

	void	Init(const struct S_HexCoordinates& coordinates);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	void SetSelectedMaterial(bool);
};
