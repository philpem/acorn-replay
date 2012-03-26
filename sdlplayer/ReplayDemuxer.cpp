#include <iostream>
#include <sstream>
#include <algorithm>
#include "exceptions.h"
#include "ReplayInterfaces.h"
#include "ReplayDemuxer.h"

using namespace std;

ReplayDemuxer::ReplayDemuxer() : IDemuxer()
{
	// empty ctor shouldn't try and load the Replay header...!
	// TODO: connect method
}

ReplayDemuxer::ReplayDemuxer(std::istream *strm) : IDemuxer(strm)
{
	LoadReplayHeader();
}

void ReplayDemuxer::LoadReplayHeader(void)
{
	string st;
	istringstream ist;

	// Line 1: ARMovie [fixed]
	getline(*_stream, st);
	TrimSpaces(st);
	if (st.compare("ARMovie") != 0) {
		throw ENotAReplayMovie();
	}

	_xAspect = _yAspect = 1;

	// movie information
	std::getline(*_stream, _movieName);	TrimSpaces(_movieName);
	std::getline(*_stream, _copyright);	TrimSpaces(_copyright);
	std::getline(*_stream, _author);		TrimSpaces(_author);

	// video compression format
	std::getline(*_stream, _sVideoFormat); TrimSpaces(_sVideoFormat); ist.str(_sVideoFormat);
	ist >> _iVideoFormat;
	// TODO: decode the compression format parameter list

	// X size in pixels
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _xSize;
	// TODO: decode optional aspect ratio

	// Y size in pixels
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _ySize;

	// Pixel depth in bits
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _bpp;
	// This line also encodes the colour space type (YUV or RGB)
	std::transform(st.begin(), st.end(), st.begin(), ::toupper);
	if (st.find("YUV") != string::npos) {
		_colourSpace = COLOURSPACE_YUV;
	} else if (st.find("RGB") != string::npos) {
		_colourSpace = COLOURSPACE_RGB;
	} else {
		_colourSpace = COLOURSPACE_UNKNOWN;
	}

	// Number of frames per second
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _fps;
	// TODO: decode optional start timecode

	// Sound compression format
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iSoundFormat;

	// Sound rate in Hz
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _soundSampleRate;

	// Number of audio channels
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iSoundChannels;

	// Bits per sound sample and sound format
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iSoundBitsPerSample;
	_sCustomSoundFormat = "";
	if (_iSoundFormat == 0) {
		// Sound Format 0 -- no sound
		_iSoundBitsPerSample = 0;
		_soundFormat = SOUND_NONE;
	} else if (_iSoundFormat == 1) {
		// Sound Format 1 -- MOVING_LINES decompressor format
		_soundFormat = decodeSoundFormat(st);
	} else if (_iSoundFormat == 2) {
		// Sound Format 2 -- custom audio stream format
		_sCustomSoundFormat = st.substr(st.find(" ")+1, string::npos);
		_soundFormat = SOUND_CUSTOM;
	} else {
		// Unknown sound format (not defined by the Replay spec!)
		_soundFormat = SOUND_UNKNOWN;
	}

	// Frames per chunk
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iFramesPerChunk;

	// Chunk count
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iNumberOfChunks;

	// Even chunk size
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iEvenChunkSize;

	// Odd chunk size
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iOddChunkSize;

	// Catalogue offset
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _oCatalogueOffset;

	// Offset to sprite
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _oSpriteOffset;

	// Size of sprite
	std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iSpriteSize;

	// Keyframe position (ONLY FOR VIDEO!)
	// Set to -1 for "no keys"
	if (_iVideoFormat != 0) {
		std::getline(*_stream, st); TrimSpaces(st); ist.str(st);
		ist >> _iKeyframes;
	} else {
		_iKeyframes = -1;
	}

	// Now load the catalogue
	_stream->seekg(_oCatalogueOffset, ios_base::beg);
	_catalogue.load(*_stream, _iNumberOfChunks);

#if 0
	ofstream videostream("video", ios::out | ios::binary | ios::trunc);
	ofstream soundstream("sound", ios::out | ios::binary | ios::trunc);
	for (size_t i=0; i<catalogue().size(); i++) {
		size_t chunkofs, vidsz, sndsz;
		catalogue().get(i, &chunkofs, &vidsz, &sndsz);
		stream.seekg(chunkofs, ios_base::beg);
		if (vidsz > 0) {
			char vbuf[vidsz];
			stream.read(vbuf, vidsz);
			videostream.write(vbuf, vidsz);
		}
		if (sndsz > 0) {
			char sbuf[sndsz];
			stream.read(sbuf, sndsz);
			soundstream.write(sbuf, sndsz);
		}
	}
	videostream.close(); soundstream.close();
#endif
}

