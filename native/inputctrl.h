
//	"inputctrl.h" -*- C++ -*-
//	Created on: 11 April 2016
//	Input devices' main indexer.
//
//	Copyright (C) 2016  Geoffrey Tang.
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef NATIVE_INPUTCTRL_H_
#define NATIVE_INPUTCTRL_H_

#include "include/public.h"

class InputCtrlType
{
public:
	int			MouseX, MouseY, MouseEntry;
	bool		LDown, MDown, RDown, WheelUp, WheelDn;
	long long	PlayerGuid;
	double		CameraX, CameraY;
	InputCtrlType(void)
	{
		MouseX = MouseY = MouseEntry = 0;
		LDown = MDown = RDown = false;
		WheelUp = WheelDn = false;
		PlayerGuid = 0;
		CameraX = CameraY = 0.0;
		return ;
	}
};

#endif /* NATIVE_INPUTCTRL_H_ */
