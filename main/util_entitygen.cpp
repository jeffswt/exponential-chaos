
//	"util_entitygen.cpp" -*- C++ -*-
//	Created on: 18 May 2016
//	Definitions for entity generator that reads Comma Separated Values.
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

#include "main/util_entitygen.h"

#include <fstream>
#include <map>
#include <sstream>
#include <typeinfo>
#include <vector>

std::map<std::string, int>	Indexer;

template <typename Type>
bool	ImportCSVData(
		std::string	Source,
		Type		Target)
{
	std::stringstream	Stream;
	Stream.clear();
	Stream << Source;
	if (typeid(Type) == typeid(bool)) {
		Target = Source == "TRUE" ? true : false;
	} else {
		Stream >> Target;
	}
	return true;
}

std::vector<std::string>	my_preprocess_line(
		std::string	Line)
{
	std::string	Item = "";
	std::vector<std::string>	Vec;
	Line += ','; Vec.clear();
	for (unsigned int i = 0; i < Line.length(); i++)
		if (Line[i] == ',') {
			if (Item == "TRUE") Item = "true";
			if (Item == "FALSE") Item = "false";
			Vec.push_back(Item);
			Item = "";
		} else {
			Item += Line[i];
		}
	return Vec;
}

bool	my_process_first_line(
		std::string	Line)
{
	std::vector<std::string>	Vec;
	Vec = my_preprocess_line(Line);
	for (unsigned int i = 0; i < Vec.size(); i++)
		Indexer[Vec[i]] = i + 1;
	return true;
}

bool	my_process_line(
		std::string	Line)
{
	std::vector<std::string>	Vec;
	std::stringstream			Stream;
	Vec = my_preprocess_line(Line);
#define my_get_item(_stridx) (Indexer[_stridx] > 0 ? Vec[Indexer[_stridx] - 1] : "")
#define my_print_indent(_indent)\
	for (int _i = 0; _i < _indent * 4; _i++) Stream << ' ';
#define my_print(_indent, _jidx, _cidx, _term)\
	my_print_indent(_indent);\
	Stream << "\"" << _jidx << "\" : " << _cidx << _term << "\n";
#define my_echo(_indent, _jidx, _cidx, _term)\
	my_print(_indent, _jidx, my_get_item(_cidx), _term);
#define my_echo_(_indent, _jidx, _cidx)\
	my_echo(_indent, _jidx, _cidx, "")
#define my_echo_d(_indent, _jidx, _term)\
	my_echo(_indent, _jidx, "", _term)
#define my_echo_str(_indent, _jidx, _cidx, _term)\
	my_print(_indent, _jidx, "\"" << my_get_item(_cidx) << "\"", _term);
//	Things have been declared, now coming to the best part
	my_print_indent(0); Stream << "{\n";
	my_echo_d(1, "Properties", "{");
	my_echo_str(2, "Name", "Name", ",");
	my_echo_str(2, "Type", "Type", ",");
	my_echo_str(2, "Description", "Description", ",");
	my_echo_(2, "ShowInCreative", "ShowInCreative");
	my_print_indent(1); Stream << "},\n";
	my_echo_d(1, "Physics", "{");
	my_echo(2, "PhysicsEnabled", "PhysicsEnabled", ",");
	my_echo(2, "CollisionEnabled", "CollisionEnabled", ",");
	my_echo(2, "LengthX", "pLengthX", ",");
	my_echo(2, "LengthY", "pLengthX", ",");
	my_echo(2, "Mass", "Mass", ",");
	my_echo(2, "FrictionFactorTop", "FrictionFactorTop", ",");
	my_echo(2, "FrictionFactorBottom", "FrictionFactorBottom", ",");
	my_echo(2, "FrictionFactorLeft", "FrictionFactorLeft", ",");
	my_echo(2, "FrictionFactorRight", "FrictionFactorRight", ",");
	my_echo(2, "ElasticFactorTop", "ElasticFactorTop", ",");
	my_echo(2, "ElasticFactorBottom", "ElasticFactorBottom", ",");
	my_echo(2, "ElasticFactorLeft", "ElasticFactorLeft", ",");
	my_echo(2, "ElasticFactorRight", "ElasticFactorRight", ",");
	my_echo_(2, "BlastResistance", "BlastResistance");
	my_print_indent(1); Stream << "},\n";
	my_echo_d(1, "Graphics", "{");
	my_echo(2, "RenderEnabled", "RenderEnabled", ",");
	my_echo(2, "LengthX", "gLengthX", ",");
	my_echo(2, "LengthY", "gLengthY", ",");
	my_echo(2, "AnimationInterval", "AnimationInterval", ",");
	my_echo_str(2, "TextureOnHand", "TextureOnHand", ",");
	my_echo_d(2, "TextureList", "[");
	my_print_indent(3); Stream << "\"" << Vec[Indexer["Texture"] - 1] << "\"\n";
	my_print_indent(2); Stream << "]\n";
	my_print_indent(1); Stream << "}\n";
	Stream << "}\n";
//	Preparing to output...
	std::string		Output = "", sTemp, openFile;
	std::ofstream	Flow;
	while (getline(Stream, sTemp))
		Output += sTemp + '\n';
	openFile = my_get_item("ExportPath");
	if (openFile == "") {
		openFile = my_get_item("Name");
		for (char& c : openFile) {
			if (c == '.') c = '_';
			if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
		}
		openFile += ".json";
	}
	openFile = "./entities/" + openFile;
	Flow.open(openFile);
	Flow << Output;
	Flow.close();
	return true;
}

bool	utilGenerateEntityFromCSV(
		std::string	FileName)
{
	std::ifstream	Table;
	std::string		Line;
	Table.open(FileName.c_str());
	if (!Table)
		return false;
	getline(Table, Line);
	my_process_first_line(Line);
	while (getline(Table, Line))
		my_process_line(Line);
	Table.close();
	return true;
}
