


/*
	Servo.h - Servo library for Raspberry PI using its built in hardware PWM channels; Version 1.
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

#include "DorheaMG90S.h"


extern const char* __progname;			// The program name is provided by libc.
#define PROGRAM_NAME __progname


DorheaMG90S::DorheaMG90S(const char *strDescription/*= NULL*/, E_SERVO_CHANNELS sIndex/*=E_PWM_0*/) :
	Servo(sIndex)
{

	(void)strncpy(achGimbalName, "Dorhea MG90S with only one Metal Gear", sizeof(achGimbalName));

}

DorheaMG90S::~DorheaMG90S()
{

}



