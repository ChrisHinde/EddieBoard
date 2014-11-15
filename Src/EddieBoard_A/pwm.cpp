/*
 * pwm.c
 *
 * Created: 2014-10-28 22:25:08
 *  Author: Christopher
 * 
 *  Code from http://www.avrfreaks.net/comment/853254#comment-853254
 */ 


#define F_CPU 1600000UL // 16 MHz clock speed

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "pwm.h"


uint8_t red_pwm;
uint8_t green_pwm;
uint8_t blue_pwm;
uint8_t invert_pwm = 0;

RgbColor rgb;

/* Initialize the PWM functionality */
void pwm_initialize()
{
	pwm_initialize( 0 );
}
/* Initialize the PWM functionality (Invert is useful when having a RGB LED with common anode) */
void pwm_initialize( uint8_t invert )
{
	invert_pwm  = invert;

	red_pwm   = 0;
	green_pwm = 0;
	blue_pwm  = 0;

	//current_color = { .h = 0, .s = 255, .v = 255 };
		
	// Set mode of operation to FastPWM
	TCCR0A |= (1<<WGM00 | 1<<WGM01);
	TCCR2A |= (1<<WGM20 | 1<<WGM21);
	
	// Set clock source (prescaler)
	TCCR0B |= (1<<CS01);
	TCCR2B |= (1<<CS21);
	
	// Set to 0% duty cycle
	OCR0A = 0x0;
	OCR0B = 0x0;
	OCR2B = 0x0;
	
	// 3 PWM channel outputs
	DDRD |= 1<<PD3; // OC2B
	DDRD |= 1<<PD5; // OC0B
	DDRD |= 1<<PD6; // OC0A
	
}
/* Enable PWM channels */
void pwm_enable(int channel) {
	if (channel & CHAN_OC0A) TCCR0A |= 1<<COM0A1;
	if (channel & CHAN_OC0B) TCCR0A |= 1<<COM0B1;
	if (channel & CHAN_OC2B) TCCR2A |= 1<<COM2B1;
}

/* Disable PWM channels */
void pwm_disable(int channel) {
	if (channel & CHAN_OC0A) TCCR0A &= ~(1<<COM0A1);
	if (channel & CHAN_OC0B) TCCR0A &= ~(1<<COM0B1);
	if (channel & CHAN_OC2B) TCCR2A &= ~(1<<COM2B1);
}

/* Sets the channel brightness */
void pwm_dutycycle(int channel, uint8_t dutycycle) {
	if (invert_pwm) dutycycle = 255 - dutycycle;
	if (channel & CHAN_OC0A) OCR0A = dutycycle;
	if (channel & CHAN_OC0B) OCR0B = dutycycle;
	if (channel & CHAN_OC2B) OCR2B = dutycycle;
}

/* Use HSV data to set the color of the LEDs */
void setHSV( HsvColor color )
{
	rgb = color.ToRGB();
	
	red_pwm   = rgb.r;
	green_pwm = rgb.g;
	blue_pwm  = rgb.b;

	pwm_dutycycle( CHAN_RED,   red_pwm );
	pwm_dutycycle( CHAN_GREEN, green_pwm );
	pwm_dutycycle( CHAN_BLUE,  blue_pwm );
}

/* Simple delay function that takes an variable (_delay_ms only takes constants) */
void delay( int ms )
{
	while(ms--) {
		_delay_ms(1);
	}
}