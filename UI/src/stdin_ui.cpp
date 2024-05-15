/*
	UI.h - Base User Interface class for Raspberry PI; Version 1.
	Uses "stdin"
	Copyright (c) 2024 Kirby W. Cartwright. All rights reserved.
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <time.h>
#include "stdin_ui.h"

UI::UI(const char *UIName/* = NULL*/) :
	bDebug(true),bContinue(true), bCalibrated(false),
	dPitchSetting(0.0), dRollSetting(0.0), dYawSetting(0.0),
	dAltitudeSetting(0.0), dThrottleSetting(0.0),
	dAltitudeValue(0.0), dPressureValue(0.0), dTemperatureValue(0.0),
	dPitchValue(0.0), dRollValue(0.0), dYawValue(0.0),

	dLatitudeValue(0.0), dLongitudeValue(0.0), dLocationAltitudeValue(0.0),
	dX(0.0), dY(0.0), dZ(0.0), dTime(0.0),

	dXaccel(0.0), dYaccel(0.0), dZaccel(0.0),
	rockHopper(NULL), keyBuffer(NULL)
{
	if ( NULL!=UIName )
	{
		(void)strncpy(achUIName, UIName, sizeof(achUIName));
	}
	else
		(void)memset(achUIName, '\0', sizeof(achUIName));

	keyBuffer = new char[KEY_BUFFER_SIZE];
	(void)memset(keyBuffer, '\0', KEY_BUFFER_SIZE);

	(void)memset(&origTerm, 0, sizeof(struct termios));
	(void)memset(&newTerm, 0, sizeof(struct termios));

	rockHopper = new Rockhopper("\"rockhopper,\" an ancestor of \"rocinante.\"");

}

UI::~UI()
{
	bDebug = bContinue = bCalibrated = false;
	(void)memset(achUIName, '\0', sizeof(achUIName));
	(void)memset(&origTerm, 0, sizeof(struct termios));
	(void)memset(&newTerm, 0, sizeof(struct termios));	
	delete keyBuffer, keyBuffer = NULL;
	delete rockHopper, rockHopper = NULL;
}

int32_t UI::getChar(const useconds_t usecTimeout/* = KEY_TIMEOUT_MICROSECONDS*/)
{
	fd_set readFDS, errorFDS;		// // Set up the file descriptor sets.
	FD_ZERO(&readFDS);
	FD_ZERO(&errorFDS);
  
	const int32_t nStdinFD = fileno(stdin);

  	FD_SET(nStdinFD, &readFDS); 			
									// Add stdin to the file descriptor sets.
	FD_SET(nStdinFD, &errorFDS);

	struct timeval timeout;			// Set up the timeout.
	timeout.tv_sec  = 0;
	timeout.tv_usec = usecTimeout;

  	int32_t n = select(1+nStdinFD, &readFDS, NULL, &errorFDS, &timeout);

	if ( 0 > n )
	{
		(void)fprintf(stderr, "%s: \"select\" error!\n\t\"%s\n", __FUNCTION__, strerror(errno));
		return EOF;
	}

	timeout.tv_sec  = 0;
	timeout.tv_usec = usecTimeout;	// Reset the timeout; if needed.

	n = FD_ISSET(0, &errorFDS);

  	if ( 0 < n ) 
	{
		(void)fprintf(stderr, "%s: \"select\" error!\n\t\"%s\n", __FUNCTION__, "Exceptional condition!");		
		return EOF;
	}

	n = FD_ISSET(0, &readFDS);

  	if ( 0 < n ) 
	{	
		int32_t c = getc(stdin);
			
        if ( 0 > c )
        {
            (void)fprintf(stderr, "%s: \"getc\" error!\n\t\"%s\n", __FUNCTION__, strerror(errno));
            return EOF;        
        }			
			
		else if ( ESC == c )
		{
			c = getc(stdin);
			if ( '[' == c )
			{
				c = getc(stdin);
				switch ( c )
				{
					case 'A':
					{
						c = UP;
					}
					break;
					case 'B':
					{
						c = DOWN;
					}
					break;
					case 'C':
					{
						c = RIGHT;
					}
					break;
					case 'D':
					{
						c = LEFT;
					}
					break;
					default:
					{
						(void)fflush(stdin);
						return E_UNHANDLED_KEY_SEQUENCE;
					}
				}
			}
			else 
			{
				return E_UNHANDLED_KEY_SEQUENCE;
			}
		}	
		
		else
			;

		if ( false )
			(void)printf("getChar returns 0x%4.4x.\n",c);
			
		return c;
			
	}

	else
	{
		if ( false )
			(void)printf("%s: \"select\" %s.\n", __FUNCTION__, "timeout");		
    	return E_TIMED_OUT;			// Timeout occurred?
  	}


	return EOF;
}	

