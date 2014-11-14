/*
 * commands.h
 *
 * Created: 2014-11-10 01:06:21
 *  Author: Christopher
 */ 

#include <stdint.h>

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <stdbool.h>
#include <stdint.h>

#include "HSV.h"
#include "color_modes.h"

/*
 Data structure:
 
     COMMAND                                    DATA
    |       |  |                                                                         |
    0000 0000  0000 0000  0000 0000  0000 0000  0000 0000  0000 0000  0000 0000  0000 0000
	|  | |  |  |       |  |       |  |       |  |       |  |       |  |       |  |       |
	CMMD MODE    TYPE        H/R        S/G        V/B       SPEED       TTL        TTL
	           (HSV/RGB)                                                (MiS)      (xMS)
*/

enum COMMAND_TYPE {
	SET_MODE      = 0x30,
	SET_TEMP_MODE = 0x40,
	
	HSV = 0x01,
	RGB = 0x02
};

enum COMMUNICATION_STATUS {
	OK,
	FAILED,
	IS_SENDING,
	
	UNKNOWN
};


class CommunicationWrapper
{
  public:
	virtual void sendData(  uint8_t data[8] );
	virtual bool isSending();
	virtual COMMUNICATION_STATUS status();
	
	virtual void waiting();
	
};

class CommandHandler
{
  public:
	CommandHandler();
	//CommandHandler( void (*send)(uint8_t[8]) );
	CommandHandler( CommunicationWrapper* );
	
	void incoming( uint8_t data[8] );
	
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color );
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color, uint8_t speed );
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color, uint8_t speed, uint8_t ttl );
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color );
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color, uint8_t speed );
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color, uint8_t speed, uint8_t ttl );
	int sendCommand( uint8_t command, uint8_t data_0, uint8_t data_1, uint8_t data_2, uint8_t data_3, uint8_t data_4, uint8_t data_5, uint8_t data_6 );
	
	//void (*sendData)(uint8_t[8]);

  private:
    CommunicationWrapper *m_comm;
};


#endif /* COMMANDS_H_ */