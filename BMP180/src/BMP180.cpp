/*
BMP180.cpp - Class file for the BMP180/BMP180 Barometric Pressure & Temperature Sensor Arduino Library.

Version: 1.0.2
(c) 2014 Korneliusz Jarzebski
www.jarzebski.pl

This library uses integer equations from BMP180 Datasheet

This library use floating-point equations from the Weather Station Data Logger project
http://wmrx00.sourceforge.net/
http://wmrx00.sourceforge.net/Arduino/BMP180-Calcs.pdf

This program is free software: you can redistribute it and/or modify
it under the terms of the version 3 GNU General Public License as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
* 
* Todo: update licensing.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "BMP180.h"

/*

/sys/bus/iio/devices/iio:device0

-rw-r--r-- 1 root root 4096 Dec 22 17:28 in_pressure_input
-rw-r--r-- 1 root root 4096 Dec 22 17:28 in_pressure_oversampling_ratio
-rw-r--r-- 1 root root 4096 Dec 22 17:28 in_temp_input
-rw-r--r-- 1 root root 4096 Dec 22 17:28 in_temp_oversampling_ratio
-r--r--r-- 1 root root 4096 Dec 21 23:39 name
*/

const bool BMP180::bDebug = false;

const char *BMP180::getName(void)
{
	const char *pName =
	{
		IIO_PATH_PREFACE "/name"
	};

	if ( bDebug )
		(void)printf("\nTrying to open \"%s.\"\n", pName);

	pHandle = fopen(pName,"r");
	
	if ( NULL == pHandle )
	{	
		(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pName, strerror(errno));	
		return (const char *)achDeviceName;		
	}

	else if ( bDebug )
		(void)printf("Successfully opened \"%s.\"\n", pName);	
	else
		;
	
	(void)memset(achDeviceName, '\0', sizeof(achDeviceName));
	if ( 1 != fscanf(pHandle, "%s", achDeviceName) )
	{
		(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pName, strerror(errno));
	}
	else if ( bDebug )
	{
		(void)printf("The name reported from \"%s\" is \"%s.\"\n", pName, achDeviceName);
	}
	else
		;

	if ( NULL!=pHandle )
		(void)fclose(pHandle);
	pHandle = NULL;
 	
	return (const char *)achDeviceName;
};

void BMP180::setOversampling(E_BMP180_OSS oss /*= BMP180_ULTRA_HIGH_RES*/)
{
	const char *pPressureOversampling =
	{
		IIO_PATH_PREFACE  "/in_pressure_oversampling_ratio"
	};

	if ( bDebug )
		(void)printf("\nTrying to open \"%s\" for read/write.\n", pPressureOversampling);

	pHandle = fopen(pPressureOversampling,"r+");
	
	if ( NULL == pHandle )
	{	
		(void)printf("Unable to open \"%s!\" You might need to be root.\n\t\"%s\"\n", pPressureOversampling, strerror(errno));	
		return;		
	}

	else if ( bDebug )
		(void)printf("Successfully opened \"%s.\"\n", pPressureOversampling);	
	else
		;	
		
	in_pressure_oversampling_ratio = oss;
		
	if ( 0 > fprintf(pHandle, "%u", in_pressure_oversampling_ratio) )
	{
		(void)printf("Unable to write to \"%s!\" You might need to be root.\n\t\"%s\"\n", pPressureOversampling, strerror(errno));
		return;			 
	}
	else if ( bDebug )
	{
		(void)printf("Wrote oversampling ratio %u.\n", in_pressure_oversampling_ratio);
	}
	else
		;
				
	eOSS = oss;
	if ( NULL!=pHandle )
		(void)fclose(pHandle);
	pHandle = NULL;		
	
}

