
//	"time.h" -*- C++ -*-
//	Created on: 10 April 2016
//	Nanosecond implementation of timing functions.
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

#ifndef NATIVE_TIME_H_
#define NATIVE_TIME_H_

#include "include/public.h"

/**
 *  @brief  get current time
 *  This function returns the seconds since processor commence. That is, if the
 *  computer is *JUST* turned on, this returns 0.0.
 */
double	GetProcessTime(
		void);
double	GetProcessTimeUnsynced(
		void);

/**
 *  @brief  get actual time (GMT)
 */
int		GetSystemTime(
		void);

/**
 *  @brief  get the string for the given time
 *  @param  Time: the given time
 */
std::string	GetSystemTimeString(
		int		Time);

/**
 *  @brief  sleep function
 *  @param  EndTime: the termination time
 *  Sleep until EndTime.
 */
bool	SleepUntilTime(
		double	EndTime);

/**
 *  @brief  sleep function
 *  @param  BeginTime: the begin time
 *          Duration: the duration
 *  Sleep for Duration seconds from BeginTime.
 */
bool	SleepFromTime(
		double	BeginTime,
		double	Duration);

/**
 *  @brief  sleep function
 *  @param  Duration: the duration
 *  Sleep for Duration seconds from now.
 */
bool	SleepForTime(
		double	Duration);

#endif /* NATIVE_TIME_H_ */
