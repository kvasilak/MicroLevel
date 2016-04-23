/*************************************************************************
**   Microlevel 
**
** Copyright 2015 Keith Vasilakes
**
** This file is part of Microlevel, an Air ride controller.
**
** Microlevel is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
** License as published by the Free Software Foundation, either version 3 of the License, or (at your option) 
** any later version.
**
** Microlevel is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
** implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public 
** License for more details.
**
** You should have received a copy of the GNU General Public License along with Microlevel. If not, see
** http://www.gnu.org/licenses/.
**
**    corner.cpp
**************************************************************************/

//#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>
#include "corner.h"
#include "common.h"
#include "Cio.h"
#include "CSerial.h"
#include "CTimer.h"
#include "CADC.h"

//This class is responsible for managing the height of a corner.
//There is one instance of this class for each corner of the vehicle usually 4
//This is where the decisions are made as to add or remove air from the suspension
CCorner::CCorner(Position p):
	DeadBand(DEAD_BAND),
	CycleTime(100), //ms between updates
	State(ValveIniting),
	PulseTime(250),
	LimitLow(100),            // set limits to 100 from high and low
	LimitHigh(924),
	LongFilter(false),
	IsAtHeight(false)
{

	LastTime 		= CTimer::GetTick();
	filter_reg 		= GetHeight() << FILTER_SHIFT;
   
    UpdateTime = CTimer::GetTick();
}

void CCorner::Init(Position p)
{
    int i;
    int32_t height = GetHeight();
    
    //initialize filters
    for(i=0; i<10; i++)
    {
        SmoothAvg[i] = height;      
    }
    
    for(i=0; i<100; i++)
    {
        HeightAvg[i] = height;      
    }
    
    filter_reg = (height << FILTER_SHIFT);

    count = 0;
    corner = p;
    SlowAt = 0;
    SmoothAt = 0;
    
    HeightSpeed = 0;
    OldHeight = height;
    SpeedTime = CTimer::GetTick();
}
void CCorner::Limits(int16_t Low, int16_t high)
{
    LimitLow = Low +2;
    LimitHigh = high -2;
}

//Get the height of this corner
int32_t CCorner::GetHeight()
{
    int32_t height=0;
    
    switch(corner)
   {
        case LeftRear:
			height = CADC::is().GetLeftHeight();
			//
            break;
        case RightRear:
			height = CADC::is().GetRightHeight();
            break;
   }   
   
   //calculate speed, counts/second
   if(CTimer::IsTimedOut(1000, SpeedTime))
   {
        if(height > OldHeight)
        {
            HeightSpeed = height - OldHeight;
        }
        else
        {
            HeightSpeed = OldHeight - height;
        }
        
        OldHeight = height;
        SpeedTime = CTimer::GetTick();
   }
   
   return height;
}

//open or close the fill solenoid for this corner
void CCorner::FillOn()
{
    switch(corner)
   {
        case LeftRear:
            Cio::is().LeftFillOn();
            break;
        case RightRear:
            Cio::is().RightFillOn();
            break;
   }   
}
void CCorner::FillOff()
{
	switch(corner)
	{
		case LeftRear:
			Cio::is().LeftFillOff();
			break;
		case RightRear:
			Cio::is().RightFillOff();
			break;
	}
}

//open or close the dump solenoid for this corner
void CCorner::DumpOn()
{
    switch(corner)
   {
        case LeftRear:
           Cio::is().LeftDumpOn();
            break;
        case RightRear:
            Cio::is().RightDumpOn();
            break;
   }   
}

void CCorner::DumpOff()
{
	switch(corner)
	{
		case LeftRear:
			Cio::is().LeftDumpOff();
			break;
		case RightRear:
			Cio::is().RightDumpOff();
			break;
	}
}

void CCorner::SetState(ValveOp s)
{
	static const char *CornerName[] = {"Left,", "Right,"};
   static const char *StateStrs[] = {VALVE_STATES_LIST(STRINGIFY)};

    CSerial::is() << ">Corner,";
    CSerial::is() << CornerName[corner];
    CSerial::is() << StateStrs[s] ;
    CSerial::is() << "<\r\n";

    State = s;
}

void CCorner::FillExit()
{
    FillOff();
    SetState(ValveHoldEntry);

    HoldOffTime = CTimer::GetTick();
}

