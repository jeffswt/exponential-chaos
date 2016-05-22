
//	"gamemap.cpp" -*- C++ -*-
//	Created on: 7 March 2016
//	Definitions of game map management.
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

#include "physics/gamemap.h"

#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <sstream>

#include "debug/exception.h"
#include "native/directory.h"
#include "native/guid.h"
#include "native/jsonimport.h"
#include "native/time.h"
#include "physics/config.h"
#include "physics/entitytype.h"

extern	ConfigType	GameConfig;
extern	std::map<std::string, EntityType*>	EntityTypes;

bool	GameMap::QueryAdjacentEntities(
		Entity*				Source,
		double				Tolerance,
		std::set<Entity*>&	TargetIdx)
{
	return true;
}

bool	GameMap::InsertEntity(
		Entity*	NewEnt)
{
	if (!NewEnt->DataIntact())
		return false;
//	I don't know whether this would cause problems or not...
//	if (EntityList.size() > 262144)
//		return false;
//	Then it's not too large, we can deal with it.
	int	ChunkNum = GetEntityChunkNum(NewEnt->Physics.PosX);
	CreateChunk(ChunkNum);
	Chunk*	InsChnk = ChunkList[ChunkNum];
//	Generate individual GUID for this entity
	if (NewEnt->Properties.Guid == 0) {
		do {
			NewEnt->Properties.Guid = GenerateGuid();
		} while (EntityList[NewEnt->Properties.Guid] != NULL);
	}
	EntityList[NewEnt->Properties.Guid] = NewEnt;
//	Certain specific properties for this entity
	if (NewEnt->Properties.Type->Properties.Type == "Player") {
		PlayerEntityList[NewEnt->Properties.Guid] = NewEnt;
	} else if (NewEnt->Properties.Type->Properties.Type == "Particle") {
		NewEnt->Properties.Layer = 32767;
		ParticleList[NewEnt->Properties.Guid] = NewEnt;
	}
//	Then make the globally appliable insertion
	InsChnk->InsertEntity(NewEnt);
//	Seeking whether to invoke triggers
	for (Trigger* trig : NewEnt->Physics.TriggerList)
		if (trig->PreliminaryAction == "OnGeneration")
			trig->ProcessConsequence(std::vector<void*>({
				(void*)this, (void*)NewEnt}));
	return true;
}

bool	GameMap::RemoveEntity(
		Entity*		RmEnt)
{
	if (!RmEnt->DataIntact())
		return false;
	return this->RemoveEntity(RmEnt->Properties.Guid);
}

bool	GameMap::RemoveEntity(
		long long	RmGuid)
{
	Entity*		RmEnt = EntityList[RmGuid];
	if (RmEnt->DataIntact()) {
		int RmBnd = GetEntityChunkNum(RmEnt->Physics.PosX);
		if (ChunkList[RmBnd] != NULL)
			ChunkList[RmBnd]->RemoveEntity(RmEnt);
		EntityList.erase(RmGuid);
		PlayerEntityList.erase(RmGuid);
		ParticleList.erase(RmGuid);
		for (Trigger* trig : RmEnt->Physics.TriggerList)
			if (trig->PreliminaryAction == "OnDestruction")
				trig->ProcessConsequence(std::vector<void*>({
					(void*)this, (void*)RmEnt}));
	} else { // if (!RmEnt->DataIntact())
		for (auto citert : ChunkList)
			citert.second->RemoveEntity(RmGuid);
		EntityList.erase(RmGuid);
		PlayerEntityList.erase(RmGuid);
		ParticleList.erase(RmGuid);
	}
	return true;
}