const char *UI::getLine(const useconds_t usecTimeout /*= LINE_TIMEOUT_MICROSECONDS*/)
{
	struct timespec thisTime, lastTime;

	(void)memset(keyBuffer, '\0', KEY_BUFFER_SIZE);

	(void)clock_gettime(CLOCK_REALTIME, &thisTime);
	(void)clock_gettime(CLOCK_REALTIME, &lastTime);

	size_t numChars   = 0;

	const size_t maxNumChars = KEY_BUFFER_SIZE;

	while ( numChars < maxNumChars )
	{
		int32_t c = getChar();		// defaults to 10 ms timeout.

		if ( 0 < c )
		{
            if ( false )
                (void)printf("getChar returns 0x%2.2x.\n",c);
			keyBuffer[numChars++] = c;
			if ( '\n' == c )
            {
                keyBuffer[--numChars] = '\0';   // strip the newline.
				return (const char *)keyBuffer;
            }
            c = EOF;
		}
		else if ( EOF == c )
		{
			break;
		}
		else
			;						// timeout.

		(void)clock_gettime(CLOCK_REALTIME, &thisTime);

		useconds_t deltaT 	= 0;
		time_t tv_delta_sec = ( thisTime.tv_sec - lastTime.tv_sec );
		long tv_delta_ns	= thisTime.tv_nsec - lastTime.tv_nsec;

		deltaT = tv_delta_ns / 1000;
		deltaT += 1000000 * tv_delta_sec;

		if ( deltaT >= usecTimeout )
			break;
	}

	(void)memset(keyBuffer, '\0', KEY_BUFFER_SIZE);
	return NULL;
}

void UI::showCursor(void)
{
	(void)fprintf(stdout, "%c[?25h", ESC);
}

void UI::hideCursor(void)
{
	(void)fprintf(stdout, "%c[?25l", ESC);
}

void UI::clearScreen()
{
	(void)fprintf(stdout, "%c%c%c%c", ESC, '[', '2', 'J');
}

void UI::initScreen()
{
	// (void)fprintf(stdout, "%c[=80h", ESC);	
	(void)fprintf(stdout, "%c[9;24;80r", ESC);
	clearScreen();
}

void UI::displayHelp()
{
	displayMenu();

	(void)getChar(3 * 100 * KEY_TIMEOUT_MICROSECONDS);
									// Wait on key or timeout after 3s.
	clearScreen();
}

