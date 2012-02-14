#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdint>
#include <vector>
#include <algorithm>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// get number of elements in an array
#define NELEMS(x) (sizeof(x)/sizeof(x[0]))

// generic accessor and mutator methods
#define ROVAR(type, name) \
	const type name() const { return _##name; }

#define RWVAR(type, name) \
	const type name() const { return _##name; }			\
	void name(type val) { _##name = val; }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// trim spaces from a string
static void TrimSpaces(string &str)
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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class ENotAReplayMovie : public exception {
	public:
		virtual const char *what() { return "File is not an Acorn Replay movie"; }
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Standard video formats supported by Replay.
 */
const char * S_VIDEOFORMATS[] = {
	"<< no video >>",
	"Moving Lines",
	"16bpp uncompressed",
	"10bpp YUV, chroma horiz subsampled by 2",
	"8bpp uncompressed",
	"8bpp YUV, chroma H/V subsampled by 2",
	"8bpp YUV, chroma H/V subsampled by 4",
	"Moving Blocks",
	"24bpp uncompressed",
	"16bpp YUV, chroma horiz subsampled by 2 (YUV)",
	"12bpp YUV, chroma H/V subsampled by 2 (YUV)",
	"9bpp YUV, chroma H/V subsampled by 4 (6Y5UV)",
	"Pointer to MPEG movie",
	"MPEG video data stream in ARMovie file",
	"UltiMotion",
	"Indirect video",
	"12bpp YUV chroma H/V subsampled by 2 (6Y5UV)",
	"Moving Blocks HQ (YUV)",
	"H.263 (6Y5UV)",
	"Super Moving Blocks (6Y5UV)",
	"Moving Blocks Beta (6Y6UV)",
	"16bpp YUV Chroma horiz subsampled by 2 (6Y5UV)",
	"12bpp YY8UVd4 chroma horiz subsampled by 2 (6Y5UV)",
	"11bpp 6Y6Y5U5V chroma horiz subsampled by 2 (6Y5UV)",
	"8.25vpp 6Y5UV chroma H/V subsampled by 2 (6Y5UV)",
	"6bpp YYYYd4UVd4 chroma H/V subsampled by 2 (6Y6UV)"
};

/**
 * @brief Sound formats supported by Replay.
 *
 * There are five different Replay sound formats under SoundFormat 1, plus
 * the Custom sound format (SoundFormat 2) which specifies the name of an
 * external CODEC.
 *
 * All SF1 formats support both Mono and Stereo.
 */
typedef enum {
	SOUND_4BIT_ADPCM,				// ADPCM, 4-bit, either mono or stereo
	SOUND_8BIT_LINEAR_SIGNED,		// 8-bit Linear, Signed
	SOUND_8BIT_LINEAR_UNSIGNED,		// 8-bit Linear, Unsigned
	SOUND_8BIT_ULAW,				// 8-bit Exponential mu-Law
	SOUND_16BIT_LINEAR_SIGNED,		// 16-bit Linear, Signed
	SOUND_CUSTOM,					// Custom sound format (use the SoundFormat string)
	SOUND_NONE,						// No audio track
	SOUND_UNKNOWN,					// No idea what type of audio track, or something invalid
} E_SOUND_FORMAT;

/**
 * @brief Colour spaces supported by Replay.
 *
 * Replay supports YUV and RGB colour. Unknown is an option if we couldn't
 * decode the colour space identifier.
 */
typedef enum {
	COLOURSPACE_RGB,
	COLOURSPACE_YUV,
	COLOURSPACE_UNKNOWN
} E_COLOUR_SPACE;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief An entry in a Replay file's chunk catalogue.
 */
typedef struct {
	size_t chunkOffset, videoSize, soundSize;
} ReplayCatalogueEntry;

/**
 * @brief Encapsulation of an Acorn Replay file's chunk catalogue
 */
class ReplayCatalogue {
	private:
		vector<ReplayCatalogueEntry>	entries;

	public:
		/**
		 * Clear the catalogue.
		 */
		void clear();

		/**
		 * Load catalogue.
		 *
		 * Assumes the stream is positioned at the start of the catalogue block.
		 */
		void load(istream &stream, const size_t numChunks);

		/**
		 * Get catalogue size
		 */
		const size_t size() const
		{
			return entries.size();
		}

		/**
		 * Get a catalogue entry
		 */
		const void get(size_t index, size_t *chunkOffset, size_t *videoSize, size_t *soundSize) const
		{
			// TODO: Range Check
			if (chunkOffset)
				*chunkOffset = entries[index].chunkOffset;
			if (videoSize)
				*videoSize = entries[index].videoSize;
			if (soundSize)
				*soundSize = entries[index].soundSize;
		}

		/**
		 * Dump the catalogue to stdout.
		 *
		 * TODO: Allow other streams to be used.
		 */
		const void dump() const
		{
			cout << "Catalogue entries: " << size() << endl;
			cout << "ChunkOf\tVideoSz\tSoundSz" << endl;
			for (size_t i=0; i<size(); i++) {
				cout << entries[i].chunkOffset << "\t"
					<< entries[i].videoSize << "\t"
					<< entries[i].soundSize << endl;
			}
		}
};

void ReplayCatalogue::clear()
{
	entries.clear();
}

void ReplayCatalogue::load(istream &stream, const size_t numChunks)
{
	string st;

	// We're loading new data; clear any existing catalogue data
	clear();

	// Load all the catalogue chunks...
	for (size_t i=0; i<numChunks+1; i++) {
		istringstream ist;
		ReplayCatalogueEntry ent;

		// Get a line, trim the spaces and pull it into the string stream
		std::getline(stream, st); TrimSpaces(st); ist.clear(); ist.str(st);

		// Get the chunk offset, video size and sound size
		ist >> ent.chunkOffset;	ist.ignore(st.size(), ',');
		ist >> ent.videoSize;	ist.ignore(st.size(), ';');
		ist >> ent.soundSize;

		// Store the chunk offset and size
		entries.push_back(ent);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Encapsulation of an Acorn Replay file.
 */
class ReplayFile {
	private:
		string			_movieName, _copyright, _author;
		uint32_t		_iVideoFormat;
		string			_sVideoFormat;
		vector<string>	_videoFormatParams;
		uint32_t		_xSize, _ySize;
		float			_xAspect, _yAspect;
		uint32_t		_bpp;
		E_COLOUR_SPACE	_colourSpace;
		float			_fps;
		uint32_t		_iSoundFormat;
		string			_sCustomSoundFormat;
		E_SOUND_FORMAT	_soundFormat;
		float			_soundSampleRate;
		uint32_t		_iSoundChannels;
		uint32_t		_iSoundBitsPerSample;
		uint32_t		_iFramesPerChunk;
		size_t			_iNumberOfChunks, _iEvenChunkSize, _iOddChunkSize;
		off_t			_oCatalogueOffset, _oSpriteOffset;
		size_t			_iSpriteSize;
		ssize_t			_iKeyframes;
		ReplayCatalogue	_catalogue;

		E_SOUND_FORMAT decodeSoundFormat(string formatString);

	public:
		ROVAR(string,			movieName);				///< Movie name
		ROVAR(string,			copyright);				///< Copyright
		ROVAR(string,			author);				///< Author
		ROVAR(uint32_t,			iVideoFormat);			///< Video format ID
		ROVAR(string,			sVideoFormat);			///< Video format string
		ROVAR(uint32_t,			xSize);					///< Horizontal pixel size
		ROVAR(uint32_t,			ySize);					///< Vertical pixel size
		ROVAR(uint32_t,			xAspect);				///< Horizontal component of aspect ratio
		ROVAR(uint32_t,			yAspect);				///< Vertical component of aspect ratio
		ROVAR(uint32_t,			bpp);					///< Bits per pixel
		ROVAR(E_COLOUR_SPACE,	colourSpace);			///< Colour space
		ROVAR(float,			fps);					///< Frames per second
		ROVAR(uint32_t,			iSoundFormat);			///< Sound format value -- TODO: REMOVEME!
		ROVAR(string,			sCustomSoundFormat);	///< Custom sound format -- only valid if soundFormat == SOUND_CUSTOM
		ROVAR(E_SOUND_FORMAT,	soundFormat);			///< Sound format value, decoded
		ROVAR(float,			soundSampleRate);		///< Sound sample rate
		ROVAR(uint32_t,			iSoundChannels);		///< Number of sound channels (1=mono, 2=stereo, 0=no audio)
		ROVAR(uint32_t,			iSoundBitsPerSample);	///< Bits per sound sample
		ROVAR(uint32_t,			iFramesPerChunk);		///< Frames per A/V chunk
		ROVAR(size_t,			iNumberOfChunks);		///< Number of complete A/V chunks
		// Even and odd chunk size skipped, we don't need them
		// Catalogue and sprite offsets skipped, not required either.
		// No, we don't even provide the sprite size and keyframe table offset...
		ROVAR(ReplayCatalogue,	catalogue);

		ReplayFile();
		ReplayFile(istream &stream);
		void dump(void);
};

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Constructor: empty ReplayFile
 */
ReplayFile::ReplayFile()
{
	_xAspect = _yAspect = 1;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Constructor: load a ReplayFile from a stream
 */
ReplayFile::ReplayFile(istream &stream)
{
	string st;
	istringstream ist;

	// Line 1: ARMovie [fixed]
	getline(stream, st);
	TrimSpaces(st);
	if (st.compare("ARMovie") != 0) {
		throw ENotAReplayMovie();
	}

	_xAspect = _yAspect = 1;

	// movie information
	std::getline(stream, _movieName);	TrimSpaces(_movieName);
	std::getline(stream, _copyright);	TrimSpaces(_copyright);
	std::getline(stream, _author);		TrimSpaces(_author);

	// video compression format
	std::getline(stream, _sVideoFormat); TrimSpaces(_sVideoFormat); ist.str(_sVideoFormat);
	ist >> _iVideoFormat;
	// TODO: decode the compression format parameter list

	// X size in pixels
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _xSize;
	// TODO: decode optional aspect ratio

	// Y size in pixels
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _ySize;

	// Pixel depth in bits
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
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
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _fps;
	// TODO: decode optional start timecode

	// Sound compression format
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iSoundFormat;

	// Sound rate in Hz
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _soundSampleRate;

	// Number of audio channels
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iSoundChannels;

	// Bits per sound sample and sound format
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
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
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iFramesPerChunk;

	// Chunk count
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iNumberOfChunks;

	// Even chunk size
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iEvenChunkSize;

	// Odd chunk size
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iOddChunkSize;

	// Catalogue offset
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _oCatalogueOffset;

	// Offset to sprite
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _oSpriteOffset;

	// Size of sprite
	std::getline(stream, st); TrimSpaces(st); ist.str(st);
	ist >> _iSpriteSize;

	// Keyframe position (ONLY FOR VIDEO!)
	// Set to -1 for "no keys"
	if (_iVideoFormat != 0) {
		std::getline(stream, st); TrimSpaces(st); ist.str(st);
		ist >> _iKeyframes;
	} else {
		_iKeyframes = -1;
	}

	// Now load the catalogue
	stream.seekg(_oCatalogueOffset, ios_base::beg);
	_catalogue.load(stream, _iNumberOfChunks);

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
}

///////////////////////////////////////////////////////////////////////////////

void ReplayFile::dump(void)
{
	cout << "Movie name:   [" << _movieName << "]" << endl;
	cout << "Copyright:    [" << _copyright << "]" << endl;
	cout << "Author:       [" << _author << "]" << endl;
	cout << "Video format: [" << _sVideoFormat << "], fmt id " << _iVideoFormat << endl << "\t";
	if (_iVideoFormat == 0) {
		cout << "<< no video >>" << endl;
	} else {
		if (_iVideoFormat < NELEMS(S_VIDEOFORMATS))
			cout << S_VIDEOFORMATS[_iVideoFormat] << endl;
		else if ((_iVideoFormat >= 100) && (_iVideoFormat < 200))
			cout << "EIDOS "					<< _iVideoFormat << endl;
		else if ((_iVideoFormat >= 200) && (_iVideoFormat < 300))
			cout << "Irlam Instruments "		<< _iVideoFormat << endl;
		else if ((_iVideoFormat >= 300) && (_iVideoFormat < 400))
			cout << "Wild Vision "				<< _iVideoFormat << endl;
		else if ((_iVideoFormat >= 400) && (_iVideoFormat < 500))
			cout << "Aspex Software "			<< _iVideoFormat << endl;
		else if ((_iVideoFormat >= 500) && (_iVideoFormat < 600))
			cout << "Iota Software "			<< _iVideoFormat << endl;
		else if ((_iVideoFormat >= 600) && (_iVideoFormat < 700))
			cout << "Warm Silence Software "	<< _iVideoFormat << endl;
		else if ((_iVideoFormat >= 800) && (_iVideoFormat < 809))
			cout << "Henrik Bjerregard Pedersen [small_users block] " << _iVideoFormat << endl;
		else if ((_iVideoFormat >= 900) && (_iVideoFormat < 1000))
			cout << "Innovative Media Solutions " << _iVideoFormat << endl;
		else
			cout << "Unknown ID " << _iVideoFormat << endl;

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

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Decode a sound format string (the bits-per-sample string)
 *
 * @param formatString	Sound format / bits per sample string
 * @returns an E_SOUND_FORMAT tag representing the audio format.
 */
E_SOUND_FORMAT ReplayFile::decodeSoundFormat(string formatString)
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

///////////////////////////////////////////////////////////////////////////////
//          ///////////////////////////////////////////////////////////////////
//  main()  ///////////////////////////////////////////////////////////////////
//          ///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	if (argc < 2) {
		cerr << "syntax: demux ARMovieFilename" << endl;
		return -1;
	}

	ifstream moviestream(argv[1], ifstream::in);

	ReplayFile arh(moviestream);
	arh.dump();

/*
	std::getline(moviehdr, st);
	cout << "line: [" << st << "]" << endl;

	istringstream strm(st);
	int y;
	strm >> y;
	cout << y << endl;
*/
}
