
//	"texture_png.h" -*- C++ -*-
//	Created on: 9 April 2016
//	OpenGL texture reader from PNG.
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

#ifndef NATIVE_TEXTURE_PNG_H_
#define NATIVE_TEXTURE_PNG_H_

#include "include/public.h"

#include <cstdlib>
#include <GL/gl.h>

GLuint	LoadPNGTexture(
		std::string	FileName,
		int* 		Width,
		int* 		Height);

#endif /* NATIVE_TEXTURE_PNG_H_ */
