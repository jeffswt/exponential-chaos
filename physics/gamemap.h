
//	"gamemap.h" -*- C++ -*-
//	Created on: 7 March 2016
//	Game map management.
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

#ifndef PHYSICS_GAMEMAP_H_
#define PHYSICS_GAMEMAP_H_

#include "include/public.h"

#include <iostream>
#include <vector>
#include <map>
#include <mutex>
#include <set>

#include "algo/triple_pair.h"
#include "physics/chunk.h"
#include "physics/entity.h"

class GameMap
{
protected:
	std::map<Entity*, triple_pair<
		int, double, double> >	PendInsertList;
	std::set<Entity*>		PendInsertForceList;
	std::set<Entity*>		PendRemoveList;
	std::mutex				MemoryLock; // Avoid SIGSEGV
public:
//	Basic and fundamental properties (static)
	std::string	Name;
	std::string	Description;
	double		GravityConst;
	double		RegenerationValue; // Lives in double per second
	int			ModifyTime; // Last save time
	std::string	LevelPath; // Parent directory
//	Basic and fundamental properties (dynamic)
	bool		IsHost; // true : Server, false : Client
	double		CurTime;
//	Memory locking functions
	void	LockMemory(void) { MemoryLock.lock(); }
	void	UnlockMemory(void) { MemoryLock.unlock(); }
//	Fundamental lists that composes the map
	std::set<int>					ForbiddenLayers;
	double							WorldBoundary[4]; // 0 Left, 1 Right, 2 Top, 3 Bottom
	std::map<int, Chunk*>			ChunkList;
	std::map<long long, Entity*>	EntityList;
	std::map<long long, Entity*>	PlayerEntityList;
	std::map<long long, Entity*>	ParticleList;
//	Entity operations declarations
	bool	QueryAdjacentEntities(
			Entity*				Source,
			double				Tolerance,
			std::set<Entity*>&	TargetIdx);
	bool	InsertEntity(
			Entity*	InsEnt);
	bool	RemoveEntity(
			Entity*	RmEnt);
	bool	RemoveEntity(
			long long	RmGuid);
	Entity*	CreatePlayer(
			std::string	Name);
//	Entity operations (cached versions)
	/**
	 *  @brief  pend an insertion of entity
	 *  @param  InsEnt: the entity to be inserted
	 *          NewLayer: the layer to be changed to after insertion
	 *          NewX: the X-axis position to be changed to after insertion
	 *          NewY: the Y-axis position to be changed to after insertion
	 *  This function is designed as some functions might invoke SIGSEGV after
	 *  change in the std::map structure. To avoid this, we let the insertions
	 *  and deletion happen after the iteration of the container.
	 */
	bool	InsertEntityPended(
			Entity*	InsEnt,
			int		NewLayer,
			double	NewX,
			double	NewY);
	/**
	 *  @brief  pend an insertion of entity
	 *  @param  InsEnt: the entity to be inserted
	 *          NewLayer: the layer to be changed to after insertion
	 *  See InsertEntityPended(Entity*, int, double, double)
	 */
	bool	InsertEntityPended(
			Entity*	InsEnt,
			int		NewLayer);
	/**
	 *  @brief  pend an insertion of entity
	 *  @param  InsEnt: the entity to be inserted
	 *          NewX: the X-axis position to be changed to after insertion
	 *          NewY: the Y-axis position to be changed to after insertion
	 *  See InsertEntityPended(Entity*, int, double, double)
	 */
	bool	InsertEntityPended(
			Entity*	InsEnt,
			double	NewX,
			double	NewY);
	/**
	 *  @brief  pend an insertion of entity
	 *  @param  InsEnt: the entity to be inserted
	 *  See InsertEntityPended(Entity*, int, double, double)
	 */
	bool	InsertEntityPended(
			Entity*	InsEnt);
	/**
	 *  @brief  pend an insertion of entity and force it to be re-inserted
	 *  @param  InsEnt: the entity to be inserted
	 *          NewX: the X-axis position to be changed to after insertion
	 *          NewY: the Y-axis position to be changed to after insertion
	 *  This forces a re-insertion and should not be often called. In some
	 *  cases entities that didn't exist would be considered to be existed.
	 *  This causes the GUID indexes to forcefully point to the new entity.
	 */
	bool	InsertEntityPendedForce(
			Entity*	InsEnt);
	/**
	 *  @brief  pend a removal of entity
	 *  @param  RmEnt: the entity to be removed
	 *  See InsertEntityPended(Entity*, int, double, double). This function has
	 *  a similar purpose to the abovementioned function.
	 */
	bool	RemoveEntityPended(
			Entity*	RmEnt);
	/**
	 *  @brief  apply all pended changes
	 *  See InsertEntityPended(Entity*, int, double, double). This function has
	 *  a similar purpose to the abovementioned function.
	 */
	bool	CommitPendedChanges(
			void);
//	JSON-related functions
	/**
	 *  @brief  import only headers from the JSON file
	 *  @param  Input: a reference to the header string.
	 *  As designed, only to be called in networks, as clients need to know
	 *  some other properties about this GameMap.
	 */
	bool	ImportHeaderFromJson(
			std::string&	Input);
	/**
	 *  @brief  import from JSON file
	 *  @param  ImportChunks: true to do so and vice versa.
	 *          WorkingDirectory: the directory that the "level.json" lays under.
	 *          Stream: opened file stream of the level.
	 *  Import data from JSON, and import chunks if told to do so.
	 */
	bool	ImportFromJson(
			bool			ImportChunks,
			std::string		WorkingDirectory,
			std::ifstream&	Stream);
	/**
	 *  @brief  export to JSON document
	 *  @param  ExportChunks: as said
	 *          Output: a reference to the string that accepts the output
	 *  Export to JSON. If ExportChunks is set to false, then often it was called
	 *  by network environments.
	 */
	bool	ExportToJson(
			bool			ExportChunks,
			std::string&	Output);
	/**
	 *  @brief  export to JSON document
	 *  This is only available, as directly exporting to the specified path. This
	 *  path is given in ImportFromJson. Clients shall never call this function
	 *  as they do not have the sufficient data to invoke this.
	 */
	bool	ExportToJson(
			void);
	/**
	 *  @brief  reimport chunk data from JSON document
	 *  This function does not need to specify the path, and therefore does not
	 *  cause too much trouble.
	 */
	bool	ReimportFromJson(
			void);
//	Global affective functions
	/**
	 *  @brief  create chunks nearby chunk #ChunkNum.
	 *  Should call this, however most functions have been designed to avoid
	 *  SIGSEGVs caused by not calling this function.
	 */
	bool	CreateChunk(
			int	ChunkNum);
	/**
	 *  @brief  re-arrange entity positions to dedicated chunks
	 *  This is time consuming. Call only before final export. This might help
	 *  clean the chunk environment to avoid duplication.
	 */
	bool	ResetEntityPositions(
			void);
	/**
	 *  @brief  clear the GameMap.
	 *  Entity memory addresses are not destroyed, however.
	 */
	bool	Clear(
			void);
	GameMap(void);
};

#endif /* PHYSICS_GAMEMAP_H_ */
