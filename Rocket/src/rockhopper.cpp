/*
	Rockhopper.cpp - Inherited Rockhopper (EDF, electri-ducted-fan) class for Raspberry PI; Version 1.
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

#include "rockhopper.h"

// Todo: update this:
const double_t Rockhopper::ROCKHOPPER_MASS = 500;   // g

Rockhopper::Rockhopper(const char *RockhopperName /*= "rockhopper. a simulated rocket with an EDF."*/) :
    Rocket(RockhopperName), pressureSensor(NULL), orientationSensor(NULL), canineGimbal(NULL),
    controlSystem(NULL), rocketEDF(NULL), stdoutTelemetry(NULL)
{
    dRocketMass         = ROCKHOPPER_MASS;

    pressureSensor      = new BMP180(BMP180_ULTRA_HIGH_RES);
    orientationSensor   = new BNO055();
    canineGimbal        = new K9TvcGimbal();
    controlSystem       = new RockHopperControl();
    rocketEDF           = new DoBoFo70Pro12(E_JET_0, E_PWM_2);
    stdoutTelemetry     = new Telemetry();
    locationGPS         = new GPS("GoouuTech (Beffkkip) GT-U7 Ublox NEO-6M GPS", E_GPS_NUM_0); 

    (void)memset(&scalibratePressureThread, 0, sizeof(pthread_t));
    (void)memset(&sCalibrateImuThread, 0, sizeof(pthread_t));    

    setFeedback(E_FEEDBACK_OFF);
    update();

}

Rockhopper::~Rockhopper()
{
    setFeedback(E_FEEDBACK_OFF);
    update();    

    delete pressureSensor, pressureSensor = NULL;
    delete orientationSensor, orientationSensor = NULL;
    delete canineGimbal, canineGimbal = NULL;
    delete controlSystem, controlSystem = NULL;
    delete rocketEDF, rocketEDF = NULL;
    delete stdoutTelemetry, stdoutTelemetry = NULL;
    delete locationGPS, locationGPS = NULL;

}

double_t Rockhopper::getTemperature(void)
{
    return pressureSensor->readTemperature();
}
double_t Rockhopper::getPressure(void)
{
    return pressureSensor->readPressure();
}
double_t Rockhopper::getAltitude(void)
{
    return pressureSensor->readAltitude();
}

void Rockhopper::setOrientationDegrees(const double_t &dPitch, const double_t &dRoll, const double_t &dYaw)
{
    controlSystem->SetControlledInputAngleDegreesValues(dPitch, dRoll, dYaw);
}

void Rockhopper::getOrientationDegrees(double_t &dPitch, double_t &dRoll, double_t &dYaw)
{
    controlSystem->GetControlledInputAngleDegreesValues(dPitch, dRoll, dYaw);
}

void Rockhopper::readOrientationDegrees(double_t &dPitch, double_t &dRoll, double_t &dYaw)
{
    double_t dP = 0.0, dR = 0.0, dY = 0.0;
    orientationSensor->readOrientation(dP, dR, dY);

    // I don't know why pitch and roll and yaw need these.

    if ( -180.0 > dP )
        dP += 360.0;
    else if ( 180.0 < dP )
        dP = -( 360.0 - dP );
    else 
        dP = dP;    

    if ( -180.0 > dR )
        dR += 360.0;        
    else if ( 180.0 < dR )
        dR = -( 360.0 - dR );
    else 
        dR = dR;
   
   
    if ( -180.0 > dY )
        dY += 360.0;
    else if ( 180.0 < dY )
        dY = ( 360.0 -dY );    
    else
        dY = dY;

     // 03/10/2024 Upside down sensor.
    // dPitch = dP, dRoll = dY,  dYaw = -dR;
    // 03/23/2024 BNO055 sensor facing up, servo connectors facing you.
    dPitch = -dP, dRoll = dY, dYaw = dR;
}

void Rockhopper::getAngularVelocities(double_t &x, double_t &y, double_t &z)
{
    double_t xX = 0.0, yY = 0.0, zZ = 0.0;
    orientationSensor->readGyroscope(xX, yY, zZ);
    // 03/27/2024 BNO055 sensor facing up, servo connectors facing you.
    x = -xX, y = zZ, z = yY;
}

void Rockhopper::startTelemetry(void)
{
    stdoutTelemetry->startTelemetry();
}
void Rockhopper::stopTelemetry(void)
{
    stdoutTelemetry->stopTelemetry();
}

bool Rockhopper::getTelemetry(void)
{
    return stdoutTelemetry->getTelemetry();
}

void Rockhopper::setFeedback(const E_FEEDBACK_MODE &e)
{
    const E_FEEDBACK_MODE ePitch = e,
        eRoll = E_FEEDBACK_OFF,
        eYaw = e;
    controlSystem->SetControlledAxes(ePitch, eRoll, eYaw);
}