E_BMP180_OSS BMP180::getOversampling(void)
{
	const char *pPressureOversampling =
	{
		IIO_PATH_PREFACE "/in_pressure_oversampling_ratio"
	};

	if ( bDebug )
		(void)printf("\nTrying to open \"%s.\"\n", pPressureOversampling);

	pHandle = fopen(pPressureOversampling,"r");
	
	if ( NULL == pHandle )
	{	
		(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pPressureOversampling, strerror(errno));	
		return eOSS;		
	}

	else if ( bDebug )
		(void)printf("Successfully opened \"%s.\"\n", pPressureOversampling);	
	else
		;
	
	in_pressure_oversampling_ratio =		// The Linux driver's default value.
		eOSS = BMP180_ULTRA_HIGH_RES;
	
	if ( 1 != fscanf(pHandle, "%u", &in_pressure_oversampling_ratio) )
	{
		(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pPressureOversampling, strerror(errno));
	}
	else if ( bDebug )
	{
		(void)printf("The oversampling value reported from \"%s\" is \"%u.\"\n", pPressureOversampling, in_pressure_oversampling_ratio);
	}
	else
		;

	eOSS = (E_BMP180_OSS)in_pressure_oversampling_ratio;

	if ( NULL!=pHandle )
		(void)fclose(pHandle);
	pHandle = NULL;	
	
	return eOSS;
}

uint32_t BMP180::readRawTemperature(void)
{
	in_temp_input = 0xffffffff;
	
	const char *pTemperatureInput =
	{
		IIO_PATH_PREFACE "/in_temp_input"
	};

	if ( bDebug )
		(void)printf("\nTrying to open \"%s.\"\n", pTemperatureInput);

	pHandle = fopen(pTemperatureInput,"r");	
	
	if ( NULL == pHandle )
	{	
		(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pTemperatureInput, strerror(errno));	
		return in_temp_input;		
	}

	else if ( bDebug )
		(void)printf("Successfully opened \"%s.\"\n", pTemperatureInput);	
	else
		;	
	
	if ( 1 != fscanf(pHandle, "%u", &in_temp_input) )
	{
		(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pTemperatureInput, strerror(errno));
	}
	else if ( bDebug )
	{
		(void)printf("The raw (unzeroed) temperature value reported from \"%s\" is \"%u.\"\n", pTemperatureInput, in_temp_input);
	}
	else
		;	
	
	if ( NULL!=pHandle )
		(void)fclose(pHandle);
	pHandle = NULL;	

	return in_temp_input;
}

const double_t BMP180::RAW_TEMPERATURE_CONVERSION = 1000.0;

double_t BMP180::readTemperature(void)	// in degrees C.
{
	double_t d = readRawTemperature();
	
	d /= RAW_TEMPERATURE_CONVERSION;

	d -= raw_temp_offset;
	
	if ( bDebug )
		(void)printf("The reported temperature is %lf degrees C.\n", d);
	
	return d;
}

double_t BMP180::readPressure(void)
{
	return getPressure() - raw_pressure_offset;
}

