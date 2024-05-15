/*
	DoBoFo70Pro12.h - DoBoFo EDF 70mm pro CW 6S 2100KV 25.2V 12 Blades with 80A ESC
    Electric Ducted Fan class for Raspberry PI; Version 1.
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

/*
DoBoFo70Pro12

70mm pro EDF 4S 2800KV&ESC Specifications and Parameters
Max Voltage	16.8V
Max Current	75A
Max Thrust	1820g(4.01 lb)
Peak Power	1260W
Set Weight	182g(0.401 lb)
Housing set Weight	79g(0.174 lb)
Motor Weight	103g(0.227 lb)
Recom. Lipo	4S 3500MAH 35C
Recom.ESC	80A
Gold plated connector	Φ3.5
Fan diameter	Φ69.4mm（2.732 in）
Material	PA66+GF30（Nylon glass fiber）
ESC Model	80A
ESC Input terminal	XT60 /Wire length:20cm(7.8 in)
ESC Output terminal	Φ3.5mm /Wire length:42cm(16.5 in)
ESC Weight	111g(0.244 lb)
*/

#ifndef	_DOBOFO70PRO12_
#define _DOBOFO70PRO12_

#include "EDF.h"

#define DOBOFO70PRO12_MAX_THRUST (1820 * 0.00980665)

#define DOBOFO70PRO12_NAME ("DoBoFo70Pro12")


/*
ESC Model	80A 3S to 6S 
BEC 7 A @ 5V
ESC Input terminal	XT60 /Wire length:20cm(7.8 in)
ESC Output terminal	Φ3.5mm /Wire length:42cm(16.5 in)
ESC Weight	111g(0.244 lb)
"Duty-cycle" 1000 to 2000 us
For the first use of the product, please read the ESC manual to calibrate the throttle stroke.
*/

class DoBo80AEsc : public ESC
{
public:
	DoBo80AEsc(void);
	~DoBo80AEsc();
protected:
private:

};

class DoBoFo70Pro12 : public EDF
{

public:
	DoBoFo70Pro12(E_JETS sIndex=E_JET_0, E_SERVO_CHANNELS spIndex=E_PWM_2);
	~DoBoFo70Pro12();

	virtual void throttle(double_t position = 0.0);

	virtual double_t throttlePosition(void);

	virtual double_t thrust(void);	

protected:
    static const double_t maxVoltage;
    static const double_t maxCurrent;
    static const double_t peakPower;
    static const double_t setWeight;
    static const double_t fanDiameter;	

private:

};

#endif	// _DOBOFO70PRO12_
