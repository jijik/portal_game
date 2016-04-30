// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Dude.generated.h"

UCLASS()
class PORTALGAME_API ADude : public ADefaultPawn
{
	GENERATED_BODY()

public:
	ADude();

	virtual void	BeginPlay() override;
	virtual void	Tick( float DeltaSeconds ) override;

	void					Move();
};
