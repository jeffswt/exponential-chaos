
//	"engine_updprjsmn.cpp" -*- C++ -*-
//	Created on: 28 April 2016
//	Physics engine / Projectile summon manager.
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

bool	PhEngine::UpdateProjectileSummon(
		GameMap*	MainMap,
		double		DeltaTime)
{
//	Projectile summons
	Entity*					playerEntity;
	PlayerEntity*			playerExt;
	EntityType*				projEntity;
	ProjectileEntityType*	projEnttyp;
	playerEntity = MainMap->EntityList[InputControl.PlayerGuid];
	if (!playerEntity->DataIntact())
		return false;
	playerExt = (PlayerEntity*)playerEntity->Physics.ExtendedTags;
//	If the player had already quit the game (or deceased), don't bother him.
	if (playerExt->Life <= 0)
		return true;
	if (playerExt->Inventory.size() <= 0)
		return true;
//	Make all those memory overflows go to the end...
	if ((unsigned int)playerExt->InventoryFocus > playerExt->Inventory.size()) {
		return true;
	} else if (playerExt->InventoryFocus <= 0) {
		playerExt->InventoryFocus = 1;
		return true;
	}
//	Finding this entity...
	projEntity = playerExt->Inventory[playerExt->InventoryFocus - 1].first;
	if (!projEntity)
		throw NullPointerException();
//	Defining whether to throw the projectile or not
	if (projEntity->Properties.Type == "Projectile") {
		projEnttyp = (ProjectileEntityType*)projEntity->Properties.SpecificProperties;
		if (!projEnttyp)
			throw NullPointerException();
		if (InputControl.LDown && MainMap->CurTime - playerExt->LastProjectileTime >
				projEnttyp->LaunchInterval) {
			Entity *newEntity = new Entity;
			newEntity->InheritFrom(projEntity);
//			Editing general data for generated entity
			newEntity->Properties.Layer = playerEntity->Properties.Layer;
			newEntity->Properties.Name = "__ZwProgramGeneratedEntity7";
			newEntity->Properties.Owner = playerEntity->Properties.Name;
//			Defining position for this entity class
			double	distX = (double)(InputControl.MouseX - GameConfig.WindowWidth / 2) /
					GameConfig.PixelRatio + InputControl.CameraX - playerEntity->Physics.PosX;
			double	distY = (double)(GameConfig.WindowHeight / 2 - InputControl.MouseY) /
					GameConfig.PixelRatio + InputControl.CameraY - playerEntity->Physics.PosY;
			double	distLen = sqrt(distX * distX + distY * distY);
			const	double	deployTolerance = 0.08;
			double	realDist = std::max(playerEntity->Properties.Type->Physics.LengthX,
										playerEntity->Properties.Type->Physics.LengthY) / 2.0 +
								deployTolerance +
								std::max(newEntity->Properties.Type->Physics.LengthX,
										newEntity->Properties.Type->Physics.LengthY) / 2.0;
			newEntity->Physics.PosX = playerEntity->Physics.PosX + realDist * distX / distLen;
			newEntity->Physics.PosY = playerEntity->Physics.PosY + realDist * distY / distLen;
			double	distCombine = projEnttyp->DeployRadius;
			newEntity->Physics.VelX = playerEntity->Physics.VelX + projEnttyp->LaunchSpeed * distX / distCombine;
			newEntity->Physics.VelY = playerEntity->Physics.VelY + projEnttyp->LaunchSpeed * distY / distCombine;
//			Consuming this item in player inventory
			if (!playerExt->IsCreative)
				playerExt->Inventory[playerExt->InventoryFocus - 1].second--;
			if (playerExt->Inventory[playerExt->InventoryFocus - 1].second <= 0)
				playerExt->Inventory.erase(playerExt->Inventory.begin() + playerExt->InventoryFocus - 1);
//			Inserting to main structure
			MainMap->InsertEntityPendedForce(newEntity);
			NetmgrInsertEntity(newEntity);
			playerExt->LastProjectileTime = MainMap->CurTime;
		}
	} else {
		if (InputControl.LDown && MainMap->CurTime - playerExt->LastProjectileTime > 0.1) {
//			This is a (mostly static...) new entity, often used in map editing.
//			We create this in the mood of letting it interact with the outer world more profoundly.
			Entity*	newEntity = new Entity;
			newEntity->InheritFrom(projEntity);
			newEntity->Properties.Layer = playerEntity->Properties.Layer;
			double	distX = (double)(InputControl.MouseX - GameConfig.WindowWidth / 2) /
					GameConfig.PixelRatio + InputControl.CameraX;
			double	distY = (double)(GameConfig.WindowHeight / 2 - InputControl.MouseY) /
					GameConfig.PixelRatio + InputControl.CameraY;
			newEntity->Physics.PosX = distX;
			newEntity->Physics.PosY = distY;
			newEntity->Physics.VelX = 0.0;
			newEntity->Physics.VelY = 0.0;
			if (projEntity->Properties.Type == "Block") {
				newEntity->Physics.PosX = trunc(distX >= 0 ? distX + 0.5 : distX - 0.5);
				newEntity->Physics.PosY = trunc(distY >= 0 ? distY + 0.5 : distY - 0.5);
			}
//			Consuming this item in player inventory
			if (!playerExt->IsCreative)
				playerExt->Inventory[playerExt->InventoryFocus - 1].second--;
			if (playerExt->Inventory[playerExt->InventoryFocus - 1].second <= 0)
				playerExt->Inventory.erase(playerExt->Inventory.begin() + playerExt->InventoryFocus - 1);
//			Inserting to main structure
			MainMap->InsertEntityPendedForce(newEntity);
			NetmgrInsertEntity(newEntity);
			playerExt->LastProjectileTime = MainMap->CurTime;
		}
		if (InputControl.LDown)
			playerExt->LastProjectileTime = MainMap->CurTime;
	}
	return true;
}
