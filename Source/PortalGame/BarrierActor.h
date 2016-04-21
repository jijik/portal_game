// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <utility>

#include "PortalUtils.h"
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
	
	void						SetOwningTileBeforePlace(AHexTileActor*, unsigned sector = s_InvalidSector);
	bool						IsReadyToPlace();

	AHexTileActor*	GetOwningTileBeforePlace();
	unsigned				GetOwningSectorBeforePlace();

	void						Place(AHexTileActor& front, AHexTileActor* back);

private:
	unsigned																	m_CurrentModelId = 0;
	std::pair<AHexTileActor*, AHexTileActor*> m_Neighbors;
	AHexTileActor*														m_OwningTileBeforePlace = nullptr;
	unsigned																	m_OwningSectorBeforePlace = s_InvalidSector;
};
