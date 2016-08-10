#pragma once

#include "AIController.h"

class ADude;
class ACompanionActor;

//========================================================================
class C_DudeAction
{
public:
	C_DudeAction(ADude& dude);

	virtual void	Start() { };
	virtual bool	Update(float DeltaTime) { return false; } //return true if want more update
	virtual void	End() { }
	virtual void	Cancel() { }

protected:
	ADude& m_Dude;
};

//========================================================================
class C_DudeMoveTo : public C_DudeAction
{
public:
	using C_DudeAction::C_DudeAction;

	virtual void	Start() override;
	virtual bool	Update(float DeltaTime) override;
	virtual void	End() override;
	virtual void	Cancel() override;

	void OnMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	EPathFollowingResult::Type result;
	FVector target;
	FDelegateHandle cbHandle;
};

//========================================================================
class C_DudePick : public C_DudeAction
{
public:
	using C_DudeAction::C_DudeAction;

	virtual void Start() override;
	ACompanionActor* companion = nullptr;
};

//========================================================================
class C_DudeDrop : public C_DudeAction
{
public:
	using C_DudeAction::C_DudeAction;

	virtual void Start() override;
};


//========================================================================
class C_DudeWait : public C_DudeAction
{
public:
	C_DudeWait(ADude& dude, float howMuch);

	virtual bool	Update(float DeltaTime) override;

private:
	float m_Counter;
};
