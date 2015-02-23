/* 
* CController.cpp
*
* Created: 1/24/2015 8:13:35 PM
* Author: keith
*/


#include "CController.h"
#include "CSerial.h"
#include <avr/io.h>
#include "common.h"
#include "CTimer.h"


// default constructor
CController::CController() :
m_stateManual(*this), //call the state constructor with a pointer to the manager class ( this )
m_stateTravel(*this), 
m_stateCamp(*this),
m_stateManualCal(*this),
m_stateTravelCal(*this),
m_stateCampCal(*this),
m_CurrState(STATE_MANUAL)
{
	//this order determines the state machine order
	m_StateList[0] = &m_stateManual;
	m_StateList[1] = &m_stateTravel;
	m_StateList[2] = &m_stateCamp;
	m_StateList[3] = &m_stateManualCal;
	m_StateList[4] = &m_stateTravelCal;
	m_StateList[5] = &m_stateCampCal;
	
	
} 

void CController::Init()
{
	m_StateList[m_CurrState]->OnEntry();
	Time = CTimer::GetTick();
}

void CController::ScheduleEvent(EVENT evt)
{
	static const char *events[]  = {
		"TimerEvent",
		"ManualEvent",
		"TravelEvent",
		"CampEvent",
		"CalibrateEvent"
	};

	//Don't show timer events, too much noise
	if(evt > TimerEvent)
	{
		CSerial::is() << "Event received: " << events[evt] << "\n";
	}
	
	m_StateList[m_CurrState]->HandleEvent(evt);
}

//Exit the old state, enter the new state
void CController::ChangeState(STATE newState)
{
	// put up a warning if the function is called
	//from the entry or exit function.. not legal! 
	//(guard is not thread safe)
	static bool entered = false;
	if(entered)
	{
		CSerial::is() << "DO NOT Call from OnEntry() or OnExit()!!\n";
	}
	
	entered = true;
	
	if(m_CurrState != newState)
	{
		m_StateList[m_CurrState]->OnExit();
		
		m_CurrState = newState;
		
		m_StateList[m_CurrState]->OnEntry();
	}
	
	entered = false;
}


//check all inputs for a state change
//We will generate Manual, Travel and camp events
//Manual event anytime there is any change of the rocker or either remote
//Travel event for any change of the travel button
//Camp event for any change of the camp button
void CController::CheckEvent()
{
	if(Cio::is().ManualChanged())
	{
		ScheduleEvent(ManualEvent);
		
		//update the IO
		Cio::is().LeftSwitches();
		Cio::is().RightSwitches();
	}

 	if(Cio::is().CampChanged())
 	{
 		ScheduleEvent(CampEvent);
 	}

	if(Cio::is().TravelChanged())
	{
		ScheduleEvent(TravelEvent);
	}
	
}

//called on a 100ms timer
void CController::Run()
{
	Cio::is().Run();
	
	CheckEvent();
	
	if(CTimer::IsTimedOut(100, Time))
	{
		ScheduleEvent(TimerEvent);
		Time = CTimer::GetTick();
	}
}

