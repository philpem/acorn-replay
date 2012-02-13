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
	int r, g, b;
} RGB_TRIPLET;

RGB_TRIPLET YUV2RGB(int y, int u, int v)
{
	RGB_TRIPLET t;
	int r,g,b;

	r = (1.164*(y/*-16*/))                     + (2.018*(u-128));
	g = (1.164*(y/*-16*/)) - (0.813 * (v-128)) - (0.391*(u-128));
	b = (1.164*(y/*-16*/)) + (1.596 * (v-128));

	// clip values
	if (r > 255) t.r = 255;
	if (g > 255) t.g = 255;
	if (b > 255) t.b = 255;
	if (r < 0) t.r = 0;
	if (g < 0) t.g = 0;
	if (b < 0) t.b = 0;

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

	return 0;
}
