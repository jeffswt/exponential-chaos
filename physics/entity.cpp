
//	"entity.cpp" -*- C++ -*-
//	Created on: 8 March 2016
//	Definitions of entity management.
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

#include "physics/entity.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "debug/class_table.h"
#include "native/jsonimport.h"
#include "native/time.h"

extern	std::map<std::string, EntityType*>	EntityTypes;

bool	Entity::PhysicsEnabled(void)
	{ return Properties.Type->Physics.PhysicsEnabled ^ Physics.PhysicsChanged; }

bool	Entity::CollisionEnabled(
		Entity*	Object)
{
	EntityType*	mnType = NULL;
	EntityType*	tgType = NULL;
	mnType = Properties.Type;
	if (Object != NULL)
		tgType = Object->Properties.Type;
#define	my_check_availability(_a,_b,_c) (_a->Physics._c.find(_b) != _a->Physics._c.end())
	if (!mnType->Physics.CollisionEnabled ^ Physics.CollisionChanged)
		return false;
	if (Object != NULL && tgType != NULL)
		if (my_check_availability(mnType, tgType, CollisionIgnore) ||
				my_check_availability(tgType, mnType, CollisionIgnore))
			return false ^ Physics.CollisionChanged;
	return true;
}

bool	Entity::CollisionEnabled(void)
	{ return CollisionEnabled(NULL); }

bool	Entity::RenderEnabled(void)
	{ return Properties.Type->Graphics[Properties.TypeState].RenderEnabled
			&& (!Physics.RenderDisabled); }

bool	Entity::DataIntact(void)
{
	if (!this) return false; // Which normally should not happen
	EntityType*	EntTyp = NULL;
	std::string	TypeName = "";
	EntTyp = Properties.Type;
//	Check entity type data intactness
	if (!EntTyp->DataIntact())
		return false;
	if (Properties.TypeState < 0)
		return false;
	if (Properties.TypeState >= (int)EntTyp->Graphics.size())
		return false;
//	Checking type-specific properties' intactness
	TypeName = EntTyp->Properties.Name;
	if (TypeName == "Player") {
		PlayerEntity*	EntExt = (PlayerEntity*)Physics.ExtendedTags;
		if (!EntExt) return false;
	}
	return true;
}

bool	Entity::InheritFrom(
		EntityType*	EntTyp)
{
	if (!EntTyp->DataIntact())
		return false;
	Properties.Type = EntTyp;
	Properties.TypeName = EntTyp->Properties.Name;
	Properties.TypeState = 0;
	Properties.GenTime = GetProcessTimeUnsynced();
	Properties.Name = "";
	Properties.Owner = "";
	if (EntTyp->Properties.Type == "Player") {
		PlayerEntity*	Confs = new PlayerEntity;
		Confs->LastJumpTime[0] = Confs->LastJumpTime[1] = 0.0;
		Confs->LastProjectileTime = 0.0;
		Confs->Inventory.clear();
		Confs->InventoryFocus = 0;
		Confs->Life = ((PlayerEntityType*)EntTyp->Properties.SpecificProperties)->MaxLife;
		Physics.ExtendedTags = Confs;
	}
	for (Trigger* trig : EntTyp->Physics.TriggerList) {
		Trigger*	ntrig = new Trigger;
		*ntrig = *trig;
		Physics.TriggerList.push_back(ntrig);
	}
	return true;
}

bool	Entity::InheritFrom(
		Entity*	OtherEnt)
{
	Properties.Type = OtherEnt->Properties.Type;
	Properties.TypeName = Properties.Type->Properties.Name;
	Properties.TypeState = OtherEnt->Properties.TypeState;
	Properties.Guid = 0;
	Properties.Layer = OtherEnt->Properties.Layer;
	Properties.GenTime = GetProcessTime();
	Properties.Name = OtherEnt->Properties.Name;
	Properties.Owner = OtherEnt->Properties.Owner;
	Physics.PhysicsChanged = OtherEnt->Physics.PhysicsChanged;
	Physics.CollisionChanged = OtherEnt->Physics.CollisionChanged;
	Physics.RenderDisabled = OtherEnt->Physics.RenderDisabled;
	Physics.PosX = OtherEnt->Physics.PosX;
	Physics.PosY = OtherEnt->Physics.PosY;
	Physics.VelX = OtherEnt->Physics.VelX;
	Physics.VelY = OtherEnt->Physics.VelY;
	if (Properties.Type->Properties.Type == "Player") {
		PlayerEntity*	Confs = new PlayerEntity;
		PlayerEntity*	OldConf = (PlayerEntity*)OtherEnt->Physics.ExtendedTags;
		*Confs = *OldConf;
		Physics.ExtendedTags = (void*)Confs;
	}
	for (Trigger* trig : OtherEnt->Physics.TriggerList) {
		Trigger*	ntrig = new Trigger;
		*ntrig = *trig;
		Physics.TriggerList.push_back(ntrig);
	}
	return true;
}

