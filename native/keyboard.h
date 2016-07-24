
//	"keyboard.h" -*- C++ -*-
//	Created on: 30 April 2016
//	Native keyboard state detection library.
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

#ifndef NATIVE_KEYBOARD_H_
#define NATIVE_KEYBOARD_H_

#include "include/public.h"

/**
 *  @brief  Some definitions of virtual key numbers.
 *  @param  to invoke, see kGet.*.
 */
enum KNUM {
	KNUM_LEFT,
	KNUM_RIGHT,
	KNUM_UP,
	KNUM_DOWN,
	KNUM_PGUP,
	KNUM_PGDN,
	KNUM_HOME,
	KNUM_END,
	KNUM_INSERT,
	KNUM_F1,
	KNUM_F2,
	KNUM_F3,
	KNUM_F4,
	KNUM_F5,
	KNUM_F6,
	KNUM_F7,
	KNUM_F8,
	KNUM_F9,
	KNUM_F10,
	KNUM_LCTRL,
	KNUM_RCTRL,
	KNUM_LALT,
	KNUM_RALT,
	KNUM_LSHIFT,
	KNUM_RSHIFT
};

/**
 *  @brief  kApi transformation functions
 *  @param  the inputted character / enumeration.
 */
int		kApiSpecialInterpret(KNUM);
KNUM	kApiSpecialInterpret(int);
char	kApiDecapitalize(char);

/**
 *  @brief  freeGlut entry functions
 *  @param  Defined by freeGlut APIs.
 */
void	kApiKeyboardFunc(unsigned char, int, int);
void	kApiKeyboardUpFunc(unsigned char, int, int);
void	kApiSpecialFunc(int, int, int);
void	kApiSpecialUpFunc(int, int, int);

/**
 *  @brief  query whether this key was just pressed
 *  @param  VirtualKey: the ASCII number indication.
 *                      or the given indication in keyboard.h.
 */
bool	kGetKeyOnpress(
		int		VirtualKey);
bool	kGetKeyOnpress(
		KNUM	VirtualKey);

/**
 *  @brief  query whether this key is pressed
 *  @param  VirtualKey: the VK number of the key (defined on MSDN).
 */
bool	kGetKeyState(
		int		VirtualKey);
bool	kGetKeyState(
		KNUM	VirtualKey);

/**
 *  @brief  Get last inputted ASCII key
 *  @param  sans parameters.
 */
char	kGetKeyInput(
		void);

#endif /* NATIVE_KEYBOARD_H_ */
