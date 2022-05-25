#include "Server.h"

int main()
{
	// Create a WSADATA object called wsaData.
	// Create and bind a socket to an internet address.
	// Listen through the socket for incoming connections.
	WSAData wsaData;
	checkError(NO_ERROR, WSAStartup(MAKEWORD(2, 2), &wsaData), "Server: Error at WSAStartup()\n", false, -1);

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	checkError(INVALID_SOCKET, listenSocket, "Time Server: Error at socket(): ", true, -1);

	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;
	serverService.sin_port = htons(USE_PORT);
	checkError(SOCKET_ERROR, bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService)),
		"Server: Error at bind() : ", true, listenSocket);

	checkError(SOCKET_ERROR, listen(listenSocket, 5), "Server: Error at listen(): ", true, listenSocket);

	try {
		Server myServer(listenSocket);
		cout << ">>> Server is runnning..." << endl;
		cout << ">>> Listening on port " << USE_PORT << endl << endl;

		myServer.runServer();
	}
	catch (exception e) {
		cout << "Server stopped due ERROR: " << e.what() << endl;
	}
	
	cout << "Server: Closing Connection.\n";
	closesocket(listenSocket);
	WSACleanup();

	return 0;
}