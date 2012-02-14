/**
 * @file
 * @brief YCbCr to RGB conversion
 *
 * From http://en.wikipedia.org/wiki/YCbCr
 */

#include <iostream>
#include "CImg.h"
using namespace cimg_library;
using namespace std;

typedef struct {
	int y, u, v;
} YUV_TRIPLET;

typedef struct {
	unsigned char r, g, b;
} RGB_TRIPLET;

RGB_TRIPLET YUV2RGB(int y, int u, int v)
{
	RGB_TRIPLET t;
	int r,g,b;

	// YUV to RGB conversion
	r = (1.164*(y/*-16*/))                     + (2.018*(u-128));
	g = (1.164*(y/*-16*/)) - (0.813 * (v-128)) - (0.391*(u-128));
	b = (1.164*(y/*-16*/)) + (1.596 * (v-128));

	// clip values
	if (r > 255) r = 255;
	if (g > 255) g = 255;
	if (b > 255) b = 255;
	if (r < 0) r = 0;
	if (g < 0) g = 0;
	if (b < 0) b = 0;

	// copy into result block
	t.r = r;
	t.g = g;
	t.b = b;

	return t;
}

RGB_TRIPLET YUV2RGB(YUV_TRIPLET x)
{
	return YUV2RGB(x.y, x.u, x.v);
}

int main(void)
{
	RGB_TRIPLET t = YUV2RGB(128, 128, 0);
/*
	CImg<int> img(256,256,1,3);

	int y, u, v;
	y = 128;
	for (v=0; v<256; v++) {
		for (u=0; u<256; u++) {
			t = YUV2RGB(y, u, v);
			img(v, 255-u, 0, 0) = t.r;
			img(v, 255-u, 0, 1) = t.g;
			img(v, 255-u, 0, 2) = t.b;
		}
	}
	img.display();
*/
	RGB_TRIPLET yuvtable[0x20][0x20][0x20];
	int y=0,u=0,v=0;
	for (y=0; y<0x20; y++) {
		for (u=0; u<0x20; u++) {
			for (v=0; v<0x20; v++) {
				yuvtable[y][u][v] = YUV2RGB(y*8,u*8,v*8);
			}
		}
	}

	CImg<int> k(256,256,1,3);
	for (y=0; y<256; y+=127) {
	//	y=128;
		for (v=0; v<256; v++) {
			for (u=0; u<256; u++) {
				k(v, 255-u, 0, 0) = yuvtable[y/8][u/8][v/8].r;
				k(v, 255-u, 0, 1) = yuvtable[y/8][u/8][v/8].g;
				k(v, 255-u, 0, 2) = yuvtable[y/8][u/8][v/8].b;
			}
		}
		k.display();
	}

	return 0;
}
