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

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// trim spaces from a string
void TrimSpaces(string &str)
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
	"16bpp YUV, chroma horiz subsampled by 2",
	"12bpp YUV, chroma H/V subsampled by 2",
	"9bpp YUV, chroma H/V subsampled by 4",
	"Pointer to MPEG movie",
	"MPEG video data stream in ARMovie file",
	"UltiMotion",
	"Indirect video",
	"12bpp YUV chroma H/V subsampled by 2",
	"Moving Blocks HQ",
	"H.263",
	"Super Moving Blocks",
	"Moving Blocks Beta",
	"16bpp YUV Chroma horiz subsampled by 2",
	"12bpp YY8UVd4 chroma horiz subsampled by 2",
	"11bpp 6Y6Y5U5V chroma horiz subsampled by 2",
	"8.25vpp 6Y5UV chroma H/V subsampled by 2",
	"6bpp YYYYd4UVd4 chroma H/V subsampled by 2"
};

// There are five different Replay sound formats under SoundFormat 1, plus
// the Custom sound format (SoundFormat 2).
// All SF1 formats support both Mono and Stereo.
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

// Replay supports YUV and RGB colour
typedef enum {
	COLOURSPACE_RGB,
	COLOURSPACE_YUV,
	COLOURSPACE_UNKNOWN
} E_COLOUR_SPACE;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Decode a sound format string (the bits-per-sample string)
 *
 * @param formatString	Sound format / bits per sample string
 * @returns an E_SOUND_FORMAT tag representing the audio format.
 */
E_SOUND_FORMAT decodeSoundFormat(string formatString)
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
///////////////////////////////////////////////////////////////////////////////

class ReplayCatalogue {

	vector<size_t>		chunkOffsets, videoSizes, soundSizes;