void CCorner::DumpExit()
{
    DumpOff();
    SetState(ValveHoldEntry);
    HoldOffTime = CTimer::GetTick();
}

uint16_t CCorner::Average(uint16_t value)
{
    uint32_t average = 0;
    
    //rollover if we hit the end
    if(SlowAt > 99) SlowAt = 0;
    
    //add new value to array
    HeightAvg[SlowAt] = value;
    
    SlowAt++;
    
    //Calculate new average
    for(int i=0; i<100; i++)
    {
        average += HeightAvg[i];
    }
    
    return (uint16_t)(average/100);
}

uint16_t CCorner::Smooth(uint16_t value)
{
    uint32_t average = 0;
    
    //rollover if we hit the end
    if(SmoothAt > 9) SmoothAt = 0;
    
    //add new value to array
    SmoothAvg[SmoothAt] = value;
    
    SmoothAt++;
    
    //Calculate new average
    for(int i=0; i<10; i++)
    {
        average += SmoothAvg[i];
    }
    
    return (uint16_t)(average/10);
}


void CCorner::SetLongFilter(bool slow)
{
    //reset filters if switching to long filter
    if((false == LongFilter) && (true == slow))
    {
        for(int i=0; i<100; i++)
        {
            HeightAvg[i] = SmoothHeight;      
        }
    }
    
    //Always Reset IIR filter
    filter_reg = (SmoothHeight << FILTER_SHIFT);
    
    LongFilter = slow;
}

void CCorner::AtHeight(bool at)
{
    IsAtHeight = at;
	
	//reset valve state machine
	SetState(ValveIniting);
}

bool CCorner::AtHeight()
{
    return IsAtHeight;
}

//Uses the low pass IIR filter described in "Simple Software Lowpass Filter.pdf"
//And two simple FIR filters
void CCorner::FilterHeight(int32_t height, int32_t setpoint)
{
	//sample Slowly
	if(CTimer::IsTimedOut(CycleTime, LastTime) )
	{        
        if( LongFilter)
        {
            //fast smoothing filter to take out sampling noise
            SmoothHeight = Smooth(height);
            
            if(count >9)
            {
                count = 0;
            
                //Longer filter to take out bumps and roll
                AverageHeight = Average( SmoothHeight);

                //IIR filter to take out even more bump and roll
                // Update IIR filter with current sample.
                filter_reg = filter_reg - (filter_reg >> FILTER_SHIFT) + AverageHeight;

                // Scale output for unity gain.
                slowheight = (filter_reg >> FILTER_SHIFT);
            }
            
            count++;
        }
        else
        {
            SmoothHeight = height;
            
            filter_reg = filter_reg - (filter_reg >> FILTER_SHIFT) + height;

            // Scale output for unity gain.
            slowheight = (filter_reg >> FILTER_SHIFT);
        }

		LastTime = CTimer::GetTick();       
        
        if(CTimer::IsTimedOut(250, UpdateTime))
        {
           switch(corner)
           {
                case LeftRear:
                    CSerial::is() << ">Cnr, L_Err,";
                    CSerial::is() << uint16_t(height - setpoint);
                    CSerial::is() << "<";

                    CSerial::is() << ">Cnr, L_Hght,";
                    CSerial::is() << uint16_t(height);
                    CSerial::is() << "<";

                    CSerial::is() << ">Cnr, L_Slow,";
                    CSerial::is() << uint16_t(slowheight - setpoint);
                    CSerial::is() << "<\r\n";

                    break;
                case RightRear:
                    CSerial::is() << ">Cnr, R_Err,";
                    CSerial::is() << uint16_t(height - setpoint);
                    CSerial::is() << "<";

                    CSerial::is() << ">Cnr, R_Hght,";
                    CSerial::is() << uint16_t(height);
                    CSerial::is() << "<";

                    CSerial::is() << ">Cnr, R_Slow,";
                    CSerial::is() << uint16_t(slowheight - setpoint);
                    CSerial::is() << "<\r\n";                

                    break;
           } 
           UpdateTime = CTimer::GetTick();
        }
    }
}



