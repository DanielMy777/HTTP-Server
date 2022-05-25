#pragma once
#include "SocketList.h"
#include "HTTPOperations.h"

class Server
{
private:
	SocketList allSockets;
	fd_set waitRecv;
	fd_set waitSend;
	timeval timeout = TIMEOUT;

	// Teminate all sockets that last interacted more than 'TERMINATION_TIME' seconds ago.
	void TerminateTimedOuts();
	// Map all sockets to send / receive fd_sets.
	void mapSockets();
	// Send responses to all sockets the are ready to get a response.
	void sendMessages(int& nfd);
	// Get requests from all sockets the sent a messages.
	void receiveMessages(int& nfd);
	// Accept a connection onto the server. 
	void acceptConnection(SOCKET listenSock);
public:
	Server(SOCKET listenSocket);
	Server() = delete;
	Server(const Server& other) = delete;
	~Server() = default;

	// Begin running the server.
	void runServer();
};