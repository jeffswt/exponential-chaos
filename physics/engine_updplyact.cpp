
//	"engine_updplyact.cpp" -*- C++ -*-
//	Created on: 28 April 2016
//	Physics engine / Player activity updater.
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

#include "physics/engine_private.h"

#include "native/keyboard.h"

bool	PhEngine::UpdatePlayerActivity(
		GameMap*	MainMap,
		double		DeltaTime)
{
	Entity*				playerEntity;
	PlayerEntityType*	spcProp;
	PlayerEntity*		playerExt;
	bool				flagRequireUpdatePosition = false;
	bool				flagRequireUpdateDefinition = false;
//	Find controlled player in game map
	playerEntity = MainMap->EntityList[InputControl.PlayerGuid];
	if (!playerEntity->DataIntact())
		return false;
	spcProp = (PlayerEntityType*)playerEntity->Properties.Type->Properties.SpecificProperties;
	playerExt = (PlayerEntity*)playerEntity->Physics.ExtendedTags;
//	Move leftwards
	if (kGetKeyState('A') || kGetKeyState(VK_LEFT)) {
		playerEntity->Physics.VelX -= spcProp->MoveSpeed * DeltaTime;
//		Sprinting and fast leaping
		if (kGetKeyState(VK_CONTROL))
			playerEntity->Physics.VelX -= spcProp->MoveSpeed * 0.45 * DeltaTime;
		flagRequireUpdatePosition = true;
	}
//	Move rightwards
	if (kGetKeyState('D') || kGetKeyState(VK_RIGHT)) {
		playerEntity->Physics.VelX += spcProp->MoveSpeed * DeltaTime;
//		Sprinting and fast leaping
		if (kGetKeyState(VK_CONTROL))
			playerEntity->Physics.VelX += spcProp->MoveSpeed * 0.45 * DeltaTime;
		flagRequireUpdatePosition = true;
	}
//	Jumping on the ground
	if (kGetKeyState(' ')) {
		if (!playerExt->IsCreative) {
			if (MainMap->CurTime - playerExt->LastJumpTime[1] < 0.04)
				playerEntity->Physics.VelY += spcProp->JumpSpeed * 0.097 * MainMap->GravityConst;
		} else {
			playerEntity->Physics.VelY += spcProp->JumpSpeed * 0.006 * MainMap->GravityConst;
		}
		flagRequireUpdatePosition = true;
	}
//	Sneaking and holding X-axis position
	if (kGetKeyState(VK_SHIFT)) {
		playerEntity->Physics.VelX *= 0.87;
		playerEntity->Physics.VelY -= spcProp->JumpSpeed * 0.006 * MainMap->GravityConst;
		flagRequireUpdatePosition = true;
	}
//	Changing between layers
	if (kGetKeyOnpress('W') || kGetKeyOnpress(VK_UP)) {
		if (MainMap->ForbiddenLayers.find(playerEntity->Properties.Layer - 1)
				== MainMap->ForbiddenLayers.end())
			MainMap->InsertEntityPended(playerEntity,
				playerEntity->Properties.Layer - 1);
		flagRequireUpdatePosition = true;
	}
	if (kGetKeyOnpress('S') || kGetKeyOnpress(VK_DOWN)) {
		if (MainMap->ForbiddenLayers.find(playerEntity->Properties.Layer + 1)
				== MainMap->ForbiddenLayers.end())
			MainMap->InsertEntityPended(playerEntity,
				playerEntity->Properties.Layer + 1);
		flagRequireUpdatePosition = true;
	}
//	Switching slots using numbers 1 to 9
	for (int procNum = 1; procNum <= 9; procNum++)
		if (kGetKeyOnpress('0' + procNum)) {
			playerExt->InventoryFocus = procNum;
			flagRequireUpdateDefinition = true;
		}
//	Special buttons
	if (kGetKeyOnpress(VK_ESCAPE))
		PhEngine::PhEngineState = PhEngine::Paused;
	if (kGetKeyOnpress('E'))
		PhEngine::PhEngineState = PhEngine::InventoryOpen;
//	Upload player data to server
	if (flagRequireUpdateDefinition)
		NetmgrInsertEntity(playerEntity);
	else if (flagRequireUpdatePosition)
		NetmgrMoveEntity(playerEntity);
	return true;
}