Entity*	GameMap::CreatePlayer(
		std::string	newName)
{
	EntityType*	undefPlayer = NULL;
	Entity*		defaultPlayer = NULL;
	Entity*		newPlayer = NULL;
	for (auto itert : EntityList) {
		Entity*	worker = itert.second;
		if (!worker->DataIntact()) continue;
		if (worker->Properties.Name == "__ZwDefaultEntity7Player") {
			defaultPlayer = worker;
			newPlayer = new Entity;
			newPlayer->InheritFrom(defaultPlayer);
			break;
		}
	}
	if (defaultPlayer == NULL)
	for (auto itert : EntityTypes) {
		EntityType*	worker = itert.second;
		if (!worker->DataIntact()) continue;
		if (worker->Properties.Type == "Player") {
			undefPlayer = worker;
			newPlayer = new Entity;
			newPlayer->InheritFrom(undefPlayer);
			break;
		}
	}
	if (!newPlayer->DataIntact())
		throw NullPointerException();
	newPlayer->Properties.Name = newName;
	newPlayer->Physics.PosX += newPlayer->Properties.Type->Physics.LengthX;
	newPlayer->Physics.PhysicsChanged = false;
	newPlayer->Physics.CollisionChanged = false;
	newPlayer->Physics.RenderDisabled = false;
	InsertEntity(newPlayer);
	return newPlayer;
}

bool	GameMap::InsertEntityPended(
		Entity*	InsEnt,
		int		NewLayer,
		double	NewX,
		double	NewY)
{
	if (!InsEnt->DataIntact())
		return false;
	triple_pair<int, double, double>	InsPair;
	InsPair = make_triple_pair(NewLayer, NewX, NewY);
	PendInsertList[InsEnt] = InsPair;
	if (InsEnt->Properties.Guid == 0) {
		do {
			InsEnt->Properties.Guid = GenerateGuid();
		} while (EntityList[InsEnt->Properties.Guid] != NULL);
	}
	return true;
}

bool	GameMap::InsertEntityPended(
		Entity*	InsEnt,
		int		NewLayer)
{
	if (!InsEnt->DataIntact())
		return false;
	double	NewX = 0.0, NewY = 0.0;
	NewX = InsEnt->Physics.PosX;
	NewY = InsEnt->Physics.PosY;
	return InsertEntityPended(InsEnt, NewLayer, NewX, NewY);
}

bool	GameMap::InsertEntityPended(
		Entity*	InsEnt,
		double	NewX,
		double	NewY)
{
	if (!InsEnt->DataIntact())
		return false;
	int	NewLayer = InsEnt->Properties.Layer;
	return InsertEntityPended(InsEnt, NewLayer, NewX, NewY);
}

bool	GameMap::InsertEntityPended(
		Entity*	InsEnt)
{
	if (!InsEnt->DataIntact())
		return false;
	int		NewLayer = 0;
	double	NewX = 0.0, NewY = 0.0;
	NewLayer = InsEnt->Properties.Layer;
	NewX = InsEnt->Physics.PosX;
	NewY = InsEnt->Physics.PosY;
	return InsertEntityPended(InsEnt, NewLayer, NewX, NewY);
}

bool	GameMap::InsertEntityPendedForce(
		Entity*	InsEnt)
{
	if (!InsEnt->DataIntact())
		return false;
	PendInsertForceList.insert(InsEnt);
	if (InsEnt->Properties.Guid == 0) {
		do {
			InsEnt->Properties.Guid = GenerateGuid();
		} while (EntityList[InsEnt->Properties.Guid] != NULL);
	}
	return true;
}

bool	GameMap::RemoveEntityPended(
		Entity*	InsEnt)
{
	if (!InsEnt->DataIntact())
		return false;
	if (PendRemoveList.find(InsEnt) == PendRemoveList.end())
		PendRemoveList.insert(InsEnt);
	return true;
}

bool	GameMap::CommitPendedChanges(
		void)
{
//	Update insertions
	for (auto itert : PendInsertList) {
		Entity*	InsEnt = itert.first;
		if (!InsEnt->DataIntact()) continue;
		triple_pair<int, double, double>	thsPair = itert.second;
		if (EntityList.find(InsEnt->Properties.Guid) != EntityList.end()) {
//			This is considered a re-insertion
			int	OrigChnk = GetEntityChunkNum(InsEnt->Physics.PosX);
			int	NewChnk = GetEntityChunkNum(thsPair.second);
			if (ChunkList[OrigChnk])
				ChunkList[OrigChnk]->RemoveEntity(InsEnt);
			itert.first->Physics.PosX = thsPair.second;
			itert.first->Physics.PosY = thsPair.third;
			itert.first->Properties.Layer = thsPair.first;
			if (ChunkList[NewChnk])
				ChunkList[NewChnk]->InsertEntity(InsEnt);
		} else {
			itert.first->Physics.PosX = thsPair.second;
			itert.first->Physics.PosY = thsPair.third;
			InsertEntity(itert.first);
		}
	}
//	Update force new insertions
	for (auto itert : PendInsertForceList) {
		InsertEntity(itert);
	}
//	Update removals
	for (Entity* itert : PendRemoveList) {
		RemoveEntity(itert);
		delete itert;
	}
	PendInsertList.clear();
	PendInsertForceList.clear();
	PendRemoveList.clear();
	return true;
}

