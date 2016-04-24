// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorActor.h"
#include "BarrierActor.h"
#include "ExpandArrowComponent.h"
#include "Engine/StaticMeshActor.h"

//========================================================================
AHexEditorActor::AHexEditorActor()
	:m_RootTileCoordinates(0,0,0)
{
	gHexEditor = this;

	m_Grid.SetTileRadius(100.0);
	m_SelectedHexTile = nullptr;
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
	InputComponent->BindAction("Deselect", IE_Released, this, &AHexEditorActor::Deselect);
	InputComponent->BindAction("DEL", IE_Released, this, &AHexEditorActor::DeleteTile);
	InputComponent->BindAction("CycleModel", IE_Pressed, this, &AHexEditorActor::CycleModel);
	InputComponent->BindAction("RotateModel", IE_Pressed, this, &AHexEditorActor::RotateModel);
	InputComponent->BindAction("ExpandUp", IE_Pressed, this, &AHexEditorActor::ExpandUp);
	InputComponent->BindAction("InputMode", IE_Pressed, this, &AHexEditorActor::ChangeInputMode);

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
		m_Arrows[i]->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		m_Arrows[i]->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
		auto* arrowComp = m_Arrows[i]->FindComponentByClass<UExpandArrowComponent>();
		check(arrowComp);
		arrowComp->SetRelativeDirection(i);
		m_Arrows[i]->GetStaticMeshComponent()->OnClicked.AddDynamic(arrowComp, &UExpandArrowComponent::OnClick);
		m_Arrows[i]->AttachRootComponentToActor(m_ArrowsParent);
	}
}

//========================================================================
void AHexEditorActor::Tick(float DeltaTime)
{
	UpdateBarriers();
}

//========================================================================
void AHexEditorActor::ClickOnTile(AHexTileActor& hexTile)
{
	if (m_InputType == InputMode::Expanding)
	{
		SelectTile(&hexTile);
	}
	else if (m_InputType == InputMode::Barriers)
	{
		PlaceBarrier();
	}
}

//========================================================================
void AHexEditorActor::SelectTile(AHexTileActor* hexTile)
{
	HandleSelectionMaterial(hexTile);

	m_SelectedHexTile = hexTile;
	ShowExpansionArrows();

	if (hexTile)
	{
		SelectBarrier(nullptr);
	}
}

//========================================================================
void AHexEditorActor::DeselectTile()
{
	SelectTile(nullptr);
}

//========================================================================
void AHexEditorActor::Deselect()
{
	if (m_InputType == InputMode::Barriers)
	{
		ChangeInputMode();
	}

	DeselectTile();
	SelectBarrier(nullptr);
}

//========================================================================
void AHexEditorActor::HandleSelectionMaterial(AHexTileActor* hexTile)
{
	if (m_SelectedHexTile)
	{
		m_SelectedHexTile->SetSelectedMaterial(false);
	}

	if (hexTile)
	{
		hexTile->SetSelectedMaterial(true);
	}
}

