// Fill out your copyright notice in the Description page of Project Settings.

#include "PortalGame.h"
#include "Dude.h"
#include "CompanionActor.h"
#include "AIController.h"
#include "HexGame.h"
#include "PortalUtils.h"
#include "DudeActions.h"

//========================================================================
ADude::ADude()
{
	PrimaryActorTick.bCanEverTick = true;
}

//========================================================================
void ADude::BeginPlay()
{
	Super::BeginPlay();
}

//========================================================================
void ADude::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (!m_ActionQueue.empty())
	{
		auto* currentAction = m_ActionQueue.front();
		if (!currentAction->Update(DeltaTime))
		{
			currentAction->End();
			delete currentAction;

			m_ActionQueue.pop();
			if (!m_ActionQueue.empty())
			{
				m_ActionQueue.front()->Start();
			}
		}
	}
}

//========================================================================
ACompanionActor* ADude::GetCompanion()
{
	return m_Companion;
}

//========================================================================
void ADude::SetCompanion(ACompanionActor* c)
{
	m_Companion = c;
}

//========================================================================
void ADude::Move(const FVector& vec)
{
	auto* gotoAction = new C_DudeMoveTo(*this);
	gotoAction->target = vec;
	SetAction(*gotoAction);
}

//========================================================================
void ADude::Stop()
{
	AAIController* controller = CastChecked<AAIController>(GetController());
	controller->StopMovement();
}

//========================================================================
void ADude::Pick(ACompanionActor* companion)
{
	auto* action = new C_DudePick(*this);
	action->companion = companion;
	SetAction(*action);
}

//========================================================================
void ADude::Drop()
{
	SetAction(*new C_DudeDrop(*this));
}

//========================================================================
void ADude::SetAction(C_DudeAction& action)
{
	ClearActionQueue();
	PushAction(action);
}

//========================================================================
void ADude::PushAction(C_DudeAction& action, bool start)
{
	m_ActionQueue.push(&action);
	if (start && m_ActionQueue.size() == 1)
	{
		m_ActionQueue.front()->Start();
	}
}

//========================================================================
void ADude::PushActions(std::initializer_list<C_DudeAction*> actions)
{
	for (auto& action : actions)
	{
		m_ActionQueue.push(action);
	}

	if (m_ActionQueue.size() == actions.size())
	{
		m_ActionQueue.front()->Start();
	}
}

//========================================================================
void ADude::StartActions()
{
	check(!m_ActionQueue.empty());
	m_ActionQueue.front()->Start();
}

//========================================================================
void ADude::ClearActionQueue()
{
	if (!m_ActionQueue.empty())
	{
		m_ActionQueue.front()->Cancel();
	}

	while (!m_ActionQueue.empty())
	{
		delete m_ActionQueue.front();
		m_ActionQueue.pop();
	}
}