
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
		if (OwnrEnt->DataIntact()) {
			PlayerEntity*	PlyrTyp = (PlayerEntity*)OwnrEnt->Physics.ExtendedTags;
			PlyrTyp->LastJumpHeight = tgY;
			MainMap->InsertEntityPended(OwnrEnt, tgX, tgY);
		}
	}
//	Teleportation of player entity complete.
	return true;
}

int		tpcWorldEdit_Fill_Pos[3];
int		tpcWorldEdit_Fill_Grant = 0;
Entity*	tpcWorldEdit_Fill_Choice = NULL;
int		tpcWorldEdit_Copy_Pos[6];
int		tpcWorldEdit_Copy_Grant = 0;

bool	tpcWorldEdit_IsWorldEditBlock(
		Entity*	tEntity)
{
	if (tEntity->Properties.TypeName == "WorldEdit::Fill.Pick") return true;
	if (tEntity->Properties.TypeName == "WorldEdit::Fill.Begin") return true;
	if (tEntity->Properties.TypeName == "WorldEdit::Fill.End") return true;
	if (tEntity->Properties.TypeName == "WorldEdit::Copy.Begin") return true;
	if (tEntity->Properties.TypeName == "WorldEdit::Copy.End") return true;
	if (tEntity->Properties.TypeName == "WorldEdit::Copy.ApplyCopy") return true;
	if (tEntity->Properties.TypeName == "WorldEdit::Copy.ApplyMove") return true;
	return false;
}

bool	Trigger::tpcWorldEdit_Fill_Pick(
		void*	vMainMap,
		void*	vThisEntity)
{
	GameMap*	MainMap = (GameMap*)vMainMap;
	Entity*		ThisEntity = (Entity*)vThisEntity;
	if (!MainMap->IsHost) return false; // This will lead to security problems
	tpcWorldEdit_Fill_Choice = NULL;
	int	ChnkPos = GetEntityChunkNum(ThisEntity->Physics.PosX);
	Chunk*	WrkChnk = MainMap->ChunkList[ChnkPos];
	if (!WrkChnk) return false;
	if (WrkChnk->EntityLayerList.find(ThisEntity->Properties.Layer) == WrkChnk->EntityLayerList.end()) return false;
	for (auto itert : WrkChnk->EntityLayerList[ThisEntity->Properties.Layer]) {
		Entity*	fndEnt = itert.second;
		if (!fndEnt->DataIntact()) continue;
		if (tpcWorldEdit_IsWorldEditBlock(fndEnt)) continue;
		if (abs(ThisEntity->Physics.PosX - fndEnt->Physics.PosX) > 0.2) continue;
		if (abs(ThisEntity->Physics.PosY - fndEnt->Physics.PosY) > 0.2) continue;
		if (ThisEntity->Properties.Layer != fndEnt->Properties.Layer) continue;
		tpcWorldEdit_Fill_Choice = fndEnt;
	}
	tpcWorldEdit_Fill_Grant = 1;
	return true;
}

