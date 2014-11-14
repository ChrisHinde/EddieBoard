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
uint8_t rx_address[5] = {0x00,0x00,0x00,0x01,0x02}; // <-- Adress of this unit
uint8_t tx_address[5] = {0x00,0x00,0x00,0x00,0x01};
/* ------------------------------------------------------------------------- */

/***
 G = Grön
 B = Blå
 g = Gul
 
 GB**
 *g**

***/

void init()
{
	pwm_initialize(0);
	
	pwm_enable(CHAN_RED|CHAN_GREEN|CHAN_BLUE);
	
	pwm_dutycycle(CHAN_RED | CHAN_GREEN | CHAN_BLUE, 0);
	
	_delay_ms(250);
	
	pwm_dutycycle(CHAN_RED, 255);
	_delay_ms(250);
	
	pwm_dutycycle(CHAN_RED, 0);
	pwm_dutycycle(CHAN_GREEN, 255);
	_delay_ms(250);
	
	pwm_dutycycle(CHAN_GREEN, 0);
	pwm_dutycycle(CHAN_BLUE, 255);
	_delay_ms(250);
	 
	pwm_dutycycle(CHAN_BLUE, 0);
	_delay_ms(100);


	/* initializes hardware pins */
	nrf24_init();

	/* RF channel: #2 , payload length: 4 */
	nrf24_config(2,8);
	
	/* Set the module's own address */
	nrf24_rx_address(rx_address);

	/* Set the transmit address */
	nrf24_tx_address(tx_address);

	data_array[0] = 0x00;
	data_array[1] = 0x22;
	data_array[2] = 0x44;
	data_array[3] = 0x88;
	data_array[4] = 0xAA;
	data_array[5] = 0xCC;
	data_array[6] = 0xDD;
	data_array[8] = 0xFF;

	pwm_dutycycle(CHAN_BLUE, 255);

	/* Automatically goes to TX mode */
	nrf24_send(data_array);


	pwm_dutycycle(CHAN_BLUE, 0);
	pwm_dutycycle(CHAN_GREEN | CHAN_RED, 255);

	/* Wait for transmission to end */
	//while(nrf24_isSending());
	_delay_ms(500);

	/* Make analysis on last transmission attempt */
	temp = nrf24_lastMessageStatus();
	
	pwm_dutycycle(CHAN_GREEN | CHAN_RED, 0);
	_delay_ms(100);

	if ( temp == NRF24_TRANSMISSON_OK ) {
		pwm_dutycycle(CHAN_GREEN, 255);
	} else if ( temp == NRF24_MESSAGE_LOST) {
		pwm_dutycycle(CHAN_RED , 255);
		_delay_ms(2000);
	} else if ( nrf24_isSending() ) {
		pwm_dutycycle(CHAN_BLUE, 128);
		pwm_dutycycle(CHAN_RED , 255);
	}
	_delay_ms(500);

	/* Go back to RX mode ... */
	nrf24_powerUpRx();

	_delay_ms( 1000 );
	
	//pwm_dutycycle(CHAN_RED | CHAN_GREEN | CHAN_BLUE, 0);
	setMode( OFF );

	/*pwm_disable(CHAN_RED | CHAN_GREEN | CHAN_BLUE);
	
	_delay_ms( 500 );*/

	//pwm_enable(CHAN_RED | CHAN_GREEN | CHAN_BLUE);

	_delay_ms( 500 );
}


HsvColor color = HsvColor(	);
unsigned int time_pos = 0;
CommandHandler cmdHandle;


int
main(void)
{
	init();

	nrf24_Communication nrf24comm = nrf24_Communication();
	cmdHandle = CommandHandler ( &nrf24comm );

	//setMode( FADE );
	color = HsvColor( 18, 250, 250 );
	setModeHSV( FLICKER, color );
	//_delay_ms(500);
	
	color.h = 0;
	
	while(1)
	{
		/*if ( nrf24_dataReady() && ( time_pos ==1800 ) ) {
			color.s = 255;
			color.h = 180;
			color.v = 42;
			
			nrf24_getData( data_array );

			setTempModeHSV( PULSE, color );
			time_pos = 0;
			while ( time_pos < 600 ) {
				runTempMode();
				time_pos++;
			}

			nrf24_send( data_array );

			color.h = 120;
			setTempModeHSV( PULSE, color );

			time_pos = 0;
			while ( time_pos < 600 ) {
				runTempMode();
				time_pos++;
			}
			_delay_ms(10);
			setTempMode(OFF);
			_delay_ms(200);
			time_pos = 0;
		}*/
		if ( nrf24_dataReady() ) {
			nrf24_getData( data_array );
			cmdHandle.incoming( data_array );
		}

	/*	if ( time_pos == 1000 ) {
			setTempModeHSV( PULSE, color );
			
			cmdHandle.sendCommand( SET_TEMP_MODE, HSV, FLASH, color, 0, 200 );
			
			color.h += 5;
			
			if ( color.h >= 255 )
				color.h = 0;

			/* // Fill the data buffer 
			data_array[0] = 0x00;
			data_array[1] = 0xAA;
			data_array[2] = 0x55;
			data_array[3] = q++;

			// Automatically goes to TX mode 
			nrf24_send(data_array);        
        
			// Wait for transmission to end 
			while(nrf24_isSending());

			// Make analysis on last transmission attempt 
			temp = nrf24_lastMessageStatus();

			if (temp == NRF24_TRANSMISSON_OK) {
				color.h = 120; 
				setTempModeHSV( PULSE, color );
			} else if (temp == NRF24_MESSAGE_LOST) {
				color.h = 0;
				setTempModeHSV( PULSE, color );
			}* /

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

		/*if ( time_pos == 256 ) {
			//setSpeed( 2 );
			color.h = 90;
			color.v = 255;
			setTempModeHSV( COLOR, color );
		} else if ( time_pos == 512) {
			setTempMode( OFF );
		} else if ( time_pos == 10240 ) {
			setSpeed( 2 );
			color.h = 0;
			color.s = 255;
			color.v = 42;
			setTempModeHSV( PULSE, color );
		} else if ( time_pos == 2048 ) {
			setSpeed( 25 );
			setTempMode( OFF );
		} else if ( time_pos == 8192 )
			time_pos = 0;*/

		runMode();

		time_pos++;
		
		if ( time_pos == 40960 )
			time_pos = 0;
	}
}