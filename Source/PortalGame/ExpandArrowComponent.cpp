// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorActor.h"
#include "ExpandArrowComponent.h"

void UExpandArrowComponent::SetRelativeDirection(HexDir dir)
{
	m_RelativeDirection = dir;
}

void UExpandArrowComponent::OnClick(UPrimitiveComponent*)
{
	auto& dir = gHexEditor->GetHexGrid().HorizontalNeighborIndexes[m_RelativeDirection];
	gHexEditor->Expand(dir);
}
