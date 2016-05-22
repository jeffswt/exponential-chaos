
//	"trigger.cpp" -*- C++ -*-
//	Created on: 10 April 2016
//	Definitions of trigger management.
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

#include "network/netsync.h"

#include "physics/trigger.h"

#include <map>
#include <set>
#include <sstream>

#include "native/jsonimport.h"
#include "physics/entitytype.h"
#include "physics/gamemap.h"

extern	std::map<std::string, EntityType*>	EntityTypes;

bool	Trigger::ImportFromJson(
		rapidjson::Value&	Config)
{
	if (!Config.IsObject())
		return false;
	if (!ImportJsonData(DelayTime, Config["DelayTime"]))
		DelayTime = 1.0; // Setting default as 1.0.
	if (!ImportJsonData(PreliminaryAction, Config["PreliminaryAction"]))
		return false;
	ImportJsonData(PreliminaryObject, Config["PreliminaryObject"]);
	if (!ImportJsonData(ConsequentialAction, Config["ConsequentialAction"]))
		return false;
	ImportJsonData(ConsequentialObject, Config["ConsequentialObject"]);
	return true;
}

bool	Trigger::ExportToJson(
		rapidjson::Value&	Config,
		rapidjson::Document::
			AllocatorType&	CAlloc)
{
	Config.SetObject();
	Config.AddMember("DelayTime", DelayTime, CAlloc);
	Config.AddMember("PreliminaryAction", PreliminaryAction, CAlloc);
	if (PreliminaryObject.size() > 0) {
		Config.AddMember("PreliminaryObject", rapidjson::Value(rapidjson::kArrayType), CAlloc);
		for (std::string str : PreliminaryObject)
			Config["PreliminaryObject"].PushBack(str, CAlloc);
	}
	Config.AddMember("ConsequentialAction", ConsequentialAction, CAlloc);
	if (ConsequentialObject.size() > 0) {
		Config.AddMember("ConsequentialObject", rapidjson::Value(rapidjson::kArrayType), CAlloc);
		for (std::string str : ConsequentialObject)
			Config["ConsequentialObject"].PushBack(str, CAlloc);
	}
	return true;
}

