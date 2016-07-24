
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

#include <cmath>
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

bool	Trigger::tpcCreateEntity(
		void*	vMainMap,
		void*	vThisEntity)
{
	GameMap*	MainMap = (GameMap*)vMainMap;
	Entity*		ThisEntity = (Entity*)vThisEntity;
	std::string	LookupName = ConsequentialObject[0];
	Entity*		NewEnt = new Entity;
	EntityType*	NewTyp = EntityTypes[LookupName];
//	Failure, then we abandon it
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
//	Insertion of entity complete.
	return true;
}

bool	Trigger::tpcDeductPlayerLife(
		void*	vMainMap,
		void*	vThisEntity)
{
	GameMap*	MainMap = (GameMap*)vMainMap;
	Entity*		ThisEntity = (Entity*)vThisEntity;
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
//			Here we use manhattan distance for ease of computation
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
//	Deduction of player life complete.
	return true;
}

bool	Trigger::tpcDeployProjectile(
		void*	vMainMap,
		void*	vThisEntity)
{
	Entity*		ThisEntity = (Entity*)vThisEntity;
	EntityType*	EntTyp = ThisEntity->Properties.Type;
	if (EntTyp->Properties.Type == "Projectile") {
		ProjectileEntityType*	EntExt = (ProjectileEntityType*)
				EntTyp->Properties.SpecificProperties;
		ThisEntity->Properties.GenTime = GetProcessTimeUnsynced() -
				EntExt->DeployDelay;
	}
//	Deployment of projectile complete. The rest of the job are
//	dedicated to the physics renderers to complete.
	return true;
}

