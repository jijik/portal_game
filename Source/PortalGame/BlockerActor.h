// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CompanionActor.h"
#include "HexCoordinates.h"
#include "BlockerActor.generated.h"

class AHexTileActor;

UCLASS()
class PORTALGAME_API ABlockerActor : public ACompanionActor
{
	GENERATED_BODY()
public:
	ABlockerActor();
	virtual void		Tick(float DeltaSeconds) override;
	virtual void		OnDrop() override;
	virtual void		OnPick() override;

	AHexTileActor* m_BaseTile = nullptr;
	bool	m_Placing = false;
	HexDir m_NeighborId = InvalidHexDir;
};
