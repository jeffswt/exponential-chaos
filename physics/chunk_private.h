
//	"chunk_private.h" -*- C++ -*-
//	Created on: 28 April 2016
//	Private headers for chunk management.
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

#ifndef PHYSICS_CHUNK_PRIVATE_H_
#define PHYSICS_CHUNK_PRIVATE_H_

namespace	ChunkInternals
{
	/**
	 *  @brief  Comparison function and definitions.
	 *  If PHYSICS_CHUNK_USE_RED_BLACK_TREE_ALGORITHM_ is enabled, these
	 *  functions would be put to use.
	 */
	bool set_cmp_lbnd_fun(Entity* __x, Entity* __y);
	bool set_cmp_rbnd_fun(Entity* __x, Entity* __y);
	bool set_cmp_ubnd_fun(Entity* __x, Entity* __y);
	bool set_cmp_dbnd_fun(Entity* __x, Entity* __y);
	/**
	 *  @brief  tricks to trick the STL for its strange mechanism
	 *  STL uses STRUCT to compare values.
	 */
	struct set_cmp_lbnd : public std::binary_function<Entity*, Entity*, bool>
	  { bool operator() (Entity* __x, Entity* __y) const
	    { return set_cmp_lbnd_fun(__x, __y); } };
	struct set_cmp_rbnd : public std::binary_function<Entity*, Entity*, bool>
	  { bool operator() (Entity* __x, Entity* __y) const
	    { return set_cmp_rbnd_fun(__x, __y); } };
	struct set_cmp_ubnd : public std::binary_function<Entity*, Entity*, bool>
	  { bool operator() (Entity* __x, Entity* __y) const
	    { return set_cmp_ubnd_fun(__x, __y); } };
	struct set_cmp_dbnd : public std::binary_function<Entity*, Entity*, bool>
	  { bool operator() (Entity* __x, Entity* __y) const
	    { return set_cmp_dbnd_fun(__x, __y); } };
	bool bnd_cmp_lbnd(Entity* __x, double __y);
	bool bnd_cmp_rbnd(Entity* __x, double __y);
	bool bnd_cmp_ubnd(Entity* __x, double __y);
	bool bnd_cmp_dbnd(Entity* __x, double __y);
}

#endif /* PHYSICS_CHUNK_PRIVATE_H_ */
