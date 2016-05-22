
//	"netsync.h" -*- C++ -*-
//	Created on: 14 May 2016
//	Game network manager.
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

#ifndef NETWORK_NETSYNC_H_
#define NETWORK_NETSYNC_H_

#include "include/public.h"

#include "network/cnl.h"
#include "network/tusnil.h"
#include "physics/entity.h"
#include "physics/gamemap.h"

/**
 *  @brief  initiate network managers
 *  @param  MainMap: the target GameMap.
 *          TargetAddr: (client only) to connect to a certain computer
 */
bool	BeginServerNetworkManager(
		GameMap*	MainMap);

bool	BeginClientNetworkManager(
		std::string	TargetAddr,
		GameMap*	MainMap);

/**
 *  @brief  terminate network managers
 *  Blocking functions, does not finish until the managers are stopped. This
 *  provides the exception safety such std::thread would cause std::terminate
 *  to function.
 */
bool	EndServerNetworkManager(
		void);

bool	EndClientNetworkManager(
		void);

/**
 *  @brief  post entity info to the network
 *  @param  InsEnt: the entity to be read
 *  Does no modification to InsEnt. Returns false if the given parameter(s) are
 *  not intact.
 */
bool	NetmgrInsertEntity(
		Entity*	InsEnt);

/**
 *  @brief  tell network clients / server to remove an entity
 *  @param  RmEnt: the entity to be removed
 *  Does no modification to RmEnt. Returns false if the given parameter(s) are
 *  not intact.
 */
bool	NetmgrRemoveEntity(
		Entity*	RmEnt);

/**
 *  @brief  post entity movement to the network
 *  @param  MvEnt: the entity to be moved
 *  Does no modification to MvEnt. Returns false if the given parameter(s) are
 *  not intact.
 */
bool	NetmgrMoveEntity(
		Entity*	MvEnt);

/**
 *  @brief  post chat message to the network
 *  @param  Msg: the message to be broadcasted
 *  Does not display message locally. Only displays message to remote computers.
 *  Therefore if want to display it locally, do it yourself.
 */
bool	NetmgrPostMessage(
		std::string	Msg);

/**
 *  @brief  post player life to the network
 *  @param  SetEnt: the entity to be broadcasted
 *  Tell the network that a certain player's life to be modified to its given
 *  value. If the parameter is not intact, or is not a player, it returns false.
 */
bool	NetmgrSetEntityLife(
		Entity*	SetEnt);

/**
 *  @brief  query whether the downloading of initial information is complete
 *  This function is a blocking function. It does not finish until all data
 *  is reported to be loaded by the server.
 */
bool	NetmgrQueryLoadComplete(
		void);

#endif /* NETWORK_NETSYNC_H_ */
