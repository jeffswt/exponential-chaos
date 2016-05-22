
//	"guid.cpp" -*- C++ -*-
//	Created on: 8 March 2016
//	Definitions for GUID generation and management.
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

#include "native/guid.h"

/*
 * GenerateGuid : generates a globally unique identifier
 *
 * Generates a 64 bit number / actually 56 bits in case of overflowing. Potential
 * of duplication must be defined by server in order to acknowledge duplication,
 * yet not one should directly use this except the server.
 *
 * Returns a long long value, representing the generated GUID.
 */
long long GenerateGuid(void) {
	/* This GUID contains 56 bits instead of 64 bits for reading easiness */
	long long ret_val;
	ret_val = ((long long)rand() << 48)
		+ ((long long)rand() << 40)
		+ ((long long)rand() << 32)
		+ ((long long)rand() << 24)
		+ ((long long)rand() << 16)
		+ ((long long)rand() << 8)
		+ ((long long)rand());
	return ret_val;
}
