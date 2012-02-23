#ifndef _ReplayDemuxer_H
#define _ReplayDemuxer_H

#include <iostream>
#include <string>
#include "utils.h"
#include "ReplayCatalogue.h"
#include "ReplayCodecManager.h"
#include "ReplayInterfaces.h"

class ReplayDemuxer : public IDemuxer {
	public:
		ROVAR(std::string,			movieName);				///< Movie name
		ROVAR(std::string,			copyright);				///< Copyright
		ROVAR(std::string,			author);				///< Author
		ROVAR(int,					iVideoFormat);			///< Video format ID
		ROVAR(std::string,			sVideoFormat);			///< Video format std::string
		ROVAR(int,					xSize);					///< Horizontal pixel size
		ROVAR(int,					ySize);					///< Vertical pixel size
		ROVAR(int,					xAspect);				///< Horizontal component of aspect ratio
		ROVAR(int,					yAspect);				///< Vertical component of aspect ratio
		ROVAR(int,					bpp);					///< Bits per pixel
		ROVAR(REPLAY_COLOUR_SPACE,	colourSpace);			///< Colour space
		ROVAR(float,				fps);					///< Frames per second
		ROVAR(int,					iSoundFormat);			///< Sound format value -- TODO: REMOVEME!
		ROVAR(std::string,			sCustomSoundFormat);	///< Custom sound format -- only valid if soundFormat == SOUND_CUSTOM
		ROVAR(REPLAY_SOUND_FORMAT,	soundFormat);			///< Sound format value, decoded
		ROVAR(float,				soundSampleRate);		///< Sound sample rate
		ROVAR(int,					iSoundChannels);		///< Number of sound channels (1=mono, 2=stereo, 0=no audio)
		ROVAR(int,					iSoundBitsPerSample);	///< Bits per sound sample
		ROVAR(int,					iFramesPerChunk);		///< Frames per A/V chunk
		ROVAR(size_t,				iNumberOfChunks);		///< Number of complete A/V chunks
		// Even and odd chunk size skipped, we don't need them
		// Catalogue and sprite offsets skipped, not required either.
		// No, we don't even provide the sprite size and keyframe table offset...
		ROVAR(ReplayCatalogue,	catalogue);

		ReplayDemuxer();
		ReplayDemuxer(std::istream *strm);
		std::string getType(void) { return "ReplayDemuxer"; };
		size_t getVideo(const size_t bufsize, char *buf);
		size_t getAudio(const size_t bufsize, char *buf);

	private:
		std::string					_movieName, _copyright, _author;
		int							_iVideoFormat;
		std::string					_sVideoFormat;
		std::vector<std::string>	_videoFormatParams;
		int							_xSize, _ySize;
		float						_xAspect, _yAspect;
		int							_bpp;
		REPLAY_COLOUR_SPACE			_colourSpace;
		float						_fps;
		int							_iSoundFormat;
		std::string					_sCustomSoundFormat;
		REPLAY_SOUND_FORMAT			_soundFormat;
		float						_soundSampleRate;
		int							_iSoundChannels;
		int							_iSoundBitsPerSample;
		int							_iFramesPerChunk;
		size_t						_iNumberOfChunks, _iEvenChunkSize, _iOddChunkSize;
		off_t						_oCatalogueOffset, _oSpriteOffset;
		size_t						_iSpriteSize;
		ssize_t						_iKeyframes;
		ReplayCatalogue				_catalogue;

		void LoadReplayHeader(void);
		void DumpDebugInfo(void);
		REPLAY_SOUND_FORMAT decodeSoundFormat(std::string formatstring);
};

#endif // _ReplayDemuxer_H
