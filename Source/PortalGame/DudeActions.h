#pragma once

#include "AIController.h"

class ADude;
class ACompanionActor;

//========================================================================
class C_DudeAction
{
public:
	C_DudeAction(ADude& dude);

	virtual void Start() = 0;
	virtual bool Update() { return false; } //return true if want more update

protected:
	ADude& m_Dude;
};

//========================================================================
class C_DudeMoveTo : public C_DudeAction
{
public:
	using C_DudeAction::C_DudeAction;

	virtual void Start() override;
	virtual bool Update() override;
	
	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	EPathFollowingResult::Type result;
	FVector target;
};

//========================================================================
class C_DudePick : public C_DudeAction
{
public:
	using C_DudeAction::C_DudeAction;

	virtual void Start() override;
	ACompanionActor* compation = nullptr;
};