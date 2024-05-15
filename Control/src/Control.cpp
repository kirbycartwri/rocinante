/*
	Control.cpp - Base Closed-loop control class for Raspberry PI; Version 1.
	Control.h - Base Electric Ducted Fan class for Raspberry PI; Version 1.
	
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
#include "Control.h"

const bool Control::bDebug = true;

Control::Control(const char *ControlName) 
{
	if ( NULL!=ControlName )
		(void)strncpy(achControlName, ControlName, sizeof(achControlName));
	else
		(void)memset(achControlName, '\0', sizeof(achControlName));

	(void)clock_gettime(CLOCK_REALTIME, &thisTime);
	(void)clock_gettime(CLOCK_REALTIME, &lastTime);	

	sampleTime = 0.0;

	for ( int32_t i = E_PITCH_AXIS; i < E_ROLL_AXIS ; i++ )
	{

		eControlled[i] = E_FEEDBACK_OFF;

		dControlRadiansUpperLimits[i]	= 0.0;
		dControlRadiansLowerLimits[i] 	= 0.0;
		dControlRadiansDefaults[i] 		= 0.0;
		dControlRadiansCenters[i] 		= 0.0;

		dControlRadiansSettings[i] 			= 0.0;
		dSensorRadiansValues[i] 			= 0.0;
		dSensorRadiansPerSecondValues[i]	= 0.0;
		dSensorRadiansIntegratedValues[i]	= 0.0;
		dOutputValues[i]					= 0.0;
		dDeltaValues[i]						= 0.0;

		Ki[i] = 0.0;
		Kp[i] = 0.0;
		Kd[i] = 0.0;


	}


}

Control::~Control()
{
	;
}

void Control::GetInputAngleRadiansValues(double_t &dPitch, double_t &dRoll, double_t &dYaw)
{
	dPitch 	= dSensorRadiansValues[E_PITCH_AXIS];
	dRoll 	= dSensorRadiansValues[E_ROLL_AXIS];
	dYaw 	= dSensorRadiansValues[E_YAW_AXIS];
}

void Control::GetInputAngleDegreesValues(double_t &dPitch, double_t &dRoll, double_t &dYaw)
{
	dPitch 	= MAX_ANGLE_DEGREES * dSensorRadiansValues[E_PITCH_AXIS] / M_PI;
	dRoll 	= MAX_ANGLE_DEGREES * dSensorRadiansValues[E_ROLL_AXIS] / M_PI;
	dYaw 	= MAX_ANGLE_DEGREES * dSensorRadiansValues[E_YAW_AXIS] / M_PI;
}

void Control::SetInputAngleRadiansValues(double_t dPitch, double_t dRoll, double_t dYaw)
{
	dSensorRadiansValues[E_PITCH_AXIS] 	= dPitch;
	dSensorRadiansValues[E_ROLL_AXIS] 	= dRoll;
	dSensorRadiansValues[E_YAW_AXIS]	= dYaw;
}

void Control::SetInputAngleDegreesValues(double_t dPitch, double_t dRoll, double_t dYaw)
{
	dSensorRadiansValues[E_PITCH_AXIS] 	= M_PI * dPitch / MAX_ANGLE_DEGREES;
	dSensorRadiansValues[E_ROLL_AXIS] 	= M_PI * dRoll / MAX_ANGLE_DEGREES;
	dSensorRadiansValues[E_YAW_AXIS]	= M_PI * dYaw / MAX_ANGLE_DEGREES;
}

void Control::SetControlledInputAngleRadiansValues(double_t dPitch, double_t dRoll, double_t dYaw)
{
	dControlRadiansSettings[E_PITCH_AXIS] 	= dPitch;
	dControlRadiansSettings[E_ROLL_AXIS] 	= dRoll;
	dControlRadiansSettings[E_YAW_AXIS]		= dYaw;
}

void Control::SetControlledInputAngleDegreesValues(double_t dPitch, double_t dRoll, double_t dYaw)
{
	dControlRadiansSettings[E_PITCH_AXIS] 	= M_PI * dPitch / MAX_ANGLE_DEGREES;
	dControlRadiansSettings[E_ROLL_AXIS] 	= M_PI * dRoll / MAX_ANGLE_DEGREES;
	dControlRadiansSettings[E_YAW_AXIS]		= M_PI * dYaw / MAX_ANGLE_DEGREES;
}

void Control::GetControlledInputAngleRadiansValues(double_t dPitch, double_t dRoll, double_t dYaw)
{
	dPitch = dControlRadiansSettings[E_PITCH_AXIS];
	dRoll = dControlRadiansSettings[E_ROLL_AXIS];
	dYaw   = dControlRadiansSettings[E_YAW_AXIS];
}

void Control::GetControlledInputAngleDegreesValues(double_t dPitch, double_t dRoll, double_t dYaw)
{
	dPitch = MAX_ANGLE_DEGREES * dControlRadiansSettings[E_PITCH_AXIS] / M_PI;
	dRoll = MAX_ANGLE_DEGREES * dControlRadiansSettings[E_ROLL_AXIS] / M_PI;
	dYaw   = MAX_ANGLE_DEGREES * dControlRadiansSettings[E_YAW_AXIS] / M_PI;
}

void Control::SetInputAngularVelocityRadiansPerSecondValues(double_t dPitchRate, double_t dRollRate, double_t dYawRate)
{
	dSensorRadiansPerSecondValues[E_PITCH_AXIS] = dPitchRate;
	dSensorRadiansPerSecondValues[E_ROLL_AXIS] 	= dRollRate;
	dSensorRadiansPerSecondValues[E_YAW_AXIS]	= dYawRate;
}

void Control::GetControlledOutputAngleRadiansValues(double_t &dPitch, double_t &dRoll, double_t &dYaw)
{
	dPitch = dOutputValues[E_PITCH_AXIS];
	dRoll  = dOutputValues[E_ROLL_AXIS];
	dYaw   = dOutputValues[E_YAW_AXIS];
}

void Control::GetControlledOutputAngleDegreesValues(double_t &dPitch, double_t &dRoll, double_t &dYaw)
{
	dPitch = MAX_ANGLE_DEGREES * dOutputValues[E_PITCH_AXIS] / M_PI;
	dRoll  = MAX_ANGLE_DEGREES * dOutputValues[E_ROLL_AXIS] / M_PI;
	dYaw   = MAX_ANGLE_DEGREES * dOutputValues[E_YAW_AXIS] / M_PI;
}

void Control::update(void)
{
	(void)clock_gettime(CLOCK_REALTIME, &thisTime);

	double_t deltaT = 0.0;

	time_t tv_delta_sec = ( thisTime.tv_sec - lastTime.tv_sec );
	long tv_delta_ns	= thisTime.tv_nsec - lastTime.tv_nsec;

	deltaT = tv_delta_ns;
	deltaT /= 1e9;
	deltaT += tv_delta_sec;

	if ( deltaT < sampleTime )
		return;

	lastTime = thisTime;

	double_t dProportional[NUM_AXES] = 
	{
		0.0, 0.0, 0.0
	};

	for ( int32_t i = E_PITCH_AXIS; i <= E_YAW_AXIS ; i++ )
	{
		if ( E_FEEDBACK_FOLLOW == eControlled[i] )
		{
			dOutputValues[i] = dSensorRadiansValues[i];
			dSensorRadiansIntegratedValues[i] = 0.0;
			continue;
		}

		else if ( E_FEEDBACK_OFF == eControlled[i] )
		{
			dSensorRadiansIntegratedValues[i] = 0.0;
			continue;
		}

		else
			;

		// Default to E_FEEDBACK_ON.
		dProportional[i] 					= dControlRadiansSettings[i] - dSensorRadiansValues[i];
		dSensorRadiansPerSecondValues[i]	= ( dProportional[i] - dDeltaValues[i] ) / deltaT;
		dDeltaValues[i] 					= dProportional[i];	
	
		dSensorRadiansIntegratedValues[i] 	= dSensorRadiansIntegratedValues[i] + ( dProportional[i] * deltaT );

		dOutputValues[i] = Kp[i] * dProportional[i] +
			Kd[i] * dSensorRadiansPerSecondValues[i] +
			Ki[i] * dSensorRadiansIntegratedValues[i];

	}	

}

void Control::SetControlledAxes(const E_FEEDBACK_MODE &ePitch, const E_FEEDBACK_MODE &eRoll, const E_FEEDBACK_MODE &eYaw)
{
	eControlled[E_PITCH_AXIS]	= ePitch,
	eControlled[E_ROLL_AXIS] 	= eRoll,
	eControlled[E_YAW_AXIS] 	= eYaw;
}

void Control::GetControlledAxes(E_FEEDBACK_MODE &ePitch, E_FEEDBACK_MODE &eRoll, E_FEEDBACK_MODE &eYaw)
{
	ePitch	= eControlled[E_PITCH_AXIS],
	eRoll 	= eControlled[E_ROLL_AXIS],
	eYaw 	= eControlled[E_YAW_AXIS];
}

