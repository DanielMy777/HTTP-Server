#include "SystemOperations.h"
//--------------------------------------------------------------------------------------------------------
bool SystemOperations::doesFileExist(const char* path)
{
	ifstream f(path);
	return f.good();
}
//--------------------------------------------------------------------------------------------------------
string SystemOperations::readFile(const char* path)
{
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();

	return buffer.str();
}
//--------------------------------------------------------------------------------------------------------