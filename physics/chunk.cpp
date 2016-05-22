
//	"chunk.cpp" -*- C++ -*-
//	Created on: 10 April 2016
//	Definitions for chunk management.
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

#include "physics/chunk.h"

#include <algorithm>
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include "debug/exception.h"
#include "native/guid.h"
#include "native/jsonimport.h"

using namespace std;
namespace ChunkInternals
{
#define my_get_tiny_guid(_a) (_a->Properties.Guid * 1.0842021724855044340074528008699e-19)
#ifdef PHYSICS_CHUNK_USE_RED_BLACK_TREE_ALGORITHM_
	bool set_cmp_lbnd_fun(Entity* __x, Entity* __y)
	  { return __x->Physics.PosX - __x->Properties.Type->Physics.LengthX / 2 + my_get_tiny_guid(__x) <
	           __y->Physics.PosX - __y->Properties.Type->Physics.LengthX / 2 + my_get_tiny_guid(__y); }
	bool set_cmp_rbnd_fun(Entity* __x, Entity* __y)
      { return __x->Physics.PosX + __x->Properties.Type->Physics.LengthX / 2 + my_get_tiny_guid(__x) >
	           __y->Physics.PosX + __y->Properties.Type->Physics.LengthX / 2 + my_get_tiny_guid(__y); }
	bool set_cmp_ubnd_fun(Entity* __x, Entity* __y)
	  { return __x->Physics.PosY + __x->Properties.Type->Physics.LengthY / 2 + my_get_tiny_guid(__x) >
	           __y->Physics.PosY + __y->Properties.Type->Physics.LengthY / 2 + my_get_tiny_guid(__y); }
	bool set_cmp_dbnd_fun(Entity* __x, Entity* __y)
	  { return __x->Physics.PosY - __x->Properties.Type->Physics.LengthY / 2 + my_get_tiny_guid(__x) <
	           __y->Physics.PosY - __y->Properties.Type->Physics.LengthY / 2 + my_get_tiny_guid(__y); }
	bool bnd_cmp_lbnd(Entity* __x, double __y)
	  { return __y > __x->Physics.PosX - __x->Properties.Type->Physics.LengthX / 2; }
	bool bnd_cmp_rbnd(Entity* __x, double __y)
	  { return __y < __x->Physics.PosX + __x->Properties.Type->Physics.LengthX / 2; }
	bool bnd_cmp_ubnd(Entity* __x, double __y)
	  { return __y < __x->Physics.PosY + __x->Properties.Type->Physics.LengthY / 2; }
	bool bnd_cmp_dbnd(Entity* __x, double __y)
	  { return __y > __x->Physics.PosY - __x->Properties.Type->Physics.LengthY / 2; }
#endif
}

int	GetEntityChunkNum(
		double	PosX)
{
	return PosX >= 0 ?
			(PosX + 8.0) / 16.0 :
			(PosX - 8.0) / 16.0;
}

Chunk::Chunk(void)
{
	lBound = - 8.0;
	rBound = 8.0;
	EntityList.clear();
	PhysicsEntityList.clear();
	ProjectileEntityList.clear();
	return ;
}

#ifdef PHYSICS_CHUNK_USE_RED_BLACK_TREE_ALGORITHM_
bool	Chunk::QueryAdjacentEntities(
		Entity*				Source,
		double				Tolerance,
		std::set<Entity*>&	TargetIdx)
{
	using	namespace	ChunkInternals;
//	Defining iterators for later use.
	auto	itertLB = std::lower_bound(PosListRBnd.begin(), PosListRBnd.end(),
			Source->Physics.PosX, bnd_cmp_rbnd),
			itertLE = std::lower_bound(PosListRBnd.begin(), PosListRBnd.end(),
			Source->Physics.PosX - Source->Properties.Type->Physics.LengthX / 2 - Tolerance, bnd_cmp_rbnd);
	auto	itertRB = std::lower_bound(PosListLBnd.begin(), PosListLBnd.end(),
			Source->Physics.PosX, bnd_cmp_lbnd),
			itertRE = std::lower_bound(PosListLBnd.begin(), PosListLBnd.end(),
			Source->Physics.PosX + Source->Properties.Type->Physics.LengthX / 2 + Tolerance, bnd_cmp_lbnd);
	auto	itertUB = std::lower_bound(PosListDBnd.begin(), PosListDBnd.end(),
			Source->Physics.PosY, bnd_cmp_dbnd),
			itertUE = std::lower_bound(PosListDBnd.begin(), PosListDBnd.end(),
			Source->Physics.PosY + Source->Properties.Type->Physics.LengthY / 2 + Tolerance, bnd_cmp_dbnd);
	auto	itertDB = std::lower_bound(PosListUBnd.begin(), PosListUBnd.end(),
			Source->Physics.PosY, bnd_cmp_ubnd),
			itertDE = std::lower_bound(PosListUBnd.begin(), PosListUBnd.end(),
			Source->Physics.PosY - Source->Properties.Type->Physics.LengthY / 2 - Tolerance, bnd_cmp_ubnd);
#define ForEachWithIterator(_itert,_begin,_end,_container)\
	for (auto _itert = _begin; _itert != _end && itert != _container.end(); _itert++)\
		if (TargetIdx.find(*_itert) == TargetIdx.end())\
			TargetIdx.insert(*_itert)
	ForEachWithIterator(itert, itertLB, itertLE, PosListRBnd);
	ForEachWithIterator(itert, itertRB, itertRE, PosListLBnd);
	ForEachWithIterator(itert, itertUB, itertUE, PosListDBnd);
	ForEachWithIterator(itert, itertDB, itertDE, PosListUBnd);
	return true;
}
#endif

