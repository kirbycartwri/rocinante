/*
	Control.h - Base Closed-loop control class for Raspberry PI; Version 1.
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

#ifndef	_CONTROL_H
#define _CONTROL_H

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <float.h>
#include "Gimbal.h"
#include "Control.h"

#define CONTROL_VERSION	2     			// the software version of this library

typedef enum efm 
{
	E_FEEDBACK_UNDEFINED	= -1,
	E_FEEDBACK_OFF			= 0,
	E_FEEDBACK_ON			= 1,
	E_FEEDBACK_FOLLOW		= 2,

	NUM_FEEDBACK_MODES		= 3

} E_FEEDBACK_MODE;



class Control
{
public:
	Control(const char *ControlName);
	virtual ~Control();

	virtual void SetControlledAxes(const E_FEEDBACK_MODE &ePitch, const E_FEEDBACK_MODE &eRoll, const E_FEEDBACK_MODE &eYaw);

	virtual void GetControlledAxes(E_FEEDBACK_MODE &ePitch, E_FEEDBACK_MODE &eRoll, E_FEEDBACK_MODE &eYaw);

	virtual void SetInputAngleRadiansValues(double_t dPitch, double_t dRoll, double_t dYaw);

	virtual void GetInputAngleRadiansValues(double_t &dPitch, double_t &dRoll, double_t &dYaw);

	virtual void SetControlledInputAngleRadiansValues(double_t dPitch, double_t dRoll, double_t dYaw);

	virtual void GetControlledInputAngleRadiansValues(double_t dPitch, double_t dRoll, double_t dYaw);

	virtual void SetInputAngularVelocityRadiansPerSecondValues(double_t dPitchRate, double_t dRollRate, double_t dYawRate);

	virtual void GetControlledOutputAngleRadiansValues(double_t &dPitch, double_t &dRoll, double_t &dYaw);

	virtual void SetInputAngleDegreesValues(double_t dPitch, double_t dRoll, double_t dYaw);

	virtual void GetInputAngleDegreesValues(double_t &dPitch, double_t &dRoll, double_t &dYaw);

	virtual void SetControlledInputAngleDegreesValues(double_t dPitch, double_t dRoll, double_t dYaw);

	virtual void GetControlledInputAngleDegreesValues(double_t dPitch, double_t dRoll, double_t dYaw);

	virtual void GetControlledOutputAngleDegreesValues(double_t &dPitch, double_t &dRoll, double_t &dYaw);

	virtual void update(void);

protected:

	char achControlName[FILENAME_MAX];

	struct timespec thisTime, lastTime;

	static const bool bDebug;

	E_FEEDBACK_MODE eControlled[NUM_AXES];

// Are the following four really needed?
	double_t dControlRadiansUpperLimits[NUM_AXES];
	double_t dControlRadiansLowerLimits[NUM_AXES];
	double_t dControlRadiansDefaults[NUM_AXES];
	double_t dControlRadiansCenters[NUM_AXES];

	double_t dControlRadiansSettings[NUM_AXES];
	double_t dSensorRadiansValues[NUM_AXES];	
	double_t dSensorRadiansPerSecondValues[NUM_AXES];

	double_t dDeltaValues[NUM_AXES];
	double_t dSensorRadiansIntegratedValues[NUM_AXES];
	double_t dOutputValues[NUM_AXES];

	double_t Ki[NUM_AXES],
		Kp[NUM_AXES],
		Kd[NUM_AXES];

	double_t sampleTime;				// in seconds.


private:



};

#endif	// _CONTROL_H
