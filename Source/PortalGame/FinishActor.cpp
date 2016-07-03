// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexGame.h"
#include "Dude.h"
#include "FinishActor.h"

//========================================================================
AFinishActor::AFinishActor()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Obj(TEXT("StaticMesh'/Game/Models/Finish.Finish'"));

	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->SetStaticMesh(Obj.Object);

	PrimaryActorTick.bCanEverTick = true;
}

//========================================================================
void AFinishActor::Tick(float DeltaSeconds)
{
	auto dist = FVector::Dist(gHexGame->Dude->GetActorLocation(), GetActorLocation());
	if (dist < 40)
	{
		print_frame("YOU'VE WON", DeltaSeconds);
	}
}

//========================================================================
void AFinishActor::Save(std::ofstream& file)
{
	binary_write(file, GetActorLocation());
}

//========================================================================
void AFinishActor::Load(std::ifstream& file)
{
	FVector pos;
	binary_read(file, pos);
	SetActorLocation(pos);
}
