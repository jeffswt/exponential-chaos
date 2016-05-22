
//	"keystate2ascii.h" -*- C++ -*-
//	Created on: 22 May 2016
//	Keyboard state numbers' conversion to ASCII codes.
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

#ifndef NATIVE_KEYSTATE2ASCII_H_
#define NATIVE_KEYSTATE2ASCII_H_

#include "include/public.h"

/**
 *  @brief  get ASCII code for the just-pressed key
 *  Used mostly in inputting string data.
 */
int	KeystateToAscii(
		void);

#endif /* NATIVE_KEYSTATE2ASCII_H_ */
