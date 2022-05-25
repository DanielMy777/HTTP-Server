#include "Response.h"
//--------------------------------------------------------------------------------------------------------
Response::Response(int code, const char* phrase)
{
	this->statusCode = code;
	this->statusPhrase = phrase;
}
//--------------------------------------------------------------------------------------------------------
void Response::addHeader(const char* name, const char* val)
{
	for (Header& h : headers)
	{
		if (!strcmp(name, h.name.c_str()))
		{
			h.value = val;
			return;
		}
	}
	this->headers.push_back({ name, val });
}
//--------------------------------------------------------------------------------------------------------
void Response::setBody(const string& content)
{
	this->body = content;
}
//--------------------------------------------------------------------------------------------------------
string Response::toString(bool statusOnly)
{
	string message = "";
	message += VERSION + " " + std::to_string(statusCode) + " " + statusPhrase + CRLF;
	if (!statusOnly)
	{
		for (Header& hdr : headers)
			message += hdr.name + ": " + hdr.value + CRLF;

		message += CRLF;
		if (!body.empty())
			message += body;
	}

	return message;
}
//--------------------------------------------------------------------------------------------------------