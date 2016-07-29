
//	"log.cpp" -*- C++ -*-
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

#include "debug/log.h"

#include <thread>

#include "algo/blocking_queue.h"

int				eclogLevel = 9;
blocking_queue<std::pair<int, std::string>>
				eclogQueue;
std::thread*	eclogThreadId = NULL;

void	eclogThread(
		void)
{
	while (!eclogQueue.empty()) {
		auto workPr = eclogQueue.front();
		eclogQueue.pop();
		std::string	sTm2 = GetSystemTimeString(workPr.first), sTm;
		std::string	sCn = workPr.second;
		if (workPr.first == 0 && workPr.second == "TERMINATE")
			break;
		for (int i = 0; i < (int)sTm2.length() && i < 30; i++)
			if (sTm2[i] != '\n' && sTm2[i] != '\r')
				sTm += sTm2[i];
		sTm = "[" + sTm + "]";
		while (sTm.length() < 30)
			sTm += ' ';
		sCn = sTm + " " + sCn;
		std::cout << sCn << "\n" << std::flush;
	}
	return ;
}

bool	eclogThreadStart(
		void)
{
	if (eclogThreadId)
		return false;
	eclogThreadId = new std::thread(eclogThread);
	eclogPost(1, "Log manager started");
	return true;
}

bool	eclogThreadEnd(
		void)
{
	if (!eclogThreadId)
		return false;
	auto	workPr = make_pair(0, (std::string)"TERMINATE");
	eclogQueue.push(workPr);
	if (eclogThreadId->joinable())
		eclogThreadId->join();
	return true;
}

bool	eclogSetVerbosity(
		int	LogLevel)
{
	if (LogLevel < 0 || LogLevel > 10)
		return false;
	eclogLevel = LogLevel;
	eclogPost(1, "Verbosity set to " + eclogToString(LogLevel));
	return true;
}

bool	eclogPost(
		int			LogLevel,
		std::string	Content)
{
	if (LogLevel > eclogLevel)
		return true;
	auto	workPr = make_pair(GetSystemTime(), Content);
	eclogQueue.push(workPr);
	return true;
}