bool	Chunk::InsertEntity(
		Entity*		NewEnt)
{
	if (EntityList[NewEnt->Properties.Guid] != NULL)
		return true;
	EntityList[NewEnt->Properties.Guid] = NewEnt;
	if (EntityLayerList.find(NewEnt->Properties.Layer) == EntityLayerList.end()) {
		std::map<long long, Entity*> newMap;
		EntityLayerList[NewEnt->Properties.Layer] = newMap;
	}
	EntityLayerList[NewEnt->Properties.Layer][NewEnt->Properties.Guid] = NewEnt;
//	Inserting NewEnt into specific vectors
	if (NewEnt->PhysicsEnabled())
		PhysicsEntityList[NewEnt->Properties.Guid] = NewEnt;
	if (NewEnt->Properties.Type->Properties.Type == "Projectile")
		ProjectileEntityList[NewEnt->Properties.Guid] = NewEnt;
//	Inserting entity into position-sensitive indexers
#ifdef PHYSICS_CHUNK_USE_RED_BLACK_TREE_ALGORITHM_
	PosListLBnd.insert(NewEnt);
	PosListRBnd.insert(NewEnt);
	PosListUBnd.insert(NewEnt);
	PosListDBnd.insert(NewEnt);
#endif
	return true;
}

bool	Chunk::RemoveEntity(
		Entity*	RmEnt)
{
	if (!RmEnt->DataIntact())
		return false;
//	Look up in map structure and desire whether to delete it
	EntityList.erase(RmEnt->Properties.Guid);
	EntityLayerList[RmEnt->Properties.Layer].erase(RmEnt->Properties.Guid);
	if (EntityLayerList[RmEnt->Properties.Layer].size() <= 0)
		EntityLayerList.erase(RmEnt->Properties.Layer);
//	Specific deletion for specific types
	if (RmEnt->PhysicsEnabled())
		PhysicsEntityList.erase(RmEnt->Properties.Guid);
	if (RmEnt->Properties.Type->Properties.Type == "Projectile")
		ProjectileEntityList.erase(RmEnt->Properties.Guid);
//	Erasing entity from position-sensitive indexers
#ifdef PHYSICS_CHUNK_USE_RED_BLACK_TREE_ALGORITHM_
	PosListLBnd.erase(RmEnt);
	PosListRBnd.erase(RmEnt);
	PosListUBnd.erase(RmEnt);
	PosListDBnd.erase(RmEnt);
#endif
	return true;
}

bool	Chunk::RemoveEntity(
		long long	RmGuid)
{
	EntityList.erase(RmGuid);
	for (auto& itert : EntityLayerList)
		itert.second.erase(RmGuid);
	PhysicsEntityList.erase(RmGuid);
	ProjectileEntityList.erase(RmGuid);
	return true;
}

bool	Chunk::Clear(
		void)
{
	EntityList.clear();
	for (auto& itert : EntityLayerList)
		itert.second.clear();
	EntityLayerList.clear();
	PhysicsEntityList.clear();
	ProjectileEntityList.clear();
//	Clearing position-sensitive indexers
#ifdef PHYSICS_CHUNK_USE_RED_BLACK_TREE_ALGORITHM_
	PosListLBnd.clear();
	PosListRBnd.clear();
	PosListUBnd.clear();
	PosListDBnd.clear();
#endif
	return true;
}
