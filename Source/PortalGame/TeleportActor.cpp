// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexGame.h"
#include "Dude.h"
#include "HexEditorActor.h"
#include "TeleportActor.h"

unsigned ATeleportActor::IDProvider = 1;

//========================================================================
ATeleportActor::ATeleportActor()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Obj(TEXT("StaticMesh'/Game/Models/Teleport.Teleport'"));

	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->SetStaticMesh(Obj.Object);

	m_Id = ++IDProvider;

	PrimaryActorTick.bCanEverTick = true;
}

//========================================================================
void ATeleportActor::Tick(float DeltaSeconds)
{
	if (m_LinkedTeleport)
	{
		auto z = FVector(0, 0, 30);
		DrawDebugLine(GetWorld(), GetActorLocation() + z, m_LinkedTeleport->GetActorLocation() + z, FColor::Yellow, false, -1.f, 0, 1.3f);
	}
}

//========================================================================
void ATeleportActor::Link(ATeleportActor& other)
{
	m_LinkedTeleport = &other;
}

//========================================================================
void ATeleportActor::Use(AActor& actor)
{
	if (m_LinkedTeleport)
	{
		gHexGame->Dude->Stop();
		actor.SetActorLocation(m_LinkedTeleport->GetActorLocation() + FVector(0,0,20));
	}
}

//========================================================================
void ATeleportActor::Save(std::ofstream& file)
{
	binary_write(file, m_Id);
	binary_write(file, GetActorLocation());

	if (m_LinkedTeleport)
	{
		binary_write(file, m_LinkedTeleport->m_Id);
	}
	else
	{
		binary_write(file, 0u);
	}
}

//========================================================================
void ATeleportActor::Load(std::ifstream& file)
{
	binary_read(file, m_Id);

	FVector pos;
	binary_read(file, pos);
	SetActorLocation(pos);

	binary_read(file, m_LoadedId);
}

//========================================================================
void ATeleportActor::PostLoad()
{
	if (m_LoadedId)
	{
		auto it = std::find_if(Cont(gHexEditor->m_AllTeleports), [&](auto* t) { return t->GetId() == m_LoadedId; });
		if (it != gHexEditor->m_AllTeleports.end())
		{
			Link(**it);
		}
	}
}