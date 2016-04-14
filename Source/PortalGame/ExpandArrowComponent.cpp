// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorActor.h"
#include "ExpandArrowComponent.h"

UExpandArrowComponent::UExpandArrowComponent()
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UExpandArrowComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UExpandArrowComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
}

void UExpandArrowComponent::OnClick(UPrimitiveComponent* ClickedComp)
{
	gHexEditor->Expand(m_RelativeDirection);
}
