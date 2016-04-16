// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "HexCoordinates.h"
#include "HexEditorActor.h"
#include "HexTileComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTALGAME_API UHexTileComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	void										SetCoordinates(const S_HexCoordinates& c);
	const S_HexCoordinates& GetCoordinates();

	UFUNCTION()
	void	OnClick(UPrimitiveComponent* ClickedComp);

private:
	S_HexCoordinates						m_HexCoordinates;
};