bool	Trigger::tpcTeleportEntity(
		void*	vMainMap,
		void*	vThisEntity)
{
	GameMap*	MainMap = (GameMap*)vMainMap;
	Entity*		ThisEntity = (Entity*)vThisEntity;
	double	tgX = 0.0, tgY = 0.0; // TargetX, TargetY
	if (ThisEntity->Properties.Owner != "") {
		Entity*		OwnrEnt = NULL;
//		Find this owner among the players
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
//	Teleportation of player entity complete.
	return true;
}

int		tpcWorldEdit_Fill_Pos[3];
int		tpcWorldEdit_Copy_Pos[6];
bool	tpcWorldEdit_Fill_Grant;
int		tpcWorldEdit_Copy_Grant;

bool	tpcWorldEdit_IsWorldEditBlock(
		Entity*	tEntity)
{
	if (tEntity->Properties.TypeName == "WorldEdit::Fill.Begin") return true;
	if (tEntity->Properties.TypeName == "WorldEdit::Fill.End") return true;
	if (tEntity->Properties.TypeName == "WorldEdit::Copy.Begin") return true;
	if (tEntity->Properties.TypeName == "WorldEdit::Copy.End") return true;
	if (tEntity->Properties.TypeName == "WorldEdit::Copy.ApplyCopy") return true;
	if (tEntity->Properties.TypeName == "WorldEdit::Copy.ApplyMove") return true;
	return false;
}

bool	Trigger::tpcWorldEdit_Fill_Begin(
		void*	vMainMap,
		void*	vThisEntity)
{
	Entity*		ThisEntity = (Entity*)vThisEntity;
	tpcWorldEdit_Fill_Grant = true;
	tpcWorldEdit_Fill_Pos[0] = (int)ceil(ThisEntity->Physics.PosX);
	tpcWorldEdit_Fill_Pos[1] = (int)ceil(ThisEntity->Physics.PosY);
	tpcWorldEdit_Fill_Pos[2] = ThisEntity->Properties.Layer;
	return true;
}

bool	Trigger::tpcWorldEdit_Fill_End(
		void*	vMainMap,
		void*	vThisEntity)
{
	GameMap*	MainMap = (GameMap*)vMainMap;
	Entity*		ThisEntity = (Entity*)vThisEntity;
	if (!tpcWorldEdit_Fill_Grant)
		return false;
	tpcWorldEdit_Fill_Grant = false;
//	Pre-define options to make code shorter (I know my code is long...)
	int	x1 = tpcWorldEdit_Fill_Pos[0],
		x2 = floor(ThisEntity->Physics.PosX),
		y1 = tpcWorldEdit_Fill_Pos[1],
		y2 = floor(ThisEntity->Physics.PosY),
		z1 = tpcWorldEdit_Fill_Pos[2],
		z2 = ThisEntity->Properties.Layer;
	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);
	if (z1 > z2) std::swap(z1, z2);
//	Find an appropriate block to use...
	Entity*		EntInherit = NULL;
	MainMap->RemoveEntityPended(ThisEntity);
	NetmgrRemoveEntity(ThisEntity);
#define my_get_arr_pos(a,b,c) (((a)-x1)*(y2-y1+1)*(z2-z1+1)+((b)-y1)*(z2-z1+1)+((c)-z1))
#define my_get_arr_size() ((x2-x1+1)*(y2-y1+1)*(z2-z1+1))
//	It might be astonishing, but short takes less memory than bool
	short*	IndexArr = new short[my_get_arr_size()];
	memset(IndexArr, 0, sizeof(short) * my_get_arr_size());
	for (auto itert : MainMap->EntityList) {
		Entity*	EntFind = itert.second;
//		A worldedit block should **NEVER** appear here as another fill object...
		if (!EntFind->DataIntact()) continue;
		if (tpcWorldEdit_IsWorldEditBlock(EntFind)) {
			MainMap->RemoveEntityPended(EntFind);
			NetmgrRemoveEntity(EntFind);
			continue;
		}
		if (EntFind->Physics.PosX < x1) continue;
		if (EntFind->Physics.PosX > x2) continue;
		if (EntFind->Physics.PosY < y1) continue;
		if (EntFind->Physics.PosY > y2) continue;
		if (EntFind->Properties.Layer < z1) continue;
		if (EntFind->Properties.Layer > z2) continue;
		if (EntFind->Properties.Type->Properties.Type != "Block") continue;
		IndexArr[my_get_arr_pos((int)EntFind->Physics.PosX,
				(int)EntFind->Physics.PosY,
				EntFind->Properties.Layer)] = 1;
		if (EntInherit == NULL)
			EntInherit = EntFind;
	}
	if (!EntInherit) {
		delete IndexArr;
		return false; // I've found nothing! How am I supposed to do it?
	}
//	Now it's time to fill up our area!
	for (int x = (int)ceil(x1); x <= x2; x++)
		for (int y = (int)ceil(y1); y <= y2; y++)
			for (int z = z1; z <= z2; z++) {
				if (IndexArr[my_get_arr_pos(x, y, z)])
					continue; // Marked used
				Entity*	EntMake = new Entity;
				EntMake->InheritFrom(EntInherit);
				EntMake->Physics.PosX = (double)x;
				EntMake->Physics.PosY = (double)y;
				EntMake->Properties.Layer = z;
				if (!EntMake->DataIntact())
					throw RuntimeErrorException();
				MainMap->InsertEntity(EntMake);
				NetmgrInsertEntity(EntMake);
			}
//	What a nice, happy ending!
	delete IndexArr;
	return true;
}

bool	Trigger::tpcWorldEdit_Copy_Begin(
		void*	vMainMap,
		void*	vThisEntity)
{
	Entity*		ThisEntity = (Entity*)vThisEntity;
	tpcWorldEdit_Copy_Grant = 1;
	tpcWorldEdit_Copy_Pos[0] = (int)ceil(ThisEntity->Physics.PosX);
	tpcWorldEdit_Copy_Pos[1] = (int)ceil(ThisEntity->Physics.PosY);
	tpcWorldEdit_Copy_Pos[2] = ThisEntity->Properties.Layer;
	return true;
}

