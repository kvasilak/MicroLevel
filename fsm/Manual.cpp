/* 
* Manual.cpp
*
* Created: 2/1/2015 10:14:32 PM
* Author: keith
*/


#include "Manual.h"
#include "..\CController.h"


FsmManual::FsmManual(CController& SMManager) :
CState(SMManager, STATE_MANUAL)
{
}

void FsmManual::OnEntry()
{
	//cout << "\nCStateIdle::OnEntry()" << endl;
}

void FsmManual::HandleEvent(EVENT evt)
{
	switch(evt)
	{
		case TravelEvent:
			m_SMManager.ChangeState(STATE_TRAVEL);
		break;
		case CampEvent:
			m_SMManager.ChangeState(STATE_CAMP);
		break;
		case CalibrateEvent:
			m_SMManager.ChangeState(STATE_MANUAL_CALIBRATE);
		break;
		default:
		break;
	}
}

void FsmManual::OnExit()
{
	//cout << "CStateIdle::OnExit()" << endl;
}