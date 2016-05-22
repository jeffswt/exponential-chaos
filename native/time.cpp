
//	"time.cpp" -*- C++ -*-
//	Created on: 11 April 2016
//	Definitions for nanosecond implementation of timing functions.
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

#include "native/time.h"

#include <chrono>
#include <ctime>
#include <thread>

double	CurProcessTimeUnsynced = 0.0;

double	GetProcessTime(
		void)
{
	std::chrono::steady_clock	CurTimeClock;
	double						CurTime;
	CurTime = std::chrono::duration_cast<
			std::chrono::nanoseconds>(
			CurTimeClock.now().time_since_epoch()).count() /
			1000000000.0;
	CurProcessTimeUnsynced = CurTime;
	return CurTime;
}

double	GetProcessTimeUnsynced(
		void)
{
	return CurProcessTimeUnsynced;
}

int		GetSystemTime(
		void)
{
	time_t	CurTime;
	CurTime = time(&CurTime);
	return (int)CurTime;
}

std::string	GetSystemTimeString(
		int	CurTime)
{
	time_t	TmTime = CurTime;
	return ctime(&TmTime);
}

bool	SleepUntilTime(
		double	EndTime)
{
	std::this_thread::sleep_until(
			std::chrono::steady_clock::time_point(
			std::chrono::duration<int64_t, std::nano>(
			(int64_t)(EndTime * 1000000000.0)
			)));
	return true;
}
bool	SleepFromTime(
		double	BeginTime,
		double	Duration)
{
	return SleepUntilTime(BeginTime + Duration);
}
bool	SleepForTime(
		double	Duration)
{
	return SleepFromTime(GetProcessTime(), Duration);
}
