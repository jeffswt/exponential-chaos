
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
 *  @brief  query whether this key was just pressed
 *  @param  VirtualKey: the VK number of the key (defined on MSDN).
 */
bool	kGetKeyOnpress(
		int	VirtualKey);

/**
 *  @brief  query whether this key is pressed
 *  @param  VirtualKey: the VK number of the key (defined on MSDN).
 */
bool	kGetKeyState(
		int	VirtualKey);

/**
 *  @brief  query whether this key is pressed
 *  @param  VirtualKey: the VK number of the key (defined on MSDN).
 *  This function does not provide up-to-date results.
 */
bool	kGetKeyStateUnsynced(
		int	VirtualKey);

/**
 *  @brief  query the last time this key was pressed
 *  @param  VirtualKey: the VK number of the key (defined on MSDN).
 */
double	kGetKeyLastPressedTime(
		int	VirtualKey);

#endif /* NATIVE_KEYBOARD_H_ */
