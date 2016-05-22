
//	"netsync.cpp" -*- C++ -*-
//	Created on: 14 May 2016
//	Definitions of game network manager.
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

#include "network/netsync.h"

#include <iomanip>
#include <map>
#include <mutex>
#include <queue>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <set>
#include <sstream>
#include <thread>

#include "native/inputctrl.h"
#include "network/chat.h"
#include "network/deflate.h"

enum	tMgrState { Undefined, Server, Client } MgrState;
tusnilDirectedNode*		MainSock;
std::thread				NetThreadIn, NetThreadOut;
std::queue<std::string>	MsgQueue;
std::mutex				MsgLock;
extern	InputCtrlType	InputControl;
bool					flagDatRequireBroadcast;
std::mutex				flagClnChunkLoadComplete;
std::map<long long, double>	flagLastPlayerMsgTime;

static	rapidjson::Document		JConfig;
static	rapidjson::Document::
		AllocatorType&			JAlloc = JConfig.GetAllocator();
static	double					SvrTimeAhead = 0.0;

std::string	NetmgrGetEntityPositionStr(
		Entity*		MvEnt)
{
	std::stringstream	Stream;
	std::string			OutStr;
	Stream << std::fixed << std::setprecision(8);
	Stream << "MOV "
		   << MvEnt->Properties.Guid << ' '
		   << MvEnt->Properties.Layer << ' '
		   << MvEnt->Physics.PosX << ' '
		   << MvEnt->Physics.PosY << ' '
		   << MvEnt->Physics.VelX << ' '
		   << MvEnt->Physics.VelY;
	getline(Stream, OutStr);
	return OutStr;
}

std::string	NetmgrGetEntityDefinitionStr(
		Entity*		OutEnt)
{
	JConfig.Clear();
	JConfig.SetObject();
	JConfig.AddMember("Data", rapidjson::Value(rapidjson::kObjectType), JAlloc);
//	Synchronisation needed...
	OutEnt->Properties.GenTime += SvrTimeAhead;
	OutEnt->ExportToJson(JConfig["Data"], JAlloc, true);
	OutEnt->Properties.GenTime -= SvrTimeAhead;
	rapidjson::StringBuffer		JBuffer;
	rapidjson::Writer<
		rapidjson::StringBuffer>JWriter(JBuffer);
	JConfig.Accept(JWriter);
	std::string			Result;
	std::stringstream	Stream;
	Stream << OutEnt->Properties.Guid;
	Stream >> Result;
	Result = (std::string)"DEF " + Result + " " + JBuffer.GetString();
//	Avoid memory leaks... this was a problem
	JConfig.Clear();
	JBuffer.Clear();
	return Result;
}

std::string	NetmgrGetEntityLifeStr(
		Entity*		OutEnt)
{
	if (!OutEnt->DataIntact() || OutEnt->Properties.
			Type->Properties.Type != "Player")
		return "NUL 0";
	std::stringstream	Stream;
	std::string			Result;
	Stream << std::fixed << std::setprecision(8)
		   << "LIF " << OutEnt->Properties.Guid << ((PlayerEntity*)
			(OutEnt->Physics.ExtendedTags))->Life;
	getline(Stream, Result);
	return Result;
}

//	Network protocol definition
//	NUL 0 (This message does nothing at all!)
//	DON	0 (Tell newly joined clients that chunks have been loaded)
//	TIM 1678.635469 (Broadcast server time, server only)
//	REM 123456789012 (Remove entity)
//	MOV 123456789012 7 0.5456 0.879878 1.051 1.999 (Set position, velocity)
//	DEF 123456789012 {"Data":{...JSON...}} (Define entity data)
//	HED {...JSON...} (Define map headers, used only once)
//	LIF 123456789012 168.36 (Define player life)
//	MSG	Hello, world (Broadcast a chat message)

