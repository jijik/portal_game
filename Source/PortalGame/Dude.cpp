// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "Dude.h"
#include "CompanionActor.h"
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

//========================================================================
void ADude::Pick(ACompanionActor* companion)
{
	m_Companion = companion;

 	m_Companion->SetActorRelativeLocation(FVector(0, 0, 100));
 	m_Companion->SetActorRelativeScale3D(FVector(1, 1, 1));
 	m_Companion->SetActorRelativeRotation(FRotator(0, 0, 0));

	companion->AttachRootComponentToActor(this);
}

//========================================================================