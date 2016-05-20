// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "PortalUtils.h"
#include "TeleportActor.generated.h"

UCLASS()
class PORTALGAME_API ATeleportActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	ATeleportActor();
	
	void	Tick(float DeltaSeconds);
	void	Link(ATeleportActor& other);
	void	Use(AActor&);

	void	Save(std::ofstream& file);
	void	Load(std::ifstream& file);
	void	PostLoad();

	unsigned GetId() { return m_Id; }

private:
	ATeleportActor* m_LinkedTeleport = nullptr;

	unsigned				m_Id;
	static unsigned	IDProvider;
	unsigned				m_LoadedId;
};
