#pragma once
#include "Utilities.h"

struct Header
{
	string name;
	string value;
};

class Message // ABSTRACT CLASS
{
protected:
	const string VERSION = "HTTP/1.1";
	const string CRLF = "\r\n";
	list<Header> headers;

	// Add a header to a message (<name>: <val>). PURE VIRTUAL FUNC.
	virtual void addHeader(const char* name, const char* val) = 0;
public:
	~Message() = default;
};