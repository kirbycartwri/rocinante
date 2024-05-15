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

#define	MIN_THROTTLE_POSITION	0.0
#define MAX_THROTTLE_POSITION 100.0


typedef enum 
{
	E_JET_UNDEFINED	= -1,
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
	virtual ~Jet();

	// Useful method for all classes; like Arduino's "map."
	static double_t map(double_t value, double_t fromLow, double_t fromHigh, double_t toLow, double_t toHigh);

								// Set position from 0 to 100 %; i.e., 0.0 to 100.0.
	virtual void throttle(double_t position = 0.0);

	virtual double_t throttlePosition(void);
								// Return position from 0 to 100 %; i.e., 0.0 to 100.0.
	virtual double_t thrust(void);
								// Return the current thrust value in Newtons based on whatever parameters; e.g., a lookup table and time.

protected:
	
	E_JETS jetIndex;            // Index for this Jet.

	double_t ips, totalImpulse, timeDelay,
		maxThrust, thrustDuration,
		length, diameter, mass, propellentMass;

	char achJetName[FILENAME_MAX];

	static const bool bDebug;

private:

};

#endif	// _Jet_h
