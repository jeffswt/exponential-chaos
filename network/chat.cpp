
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
	my_def_msg( "$PLAYER$ blamed lag for that death" );
	my_def_msg( "$PLAYER$ faked that death for money" );
	my_def_msg( "$PLAYER$ flipped the wall and drank tea." );
	my_def_msg( "$PLAYER$ flipped the wall and drank coffee." );
	my_def_msg( "$PLAYER$ pressed E" );
	my_def_msg( "$PLAYER$ says that BOOST is not a good library." );
	my_def_msg( "$PLAYER$ says that C++11 is not a good dialect." );
	my_def_msg( "$PLAYER$ says that C98 was not a good dialect." );
	my_def_msg( "$PLAYER$ urges developers to stop using std::map." );
	my_def_msg( "$PLAYER$ urges developers to stop using std::string." );
	my_def_msg( "$PLAYER$ urges developers to stop using STL." );
	my_def_msg( "$PLAYER$ was told to use Java." );
	my_def_msg( "$PLAYER$ was told to use CPython." );
	my_def_msg( "$PLAYER$ was told to use Dalvik instead of ART." );
	my_def_msg( "$PLAYER$ was given such a heavy shot." );
	my_def_msg( "$PLAYER$ was dealt so much damage to death." );
	my_def_msg( "Alas, how do we see the sky raining with $PLAYER$!" );
	my_def_msg( "Alas, birds carrying $PLAYER$ down the sky!" );
	my_def_msg( "Monday, $PLAYER$ was taken aback by the Chinese homework." );
	my_def_msg( "Monday, $PLAYER$ was forced to say in classroom." );
	my_def_msg( "Tuesday, $PLAYER$ was harvested." );
	my_def_msg( "Tuesday, $PLAYER$ was flooded by homework." );
	my_def_msg( "Wednesday, $PLAYER$ met the emperor." );
	my_def_msg( "Wednesday, $PLAYER$ was given sufficient homework by the emperor." );
	my_def_msg( "Thursday, $PLAYER$ exaggerated that there were no homework." );
	my_def_msg( "Thursday, $PLAYER$ went to YJY for a cup of tea." );
	my_def_msg( "Thursday, $PLAYER$ wrote a chairman tree." );
	my_def_msg( "Thursday, $PLAYER$ gazed at 'Segment Tree Beats'." );
	my_def_msg( "Friday, $PLAYER$ took an exam." );
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
