// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HexGrid.h"
#include "HexTileActor.h"
#include "HexEditorActor.generated.h"

class UHexTileComponent;

UCLASS()
class PORTALGAME_API AHexEditorActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexEditorActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void					SelectTile(UHexTileComponent* hexTile);
	void					DeselectTile();

	UPROPERTY(EditAnywhere)
	UClass* ExtensionArrowActor;

	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*> m_AvailableTiles;

	UHexTileComponent*	 m_SelectedHexComponent;

	using T_HexGrid = C_HexGrid<AStaticMeshActor*>;

	Array6<AStaticMeshActor*>	m_Arrows;

private:
	void ShowExpansionArrows();

private:
	T_HexGrid m_Grid;

};
