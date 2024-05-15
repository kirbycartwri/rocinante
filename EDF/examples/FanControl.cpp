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
#include "DoBoFo70Pro12.h"

static double_t dThrottlePosition
								= 0;

static char *pcProgramName 		= NULL;

static bool bDebug 				= true;

static bool bContinue			= true;

static E_SERVO_CHANNELS servoChannel	
								= E_PWM_2;

static 	int32_t c				= -1; 

static DoBoFo70Pro12 *myEDF 	= NULL;

#define PRINTF if ( bDebug )printf	// useful debug macro

static void teardown(void)
{
	if ( NULL != myEDF )
	{
		myEDF->throttle(0.0);
		delete myEDF;
		myEDF = NULL;
	}
}

static void setup(void)
{

	if ( NULL!= pcProgramName )
		PRINTF("%s:%s\n", pcProgramName, __FUNCTION__);

	myEDF = new DoBoFo70Pro12(DoBoFo70Pro12(E_JET_0, servoChannel));
	
	myEDF->throttle(0.0);

	(void)usleep(1000*150);     	// wait 150 ms for the fan to wakeup.

}

static void loop(void)
{
	myEDF->throttle((double_t)dThrottlePosition);

    while ((c = getchar()) != EOF)
	{		
		if ( 0 != isdigit(c) )
		{
			char achNumberBuffer[6] = 
			{
				"100.0"
			};
			
			(void)ungetc(c, stdin);
			if ( NULL!=fgets(achNumberBuffer, sizeof(achNumberBuffer), stdin) )
			{
				dThrottlePosition = atof(achNumberBuffer);
				if ( 0.0 > dThrottlePosition )
					dThrottlePosition = 0.0;
				else if ( 100.0 < dThrottlePosition )
					dThrottlePosition = 100.0;
				else
					dThrottlePosition = dThrottlePosition;
				break;
			}
			else
				break;
		}
		else if ( 0 != isalpha(c) )
		{
			if ( 'Q' == toupper(c) )
			{
				bContinue = false;
			 	break;
			}
		}
		else
			break;
	}	

}


int main( int argc, char *argv[] ) 
{

	pcProgramName 		= argv[0];

	// "getopt" returns -1 when done parsing command line options.
    while ((c = getopt(argc, argv, "-N:n:P:p:C:c:HhS:s:dD")) != -1)
	{
		switch (c){
			case 'N':
    		case 'n':
			{
				dThrottlePosition = atof(optarg);
    		}			
		    break;
			case 'C':
    		case 'c':
			{

		        break;
    		}
			case 'D':
    		case 'd':
			{
				if ( !bDebug )
					bDebug = true;
				else
					bDebug = false;
		        break;
    		}			
    		case 'p':
		  	case 'P':
			{
				int32_t  nDex = atoi(optarg);
				servoChannel	 = nDex ? E_PWM_2 : E_PWM_2 ;
			}
			break;
			case 'S':
    	    case 's':
			{				
			break;
			}

			case 'H':
			case 'h':{
				printf( "I see you asked for help with %s:\n", pcProgramName);
        	  	printf( "\t     -h to show this help screen.\n" );
 	        	printf( "\t     -p# or -P# to set the PWM channel.\n" );
    	      	printf( "\t     -n# or -N# to set the throttle position; 0%% to 100%%.\n" );
	        	printf( "\t     -d to toggle debugging messages ON/OFF.\n" );
                exit (EXIT_SUCCESS);
        	  	}

		   	default:{
    			PRINTF( "%s:  getopt() returned a strange command-line option, %c.\n","pcProgramName",c  );
        		break;
	 			}
			}
		}

	setup();

	printf( "How to use %s.\n", pcProgramName);
	printf( "\t\'q\' or \'Q\' to quit.\n" );
	printf( "\tEnter a number to set the throttle position; 0 to 100 %%.\n" );	

	while ( bContinue )
		loop();

	teardown();

	return EXIT_SUCCESS;	

}
