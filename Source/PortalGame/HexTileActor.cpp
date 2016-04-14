// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "PortalUtils.h"
#include "HexEditorActor.h"
#include "HexTileActor.h"
#include "HexTileComponent.h"

AHexTileActor::AHexTileActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHexTileActor::Init(const S_HexCoordinates& coordinates)
{
	auto pos = gHexEditor->GetHexGrid().GetPosition(coordinates);

	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->SetStaticMesh(gHexEditor->m_AvailableTiles[0]);

	SetActorLocation(pos);

	UHexTileComponent* hexComp = NewObject<UHexTileComponent>(this);
	hexComp->RegisterComponent();
	hexComp->SetCoordinates(coordinates);

	meshComp->OnClicked.AddDynamic(hexComp, &UHexTileComponent::OnClick);
}

void AHexTileActor::BeginPlay()
{
	Super::BeginPlay();
}

void AHexTileActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

