


/*
	Servo.h - Servo library for Raspberry PI using its built in hardware PWM channels; Version 1.
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

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "Servo.h"


extern const char* __progname;			// The program name is provided by libc.
#define PROGRAM_NAME __progname

const bool Servo::bDebug = false;

bool Servo::bPeriodWritten = false;


const char *Servo::pNumPWMs =
{
	PWM_PATH_PREFACE "/npwm"
};

const char *Servo::pExportPath =
{	
	PWM_PATH_PREFACE "/export"
};

const char *Servo::pEnablePaths[NUM_PWM_CHANNELS] =
{
	PWM_PATH_PREFACE "/pwm0/enable",
	PWM_PATH_PREFACE "/pwm1/enable",
#ifdef 	_PCA9685
	PWM_PATH_PREFACE "/pwm2/enable",
	PWM_PATH_PREFACE "/pwm3/enable",
	PWM_PATH_PREFACE "/pwm4/enable",
	PWM_PATH_PREFACE "/pwm5/enable",	
	PWM_PATH_PREFACE "/pwm6/enable",
	PWM_PATH_PREFACE "/pwm7/enable",
	PWM_PATH_PREFACE "/pwm8/enable",
	PWM_PATH_PREFACE "/pwm9/enable",
	PWM_PATH_PREFACE "/pwm10/enable",
	PWM_PATH_PREFACE "/pwm11/enable",
	PWM_PATH_PREFACE "/pwm12/enable",
	PWM_PATH_PREFACE "/pwm13/enable",
	PWM_PATH_PREFACE "/pwm14/enable",
	PWM_PATH_PREFACE "/pwm15/enable"
#endif	

};

const char *Servo::pPeriodPaths[NUM_PWM_CHANNELS] =
{
	PWM_PATH_PREFACE "/pwm0/period",
	PWM_PATH_PREFACE "/pwm1/period",
#ifdef 	_PCA9685
	PWM_PATH_PREFACE "/pwm2/period",
	PWM_PATH_PREFACE "/pwm3/period",
	PWM_PATH_PREFACE "/pwm4/period",
	PWM_PATH_PREFACE "/pwm5/period",
	PWM_PATH_PREFACE "/pwm6/period",
	PWM_PATH_PREFACE "/pwm7/period",
	PWM_PATH_PREFACE "/pwm8/period",
	PWM_PATH_PREFACE "/pwm9/period",
	PWM_PATH_PREFACE "/pwm10/period",
	PWM_PATH_PREFACE "/pwm11/period",
	PWM_PATH_PREFACE "/pwm12/period",
	PWM_PATH_PREFACE "/pwm13/period",
	PWM_PATH_PREFACE "/pwm14/period",
	PWM_PATH_PREFACE "/pwm15/period"		
#endif	
};

const char *Servo::pPulseWidthPaths[NUM_PWM_CHANNELS] =
{
	PWM_PATH_PREFACE "/pwm0/duty_cycle",
	PWM_PATH_PREFACE "/pwm1/duty_cycle",
#ifdef 	_PCA9685
	PWM_PATH_PREFACE "/pwm2/duty_cycle",
	PWM_PATH_PREFACE "/pwm3/duty_cycle",
	PWM_PATH_PREFACE "/pwm4/duty_cycle",
	PWM_PATH_PREFACE "/pwm5/duty_cycle",	
	PWM_PATH_PREFACE "/pwm6/duty_cycle",
	PWM_PATH_PREFACE "/pwm7/duty_cycle",
	PWM_PATH_PREFACE "/pwm8/duty_cycle",
	PWM_PATH_PREFACE "/pwm9/duty_cycle",
	PWM_PATH_PREFACE "/pwm10/duty_cycle",
	PWM_PATH_PREFACE "/pwm11/duty_cycle",
	PWM_PATH_PREFACE "/pwm12/duty_cycle",
	PWM_PATH_PREFACE "/pwm13/duty_cycle",
	PWM_PATH_PREFACE "/pwm14/duty_cycle",
	PWM_PATH_PREFACE "/pwm15/duty_cycle"
#endif	
};


uint32_t Servo::uNumPWMs = 0xffffffff;

Servo::Servo(E_SERVO_CHANNELS sIndex/*=E_PWM_0*/) :
	pHandle(NULL), dAngleUpperLimit(MAX_ANGLE), dAngleLowerLimit(MIN_ANGLE),
	dAngleDefault(DEFAULT_ANGLE), dAngleCenter(CENTER_ANGLE), bWaitForSlew(true), ePwmChannel(sIndex)
{
	(void)memset(achGimbalName,'\0', sizeof(achGimbalName));

	if (bDebug)
		(void)printf("%s: Trying to open hardware PWM channel\"%i.\"\n", __FUNCTION__, ePwmChannel);
	
	// Note: setting up the Raspberry Pi PWM channel is order dependent.

	while (true)
	{
		if (bDebug)		
			(void)printf("\nTrying to open \"%s.\"\n", pNumPWMs);

		pHandle = fopen(pNumPWMs,"r");
		
		if ( NULL == pHandle )
		{	
			(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pNumPWMs, strerror(errno));		
			break;	
		}

		if (bDebug)
		 	(void)printf("Successfully opened \"%s.\"\n", pNumPWMs);

		int numPwms = -1;
		if ( 1 != fscanf(pHandle, "%i", &numPwms) )
		{
			(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pNumPWMs, strerror(errno));
			break;
		}
		else if (bDebug)
		{
			(void)printf("The number of Raspberry Pi hardware PWM channels is \"%i.\"\n", numPwms);
		}
		else
			;

		(void)usleep(PWM_SETTING_DELAY);


		(void)fclose(pHandle);
		pHandle = NULL;

		if (bDebug)
			(void)printf("\nTrying to open \"%s.\"\n", pExportPath);
		
		pHandle = fopen(pExportPath,"w");

		if ( NULL == pHandle )
		{
			(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pExportPath, strerror(errno));
			break;		
		}

		else 
		{
			 if ( 0 > fprintf(pHandle, "%i", ePwmChannel) )
			 {
				(void)printf("Unable to write to \"%s!\"\n\t\"%s\"\n", pExportPath, strerror(errno));
				break;				 
			 }
			 else if (bDebug)
			 {
				(void)printf("Exported PWM channel %i.\n", ePwmChannel);
			 }
			 else
			 	;

		}

		(void)usleep(PWM_SETTING_DELAY);

		(void)fclose(pHandle);
		pHandle = NULL;

		if ( !bPeriodWritten )
		{
			if (bDebug)
				(void)printf("\nTrying to open \"%s.\"\n", pPeriodPaths[ePwmChannel]);
			
			pHandle = fopen(pPeriodPaths[ePwmChannel],"r+");

			if ( NULL == pHandle )
			{
				(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pPeriodPaths[ePwmChannel], strerror(errno));
				break;		
			}

			else 
			{
				// Trying to set 400 Hz.
				if ( 0 > fprintf(pHandle, "%i", SERVO_PERIOD_WIDTH) )
				{
					(void)printf("Unable to write to \"%s!\"\n\t\"%s\"\n", pPeriodPaths[ePwmChannel], strerror(errno));
					break;				 
				}
				else if (bDebug)
				{
					(void)printf("Wrote %i nanoseconds to PWM Channel %i period (400 Hz).\n", SERVO_PERIOD_WIDTH, ePwmChannel);
				}
				else
					;

			}
					
			(void)usleep(PWM_SETTING_DELAY);		
		
			(void)fclose(pHandle);
			pHandle = NULL;
		
			bPeriodWritten = true;

		}
		
		else if (bDebug)
		{	
			(void)printf("\nThe period was already written for this chip, \"%s.\"\n", PWM_PATH_PREFACE);
		}
		else
			;


		if (bDebug)		
			(void)printf("\nTrying to open \"%s.\"\n", pPulseWidthPaths[ePwmChannel]);
		
		pHandle = fopen(pPulseWidthPaths[ePwmChannel],"r+");

		if ( NULL == pHandle)
		{
			(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pPulseWidthPaths[ePwmChannel], strerror(errno));
			break;		
		}

		else 
		{
			// Trying to set the default,
			 if ( 0 > fprintf(pHandle, "%i", DEFAULT_PULSE_WIDTH) )
			 {
				(void)printf("Unable to write to \"%s!\"\n\t\"%s\"\n", pPulseWidthPaths[ePwmChannel], strerror(errno));
				break;				 
			 }
			 else if (bDebug)
			 {
				(void)printf("Wrote %i nanoseconds to PWM Channel %i duty cycle (50 percent at 400 Hz).\n", DEFAULT_PULSE_WIDTH, ePwmChannel);
			 }
			 else
			 	;
		}		
	
		(void)usleep(PWM_SETTING_DELAY);

		(void)fclose(pHandle);
		pHandle = NULL;
		
		if (bDebug)
			(void)printf("\nTrying to open \"%s.\"\n", pEnablePaths[ePwmChannel]);
		
		pHandle = fopen(pEnablePaths[ePwmChannel],"r+");

		if ( NULL == pHandle )
		{
			(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pEnablePaths[ePwmChannel], strerror(errno));
			break;		
		}

		else 
		{
			// Trying to turn on.
			 if ( 0 > fprintf(pHandle, "%i", 1) )
			 {
				(void)printf("Unable to write to \"%s!\"\n\t\"%s\"\n", pEnablePaths[ePwmChannel], strerror(errno));
				break;				 
			 }
			 else if (bDebug)
			 {
				(void)printf("Turned-on PWM Channel %i.\n", ePwmChannel);
			 }
			 else
			 	;

		}			
			
		(void)usleep(PWM_SETTING_DELAY);

		break;
	}

	if ( NULL!=pHandle)
		(void)fclose(pHandle);
	pHandle = NULL;	

}

