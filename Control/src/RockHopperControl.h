/*
	RockHopperControl.h - Intherited Control Class for Test Vehicle using
    Electric Ducted Fan for Raspberry PI; Version 1.
	Copyright (c) 2024 Kirby W. Cartwright. All rights reserved.

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

#ifndef	_RockHopperControl_H
#define _RockHopperControl_H

#include "K_9_TVC_Gimbal_Generation_2.h"
#include "Control.h"

class RockHopperControl : public Control
{
public:
	RockHopperControl(void);
	~RockHopperControl();

	virtual void GetControlledAxes(E_FEEDBACK_MODE &ePitch, E_FEEDBACK_MODE &eRoll, E_FEEDBACK_MODE &eYaw);
	virtual void update(void);

protected:


private:


};

#endif	// _RockHopperControl_H
