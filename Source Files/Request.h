#pragma once
#include "Message.h"

class Request : Message
{
private:
	SERVICE reqType;
	string reqData;
	string reqBody;

	// Receive a message request (first) line and parse it into reqType(http method) and reqData (entity).
	// Return true iff the line was valid and the parse was OK.
	bool parseReqLine(string& line);
	// Receive a message's header (middle) line and parse it into a header.
	// Return true iff the line was valid and the parse was OK.
	bool parseHeaderLine(string& line);

public:

	Request();
	Request(const char* message);
	Request& operator=(Request& other);
	~Request() = default;

	virtual void addHeader(const char* name, const char* val);

	// Get the value of the header with key <name>.
	const string& getHeaderValue(const char* name);
	// Get the http method of the request.
	SERVICE getType() { return this->reqType; }
	// Get the resource the request is targeted at.
	const string& getResource() { return this->reqData; }
	// Get the body of the request.
	const string& getBody() { return this->reqBody; }

	// Parse the request into a pure HTTP format.
	string toString(bool statusOnly = false);
};