/*
	RYLR406.cpp - Inherited Telemetry class for Raspberry PI Version 1.
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

    Based on "https://github.com/crayzeewulf/libserial/blob/master/examples/serial_port_read_write.cpp" example.
    No copyright in file.
*/

#include "RYLR406.h"

const char *RYLR406::atCommands[AT_COMMAND_SET_NUMBER] =
{
    "AT\r\n",
    "AT+RESET\r\n",
    "AT+MODE=0\r\n",
    "AT+IPR=115200\r\n",
    "AT+PARAMETER=12,7,1,4\r\n",
    "AT+BAND=%d\r\n",
    "AT+ADDRESS=%d\r\n",
    "AT+NETWORKID=%d\r\n",
    "AT+CPIN=%s\r\n",
    "AT+CRFOP=%d\r\n",
    "AT+SEND=%d,%d,%s\r\n",
    "+RCV=50,5,HELLO,-99,40",
    "AT+VER?\r\n",
    "AT+UID?\r\n",
    "AT+FACTORY\r\n"
};

const char *RYLR406::atCommandReplies[AT_COMMAND_SET_NUMBER] =
{
    "+OK\r\n",
    "+RESET\r\n+READY\r\n",
    "+OK\r\n",
    "+OK\r\n",
    "+OK\r\n",
    "+OK\r\n",
    "+OK\r\n",
    "+OK\r\n",
    "+OK\r\n",
    "+OK\r\n",
    "+RCV=50,5,HELLO,-99,40",
    "+OK\r\n",                            
    "+VER=RYLR406_Vx.x.x(RYLR40x)\r\n",
    "+FACTORY\r\n"
};

using namespace LibSerial;
SerialPort mySerialPort;

RYLR406::RYLR406(const char *serialPort/*=NULL*/, const uint32_t uB/*=868500000*/, 
    const uint16_t uMyAddr/*=120*/, const uint8_t nID/*=6*/, const char *achPW/*="FABC0002EEDCAA90FABC0002EEDCAA90"*/,
    const uint8_t rfP/*=10*/, const uint16_t uTheirAddr/*=50*/) : Telemetry(),
    uBand(uB), uMyAddress(uMyAddr), uTheirAddress(uTheirAddr), networkID(nID), rfPower(rfP), bWaitForSend(false)
{
    inBuffer = new char[TELEMETRY_BUFFER_SIZE];
	(void)memset(inBuffer, '\0', TELEMETRY_BUFFER_SIZE);

    textBuffer = new char[TELEMETRY_BUFFER_SIZE];
	(void)memset(textBuffer, '\0', TELEMETRY_BUFFER_SIZE);


    if ( NULL==serialPort )
        return;

    try
    {
        // Open the Serial Port at the desired hardware devices.
        mySerialPort.Open(serialPort) ;
    }
    catch (const OpenFailed&)
    {
        char achError[FILENAME_MAX];
        (void)sprintf(achError, "Serial port %s did not open correctly!\n", serialPort);
        (void)perror(achError);
        return;
    }

    (void)strncpy(achSerialPort, serialPort, sizeof(achSerialPort));

    // Use achPW == NULL to mean "No Password."
    if ( !strlen(achPW) )
        (void)memset(achPassword, '\0', sizeof(achPassword));

    // Set the baud rate.
    mySerialPort.SetBaudRate(BaudRate::BAUD_115200);

    // Set the number of data bits.
    mySerialPort.SetCharacterSize(CharacterSize::CHAR_SIZE_8);

    // Set the hardware flow control.
    mySerialPort.SetFlowControl(FlowControl::FLOW_CONTROL_NONE);

    // Set the parity.
    mySerialPort.SetParity(Parity::PARITY_NONE);
    
    // Set the number of stop bits.
    mySerialPort.SetStopBits(StopBits::STOP_BITS_1);

    while ( true )
    {
        if ( !writeAtCommand(AT_TEST) )
            break;
       if ( !writeAtCommand(AT_SET_BAUDRATE) )
            break;
       if ( !writeAtCommand(AT_RF_PARAMETERS) )
            break;

        char ach[FILENAME_MAX];
        (void)sprintf(ach, atCommands[AT_SET_RF_FREQUENCY], uBand);

       if ( !writeAtCommand(AT_RF_PARAMETERS, ach) )
            break;

        (void)sprintf(ach, atCommands[AT_SET_MODULE_ADDRESS], uMyAddress);

       if ( !writeAtCommand(AT_SET_MODULE_ADDRESS, ach) )
            break;

        (void)sprintf(ach, atCommands[AT_SET_NETWORK_ID], networkID);

       if ( !writeAtCommand(AT_SET_NETWORK_ID, ach) )
            break;

        if ( strlen(achPassword) )
        {
            (void)sprintf(ach, atCommands[AT_SET_AES128_PASSWORD], achPassword);

            if ( !writeAtCommand(AT_SET_AES128_PASSWORD, ach) )
                    break;
        }

        (void)sprintf(ach, atCommands[AT_SET_RF_OUTPUT_POWER], rfPower);

       if ( !writeAtCommand(AT_SET_RF_OUTPUT_POWER, ach) )
            break;


        // Todo: save/print fw version and module ID.
/*        
        if ( !writeAtCommand(AT_GET_FIRMWARE_VERSION) )
            break;    


        if ( !writeAtCommand(AT_GET_MODULE_ID) )
            break;

        if ( bDebug )
            (void)printf("Firmware version: %s.\r\n",);    
*/

    }

}    