//========================================================================
void AHexEditorActor::ShowExpansionArrows()
{
	for (auto* arrow : m_Arrows)
	{
		arrow->GetStaticMeshComponent()->SetVisibility(false);
		arrow->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (m_SelectedHexTile == nullptr)
	{
		return;
	}

	auto& currentTile = m_SelectedHexTile->GetCoordinates();
	for (int i = 0; i < 6; ++i)
	{
		auto potentialTile = T_HexGrid::HorizontalNeighborIndexes[i] + currentTile;

		if (m_Grid.GetElement(potentialTile) == nullptr)
		{
			auto p = m_Grid.GetPosition(currentTile);
			m_Arrows[i]->SetActorLocation(p);
			m_Arrows[i]->GetStaticMeshComponent()->SetVisibility(true);
			m_Arrows[i]->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
	}
}

//========================================================================
void AHexEditorActor::Expand(const S_HexCoordinates& dir)
{
	check(m_SelectedHexTile);

	S_HexCoordinates expandedCoordinated = m_SelectedHexTile->GetCoordinates() + dir;

	check(m_Grid.GetElement(expandedCoordinated) == nullptr);
	
	auto* tile = GetWorld()->SpawnActor<AHexTileActor>();
	tile->Init(expandedCoordinated);

	m_Grid.InsertElement(expandedCoordinated, tile);

	if (dir.IsHorizontalDir())
	{
		auto hexDir = dir.ToHexDir();
		auto* barrier = m_SelectedHexTile->GetBarrierAt(hexDir);
		if (barrier)
		{
			auto complementDir = T_HexGrid::GetComplementaryNeighborIndex(hexDir);
			tile->PlaceBarrierAt(*barrier, complementDir);
			barrier->Place(*m_SelectedHexTile, hexDir, tile, complementDir);
		}
	}

	DeselectTile();
	SelectTile(tile);
}

//========================================================================
void AHexEditorActor::ExpandUp()
{
	if (m_SelectedHexTile == nullptr)
	{
		return;
	}

	S_HexCoordinates up(0, 0, 1);
	S_HexCoordinates expandedCoordinated = m_SelectedHexTile->GetCoordinates() + up;

	if (m_Grid.GetElement(expandedCoordinated) != nullptr)
	{
		return; //full
	}

	Expand(up);
}

//========================================================================
void AHexEditorActor::DeleteTile()
{
	check(m_InputType == InputMode::Expanding);

	DeleteBarrier();

	if (m_SelectedHexTile == nullptr)
	{
		return;
	}

	auto coords = m_SelectedHexTile->GetCoordinates();

	if (coords == m_RootTileCoordinates)
	{
		FMessageDialog::Debugf(FText::FromString("Cannot delete the root tile!"));
		return;
	}

	auto* toDestroy = m_SelectedHexTile;

	for (HexDir i = 0; i < 6; ++i)
	{
		auto* barrier = m_SelectedHexTile->GetBarrierAt(i);
		if (barrier)
		{
			auto orphan = barrier->UnlinkTileFomBarrier(*m_SelectedHexTile);
			if (orphan)
			{
				GetWorld()->DestroyActor(barrier);
			}
		}
	}

	DeselectTile();
	GetWorld()->DestroyActor(toDestroy);
	m_Grid.RemoveElement(coords);
}

//========================================================================
void AHexEditorActor::DeleteBarrier()
{
	if (m_SelectedBarrier == nullptr)
	{
		return;
	}

	check(m_SelectedHexTile == nullptr);

	m_SelectedBarrier->UnlinkBarrierFromNeighborTiles();
	GetWorld()->DestroyActor(m_SelectedBarrier);
	m_SelectedBarrier = nullptr;
}

//========================================================================
AHexEditorActor::T_HexGrid& AHexEditorActor::GetHexGrid()
{
	return m_Grid;
}

//========================================================================
void AHexEditorActor::CycleModel()
{
	if (m_InputType == InputMode::Expanding)
	{
		if (m_SelectedHexTile)
		{
			m_SelectedHexTile->CycleModel();
		}
	}
	else if (m_InputType == InputMode::Barriers)
	{
		check(m_CurrentBarrier);
		m_CurrentBarrier->CycleModel();
	}
}

//========================================================================
void AHexEditorActor::RotateModel()
{
	if (m_InputType == InputMode::Expanding && m_SelectedHexTile)
	{
		m_SelectedHexTile->RotateModel();
	}
}

//========================================================================
void AHexEditorActor::ChangeInputMode()
{
	if (m_InputType == InputMode::Expanding)
	{
		m_InputType = InputMode::Barriers;
		CreateBarrierForPlacing();
	}
	else if (m_InputType == InputMode::Barriers)
	{
		m_InputType = InputMode::Expanding;

		check(m_CurrentBarrier);
		GetWorld()->DestroyActor(m_CurrentBarrier);
		m_CurrentBarrier = nullptr;
	}

	DeselectTile();
}

//========================================================================
void AHexEditorActor::CreateBarrierForPlacing()
{
	check(m_CurrentBarrier == nullptr);
	m_CurrentBarrier = GetWorld()->SpawnActor<ABarrierActor>();
	m_CurrentBarrier->Init();
}

//========================================================================
void AHexEditorActor::UpdateBarriers()
{
	if (m_CurrentBarrier)
	{
		check(m_InputType == InputMode::Barriers);
		
		auto* pc = GetWorld()->GetFirstPlayerController();
		FHitResult TraceResult(ForceInit);
		pc->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_WorldStatic), false, TraceResult);
		auto* actor = TraceResult.GetActor();
		auto* hexActor = Cast<AHexTileActor>(actor);
		if (hexActor)
		{
			auto& coords = hexActor->GetCoordinates();
			auto tilePos = m_Grid.GetPosition(coords);
			auto toHit = TraceResult.ImpactPoint - tilePos;
			auto nId = GetNeighborId(toHit);
			auto neighborRelativeCoordinates = T_HexGrid::HorizontalNeighborIndexes[nId];

			auto pos = m_Grid.GetPositionBetweenTiles(coords, coords + neighborRelativeCoordinates);
			m_CurrentBarrier->SetActorLocation(pos);
			m_CurrentBarrier->SetActorRotation(FRotator(0, 60 * -(int)nId, 0));

			m_CurrentBarrier->SetOwningTileBeforePlace(hexActor, nId);
		}
		else
		{
			m_CurrentBarrier->SetOwningTileBeforePlace(nullptr);
		}
	}
}

