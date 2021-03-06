// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexGame.h"
#include "Dude.h"
#include "PortalUtils.h"
#include "PlatformActor.h"
#include "HexEditorActor.h"
#include "CompanionActor.h"
#include "BarrierActor.h"

//========================================================================
APlatformActor::APlatformActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

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

	if (m_Target)
	{
		auto z = FVector(0, 0, 30);
		DrawDebugLine(GetWorld(), GetActorLocation() + z, m_Target->GetActorLocation() + z, FColor::Yellow, false, -1.f, 0, 1.3f);

		auto d = 40.0f;

		auto mypos = GetActorLocation();

		bool hasCompanion = std::any_of(Cont(gHexEditor->m_AllCompanions), [&](auto& c) { return FVector::Dist(mypos, c->GetActorLocation()) < d; });
		if (hasCompanion)
		{
			m_Target->Off();
			return;
		}

		auto dist = FVector::Dist(gHexGame->Dude->GetActorLocation(), GetActorLocation());
		if (dist < d)
		{
			m_Target->Off();
		}
	}
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
void APlatformActor::SetTarget(ABarrierActor* a)
{
	m_Target = a;
}

//========================================================================
ABarrierActor* APlatformActor::GetTarget()
{
	return m_Target;
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

	bool hasTarget = m_Target != nullptr;
	binary_write(file, hasTarget);

	if (hasTarget)
	{
		binary_write(file, m_Target->GetId());
	}
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

	bool hasTarget;
	binary_read(file, hasTarget);

	if (hasTarget)
	{
		unsigned id;
		binary_read(file, id);

		auto& cont = gHexEditor->m_AllBarriers;
		auto it = std::find_if(cont.begin(), cont.end(), [&](ABarrierActor* a) { return a->GetId() == id; });
		check(it != cont.end());
		m_Target = *it;
	}
}