bool	GameMap::ImportHeaderFromJson(
		std::string&	Input)
{
	rapidjson::Document	Config;
	Config.Parse(Input.c_str());
	if (!Config.IsObject())
		return false;
	ImportJsonData(Name, Config["Name"]);
	ImportJsonData(Description, Config["Description"]);
	ImportJsonData(GravityConst, Config["GravityConst"]);
	ImportJsonData(RegenerationValue, Config["RegenerationValue"]);
	ImportJsonData(ModifyTime, Config["ModifyTime"]);
//	Import world boundaries
	ImportJsonData(WorldBoundary[0], Config["WorldBoundaryLeft"]);
	ImportJsonData(WorldBoundary[1], Config["WorldBoundaryRight"]);
	ImportJsonData(WorldBoundary[2], Config["WorldBoundaryTop"]);
	ImportJsonData(WorldBoundary[3], Config["WorldBoundaryBottom"]);
//	Import forbidden layers' list
	if (Config["ForbiddenLayers"].IsArray())
		for (unsigned int i = 0; i < Config["ForbiddenLayers"].Size(); i++) {
			int	proc; ImportJsonData(proc, Config["ForbiddenLayers"][i]);
			ForbiddenLayers.insert(proc);
		}
	return true;
}

bool	GameMap::ImportFromJson(
		bool			ImportChunks,
		std::string		WorkingDirectory,
		std::ifstream&	Stream)
{
	rapidjson::Document	Config;
//	Retrieving data from file stream and parse
	std::string Content = "", ReadLine;
	while (getline(Stream, ReadLine))
		Content += ReadLine;
	Config.Parse(Content.c_str());
	if (!Config.IsObject())
		return false;
	LevelPath = WorkingDirectory;
	this->LockMemory();
//	Uploading parse results to data structure
	this->ImportHeaderFromJson(Content);
//	Recursing for every chunk to its subsidiary
	if (ImportChunks && Config["Chunks"].IsArray())
		for (unsigned int i = 0; i < Config["Chunks"].Size(); i++) {
			if (!Config["Chunks"][i].IsString())
				continue;
			rapidjson::Document	ChunkConfig;
			std::ifstream		ChunkStream;
			std::string			ChunkPath = WorkingDirectory + Config["Chunks"][i].GetString();
			std::string			StringStream, StringRead;
			ChunkStream.open(ChunkPath.c_str());
			if (!ChunkStream)
				continue;
			while (getline(ChunkStream, StringRead))
				StringStream += StringRead;
			ChunkStream.close();
			ChunkConfig.Parse(StringStream.c_str());
			if (!ChunkConfig.IsObject())
				continue;
//			Directly importing chunk entities
			if (!ChunkConfig["EntityList"].IsArray())
				continue;
			for (unsigned int i = 0; i < ChunkConfig["EntityList"].Size(); i++) {
				Entity*				NewEnt = new Entity;
				if (NewEnt->ImportFromJson(ChunkConfig["EntityList"][i], false))
					InsertEntity(NewEnt);
			}
		}
//	Importing entities directly from level configuration file
	if (Config["EntityList"].IsArray())
		for (unsigned int i = 0; i < Config["EntityList"].Size(); i++) {
			Entity*	NewEnt = new Entity;
			if (NewEnt->ImportFromJson(Config["EntityList"][i], false))
				InsertEntity(NewEnt);
		}
//	Updating special entities
	for (auto itert : EntityList) {
		Entity*	ChkEnt = itert.second;
		if (ChkEnt && ChkEnt->Properties.Type->Properties.Type == "Player")
			PlayerEntityList[ChkEnt->Properties.Guid] = ChkEnt;
	}
//	Finalising results
	this->UnlockMemory();
	return true;
}
bool	GameMap::ExportToJson(
		bool			ExportChunks,
		std::string&	Output)
{
	rapidjson::Document		Config;
	rapidjson::Document::
		AllocatorType&		JAlloc = Config.GetAllocator();
	std::ofstream			Stream;
//	I hope people hold no intention to steal maps...
	if (!IsHost)
		return false;
	if (ExportChunks)
		this->LockMemory();
//	Adding JSON members
	Config.SetObject();
	Config.AddMember("Name", Name, JAlloc);
	Config.AddMember("Description", Description, JAlloc);
	Config.AddMember("GravityConst", GravityConst, JAlloc);
	Config.AddMember("RegenerationValue", RegenerationValue, JAlloc);
	ModifyTime = GetSystemTime();
	Config.AddMember("ModifyTime", ModifyTime, JAlloc);
//	Generating world boundary configuration
	Config.AddMember("WorldBoundaryLeft", WorldBoundary[0], JAlloc);
	Config.AddMember("WorldBoundaryRight", WorldBoundary[1], JAlloc);
	Config.AddMember("WorldBoundaryTop", WorldBoundary[2], JAlloc);
	Config.AddMember("WorldBoundaryBottom", WorldBoundary[3], JAlloc);
//	Generating forbidden layers list
	Config.AddMember("ForbiddenLayers", rapidjson::Value(rapidjson::kArrayType), JAlloc);
	for (int layer : ForbiddenLayers)
		Config["ForbiddenLayers"].PushBack(layer, JAlloc);
//	Generating chunk list
	Config.AddMember("Chunks", rapidjson::Value(rapidjson::kArrayType), JAlloc);
	if (ExportChunks)
		ResetEntityPositions();
	if (ExportChunks) {
		for (auto itert : ChunkList) {
			int					ChunkNum = itert.first;
			Chunk*				sChnk = itert.second;
			rapidjson::Value	sValue;
			std::stringstream	sStream;
			std::string			sOutput;
			char*				sWorker = new char[256];
			if (!sChnk)
				throw NullPointerException();
//			To generate the filename of the chunk
			sStream << "chunk_" << ChunkNum << ".json";
			sStream >> sOutput;
//			Inserting chunk index into level.json
			memset(sWorker, 0, sizeof(char) * 256);
			for (unsigned int i = 0; i < sOutput.length(); i++)
				sWorker[i] = sOutput[i];
			sValue.SetString(sWorker, JAlloc);
			Config["Chunks"].PushBack(sValue, JAlloc);
//			Creating new chunk file.
			rapidjson::StringBuffer			CJBuffer;
			rapidjson::PrettyWriter<
				rapidjson::StringBuffer>	CJWriter(CJBuffer);
			rapidjson::Document				CConfig;
			rapidjson::Document::
				AllocatorType&				CJAlloc = CConfig.GetAllocator();
			std::ofstream					CStream;
			CConfig.SetObject();
			CConfig.AddMember("EntityList", rapidjson::Value(rapidjson::kArrayType), CJAlloc);
			for (auto itert : sChnk->EntityList) {
				rapidjson::Value	nVal;
				Entity*				expEnt = itert.second;
				if (!expEnt->DataIntact()) continue;
				if (expEnt->Properties.Name == "__ZwProgramGeneratedEntity7") continue;
				if (expEnt->Properties.Type->Properties.Type == "Player") continue;
//				This is an entity we want. Output it now.
				expEnt->ExportToJson(nVal, CJAlloc, false);
				CConfig["EntityList"].PushBack(nVal.Move(), CJAlloc);
			}
//			Writing chunk configuration to file.
			CConfig.Accept(CJWriter);
			CStream.open((LevelPath + sOutput).c_str());
			if (!CStream)
				return false;
			CStream << CJBuffer.GetString();
			CStream.close();
		}
//		Output players, such it would simplify operations in editing player
//		data. Otherwise it would be extremely difficult!
		Config.AddMember("EntityList", rapidjson::Value(rapidjson::kArrayType), JAlloc);
		for (auto itert : PlayerEntityList) {
			rapidjson::Value	nVal;
			Entity*				expEnt = itert.second;
			if (!expEnt->DataIntact()) continue;
			expEnt->ExportToJson(nVal, JAlloc, false);
			Config["EntityList"].PushBack(nVal.Move(), JAlloc);
		}
	}
//	Output the configuration
	if (ExportChunks)
		this->UnlockMemory();
	if (ExportChunks) {
		rapidjson::StringBuffer			JBuffer;
		rapidjson::PrettyWriter<
			rapidjson::StringBuffer>	JWriter(JBuffer);
		Config.Accept(JWriter);
		Stream.open((LevelPath + "level.json").c_str());
		if (!Stream)
			return false;
		Stream << JBuffer.GetString();
		Stream.close();
	} else if (!ExportChunks) {
		rapidjson::StringBuffer			JBuffer;
		rapidjson::Writer<
			rapidjson::StringBuffer>	JWriter(JBuffer);
		Config.Accept(JWriter);
		Output = JBuffer.GetString();
	}
	return true;
}

