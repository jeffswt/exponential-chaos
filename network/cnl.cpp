
//	"cnl.cpp" -*- C++ -*-
//	Created on: 2016
//	Definitions of cross platform network library.
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

#include "network/cnl.h"

#include <cstring>
#include <sstream>

bool	cnlSocket::SetAddress(
		std::string	Address,
		int			Port,
		std::string	Protocol)
{
	if (SocketType != Uninitialized &&
			SocketType != AddressBinded)
		return false;
//	Initialize windows socket system
	WORD	sockVersion = MAKEWORD(2, 2);
	WSADATA	wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
		goto FailOperation;
//	Assigning socket type
	if (Protocol == "TCP")
		Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	else if (Protocol == "UDP")
		Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	else
		goto FailOperation; // Still unsupported.
//	Assigning socket address
	bndAddr.sin_family = AF_INET;
	if (Address == "Any")
		bndAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	else
		bndAddr.sin_addr.S_un.S_addr = inet_addr(Address.c_str());
	bndAddr.sin_port = htons(Port);
	tAddress = Address;
	tPort = Port;
	tProtocol = Protocol;
//	Operation succeeded!
	SocketType = AddressBinded;
	return true;
FailOperation:
	return false;
}

bool	cnlSocket::ConnectServer(
		void)
{
	if (SocketType != AddressBinded)
		return false;
	if (tProtocol == "UDP")
		return this->CreateServer(32);
	if (connect(Socket, (sockaddr*)&bndAddr, sizeof(bndAddr)) == SOCKET_ERROR)
		return false;
	SocketType = Client;
	return true;
}

bool	cnlSocket::ConnectServer(
		std::string	Address,
		int			Port,
		std::string	Protocol)
{
	if (!this->SetAddress(Address, Port, Protocol))
		return false;
	return this->ConnectServer();
}

bool	cnlSocket::CreateServer(
		void)
{
//	Default maximum count of clients is 32.
	return this->CreateServer(32);
}

bool	cnlSocket::CreateServer(
		int	Clients)
{
	if (SocketType != AddressBinded)
		goto FailOperation;
	if (bind(Socket, (sockaddr*)&bndAddr, sizeof(bndAddr)) == SOCKET_ERROR)
		goto FailOperation;
	if (tProtocol == "TCP")
		if (listen(Socket, Clients) == SOCKET_ERROR)
			goto FailOperation;
	SocketType = Server;
	return true;
FailOperation:
	return false;
}

bool	cnlSocket::CreateServer(
		std::string	Address,
		int			Port,
		std::string	Protocol)
{
//	Using default client count.
	if (!this->SetAddress(Address, Port, Protocol))
		return false;
	return this->CreateServer();
}

cnlSocket	cnlSocket::GetClient(
		void)
{
	cnlSocket	nSocket;
	nSocket.Close();
//	Already nullified socket, use Connected() to check validity
	if (SocketType != Server)
		return nSocket;
	if (tProtocol != "TCP")
		return nSocket;
	int	addrBufSiz = sizeof(nSocket.bndAddr);
	nSocket.Socket = accept(
		this->Socket,
		(sockaddr*)&nSocket.bndAddr,
		&addrBufSiz);
//	Check socket validity
	if (nSocket.Socket == INVALID_SOCKET) {
		nSocket.Close();
		return nSocket;
	}
	char*	chAddr = new char[256];
	memset(chAddr, 0, sizeof(char) * 256);
	chAddr = inet_ntoa(nSocket.bndAddr.sin_addr);
	nSocket.tAddress = "";
	for (unsigned int i = 0; i < strlen(chAddr); i++)
		nSocket.tAddress += chAddr[i];
	nSocket.tPort = nSocket.bndAddr.sin_port;
	nSocket.tProtocol = this->tProtocol;
	nSocket.SocketType = Client;
	return nSocket;
}

bool	cnlSocket::Send(
		std::string&	Data)
{
	if (SocketType != Server &&
			SocketType != Client)
		return false;
	char*	sndData = new char[257];
	int		allLen = Data.length(),
			curLen = 0;
	for (int i = 0; i < allLen; i += 256) {
		curLen = std::min(256, allLen - i);
		for (int j = 0; j < curLen; j++)
			sndData[j] = Data[i + j];
		sndData[curLen] = 0x00;
		if (send(Socket, sndData, curLen, 0) <= 0)
			goto FailOperation;
	}
	delete sndData;
	return true;
FailOperation:
	delete sndData;
	this->Close();
	return false;
}

