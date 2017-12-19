/*
tinypng.h - v1.02

To create implementation (the function definitions)
#define TINYPNG_IMPLEMENTATION
in *one* C/CPP file (translation unit) that includes this file


SUMMARY:

This header wraps some very nice functions by Richard Mitton from his
tigr (Tiny Graphics) library, with some additional features and small
bug-fixes.


Revision history:
1.00 (12/23/2016) initial release
1.01 (03/08/2017) tRNS chunk support for paletted images
1.02 (10/23/2017) support for explicitly loading paletted png images
1.03 (11/12/2017) construct atlas in memory


EXAMPLES:

Loading a PNG from disk, then freeing it
tpImage img = tpLoadPNG( "images/pic.png" );
...
free( img.pix );
memset( &img, 0, sizeof( img ) );

Loading a PNG from memory, then freeing it
tpImage img = tpLoadPNGMem( memory, sizeof( memory ) );
...
free( img.pix );
memset( &img, 0, sizeof( img ) );

Saving a PNG to disk
tpSavePNG( "images/example.png", &img );
// img is just a raw RGBA buffer, and can come from anywhere,
// not only from tpLoad*** functions

Creating a texture atlas in memory
int w = 1024;
int h = 1024;
tpAtlasImage* imgs_out = (tpAtlasImage*)malloc( sizeof( tpAtlasImage ) * my_png_count );
tpImage atlas_img = tpMakeAtlas( w, int h, my_png_array, my_png_count, imgs_out );
// just pass an array of pointers to images along with the image count. Make sure to also
// provide an array of `tpAtlasImage` for `tpMakeAtlas` to output important UV info for the
// images that fit into the atlas.

Using the default atlas saver
int errors = tpDefaultSaveAtlas( "atlas.png", "atlas.txt", atlas_img, atlas_imgs, img_count, names_of_all_images ? names_of_all_images : 0 );
if ( errors ) { ... }
// Atlas info (like uv coordinates) are in "atlas.txt", and the image was writen to "atlas.png".
// atlas_imgs was an array of `tpAtlasImage` from the `tpMakeAtlas` function.

Inflating a DEFLATE block (decompressing memory stored in DEFLATE format)
tpInflate( in, in_bytes, out, out_bytes );
// this function requires knowledge of the un-compressed size
// does *not* do any internal realloc! Will return errors if an
// attempt to overwrite the out buffer is made
*/

/*
Contributors:
Zachary Carter          1.01 - bug catch for tRNS chunk in paletted images
Diego Lopes (DCubix)    Move impl to a cpp file
*/

#ifndef TINYPNG_H
#define TINYPNG_H

#ifdef _WIN32

#if !defined( _CRT_SECURE_NO_WARNINGS )
#define _CRT_SECURE_NO_WARNINGS
#endif

#endif

#define TP_ATLAS_MUST_FIT           1 // returns error from tpMakeAtlas if *any* input image does not fit
#define TP_ATLAS_FLIP_Y_AXIS_FOR_UV 1 // flips output uv coordinate's y. Can be useful to "flip image on load"
#define TP_ATLAS_EMPTY_COLOR        0x000000FF

#include <stdint.h>

typedef struct tpPixel tpPixel;
typedef struct tpImage tpImage;
typedef struct tpIndexedImage tpIndexedImage;
typedef struct tpAtlasImage tpAtlasImage;

// Read this in the event of errors from any function
extern const char* g_tpErrorReason;

// return 1 for success, 0 for failures
int tpInflate(void* in, int in_bytes, void* out, int out_bytes);
int tpSavePNG(const char* fileName, const tpImage* img);

// Constructs an atlas image in-memory. The atlas pixels are stored in the returned image. free the pixels
// when done with them. The user must provide an array of tpAtlasImage for the `imgs` param. `imgs` holds
// information about uv coordinates for an associated image in the `pngs` array. Output image has NULL
// pixels buffer in the event of errors.
tpImage tpMakeAtlas(int atlasWidth, int atlasHeight, const tpImage* pngs, int png_count, tpAtlasImage* imgs_out);

// A decent "default" function, ready to use out-of-the-box. Saves out an easy to parse text formatted info file
// along with an atlas image. `names` param can be optionally NULL.
int tpDefaultSaveAtlas(const char* out_path_image, const char* out_path_atlas_txt, const tpImage* atlas, const tpAtlasImage* imgs, int img_count, const char** names);

// these two functions return tpImage::pix as 0 in event of errors
// call free on tpImage::pix when done
tpImage tpLoadPNG(const char *fileName);
tpImage tpLoadPNGMem(const void *png_data, int png_length);

// loads indexed (paletted) pngs, but does not depalette the image into RGBA pixels
// these two functions return tpIndexedImage::pix as 0 in event of errors
// call free on tpIndexedImage::pix when done
tpIndexedImage tpLoadIndexedPNG(const char* fileName);
tpIndexedImage tpLoadIndexedPNGMem(const void *png_data, int png_length);

// converts paletted image into a standard RGBA image
// call free on tpImage::pix when done
tpImage tpDepaletteIndexedImage(tpIndexedImage* img);

// Pre-process the pixels to transform the image data to a premultiplied alpha format.
// Resource: http://www.essentialmath.com/GDC2015/VanVerth_Jim_DoingMathwRGB.pdf
void tpPremultiply(tpImage* img);

struct tpPixel {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

struct tpImage {
	int w;
	int h;
	tpPixel* pix;
};

struct tpIndexedImage {
	int w;
	int h;
	uint8_t* pix;
	uint8_t palette_len;
	tpPixel palette[256];
};

struct tpAtlasImage {
	int img_index;    // index into the `imgs` array
	int w, h;         // pixel w/h of original image
	float minx, miny; // u coordinate
	float maxx, maxy; // v coordinate
	int fit;          // non-zero if image fit and was placed into the atlas
};

#endif // TINYPNG_H

/*
This is free and unencumbered software released into the public domain.

Our intent is that anyone is free to copy and use this software,
for any purpose, in any form, and by any means.

The authors dedicate any and all copyright interest in the software
to the public domain, at their own expense for the betterment of mankind.

The software is provided "as is", without any kind of warranty, including
any implied warranty. If it breaks, you get to keep both pieces.
*/
