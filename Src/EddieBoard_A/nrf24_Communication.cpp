/* 
* nrf24_Communication.cpp
*
* Created: 2014-11-12 23:08:05
* Author: Christopher
*/

#include "nRF24L01.h"
#include "nrf24.h"


#include "nrf24_Communication.h"

// default constructor
nrf24_Communication::nrf24_Communication()
{
} //nrf24_Communication

// default destructor
nrf24_Communication::~nrf24_Communication()
{
} //~nrf24_Communication

void
nrf24_Communication::sendData(uint8_t data[8])
{
	nrf24_send( data );
}

bool
nrf24_Communication::isSending()
{
	return nrf24_isSending();
}

COMMUNICATION_STATUS
nrf24_Communication::status()
{
	if ( nrf24_isSending() )
		return IS_SENDING;
	
	uint8_t stat = nrf24_lastMessageStatus();
	
	if ( stat == NRF24_TRANSMISSON_OK )
		return OK;
	else if ( stat == NRF24_MESSAGE_LOST )
		return FAILED;

	return UNKNOWN;
}

void
nrf24_Communication::waiting()
{
	runMode();
}