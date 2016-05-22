
//	"entry.h" -*- C++ -*-
//	Created on: 5 March 2016
//	Entry functions to GLUT.
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

#ifndef GRAPHICS_ENTRY_H_
#define GRAPHICS_ENTRY_H_

#include "include/public.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <physics/importer.h>
#include <windows.h>
#include "graphics/render.h"

void	graphicsInit(
		void);

bool	graphicsEntry(
		int& ArgumentCount,
		char** ArgumentData);

#endif /* GRAPHICS_ENTRY_H_ */
