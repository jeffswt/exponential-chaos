
//	"engine_private.h" -*- C++ -*-
//	Created on: 28 April 2016
//	Private header for physics engine.
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

#ifndef PHYSICS_ENGINE_PRIVATE_H_
#define PHYSICS_ENGINE_PRIVATE_H_

#include "network/netsync.h"

#include "physics/engine.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <thread>
#include <vector>

#include "debug/exception.h"
#include "native/guid.h"
#include "native/inputctrl.h"
#include "native/time.h"
#include "physics/config.h"
#include "physics/entity.h"
#include "physics/entitytype.h"
#include "physics/gamemap.h"
#include "physics/importer.h"
#include "physics/trigger.h"

extern	InputCtrlType	InputControl;
extern	ConfigType		GameConfig;

namespace PhEngine
{
	/**
	 *  @brief  physics frame renderer
	 *  @param  MainMap:  the target focused on, physics rendering would be
	 *            applied to this GameMap.
	 *          DeltaTime:  a very small time period.
	 *  This procedure is responsible for all the physical entities in the
	 *  given view distance to be moved.
	 */
	bool	RenderPhysicsFrame(
			GameMap*	MainMap,
			double		DeltaTime);
	/**
	 *  @brief  camera position updater
	 *  @param  MainMap:  the target focused on, camera position would be
	 *            updated according to this GameMap.
	 *          DeltaTime:  a very small time period.
	 *          DetectMouse:  if set to false, it means that mouse movements
	 *            would not be applied as camera position updating arguments.
	 *            more definitely, camera would not move according to the mouse.
	 *            Otherwise the camera would move to the mouse and the player.
	 *  This procedure would update the camera position, which is focused within
	 *  a certain circle around the main player.
	 */
	bool	UpdateCameraPosition(
			GameMap*	MainMap,
			double		DeltaTime,
			bool		DetectMouse);
	/**
	 *  @brief  player activity updater
	 *  @param  MainMap:  the target focused on, player activity would be
	 *            applied according to this GameMap.
	 *          DeltaTime:  a very small time period.
	 *  The movements of the player, projectile summons are initiated through
	 *  this function.
	 */
	bool	UpdatePlayerActivity(
			GameMap*	MainMap,
			double		DeltaTime);
	/**
	 *  @brief  player life updater
	 *  @param  MainMap:  the target focused on, all the players' life would be
	 *            calculated and updated according to this GameMap.
	 *          DeltaTime:  a very small time period.
	 *  The health of all points would be calculated, considering:
	 *    * regeneration of health points, in double
	 *    * if life is smaller than 0, then all activities would be disabled
	 *  Which would be proper, disregarding it is the server or not.
	 */
	bool	UpdatePlayerLife(
			GameMap*	MainMap,
			double		DeltaTime);
	/**
	 *  @brief  projectile summon manager
	 *  @param  MainMap:  the target focused on, projectiles would be summoned
	 *            within this GameMap.
	 *          DeltaTime:  a very small time period.
	 *  Projectile summons according to the player. Whether the block is snapping
	 *  to the grid or not is desired by the size. If both lengths (X and Y) are
	 *  exactly 1.0, then it would be snapped to grid. Otherwise not. If it is
	 *  projectile, speed and other specific properties would be applied, otherwise
	 *  would not.
	 */
	bool	UpdateProjectileSummon(
			GameMap*	MainMap,
			double		DeltaTime);
	/**
	 *  @brief  projectile deployment manager
	 *  @param  MainMap:  the target focused on, projectiles in this GameMap
	 *            would be considered whether to deploy.
	 *          DeltaTime:  a very small time period.
	 *  When projectiles reach their lifetime, they would be deployed. Then they
	 *  would cause damage to nearby players, destroy related blocks under a
	 *  specific blast resistance, cause movement to nearby moving entities.
	 */
	bool	UpdateProjectileDeployment(
			GameMap*	MainMap,
			double		DeltaTime);
	/**
	 *  @brief  particle destruction watcher
	 *  @param  MainMap:  the target focused on, particles in this GameMap
	 *            would be considered whether to deploy.
	 *          DeltaTime:  a very small time period.
	 *  When particles reach their lifetime, they would be destroyed.
	 */
	bool	UpdateParticleDestruction(
			GameMap*	MainMap,
			double		DeltaTime);
};

#endif /* PHYSICS_ENGINE_PRIVATE_H_ */
