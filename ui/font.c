/*  Upside-down font transformed from SDL_gfxPrimitives_font.h */
/*  LGPL (c) A. Schiffler */

#include "font.h"

unsigned char oort_font[OORT_FONT_SIZE] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x7e, 0x81, 0x99, 0xbd, 0x81, 0xa5, 0x81, 0x7e, 
0x7e, 0xff, 0xe7, 0xc3, 0xff, 0xdb, 0xff, 0x7e, 
0x0, 0x10, 0x38, 0x7c, 0xfe, 0xfe, 0xfe, 0x6c, 
0x0, 0x10, 0x38, 0x7c, 0xfe, 0x7c, 0x38, 0x10, 
0x38, 0x10, 0xd6, 0xfe, 0xfe, 0x38, 0x7c, 0x38, 
0x38, 0x10, 0x7c, 0xfe, 0xfe, 0x7c, 0x38, 0x10, 
0x0, 0x0, 0x18, 0x3c, 0x3c, 0x18, 0x0, 0x0, 
0xff, 0xff, 0xe7, 0xc3, 0xc3, 0xe7, 0xff, 0xff, 
0x0, 0x3c, 0x66, 0x42, 0x42, 0x66, 0x3c, 0x0, 
0xff, 0xc3, 0x99, 0xbd, 0xbd, 0x99, 0xc3, 0xff, 
0x78, 0xcc, 0xcc, 0xcc, 0x7d, 0xf, 0x7, 0xf, 
0x18, 0x7e, 0x18, 0x3c, 0x66, 0x66, 0x66, 0x3c, 
0xe0, 0xf0, 0x70, 0x30, 0x30, 0x3f, 0x33, 0x3f, 
0xc0, 0xe6, 0x67, 0x63, 0x63, 0x7f, 0x63, 0x7f, 
0x18, 0xdb, 0x3c, 0xe7, 0xe7, 0x3c, 0xdb, 0x18, 
0x0, 0x80, 0xe0, 0xf8, 0xfe, 0xf8, 0xe0, 0x80, 
0x0, 0x2, 0xe, 0x3e, 0xfe, 0x3e, 0xe, 0x2, 
0x18, 0x3c, 0x7e, 0x18, 0x18, 0x7e, 0x3c, 0x18, 
0x0, 0x66, 0x0, 0x66, 0x66, 0x66, 0x66, 0x66, 
0x0, 0x1b, 0x1b, 0x1b, 0x7b, 0xdb, 0xdb, 0x7f, 
0x7c, 0x86, 0x3c, 0x66, 0x66, 0x3c, 0x61, 0x3e, 
0x0, 0x7e, 0x7e, 0x7e, 0x0, 0x0, 0x0, 0x0, 
0xff, 0x18, 0x3c, 0x7e, 0x18, 0x7e, 0x3c, 0x18, 
0x0, 0x18, 0x18, 0x18, 0x18, 0x7e, 0x3c, 0x18, 
0x0, 0x18, 0x3c, 0x7e, 0x18, 0x18, 0x18, 0x18, 
0x0, 0x0, 0x18, 0xc, 0xfe, 0xc, 0x18, 0x0, 
0x0, 0x0, 0x30, 0x60, 0xfe, 0x60, 0x30, 0x0, 
0x0, 0x0, 0xfe, 0xc0, 0xc0, 0xc0, 0x0, 0x0, 
0x0, 0x0, 0x24, 0x66, 0xff, 0x66, 0x24, 0x0, 
0x0, 0x0, 0xff, 0xff, 0x7e, 0x3c, 0x18, 0x0, 
0x0, 0x0, 0x18, 0x3c, 0x7e, 0xff, 0xff, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x18, 0x0, 0x18, 0x18, 0x3c, 0x3c, 0x18, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x24, 0x66, 0x66, 
0x0, 0x6c, 0x6c, 0xfe, 0x6c, 0xfe, 0x6c, 0x6c, 
0x0, 0x18, 0x7c, 0x6, 0x3c, 0x60, 0x3e, 0x18, 
0x0, 0xc6, 0x66, 0x30, 0x18, 0xcc, 0xc6, 0x0, 
0x0, 0x76, 0xcc, 0xdc, 0x76, 0x38, 0x6c, 0x38, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x30, 0x18, 0x18, 
0x0, 0xc, 0x18, 0x30, 0x30, 0x30, 0x18, 0xc, 
0x0, 0x30, 0x18, 0xc, 0xc, 0xc, 0x18, 0x30, 
0x0, 0x0, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x0, 
0x0, 0x0, 0x18, 0x18, 0x7e, 0x18, 0x18, 0x0, 
0x30, 0x18, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x7e, 0x0, 0x0, 0x0, 
0x0, 0x18, 0x18, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x80, 0xc0, 0x60, 0x30, 0x18, 0xc, 0x6, 
0x0, 0x38, 0x6c, 0xc6, 0xd6, 0xc6, 0x6c, 0x38, 
0x0, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x38, 0x18, 
0x0, 0xfe, 0x66, 0x30, 0x1c, 0x6, 0xc6, 0x7c, 
0x0, 0x7c, 0xc6, 0x6, 0x3c, 0x6, 0xc6, 0x7c, 
0x0, 0x1e, 0xc, 0xfe, 0xcc, 0x6c, 0x3c, 0x1c, 
0x0, 0x7c, 0xc6, 0x6, 0xfc, 0xc0, 0xc0, 0xfe, 
0x0, 0x7c, 0xc6, 0xc6, 0xfc, 0xc0, 0x60, 0x38, 
0x0, 0x30, 0x30, 0x30, 0x18, 0xc, 0xc6, 0xfe, 
0x0, 0x7c, 0xc6, 0xc6, 0x7c, 0xc6, 0xc6, 0x7c, 
0x0, 0x78, 0xc, 0x6, 0x7e, 0xc6, 0xc6, 0x7c, 
0x0, 0x18, 0x18, 0x0, 0x0, 0x18, 0x18, 0x0, 
0x30, 0x18, 0x18, 0x0, 0x0, 0x18, 0x18, 0x0, 
0x0, 0x6, 0xc, 0x18, 0x30, 0x18, 0xc, 0x6, 
0x0, 0x0, 0x7e, 0x0, 0x0, 0x7e, 0x0, 0x0, 
0x0, 0x60, 0x30, 0x18, 0xc, 0x18, 0x30, 0x60, 
0x0, 0x18, 0x0, 0x18, 0x18, 0xc, 0xc6, 0x7c, 
0x0, 0x78, 0xc0, 0xde, 0xde, 0xde, 0xc6, 0x7c, 
0x0, 0xc6, 0xc6, 0xc6, 0xfe, 0xc6, 0x6c, 0x38, 
0x0, 0xfc, 0x66, 0x66, 0x7c, 0x66, 0x66, 0xfc, 
0x0, 0x3c, 0x66, 0xc0, 0xc0, 0xc0, 0x66, 0x3c, 
0x0, 0xf8, 0x6c, 0x66, 0x66, 0x66, 0x6c, 0xf8, 
0x0, 0xfe, 0x62, 0x68, 0x78, 0x68, 0x62, 0xfe, 
0x0, 0xf0, 0x60, 0x68, 0x78, 0x68, 0x62, 0xfe, 
0x0, 0x3a, 0x66, 0xce, 0xc0, 0xc0, 0x66, 0x3c, 
0x0, 0xc6, 0xc6, 0xc6, 0xfe, 0xc6, 0xc6, 0xc6, 
0x0, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 
0x0, 0x78, 0xcc, 0xcc, 0xc, 0xc, 0xc, 0x1e, 
0x0, 0xe6, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0xe6, 
0x0, 0xfe, 0x66, 0x62, 0x60, 0x60, 0x60, 0xf0, 
0x0, 0xc6, 0xc6, 0xd6, 0xfe, 0xfe, 0xee, 0xc6, 
0x0, 0xc6, 0xc6, 0xce, 0xde, 0xf6, 0xe6, 0xc6, 
0x0, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 
0x0, 0xf0, 0x60, 0x60, 0x7c, 0x66, 0x66, 0xfc, 
0xe, 0x7c, 0xce, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 
0x0, 0xe6, 0x66, 0x6c, 0x7c, 0x66, 0x66, 0xfc, 
0x0, 0x3c, 0x66, 0xc, 0x18, 0x30, 0x66, 0x3c, 
0x0, 0x3c, 0x18, 0x18, 0x18, 0x5a, 0x7e, 0x7e, 
0x0, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 
0x0, 0x38, 0x6c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 
0x0, 0x6c, 0xfe, 0xd6, 0xd6, 0xc6, 0xc6, 0xc6, 
0x0, 0xc6, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0xc6, 
0x0, 0x3c, 0x18, 0x18, 0x3c, 0x66, 0x66, 0x66, 
0x0, 0xfe, 0x66, 0x32, 0x18, 0x8c, 0xc6, 0xfe, 
0x0, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c, 
0x0, 0x2, 0x6, 0xc, 0x18, 0x30, 0x60, 0xc0, 
0x0, 0x3c, 0xc, 0xc, 0xc, 0xc, 0xc, 0x3c, 
0x0, 0x0, 0x0, 0x0, 0xc6, 0x6c, 0x38, 0x10, 
0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0xc, 0x18, 0x30, 
0x0, 0x76, 0xcc, 0x7c, 0xc, 0x78, 0x0, 0x0, 
0x0, 0xdc, 0x66, 0x66, 0x66, 0x7c, 0x60, 0xe0, 
0x0, 0x7c, 0xc6, 0xc0, 0xc6, 0x7c, 0x0, 0x0, 
0x0, 0x76, 0xcc, 0xcc, 0xcc, 0x7c, 0xc, 0x1c, 
0x0, 0x7c, 0xc0, 0xfe, 0xc6, 0x7c, 0x0, 0x0, 
0x0, 0xf0, 0x60, 0x60, 0xf8, 0x60, 0x66, 0x3c, 
0xf8, 0xc, 0x7c, 0xcc, 0xcc, 0x76, 0x0, 0x0, 
0x0, 0xe6, 0x66, 0x66, 0x76, 0x6c, 0x60, 0xe0, 
0x0, 0x3c, 0x18, 0x18, 0x18, 0x38, 0x0, 0x18, 
0x3c, 0x66, 0x66, 0x6, 0x6, 0x6, 0x0, 0x6, 
0x0, 0xe6, 0x6c, 0x78, 0x6c, 0x66, 0x60, 0xe0, 
0x0, 0x3c, 0x18, 0x18, 0x18, 0x18, 0x18, 0x38, 
0x0, 0xd6, 0xd6, 0xd6, 0xfe, 0xec, 0x0, 0x0, 
0x0, 0x66, 0x66, 0x66, 0x66, 0xdc, 0x0, 0x0, 
0x0, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x0, 0x0, 
0xf0, 0x60, 0x7c, 0x66, 0x66, 0xdc, 0x0, 0x0, 
0x1e, 0xc, 0x7c, 0xcc, 0xcc, 0x76, 0x0, 0x0, 
0x0, 0xf0, 0x60, 0x60, 0x76, 0xdc, 0x0, 0x0, 
0x0, 0xfc, 0x6, 0x7c, 0xc0, 0x7e, 0x0, 0x0, 
0x0, 0x1c, 0x36, 0x30, 0x30, 0xfc, 0x30, 0x30, 
0x0, 0x76, 0xcc, 0xcc, 0xcc, 0xcc, 0x0, 0x0, 
0x0, 0x38, 0x6c, 0xc6, 0xc6, 0xc6, 0x0, 0x0, 
0x0, 0x6c, 0xfe, 0xd6, 0xd6, 0xc6, 0x0, 0x0, 
0x0, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0x0, 0x0, 
0xfc, 0x6, 0x7e, 0xc6, 0xc6, 0xc6, 0x0, 0x0, 
0x0, 0x7e, 0x32, 0x18, 0x4c, 0x7e, 0x0, 0x0, 
0x0, 0xe, 0x18, 0x18, 0x70, 0x18, 0x18, 0xe, 
0x0, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x0, 0x70, 0x18, 0x18, 0xe, 0x18, 0x18, 0x70, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xdc, 0x76, 
0x0, 0xfe, 0xc6, 0xc6, 0x6c, 0x38, 0x10, 0x0, 
0x78, 0xc, 0x7c, 0xc6, 0xc0, 0xc0, 0xc6, 0x7c, 
0x0, 0x76, 0xcc, 0xcc, 0xcc, 0xcc, 0x0, 0xcc, 
0x0, 0x7c, 0xc0, 0xfe, 0xc6, 0x7c, 0x18, 0xc, 
0x0, 0x76, 0xcc, 0x7c, 0xc, 0x78, 0x82, 0x7c, 
0x0, 0x76, 0xcc, 0x7c, 0xc, 0x78, 0x0, 0xc6, 
0x0, 0x76, 0xcc, 0x7c, 0xc, 0x78, 0x18, 0x30, 
0x0, 0x76, 0xcc, 0x7c, 0xc, 0x78, 0x30, 0x30, 
0x38, 0xc, 0x7e, 0xc0, 0xc0, 0x7e, 0x0, 0x0, 
0x0, 0x7c, 0xc0, 0xfe, 0xc6, 0x7c, 0x82, 0x7c, 
0x0, 0x7c, 0xc0, 0xfe, 0xc6, 0x7c, 0x0, 0xc6, 
0x0, 0x7c, 0xc0, 0xfe, 0xc6, 0x7c, 0x18, 0x30, 
0x0, 0x3c, 0x18, 0x18, 0x18, 0x38, 0x0, 0x66, 
0x0, 0x3c, 0x18, 0x18, 0x18, 0x38, 0x82, 0x7c, 
0x0, 0x3c, 0x18, 0x18, 0x38, 0x0, 0x18, 0x30, 
0x0, 0xc6, 0xc6, 0xfe, 0xc6, 0x6c, 0x38, 0xc6, 
0x0, 0xc6, 0xc6, 0xfe, 0xc6, 0x7c, 0x6c, 0x38, 
0x0, 0xfe, 0xc0, 0xf8, 0xc0, 0xfe, 0x30, 0x18, 
0x0, 0x7e, 0xd8, 0x7e, 0x18, 0x7e, 0x0, 0x0, 
0x0, 0xce, 0xcc, 0xcc, 0xfe, 0xcc, 0x6c, 0x3e, 
0x0, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x82, 0x7c, 
0x0, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x0, 0xc6, 
0x0, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x18, 0x30, 
0x0, 0x76, 0xcc, 0xcc, 0xcc, 0x0, 0x84, 0x78, 
0x0, 0x76, 0xcc, 0xcc, 0xcc, 0xcc, 0x30, 0x60, 
0xfc, 0x6, 0x7e, 0xc6, 0xc6, 0xc6, 0x0, 0xc6, 
0x0, 0x38, 0x6c, 0xc6, 0xc6, 0x6c, 0x38, 0xc6, 
0x0, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0x0, 0xc6, 
0x18, 0x18, 0x7e, 0xc0, 0xc0, 0x7e, 0x18, 0x18, 
0x0, 0xfc, 0x66, 0x60, 0xf0, 0x64, 0x6c, 0x38, 
0x18, 0x18, 0x7e, 0x18, 0x7e, 0x3c, 0x66, 0x66, 
0xc7, 0xc6, 0xcf, 0xc6, 0xfa, 0xcc, 0xcc, 0xf8, 
0x0, 0x70, 0xd8, 0x18, 0x3c, 0x18, 0x1b, 0xe, 
0x0, 0x76, 0xcc, 0x7c, 0xc, 0x78, 0x30, 0x18, 
0x0, 0x3c, 0x18, 0x18, 0x38, 0x0, 0x18, 0xc, 
0x0, 0x7c, 0xc6, 0xc6, 0xc6, 0x7c, 0x18, 0xc, 
0x0, 0x76, 0xcc, 0xcc, 0xcc, 0xcc, 0x30, 0x18, 
0x0, 0x66, 0x66, 0x66, 0xdc, 0x0, 0xdc, 0x76, 
0x0, 0xce, 0xde, 0xf6, 0xe6, 0x0, 0xdc, 0x76, 
0x0, 0x0, 0x7e, 0x0, 0x3e, 0x6c, 0x6c, 0x3c, 
0x0, 0x0, 0x7c, 0x0, 0x38, 0x6c, 0x6c, 0x38, 
0x0, 0x3e, 0x63, 0x30, 0x18, 0x18, 0x0, 0x18, 
0x0, 0x0, 0xc0, 0xc0, 0xfe, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x6, 0x6, 0xfe, 0x0, 0x0, 0x0, 
0xf, 0xcc, 0x66, 0x33, 0x7e, 0x6c, 0xe6, 0x63, 
0x6, 0xdf, 0x6a, 0x36, 0x7a, 0x6c, 0xe6, 0x63, 
0x0, 0x18, 0x3c, 0x3c, 0x18, 0x18, 0x0, 0x18, 
0x0, 0x0, 0x33, 0x66, 0xcc, 0x66, 0x33, 0x0, 
0x0, 0x0, 0xcc, 0x66, 0x33, 0x66, 0xcc, 0x0, 
0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 
0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 
0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0xf8, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0xf8, 0x18, 0xf8, 0x18, 0x18, 
0x36, 0x36, 0x36, 0xf6, 0x36, 0x36, 0x36, 0x36, 
0x36, 0x36, 0x36, 0xfe, 0x0, 0x0, 0x0, 0x0, 
0x18, 0x18, 0x18, 0xf8, 0x18, 0xf8, 0x0, 0x0, 
0x36, 0x36, 0x36, 0xf6, 0x6, 0xf6, 0x36, 0x36, 
0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 
0x36, 0x36, 0x36, 0xf6, 0x6, 0xfe, 0x0, 0x0, 
0x0, 0x0, 0x0, 0xfe, 0x6, 0xf6, 0x36, 0x36, 
0x0, 0x0, 0x0, 0xfe, 0x36, 0x36, 0x36, 0x36, 
0x0, 0x0, 0x0, 0xf8, 0x18, 0xf8, 0x18, 0x18, 
0x18, 0x18, 0x18, 0xf8, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x1f, 0x18, 0x18, 0x18, 0x18, 
0x0, 0x0, 0x0, 0xff, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0xff, 0x0, 0x0, 0x0, 0x0, 
0x18, 0x18, 0x18, 0x1f, 0x18, 0x18, 0x18, 0x18, 
0x0, 0x0, 0x0, 0xff, 0x0, 0x0, 0x0, 0x0, 
0x18, 0x18, 0x18, 0xff, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x1f, 0x18, 0x1f, 0x18, 0x18, 
0x36, 0x36, 0x36, 0x37, 0x36, 0x36, 0x36, 0x36, 
0x0, 0x0, 0x0, 0x3f, 0x30, 0x37, 0x36, 0x36, 
0x36, 0x36, 0x36, 0x37, 0x30, 0x3f, 0x0, 0x0, 
0x0, 0x0, 0x0, 0xff, 0x0, 0xf7, 0x36, 0x36, 
0x36, 0x36, 0x36, 0xf7, 0x0, 0xff, 0x0, 0x0, 
0x36, 0x36, 0x36, 0x37, 0x30, 0x37, 0x36, 0x36, 
0x0, 0x0, 0x0, 0xff, 0x0, 0xff, 0x0, 0x0, 
0x36, 0x36, 0x36, 0xf7, 0x0, 0xf7, 0x36, 0x36, 
0x0, 0x0, 0x0, 0xff, 0x0, 0xff, 0x18, 0x18, 
0x0, 0x0, 0x0, 0xff, 0x36, 0x36, 0x36, 0x36, 
0x18, 0x18, 0x18, 0xff, 0x0, 0xff, 0x0, 0x0, 
0x36, 0x36, 0x36, 0xff, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x3f, 0x36, 0x36, 0x36, 0x36, 
0x0, 0x0, 0x0, 0x1f, 0x18, 0x1f, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x1f, 0x18, 0x1f, 0x0, 0x0, 
0x36, 0x36, 0x36, 0x3f, 0x0, 0x0, 0x0, 0x0, 
0x36, 0x36, 0x36, 0xff, 0x36, 0x36, 0x36, 0x36, 
0x18, 0x18, 0x18, 0xff, 0x18, 0xff, 0x18, 0x18, 
0x0, 0x0, 0x0, 0xf8, 0x18, 0x18, 0x18, 0x18, 
0x18, 0x18, 0x18, 0x1f, 0x0, 0x0, 0x0, 0x0, 
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0, 
0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 
0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 0xf, 
0x0, 0x0, 0x0, 0x0, 0xff, 0xff, 0xff, 0xff, 
0x0, 0x76, 0xdc, 0xc8, 0xdc, 0x76, 0x0, 0x0, 
0x0, 0xcc, 0xc6, 0xcc, 0xd8, 0xcc, 0xcc, 0x78, 
0x0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc6, 0xfe, 
0x0, 0x6c, 0x6c, 0x6c, 0x6c, 0xfe, 0x0, 0x0, 
0x0, 0xfe, 0xc6, 0x60, 0x30, 0x60, 0xc6, 0xfe, 
0x0, 0x70, 0xd8, 0xd8, 0xd8, 0x7e, 0x0, 0x0, 
0xc0, 0x7c, 0x66, 0x66, 0x66, 0x66, 0x0, 0x0, 
0x0, 0x18, 0x18, 0x18, 0x18, 0xdc, 0x76, 0x0, 
0x7e, 0x18, 0x3c, 0x66, 0x66, 0x3c, 0x18, 0x7e, 
0x0, 0x38, 0x6c, 0xc6, 0xfe, 0xc6, 0x6c, 0x38, 
0x0, 0xee, 0x6c, 0x6c, 0xc6, 0xc6, 0x6c, 0x38, 
0x0, 0x3c, 0x66, 0x66, 0x3e, 0xc, 0x18, 0xe, 
0x0, 0x0, 0x7e, 0xdb, 0xdb, 0x7e, 0x0, 0x0, 
0xc0, 0x60, 0x7e, 0xdb, 0xdb, 0x7e, 0xc, 0x6, 
0x0, 0x1e, 0x30, 0x60, 0x7e, 0x60, 0x30, 0x1e, 
0x0, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x0, 
0x0, 0x0, 0xfe, 0x0, 0xfe, 0x0, 0xfe, 0x0, 
0x0, 0x7e, 0x0, 0x18, 0x18, 0x7e, 0x18, 0x18, 
0x0, 0x7e, 0x0, 0x30, 0x18, 0xc, 0x18, 0x30, 
0x0, 0x7e, 0x0, 0xc, 0x18, 0x30, 0x18, 0xc, 
0x18, 0x18, 0x18, 0x18, 0x18, 0x1b, 0x1b, 0xe, 
0x70, 0xd8, 0xd8, 0x18, 0x18, 0x18, 0x18, 0x18, 
0x0, 0x0, 0x18, 0x0, 0x7e, 0x0, 0x18, 0x0, 
0x0, 0x0, 0xdc, 0x76, 0x0, 0xdc, 0x76, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x38, 0x6c, 0x6c, 0x38, 
0x0, 0x0, 0x0, 0x18, 0x18, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x18, 0x0, 0x0, 0x0, 
0x1c, 0x3c, 0x6c, 0xec, 0xc, 0xc, 0xc, 0xf, 
0x0, 0x0, 0x0, 0x36, 0x36, 0x36, 0x36, 0x6c, 
0x0, 0x0, 0x0, 0x7c, 0x30, 0x18, 0xc, 0x78, 
0x0, 0x0, 0x3c, 0x3c, 0x3c, 0x3c, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
};
