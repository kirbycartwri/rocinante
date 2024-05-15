#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <float.h>
#include "RockHopperControl.h"


extern const char* __progname;			// The program name is provided by libc.
#define PROGRAM_NAME __progname

RockHopperControl *myRH = NULL;

static void teardown(void)
{
	if ( NULL != myRH )
	{
		delete myRH;
		myRH = NULL;
	}
}


static void setup(void)
{
	(void)printf("%s:%s\n", PROGRAM_NAME, __FUNCTION__);

	myRH = new RockHopperControl();

	(void)usleep(1000*150);     	// wait 150 ms for the doohickey to wakeup.

}

static void loop(void) {


	(void)usleep(1000*150);     // wait 150 ms for the rocket to catch up.

	return;

}

int main( void )
{

	(void)printf("%s:%s\n", PROGRAM_NAME, __FUNCTION__);

	setup();
	
	while ( true )
	{
		loop();
	}
	
	teardown();

	return 0;

}
