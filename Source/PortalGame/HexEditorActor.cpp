// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "HexEditorActor.h"
#include "CompanionActor.h"
#include "BlockerActor.h"
#include "BarrierActor.h"
#include "BridgeActor.h"
#include "TurretActor.h"
#include "PlatformActor.h"
#include "TeleportActor.h"
#include "Dude.h"
#include "HexGame.h"
#include "ExpandArrowComponent.h"
#include "Engine/StaticMeshActor.h"

#include <ostream>
#include <fstream>
#include <sstream>

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
	
	SwitchBindings(InputMode::Expanding);

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
void AHexEditorActor::SwitchBindings(InputMode to)
{
	UnregisterAllBindings();
	switch (to)
	{
	case AHexEditorActor::None:				RegisterNoneBinding();			break;
	case AHexEditorActor::Expanding:	RegisterExpandingBinding();	break;
	case AHexEditorActor::Barriers:		RegisterBarriersBinding();	break;
	case AHexEditorActor::Platforms:	RegisterPlatformsBinding();	break;
	case AHexEditorActor::Companions:	RegisterCompanionsBinding();break;
	case AHexEditorActor::Blockers:		RegisterBlockersBinding();	break;
	case AHexEditorActor::Bridges:		RegisterBridgesBinding();		break;
	case AHexEditorActor::Turrets:		RegisterTurretsBinding();		break;
	case AHexEditorActor::Teleports:	RegisterTeleportsBinding();	break;
	case AHexEditorActor::Game:				RegisterGameBinding();			break;
	default:	check(false);	break;
	}
}

//========================================================================
void AHexEditorActor::RegisterNoneBinding()
{
	InputComponent->BindAction("InputMode", IE_Pressed, this, &AHexEditorActor::CycleInputMode);
}

//========================================================================
void AHexEditorActor::RegisterExpandingBinding()
{
	InputComponent->BindAction("Deselect", IE_Released, this, &AHexEditorActor::Deselect);
	InputComponent->BindAction("DEL", IE_Released, this, &AHexEditorActor::DeleteTile);
	InputComponent->BindAction("CycleModel", IE_Pressed, this, &AHexEditorActor::CycleModel);
	InputComponent->BindAction("RotateModel", IE_Pressed, this, &AHexEditorActor::RotateModel);
	InputComponent->BindAction("ExpandUp", IE_Pressed, this, &AHexEditorActor::ExpandUp);
	InputComponent->BindAction("InputMode", IE_Pressed, this, &AHexEditorActor::CycleInputMode);
}

//========================================================================
void AHexEditorActor::RegisterBarriersBinding()
{
	InputComponent->BindAction("Deselect", IE_Released, this, &AHexEditorActor::Deselect);
	InputComponent->BindAction("CycleModel", IE_Pressed, this, &AHexEditorActor::CycleModel);
	InputComponent->BindAction("InputMode", IE_Pressed, this, &AHexEditorActor::CycleInputMode);
}

//========================================================================
void AHexEditorActor::RegisterPlatformsBinding()
{
	InputComponent->BindAction("Deselect", IE_Released, this, &AHexEditorActor::Deselect);
	InputComponent->BindAction("CycleModel", IE_Pressed, this, &AHexEditorActor::CycleModel);
	InputComponent->BindAction("InputMode", IE_Pressed, this, &AHexEditorActor::CycleInputMode);
}

//========================================================================
void AHexEditorActor::RegisterCompanionsBinding()
{
	InputComponent->BindAction("DEL", IE_Released, this, &AHexEditorActor::DeleteAllCompanions);
	InputComponent->BindAction("InputMode", IE_Pressed, this, &AHexEditorActor::CycleInputMode);
}

//========================================================================
void AHexEditorActor::RegisterBlockersBinding()
{
	InputComponent->BindAction("DEL", IE_Released, this, &AHexEditorActor::DeleteAllBlockers);
	InputComponent->BindAction("InputMode", IE_Pressed, this, &AHexEditorActor::CycleInputMode);
}

//========================================================================
void AHexEditorActor::RegisterBridgesBinding()
{
	InputComponent->BindAction("DEL", IE_Released, this, &AHexEditorActor::DeleteAllBridges);
	InputComponent->BindAction("InputMode", IE_Pressed, this, &AHexEditorActor::CycleInputMode);
}