bool	Entity::ImportFromJson(
		rapidjson::Value&	Config,
		bool	ImportAdditionalData)
{
	if (!Config.IsObject())
		return false;
//	Importing type name and updating crucial data
	ImportJsonData(Properties.TypeName, Config["Properties"]["TypeName"]);
	if (EntityTypes.find(Properties.TypeName) != EntityTypes.end())
		Properties.Type = EntityTypes[Properties.TypeName];
	else
		return false;
	if (!InheritFrom(Properties.Type))
		return false;
//	Specify data from JSON values
	ImportJsonData(Properties.TypeState, Config["Properties"]["TypeState"]);
	ImportJsonData(Properties.Layer, Config["Properties"]["Layer"]);
	ImportJsonData(Properties.Name, Config["Properties"]["Name"]);
	ImportJsonData(Physics.PhysicsChanged, Config["Physics"]["PhysicsChanged"]);
	ImportJsonData(Physics.CollisionChanged, Config["Physics"]["CollisionChanged"]);
	ImportJsonData(Physics.RenderDisabled, Config["Physics"]["RenderDisabled"]);
	ImportJsonData(Physics.PosX, Config["Physics"]["PosX"]);
	ImportJsonData(Physics.PosY, Config["Physics"]["PosY"]);
	if (ImportAdditionalData) {
		ImportJsonData(Properties.Guid, Config["Properties"]["Guid"]);
		ImportJsonData(Properties.GenTime, Config["Properties"]["GenTime"]);
		ImportJsonData(Properties.Owner, Config["Properties"]["Owner"]);
		ImportJsonData(Physics.VelX, Config["Physics"]["VelX"]),
		ImportJsonData(Physics.VelY, Config["Physics"]["VelY"]);
	}
//	Importing overridden trigger data.
	Physics.TriggerList = Properties.Type->Physics.TriggerList;
	if (Config["Physics"]["TriggerList"].IsArray()) {
		Trigger*	Worker = new Trigger;
		for (unsigned int i = 0; i < Config["Physics"]["TriggerList"].Size(); i++) {
			if (!Config["Physics"]["TriggerList"][i].IsObject())
				continue;
			if (!Worker->ImportFromJson(Config["Physics"]["TriggerList"][i]))
				continue;
			Physics.TriggerList.push_back(Worker);
		}
	}
//	Cleanup something that hadn't been defined
	Physics.VelX = Physics.VelY = 0.0;
//	Building extended tags for specific items
	if (Properties.Type->Properties.Type == "Player") {
		PlayerEntity*	Confs = new PlayerEntity;
		Confs->LastJumpTime[0] = Confs->LastJumpTime[1] = 0.0;
		Confs->LastProjectileTime = 0.0;
		if (Config["Physics"]["ExtendedTags"]["Inventory"].IsArray()) {
			std::string TypeName;
			int			TypeSize;
			EntityType*	TypePntr;
			for (unsigned int i = 0; i < Config["Physics"]["ExtendedTags"]["Inventory"].Size(); i++) {
				if (!Config["Physics"]["ExtendedTags"]["Inventory"][i].IsObject())
					continue;
				ImportJsonData(TypeName, Config["Physics"]["ExtendedTags"]["Inventory"][i]["Type"]);
				ImportJsonData(TypeSize, Config["Physics"]["ExtendedTags"]["Inventory"][i]["Count"]);
				TypePntr = EntityTypes[TypeName];
				if (!TypePntr || TypeSize <= 0)
					continue;
				Confs->Inventory.push_back(std::make_pair(TypePntr, TypeSize));
			}
		}
		ImportJsonData(Confs->InventoryFocus, Config["Physics"]["ExtendedTags"]["InventoryFocus"]);
		ImportJsonData(Confs->Life, Config["Physics"]["ExtendedTags"]["Life"]);
		ImportJsonData(Confs->IsCreative, Config["Physics"]["ExtendedTags"]["IsCreative"]);
		Physics.ExtendedTags = (void*)Confs;
	}
	return true;
}

