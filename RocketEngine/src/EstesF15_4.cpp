/*
	EstesF15_4.h - Estes F15-4 Rocket Engine class for Raspberry PI; Version 1.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <float.h>
#include "EstesF15_4.h"

const double_t EstesF15_4::THRUST_STEP_TIME = 0.0625;


EstesF15_4::EstesF15_4(E_JETS sIndex/*=E_JET_0*/) :
    ModelRocketEngine("Estes Rockets F15-4 Engine (29 mm)", sIndex)
{
	totalImpulse=49.61, 				// Newton-Seconds
	timeDelay=4.0, 						// Seconds
	maxLiftWeight=595,					// Grams
	maxThrust=25.26, 					// Newtons (Note: doesn't agree with the table below!)
	thrustDuration=3.45, 				// Seconds
	length=114.0, 						// mm
	diameter=29.0,						// mm
	weight=101.8, 						// g
	propellentWeight=60.0;				// g

    if ( bDebug )
    {
        (void)printf("The device name is \"%s,\" and the maximum thrust is % lf N.\n", achJetName, maxThrust);
    }
}

EstesF15_4::~EstesF15_4()
{
    ;
}

const double_t EstesF15_4::thrustValues[NUMBER_OF_THRUST_STEPS] = 
{
	0.0, 6.6, 15.0, 23.0, 27.9, 30.0, 27.0, 22.6, 20.0, 19.7,
	19.0, 18.5, 18.4, 18.05, 17.95, 17.7, 17.6, 17.5, 17.5, 
	17.5, 17.4, 17.35, 17.3, 17.0, 16.95, 16.9, 16.9,
	16.9, 16.8, 16.79, 16.8, 16.8, 16.79, 16.7, 16.5, 
	16.4, 16.3, 16.2, 16.0, 0.0
};

double_t EstesF15_4::thrust(void)
{
	if ( !bIgnited )
		return 0.0;

	double_t deltaT = currentThrustDuration();

	if ( deltaT >= ( NUMBER_OF_THRUST_STEPS * THRUST_STEP_TIME) )
		return 0.0;

	int32_t iLowerTableIndex = (int32_t)( deltaT / THRUST_STEP_TIME ),
		iUpperTableIndex = iLowerTableIndex+1;

    if ( bDebug )
    {
        (void)printf("The lower table index is %d and the upper table index is %d.\n", iLowerTableIndex, iUpperTableIndex);
    }

	if ( iUpperTableIndex >= NUMBER_OF_THRUST_STEPS )	
		return thrustValues[NUMBER_OF_THRUST_STEPS-1];

	double_t dLowerTableTime = iLowerTableIndex * THRUST_STEP_TIME,
		dUpperTableTime = iUpperTableIndex * THRUST_STEP_TIME;

	if ( bDebug )
    {
        (void)printf("The lower table time is %lf and the upper table time is %lf.\n", 
			dLowerTableTime, dUpperTableTime);
    }

	return Jet::map(deltaT, dLowerTableTime, dUpperTableTime, 
		thrustValues[iLowerTableIndex], thrustValues[iUpperTableIndex]);	

}