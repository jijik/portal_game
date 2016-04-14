// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexTileActor.h"

AHexTileActor::AHexTileActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHexTileActor::BeginPlay()
{
	Super::BeginPlay();
}

void AHexTileActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

