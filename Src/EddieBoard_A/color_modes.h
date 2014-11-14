/*
 * color_modes.h
 *
 * Created: 2014-11-05 23:31:44
 *  Author: Christopher
 */ 


#ifndef COLOR_MODES_H_
#define COLOR_MODES_H_

#include "HSV.h"

enum COLOR_MODE {
	OFF             = 0x00,
	
	COLOR           = 0x01, CONSTANT_COLOR = 0x01,
	FLASH           = 0x02, BLINK = 0x02,
	PULSE           = 0x03,
	PULSE_SINE      = 0x04,
	DIM             = 0x05,
	DIM_TO          = 0x06,
	FADE_PALINDROME = 0x07,
	FADE            = 0x08,
	FADE_TO         = 0x09,
	WANDER          = 0x0A,
	JUMP            = 0x0B,
	FLICKER			= 0x0C,
	RAINBOW			= 0x0D
};

void setMode( enum COLOR_MODE mode );
void setModeHSV( enum COLOR_MODE mode, HsvColor &color );
void setModeRGB( enum COLOR_MODE mode, RgbColor &color );

void setTempMode( enum COLOR_MODE mode );
void setTempMode( enum COLOR_MODE mode, uint8_t ttl );
void setTempModeHSV( enum COLOR_MODE mode, HsvColor &color );
void setTempModeHSV( enum COLOR_MODE mode, HsvColor &color, uint8_t ttl );
void setTempModeRGB( enum COLOR_MODE mode, RgbColor &color );
void setTempModeRGB( enum COLOR_MODE mode, RgbColor &color, uint8_t ttl );


void setCurrentHSV( HsvColor color );
void setTempHSV( HsvColor color );

void setBrightness( uint8_t value );
void setValue( uint8_t value );

void setSpeed( uint8_t speed );
void setTempSpeed( uint8_t speed );

void runMode();
void runTempMode();

int random( int value );
int random_sin( int value );

#endif /* COLOR_MODES_H_ */