void	NetmgrInThread(
		GameMap*	MainMap)
{
	double	CurTime = 0.0;
	SvrTimeAhead = 0.0;
#ifdef NETWORK_NETSYNC_DELETE_UNSYNCED_ENTITIES_
	std::set<Entity*>	EntBuffer;
#endif
	while (MainSock->Connected()) {
		std::string			RecvMsg, OpType, OpCmd;
		std::stringstream	Stream;
		Entity*				thisPlayer = MainMap->
				PlayerEntityList[InputControl.PlayerGuid];
		CurTime = GetProcessTime();
		MainSock->Receive(RecvMsg);
		gameProtocolDeflate(RecvMsg);
		Stream << RecvMsg;
#ifdef NETWORK_NETSYNC_DELETE_UNSYNCED_ENTITIES_
		EntBuffer.clear();
#endif
		MainMap->LockMemory();
		while (getline(Stream, RecvMsg)) {
			if (RecvMsg.length() < 3) continue;
			OpType = OpCmd = "";
			for (unsigned int i = 0; i < 3; i++) OpType += RecvMsg[i];
			for (unsigned int i = 4; i < RecvMsg.length(); i++) OpCmd += RecvMsg[i];
//			std::cout << OpType << ' ' << OpCmd << "\n" << std::flush;
			if (OpType == "DON") {
//				Flush chunk data, in one time
				if (MainMap->IsHost) continue; // Which should not have happened
#ifdef NETWORK_NETSYNC_DELETE_UNSYNCED_ENTITIES_
				for (auto itert : MainMap->EntityList) {
					if (EntBuffer.find(itert.second) == EntBuffer.end())
						MainMap->RemoveEntityPended(itert.second);
				}
#endif
				break;
			} else if (OpType == "TIM") {
				std::stringstream	Stream;
				Stream << OpCmd; Stream >> SvrTimeAhead;
//				Time synchronisation
				SvrTimeAhead -= CurTime;
			} else if (OpType == "REM") {
				std::stringstream	Stream;
				long long	RdGuid = 0;
				Entity*		RmEnt = NULL;
				Stream << OpCmd; Stream >> RdGuid;
				if (RdGuid == 0) continue;
				RmEnt = MainMap->EntityList[RdGuid];
//				If the entity already existed, then we shall remove it
				if (RmEnt->DataIntact()) {
					if (RmEnt->Properties.Type->Properties.Type != "Projectile")
						MainMap->RemoveEntityPended(RmEnt);
				}
			} else if (OpType == "MOV") {
				std::stringstream	Stream;
				Stream << OpCmd;
				long long	RdGuid = 0;
				int			lyr = 0;
				double		sx = 0, sy = 0, vx = 0, vy = 0;
				Entity*		MvEnt = NULL;
				EntityType*	EntTyp = NULL;
				Stream >> RdGuid >> lyr >> sx >> sy >> vx >> vy;
				if (RdGuid == 0) continue;
				MvEnt = MainMap->EntityList[RdGuid];
//				If the entity exists, then we shall update its data
				if (MvEnt->DataIntact() && RdGuid != InputControl.PlayerGuid) {
					MvEnt->Physics.VelX = vx;
					MvEnt->Physics.VelY = vy;
					MainMap->InsertEntityPended(MvEnt, lyr, sx, sy);
					EntTyp = MvEnt->Properties.Type;
//					If it is a player entity, we regard it as correct
					if (EntTyp->Properties.Type == "Player")
						flagLastPlayerMsgTime[RdGuid] = CurTime;
				}
			} else if (OpType == "DEF") {
				rapidjson::Document	JConfig;
				std::stringstream	Stream;
				Entity*				DefEnt = NULL;
				long long			RdGuid = 0;
				Stream << OpCmd; Stream >> RdGuid;
				if (RdGuid == 0) continue;
				getline(Stream, OpCmd);
				JConfig.Parse(OpCmd.c_str());
				DefEnt = MainMap->EntityList[RdGuid];
				if (!DefEnt->DataIntact()) {
					DefEnt = new Entity;
				} else {
					if (thisPlayer->DataIntact() &&
							DefEnt->Properties.Owner == thisPlayer->Properties.Name)
						continue;
					MainMap->RemoveEntity(DefEnt);
				}
//				If it was yet another unknown entity, ignore it.
				if (DefEnt->ImportFromJson(JConfig["Data"], true)) {
					DefEnt->Properties.GenTime -= SvrTimeAhead;
					MainMap->InsertEntity(DefEnt);
#ifdef NETWORK_NETSYNC_DELETE_UNSYNCED_ENTITIES_
					EntBuffer.insert(DefEnt);
#endif
				} else {
					std::cerr << "\nFAULTY JSON DATA!\n" << std::flush;
				}
//				Procedure complete!
			} else if (OpType == "HED") {
				MainMap->ImportHeaderFromJson(OpCmd);
			} else if (OpType == "LIF") {
				std::stringstream	Stream;
				long long			RdGuid;
				double				RdLife;
				Entity*				PlyEnt = NULL;
				Stream << OpCmd;
				Stream >> RdGuid >> RdLife;
				if (RdGuid == 0) continue;
				PlyEnt = MainMap->EntityList[RdGuid];
				if (!PlyEnt->DataIntact() || PlyEnt->Properties.
						Type->Properties.Type != "Player")
					continue;
				PlayerEntity*		PlyExt = (PlayerEntity*)PlyEnt->Physics.ExtendedTags;
				PlyExt->Life = RdLife;
			} else if (OpType == "MSG") {
				chatInsertMessage(OpCmd);
			}
//			The server has the responsibility to dispatch central information
//			to all connected clients.
			if (MainMap->IsHost)
				MsgQueue.push(RecvMsg);
		}
		MainMap->CommitPendedChanges();
		MainMap->UnlockMemory();
		if (OpType == "DON")
			flagClnChunkLoadComplete.unlock();
	}
	return ;
}