//========================================================================
void AHexEditorActor::RegisterTurretsBinding()
{
	InputComponent->BindAction("DEL", IE_Released, this, &AHexEditorActor::DeleteAllTurrets);
	InputComponent->BindAction("InputMode", IE_Pressed, this, &AHexEditorActor::CycleInputMode);
}

//========================================================================
void AHexEditorActor::RegisterTeleportsBinding()
{
	InputComponent->BindAction("DEL", IE_Released, this, &AHexEditorActor::DeleteAllTeleports);
	InputComponent->BindAction("InputMode", IE_Pressed, this, &AHexEditorActor::CycleInputMode);
}

//========================================================================
void AHexEditorActor::RegisterGameBinding()
{
	InputComponent->BindAction("Move", IE_Released, gHexGame->Dude, &ADude::Move);
	InputComponent->BindAction("InputMode", IE_Pressed, this, &AHexEditorActor::CycleInputMode);
	InputComponent->BindAction("Action", IE_Pressed, this, &AHexEditorActor::ActionClick);
	InputComponent->BindAction("CycleModel", IE_Pressed, this, &AHexEditorActor::GameCycleModel);
}

//========================================================================
void AHexEditorActor::UnregisterAllBindings()
{
	while (InputComponent->GetNumActionBindings() > 0)
	{
		InputComponent->RemoveActionBinding(0);
	}
}

