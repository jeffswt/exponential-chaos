
//	"chat.h" -*- C++ -*-
//	Created on: 22 May 2016
//	In-game chat message manager.
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

#ifndef NETWORK_CHAT_H_
#define NETWORK_CHAT_H_

#include <iostream>
#include <vector>

/**
 *  @brief  initialise message templates
 *  The funny templates are to be initialised in this function. Change internal
 *  data to create more humorous results.
 */
bool	chatInitialise(
		void);

/**
 *  @brief  generate decease message
 *  @param  Name: the name of the player
 *  Apply death message template to the player and wait to output.
 */
std::string	chatGenDeceaseMessage(
		std::string	Name);

/**
 *  @brief  insert message to be displayed locally
 *  @param  Msg: the message to be inserted
 *  This is a non-blocking function, but waits related, parallel functions to
 *  terminate before executing and terminating itself.
 */
bool	chatInsertMessage(
		std::string	Msg);

/**
 *  @brief  retrieve message list to be displayed locally
 *  @param  List: a reference to the buffer
 *  Should be only called by graphicsRenderGameOverlay(...).
 */
bool	chatRetrieveMessageList(
		std::vector<std::string>&	List);

#endif /* NETWORK_CHAT_H_ */
