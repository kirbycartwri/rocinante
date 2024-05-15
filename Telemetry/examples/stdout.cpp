#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <Telemetry.h>

/*
 * Todo: licensing
*/

#define SENSOR_REFRESH_TIME_MICROSECONDS 30000


static Telemetry *myTelemetry = NULL;

static int32_t firstItemNumber  = 0;
static int32_t secondItemNumber = 1;

static struct timespec thisTime, lastTime;

static int32_t nTicks = 100;

static void teardown()
{

    myTelemetry->stopTelemetry();

    delete myTelemetry;
    myTelemetry = NULL;
}

static void setup()
{
    myTelemetry = new Telemetry();

	(void)clock_gettime(CLOCK_REALTIME, &thisTime);
	(void)clock_gettime(CLOCK_REALTIME, &lastTime);	    

    myTelemetry->writeItemValueHeader("Time", "s", firstItemNumber);
    myTelemetry->writeItemValueHeader("Random", "\0", secondItemNumber);

    srandom(thisTime.tv_nsec);    

    myTelemetry->startTelemetry();

    (void)sleep(Telemetry::INITIAL_DELAY_PERIOD);
}

static void loop() 
{
    (void)usleep(SENSOR_REFRESH_TIME_MICROSECONDS);
	
	(void)clock_gettime(CLOCK_REALTIME, &thisTime);

	double_t deltaT = 0.0;

	time_t tv_delta_sec = ( thisTime.tv_sec -  lastTime.tv_sec  );
	long tv_delta_ns	= ( thisTime.tv_nsec - lastTime.tv_nsec );

	deltaT = tv_delta_ns;
	deltaT /= 1e9;
	deltaT += tv_delta_sec;

    double_t dRandom = random();

    myTelemetry->writeItemValue(deltaT, firstItemNumber);

    myTelemetry->writeItemValue(dRandom, secondItemNumber);

    myTelemetry->update();
	
  
}

int main(void)
{
	setup();
	while(nTicks--)
		loop();

    teardown();    
	return 0;	
}
