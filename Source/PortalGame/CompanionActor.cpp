// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "Dude.h"
#include "HexGame.h"
#include "CompanionActor.h"
#include "DudeActions.h"

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
	SetActorLocation(pos);
}

//========================================================================
void ACompanionActor::OnClick()
{
	auto& dude = *gHexGame->Dude;
	auto* gotoAction = new C_DudeMoveTo(dude);
	gotoAction->target = GetActorLocation();

	auto* pickupAction = new C_DudePick(dude);
	pickupAction->companion = this;

	dude.PushActions({ gotoAction, pickupAction });
}

//========================================================================
void ACompanionActor::OnDrop()
{

}

//========================================================================
void ACompanionActor::Save(std::ofstream& file)
{
	binary_write(file, GetActorLocation());
	binary_write(file, GetActorRotation());
}

//========================================================================
void ACompanionActor::Load(std::ifstream& file)
{
	FVector pos;
	FRotator rot;
	binary_read(file, pos);
	binary_read(file, rot);
	SetActorLocationAndRotation(pos, rot);
}

//========================================================================