bool	Entity::ExportToJson(
		rapidjson::Value&	Config,
		rapidjson::Document::
			AllocatorType&	CAlloc,
		bool				ExportAdditionalData)
{
	Config.SetObject();
	Config.AddMember("Properties", rapidjson::Value(rapidjson::kObjectType), CAlloc);
	Config.AddMember("Physics", rapidjson::Value(rapidjson::kObjectType), CAlloc);
	Config["Properties"].AddMember("TypeName", Properties.TypeName, CAlloc);
	Config["Properties"].AddMember("TypeState", Properties.TypeState, CAlloc);
	Config["Properties"].AddMember("Layer", Properties.Layer, CAlloc);
	Config["Properties"].AddMember("Name", Properties.Name, CAlloc);
	Config["Physics"].AddMember("PhysicsChanged", Physics.PhysicsChanged, CAlloc);
	Config["Physics"].AddMember("CollisionChanged", Physics.CollisionChanged, CAlloc);
	Config["Physics"].AddMember("RenderDisabled", Physics.RenderDisabled, CAlloc);
	Config["Physics"].AddMember("PosX", Physics.PosX, CAlloc);
	Config["Physics"].AddMember("PosY", Physics.PosY, CAlloc);
	if (ExportAdditionalData) {
		Config["Properties"].AddMember("Guid", Properties.Guid, CAlloc);
		Config["Properties"].AddMember("GenTime", Properties.GenTime, CAlloc);
		Config["Properties"].AddMember("Owner", Properties.Owner, CAlloc);
		Config["Physics"].AddMember("VelX", Physics.VelX, CAlloc);
		Config["Physics"].AddMember("VelY", Physics.VelY, CAlloc);
	}
//	Importing overridden trigger data.
	if (Physics.TriggerList.size() > 0) {
		Config["Physics"].AddMember("TriggerList", rapidjson::Value(rapidjson::kArrayType), CAlloc);
		for (Trigger*	trig : Physics.TriggerList) {
			rapidjson::Value	impValue;
			trig->ExportToJson(impValue, CAlloc);
			Config["Physics"]["ExtendedTags"].PushBack(impValue.Move(), CAlloc);
		}
	}
//	Building extended tags for specific items
	if (Properties.Type->Properties.Type == "Player") {
		PlayerEntity*		Confs = (PlayerEntity*)Physics.ExtendedTags;
		if (!Confs)
			throw NullPointerException();
		Config["Physics"].AddMember("ExtendedTags", rapidjson::Value(rapidjson::kObjectType), CAlloc);
		if (Confs->Inventory.size() > 0) {
			Config["Physics"]["ExtendedTags"].AddMember("Inventory", rapidjson::Value(rapidjson::kArrayType), CAlloc);
			for (unsigned int i = 0; i < Confs->Inventory.size(); i++) {
				if (Confs->Inventory[i].first == NULL)
					continue; // I decided not to throw NullPointerException().
				rapidjson::Value	invItem;
				invItem.SetObject();
				invItem.AddMember("Type", Confs->Inventory[i].first->Properties.Name, CAlloc);
				invItem.AddMember("Count", Confs->Inventory[i].second, CAlloc);
				Config["Physics"]["ExtendedTags"]["Inventory"].PushBack(invItem.Move(), CAlloc);
			}
		}
		Config["Physics"]["ExtendedTags"].AddMember("InventoryFocus", Confs->InventoryFocus, CAlloc);
		Config["Physics"]["ExtendedTags"].AddMember("Life", Confs->Life, CAlloc);
		Config["Physics"]["ExtendedTags"].AddMember("IsCreative", Confs->IsCreative, CAlloc);
	}
	return true;
}

Entity::Entity(void)
{
	Properties.Type = NULL;
	Properties.TypeName = "";
	Properties.TypeState = 0;
	Properties.Guid = 0;
	Properties.Layer = 0;
	Properties.GenTime = 0;
	Properties.Name = "";
	Properties.Owner = "";
	Physics.PhysicsChanged = false;
	Physics.CollisionChanged = false;
	Physics.RenderDisabled = false;
	Physics.PosX = 0.0;
	Physics.PosY = 0.0;
	Physics.VelX = 0.0;
	Physics.VelX = 0.0;
	Physics.TriggerList.clear();
	Physics.ExtendedTags = NULL;
	return ;
}

Entity::Entity(
		Entity*	OtherEnt) :
		Entity() {
	InheritFrom(OtherEnt);
	return ;
}

Entity::Entity(
		EntityType*	EntTyp) :
		Entity() {
	InheritFrom(EntTyp);
	return ;
}

Entity::~Entity(
		void)
{
//	Crucial datum that may cause trouble
	if (Properties.Type->DataIntact() &&
			Properties.Type->Properties.Type == "Player") {
		PlayerEntity*	extTag;
		extTag = (PlayerEntity*)Physics.ExtendedTags;
		if (extTag)
			delete extTag;
	}
	Physics.TriggerList.clear();
	return ;
}

PlayerEntity::PlayerEntity(
		void)
{
	LastJumpTime[0] = LastJumpTime[1] = 0.0;
	LastJumpHeight = 0.0;
	LastProjectileTime = 0.0;
	Life = 20.0;
	InventoryFocus = 1;
	IsCreative = false;
	Inventory.clear();
	return ;
}
