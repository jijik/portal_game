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
	controller->MoveToLocation(target);

	controller->GetPathFollowingComponent()->OnMoveFinished.AddRaw(this, &C_DudeMoveTo::OnMoveCompleted);
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

//========================================================================
void C_DudePick::Start()
{
	m_Dude.Pick(compation);
}