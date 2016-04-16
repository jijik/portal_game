// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "PortalUtils.h"
#include "HexEditorActor.h"
#include "HexTileActor.h"
#include "HexTileComponent.h"

//========================================================================
void AHexTileActor::Init(const S_HexCoordinates& coordinates)
{
	auto pos = gHexEditor->GetHexGrid().GetPosition(coordinates);

	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->SetStaticMesh(gHexEditor->AvailableTiles[0]);

	SetActorLocation(pos);

	UHexTileComponent* hexComp = NewObject<UHexTileComponent>(this);
	hexComp->RegisterComponent();
	hexComp->SetCoordinates(coordinates);

	meshComp->OnClicked.AddDynamic(hexComp, &UHexTileComponent::OnClick);

	SetSelectedMaterial(false);
}

//========================================================================
void AHexTileActor::SetSelectedMaterial(bool b)
{
	if (b)
	{
		GetStaticMeshComponent()->SetMaterial(0, gHexEditor->m_SelectedMaterial);
	}
	else
	{
		GetStaticMeshComponent()->SetMaterial(0, gHexEditor->m_DefaultMaterial);
	}
}

//========================================================================
void AHexTileActor::CycleModel()
{
	++m_CurrentModelId;
	m_CurrentModelId %= gHexEditor->AvailableTiles.Num();
	GetStaticMeshComponent()->SetStaticMesh(gHexEditor->AvailableTiles[m_CurrentModelId]);
}

//========================================================================
void AHexTileActor::RotateModel()
{
	auto rotator = GetActorRotation();
	rotator.Yaw += 60;
	SetActorRotation(rotator);
}