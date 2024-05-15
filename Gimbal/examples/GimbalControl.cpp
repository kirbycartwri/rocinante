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
#include "K_9_TVC_Gimbal_Generation_2.h"

#undef WIDE_OPEN

#ifdef 	WIDE_OPEN

#undef K9_MIN_YAW_ANGLE_DEGREES        
#undef K9_MAX_YAW_ANGLE_DEGREES		
#undef K9_CENTER_YAW_ANGLE_DEGREES	    
#undef K9_DEFAULT_YAW_ANGLE_DEGREES	

#undef K9_MIN_PITCH_ANGLE_DEGREES	    
#undef K9_MAX_PITCH_ANGLE_DEGREES	    
#undef K9_CENTER_PITCH_ANGLE_DEGREES   
#undef K9_DEFAULT_PITCH_ANGLE_DEGREES  

#undef K9_MIN_YAW_ANGLE_RADIANS    
#undef K9_MAX_YAW_ANGLE_RADIANS		
#undef K9_CENTER_YAW_ANGLE_RADIANS	    
#undef K9_DEFAULT_YAW_ANGLE_RADIANS	

#undef K9_MIN_PITCH_ANGLE_RADIANS	    
#undef K9_MAX_PITCH_ANGLE_RADIANS	    
#undef K9_CENTER_PITCH_ANGLE_RADIANS   
#undef K9_DEFAULT_PITCH_ANGLE_RADIANS  

#define K9_MIN_YAW_ANGLE_DEGREES        (-179.9)
#define K9_MAX_YAW_ANGLE_DEGREES		(180.0)
#define K9_CENTER_YAW_ANGLE_DEGREES	    (0.0)
#define K9_DEFAULT_YAW_ANGLE_DEGREES	(0.0)

#define K9_MIN_PITCH_ANGLE_DEGREES	    (-179.9)
#define K9_MAX_PITCH_ANGLE_DEGREES	    (180.0)
#define K9_CENTER_PITCH_ANGLE_DEGREES   (0.0)
#define K9_DEFAULT_PITCH_ANGLE_DEGREES  (0.0)

#define K9_MIN_YAW_ANGLE_RADIANS        (K9_MIN_YAW_ANGLE_DEGREES * M_PI / 180.0)
#define K9_MAX_YAW_ANGLE_RADIANS		(K9_MAX_YAW_ANGLE_DEGREES * M_PI / 180.0)
#define K9_CENTER_YAW_ANGLE_RADIANS	    (K9_CENTER_YAW_ANGLE_DEGREES * M_PI / 180.0)
#define K9_DEFAULT_YAW_ANGLE_RADIANS	(K9_DEFAULT_YAW_ANGLE_DEGREES * M_PI / 180.0)

#define K9_MIN_PITCH_ANGLE_RADIANS	    (K9_MIN_PITCH_ANGLE_DEGREES * M_PI / 180.0)
#define K9_MAX_PITCH_ANGLE_RADIANS	    (K9_MAX_PITCH_ANGLE_DEGREES * M_PI / 180.0)
#define K9_CENTER_PITCH_ANGLE_RADIANS   (K9_CENTER_PITCH_ANGLE_DEGREES * M_PI / 180.0)
#define K9_DEFAULT_PITCH_ANGLE_RADIANS  (K9_DEFAULT_PITCH_ANGLE_DEGREES * M_PI / 180.0)


#else	// WIDE_OPEN



#endif

static 	double_t dPitchPosition	= K9_CENTER_PITCH_ANGLE_DEGREES,
	dYawPosition				= K9_CENTER_YAW_ANGLE_DEGREES;

static char *pcProgramName 		= NULL;

static bool bDebug 				= true;

static bool bContinue			= true;

static 	int32_t c				= -1; 

static K9TvcGimbal *myCanineGimbal
								= NULL;

#define PRINTF if ( bDebug )printf	// useful debug macro

static void teardown(void)
{
	if ( NULL != myCanineGimbal )
	{
		myCanineGimbal->writeAngleDegrees(E_PITCH_AXIS, K9_CENTER_PITCH_ANGLE_DEGREES);
		myCanineGimbal->writeAngleDegrees(E_YAW_AXIS, K9_CENTER_YAW_ANGLE_DEGREES);		
		delete myCanineGimbal;
		myCanineGimbal = NULL;
	}
}

static void setup(void)
{

	if ( NULL!= pcProgramName )
		PRINTF("%s: %s\n", pcProgramName, __FUNCTION__);

	myCanineGimbal = new K9TvcGimbal();

	// PITCH is the inner (tube) gimbal.
	myCanineGimbal->writeAngleUpperLimitDegrees(E_PITCH_AXIS, K9_MAX_PITCH_ANGLE_DEGREES);
	myCanineGimbal->writeAngleDefaultDegrees(E_PITCH_AXIS, K9_DEFAULT_PITCH_ANGLE_DEGREES);
	myCanineGimbal->writeAngleCenterDegrees(E_PITCH_AXIS, K9_CENTER_PITCH_ANGLE_DEGREES);
	myCanineGimbal->writeAngleLowerLimitDegrees(E_PITCH_AXIS, K9_MIN_PITCH_ANGLE_DEGREES); 
	myCanineGimbal->writeAngleDegrees(E_PITCH_AXIS, K9_CENTER_PITCH_ANGLE_DEGREES);

	// YAW is the outer gimbal.
	myCanineGimbal->writeAngleUpperLimitDegrees(E_YAW_AXIS, K9_MAX_YAW_ANGLE_DEGREES);
	myCanineGimbal->writeAngleDefaultDegrees(E_YAW_AXIS, K9_DEFAULT_YAW_ANGLE_DEGREES);
	myCanineGimbal->writeAngleCenterDegrees(E_YAW_AXIS, K9_CENTER_YAW_ANGLE_DEGREES);
	myCanineGimbal->writeAngleLowerLimitDegrees(E_YAW_AXIS, K9_MIN_YAW_ANGLE_DEGREES); 
	myCanineGimbal->writeAngleDegrees(E_YAW_AXIS, K9_CENTER_YAW_ANGLE_DEGREES);

	(void)usleep(1000*150);     	// wait 150 ms for the servos to wakeup.

}

