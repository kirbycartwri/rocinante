/*
	EstesF15_4.h - Estes F15-4 Rocket Engine class for Raspberry PI; Version 1.
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


#ifndef	_ESTESF15_4_
#define _ESTESF15_4_

#include <stdio.h>
#include <inttypes.h>
#include <math.h>
#include <unistd.h>
#include "ModelRocketEngine.h"

#define NUMBER_OF_THRUST_STEPS ( 40 )


class EstesF15_4 : public ModelRocketEngine
{
public:
	EstesF15_4(E_JETS sIndex=E_JET_0);
	~EstesF15_4();

	virtual double_t thrust(void);

protected:

private:
	static const double_t THRUST_STEP_TIME;

	static const double_t thrustValues[NUMBER_OF_THRUST_STEPS];

};

#endif	// _ESTESF15_4_