bool	cnlSocket::SendTo(
		std::string&	Data,
		std::string		Address,
		int				Port)
{
	if (SocketType != Server ||
			tProtocol != "UDP")
		return false;
	sockaddr_in	nAddr;
//	Assigning target address
	nAddr.sin_family = AF_INET;
	if (Address == "Any")
		nAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	else
		nAddr.sin_addr.S_un.S_addr = inet_addr(Address.c_str());
	nAddr.sin_port = htons(Port);
//	Processing send operation
	char*	sndData = new char[257];
	int		allLen = Data.length(),
			curLen = 0;
	for (int i = 0; i < allLen; i += 256) {
		curLen = std::min(256, allLen - i);
		for (int j = 0; j < curLen; j++)
			sndData[j] = Data[i + j];
		sndData[curLen] = 0x00;
		if (sendto(Socket, sndData, curLen, 0, (sockaddr*)&nAddr, sizeof(nAddr)) <= 0)
			goto FailOperation;
	}
	delete sndData;
	return true;
FailOperation:
	delete sndData;
	return false;
}

bool	cnlSocket::Receive(
		std::string&	Data)
{
	Data = "";
	if (SocketType != Server &&
			SocketType != Client)
		return false;
	char*	recData = new char[257];
	int		getLen = 0;
	memset(recData, 0, sizeof(char) * 257);
	while (true) {
		getLen = recv(Socket, recData, 256, 0);
		if (getLen == SOCKET_ERROR)
			goto FailOperation;
		recData[getLen] = 0x00;
		for (int i = 0; i < getLen; i++)
			Data += recData[i];
		if (getLen < 256)
			break;
	}
	delete recData;
	return true;
FailOperation:
	delete recData;
	Data = "";
	return false;
}

std::string	cnlSocket::Receive(
		void)
{
	std::string	RecvData = "";
	if (!this->Receive(RecvData))
		return "";
	return RecvData;
}


bool	cnlSocket::ReceiveFrom(
		std::string&	Data,
		std::string		Address,
		int				Port)
{
	if (SocketType != Server ||
			tProtocol != "UDP") {
		return false;
	}
	sockaddr_in	nAddr;
//	Assigning target address
	nAddr.sin_family = AF_INET;
	if (Address == "Any")
		nAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	else
		nAddr.sin_addr.S_un.S_addr = inet_addr(Address.c_str());
	nAddr.sin_port = htons(Port);
//	Processing receive operation
	char*	recData = new char[257];
	int		getLen = 0;
	memset(recData, 0, sizeof(char) * 257);
	while (true) {
		int	fromLen = sizeof(nAddr);
		getLen = recvfrom(Socket, recData, 256, 0, (sockaddr*)&nAddr, &fromLen);
		if (getLen == SOCKET_ERROR)
			goto FailOperation;
		recData[getLen] = 0x00;
		for (int i = 0; i < getLen; i++)
			Data += recData[i];
		if (getLen < 256)
			break;
	}
	delete recData;
	return true;
FailOperation:
	delete recData;
	return false;
}

bool	cnlSocket::Close(
		void)
{
//	This function has no locks because it should be called immediately.
	closesocket(Socket);
	SocketType = Uninitialized;
	Socket = 0;
	memset(&bndAddr, 0, sizeof(bndAddr));
	tAddress = "0.0.0.0";
	tPort = 0;
	tProtocol = "";
	return true;
}

bool	cnlSocket::Connected(
		void)
{
	bool	result = SocketType == Server ||
		SocketType == Client;
	return result;
}

std::string	cnlSocket::GetAddress(
		void)
{
	std::string	result = "0.0.0.0";
	if (SocketType != Uninitialized)
		result = tAddress;
	return result;
}

int	cnlSocket::GetPort(
		void)
{
	int	result = 0;
	if (SocketType != Uninitialized)
		result = tPort;
	return result;
}

std::string	cnlSocket::GetPortName(
		void)
{
	std::stringstream	Stream;
	std::string	Result;
	int	port = GetPort();
	Stream << port;
	Stream >> Result;
	return Result;
}

std::string	cnlSocket::GetProtocol(
		void)
{
	std::string	result = "";
	if (SocketType != Uninitialized)
		result = tProtocol;
	return result;
}

cnlSocket::cnlSocket(
		void)
{
	closesocket(Socket);
	SocketType = Uninitialized;
	Socket = 0;
	memset(&bndAddr, 0, sizeof(bndAddr));
	tAddress = "0.0.0.0";
	tPort = 0;
	tProtocol = "";
	return ;
}

cnlSocket::cnlSocket(
		cnlSocket&&	__x)
{
	this->SocketType = __x.SocketType;
	this->Socket = __x.Socket;
	this->bndAddr = __x.bndAddr;
	this->tAddress = __x.tAddress;
	this->tPort = __x.tPort;
	this->tProtocol = __x.tProtocol;
	return ;
}

cnlSocket& cnlSocket::operator= (const cnlSocket& __x)
{
	this->SocketType = __x.SocketType;
	this->Socket = __x.Socket;
	this->bndAddr = __x.bndAddr;
	this->tAddress = __x.tAddress;
	this->tPort = __x.tPort;
	this->tProtocol = __x.tProtocol;
	return (*this);
}

