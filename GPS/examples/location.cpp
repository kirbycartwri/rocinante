/*
	location.cpp - GPS test for Raspberry PI Version 1.
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
#include <termios.h>
#include <assert.h>
#include "unistd.h"
#include "sys/ioctl.h"
#include "fcntl.h"
#include "sys/time.h"
#include "time.h"
#include <string.h>
#include <errno.h>
#include <float.h>
#include <ctype.h>
#include <stdint.h>


#include "GPS.h"

static double_t latitude, longitude, altitude;

static double_t seconds, X, Y, Z;

static char *pcProgramName 		= NULL;

static bool bDebug 				= true;

static bool bContinue			= true;

static E_GPS gpsChannel			= E_GPS_NUM_0;

static GPS *mySuperGPS 		    = NULL;

#define PRINTF if ( bDebug )printf	    // useful debug macro


static const useconds_t KEY_TIMEOUT_MICROSECONDS = 2000000;
                                        // Use this as the loop timeout too.

static const char ESC 	= 0x1b;			// ANSI

// Unicode:
static const wchar_t UP 	= 0x2191;	// From ESC [ A

static const wchar_t DOWN	= 0x2193;	// From ESC [ B

static const wchar_t RIGHT  = 0x2192;	// From ESC [ C

static const wchar_t LEFT 	= 0x2190;	// From ESC [ D

static const int32_t E_TIMED_OUT = (EOF - 1);

static const int16_t E_UNHANDLED_KEY_SEQUENCE = (E_TIMED_OUT - 1);

static int32_t getChar(const useconds_t usecTimeout = KEY_TIMEOUT_MICROSECONDS);
static void initTermios(void);
static void resetTermios(void);

static void teardown(void)
{
	resetTermios();

	if ( NULL != mySuperGPS )
	{
        mySuperGPS->teardown();
		delete mySuperGPS;
		mySuperGPS = NULL;
	}
}

static void setup(void)
{
    initTermios();

	if ( NULL!= pcProgramName )
		PRINTF("%s:%s\n", pcProgramName, __FUNCTION__);

	mySuperGPS = new GPS("GoouuTech (Beffkkip) GT-U7 Ublox NEO-6M GPS", gpsChannel);

    mySuperGPS->setup();

   (void)fprintf(stdout, "GPS latitude (degrees), longitude(degrees), altitude(meters),  X(meters), Y(meters), Z(meters), time(seconds)\n ");


}

static void loop(void)
{

	mySuperGPS->loop();

	mySuperGPS->readGpsLocation(latitude, longitude, altitude);

    mySuperGPS->readGpsLocationXYZ(X, Y, Z);

    mySuperGPS->readGpsTime(seconds);
    
    (void)fprintf(stdout, "%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", latitude, longitude, altitude, X, Y, Z, seconds );

    if ( 0 < getChar() )
        bContinue = false;

}


/* Initialize new terminal i/o settings */
static struct termios old, new1;

static void initTermios(void)
{
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */

    new1.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);

    tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
static void resetTermios(void)
{
    tcsetattr(0, TCSANOW, &old);
}

static int32_t getChar(const useconds_t usecTimeout/* = KEY_TIMEOUT_MICROSECONDS*/)
{
	fd_set readFDS, errorFDS;		// // Set up the file descriptor sets.
	FD_ZERO(&readFDS);
	FD_ZERO(&errorFDS);
  
	const int32_t nStdinFD = fileno(stdin);

  	FD_SET(nStdinFD, &readFDS); 			
									// Add stdin to the file descriptor sets.
	FD_SET(nStdinFD, &errorFDS);

	struct timeval timeout;			// Set up the timeout.
	timeout.tv_sec  = 0;
	timeout.tv_usec = usecTimeout;

  	int32_t n = select(1+nStdinFD, &readFDS, NULL, &errorFDS, &timeout);

	if ( 0 > n )
	{
		(void)fprintf(stderr, "%s: \"select\" error!\n\t\"%s\n", __FUNCTION__, strerror(errno));
		return EOF;
	}

	timeout.tv_sec  = 0;
	timeout.tv_usec = usecTimeout;	// Reset the timeout; if needed.

	n = FD_ISSET(0, &errorFDS);

  	if ( 0 < n ) 
	{
		(void)fprintf(stderr, "%s: \"select\" error!\n\t\"%s\n", __FUNCTION__, "Exceptional condition!");		
		return EOF;
	}

	n = FD_ISSET(0, &readFDS);

  	if ( 0 < n ) 
	{	
		int32_t c = getc(stdin);
			
        if ( 0 > c )
        {
            (void)fprintf(stderr, "%s: \"getc\" error!\n\t\"%s\n", __FUNCTION__, strerror(errno));
            return EOF;        
        }			
			
		else if ( ESC == c )
		{
			c = getc(stdin);
			if ( '[' == c )
			{
				c = getc(stdin);
				switch ( c )
				{
					case 'A':
					{
						c = UP;
					}
					break;
					case 'B':
					{
						c = DOWN;
					}
					break;
					case 'C':
					{
						c = RIGHT;
					}
					break;
					case 'D':
					{
						c = LEFT;
					}
					break;
					default:
					{
						(void)fflush(stdin);
						return E_UNHANDLED_KEY_SEQUENCE;
					}
				}
			}
			else 
			{
				return E_UNHANDLED_KEY_SEQUENCE;
			}
		}	
		
		else
			;

		if ( bDebug )
			(void)printf("getChar returns 0x%4.4x.\n",c);
			
		return c;
			
	}

	else
	{
		if ( false )
			(void)printf("%s: \"select\" %s.\n", __FUNCTION__, "timeout");		
    	return E_TIMED_OUT;			// Timeout occurred?
  	}


	return EOF;
}


int main( int argc, char *argv[] ) 
{

	pcProgramName 		= argv[0];

	setup();	

	while ( bContinue )
		loop();

	teardown();

	return EXIT_SUCCESS;	

}