void UI::displayMenu(void)
{
	clearScreen();
	gotoXY(1,6);
	(void)fprintf(stdout, "\n*************************************************************************\n");
	(void)fprintf(stdout, "\nI see you asked for help with this program; # means numeric entry.\n");
	(void)fprintf(stdout, "\nThese characters also work on the command-line (with \'-\').\n");
	(void)fprintf(stdout, "\t     Press h or H to show this help screen.\n" );
	(void)fprintf(stdout, "\t     Press q or Q to quit.\n" );	
	(void)fprintf(stdout, "\t     Enter p# or -P# to set the PITCH position in degrees.\n" );
	(void)fprintf(stdout, "\t     Enter y# or -Y# to set the YAW position in degrees.\n" );
	(void)fprintf(stdout, "\t     Press c or C to center the gimbal.\n" );
	(void)fprintf(stdout, "\t     Press d or D to toggle debugging messages ON/OFF.\n" );
	(void)fprintf(stdout, "\t     Press f# or F# to set feedback to OFF/ON/FOLLOW (0/1/2).\n" );
	(void)fprintf(stdout, "\t     Enter T# to set the throttle position in percent; 0 to 100%%.\n" );
	(void)fprintf(stdout, "\t     Enter t to toggle the Telemetry On/Off.\n" );	
	(void)fprintf(stdout, "\t     Press z or Z to zero sensors. This take five minutes.\n" );
	(void)fprintf(stdout, "\t     Press + or - to increase or decrease the throttle.\n" );	
	(void)fprintf(stdout, "\t     Press Up or Down Arrow to change pitch.\n" );		
	(void)fprintf(stdout, "\t     Press Left or Right Arrow to change Yaw.\n" );		
	(void)fprintf(stdout, "\n***************************************************************************\n\n");

}

void UI::displayWindow(void)
{
	clearScreen();

	(void)fprintf(stdout, "\n*************************************************************************\n");
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");									
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");
	(void)fprintf(stdout, "*                                                                       *\n");	
	(void)fprintf(stdout, "*************************************************************************\n\n"
	);

}

const char *UI::pOptions 	= "cCqQdDY:y:P:p:F:f:HhT:t:Zz";
void UI::handleCommandLineOptions(int argc, char *argv[])
{
	opterr = 0;						// We will handle errors ourselves. I.e., "getopt" will return '?' when there is an error.

	int32_t c = -1;
									// "getopt" returns -1 when done parsing command line options.
    while ( true )
	{
		c = getopt(argc, argv, pOptions);

		if ( -1 == c )
			break;

		switch (c)
		{
			case 'c':
			case 'C':
			{
				// Todo: change to radians?
				dPitchSetting = dRollSetting = dYawSetting = 0.0;
				setOrientation(dPitchSetting, dRollSetting, dYawSetting);
				displayCommand(E_COMMAND_SET_CENTER);
				displaySettings(E_ORIENTATION_SETTINGS);								
			}
			break;
			case 'q':
			case 'Q':
			{
				bContinue = false;
			}
			break;
			case 'z':
			case 'Z':
			{
				bool bOnOff = atoi(optarg);
				if ( bOnOff )
					zeroSensors();				
			}
			break;
			case 'T':
			{
				dThrottleSetting = atof(optarg);
				setThrottle(dThrottleSetting);
				displayCommand(E_COMMAND_SET_THROTTLE);				
				displaySettings(E_THROTTLE_SETTING);				
			}
			break;
			case 't':
			{
				bool bOnOff = atoi(optarg);				
				setTelemetry(bOnOff);
			}
			break;
			case 'Y':
    		case 'y':
			{
				getOrientation(dPitchSetting, dRollSetting, dYawSetting);
				dYawSetting = atof(optarg);
				setOrientation(dPitchSetting, dRollSetting, dYawSetting);
				displaySettings(E_ORIENTATION_SETTINGS);	
				displayCommand(E_COMMAND_SET_YAW);							
    		}			
		    break;
			
			case 'F':
    		case 'f':
			{
				int32_t iFeedback = atoi(optarg);

				E_FEEDBACK_MODE e = E_FEEDBACK_OFF; 

				if ( !iFeedback )
				{
					setFeedback(e);
				}

				else if ( 1 == iFeedback )
				{ 
					setOrientation(dPitchSetting, dRollSetting, dYawSetting);
					e = E_FEEDBACK_ON;
					setFeedback(e);
				}	
				else
				{
					e = E_FEEDBACK_FOLLOW;
					setFeedback(e);
				}			
				displayCommand(E_COMMAND_SET_FEEDBACK);
    		}
		    break;
			
			case 'D':
    		case 'd':
			{
				if ( !bDebug )
					bDebug = true;
				else
					bDebug = false;
				displayCommand(E_COMMAND_TOGGLE_DEBUG);					
    		}			
		    break;			
    		case 'p':
		  	case 'P':
			{
				getOrientation(dPitchSetting, dRollSetting, dYawSetting);
				dPitchSetting = atof(optarg);
				setOrientation(dPitchSetting, dRollSetting, dYawSetting);
				displaySettings(E_ORIENTATION_SETTINGS);
				displayCommand(E_COMMAND_SET_PITCH);								
    		}			
		    break;

			case 'H':
			case 'h':
			{
				displayHelp();
				displayCommand(E_COMMAND_HELP);				
       	  	}
			break;
			case '?':
		   	default:
			{
    			(void)fprintf(stdout, "%s:  \"getopt\" returned a strange command-line option or an error happened, %c!\n", argv[0], c );
			}
        	break;

		}
	}

}

