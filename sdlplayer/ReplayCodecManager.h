#ifndef _ReplayCodecManager_H
#define _ReplayCodecManager_H

#include <map>
#include <string>

/**
 * @file
 */

namespace replay {

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
} REPLAY_SOUND_FORMAT;

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
} REPLAY_COLOUR_SPACE;


class ReplayCodecManager {
	public:
		/// Return instance of ReplayCodecManager singleton
		static ReplayCodecManager *inst();
		~ReplayCodecManager();

		/**
		 * @brief Convert a video codec ID to a codec name.
		 * @param id  The ID number of the video codec being queried (video format ID).
		 * @returns   A string containing the name of the codec, or an empty string if the codec is not known.
		 */
		const std::string getVideoCodecName(int id) const;

	protected:
		ReplayCodecManager();

	private:
		static ReplayCodecManager *pInstance;

		/// List of standard video formats supported by Replay.
		std::map<int, std::string>	M_VIDEOFORMATS;

		/// List of registered video decompressors
};

} // namespace replay

#endif // _ReplayCodecManager_H

