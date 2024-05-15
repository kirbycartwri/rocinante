/*
	Jet.h - Base Jet class for Raspberry PI; Version 1.
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

#ifndef	_Jet_h
#define _Jet_h

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>

#define JET_VERSION	1     		// software version of this library



typedef enum 
{
	E_JET_0			= 0,
	E_JET_1 		= 1,
	E_JET_2			= 2,
	E_JET_3 		= 3,
	E_JET_4			= 4,
	E_JET_5 		= 5,
	E_JET_6			= 6,
	E_JET_7 		= 7,	
	E_JET_8			= 8,
	E_JET_9 		= 9,	
	E_JET_10		= 10,
	E_JET_11 		= 11,
	E_JET_12		= 12,
	E_JET_13 		= 13,
	E_JET_14		= 14,
	E_JET_15 		= 15,	
	NUM_JetS 	/* The max number of jets; i.e., 16..*/
} E_JETS;


class Jet
{
public:
	Jet(const char *jetName, E_JETS sIndex=E_JET_0);
	~Jet();

protected:
	
	E_JETS jetIndex;            // Index for this Jet.

	virtual void throttle(double_t position = 0.0) = 0;

	virtual double_t throttlePosition(void) = 0;

	virtual double_t thrust(void) = 0;

	double_t thrust, position,
		totalImpulse, timeDelay,
		maxThrust, thrustDuration,
		length, diameter, mass, propellentMass;


private:



};

#endif	// _Jet_h