//========================================================================
void AHexEditorActor::Tick(float DeltaTime)
{
	print_frame(InputModeStr[m_InputType].c_str(), DeltaTime);

	UpdateBarrierPlacing();
	UpdatePlatformPlacing();

	if (m_AttachingPlatform)
	{
		print_frame("Click on the platform target", DeltaTime);
	}

// 	Raycast<AActor>(this, [&](auto& resultActor, auto& traceResult)
// 		{
// 			print_frame((*resultActor->GetHumanReadableName()), DeltaTime);
// 
// 			auto tileCenter = m_Grid.GetPosition(m_Grid.GetCoordinates(traceResult.Location));
// 			DrawDebugCircle(GetWorld(), tileCenter, 50, 32, FColor::Red, false, -1.f, 0, 3);
// 		}); 

// 	for (auto& c : m_Grid.GetStorage())
// 	{
// 		auto pos = c.second->GetActorLocation();
// 		auto newCoord = m_Grid.GetCoordinates(pos);
// 
// 		std::stringstream ss;
// 		ss << c.first.s << "," << c.first.t << "\n";
// 		ss << newCoord.s << "," << newCoord.t << "\n";
// 		DrawDebugString(GetWorld(), pos, ss.str().c_str());
// 	}
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
	else if (m_InputType == InputMode::Platforms)
	{
		PlacePlatform();
	}
	else if (m_InputType == InputMode::Companions)
	{
		PlaceCompanion();
	}
	else if (m_InputType == InputMode::Blockers)
	{
		PlaceBlocker(hexTile);
	}
	else if (m_InputType == InputMode::Bridges)
	{
		PlaceBridge(hexTile);
	}
	else if (m_InputType == InputMode::Turrets)
	{
		PlaceTurret(hexTile);
	}
	else if (m_InputType == InputMode::Teleports)
	{
		PlaceTeleport(hexTile);
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
	SelectPlatform(nullptr);
}

//========================================================================
void AHexEditorActor::DeselectTile()
{
	SelectTile(nullptr);

	m_AttachingPlatform = false;
}

//========================================================================
void AHexEditorActor::Deselect()
{
	DeselectTile();
	SelectBarrier(nullptr);
	SelectPlatform(nullptr);
	
	if (m_InputType != InputMode::Expanding)
	{
		ChangeInputMode(InputMode::None);
	}
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
	DeleteBarrier();
	DeletePlatform();

	if (m_SelectedHexTile == nullptr)
	{
		return;
	}

	DeleteTileImpl(*m_SelectedHexTile, true);
}

//========================================================================
void AHexEditorActor::DeleteTileImpl(AHexTileActor& hexTile, bool deselectOld)
{
	auto coords = hexTile.GetCoordinates();

	if (coords == m_RootTileCoordinates)
	{
		FMessageDialog::Debugf(FText::FromString("Cannot delete the root tile!"));
		return;
	}

	UnlinkAllFromTile(hexTile);

	if (deselectOld)
	{
		DeselectTile();
	}

	GetWorld()->DestroyActor(&hexTile);
	m_Grid.RemoveElement(coords);
}

//========================================================================
void AHexEditorActor::UnlinkAllFromTile(AHexTileActor& hexTile)
{
	for (HexDir i = 0; i < 6; ++i)
	{
		auto* barrier = hexTile.GetBarrierAt(i);
		if (barrier)
		{
			auto orphan = barrier->UnlinkTileFomBarrier(hexTile);
			if (orphan)
			{
				GetWorld()->DestroyActor(barrier);
				m_AllBarriers.erase(barrier);
			}
			hexTile.RemoveBarrierAt(i);
		}
	}

	auto* platform = hexTile.GetPlatform();
	if (platform)
	{
		hexTile.RemovePlatform();
		GetWorld()->DestroyActor(platform);
		m_AllPlatforms.erase(platform);
	}
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

	m_AllBarriers.erase(m_SelectedBarrier);

	for (auto* platform : m_AllPlatforms)
	{
		if (platform->GetTarget() == m_SelectedBarrier)
		{
			platform->SetTarget(nullptr);
		}
	}

	m_SelectedBarrier = nullptr;
}

//========================================================================
void AHexEditorActor::CreatePlatformForPlacing()
{
	check(m_CurrentPlatform == nullptr);
	m_CurrentPlatform = GetWorld()->SpawnActor<APlatformActor>();
	m_CurrentPlatform->Init();
}

//========================================================================
void AHexEditorActor::UpdatePlatformPlacing()
{
	if (m_CurrentPlatform)
	{
		check(m_InputType == InputMode::Platforms);
		
		Raycast<AHexTileActor>(this,
			[&](auto& resultActor, auto& traceResult) 
			{
				auto& coords = resultActor->GetCoordinates();
				auto tilePos = m_Grid.GetPosition(coords);
				
				m_CurrentPlatform->SetActorLocation(tilePos);
				m_CurrentPlatform->SetOwningTileBeforePlace(resultActor);
			},
			[&]()
			{
				m_CurrentPlatform->SetOwningTileBeforePlace(nullptr);
			});
	}
}

//========================================================================
void AHexEditorActor::PlacePlatform(bool inGame)
{
	if(m_AttachingPlatform || !m_CurrentPlatform->IsReadyToPlace())
	{
		return;
	}

	auto* owner = m_CurrentPlatform->GetOwningTileBeforePlace();
	check(owner);

	if (owner->HasPlatform())
	{
		return;
	}

	owner->PlacePlatform(*m_CurrentPlatform);
	m_CurrentPlatform->Place(*owner);

	m_AllPlatforms.insert(m_CurrentPlatform);

	auto* placed = m_CurrentPlatform;

	m_CurrentPlatform = nullptr; //leave it to live it's life

	if (inGame)
	{
		m_AttachingPlatform = true;
		SelectPlatform(placed);
	}

	print("Platform placed...");
}

//========================================================================
void AHexEditorActor::DeletePlatform()
{
	if (m_SelectedPlatform == nullptr)
	{
		return;
	}

	check(m_SelectedHexTile == nullptr);

	m_SelectedPlatform->UnlinkPlatformFromOwningTile();

	GetWorld()->DestroyActor(m_SelectedPlatform);
	
	m_AllPlatforms.erase(m_SelectedPlatform);

	m_SelectedPlatform = nullptr;
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
	else if (m_InputType == InputMode::Platforms)
	{
		check(m_CurrentPlatform);
		m_CurrentPlatform->CycleModel();
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
void AHexEditorActor::CycleInputMode()
{
	unsigned tmp = (unsigned)m_InputType;
	tmp += 1;
	tmp %= InputMode::TOTAL;
	ChangeInputMode((InputMode)tmp);
}

//========================================================================
void AHexEditorActor::ChangeInputMode(InputMode to, bool deselect)
{
	if (m_InputType == InputMode::Barriers)
	{
		check(m_CurrentBarrier);
		GetWorld()->DestroyActor(m_CurrentBarrier);
		m_CurrentBarrier = nullptr;
	}
	else if (m_InputType == InputMode::Platforms)
	{
		if (m_CurrentPlatform)
		{
			GetWorld()->DestroyActor(m_CurrentPlatform);
			m_CurrentPlatform = nullptr;
		}
	}
	else if (m_InputType == InputMode::Teleports)
	{
		if (m_CurrentTeleport)
		{
			GetWorld()->DestroyActor(m_CurrentTeleport);
			m_CurrentTeleport = nullptr;
		}
	}

	if (to == InputMode::Barriers)
	{
		CreateBarrierForPlacing();
	}
	else if (to == InputMode::Platforms)
	{
		if (!m_AttachingPlatform)
		{
			CreatePlatformForPlacing();
		}
	}

	m_InputType = to;

	if (deselect)
	{
		DeselectTile();
	}

	SwitchBindings(to);
}

//========================================================================
void AHexEditorActor::ActionClick()
{
	if (m_CurrentBlocker)
	{
		FinishBlockerPlacing();
	}

	if (m_CurrentBridge)
	{
		FinishBridgePlacing();
	}

	gHexGame->Dude->Drop();

	Raycast<AActor>(this, 
		[&](auto& resultActor, auto& traceResult)
		{
			auto* companion = Cast<ACompanionActor>(resultActor);
			if (companion)
			{
				companion->OnClick();
			}
		});
}

//========================================================================
void AHexEditorActor::CreateBarrierForPlacing()
{
	check(m_CurrentBarrier == nullptr);
	m_CurrentBarrier = GetWorld()->SpawnActor<ABarrierActor>();
	m_CurrentBarrier->Init();
}

//========================================================================
void AHexEditorActor::UpdateBarrierPlacing()
{
	if (m_CurrentBarrier)
	{
		check(m_InputType == InputMode::Barriers);
		
		Raycast<AHexTileActor>(this,
			[&](auto& resultActor, auto& traceResult) 
			{
				auto& coords = resultActor->GetCoordinates();
				auto tilePos = m_Grid.GetPosition(coords);
				auto toHit = traceResult.ImpactPoint - tilePos;
				auto nId = GetNeighborId(toHit);
				auto neighborRelativeCoordinates = T_HexGrid::HorizontalNeighborIndexes[nId];

				auto pos = m_Grid.GetPositionBetweenTiles(coords, coords + neighborRelativeCoordinates);
				m_CurrentBarrier->SetActorLocation(pos);
				m_CurrentBarrier->SetActorRotation(FRotator(0, 60 * -(int)nId, 0));

				m_CurrentBarrier->SetOwningTileBeforePlace(resultActor, nId);
			},
			[&]()
			{
				m_CurrentBarrier->SetOwningTileBeforePlace(nullptr);
			});
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
void AHexEditorActor::PlaceBarrier(bool createAnother)
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

	m_AllBarriers.insert(m_CurrentBarrier);

	m_CurrentBarrier = nullptr; //leave it to live it's life

	if (createAnother)
	{
		CreateBarrierForPlacing();
	}

	print("Barrier placed...");
}

//========================================================================
void AHexEditorActor::PlaceCompanion()
{
	Raycast<AHexTileActor>(this,
		[&](auto& resultActor, auto& traceResult)
	{
		auto* companion = GetWorld()->SpawnActor<ACompanionActor>();
		m_AllCompanions.insert(companion);
		companion->Init(traceResult.Location);
	});
}

//========================================================================
void AHexEditorActor::DeleteAllCompanions()
{
	for (auto* c : m_AllCompanions)
	{
		GetWorld()->DestroyActor(c);
	}
	m_AllCompanions.clear();
}

//========================================================================
void AHexEditorActor::PlaceBlocker(AHexTileActor& hexTile)
{
	Raycast<AHexTileActor>(this,
		[&](auto& resultActor, auto& traceResult)
	{
		auto* blocker = GetWorld()->SpawnActor<ABlockerActor>();
		m_AllBlockers.insert(blocker);
		blocker->Init(traceResult.Location);
	});
}

//========================================================================
void AHexEditorActor::DeleteAllBlockers()
{
	for (auto* c : m_AllBlockers)
	{
		GetWorld()->DestroyActor(c);
	}
	m_AllBlockers.clear();
}

//========================================================================
void AHexEditorActor::FinishBlockerPlacing()
{
	m_CurrentBlocker->m_Placing = false;
	m_CurrentBlocker = nullptr;
}

//========================================================================
void AHexEditorActor::PlaceBridge(AHexTileActor& hexTile)
{
	Raycast<AHexTileActor>(this,
		[&](auto& resultActor, auto& traceResult)
	{
		auto* bridge = GetWorld()->SpawnActor<ABridgeActor>();
		m_AllBridges.insert(bridge);
		bridge->Init(traceResult.Location);
	});
}

//========================================================================
void AHexEditorActor::DeleteAllBridges()
{
	for (auto* c : m_AllBridges)
	{
		GetWorld()->DestroyActor(c);
	}
	m_AllBridges.clear();
}

//========================================================================
void AHexEditorActor::FinishBridgePlacing()
{
	m_CurrentBridge->m_Placing = false;
	m_CurrentBridge = nullptr;
}

//========================================================================
void AHexEditorActor::PlaceTurret(AHexTileActor& hexTile)
{
	Raycast<AHexTileActor>(this,
		[&](auto& resultActor, auto& traceResult)
	{
		auto* turret = GetWorld()->SpawnActor<ATurretActor>();
		m_AllTurrets.insert(turret);
		turret->SetActorLocation(m_Grid.GetPosition(hexTile.GetCoordinates()));
	});
}

//========================================================================
void AHexEditorActor::DeleteAllTurrets()
{
	for (auto* c : m_AllTurrets)
	{
		GetWorld()->DestroyActor(c);
	}
	m_AllTurrets.clear();
}

//========================================================================
void AHexEditorActor::PlaceTeleport(AHexTileActor& hexTile)
{
	auto createTeleport = [&]() 
	{
		auto* teleport = GetWorld()->SpawnActor<ATeleportActor>();
		m_AllTeleports.insert(teleport);
		teleport->SetActorLocation(m_Grid.GetPosition(hexTile.GetCoordinates())); 
		return teleport;
	};

	if (!m_CurrentTeleport)
	{
		m_CurrentTeleport = createTeleport();
	}
	else
	{
		auto* teleport = createTeleport();
		teleport->Link(*m_CurrentTeleport);
		m_CurrentTeleport->Link(*teleport);
		m_CurrentTeleport = nullptr;
	}
}

//========================================================================
void AHexEditorActor::DeleteAllTeleports()
{
	for (auto* c : m_AllTeleports)
	{
		GetWorld()->DestroyActor(c);
	}
	m_AllTeleports.clear();
}

//========================================================================
void AHexEditorActor::GameCycleModel()
{
	if (m_CurrentBridge)
	{
		m_CurrentBridge->Cycle();
	}
}

//========================================================================
void AHexEditorActor::SelectBarrier(class ABarrierActor* ba)
{
	if (m_InputType == InputMode::Platforms && m_AttachingPlatform)
	{
		check(m_SelectedPlatform && ba);
		m_SelectedPlatform->SetTarget(ba);
		DeselectTile();
		m_AttachingPlatform = false;
		CreatePlatformForPlacing();
	}

	if (m_InputType != InputMode::Expanding)
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
void AHexEditorActor::SelectPlatform(class APlatformActor* pa)
{
	if (!(m_InputType == InputMode::Expanding ||
				m_InputType == InputMode::Platforms && m_AttachingPlatform))
	{
		return;
	}

	if (pa && !pa->GetTarget())
	{
		m_AttachingPlatform = true;
		ChangeInputMode(InputMode::Platforms, false);
	}

	if (pa)
	{
		DeselectTile();
	}

	check(m_CurrentPlatform == nullptr);

	if (m_SelectedPlatform)
	{
		m_SelectedPlatform->SetSelectedMaterial(false);
	}

	m_SelectedPlatform = pa;

	if (m_SelectedPlatform)
	{
		m_SelectedPlatform->SetSelectedMaterial(true);
	}

	if (m_SelectedPlatform && !m_SelectedPlatform->GetTarget())
	{
		m_AttachingPlatform = true;
	}
}

//========================================================================
void AHexEditorActor::ClearAll()
{
	std::vector<AHexTileActor*> toDelete;

	for (auto& pair: m_Grid.GetStorage())
	{
		if (!pair.first.IsZero()) //delete all but main tile
		{
			toDelete.push_back(pair.second);
		}
	}

	while (!toDelete.empty())
	{
		SelectTile(toDelete.back());
		DeleteTile();
		toDelete.pop_back();
	}

	DeleteAllCompanions();
	DeleteAllBlockers();
	DeleteAllTurrets();

	auto* element = m_Grid.GetElement({ 0,0,0 });
	check(element);
	UnlinkAllFromTile(*element);
}

//========================================================================
void AHexEditorActor::SaveMap(const FString& name)
{
	std::ofstream file;
	file.open(*name, std::ofstream::binary);
	
	auto& gridStorage = m_Grid.GetStorage();

	binary_write(file, (unsigned)gridStorage.size());
	for (auto& pair : gridStorage)
	{
		pair.second->Save(file);
	}

	binary_write(file, (unsigned)m_AllBarriers.size());
	for (auto* barrier : m_AllBarriers)
	{
		barrier->Save(file);
	}

	binary_write(file, (unsigned)m_AllPlatforms.size());
	for (auto* platform : m_AllPlatforms)
	{
		platform->Save(file);
	}

	binary_write(file, (unsigned)m_AllCompanions.size());
	for (auto* companion : m_AllCompanions)
	{
		companion->Save(file);
	}

	binary_write(file, (unsigned)m_AllBlockers.size());
	for (auto* blocker : m_AllBlockers)
	{
		blocker->Save(file);
	}

	binary_write(file, (unsigned)m_AllBridges.size());
	for (auto* bridges : m_AllBridges)
	{
		bridges->Save(file);
	}

	binary_write(file, (unsigned)m_AllTurrets.size());
	for (auto* turret : m_AllTurrets)
	{
		turret->Save(file);
	}

	file.close();
}

//========================================================================
void AHexEditorActor::LoadMap(const FString& name)
{
	ClearAll();

	std::ifstream file(*name, std::ifstream::binary);
	if (!file.good())
	{
		print(TEXT("no such map file!"));
		return;
	}

	unsigned count = 0;
	binary_read(file, count); //tiles
	for (unsigned i = 0; i < count; ++i)
	{
		auto* tile = GetWorld()->SpawnActor<AHexTileActor>();
		tile->Init({ 0,0,0 }); //tmp coords;
		tile->Load(file);
		m_Grid.InsertElement(tile->GetCoordinates(), tile);
	}

	binary_read(file, count); //barriers
	for (unsigned i = 0; i < count; ++i)
	{
		CreateBarrierForPlacing();
		m_CurrentBarrier->Load(file);
		PlaceBarrier(false);
	}

	binary_read(file, count); //platforms
	for (unsigned i = 0; i < count; ++i)
	{
		CreatePlatformForPlacing();
		m_CurrentPlatform->Load(file);
		PlacePlatform(false);
	}

	binary_read(file, count); //companions
	for (unsigned i = 0; i < count; ++i)
	{
		auto* companion = GetWorld()->SpawnActor<ACompanionActor>();
		m_AllCompanions.insert(companion);
		companion->Load(file);
	}

	binary_read(file, count); //blocker
	for (unsigned i = 0; i < count; ++i)
	{
		auto* blocker = GetWorld()->SpawnActor<ABlockerActor>();
		m_AllBlockers.insert(blocker);
		blocker->Load(file);
	}

	binary_read(file, count); //bridges
	for (unsigned i = 0; i < count; ++i)
	{
		auto* bridge = GetWorld()->SpawnActor<ABridgeActor>();
		m_AllBridges.insert(bridge);
		bridge->Load(file);
	}

	binary_read(file, count); //turret
	for (unsigned i = 0; i < count; ++i)
	{
		auto* turret = GetWorld()->SpawnActor<ATurretActor>();
		m_AllTurrets.insert(turret);
		turret->Load(file);
	}

	file.close();

	UNavigationSystem* NavSys = UNavigationSystem::GetCurrent(GetWorld());
	if (NavSys)
	{
		NavSys->Build();
	}
}

//========================================================================

//========================================================================
