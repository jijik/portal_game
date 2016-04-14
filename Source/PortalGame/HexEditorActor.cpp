// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorActor.h"
#include "HexTileComponent.h"
#include "ExpandArrowComponent.h"
#include "Engine/StaticMeshActor.h"


// Sets default values
AHexEditorActor::AHexEditorActor()
	:m_RootTileCoordinates(0,0)
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
	hexComp->SetCoordinates(m_RootTileCoordinates);
	hexComp->SetEditorActor(*this);
	GetStaticMeshComponent()->OnClicked.AddDynamic(hexComp, &UHexTileComponent::OnClick);

	m_Grid.InsertElement({ 0, 0 }, this);

	EnableInput(GetWorld()->GetFirstPlayerController());
	InputComponent->BindAction("RMB", IE_Released, this, &AHexEditorActor::DeselectTile);
	InputComponent->BindAction("DEL", IE_Released, this, &AHexEditorActor::DeleteTile);

	for (int i = 0; i < 6; ++i)
	{
		FVector locator(0, 0, 0);
		FRotator rot(0, 60 * -i, 0);
		auto* actor = GetWorld()->SpawnActor(ExpansionArrowActor, &locator, &rot);
		m_Arrows[i] = CastChecked<AStaticMeshActor>(actor);
		m_Arrows[i]->GetStaticMeshComponent()->SetVisibility(false);
		m_Arrows[i]->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
		auto* arrowComp = m_Arrows[i]->FindComponentByClass<UExpandArrowComponent>();
		check(arrowComp);
		arrowComp->SetRelativeDirection(T_HexGrid::NeighborIndexes[i]);
		arrowComp->SetEditorActor(*this);
		m_Arrows[i]->GetStaticMeshComponent()->OnClicked.AddDynamic(arrowComp, &UExpandArrowComponent::OnClick);
	}
}

// Called every frame
void AHexEditorActor::Tick(float DeltaTime)
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
	for (auto* arrow : m_Arrows)
	{
		arrow->GetStaticMeshComponent()->SetVisibility(false);
	}

	if (m_SelectedHexComponent == nullptr)
	{
		return;
	}

	auto& currentTile = m_SelectedHexComponent->GetCoordinates();
	for (int i = 0; i < 6; ++i)
	{
		auto potentialTile = T_HexGrid::NeighborIndexes[i] + currentTile;

		if (m_Grid.GetElement(potentialTile) == nullptr)
		{
			auto p = m_Grid.GetPosition(currentTile);
			m_Arrows[i]->SetActorLocation(p);
			m_Arrows[i]->GetStaticMeshComponent()->SetVisibility(true);
		}
	}
}

void AHexEditorActor::Expand(const S_HexCoordinates& dir)
{
	check(m_AvailableTiles.Num() != 0);
	check(m_SelectedHexComponent);

	S_HexCoordinates expandedCoordinated = m_SelectedHexComponent->GetCoordinates() + dir;

	check(m_Grid.GetElement(expandedCoordinated) == nullptr);
	
	FVector loc = m_Grid.GetPosition(expandedCoordinated);
	AStaticMeshActor* tile = GetWorld()->SpawnActor<AStaticMeshActor>(loc, FRotator(0, 0, 0));
	check(tile);
	tile->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
	tile->GetStaticMeshComponent()->SetStaticMesh(m_AvailableTiles[0]);
	
	UHexTileComponent* hexComp = NewObject<UHexTileComponent>(tile);
	hexComp->RegisterComponent();
	hexComp->SetCoordinates(expandedCoordinated);
	hexComp->SetEditorActor(*this);

	tile->GetStaticMeshComponent()->OnClicked.AddDynamic(hexComp, &UHexTileComponent::OnClick);

	m_Grid.InsertElement(expandedCoordinated, tile);

	DeselectTile();
	SelectTile(hexComp);
}

void AHexEditorActor::DeleteTile()
{
	if (m_SelectedHexComponent == nullptr)
	{
		return;
	}

	auto* owner = m_SelectedHexComponent->GetOwner();
	auto coords = m_SelectedHexComponent->GetCoordinates();

	if (coords == m_RootTileCoordinates)
	{
		FMessageDialog::Debugf(FText::FromString("Cannot delete the root tile!"));
		return;
	}

	DeselectTile();
	GetWorld()->DestroyActor(owner);
	m_Grid.RemoveElement(coords);
}