static void loop(void)
{
	myCanineGimbal->writeAngleDegrees(E_PITCH_AXIS, dPitchPosition);	
	myCanineGimbal->writeAngleDegrees(E_YAW_AXIS, dYawPosition);	

    while ((c = getchar()) != EOF)
	{
		char achNumberBuffer[6] = 
		{
			"180.0"
		};
			
		if ( 0 != isalpha(c) )
		{
			if ( 'Q' == toupper(c) )
			{
				bContinue = false;
			 	break;
			}
			else if ( 'P' == toupper(c) )
			{
				if ( NULL!=fgets(achNumberBuffer, sizeof(achNumberBuffer), stdin) )
				{

					dPitchPosition = atof(achNumberBuffer);

					if ( ( K9_MAX_PITCH_ANGLE_DEGREES < dPitchPosition ) || ( K9_MIN_PITCH_ANGLE_DEGREES > dPitchPosition ) )
					{
						printf( "PITCH setting  (%3.1lf degrees) is out-of-range (%3.1lf to %3.1lf degrees)!\n",dPitchPosition, (double_t)K9_MIN_PITCH_ANGLE_DEGREES, (double_t)K9_MAX_PITCH_ANGLE_DEGREES);
						dPitchPosition = myCanineGimbal->readAngleDegrees(E_PITCH_AXIS);
					}
					else 
					{
						printf( "Setting PITCH to %3.1lf degrees.\n",dPitchPosition );					
					}
				}
				break;		
			}
			else if ( 'Y' == toupper(c) )
			{
				if ( NULL!=fgets(achNumberBuffer, sizeof(achNumberBuffer), stdin) )
				{
					dYawPosition = atof(achNumberBuffer);

					if ( ( K9_MAX_YAW_ANGLE_DEGREES < dYawPosition ) || ( K9_MIN_YAW_ANGLE_DEGREES > dYawPosition ) )
					{
						printf( "YAW setting  (%3.1lf degrees) is out-of-range (%3.1lf to %3.1lf degrees)!\n",dYawPosition, (double_t)K9_MIN_YAW_ANGLE_DEGREES, (double_t)K9_MAX_YAW_ANGLE_DEGREES);
						dYawPosition = myCanineGimbal->readAngleDegrees(E_YAW_AXIS);
					}
					else 
					{
						printf( "Setting YAW to %3.1lf degrees.\n",dYawPosition );	
					}
				}
				break;
			}		
		}
	}	

}


int main( int argc, char *argv[] ) 
{

	pcProgramName 		= argv[0];

	// "getopt" returns -1 when done parsing command line options.
    while ((c = getopt(argc, argv, "-Y:y:P:p:C:c:Hh")) != -1)
	{
		switch (c)
		{
			case 'Y':
    		case 'y':
			{
				dYawPosition = atof(optarg);
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
				dPitchPosition = atof(optarg);
    		}			
		    break;

			case 'H':
			case 'h':
			{
				printf( "I see you asked for help with %s:\n", pcProgramName);
        	  	printf( "\t     -h or -H to show this help screen.\n" );
 	        	printf( "\t     -p# or -P# to set the PITCH position in degrees.\n" );
 	        	printf( "\t     -y# or -Y# to set the YAW position in degrees.\n" );
	        	printf( "\t     -d to toggle debugging messages ON/OFF.\n" );
                exit (EXIT_SUCCESS);
        	  	}

		   	default:
			{
    			PRINTF( "%s:  getopt() returned a strange command-line option, %c.\n","pcProgramName",c  );
			}
        	break;

		}
	}

	setup();

	printf( "\nHow to use %s.\n", pcProgramName);
	printf( "\t\'q\' or \'Q\' to quit.\n" );
	printf( "\tEnter P# or Y# to set the PITCH or YAW position in degrees.\n" );
	printf("\tThe PITCH range is from %3.1lf to %3.1lf degrees.\n", (double_t)K9_MIN_PITCH_ANGLE_DEGREES, (double_t)K9_MAX_PITCH_ANGLE_DEGREES);
	printf("\tThe YAW range is from %3.1lf to %3.1lf degrees.\n\n", (double_t)K9_MIN_YAW_ANGLE_DEGREES, (double_t)K9_MAX_YAW_ANGLE_DEGREES);

	while ( bContinue )
		loop();

	teardown();

	return EXIT_SUCCESS;	

}