//========================================================================
unsigned AHexEditorActor::GetNeighborId(const FVector& fromCenter)
{
	// compensate axis switch etc, nasty piece of code... redo
	auto rad = atan2(fromCenter.Y, fromCenter.X);
	auto deg = FMath::RadiansToDegrees(rad);
	unsigned segment;
	if (deg < 0) deg = 360.0 + deg;
	deg = 360 - deg;
	segment = (unsigned)deg / 60;
	segment += 2;
	segment %= 6;
	return segment;
}

//========================================================================
void AHexEditorActor::PlaceBarrier()
{
	if (!m_CurrentBarrier->IsReadyToPlace())
	{
		return;
	}

	auto* owningTile = m_CurrentBarrier->GetOwningTileBeforePlace();
	auto sectorDir = m_CurrentBarrier->GetOwningSectorBeforePlace();

	check(owningTile);

	if (owningTile->HasBarrierAt(sectorDir))
	{
		return;
	}

	auto neighborCoordinates = owningTile->GetCoordinates() + T_HexGrid::HorizontalNeighborIndexes[sectorDir];
	auto neighborTile = m_Grid.GetElement(neighborCoordinates);

	owningTile->PlaceBarrierAt(*m_CurrentBarrier, sectorDir);

	auto complementarySector = T_HexGrid::GetComplementaryNeighborIndex(sectorDir);
	if (neighborTile)
	{
		check(!neighborTile->HasBarrierAt(complementarySector));
		neighborTile->PlaceBarrierAt(*m_CurrentBarrier, complementarySector);
	}

	m_CurrentBarrier->Place(*owningTile, sectorDir, neighborTile, complementarySector);

	m_CurrentBarrier = nullptr; //leave it to live it's life

	CreateBarrierForPlacing();
}

//========================================================================
void AHexEditorActor::SelectBarrier(class ABarrierActor* ba)
{
	if (m_InputType == InputMode::Barriers)
	{
		return;
	}

	if (ba)
	{
		DeselectTile();
	}

	check(m_CurrentBarrier == nullptr);

	if (m_SelectedBarrier)
	{
		m_SelectedBarrier->SetSelectedMaterial(false);
	}

	m_SelectedBarrier = ba;

	if (m_SelectedBarrier)
	{
		m_SelectedBarrier->SetSelectedMaterial(true);
	}
}

//========================================================================

//========================================================================