void UI::handleMenu(void)
{
	int32_t c 		= -1;

	char *optarg 	= NULL;	

	c = getChar();

	switch (c)
	{
		case EOF:
		{
			bContinue = false;
			break;
		}
		break;
		case E_UNHANDLED_KEY_SEQUENCE:
		case E_TIMED_OUT:
		{
			break;
		}
		break;

		case UP:
		{
			getOrientation(dPitchSetting, dRollSetting, dYawSetting);
			dPitchSetting += 0.5;
			setOrientation(dPitchSetting, dRollSetting, dYawSetting);
			displayCommand(E_COMMAND_INCREASE_PITCH);
			displaySettings(E_ORIENTATION_SETTINGS);
		}
		break;
		case DOWN:
		{
			getOrientation(dPitchSetting, dRollSetting, dYawSetting);
			dPitchSetting -= 0.5;
			setOrientation(dPitchSetting, dRollSetting, dYawSetting);
			displayCommand(E_COMMAND_DECREASE_PITCH);
			displaySettings(E_ORIENTATION_SETTINGS);					
		}
		break;
		case LEFT:
		{
			getOrientation(dPitchSetting, dRollSetting, dYawSetting);
			dYawSetting -= 0.5;
			setOrientation(dPitchSetting, dRollSetting, dYawSetting);
			displayCommand(E_COMMAND_TURN_LEFT);
			displaySettings(E_ORIENTATION_SETTINGS);					
		}
		break;
		case RIGHT:
		{
			getOrientation(dPitchSetting, dRollSetting, dYawSetting);
			dYawSetting += 0.5;
			setOrientation(dPitchSetting, dRollSetting, dYawSetting);
			displayCommand(E_COMMAND_TURN_RIGHT);
			displaySettings(E_ORIENTATION_SETTINGS);					
		}
		break;

		case '+':
		{
			getThrottle(dThrottleSetting);
			dThrottleSetting += 10.0;
			setThrottle(dThrottleSetting);
			displayCommand(E_COMMAND_INCREASE_THROTTLE);
			displaySettings(E_THROTTLE_SETTING);			
		}
		break;
		case '-':
		{
			getThrottle(dThrottleSetting);
			dThrottleSetting -= 10.0;
			setThrottle(dThrottleSetting);
			displayCommand(E_COMMAND_DECREASE_THROTTLE);
			displaySettings(E_THROTTLE_SETTING);			
		}
		break;
		case 'q':
		case 'Q':
		{
			bContinue = false;
			displayCommand(E_COMMAND_QUIT);
			(void)sleep(1);
		}
		break;
		case 'z':
		case 'Z':
		{
			optarg = (char *)getLine();
			if ( NULL != optarg )
			{
				bool bOnOff = atoi(optarg);
				if ( bOnOff )
				{
					zeroSensors();
					displayCommand(E_COMMAND_ZERO_SENSORS);
				}
			}		
		}
		break;
		case 'T':
		{
			optarg = (char *)getLine();
			if ( NULL != optarg )
			{				
				dThrottleSetting = atof(optarg);
				setThrottle(dThrottleSetting);
				displayCommand(E_COMMAND_SET_THROTTLE);
				displaySettings(E_THROTTLE_SETTING);				
			}
		}
		break;
		case 't':
		{
			optarg = (char *)getLine();
			if ( NULL != optarg )
			{				
				bool bOnOff = atoi(optarg);
				setTelemetry(bOnOff);
				displayCommand(E_COMMAND_TOGGLE_TELEM); 
			}
		}
		break;
		case 'Y':
		case 'y':
		{
			optarg = (char *)getLine();
			if ( NULL != optarg )
			{				
				getOrientation(dPitchSetting, dRollSetting, dYawSetting);
				dYawSetting = atof(optarg);
				setOrientation(dPitchSetting, dRollSetting, dYawSetting);
				displayCommand(E_COMMAND_SET_YAW);
				displaySettings(E_ORIENTATION_SETTINGS);				
			}
		}			
		break;
		
		case 'F':
		case 'f':
		{
			optarg = (char *)getLine();
			if ( NULL != optarg )
			{				
				int32_t iFeedback = atoi(optarg);
				E_FEEDBACK_MODE e = E_FEEDBACK_OFF;

				if ( !iFeedback )
				{
					setFeedback(e);
				}

				else if ( 1 == iFeedback )
				{ 
					setOrientation(dPitchSetting, dRollSetting, dYawSetting);
					e = E_FEEDBACK_ON;
					setFeedback(e);
				}	
				else
				{
					e = E_FEEDBACK_FOLLOW;
					setFeedback(e);
				}			
				displayCommand(E_COMMAND_SET_FEEDBACK);
			}
		}
		break;
		
		case 'D':
		case 'd':
		{
			if ( !bDebug )
				bDebug = true;
			else
				bDebug = false;
			displayCommand(E_COMMAND_TOGGLE_DEBUG);
		}			
		break;			
		case 'p':
		case 'P':
		{
			optarg = (char *)getLine();
			if ( NULL != optarg )
			{				
				getOrientation(dPitchSetting, dRollSetting, dYawSetting);
				dPitchSetting = atof(optarg);
				setOrientation(dPitchSetting, dRollSetting, dYawSetting);
				displayCommand(E_COMMAND_SET_PITCH);
				displaySettings(E_ORIENTATION_SETTINGS);				
			}
		}			
		break;

		case 'H':
		case 'h':
		{
			displayHelp();
			displayCommand(E_COMMAND_HELP);
		}
		break;
		case 'c':
		case 'C':
		{
			// Todo: change to radians?
			dPitchSetting = dRollSetting = dYawSetting = 0.0;
			setOrientation(dPitchSetting, dRollSetting, dYawSetting);
			displayCommand(E_COMMAND_SET_CENTER);
			displaySettings(E_ORIENTATION_SETTINGS);								
		}
		break;		
		case '?':
		default:
		{
			// displayCommand(E_COMMAND_NOTHING);
		}
		break;

	}

}

