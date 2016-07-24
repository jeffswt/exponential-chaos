
//	"trigger.h" -*- C++ -*-
//	Created on: 10 April 2016
//	Trigger management.
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

#ifndef PHYSICS_TRIGGER_H_
#define PHYSICS_TRIGGER_H_

#include "include/public.h"

#include <iostream>
#include <rapidjson/document.h>
#include <vector>

class Trigger
{
protected:
	/**
	 *  @brief  All the consequences are deliberately given here
	 *  Re-compile is needed after addition of ProcessConsequence. Item should
	 *  be indexed in this->ProcessConsequence(Args).
	 */
	bool	tpcCreateEntity(void*, void*);
	bool	tpcDeductPlayerLife(void*, void*);
	bool	tpcDeployProjectile(void*, void*);
	bool	tpcTeleportEntity(void*, void*);
	bool	tpcWorldEdit_Fill_Begin(void*, void*);
	bool	tpcWorldEdit_Fill_End(void*, void*);
	bool	tpcWorldEdit_Copy_Begin(void*, void*);
	bool	tpcWorldEdit_Copy_End(void*, void*);
	bool	tpcWorldEdit_Copy_ApplyCopy(void*, void*);
	bool	tpcWorldEdit_Copy_ApplyMove(void*, void*);
public:
	double						LastTriggerTime;
	double						DelayTime;
	std::string					PreliminaryAction;
	std::vector<std::string>	PreliminaryObject;
	std::string					ConsequentialAction;
	std::vector<std::string>	ConsequentialObject;
	/**
	 *  @brief  import data from JSON value
	 *  @param  Config: The source value
	 *  Mostly returns true.
	 */
	bool	ImportFromJson(
			rapidjson::Value&	Config);
	/**
	 *  @brief  export data to JSON documents
	 *  @param  Config: the target value
	 *          CAlloc: the allocator of the target rapidjson::Document
	 *  Mostly returns true.
	 */
	bool	ExportToJson(
			rapidjson::Value&	Config,
			rapidjson::Document::
				AllocatorType&	CAlloc);
	/**
	 *  @brief  process consequence
	 *  After the parent function get to know that the consequence had to be
	 *  triggered, it sends an argument list to let the trigger process
	 *  related consequences. The consequence programming standards are referred
	 *  to the documentation.
	 */
	bool	ProcessConsequence(
			std::vector<void*>	Args);
	Trigger(void);
};

#endif /* PHYSICS_TRIGGER_H_ */
