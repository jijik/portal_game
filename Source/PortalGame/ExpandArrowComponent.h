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
	UExpandArrowComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void		SetRelativeDirection(const S_HexCoordinates& dir) { m_RelativeDirection = dir; }
	void		SetEditorActor(AHexEditorActor& editorActor);

	UFUNCTION()
	void OnClick(UPrimitiveComponent* ClickedComp);

private:
	S_HexCoordinates	m_RelativeDirection;
	AHexEditorActor*	m_HexEditor;
};
