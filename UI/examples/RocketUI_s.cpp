/*
	FanControl.cpp - stdin UI to set the EDF throttle for Raspberry PI Version 1.
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

#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "sys/time.h"
#include "time.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <float.h>
#include <ctype.h>
#include "stdin_ui.h"

static char *pcProgramName 		= NULL;

static bool bDebug 				= false;

static bool bContinue			= true;

static UI *myConsole			= NULL;

static int32_t numArgs 			= -1;
static char **arguments			= NULL;


static void teardown(void)
{
	if ( bDebug && ( NULL!= pcProgramName ) )
		(void)printf("%s: %s\n", pcProgramName, __FUNCTION__);

	if ( NULL != myConsole )
	{	
		myConsole->teardown();
		delete myConsole;
		myConsole = NULL;
	}
}

static void setup(void)
{

	if ( bDebug && ( NULL!= pcProgramName ) )
		(void)printf("%s: %s\n", pcProgramName, __FUNCTION__);

	myConsole = new UI();

	myConsole->setup();

	myConsole->handleCommandLineOptions(numArgs, arguments);

	(void)usleep(1500000);     	// wait 1.5 s for everything to wakeup.

}

static void loop(void)
{

	while ( bContinue ) 
	{
		myConsole->loop();
		bContinue = false;
	}

}


int main( int argc, char *argv[] ) 
{

	pcProgramName 		= argv[0];

	numArgs 			= argc;
	arguments			= argv;

	setup();

	while ( bContinue )
		loop();

	teardown();

	return EXIT_SUCCESS;	

}