bool	GameMap::ExportToJson(
		void)
{
	std::string	PseudoOutput = "";
	return this->ExportToJson(true, PseudoOutput);
}

bool	GameMap::ReimportFromJson(
		void)
{
	std::ifstream	Stream;
	std::string		Path = LevelPath + "level.json";
	Stream.open(Path.c_str());
	if (!Stream)
		return false;
	ImportFromJson(true, LevelPath, Stream);
	Stream.close();
	return true;
}

bool	GameMap::CreateChunk(
		int	ChunkNum)
{
	for (int i = - GameConfig.ViewDistance - 1; i <= GameConfig.ViewDistance + 1; i++) {
		int CurNum = ChunkNum + i;
		if (ChunkList[CurNum] != NULL)
			continue;
		Chunk*	InsChnk = new Chunk;
		if (!InsChnk)
			throw IllegalPointerException();
		InsChnk->lBound = CurNum * 16.0 - 8.0;
		InsChnk->rBound = CurNum * 16.0 + 8.0;
		ChunkList[CurNum] = InsChnk;
	}
	return true;
}

bool	GameMap::ResetEntityPositions(
		void)
{
//	Clearing original chunks in prevention of duplication
	for (auto itert = ChunkList.begin(); itert != ChunkList.end(); itert++) {
		itert->second->Clear();
		delete itert->second;
	}
	ChunkList.clear();
//	Re-insert entities into individual chunks without duplication.
	for (auto itert = EntityList.begin(); itert != EntityList.end(); itert++) {
		Entity*	EWorker = itert->second;
		int		ChnkNum;
		if (!EWorker) continue;
		ChnkNum = GetEntityChunkNum(EWorker->Physics.PosX);
//		If chunk does not exist, insert one / create one
		if (!ChunkList[ChnkNum])
			ChunkList[ChnkNum] = new Chunk;
		ChunkList[ChnkNum]->InsertEntity(EWorker);
	}
	return true;
}

