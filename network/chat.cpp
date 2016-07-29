
//	"chat.cpp" -*- C++ -*-
//	Created on: 22 May 2016
//	Definitions for in-game chat message manager.
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

#include "network/chat.h"

#include <mutex>
#include <queue>
#include <regex>

#include "native/time.h"

std::vector<std::string>	MessageList;
std::queue<std::string>		MessageQueue;
std::queue<double>			MessageQueueTimestamp;
std::mutex					MessageLock;

bool	chatInitialise(
		void)
{
#define my_def_msg(_x) MessageList.push_back(_x);
#define my_def_msg_cnt(_x,_y) for(int i=1;i<=_y;i++)my_def_msg(_x);
	my_def_msg_cnt( "$PLAYER$ blamed lag for that death" , 15);
	my_def_msg_cnt( "$PLAYER$ accidently ran into a lag" , 12);
	my_def_msg_cnt( "$PLAYER$ was way too slow for his enemy" , 12);
	my_def_msg_cnt( "$PLAYER$ seemed to run into a Blue Screen of Death" , 8);
	my_def_msg_cnt( "$PLAYER$ underestimated his opponent" , 8);
	my_def_msg_cnt( "$PLAYER$ did not like the Kernel Panic just now" , 6);
	my_def_msg_cnt( "$PLAYER$ asked NVIDIA to be faster" , 6);
	my_def_msg_cnt( "$PLAYER$ asked AMD to serve a faster FPS" , 6);
	my_def_msg_cnt( "$PLAYER$ blamed @ht35268 for using STL" , 3);
	my_def_msg_cnt( "$PLAYER$ blamed @ht35268 for brute force" , 3);
	my_def_msg_cnt( "$PLAYER$ pressed E" , 3);
	my_def_msg_cnt( "$PLAYER$ pressed T" , 3);
	my_def_msg_cnt( "$PLAYER$ faked that death for spawn point" , 2);
	my_def_msg_cnt( "$PLAYER$ fell from the sky to his end" , 2);
	my_def_msg_cnt( "$PLAYER$ tried to swim in homework" , 2);
	my_def_msg_cnt( "$PLAYER$ received his last blow" , 2);
	my_def_msg_cnt( "$PLAYER$ was blown up by an invisible creeper" , 2);
	my_def_msg_cnt( "$PLAYER$ was harvested" , 1);
	my_def_msg_cnt( "$PLAYER$ saw the emperor" , 1);
	my_def_msg_cnt( "$PLAYER$ has no quality (mass)" , 1);
	my_def_msg_cnt( "$PLAYER$'s M was too large" , 1);
	my_def_msg_cnt( "$PLAYER$ didn't read segment tree beats" , 1);
	eclogPost(1, "Done importing chat message templates");
	return true;
}

std::string	chatGenDeceaseMessage(
		std::string	Name)
{
//	Basic initializations
	std::string	sPattern = "^(.*)(\\$PLAYER\\$)(.*)$";
	std::string	sFormat = "$1" + Name + "$3";
	std::regex	sRegex(sPattern);
//	Matching and outputting
	int	lsSize = MessageList.size();
	int	lsIdx = rand() % lsSize;
	std::string	Output = MessageList[lsIdx];
	Output = std::regex_replace(Output, sRegex, sFormat);
	return Output;
}

bool	chatInsertMessage(
		std::string	Msg)
{
	MessageLock.lock();
	MessageQueue.push(Msg);
	MessageQueueTimestamp.push(
			GetProcessTimeUnsynced());
	MessageLock.unlock();
	eclogPost(1, "Chat message: " + Msg);
	return true;
}

bool	chatRetrieveMessageList(
		std::vector<std::string>&	List)
{
	MessageLock.lock();
	List.clear();
	while (!MessageQueue.empty()
			|| !MessageQueueTimestamp.empty()) {
		double	curTime = GetProcessTimeUnsynced();
		double	firstTime = MessageQueueTimestamp.front();
		if (curTime - firstTime >= 10.0) {
			MessageQueue.pop();
			MessageQueueTimestamp.pop();
		} else {
			break;
		}
	}
	std::queue<std::string> nmsgQueue;
	while (!MessageQueue.empty()) {
		List.push_back(MessageQueue.front());
		nmsgQueue.push(MessageQueue.front());
		MessageQueue.pop();
	}
	while (!nmsgQueue.empty()) {
		MessageQueue.push(nmsgQueue.front());
		nmsgQueue.pop();
	}
	MessageLock.unlock();
	return true;
}
