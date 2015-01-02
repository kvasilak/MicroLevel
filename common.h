/*
 * common.h
 *
 * Created: 11/11/2014 8:45:44 PM
 *  Author: keith
 */ 


#ifndef COMMON_H_
#define COMMON_H_

#define F_CPU 16000000
#define UART_STREAMS

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


#endif /* COMMON_H_ */