void Rockhopper::getFeedback(E_FEEDBACK_MODE &e)
{
    E_FEEDBACK_MODE ePitch = E_FEEDBACK_OFF,
        eRoll = E_FEEDBACK_OFF,
        eYaw = E_FEEDBACK_OFF;

    controlSystem->GetControlledAxes(ePitch, eRoll, eYaw);

    e = ePitch;   

}

void Rockhopper::throttle(double_t position /*= 0.0*/)
{
    rocketEDF->throttle(position);
}
double_t Rockhopper::throttlePosition(void)
{
    return rocketEDF->throttlePosition();
}
double_t Rockhopper::thrust(void)
{
    return rocketEDF->thrust();
}

void Rockhopper::update(void)
{
    double_t dPitch = 0.0, 
        dRoll       = 0.0, 
        dYaw        = 0.0;

    double_t dPitchRate = 0.0, 
        dRollRate       = 0.0, 
        dYawRate        = 0.0;

    readOrientationDegrees(dPitch, dRoll, dYaw);
    getAngularVelocities(dPitchRate, dRollRate, dYawRate);

	controlSystem->SetInputAngleDegreesValues(dPitch, dRoll, dYaw);
	controlSystem->SetInputAngularVelocityRadiansPerSecondValues(dPitchRate, dRollRate, dYawRate);

    controlSystem->update();

    controlSystem->GetControlledOutputAngleDegreesValues(dPitch, dRoll, dYaw);

    canineGimbal->writeAngleDegrees(E_PITCH_AXIS, dPitch);
    canineGimbal->writeAngleDegrees(E_YAW_AXIS, dYaw);    

}

void Rockhopper::getAccelerations(double_t &x, double_t &y, double_t &z)
{
	orientationSensor->readAccelerations(x, y, z);
}

void Rockhopper::calibrateSensors(void)
{

    bCalibrated = false;

    int32_t iRet = pthread_create( &scalibratePressureThread, NULL, calibratePressureSensorBackground, (void*) this);

    if ( 0 != iRet )
    {
        (void)fprintf(stderr, "%s: thread creation error!\n\t\"%s\"", __FUNCTION__, strerror(iRet));
        return;
    }
    else if ( bDebug )
        (void)printf("Pressure sensor calibration thread created successfully.\n");
    else
        ;

    iRet = pthread_create( &sCalibrateImuThread, NULL, calibrateOrientationSensorBackground, (void*) this);

    if ( 0 != iRet )
    {
        (void)fprintf(stderr, "%s: thread creation error!\n\t\"%s\"", __FUNCTION__, strerror(iRet));
        (void)pthread_cancel(scalibratePressureThread);
        return;
    }
    else if ( bDebug )
        (void)printf("Pressure sensor calibration thread created successfully.\n");
    else
        ;

    // The following are not needed unless you want to wait. Just monitor "calibrated."
    // (void)pthread_join( scalibratePressureThread, NULL);
    // (void)pthread_join( sCalibrateImuThread, NULL); 

}

void *Rockhopper::calibratePressureSensorBackground( void *pContext )
{
    Rockhopper *pThis = (Rockhopper *)pContext;
    pThis->pressureSensor->getOffsets(BMP180::NUMBER_OF_OFFSET_SAMPLES_FOR_FIVE_MINUTES, DEFAULT_PRESSURE, DEFAULT_ALTITUDE);
    return NULL;
}

void *Rockhopper::calibrateOrientationSensorBackground( void *pContext )
{
    Rockhopper *pThis = (Rockhopper *)pContext;
    pThis->orientationSensor->getOffsets(BNO055::NUMBER_OF_OFFSET_SAMPLES_FOR_FIVE_MINUTES);
    return NULL;
}

bool Rockhopper::calibrated(void)
{
    bCalibrated = pressureSensor->calibrated() && orientationSensor->calibrated();
    return bCalibrated;
}

void Rockhopper::readLocatation(double_t &dLatitude, double_t &dLongitude, double_t &dAltitude)
{
    locationGPS->readGpsLocation(dLatitude, dLongitude, dAltitude);
}

void Rockhopper::readLocatationXYZ(double_t &dX, double_t &dY, double_t &dZ)
{
    locationGPS->readGpsLocationXYZ(dX, dY, dZ);
}

void Rockhopper::setup(void)
{
	locationGPS->setup();
}
void Rockhopper::loop(void)
{
	locationGPS->loop();
}
void Rockhopper::teardown(void)
{
	locationGPS->teardown();
}

void Rockhopper::readLocationTime(double_t dTime)
{
    locationGPS->readGpsTime(dTime);
}