// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "BarrierActor.h"
#include "HexEditorActor.h"

//========================================================================
ABarrierActor::ABarrierActor()
{
	PrimaryActorTick.bCanEverTick = true;
	m_Neighbors.first = m_Neighbors.second = nullptr;
}

//========================================================================
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

//========================================================================
void ABarrierActor::CycleModel()
{
	++m_CurrentModelId;
	m_CurrentModelId %= gHexEditor->AvailableBarriers.Num();
	GetStaticMeshComponent()->SetStaticMesh(gHexEditor->AvailableBarriers[m_CurrentModelId]);
}

//========================================================================
void ABarrierActor::SetOwningTileBeforePlace(AHexTileActor* a, unsigned sector)
{
	m_OwningTileBeforePlace = a;
	m_OwningSectorBeforePlace = sector;
}

//========================================================================
bool ABarrierActor::IsReadyToPlace()
{
	return m_OwningTileBeforePlace != nullptr;
}

//========================================================================
AHexTileActor* ABarrierActor::GetOwningTileBeforePlace()
{
	return m_OwningTileBeforePlace;
}

//========================================================================
unsigned ABarrierActor::GetOwningSectorBeforePlace()
{
	return m_OwningSectorBeforePlace;
}

//========================================================================
void ABarrierActor::Place(AHexTileActor& front, AHexTileActor* back)
{
	m_Neighbors.first = &front;
	m_Neighbors.second = back;
}

//========================================================================