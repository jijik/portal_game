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
		if (!currentAction->Update())
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
void ADude::Move(const FVector& vec)
{
	ClearActionQueue();
	auto* gotoAction = new C_DudeMoveTo(*this);
	gotoAction->target = vec;
	PushAction(*gotoAction);
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
	if (m_Companion != nullptr)
	{
		return;
	}

	m_Companion = companion;
 	m_Companion->SetActorRelativeLocation(FVector(0, 0, 100));
 	m_Companion->SetActorRelativeScale3D(FVector(1, 1, 1));
 	m_Companion->SetActorRelativeRotation(FRotator(0, 0, 0));
	m_Companion->AttachRootComponentToActor(this);
	m_Companion->OnPick();
}

//========================================================================
void ADude::Drop()
{
	if (m_Companion == nullptr)
	{
		return;
	}

	m_Companion->DetachRootComponentFromParent(false);
	m_Companion->SetActorLocation(GetActorLocation());
	m_Companion->OnDrop();
	m_Companion = nullptr;
}

//========================================================================
void ADude::PushAction(C_DudeAction& action)
{
	m_ActionQueue.push(&action);
	if (m_ActionQueue.size() == 1)
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