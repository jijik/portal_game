// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HexTileActor.generated.h"

class ABarrierActor;

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

	void			PlaceBarrierAt(ABarrierActor& b, unsigned id);
	void			RemoveBarrierAt(unsigned id);
	bool			HasBarrierAt(unsigned id);

private:
	S_HexCoordinates				m_Coordinates;
	unsigned								m_CurrentModelId = 0;

	Array6<ABarrierActor*>	m_NeighborBarriers;
};