bool	GameMap::Clear(
		void)
{
	PendInsertForceList.clear();
	PendInsertList.clear();
	PendRemoveList.clear();
	std::vector<Entity*>	ErsWrkr;
	for (auto itert : EntityList)
		ErsWrkr.push_back(itert.second);
	for (auto itert : ErsWrkr) {
//		Entity*	ErsEnt = EntityList[*itert];
		RemoveEntity(itert);
//		This will **** the program...
//		delete ErsEnt;
	}
//	Clearing chunks...
	for (auto itert : ChunkList) {
		itert.second->Clear();
		delete itert.second;
	}
	EntityList.clear();
	ChunkList.clear();
	PlayerEntityList.clear();
	ParticleList.clear();
	return true;
}

GameMap::GameMap(
		void)
{
	PendInsertForceList.clear();
	PendInsertList.clear();
	PendRemoveList.clear();
	IsHost = false;
	LevelPath = "";
	Name = "Untitled";
	Description = "";
	GravityConst = 9.8;
//	Set world boundaries to a reasonable value
	WorldBoundary[0] = -32767;
	WorldBoundary[1] = 32767;
	WorldBoundary[2] = 256;
	WorldBoundary[3] = -256;
//	Forbidden layers are not available
	ForbiddenLayers.clear();
//	All lists should be cleared
	EntityList.clear();
	ChunkList.clear();
	PlayerEntityList.clear();
	ParticleList.clear();
//	Generate spawn chunk in case of memory exceptions
	CreateChunk(0);
	return ;
}
