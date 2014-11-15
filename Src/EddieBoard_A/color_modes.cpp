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

// Pi is very useful when working with sinus..
#define PI 3.14159265358979323846

/* Current ... */
/* ... modes */
enum COLOR_MODE current_mode = OFF;
enum COLOR_MODE temp_mode = OFF;

/* ... speeds */
uint8_t speed = 50;
uint8_t temp_speed = 50;

/* ... colors */
HsvColor current_color      = HsvColor( 0, 255, 255 );
HsvColor fade_to_color      = HsvColor( 0, 255, 255 );
HsvColor temp_fade_to_color = HsvColor( 0, 255, 255 );
HsvColor temp_color         = HsvColor( 0, 255, 255 );

/* Some other handy data */
uint8_t dim_to = 255;
uint8_t step   = 1;
int position   = 1;
uint8_t temp_dim_to = 255;
uint8_t temp_step   = 1;
int temp_ttl        = 0;
int temp_position   = 1;

/* Set the current mode */
void
setMode( enum COLOR_MODE mode )
{
	// If the current mode (before changing) is OFF (and were setting the mode to OFF)
	if ( ( current_mode == OFF ) && ( mode != OFF ) ) {
		// Reset and turn on the RGB outputs
		pwm_dutycycle( CHAN_RED | CHAN_GREEN | CHAN_BLUE, 0 );
		pwm_enable( CHAN_RED | CHAN_GREEN | CHAN_BLUE );
	}

	// React according to mode (Modes not listed is ignored)
	switch (mode)
	{
		// Turning off?
		case OFF:
			// Turn of the LEDs and disable the outputs
			pwm_dutycycle( CHAN_RED | CHAN_GREEN | CHAN_BLUE, 0 );
			pwm_disable( CHAN_RED | CHAN_GREEN | CHAN_BLUE );
			break;
		// Setting a static color
		case COLOR:
			// Set the LEDS to the current color
			setHSV( current_color );
			break;
		// Blinking?
		case BLINK:
			// Reset the position
			position = 1;
			break;
		// Pulse the light
		case PULSE:
			break;
		// Pulse the light according to a sine curve
		case PULSE_SINE:
			// Reset the position
			position = 1;
			break;
		// Fade between colors (Goes around the color wheel [starts over when the end is reached])
		case FADE:
			// Reset the step between hue
			step = 1;
			// Reset the color info (we keep the set value)
			current_color.h = 0;
			current_color.s = 255;
			break;
		// Fade between colors (Goes backwards when the end is reach and vice versa)
		case FADE_PALINDROME:
			// Reset the step
			step = 1;
			break;
		// Fade to a color(/hue)
		case FADE_TO:
			break;
		// Dim to off
		case DIM:
			break;
		// Dim to a value/brightness
		case DIM_TO:
			break;
		// Let the light Wander 
		case WANDER:
			break;
		// Jump between random colors
		case JUMP:
			// Jump uses a default speed
			setSpeed(60);
			break;
		// Set the light to flicker
		case FLICKER:
			// Reset step and position
			step     = 1;
			position = 1;
			// Set a default speed
			setSpeed(50);
			break;
		// Didn't recognize the mode
		default:
			// Do nothing...
			return;
	}

	// Set the current mode
	current_mode = mode;
}

/* Set the current mode and HSV color */
void
setModeHSV( enum COLOR_MODE mode, HsvColor &color )
{
	// If we're going to fade to a color
	if ( mode == FADE_TO ) {
		// Set the color to fade to
		fade_to_color = color;
	// If we're going to dim to a value
	} else if ( mode == DIM_TO ) {
		// Set the brightness to dim to
		dim_to = color.v;
	// If we're going to pulse
	} else if ( ( mode == PULSE ) || ( mode == PULSE_SINE ) ) {
		// Get the value from the color to use as low value for pulse
		dim_to = color.v;
		// Set the step
		step = 1;

		// Set the color as current color
		setCurrentHSV( color );
	// If we're going to flicker
	} else if ( mode == FLICKER ) {
		// Store the color to flicker at (we deviate +/- on all channels from this)
		fade_to_color = color;
		// Set the default speed
		setSpeed(100);

		// Set the color as current color
		setCurrentHSV( color );
	// Anything else...
	} else {
		// Set the color as current color
		setCurrentHSV( color );
	}

	// Set the mode
	setMode( mode );
}	

