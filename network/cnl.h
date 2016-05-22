
//	"cnl.h" -*- C++ -*-
//	Created on: 2016
//	Cross platform network library.
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

#ifndef CROSS_NETWORK_LIBRARY_H_
#define CROSS_NETWORK_LIBRARY_H_

#include <iostream>
#include <winsock2.h>

class	cnlSocket
{
protected:
	enum	tSocketType {
		Uninitialized,
		AddressBinded,
		Server,
		Client
	} SocketType;
	SOCKET		Socket;
	sockaddr_in	bndAddr;
	std::string	tAddress;
	int			tPort;
	std::string	tProtocol;
public:
	bool	SetAddress(
			std::string	Address,
			int			Port,
			std::string	Protocol);
//	Client connection functions
	bool	ConnectServer(
			void);
	bool	ConnectServer(
			std::string	Address,
			int			Port,
			std::string	Protocol);
//	Server connection functions
	bool	CreateServer(
			void);
	bool	CreateServer(
			int	Clients);
	bool	CreateServer(
			std::string	Address,
			int			Port,
			std::string	Protocol);
	bool	Close(void);
//	Server-only functions
	cnlSocket	GetClient(void);
//	Data transmission functions
	bool	Send(
			std::string&	Data);
	bool	SendTo(
			std::string&	Data,
			std::string		Address,
			int				Port);
	bool	Receive(
			std::string&	Data);
	std::string	Receive(
			void);
	bool	ReceiveFrom(
			std::string&	Data,
			std::string		Address,
			int				Port);
//	Data retrieving functions
	bool		Connected(void);
	std::string	GetAddress(void);
	int			GetPort(void);
	std::string	GetPortName(void);
	std::string	GetProtocol(void);
//	Initializers
	cnlSocket(void);
	cnlSocket(cnlSocket&&);
	cnlSocket& operator= (const cnlSocket&);
};

#endif /* CROSS_NETWORK_LIBRARY_H_ */

