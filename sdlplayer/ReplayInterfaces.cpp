#include "ReplayInterfaces.h"

#include <iostream>

using namespace std;

IDemuxer::IDemuxer() : _stream(NULL) {}

IDemuxer::IDemuxer(istream *stream)
{
	this->connect(stream);
}

void IDemuxer::connect(istream *stream)
{
	_stream = stream;
}


IVideoDecoder::IVideoDecoder() : _demux(NULL) {}

IVideoDecoder::IVideoDecoder(IDemuxer *demux)
{
	this->connect(demux);
}

void IVideoDecoder::connect(IDemuxer *demux)
{
	_demux = demux;
}


