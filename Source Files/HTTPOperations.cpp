#include "HTTPOperations.h"

//--------------------------------------------------------------------------------------------------------
bool HTTPOperations::receiveMessage(SocketState& sock)
{
	SOCKET msgSocket = sock.id;

	int messegeLen;
	int len = sock.len;
	int bytesRecv = recv(msgSocket, &sock.buffer[len], sizeof(sock.buffer) - len, 0);

	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Server: Error at recv(): " << WSAGetLastError() << endl;
		return true;
	}
	if (bytesRecv == 0)
	{
		cout << ">>> Server: Client in socket " << msgSocket << " disconnected" << endl;
		return true;
	}
	else
	{
		sock.buffer[len + bytesRecv] = '\0';
		sock.len += bytesRecv;

		if (sock.len > 0)
		{
			messegeLen = strlen(sock.buffer);
			try {
				
				Request newReq(sock.buffer);
				sock.req = newReq;
			}
			catch (...) {
				Request badReq("BAD REQUEST");
				sock.req = badReq;
			}

			sock.lastReqTime = chrono::steady_clock::now();
			sock.send = STATE::SEND;
			memcpy(sock.buffer, &sock.buffer[messegeLen], sock.len - messegeLen);
			sock.len -= messegeLen;
			cout << ">>> Server: Recieved - " << sock.req.toString(true);

			return false;
		}
	}

	return true;
}
//--------------------------------------------------------------------------------------------------------
void HTTPOperations::sendMessage(SocketState& sock)
{
	int bytesSent = 0;
	char sendBuff[BUF_SIZE];

	SOCKET msgSocket = sock.id;
	Response response = generateResponseMessage(sock);

	strcpy(sendBuff, response.toString().c_str());

	bytesSent = send(msgSocket, sendBuff, (int)strlen(sendBuff), 0);
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Server: Error at send(): " << WSAGetLastError() << endl;
		return;
	}

	cout << ">>> Server: Sent - " << response.toString(true) << endl;

	if (sock.len == 0)
		sock.send = STATE::IDLE;
}
//--------------------------------------------------------------------------------------------------------
Response HTTPOperations::generateResponseMessage(SocketState& sock)
{
	switch (sock.req.getType())
	{
	case SERVICE::OPTIONS:
		return generateOptionsResponse(sock.req.getResource());
	case SERVICE::GET:
		return generateGetResponse(sock.req.getResource());
	case SERVICE::HEAD:
		return generateHeadResponse(sock.req.getResource());
	case SERVICE::POST:
		return generatePostResponse(sock.req.getBody());
	case SERVICE::PUT:
		return generatePutResponse(sock.req.getResource(), sock.req.getBody());
	case SERVICE::DEL:
		return generateDeleteResponse(sock.req.getResource());
	case SERVICE::TRACE:
		return generateTraceResponse(sock.req.toString());
	default:
		return generateErrorResponse(400, "BAD REQUEST");
		break;
	}
}
//--------------------------------------------------------------------------------------------------------
Response HTTPOperations::generateOptionsResponse(const string& resource)
{
	vector<string> vec;
	string page, path, services;

	if (resource.front() != '/')
	{
		return generateErrorResponse(400, "BAD REQUEST");
	}
	mySplit(resource, "?", vec, 1);
	page = vec[0];
	path = ROOT_PATH + page;
	if (!SystemOperations::doesFileExist(path.c_str()) && page != "/*")
	{
		return generateErrorResponse(404, "NOT FOUND");
	}

	Response res(204, "NO CONTENT");
	res.setBody("");
	services = "";
	for (pair<SERVICE, string> p : SERV_NAME::names) { services += p.second + ", "; }
	if (!services.empty()) services = services.substr(0, services.size() - 2); // remove ', ' at end

	addBasicHeaders(res);
	res.addHeader("Allow", services.c_str());
	res.addHeader("Content-Length", "0");

	return res;
}
//--------------------------------------------------------------------------------------------------------
Response HTTPOperations::generateGetResponse(const string& resource)
{
	vector<string> vec;
	map<string, string> queries;
	string page, path;

	if (resource.front() != '/')
	{
		return generateErrorResponse(400, "BAD REQUEST");
	}
	mySplit(resource, "?", vec, 1);
	page = vec[0];
	if (vec.size() > 1) // there are query params
	{
		parseQueryString(queries, vec[1]);
		if(!checkSupportedParams(queries))
			return generateErrorResponse(422, "UNPROCESSABLE ENTITY", "Query parameter unsupported");
	}

	page.insert(page.find_last_of("/") + 1,
		queries.find("lang") != queries.end() ? queries["lang"] + "-" : "");
	path = ROOT_PATH + page;
	if (!SystemOperations::doesFileExist(path.c_str()))
	{
		return generateErrorResponse(404, "NOT FOUND");
	}

	Response res(200, "OK");
	res.setBody(SystemOperations::readFile(path.c_str()));
	res.addHeader("Content-Length", std::to_string(res.getBody().length()).c_str());
	addBasicHeaders(res);

	return res;
}
//--------------------------------------------------------------------------------------------------------
Response HTTPOperations::generateHeadResponse(const string& resource)
{
	Response temp(generateGetResponse(resource));
	temp.setBody(""); //empty

	return temp;
}
//--------------------------------------------------------------------------------------------------------
Response HTTPOperations::generatePostResponse(const string& data)
{
	cout << endl << ">>> Incoming post message: " << endl << "------" << endl << data << "------" << endl << endl;
	Response res(200, "OK");
	res.setBody("<h2> Post Successful </h2>");
	res.addHeader("Content-Length", std::to_string(res.getBody().length()).c_str());
	addBasicHeaders(res);

	return res;
}
//--------------------------------------------------------------------------------------------------------
Response HTTPOperations::generatePutResponse(const string& resource, const string& content)
{
	if (resource.front() != '/')
		return generateErrorResponse(400, "BAD REQUEST");

	string path = ROOT_PATH + resource;
	string content_cpy = content;
	Response res(200, "OK");

	if (!SystemOperations::doesFileExist(path.c_str()))
	{
		res.setStatus(201, "CREATED");
		res.setBody("<h2> Put successful, new file was created at " + resource);
	}
	else
		res.setBody("<h2> Put successful, file updated at " + resource);

	ofstream file(path);
	removeOccurences(content_cpy, '\r');
	file.write(content_cpy.c_str(), content_cpy.size());
	if (file.bad())
		throw logic_error("Failure writing to file");
	
	res.addHeader("Content-Length", std::to_string(res.getBody().length()).c_str());
	addBasicHeaders(res);

	return res;
	
}
//--------------------------------------------------------------------------------------------------------
Response HTTPOperations::generateDeleteResponse(const string& resource)
{
	if (resource.front() != '/')
		return generateErrorResponse(400, "BAD REQUEST");

	string path = ROOT_PATH + resource;
	std::remove(path.c_str());
	
	Response res(204, "NO CONTENT");
	res.addHeader("Content-Length", "0");
	addBasicHeaders(res);

	return res;
}
//--------------------------------------------------------------------------------------------------------
Response HTTPOperations::generateTraceResponse(const string& fullHttpRequest)
{
	Response res(200, "OK");
	res.setBody(fullHttpRequest);
	res.addHeader("Content-Length", std::to_string(res.getBody().length()).c_str());
	res.addHeader("Content-Type", "message/http");
	addBasicHeaders(res);

	return res;
}
//--------------------------------------------------------------------------------------------------------
Response HTTPOperations::generateErrorResponse(int code, string phrase, string comment)
{
	Response err(code, phrase.c_str());
	err.setBody("<h1> " + std::to_string(code) + " " + phrase + " </h1>" +
		(!comment.empty() ? "<br><h3> " + comment + " </h3>" : ""));
	err.addHeader("Content-Length", std::to_string(err.getBody().length()).c_str());
	addBasicHeaders(err);
	return err;
}
//--------------------------------------------------------------------------------------------------------
int HTTPOperations::parseQueryString(map<string, string>& res, string& query)
{
	int counter = 0;
	vector<string> splittedQueryString;
	vector<string> currentQuery;

	mySplit(query, "&", splittedQueryString, INT_MAX);
	for (string s : splittedQueryString)
	{
		currentQuery.clear();
		mySplit(s, "=", currentQuery, 1);
		if (currentQuery.size() != 2)
			continue;
		res[currentQuery[0]] = currentQuery[1];
		counter++;
	}

	return counter;
}
//--------------------------------------------------------------------------------------------------------
bool HTTPOperations::checkSupportedParams(map<string, string>& params)
{
	bool flag = false;
	if (params.find("lang") != params.end())
		for (string l : SUPPORTED_LANG)
			flag |= (l == params["lang"]);
	else
		flag = true;

	return flag;
}
//--------------------------------------------------------------------------------------------------------
void HTTPOperations::addBasicHeaders(Response& res)
{
	char timeBuf[BUF_SIZE];
	time_t now = time(0);
	struct tm tm = *gmtime(&now);
	strftime(timeBuf, BUF_SIZE, "%a, %d %b %Y %H:%M:%S %Z", &tm);

	res.addHeader("Content-Type", "text/html");
	res.addHeader("Server", "WebServer");
	res.addHeader("Date", timeBuf);
}
//--------------------------------------------------------------------------------------------------------