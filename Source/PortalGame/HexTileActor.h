// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HexCoordinates.h"
#include "GameFramework/Actor.h"
#include "HexTileActor.generated.h"

class ABarrierActor;
class APlatformActor;

UCLASS()
class PORTALGAME_API AHexTileActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:	

	UFUNCTION()
	void			OnClick(UPrimitiveComponent* c);

	void			Init(const struct S_HexCoordinates& coordinates);
	void			SetSelectedMaterial(bool);
	void			CycleModel();
	void			RotateModel();

	const S_HexCoordinates& GetCoordinates();
	void										SetCoordinates(const S_HexCoordinates&);

	void						PlaceBarrierAt(ABarrierActor& b, HexDir id);
	void						RemoveBarrierAt(HexDir id);
	bool						HasBarrierAt(HexDir id);
	ABarrierActor*	GetBarrierAt(HexDir id);

	void						PlacePlatform(APlatformActor& p);
	void						RemovePlatform();
	bool						HasPlatform();
	APlatformActor* GetPlatform();

	void						Save(std::ofstream& stream);
	void						Load(std::ifstream& stream);

private:
	S_HexCoordinates				m_Coordinates;
	unsigned								m_CurrentModelId = 0;

	Array6<ABarrierActor*>	m_NeighborBarriers;
	APlatformActor*					m_Platform = nullptr;
};
