// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexTileComponent.h"

UHexTileComponent::UHexTileComponent()
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

//========================================================================
void UHexTileComponent::BeginPlay()
{
	Super::BeginPlay();
}

//========================================================================
void UHexTileComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

//========================================================================
void UHexTileComponent::SetCoordinates(const S_HexCoordinates& c)
{
	m_HexCoordinates = c;
}

//========================================================================
const S_HexCoordinates& UHexTileComponent::GetCoordinates()
{
	return m_HexCoordinates;
}

//========================================================================
void UHexTileComponent::OnClick(UPrimitiveComponent* ClickedComp)
{
	gHexEditor->SelectTile(this);
}