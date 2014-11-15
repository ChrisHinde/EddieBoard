/*
 * EddiiBoard_A.cpp
 *
 * Created: 2014-11-08 05:32:23
 *  Author: Christopher
 */ 

#define F_CPU 1600000UL // 16 MHz clock speed

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>

#include "HSV.h"
#include "pwm.h"
#include "color_modes.h"
#include "commands.h"
#include "nrf24/nrf24_Communication.h"

#include "nrf24/nRF24L01.h"
#include "nrf24/nrf24.h"

/* ------------------------------------------------------------------------- */
uint8_t temp;
uint8_t q = 0;
uint8_t data_array[8];
uint8_t rx_address[5] = {0x00,0x00,0x00,0x01,0x02}; // <-- Adress of this unit    NOTE: You need/should to change this for each unit
uint8_t tx_address[5] = {0x00,0x00,0x00,0x00,0x01};

HsvColor color = HsvColor(	);		// General purpose variable for holding a HSV color
unsigned int time_pos = 0;
CommandHandler cmdHandle;			// Handles incoming and outgoing commands/traffic
nrf24_Communication nrf24comm;		// Wrapper for communication via nrf24L01
/* ------------------------------------------------------------------------- */

/***
 Personal note for programming setup
 G = Grön
 B = Blå
 g = Gul
 
 GB**
 *g**

***/

void init()
{
	// Init PWM (set up outputs etc)
	pwm_initialize(0);

	// Enable the RGB channels
	pwm_enable(CHAN_RED|CHAN_GREEN|CHAN_BLUE);

	// Set the LED to "off"
	pwm_dutycycle(CHAN_RED | CHAN_GREEN | CHAN_BLUE, 0);
	
  /** "Wake up" run/Status check **/

	// Turn on Red at full and wait 200ms
	pwm_dutycycle(CHAN_RED, 255);
	_delay_ms(200);

	// Turn off Red and turn on Green at full and wait 200ms
	pwm_dutycycle(CHAN_RED, 0);
	pwm_dutycycle(CHAN_GREEN, 255);
	_delay_ms(200);

	// Turn off Green and turn on Blue at full and wait 200ms
	pwm_dutycycle(CHAN_GREEN, 0);
	pwm_dutycycle(CHAN_BLUE, 255);
	_delay_ms(200);

	// Turn off Blue and wait 100ms
	pwm_dutycycle(CHAN_BLUE, 0);
	_delay_ms(100);

  /** Initialize radio communication **/

	// Initializes hardware pins
	nrf24_init();

	// RF channel: #2 , payload length: 8
	nrf24_config(2,8);
	
	// Set the module's own address
	nrf24_rx_address(rx_address);

	// Set the transmit address
	nrf24_tx_address(tx_address);

	// Set test data
	data_array[0] = 0x00;
	data_array[1] = 0x22;
	data_array[2] = 0x44;
	data_array[3] = 0x88;
	data_array[4] = 0xAA;
	data_array[5] = 0xCC;
	data_array[6] = 0xDD;
	data_array[8] = 0xFF;

	// Turn on Blue (to indicate state)
	pwm_dutycycle(CHAN_BLUE, 255);

	// Send the data (Automatically goes to TX mode)
	nrf24_send(data_array);

	// Turn off Blue and turn on Green & Red (= Yellow)
	pwm_dutycycle(CHAN_BLUE, 0);
	pwm_dutycycle(CHAN_GREEN | CHAN_RED, 255);

	// Wait for transmission to end
	while(nrf24_isSending());
	//_delay_ms(500);

	// Turn off light and wait 100ms
	pwm_dutycycle(CHAN_GREEN | CHAN_RED, 0);
	_delay_ms(100);

	// Get the status of the transmission attempt
	temp = nrf24_lastMessageStatus();

	// Indicate the result
	if ( temp == NRF24_TRANSMISSON_OK ) {
		pwm_dutycycle(CHAN_GREEN, 255);
	} else if ( temp == NRF24_MESSAGE_LOST) {
		pwm_dutycycle(CHAN_RED , 255);
		_delay_ms(1000);
	} else if ( nrf24_isSending() ) {
		pwm_dutycycle(CHAN_BLUE, 128);
		pwm_dutycycle(CHAN_RED , 255);
	}
	_delay_ms(500);

	// Go back to RX mode
	nrf24_powerUpRx();

	_delay_ms(500);
	
	// Set the mode to OFF (turn off LEDs, and disables PWM [i.e. actually turn the LEDs off completely])
	setMode( OFF );
}


int
main(void)
{
	// Run the init sequence
	init();

	// Initiate the communication handling
	nrf24comm = nrf24_Communication();
	cmdHandle = CommandHandler ( &nrf24comm );

	// Set the default/starter color & mode (15 == orange [#ff5900])
	color = HsvColor( 15, 250, 250 );
	setModeHSV( FLICKER, color );

	// Reset the hue value
	color.h = 0;

	// Do the stuff ...
	while(1)
	{
		// Check for incoming data (also reacts to it)
		cmdHandle.checkForIncomming();

	/*	if ( time_pos == 1000 ) {
			setTempModeHSV( PULSE, color );
			
			cmdHandle.sendCommand( SET_TEMP_MODE, HSV, FLASH, color, 0, 200 );
			
			color.h += 5;
			
			if ( color.h >= 255 )
				color.h = 0;

			time_pos = 0;
			while ( time_pos < 400 ) {
				runTempMode();
				time_pos++;
			}

			//_delay_ms(10);
			setTempMode(OFF);
			//_delay_ms(200);
			time_pos = 0;
		}  */

		time_pos++;
		
		if ( time_pos == 40960 )
			time_pos = 0;
		
		// "Run the mode"
		runMode();
	}
}
