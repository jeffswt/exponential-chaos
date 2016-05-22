
//	"tusnil.cpp" -*- C++ -*-
//	Created on: 2016
//	Definitions of Trusted Users' Scalable Network Interconnection Library.
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

#include "tusnil.h"

#include <sstream>

void my_debug_func(std::string __x)
	{ static	std::mutex	Locker; Locker.lock();
	  std::cout << std::flush << "DEBUG: " << __x << "\n" << std::flush;
	  Locker.unlock(); return ; }

//#define my_debug(__x) my_debug_func(__x)
#define my_debug(__x)

void	tusnilDirectedNode::funcRootManagerUdp(
		void)
{
	while (rootSockUdp.Connected()) {
		std::string	SendMsg = "";
		try {
			SendMsg = rootSendQueue.front();
		} catch (const char* exc) {
			break;
		}
		rootSendQueue.pop();
		for (auto clnIdx : rootClnIdx)
			rootSockUdp.SendTo(SendMsg, clnIdx.first, clnIdx.second);
		continue;
	}
	rootSockUdp.Close();
	return ;
}

void	tusnilDirectedNode::funcRootManager(
		void)
{
//	Define vector of threads for final joining
	message_queue	SendQue;
	std::thread		udpThread(funcRootManagerUdp, this);
	my_debug("Root watcher - started");
//	Receiving connection and spawning managers
	while (rootSock.Connected()) {
		cnlSocket*	clnSock = new cnlSocket;
		*clnSock = rootSock.GetClient();
		if (!clnSock->Connected())
			break;
		rootClients++;
		my_debug("Root watcher - received incoming connection");
		std::string	RecvData = "";
		clnSock->Receive(RecvData);
		std::stringstream	Stream;
		Stream.clear(); Stream << RecvData;
		int tPort = 0; Stream >> tPort;
		rootClnIdx.insert(std::make_pair(clnSock->GetAddress(), tPort));
		clnSock->Close();
//		Waiting for more clients to connect
		continue;
	}
//	Make sure all threads are terminated
	rootSock.Close();
	rootSockUdp.Close();
	rootSendQueue.release();
	if (udpThread.joinable())
		udpThread.join();
	return ;
}

bool	tusnilDirectedNode::Connected(
		void)
{
	if (State == Server)
		return rootSock.Connected() &&
				rootSockUdp.Connected();
	else if (State == Client)
		return rootSock.Connected();
	return false;
}

int		tusnilDirectedNode::GetClientCount(
		void)
{
	if (!this->Connected())
		return 0;
	if (State == Server)
		return this->rootClnIdx.size();
	else if (State == Client)
		return 1;
	return 0;
}

bool	tusnilDirectedNode::SetAddress(
		std::string	Address,
		int			Port)
{
	if (State != Uninitialized &&
			State != AddressBinded)
		return false;
	stAddress = Address;
	stPort = Port;
	State = AddressBinded;
	return true;
}

bool	tusnilDirectedNode::CreateServer(
		int	Clients)
{
	if (State != AddressBinded)
		return false;
	if (!rootSock.SetAddress(stAddress, stPort, "TCP"))
		return false;
	if (!rootSock.CreateServer(Clients))
		return false;
	if (!rootSockUdp.SetAddress(stAddress, stPort + 1, "UDP"))
		return false;
	if (!rootSockUdp.CreateServer(Clients))
		return false;
	rootManagerTh = std::thread(funcRootManager, this);
	State = Server;
	my_debug("Create server - service started");
	return true;
}

bool	tusnilDirectedNode::ConnectServer(
		void)
{
	if (State != AddressBinded)
		return false;
	int nPort = stPort;
	cnlSocket	lnSock;
//	Report UDP port to server
	while (!rootSock.Connected()) {
		nPort = rand() % 8192 + 16384;
		rootSock.SetAddress("Any", nPort, "UDP");
		rootSock.CreateServer(32);
	}
	if (!lnSock.SetAddress(stAddress, stPort, "TCP"))
		return false;
	if (!lnSock.ConnectServer())
		return false;
	std::stringstream	Stream;
	Stream.clear(); Stream << nPort;
	std::string	Work; Stream >> Work;
	lnSock.Send(Work);
	my_debug("Connect server - success");
	State = Client;
	return true;
}

bool	tusnilDirectedNode::Close(
		void)
{
	if (State != Server &&
			State != Client)
		return false;
	rootSock.Close();
	if (rootManagerTh.joinable())
		rootManagerTh.join();
	State = Uninitialized;
	return true;
}

bool	tusnilDirectedNode::Send(
		std::string&	Data)
{
	if (State == Server) {
		rootSendQueue.push(Data);
	} else if (State == Client) {
		rootSock.SendTo(Data, stAddress, stPort + 1);
	} else {
		return false;
	}
	return true;
}

bool	tusnilDirectedNode::Receive(
		std::string&	Data)
{
	if (State == Server) {
		rootSockUdp.Receive(Data);
	} else if (State == Client) {
		rootSock.ReceiveFrom(Data, stAddress, stPort + 1);
	} else {
		return false;
	}
	return true;
}

tusnilDirectedNode::tusnilDirectedNode(
		void)
{
	State = Uninitialized;
	rootSock.Close();
	rootClients = 0;
	stPort = 0;
	return ;
}

