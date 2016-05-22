
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
//	Graphics properties
	Graphics.RenderEnabled = false;
	Graphics.AnimationInterval = 0.0;
	Graphics.Luminosity = 0.0;
	Graphics.LengthX = 1.0;
	Graphics.LengthY = 1.0;
	Graphics.TextureList.clear();
	Graphics.TextureOnHand = 0;
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
	ImportJsonData(Graphics.RenderEnabled, ConfigData["Graphics"]["RenderEnabled"]);
	ImportJsonData(Graphics.AnimationInterval, ConfigData["Graphics"]["AnimationInterval"]);
	ImportJsonData(Graphics.Luminosity, ConfigData["Graphics"]["Luminosity"]);
	ImportJsonData(Graphics.LengthX, ConfigData["Graphics"]["LengthX"]);
	ImportJsonData(Graphics.LengthY, ConfigData["Graphics"]["LengthY"]);
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
//	Import textures list of PNGs
	if (ConfigData["Graphics"]["TextureList"].IsArray()) {
		std::vector<std::string> TexturePath;
		ImportJsonData(TexturePath, ConfigData["Graphics"]["TextureList"]);
		for (auto i = TexturePath.begin(); i != TexturePath.end(); i++) {
			GLuint		Texture;
			std::string	Path = CurrentTextureDirectory + (*i);
			Texture = LoadPNGTexture(Path.c_str(), NULL, NULL);
			Graphics.TextureList.push_back(Texture);
		}
	}
	if (ConfigData["Graphics"]["TextureOnHand"].IsString()) {
		std::string	OnHandTex;
		std::string	Path;
		ImportJsonData(OnHandTex, ConfigData["Graphics"]["TextureOnHand"]);
		Path = CurrentTextureDirectory + OnHandTex;
		Graphics.TextureOnHand = LoadPNGTexture(Path.c_str(), NULL, NULL);
	} else if (Graphics.TextureList.size() > 0){
		Graphics.TextureOnHand = Graphics.TextureList[0];
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

std::ostream&	operator << (
		std::ostream&	Stream,
		EntityType*		OutTyp)
{
	class_table_head(Stream);
	class_table_body(Stream, "Debug object", "Entity type");
	void*	ActualAddress = (void*)OutTyp;
	if (!OutTyp->DataIntact()) {
		class_table_body(Stream, "Memory address (unreadable)", ActualAddress);
		goto ProcessEnd;
	}
	class_table_body(Stream, "Memory address", ActualAddress);
	class_table_body(Stream, "Properties.Name", OutTyp->Properties.Name);
	class_table_body(Stream, "Properties.Type", OutTyp->Properties.Type);
	class_table_body(Stream, "Properties.Description", OutTyp->Properties.Description);
	class_table_body(Stream, "Properties.ShowInCreative", OutTyp->Properties.ShowInCreative);
	class_table_body(Stream, "Physics.LengthX", OutTyp->Physics.LengthX);
	class_table_body(Stream, "Physics.LengthX", OutTyp->Physics.LengthX);
	class_table_body(Stream, "Physics.Mass", OutTyp->Physics.Mass);
	class_table_body(Stream, "Physics.BlastResistance", OutTyp->Physics.BlastResistance);
	class_table_body(Stream, "Graphics.RenderEnabled", OutTyp->Graphics.RenderEnabled);
	if (OutTyp->Graphics.TextureList.size() > 1)
		class_table_body(Stream, "OutTyp->Graphics.AnimationInterval", OutTyp->Graphics.AnimationInterval);
//	FIXME: Nothing to do when outputting entity type specific data...
	if (OutTyp->Properties.Type == "Player") {

	} else if (OutTyp->Properties.Type == "Projectile") {

	} else if (OutTyp->Properties.Type == "Particle") {

	}
ProcessEnd:
	class_table_end(Stream);
	return Stream;
}
