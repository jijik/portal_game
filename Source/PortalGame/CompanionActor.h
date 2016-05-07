// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/StaticMeshActor.h"
#include "PortalUtils.h"
#include "CompanionActor.generated.h"

UCLASS()
class PORTALGAME_API ACompanionActor : public AStaticMeshActor
{
	GENERATED_BODY()
public:
	ACompanionActor();

	void						Init(const FVector& pos);
	
	void						OnClick();

	virtual void		OnPick() {};
	virtual void		OnDrop();

	void						Save(std::ofstream& file);
	void						Load(std::ifstream& file);

private:
	UStaticMesh*	m_Mesh;
	
};
