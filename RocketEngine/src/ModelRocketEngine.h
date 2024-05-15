/*
	ModelRocketEngine.h - Base Hobby Model Rocket class for Raspberry PI; Version 1.
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

#ifndef	_ModelRocketEngine_h
#define _ModelRocketEngine_h

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include "Jet.h"

class ModelRocketEngine : public Jet
{
public:
	ModelRocketEngine(const char *ModelRocketEngineName, E_JETS sIndex=E_JET_0);
	~ModelRocketEngine();

	virtual void throttle(double_t position = 0.0);

	virtual double_t thrust(void) = 0;	// each inheritor MUST provide this (using a lookup table)!

	virtual double_t throttlePosition(void);

	double_t currentThrustDuration(void);	

protected:
	struct timespec startTime, currentTime;

	bool bIgnited;

	double_t totalImpulse, timeDelay, maxLiftWeight,
		maxThrust, thrustDuration, length, diameter,
		weight, propellentWeight;

private:

	



};

#endif	// _ModelRocketEngine_h
