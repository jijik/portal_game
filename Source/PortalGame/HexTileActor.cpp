// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "PortalUtils.h"
#include "BarrierActor.h"
#include "HexEditorActor.h"
#include "HexTileActor.h"

//========================================================================
void AHexTileActor::Init(const S_HexCoordinates& coordinates)
{
	SetCoordinates(coordinates);

	auto pos = gHexEditor->GetHexGrid().GetPosition(coordinates);

	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->SetStaticMesh(gHexEditor->AvailableTiles[0]);

	SetActorLocation(pos);

	meshComp->OnClicked.AddDynamic(this, &AHexTileActor::OnClick);

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

//========================================================================
void AHexTileActor::OnClick(UPrimitiveComponent*)
{
	gHexEditor->ClickOnTile(*this);
}

//========================================================================
const S_HexCoordinates& AHexTileActor::GetCoordinates()
{
	return m_Coordinates;
}

//========================================================================
void AHexTileActor::SetCoordinates(const S_HexCoordinates& c)
{
	m_Coordinates = c;
}

//========================================================================
void AHexTileActor::PlaceBarrierAt(ABarrierActor& b, HexDir id)
{
	check(!HasBarrierAt(id));
	m_NeighborBarriers[id] = &b;
}

//========================================================================
void AHexTileActor::RemoveBarrierAt(HexDir id)
{
	check(HasBarrierAt(id));
	m_NeighborBarriers[id] = nullptr;
}

//========================================================================
bool AHexTileActor::HasBarrierAt(HexDir id)
{
	return m_NeighborBarriers[id] != nullptr;
}

//========================================================================
ABarrierActor* AHexTileActor::GetBarrierAt(HexDir id)
{
	if (id == InvalidHexDir)
	{
		return nullptr;
	}
	return m_NeighborBarriers[id];
}

//========================================================================
void AHexTileActor::Save(std::ofstream& stream)
{
	binary_write(stream, m_CurrentModelId);
	m_Coordinates.Save(stream);

	binary_write(stream, GetActorLocation());
	binary_write(stream, GetActorRotation());
}

//========================================================================
void AHexTileActor::Load(std::ifstream& stream)
{
	binary_read(stream, m_CurrentModelId);
	m_Coordinates.Load(stream);

	GetStaticMeshComponent()->SetStaticMesh(gHexEditor->AvailableTiles[m_CurrentModelId]);

	FVector pos;
	FRotator rot;
	binary_read(stream, pos);
	binary_read(stream, rot);
	SetActorLocationAndRotation(pos, rot);
}
