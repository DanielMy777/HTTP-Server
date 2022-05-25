#pragma once
#include "Message.h"

class Response : Message
{
private:
	int statusCode;
	string statusPhrase;
	string body;

public:
	Response(int code, const char* phrase);
	Response(const Response& other) = default;
	~Response() = default;

	virtual void addHeader(const char* name, const char* val);

	// Set the body of the response.
	void setBody(const string& content);
	// Get the body of the response.
	const string& getBody() { return this->body; }
	// Set the status code and status phrase of the response.
	void setStatus(int code, const string& phrase) { this->statusCode = code; this->statusPhrase = phrase; }
	// Get the status code of the response.
	int getStatusCode() { return this->statusCode; }
	// Get the status phrase of the response.
	const string& getStatusPhrase() { return this->statusPhrase; }

	// Parse the response into a pure HTTP format.
	string toString(bool statusOnly = false);
};