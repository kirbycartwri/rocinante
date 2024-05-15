/*
	Gimbal.cpp - Base Gimbal class for Raspberry PI; Version 1.
	Copyright (c) 2024 Kirby W. Cartwright. All rights reserved.

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
#include <string.h>
#include "Gimbal.h"

bool Gimbal::bDebug = false;

Gimbal::~Gimbal()
{
    for ( int32_t e = E_PITCH_AXIS; e < NUM_AXES ; e++ )
    {
        Gimbal::writeAngleDegrees((E_CONTROLLED_AXES)e, 0.0);
    }
}


Gimbal::Gimbal(const char *gimbalName/* = NULL*/)
{

    if ( NULL != gimbalName )
    {
        (void)strncpy(achGimbalName, gimbalName, sizeof(achGimbalName));
    }

    for ( int32_t e = E_PITCH_AXIS; e < NUM_AXES ; e++ )
    {
        dAngleUpperLimits[e]    = MAX_ANGLE_DEGREES;
        dAngleLowerLimits[e]    = MIN_ANGLE_DEGREES;
        dAngleDefaults[e]       = DEFAULT_ANGLE_DEGREES; 
        dAngleCenters[e]        = CENTER_ANGLE_DEGREES;

        Gimbal::writeAngleDegrees((E_CONTROLLED_AXES)e, CENTER_ANGLE_DEGREES);
    }

    weight = 0.0, diameter = 0.0, height = center_of_mass = 0.0;

    numberOfParts = 0;

}

void Gimbal::writeAngleUpperLimitDegrees(E_CONTROLLED_AXES e, double_t dDegrees /*= MAX_ANGLE_DEGREES*/)
{
    dAngleUpperLimits[e] = dDegrees;
}

void Gimbal::writeAngleDefaultDegrees(E_CONTROLLED_AXES e, double_t dDegrees /*= DEFAULT_ANGLE_DEGREES*/)
{
    dAngleDefaults[e] = dDegrees;
}

void Gimbal::writeAngleLowerLimitDegrees(E_CONTROLLED_AXES e, double_t dDegrees /*= MIN_ANGLE_DEGREES*/)
{
    dAngleLowerLimits[e] = dDegrees;
}

void Gimbal::writeAngleUpperLimitRadians(E_CONTROLLED_AXES e, double_t dRadians /*= MAX_ANGLE_RADIANS*/)
{
    double_t dDegrees = MAX_ANGLE_DEGREES * dRadians / M_PI ;
    writeAngleUpperLimitDegrees(e, dDegrees);

}

void Gimbal::writeAngleDefaultRadians(E_CONTROLLED_AXES e, double_t dRadians /*dRadians = DEFAULT_ANGLE_RADIANS*/)
{
    double_t dDegrees = MAX_ANGLE_DEGREES * dRadians / M_PI ;
    writeAngleDefaultDegrees(e, dDegrees);

}

void Gimbal::writeAngleCenterDegrees(E_CONTROLLED_AXES e, double_t dDegrees /*= CENTER_ANGLE_DEGREES*/)
{
    dAngleCenters[e] = dDegrees;
}

void Gimbal::writeAngleCenterRadians(E_CONTROLLED_AXES e, double_t dRadians /*= CENTER_ANGLE_RADIANS*/)
{
    double_t dDegrees = MAX_ANGLE_DEGREES * dRadians / M_PI ;
    writeAngleCenterDegrees(e, dDegrees);

}

void Gimbal::writeAngleLowerLimitRadians(E_CONTROLLED_AXES e, double_t dRadians /*= MIN_ANGLE_RADIANS*/)
{
    double_t dDegrees = MAX_ANGLE_DEGREES * dRadians / M_PI ;
    writeAngleLowerLimitDegrees(e, dDegrees);

}

double_t Gimbal::readAngleDegrees(E_CONTROLLED_AXES e)
{
    return 0.0;
}
double_t Gimbal::readAngleRadians(E_CONTROLLED_AXES e)
{
    return 0.0;
}
void Gimbal::writeAngleRadians(E_CONTROLLED_AXES e, double_t dRadians)
{
    ;
}	
void Gimbal::writeAngleDegrees(E_CONTROLLED_AXES e, double_t dDegrees)
{
    ;
}


