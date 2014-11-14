/*
 * commands.cpp
 *
 * Created: 2014-11-10 01:06:37
 *  Author: Christopher
 */ 

#include "commands.h"
#include "color_modes.h"


CommandHandler::CommandHandler()
{
	
}
/*CommandHandler::CommandHandler( void (*send)(uint8_t[8]) ) : CommandHandler()
{
	this->sendData = send;
}*/
//CommandHandler::CommandHandler( CommunicationWrapper *commWrapper ) : CommandHandler()
CommandHandler::CommandHandler( CommunicationWrapper *commWrapper ) : CommandHandler()
{
	m_comm = commWrapper;
}

void
CommandHandler::incoming( uint8_t data[8] )
{
	// Check so it isn't "test data"
	if ( ( data[0] == 0x00 ) && ( data[1] == 0x22 ) ) {
		return;
	}

	uint8_t first  = data[0] & 0xF0;
	uint8_t second = data[0] & 0x0F;

	if ( first == SET_MODE ) {
		if ( data[1] == HSV ) {
			HsvColor color = HsvColor( data[2], data[3], data[4] );
			setModeHSV( (COLOR_MODE)second, color );
			
			if ( data[5] != 0 )
				setSpeed( data[5] );
		} else if ( data[1] == RGB ) {
			RgbColor color = RgbColor( data[2], data[3], data[4] );
			setModeRGB( (COLOR_MODE)second, color );
			
			if ( data[5] != 0 )
				setSpeed( data[5] );
		} else {
			setMode( (COLOR_MODE)second );
		}
		
	} else if ( first == SET_TEMP_MODE ) {
		if ( data[1] == HSV ) {	
			HsvColor color = HsvColor( data[2], data[3], data[4] );
			setTempModeHSV( (COLOR_MODE)second, color, data[6] );

			if ( data[5] != 0 )
				setTempSpeed( data[5] );
		} else if ( data[1] == RGB ) {
			RgbColor color = RgbColor( data[2], data[3], data[4] );
			setTempModeRGB( (COLOR_MODE)second, color, data[6] );
			
			if ( data[5] != 0 )
				setTempSpeed( data[5] );
		} else {
			setTempMode( (COLOR_MODE)second );
		}
	}
}

int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color )
{
	return this->sendCommand( command, sub_command, mode, color, 0, 0 );
}
int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color, uint8_t speed )
{
	return this->sendCommand( command, sub_command, mode, color, speed, 0 );
}
int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color, uint8_t speed, uint8_t ttl )
{
	return this->sendCommand( command | mode, sub_command, color.h, color.s, color.v, speed, ttl, 0 );
}

int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color )
{
	return this->sendCommand( command, sub_command, mode, color, 0, 0 );
}
int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color, uint8_t speed )
{
	return this->sendCommand( command, sub_command, mode, color, speed, 0 );
}
int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color, uint8_t speed, uint8_t ttl )
{
	return this->sendCommand( command | mode, sub_command, color.r, color.g, color.b, speed, ttl, 0 );
}

int
CommandHandler::sendCommand( uint8_t command, uint8_t data_0, uint8_t data_1, uint8_t data_2, uint8_t data_3, uint8_t data_4, uint8_t data_5, uint8_t data_6 )
{
	uint8_t data[8] = { command, data_0, data_1, data_2, data_3, data_4, data_5, data_6 };

	this->m_comm->sendData( data );
	
	while ( this->m_comm->isSending() ) {
		this->m_comm->waiting();
	}
	
	return this->m_comm->status();
}