Servo::~Servo()
{
	if ( NULL!= pHandle )
	{
		(void)fprintf(pHandle, "%i", DEFAULT_PULSE_WIDTH);
		(void)fclose(pHandle);
		pHandle = NULL;
		(void)usleep((SERVO_PERIOD_WIDTH+500)/1000 );
	}
}

void Servo::writeAngleRadians(double_t dRadians)
{
	writeAngleDegrees( ( MAX_ANGLE * dRadians ) / M_PI );
}

uint32_t Servo::readPulseWidth(void)
{
	uint32_t uPulseWidth = 0;

	FILE *pHandle = fopen(pPulseWidthPaths[ePwmChannel],"r+");
	
	if ( NULL!=pHandle )
	{
		
		int nRead = fscanf(pHandle, "%i", &uPulseWidth);
		
		if ( 0 > nRead )
		{
			(void)fprintf(stderr, "Unable to read from \"%s!\"\n\t\"%s\"\n", pPulseWidthPaths[ePwmChannel], strerror(errno));
		}
		else if (bDebug)
		{
			(void)printf("Read %i nanoseconds from PWM Channel %i duty cycle (50 percent at 36 Hz).\n", uPulseWidth, ePwmChannel);		
		}
		else
			;
	}

	return uPulseWidth;

}

