
//	"log.h" -*- C++ -*-
//	Created on: 29 July 2016
//	Game debug logger
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

#ifndef DEBUG_LOG_H_
#define DEBUG_LOG_H_

#include "include/public.h"

#include <iostream>
#include <sstream>

template <typename _T>
std::string	eclogToString(
		_T __x)
{
	std::stringstream	__z;
	std::string			__y;
	__z << __x;
	getline(__z, __y);
	return __y;
}

bool	eclogThreadStart(
		void);

bool	eclogThreadEnd(
		void);

bool	eclogSetVerbosity(
		int	LogLevel);

bool	eclogPost(
		int			LogLevel,
		std::string	Content);

#endif /* DEBUG_LOG_H_ */