	// load catalogue
	ReplayCatalogue(istream &stream, size_t numChunks)
	{
		// TODO implement this
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class ReplayHeader {
	public:
		string			movieName, copyright, author;
		uint32_t		iVideoFormat;
		string			sVideoFormat;
		vector<string>	videoFormatParams;
		uint32_t		xSize, ySize;
		float			xAspect, yAspect;
		uint32_t		bpp;
		E_COLOUR_SPACE	colourSpace;
		float			fps;
		uint32_t		iSoundFormat;
		string			sCustomSoundFormat;
		E_SOUND_FORMAT	soundFormat;
		float			soundSampleRate;
		uint32_t		iSoundChannels;
		uint32_t		iSoundBitsPerSample;
		uint32_t		iFramesPerChunk;
		size_t			iNumberOfChunks, iEvenChunkSize, iOddChunkSize;
		off_t			oCatalogueOffset, oSpriteOffset;
		size_t			iSpriteSize;
		ssize_t			iKeyframes;

		ReplayHeader()
		{
			xAspect = yAspect = 1;
		}

		ReplayHeader(istream &stream)
		{
			string st;
			istringstream ist;

			// Line 1: ARMovie [fixed]
			getline(stream, st);
			TrimSpaces(st);
			if (st.compare("ARMovie") != 0) {
				throw ENotAReplayMovie();
			}

			xAspect = yAspect = 1;

			// movie information
			std::getline(stream, movieName);	TrimSpaces(movieName);
			std::getline(stream, copyright);	TrimSpaces(copyright);
			std::getline(stream, author);		TrimSpaces(author);

			// video compression format
			std::getline(stream, sVideoFormat); TrimSpaces(sVideoFormat); ist.str(sVideoFormat);
			ist >> iVideoFormat;
			// TODO: decode the compression format parameter list

			// X size in pixels
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> xSize;
			// TODO: decode optional aspect ratio

			// Y size in pixels
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> ySize;

			// Pixel depth in bits
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> bpp;
			// This line also encodes the colour space type (YUV or RGB)
			std::transform(st.begin(), st.end(), st.begin(), ::toupper);
			if (st.find("YUV") != string::npos) {
				colourSpace = COLOURSPACE_YUV;
			} else if (st.find("RGB") != string::npos) {
				colourSpace = COLOURSPACE_RGB;
			} else {
				colourSpace = COLOURSPACE_UNKNOWN;
			}

			// Number of frames per second
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> fps;
			// TODO: decode optional start timecode

			// Sound compression format
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> iSoundFormat;

			// Sound rate in Hz
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> soundSampleRate;

			// Number of audio channels
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> iSoundChannels;

			// Bits per sound sample and sound format
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> iSoundBitsPerSample;
			sCustomSoundFormat = "";
			if (iSoundFormat == 0) {
				// Sound Format 0 -- no sound
				iSoundBitsPerSample = 0;
				soundFormat = SOUND_NONE;
			} else if (iSoundFormat == 1) {
				// Sound Format 1 -- MOVING_LINES decompressor format
				soundFormat = decodeSoundFormat(st);
			} else if (iSoundFormat == 2) {
				// Sound Format 2 -- custom audio stream format
				sCustomSoundFormat = st.substr(st.find(" ")+1, string::npos);
				soundFormat = SOUND_CUSTOM;
			} else {
				// Unknown sound format (not defined by the Replay spec!)
				soundFormat = SOUND_UNKNOWN;
			}

			// Frames per chunk
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> iFramesPerChunk;

			// Chunk count
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> iNumberOfChunks;

			// Even chunk size
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> iEvenChunkSize;

			// Odd chunk size
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> iOddChunkSize;

			// Catalogue offset
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> oCatalogueOffset;

			// Offset to sprite
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> oSpriteOffset;

			// Size of sprite
			std::getline(stream, st); TrimSpaces(st); ist.str(st);
			ist >> iSpriteSize;

			// Keyframe position (ONLY FOR VIDEO!)
			// Set to -1 for "no keys"
			if (iVideoFormat != 0) {
				std::getline(stream, st); TrimSpaces(st); ist.str(st);
				ist >> iKeyframes;
			} else {
				iKeyframes = -1;
			}
		}

		void dump(void);
};

void ReplayHeader::dump(void)
{
	cout << "Movie name:   [" << movieName << "]" << endl;
	cout << "Copyright:    [" << copyright << "]" << endl;
	cout << "Author:       [" << author << "]" << endl;
	cout << "Video format: [" << sVideoFormat << "], fmt id " << iVideoFormat << endl << "\t";
	if (iVideoFormat == 0) {
		cout << "<< no video >>" << endl;
	} else {
		if (iVideoFormat < NELEMS(S_VIDEOFORMATS))
			cout << S_VIDEOFORMATS[iVideoFormat] << endl;
		else if ((iVideoFormat >= 100) && (iVideoFormat < 200))
			cout << "EIDOS " << iVideoFormat << endl;
		else if ((iVideoFormat >= 200) && (iVideoFormat < 300))
			cout << "Irlam Instruments " << iVideoFormat << endl;
		else if ((iVideoFormat >= 300) && (iVideoFormat < 400))
			cout << "Wild Vision " << iVideoFormat << endl;
		else if ((iVideoFormat >= 400) && (iVideoFormat < 500))
			cout << "Aspex Software " << iVideoFormat << endl;
		else if ((iVideoFormat >= 500) && (iVideoFormat < 600))
			cout << "Iota Software " << iVideoFormat << endl;
		else if ((iVideoFormat >= 600) && (iVideoFormat < 700))
			cout << "Warm Silence Software " << iVideoFormat << endl;
		else if ((iVideoFormat >= 900) && (iVideoFormat < 800))
			cout << "Innovative Media Solutions " << iVideoFormat << endl;
		else
			cout << "Unknown ID " << iVideoFormat << endl;

		switch (colourSpace) {
			case COLOURSPACE_RGB:	cout << "\tColour space: RGB" << endl; break;
			case COLOURSPACE_YUV:	cout << "\tColour space: YUV" << endl; break;
			default:				cout << "\t** Unknown or unspecified colour space **" << endl; break;
		}
	}

	if (videoFormatParams.size() > 0) {
		// TODO: print format parameters
		cout << "\t" << "Have format parameters..." << endl;
	}
	cout << "Pixel size:   " << xSize << " * " << ySize << endl;
	cout << "Pixel aspect: " << xAspect << " : " << yAspect << endl;
	cout << bpp << " bits per pixel" << endl;
	cout << fps << " frames per second" << endl;
	
	cout << "Sound format: [" << sCustomSoundFormat << "], fmt id " << iSoundFormat << endl;
	if (iSoundFormat == 0) {
		cout << "\t<< no audio >>" << endl;
	} else {
		cout << "Sound:        " << soundSampleRate << " Hz, "
			<< iSoundChannels << " channels, "
			<< iSoundBitsPerSample << " bits/sample"
			<< endl;
		if (iSoundFormat == 1) {
			cout << "\t";
			switch (soundFormat) {
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
					cout << "\t** Unknown compression format " << iSoundFormat << "!!!" << endl; break;
			}
		}
	}

	cout << "Chunk info:   "
		<< iNumberOfChunks << " chunks, "
		<< iFramesPerChunk << " frames per chunk, "
		<< iEvenChunkSize  << " bytes per even chunk, "
		<< iOddChunkSize   << " bytes per odd chunk"
		<< endl;
	cout << "Catalogue at: " << oCatalogueOffset << endl;
	cout << "Sprite:       offset " << oSpriteOffset << ", " << iSpriteSize << " bytes in length" << endl;
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

	ReplayHeader arh(moviestream);
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
