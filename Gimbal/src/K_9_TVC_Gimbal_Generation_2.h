/*
	K_9_TVC_Gimbal_Generation_2.h - K9TvcGimbal class for Raspberry PI; Version 1.
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

#ifndef _K9_TVC_K9_GIMBAL_H
#define _K9_TVC_K9_GIMBAL_H

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>
#include "Gimbal.h"
#include "Servo.h"

#if 0

#define K9_MIN_YAW_ANGLE_DEGREES        (-90.0)
#define K9_MAX_YAW_ANGLE_DEGREES		(90.0)
#define K9_CENTER_YAW_ANGLE_DEGREES	    (3.0)
#define K9_DEFAULT_YAW_ANGLE_DEGREES	(3.0)

#define K9_MIN_PITCH_ANGLE_DEGREES	    (-90.0)
#define K9_MAX_PITCH_ANGLE_DEGREES	    (90.0)
#define K9_CENTER_PITCH_ANGLE_DEGREES   (-8.0)
#define K9_DEFAULT_PITCH_ANGLE_DEGREES  (-8.0)

#else


#define K9_MIN_YAW_ANGLE_DEGREES        (-10.0)
#define K9_MAX_YAW_ANGLE_DEGREES		(10.0)
#define K9_CENTER_YAW_ANGLE_DEGREES	    (0.0)
#define K9_DEFAULT_YAW_ANGLE_DEGREES	(0.0)

#define K9_MIN_PITCH_ANGLE_DEGREES	    (-15.0)
#define K9_MAX_PITCH_ANGLE_DEGREES	    (15.0)
#define K9_CENTER_PITCH_ANGLE_DEGREES   (0.0)
#define K9_DEFAULT_PITCH_ANGLE_DEGREES  (0.0)

#endif

#define K9_MIN_YAW_ANGLE_RADIANS        (K9_MIN_YAW_ANGLE_DEGREES * M_PI / 180.0)
#define K9_MAX_YAW_ANGLE_RADIANS		(K9_MAX_YAW_ANGLE_DEGREES * M_PI / 180.0)
#define K9_CENTER_YAW_ANGLE_RADIANS	    (K9_CENTER_YAW_ANGLE_DEGREES * M_PI / 180.0)
#define K9_DEFAULT_YAW_ANGLE_RADIANS	(K9_DEFAULT_YAW_ANGLE_DEGREES * M_PI / 180.0)

#define K9_MIN_PITCH_ANGLE_RADIANS	    (K9_MIN_PITCH_ANGLE_DEGREES * M_PI / 180.0)
#define K9_MAX_PITCH_ANGLE_RADIANS	    (K9_MAX_PITCH_ANGLE_DEGREES * M_PI / 180.0)
#define K9_CENTER_PITCH_ANGLE_RADIANS   (K9_CENTER_PITCH_ANGLE_DEGREES * M_PI / 180.0)
#define K9_DEFAULT_PITCH_ANGLE_RADIANS  (K9_DEFAULT_PITCH_ANGLE_DEGREES * M_PI / 180.0)

class K9TvcGimbal : public Gimbal
{
public:
	K9TvcGimbal(void);
	virtual ~K9TvcGimbal();  

	// -180 to +180 degrees.
	virtual double_t readAngleDegrees(E_CONTROLLED_AXES e); 
	virtual void writeAngleDegrees(E_CONTROLLED_AXES e, double_t dDegrees);

	// -Pi to +Pi radians.
	virtual double_t readAngleRadians(E_CONTROLLED_AXES e);
	virtual void writeAngleRadians(E_CONTROLLED_AXES e, double_t dRadians);

protected:

    Servo *apcServos[NUM_AXES];


private:

};



#endif // _K9_TVC_K9_GIMBAL_H