
//	"entity.h" -*- C++ -*-
//	Created on: 5 March 2016
//	Entity management.
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

#ifndef PHYSICS_ENTITY_H_
#define PHYSICS_ENTITY_H_

#include "include/public.h"

#include <cstdlib>
#include <map>
#include <rapidjson/document.h>

#include "algo/triple_pair.h"
#include "physics/entitytype.h"
#include "physics/trigger.h"

class Entity
{
public:
	class typeProperties
	{
	public:
		EntityType*	Type; // Pointer to the original type
		std::string	TypeName; // Actual type name
		int			TypeState; // The graphical block state of the entity type
		long long	Guid; // Global Unique Identifier, used to pass over network
		int			Layer; // Layer of this Entity.
		double		GenTime; // Generation time
		std::string	Name; // Given name by the map creator, not necessary
		std::string	Owner; // Who generated this entity ( blank by default )
	} Properties;
	class typePhysics
	{
	public:
		bool	PhysicsChanged; // Reverse original physics simulation (boolean) settings
		bool	CollisionChanged; // Reverse original collision settings
		bool	RenderDisabled; // Disable render, in actuality
		double	PosX, PosY; // Accurate position
		double	VelX, VelY; // Current velocity
		void*	ExtendedTags; // Entity-specific properties
		std::vector<Trigger*>	TriggerList;
	} Physics;
//	Informative functions, used to reduce coding complexity
	bool	PhysicsEnabled(void);
	bool	CollisionEnabled(Entity* Object);
	bool	CollisionEnabled(void);
	bool	RenderEnabled(void);
	/**
	 *  @brief  check data intactness
	 *  If the entity is not like what it is supposed to be, it would not be
	 *  considered intact:
	 *    #1: a NULL pointer to Entity*
	 *    #2: faulty extended tags
	 *    #3: a NULL pointer to EntityType*
	 */
	bool	DataIntact(void);
//	Operative functions, to manipulate its data
	/**
	 *  @brief  process inheritance
	 *  @param  EntTyp: To inherit properties from this type.
	 *  Returns false if failed in any part, otherwise true.
	 */
	bool	InheritFrom(
			EntityType*	EntTyp);
	/**
	 *  @brief  process inheritance
	 *  @param  OtherEntity: to copy data from this entity.
	 *  Returns false if failed in any part, otherwise true.
	 */
	bool	InheritFrom(
			Entity*	OtherEntity);
//	JSON-related functions
	/**
	 *  @brief  import data from JSON value
	 *  @param  Config: The source value
	 *          ImportAdditionalData: false if importing from file, true if
	 *            importing over networks.
	 *  Mostly returns true.
	 */
	bool	ImportFromJson(
			rapidjson::Value&	Config,
			bool				ImportAdditionalData);
	/**
	 *  @brief  export data to JSON documents
	 *  @param  Config: the target value
	 *          CAlloc: the allocator of the target rapidjson::Document
	 *          ExportAdditionalData: false if exporting to file, true if
	 *            exporting over networks.
	 *  Mostly returns true.
	 */
	bool	ExportToJson(
			rapidjson::Value&	Config,
			rapidjson::Document::
				AllocatorType&	CAlloc,
			bool				ExportAdditionalData);
//	Constructors and destruction operators
	Entity(void);
	Entity(Entity*);
	Entity(EntityType*);
	~Entity(void);
};

class PlayerEntity
{
public:
	double	LastJumpTime[2];
	double	LastJumpHeight;
	double	LastProjectileTime;
	double	Life;
	double	DeceaseTime;
	int		InventoryFocus;
	bool	IsCreative;
	std::vector<triple_pair<
			EntityType*, int, int> >	Inventory;
//	Constructors and destruction operators
	PlayerEntity(void);
};

#endif /* PHYSICS_ENTITY_H_ */
