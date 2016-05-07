// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <utility>

#include "PortalUtils.h"
#include "HexCoordinates.h"
#include "GameFramework/Actor.h"
#include "BarrierActor.generated.h"

class AHexTileActor;

UCLASS()
class PORTALGAME_API ABarrierActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	
	ABarrierActor();

	void						Init();

	virtual void		BeginPlay() override;
	virtual void		Tick( float DeltaSeconds ) override;

	void						CycleModel();
	
	void						SetOwningTileBeforePlace(AHexTileActor*, HexDir sector = InvalidHexDir);
	bool						IsReadyToPlace();

	AHexTileActor*	GetOwningTileBeforePlace();
	unsigned				GetOwningSectorBeforePlace();

	void						Place(AHexTileActor& front, HexDir frontSlot, AHexTileActor* back, HexDir backSlot);
	bool						UnlinkTileFomBarrier(AHexTileActor& tile); // returns if left orphan
	void						UnlinkBarrierFromNeighborTiles();

	void						Off();
	bool						m_On = true;
	bool						m_OnImpl = true;

	UFUNCTION()
	void						OnClick(UPrimitiveComponent*pc);
	void						SetSelectedMaterial(bool b);

	void						Save(std::ofstream& file);
	void						Load(std::ifstream& file);

	unsigned				GetId() { return m_Id; }

private:
	unsigned																	m_CurrentModelId = 0;
	struct S_NeighborInfo { AHexTileActor* neighbor; HexDir slotAtNeighbor; };
	std::pair<S_NeighborInfo, S_NeighborInfo> m_Neighbors; //first is always valid
	AHexTileActor*														m_OwningTileBeforePlace = nullptr;
	HexDir																		m_OwningSectorBeforePlace = InvalidHexDir;

	UStaticMesh*															m_EmptyBarrierMesh;

	unsigned																	m_Id;
	static unsigned														IDProvider;
};
