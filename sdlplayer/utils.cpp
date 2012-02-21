#include <string>
#include "utils.h"

using namespace std;

void TrimSpaces(std::string &str)
{
	// Trim Both leading and trailing spaces
	size_t startpos = str.find_first_not_of(" \t"); // Find the first character position after excluding leading blank spaces
	size_t endpos = str.find_last_not_of(" \t"); // Find the first character position from reverse af

	// if all spaces or empty return an empty string
	if ((string::npos == startpos) || (string::npos == endpos)) {
		str = "";
	} else {
		str = str.substr(startpos, endpos-startpos+1);
	}
}

