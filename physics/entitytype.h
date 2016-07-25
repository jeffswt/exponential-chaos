
//	"entitytype.h" -*- C++ -*-
//	Created on: 3 March 2016
//	Entity type management.
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

#ifndef PHYSICS_ENTITYTYPE_H_
#define PHYSICS_ENTITYTYPE_H_

#include "include/public.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/glext.h>
#include <map>
#include <set>
#include <vector>

#include "physics/trigger.h"

/*
 * All units are in Standard Units / a.k.a metres, seconds, etc. We need total
 * accurate position when simulating physics.
 */

class EntityType
{
protected:
public:
	class typeProperties
	{
	public:
		std::string	Name; // Name of this entity type
		std::string	Type; // Classification
		std::string	Description; // Description
		void*		SpecificProperties; // Class-specific
		bool		ShowInCreative; // Display in creative inventory
	} Properties;
	class typePhysics
	{
	public:
		bool	PhysicsEnabled;
		bool	CollisionEnabled;
		double	LengthX, LengthY;
		double	Mass;
		double	FrictionFactor[4];	// Left, Right, Up, Down
		double	ElasticFactor[4]; // Left, Right, Up, Down
		double	BlastResistance;
		std::vector<std::string>	CollisionIgnoreName;
		std::set<EntityType*>		CollisionIgnore;
		std::vector<Trigger*>		TriggerList;
	} Physics;
	class typeGraphics
	{
	public:
		bool	RenderEnabled;
		double	AnimationInterval; // Time interval of each repetition
		double	Luminosity;
		double	LengthX, LengthY; // Length as displayed on screen
		std::vector<GLuint>	TextureList; // Animated textures
		GLuint				TextureOnHand; // Texture on the hotbar
	};
	std::vector<typeGraphics>	Graphics;
//	Informative functions
	/**
	 *  @brief  check data intactness
	 *  If the type is not like what it is supposed to be, it would not be
	 *  considered intact:
	 *    #1: a NULL pointer to EntityType*
	 *    #2: faulty specific properties
	 */
	bool	DataIntact(void);
//	JSON-related functions
	/**
	 *  @brief  import data from file
	 *  @param  Stream: opened file, given by the parent routine.
	 *  Mostly returns true.
	 */
	bool	ImportFromJson(
			std::istream&	Stream);
//	Globally accessible functions
	/**
	 *  @brief  process data that could not be processed before
	 *  Called after all entity types are loaded. Previously, names such as
	 *  CollisionIgnore are stored as strings, now they are to be translated
	 *  to pointers directly.
	 */
	bool	PostProcessDatum(
			void);
	EntityType(void);
};

/**
 *  @brief  specific properties for players
 *  The fundamental movement properties for players.
 */
class PlayerEntityType
{
public:
	double	MoveSpeed;
	double	JumpSpeed;
	double	MaxLife;
//	Constructors and destruction operators
	PlayerEntityType(void);
};

/**
 *  @brief  specific properties for entities
 *  The explosiveness responsive parameters for projectiles are defined here,
 *  which are to be read by projectile deployment and summon managers.
 */
class ProjectileEntityType
{
public:
	double	LaunchSpeed;
	double	LaunchInterval;
	double	DeployDelay;
	double	DeployRadius;
	double	DeployPowerDamage;
	double	DeployPowerMotion;
	double	DeployPowerBlast;
//	Constructors and destruction operators
	ProjectileEntityType(void);
};

/**
 *  @brief  specific properties for particles
 */
class ParticleEntityType
{
public:
	double	LifeTime;
//	Constructors and destruction operators
	ParticleEntityType(void);
};

#endif /* PHYSICS_ENTITYTYPE_H_ */