bool	Trigger::ProcessConsequence(
		std::vector<void*>	Args)
{
	GameMap*	MainMap = NULL;
	Entity*		ThisEntity = NULL;
	double		TimeB, TimeE;
//	Determine whether the time was enough or not
	TimeB = LastTriggerTime;
	TimeE = GetProcessTimeUnsynced();
	if (TimeE - TimeB < DelayTime)
		return true;
//	If triggered, then we shall update the time...
	LastTriggerTime = TimeE;
	if (Args.size() >= 1) MainMap = (GameMap*)Args[0];
	if (Args.size() >= 2) ThisEntity = (Entity*)Args[1];
	if (MainMap == NULL) return false;
	if (!ThisEntity->DataIntact()) return false;
	if (ConsequentialAction == "CreateEntity") {
		std::string	LookupName = ConsequentialObject[0];
		Entity*		NewEnt = new Entity;
		EntityType*	NewTyp = EntityTypes[LookupName];
//		Failure, then we abandon it
		if (!NewEnt->InheritFrom(NewTyp)) {
			delete NewEnt;
			return false;
		}
		if (ConsequentialObject.size() == 1) {
			NewEnt->Physics.PosX = ThisEntity->Physics.PosX;
			NewEnt->Physics.PosY = ThisEntity->Physics.PosY;
		} else if (ConsequentialObject.size() == 3) {
			std::stringstream	Stream;
			Stream << ConsequentialObject[1];
			Stream >> NewEnt->Physics.PosX;
			Stream << ConsequentialObject[2];
			Stream >> NewEnt->Physics.PosY;
		}
		NewEnt->Properties.Owner = ThisEntity->Properties.Owner;
		NetmgrInsertEntity(NewEnt);
		MainMap->InsertEntityPendedForce(NewEnt);
//		Insertion of entity complete.
	} else if (ConsequentialAction == "DeductPlayerLife") {
		double		DeductVal = 0.0;
		std::string	LookupName = ConsequentialObject[1];
		std::stringstream	Stream;
		Stream << ConsequentialObject[0];
		Stream >> DeductVal;
		if (LookupName == "__ZwTrigger7All") {
			for (auto itert : MainMap->PlayerEntityList)
			if (itert.second->DataIntact()) {
				PlayerEntity*	PlayerExt = (PlayerEntity*)itert.second->
						Physics.ExtendedTags;
				PlayerExt->Life -= DeductVal;
			}
		} else if (LookupName == "__ZwTrigger7Nearest") {
			Entity*	NearestEnt = NULL;
			double	NearestVal = 32767.0;
			for (auto itert : MainMap->PlayerEntityList)
			if (itert.second->DataIntact()) {
//				Here we use manhattan distance for ease of computation
				double	Dist = abs(itert.second->Physics.PosX - ThisEntity->Physics.PosX) +
						abs(itert.second->Physics.PosY - ThisEntity->Physics.PosY);
				if (Dist < NearestVal) {
					NearestEnt = itert.second;
					NearestVal = Dist;
				}
				NetmgrSetEntityLife(itert.second);
			}
			if (NearestEnt->DataIntact()) {
				PlayerEntity*	PlayerExt = (PlayerEntity*)NearestEnt->Physics.ExtendedTags;
				NetmgrSetEntityLife(NearestEnt);
				PlayerExt->Life -= DeductVal;
			}
		} else {
			for (auto itert : MainMap->PlayerEntityList) {
				if (!itert.second->DataIntact())
					continue;
				if (itert.second->Properties.Name != LookupName)
					continue;
				PlayerEntity*	PlayerExt = (PlayerEntity*)itert.second->
						Physics.ExtendedTags;
				PlayerExt->Life -= DeductVal;
				NetmgrSetEntityLife(itert.second);
				break;
			}
		}
//		Deduction of player life complete.
	} else if (ConsequentialAction == "DeployProjectile") {
		EntityType*	EntTyp = ThisEntity->Properties.Type;
		if (EntTyp->Properties.Type == "Projectile") {
			ProjectileEntityType*	EntExt = (ProjectileEntityType*)
					EntTyp->Properties.SpecificProperties;
			ThisEntity->Properties.GenTime = GetProcessTimeUnsynced() -
					EntExt->DeployDelay;
		}
//		Deployment of projectile complete. The rest of the job are
//		dedicated to the physics renderers to complete.
	} else if (ConsequentialAction == "TeleportEntity") {
		double	tgX = 0.0, tgY = 0.0; // TargetX, TargetY
		if (ThisEntity->Properties.Owner != "") {
			Entity*		OwnrEnt = NULL;
//			Find this owner among the players
			for (auto itert : MainMap->PlayerEntityList)
				if (itert.second && itert.second->Properties.Name ==
						ThisEntity->Properties.Owner)
					{ OwnrEnt = itert.second; break; }
			tgX = ThisEntity->Physics.PosX;
			tgY = ThisEntity->Physics.PosY;
			if (ConsequentialObject.size() == 2) {
				std::stringstream	Stream;
				Stream << ConsequentialObject[0];
				Stream >> tgX;
				Stream << ConsequentialObject[1];
				Stream >> tgY;
			}
			if (OwnrEnt && OwnrEnt->DataIntact()) {
				PlayerEntity*	PlyrTyp = (PlayerEntity*)OwnrEnt->Physics.ExtendedTags;
				PlyrTyp->LastJumpHeight = tgY;
				MainMap->InsertEntityPended(OwnrEnt, tgX, tgY);
			}
		}
//		Teleportation of player entity complete.
	} else {
		return false;
	}
	return true;
}

Trigger::Trigger(
		void)
{
	LastTriggerTime = 0.0;
	DelayTime = 1.0;
	PreliminaryAction = "";
	PreliminaryObject.clear();
	ConsequentialAction = "";
	ConsequentialObject.clear();
}
