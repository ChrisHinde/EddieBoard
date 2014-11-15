/*
 * commands.cpp
 *
 * Created: 2014-11-10 01:06:37
 *  Author: Christopher
 */ 

#include "commands.h"
#include "color_modes.h"


/* Default constructor for the communications handler */
CommandHandler::CommandHandler()
{
}

/* Constructor for the communications handler - Sets the communication wrapper */
CommandHandler::CommandHandler( CommunicationWrapper *commWrapper ) : CommandHandler()
{
	m_comm = commWrapper;
}

/* Checks for incoming data, and handles it if there's any! */
bool
CommandHandler::checkForIncomming()
{
	// Check for data
	if ( m_comm->hasData() ) {
		uint8_t *data = 0;

		// Retrieve the data
		m_comm->getData( data );

		// Handle the data
		this->incoming( data );

		// We got some data
		return true;
	} else {
		// We didn't get some data
		return false;
	}
}

/* Handle incoming data */
void
CommandHandler::incoming( uint8_t data[8] )
{
	// Check so it isn't "test data"
	if ( ( data[0] == 0x00 ) && ( data[1] == 0x22 ) ) {
		return;
	}

	// Get the parts of the MODE byte
	uint8_t first  = data[0] & 0xF0;
	uint8_t second = data[0] & 0x0F;

	// Check the MODE byte, is it SET_MODE?
	if ( first == SET_MODE ) {
		// Check the TYPE byte to see if it's HSV-data
		if ( data[1] == HSV ) {
			// Create the color from the data
			HsvColor color = HsvColor( data[2], data[3], data[4] );
			// Set the mode and color
			setModeHSV( (COLOR_MODE)second, color );

		// Check the TYPE byte to see if it's RGB-data
		} else if ( data[1] == RGB ) {
			// Create the color from the data
			RgbColor color = RgbColor( data[2], data[3], data[4] );
			// Set the mode and color
			setModeRGB( (COLOR_MODE)second, color );

		// Other data type?? (Ignore any other data and just set the mode)
		} else {
			setMode( (COLOR_MODE)second );
		}

		// If the SPEED byte isn't 0, set the speed
		if ( data[5] != 0 )
			setSpeed( data[5] );

	// Check the MODE byte, is it SET_TEMP_MODE?
	} else if ( first == SET_TEMP_MODE ) {
		// Check the TYPE byte to see if it's HSV-data
		if ( data[1] == HSV ) {
			// Create the color from the data
			HsvColor color = HsvColor( data[2], data[3], data[4] );
			// Set the temporary mode and color, and concat the TTL value
			setTempModeHSV( (COLOR_MODE)second, color, (data[7] << 8) | data[6] );

		// Check the TYPE byte to see if it's RGB-data
		} else if ( data[1] == RGB ) {
			// Create the color from the data
			RgbColor color = RgbColor( data[2], data[3], data[4] );
			// Set the temporary mode and color, and concat the TTL value
			setTempModeRGB( (COLOR_MODE)second, color, (data[7] << 8) | data[6] );

		// Other data type?? (Ignore any other data and just set the temp mode)
		} else {
			setTempMode( (COLOR_MODE)second );
		}
		
		// If the SPEED byte isn't 0, set the speed
		if ( data[5] != 0 )
			setSpeed( data[5] );
	}
}

/* Send a command with a HSV color (default speed & TTL to 0/not set) */
int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color )
{
	return this->sendCommand( command, sub_command, mode, color, 0, 0 );
}
/* Send a command with a HSV color (default TTL to 0/not set) */
int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color, uint8_t speed )
{
	return this->sendCommand( command, sub_command, mode, color, speed, 0 );
}
/* Send a command with a HSV color */
int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, HsvColor color, uint8_t speed, int16_t ttl )
{
	// Split the color data into separate bytes and TTL to 2 bytes (LSB & MSB)
	return this->sendCommand( command | mode, sub_command, color.h, color.s, color.v, speed, ((ttl >> 8) & (0xFF)), (ttl & 0xFF) );
}

/* Send a command with a RGB color (default speed & TTL to 0/not set) */
int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color )
{
	return this->sendCommand( command, sub_command, mode, color, 0, 0 );
}
/* Send a command with a RGB color (default TTL to 0/not set) */
int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color, uint8_t speed )
{
	return this->sendCommand( command, sub_command, mode, color, speed, 0 );
}
/* Send a command with a RGB color */
int
CommandHandler::sendCommand( COMMAND_TYPE command, COMMAND_TYPE sub_command, COLOR_MODE mode, RgbColor color, uint8_t speed, int16_t ttl )
{
	// Split the color data into separate bytes and TTL to 2 bytes (LSB & MSB)
	return this->sendCommand( command | mode, sub_command, color.r, color.g, color.b, speed, ((ttl >> 8) & (0xFF)), (ttl & 0xFF) );
}

/* Send a command/data */
int
CommandHandler::sendCommand( uint8_t command, uint8_t data_0, uint8_t data_1, uint8_t data_2, uint8_t data_3, uint8_t data_4, uint8_t data_5, uint8_t data_6 )
{
	uint8_t data[8] = { command, data_0, data_1, data_2, data_3, data_4, data_5, data_6 };

	// Use the wrapper to send the data
	this->m_comm->sendData( data );
	
	// Check if it's sending
	while ( this->m_comm->isSending() ) {
		// While it's sending, run some "waiting code"
		this->m_comm->waiting();
	}
	
	// Return the status
	return this->m_comm->status();
}