

/*
	K9TvcGimbal.cpp - K9TvcGimbal class for Raspberry PI; Version 1.
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

#include "Jet.h"
#include "K_9_TVC_Gimbal_Generation_2.h"

#if 1   // 03/29/2024


#define SERVO_MIN_YAW_ANGLE_DEGREES         (0.0)
#define SERVO_MAX_YAW_ANGLE_DEGREES	        (180.0)
#define SERVO_CENTER_YAW_ANGLE_DEGREES	    (103.0)
#define SERVO_DEFAULT_YAW_ANGLE_DEGREES	    (103.0)

#define SERVO_MIN_PITCH_ANGLE_DEGREES	    (0.0)
#define SERVO_MAX_PITCH_ANGLE_DEGREES	    (180.0)
#define SERVO_CENTER_PITCH_ANGLE_DEGREES    (80.0)
#define SERVO_DEFAULT_PITCH_ANGLE_DEGREES   (80.0)


#elif 0

#define SERVO_MIN_YAW_ANGLE_DEGREES         (0.0)
#define SERVO_MAX_YAW_ANGLE_DEGREES	        (180.0)
#define SERVO_CENTER_YAW_ANGLE_DEGREES	    (93.0)
#define SERVO_DEFAULT_YAW_ANGLE_DEGREES	    (93.0)

#define SERVO_MIN_PITCH_ANGLE_DEGREES	    (0.0)
#define SERVO_MAX_PITCH_ANGLE_DEGREES	    (180.0)
#define SERVO_CENTER_PITCH_ANGLE_DEGREES    (82.0)
#define SERVO_DEFAULT_PITCH_ANGLE_DEGREES   (82.0)

#else

#define SERVO_MIN_YAW_ANGLE_DEGREES         (83.0)
#define SERVO_MAX_YAW_ANGLE_DEGREES	        (103.0)
#define SERVO_CENTER_YAW_ANGLE_DEGREES	    (93.0)
#define SERVO_DEFAULT_YAW_ANGLE_DEGREES	    (93.0)

#define SERVO_MIN_PITCH_ANGLE_DEGREES	    (67.0)
#define SERVO_MAX_PITCH_ANGLE_DEGREES	    (97.0)
#define SERVO_CENTER_PITCH_ANGLE_DEGREES    (82.0)
#define SERVO_DEFAULT_PITCH_ANGLE_DEGREES   (82.0)

#endif

#define SERVO_MIN_YAW_ANGLE_RADIANS         (SERVO_MIN_YAW_ANGLE_DEGREES * M_PI / 180.0)
#define SERVO_MAX_YAW_ANGLE_RADIANS		    (SERVO_MAX_YAW_ANGLE_DEGREES * M_PI / 180.0)
#define SERVO_CENTER_YAW_ANGLE_RADIANS	    (SERVO_CENTER_YAW_ANGLE_DEGREES * M_PI / 180.0)
#define SERVO_DEFAULT_YAW_ANGLE_RADIANS	    (SERVO_DEFAULT_YAW_ANGLE_DEGREES * M_PI / 180.0)

#define SERVO_MIN_PITCH_ANGLE_RADIANS	    (SERVO_MIN_PITCH_ANGLE_DEGREES * M_PI / 180.0)
#define SERVO_MAX_PITCH_ANGLE_RADIANS	    (SERVO_MAX_PITCH_ANGLE_DEGREES * M_PI / 180.0)
#define SERVO_CENTER_PITCH_ANGLE_RADIANS    (SERVO_CENTER_PITCH_ANGLE_DEGREES * M_PI / 180.0)
#define SERVO_DEFAULT_PITCH_ANGLE_RADIANS   (SERVO_DEFAULT_PITCH_ANGLE_DEGREES * M_PI / 180.0)

K9TvcGimbal::K9TvcGimbal(void) : Gimbal("K-9 TVC Gimbal Generation 2")
{

    bDebug = false;

    // Todo: change PWM numbers to pass parameters.
    apcServos[E_PITCH_AXIS] = new Servo(E_PWM_0);
    apcServos[E_ROLL_AXIS]  = NULL;
    apcServos[E_YAW_AXIS]   = new Servo(E_PWM_1); 

    weight = 70.0;                      // g
    
    diameter = 74.4;                    // mm
    
    height = 74.4;                      // mm

    numberOfParts = 5;                  // Not including screws.

    center_of_mass = 32.2;              // mm

	writeAngleUpperLimitDegrees(    E_PITCH_AXIS, K9_MAX_PITCH_ANGLE_DEGREES);
	writeAngleDefaultDegrees(       E_PITCH_AXIS, K9_DEFAULT_PITCH_ANGLE_DEGREES);
	writeAngleCenterDegrees(        E_PITCH_AXIS, K9_CENTER_PITCH_ANGLE_DEGREES);
	writeAngleLowerLimitDegrees(    E_PITCH_AXIS, K9_MIN_PITCH_ANGLE_DEGREES);  

	writeAngleUpperLimitDegrees(    E_YAW_AXIS,   K9_MAX_YAW_ANGLE_DEGREES);
	writeAngleDefaultDegrees(       E_YAW_AXIS,   K9_DEFAULT_YAW_ANGLE_DEGREES);
	writeAngleCenterDegrees(        E_YAW_AXIS,   K9_CENTER_YAW_ANGLE_DEGREES);
	writeAngleLowerLimitDegrees(    E_YAW_AXIS,   K9_MIN_YAW_ANGLE_DEGREES);  


    apcServos[E_PITCH_AXIS]->writeAngleUpperLimitDegrees(SERVO_MAX_PITCH_ANGLE_DEGREES);
    apcServos[E_PITCH_AXIS]->writeAngleLowerLimitDegrees(SERVO_MIN_PITCH_ANGLE_DEGREES);
    apcServos[E_PITCH_AXIS]->writeAngleCenterDegrees(SERVO_CENTER_PITCH_ANGLE_DEGREES);
    apcServos[E_PITCH_AXIS]->writeAngleDefaultDegrees(SERVO_DEFAULT_PITCH_ANGLE_DEGREES);

    apcServos[E_YAW_AXIS]->writeAngleUpperLimitDegrees(SERVO_MAX_YAW_ANGLE_DEGREES);
    apcServos[E_YAW_AXIS]->writeAngleLowerLimitDegrees(SERVO_MIN_YAW_ANGLE_DEGREES);
    apcServos[E_YAW_AXIS]->writeAngleCenterDegrees(SERVO_CENTER_YAW_ANGLE_DEGREES);
    apcServos[E_YAW_AXIS]->writeAngleDefaultDegrees(SERVO_DEFAULT_YAW_ANGLE_DEGREES);


    writeAngleDegrees(E_PITCH_AXIS, K9_CENTER_PITCH_ANGLE_DEGREES);
    writeAngleDegrees(E_YAW_AXIS,   K9_CENTER_YAW_ANGLE_DEGREES);  


}

K9TvcGimbal::~K9TvcGimbal()
{
    writeAngleDegrees(E_PITCH_AXIS, K9_CENTER_PITCH_ANGLE_DEGREES);
    writeAngleDegrees(E_YAW_AXIS,   K9_CENTER_YAW_ANGLE_DEGREES);

    delete apcServos[E_PITCH_AXIS], apcServos[E_PITCH_AXIS] = NULL;
    delete apcServos[E_YAW_AXIS], apcServos[E_YAW_AXIS]     = NULL;

}

double_t K9TvcGimbal::readAngleDegrees(E_CONTROLLED_AXES e)
{
    if ( E_ROLL_AXIS == e )
        return 0.0;

    double dFromAngle = apcServos[e]->readAngleDegrees(),
        dToAngle = 0.0;

    dToAngle = Jet::map(dFromAngle, apcServos[E_YAW_AXIS]->readAngleLowerLimitDegrees(), apcServos[E_YAW_AXIS]->readAngleUpperLimitDegrees(), 
        dAngleLowerLimits[e], dAngleUpperLimits[e]);


    if ( bDebug )
        (void)printf("\n%s: from Angle = %lf degrees, to Angle = %lf degrees\n", __FUNCTION__, dFromAngle, dToAngle);   

    return dToAngle;
}

double_t K9TvcGimbal::readAngleRadians(E_CONTROLLED_AXES e)
{
    if ( E_ROLL_AXIS == e )
        return 0.0;

    return readAngleDegrees(e) * M_PI / MAX_ANGLE;
}

void K9TvcGimbal::writeAngleDegrees(E_CONTROLLED_AXES e, double_t dDegrees)
{
    if ( E_ROLL_AXIS == e )
        return;

    if ( ( dDegrees <= dAngleUpperLimits[e]) && ( dDegrees >= dAngleLowerLimits[e] ) )
    {
        double_t dToDegrees = Jet::map(dDegrees, dAngleLowerLimits[e], dAngleUpperLimits[e], 
            apcServos[E_YAW_AXIS]->readAngleLowerLimitDegrees(), apcServos[E_YAW_AXIS]->readAngleUpperLimitDegrees());

        if ( bDebug )
            (void)printf("\n%s: from Angle = %lf degrees, to Angle = %lf degrees\n", __FUNCTION__, dDegrees, dToDegrees);   


        apcServos[e]->writeAngleDegrees(dToDegrees);
    }
}

void K9TvcGimbal::writeAngleRadians(E_CONTROLLED_AXES e, double_t dRadians)
{
    if ( E_ROLL_AXIS == e )
        return;

    writeAngleDegrees(e, dRadians * MAX_ANGLE / M_PI);
}