void UI::setup(void)
{
	initTermios();
	hideCursor();

	initScreen();
	bContinue = true;
	getOrientation(dPitchSetting, dRollSetting, dYawSetting);
	readOrientation(dPitchValue, dRollValue, dYawValue);	
	getThrottle(dThrottleSetting);
	getAltitude(dAltitudeValue);
	getPressure(dPressureValue);
	getTemperature(dTemperatureValue);
	getAccelerations(dXaccel, dYaccel, dZaccel);
	// need pressure, altitude, and temperature, and acceleration values.

	displaySettings(E_ORIENTATION_SETTINGS);
	displaySettings(E_THROTTLE_SETTING);	
}

void UI::update(void)
{
	rockHopper->update();
}

void UI::loop(void)
{		
	displayMenu();
	while ( bContinue )
	{
		handleMenu();
		displayValues();
		update();
	}

}
void UI::teardown(void)
{
	clearScreen();
	bContinue	= false;
	resetTermios();
	showCursor();
}

void UI::setFeedback(const E_FEEDBACK_MODE &e)
{
	rockHopper->setFeedback(e);
}

void UI::getFeedback(E_FEEDBACK_MODE &e)
{
	rockHopper->getFeedback(e);
}

void UI::setThrottle(const double_t &dPercentage)
{
	rockHopper->throttle(dPercentage);
}

