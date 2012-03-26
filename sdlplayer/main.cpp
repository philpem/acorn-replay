#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include "SDL.h"

#include "utils.h"
#include "exceptions.h"
#include "ReplayCodecManager.h"
#include "ReplayDemuxer.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CONFIGURATION CONSTANTS
/////////////////////////////////////////////////////////////////////////////

const size_t NBUFFERS		= 4;	///< Number of frames to buffer

/////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
/////////////////////////////////////////////////////////////////////////////

/// Frame buffers
Uint32 *framebuffer[NBUFFERS];

/// Frame buffer head and tail
size_t fb_head = 0, fb_tail = 0;

/// Global option block
struct opts_t {
	Uint16		w;					///< Video width
	Uint16		h;					///< Video height
	float		framerate;			///< Frame rate (fps)
	int			framedelay;			///< Milliseconds per frame
} opts;

/// Set true to quit the player.
bool quit;

/////////////////////////////////////////////////////////////////////////////
// STATIC FUNCTIONS
/////////////////////////////////////////////////////////////////////////////

// Producer thread
static int decodeThread(void *data)
{
	// Convert private-data pointer to something useful
	istream *in = static_cast<istream*>(data);

	while (!quit) {
		// 

		// advance write pointer
		fb_tail++;
		SDL_Delay(1000);
	}

	// success
	return 0;
}

// Consumer thread
static int displayThread(void *data)
{
	Uint32 ticks = SDL_GetTicks();

	while (!quit) {
		// draw frame

		// advance read pointer
		// TODO: mutex! counter!
		fb_head++;
		Uint32 newticks = SDL_GetTicks();
		SDL_Delay(opts.framedelay - (newticks - ticks));
		ticks = newticks;
		// delay T=frametime - actualdelta
	}
}

/////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION
/////////////////////////////////////////////////////////////////////////////
//
int main(int argc, char **argv)
{
#if 0
	SDL_Surface *screen;

	// Initialise SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cerr << "Unable to initialise SDL: " << SDL_GetError() << endl;
		return -1;
	}
	// make sure SDL_Quit is called on exit
	atexit(SDL_Quit);

	// set screen video mode -- 320x240, 24bpp
	if ((screen = SDL_SetVideoMode(320, 240, 24, SDL_SWSURFACE)) == NULL) {
		cerr << "Unable to set video mode: " << SDL_GetError() << endl;
		return -1;
	}

	// Read the Replay file header

	// TODO: create frame buffer
	// TODO: set up producer/consumer threads
#endif

	ifstream replayfile("../ani1.rpl");
	ReplayDemuxer demux(&replayfile);

	cout << demux.getType() << endl;
	demux.DumpDebugInfo();

	return 0;
}
