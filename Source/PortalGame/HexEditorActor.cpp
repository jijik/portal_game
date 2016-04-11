// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorActor.h"
#include "Engine/StaticMeshActor.h"


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

	if (ExtensionArrowActor)
	{
		for (int i = 0; i < 6; ++i)
		{
			FVector locator(0.0, 0.0, 0.0);
			FRotator rot(0, 60 * i, 0);
			auto* arrow = GetWorld()->SpawnActor(ExtensionArrowActor, &locator, &rot);
			auto* comp = arrow->FindComponentByClass<UStaticMeshComponent>();
			if (comp)
			{
				comp->OnClicked.AddDynamic(this, &AHexEditorActor::Click);
			}
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
	static int i = 0;
	
	FVector locator(0.0, 0.0, ++i * 3.0);
	FRotator rot(0, 0, 0);
	auto* arrow = GetWorld()->SpawnActor(ExtensionArrowActor, &locator, &rot);
	auto* comp = arrow->FindComponentByClass<UStaticMeshComponent>();
	if (comp)
	{
		comp->OnClicked.AddDynamic(this, &AHexEditorActor::Click);
	}

}
