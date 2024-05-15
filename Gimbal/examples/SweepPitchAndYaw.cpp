#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "K-9-TVC-Gimbal-Generation-2.h"

extern const char* __progname;			// The program name is provided by libc.
#define PROGRAM_NAME __progname

static K9TvcGimbal *pmyDevice = NULL;

static bool bContinue = true;

static void teardown(void)
{
	if ( NULL!=pmyDevice)
	{
		pmyDevice->writeAngleDegrees(E_PITCH_AXIS, CENTER_ANGLE);
		pmyDevice->writeAngleDegrees(E_YAW_AXIS, CENTER_ANGLE);
		delete pmyDevice;
		pmyDevice = NULL;
	}
}

static void setup(void)
{
	(void)printf("%s:%s\n", PROGRAM_NAME, __FUNCTION__);

	pmyDevice = new K9TvcGimbal();

	// Gimbal *gilligan = new Gimbal("K-9 TVC Gimbal Generation 2")

	pmyDevice->writeAngleDegrees(E_PITCH_AXIS, CENTER_ANGLE);

	pmyDevice->writeAngleDegrees(E_YAW_AXIS, CENTER_ANGLE);

	bContinue = true;

	(void)usleep(1000*150);     // wait 150 ms for the servos to reach their default position.

}

static void loop(void) {
	double_t pos = 0;

	(void)printf("%s:%s\n", PROGRAM_NAME, __FUNCTION__);
		
	for (pos = MIN_ANGLE + 0.5; pos < MAX_ANGLE; pos += 0.5) 
	{ 								// goes from 0 degrees to 180 degrees in steps of 0.5 degree.
		pmyDevice->writeAngleDegrees(E_PITCH_AXIS, pos);
		pmyDevice->writeAngleDegrees(E_YAW_AXIS, pos);									
									// tell Gimbal to go to position in variable "pos."

		(void)usleep(1000*15);     // wait 15 ms for the servo to reach the position.
	}
	for (pos = MAX_ANGLE - 0.5; pos >= ( MIN_ANGLE + 0.5 ); pos -= 0.5 ) 
	{ 
									// goes from 180 degrees to 0 degrees in steps of 0.5 degree.
		pmyDevice->writeAngleDegrees(E_PITCH_AXIS, pos);
		pmyDevice->writeAngleDegrees(E_YAW_AXIS, pos);									
									// tell Gimbal to go to position in variable "pos."

		(void)usleep(1000*15);     // wait 15 ms for the servo to reach the position.	
	}

	bContinue = false;
}

int main( void )
{

	(void)printf("%s:%s\n", PROGRAM_NAME, __FUNCTION__);

	setup();
	
	while ( bContinue )
	{
		loop();
	}
	
	teardown();

	return 0;

}
