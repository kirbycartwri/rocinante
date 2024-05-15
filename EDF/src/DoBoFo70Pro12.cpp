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
#include <float.h>
#include "DoBoFo70Pro12.h"

DoBo80AEsc::DoBo80AEsc(void) :
	ESC("ESC Model	80A 3S to 6S", 
		"BEC 7 A @ 5V\n" \
		"ESC Input terminal	XT60 /Wire length:20cm(7.8 in)\n" \
		"ESC Output terminal	Φ3.5mm /Wire length:42cm(16.5 in)\n" \
		"ESC Weight	111g(0.244 lb)"
		)
{
		maxCurrent 		= 80.0, 		// A
			length 		= 86.0,			// mm 
			width  		= 37.0, 		// mm
			thickness 	= 10.0,			// mm 
			mass 		= 111.0;		// g

		// Nanoseconds
		minDutyCycle = 1000000, maxDutyCycle = 2000000;	
}

DoBo80AEsc::~DoBo80AEsc()
{

}


const double_t DoBoFo70Pro12::maxVoltage= 18.2;     // Vdc

const double_t DoBoFo70Pro12::maxCurrent= 75.0;     // Adc

const double_t DoBoFo70Pro12::peakPower = 1260;     // W

const double_t DoBoFo70Pro12::setWeight = 182;      // g

const double_t DoBoFo70Pro12::fanDiameter   = 69.4; // mm

DoBoFo70Pro12::DoBoFo70Pro12(E_JETS sIndex/*=E_JET_0*/, E_SERVO_CHANNELS spIndex/*=E_PWM_0*/) :
    EDF(DOBOFO70PRO12_NAME, sIndex, spIndex, DOBOFO70PRO12_MAX_THRUST)
{
	maxThrust = DOBOFO70PRO12_MAX_THRUST;

    if ( bDebug )
    {
        (void)printf("The device name is \"%s,\" and the maximum thrust is % lf N.\n", achJetName, maxThrust);
    }
	myESC = new DoBo80AEsc();

}

DoBoFo70Pro12::~DoBoFo70Pro12()
{
    delete myESC;
	myESC = NULL;
}


void DoBoFo70Pro12::throttle(double_t position /*= 0.0*/)
{
	double_t servoPosition = Jet::map(position, 0.0, 100.0, (double_t)myESC->minDutyCycle, (double_t)myESC->maxDutyCycle);

	uint32_t uValue = servoPosition + 0.5;

	myControl.writePulseWidth(uValue);

}

// Returns 0 to 100 %.
double_t DoBoFo70Pro12::throttlePosition(void)
{
	uint32_t uValue = myControl.readPulseWidth();

	double_t servoPosition = Jet::map((double_t)uValue, (double_t)myESC->minDutyCycle, (double_t)myESC->maxDutyCycle, 0.0, 100.0);

	return servoPosition;
}

double_t DoBoFo70Pro12::thrust(void)
{
	return throttlePosition() * maxThrust / 100.0;
}