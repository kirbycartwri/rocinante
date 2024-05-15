/*
	DorheaMG90S.h - Inherited Class DorheaMG90S library for Raspberry PI using its hardware PWM channels; Version 1.
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

#ifndef _DORHEAMG90S_H_
#define _DORHEAMG90S_H_

#include "Servo.h"

#define DorheaMG90S_VERSION       2     		// software version of this library

#ifndef MIN_ANGLE
#define	MIN_ANGLE			0.0
#define MAX_ANGLE			180.0
#endif
#define	DEFAULT_ANGLE		( ( ( MAX_ANGLE - MIN_ANGLE ) / 2 ) + MIN_ANGLE )
#define CENTER_ANGLE		( DEFAULT_ANGLE )

#define MIN_PULSE_WIDTH		500000     	// This is the shortest pulse width in nanoseconds sent to a DorheaMG90S; 0.0 degrees.
#define MAX_PULSE_WIDTH     2500000    	// This is the longest pulse width in nanoseconds sent to a DorheaMG90S; 180.0 degrees.
#define DEFAULT_PULSE_WIDTH	1500000    	// This is the default pulse width in nanoseconds when the DorheaMG90S is attached ("neutral"); 90 degrees.
#define DEFAULT_PERIOD_WIDTH	2500000		
										// nanoseconds for 400 Hz.

#define DorheaMG90SS_PER_TIMER	2		// The maximum number of DorheaMG90Ss controlled by the RPi firmware.

#define DorheaMG90S_SLEW_RATE		( 60.0 / 0.11 )
										// Sixty degrees in 0.11 second.

#define MAX_DorheaMG90S_SLEW_TIME	( 0.11 / 60.0 * MAX_ANGLE * 1000000)
										// Slew 180 degrees, no load.


/*
	Stall Torque: 2.0kg/cm(4.8V)
	Operating Speed: 0.11 seconds / 60 degrees (4.8V).
	The servo Maximum angle is 180.
*/

class DorheaMG90S : public Servo
{
public:
	DorheaMG90S(const char *strDescription = NULL, E_SERVO_CHANNELS sIndex=E_PWM_0);
	~DorheaMG90S();
protected:

private:

};

#endif
