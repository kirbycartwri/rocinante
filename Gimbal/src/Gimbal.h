/*
	Gimbal.h - Base Gimbal class for Raspberry PI Version 1.
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

#ifndef _GIMBAL_H
#define _GIMBAL_H

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>

#define MIN_ANGLE_DEGREES 		(0.0)
#define MAX_ANGLE_DEGREES 		(180.0)
#define CENTER_ANGLE_DEGREES	(90.0)
#define DEFAULT_ANGLE_DEGREES   (90.0)

#define MIN_ANGLE_RADIANS 		(MIN_ANGLE_DEGREES * M_PI / 180.0)
#define MAX_ANGLE_RADIANS 		(MAX_ANGLE_DEGREES * M_PI / 180.0)
#define CENTER_ANGLE_RADIANS	(CENTER_ANGLE_DEGREES * M_PI / 180.0)
#define DEFAULT_ANGLE_RADIANS   (DEFAULT_ANGLE_DEGREES * M_PI / 180.0)

typedef enum 
{
	E_PITCH_AXIS		= 0,
	E_ROLL_AXIS			= 1,
	E_YAW_AXIS			= 2,

	NUM_AXES 	                        /* Should default to the number of axes.*/
} E_CONTROLLED_AXES;


class Gimbal
{
public:
	Gimbal(const char *gimbalName = NULL);
	virtual ~Gimbal();

	// -Pi to +Pi radians.
	virtual double_t readAngleRadians(E_CONTROLLED_AXES e);
	virtual void writeAngleRadians(E_CONTROLLED_AXES e, double_t dRadians);

											// The arguments for these are an angle in radians.
	virtual void writeAngleUpperLimitRadians(E_CONTROLLED_AXES e, double_t dRadians = MAX_ANGLE_RADIANS);
	virtual void writeAngleDefaultRadians(E_CONTROLLED_AXES e, double_t dRadians = DEFAULT_ANGLE_RADIANS);
	virtual void writeAngleCenterRadians(E_CONTROLLED_AXES e, double_t dRadians = CENTER_ANGLE_RADIANS);
	virtual void writeAngleLowerLimitRadians(E_CONTROLLED_AXES e, double_t dRadians = MIN_ANGLE_RADIANS);  

	// -180 degrees to + 180 degrees
	virtual double_t readAngleDegrees(E_CONTROLLED_AXES e); 
	virtual void writeAngleDegrees(E_CONTROLLED_AXES e, double_t dDegrees);
											// The arguments for these are an angle in degrees.
	virtual void writeAngleUpperLimitDegrees(E_CONTROLLED_AXES e, double_t dDegrees = MAX_ANGLE_DEGREES);
	virtual void writeAngleDefaultDegrees(E_CONTROLLED_AXES e, double_t dDegrees = DEFAULT_ANGLE_DEGREES);
	virtual void writeAngleCenterDegrees(E_CONTROLLED_AXES e, double_t dDegrees = CENTER_ANGLE_DEGREES);
	virtual void writeAngleLowerLimitDegrees(E_CONTROLLED_AXES e, double_t dDegrees = MIN_ANGLE_DEGREES);    

protected:
	char achGimbalName[FILENAME_MAX];

	static bool bDebug;

	double_t dAngleUpperLimits[NUM_AXES];
	double_t dAngleLowerLimits[NUM_AXES];
	double_t dAngleDefaults[NUM_AXES];
	double_t dAngleCenters[NUM_AXES];

    double_t weight, diameter, height, center_of_mass;

    uint32_t numberOfParts;


private:

};



#endif // _GIMBAL_H