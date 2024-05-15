#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <float.h>
#include "DoBoFo70Pro12.h"

extern const char* __progname;			// The program name is provided by libc.
#define PROGRAM_NAME __progname

static DoBoFo70Pro12 *myEDF = NULL;

static void teardown(void)
{
	if ( NULL != myEDF )
	{
		delete myEDF;
		myEDF = NULL;
	}
}


static void setup(void)
{
	(void)printf("%s:%s\n", PROGRAM_NAME, __FUNCTION__);

	myEDF = new DoBoFo70Pro12(E_JET_0, E_PWM_2);
	
	myEDF->throttle(0.0);

	(void)usleep(1000*150);     	// wait 150 ms for the fan to wakeup.

}

static void loop(void) {
	static const int32_t NUMBER_OF_THRUST_SETTINGS = 100;

	static int32_t iTSup = 0, iTSdn = NUMBER_OF_THRUST_SETTINGS-1;
		
	if ( iTSup<=NUMBER_OF_THRUST_SETTINGS )	
	{
		myEDF->throttle((double_t)iTSup);
		iTSup++;
	}
	else if ( iTSdn>=0 )
	{
		myEDF->throttle((double_t)iTSdn);
		iTSdn--;
	}
	else
	{
		iTSup = 1, iTSdn = NUMBER_OF_THRUST_SETTINGS-1;
		return;
	}

	double_t thrust = myEDF->thrust();

	(void)printf("The Electric Ducted Fan (\"EDF\") thrust is %lf N.\n", thrust);

	(void)usleep(1000*150);     // wait 150 ms for the fan to throttle up.

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
