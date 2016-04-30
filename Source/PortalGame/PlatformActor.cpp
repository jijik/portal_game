// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "PlatformActor.h"
#include "HexEditorActor.h"

//========================================================================
void APlatformActor::Init()
{
	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->SetStaticMesh(gHexEditor->AvailablePlatforms[0]);

	meshComp->OnClicked.AddDynamic(this, &APlatformActor::OnClick);
	meshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	AttachRootComponentToActor(gHexEditor);
}

//========================================================================
void APlatformActor::BeginPlay()
{
	Super::BeginPlay();
}

//========================================================================
void APlatformActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

//========================================================================
void APlatformActor::CycleModel()
{
	++m_CurrentModelId;
	m_CurrentModelId %= gHexEditor->AvailablePlatforms.Num();
	GetStaticMeshComponent()->SetStaticMesh(gHexEditor->AvailablePlatforms[m_CurrentModelId]);
}

//========================================================================
void APlatformActor::SetOwningTileBeforePlace(AHexTileActor* a) 
{
	m_OwningTileBeforePlace = a;
}

//========================================================================
bool APlatformActor::IsReadyToPlace()
{
	return m_OwningTileBeforePlace != nullptr;
}

//========================================================================
AHexTileActor* APlatformActor::GetOwningTileBeforePlace()
{
	return m_OwningTileBeforePlace;
}

//========================================================================
void APlatformActor::Place(AHexTileActor& owner)
{
	m_OwnerTile = &owner;
	GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

//========================================================================
void APlatformActor::UnlinkPlatformFromOwningTile()
{
	check(m_OwnerTile);
	m_OwnerTile->RemovePlatform();
}

//========================================================================
void APlatformActor::OnClick(UPrimitiveComponent*pc)
{
	gHexEditor->SelectPlatform(this);
}

//========================================================================
void APlatformActor::SetSelectedMaterial(bool b)
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
void APlatformActor::Save(std::ofstream& file)
{
	binary_write(file, m_CurrentModelId);

	m_OwnerTile->GetCoordinates().Save(file); //id

	binary_write(file, GetActorLocation());
}

//========================================================================
void APlatformActor::Load(std::ifstream& file)
{
	binary_read(file, m_CurrentModelId);

	//will be placed later
	S_HexCoordinates coords;
	coords.Load(file);

	m_OwningTileBeforePlace = gHexEditor->GetHexGrid().GetElement(coords);
	check(m_OwningTileBeforePlace);

	GetStaticMeshComponent()->SetStaticMesh(gHexEditor->AvailablePlatforms[m_CurrentModelId]);

	FVector pos;
	binary_read(file, pos);
	SetActorLocation(pos);
}
