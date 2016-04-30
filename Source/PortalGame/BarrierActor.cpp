// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "BarrierActor.h"
#include "HexEditorActor.h"

//========================================================================
ABarrierActor::ABarrierActor()
{
	PrimaryActorTick.bCanEverTick = true;
	m_Neighbors.first.neighbor = m_Neighbors.second.neighbor = nullptr;
}

//========================================================================
void ABarrierActor::Init()
{
	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->SetStaticMesh(gHexEditor->AvailableBarriers[0]);

	meshComp->OnClicked.AddDynamic(this, &ABarrierActor::OnClick);
	meshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	AttachRootComponentToActor(gHexEditor);
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
void ABarrierActor::SetOwningTileBeforePlace(AHexTileActor* a, HexDir sector)
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
void ABarrierActor::Place(AHexTileActor& front, HexDir frontSlot, AHexTileActor* back, HexDir backSlot)
{
	m_Neighbors.first = { &front , frontSlot };
	m_Neighbors.second = { back, backSlot };

	GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

//========================================================================
bool ABarrierActor::UnlinkTileFomBarrier(AHexTileActor& tile)
{
	if (m_Neighbors.second.neighbor == &tile)
	{
		m_Neighbors.second.neighbor = nullptr;
		m_Neighbors.second.slotAtNeighbor = InvalidHexDir;
	}
	else if (m_Neighbors.first.neighbor == &tile)
	{
		m_Neighbors.first = m_Neighbors.second; //first is always valid
		m_Neighbors.second.neighbor = nullptr;
		m_Neighbors.second.slotAtNeighbor = InvalidHexDir;
	}
	else
	{
		check(false);
	}

	return m_Neighbors.first.neighbor == nullptr;
}

//========================================================================
void ABarrierActor::UnlinkBarrierFromNeighborTiles()
{
	if (m_Neighbors.first.neighbor)
	{
		m_Neighbors.first.neighbor->RemoveBarrierAt(m_Neighbors.first.slotAtNeighbor);
	}

	if (m_Neighbors.second.neighbor)
	{
		m_Neighbors.second.neighbor->RemoveBarrierAt(m_Neighbors.second.slotAtNeighbor);
	}
}

//========================================================================
void ABarrierActor::OnClick(UPrimitiveComponent*)
{
	gHexEditor->SelectBarrier(this);
}

//========================================================================
void ABarrierActor::SetSelectedMaterial(bool b)
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
void ABarrierActor::Save(std::ofstream& file)
{
	binary_write(file, m_CurrentModelId);
	
	m_Neighbors.first.neighbor->GetCoordinates().Save(file); //id
	binary_write(file, m_Neighbors.first.slotAtNeighbor);

	binary_write(file, GetActorLocation());
	binary_write(file, GetActorRotation());
}

//========================================================================
void ABarrierActor::Load(std::ifstream& file)
{
	binary_read(file, m_CurrentModelId);

	//will be placed later
	S_HexCoordinates coords;
	coords.Load(file);
	binary_read(file, m_OwningSectorBeforePlace);

	m_OwningTileBeforePlace = gHexEditor->GetHexGrid().GetElement(coords);
	check(m_OwningTileBeforePlace);

	GetStaticMeshComponent()->SetStaticMesh(gHexEditor->AvailableBarriers[m_CurrentModelId]);

	FRotator rot;
	FVector pos;
	binary_read(file, pos);
	binary_read(file, rot);
	SetActorLocationAndRotation(pos, rot);
}
