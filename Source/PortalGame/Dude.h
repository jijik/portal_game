// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include <queue>
#include "Dude.generated.h"

class ACompanionActor;
class C_DudeAction;

UCLASS()
class PORTALGAME_API ADude : public ADefaultPawn
{
	GENERATED_BODY()

public:
	ADude();

	virtual void			BeginPlay() override;
	virtual void			Tick( float DeltaSeconds ) override;

	void							Move(const FVector& vec);
	void							Stop();
	void							Pick(ACompanionActor* companion);
	void							Drop();

	void							PushAction(C_DudeAction& action, bool start = true); //takes ownership
	void							PushActions(std::initializer_list<C_DudeAction*> actions); //takes ownership and starts it if first
	void							SetAction(C_DudeAction& action); //takes ownership and clear other actions
	void							StartActions(); //starts the first action (when pushed without start, when pushing more actions)

	void							ClearActionQueue();

	ACompanionActor*	GetCompanion();
	void							SetCompanion(ACompanionActor*);

private:
	ACompanionActor*		m_Companion = nullptr;

	std::queue<C_DudeAction*> m_ActionQueue;
};