void Servo::writePulseWidth(uint32_t uValue)
{
	FILE *pHandle = fopen(pPulseWidthPaths[ePwmChannel],"r+");
	
	if ( NULL==pHandle )
	{
		return;
	}
	
	// Trying to set the pulse width in nanoseconds.
	if ( 0 > fprintf(pHandle, "%i", uValue) )
	{
		(void)printf("Unable to write to \"%s!\"\n\t\"%s\"\n", pPulseWidthPaths[ePwmChannel], strerror(errno));				 
	}
	else if (bDebug)
	{
		(void)printf("Wrote %i nanoseconds to PWM Channel %i duty cycle.\n", uValue, ePwmChannel);
	}
	else
		;

	if ( bWaitForSlew )
		(void)usleep((SERVO_PERIOD_WIDTH+500)/1000 );
	
	(void)fclose(pHandle);
	pHandle = NULL;	
	
}

void Servo::writeAngleDegrees(double_t dDegrees /* = DEFAULT_ANGLE */)
{ 
	if ( dDegrees  < dAngleLowerLimit ) 
	{
		dDegrees  = dAngleLowerLimit;
	}
	else if ( dDegrees > dAngleUpperLimit )
	{
		dDegrees = dAngleUpperLimit;
	}
	else
		dDegrees = dDegrees;
	
	
	// Convert the angle in degrees to Servo pulse width in nanoseconds.
	double d = dAngleUpperLimit - dAngleLowerLimit;
	d = ( dDegrees - dAngleLowerLimit ) / d;
	d *= ( (double_t) MAX_PULSE_WIDTH - (double_t) MIN_PULSE_WIDTH );
	d += (double_t)( 1 + MIN_PULSE_WIDTH );		
	uint32_t u = (uint32_t) d;
	
	writePulseWidth(u);
	
}

