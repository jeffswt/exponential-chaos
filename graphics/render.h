
//	"render.h" -*- C++ -*-
//	Created on: 5 March 2016
//	OpenGL renderer dispatcher.
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

#ifndef GRAPHICS_RENDER_H_
#define GRAPHICS_RENDER_H_

#include "include/public.h"

#include <iostream>
#include <cstdio>
#include <ctime>
#include <GL/gl.h>
#include <GL/freeglut.h>

#include "physics/importer.h"

/**
 *  @brief  data processors given to the GLUT manager.
 *  @param  see GLUT documentation.
 *  Arguments would be passed to InputControl.
 */
void	graphicsMouseClick(int, int, int, int);
void	graphicsMouseMotionActive(int, int);
void	graphicsMouseMotionPassive(int, int);
void	graphicsMouseEntry(int);
void	graphicsRender(void);

#endif /* GRAPHICS_RENDER_H_ */
