/* 
* Travel.h
*
* Created: 2/1/2015 10:14:47 PM
* Author: keith
*/


#ifndef __TRAVEL_H__
#define __TRAVEL_H__

#include "State.h"
#include "..\corner.h"

class FsmTravel :public CState
{
	public:
	FsmTravel(CController& FsmManager);
	void OnEntry();
	void HandleEvent(EVENT evt);
	void OnExit();

private:
	CCorner LeftSide;
	CCorner RightSide;
}; //Travel

#endif //__TRAVEL_H__