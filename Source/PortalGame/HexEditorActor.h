// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <set>

#include "GameFramework/Actor.h"
#include "HexGrid.h"
#include "HexTileActor.h"
#include "HexEditorActor.generated.h"

class ABarrierActor;

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
	void					ClearAll();
	void					SaveMap(const FString& name);
	void					LoadMap(const FString& name);

	UPROPERTY(EditAnywhere)
	UClass*								ExpansionArrowActor;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*>	AvailableTiles;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*>	AvailableBarriers;

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

private:
	void					DeleteTile();
	void					UnlinkBarriersFromTile(AHexTileActor& hexTile);

	void					ExpandUp();
	void					HandleSelectionMaterial(AHexTileActor* hexTile);
	void					CycleModel();
	void					RotateModel();
	void					ShowExpansionArrows();
	void					ChangeInputMode();

	void					CreateBarrierForPlacing();
	void					UpdateBarriers();
	void					PlaceBarrier(bool createAnother = true);
	unsigned			GetNeighborId(const FVector& fromCenter);
	void					DeleteBarrier();

private:

	enum InputMode
	{
		Expanding = 0,
		Barriers,
		TOTAL
	};

	InputMode					m_InputType = InputMode::Expanding;
	T_HexGrid					m_Grid;
	std::set<ABarrierActor*> m_AllBarriers;
};
