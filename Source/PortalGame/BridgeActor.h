// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CompanionActor.h"
#include "BridgeActor.generated.h"

class AHexTileActor;

UCLASS()
class PORTALGAME_API ABridgeActor : public ACompanionActor
{
	GENERATED_BODY()
public:	
	ABridgeActor();
	
	virtual void		OnDrop() override;
	virtual void		OnPick() override;

	void						Cycle();


	AHexTileActor*	m_BaseTile = nullptr;
	bool						m_Placing = false;
	
private:
	void						CreateBridge();
	void						DestroyBridge();
	AHexTileActor*	m_CreatedTile = nullptr;
	bool						m_HasBridge;
};
