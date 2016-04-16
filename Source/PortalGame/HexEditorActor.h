// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HexGrid.h"
#include "HexTileActor.h"
#include "HexEditorActor.generated.h"

class UHexTileComponent;

UCLASS()
class PORTALGAME_API AHexEditorActor : public AHexTileActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexEditorActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void					SelectTile(UHexTileComponent* hexTile);
	void					DeselectTile();
	void					Expand(const S_HexCoordinates& dir);

	UPROPERTY(EditAnywhere)
	UClass*								ExpansionArrowActor;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*>	AvailableTiles;

	UHexTileComponent*		m_SelectedHexComponent;

	using T_HexGrid =			C_HexGrid<AHexTileActor*>;
	T_HexGrid&						GetHexGrid();

	AActor*	m_ArrowsParent;
	Array6<AStaticMeshActor*>	m_Arrows;

	S_HexCoordinates			m_RootTileCoordinates;

	UMaterial*						m_DefaultMaterial;
	UMaterial*						m_SelectedMaterial;


private:
	void					DeleteTile();
	void					ExpandUp();
	void					HandleSelectionMaterial(UHexTileComponent* hexTile);
	void					CycleModel();
	void					RotateModel();
	void					ShowExpansionArrows();

private:
	T_HexGrid			m_Grid;

};
