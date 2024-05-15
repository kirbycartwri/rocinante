/*
	GPS.h - Simple GPS class for Raspberry PI; Version 1.
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

#ifndef	_GPS_h
#define _GPS_h

#include <stdio.h>
#include <inttypes.h>
#include <gps.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

#define GPS_VERSION	1     		// software version of this library

typedef enum 
{
	E_GPS_NUM_UNDEFINED	= -1,
	E_GPS_NUM_0			= 0,	
	NUM_GPSES 	        = 1     // The max number of GPSs; i.e., 1.
} E_GPS;


class GPS
{
public:
	GPS(const char *GPSName, E_GPS sIndex=E_GPS_NUM_0);
	virtual ~GPS();

    // Useful everywhere.
    static void convertGPSLocationToXYCoordinates(double_t &latitude, double_t &longitude, double_t &x, double_t &y);

	virtual void setup(void);
	virtual void loop(void);
	virtual void teardown(void);

	virtual void readGpsLocation(double_t &latitude, double_t &longitude, double &altitude);
	virtual void readGpsTime(double_t &seconds);
	virtual void readGpsLocationXYZ(double_t &X, double_t &Y, double &Z);

protected:

    virtual void getGPS(void);
	
	E_GPS GPSIndex;            // Index for this GPS.

	double_t gpsLatitude, gpsLongitude, gpsAltitude, gpsX, gpsY, gpsZ;

	char achGPSName[FILENAME_MAX];

	static const bool bDebug;

    volatile bool bOpened, bContinue;

    struct gps_data_t gpsData;

    int32_t gpsTimeoutMicroseconds;

	pthread_t gpsReadThreadStrct;

	pthread_mutex_t lockMutex;

	timespec timeLatest;

private:

    static const char *gpsHost, *gpsPort;

	static void *gpsReadThread( void *ptr );

};

#endif	// _GPS_h
