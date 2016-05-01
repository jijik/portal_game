// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "PortalUtils.h"
#include "PlatformActor.generated.h"

class APlatformActor;
class AHexTileActor;
class ABarrierActor;

UCLASS()
class PORTALGAME_API APlatformActor : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	APlatformActor();

	void						Init();

	virtual void		BeginPlay() override;
	virtual void		Tick(float DeltaSeconds) override;

	void						CycleModel();

	void						SetOwningTileBeforePlace(AHexTileActor*);
	bool						IsReadyToPlace();

	AHexTileActor*	GetOwningTileBeforePlace();

	void						Place(AHexTileActor& owner);

	void						UnlinkPlatformFromOwningTile();

	void						SetTarget(ABarrierActor*);
	ABarrierActor*	GetTarget();

	UFUNCTION()
	void						OnClick(UPrimitiveComponent*pc);
	void						SetSelectedMaterial(bool b);

	void						Save(std::ofstream& file);
	void						Load(std::ifstream& file);

private:
	unsigned																	m_CurrentModelId = 0;
	AHexTileActor*														m_OwningTileBeforePlace = nullptr;
	AHexTileActor*														m_OwnerTile = nullptr;
	ABarrierActor*														m_Target = nullptr;
};
