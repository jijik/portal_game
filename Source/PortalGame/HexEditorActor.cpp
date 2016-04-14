// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorActor.h"
#include "HexTileComponent.h"
#include "Engine/StaticMeshActor.h"


// Sets default values
AHexEditorActor::AHexEditorActor()
{
	m_Grid.SetTileRadius(100.0);
	m_SelectedHexComponent = nullptr;
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHexEditorActor::BeginPlay()
{
	Super::BeginPlay();

	UHexTileComponent* hexComp = NewObject<UHexTileComponent>(this);
	hexComp->RegisterComponent();
	hexComp->SetCoordinates({ 0, 0 });
	hexComp->SetEditorActor(*this);
	auto* mesh = FindComponentByClass<UStaticMeshComponent>();
	check(mesh);
	mesh->OnClicked.AddDynamic(hexComp, &UHexTileComponent::OnClick);

	EnableInput(GetWorld()->GetFirstPlayerController());
	InputComponent->BindAction("RMB", IE_Released, this, &AHexEditorActor::DeselectTile);

	for (int i = 0; i < 6; ++i)
	{
		FVector locator(0, 0, 0);
		FRotator rot(0, 60 * i, 0);
		auto* actor = GetWorld()->SpawnActor(ExtensionArrowActor, &locator, &rot);
		m_Arrows[i] = CastChecked<AStaticMeshActor>(actor);
		m_Arrows[i]->GetStaticMeshComponent()->SetVisibility(false);
		m_Arrows[i]->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	}
}

// Called every frame
void AHexEditorActor::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);

}

void AHexEditorActor::SelectTile(UHexTileComponent* hexTile)
{
	m_SelectedHexComponent = hexTile;
	ShowExpansionArrows();
}

void AHexEditorActor::DeselectTile()
{
	SelectTile(nullptr);
}

void AHexEditorActor::ShowExpansionArrows()
{
	bool show = m_SelectedHexComponent != nullptr;
	for (int i = 0; i < 6; ++i)
	{
		m_Arrows[i]->GetStaticMeshComponent()->SetVisibility(show);
	}

	if (!show)
	{
		return;
	}

	check(ExtensionArrowActor);

	for (int i = 0; i < 6; ++i)
	{
		m_Arrows[i]->SetActorLocation(m_Grid.GetPosition(m_SelectedHexComponent->GetCoordinates()));
	}
}

// void AHexEditorActor::Click(UPrimitiveComponent* ClickedComp)
// {
// 	if (m_AvailableTiles.Num() == 0)
// 	{
// 		return;
// 	}
// 
// 	{
// 		S_HexCoordinates coord(1, 0);
// 		FVector loc = m_Grid.GetPosition(coord);
// 		AStaticMeshActor* tile = GetWorld()->SpawnActor<AStaticMeshActor>(loc, FRotator(0, 0, 0));
// 		check(tile);
// 		tile->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
// 		tile->GetStaticMeshComponent()->SetStaticMesh(m_AvailableTiles[0]);
// 
// 		UHexTileComponent* hexComp = NewObject<UHexTileComponent>(this);
// 		hexComp->RegisterComponent();
// 		hexComp->SetCoordinates(coord);
// 	}
// }
