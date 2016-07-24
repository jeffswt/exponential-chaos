
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
	int	Onpress[256];
	int	OnpressSpecial[256];
	int	OnpressAscii[256];
}

KNUM	kApiSpecialInterpret(int x)
{
	switch (x) {
	case 1: return KNUM_F1; break;
	case 2: return KNUM_F2; break;
	case 3: return KNUM_F3; break;
	case 4: return KNUM_F4; break;
	case 5: return KNUM_F5; break;
	case 6: return KNUM_F6; break;
	case 7: return KNUM_F7; break;
	case 8: return KNUM_F8; break;
	case 9: return KNUM_F9; break;
	case 10: return KNUM_F10; break;
	case 100: return KNUM_LEFT; break;
	case 102: return KNUM_RIGHT; break;
	case 101: return KNUM_UP; break;
	case 103: return KNUM_DOWN; break;
	case 108: return KNUM_INSERT; break;
	case 104: return KNUM_PGUP; break;
	case 105: return KNUM_PGDN; break;
	case 106: return KNUM_HOME; break;
	case 107: return KNUM_END; break;
	case 114: return KNUM_LCTRL; break;
	case 115: return KNUM_RCTRL; break;
	case 116: return KNUM_LALT; break;
	case 117: return KNUM_RALT; break;
	case 112: return KNUM_LSHIFT; break;
	case 113: return KNUM_RSHIFT; break;
	}
	return KNUM_F1;
}
int	kApiSpecialInterpret(KNUM x)
{
	switch (x) {
	case KNUM_F1: return 1; break;
	case KNUM_F2: return 2; break;
	case KNUM_F3: return 3; break;
	case KNUM_F4: return 4; break;
	case KNUM_F5: return 5; break;
	case KNUM_F6: return 6; break;
	case KNUM_F7: return 7; break;
	case KNUM_F8: return 8; break;
	case KNUM_F9: return 9; break;
	case KNUM_F10: return 10; break;
	case KNUM_LEFT: return 100; break;
	case KNUM_RIGHT: return 102; break;
	case KNUM_UP: return 101; break;
	case KNUM_DOWN: return 103; break;
	case KNUM_INSERT: return 108; break;
	case KNUM_PGUP: return 104; break;
	case KNUM_PGDN: return 105; break;
	case KNUM_HOME: return 106; break;
	case KNUM_END: return 107; break;
	case KNUM_LCTRL: return 114; break;
	case KNUM_RCTRL: return 115; break;
	case KNUM_LALT: return 116; break;
	case KNUM_RALT: return 117; break;
	case KNUM_LSHIFT: return 112; break;
	case KNUM_RSHIFT: return 113; break;
	}
	return 1;
}
char kApiDecapitalize(char x)
{
	switch (x) {
	case '~': return '`'; break;
	case '!': return '1'; break;
	case '@': return '2'; break;
	case '#': return '3'; break;
	case '$': return '4'; break;
	case '%': return '5'; break;
	case '^': return '6'; break;
	case '&': return '7'; break;
	case '*': return '8'; break;
	case '(': return '9'; break;
	case ')': return '0'; break;
	case '_': return '-'; break;
	case '+': return '='; break;
	case '{': return '['; break;
	case ']': return '}'; break;
	case '|': return '\\'; break;
	case ':': return ';'; break;
	case '"': return '\''; break;
	case '<': return ','; break;
	case '>': return '.'; break;
	case '?': return '/'; break;
	}
	if (x >= 'A' && x <= 'Z')
		return x - 'A' + 'a';
	return x;
}

void	kApiKeyboardFunc(unsigned char x, int y, int z)
{
	using namespace KeyboardInternals;
	OnpressAscii[x] = 1;
	x = kApiDecapitalize(x);
	if (Onpress[x] == 0)
		Onpress[x] = 1;
	else if (Onpress[x] == 1)
		Onpress[x] = 2;
	return ;
}
void	kApiKeyboardUpFunc(unsigned char x, int y, int z)
{
	using namespace KeyboardInternals;
	OnpressAscii[x] = 0;
	x = kApiDecapitalize(x);
	Onpress[x] = 0;
	return ;
}
void	kApiSpecialFunc(int x, int y, int z)
{
	using namespace KeyboardInternals;
	int	w = (int)((KNUM)kApiSpecialInterpret(x));
	if (OnpressSpecial[w] == 0)
		OnpressSpecial[w] = 1;
	else if (OnpressSpecial[w] == 1)
		OnpressSpecial[w] = 2;
	return ;
}
void	kApiSpecialUpFunc(int x, int y, int z)
{
	using namespace KeyboardInternals;
	int	w = (int)((KNUM)kApiSpecialInterpret(x));
	OnpressSpecial[w] = 0;
	return ;
}

bool	kGetKeyOnpress(
		int	vkey)
{
	using namespace KeyboardInternals;
	vkey = (int)kApiDecapitalize(vkey);
	if (Onpress[vkey] == 1) {
		Onpress[vkey] = 2;
		return true;
	}
	return false;
}
bool	kGetKeyOnpress(
		KNUM	vkey)
{
	using namespace KeyboardInternals;
	if (OnpressSpecial[(int)vkey] == 1) {
		OnpressSpecial[(int)vkey] = 2;
		return true;
	}
	return false;
}

bool	kGetKeyState(int vkey)
	{ return KeyboardInternals::Onpress[(int)kApiDecapitalize(vkey)] > 0; }
bool	kGetKeyState(KNUM vkey)
	{ return KeyboardInternals::OnpressSpecial[(int)vkey] > 0; }

char	kGetKeyInput(
		void)
{
	using namespace KeyboardInternals;
	for (int i = 1; i < 256; i++)
		if (OnpressAscii[i] > 0) {
			OnpressAscii[i] = 0;
			return (char)i;
		}
	return (char)0;
}
