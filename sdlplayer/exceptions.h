#ifndef _EXCEPTIONS_H
#define _EXCEPTIONS_H

#include <exception>

class ENotAReplayMovie : public std::exception {
	public:
		virtual const char *what() { return "File is not an Acorn Replay movie"; }
};

#endif // _EXCEPTIONS_H
