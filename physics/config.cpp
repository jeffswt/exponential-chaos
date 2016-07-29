
//	"config.cpp" -*- C++ -*-
//	Created on: 17 April 2016
//	definitions for game configuration management.
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

#include "physics/config.h"

#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include "native/jsonimport.h"

extern	std::string	CurrentDirectory;

bool	ConfigType::ImportFromJson(
		void)
{
	rapidjson::Document	Config;
//	Retrieving data from file stream and parse
	std::string 	Content = "", ReadLine;
	std::ifstream	Stream;
	Stream.open((CurrentDirectory + "config/main.json").c_str());
	if (!Stream)
		return false;
	while (getline(Stream, ReadLine))
		Content += ReadLine;
	Stream.close();
	Config.Parse(Content.c_str());
	if (!Config.IsObject())
		return false;
//	Uploading parse results to data structure
	ImportJsonData(CameraDistTolerance, Config["CameraDistTolerance"]);
	ImportJsonData(CameraSpeed, Config["CameraSpeed"]);
	ImportJsonData(ViewDistance, Config["ViewDistance"]);
	ImportJsonData(PixelRatio, Config["PixelRatio"]);
	ImportJsonData(WindowWidth, Config["WindowWidth"]);
	ImportJsonData(WindowHeight, Config["WindowHeight"]);
	ImportJsonData(WindowFullScreen, Config["WindowFullScreen"]);
	ImportJsonData(OpenToLan, Config["OpenToLan"]);
	ImportJsonData(PlayerName, Config["PlayerName"]);
	ImportJsonData(ServerList, Config["ServerList"]);
	eclogPost(1, "Imported game configuration");
	return true;
}

bool	ConfigType::ExportToJson(
		void)
{
	rapidjson::StringBuffer			JBuffer;
	rapidjson::PrettyWriter<
		rapidjson::StringBuffer>	JWriter(JBuffer);
	rapidjson::Document				Config;
	rapidjson::Document::
		AllocatorType&				JAlloc = Config.GetAllocator();
	std::ofstream					Stream;
//	Adding JSON members
	Config.SetObject();
	Config.AddMember("CameraDistTolerance", CameraDistTolerance, JAlloc);
	Config.AddMember("CameraSpeed", CameraSpeed, JAlloc);
	Config.AddMember("ViewDistance", ViewDistance, JAlloc);
	Config.AddMember("PixelRatio", PixelRatio, JAlloc);
	Config.AddMember("WindowWidth", WindowWidth, JAlloc);
	Config.AddMember("WindowHeight", WindowHeight, JAlloc);
	Config.AddMember("WindowFullScreen", WindowFullScreen, JAlloc);
	Config.AddMember("OpenToLan", OpenToLan, JAlloc);
	Config.AddMember("PlayerName", PlayerName, JAlloc);
	Config.AddMember("ServerList", rapidjson::Value(rapidjson::kArrayType), JAlloc);
	for (std::string svrName : ServerList)
		Config["ServerList"].PushBack(svrName, JAlloc);
//	Output the configuration
	Config.Accept(JWriter);
	Stream.open((CurrentDirectory + "config/main.json").c_str());
	if (!Stream)
		return false;
	Stream << JBuffer.GetString();
	Stream.close();
	eclogPost(1, "Exported game configuration");
	return true;
}

ConfigType::ConfigType(
		void)
{
	CameraDistTolerance = 240;
	CameraSpeed = 1.0;
	ViewDistance = 3;
	PixelRatio = 80.0;
	WindowWidth = 640, WindowHeight = 480;
	WindowFullScreen = false;
	OpenToLan = true;
	PlayerName = "Steve";
	return ;
}
