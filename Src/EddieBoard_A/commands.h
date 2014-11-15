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
	CMMD MODE    TYPE        H/R        S/G        V/B       SPEED      TTL_0      TTL_1
	           (HSV/RGB)                                                (MiS)      (xMS)
*/

/* The different command modes */
enum COMMAND_TYPE {
	/* Set modes [incoming] */
	SET_MODE      = 0x30,
	SET_TEMP_MODE = 0x40,

	/* Get modes [request for data] */
	GET_SENSORS  = 0xA0,
	GET_SENSOR_0 = 0xA1,
	GET_SENSOR_1 = 0xA2,
	GET_SENSOR_2 = 0xA3,
	GET_SENSOR_3 = 0xA4,
	GET_SENSOR_4 = 0xA5,
	GET_SENSOR_5 = 0xA6,
	GET_SENSOR_6 = 0xA7,
	GET_SENSOR_7 = 0xA8,
	GET_SENSOR_8 = 0xA9,
	GET_SENSOR_9 = 0xAA,

	/* Color modes */
	HSV = 0x01,
	RGB = 0x02
};

/* Statuses for the communication */
enum COMMUNICATION_STATUS {
	OK,
	FAILED,
	IS_SENDING,
	
	UNKNOWN
};

/* Wrapper for the communication handler (nRF24/Bluetooth/Whatever..) */
class CommunicationWrapper
{
  public:
	virtual void sendData( uint8_t *data );
	virtual bool isSending();
	virtual COMMUNICATION_STATUS status();
	
	virtual void waiting();
	
	virtual bool hasData();
	virtual void getData( uint8_t *data );
	
};

/* Handler for incoming & outgoing commands/data */
class CommandHandler
{
  public:
	CommandHandler();
	CommandHandler( CommunicationWrapper* );
	
	bool checkForIncomming();
	
	void incoming( uint8_t *data );
	
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color );
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color, uint8_t speed );
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color, uint8_t speed, int16_t ttl );
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color );
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color, uint8_t speed );
	int sendCommand( COMMAND_TYPE main_command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color, uint8_t speed, int16_t ttl );
	int sendCommand( uint8_t command, uint8_t data_0, uint8_t data_1, uint8_t data_2, uint8_t data_3, uint8_t data_4, uint8_t data_5, uint8_t data_6 );

  private:
    CommunicationWrapper *m_comm;
};


#endif /* COMMANDS_H_ */