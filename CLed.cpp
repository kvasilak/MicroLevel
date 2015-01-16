/* 
* CLed.cpp
*
* Created: 1/13/2015 10:33:49 PM
* Author: keith
*/

#include <stdint.h>
#include "CLed.h"
#include "i2cmaster.h"

#ifdef __cplusplus
extern "C" {
	#endif
#include "TWI_Master.h"

#ifdef __cplusplus
}
#endif

	const uint8_t RUpOn		= uint8_t(0x03u);
	const uint8_t RUpOff	= uint8_t(~0x03U);
	const uint8_t LUpOn		= uint8_t(0x09);
	const uint8_t LUpOff	= uint8_t(~0x09);
	const uint8_t RDownOn	= uint8_t(0x30);
	const uint8_t RDownOff	= uint8_t(~0x30);
	const uint8_t LDownOn	= uint8_t(0x90);
	const uint8_t LDownOff	= uint8_t(~0x90U);


// default constructor
CLed::CLed():
LEDReg1(0x0C),
WhatsOn(32)
{
	//i2c_init();                             // initialize I2C library
	TWI_Master_Initialise();
} //CLed

void CLed::Init()
{
	  if(i2c_start_wait(0x40))
	   if(i2c_write(0x80)==0)  //Control reg autoincrement starting at mode0
	    if(i2c_write(0x80)==0)  //mode1 auto inc
	     if(i2c_write(0x00)==0)  //mode2
	      if(i2c_write(30)==0)       //PWM0
	       if(i2c_write(30)==0)       //PWM1
	        if(i2c_write(30)==0)       //PWM2
	         if(i2c_write(30)==0)       //PWM3
	          if(i2c_write(30)==0)       //PWM4
	           if(i2c_write(30)==0)       //PWM5
	            if(i2c_write(30)==0)       //PWM6
	             if(i2c_write(30)==0)       //PWM7
	              if(i2c_write(0xFF)==0)    //GRPPWM
	               if(i2c_write(0x00)==0)    //GRPFREQ
	                if(i2c_write(0X00)==0)    //LEDOUT0
	                 if(i2c_write(0x00)==0)    //LEDOUT1
	                  ;
	  
	  i2c_stop();
}

void CLed::Set(bool RU, bool RD, bool Active, bool LU, bool LD, bool Camp, bool TravelWarn, bool TravelOK, uint8_t brightness)
{
	uint8_t low;
	//unsigned char messageBuf[4];
	
	//unsigned char TWI_targetSlaveAddress   = 0x40;
	
	//messageBuf[0] = (TWI_targetSlaveAddress<<TWI_ADR_BITS) | (FALSE<<TWI_READ_BIT); // The first byte must always consit of General Call code or the TWI slave address.
    //messageBuf[1] = 0x0c;             // The first byte is used for commands.
    //messageBuf[2] = 0x55;                        // The second byte is used for the data.
    //TWI_Start_Transceiver_With_Data( messageBuf, 3 );
	
	low  = RU << 1;
	low  |= RD << 3;
	low  |= LU << 5;
	low  |= LD << 7;
	
	
	  if(i2c_start_wait(0x40))
	   if(i2c_write(0x80)==0) //Control reg autoincrement starting at mode0
	    if(i2c_write(0x80)==0) //mode auto inc
	     if(i2c_write(0x00)==0)
	      if(i2c_write(brightness)==0)       //PWM0
	       if(i2c_write(brightness)==0)       //PWM1
	        if(i2c_write(brightness)==0)       //PWM2
	         if(i2c_write(brightness)==0)       //PWM3
	          if(i2c_write(brightness)==0)       //PWM4
	           if(i2c_write(brightness)==0)       //PWM5
	            if(i2c_write(brightness)==0)       //PWM6
	             if(i2c_write(brightness)==0)       //PWM7
	              if(i2c_write(0xFF)==0)       //GRPPWM
	               if(i2c_write(0x00)==0)       //GRPFREQ
	                if(i2c_write(low)==0) //0x55)==0)       //LEDOUT0
	                 if(i2c_write(0xAA)==0)       //LEDOUT1
	                  ;
	  
	  i2c_stop();
	  
}

void CLed::Set1(bool RU, bool RD, bool Active, bool LU, bool LD, bool Camp, bool TravelWarn, bool TravelOK, uint8_t brightness)
{
	WriteReg(2, brightness);
	
	//  if(i2c_start_wait(0x40))
	//   if(i2c_write(0x02)==0) //Control reg autoincrement starting at mode0
	//      if(i2c_write(brightness)==0)       //PWM0
	       //if(i2c_write(brightness)==0)       //PWM1
	        //if(i2c_write(brightness)==0)       //PWM2
	         //if(i2c_write(brightness)==0)       //PWM3
	          //if(i2c_write(brightness)==0)       //PWM4
	           //if(i2c_write(brightness)==0)       //PWM5
	            //if(i2c_write(brightness)==0)       //PWM6
	             //if(i2c_write(brightness)==0)       //PWM7
	                  ;
	  
	  //i2c_stop();
	  
}

void CLed::RightUpOn()
{
	WhatsOn |= RUpOn;
	WriteReg(LEDReg1, WhatsOn);
}

void CLed::RightUpOff()
{
	WhatsOn &= RUpOff;
	WriteReg(LEDReg1, WhatsOn);
}

void CLed::LeftUpOn()
{
	WhatsOn |= LUpOn;
	WriteReg(LEDReg1, WhatsOn);
}

void CLed::LeftUpOff()
{
	WhatsOn &= LUpOff;
	WriteReg(LEDReg1, WhatsOn);
}

void CLed::RightDownOn()
{
	WhatsOn |= RDownOn;
	WriteReg(LEDReg1, WhatsOn);
}

void CLed::RightDownOff()
{
	WhatsOn &= RDownOff;
	WriteReg(LEDReg1, WhatsOn);
}

void CLed::LeftDownOn()
{
	WhatsOn |= LDownOn;
	WriteReg(LEDReg1, WhatsOn);
}

void CLed::LeftDownOff()
{
	WhatsOn &= LDownOff;
	WriteReg(LEDReg1, WhatsOn);
}

void CLed::GlobalBrightness(uint8_t Brightness)
{
	WriteReg(0X0A, Brightness);
}

void CLed::WriteReg(uint8_t LEDReg, uint8_t data)
{
	if(i2c_start_wait(0x40))
	   if(i2c_write(LEDReg)==0) //Control reg single register
	      if(i2c_write(data)==0)
		  {
			  i2c_stop();
		  }
}