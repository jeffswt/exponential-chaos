
//	"chunk.h" -*- C++ -*-
//	Created on: 10 April 2016
//	Chunk management.
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

#ifndef PHYSICS_CHUNK_H_
#define PHYSICS_CHUNK_H_

#include "include/public.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>

#include "physics/entity.h"
#include "physics/chunk_private.h"

//#define	PHYSICS_CHUNK_USE_RED_BLACK_TREE_ALGORITHM_

int	GetEntityChunkNum(
		double	PosX);

class Chunk
{
protected:
#ifdef PHYSICS_CHUNK_USE_RED_BLACK_TREE_ALGORITHM_
//	Sort entities with their physical positions
	std::set<Entity*, ChunkInternals::set_cmp_lbnd>	PosListLBnd;
	std::set<Entity*, ChunkInternals::set_cmp_rbnd>	PosListRBnd;
	std::set<Entity*, ChunkInternals::set_cmp_ubnd>	PosListUBnd;
	std::set<Entity*, ChunkInternals::set_cmp_dbnd>	PosListDBnd;
#endif
public:
	double	lBound, rBound;
	std::map<long long, Entity*>	EntityList;
	std::map<int, std::map<
		long long, Entity*> >		EntityLayerList;
//	Specific entity lists used to manage special items in the chunk
	std::map<long long, Entity*>	PhysicsEntityList;
	std::map<long long, Entity*>	ProjectileEntityList;
//	Entity related functions
#ifdef PHYSICS_CHUNK_USE_RED_BLACK_TREE_ALGORITHM_
	bool	QueryAdjacentEntities(
			Entity*				Source,
			double				Tolerance,
			std::set<Entity*>&	TargetList);
#endif
	/**
	 *  @brief  insert the given entity into the chunk
	 *  @param  NewEnt: the object to insert
	 *  GUID would not be generated upon call. This should *ONLY* be called
	 *  through the GameMap structure, instead of anything else. Not calling
	 *  directly through the GameMap would cause it not to be synchronised.
	 */
	bool	InsertEntity(
			Entity*	NewEnt);
	/**
	 *  @brief  remove entity safely
	 *  @param  RmEnt: the object to remove
	 *  If the RmEnt is not intact, this procedure would actively refuse the
	 *  operation. So removing a NULL entity would not be accepted.
	 */
	bool	RemoveEntity(
			Entity*	RmEnt);
	/**
	 *  @brief  remove entity very safely
	 *  @param  RmGuid: the GUId to remove
	 *  If wanted to remove an invalid entity, use the GUID instead. This is
	 *  permitted as the GUID is stored in the GameMap's structures safely, and
	 *  can be removed through this. Vice versa, if directly calling this
	 *  procedure without accessing the GameMap, faults might happen.
	 */
	bool	RemoveEntity(
			long long	RmGuid);
//	Global affective functions
	/**
	 *  @brief  clear the chunk
	 *  Empty all logic connections within the chunk.
	 */
	bool	Clear(
			void);
	Chunk(void);
};

#endif /* PHYSICS_CHUNK_H_ */
