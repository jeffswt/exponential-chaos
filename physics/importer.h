
//	"importer.h" -*- C++ -*-
//	Created on: 5 March 2016
//	Asset loading procedures.
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

#ifndef PHYSICS_IMPORTER_H_
#define PHYSICS_IMPORTER_H_

#include "include/public.h"

#include <iostream>
#include <vector>

#include "physics/gamemap.h"

/**
 *  @brief  import game entity types.
 *  This imports all resource packs by default. Begin functions and end functions
 *  are to be called, as is a proper thread. Directly calling it is not a good
 *  idea.
 */
bool	ImportGameEntityTypes(
		void);
bool	beginImportGameEntityTypes(
		void);
bool	endImportGameEntityTypes(
		void);

/**
 *  @brief  import headers of local maps.
 *  This imports all headers, but not chunks themselves, nor does this import
 *  the entities of the GameMaps.
 */
bool	ImportGameMapHeaders(
		std::vector<GameMap*>&	MapList);

#endif /* PHYSICS_IMPORTER_H_ */
