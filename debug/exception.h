
//	"exception.h" -*- C++ -*-
//	Created on: 10 April 2016
//	Self defined exception handling.
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

#ifndef DEBUG_EXCEPTION_H_
#define DEBUG_EXCEPTION_H_

#include "include/public.h"

#include <iostream>
#include <stdexcept>

#define MakeSubException(_expname,_parname,_val)     \
		class _expname : public _parname { public:   \
		_expname(void) : _parname(_val) { };         \
		_expname(unsigned int ErrorCode) : _parname(ErrorCode) { }; };

bool InitializeErrorCodeMap(void);


// Level 0 exceptions ( baseband )
class Exception : public std::exception
{
protected:
	unsigned int datErrorCode;
public:
	Exception(void);
	Exception(unsigned int ErrorCode);
	const char* what(void);
};


// Level 1 exceptions
MakeSubException(RuntimeErrorException, Exception, 0x00000001);
MakeSubException(LogicErrorException, Exception, 0x00000002);
MakeSubException(UserspaceErrorException, Exception, 0x00000003);

// Level 2 exceptions
MakeSubException(CPUException, RuntimeErrorException, 0x00000011);
MakeSubException(MemoryException, RuntimeErrorException, 0x00000012);
MakeSubException(GPUException, RuntimeErrorException, 0x00000013);
MakeSubException(HardDriveException, RuntimeErrorException, 0x00000014);
MakeSubException(OperatingSystemException, LogicErrorException, 0x00000015);

MakeSubException(STLException, LogicErrorException, 0x00000021);
MakeSubException(OutOfBoundException, LogicErrorException, 0x00000022);
MakeSubException(ArithmeticException, LogicErrorException, 0x00000023);

MakeSubException(ConfigErrorException, UserspaceErrorException, 0x00000031);
MakeSubException(DataErrorException, UserspaceErrorException, 0x00000032);
MakeSubException(ElementErrorException, UserspaceErrorException, 0x00000033);

// Level 3 exceptions
MakeSubException(InstructionFailureException, CPUException, 0x00000111);
MakeSubException(InstructionDenialException, CPUException, 0x00000112);

MakeSubException(IllegalPointerException, MemoryException, 0x00000121);
MakeSubException(NullPointerException, MemoryException, 0x00000122);
MakeSubException(MemoryOverflowException, MemoryException, 0x00000123);
MakeSubException(MemoryUnderflowException, MemoryException, 0x00000124);

MakeSubException(OpenGLException, GPUException, 0x00000131);
MakeSubException(DirectXException, GPUException, 0x00000132);

MakeSubException(HardDriveFailureException, HardDriveException, 0x00000141);
MakeSubException(FileSystemCorruptException, HardDriveException, 0x00000142);
MakeSubException(ReadDenialException, HardDriveException, 0x00000143);
MakeSubException(WriteDenialException, HardDriveException, 0x0000144);

MakeSubException(OperationDenialException, OperatingSystemException, 0x00000151);
MakeSubException(BlueScreenException, OperatingSystemException, 0x00000152);
MakeSubException(KernelFaultException, OperatingSystemException, 0x00000153);

MakeSubException(JsonMissingException, ConfigErrorException, 0x00000311);
MakeSubException(JsonFaultyException, ConfigErrorException, 0x00000312);
MakeSubException(MisledValueException, ConfigErrorException, 0x00000313);

MakeSubException(TextureMissingException, ElementErrorException, 0x00000331);

#endif /* DEBUG_EXCEPTION_H_ */
