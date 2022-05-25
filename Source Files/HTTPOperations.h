#pragma once
#include "Response.h"
#include "SocketList.h"
#include "SystemOperations.h"

class HTTPOperations
{
private:
	// Returns a response message, with the status code, status phrase, and body (comment) provided.
	// Used to generate errors.
	static Response generateErrorResponse(int code, string phrase, string comment = "");
	// Receive an entity line, and parse all parameters to the map res in <key, value> order.
	// Returns the amount of parameters.
	static int parseQueryString(map<string, string>& res, string& query);
	// Recieve a map of parameters, returns true iff all are valid and supported.
	static bool checkSupportedParams(map<string, string>& params);
public:
	// Obtain and handle a message / request from a socket (sock).
	// Return true iff the socket should be closed.
	static bool receiveMessage(SocketState& sock);
	// Generate and handle a message / response to a socket (sock).
	static void sendMessage(SocketState& sock);

	// Response generating methods:
	static Response generateResponseMessage(SocketState& sock);
	static Response generateOptionsResponse(const string& resource);
	static Response generateGetResponse(const string& resource);
	static Response generateHeadResponse(const string& resource);
	static Response generatePostResponse(const string& data);
	static Response generatePutResponse(const string& resource, const string& content);
	static Response generateDeleteResponse(const string& resource);
	static Response generateTraceResponse(const string& fullHttpRequest);

	// Adding the server's basic default headers to a response (res)_.
	static void addBasicHeaders(Response& res);
};