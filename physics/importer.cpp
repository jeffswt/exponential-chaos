
//	"importer.cpp" -*- C++ -*-
//	Created on: 5 March 2016
//	Definitions of asset loading procedures.
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

#include "physics/importer.h"

#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <GL/freeglut.h>
#include <map>
#include <rapidjson/document.h>
#include <thread>
#include <vector>
#include <windows.h>

#include "native/directory.h"
#include "native/guid.h"
#include "native/jsonimport.h"
#include "physics/entity.h"
#include "physics/entitytype.h"

extern	std::string	CurrentDirectory;
		std::string	CurrentTextureDirectory;

namespace	graphicsLoadGuiInternals{
	extern	double	LoadPercentage;
}

std::map<std::string, EntityType*>	EntityTypes;
std::thread	my_iget_thr;

extern	HDC		hDeviceContext;
extern	HGLRC	hRenderContextShared;

bool	ImportGameEntityTypes(
		void)
{
	using namespace graphicsLoadGuiInternals;
//	A trick used to share rendering contexts
	wglMakeCurrent(hDeviceContext, hRenderContextShared);
//	Wait for loading
	while (LoadPercentage > 1.5)
		SleepForTime(0.015);
//	Done waiting, which is used for better generate the visual effects
	std::vector<std::string>	ResourceList;
	std::vector<std::string>	EntityJson, EntityJsonTmp;
	std::ifstream				Stream;
	std::map<std::string, std::string>	ResourceNameIdx, PathPrefixIdx;
	LoadPercentage = 0.0;
	EntityTypes.clear();
	IterateDirectory(1, true, CurrentDirectory + "resources/", ResourceList);
//	Reading resource list, and dumping an entity list
	for (auto PathPrefix : ResourceList) {
		std::string		ResourceName;
		std::string		JStream, JRead;
		std::ifstream	JFile;
		rapidjson::Document	JConfig;
		JFile.open((PathPrefix + "resource.json").c_str());
		if (!JFile) continue;
		while (getline(JFile, JRead)) JStream += JRead;
		JConfig.Parse(JStream.c_str());
		if (!JConfig.IsObject()) continue;
		ImportJsonData(ResourceName, JConfig["Name"]);
		eclogPost(3, "Discovered resource pack " + ResourceName);
//		Finished reading resource configuration.
		EntityJsonTmp.clear();
		IterateDirectory(12, false, PathPrefix + "entities/", EntityJsonTmp);
		for (auto itert : EntityJsonTmp) {
			ResourceNameIdx[itert] = ResourceName;
			PathPrefixIdx[itert] = PathPrefix;
			EntityJson.push_back(itert);
		}
	}
//	Importing from dumped entity list
	double	loadCounter = 0.0;
	for (auto i : EntityJson) {
//		To inform the user and make better waiting...
		std::string	origName = PathPrefixIdx[i] + "entities/";
		std::string	configName;
		for (unsigned int j = origName.length(); j < i.length(); j++)
			configName += i[j];
		CurrentTextureDirectory = PathPrefixIdx[i] + "textures/";
//		This is the main procedure...
		Stream.open(i.c_str());
		if (!Stream)
			continue;
		EntityType *newEntity = new EntityType;
		if (!newEntity->ImportFromJson(Stream)) {
			std::string	FailureTitle = "Exponential Chaos: Fatal Error";
			std::string	FailureMessage = "JSON Grammar error while importing entity: \"" +
					i + "\".";
			eclogPost(0, FailureMessage);
			MessageBox(0, FailureMessage.c_str(), FailureTitle.c_str(), MB_ICONERROR | MB_OK);
			delete newEntity;
			continue;
		}
//		This is by default disabled, but enabled if designed for compatibility.
//		EntityTypes[newEntity->Properties.Name] = newEntity;
		newEntity->Properties.Name = ResourceNameIdx[i] + "::" + newEntity->Properties.Name;
		EntityTypes[newEntity->Properties.Name] = newEntity;
		Stream.close();
		LoadPercentage = (loadCounter++) / EntityJson.size();
		eclogPost(3, "Loaded entity " + newEntity->Properties.Name);
	}
//	Post-processing entity additional datum...
	for (auto i : EntityTypes)
		i.second->PostProcessDatum();
	LoadPercentage = 1.0;
	eclogPost(1, "Imported game entities");
	return true;
}

bool	beginImportGameEntityTypes(
		void)
{
	using namespace graphicsLoadGuiInternals;
	endImportGameEntityTypes();
	LoadPercentage = 2.0;
	my_iget_thr = std::thread(ImportGameEntityTypes);
	return true;
}

bool	endImportGameEntityTypes(
		void)
{
	using namespace graphicsLoadGuiInternals;
	if (my_iget_thr.joinable())
		my_iget_thr.join();
	return true;
}

bool	ImportGameMapHeaders(
		std::vector<GameMap*>&	MapList)
{
	std::vector<std::string>	MapJson;
	std::ifstream				Stream;
	IterateDirectory(1, true, CurrentDirectory + "maps/", MapJson);
	/*
	 * At this place, the importer chose to import data from the directories.
	 * The game maps are read however chunks are ignored. This enables the game
	 * menu to look for basic data contained in the game maps.
	 */
	for (auto i : MapJson) {
		Stream.open((i + "level.json").c_str());
		if (!Stream)
			continue;
		GameMap *newMap = new GameMap;
		newMap->ImportFromJson(false, i, Stream);
		MapList.push_back(newMap);
		Stream.close();
	}
	return true;
}
