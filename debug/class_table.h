
//	"class_table.h" -*- C++ -*-
//	Created on: 15 May 2016
//	To produce visually comfortable console output while debugging.
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

#ifndef DEBUG_CLASS_TABLE_H_
#define DEBUG_CLASS_TABLE_H_

#include <iomanip>
#include <iostream>
#include <sstream>

template <typename std_ostream>
std::ostream&	class_table_head(
		std_ostream&	stream)
{
	const	int	identity_length = 30;
	const	int	value_length = 50;
	int			pos = 0;
	for (pos = 0; pos < (identity_length + 2 - 8) >> 1; pos++)
		stream << " ";
	stream << "Identity"; pos += 8;
	for (; pos < identity_length + 2; pos++)
		stream << " ";
	stream << "|"; pos++;
	for (int i = 0; i < (value_length + 1 - 5) >> 1; i++, pos++)
		stream << " ";
	stream << "Value\n";
	for (int i = 0; i < identity_length + 2; i++)
		stream << "-";
	stream << "+";
	for (int i = 0; i < value_length + 1; i++)
			stream << "-";
	stream << "\n";
	return stream;
}

template <typename _T>
std::ostream&	class_table_body(
		std::ostream&	stream,
		std::string		identity,
		_T&				value)
{
	const	int			identity_length = 30;
	const	int			value_length = 50;
	std::stringstream	value_stream;
	std::string			buffer;
	for (unsigned int i = 0; i < identity_length; i++)
		identity += " ";
	stream << " ";
	for (unsigned int i = 0; i < identity_length; i++)
		stream << identity[i];
	stream << " | ";
	value_stream.clear();
	buffer.clear();
	value_stream << std::boolalpha << std::fixed << std::setprecision(8);
	value_stream << value;
	getline(value_stream, buffer);
	for (unsigned int i = 0; i < value_length; i++)
		buffer += " ";
	for (unsigned int i = 0; i < value_length; i++)
		stream << buffer[i];
	stream << "\n";
	return stream;
}

template <typename std_ostream>
std::ostream&	class_table_end(
		std_ostream&	stream)
{
	stream << "\n";
	return stream;
}

#endif /* DEBUG_CLASS_TABLE_H_ */
