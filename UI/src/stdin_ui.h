/*
	UI.h - Base User Interface class for Raspberry PI; Version 1.
	Uses "stdin"
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

#ifndef	_UI_H
#define _UI_H

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>
#include <termios.h>
#include "rockhopper.h"

#define UI_VERSION	1     			// software version of this class.

// Todo: display feedback, orientation, height, and etc.

typedef enum
{
	E_COMMAND_NOTHING			= 0,
	E_COMMAND_HELP				= 1,
	E_COMMAND_QUIT				= 2,
	E_COMMAND_SET_PITCH			= 3,
	E_COMMAND_SET_YAW			= 4,
	E_COMMAND_SET_CENTER		= 5,
	E_COMMAND_TOGGLE_DEBUG		= 6,
	E_COMMAND_SET_THROTTLE		= 7,
	E_COMMAND_TOGGLE_TELEM		= 8,
	E_COMMAND_ZERO_SENSORS		= 9,
	E_COMMAND_INCREASE_THROTTLE	= 10,
	E_COMMAND_DECREASE_THROTTLE	= 11,
	E_COMMAND_INCREASE_PITCH	= 12,
	E_COMMAND_DECREASE_PITCH	= 13,
	E_COMMAND_TURN_LEFT			= 14,
	E_COMMAND_TURN_RIGHT		= 15,
	E_COMMAND_SET_FEEDBACK		= 16,

	NUM_COMMANDS				= 17
} E_COMMAND_CODES; 

typedef enum
{
	E_NO_VALUE 				= 0,
	E_ORIENTATION_VALUES	= 1,
	E_THROTTLE_VALUE		= 3,
	E_PRESSURE_VALUE		= 4,
	E_TEMPERATURE_VALUE		= 5,
	E_ALTITUDE_VALUE		= 6,
	E_ACCELERATION_VALUES	= 7,

	E_NUMBER_OF_VALUES		= 8

} E_VALUES_CODES;

typedef enum
{
	E_NO_SETTING 			= 0,
	E_ORIENTATION_SETTINGS	= 1,
	E_ALTITUDE_SETTING		= 2,
	E_THROTTLE_SETTING		= 3,
	E_ACCELERATION_SETTINGS	= 4,	

	E_NUMBER_OF_SETTINGS	= 5,

} E_SETTINGS_CODES;


class UI
{
public:
	UI(const char *UIName = NULL);
	virtual ~UI();

	void handleCommandLineOptions(int argc, char *argv[]);

	virtual void setup(void);
	virtual void loop(void);
	virtual void teardown(void);

	void setFeedback(const E_FEEDBACK_MODE &e);

	void getFeedback(E_FEEDBACK_MODE &e);

	void setThrottle(const double_t &dPercentage);

	void getThrottle(double_t &dPercentage);

	void setTelemetry(const bool &bOnOff);

	void getTelemetry(bool &bOnOff);

	void setOrientation(const double_t &dPitch, const double_t &dRoll, const double_t &dYaw);

	void getOrientation(double_t &dPitch, double_t &dRoll, double_t &dYaw);

	void getLocation(double_t &dLatitude, double_t &dLongitued, double_t &dAltitude);	

	void getLocationXYZ(double_t &dX, double_t &dY, double_t &dZ);

	void getLocationTime(double_t &dTime);

	void readOrientation(double_t &dPitch, double_t &dRoll, double_t &dYaw);

	void getAltitude(double_t &dAltitude);

	void getTemperature(double_t &dTemperature);

	void getPressure(double_t &dPressure);

	void getAccelerations(double_t &dX, double_t &dY, double_t &dZ);

	void zeroSensors(void);

	static const useconds_t KEY_TIMEOUT_MICROSECONDS = 10000;
									// 10 milliseconds
	static const useconds_t LINE_TIMEOUT_MICROSECONDS = 10000000;
									// 10 seconds.
protected:

	char achUIName[FILENAME_MAX];

	bool bDebug;
	bool bContinue;
	bool bCalibrated;

	double_t dPitchSetting,
		dRollSetting,
		dYawSetting,
		dAltitudeSetting,
		dThrottleSetting,

		dAltitudeValue,
		dPressureValue,
		dTemperatureValue,
		dPitchValue,
		dRollValue,
		dYawValue;

		double_t dLatitudeValue, dLongitudeValue, dLocationAltitudeValue;
		double_t dX, dY, dZ, dTime;

	double_t dXaccel, dYaccel, dZaccel;

	Rockhopper *rockHopper;

	virtual void displayMenu(void);

	virtual void displayValues(void);

	virtual void displaySettings(E_SETTINGS_CODES e = E_NO_SETTING);

	virtual void displayCommand(E_COMMAND_CODES e = E_COMMAND_NOTHING);

	virtual void handleMenu(void);

	virtual void displayWindow(void);

	virtual void displayHelp(void);

	virtual void update(void);

	virtual void initScreen(void);
	// https://en.wikipedia.org/wiki/ANSI_escape_code
	// ESC [ 2 J

	virtual void clearScreen(void);

	virtual void gotoXY(int32_t x = 1, int32_t y = 1);

	virtual int32_t getChar(const useconds_t usecTimeout = KEY_TIMEOUT_MICROSECONDS);
	// https://www.gnu.org/software/libc/manual/html_node/Error-Codes.html
	// ETIMEDOUT

	virtual const char *getLine(const useconds_t usecTimeout = LINE_TIMEOUT_MICROSECONDS);
    
private:
	static const char ESC 	= 0x1b;			// ANSI

	// Unicode:
	static const wchar_t UP 	= 0x2191;	// From ESC [ A

	static const wchar_t DOWN	= 0x2193;	// From ESC [ B

	static const wchar_t RIGHT  = 0x2192;	// From ESC [ C

	static const wchar_t LEFT 	= 0x2190;	// From ESC [ D
/*
Left arrow: ←, U+02190, or ←
Right arrow: →, U+02192, or →
Down arrow: ↓, U+02193, or ↓
Up arrow U+02191, or ↑
*/
	
/*
	CSI n ; m H	Cursor Position ("CUP") CSI is ESC [ followed by ASCII numbers
		Moves the cursor to row n, column m. E.g., ESC [ 1 ; 1 
		The values are 1-based, and default to 1 (top left corner) if omitted. 
		A sequence such as CSI ;5H is a synonym for CSI 1;5H as well as CSI 17;H is the same as CSI 17H and CSI 17;1H
*/

	static const char *pOptions;

	char *keyBuffer;

	static const size_t KEY_BUFFER_SIZE = 100;

	pthread_t thread1, thread2;

									// Terminal i/o settings.
	struct termios origTerm, newTerm;
	void initTermios(void);
	void resetTermios(void);

	void showCursor(void);

	void hideCursor(void);

	static const int32_t E_TIMED_OUT = (EOF - 1);

	static const int16_t E_UNHANDLED_KEY_SEQUENCE = (E_TIMED_OUT - 1);

};

#endif	// _UI_H
