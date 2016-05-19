// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "TeleportActor.h"

//========================================================================
ATeleportActor::ATeleportActor()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Obj(TEXT("StaticMesh'/Game/Models/Teleport.Teleport'"));

	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->SetStaticMesh(Obj.Object);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

