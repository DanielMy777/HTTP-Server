#pragma once
#include "Utilities.h"
#include "Request.h"

struct SocketState
{
	SOCKET id;			// Socket handle
	STATE recv;			// Receiving?
	STATE send;			// Sending?
	Request req;	// Sending sub-type
	chrono::steady_clock::time_point lastReqTime; // Time of last request byte
	char buffer[SOCK_BUF];
	int len;
};

class SocketList
{
private:
	int maxSockets;
	int socketCount;
	SocketState* sockets;
public:
	SocketList(int sockCount = MAX_SOCKETS);
	SocketList(const SocketList& other) = delete;
	SocketList& operator=(const SocketList& other) = delete;
	~SocketList();

	// Add a new socket to the SocketList, with the given id (id) and initial state (what).
	bool addSocket(SOCKET id, STATE what);
	// Remove a socket at index 'index' in the SocketList.
	void removeSocket(int index);
	// Indexing operator
	SocketState& operator[](int i) { return this->sockets[i]; }

	// Get the amount of sockets registered.
	int getSockCount() { return this->socketCount; }
	// Get the maximum amount of sockets that can register.
	int getMaxSockets() { return this->maxSockets; }
	
};