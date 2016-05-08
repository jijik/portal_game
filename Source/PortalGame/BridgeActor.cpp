// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorActor.h"
#include "BridgeActor.h"

//========================================================================
ABridgeActor::ABridgeActor()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Obj(TEXT("StaticMesh'/Game/Models/Bridge.Bridge'"));
	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetStaticMesh(Obj.Object);
}

//========================================================================
void ABridgeActor::OnDrop()
{
	ACompanionActor::OnDrop();

	auto& grid = gHexEditor->GetHexGrid();
	auto coords = grid.GetCoordinates(GetActorLocation());
	auto tile = grid.GetElement(coords);
	check(tile);
	m_BaseTile = tile;

	m_Placing = true;
	auto tilePos = m_BaseTile->GetActorLocation();
	auto myPos = GetActorLocation();
	SetActorLocation(FVector(tilePos.X, tilePos.Y, myPos.Z));
	gHexEditor->m_CurrentBridge = this;

	CreateBridge();
}

//========================================================================
void ABridgeActor::OnPick()
{
	ACompanionActor::OnPick();

	DestroyBridge();
}

//========================================================================
void ABridgeActor::CreateBridge()
{
	auto coords = m_BaseTile->GetCoordinates();

	auto upCoords = coords + S_HexCoordinates(0, 0, 1);

	auto* tile = GetWorld()->SpawnActor<AHexTileActor>();
	tile->Init(upCoords);

	gHexEditor->GetHexGrid().InsertElement(upCoords, tile);
	m_CreatedTile = tile;
}

//========================================================================
void ABridgeActor::DestroyBridge()
{
	if (m_CreatedTile)
	{
		gHexEditor->DeleteTileImpl(*m_CreatedTile, false);
		m_CreatedTile = nullptr;
	}
}

//========================================================================
void ABridgeActor::Cycle()
{
	if (m_CreatedTile)
	{
		DestroyBridge();
	}
	else
	{
		CreateBridge();
	}
}



