
//	"tusnil.h" -*- C++ -*-
//	Created on: 2016
//	Trusted Users' Scalable Network Interconnection Library.
//
//	Copyright (C) 2016  Geoffrey Tang.
//	(https://github.com/ht35268/cross_network_library/)
//
//	This library is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	This library is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef TRUSTED_USERS_SCALABLE_NETWORK_INTERCONNECTION_LIBRARY_H_
#define TRUSTED_USERS_SCALABLE_NETWORK_INTERCONNECTION_LIBRARY_H_

#include <iostream>
#include <set>
#include <thread>

#include "network/cnl.h"
#include "algo/blocking_queue.h"

class	tusnilDirectedNode
{
protected:
	enum	tState {
		Uninitialized,
		AddressBinded,
		Server,
		Client
	} State;
	cnlSocket	rootSock, rootSockUdp;
	int			rootClients;
	std::string	stAddress;
	int			stPort;
//	Thread management
	typedef blocking_queue<std::string> message_queue;
	message_queue	rootSendQueue;
	std::set<std::pair<std::string, int> >	rootClnIdx;
	void	funcRootManager(
			void);
	void	funcRootManagerUdp(
			void);
	std::thread	rootManagerTh;
public:
//	Informative functions
	bool	Connected(
			void);
	int		GetClientCount(
			void);
//	Connection and entry establishment functions
	bool	SetAddress(
			std::string	Address,
			int			Port);
	bool	CreateServer(
			int	Clients);
	bool	ConnectServer(
			void);
	bool	Close(
			void);
//	Message sending and receiving
	bool	Send(
			std::string&	Data);
	bool	Receive(
			std::string&	Data);
//	Constructors and **destruction** procedures
	tusnilDirectedNode(
			void);
};

#endif /* TRUSTED_USERS_SCALABLE_NETWORK_INTERCONNECTION_LIBRARY_H_ */

