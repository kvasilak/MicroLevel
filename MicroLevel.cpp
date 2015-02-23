/*
 * Microlevel.cpp
 *
 * Created: 11/4/2014 9:24:15 PM
 *  Author: keith
 */ 

#include "common.h"
#include "CSerial.h"
#include "CController.h"

#include "CTimer.h"
#include "CBlink.h"
#include <avr/io.h>
#include "CLeds.h"
#include "CADC.h"
#include "Cio.h"

uint16_t a = 0;
uint8_t b = 0;
uint8_t txbuf[100];
uint8_t rxbuf[100];

//error handler that is invoked when a pure virtual function is called.
extern "C" void __cxa_pure_virtual()
{
	//reset!
	while (1);
}

int main(void)
{
	CController Controller;
	
	//Turn on 5v, PA2
	DDRA |= _BV(DDRA2); 
	PORTA |= _BV(PA2);
	
	CSerial::is().Init();
	
	CTimer::Init();

	CLeds::is().Init();
	CLeds::is().Dim(100);
	
	Cio::is().Init();
	Controller.Init();
	
	CADC::is().Init();

    while(1)
    {
		//if(CTimer::IsTimedOut(100, start))
		//{
			
			Controller.Run();

		//	start = CTimer::GetTick();
		//}
		
	
    }
}