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
	
	void					SelectTile(AHexTileActor* hexTile);
	void					DeselectTile();
	void					Expand(const S_HexCoordinates& dir);

	UPROPERTY(EditAnywhere)
	UClass*								ExpansionArrowActor;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*>	AvailableTiles;

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


private:
	void					DeleteTile();
	void					ExpandUp();
	void					HandleSelectionMaterial(AHexTileActor* hexTile);
	void					CycleModel();
	void					RotateModel();
	void					ShowExpansionArrows();

private:
	T_HexGrid			m_Grid;
};
