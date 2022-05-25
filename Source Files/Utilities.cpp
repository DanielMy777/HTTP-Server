#include "Utilities.h"
//---------------------------------STATIC VARIABLE DECLARATION--------------------------------------------
map<SERVICE, string> SERV_NAME::names = { {SERVICE::OPTIONS, "OPTIONS"},
												{SERVICE::GET, "GET"},
												{SERVICE::HEAD, "HEAD"},
												{SERVICE::POST, "POST"},
												{SERVICE::PUT, "PUT"},
												{SERVICE::DEL, "DELETE"},
												{SERVICE::TRACE, "TRACE"},
};
//--------------------------------------------------------------------------------------------------------
void checkError(int value, int expectedErr, const string& message, bool wsaStarted, int toClose)
{
	bool condition = wsaStarted ? (value == expectedErr) : (value != expectedErr);
	if (condition)
	{
		cout << message;
		if (wsaStarted)
		{
			cout << WSAGetLastError() << endl;
			WSACleanup();
			if (toClose != -1)
			{
				closesocket(toClose);
			}
		}
		throw("Server Error");
	}
}
//--------------------------------------------------------------------------------------------------------
void mySplit(const string& src, const char* delimiter, vector<string>& dst, int maxSplits)
{
	string temp;
	char *token, *str;
	if (src.find(delimiter) == string::npos)
	{
		dst.push_back(src);
		return;
	}

	str = new char[src.size() + 1];
	strcpy(str, src.c_str());
	token = strtok(str, delimiter);
	while (token != NULL)
	{
		maxSplits -= 1;
		temp = token;
		temp.erase(temp.find_last_not_of(" ") + 1); //delete trailing and leading spaces
		temp.erase(0, temp.find_first_not_of(" "));
		dst.push_back(temp);
		if (maxSplits > 0)
			token = strtok(NULL, delimiter);
		else if (maxSplits == 0)
			token += temp.size() + 1;
		else
			break;
	}
	
	delete[] str;
}
//--------------------------------------------------------------------------------------------------------
void removeOccurences(string& src, char c)
{
	string::iterator iter = src.begin();
	while (iter != src.end())
	{
		if (*iter == c)
			src.erase(iter);
		else
			iter++;
	}
}
//--------------------------------------------------------------------------------------------------------