bool	Trigger::tpcWorldEdit_Copy_End(
		void*	vMainMap,
		void*	vThisEntity)
{
	Entity*		ThisEntity = (Entity*)vThisEntity;
	if (tpcWorldEdit_Copy_Grant != 1)
		return false;
	tpcWorldEdit_Copy_Grant = 2;
	tpcWorldEdit_Copy_Pos[3] = (int)floor(ThisEntity->Physics.PosX);
	tpcWorldEdit_Copy_Pos[4] = (int)floor(ThisEntity->Physics.PosY);
	tpcWorldEdit_Copy_Pos[5] = ThisEntity->Properties.Layer;
	return true;
}

bool	Trigger::tpcWorldEdit_Copy_ApplyMove(
		void*	vMainMap,
		void*	vThisEntity)
{
	GameMap*	MainMap = (GameMap*)vMainMap;
	Entity*		ThisEntity = (Entity*)vThisEntity;
	if (tpcWorldEdit_Copy_Grant != 2)
		return false;
//	Moving shall be allowed merely once
	tpcWorldEdit_Copy_Grant = 0;
	int	x1 = tpcWorldEdit_Copy_Pos[0],
		x2 = tpcWorldEdit_Copy_Pos[3],
		x3 = floor(ThisEntity->Physics.PosX),
		y1 = tpcWorldEdit_Copy_Pos[1],
		y2 = tpcWorldEdit_Copy_Pos[4],
		y3 = floor(ThisEntity->Physics.PosY),
		z1 = tpcWorldEdit_Copy_Pos[2],
		z2 = tpcWorldEdit_Copy_Pos[5],
		z3 = ThisEntity->Properties.Layer;
	int	dx = x3 - x1,
		dy = y3 - y1,
		dz = z3 - z1;
	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);
	if (z1 > z2) std::swap(z1, z2);
	std::vector<Entity*>	mvVec;
	mvVec.clear();
	MainMap->RemoveEntityPended(ThisEntity);
	NetmgrRemoveEntity(ThisEntity);
//	Finding the objects satisfies the prerequisites to move
	for (auto itert : MainMap->EntityList) {
		Entity*	EntFind = itert.second;
//		A worldedit block should **NEVER** appear here as another fill object...
		if (!EntFind->DataIntact()) continue;
		if (tpcWorldEdit_IsWorldEditBlock(EntFind)) {
			MainMap->RemoveEntityPended(EntFind);
			NetmgrRemoveEntity(EntFind);
			continue;
		}
		if (EntFind->Physics.PosX < x1) continue;
		if (EntFind->Physics.PosX > x2) continue;
		if (EntFind->Physics.PosY < y1) continue;
		if (EntFind->Physics.PosY > y2) continue;
		if (EntFind->Properties.Layer < z1) continue;
		if (EntFind->Properties.Layer > z2) continue;
		if (EntFind->Properties.Type->Properties.Type != "Block") continue;
		mvVec.push_back(EntFind);
	}
//	Applying move operations to the objects
	for (Entity* mvEnt : mvVec) {
		MainMap->RemoveEntity(mvEnt);
		mvEnt->Properties.Layer += dz;
		mvEnt->Physics.PosX += dx;
		mvEnt->Physics.PosY += dy;
		NetmgrMoveEntity(mvEnt);
		MainMap->InsertEntity(mvEnt);
	}
	return true;
}

