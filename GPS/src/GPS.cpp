/*
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "GPS.h"

const bool GPS::bDebug = false;

GPS::GPS(const char *GPSName, E_GPS sIndex/*=E_GPS_NUM_UNDEFINED*/) :
	GPSIndex(sIndex), gpsLatitude(47.37), gpsLongitude(122.162), gpsAltitude(283), gpsX(0.0), gpsY(0.0), gpsZ(0.0),
    bOpened(false), bContinue(false),
    gpsTimeoutMicroseconds(5000000) /* 5 seconds timeout */
    
{
	if ( NULL!=GPSName )
	{
		(void)strncpy(achGPSName, GPSName, sizeof(achGPSName));
	}

    (void)memset((void *)&gpsData, 0, sizeof(gps_data_t));
    (void)memset((void *)&gpsReadThreadStrct, 0, sizeof(pthread_t));
    (void)memset((void *)&lockMutex, 0, sizeof(pthread_mutex_t));

    convertGPSLocationToXYCoordinates(gpsLatitude, gpsLongitude, gpsX, gpsY);

    (void)clock_gettime(CLOCK_REALTIME, &timeLatest);

}

GPS::~GPS()
{
	GPSIndex = E_GPS_NUM_UNDEFINED;
	(void)memset(achGPSName, '\0', sizeof(achGPSName));
	(void)memset(&timeLatest, 0, sizeof(timeLatest)); 
}

const char *GPS::gpsHost = "localhost",
    *GPS::gpsPort = "2947";

void GPS::setup(void)
{
    int32_t nReturn = gps_open(gpsHost, gpsPort, &gpsData);

    if ( 0 != nReturn ) 
    {
        (void)fprintf(stderr, "%s: gps open error!\n\t\"%s\"\n", __FUNCTION__, gps_errstr(nReturn));
        return;
    }
    else if ( bDebug )
    {
        (void)fprintf(stdout, "%s: gps open OK.\n", __FUNCTION__);

    }
    else
        ;

    (void)gps_stream(&gpsData, WATCH_ENABLE | WATCH_JSON, NULL);

    (void)pthread_mutex_init(&lockMutex, NULL);

    nReturn = pthread_create( &gpsReadThreadStrct, NULL, &gpsReadThread, ( void * ) this);

    if ( 0 != nReturn ) 
    {
        (void)fprintf(stderr, "%s: GPS read thread creation error!\n\t\"%s\"\n", __FUNCTION__, strerror(errno));
        return;
    }
    else if ( bDebug )
    {
        (void)fprintf(stdout, "%s: GPS read thread created.\n", __FUNCTION__);

    }
    else
        ;

    bOpened = bContinue = true;

}
void GPS::loop(void)
{
    getGPS();
}
void GPS::teardown(void)
{
    if ( bContinue )
    {
        bContinue = false;
        (void)pthread_join( gpsReadThreadStrct, NULL);
        (void)pthread_mutex_destroy(&lockMutex);        
    }

    if ( bOpened )
    {
        bOpened = false;
        (void)gps_stream(&gpsData, WATCH_DISABLE, NULL);
        (void)gps_close(&gpsData);        
    }
}


void GPS::convertGPSLocationToXYCoordinates(double_t &latitude, double_t &longitude, double_t &x, double_t &y)
{
    // Convert latitude and longitude to radians.
    double_t latitudeRadians = latitude * M_PI / 180.0;
    double_t longitudeRadians = longitude * M_PI / 180.0;

    // Calculate the X and Y coordinates.
    x = longitudeRadians * cos(latitudeRadians);
    y = latitudeRadians;

}

