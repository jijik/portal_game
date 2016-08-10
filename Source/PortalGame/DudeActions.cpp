#include "PortalGame.h"
#include "DudeActions.h"
#include "AIController.h"
#include "Dude.h"
#include "CompanionActor.h"
#include "HexGame.h"
#include "PortalUtils.h"

//========================================================================
C_DudeAction::C_DudeAction(ADude& dude)
	: m_Dude(dude)
{

}

//========================================================================
void C_DudeMoveTo::Start()
{
	AAIController* controller = CastChecked<AAIController>(m_Dude.GetController());
	auto ret = controller->MoveToLocation(target, -1.f, true, true, false, true, 0, false); //do not allow partial paths!
	if (ret == EPathFollowingRequestResult::Failed)
	{
		m_Dude.ClearActionQueue();
		cbHandle.Reset();
	}
	else
	{
		cbHandle = controller->GetPathFollowingComponent()->OnMoveFinished.AddRaw(this, &C_DudeMoveTo::OnMoveCompleted);
	}

}

void C_DudeMoveTo::OnMoveCompleted(FAIRequestID, EPathFollowingResult::Type Result)
{
	result = Result;
	if (result != EPathFollowingResult::Success)
	{
		m_Dude.ClearActionQueue();
	}
}

bool C_DudeMoveTo::Update()
{
	AAIController* controller = CastChecked<AAIController>(m_Dude.GetController());
	auto status = controller->GetMoveStatus();
	return status != EPathFollowingStatus::Idle;
}

void C_DudeMoveTo::End()
{
	if (cbHandle.IsValid())
	{
		AAIController* controller = CastChecked<AAIController>(m_Dude.GetController());
		controller->GetPathFollowingComponent()->OnMoveFinished.Remove(cbHandle);
		cbHandle.Reset();
	}

}

//========================================================================
void C_DudePick::Start()
{
	m_Dude.Pick(compation);
}