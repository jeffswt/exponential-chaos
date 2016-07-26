
//	"entitytype.cpp" -*- C++ -*-
//	Created on: 3 March 2016
//	Definitions of entity type management.
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

#include <physics/entitytype.h>

#include <cstdlib>
#include <cstring>
#include <fstream>

#include <queue>
#include <map>
#include <rapidjson/document.h>

#include "debug/class_table.h"
#include "native/jsonimport.h"
#include "native/texture_png.h"

extern	std::string	CurrentTextureDirectory;
extern	std::map<std::string, EntityType*>	EntityTypes;

EntityType::EntityType(void)
{
//	Basic properties
	Properties.Name = "Misc.UnknownBlock";
	Properties.Type = "Miscellaneous";
	Properties.Description = "Unknown block";
	Properties.SpecificProperties = NULL;
	Properties.ShowInCreative = false;
//	Physics properties
	Physics.PhysicsEnabled = false;
	Physics.CollisionEnabled = false;
	Physics.LengthX = 1.0;
	Physics.LengthY = 1.0;
	Physics.Mass = 1.0;
	memset(Physics.FrictionFactor, 0, sizeof(double) * 4);
	memset(Physics.ElasticFactor, 0, sizeof(double) * 4);
	Physics.BlastResistance = 10000.0; // Make it nearly invincible
	Physics.CollisionIgnoreName.clear();
	Physics.CollisionIgnore.clear();
	Physics.TriggerList.clear();
	Graphics.clear();
	return ;
}

EntityType::typeGraphics::typeGraphics(void)
{
	RenderEnabled = true;
	TexRotation = 0; // 0 = None, 1 = 90 CW, 2 = 180 CW, 3 = 90 CCW / 270 CW
	AnimationInterval = 1.0;
	Luminosity = 0.0;
	LengthX = 1.0;
	LengthY = 1.0;
	TextureList.clear();
	TextureOnHand = 0;
	return ;
}

bool	EntityType::DataIntact(void)
{
	if (!this) return false;
	std::string	TypeName = "";
	TypeName = Properties.Type;
	if (TypeName == "Player") {
		PlayerEntityType*	TypExt = (PlayerEntityType*)
				Properties.SpecificProperties;
		if (!TypExt) return false;
	} else if (TypeName == "Projectile") {
		ProjectileEntityType*	TypExt = (ProjectileEntityType*)
				Properties.SpecificProperties;
		if (!TypExt) return false;
	} else if (TypeName == "Particle") {
		ParticleEntityType*	TypExt = (ParticleEntityType*)
				Properties.SpecificProperties;
		if (!TypExt) return false;
	}
	return true;
}

