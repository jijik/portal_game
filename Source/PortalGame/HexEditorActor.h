// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HexGrid.h"
#include "HexTileActor.h"
#include "HexEditorActor.generated.h"

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
	void					Expand(const S_HexCoordinates& dir);

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

	class ABarrierActor*	m_CurrentBarrier = nullptr;

private:
	void					DeleteTile();
	void					ExpandUp();
	void					HandleSelectionMaterial(AHexTileActor* hexTile);
	void					CycleModel();
	void					RotateModel();
	void					ShowExpansionArrows();
	void					ChangeInputMode();

	void					UpdateBarriers();
	unsigned			GetNeighborId(const FVector& fromCenter);

private:

	enum InputMode
	{
		Expanding = 0,
		Barriers,
		TOTAL
	};

	InputMode					m_InputType = InputMode::Expanding;
	T_HexGrid					m_Grid;
};