bool	Trigger::tpcWorldEdit_Copy_ApplyCopy(
		void*	vMainMap,
		void*	vThisEntity)
{
	GameMap*	MainMap = (GameMap*)vMainMap;
	Entity*		ThisEntity = (Entity*)vThisEntity;
	if (tpcWorldEdit_Copy_Grant != 2
			&& tpcWorldEdit_Copy_Grant != 3)
		return false;
//	To ensure that we may have the right to copy more duplicates, hitherto will
//	not make the state resetted. However, moving would be disabled, due to
//	certain explicit and understandable issues.
	tpcWorldEdit_Copy_Grant = 3;
	int	x1 = tpcWorldEdit_Copy_Pos[0],
		x2 = tpcWorldEdit_Copy_Pos[3],
		x3 = floor(ThisEntity->Physics.PosX),
		y1 = tpcWorldEdit_Copy_Pos[1],
		y2 = tpcWorldEdit_Copy_Pos[4],
		y3 = floor(ThisEntity->Physics.PosY),
		z1 = tpcWorldEdit_Copy_Pos[2],
		z2 = tpcWorldEdit_Copy_Pos[5],
		z3 = ThisEntity->Properties.Layer;
	int	dx = x3 - x1,
		dy = y3 - y1,
		dz = z3 - z1;
	if (x1 > x2) std::swap(x1, x2);
	if (y1 > y2) std::swap(y1, y2);
	if (z1 > z2) std::swap(z1, z2);
	std::vector<Entity*>	cpVec;
	cpVec.clear();
	MainMap->RemoveEntityPended(ThisEntity);
	NetmgrRemoveEntity(ThisEntity);
//	Finding the objects satisfies the prerequisites to move
	for (auto itert : MainMap->EntityList) {
		Entity*	EntFind = itert.second;
//		A worldedit block should **NEVER** appear here as another fill object...
		if (!EntFind->DataIntact()) continue;
		if (tpcWorldEdit_IsWorldEditBlock(EntFind)) {
			MainMap->RemoveEntityPended(EntFind);
			NetmgrRemoveEntity(EntFind);
			continue;
		}
		if (EntFind->Physics.PosX < x1) continue;
		if (EntFind->Physics.PosX > x2) continue;
		if (EntFind->Physics.PosY < y1) continue;
		if (EntFind->Physics.PosY > y2) continue;
		if (EntFind->Properties.Layer < z1) continue;
		if (EntFind->Properties.Layer > z2) continue;
		if (EntFind->Properties.Type->Properties.Type != "Block") continue;
		cpVec.push_back(EntFind);
	}
//	Applying move operations to the objects
	for (Entity* cpEnt : cpVec) {
		Entity*	EntMake = new Entity;
		EntMake->InheritFrom(cpEnt);
		EntMake->Physics.PosX = (double)(cpEnt->Physics.PosX + dx);
		EntMake->Physics.PosY = (double)(cpEnt->Physics.PosY + dy);
		EntMake->Properties.Layer = cpEnt->Properties.Layer + dz;
		if (!EntMake->DataIntact())
			throw RuntimeErrorException();
		MainMap->InsertEntity(EntMake);
		NetmgrInsertEntity(EntMake);
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
	void*	vMainMap = (void*)MainMap;
	void*	vThisEntity = (void*)ThisEntity;
	if (ConsequentialAction == "CreateEntity")
		return this->tpcCreateEntity(vMainMap, vThisEntity);
	if (ConsequentialAction == "DeductPlayerLife")
		return this->tpcDeductPlayerLife(vMainMap, vThisEntity);
	if (ConsequentialAction == "DeployProjectile")
		return this->tpcDeployProjectile(vMainMap, vThisEntity);
	if (ConsequentialAction == "TeleportEntity")
		return this->tpcTeleportEntity(vMainMap, vThisEntity);
	if (ConsequentialAction == "WorldEdit.Fill.Begin")
		return this->tpcWorldEdit_Fill_Begin(vMainMap, vThisEntity);
	if (ConsequentialAction == "WorldEdit.Fill.End")
		return this->tpcWorldEdit_Fill_End(vMainMap, vThisEntity);
	if (ConsequentialAction == "WorldEdit.Copy.Begin")
		return this->tpcWorldEdit_Copy_Begin(vMainMap, vThisEntity);
	if (ConsequentialAction == "WorldEdit.Copy.End")
		return this->tpcWorldEdit_Copy_End(vMainMap, vThisEntity);
	if (ConsequentialAction == "WorldEdit.Copy.ApplyCopy")
		return this->tpcWorldEdit_Copy_ApplyCopy(vMainMap, vThisEntity);
	if (ConsequentialAction == "WorldEdit.Copy.ApplyMove")
		return this->tpcWorldEdit_Copy_ApplyMove(vMainMap, vThisEntity);
	return false;
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
