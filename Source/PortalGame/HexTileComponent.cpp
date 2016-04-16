// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexTileComponent.h"

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