/* 
* nrf24_Communication.h
*
* Created: 2014-11-12 23:08:05
* Author: Christopher
*/


#ifndef __NRF24_COMMUNICATION_H__
#define __NRF24_COMMUNICATION_H__

#include "commands.h"

class nrf24_Communication : public CommunicationWrapper
{
//variables
public:
protected:
private:

//functions
public:
	nrf24_Communication();
	~nrf24_Communication();

	virtual void sendData(uint8_t data[8]);

	virtual bool isSending();

	virtual COMMUNICATION_STATUS status();
	
	virtual void waiting();
/*
protected:
private:
	nrf24_Communication( const nrf24_Communication &c );
	nrf24_Communication& operator=( const nrf24_Communication &c );*/

}; //nrf24_Communication

#endif //__NRF24_COMMUNICATION_H__