void UI::getThrottle(double_t &dPercentage)
{
	dPercentage = rockHopper->throttlePosition();
}

void UI::setTelemetry(const bool &bOnOff){
	if ( bOnOff )
		rockHopper->startTelemetry();
	else
		rockHopper->stopTelemetry();
}

void UI::getTelemetry(bool &bOnOff)
{
	bOnOff = rockHopper->getTelemetry();
}

void UI::setOrientation(const double_t &dPitch, const double_t &dRoll, const double_t &dYaw)
{
	rockHopper->setOrientationDegrees(dPitch, dRoll, dYaw);
}

void UI::getOrientation(double_t &dPitch, double_t &dRoll, double_t &dYaw)
{
	rockHopper->getOrientationDegrees(dPitch, dRoll, dYaw);
}

void UI::readOrientation(double_t &dPitch, double_t &dRoll, double_t &dYaw)
{
	rockHopper->readOrientationDegrees(dPitch, dRoll, dYaw);
}

void UI::zeroSensors(void)
{
	bCalibrated = false;

	rockHopper->calibrateSensors();

	bCalibrated = true;
}

void UI::initTermios(void) 
{
    (void)tcgetattr(0, &origTerm);	// Grab original terminal i/o settings.
    newTerm = origTerm; 			// Make the new settings same as old settings.

									// Make keys show up "instantly."
    newTerm.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);

    (void)tcsetattr(0, TCSANOW, &newTerm);// Use these new terminal i/o settings.
}

/* Restore old terminal i/o settings */
void UI::resetTermios(void) 
{
    (void)tcsetattr(0, TCSANOW, &origTerm);
}

void UI::displayValues(void)
{ 
	// We're going to take advantage of the 10 ms keyboard timeout here; side effect.
	static int32_t timerTick = 0;

	if ( 24 < timerTick )
	{
		const int32_t tT = timerTick % 25;

		if ( false )
		{
			gotoXY(1,3);
			(void)printf("%s: %d.\n", __FUNCTION__, (int32_t) timerTick);
			timerTick = 0;
			return;
		}

		else switch ( tT )
		{
			case 0:
			{
				readOrientation(dPitchValue, dRollValue, dYawValue);
				gotoXY(1,3);
				(void)fprintf(stdout, "PITCH: %5.1lf° ROLL: %05.1f° YAW: %05.1f°", dPitchValue, dRollValue, dYawValue);
			}
			break;
			case 1:
			{			
				getThrottle(dThrottleSetting);
				gotoXY(40,3);
				(void)fprintf(stdout, "THROTTLE: %5.1lf%%", dThrottleSetting);				
			}
			break;		
			case 2:
			{					
				getAltitude(dAltitudeValue);
				gotoXY(1,4);
				(void)fprintf(stdout, "ALTITUDE: %5.1lf m", dAltitudeValue);					
			}
			break;	
			case 3:
			{						
				getPressure(dPressureValue);
				gotoXY(33,4);
				(void)fprintf(stdout, "PRESSURE: %5.1lf mbar", dPressureValue);				
			}
			break;	
			case 4:
			{						
				getTemperature(dTemperatureValue);
				gotoXY(66,4);
				(void)fprintf(stdout, "TEMPERATURE: %5.1lf°C", dTemperatureValue);				
			}
			break;
			case 5:
			{						
				getAccelerations(dXaccel, dYaccel, dZaccel);
				gotoXY(1,5);
				(void)fprintf(stdout, "X: %5.1lf m/s/s Y: %05.1f m/s/s Z: %05.1f m/s/s", dXaccel, dYaccel, dZaccel);
			}
			// Deliberately fall into the following.
			default:
			{
				timerTick = -1;
			}	
			break;	
		}
	}

	timerTick++;
}

