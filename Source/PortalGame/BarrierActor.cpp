// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "BarrierActor.h"
#include "HexEditorActor.h"

//========================================================================
ABarrierActor::ABarrierActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABarrierActor::Init()
{
	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->SetStaticMesh(gHexEditor->AvailableBarriers[0]);
}

//========================================================================
void ABarrierActor::BeginPlay()
{
	Super::BeginPlay();
}

//========================================================================
void ABarrierActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

