// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "Dude.h"
#include "AIController.h"
#include "HexGame.h"
#include "PortalUtils.h"

//========================================================================
ADude::ADude()
{
	PrimaryActorTick.bCanEverTick = true;
}

//========================================================================
void ADude::BeginPlay()
{
	Super::BeginPlay();
}

//========================================================================
void ADude::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

//========================================================================
void ADude::Move()
{
	AAIController* controller = CastChecked<AAIController>(GetController());
	auto* pc = GetWorld()->GetFirstPlayerController();
	FHitResult TraceResult(ForceInit);
	pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic), false, TraceResult);
	auto* actor = TraceResult.GetActor();
	if (actor)
	{
		controller->MoveToLocation(TraceResult.Location);
	}
}
