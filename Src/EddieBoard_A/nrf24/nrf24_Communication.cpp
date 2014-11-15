/* 
* nrf24_Communication.cpp
*
* Created: 2014-11-12 23:08:05
* Author: Christopher
*/

#include "nRF24L01.h"
#include "nrf24.h"


#include "nrf24_Communication.h"

/* default constructor */
nrf24_Communication::nrf24_Communication()
{
}

/* default destructor */
nrf24_Communication::~nrf24_Communication()
{
}

/* Send data */
void
nrf24_Communication::sendData(uint8_t data[8])
{
	nrf24_send( data );
}

/* Check if there's a transmission going */
bool
nrf24_Communication::isSending()
{
	return nrf24_isSending();
}

/* Get the status for the communication (also checks if we're currentely sending) */
COMMUNICATION_STATUS
nrf24_Communication::status()
{
	// Check if there's a transmission going
	if ( nrf24_isSending() )
		return IS_SENDING;
	
	// Get the status from the nrf42-module 
	uint8_t stat = nrf24_lastMessageStatus();
	
	// Translate it to our own status code
	if ( stat == NRF24_TRANSMISSON_OK )
		return OK;
	else if ( stat == NRF24_MESSAGE_LOST )
		return FAILED;

	return UNKNOWN;
}

/* What to do while we are transmitting/waiting */
void
nrf24_Communication::waiting()
{
	// Run the mode code
	runMode();
}

/* Check if there's data available */
bool
nrf24_Communication::hasData()
{
	return nrf24_dataReady();
}

/* Get the available data */
void
nrf24_Communication::getData( uint8_t *data )
{
	nrf24_getData( data );
}