/*
	Rocket.cpp - Base Rocket class for Raspberry PI; Version 1.
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

#include "Rocket.h"

const bool Rocket::bDebug = true;

Rocket::Rocket(const char *rocketName) :
    bCalibrated(false), dBaseAltitude(DEFAULT_ALTITUDE), dRocketMass(0.0)
{
	if ( NULL!=rocketName )
		(void)strncpy(achControlName, rocketName, sizeof(achControlName));
	else
		(void)memset(achControlName, '\0', sizeof(achControlName));

}

Rocket::~Rocket()
{
	;
}

/* At my desk.*/
const double_t Rocket::DEFAULT_ALTITUDE = 345 * 0.3048; // M
const double_t Rocket::DEFAULT_PRESSURE = 997.44;       // hPa
const double_t Rocket::ROOM_TEMPERATURE = 25.4;         // celsius

double_t Rocket::getTemperature(void)
{
    return ROOM_TEMPERATURE;
}
double_t Rocket::getPressure(void)
{
    return DEFAULT_PRESSURE;
}
double_t Rocket::getAltitude(void)
{
    return DEFAULT_ALTITUDE;
}

void Rocket::setBaseAltitude(const double_t dAltitude /*= DEFAULT_ALTITUDE*/)
{
    dBaseAltitude = dAltitude;
}

void Rocket::setOrientationDegrees(const double_t &dPitch, const double_t &dRoll, const double_t &dYaw)
{
    ;
}

void Rocket::getOrientationDegrees(double_t &dPitch, double_t &dRoll, double_t &dYaw)
{
	dPitch = dRoll = dYaw = 0.0;
}

void Rocket::readOrientationDegrees(double_t &dPitch, double_t &dRoll, double_t &dYaw)
{
    dPitch = dRoll = dYaw = 0.0;
}

void Rocket::getAngularVelocities(double_t &x, double_t &y, double_t &z)
{
    x = y = z = 0.0;    // radians-per-second
}

void Rocket::startTelemetry(void)
{
    ;
}
void Rocket::stopTelemetry(void)
{
    ;
} 

bool Rocket::getTelemetry(void)
{
	return false;
}

void Rocket::throttle(double_t position /*= 0.0*/)
{
    ;
}

double_t Rocket::throttlePosition(void)
{
    return 0.0;
}
                            
double_t Rocket::thrust(void)
{
    return 0.0;
}

void Rocket::setFeedback(const E_FEEDBACK_MODE &e)
{
    ;
}

void Rocket::getFeedback(E_FEEDBACK_MODE &e)
{
    e = E_FEEDBACK_OFF;
}

void Rocket::update(void)
{
    ;
}

void Rocket::getAccelerations(double_t &x, double_t &y, double_t &z)
{
	x = y =z = 0.0;
}

void Rocket::calibrateSensors(void)
{
	bCalibrated = false;

}

bool Rocket::calibrated(void)
{
	return bCalibrated;
}

void Rocket::readLocatation(double_t &dLatitude, double_t &dLongitude, double_t &dAltitude)
{
	dLatitude = dLongitude = dAltitude = 0.0;
}

void Rocket::readLocatationXYZ(double_t &dX, double_t &dY, double_t &dZ)
{
	dX = dY = dZ = 0.0;
}

void Rocket::setup(void)
{
	;
}
void Rocket::loop(void)
{
	;
}
void Rocket::teardown(void)
{
	;
}

void Rocket::readLocationTime(double_t dTime)
{
	dTime = 0.0;
}
