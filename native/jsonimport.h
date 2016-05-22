
//	"jsonimport.h" -*- C++ -*-
//	Created on: 10 Apr 2016
//	Easier coding implementations / wrappers for RapidJSON.
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

#include "include/public.h"

#include <iostream>
#include <queue>
#include <rapidjson/document.h>
#include <typeinfo>

#ifndef NATIVE_JSONIMPORT_H_
#define NATIVE_JSONIMPORT_H_

/**
 *  @brief  import JSON data
 *  @param  valTarget: a reference to the output variable
 *          CurVal: the source JSON value.
 *  This template supports most STL classes, which are fundamental.
 */
template <typename _Type>
bool	ImportJsonData(
		_Type&				valTarget,
		rapidjson::Value&	CurVal)
{
	std::string	CurProc = "";
	std::string	TargetType;
	void*		pntTarget = &valTarget;
	TargetType = typeid(_Type).name();
	if (TargetType == typeid(int).name()) {
		if (!CurVal.IsInt())
			return false;
		*(int*)pntTarget = (int)CurVal.GetInt();
	} else if (TargetType == typeid(long long).name()) {
		if (!CurVal.IsInt64())
			return false;
		*(long long*)pntTarget = (long long)CurVal.GetInt64();
	} else if (TargetType == typeid(double).name()) {
		if (!CurVal.IsNumber())
			return false;
		*(double*)pntTarget = (double)CurVal.GetDouble();
	} else if (TargetType == typeid(bool).name()) {
		if (!CurVal.IsBool())
			return false;
		*(bool*)pntTarget = (bool)CurVal.GetBool();
	} else if (TargetType == typeid(std::string).name()) {
		if (!CurVal.IsString())
			return false;
		*(std::string*)pntTarget = (std::string)CurVal.GetString();
	} else if (TargetType == typeid(std::vector<double>).name()) {
		if (!CurVal.IsArray())
			return false;
		std::vector<double>* Target = (std::vector<double>*)pntTarget;
		for (unsigned int i = 0; i < CurVal.Size(); i++)
			if (CurVal[i].IsNumber())
				Target->push_back(CurVal[i].GetDouble());
	} else if (TargetType == typeid(std::vector<int>).name()) {
		if (!CurVal.IsArray())
			return false;
		std::vector<int>* Target = (std::vector<int>*)pntTarget;
		for (unsigned int i = 0; i < CurVal.Size(); i++)
			if (CurVal[i].IsNumber())
				Target->push_back(CurVal[i].GetInt());
	} else if (TargetType == typeid(std::vector<std::string>).name()) {
		if (!CurVal.IsArray())
			return false;
		std::vector<std::string>* Target = (std::vector<std::string>*)pntTarget;
		for (unsigned int i = 0; i < CurVal.Size(); i++)
			if (CurVal[i].IsString())
				Target->push_back(CurVal[i].GetString());
	}
	return true;
}

#endif /* NATIVE_JSONIMPORT_H_ */
