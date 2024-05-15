#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <SimpleKalmanFilter.h>
#include <BMP180.h>

/*
 * Todo: licensing
*/
// For oversampling = 8, the conversion time is 25.5 ms.
#define SENSOR_REFRESH_TIME_MICROSECONDS 30000

SimpleKalmanFilter altitudeKalmanFilter(1, 1, 0.01);
SimpleKalmanFilter temperatureKalmanFilter(1, 1, 0.01);
SimpleKalmanFilter pressureKalmanFilter(1, 1, 0.01);

BMP180 pressureSensor(BMP180_ULTRA_HIGH_RES);

static void setup()
{
	pressureSensor.getOffsets();
	
	(void)printf("temperature, estimated_temperature, compensated altitude, estimated_altitude, pressure, estimated_pressure\n");
}

static void loop() 
{
	int32_t refresh_time = 10;
	
	double_t temperature 			= 25.0,
		estimated_temperature 		= 25.0,
		pressure					= BMP180::SEALEVEL_PRESSURE_MILLIBARS,
		estimated_pressure 			= BMP180::SEALEVEL_PRESSURE_MILLIBARS,
		altitude 					= 0.0,
		estimated_altitude 			= 0.0;

		temperature 				= pressureSensor.readTemperature();
		estimated_temperature 		= temperatureKalmanFilter.updateEstimate(temperature),
		pressure					= pressureSensor.readPressure(),
		estimated_pressure 			= pressureKalmanFilter.updateEstimate(pressure),
		altitude 					= pressureSensor.readAltitude(),
		estimated_altitude 			= altitudeKalmanFilter.updateEstimate(altitude);
	
	if ( 10 == refresh_time )
	{
		refresh_time = 0;
		(void)printf("%lf, %lf, %lf, %lf, %lf, %lf\n", temperature, estimated_temperature, altitude, estimated_altitude, pressure, estimated_pressure);
	}
	else
	{
		refresh_time++;
	}

	(void)usleep(SENSOR_REFRESH_TIME_MICROSECONDS);
  
}

int main(void)
{
	setup();
	while(true)
		loop();
	return 0;	
}
