#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "DorheaMG90S.h"

extern const char* __progname;			// The program name is provided by libc.
#define PROGRAM_NAME __progname

// Compilation: g++  -Wall ../../logger/src/logger.cpp ../src/Servo.cpp Sweep.cpp -I ./ -I ../../logger/src -I ../src -o Sweep -g

DorheaMG90S myServo("Dorhea MG90S w. Metal Gears", E_PWM_0);		
									// Create a test servo object to control a servo.
									//	Note: constructs before "main!"

static double_t pos = MIN_ANGLE;	// variable to store the servo position

static void setup()
{
	(void)printf("%s:%s\n", PROGRAM_NAME, __FUNCTION__);

	myServo.writeAngleDegrees(MIN_ANGLE);
	
	(void)usleep(1000*150);     // wait 150 ms for the servo to reach the position.

}

static void loop() {
	(void)printf("%s:%s\n", PROGRAM_NAME, __FUNCTION__);
		
	for (pos = MIN_ANGLE + 0.5; pos < MAX_ANGLE; pos += 0.5) 
	{ 								// goes from 0 degrees to 180 degrees in steps of 0.5 degree.
		myServo.writeAngleDegrees(pos);
									// tell servo to go to position in variable "pos."
		(void)usleep(1000*150);     // wait 150 ms for the servo to reach the position.
	}
	for (pos = MAX_ANGLE - 0.5; pos >= ( MIN_ANGLE + 0.5 ); pos -= 0.5 ) 
	{ 
									// goes from 180 degrees to 0 degrees in steps of 0.5 degree.
		myServo.writeAngleDegrees(pos);
		(void)usleep(1000*150);     // wait 150 ms for the servo to reach the position.
	}
}

int main( void )
{

	(void)printf("%s:%s\n", PROGRAM_NAME, __FUNCTION__);

	setup();
	
	while ( true )
	{
		loop();
		break;
	}
	
	return 0;

}
