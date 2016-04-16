// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorActor.h"
#include "ExpandArrowComponent.h"

void UExpandArrowComponent::SetRelativeDirection(const S_HexCoordinates& dir)
{
	check(dir.z == 0); m_RelativeDirection = dir;
}

void UExpandArrowComponent::OnClick(UPrimitiveComponent*)
{
	gHexEditor->Expand(m_RelativeDirection);
}
