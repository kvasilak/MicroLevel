/* 
* Cio.cpp
*
* Created: 2/15/2015 3:09:15 PM
* Author: keith
*/

#include <avr/io.h>
#include "Cio.h"


// default constructor
Cio::Cio()
{
} //Cio

//setup IO lines
//Read using  PINx
//Write using PORTx
void Cio::Init()
{
	//port A
	DDRA = 0x0C;
	
	//0		ADC		Left Height
	//1		ADC		Right height
	//2		Out		power up
	//3		OUt		Compressor
	//4		in		Remote RU
	//5		in		Remote RD
	//6		in		Remote LU
	//7		in		Remote LD
	
	
	DDRB = 0xF0;
	
	//port B
	//0		in		Remote up
	//1		in		Remote Down
	//2		in		Remote Travel
	//3		in		Remote Camp
	//4		Out		Right Up
	//5		Out		Right Down
	//6		Out		Left up
	//7		Out		Left Down
	
	//DDRC = 0x;
	//Port C
	//0		SCL		
	//1		SDA		
	//2		JTAG		
	//3		JTAG		
	//4		JTAG		
	//5		JTAG		
	//6		in		Outside valid
	//7		in		Steering Valid
	
	DDRD = 0x02;
	//Port D
	//0		RX		Debug serial
	//1		TX		Debug serial
	//2		in		Calibrate
	//3		in		Ignition On
	//4		in		switch Down
	//5		in		switch Up
	//6		in		switch Travel
	//7		in		switch Camp
	
	//use to debounce switch inputs
	RockerDown.Attach(IO_PORTD, PORTD4);
	RockerUp.Attach(IO_PORTD, PORTD5);
	PushTravel.Attach(IO_PORTD, PORTD6);
	PushCamp.Attach(IO_PORTD, PORTD7);
	
}

//update switch states and debounce
void Cio::Run()
{
	//debounce inputs
	RockerDown.Update();
	RockerUp.Update();
	PushTravel.Update();
	PushCamp.Update();
}

//returns true is any manual button has changed state.
//up/down switch
//up/down steering remote
//up/down  left and right outside remote
bool Cio::ManualChanged()
{
	static const uint8_t PortAManualMask	= 0xF0;
	static const uint8_t PortBManualMask	= 0x003;
	
	static bool OldPortAStatus = false;
	static bool OldPortBStatus = false;
	static bool OldRockerDown = false;
	static bool OldRockerUp = false;
	
	uint8_t portastatus = PINA & PortAManualMask;
	uint8_t portbstatus = PINB & PortBManualMask;
	
	bool RD = (bool)RockerDown;
	bool RU = (bool)RockerUp;

	bool ManualChanged  = OldPortAStatus	!= portastatus;
		ManualChanged  |= OldPortBStatus	!= portbstatus;		
		ManualChanged  |= OldRockerDown		!= RD;
		ManualChanged  |= OldRockerUp		!= RU;
	
	OldPortAStatus = portastatus;
	OldPortBStatus = portbstatus;
	OldRockerDown = RD;
	OldRockerUp = RU;
	
	
	return ManualChanged;
}


//returns true if any camp mode button has changed state
//camp switch
//camp on steering remote
bool Cio::CampChanged()
{
 	static bool OldCamp					= false;
 	static bool OldInsideCamp			= false;
	 
	 bool camp = (PINB & _BV(PORTB3));
 	
 	bool CampChanged	= OldCamp			!= (bool)PushCamp;		//port D bit 7
 	CampChanged			|= OldInsideCamp	!= camp;  //port B bit 3
	 
	 OldCamp		= (bool)PushCamp;
	 OldInsideCamp  = camp;
	
	return CampChanged;
}

//returns true if the travel mode buttons have changed state
//camp switch
//camp on steering remote
bool Cio::TravelChanged()
{
	static bool OldTravel				= false;
	static bool OldInsideTravel			= false;	
	
	bool trav = (PINB & _BV(PORTB2));

	bool TravelChanged	= OldTravel				!= (bool)PushTravel;		 //port D bit 6
	TravelChanged		|= OldInsideTravel		!= trav; //port B bit 2
	
	OldTravel		= (bool)PushTravel;
	OldInsideTravel = trav;
	
	return TravelChanged;
}
