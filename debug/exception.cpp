
//	"exception.cpp" -*- C++ -*-
//	Created on: 10 April 2016
//	Definitions of self defined exception handling.
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

#include "debug/exception.h"

#include <map>

std::map<unsigned int, std::string> ErrorCodeMapUS;
std::map<std::string, unsigned int> ErrorCodeMapSU;
bool ErrorCodeMapInitialized = false;

Exception::Exception(void)
{
	datErrorCode = 0x00000000;
	return ;
}

Exception::Exception(unsigned int ErrorCode)
{
	if (ErrorCodeMapUS.find(ErrorCode) == ErrorCodeMapUS.end())
		datErrorCode = 0x00000000;
	else
		datErrorCode = ErrorCode;
	return ;
}

const char* Exception::what(void)
{
	return ErrorCodeMapUS[datErrorCode].c_str();
}

bool InjectErrorCode(unsigned int ErrorCode, std::string ErrorInfo)
{
	if (ErrorInfo.length() < 1)
		ErrorInfo = "Undeclared error";
	ErrorCodeMapUS[ErrorCode] = ErrorInfo;
	ErrorCodeMapSU[ErrorInfo] = ErrorCode;
	return true;
}

bool InitializeErrorCodeMap(void)
{
	if (ErrorCodeMapInitialized) {
		ErrorCodeMapInitialized = false;
		return InitializeErrorCodeMap();
	}
	ErrorCodeMapUS.clear();
	ErrorCodeMapSU.clear();
	InjectErrorCode(0x00000000, "Unknown error");
	InjectErrorCode(0x00000001, "Runtime error: reason unknown");
	InjectErrorCode(0x00000002, "Logic error: reason unknown");
	InjectErrorCode(0x00000003, "Userspace error: reason unknown");
	InjectErrorCode(0x00000011, "The Central Processing Unit has thrown an indeterminable and catastrophic fault.");
	InjectErrorCode(0x00000012, "A problematic operation calling the Random Access Memory had been interrupted to assure stability.");
	InjectErrorCode(0x00000013, "It is told that the Graphics Processing Unit has indicated a problem.");
	InjectErrorCode(0x00000014, "Computer Hard Drive had accommodated you with a trivial yet appealing fault.");
	InjectErrorCode(0x00000015, "Strangely implemented functions in the Operating System had ceased to work in order to support this program.");
	InjectErrorCode(0x00000021, "C++ Standard Library had aborted its operation due to unknown issues.");
	InjectErrorCode(0x00000022, "Memory access had exceeded its given limits.");
	InjectErrorCode(0x00000023, "Particular arithmetic operations had caused agitating problems.");
	InjectErrorCode(0x00000031, "The configuration file given is not quite right, according to certain standards.");
	InjectErrorCode(0x00000032, "Data provided to the program is not regarded correct, re-checking may be necessary.");
	InjectErrorCode(0x00000033, "Particular elements in the edges of the program had caused imminent issues.");
	InjectErrorCode(0x00000111, "Certain commands passed to the CPU had caused functional problems.");
	InjectErrorCode(0x00000112, "The CPU had apparently denied some crucial commands of the program.");
	InjectErrorCode(0x00000121, "Pointer illegally used in program, we are sorry for this inconvenience.");
	InjectErrorCode(0x00000122, "Null Pointer illegally used in program, we are sorry for this inconvenience.");
	InjectErrorCode(0x00000123, "Memory iteration overflow. Checking upper boundaries may solve the problem.");
	InjectErrorCode(0x00000124, "Memory iteration underflow. Checking subtraction arithmetic operations may solve the problem.");
	InjectErrorCode(0x00000131, "OpenGL library had caused certain problems.");
	InjectErrorCode(0x00000132, "DirectX library had caused certain problems.");
	InjectErrorCode(0x00000141, "The hard drive had physically failed. Please consider changing the disk.");
	InjectErrorCode(0x00000142, "The file system had reported logical problems in the INODE table. Running disk checking utilities can be helpful.");
	InjectErrorCode(0x00000143, "The read operation had been officially denied by the computer.");
	InjectErrorCode(0x00000144, "The write operation had been officially denied by the computer.");
	InjectErrorCode(0x00000151, "Operating system had blocked the API call to a certain function.");
	InjectErrorCode(0x00000152, "Windows had encountered BlueScreenOfDeath.");
	InjectErrorCode(0x00000153, "Linux had encountered a Kernel Panic.");
	InjectErrorCode(0x00000311, "The JSON file is missing, according to the given instructions.");
	InjectErrorCode(0x00000312, "The JSON file has unbearably faulty grammar.");
	InjectErrorCode(0x00000313, "The values provided in the JSON file had been proved to be against certain boundaries.");
	InjectErrorCode(0x00000331, "The Texture File is missing.");
	return true;
}