void	NetmgrOutThread(
		GameMap*	MainMap)
{
	bool	flagDatPhyBroadcasted = false;
	bool	flagDatChnBroadcasted = false;
	int		lastClientCount = 0, curClientCount = 0;
	double	valSleepTime = 0.015;
	flagDatRequireBroadcast = false;
	flagLastPlayerMsgTime.clear();
	std::string	Output;
	while (MainSock->Connected()) {
		SleepForTime(valSleepTime);
		Output.clear();
		MainMap->LockMemory();
		double		CurTime = GetProcessTime();
//		Broadcast server time
		if (MgrState == Server) {
			std::stringstream	Stream;
			Stream << std::fixed << std::setprecision(8) << CurTime;
			std::string	OutTime; Stream >> OutTime;
			OutTime = "TIM " + OutTime + "\n";
			Output += OutTime;
		}
//		If the players disconnect from the system, then remove them
		if (MgrState == Server)
		for (auto itert : MainMap->PlayerEntityList)
		if (itert.first != InputControl.PlayerGuid) {
			Entity*	plyEnt = itert.second;
			if (!plyEnt->DataIntact()
					|| plyEnt->Properties.Guid == InputControl.PlayerGuid)
				continue;
			PlayerEntity*	plyExt = (PlayerEntity*)plyEnt->Physics.ExtendedTags;
			if (CurTime - flagLastPlayerMsgTime[itert.first] > 1.6
					&& !plyEnt->Physics.RenderDisabled
					&& plyExt->Life > 0) {
				plyEnt->Physics.CollisionChanged = true;
				plyEnt->Physics.PhysicsChanged = true;
				plyEnt->Physics.RenderDisabled = true;
				NetmgrInsertEntity(plyEnt);
				std::string	PostMsg = plyEnt->Properties.Name + " left the game.";
				NetmgrPostMessage(PostMsg);
				chatInsertMessage(PostMsg);
			} else if (CurTime - flagLastPlayerMsgTime[itert.first] <= 1.6
					&& plyEnt->Physics.RenderDisabled
					&& plyExt->Life > 0) {
//				If a player joined the game, it should be immediately broadcasted
//				that the players are joined, such change their states
				plyEnt->Physics.CollisionChanged = false;
				plyEnt->Physics.PhysicsChanged = false;
				plyEnt->Physics.RenderDisabled = false;
				NetmgrInsertEntity(plyEnt);
			}
		}
//		Retrieve data from (unbuffered) message queue
		MsgLock.lock();
		while (!MsgQueue.empty()) {
			Output += MsgQueue.front() + "\n";
			MsgQueue.pop();
		}
		MsgLock.unlock();
//		Physics entity position and velocity is broadcasted every 0.15 seconds,
//		guaranteed that the manager runner is a server hosting the map
		Entity*	thisPlayer = MainMap->PlayerEntityList[InputControl.PlayerGuid];
		if (CurTime / 0.15 - (int)(CurTime / 0.15) <= 0.5) {
		if (!flagDatPhyBroadcasted && MgrState == Server) {
			flagDatPhyBroadcasted = true;
//			Uploading entity data
			if (thisPlayer->DataIntact())
			for (auto citert : MainMap->ChunkList)
			for (auto itert : citert.second->PhysicsEntityList)
			if (itert.second->DataIntact() && (itert.second->Properties.Owner
					== "" || itert.second->Properties.Owner == thisPlayer->Properties.Name)) {
				if (itert.second->Properties.Type->Properties.Type != "Player")
					Output += NetmgrGetEntityPositionStr(itert.second) + '\n';
				if (itert.second->Properties.Type->Properties.Type == "Player")
					Output += NetmgrGetEntityLifeStr(itert.second) + '\n';
			}
//			Finished physics entity position updates
		} else if (flagDatPhyBroadcasted && MgrState == Client) {
			flagDatPhyBroadcasted = true;
			if (thisPlayer->DataIntact())
			for (auto citert : MainMap->ChunkList)
			for (auto itert : citert.second->PhysicsEntityList)
			if (itert.second->DataIntact() && (itert.second->Properties.Owner
					== thisPlayer->Properties.Name)) {
				Output += NetmgrGetEntityPositionStr(itert.second) + '\n';
			}
		} } else {
			flagDatPhyBroadcasted = false;
		}
//		Update sleep time, such that when physical entities become much, it would
//		be impossible to get all the data through the network buffer.
		double	DeltaTime = GetProcessTime();
		DeltaTime -= CurTime;
//		TODO: it consumes a lot of CPU... I don't understand.
		valSleepTime = std::max(0.03, DeltaTime * 16.0);
//		For a client, if it doesn't broadcast its position, it would be regarded
//		disconnected by the server. So every second (The tolerance is 4.2s) we
//		should announce our position to the server.
		if (CurTime / 1.0 - (int)(CurTime / 1.0) <= 0.5) {
		if (!flagDatPhyBroadcasted) {
			flagDatPhyBroadcasted = true;
			Entity*	locEnt = MainMap->EntityList[InputControl.PlayerGuid];
			if (locEnt != NULL)
				Output += NetmgrGetEntityPositionStr(locEnt) + '\n';
		} } else {
			flagDatPhyBroadcasted = false;
		}
//		Chunk data is broadcasted every 10 seconds, guaranteed that the manager
//		runner is a server hosting the map
		if (flagDatRequireBroadcast) {
		if (!flagDatChnBroadcasted && MgrState == Server) {
			flagDatChnBroadcasted = true;
//			Uploading map header data
			std::string	HeadOut = "";
			MainMap->ExportToJson(false, HeadOut);
			Output += "HED " + HeadOut + "\n";
			for (auto itert : MainMap->EntityList) {
				Entity*	OutEnt = itert.second;
				if (!OutEnt->DataIntact())
					continue;
				Output += NetmgrGetEntityDefinitionStr(OutEnt) + "\n";
			}
			Output += "DON 0\n";
		} } else {
			flagDatChnBroadcasted = false;
		}
		MainMap->UnlockMemory();
//		Define whether required to broadcast chunk data
		curClientCount = MainSock->GetClientCount();
		if (curClientCount > lastClientCount) {
			flagDatRequireBroadcast = true;
			flagDatChnBroadcasted = false;
		} else if (CurTime / 60 - (int)(CurTime / 60) <= 0.5) {
			flagDatRequireBroadcast = true;
		} else {
			flagDatRequireBroadcast = false;
		}
		lastClientCount = curClientCount;
//		Update changes of output
		gameProtocolInflate(Output);
		MainSock->Send(Output);
		continue;
	}
	return ;
}

