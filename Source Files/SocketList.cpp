#include "SocketList.h"
//--------------------------------------------------------------------------------------------------------
SocketList::SocketList(int sockCount)
{
	if (sockCount <= 0)
		throw invalid_argument("Max sockets must be avobe 0");
	this->sockets = new SocketState[sockCount];
	this->maxSockets = sockCount;
	this->socketCount = 0;
	for (int i = 0; i < this->maxSockets; i++)
	{
		this->sockets[i].recv = this->sockets[i].send = STATE::EMPTY;
		this->sockets[i].lastReqTime = chrono::steady_clock::now(); // arbitrary initial value
		this->sockets[i].len = 0;
	}
}
//--------------------------------------------------------------------------------------------------------
bool SocketList::addSocket(SOCKET id, STATE what)
{
	unsigned long flag = 1;
	char wsa[12];
	for (int i = 0; i < this->maxSockets; i++)
	{
		if (sockets[i].recv == STATE::EMPTY)
		{
			this->sockets[i].id = id;
			this->sockets[i].recv = what;
			this->sockets[i].send = STATE::IDLE;
			this->sockets[i].lastReqTime = chrono::steady_clock::now();
			this->sockets[i].len = 0;
			this->socketCount++;
			if (ioctlsocket(id, FIONBIO, &flag) != 0)
				throw logic_error(string("Server: Error at ioctlsocket() : ") + _itoa(WSAGetLastError(), wsa, 10));
				
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------------------------------------
void SocketList::removeSocket(int index)
{
	this->sockets[index].recv = STATE::EMPTY;
	this->sockets[index].send = STATE::EMPTY;
	this->socketCount--;
}
//--------------------------------------------------------------------------------------------------------
SocketList::~SocketList()
{
	for (int i = 0; i < this->maxSockets; i++)
	{
		if(this->sockets[i].recv == STATE::EMPTY && this->sockets[i].send == STATE::EMPTY)
			closesocket(this->sockets[i].id);
	}
	delete this->sockets;
}
//--------------------------------------------------------------------------------------------------------