void CCorner::Run(int32_t setpoint)
{		
    int i;
    int32_t height 			= GetHeight();
    
    //prevent setpoint from exceeding cal limits
    if(setpoint > LimitHigh)
    {
        setpoint = LimitHigh;
    }
    else if(setpoint < LimitLow) 
    {
        setpoint = LimitLow;
    } 
    
    FilterHeight(height, setpoint);

    switch (State)
    {
        case ValveIniting: 
            FillOff();
            DumpOff();
            SetState(ValveHolding);
            //filter_reg = (height << FILTER_SHIFT);
            HoldOffTime = CTimer::GetTick();
            break;
        case ValveHoldEntry:
            if(CTimer::IsTimedOut(1000, HoldOffTime))
            {
                //Over ride the filter, force it to the current value
                //So the hold state doesn't keep changing
                filter_reg = (height << FILTER_SHIFT);
                
                for(i=0; i<100; i++)
                {
                    HeightAvg[i] = height;      
                }

                HoldOffTime = CTimer::GetTick();
                DoPulse = false;
                CycleTime = 100;
                SetState(ValveHolding);
            }
            break;
        case ValveHolding:
            //react slowly if already within deadband
            
            //below setpoint
            if( slowheight < (setpoint - DeadBand))
            {
                //minimum hold time so we don't go crazy hunting
                //except in manual mode, react fast!
                if(!LongFilter || CTimer::IsTimedOut(5000, HoldOffTime))
                {
                    SetState(ValveFilling);
                    FillOn();
                    CycleTime = 100;
                    
                    //if within 5x deadband, only pulse the valve
                    //so we don't over shoot the setpoint due to the long lag time
                    
                    if(height > (setpoint -(DeadBand * 5)) )
                    {                        
                        //calc total pulse time as a multiple of deadbands from setpoint
                        //we know height < setpoint or we wouldn't be here
                        PulseTotal =(abs(setpoint - height) / DeadBand) * PulseTime; // pulsetime = 250ms
                        PulseStart = CTimer::GetTick();

                        SetState(ValveFillPulse);
                    }
                }
            }
            else if(slowheight > (setpoint + DeadBand)) //>524
            {
                if(!LongFilter || CTimer::IsTimedOut(5000, HoldOffTime))
                {
                    SetState(ValveDumping);
                    DumpOn();
                    
                    CycleTime = 100;
                    
                    //if within 5x deadband, only pulse the valve
                    //so we don't over shoot the setpoint due to the long lag time
                    
                    if(height < setpoint + (DeadBand * 5))
                    {                        
                        //calc total pulse time as a multiple of deadbands from setpoint
                        //we know height > setpoint or we wouldn't be here
                        PulseTotal = (abs(height - setpoint) / DeadBand) * PulseTime; // pulsetime = 250ms
                        PulseStart = CTimer::GetTick();

                        SetState(ValveDumpPulse);
                    }
                }
            }
            else
            {
                 //might not be perfectly at height, but should be pretty close
                 IsAtHeight = true;
            }

            break;
        case ValveFillPulse:
            if(CTimer::IsTimedOut(PulseTotal, PulseStart))
            {
                FillExit();
            }
            break;
        case ValveFilling:
            if(height >= (setpoint-DeadBand))
            {    
                FillExit();
            }
            break;
        case ValveDumpPulse:
            if(CTimer::IsTimedOut(PulseTotal, PulseStart))
            {
                DumpExit();
            }
            break;
        case ValveDumping:
            if(height <= (setpoint + DeadBand))
            {    
                DumpExit();
            }
            break;

        default:
            CSerial::is() << ">Corner,Default,State!!<";
            FillOff();
            DumpOff();
            SetState(ValveHoldEntry);
            HoldOffTime = CTimer::GetTick();
    }
}

//used to calibrate height limits.
//the coach may not have travel from 0 to 1024
//wait for both heights to be all the way down
//determined by the height changing less that 2 counts in 1 second
//Waits 10 seconds for the cases where it is slow to start or stop moving
bool CCorner::IsMoving()
{
    bool ret = true;

    //Serial.print(">AirRide,msg,speed; ");
    //Serial.print(HeightSpeed);
    //Serial.println("<");
    
    if(HeightSpeed > 1)
    {
        ret =  true;
    }
    else
    {
        ret =  false;
    }
    return ret;
}