double_t Servo::readFrequency(void)
{
	double_t d = 1000000000.0 / SERVO_PERIOD_WIDTH;
	return d;
}

double_t Servo::readAngleUpperLimitDegrees(void)
{
	return dAngleUpperLimit;
}
double_t Servo::readAngleDefaultDegrees(void)
{
	return dAngleDefault;
}
double_t Servo::readAngleCenterDegrees(void)
{
	return dAngleCenter;
}
double_t Servo::readAngleLowerLimitDegrees(void)
{
	return dAngleLowerLimit;
}
double_t Servo::readAngleUpperLimitRadians(void)
{
	return dAngleUpperLimit * M_PI / MAX_ANGLE;
}
double_t Servo::readAngleDefaultRadians(void)
{
	return dAngleDefault * M_PI / MAX_ANGLE;
}
double_t Servo::readAngleCenterRadians(void)
{
	return dAngleCenter * M_PI / MAX_ANGLE;
}
double_t Servo::readAngleLowerLimitRadians(void)
{
	return dAngleLowerLimit * M_PI / MAX_ANGLE;
}

void Servo::writeAngleUpperLimitDegrees(double_t dDegrees)
{
	dAngleUpperLimit = dDegrees;
}
void Servo::writeAngleDefaultDegrees(double_t dDegrees)
{
	dAngleDefault = dDegrees;	
}
void Servo::writeAngleCenterDegrees(double_t dDegrees)
{
	dAngleCenter = dDegrees;	
}
void Servo::writeAngleLowerLimitDegrees(double_t dDegrees)
{
	dAngleLowerLimit = dDegrees;	
}

void Servo::writeAngleUpperLimitRadians(double_t dRadians)
{
	dAngleUpperLimit = dRadians * MAX_ANGLE / M_PI;
}
void Servo::writeAngleDefaultRadians(double_t dRadians)
{
	dAngleDefault = dRadians  * MAX_ANGLE / M_PI;	
}
void Servo::writeAngleCenterRadians(double_t dRadians)
{
	dAngleCenter = dRadians * MAX_ANGLE / M_PI;	
}
void Servo::writeAngleLowerLimitRadians(double_t dRadians)
{
	dAngleLowerLimit = dRadians * MAX_ANGLE / M_PI;	
}

double_t Servo::readAngleDegrees(void)
{
	double_t dDegrees = -1.0;
	uint32_t uPulseWidth = readPulseWidth();

	// Convert the Servo pulse width in nanoseconds to the angle in degrees. Switch to using "map."
	dDegrees = (double_t)uPulseWidth - (double_t)MIN_PULSE_WIDTH;
	dDegrees /= ( (double_t)MAX_PULSE_WIDTH - (double_t)MIN_PULSE_WIDTH );
	dDegrees *= ( MAX_ANGLE - MIN_ANGLE );
	dDegrees += MIN_ANGLE;	

	
	return dDegrees;
}

double_t Servo::readAngleRadians(void)
{
	return ( ( M_PI *readAngleDegrees() ) / MAX_ANGLE ) ;
}

