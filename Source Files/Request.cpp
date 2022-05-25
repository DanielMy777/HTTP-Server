#include "Request.h"
//--------------------------------------------------------------------------------------------------------
Request::Request()
{
	this->reqData = "";
	this->reqBody = "";
	this->reqType = SERVICE::BAD;

}
//--------------------------------------------------------------------------------------------------------
Request::Request(const char* message)
{
	stringstream stream;
	std::string line;
	stream.str(message);
	bool firstLine = true;
	if (!strcmp(message, "BAD REQUEST"))
	{
		this->reqType = SERVICE::BAD;
		return;
	}

	while (stream.str().find(this->CRLF) != std::string::npos)
	{
		std::getline(stream, line, '\n');
		if (line.empty()) break;
		if (line.back() != '\r') // validate line ends with CRLF
			throw logic_error("Line corrupted");
		else
			line.pop_back();
		if (line.empty()) break;

		if (firstLine)
		{
			firstLine = false;
			if (!parseReqLine(line))
				throw logic_error("Request line corrupted");
		}
		else
		{
			if(!parseHeaderLine(line))
				throw logic_error("Header line corrupted");
		}
	}
	
	while (!stream.eof()) //headers finished, empty line recognized
	{
		std::getline(stream, line, '\n');
		if(!line.empty()) this->reqBody += line + '\n';
	}
}
//--------------------------------------------------------------------------------------------------------
Request& Request::operator=(Request& other)
{
	if (this != &other)
	{
		this->reqData = other.reqData;
		this->reqType = other.reqType;
		this->reqBody = other.reqBody;
		this->headers.clear();
		for (Header h : other.headers)
			this->headers.push_back(h);
	}
	return *this;
}
//--------------------------------------------------------------------------------------------------------
void Request::addHeader(const char* name, const char* val)
{
	this->headers.push_back({ name, val });
}
//--------------------------------------------------------------------------------------------------------
const string& Request::getHeaderValue(const char* name)
{
	for (Header& h : headers)
		if (!strcmp(name, h.value.c_str()))
			return h.value;
	throw invalid_argument("Header doesnt exist");
}
//--------------------------------------------------------------------------------------------------------
string Request::toString(bool statusOnly)
{
	string message = "";
	message += SERV_NAME::names[this->reqType] + " " + this->reqData + " " + VERSION + CRLF;
	if (!statusOnly)
	{
		for (Header& hdr : headers)
			message += hdr.name + ": " + hdr.value + CRLF;

		message += CRLF;
		if (!reqBody.empty())
			message += reqBody;
	}

	return message;
}
//--------------------------------------------------------------------------------------------------------
bool Request::parseReqLine(string& line)
{
	vector<string> vec;
	mySplit(line, " ", vec);

	if (vec.size() != 3)
		return false;
	
	if (!strcmp(vec[0].c_str(), "GET"))
		this->reqType = SERVICE::GET;
	else if (!strcmp(vec[0].c_str(), "OPTIONS"))
		this->reqType = SERVICE::OPTIONS;
	else if (!strcmp(vec[0].c_str(), "HEAD"))
		this->reqType = SERVICE::HEAD;
	else if (!strcmp(vec[0].c_str(), "POST"))
		this->reqType = SERVICE::POST;
	else if (!strcmp(vec[0].c_str(), "PUT"))
		this->reqType = SERVICE::PUT;
	else if (!strcmp(vec[0].c_str(), "DELETE"))
		this->reqType = SERVICE::DEL;
	else if (!strcmp(vec[0].c_str(), "TRACE"))
		this->reqType = SERVICE::TRACE;
	else
		return false;

	if (vec[2] != this->VERSION && vec[2] != "HTTP/1.0")
		return false;

	this->reqData = vec[1];

	return true;
}
//--------------------------------------------------------------------------------------------------------
bool Request::parseHeaderLine(string& line)
{
	vector<string> vec;
	mySplit(line, ":", vec, 1);
	if (vec.size() != 2)
		return false;
	this->addHeader(vec[0].c_str(), vec[1].c_str());

	return true;
}
//--------------------------------------------------------------------------------------------------------