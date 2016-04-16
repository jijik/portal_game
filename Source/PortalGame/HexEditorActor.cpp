// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorActor.h"
#include "HexTileComponent.h"
#include "ExpandArrowComponent.h"
#include "Engine/StaticMeshActor.h"

//========================================================================
AHexEditorActor::AHexEditorActor()
	:m_RootTileCoordinates(0,0,0)
{
	gHexEditor = this;

	m_Grid.SetTileRadius(100.0);
	m_SelectedHexComponent = nullptr;
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterial> DefaultMat(TEXT("Material'/Game/Materials/Default.Default'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> Selected(TEXT("Material'/Game/Materials/Default2.Default2'"));

	m_DefaultMaterial = DefaultMat.Object;
	m_SelectedMaterial = Selected.Object;
}

//========================================================================
void AHexEditorActor::BeginPlay()
{
	Super::BeginPlay();

	AHexTileActor::Init(m_RootTileCoordinates);

	m_Grid.InsertElement({ 0, 0 }, this);

	EnableInput(GetWorld()->GetFirstPlayerController());
	InputComponent->BindAction("RMB", IE_Released, this, &AHexEditorActor::DeselectTile);
	InputComponent->BindAction("DEL", IE_Released, this, &AHexEditorActor::DeleteTile);
	InputComponent->BindAction("CycleModel", IE_Pressed, this, &AHexEditorActor::CycleModel);
	InputComponent->BindAction("RotateModel", IE_Pressed, this, &AHexEditorActor::RotateModel);
	InputComponent->BindAction("ExpandUp", IE_Pressed, this, &AHexEditorActor::ExpandUp);

	FVector locator(0, 0, 0);
	m_ArrowsParent = GetWorld()->SpawnActor(AActor::StaticClass());

	m_ArrowsParent->SetRootComponent(NewObject<USceneComponent>(m_ArrowsParent,TEXT("ArrowsParent")));

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
		arrowComp->SetRelativeDirection(T_HexGrid::HorizontalNeighborIndexes[i]);
		m_Arrows[i]->GetStaticMeshComponent()->OnClicked.AddDynamic(arrowComp, &UExpandArrowComponent::OnClick);
		m_Arrows[i]->AttachRootComponentToActor(m_ArrowsParent);
	}
}

//========================================================================
void AHexEditorActor::SelectTile(UHexTileComponent* hexTile)
{
	HandleSelectionMaterial(hexTile);

	m_SelectedHexComponent = hexTile;
	ShowExpansionArrows();
}

//========================================================================
void AHexEditorActor::DeselectTile()
{
	SelectTile(nullptr);
}

//========================================================================
void AHexEditorActor::HandleSelectionMaterial(UHexTileComponent* hexTile)
{
	if (m_SelectedHexComponent)
	{
		CastChecked<AHexTileActor>(m_SelectedHexComponent->GetOwner())->SetSelectedMaterial(false);
	}
	if (hexTile)
	{
		CastChecked<AHexTileActor>(hexTile->GetOwner())->SetSelectedMaterial(true);
	}
}

//========================================================================
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
		auto potentialTile = T_HexGrid::HorizontalNeighborIndexes[i] + currentTile;

		if (m_Grid.GetElement(potentialTile) == nullptr)
		{
			auto p = m_Grid.GetPosition(currentTile);
			m_Arrows[i]->SetActorLocation(p);
			m_Arrows[i]->GetStaticMeshComponent()->SetVisibility(true);
		}
	}
}

//========================================================================
void AHexEditorActor::Expand(const S_HexCoordinates& dir)
{
	check(m_SelectedHexComponent);

	S_HexCoordinates expandedCoordinated = m_SelectedHexComponent->GetCoordinates() + dir;

	check(m_Grid.GetElement(expandedCoordinated) == nullptr);
	
	auto* tile = GetWorld()->SpawnActor<AHexTileActor>();
	tile->Init(expandedCoordinated);

	m_Grid.InsertElement(expandedCoordinated, tile);

	DeselectTile();
	SelectTile(tile->FindComponentByClass<UHexTileComponent>());
}

//========================================================================
void AHexEditorActor::ExpandUp()
{
	if (m_SelectedHexComponent == nullptr)
	{
		return;
	}

	S_HexCoordinates up(0, 0, 1);
	S_HexCoordinates expandedCoordinated = m_SelectedHexComponent->GetCoordinates() + up;

	if (m_Grid.GetElement(expandedCoordinated) != nullptr)
	{
		return; //full
	}

	Expand(up);
}

//========================================================================
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

//========================================================================
AHexEditorActor::T_HexGrid& AHexEditorActor::GetHexGrid()
{
	return m_Grid;
}

//========================================================================
void AHexEditorActor::CycleModel()
{
	if (m_SelectedHexComponent)
	{
		CastChecked<AHexTileActor>(m_SelectedHexComponent->GetOwner())->CycleModel();
	}
}

//========================================================================
void AHexEditorActor::RotateModel()
{
	if (m_SelectedHexComponent)
	{
		CastChecked<AHexTileActor>(m_SelectedHexComponent->GetOwner())->RotateModel();
	}
}