
//	"engine.cpp" -*- C++ -*-
//	Created on: 5 March 2016
//	Physics engine routine dispatcher.
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

namespace PhEngine
{
	PhEngineStateType	PhEngineState;
	std::mutex			PhEngineRequireWait;
}

void	PhysicsThread(
		GameMap*	MainMap)
{
//	Basic definitions and requirements
	double	CurTime = 0.0,
			LastTime = - 0.1,
			DeltaTime = 0.1;
	InputControl.CameraX = 0.0;
	InputControl.CameraY = 0.0;
//	Find player position
	MainMap->CommitPendedChanges();
	InputControl.PlayerGuid = 0;
	for (auto itert : MainMap->PlayerEntityList)
		if (itert.second->DataIntact() &&
				itert.second->Properties.Type &&
				itert.second->Properties.Type->Properties.Type == "Player" &&
				itert.second->Properties.Name == GameConfig.PlayerName) {
			InputControl.PlayerGuid = itert.second->Properties.Guid;
			break;
		}
	if (InputControl.PlayerGuid == 0) {
		Entity*	newEnt = MainMap->CreatePlayer(GameConfig.PlayerName);
		InputControl.PlayerGuid = newEnt->Properties.Guid;
		NetmgrInsertEntity(newEnt);
	}
//	To announce the injection of this very information.
	std::string	PostMsg = GameConfig.PlayerName + " joined the game.";
	if (MainMap->IsHost)
		chatInsertMessage(PostMsg);
	NetmgrPostMessage(PostMsg);
//	The main loop for physics engine
	MainMap->LockMemory();
	MainMap->UnlockMemory();
	if (MainMap->IsHost && GameConfig.OpenToLan) {
		BeginServerNetworkManager(MainMap);
	}
//	Initial settings for player
	MainMap->LockMemory();
	Entity*					playerEntity;
	PlayerEntity*			playerExt;
	std::set<long long>	nulRmList;
	nulRmList.clear();
	for (auto itert : MainMap->EntityList) {
//		std::cout << itert.second << "\n";
		if (!itert.second->DataIntact())
			nulRmList.insert(itert.first);
	}
	for (auto itert : nulRmList)
		MainMap->RemoveEntity(itert);
	playerEntity = MainMap->EntityList[InputControl.PlayerGuid];
	if (!playerEntity->DataIntact())
		throw NullPointerException();
	playerExt = (PlayerEntity*)playerEntity->Physics.ExtendedTags;
	CurTime = GetProcessTime();
	playerExt->LastProjectileTime = CurTime + 2.0;
	MainMap->UnlockMemory();
	PhEngine::PhEngineRequireWait.unlock();
	while (true) {
//		Terminate simulation engine if told to do so
		if (PhEngine::PhEngineState != PhEngine::Running &&
				PhEngine::PhEngineState != PhEngine::Paused &&
				PhEngine::PhEngineState != PhEngine::InventoryOpen)
			break;
		LastTime = CurTime;
		CurTime = GetProcessTime();
		DeltaTime = CurTime - LastTime;
		MainMap->LockMemory();
		MainMap->CurTime = CurTime;
		PhEngine::RenderPhysicsFrame(MainMap, DeltaTime);
		if (PhEngine::PhEngineState == PhEngine::Running) {
			PhEngine::UpdateCameraPosition(MainMap, DeltaTime, true);
			PhEngine::UpdatePlayerActivity(MainMap, DeltaTime);
			PhEngine::UpdateProjectileSummon(MainMap, DeltaTime);
		} else {
			PhEngine::UpdateCameraPosition(MainMap, DeltaTime, false);
		}
		PhEngine::UpdateProjectileDeployment(MainMap, DeltaTime);
		PhEngine::UpdateParticleDestruction(MainMap, DeltaTime);
		PhEngine::UpdatePlayerLife(MainMap, DeltaTime);
		MainMap->CommitPendedChanges();
		MainMap->UnlockMemory();
		SleepFromTime(CurTime, 0.015);
	}
//	Committing changes and leaving
	if (MainMap->IsHost && GameConfig.OpenToLan)
		EndServerNetworkManager();
	else if (!MainMap->IsHost)
		EndClientNetworkManager();
	SleepForTime(1.0);
	if (PhEngine::PhEngineState == PhEngine::SaveExit)
		MainMap->ExportToJson();
	PhEngine::PhEngineState = PhEngine::Stopped;
	MainMap->Clear();
	PhEngine::PhEngineRequireWait.unlock();
	return ;
}

bool	BeginPhysicsEngine(
		GameMap*	MainMap)
{
	PhEngine::PhEngineState = PhEngine::Running;
	PhEngine::PhEngineRequireWait.try_lock();
	std::thread*	hThread = new std::thread(PhysicsThread, MainMap);
	eclogPost(1, "Physics ENGINE started");
	return hThread != NULL;
}

bool	TerminatePhysicsEngine(
		void)
{
	if (PhEngine::PhEngineState == PhEngine::Stopped)
		return true;
	PhEngine::PhEngineState = PhEngine::Stopped;
	WaitForEngineEvent();
	eclogPost(1, "Physics ENGINE terminated");
	return true;
}

bool	WaitForEngineEvent(
		void)
{
	PhEngine::PhEngineRequireWait.lock();
	PhEngine::PhEngineRequireWait.unlock();
	PhEngine::PhEngineRequireWait.try_lock();
	return true;
}
