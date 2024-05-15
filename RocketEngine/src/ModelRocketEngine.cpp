/*
	Copyright (c) 2023 Kirby W. Cartwright. All right reserved.
	ModelRocketEngine.cpp - Base Hobby Model Rocket for Raspberry PI; Version 1.
	
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <float.h>
#include "ModelRocketEngine.h"

ModelRocketEngine::ModelRocketEngine(const char *ModelRocketEngineName, E_JETS sIndex/*=E_JET_0*/) :
	Jet(ModelRocketEngineName, sIndex),
	bIgnited(false), totalImpulse(0.0), timeDelay(0.0), maxLiftWeight(0.0),
		maxThrust(0.0), thrustDuration(0.0), length(0.0), diameter(0.0),
		weight(0.0), propellentWeight(0.0)
{
	(void)clock_gettime(CLOCK_REALTIME, &currentTime);
	(void)clock_gettime(CLOCK_REALTIME, &startTime);
}

ModelRocketEngine::~ModelRocketEngine()
{
	;
}

double_t ModelRocketEngine::throttlePosition(void)
{
	return bIgnited ? MAX_THROTTLE_POSITION : MIN_THROTTLE_POSITION;
}

void ModelRocketEngine::throttle(double_t position/*= 0.0*/)
{
	if ( (!bIgnited) && ( MIN_THROTTLE_POSITION < position ) )
	{
		(void)clock_gettime(CLOCK_REALTIME, &currentTime);
		(void)clock_gettime(CLOCK_REALTIME, &startTime);

		bIgnited = true;
	
		if ( bDebug )
		{
			(void)printf("%s: Ignition!\n", __FUNCTION__);
		}		
	
	}  
}

double_t ModelRocketEngine::currentThrustDuration(void)
{
	double_t deltaT = 0.0;

	if ( clock_gettime(CLOCK_REALTIME, &currentTime) )
	{
		(void)printf("Unable to read from \"%s!\"\n\t\"%s\"\n", "CLOCK_REALTIME", strerror(errno));
	}
	else
	{
		time_t tv_delta_sec = ( currentTime.tv_sec - startTime.tv_sec );
		long tv_delta_ns	= currentTime.tv_nsec - startTime.tv_nsec;

		deltaT = tv_delta_sec;
		deltaT *= 1e9;
		deltaT += tv_delta_ns;
		deltaT /= 1e9;

	}

	return deltaT;

}