// Example: AT+SEND=50,5,HELLO
void RYLR406::writeBuffer(void)
{
    size_t s = strlen(outBuffer);

    char ach[FILENAME_MAX];
    (void)sprintf(ach, atCommands[AT_SEND_TEXT_DATA], s, uTheirAddress);

    if ( !writeAtCommand(AT_SEND_TEXT_DATA, ach) )
        return;

    // Mark data as written.
	for (int32_t i=0 ; ( i<nItems ) && (!data[i].bWritten); i++ )
	{
		data[i].bWritten = 1;
	}

    // Clear, empty, the text-based buffer.
    (void)memset(outBuffer, '\0', TELEMETRY_BUFFER_SIZE);

}

void RYLR406::update(void)
{
    Telemetry::update();

    if ( readTextData() )
    {
        (void)puts(textBuffer); 	// includes "... a trailing newline ..."
        (void)fflush(stdout);
        (void)memset(textBuffer, '\0', TELEMETRY_BUFFER_SIZE);
        // Can be redirected into a file.
    }

}

RYLR406::~RYLR406()
{
    // Close the serial port.
    mySerialPort.Close();

    delete inBuffer;
    inBuffer = NULL;

    delete textBuffer;
    textBuffer = NULL;
}


bool RYLR406::writeAtCommand(const E_RYLR406_AT_COMMANDS e, const char *filledBuffer /*=NULL*/)
{
    // Write the command to the serial port.
    if ( NULL==filledBuffer )
        mySerialPort.Write(atCommands[e]) ;
    else 
        mySerialPort.Write(filledBuffer);

    // Wait until the data has actually been transmitted.
    if ( bWaitForSend)
        mySerialPort.DrainWriteBuffer();


    if (!readReply(e) )
        return false;

    return true;
}

bool RYLR406::readReply(E_RYLR406_AT_COMMANDS e)
{
    uint32_t nTries = 1000;

    // Wait for data to be available at the serial port.
    while ( nTries-- && !mySerialPort.IsDataAvailable() ) 
    {
        (void)usleep(SERIAL_TIMEOUT_MS) ;
    }

    if ( !nTries )
        return false;

    std::string theReply;

    try
    {
        mySerialPort.ReadLine(theReply, '\n', SERIAL_TIMEOUT_MS);    
    }
    catch (const ReadTimeout&)
    {
        (void)perror("The ReadLine call timed out waiting for additional data.\r\n");
        return false;
    }

	(void)memset(inBuffer, '\0', TELEMETRY_BUFFER_SIZE);

    for (size_t i = 0 ; i < theReply.size() ; i++)
    {
        inBuffer[i] = theReply.at(i);
    }

    if ( strncmp(inBuffer, atCommandReplies[e], strlen(atCommandReplies[e])))
        return false;

    return true;

}

// Example: +RCV=50,5,HELLO,-99,40
bool RYLR406::readTextData(void)
{
    if ( !readReply(AT_RECEIVE_TEXT_DATA) )
        return false;

    if ( strncmp(inBuffer, atCommands[AT_RECEIVE_TEXT_DATA], 5) )
        return false;

    uint32_t uNumTextChars, SNR = 0, uAddress = 0;
    int32_t dbm = 0;


    if ( 2 != sscanf(inBuffer, "+RCV=%u,%u,",&uNumTextChars, &uAddress) )
        return false;

    uTheirAddress = (uint16_t)uAddress;

    char *saveptr = NULL, *token = NULL;
    const char delim = ',';

    token = strtok_r(inBuffer, &delim, &saveptr);

    if ( NULL==token )
        return false;

    token = strtok_r(NULL, &delim, &saveptr);

    if ( NULL==token )
        return false;    

    (void)strncpy(token+strlen(token)+1, textBuffer, uNumTextChars);

    if ( 2 != sscanf(token+strlen(token)+uNumTextChars+1, "%d,%u,",&dbm, &SNR) )
        return false;

    return true;
}

/*
Todo: handle error codes:
There is not “enter” or 0x0D 0x0A in the end of the AT Command.
The head of AT command is not “AT” string.
There is not “=”symbol in the AT command.
Unknown command.
TX is over timeout.
RX is over timeout.
CRC error.
TX data more than 240 bytes.
Unknown error.

+ERR=1
+ERR=2
+ERR=3
+ERR=4
+ERR=10
+ERR=11
+ERR=12
+ERR=13
+ERR=15
*/