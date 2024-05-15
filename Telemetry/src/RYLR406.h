/*
	RYLR406.h - Inherited Telemetry class for Raspberry PI Version 1.
	Copyright (c) 2024 Kirby W. Cartwright. All right reserved.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/
#ifndef _RYLR406_H
#define _RYLR406_H

#include <libserial/SerialPort.h>
#include "Telemetry.h"

typedef enum 
{
	AT_TEST 				= 0,
	AT_SOFTWARE_RESET		= 1,
	AT_MODE					= 2,
	AT_SET_BAUDRATE			= 3,
	AT_RF_PARAMETERS		= 4,
	AT_SET_RF_FREQUENCY		= 5,
	AT_SET_MODULE_ADDRESS	= 6,
	AT_SET_NETWORK_ID		= 7,
	AT_SET_AES128_PASSWORD	= 8,
	AT_SET_RF_OUTPUT_POWER	= 9,
	AT_SEND_TEXT_DATA		= 10,
	AT_RECEIVE_TEXT_DATA	= 11,
	AT_GET_FIRMWARE_VERSION	= 12,
	AT_GET_MODULE_ID		= 13,
	AT_SET_FACTORY_DEFAULTS	= 14,

	AT_COMMAND_SET_NUMBER 	= 15
} E_RYLR406_AT_COMMANDS;

#define AES_PASSWORD_LENGTH (32)

class RYLR406 : public Telemetry
{
public:
    RYLR406(const char *serialPort=NULL, const uint32_t uB=868500000, 
		const uint16_t uMyAddr=120, const uint8_t nID=6, const char *achPW="FABC0002EEDCAA90FABC0002EEDCAA90",
		const uint8_t rfP=10, const uint16_t uTheirAddr=50);

    virtual ~RYLR406();

	static const int32_t SERIAL_TIMEOUT_MS = 25;
	// Use "https://www.semtech.com/design-support/lora-calculator" to get timeouts?

protected:
	char achSerialPort[FILENAME_MAX];
	uint32_t uBand;
	uint16_t uMyAddress, uTheirAddress;
	uint8_t networkID, rfPower;
	bool bWaitForSend;

	char achPassword[AES_PASSWORD_LENGTH];
									// Use "openssl enc -aes-256-cbc -k secret -P -md sha1" 
									// “secret” is a passphrase for generating the key.

    virtual void writeBuffer(void);			

	virtual void update(void);				

	char *inBuffer;		

	char *textBuffer;

private:
	bool readReply(E_RYLR406_AT_COMMANDS e);

	bool writeAtCommand(const E_RYLR406_AT_COMMANDS e, const char *filledBuffer=NULL);

	bool readTextData(void);

    static const char * atCommands[AT_COMMAND_SET_NUMBER];

    static const char * atCommandReplies[AT_COMMAND_SET_NUMBER];

};


#endif  // _RYLR406_H