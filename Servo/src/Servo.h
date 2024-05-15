/*
	Servo.h - Base Class Servo library for Raspberry PI using its hardware PWM channels; Version 1.
	Copyright (c) 2023 Kirby W. Cartwright. All right reserved.

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

	You have to add this to the Raspberry Pi's "/boot/config.txt" file for the microcontroller's PWM channels:
		# Select Alt0 for GPIO pins 12 to 13 (for PWM output)
		gpio=12-13=a0
		dtoverlay=pwm,pin=12,func=4
		dtoverlay=pwm,pin=13,func=4		
	For the pca9685a PWM controller, put the following in /boot/config.txt:
		dtoverlay=i2c-pwm-pca9685a,addr=0x40 

*/

#ifndef _SERVO_H_
#define _SERVO_H_

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>

#define SERVO_VERSION       2     		// software version of this library

#ifndef MIN_ANGLE
#define	MIN_ANGLE			0.0
#define MAX_ANGLE			180.0
#endif
#define	DEFAULT_ANGLE		( ( ( MAX_ANGLE - MIN_ANGLE ) / 2 ) + MIN_ANGLE )
#define CENTER_ANGLE		( DEFAULT_ANGLE )

#define MIN_PULSE_WIDTH		500000     	// This is the shortest pulse width in nanoseconds sent to a servo; 0.0 degrees.
#define MAX_PULSE_WIDTH     2500000    	// This is the longest pulse width in nanoseconds sent to a servo; 180.0 degrees.
#define DEFAULT_PULSE_WIDTH	1500000    	// This is the default pulse width in nanoseconds when the servo is attached ("neutral"); 90 degrees.
#define SERVO_PERIOD_WIDTH	2500000		// nanoseconds for 400 Hz.

#define SERVOS_PER_TIMER	2			// The maximum number of servos controlled by the RPi firmware.

#define SERVO_SLEW_RATE		( 60.0 / 0.11 )
										// Sixty degrees in 0.11 second.

#define MAX_SERVO_SLEW_TIME	( 0.11 / 60.0 * MAX_ANGLE * 1000000)
										// Slew 180 degrees, no load.

#define	_PCA9685
#ifdef 	_PCA9685
#define PWM_PATH_PREFACE	"/sys/class/pwm/pwmchip2"
#else
#define PWM_PATH_PREFACE	"/sys/class/pwm/pwmchip0"
#endif

typedef enum 
{
	E_PWM_0				= 0,
	E_PWM_1 			= 1,
#ifdef 	_PCA9685
	E_PWM_2				= 2,
	E_PWM_3 			= 3,
	E_PWM_4				= 4,
	E_PWM_5 			= 5,
	E_PWM_6				= 6,
	E_PWM_7 			= 7,	
	E_PWM_8				= 8,
	E_PWM_9 			= 9,	
	E_PWM_10			= 10,
	E_PWM_11 			= 11,
	E_PWM_12			= 12,
	E_PWM_13 			= 13,
	E_PWM_14			= 14,
	E_PWM_15 			= 15,	
#endif
	NUM_PWM_CHANNELS 	/* Should default to the number of channels.*/
} E_SERVO_CHANNELS;


class Servo
{
public:
	Servo(E_SERVO_CHANNELS sIndex=E_PWM_0);
	~Servo();
	void writeAngleDegrees(double_t dDegrees = DEFAULT_ANGLE );		
											// The arguments for these are an angle in degrees.
	void writeAngleUpperLimitDegrees(double_t dDegrees);
	void writeAngleDefaultDegrees(double_t dDegrees);
	void writeAngleCenterDegrees(double_t dDegrees);
	void writeAngleLowerLimitDegrees(double_t dDegrees);

	void writeAngleRadians(double_t dRadians);
											// The arguments for these are an angle in radians.
	void writeAngleUpperLimitRadians(double_t dRadians);
	void writeAngleDefaultRadians(double_t dRadians);
	void writeAngleCenterRadians(double_t dRadians);
	void writeAngleLowerLimitRadians(double_t dRadians);

	double_t readAngleUpperLimitDegrees(void);
	double_t readAngleDefaultDegrees(void);
	double_t readAngleCenterDegrees(void);
	double_t readAngleLowerLimitDegrees(void);
											// The arguments for these are an angle in radians.
	double_t readAngleUpperLimitRadians(void);
	double_t readAngleDefaultRadians(void);
	double_t readAngleCenterRadians(void);
	double_t readAngleLowerLimitRadians(void);

	double_t readAngleDegrees(void);      	// returns current pulse width as an angle between 0 and 180 degrees.
	double_t readAngleRadians(void);        // returns current pulse width as an angle between 0 and pi radians.
	double_t readFrequency(void);

	uint32_t readPulseWidth(void);          // Returns the current pulse width in nanoseconds for this servo channel.

	void writePulseWidth(uint32_t uValue);	// Write pulse width in nanoseconds.


	static const useconds_t PWM_SETTING_DELAY = 10000;
											// 10 milliseconds.

protected:
	static uint32_t uNumPWMs;
	FILE *pHandle;							// Used to access the "/sys/class/pwm/" abstraction.
		
	E_SERVO_CHANNELS servoIndex;            // Index into the channel data for this servo.

	char achGimbalName[FILENAME_MAX];

private:
	static const char *pNumPWMs;
	static const char *pExportPath;
	static const char *pEnablePaths[NUM_PWM_CHANNELS];
	static const char *pPeriodPaths[NUM_PWM_CHANNELS];
	static const char *pPulseWidthPaths[NUM_PWM_CHANNELS];
	
	double_t dAngleUpperLimit;
	double_t dAngleLowerLimit;
	double_t dAngleDefault;
	double_t dAngleCenter;
	
	bool bWaitForSlew;
	
	E_SERVO_CHANNELS ePwmChannel;

	static const bool bDebug;	

	static bool bPeriodWritten;				// Per PWM chip, you can only write the period once.
};

#endif
