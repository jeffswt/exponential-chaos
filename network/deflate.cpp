
//	"deflate.cpp" -*- C++ -*-
//	Created on: 17 May 2016
//	Definitions of ZLib wrapper for data compression / decompression
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

#include "network/deflate.h"

#include <zlib.h>

std::string	my_to_hex(
		int	_x)
{
	std::string	_y = "";
	const char*	chr_map = "0123456789ABCDEF........";
	for (int i = 0; i < 8; _x >>= 4, i++)
		_y = chr_map[_x & 0x0f] + _y;
	return _y;
}

int		my_from_hex(
		std::string&	_x)
{
	int	_y = 0;
	if (_x.length() < 8)
		return 0;
	char*			chr_map = new char[256];
	for (int i = 0; i <= 9; i++) chr_map[i + '0'] = i;
	for (int i = 0; i <= 5; i++) chr_map[i + 'A'] = i + 10;
	for (int i = 0; i < 8; i++)
		_y = (_y << 4) + chr_map[(int)_x[i]];
	delete chr_map;
	return _y;
}

bool	gameProtocolInflate(
		std::string&	Input)
{
//	return true;
	int				bufflen = Input.length() + 16;
	unsigned char*	inp_str = NULL;
	unsigned char*	out_str = NULL;
	try {
		inp_str = new unsigned char[bufflen];
		out_str = new unsigned char[bufflen];
	} catch (std::bad_alloc& exc) {
		if (inp_str && out_str) delete inp_str;
		return false;
	}
	const char*		chr_map = "0123456789ABCDEF........";
	memset(inp_str, 0, sizeof(char) * bufflen);
	memset(out_str, 0, sizeof(char) * bufflen);
	for (unsigned int i = 0; i < Input.length(); i++)
		inp_str[i] = Input[i];
	unsigned long int	dest_len = bufflen;
	unsigned long int	src_len = Input.length();
//	Here comes the best part of the library...
	int	ret_res = compress(out_str, &dest_len, (const unsigned char*)inp_str, src_len);
	Input.clear();
	if (ret_res != Z_OK) {
		delete inp_str;
		delete out_str;
		return false;
	}
	for (unsigned int i = 0; i < dest_len; i++) {
		Input += chr_map[out_str[i] >> 4];
		Input += chr_map[out_str[i] & 0x0f];
	}
	Input = my_to_hex(src_len) + Input;
	delete inp_str;
	delete out_str;
	return true;
}

bool	gameProtocolDeflate(
		std::string&	Input)
{
//	return true;
	int				bufflen = my_from_hex(Input) + 16;
	unsigned char*	inp_str = NULL;
	unsigned char*	out_str = NULL;
	char*			chr_map = NULL;
	try {
		inp_str = new unsigned char[bufflen];
		out_str = new unsigned char[bufflen];
		chr_map = new char[256];
	} catch (std::bad_alloc& exc) {
		if (inp_str && out_str) delete inp_str;
		if (out_str && chr_map) delete out_str;
		return false;
	}
	for (int i = 0; i <= 9; i++) chr_map[i + '0'] = i;
	for (int i = 0; i <= 5; i++) chr_map[i + 'A'] = i + 10;
	memset(inp_str, 0, sizeof(char) * bufflen);
	memset(out_str, 0, sizeof(char) * bufflen);
//	Memory leak, be aware
	if (bufflen < (int)Input.length() >> 1) {
		delete inp_str;
		delete out_str;
		delete chr_map;
		return false;
	}
	for (unsigned int i = 0; i < Input.length() >> 1; i++)
		inp_str[i] = (chr_map[(int)Input[(i << 1) + 8]] << 4)
				+ chr_map[(int)Input[(i << 1) + 1 + 8]];
	unsigned long int	dest_len = bufflen;
	unsigned long int	src_len = Input.length() >> 1;
//	Here comes the best part of the library...
	int	ret_res = uncompress(out_str, &dest_len, (const unsigned char*)inp_str, src_len);
	Input.clear();
	if (ret_res != Z_OK) {
		delete inp_str;
		delete out_str;
		delete chr_map;
		return false;
	}
	for (unsigned int i = 0; i < dest_len; i++)
		Input += out_str[i];
	delete inp_str;
	delete out_str;
	delete chr_map;
	return true;
}