/* Set the current mode and RGB color */
void
setModeRGB( enum COLOR_MODE mode, RgbColor &color )
{
	// Convert to HSV
	HsvColor hsv = color.ToHSV();
	// Set the mode
	setModeHSV( mode, hsv );
}

/* Set the temporary mode (with TTL[=Time To Live, how long the temp mode should last, 0 = until changed again]) */
void
setTempMode( enum COLOR_MODE mode, int16_t ttl )
{
	// If the current mode is OFF
	if ( ( current_mode == OFF ) && ( mode != OFF ) ) {
		// Reset the channels and enable them
		pwm_dutycycle( CHAN_RED | CHAN_GREEN | CHAN_BLUE, 0 );
		pwm_enable( CHAN_RED | CHAN_GREEN | CHAN_BLUE );
	}

	// Check the mode
	switch (mode)
	{
		// Turn temp mode off? (Go back to normal mode)
		case OFF:
			break;
		// Set a static color?
		case COLOR:
			// Set the color as current color
			setHSV( temp_color );
			break;
		// Should we blink?
		case BLINK:
			// Reset position
			temp_position = 1;
			break;
		// Should we pulse?
		case PULSE:
			break;
		// Should we pulse according to a sine curve?
		case PULSE_SINE:
			// Reset the position
			temp_position = 1;
			break;
		// Should we fade between colors? (Along the hue color wheel, starts from the beginning when the end is reached)
		case FADE:
			// Reset the step
			temp_step = 1;
			break;
		// Should we fade between colors? (Goes backwards when the end is reached and vice versa)
		case FADE_PALINDROME:
			// Reset the step
			temp_step = 1;
			break;
		// Should we fade to color?
		case FADE_TO:
			break;
		// Should we dim the light to off?
		case DIM:
			break;
		// Should we dim to a certain brightness?
		case DIM_TO:
			break;
		// Should the light "wander"?
		case WANDER:
			break;
		// Should we jump randomly between colors?
		case JUMP:
			break;
		// Should the light flicker
		case FLICKER:
			// Reset the temp and position
			temp_step     = 1;
			temp_position = 1;
			// Set a default speed
			setTempSpeed(50);
			break;
		// None of the above
		default:
			// Do nothing
			return;
	}

	// Store TTL
	temp_ttl = ttl;
	// Store the mode
	temp_mode = mode;
}
/* Set the temp mode (without TTL[=0]) */
void
setTempMode( enum COLOR_MODE mode )
{
	setTempMode( mode, 0 );
}

/* Set the temp mode with a HSV color and TTL */
void
setTempModeHSV( enum COLOR_MODE mode, HsvColor &color, int16_t ttl )
{
	// If we should fade to a color
	if ( mode == FADE_TO ) {
		// Store the color to fade to
		temp_fade_to_color = color;
	// If we should dim to a brightness
	} else if ( mode == DIM_TO ) {
		// Get the value of the color to dim to
		temp_dim_to = color.v;
	// If we should pulsate the light
	} else if ( ( mode == PULSE ) || ( mode == PULSE_SINE ) ) {
		// Get the low limit from the brightness of the incoming color
		temp_dim_to = color.v;
		// Reset the steps
		temp_step = 1;
		// Set a default speed
		setTempSpeed(2);

		// Set the current temp color to be the incoming color
		setTempHSV( color );
	// If we should flicker the light
	} else if ( mode == FLICKER ) {
		// Store the color to flicker at (we deviate +/- on all channels from this)
		temp_fade_to_color = color;

		// Set the current temp color to be the incoming color
		setTempHSV( color );
	// Everything else
	} else {
		// Set the current temp color to be the incoming color
		setTempHSV( color );
	}

	// Set the mode with TTL
	setTempMode( mode, ttl );
}

/* Set the temp mode with a HSV color and no TTL[=0] */
void
setTempModeHSV( enum COLOR_MODE mode, HsvColor &color )
{
	setTempModeHSV( mode, color, 0 );
}

