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

// Compile with "g++ test_getchar.cpp -o tgchar"

static char *pcProgramName 		= NULL;

static bool bDebug 				= true;

static bool bContinue			= true;

static int32_t numArgs 			= -1;
static char **arguments			= NULL;

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
    	c = ETIMEDOUT;			// Timeout occurred?
  	}

    resetTermios();

    return c;
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


	(void)usleep(1500000);     	// wait 1.5 s for everything to wakeup.

}

static void loop(void)
{

	while ( bContinue ) 
	{
        int i = getChar();
        if ( bDebug && ( ETIMEDOUT != i ) )
            (void)printf("getche returns 0x%2.2x.\n", i);
        i = toupper(i);
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

#ifdef USE_THIS
/* Initialize new terminal i/o settings */
static struct termios old, new1;
void initTermios(int echo) {
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    new1 = old; /* make new settings same as old settings */
    new1.c_lflag &= ~ICANON; /* disable buffered i/o */
    new1.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
    tcsetattr(0, TCSANOW, &new1); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) {
    tcsetattr(0, TCSANOW, &old);
}

#include <iostream> 
#include <stdio.h>  
using namespace std;  

int main() { 
  // Output prompt 
  cout << "Press any key to continue..." << endl; 

  // Set terminal to raw mode 
  system("stty raw"); 

  // Wait for single character 
  char input = getchar(); 

  // Echo input:
  cout << "--" << input << "--";

  // Reset terminal to normal "cooked" mode 
  system("stty cooked"); 

  // And we're out of here 
  return 0; 
}

While this works, for what it is worth, shelling out to the system is rarely the "best" way to do it in my opinion. 
The stty program is written in C, so you can include <termios.h> or <sgtty.h> and call the same code that stty uses, 
without depending on an external program/fork/whatnot. 
#endif