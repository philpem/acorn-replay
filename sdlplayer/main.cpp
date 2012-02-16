#include <cstdlib>
#include <iostream>
#include <string>

#include "SDL.h"

using namespace std;


/////////////////////////////////////////////////////////////////////////////
// CONFIGURATION CONSTANTS
/////////////////////////////////////////////////////////////////////////////

const size_t NBUFFERS		= 4;	///< Number of frames to buffer

/////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
/////////////////////////////////////////////////////////////////////////////

char *framebuffer[NBUFFERS];

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
static int decoder(void *data)
{
	// Convert private-data pointer to something useful
	istream *in = static_cast<istream*>(data);

	while (!quit) {
		SDL_Delay(1000);
	}
}

// Consumer thread


/////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTION
/////////////////////////////////////////////////////////////////////////////
//
int main(int argc, char **argv)
{
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

	// TODO: create frame buffer
	// TODO: set up producer/consumer threads

	return 0;
}