bool	EntityType::ImportFromJson(
		std::istream&	FileStream)
{
//	If it isn't a valid file stream, interrupt operation.
	rapidjson::Document	ConfigData;
	rapidjson::Value	WorkVal;
	std::string			StringStream, StringRead;
	while (getline(FileStream, StringRead))
		StringStream += StringRead;
	ConfigData.Parse(StringStream.c_str());
	if (!ConfigData.IsObject())
		return false;
//	Importing general properties
	ImportJsonData(Properties.Name, ConfigData["Properties"]["Name"]);
	ImportJsonData(Properties.Type, ConfigData["Properties"]["Type"]);
	Properties.Description = Properties.Name;
	ImportJsonData(Properties.Description, ConfigData["Properties"]["Description"]);
	ImportJsonData(Properties.ShowInCreative, ConfigData["Properties"]["ShowInCreative"]);
	ImportJsonData(Physics.PhysicsEnabled, ConfigData["Physics"]["PhysicsEnabled"]);
	ImportJsonData(Physics.CollisionEnabled, ConfigData["Physics"]["CollisionEnabled"]);
	ImportJsonData(Physics.LengthX, ConfigData["Physics"]["LengthX"]);
	ImportJsonData(Physics.LengthY, ConfigData["Physics"]["LengthY"]);
	ImportJsonData(Physics.Mass, ConfigData["Physics"]["Mass"]);
//	Friction factor, in all 4 faces
	ImportJsonData(Physics.FrictionFactor[2], ConfigData["Physics"]["FrictionFactorTop"]);
	Physics.FrictionFactor[3] = Physics.FrictionFactor[1] = Physics.FrictionFactor[0] = Physics.FrictionFactor[2];
	ImportJsonData(Physics.FrictionFactor[0], ConfigData["Physics"]["FrictionFactorLeft"]);
	ImportJsonData(Physics.FrictionFactor[1], ConfigData["Physics"]["FrictionFactorRight"]);
	ImportJsonData(Physics.FrictionFactor[3], ConfigData["Physics"]["FrictionFactorBottom"]);
//	Elasticity factor, in all 4 faces.
	ImportJsonData(Physics.ElasticFactor[2], ConfigData["Physics"]["ElasticFactorTop"]);
	Physics.ElasticFactor[3] = Physics.ElasticFactor[1] = Physics.ElasticFactor[0] = Physics.ElasticFactor[2];
	ImportJsonData(Physics.ElasticFactor[0], ConfigData["Physics"]["ElasticFactorLeft"]);
	ImportJsonData(Physics.ElasticFactor[1], ConfigData["Physics"]["ElasticFactorRight"]);
	ImportJsonData(Physics.ElasticFactor[3], ConfigData["Physics"]["ElasticFactorBottom"]);
//	Miscellaneous properties.
	ImportJsonData(Physics.BlastResistance, ConfigData["Physics"]["BlastResistance"]);
	ImportJsonData(Physics.CollisionIgnoreName, ConfigData["Physics"]["CollisionIgnore"]);
//	Importing general yet self-defined importing procedure properties
//	Entity class specific properties
	if (Properties.Type == "Player") {
		PlayerEntityType*	SubData = new PlayerEntityType;
		ImportJsonData(SubData->MoveSpeed, ConfigData["Properties"]["SpecificProperties"]["MoveSpeed"]);
		ImportJsonData(SubData->JumpSpeed, ConfigData["Properties"]["SpecificProperties"]["JumpSpeed"]);
		ImportJsonData(SubData->MaxLife, ConfigData["Properties"]["SpecificProperties"]["MaxLife"]);
		Properties.SpecificProperties = (void*)SubData;
	} else if (Properties.Type == "Projectile") {
		ProjectileEntityType*	SubData = new ProjectileEntityType;
		ImportJsonData(SubData->LaunchSpeed, ConfigData["Properties"]["SpecificProperties"]["LaunchSpeed"]);
		ImportJsonData(SubData->LaunchInterval, ConfigData["Properties"]["SpecificProperties"]["LaunchInterval"]);
		ImportJsonData(SubData->DeployDelay, ConfigData["Properties"]["SpecificProperties"]["DeployDelay"]);
		ImportJsonData(SubData->DeployRadius, ConfigData["Properties"]["SpecificProperties"]["DeployRadius"]);
		ImportJsonData(SubData->DeployPowerDamage, ConfigData["Properties"]["SpecificProperties"]["DeployPowerDamage"]);
		ImportJsonData(SubData->DeployPowerMotion, ConfigData["Properties"]["SpecificProperties"]["DeployPowerMotion"]);
		ImportJsonData(SubData->DeployPowerBlast, ConfigData["Properties"]["SpecificProperties"]["DeployPowerBlast"]);
		Properties.SpecificProperties = (void*)SubData;
	} else if (Properties.Type == "Particle") {
		ParticleEntityType*	SubData = new ParticleEntityType;
		ImportJsonData(SubData->LifeTime, ConfigData["Properties"]["SpecificProperties"]["LifeTime"]);
		Properties.SpecificProperties = (void*)SubData;
	}
//	Import textures list of PNGs, defined by whether is array or not.
	if (!ConfigData["Graphics"].IsArray()) {
		typeGraphics	nGraphics;
		ImportJsonData(nGraphics.RenderEnabled, ConfigData["Graphics"]["RenderEnabled"]);
		ImportJsonData(nGraphics.TexRotation, ConfigData["Graphics"]["TexRotation"]);
		ImportJsonData(nGraphics.AnimationInterval, ConfigData["Graphics"]["AnimationInterval"]);
		ImportJsonData(nGraphics.Luminosity, ConfigData["Graphics"]["Luminosity"]);
		ImportJsonData(nGraphics.LengthX, ConfigData["Graphics"]["LengthX"]);
		ImportJsonData(nGraphics.LengthY, ConfigData["Graphics"]["LengthY"]);
		if (ConfigData["Graphics"]["TextureList"].IsArray()) {
			std::vector<std::string> TexturePath;
			ImportJsonData(TexturePath, ConfigData["Graphics"]["TextureList"]);
			for (auto i = TexturePath.begin(); i != TexturePath.end(); i++) {
				GLuint		Texture;
				std::string	Path = CurrentTextureDirectory + (*i);
				Texture = LoadPNGTexture(Path.c_str(), NULL, NULL);
				nGraphics.TextureList.push_back(Texture);
			}
		}
		if (ConfigData["Graphics"]["TextureOnHand"].IsString()) {
			std::string	OnHandTex;
			std::string	Path;
			ImportJsonData(OnHandTex, ConfigData["Graphics"]["TextureOnHand"]);
			Path = CurrentTextureDirectory + OnHandTex;
			nGraphics.TextureOnHand = LoadPNGTexture(Path.c_str(), NULL, NULL);
		} else if (nGraphics.TextureList.size() > 0){
			nGraphics.TextureOnHand = nGraphics.TextureList[0];
		}
		Graphics.push_back(nGraphics);
	} else {
		for (int Indexer = 0; Indexer < (int)ConfigData["Graphics"].Size(); Indexer++) {
			typeGraphics	nGraphics;
			ImportJsonData(nGraphics.RenderEnabled, ConfigData["Graphics"][Indexer]["RenderEnabled"]);
			ImportJsonData(nGraphics.TexRotation, ConfigData["Graphics"][Indexer]["TexRotation"]);
			ImportJsonData(nGraphics.AnimationInterval, ConfigData["Graphics"][Indexer]["AnimationInterval"]);
			ImportJsonData(nGraphics.Luminosity, ConfigData["Graphics"][Indexer]["Luminosity"]);
			ImportJsonData(nGraphics.LengthX, ConfigData["Graphics"][Indexer]["LengthX"]);
			ImportJsonData(nGraphics.LengthY, ConfigData["Graphics"][Indexer]["LengthY"]);
			if (ConfigData["Graphics"][Indexer]["TextureList"].IsArray()) {
				std::vector<std::string> TexturePath;
				ImportJsonData(TexturePath, ConfigData["Graphics"][Indexer]["TextureList"]);
				for (auto i = TexturePath.begin(); i != TexturePath.end(); i++) {
					GLuint		Texture;
					std::string	Path = CurrentTextureDirectory + (*i);
					Texture = LoadPNGTexture(Path.c_str(), NULL, NULL);
					nGraphics.TextureList.push_back(Texture);
				}
			}
			if (ConfigData["Graphics"][Indexer]["TextureOnHand"].IsString()) {
				std::string	OnHandTex;
				std::string	Path;
				ImportJsonData(OnHandTex, ConfigData["Graphics"][Indexer]["TextureOnHand"]);
				Path = CurrentTextureDirectory + OnHandTex;
				nGraphics.TextureOnHand = LoadPNGTexture(Path.c_str(), NULL, NULL);
			} else if (nGraphics.TextureList.size() > 0){
				nGraphics.TextureOnHand = nGraphics.TextureList[0];
			}
			Graphics.push_back(nGraphics);
		}
	}
//	Import trigger operators from JSON
	if (ConfigData["Physics"]["TriggerList"].IsArray()) {
		for (unsigned int i = 0; i < ConfigData["Physics"]["TriggerList"].Size(); i++) {
			Trigger*	Worker = new Trigger;
			if (!ConfigData["Physics"]["TriggerList"][i].IsObject())
				continue;
			if (!Worker->ImportFromJson(ConfigData["Physics"]["TriggerList"][i]))
				continue;
			Physics.TriggerList.push_back(Worker);
		}
	}
	return true;
}

bool	EntityType::PostProcessDatum(
		void)
{
	for (auto& i : Physics.CollisionIgnoreName)
		if (EntityTypes[i] != NULL)
			Physics.CollisionIgnore.insert(EntityTypes[i]);
	return true;
}

PlayerEntityType::PlayerEntityType(void)
{
	MoveSpeed = 1.0;
	JumpSpeed = 1.0;
	MaxLife = 1.0;
	return ;
}

ProjectileEntityType::ProjectileEntityType(void)
{
	LaunchSpeed = 1.0;
	LaunchInterval = 1.0;
	DeployDelay = 1.0;
	DeployRadius = 1.0;
	DeployPowerDamage = 0.0;
	DeployPowerMotion = 0.0;
	DeployPowerBlast = 0.0;
	return ;
}

ParticleEntityType::ParticleEntityType(void)
{
	LifeTime = 0.0;
	return ;
}
