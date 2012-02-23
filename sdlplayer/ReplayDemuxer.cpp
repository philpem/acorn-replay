#include <iostream>
#include "ReplayInterfaces.h"
#include "ReplayDemuxer.h"

using namespace std;

ReplayDemuxer::ReplayDemuxer()
{
//	LoadReplayHeader();
}

ReplayDemuxer::ReplayDemuxer(std::istream *strm)
{
//	LoadReplayHeader();
}

size_t ReplayDemuxer::getVideo(const size_t bufsize, char *buf)
{
	return 0;	// NO DATA!
}

size_t ReplayDemuxer::getAudio(const size_t bufsize, char *buf)
{
	return 0;	// NO DATA!
}
