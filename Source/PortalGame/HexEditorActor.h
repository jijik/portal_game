// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "GameFramework/Actor.h"
#include "HexGrid.h"
#include "PortalAI.h"
#include "HexTileActor.h"
#include "HexEditorActor.generated.h"

class ABarrierActor;
class ACompanionActor;
class ABlockerActor;
class ABridgeActor;
class ATeleportActor;
class ATurretActor;

UCLASS()
class PORTALGAME_API AHexEditorActor : public AHexTileActor
{
	GENERATED_BODY()
	
public:	
	AHexEditorActor();

	virtual void	BeginPlay() override;
	virtual void	Tick(float DeltaTime) override;

	void					ClickOnTile(AHexTileActor& hexTile);
	void					SelectTile(AHexTileActor* hexTile);
	void					DeselectTile();
	void					Deselect();
	void					Expand(const S_HexCoordinates& dir);
	void					SelectBarrier(ABarrierActor* ba);
	void					SelectPlatform(APlatformActor* pa);
	unsigned			GetNeighborId(const FVector& fromCenter);
	void					ClearAll();
	void					SaveMap(const FString& name);
	void					LoadMap(const FString& name);

	UPROPERTY(EditAnywhere)
	UClass*								ExpansionArrowActor;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*>	AvailableTiles;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*>	AvailableBarriers;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*>	AvailablePlatforms;

	AHexTileActor*				m_SelectedHexTile;

	using T_HexGrid =			C_HexGrid<AHexTileActor*>;
	T_HexGrid&						GetHexGrid();

	AActor*	m_ArrowsParent;
	Array6<AStaticMeshActor*>	m_Arrows;

	S_HexCoordinates			m_RootTileCoordinates;
	UMaterial*						m_DefaultMaterial;
	UMaterial*						m_SelectedMaterial;
	float									m_TimeWhenRotateBegin;
	bool									m_SuppressCursor = false;

	ABarrierActor*	m_CurrentBarrier = nullptr; //during placing
	ABarrierActor*	m_SelectedBarrier = nullptr;

	APlatformActor*	m_CurrentPlatform = nullptr; //during placing
	APlatformActor*	m_SelectedPlatform = nullptr;
	bool						m_AttachingPlatform = false;

	ABlockerActor*	m_CurrentBlocker = nullptr; //during placing
	ABridgeActor*		m_CurrentBridge = nullptr; //during placing
	ATeleportActor*	m_CurrentTeleport = nullptr; //during placing

	std::set<ABarrierActor*>		m_AllBarriers;
	std::set<APlatformActor*>		m_AllPlatforms;
	std::set<ACompanionActor*>	m_AllCompanions;
	std::set<ABlockerActor*>		m_AllBlockers;
	std::set<ABridgeActor*>			m_AllBridges;
	std::set<ATurretActor*>			m_AllTurrets;
	std::set<ATeleportActor*>		m_AllTeleports;

	enum InputMode
	{
		None = 0,
		Expanding,
		Barriers,
		Platforms,
		Companions,
		Blockers,
		Bridges,
		Turrets,
		Teleports,
		Game,
		TOTAL
	};
	std::string InputModeStr[TOTAL] = { 
		"None", "Expanding", "Barriers", "Platform",
		"Companions", "Blockers", "Bridges", "Turrets",
		"Teleports", "Game" };

	void					DeleteTile();
	void					DeleteTileImpl(AHexTileActor& hexTile, bool deselectOld);
	void					UnlinkAllFromTile(AHexTileActor& hexTile);

private:
	void					ExpandUp();
	void					HandleSelectionMaterial(AHexTileActor* hexTile);
	void					CycleModel();
	void					RotateModel();
	void					ShowExpansionArrows();
	void					CycleInputMode();
	void					ChangeInputMode(InputMode to, bool deselect = true);
	void					ActionClick();

	void					CreateBarrierForPlacing();
	void					UpdateBarrierPlacing();
	void					PlaceBarrier(bool createAnother = true);
	void					DeleteBarrier();

	void					CreatePlatformForPlacing();
	void					UpdatePlatformPlacing();
	void					PlacePlatform(bool inGame = true);
	void					DeletePlatform();

	void					PlaceCompanion();
	void					DeleteAllCompanions();

	void					PlaceBlocker(AHexTileActor& hexTile);
	void					DeleteAllBlockers();
	void					FinishBlockerPlacing();

	void					PlaceBridge(AHexTileActor& hexTile);
	void					DeleteAllBridges();
	void					FinishBridgePlacing();
	void					GameCycleModel();

	void					PlaceTurret(AHexTileActor& hexTile);
	void					DeleteAllTurrets();

	void					PlaceTeleport(AHexTileActor& hexTile);
	void					DeleteAllTeleports();

	void					RegisterNoneBinding();
	void					RegisterExpandingBinding();
	void					RegisterBarriersBinding();
	void					RegisterPlatformsBinding();
	void					RegisterCompanionsBinding();
	void					RegisterBlockersBinding();
	void					RegisterBridgesBinding();
	void					RegisterTurretsBinding();
	void					RegisterTeleportsBinding();
	void					RegisterGameBinding();
	void					UnregisterAllBindings();
	void					SwitchBindings(InputMode to);

	InputMode					m_InputType = InputMode::Expanding;
	T_HexGrid					m_Grid;

	C_PortalAI				m_PortalAI;
};
