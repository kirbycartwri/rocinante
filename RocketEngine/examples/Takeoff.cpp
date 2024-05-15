#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <float.h>
#include "EstesF15_4.h"

extern const char* __progname;			// The program name is provided by libc.
#define PROGRAM_NAME __progname

EstesF15_4 *myEstes = NULL;

static bool bContinue = true;

static void teardown(void)
{
	if ( NULL != myEstes )
	{
		delete myEstes;
		myEstes = NULL;
	}
}


static void setup(void)
{
	bContinue = true;

	(void)printf("%s:%s\n", PROGRAM_NAME, __FUNCTION__);

	myEstes = new EstesF15_4();
	
	myEstes->throttle(100.0);	// Ignition!

	(void)usleep(62500/7);     	// waitfor the rocket to wakeup.

}

static void loop(void) {

	static int32_t iResponseCurve = 0;
		
	if ( iResponseCurve<=( 3 * NUMBER_OF_THRUST_STEPS ) )	
	{
		iResponseCurve++;

		double_t thrust = myEstes->thrust();
		double_t deltaT = myEstes->currentThrustDuration();

		(void)printf("The rockets (\"F15-4\") thrust at time %lf s is %lf N.\n", deltaT, thrust);

		(void)usleep(62500/3);     // wait 62.5/3 ms for the next step.

	}

	else
	{
		bContinue = false;
	}

	return;

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
