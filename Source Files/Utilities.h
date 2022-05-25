#pragma once
#pragma comment(lib, "Ws2_32.lib")

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define USE_PORT 8080
#define MAX_SOCKETS 60
#define SOCK_BUF 4095
#define BUF_SIZE 255
#define TIMEOUT { 1,0 }
#define TERMINATION_TIME 120
#define SUPPORTED_LANG {"he", "en", "fr"}
#define ROOT_PATH "C:\\temp"

#include <iostream>
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include <chrono>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <time.h>
#include <fstream>
#include <sstream>

using namespace std;

enum class STATE {EMPTY, LISTEN, RECEIVE, SEND, IDLE};					// State of a socket
enum class SERVICE {OPTIONS, GET, HEAD, POST, PUT, DEL, TRACE, BAD};	// Sevices supported
struct SERV_NAME														// Services as enums and their actual names.
{
	static map<SERVICE, string> names;
};

// Check if an error exists and throw it while printing an error message (message)
// An error occurs between value and expectedErr at changing conditions, depending on wsaStarted.
// if toClose is not -1, the socket at that id will be closed.
void checkError(int value, int expectedErr, const string& message, bool wsaStarted, int toClose);

// Split a string (src) by the delimiter (delimiter) and insert the fractions into dst.
// Maximum amount of splits to be done is masSplits, after that everything will be considered ine peice.
void mySplit(const string& src, const char* delimiter, vector<string>& dst, int maxSplits = 10);

// Remove all occurences of character c from string src.
void removeOccurences(string& src, char c);