/* Set the temp mode with a RGB color and TTL */
void
setTempModeRGB( enum COLOR_MODE mode, RgbColor &color, int16_t ttl )
{
	// Convert to HSV
	HsvColor hsv = color.ToHSV();
	setTempModeHSV( mode, hsv, ttl );
}
/* Set the temp mode with a HSV color and no TTL[=0] */
void
setTempModeRGB( enum COLOR_MODE mode, RgbColor &color )
{
	// Convert to HSV
	HsvColor hsv = color.ToHSV();
	setTempModeHSV( mode, hsv, 0 );
}

/* Set the current color using HSV */
void
setCurrentHSV( HsvColor color )
{
	current_color = color;
}
/* Set the temporary color using HSV */
void
setTempHSV( HsvColor color )
{
	temp_color = color;
}

/* Set the current brightness */
void
setBrightness( uint8_t value )
{
	current_color.v = value;
}
/* Set the current brightness (alias for setBrightness) */
void
setValue( uint8_t value )
{
	setBrightness(value);
}

/* Set the current speed */
void
setSpeed( uint8_t s )
{
	speed = s;
}
/* Set the temporary speed */
void
setTempSpeed( uint8_t s )
{
	temp_speed = s;
}

/************************************************************************/

/* Run the current mode */
void
runMode()
{
	// If there's a temporary mode set
	if ( temp_mode != OFF ) {
		// Run the temporary mode instead
		runTempMode();
		return;
	}

	// If we're currently "turned off" or have a static color (i.e. Nothing to do!)
	if ( ( current_mode == OFF ) || ( current_mode == COLOR ) )
		return;

	// If we're dimming to brightness
	if ( current_mode == DIM_TO ) {
		// If we reached the brightness we wanted to dim to
		if ( current_color.v == dim_to ) {
			// If we dimmed to 0 (No brightness)
			if ( dim_to == 0 ) {
				// Set the mode to off
				setMode( OFF );
			// Otherwise .. (The light is still shining!)
			} else {
				// Set the current color as static color
				setMode( COLOR );
			}
			return;
		}

		// If we're dimming to a lower value
		if ( current_color.v > dim_to )
			// Go down
			current_color.v--;
		else
			// Go up
			current_color.v++;

	// If we're dimming to off
	} else if ( current_mode == DIM ) {
		// If the current value/brightness is 1
		if ( current_color.v == 1 ) {
			// Turn the light off (the next step wouldn't have been 0 anyway..)
			setMode( OFF );
			return;
		}

		// Lower the brightness one step
		current_color.v--;
	
	// If we're fading to a color
	} else if ( current_mode == FADE_TO ) {
		// If we reached the color
		if ( current_color.h == fade_to_color.h ) {
			// Set the current color as a static color
			setMode( COLOR );
			return;
		}

		// If we're fading to a "lower" hue
		if ( current_color.h > fade_to_color.h )
			// Reduce the hue with one step
			current_color.h--;
		else
			// Increase the hue with one step
			current_color.h++;

	// If we're fading between the colors (going around the hue wheel)
	} else if ( current_mode == FADE ) {
		// Move along the hue scale according to the step
		current_color.h += step;

		// If we reached the end of the scale and we're moving forward
		if ( ( current_color.h == 255 ) && ( step == 1 ) )
			// Reset the hue to 0
			current_color.h = 0;
		// If we reached the end of the scale and we're moving backward
		else if ( ( current_color.h == 0 ) && ( step == -1 ) )
			// Set the hue to max
			current_color.h = 255;

	// If we're fading between the colors (going around the hue wheel back and forth)
	} else if ( current_mode == FADE_PALINDROME ) {
		// Move along the hue scale according to the step
		current_color.h += step;

		// If we reached the beginning
		if ( current_color.h == 0 )
			// Change the direction
			step = 1;
		// If we reached the end
		else if ( current_color.h == 255 )
			// Change the direction
			step = -1;

	// If we're pulsating the light
	} else if ( current_mode == PULSE ) {
		// Change the value according to the current step
		current_color.v += step;

		// If we reached the lowest value that we should go to
		if ( current_color.v == dim_to )
			// Go forward
			step = 1;
		// If we reached the highest value
		else if ( current_color.v == 255 )
			// Go backwards
			step = -1;

	// If we're pulsating the light according to a sine curve (This was just a test, but it can be fun!?)
	} else if ( current_mode == PULSE_SINE ) {
		// Change the position according to the current step
		position += step;
		
		// Calculate the value according to the position on the sine curve
		current_color.v = (int)(255.0 * sin(position * ( PI / 2.0 / 255.0 )));

		// If we reached the lowest value that we should go to
		if ( position == dim_to )
			// Go forward
			step = 1;
		// If we reached the highest value
		else if ( position == 255 )
			// Go backwards
			step = -1;

	// If we're flickering the light
	} else if ( current_mode == FLICKER ) {
		// Calculate a change for hue (from "default") (can go +-4)
		position = (int)((double)random_sin(4) * sin((double)step / 255 * 2 * PI));
		current_color.h = fade_to_color.h + position;
		
	//	delay( 2 );

		// Calculate a change for saturation (from "default") (can go +-2)
		position = (int)((double)random_sin(2) * sin((double)step / 255 * 2 * PI));
		current_color.s = fade_to_color.s + position;
		
	//	delay( 2 );

		// Calculate a change for value (from "default") (can go +-5)
		position = (int)((double)random_sin(5) * sin((double)step / 255 * 2 * PI));
		current_color.v = fade_to_color.v + position;

		// If we're between the 60th and 180th steps
		if ( ( step > 60 ) && ( step < 180 ) ) {
			// Dim the brightness even more
			current_color.v -= ((double)position * (random(5) + 1)) * sin((step - 60) / 120 * PI); 
		}

		// Increase the current step
		step++;
		
		// If we reached the maximum steps
		if ( step == 255 )
			// Reset
			step = 0;

		// Delay a random amount (up to 90ms)
		delay( random(90) );

	// If we're flashing/blinking with the light (on and off)
	} else if ( current_mode == FLASH ) {
		// Increase the position
		position++;

		// Every 32nd step, turn on
		if ( position % 32 == 0 ) {
			current_color.v = 255;
		// Every 16th step (between the 32), turn off
		} else if ( position % 16 == 0 ) {
			current_color.v = 1;	// Don't set value (or saturation) to 0, it will reset the hue (and saturation)
		}

		// If we reached the max
		if ( position == 255 )
			// Start over
			position = 0;
	
	// If we're jumping between colors
	} else if ( current_mode == JUMP ) {
		// Set a random hue
		current_color.h = random(255);
	}

	// Just to be on the safe side, make sure the value isn't 0
	if ( current_color.v == 0 )
		current_color.v = 1;

	// Set the color
	setHSV( current_color );

	// Delay
	delay(speed);
}

