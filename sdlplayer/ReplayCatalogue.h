#ifndef _ReplayCatalogue_H
#define _ReplayCatalogue_H

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
		 * @brief Get catalogue size
		 */
		const size_t size() const;

		/**
		 * @brief Get a catalogue entry
		 */
		const void get(size_t index, size_t *chunkOffset, size_t *videoSize, size_t *soundSize) const;

		/**
		 * Dump the catalogue to stdout.
		 *
		 * TODO: Allow other streams to be used.
		 */
		const void dump() const;
};

#endif // _ReplayCatalogue_H
