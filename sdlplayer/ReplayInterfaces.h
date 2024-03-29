#ifndef _ReplayInterfaces_H
#define _ReplayInterfaces_H

#include <iostream>
#include <string>

class IDemuxer {
	public:
		IDemuxer();
		IDemuxer(std::istream *strm);
		virtual ~IDemuxer() {};
		virtual void connect(std::istream *strm);
		virtual std::string getType(void)								=0;
		virtual size_t getVideo(const size_t bufsize, char *buf)		=0;
		virtual size_t getAudio(const size_t bufsize, char *buf)		=0;

	protected:
		std::istream *_stream;
};

class IVideoDecoder {
	public:
		IVideoDecoder();
		IVideoDecoder(IDemuxer *demux);
		virtual ~IVideoDecoder() {};
		virtual void connect(IDemuxer *demux);
		virtual std::string getType(void)								=0;
		virtual size_t getNextFrame(const size_t bufsize, char *buf)	=0;

	protected:
		IDemuxer *_demux;
};

#endif // _ReplayInterfaces_H
