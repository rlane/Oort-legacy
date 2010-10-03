#ifndef TGA_G
#define TGA_H

struct tga_header {
   char  idlength;
   char  colourmaptype;
   char  datatypecode;
   short int colourmaporigin;
   short int colourmaplength;
   char  colourmapdepth;
   short int x_origin;
   short int y_origin;
   short width;
   short height;
   char  bitsperpixel;
   char  imagedescriptor;
	 unsigned char data[0];
} __attribute__((__packed__));

static const struct tga_header tga_defaults = {
	.idlength = 0,
	.colourmaptype = 0,
	.datatypecode = 2,
	.colourmaporigin = 0,
	.colourmaplength = 0,
	.colourmapdepth = 0,
	.x_origin = 0,
	.y_origin = 0,
	.width = 0, // must set
	.height = 0, // must set
	.bitsperpixel = 24,
	.imagedescriptor = 0,
};

#endif
