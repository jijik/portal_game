// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexGame.h"
#include "PortalUtils.h"
#include "Dude.h"

//========================================================================
AHexGame::AHexGame()
{
	gHexGame = this;

	PrimaryActorTick.bCanEverTick = true;
}

//========================================================================
void AHexGame::BeginPlay()
{
	Super::BeginPlay();
	//EnableInput(GetWorld()->GetFirstPlayerController());
	//InputComponent->BindAction("Move", IE_Released, Dude, &ADude::Move);
}

//========================================================================
void AHexGame::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

