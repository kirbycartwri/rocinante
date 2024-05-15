/*
	RockHopperControl.cpp - Intherited Control Class for Test Vehicle using
    Electric Ducted Fan class for Raspberry PI; Version 1.	
	Copyright (c) 2023 Kirby W. Cartwright. All rights reserved.

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

#include "RockHopperControl.h"
#include "BNO055.h"

RockHopperControl::RockHopperControl(void) :
    Control("rockhopper using an EDF, and using a BNO055 IMU and a BMP180.")
{

	sampleTime = SERVO_PERIOD_WIDTH * 1e-9;
										// for servos' the update rate use 400 Hz.
	sampleTime = 1 / BNO055::UPDATE_RATE;
										// for the BNO055's update rate use 100 Hz.

	sampleTime = 0.02;					// We'll use 50 Hz.

	for ( int32_t i = E_PITCH_AXIS; i < E_ROLL_AXIS ; i++ )
	{
		eControlled[i] = E_FEEDBACK_OFF;

		Ki[i] = 0.330;
		Kp[i] = 0.300;
		Kd[i] = 0.230;
	}

}

RockHopperControl::~RockHopperControl()
{
    ;
}

// Try to use base class' method!
void RockHopperControl::update(void)
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
			dOutputValues[i] = dControlRadiansSettings[i];
			continue;
		}
		else
			;						// Default to E_FEEDBACK_ON.	

		dProportional[i] 					= dControlRadiansSettings[i] - dSensorRadiansValues[i];
		dSensorRadiansIntegratedValues[i] 	= dSensorRadiansIntegratedValues[i] + ( dProportional[i] * deltaT );

		// For RockHopperControl, the angular velocity comes from the IMU and is set elsewhere.

		dOutputValues[i] = Kp[i] * dProportional[i] +
			Kd[i] * dSensorRadiansPerSecondValues[i] +
			Ki[i] * dSensorRadiansIntegratedValues[i];

	}	

}

void RockHopperControl::GetControlledAxes(E_FEEDBACK_MODE &ePitch, E_FEEDBACK_MODE &eRoll, E_FEEDBACK_MODE &eYaw)
{
	ePitch	= eControlled[E_PITCH_AXIS],
	eRoll 	= E_FEEDBACK_OFF,
	eYaw 	= eControlled[E_YAW_AXIS];
}