bool	Trigger::tpcWorldEdit_Fill_Begin(
		void*	vMainMap,
		void*	vThisEntity)
{
	GameMap*	MainMap = (GameMap*)vMainMap;
	Entity*		ThisEntity = (Entity*)vThisEntity;
	if (!MainMap->IsHost) return false; // This will lead to security problems
	if (tpcWorldEdit_Fill_Grant != 1
			&& tpcWorldEdit_Fill_Grant != 2)
		return false;
	tpcWorldEdit_Fill_Grant = 2;
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
	if (!MainMap->IsHost) return false; // This will lead to security problems
	if (tpcWorldEdit_Fill_Grant != 2)
		return false;
	tpcWorldEdit_Fill_Grant = 1;
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
	Entity*	EntInherit = tpcWorldEdit_Fill_Choice;
	int	cx1 = GetEntityChunkNum(x1 - 1);
	int	cx2 = GetEntityChunkNum(x2 + 1);
//	This seems more slower in the surface, but on most cases work faster, if not
//	the player fills everything in one chunk in one layer. This would be certainly
//	astonishing and unacceptable, merely disadvantaging, but will eventually occur
//	if someone intends to test my codes. On this occasion this works slower, yet
//	the time complexity stays same.
	for (int x = cx1; x <= cx2; x++) {
		Chunk*	WrkChnk = MainMap->ChunkList[x];
		if (!WrkChnk) continue;
		for (int z = z1; z <= z2; z++) {
			if (WrkChnk->EntityLayerList.find(z) == WrkChnk->EntityLayerList.end()) continue;
			for (auto itert : WrkChnk->EntityLayerList[z]) {
				Entity*	EntFind = itert.second;
				if (!EntFind->DataIntact()) continue;
//				A worldedit block should **NEVER** appear here as another fill object...
				if (tpcWorldEdit_IsWorldEditBlock(EntFind)) {
					MainMap->RemoveEntityPended(EntFind);
					NetmgrRemoveEntity(EntFind);
					continue;
				}
				if (EntFind->Physics.PosX < x1) continue;
				if (EntFind->Physics.PosX > x2) continue;
				if (EntFind->Physics.PosY < y1) continue;
				if (EntFind->Physics.PosY > y2) continue;
				if (EntFind->Properties.Type->Properties.Type != "Block") continue;
//				As everything would be filled, what would be the meaning to stay here?
				MainMap->RemoveEntityPended(EntFind);
				NetmgrRemoveEntity(EntFind);
			}
		}
	}
	if (!EntInherit)
		return false; // I've found nothing! Then I'll leave the other things alone.
//	Now it's time to fill up our area!
	if (EntInherit != NULL)
	for (int x = (int)ceil(x1); x <= x2; x++)
		for (int y = (int)ceil(y1); y <= y2; y++)
			for (int z = z1; z <= z2; z++) {
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
	return true;
}

bool	Trigger::tpcWorldEdit_Copy_Begin(
		void*	vMainMap,
		void*	vThisEntity)
{
	GameMap*	MainMap = (GameMap*)vMainMap;
	Entity*		ThisEntity = (Entity*)vThisEntity;
	if (!MainMap->IsHost) return false; // This will lead to security problems
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
	GameMap*	MainMap = (GameMap*)vMainMap;
	Entity*		ThisEntity = (Entity*)vThisEntity;
	if (!MainMap->IsHost) return false; // This will lead to security problems
	if (tpcWorldEdit_Copy_Grant != 1
			&& tpcWorldEdit_Copy_Grant != 2)
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
	if (!MainMap->IsHost) return false; // This will lead to security problems
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
	int	cx1 = GetEntityChunkNum(x1 - 1);
	int	cx2 = GetEntityChunkNum(x2 + 1);
	for (int x = cx1; x <= cx2; x++) {
		Chunk*	WrkChnk = MainMap->ChunkList[x];
		if (!WrkChnk) continue;
		for (int z = z1; z <= z2; z++) {
			if (WrkChnk->EntityLayerList.find(z) == WrkChnk->EntityLayerList.end()) continue;
			for (auto itert : WrkChnk->EntityLayerList[z]) {
				Entity*	EntFind = itert.second;
//				A worldedit block should **NEVER** appear here as another fill object...
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
		}
	}
//	Applying move operations to the objects
	for (Entity* mvEnt : mvVec) {
		mvEnt->Properties.Layer += dz;
		mvEnt->Physics.PosX += dx;
		mvEnt->Physics.PosY += dy;
		NetmgrMoveEntity(mvEnt);
		mvEnt->Properties.Layer -= dz;
		mvEnt->Physics.PosX -= dx;
		mvEnt->Physics.PosY -= dy;
		MainMap->InsertEntityPended(mvEnt,
				mvEnt->Properties.Layer + dz,
				mvEnt->Physics.PosX + dx,
				mvEnt->Physics.PosY + dy);
	}
	return true;
}

bool	Trigger::tpcWorldEdit_Copy_ApplyCopy(
		void*	vMainMap,
		void*	vThisEntity)
{
	GameMap*	MainMap = (GameMap*)vMainMap;
	Entity*		ThisEntity = (Entity*)vThisEntity;
	if (!MainMap->IsHost) return false; // This will lead to security problems
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
//	To define the copy operation proper or not.
//	if (!(dx <= x2 - x1 && dx >= x1 - x2)) return false;
//	if (!(dy <= y2 - y1 && dy >= y1 - y2)) return false;
//	if (!(dz <= z2 - z1 && dz >= z1 - z2)) return false;
//	Now to proceed in the copy operation.
	std::vector<Entity*>	cpVec;
	cpVec.clear();
	MainMap->RemoveEntityPended(ThisEntity);
	NetmgrRemoveEntity(ThisEntity);
//	Finding the objects satisfies the prerequisites to move
	int	cx1 = GetEntityChunkNum(x1 - 1);
	int	cx2 = GetEntityChunkNum(x2 + 1);
	for (int x = cx1; x <= cx2; x++) {
		Chunk*	WrkChnk = MainMap->ChunkList[x];
		if (!WrkChnk) continue;
		for (int z = z1; z <= z2; z++) {
			if (WrkChnk->EntityLayerList.find(z) == WrkChnk->EntityLayerList.end()) continue;
			for (auto itert : WrkChnk->EntityLayerList[z]) {
				Entity*	EntFind = itert.second;
				if (!EntFind->DataIntact()) continue;
//				A worldedit block should **NEVER** appear here as another fill object...
				if (tpcWorldEdit_IsWorldEditBlock(EntFind)) {
					MainMap->RemoveEntityPended(EntFind);
					NetmgrRemoveEntity(EntFind);
					continue;
				}
				if (EntFind->Physics.PosX < x1) continue;
				if (EntFind->Physics.PosX > x2) continue;
				if (EntFind->Physics.PosY < y1) continue;
				if (EntFind->Physics.PosY > y2) continue;
				if (EntFind->Properties.Type->Properties.Type != "Block") continue;
				cpVec.push_back(EntFind);
			}
		}
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
		MainMap->InsertEntityPended(EntMake);
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
	if (ConsequentialAction == "WorldEdit.Fill.Pick")
		return this->tpcWorldEdit_Fill_Pick(vMainMap, vThisEntity);
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