void ReplayDemuxer::DumpDebugInfo(void)
{
	if (_stream == NULL) return;	// TODO: throw exception
	// TODO: check if header has been loaded?

	cout << "Movie name:   [" << _movieName << "]" << endl;
	cout << "Copyright:    [" << _copyright << "]" << endl;
	cout << "Author:       [" << _author << "]" << endl;
	cout << "Video format: [" << _sVideoFormat << "], fmt id " << _iVideoFormat << endl << "\t";
	if (_iVideoFormat == 0) {
		cout << "<< no video >>" << endl;
	} else {
		cout << ReplayCodecManager::inst()->getVideoCodecName(_iVideoFormat) << " (format code " << _iVideoFormat << ")" << endl;

		switch (_colourSpace) {
			case COLOURSPACE_RGB:	cout << "\tColour space: RGB" << endl; break;
			case COLOURSPACE_YUV:	cout << "\tColour space: YUV" << endl; break;
			default:				cout << "\t** Unknown or unspecified colour space **" << endl; break;
		}
	}

	if (_videoFormatParams.size() > 0) {
		// TODO: print format parameters
		cout << "\t" << "Have format parameters..." << endl;
	}
	cout << "Pixel size:   " << _xSize << " * " << _ySize << endl;
	cout << "Pixel aspect: " << _xAspect << " : " << _yAspect << endl;
	cout << _bpp << " bits per pixel" << endl;
	cout << _fps << " frames per second" << endl;
	
	cout << "Sound format: [" << _sCustomSoundFormat << "], fmt id " << _iSoundFormat << endl;
	if (_iSoundFormat == 0) {
		cout << "\t<< no audio >>" << endl;
	} else {
		cout << "Sound:        " << _soundSampleRate << " Hz, "
			<< _iSoundChannels << " channels, "
			<< _iSoundBitsPerSample << " bits/sample"
			<< endl;
		if (_iSoundFormat == 1) {
			cout << "\t";
			switch (_soundFormat) {
				case SOUND_4BIT_ADPCM:
					cout << "ADPCM" << endl;
					break;
				case SOUND_8BIT_LINEAR_SIGNED:
				case SOUND_16BIT_LINEAR_SIGNED:
					cout << "Linear signed" << endl;
					break;
				case SOUND_8BIT_LINEAR_UNSIGNED:
					cout << "Linear unsigned" << endl;
					break;
				case SOUND_8BIT_ULAW:
					cout << "mu-Law" << endl;
					break;
				default:
					cout << "\t** Unknown compression format " << _iSoundFormat << "!!!" << endl; break;
			}
		}
	}

	cout << "Chunk info:   "
		<< _iNumberOfChunks << " chunks, "
		<< _iFramesPerChunk << " frames per chunk, "
		<< _iEvenChunkSize  << " bytes per even chunk, "
		<< _iOddChunkSize   << " bytes per odd chunk"
		<< endl;
	cout << "Catalogue at: " << _oCatalogueOffset << endl;
	cout << "Sprite:       offset " << _oSpriteOffset << ", " << _iSpriteSize << " bytes in length" << endl;
	cout << endl;

	catalogue().dump();
}

/**
 * @brief Decode a sound format string (the bits-per-sample string)
 *
 * @param formatString	Sound format / bits per sample string
 * @returns an E_SOUND_FORMAT tag representing the audio format.
 */
REPLAY_SOUND_FORMAT ReplayDemuxer::decodeSoundFormat(string formatString)
{
	string str;
	istringstream ist;
	int bits;

	// convert format string to upper case
	str.resize(formatString.size());
	std::transform(formatString.begin(), formatString.end(), str.begin(), ::toupper);

	// get bits per sample
	ist.str(formatString);
	ist >> bits;

	// 0 bits means no sound!
	if (bits == 0) return SOUND_NONE;

	// check for linear, ADPCM or exponential
	if (str.find("LIN") != string::npos) {
		// Linear -- check for signedness
		if (str.find("UNSIGN") != string::npos) {
			// Linear unsigned
			switch (bits) {
				case 8:  return SOUND_8BIT_LINEAR_UNSIGNED;
				case 16:    // 16 Linear Unsigned is not valid per the Acorn Replay spec!
				default: return SOUND_UNKNOWN;
			}
		} else {
			// Linear signed
			switch (bits) {
				case 8:  return SOUND_8BIT_LINEAR_SIGNED;
				case 16: return SOUND_16BIT_LINEAR_SIGNED;
				default: return SOUND_UNKNOWN;
			}
		}
	} else if ((bits == 4) || (str.find("ADPCM") != string::npos)) {
		// 4-bit Linear ADPCM
		return SOUND_4BIT_ADPCM;
	} else {
		// must be 8-bit Exponential mu-Law / u-Law then...
		return SOUND_8BIT_ULAW;
	}
}


size_t ReplayDemuxer::getVideo(const size_t bufsize, char *buf)
{
	return 0;	// NO DATA!
}

size_t ReplayDemuxer::getAudio(const size_t bufsize, char *buf)
{
	return 0;	// NO DATA!
}
