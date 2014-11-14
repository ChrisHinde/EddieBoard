/*
 * modes.c
 *
 * Created: 2014-11-05 23:32:09
 *  Author: Christopher
 */

#include <stdlib.h>
#include <math.h>       /* sin */

#include "pwm.h"
 
#include "color_modes.h"

#define PI 3.14159265358979323846

enum COLOR_MODE current_mode = OFF;
enum COLOR_MODE temp_mode = OFF;

uint8_t speed = 50;
uint8_t temp_speed = 50;

HsvColor current_color      = HsvColor( 0, 255, 255 );
HsvColor fade_to_color      = HsvColor( 0, 255, 255 );
HsvColor temp_fade_to_color = HsvColor( 0, 255, 255 );
HsvColor temp_color         = HsvColor( 0, 255, 255 );

uint8_t dim_to = 255;
uint8_t step   = 1;
int position   = 1;
uint8_t temp_dim_to = 255;
uint8_t temp_step   = 1;
uint8_t temp_ttl    = 0;
int temp_position   = 1;

void
setMode( enum COLOR_MODE mode )
{
	if ( ( current_mode == OFF ) && ( mode != OFF ) ) {
		pwm_dutycycle( CHAN_RED | CHAN_GREEN | CHAN_BLUE, 0 );
		pwm_enable( CHAN_RED | CHAN_GREEN | CHAN_BLUE );
	}

	switch (mode)
	{
		case OFF:
			pwm_dutycycle( CHAN_RED | CHAN_GREEN | CHAN_BLUE, 0 );
			pwm_disable( CHAN_RED | CHAN_GREEN | CHAN_BLUE );
			break;
		case COLOR:
			setHSV( current_color );
			break;
		case BLINK:
			position = 1;
			break;
		case PULSE:
			break;
		case PULSE_SINE:
			position = 1;
			break;
		case FADE:
			step = 1;
			current_color.h = 0;
			current_color.s = 255;
			break;
		case FADE_PALINDROME:
			step = 1;
			break;
		case FADE_TO:
			break;
		case DIM:
			break;
		case DIM_TO:
			break;
		case WANDER:
			break;
		case JUMP:
			setSpeed(60);
			break;
		case FLICKER:
			step     = 1;
			position = 1;
			setSpeed(50);
			break;
		default:
			return;
	}
	current_mode = mode;
}

void
setModeHSV( enum COLOR_MODE mode, HsvColor &color )
{
	if ( mode == FADE_TO ) {
		fade_to_color = color;
	} else if ( mode == DIM_TO ) {
		dim_to = color.v;
	} else if ( ( mode == PULSE ) || ( mode == PULSE_SINE ) ) {
		dim_to = color.v;
		step = 1;

		setCurrentHSV( color );
	} else if ( mode == FLICKER ) {
		fade_to_color = color;
		setSpeed(100);

		setCurrentHSV( color );
	} else {
		setCurrentHSV( color );
	}
	
	setMode( mode );
}	

void
setModeRGB( enum COLOR_MODE mode, RgbColor &color )
{
	HsvColor hsv = RgbToHsv( color );
	setModeHSV( mode, hsv );
}
void
setTempMode( enum COLOR_MODE mode, uint8_t ttl )
{
	if ( ( current_mode == OFF ) && ( mode != OFF ) ) {
		pwm_dutycycle( CHAN_RED | CHAN_GREEN | CHAN_BLUE, 0 );
		pwm_enable( CHAN_RED | CHAN_GREEN | CHAN_BLUE );
	}

	switch (mode)
	{
		case OFF:
			break;
		case COLOR:
			setHSV( temp_color );
			break;
		case BLINK:
			temp_position = 1;
			break;
		case PULSE:
			break;
		case PULSE_SINE:
			temp_position = 1;
			break;
		case FADE:
			temp_step = 1;
			break;
		case FADE_PALINDROME:
			temp_step = 1;
			break;
		case FADE_TO:
			break;
		case DIM:
			break;
		case DIM_TO:
			break;
		case WANDER:
			break;
		case JUMP:
			break;
		case FLICKER:
			temp_step     = 1;
			temp_position = 1;
			setTempSpeed(50);
			break;
		default:
			return;
	}
	temp_ttl = ttl;
	temp_mode = mode;
}
void
setTempMode( enum COLOR_MODE mode )
{
	setTempMode( mode, 0 );
};

