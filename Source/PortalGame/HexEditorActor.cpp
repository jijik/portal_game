// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorActor.h"


// Sets default values
AHexEditorActor::AHexEditorActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHexEditorActor::BeginPlay()
{
	Super::BeginPlay();

	if (ExtensionArrowMesh)
	{
		FActorSpawnParameters params;
		
		for (int i = 0; i < 6; ++i)
		{
			FRotator rot(0, 60*i, 0);
			auto* staticMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(FVector(0.0, 0.0, 0.0), rot, params);
			staticMeshActor->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
			staticMeshActor->GetStaticMeshComponent()->SetStaticMesh(ExtensionArrowMesh);
			staticMeshActor->GetStaticMeshComponent()->OnClicked.AddDynamic(this, &AHexEditorActor::Click);
		}
	}
}

// Called every frame
void AHexEditorActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AHexEditorActor::Click(UPrimitiveComponent* ClickedComp)
{
	auto test = ClickedComp->IsEditorOnly();
}