double_t BMP180::getPressure(void)		// in mbars.
{
	in_pressure_input = SEALEVEL_PRESSURE_MILLIBARS;
	
	const char *pPressureInput =
	{
		IIO_PATH_PREFACE "/in_pressure_input"
	};

	if ( bDebug )
		(void)printf("\nTrying to open \"%s.\"\n", pPressureInput);

	pHandle = fopen(pPressureInput,"r");		
	
	if ( NULL == pHandle )
	{	
		(void)printf("Unable to open \"%s!\"\n\t\"%s\"\n", pPressureInput, strerror(errno));	
		return in_pressure_input;		
	}

	else if ( bDebug )
		(void)printf("Successfully opened \"%s.\"\n", pPressureInput);	
	else
		;		
	
	if ( 1 != fscanf(pHandle, "%lf", &in_pressure_input) )
	{
		(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", pPressureInput, strerror(errno));
	}

	else
	{
		in_pressure_input *= 10.0;		// I don't know why I need this.	
	}
	
	if ( bDebug )
	{
		(void)printf("The raw, unzeroed, pressure value reported from \"%s\" is \"%lf mbar(s).\"\n", pPressureInput, in_pressure_input);
	}	
	
	if ( NULL!=pHandle )
		(void)fclose(pHandle);
	pHandle = NULL;	
	
	return in_pressure_input;
}

BMP180::BMP180(E_BMP180_OSS e /*= BMP180_ULTRA_HIGH_RES*/) :
	eOSS(e), bCalibrated(false), 
	
	dBaselinePressure(SEALEVEL_PRESSURE_MILLIBARS),
	dBaselineAltitude(DEFAULT_ALTITUDE),

	pHandle(NULL), in_temp_input(0xffffffff),
	in_pressure_oversampling_ratio(BMP180_ULTRA_HIGH_RES), in_temp_oversampling_ratio(1),
	in_pressure_input(SEALEVEL_PRESSURE_MILLIBARS),
	raw_temp_offset(0.0), raw_pressure_offset(0.0),
	raw_altitude_offset(0.0), set_altitude_value(DEFAULT_ALTITUDE), set_pressure_value(DEFAULT_PRESSURE), set_temperature_value(DEFAULT_TEMPERATURE)
{
	// setOversampling(e);
	(void)usleep(BMP180_SAMPLE_DELAY_US);	
	(void)getName();
	(void)usleep(BMP180_SAMPLE_DELAY_US);
	(void)getOversampling();
	(void)usleep(BMP180_SAMPLE_DELAY_US);
	(void)readTemperature();
	(void)usleep(BMP180_SAMPLE_DELAY_US);
	(void)getPressure();
	(void)usleep(BMP180_SAMPLE_DELAY_US);
	(void)getAltitude();
	(void)usleep(BMP180_SAMPLE_DELAY_US);	
}

BMP180::~BMP180()
{
	if ( NULL!=pHandle )
		(void)fclose(pHandle);
	pHandle = NULL;
	bCalibrated = false;
}

// The altitude in meters can be calculated with the international barometric formula:
// H = 44330 * [1 - (P/p0)^(1/5.255) ]
// H = altitude (m)
// P = measured pressure (mbars) from the sensor
// p0 = reference pressure at sea level (e.g. 1013.25 hPa (mb) )

const double_t BMP180::SEALEVEL_PRESSURE_PASCALS	= 101325.0;
// Note: The standard pressure at sea-level is 1013.25 in both millibars (mb) and hectopascal (hPa).

const double_t BMP180::SEALEVEL_PRESSURE_MILLIBARS	= 1013.25;
const double_t BMP180::MILLIBARS_TO_PASCALS 		= 100.0;

// Returns compensated meter(s).
double_t BMP180::readAltitude(void)
{

	return getAltitude(dBaselinePressure)+dBaselineAltitude;

}
 
// Returns meter(s).
double_t BMP180::getAltitude(double_t p0 /*= SEALEVEL_PRESSURE_MILLIBARS*/)
{
	double_t p 	= getPressure();
	
	double_t H = 44330.0f * (1.0f - pow( ( p/p0 ), ( 1.0f / 5.255f ) ) );

	return H;
}

// Calculate sea level from the pressure in pascals given at a specific altitude in meters.
double BMP180::getSeaLevel(double pressure, double altitude)
{
    return ((double)pressure / pow(1.0f - ((double)altitude / 44330.0f), 5.255f));
}

// "Oversampling 8 Conversion time 25.5 ms." 
const uint32_t BMP180::BMP180_SAMPLE_DELAY_US						= 50000;


// Try to make evenly divisible by 3.
const uint32_t BMP180::NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_SECOND 		= 21;

const uint32_t BMP180::NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_MINUTE 		= ( 60 * NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_SECOND );

const uint32_t BMP180::NUMBER_OF_OFFSET_SAMPLES_FOR_FIVE_MINUTES	= ( 5 * NUMBER_OF_OFFSET_SAMPLES_FOR_FIVE_MINUTES );

const double_t BMP180::DEFAULT_TEMPERATURE 							= 22.6, 
	BMP180::DEFAULT_ALTITUDE 										= 334.0,
	BMP180::DEFAULT_PRESSURE										= 1001.02;

void BMP180::getOffsets(const uint32_t uN /*= NUMBER_OF_OFFSET_SAMPLES_FOR_ONE_MINUTE*/, const double_t dPressure /*= DEFAULT_PRESSURE*/,
	const double_t dTemperature /*= DEFAULT_TEMPERATURE*/, const double_t dAltitude /*= DEFAULT_ALTITUDE*/ )
{
	bCalibrated = false;

	raw_temp_offset = 0.0;
	raw_pressure_offset = 0.0;
	raw_altitude_offset = 0.0, set_altitude_value = dAltitude, set_pressure_value = dPressure, 
		set_temperature_value = dTemperature;


	double_t dRawTempSum 		= 0.0;
	double_t dRawPressureSum 	= 0.0;
	double_t dRawAltitudeSum 	= 0.0;

#if 0
	// throw away some initial samples?
	for ( uint32_t i = 0 ; i < uN ; i++ )
	{
		(void)readTemperature();
		(void)usleep(BMP180_SAMPLE_DELAY_US);		
		(void)readPressure();
		(void)usleep(BMP180_SAMPLE_DELAY_US);
		(void)getAltitude();
		(void)usleep(BMP180_SAMPLE_DELAY_US);
	}
#endif

	for ( uint32_t i = 0 ; i < uN ; i++ )
	{
		dRawTempSum += readTemperature();
		(void)usleep(BMP180_SAMPLE_DELAY_US);		
		dRawPressureSum += getPressure();
		(void)usleep(BMP180_SAMPLE_DELAY_US);		
		dRawAltitudeSum += getAltitude(0.0);
		(void)usleep(BMP180_SAMPLE_DELAY_US);
	}

#if 0
	(void)printf("\nThe uncompensated temperature is %lf degree(s) C.\n", readTemperature());
	(void)usleep(BMP180_SAMPLE_DELAY_US);
	(void)printf("The uncompensated barometric pressure is %lf mbar(s).\n", getPressure());
	(void)usleep(BMP180_SAMPLE_DELAY_US);	
	(void)printf("The uncompensated altitude is %lf meter(s).\n", getAltitude());	
	(void)usleep(BMP180_SAMPLE_DELAY_US);
#endif

	if ( 0.0 > dRawTempSum )
		dRawTempSum -= (double_t)uN/2;
	else
		dRawTempSum += (double_t)uN/2;

	dRawTempSum /= (double_t)uN;
	raw_temp_offset = dRawTempSum;

	if ( 0.0 > dRawPressureSum )
		dRawPressureSum -= (double_t)uN/2;
	else
		dRawPressureSum += (double_t)uN/2;

	if ( 0.0 > dRawAltitudeSum )
		dRawAltitudeSum -= (double_t)uN/2;
	else
		dRawAltitudeSum += (double_t)uN/2;

	raw_pressure_offset = dRawPressureSum / (double_t)uN;

	dBaselinePressure = raw_pressure_offset;

	raw_altitude_offset = dRawAltitudeSum / (double_t)uN;
	raw_temp_offset     = dTemperature - raw_temp_offset;
	raw_pressure_offset = dPressure - raw_pressure_offset;
	raw_altitude_offset = dAltitude - raw_altitude_offset;

	if ( bDebug )
	{
		(void)printf("\n\nThe temperature offset is %lf degree(s) C.\n", raw_temp_offset);
		(void)printf("The pressure offset is %lf mbar(s).\n", raw_pressure_offset);
		(void)printf("The altitude offset is %lf meter(s).\n", raw_altitude_offset);

		(void)usleep(BMP180_SAMPLE_DELAY_US);
		(void)printf("\nThe temperature is %lf degree(s) C.\n", readTemperature());
		(void)usleep(BMP180_SAMPLE_DELAY_US);		
		(void)printf("The barometric pressure is %lf mbar(s).\n", getPressure());
		(void)usleep(BMP180_SAMPLE_DELAY_US);		
		(void)printf("The altitude is %lf meter(s).\n", getAltitude());		

		(void)printf("\nThe set temperature is %lf degree(s) C.\n", dTemperature);
		(void)printf("The set barometric pressure is %lf mbar(s).\n", dPressure);
		(void)printf("The set altitude is %lf meter(s).\n\n", dAltitude);	

	}

	bCalibrated = true;

}
