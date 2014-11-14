/*
 * pwm.h
 *
 * Created: 2014-10-28 22:24:19
 *  Author: Christopher
 */ 


#ifndef PWM_H_
#define PWM_H_

#include <avr/io.h>

#include "HSV.h"

enum PWM_CHANNEL {
	// Name by color
	CHAN_RED		= 1<<3,
	CHAN_GREEN		= 1<<1,
	CHAN_BLUE		= 1,
	
	// Name by index
	CHAN_1			= 1,
	CHAN_2			= 1<<1,
	CHAN_4			= 1<<3,
	
	// Name by port
	CHAN_PD6		= 1,
	CHAN_PD5		= 1<<1,
	CHAN_PD3		= 1<<3,
	
	// Name by register
	CHAN_OC0A		= 1,
	CHAN_OC0B		= 1<<1,
	CHAN_OC2B		= 1<<3
	
};


void pwm_initialize( );
void pwm_initialize( uint8_t invert );
// Enable PWM channels
void pwm_enable(int channel);
// Disable PWM channels
void pwm_disable(int channel);

// Sets the channel brightness
void pwm_dutycycle(int channel, uint8_t dutycycle);

void setHSV( HsvColor color );

void delay( int ms );

#endif /* PWM_H_ */