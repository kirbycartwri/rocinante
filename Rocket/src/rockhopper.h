/*
	Rockhopper.h - Inherited Rockhopper (EDF, electric-ducted-fan) class for Raspberry PI; Version 1.
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

#ifndef	_ROCKHOPPER_H
#define _ROCKHOPPER_H

#include <pthread.h>
#include <gps.h>
#include "Rocket.h"
#include "BMP180.h"
#include "BNO055.h"
#include "K_9_TVC_Gimbal_Generation_2.h"
#include "RockHopperControl.h"
#include "DoBoFo70Pro12.h"
#include "Gimbal.h"
#include "Control.h"
#include "Jet.h"
#include "Telemetry.h"
#include "GPS.h"

#define ROCKHOPPER_VERSION	1     			// the software version of this library

class Rockhopper : public Rocket
{
public:
	Rockhopper(const char *RockhopperName = "rockhopper. a electric-ducted-fan rocket with an EDF.");

	virtual ~Rockhopper();

	// Todo: get this.
	static const double_t ROCKHOPPER_MASS;

	virtual void setup(void);
	virtual void loop(void);
	virtual void teardown(void);		

	virtual double_t getTemperature(void);
	virtual double_t getPressure(void);
	virtual double_t getAltitude(void);

	virtual bool calibrated(void);

	virtual void calibrateSensors(void);

	static void *calibratePressureSensorBackground( void *pContext );		
	static void *calibrateOrientationSensorBackground( void *pContext );	

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

    virtual void update(void);

protected:    

private:
    BMP180 *pressureSensor;
    BNO055 *orientationSensor;
    K9TvcGimbal *canineGimbal;
    RockHopperControl *controlSystem;
    DoBoFo70Pro12 *rocketEDF;
    Telemetry *stdoutTelemetry;
	GPS *locationGPS;

	pthread_t scalibratePressureThread, sCalibrateImuThread;

};

#endif // _ROCKHOPPER_H
