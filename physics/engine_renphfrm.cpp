
//	"engine_renphfrm.cpp" -*- C++ -*-
//	Created on: 28 April 2016
//	Physics engine / Frame simulation renderer.
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

#include "algo/triple_pair.h"

bool	PhEngine::RenderPhysicsFrame(
		GameMap*	MainMap,
		double		DeltaTime)
{
	if (MainMap == NULL)
		throw NullPointerException();
//	Too long a time delta makes trouble.
	if (DeltaTime >= 0.42)
		return false;
//	Render physics simulation at everywhere where players exist.
	std::set<int>			PendRender;
#ifdef PHYSICS_CHUNK_USE_RED_BLACK_TREE_ALGORITHM_
	std::set<Entity*>		PendCollision;
#endif
	for (auto Itert : MainMap->PlayerEntityList)
	if (Itert.second->DataIntact()) {
		int ChnkNum = GetEntityChunkNum(Itert.second->Physics.PosX);
		for (int i = - GameConfig.ViewDistance; i <= GameConfig.ViewDistance; i++)
			if (MainMap->ChunkList[ChnkNum + i] != NULL)
				PendRender.insert(ChnkNum + i);
	}
//	Iterate through all prepared chunks, and, individually, render their
//	physics states. This procedure should be previously locked by the
//	mutex by the PhysicsThread. Otherwise, runtime errors will happen.
	for (auto PRChunkItertA : PendRender)
	for (auto ItertObject : MainMap->ChunkList[PRChunkItertA]->PhysicsEntityList) {
		Entity*		Object = ItertObject.second;
		if (!Object->DataIntact()) continue;
		EntityType*	ObjType = Object->Properties.Type;
//		For ease of coding and reduction of code length, we define the following variables
		double	sX = Object->Physics.PosX, sY = Object->Physics.PosY,
				vX = Object->Physics.VelX, vY = Object->Physics.VelY,
				lX = Object->Properties.Type->Physics.LengthX, lY = Object->Properties.Type->Physics.LengthY;
		double	mA, mB, vcX, vcY; // The two mass, the velocity of CollideObject
		double	dT = DeltaTime;
		double	g = MainMap->GravityConst; // As written...
		double	lB = sX - lX / 2, rB = sX + lX / 2,
				uB = sY + lY / 2, dB = sY - lY / 2; // Boundaries of Object
		double	lcB, rcB, ucB, dcB; // Boundaries of CollideObject (introduced later)
		bool	GravityReversed = ObjType->Physics.Mass < 0;
		bool	hasCollidedBottom = false;
		int		objChunk, nobjChunk; // Chunk position
		double	obj_move_dist_sqr = 0.0, cobj_dist_sqr = 0.0;
//		A specialty on calculating gravity interaction
		if (GravityReversed)
			vY -= ObjType->Physics.Mass * dT;
		else
			vY -= ObjType->Physics.Mass > 0 ? g * dT : 0.0;
//		If this object has been marked to ignore simulation, we follow its deeds
		if (!Object->PhysicsEnabled()) continue;
//		Calculate the move distance of the object
		obj_move_dist_sqr = (vX * vX + vY * vY) * dT * dT;
//		Pre-process potentially colliding objects in pended rendering chunks. All
//		potentials will be sent to an indexer, defining the potentially correct object.
//		It appears that using the O(nlogn) method is way slower than the O(n^2) method...
#ifdef PHYSICS_CHUNK_USE_RED_BLACK_TREE_ALGORITHM_
		PendCollision.clear();
		for (auto PRChunkItertB = PendRender.begin(); PRChunkItertB != PendRender.end(); PRChunkItertB++)
			MainMap->ChunkList[*PRChunkItertB]->QueryAdjacentEntities(Object, 2.0, PendCollision);
		for (auto ItertCollideObject = PendCollision.begin(); ItertCollideObject != PendCollision.end(); ItertCollideObject++) {
#else
		if (Object->CollisionEnabled())
		for (int PRChunkItertB : {PRChunkItertA - 1, PRChunkItertA, PRChunkItertA + 1})
		for (auto ItertCollideObject : (MainMap->ChunkList[PRChunkItertB])->EntityList) {
#endif
			Entity*		CollideObject = ItertCollideObject.second;
			EntityType*	CollideType = NULL;
			int			CollisionSide = 0; // 1 left 2 right 3 up 4 down ( of CollideObject )
			int			RevCollisionSide = 0; // From the other entity's view ( of Object)
			if (!CollideObject->DataIntact()) continue;
			CollideType = CollideObject->Properties.Type;
//			Some extra precautions we need to take care of, in order to reduce simulation burden.
			if (CollideObject == Object) continue;
			if (!CollideObject->DataIntact()) continue;
			if (!CollideObject->CollisionEnabled(Object)) continue;
			if (CollideObject->Properties.Layer != Object->Properties.Layer) continue;
//			Define numbers, for ease of coding and shorten of code length.
			mA = ObjType->Physics.Mass,
			mB = CollideType->Physics.Mass;
			vcX = CollideObject->Physics.VelX,
			vcY = CollideObject->Physics.VelY;
			lcB = CollideObject->Physics.PosX - CollideObject->Properties.Type->Physics.LengthX / 2;
			rcB = CollideObject->Physics.PosX + CollideObject->Properties.Type->Physics.LengthX / 2;
			ucB = CollideObject->Physics.PosY + CollideObject->Properties.Type->Physics.LengthY / 2;
			dcB = CollideObject->Physics.PosY - CollideObject->Properties.Type->Physics.LengthY / 2;
			cobj_dist_sqr = (lcB - lB) * (lcB - lB) + (ucB - uB) * (ucB - uB);
//			Too distant, so that collision made impossible
			if (cobj_dist_sqr * 2 < obj_move_dist_sqr
					&& cobj_dist_sqr > 1.0) continue;
//			Calculate collision side by deterministic computation
//			This area of code might seem quite awkward, but by simply drafting on a
//			piece of paper, you will soon understand the basis of this snippet.
#define my_have_union(_al,_ar,_bl,_br) (!((_al)>(_br)||(_ar)<(_bl)))
#define calc_union_len(_al,_ar,_bl,_br) ((_al<_bl&&_ar>_br)?_br-_bl:((_al>_bl&&_ar<_br)?_ar-_al:(_ar<_br&&_ar>_bl?_ar-_bl:(_al>_bl&&_al<_br?_br-_al:0))))
			double	tlrnc = 0.01; // Tolerance for collision detection.
			double	tlrncpre = std::min(CollideObject->Properties.Type->Physics.LengthX,
					CollideObject->Properties.Type->Physics.LengthY) * 0.15; // Tolerance for pre-detection (before...)
			double	skpvl = 0.01; // The length to move the entity
//			Motion-based collision detection
			if (!CollisionSide &&
					my_have_union(lB + (dB - ucB) / vY * vX,
					rB + (dB - ucB) / vY * vX, lcB, rcB)) {
				if (dB > ucB - tlrncpre && dB + vY * dT <= ucB + tlrnc)
					CollisionSide = 3;
			}
			if (!CollisionSide &&
					my_have_union(lB + (dcB - uB) / vY * vX,
					rB + (dcB - uB) / vY * vX, lcB, rcB)) {
				if (uB < dcB + tlrncpre && uB + vY * dT >= dcB - tlrnc)
					CollisionSide = 4;
			}
			if (!CollisionSide &&
					my_have_union(dB + (lcB - rB) / vX * vY,
					uB + (lcB - rB) / vX * vY, dcB + tlrnc, ucB - tlrnc)) {
				if (rB < lcB + tlrncpre && rB + vX * dT >= lcB - tlrnc)
					CollisionSide = 1;
			}
			if (!CollisionSide &&
					my_have_union(dB + (lB - rcB) / vX * vY,
					uB + (lB - rcB) / vX * vY, dcB + tlrnc, ucB - tlrnc)) {
				if (lB > rcB - tlrncpre && lB + vX * dT <= rcB + tlrnc)
					CollisionSide = 2;
			}
//			Overlap-based collision detection
			if (CollisionSide == 0 &&
					my_have_union(lB + tlrnc, rB - tlrnc, lcB + tlrnc, rcB - tlrnc) &&
					my_have_union(dB + tlrnc, uB - tlrnc, dcB + tlrnc, ucB - tlrnc)) {
				double	Xdis = 0.0, Ydis = 0.0;
				Xdis = calc_union_len(lB + tlrnc, rB - tlrnc, lcB + tlrnc, rcB - tlrnc);
				Ydis = calc_union_len(dB + tlrnc, uB - tlrnc, dcB + tlrnc, ucB - tlrnc);
				if (Xdis < Ydis)
					CollisionSide = lcB + tlrnc > lB + tlrnc ? 1 : 2;
				else
					CollisionSide = dcB + tlrnc > dB + tlrnc ? 4 : 3;
			}
//			Eject the entities out of the way to prevent duplicate collisions
			switch (CollisionSide) {
				case 1: sX -= skpvl; RevCollisionSide = 2; break;
				case 2: sX += skpvl; RevCollisionSide = 1; break;
				case 3: sY += skpvl; RevCollisionSide = 4; break;
				case 4: sY -= skpvl; RevCollisionSide = 3; break;
				default: break;
			}
			if (CollisionSide == 0)
				continue;
			if ((CollisionSide == 3 && g >= 0) || (CollisionSide == 4 && g <= 0))
				hasCollidedBottom = true;
//			The player should not take no damage when colliding with great speed.
//			This is why we should do this, in order to make the players consider thrice
//			before they jump off a cliff.
			if (ObjType->Properties.Type == "Player") {
				PlayerEntity*		PlayerExt = (PlayerEntity*)
						Object->Physics.ExtendedTags;
				PlayerEntityType*	TypeExt = (PlayerEntityType*)
						ObjType->Properties.SpecificProperties;
				double	FallHeight = PlayerExt->LastJumpHeight - sY;
				double	AllowFallHeight = TypeExt->JumpSpeed * 2.5;
				if (FallHeight > AllowFallHeight &&
						(CollisionSide == 3 || CollisionSide == 4) &&
						(vY - vcY) * (vY - vcY) > 2 * g * TypeExt->JumpSpeed * 2.5 &&
						!PlayerExt->IsCreative)
					PlayerExt->Life -= (FallHeight - AllowFallHeight) *
						MainMap->RegenerationValue * 2.1;
//				Re-update, so can be used next time
				PlayerExt->LastJumpHeight = sY;
			}
//			Now, we shall have to process the results according to the entity type.
//			It shall be cleared that, this version of collision judging system should
//			have contained far fewer bugs than the older ones.
			if (!CollideObject->PhysicsEnabled()) {
				double	ratioFriction = (100.0 - ObjType->Physics.FrictionFactor[CollisionSide - 1]) / 100 *
						(100.0 - CollideType->Physics.FrictionFactor[RevCollisionSide - 1]) / 100;
				double	ratioElasticity = ObjType->Physics.ElasticFactor[CollisionSide - 1] *
						CollideType->Physics.ElasticFactor[RevCollisionSide - 1];
				if (CollisionSide == 3 || CollisionSide == 4) {
					vY = - vY;
					vX *= ratioFriction;
					vY *= ratioElasticity;
				} else if (CollisionSide == 1 || CollisionSide == 2) {
					vX = - vX;
					vX *= ratioElasticity;
					vY *= ratioFriction;
				}
			} else {
//#define my_safe_mass(_a) (abs(_a)!=0?abs(_a):1.0)
//				double	v1, v2, v1_, v2_, phi1, phi2, m1, m2;
//				double	a = 0, b = 0, c = 0, delta = 0;
//				m1 = my_safe_mass(mA), m2 = my_safe_mass(mB);
////				Calculate momentum on axis X
//				switch (CollisionSide) {
//					case 1: v1 = vX - vcX; v2 = vcX; break;
//					case 2: v1 = vX - vcX; v2 = vcX; break;
//					case 3: v1 = vY - vcY; v2 = vcY; break;
//					case 4: v1 = vY - vcY; v2 = vcY; break;
//					default: break;
//				}
//				v1_ = 0, v2_ = 0;
//				phi1 = 1.0 / ObjType->Physics.ElasticFactor[CollisionSide - 1];
//				phi2 = 1.0 / CollideType->Physics.ElasticFactor[RevCollisionSide - 1];
////				Equations are listed here in LaTeX:
////				m_1 v_1 = m_1 v_1^' + m_2 v_2^'
////				m_1 v_1^2 = m_1 v_1^{'2} {\phi}_1 + m_2 v_2^{'2} {\phi}_2
//				a = m2 * m2 * phi1 + m1 * m2 * phi2; if (a == 0) a = 1.0;
//				b = 2 * m1 * m2 * phi1 * v1;
//				c = m1 * m1 * v1 * v1 * (phi1 - 1);
//				delta = b * b - 4 * a * c; if (delta < 0) delta = 1.0;
//				v2_ = (-b + sqrt(delta) * (v1 > 0 ? 1 : -1)) / (2 * a);
//				v1_ = (m1 * v1 - m2 * v2_) / m1;
//				v2_ += v2, v1_ += v2;
//				switch (CollisionSide) {
//					case 1: vX = v2_; vcX = v1_; break;
//					case 2: vX = v2_; vcX = v1_; break;
//					case 3: vY = v2_; vcY = v1_; break;
//					case 4: vY = v2_; vcY = v1_; break;
//					default: break;
//				}
//				Momentum sum stays unchanged after collision
				double	pAX1 = vX * abs(mA), pAY1 = vY * abs(mA), // Begin-state momentum
						pBX1 = vcX * abs(mB), pBY1 = vcY * abs(mB);
				double	ratioElasticity = ObjType->Physics.ElasticFactor[CollisionSide - 1] *
						CollideType->Physics.ElasticFactor[RevCollisionSide - 1];
				double	pAX = pAX1, pAY = pAY1, pBX = pBX1, pBY = pBY1;
//				Now we shall consider the elasticity, which involves a sophisticated equation.
				if (0.0 < ratioElasticity && ratioElasticity < 1.0) {
					pAX = pAX1 * (1.0 - ratioElasticity) + pBX1 * ratioElasticity;
					pAY = pAY1 * (1.0 - ratioElasticity) + pBY1 * ratioElasticity;
					pBX = pBX1 * (1.0 - ratioElasticity) + pAX1 * ratioElasticity;
					pBY = pBY1 * (1.0 - ratioElasticity) + pAY1 * ratioElasticity;
				}
//				Return the velocity to the objects
#define my_safe_mass(_a) (abs(_a)!=0?abs(_a):1.0)
				vX = pAX / my_safe_mass(mA), vY = pAY / my_safe_mass(mA);
				vcX = pBX / my_safe_mass(mB), vcY = pBY / my_safe_mass(mB);
				CollideObject->Physics.VelX = vcX;
				CollideObject->Physics.VelY = vcY;
			}
//			Update triggers for this procedure
			for (Trigger*	trig : Object->Physics.TriggerList)
				if (trig->PreliminaryAction == "OnCollision")
					trig->ProcessConsequence(std::vector<void*>({
						(void*)MainMap, (void*)Object}));
			for (Trigger*	trig : CollideObject->Physics.TriggerList)
				if (trig->PreliminaryAction == "OnCollision")
					trig->ProcessConsequence(std::vector<void*>({
						(void*)MainMap, (void*)CollideObject}));
		}
		objChunk = GetEntityChunkNum(sX);
		sX += vX * dT, sY += vY * dT;
		if (Object->Properties.Type->Properties.Type == "Player") {
			PlayerEntity*	Confs = (PlayerEntity*)Object->Physics.ExtendedTags;
			if (hasCollidedBottom) Confs->LastJumpTime[1] = MainMap->CurTime;
			else Confs->LastJumpTime[0] = MainMap->CurTime;
			if (Confs->IsCreative)
				vY += GravityReversed ? ObjType->Physics.Mass * dT : g * dT;
		}
//		Simulate air friction
		vX *= 0.991;
		vY *= 0.998;
//		Red-black tree defect: must re-insert after updating related values
//		Also, the position update is strictly related to the red-black-tree
//		structure, therefore must be pended for later batch process.
		Object->Physics.VelX = vX, Object->Physics.VelY = vY;
//		Check whether physics enabled object needs to be put into its correct
//		value, a.k.a. to know if it is necessary to return it back within the
//		world boundaries. Wrapping is default on, that is, one should appear
//		from the other side of the world when out of the world
		if (sX < MainMap->WorldBoundary[0]) sX = MainMap->WorldBoundary[1] - 0.1;
		else if (sX > MainMap->WorldBoundary[1]) sX = MainMap->WorldBoundary[0] + 0.1;
		if (sY < MainMap->WorldBoundary[3]) sY = MainMap->WorldBoundary[2] - 0.1;
		else if (sY > MainMap->WorldBoundary[2]) sY = MainMap->WorldBoundary[3] + 0.1;
//		Re-inserting into new chunk (might not have changed...)
		nobjChunk = GetEntityChunkNum(sX);
		MainMap->CreateChunk(nobjChunk);
		if (nobjChunk != objChunk)
			MainMap->InsertEntityPended(Object, sX, sY);
		else {
			Object->Physics.PosX = sX;
			Object->Physics.PosY = sY;
		}
	}
	return true;
}
