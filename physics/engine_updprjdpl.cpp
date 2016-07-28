
//	"engine_updprjdpl.cpp" -*- C++ -*-
//	Created on: 28 April 2016
//	Physics engine / Projectile deployment manager.
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

#include <sstream>

bool	PhEngine::UpdateProjectileDeployment(
		GameMap*	MainMap,
		double		DeltaTime)
{
	/*
	 * When the certain times have reached, it seems sure that we would have to deploy
	 * these projectiles and send other entities away farther from the projectiles.
	 * If you set the blast radius too long and the blast power too large, physics engine
	 * simulation may occur problems (i.e. You will be blasted through walls).
	 */
	std::set<int>		PendRender;
	for (auto Itert : MainMap->PlayerEntityList)
	if (Itert.second->DataIntact()) {
		int ChnkNum = GetEntityChunkNum(Itert.second->Physics.PosX);
		for (int i = - GameConfig.ViewDistance; i <= GameConfig.ViewDistance; i++)
			PendRender.insert(ChnkNum + i);
	}
	for (auto ChnkItrt : PendRender) {
		Chunk*	RendChnk = MainMap->ChunkList[ChnkItrt];
		if (!RendChnk) continue;
		for (auto itert : RendChnk->ProjectileEntityList) {
			Entity*					Object = itert.second;
			EntityType*				ObjTyp;
			ProjectileEntityType*	ProjTyp;
//			Defining some types and exiting if necessary
			if (!Object->DataIntact()) throw NullPointerException();
			ObjTyp = Object->Properties.Type;
			if (ObjTyp->Properties.Type != "Projectile") continue;
			ProjTyp = (ProjectileEntityType*)ObjTyp->Properties.SpecificProperties;
//			Detecting time settings and impose triggers
			for (Trigger* trig : Object->Physics.TriggerList)
				if (trig->PreliminaryAction == "BeforeDestruction") {
					std::stringstream	Stream;
					double	PredetectTime = ProjTyp->DeployDelay;
					if (trig->PreliminaryObject.size() == 1) {
						Stream << trig->PreliminaryObject[0];
						Stream >> PredetectTime;
					}
					if (MainMap->CurTime - Object->Properties.GenTime > ProjTyp->
							DeployDelay - PredetectTime)
						trig->ProcessConsequence(std::vector<void*>({
							(void*)MainMap, (void*)Object}));
					break;
				}
			if (MainMap->CurTime - Object->Properties.GenTime < ProjTyp->DeployDelay)
				continue;
			/*
			 * Now the projectile has all properties defined, then we should take
			 * care about what is about to happen next. All the objects / entities
			 * that the projectile literally destructed, will return to the player
			 * who created the projectile's pocket.
			 */
			std::string	SentPlyrNam = Object->Properties.Owner;
			Entity*		SentPlyrEnt = NULL;
			for (auto itert : MainMap->PlayerEntityList)
				if (itert.second->DataIntact() &&
						itert.second->Properties.Name == SentPlyrNam) {
					SentPlyrEnt = itert.second;
					break;
				}
			/*
			 * For every entity in the world / chunk, we assume that it is in simple state that
			 * it might be injected or blasted away. We call him the victim. The victim is thrown
			 * away if and only if:
			 * 1. Be in a length of less than projectileDeployRadius.
			 * 2. In physics simulation.
			 * 3. Allow collision (This might look strange...)
			 */
			for (auto ChnkJtrt : PendRender) {
				Chunk*	ChkChnk = MainMap->ChunkList[ChnkJtrt];
				if (!ChkChnk) continue;
				for (auto jtert : ChkChnk->EntityList) {
					Entity*	Victim = jtert.second;
					if (!Victim->DataIntact()) continue;
					if (Victim == Object) continue; // That's so weird...
//					Calculate the distance and define whether to process
					double	distX = Victim->Physics.PosX - Object->Physics.PosX;
					double	distY = Victim->Physics.PosY - Object->Physics.PosY;
					double	distCombine = distX * distX + distY * distY;
					bool	victMovable = Victim->PhysicsEnabled() && Victim->CollisionEnabled();
//					Square root calculations cost much more time than multiplication...
//					This is the power of constant optimisation.
					if (distCombine > ProjTyp->DeployRadius * ProjTyp->DeployRadius)
						continue;
//					A performance worker... Use this to ensure working speed when using rockets.
					distCombine = sqrt(distCombine);
//					If it is movable, then shall we change its velocity.
//					To avoid moving too fast, we apply a 10% factor to the speed adjustment
					if (victMovable && ProjTyp->DeployRadius != 0.0) {
						Victim->Physics.VelX += ProjTyp->DeployPowerMotion * (ProjTyp->DeployRadius -
								distCombine) / ProjTyp->DeployRadius * distX / distCombine * 0.1;
						Victim->Physics.VelY += ProjTyp->DeployPowerMotion * (ProjTyp->DeployRadius -
								distCombine) / ProjTyp->DeployRadius * distY / distCombine * 0.1;
					}
//					But in any case it should have a blast resistance checker.
//					As in multiplayer, one projectile need not and should not be deployed
//					twice or many times depending on the joined players. Therefore we
//					strictly affirm that such restrictions must be made so that one
//					removal simulations are only done on the server.
					if (Victim->Properties.Type->Physics.BlastResistance < ProjTyp->DeployPowerBlast *
							(ProjTyp->DeployRadius - distCombine) / ProjTyp->DeployRadius &&
							Victim->Properties.Type->Properties.Type != "Player" &&
							Victim->Properties.Type->Properties.Type != "Particle") {
//						Inventory of the set player should be filled with this item
						if (SentPlyrEnt->DataIntact()) {
							bool			SentPlyrAdded = false;
							PlayerEntity*	SentPlyrExt = (PlayerEntity*)SentPlyrEnt->Physics.ExtendedTags;
							for (auto itert_pair = SentPlyrExt->Inventory.begin();
									itert_pair != SentPlyrExt->Inventory.end();
									itert_pair++) {
								EntityType*	EntTyp = itert_pair->first;
								if (!EntTyp->DataIntact()) continue;
								if (EntTyp != Victim->Properties.Type) continue;
								itert_pair->third++;
								SentPlyrAdded = true;
								break;
							}
							if (!SentPlyrAdded) {
								EntityType*	EntTyp = Victim->Properties.Type;
								SentPlyrExt->Inventory.push_back(make_triple_pair(EntTyp, 0, 1));
							}
						}
//						Now to pend the removal of this entity
						if (MainMap->IsHost) {
							MainMap->RemoveEntityPended(Victim);
							NetmgrRemoveEntity(Victim);
						}
					}
//					And in some particular cases it would make sure that if, the victims
//					were players, it would certainly deduct life from the players
					if (Victim->Properties.Type->Properties.Type == "Player") {
						PlayerEntity*	VicEnt = (PlayerEntity*)Victim->Physics.ExtendedTags;
						if (!VicEnt) throw NullPointerException();
						if (!VicEnt->IsCreative && Victim->Properties.Name != "__ZwDefaultEntity7Player") {
							VicEnt->Life -= ProjTyp->DeployPowerDamage * (ProjTyp->DeployRadius -
								distCombine) / ProjTyp->DeployRadius;
							if (Object->Properties.Owner == GameConfig.PlayerName)
								if (MainMap->IsHost)
									NetmgrSetEntityLife(Victim);
						}
					}
				}
			}
//			Hither does not need only server simulations. This is to keep the
//			area clean and tidy - no muss, no fuss.
			MainMap->RemoveEntityPended(Object);
			NetmgrRemoveEntity(Object);
			continue;
		}
	}
	return true;
}