void *GPS::gpsReadThread( void *ptr )
{
    GPS *thisSensor = ( GPS *)ptr;

    int32_t nReturn = -1;

    if ( NULL!= thisSensor )
    {
        (void)usleep(thisSensor->gpsTimeoutMicroseconds);
        while ( thisSensor->bContinue ) 
        {

            if (  !gps_waiting(&thisSensor->gpsData, thisSensor->gpsTimeoutMicroseconds ) ) 
            {
                (void)fprintf(stderr, "%s: GPS wait error or timeout!\n\t\"%s\"\n", __FUNCTION__, strerror(errno));
                continue;
            }
            else if ( bDebug )
            {
                (void)fprintf(stdout, "%s: GPS wait OK.\n", __FUNCTION__);

            }
            else
                ;

            nReturn = gps_read(&thisSensor->gpsData, NULL, 0);

            if ( 0 >= nReturn  ) 
            {
                (void)fprintf(stderr, "%s: GPS read error!\n\t\"%s\"\n", __FUNCTION__, strerror(errno));            
                continue;
            }
            else if ( bDebug )
            {
                (void)fprintf(stdout, "%s: GPS read OK.\n", __FUNCTION__);
            }
            else
                ;
        
            if ( STATUS_NO_FIX == thisSensor->gpsData.fix.status )
            {
                if ( bDebug )
                {
                    (void)fprintf(stdout, "%s: No GPS fix.\n", __FUNCTION__);
                }
                ;
            }

            if ( MODE_SET != ( MODE_SET & thisSensor->gpsData.set ) ) 
            {
                // did not even get mode, nothing to see here.
                if ( bDebug )
                {
                    (void)fprintf(stdout, "%s: No GPS data available.\n", __FUNCTION__);
                }
                continue;
            }
        
            (void)pthread_mutex_lock(&thisSensor->lockMutex);
            {
                if ( bDebug )
                {
                    (void)fprintf(stdout, "%s: mutex grabbed.\n", __FUNCTION__);
                }

                if ( TIME_SET == ( TIME_SET & thisSensor->gpsData.set ) )
                {
                    thisSensor->timeLatest = thisSensor->gpsData.fix.time;
                    if ( bDebug )
                    {
                        (void)fprintf(stdout, "%s: time copied.\n", __FUNCTION__);
                    }                    
                }

                if ( isfinite( thisSensor->gpsData.fix.latitude ) )
                {
                    thisSensor->gpsLatitude = thisSensor->gpsData.fix.latitude;
                    if ( bDebug )
                    {
                        (void)fprintf(stdout, "%s: latitude (%lf degrees) copied.\n", __FUNCTION__, thisSensor->gpsLatitude);
                    }                    
                }

                if ( isfinite( thisSensor->gpsData.fix.longitude ) )
                {
                    thisSensor->gpsLongitude = thisSensor->gpsData.fix.longitude;
                    if ( bDebug )
                    {
                        (void)fprintf(stdout, "%s: longitude (%lf degrees) copied.\n", __FUNCTION__, thisSensor->gpsLongitude);
                    }                   
                }

                thisSensor->gpsAltitude = thisSensor->gpsData.fix.altitude;
                if ( bDebug )
                {
                    (void)fprintf(stdout, "%s: altitude (%lf meters) copied.\n", __FUNCTION__, thisSensor->gpsAltitude);
                }

            }
 
            (void)pthread_mutex_unlock(&thisSensor->lockMutex);
            
            if ( bDebug )
            {
                (void)fprintf(stdout, "%s: mutex released.\n", __FUNCTION__);
            }
        }
    }

    thisSensor = NULL;
    return thisSensor;

}

void GPS::readGpsLocation(double_t &latitude, double_t &longitude, double &altitude)
{
    (void)pthread_mutex_lock(&lockMutex);
    {

        latitude = gpsLatitude;
        longitude = gpsLongitude;
        altitude = gpsAltitude;
    }
    (void)pthread_mutex_unlock(&lockMutex); 
}
void GPS::readGpsTime(double_t &seconds)
{
    (void)pthread_mutex_lock(&lockMutex);
    {
        seconds = (double_t)timeLatest.tv_nsec;
        seconds /= 1e9;
        seconds += (double_t)timeLatest.tv_sec;
    }
    (void)pthread_mutex_unlock(&lockMutex); 
}
void GPS::readGpsLocationXYZ(double_t &X, double_t &Y, double &Z)
{

    double_t latitude = 0.0, longitude = 0.0, altitude = 0.0;
    readGpsLocation(latitude, longitude, altitude);

    convertGPSLocationToXYCoordinates(latitude, longitude, X, Y);
    Z = altitude;

}
void GPS::getGPS(void)
{   
    ;
}





