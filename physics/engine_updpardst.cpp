
//	"engine_updpardst.cpp" -*- C++ -*-
//	Created on: 1 May 2016
//	Physics engine / Particle destruction manager.
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

bool	PhEngine::UpdateParticleDestruction(
		GameMap*	MainMap,
		double		DeltaTime)
{
	for (auto itert : MainMap->ParticleList) {
		Entity*				Object;
		EntityType*			ObjType;
		ParticleEntityType*	TypeExt;
		Object = itert.second;
		if (!Object->DataIntact()) continue;
		ObjType = Object->Properties.Type;
		TypeExt = (ParticleEntityType*)ObjType->Properties.SpecificProperties;
		if (MainMap->CurTime - Object->Properties.GenTime >
				TypeExt->LifeTime - 0.05) {
			MainMap->RemoveEntityPended(Object);
			NetmgrRemoveEntity(Object);
		}
	}
	return true;
}
