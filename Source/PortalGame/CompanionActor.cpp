// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "Dude.h"
#include "HexGame.h"
#include "CompanionActor.h"

//========================================================================
ACompanionActor::ACompanionActor()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Obj(TEXT("StaticMesh'/Game/Models/Companion.Companion'"));
	m_Mesh = Obj.Object;

	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->SetStaticMesh(m_Mesh);

	meshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	meshComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	meshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Block);
}

//========================================================================
void ACompanionActor::Init(const FVector& pos)
{
	auto* meshComp = GetStaticMeshComponent();

	SetActorLocation(pos);
}

//========================================================================
void ACompanionActor::OnClick()
{
	auto dudePos = gHexGame->Dude->GetActorLocation();

	//if (FVector::Dist(dudePos, GetActorLocation()) < 30)
	{
		gHexGame->Dude->Pick(this);
	}
}

//========================================================================
void ACompanionActor::Save(std::ofstream& file)
{
	binary_write(file, GetActorLocation());
}

//========================================================================
void ACompanionActor::Load(std::ifstream& file)
{
	FVector pos;
	binary_read(file, pos);
	SetActorLocation(pos);
}

//========================================================================
