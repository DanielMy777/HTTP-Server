#include "Server.h"
//--------------------------------------------------------------------------------------------------------
Server::Server(SOCKET listenSocket) : allSockets(MAX_SOCKETS)
{
	checkError(SOCKET_ERROR, listen(listenSocket, 5), "Server: Error at listen(): ", true, listenSocket);
	this->allSockets.addSocket(listenSocket, STATE::LISTEN);
	this->timeout = TIMEOUT;
	FD_ZERO(&this->waitRecv);
	FD_ZERO(&this->waitSend);
}
//--------------------------------------------------------------------------------------------------------
void Server::runServer()
{
	int nfd;
	while (true)
	{
		TerminateTimedOuts();
		mapSockets();

		nfd = select(0, &this->waitRecv, &this->waitSend, NULL, &this->timeout);
		checkError(nfd, SOCKET_ERROR, "Server: Error at select() : ", true, -1);

		sendMessages(nfd);
		receiveMessages(nfd);
	}
}
//--------------------------------------------------------------------------------------------------------
void Server::mapSockets()
{
	int maxSockets = this->allSockets.getMaxSockets();

	FD_ZERO(&this->waitRecv);
	FD_ZERO(&this->waitSend);
	for (int i = 0; i < maxSockets; i++)
	{
		if ((allSockets[i].recv == STATE::LISTEN) || (allSockets[i].recv == STATE::RECEIVE))
		{
			FD_SET(allSockets[i].id, &waitRecv);
		}

		if (allSockets[i].send == STATE::SEND)
		{
			FD_SET(allSockets[i].id, &waitSend);
		}
	}
}
//--------------------------------------------------------------------------------------------------------
void Server::sendMessages(int& nfd)
{
	int maxSockets = this->allSockets.getMaxSockets();

	for (int i = 0; i < maxSockets && nfd > 0; i++)
	{
		if (FD_ISSET(allSockets[i].id, &waitSend))
		{
			nfd--;
			switch (allSockets[i].send)
			{
			case STATE::SEND:
				HTTPOperations::sendMessage(allSockets[i]);
				break;
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------
void Server::receiveMessages(int& nfd)
{
	int maxSockets = this->allSockets.getMaxSockets();

	for (int i = 0; i < maxSockets && nfd > 0; i++)
	{
		if (FD_ISSET(allSockets[i].id, &waitRecv))
		{
			nfd--;
			switch (allSockets[i].recv)
			{
			case STATE::LISTEN:
				acceptConnection(allSockets[i].id);
				break;

			case STATE::RECEIVE:
				if (HTTPOperations::receiveMessage(allSockets[i]))
				{
					allSockets.removeSocket(i);
					closesocket(allSockets[i].id);
				}
				break;
			}
		}
	}
}
//--------------------------------------------------------------------------------------------------------
void Server::acceptConnection(SOCKET listenSock)
{
	struct sockaddr_in from;
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(listenSock, (struct sockaddr*) & from, &fromLen);
	checkError(INVALID_SOCKET, msgSocket, "Server: Error at accept(): ", true, -1);
	if (INVALID_SOCKET == msgSocket)
	{
		cout << "Server: Error at accept(): " << WSAGetLastError() << endl;
		return;
	}
	cout << ">>> Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " has connected." << endl << endl;

	try {
		if (allSockets.addSocket(msgSocket, STATE::RECEIVE) == false)
		{
			cout << ">>> Too many connections, dropped!\n";
			closesocket(msgSocket);
		}
	}
	catch (exception & e) {
		cout << e.what() << endl;
		closesocket(msgSocket);
	}

	return;
}
//--------------------------------------------------------------------------------------------------------
void Server::TerminateTimedOuts()
{
	long long dur;
	int maxSockets = this->allSockets.getMaxSockets();
	chrono::steady_clock::time_point now = chrono::steady_clock::now();

	for (int i = 1; i < maxSockets; i++)
	{
		if ((allSockets[i].recv == STATE::EMPTY) && (allSockets[i].send == STATE::EMPTY))
			continue;

		dur = chrono::duration_cast<chrono::seconds>(now - allSockets[i].lastReqTime).count();
		if (dur >= TERMINATION_TIME)
		{
			cout << ">>> Server: Client in socket " << allSockets[i].id << " terminated due to inactivity" << endl;
			allSockets.removeSocket(i);
			closesocket(allSockets[i].id);
		}
	}
	
}
//--------------------------------------------------------------------------------------------------------