bool	BeginServerNetworkManager(
		GameMap*	MainMap)
{
	if (!MainMap)
		return false;
	MainSock = new tusnilDirectedNode;
	if (!MainSock->SetAddress("Any", 4689))
		return false;
	if (!MainSock->CreateServer(64))
		return false;
	MgrState = Server;
	NetThreadIn = std::thread(NetmgrInThread, MainMap);
	NetThreadOut = std::thread(NetmgrOutThread, MainMap);
	return true;
}

bool	BeginClientNetworkManager(
		std::string	TargetAddr,
		GameMap*	MainMap)
{
	if (!MainMap)
		return false;
	MainSock = new tusnilDirectedNode;
	if (!MainSock->SetAddress(TargetAddr, 4689))
		return false;
	if (!MainSock->ConnectServer())
		return false;
	MgrState = Client;
	flagClnChunkLoadComplete.unlock();
	flagClnChunkLoadComplete.lock();
	NetThreadIn = std::thread(NetmgrInThread, MainMap);
	NetThreadOut = std::thread(NetmgrOutThread, MainMap);
	return true;
}

bool	EndServerNetworkManager(
		void)
{
	if (MgrState == Undefined)
		return true;
	MainSock->Close();
	MgrState = Undefined;
	if (NetThreadIn.joinable())
		NetThreadIn.join();
	if (NetThreadOut.joinable())
		NetThreadOut.join();
	delete MainSock;
	return true;
}

