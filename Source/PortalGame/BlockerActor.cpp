// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexTileActor.h"
#include "BarrierActor.h"
#include "HexEditorActor.h"
#include "BlockerActor.h"

//========================================================================
ABlockerActor::ABlockerActor()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Obj(TEXT("StaticMesh'/Game/Models/Blocker.Blocker'"));
	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetStaticMesh(Obj.Object);
}

//========================================================================
void ABlockerActor::Tick(float DeltaSeconds)
{
	if (m_Placing)
	{
		Raycast<AActor>(this,
			[&](auto& resultActor, auto& traceResult)
		{
			auto mousePos = traceResult.Location;
			auto toMouse = mousePos - GetActorLocation();
			toMouse.Z = 0;

			unsigned id = gHexEditor->GetNeighborId(toMouse);
			
			SetActorRotation(FRotator(0, 60 * -(int)id + 90, 0));

			m_NeighborId = id;
		});
	}

	auto barrier = m_BaseTile->GetBarrierAt(m_NeighborId);
	if (barrier)
	{
		barrier->Off();
	}
	
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorRotation().Vector() * 250, FColor::Red, false, -1.f, 0, 4.0f);
}

//========================================================================
void ABlockerActor::OnDrop()
{
	m_Placing = true;
}

//========================================================================
void ABlockerActor::OnPick()
{
	m_NeighborId = InvalidHexDir;
	m_Placing = false;
}