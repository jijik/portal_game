// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexGame.h"
#include "HexEditorActor.h"
#include "CompanionActor.h"
#include "Dude.h"
#include "TurretActor.h"

//========================================================================
ATurretActor::ATurretActor()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Obj(TEXT("StaticMesh'/Game/Models/Turret.Turret'"));

	auto* meshComp = GetStaticMeshComponent();
	meshComp->SetMobility(EComponentMobility::Movable);
	meshComp->SetStaticMesh(Obj.Object);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PrimaryActorTick.bCanEverTick = true;
}

//========================================================================
void ATurretActor::Tick(float DeltaSeconds)
{
	auto& grid			= gHexEditor->GetHexGrid();
	auto coords			= grid.GetCoordinates(GetActorLocation());
	auto myPos			= grid.GetPosition(coords);
	auto dudePos		= gHexGame->Dude->GetActorLocation();
	auto dudeCoords = gHexEditor->GetHexGrid().GetCoordinates(dudePos);

	bool drawDudeRay = false;

	FVector z(0, 0, 20);
	FHitResult hitRes;
	bool distracted = false;

	for (auto& c : grid.HorizontalNextNeighborIndexes)
	{
		auto tileC = coords + c;
		if (grid.GetElement(tileC))
		{
			auto pos = grid.GetPosition(tileC);
			auto blocked = GetWorld()->LineTraceSingle(hitRes, myPos+z, pos+z, FCollisionQueryParams(), FCollisionObjectQueryParams());
			if (!blocked)
			{
				DrawDebugCylinder(GetWorld(), pos, pos + z, 50, 6, FColor::Yellow);

				for (auto* companion : gHexEditor->m_AllCompanions)
				{
					auto companionPos = companion->GetActorLocation();
					auto companionCoords = gHexEditor->GetHexGrid().GetCoordinates(companionPos);
					if (companionCoords == tileC)
					{
						distracted = true;
						DrawDebugLine(GetWorld(), myPos + z, companionPos + z, FColor::Yellow, false, -1, 0, 2);
					}
				}

				if (dudeCoords == tileC)
				{
					drawDudeRay = true;
				}
			}
		}
	}

	if (drawDudeRay && !distracted)
	{
		DrawDebugLine(GetWorld(), myPos + z, dudePos + z, FColor::Yellow, false, -1, 0, 2);
	}

	for (auto& c : grid.HorizontalNeighborIndexes)
	{
		auto tileC = coords + c;
		if (grid.GetElement(tileC))
		{
			auto pos = grid.GetPosition(tileC);
			auto blocked = GetWorld()->LineTraceSingle(hitRes, myPos+z, pos+z, FCollisionQueryParams(), FCollisionObjectQueryParams());
			if (!blocked)
			{
				DrawDebugCylinder(GetWorld(), pos, pos + z, 50, 6, FColor::Red);

				if (dudeCoords == tileC && !distracted)
				{
					DrawDebugLine(GetWorld(), myPos + z, dudePos + z, FColor::Red, false, -1, 0, 5);
				}
			}
		}
	}
}

//========================================================================

void ATurretActor::Save(std::ofstream& file)
{
	binary_write(file, GetActorLocation());
}

//========================================================================
void ATurretActor::Load(std::ifstream& file)
{
	FVector pos;
	binary_read(file, pos);
	SetActorLocation(pos);
}