bool	EndClientNetworkManager(
		void)
{
	if (MgrState == Undefined)
		return true;
	MainSock->Close();
	MgrState = Undefined;
	if (NetThreadIn.joinable())
		NetThreadIn.join();
	if (NetThreadOut.joinable())
		NetThreadOut.join();
	delete MainSock;
	return true;
}

bool	NetmgrInsertEntity(
		Entity*	InsEnt)
{
	std::string	Str = NetmgrGetEntityDefinitionStr(InsEnt);
	MsgLock.lock();
	MsgQueue.push(Str);
	MsgLock.unlock();
	return true;
}

bool	NetmgrRemoveEntity(
		Entity*	RmEnt)
{
	std::stringstream	Stream;
	std::string			Str;
	if (RmEnt->Properties.Type->Properties.Type == "Projectile")
		return true;
	Stream << "REM " << RmEnt->Properties.Guid;
	getline(Stream, Str);
	MsgLock.lock();
	MsgQueue.push(Str);
	MsgLock.unlock();
	return true;
}

bool	NetmgrMoveEntity(
		Entity*	MvEnt)
{
	std::string	Str = NetmgrGetEntityPositionStr(MvEnt);
	MsgLock.lock();
	MsgQueue.push(Str);
	MsgLock.unlock();
	return true;
}

bool	NetmgrPostMessage(
		std::string	Msg)
{
	std::string	Str = "MSG " + Msg;
	MsgLock.lock();
	MsgQueue.push(Str);
	MsgLock.unlock();
	return true;
}

bool	NetmgrSetEntityLife(
		Entity*	SetEnt)
{
	std::string	Str = NetmgrGetEntityLifeStr(SetEnt);
	MsgLock.lock();
	MsgQueue.push(Str);
	MsgLock.unlock();
	return true;
}

bool	NetmgrQueryLoadComplete(
		void)
{
	if (MgrState != Client)
		return true;
	flagClnChunkLoadComplete.lock();
	flagClnChunkLoadComplete.unlock();
	return true;
}
