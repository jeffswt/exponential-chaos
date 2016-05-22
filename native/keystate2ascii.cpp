
//	"keystate2ascii.cpp" -*- C++ -*-
//	Created on: 22 May 2016
//	Definitions for keyboard state numbers' conversion to ASCII codes.
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

#include "native/keystate2ascii.h"

#include <windows.h>

#include "native/keyboard.h"

int	KeystateToAscii(void)
{
#define MapTo(_begin_,_end_) if(kGetKeyOnpress(_begin_)){return _end_;}
#define	MapToC(_begin_,_end_) if(kGetKeyState(VK_SHIFT)){MapTo(_begin_,_end_)}
#define	MapToA(_begin_,_end1_,_end2_) MapToC(_begin_,_end2_);MapTo(_begin_,_end1_);
//	Functional keys
	MapTo(VK_SPACE, ' ');
	MapTo(VK_BACK, '\r'); // Backspace
	MapTo(VK_RETURN, '\n');
//	Alphabetical keys
	for (short int i = 0; i < 26; i++) {
		MapToA(i + 'A', i + 'a', i + 'A');
	}
//	Numerical keys
	const char*	NumMap = ")!@#$%^&*(";
	for (short int i = 0; i < 10; i++) {
		MapToA(i + '0', i + '0', NumMap[i]);
		MapTo(i + VK_NUMPAD0, i + '0');
	}
//	Symbolic keys
	MapToA(VK_OEM_1, ';', ':');
	MapToA(VK_OEM_2, '/', '?');
	MapTo(VK_OEM_3, '~');
	MapToA(VK_OEM_4, '[', '{');
	MapToA(VK_OEM_5, '\\', '|');
	MapToA(VK_OEM_6, ']', '}');
	MapTo(VK_ADD, '+');
	MapTo(VK_SUBTRACT, '-');
	MapTo(VK_DIVIDE, '/');
	MapTo(106, '*');
	MapToA(188, ',', '<');
	MapToA(190, '.', '>');
	MapTo(110, '.');
	return 0;
}
