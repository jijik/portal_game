// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "TeleportActor.generated.h"

UCLASS()
class PORTALGAME_API ATeleportActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	ATeleportActor();
	
	void	Tick(float DeltaSeconds);
	void	Link(ATeleportActor& other);

private:
	ATeleportActor* m_LinkedTeleport = nullptr;
};
