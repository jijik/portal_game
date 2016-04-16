// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HexCoordinates.h"
#include "Components/ActorComponent.h"
#include "ExpandArrowComponent.generated.h"

class AHexEditorActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTALGAME_API UExpandArrowComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFUNCTION()
	void		OnClick(UPrimitiveComponent* ClickedComp);
	
	void		SetRelativeDirection(const S_HexCoordinates& dir);

private:
	S_HexCoordinates	m_RelativeDirection;
};
