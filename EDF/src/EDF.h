/*
	EDF.h - Base Electric Ducted Fan class for Raspberry PI; Version 1.
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

#ifndef	_EDF_h
#define _EDF_h

#include "Servo.h"
#include "Jet.h"

#define EDF_VERSION	2     		// software version of this library


#define	MIN_EDF_ANGLE	(45.0)	// Corresponds to 1 ms or OFF
#define MAX_EDF_ANGLE	(135.0)	// Corresponds to 2 ms or 100%

class ESC 
{
public:
	ESC(const char *escName = NULL, const char *escDesc = NULL);
	~ESC();	
	uint32_t minDutyCycle, maxDutyCycle;	
protected:
	double_t maxCurrent, length, width, thickness, mass;
	char achEscName[FILENAME_MAX];
	char achDescription[FILENAME_MAX];
private:
};

class EDF : public Jet
{
public:
	EDF(const char *edfName, E_JETS sIndex=E_JET_0, E_SERVO_CHANNELS spIndex=E_PWM_0, double_t mThrust = DBL_MAX);
	~EDF();

	virtual void throttle(double_t position = 0.0);

	virtual double_t throttlePosition(void);

	virtual double_t thrust(void);		// Note: can be overridden with current acceleration and mass.


protected:

	Servo myControl;

	double_t maxThrust;

	ESC *myESC;

private:


};

#endif	// _EDF_h