void UI::displaySettings(E_SETTINGS_CODES e /*= E_NO_SETTING*/)
{
	const char settingStrings[E_NUMBER_OF_SETTINGS][50] =
	{
		"                                         ",
		"PITCH: %5.1lf° ROLL: %5.1f° YAW: %5.1f°",
		"                                         ",
		"THROTTLE: %5.1lf%%                      ",
		"                                         "	
	};
	
	if ( false )
	{
		gotoXY(1,2);
		(void)printf("%s: %d.\n", __FUNCTION__, (int32_t) e);
	}

	else switch ( (int32_t) e )
	{
		default:
		case E_ALTITUDE_SETTING:
		case E_ACCELERATION_SETTINGS:
		case E_NO_SETTING:
		{
			gotoXY(1,2);
			(void)fprintf(stdout, "%s", &settingStrings[E_NO_SETTING][0]);
		}
		break;
		case E_ORIENTATION_SETTINGS:
		{
			gotoXY(1,2);
			getOrientation(dPitchSetting, dRollSetting, dYawSetting);
			(void)fprintf(stdout, &settingStrings[E_ORIENTATION_SETTINGS][0], dPitchSetting, dRollSetting, dYawSetting);
		}
		break;
		case E_THROTTLE_SETTING:
		{
			gotoXY(40,2);
			getThrottle(dThrottleSetting);
			(void)fprintf(stdout, &settingStrings[E_THROTTLE_SETTING][0], dThrottleSetting);
		}
		break;
	}	

}

void UI::displayCommand(E_COMMAND_CODES e /*= E_COMMAND_NOTHING*/)
{
	const char commandStrings[NUM_COMMANDS][18] =
	{
		"                 ",
		"HELP             ",
		"QUIT             ",
		"SET PITCH        ",
		"SET YAW          ",
		"SET CENTER       ",
		"DEBUG ON/OFF     ",
		"SET THROTTLE     ",
		"TELEMETRY ON/OFF ",
		"ZERO SENSORS     ",
		"INCREASE THROTTLE",
		"DECREASE_THROTTLE",
		"PITCH UP         ",
		"PITCH DOWN       ",
		"TURN LEFT        ",
		"TURN RIGHT       ",
		"SET FEEDBACK     "
	};

	gotoXY(1,1);

	if ( false )
		(void)printf("%s: %d.\n", __FUNCTION__, (int32_t) e);
	else 		
		(void)fputs(&commandStrings[e][0], stdout);

}

void UI::gotoXY(int32_t x /*= 1*/, int32_t y /*= 1*/)
{
	const int32_t DISPLAY_X_SIZE = 80,
		DISPLAY_Y_SIZE = 24;

	char buffer[18] =
	{
		// E.g., moves the cursor to row 24, column 80.
		ESC, '[', '2','4', ';', '8','0', 'H', '\0'
	};

	if ( ( 1 > x ) || ( DISPLAY_X_SIZE < x ) )
		x = DISPLAY_X_SIZE / 2;

	if ( ( 1 > y ) || ( DISPLAY_Y_SIZE < y ) )
		y = DISPLAY_Y_SIZE / 2;

	(void)sprintf(buffer, "%c[%d;%dH", ESC, y, x );

	(void)fputs(buffer, stdout);

}

void UI::getAltitude(double_t &dAltitude)
{
	dAltitude = rockHopper->getAltitude();
}

void UI::getTemperature(double_t &dTemperature)
{
	dTemperature = rockHopper->getTemperature();
}

void UI::getPressure(double_t &dPressure)
{
	dPressure = rockHopper->getPressure();
}

void UI::getAccelerations(double_t &dX, double_t &dY, double_t &dZ)
{
	rockHopper->getAccelerations(dX, dY, dZ);
}



