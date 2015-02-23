/* 
* Cio.h
*
* Created: 2/15/2015 3:09:15 PM
* Author: keith
*/


#ifndef __CIO_H__
#define __CIO_H__

#include "Debounced.h"

enum FillStates
{
	SolenoidFilling,
	SolenoidHolding,
	SolenoidDumping
};

class Cio
{
public:
static Cio &is()
{
	static Cio a;
	return a;
}

	void Init();
	void Run();
	
	void AllOff();
	void RightFillOn();
	void RightFillOff();
	void RightDumpOn();
	void RightDumpOff();
	void LeftFillOn();
	void LeftFillOff();
	void LeftDumpOn();
	void LeftDumpOff();

	//whats changed
	bool ManualChanged();
	bool CampChanged();
	bool TravelChanged();

//What to do
	void LeftSwitches();
	void RightSwitches();
	void CampSwitches();
	void TravelSwitches();
	
	FillStates LeftState;
	FillStates RightState;

private:
	Cio();
	Cio& operator=( const Cio &c );

	Debounced RockerUp;
	Debounced RockerDown;
	Debounced PushCamp;
	Debounced PushTravel;

}; //Cio

#endif //__CIO_H__