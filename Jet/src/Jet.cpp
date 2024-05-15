


/*
	Copyright (c) 2023 Kirby W. Cartwright. All right reserved.

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

#include "Jet.h"

const bool Jet::bDebug = false;

Jet::Jet(const char *jetName, E_JETS sIndex/*=E_JET_0*/) :
	jetIndex(sIndex),ips(0.0), totalImpulse(0.0), timeDelay(0.0),maxThrust(0.0), thrustDuration(0.0),
		length(0.0), diameter(0.0), mass(0.0), propellentMass(0.0)
{
	if ( NULL!=jetName )
	{
		(void)strncpy(achJetName, jetName, sizeof(achJetName));
	}

}

Jet::~Jet()
{
	jetIndex = E_JET_UNDEFINED;
	(void)memset(achJetName, '\0', sizeof(achJetName));
}

// Useful method for all classes; like Arduino's "map."
double_t Jet::map(double_t value, double_t fromLow, double_t fromHigh, double_t toLow, double_t toHigh)
{
	double_t returnvalue = toLow;

	if ( value <= fromLow )
	{
		returnvalue = toLow;
	}
	else if ( value >= fromHigh)
	{
		returnvalue = toHigh;
	}
	else
	{
		returnvalue = (toHigh - toLow)/(fromHigh - fromLow) * ( value - fromLow ) + toLow;
	}

	return returnvalue;
}

void Jet::throttle(double_t position /*= 0.0*/)
{
	;
}

double_t Jet::throttlePosition(void)
{
	return 0.0;
}

double_t Jet::thrust(void)
{
	return 0.0;
}