void
setTempModeHSV( enum COLOR_MODE mode, HsvColor &color, uint8_t ttl )
{
	if ( mode == FADE_TO ) {
		temp_fade_to_color = color;
	} else if ( mode == DIM_TO ) {
		temp_dim_to = color.v;
	} else if ( ( mode == PULSE ) || ( mode == PULSE_SINE ) ) {
		temp_dim_to = color.v;
		temp_step = 1;
		setTempSpeed(2);

		setTempHSV( color );
	} else if ( mode == FLICKER ) {
		temp_fade_to_color = color;

		setTempHSV( color );
	} else {
		setTempHSV( color );
	}

	setTempMode( mode, ttl );
}
void
setTempModeHSV( enum COLOR_MODE mode, HsvColor &color )
{
	setTempModeHSV( mode, color, 0 );
}

void
setTempModeRGB( enum COLOR_MODE mode, RgbColor &color, uint8_t ttl )
{
	HsvColor hsv = RgbToHsv( color );
	setTempModeHSV( mode, hsv, ttl );
}
void
setTempModeRGB( enum COLOR_MODE mode, RgbColor &color )
{
	HsvColor hsv = RgbToHsv( color );
	setTempModeHSV( mode, hsv, 0 );
}

void
setCurrentHSV( HsvColor color )
{
	current_color = color;
}
void
setTempHSV( HsvColor color )
{
	temp_color = color;
}

void
setBrightness( uint8_t value )
{
	current_color.v = value;
}
void
setValue( uint8_t value )
{
	setBrightness(value);
}

void
setSpeed( uint8_t s )
{
	speed = s;
}
void
setTempSpeed( uint8_t s )
{
	temp_speed = s;
}

void
runMode()
{
	if ( temp_mode != OFF ) {
		runTempMode();
		//delay(temp_speed);
		return;
	}

	if ( ( current_mode == OFF ) || ( current_mode == COLOR ) )
		return;

	if ( current_mode == DIM_TO ) {
		if ( current_color.v == dim_to ) {
			if ( dim_to == 0 ) {
				setMode( OFF );
			} else {
				setMode( COLOR );
			}
			return;
		}

		if ( current_color.v > dim_to )
			current_color.v--;
		else
			current_color.v++;
		
	} else if ( current_mode == DIM ) {
		if ( current_color.v == 1 ) {
			setMode( OFF );
			return;
		}

		if ( current_color.v > 1 )
			current_color.v--;
	
	} else if ( current_mode == FADE_TO ) {
		if ( current_color.h == fade_to_color.h ) {
			setMode( COLOR );
			return;
		}
			
		if ( current_color.h > fade_to_color.v )
			current_color.h--;
		else
			current_color.h++;
	} else if ( current_mode == FADE ) {
		current_color.h += step;

		if ( ( current_color.h == 255 ) && ( step == 1 ) )
			current_color.h = 0;
		else if ( ( current_color.h == 0 ) && ( step == -1 ) )
			current_color.h = 255;
	} else if ( current_mode == FADE_PALINDROME ) {
		current_color.h += step;

		if ( current_color.h == 0 )
			step = 1;
		else if ( current_color.h == 255 )
			step = -1;
	} else if ( current_mode == PULSE ) {
		current_color.v += step;

		if ( current_color.v == dim_to )
			step = 1;
		else if ( current_color.v == 255 )
			step = -1;
	} else if ( current_mode == PULSE_SINE ) {

		position += step;
		
		current_color.v = (int)(255.0 * sin(position * ( PI / 2.0 / 255.0 )));

		if ( position == dim_to )
			step = 1;
		else if ( position == 255 )
			step = -1;
	} else if ( current_mode == FLICKER ) {
		position = (int)((double)random_sin(4) * sin((double)step / 255 * 2 * PI));
		current_color.h = fade_to_color.h + position;
		
		delay( 2 );
		
		position = (int)((double)random_sin(2) * sin((double)step / 255 * 2 * PI));
		current_color.s = fade_to_color.s + position;
		
		delay( 2 );
		
		position = (int)((double)random_sin(5) * sin((double)step / 255 * 2 * PI));
		current_color.v = fade_to_color.v + position;
		
		if ( ( step > 60 ) && ( step < 180 ) ) {
			current_color.v -= ((double)position * (random(4) + 1)) * sin((step - 60) / 120 * PI); 
		}

		step++;
		
		if ( step == 255 )
		  step = 0;

		delay( random(60) );
	} else if ( current_mode == FLASH ) {
		position++;
		
		if ( position % 32 == 0 ) {
			current_color.v = 255;
		} else if ( position % 16 == 0 ) {
			current_color.v = 1;
		}

		if ( position == 255 )
			position = 0;
	} else if ( current_mode == JUMP ) {
		current_color.h = random(255);
	}

	if ( current_color.v == 0 )
		current_color.v = 1;

	setHSV( current_color );

	delay(speed);
}

