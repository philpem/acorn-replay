#include <iostream>
#include <string>
#include "ReplayCatalogue.h"

using namespace std;

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

const size_t ReplayCatalogue::size() const
{
	return entries.size();
}

const void ReplayCatalogue::get(size_t index, size_t *chunkOffset, size_t *videoSize, size_t *soundSize) const
{
	// TODO: Range Check
	if (chunkOffset)
		*chunkOffset = entries[index].chunkOffset;
	if (videoSize)
		*videoSize = entries[index].videoSize;
	if (soundSize)
		*soundSize = entries[index].soundSize;
}

const void ReplayCatalogue::dump() const
{
	cout << "Catalogue entries: " << size() << endl;
	cout << "ChunkOf\tVideoSz\tSoundSz" << endl;
	for (size_t i=0; i<size(); i++) {
		cout << entries[i].chunkOffset << "\t"
			<< entries[i].videoSize << "\t"
			<< entries[i].soundSize << endl;
	}
}

