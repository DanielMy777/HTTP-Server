#pragma once
#include "Utilities.h"

class SystemOperations
{
public:
	// Returns true iff a file at ROOT_DIR/path exists.
	static bool doesFileExist(const char* path);
	// Return the contents of a file in ROOT_DIR/path (returns an empty string if doesnt exist).
	static string readFile(const char* path);
};