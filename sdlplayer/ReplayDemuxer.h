#ifndef _ReplayDemuxer_H
#define _ReplayDemuxer_H

#include <iostream>
#include <string>
#include "ReplayInterfaces.h"

class ReplayDemuxer : public IDemuxer {
	public:
		ReplayDemuxer();
		ReplayDemuxer(std::istream *strm);
		std::string getType(void) { return "ReplayDemuxer"; };
		size_t getVideo(const size_t bufsize, char *buf);
		size_t getAudio(const size_t bufsize, char *buf);
};

#endif // _ReplayDemuxer_H
