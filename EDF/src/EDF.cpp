/*
	Copyright (c) 2023 Kirby W. Cartwright. All right reserved.
	EDF.h - Base Electric Ducted Fan class for Raspberry PI; Version 1.
	
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
#include <float.h>
#include "EDF.h"

EDF::EDF(const char *edfName, E_JETS sIndex/*=E_JET_0*/, E_SERVO_CHANNELS spIndex/*=E_PWM_0*/, double_t mThrust /*= DBL_MAX*/) :
	Jet(edfName, sIndex),
	myControl(spIndex), maxThrust(mThrust),
	myESC(NULL)
{
	throttle(0.0);
}

EDF::~EDF()
{
	throttle(0.0);
}

void EDF::throttle(double_t position/*= 0.0*/)
{
	double_t servoPosition = Jet::map(position, MIN_THROTTLE_POSITION, MAX_THROTTLE_POSITION, MIN_EDF_ANGLE, MAX_EDF_ANGLE);

	myControl.writeAngleDegrees(servoPosition);
}

double_t EDF::throttlePosition(void)
{

	double_t servoPosition = myControl.readAngleDegrees();

	double_t position = Jet::map(servoPosition, MIN_ANGLE, MAX_ANGLE, MIN_THROTTLE_POSITION, MAX_THROTTLE_POSITION);

	return position;

}

double_t EDF::thrust(void)
{
	double_t position = throttlePosition();

	double_t currentThrust = Jet::map(position,  MIN_THROTTLE_POSITION, MAX_THROTTLE_POSITION, 0.0, maxThrust);

	return currentThrust;

}

ESC::ESC(const char *escName /*= NULL*/, const char *escDesc /*= NULL*/) :
	minDutyCycle(0), maxDutyCycle(0),
	maxCurrent(0.0), length(0.0), width(0.0), thickness(0.0), mass(0.0)
{
	if ( NULL!= escName )
		(void)strncpy(achEscName, escName, sizeof(achEscName));
	else
		(void)memset(achEscName, '\0', sizeof(achEscName));

	if ( NULL!= escDesc )
		(void)strncpy(achDescription, escDesc, sizeof(achDescription));
	else
		(void)memset(achDescription, '\0', sizeof(achDescription));


}
ESC::~ESC()
{
	;
}