void
runTempMode()
{
	if ( ( temp_mode == OFF ) || ( temp_mode == COLOR ) )
		return;

	if ( temp_mode == DIM_TO ) {
		if ( temp_color.v == temp_dim_to ) {
			if ( temp_dim_to == 0 ) {
				setTempMode( OFF );
			} else {
				setTempMode( COLOR );
			}
			return;
		}

		if ( temp_color.v > temp_dim_to )
			temp_color.v--;
		else
			temp_color.v++;
		
	} else if ( temp_mode == DIM ) {
		if ( temp_color.v == 1 ) {
			setTempMode( OFF );
			return;
		}

		if ( temp_color.v > 1 )
			temp_color.v--;
	
	} else if ( temp_mode == FADE_TO ) {
		if ( temp_color.h == temp_fade_to_color.h ) {
			setTempMode( COLOR );
			return;
		}
			
		if ( temp_color.h > temp_fade_to_color.v )
			temp_color.h--;
		else
			temp_color.h++;
	} else if ( temp_mode == FADE ) {
		temp_color.h += temp_step;

		if ( ( temp_color.h == 255 ) && ( temp_step == 1 ) )
			temp_color.h = 0;
		else if ( ( temp_color.h == 0 ) && ( temp_step == -1 ) )
			temp_color.h = 255;
	} else if ( temp_mode == FADE_PALINDROME ) {
		temp_color.h += temp_step;

		if ( temp_color.h == 0 )
			temp_step = 1;
		else if ( temp_color.h == 255 )
			temp_step = -1;
	} else if ( temp_mode == PULSE ) {
		temp_color.v += temp_step;

		if ( temp_color.v == temp_dim_to )
			temp_step = 1;
		else if ( temp_color.v == 255 )
			temp_step = -1;
	} else if ( temp_mode == PULSE_SINE ) {
		temp_position += temp_step;
		
		//temp_color.v = (int)(255.0 * sin(position * ( PI / 2.0 / 255.0 )));

		if ( temp_position == temp_dim_to )
			temp_step = 1;
		else if ( temp_position == 255 )
			temp_step = -1;
	} else if ( temp_mode == FLICKER ) {
		temp_position = random_sin(6);
		temp_color.h = temp_fade_to_color.h + position;
		
		delay( 1 );
		
		temp_position = random_sin(3);
		temp_color.s = temp_fade_to_color.s + position;
		
		delay( 1 );
		
		temp_position = random_sin(5);
		temp_color.v = temp_fade_to_color.v + position;

		delay( random( 5 ) );
	} else if ( temp_mode == FLASH ) {
		temp_position++;
		
		if ( temp_position % 32 == 0 ) {
			temp_color.v = 255;
		} else if ( temp_position % 16 == 0 ) {
			temp_color.v = 1;
		}

		if ( temp_position == 255 )
			temp_position = 0;
	}
	
	if ( temp_color.v == 0 )
		temp_color.v = 1;

	setHSV( temp_color );

	if ( temp_ttl > 0 ) {
		if ( temp_ttl == 1 ) {
			temp_ttl = 0;
			setTempMode( OFF );
		} else
			temp_ttl--;
	}

	delay(temp_speed);
}

int
random( int value )
{
	return (int)((double)rand() / ((double)RAND_MAX + 1) * value)	;
}
int
random_sin( int value )
{
	return (int)(sin( ((double)rand() / ((double)RAND_MAX + 1)) * 2 * PI) * value);
}