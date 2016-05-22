
//	"engine.h" -*- C++ -*-
//	Created on: 5 March 2016
//	Physics engine.
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

#ifndef PHYSICS_ENGINE_H_
#define PHYSICS_ENGINE_H_

#include "include/public.h"

#include "physics/gamemap.h"

namespace PhEngine
{
	enum	PhEngineStateType
	{
		Stopped,
		Paused,
		Running,
		InventoryOpen,
		SaveExit
	};
	extern	PhEngineStateType	PhEngineState;
}

/**
 *  @brief  start physics simulator
 *  @param  MainMap: the map that needs to start.
 *  Called by GUI, should not be interpreted by user. MainMap needs to be
 *  previously generated through specific routines.
 */
bool	BeginPhysicsEngine(
		GameMap*	MainMap);

/**
 *  @brief  terminate physics simulator
 *  This function is a blocking function, only until the engine is fully
 *  terminated should the function return a value.
 */
bool	TerminatePhysicsEngine(
		void);

/**
 *  @brief  wait for engine event
 *  This function waits for particular events:
 *    #1: network initialisation
 *    #2: engine termination
 */
bool	WaitForEngineEvent(
		void);

#endif /* PHYSICS_ENGINE_H_ */
