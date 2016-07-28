
//	"engine_updplylif.cpp" -*- C++ -*-
//	Created on: 28 April 2016
//	Physics engine / Player health point manager.
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

#include "network/chat.h"

bool	PhEngine::UpdatePlayerLife(
		GameMap*	MainMap,
		double		DeltaTime)
{
	for (auto Itert : MainMap->PlayerEntityList) {
		Entity*				PlayerEnt = Itert.second;
		PlayerEntity*		PlayerExt;
		PlayerEntityType*	PlayerTyp;
		if (!PlayerEnt->DataIntact())
			return false;
		PlayerExt = (PlayerEntity*)PlayerEnt->Physics.ExtendedTags;
		PlayerTyp = (PlayerEntityType*)PlayerEnt->Properties.Type->Properties.SpecificProperties;
		if (PlayerEnt->Properties.Name == "__ZwDefaultEntity7Player")
			continue;
//		An already deceased entity... Why should we bother to mess up with it?
		if (PlayerExt->Life < 0) {
			if (!PlayerEnt->Physics.PhysicsChanged) {
				PlayerEnt->Physics.PhysicsChanged = true;
				PlayerEnt->Physics.CollisionChanged = true;
				PlayerEnt->Physics.RenderDisabled = true;
				PlayerExt->Life = -1.0;
				PlayerExt->DeceaseTime = GetProcessTime();
				NetmgrInsertEntity(PlayerEnt);
				if (MainMap->IsHost) {
					std::string PostMsg = chatGenDeceaseMessage(PlayerEnt->Properties.Name);
					chatInsertMessage(PostMsg);
					NetmgrPostMessage(PostMsg);
				}
			} else if (GetProcessTime() - PlayerExt->DeceaseTime >= MainMap->RespawnDelay) {
//				That means the player needs to be respawned
				if (MainMap->IsHost ||
						PlayerEnt->Properties.Name == GameConfig.PlayerName) {
					MainMap->RespawnPlayer(PlayerEnt);
					NetmgrInsertEntity(PlayerEnt);
				}
			}
		} else {
//			The physics datum of the player should be properly updated, as to
//			properly calculate the player life deduction as fall damage.
			PlayerExt->LastJumpHeight = std::max(
					PlayerExt->LastJumpHeight,
					PlayerEnt->Physics.PosY);
//			If in the middle the player tried to slow down through a rocket
//			booster or something else, we suggest that he should take less fall
//			damage.
			if (!(PlayerEnt->Physics.VelY > 0) ^ (MainMap->GravityConst > 0))
				PlayerExt->LastJumpHeight = std::min(
					PlayerExt->LastJumpHeight,
					PlayerEnt->Physics.PosY);
//			This is to update its life.
			if (PlayerExt->Life < PlayerTyp->MaxLife)
				PlayerExt->Life += MainMap->RegenerationValue * DeltaTime;
			else
				PlayerExt->Life = PlayerTyp->MaxLife;
		}
//		A useless thing used to make the code look nicer...
		continue;
	}
	return true;
}