/* Run the temporary mode */
void
runTempMode()
{
	// If the temporary mode is turned off or showing a static color
	if ( ( temp_mode == OFF ) || ( temp_mode == COLOR ) )
		// Do nothing
		return;

	// If we're dimming to brightness
	if ( temp_mode == DIM_TO ) {
		// If we reached the brightness we wanted to dim to
		if ( temp_color.v == temp_dim_to ) {
			// If we dimmed to 0 (No brightness)
			if ( temp_dim_to == 0 ) {
				// Set the mode to off
				setTempMode( OFF );
			// Otherwise .. (The light is still shining!)
			} else {
				// Set the current color as static color
				setTempMode( COLOR );
			}
			return;
		}

		// If we're dimming to a lower value
		if ( temp_color.v > temp_dim_to )
			// Go down
			temp_color.v--;
		else
			// Go up
			temp_color.v++;

	// If we're dimming to off
	} else if ( temp_mode == DIM ) {
		// If the current value/brightness is 1
		if ( temp_color.v == 1 ) {
			// Turn the light off (the next step wouldn't have been 0 anyway..)
			setTempMode( OFF );
			return;
		}

		// Lower the brightness one step
		temp_color.v--;
	
	// If we're fading to a color
	} else if ( temp_mode == FADE_TO ) {
		// If we reached the color
		if ( temp_color.h == temp_fade_to_color.h ) {
			// Set the current color as a static color
			setTempMode( COLOR );
			return;
		}

		// If we're fading to a "lower" hue
		if ( temp_color.h > temp_fade_to_color.v )
			// Reduce the hue with one step
			temp_color.h--;
		else
			// Increase the hue with one step
			temp_color.h++;

	// If we're fading between the colors (going around the hue wheel)
	} else if ( temp_mode == FADE ) {
		// Move along the hue scale according to the step
		temp_color.h += temp_step;

		// If we reached the end of the scale and we're moving forward
		if ( ( temp_color.h == 255 ) && ( temp_step == 1 ) )
			// Reset the hue to 0
			temp_color.h = 0;
		// If we reached the end of the scale and we're moving backward
		else if ( ( temp_color.h == 0 ) && ( temp_step == -1 ) )
			// Set the hue to max
			temp_color.h = 255;

	// If we're fading between the colors (going around the hue wheel back and forth)
	} else if ( temp_mode == FADE_PALINDROME ) {
		// Move along the hue scale according to the step
		temp_color.h += temp_step;

		// If we reached the beginning
		if ( temp_color.h == 0 )
			// Change the direction
			temp_step = 1;
		// If we reached the end
		else if ( temp_color.h == 255 )
			// Change the direction
			temp_step = -1;

	// If we're pulsating the light
	} else if ( temp_mode == PULSE ) {
		// Change the value according to the current step
		temp_color.v += temp_step;

		// If we reached the lowest value that we should go to
		if ( temp_color.v == temp_dim_to )
			// Go forward
			temp_step = 1;
		// If we reached the highest value
		else if ( temp_color.v == 255 )
			// Go backwards
			temp_step = -1;

	// If we're pulsating the light according to a sine curve (This was just a test, but it can be fun!?)
	} else if ( temp_mode == PULSE_SINE ) {
		// Change the position according to the current step
		temp_position += temp_step;

		// Calculate the value according to the position on the sine curve
		temp_color.v = (int)(255.0 * sin(position * ( PI / 2.0 / 255.0 )));

		// If we reached the lowest value that we should go to
		if ( temp_position == temp_dim_to )
			// Go forward
			temp_step = 1;
		// If we reached the highest value
		else if ( temp_position == 255 )
			// Go backwards
			temp_step = -1;

	// If we're flickering the light (Currentely differs from the normal flicker mode)
	} else if ( temp_mode == FLICKER ) {
		// Calculate a change for hue (from "default") (can go +-6)
		temp_position = random_sin(6);
		temp_color.h = temp_fade_to_color.h + position;
		
		//delay( 1 );

		// Calculate a change for saturation (from "default") (can go +-3)
		temp_position = random_sin(3);
		temp_color.s = temp_fade_to_color.s + position;
		
		//delay( 1 );
		
		// Calculate a change for value (from "default") (can go +-5)
		temp_position = random_sin(5);
		temp_color.v = temp_fade_to_color.v + position;

		// Delay a random amount (up to 90ms)
		delay( random(90) );

	// If we're flashing/blinking with the light (on and off)
	} else if ( temp_mode == FLASH ) {
		// Increase the position
		temp_position++;

		// Every 32nd step, turn on
		if ( temp_position % 32 == 0 ) {
			temp_color.v = 255;
		// Every 16th step (between the 32), turn off
		} else if ( temp_position % 16 == 0 ) {
			temp_color.v = 1;	// Don't set value (or saturation) to 0, it will reset the hue (and saturation)
		}

		// If we reached the max
		if ( temp_position == 255 )
			// Start over
			temp_position = 0;

	// If we're jumping between colors
	} else if ( temp_mode == JUMP ) {
		// Set a random hue
		temp_color.h = random(255);
	}

	// Just to be on the safe side, make sure the value isn't 0
	if ( temp_color.v == 0 )
		temp_color.v = 1;

	// Set the color
	setHSV( temp_color );

	// If TTL is set
	if ( temp_ttl > 0 ) {
		// If we reached the end of life
		if ( temp_ttl == 1 ) {
			// Turn TTL off
			temp_ttl = 0;
			// Turn off temp mode
			setTempMode( OFF );
		// Still living
		} else
			// But counting down...
			temp_ttl--;
	}

	// Delay
	delay(temp_speed);
}

/* Get a random integer with 'value' as max */
int
random( int value )
{
	return (int)((double)rand() / ((double)RAND_MAX + 1) * value)	;
}
/* Get a random integer with 'value' as max, but throw in a sine! */
int
random_sin( int value )
{
	return (int)(sin( ((double)rand() / ((double)RAND_MAX + 1)) * 2 * PI) * value);
}