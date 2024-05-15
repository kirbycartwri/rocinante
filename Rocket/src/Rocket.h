/*
	Rocket.h - Base Rocket class for Raspberry PI; Version 1.
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

#ifndef	_ROCKET_H
#define _ROCKET_H

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <float.h>
#include "Control.h"

#define ROCKET_VERSION	1     			// the software version of this library

const double_t DEFAULT_ALTITUDE = 345 * 0.3048; // M
const double_t DEFAULT_PRESSURE = 997.44;       // hPa

class Rocket
{
public:
	Rocket(const char *RocketName = NULL);
	virtual ~Rocket();

	virtual double_t getTemperature(void);
	virtual double_t getPressure(void);
	virtual double_t getAltitude(void);

	virtual void setup(void);
	virtual void loop(void);
	virtual void teardown(void);	

	virtual bool calibrated(void);

	virtual void calibrateSensors(void);

    virtual void setBaseAltitude(const double_t dAltitude = DEFAULT_ALTITUDE );

	virtual void setOrientationDegrees(const double_t &dPitch, const double_t &dRoll, const double_t &dYaw);

	virtual void getOrientationDegrees(double_t &dPitch, double_t &dRoll, double_t &dYaw);

	virtual void readOrientationDegrees(double_t &dPitch, double_t &dRoll, double_t &dYaw);

	virtual void readLocatation(double_t &dLatitude, double_t &dLongitude, double_t &dAltitude);

	virtual void readLocatationXYZ(double_t &dX, double_t &dY, double_t &dZ);

	virtual void readLocationTime(double_t dTime);

    virtual void getAngularVelocities(double_t &x, double_t &y, double_t &z);

    virtual void getAccelerations(double_t &x, double_t &y, double_t &z);

    virtual void startTelemetry(void);
    virtual void stopTelemetry(void); 
    virtual bool getTelemetry(void); 

    virtual void setFeedback(const E_FEEDBACK_MODE &e);
	virtual void getFeedback(E_FEEDBACK_MODE &e);

	virtual void throttle(double_t position = 0.0);
	virtual double_t throttlePosition(void);
	virtual double_t thrust(void);

    static const double_t DEFAULT_ALTITUDE;
    static const double_t DEFAULT_PRESSURE;
    static const double_t ROOM_TEMPERATURE;

    virtual void update(void);

protected:
    static const bool bDebug;
	bool bCalibrated;
	char achControlName[FILENAME_MAX];    

    double_t dBaseAltitude;

    double_t dRocketMass;

private:

};

#endif // _ROCKET_H