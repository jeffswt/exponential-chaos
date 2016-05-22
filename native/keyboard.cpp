
//	"keyboard.cpp" -*- C++ -*-
//	Created on: 30 April 2016
//	Definitions for native keyboard state detection library.
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

#include "native/keyboard.h"

#include "native/time.h"
#include <windows.h>

namespace	KeyboardInternals
{
	double	LastPressTime[256];
	bool	Onpress[256];
}

bool	kGetKeyOnpress(
		int	vkey)
{
	double	lastTime = kGetKeyLastPressedTime(vkey);
	if (kGetKeyState(vkey))
		if (GetProcessTime() - lastTime > 0.035)
			return true;
	return false;
}

bool	kGetKeyState(
		int	vkey)
{
	using namespace KeyboardInternals;
	bool	State = GetAsyncKeyState(vkey);
	Onpress[vkey] = State;
	if (State)
		LastPressTime[vkey] = GetProcessTime();
	return State;
}

bool	kGetKeyStateUnsynced(
		int	vkey)
{
	using namespace KeyboardInternals;
	if (Onpress[vkey])
		LastPressTime[vkey] = GetProcessTime();
	return Onpress[vkey];
}

double	kGetKeyLastPressedTime(
		int	vkey)
{
	using namespace KeyboardInternals;
	return LastPressTime[vkey];
}
