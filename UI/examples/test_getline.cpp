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
#include <unistd.h>
#include <float.h>
#include <ctype.h>

// Compile with "g++ test_getline.cpp -o tline"

static char *pcProgramName 		= NULL;

static bool bDebug 				= true;

static bool bContinue			= true;

static int32_t numArgs 			= -1;
static char **arguments			= NULL;

#define E_TIMED_OUT (EOF - 1)


/* Initialize new terminal i/o settings */
static struct termios old, new1;

static void initTermios(void) {
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */

    new1.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);

    tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
static void resetTermios(void) {
    tcsetattr(0, TCSANOW, &old);
}

// Named after an archaic library function in MS-DOS.
static int32_t getche(void) 
{
    int32_t c = EOF;

    struct termios org_opts, new_opts;
    (void)memset(&org_opts, 0, sizeof(struct termios));
    (void)memset(&new_opts, 0, sizeof(struct termios));

    int32_t res = EOF;
    
    //-----  get and store old settings -----------
    res = tcgetattr(STDIN_FILENO, &org_opts);
    if ( 0 > res )
    {
		(void)fprintf(stderr, "%s: \"tcgetattr\" error!\n\t\"%s\n", __FUNCTION__, strerror(errno));
		return EOF;        
    }

    //---- create new terminal setting --------
    (void)memcpy(&new_opts, &org_opts, sizeof(new_opts));  
    new_opts.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);
    
    //---- use new terminal setting --------    
    res = tcsetattr(STDIN_FILENO, TCSANOW, &new_opts);
    if ( 0 > res )
    {
		(void)fprintf(stderr, "%s: \"tcsetattr\" error!\n\t\"%s\n", __FUNCTION__, strerror(errno));
		c = EOF;        
    }    

    else
    {
        c = getc(stdin);

        if ( 0 > c )
        {
            (void)fprintf(stderr, "%s: \"getc\" error!\n\t\"%s\n", __FUNCTION__, strerror(errno));
            return EOF;        
        }
    }

    //------  restore old settings ---------
    res = tcsetattr(STDIN_FILENO, TCSANOW, &org_opts);
    
    if ( 0 > res )
    {
		(void)fprintf(stderr, "%s: \"tcsetattr\" error!\n\t\"%s\n", __FUNCTION__, strerror(errno));
		return EOF;        
    } 

    return c;
}

static const useconds_t KEY_TIMEOUT_MICROSECONDS = 10000;

int32_t getChar(const useconds_t usecTimeout = KEY_TIMEOUT_MICROSECONDS )
{
    int32_t c = EOF;

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

    initTermios();

  	int32_t n = select(1+nStdinFD, &readFDS, NULL, &errorFDS, &timeout);

	if ( 0 > n )
	{
		(void)fprintf(stderr, "%s: \"select\" error!\n\t\"%s\n", __FUNCTION__, strerror(errno));
		return EOF;
	}

	timeout.tv_sec  = 0;
	timeout.tv_usec = usecTimeout;	// Reset the timeout; if needed.

	n = FD_ISSET(0, &errorFDS);

  	if ( 0 < n) 
	{
		(void)fprintf(stderr, "%s: \"select\" error!\n\t\"%s\n", __FUNCTION__, "Exceptional condition!");		
		return EOF;
	}

	n = FD_ISSET(0, &readFDS);

  	if ( 0 < n ) 
	{		
    	c = getc(stdin);		// Get the character from "stdin."
  	} 
	else
	{
		if ( false )
			(void)printf("%s: \"select\" %s.\n", __FUNCTION__, "timeout");		
    	c = E_TIMED_OUT;			// Timeout occurred?
  	}

    resetTermios();

    return c;
}

static const size_t KEY_BUFFER_SIZE = 100;

static char keyBuffer[KEY_BUFFER_SIZE];

static const useconds_t LINE_TIMEOUT_MICROSECONDS = 10000000;

const char *getLine(const useconds_t usecTimeout =LINE_TIMEOUT_MICROSECONDS)
{
	struct timespec thisTime, lastTime;

	(void)memset(keyBuffer, '\0', KEY_BUFFER_SIZE);

	(void)clock_gettime(CLOCK_REALTIME, &thisTime);
	(void)clock_gettime(CLOCK_REALTIME, &lastTime);

	size_t numChars   = 0;

	const size_t maxNumChars = KEY_BUFFER_SIZE;

	while ( numChars < maxNumChars )
	{
		int32_t c = getChar();		// defaults to 10 ms timeout.

		if ( 0 < c )
		{
            if ( bDebug )
                (void)printf("getChar returns 0x%2.2x.\n",c);
			keyBuffer[numChars++] = c;
			if ( '\n' == c )
            {
                keyBuffer[--numChars] = '\0';   // strip the newline.
				return (const char *)keyBuffer;
            }
            c = EOF;
		}
		else if ( EOF == c )
		{
			break;
		}
		else
			;						// timeout.

		(void)clock_gettime(CLOCK_REALTIME, &thisTime);

		useconds_t deltaT 	= 0;
		time_t tv_delta_sec = ( thisTime.tv_sec - lastTime.tv_sec );
		long tv_delta_ns	= thisTime.tv_nsec - lastTime.tv_nsec;

		deltaT = tv_delta_ns / 1000;
		deltaT += 1000000 * tv_delta_sec;

		if ( deltaT >= usecTimeout )
			break;
	}

	(void)memset(keyBuffer, '\0', KEY_BUFFER_SIZE);
	return NULL;
}

static void teardown(void)
{
	if ( bDebug && ( NULL!= pcProgramName ) )
		(void)printf("%s: %s\n", pcProgramName, __FUNCTION__);

}

static void setup(void)
{

	if ( bDebug && ( NULL!= pcProgramName ) )
		(void)printf("%s: %s\n", pcProgramName, __FUNCTION__);


    (void)memset(keyBuffer, '\0', sizeof(keyBuffer));

	(void)usleep(1500000);     	// wait 1.5 s for everything to wakeup.

}

static void loop(void)
{

	while ( bContinue ) 
	{
        const char *c = getLine();
        if ( NULL != c )
        {
            if ( bDebug )
                (void)printf("getLine returns \"%s.\"\n", c);
        }
        else
            continue;
        int32_t i = toupper(c[0]);
        if ( 'Q' == i )
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
