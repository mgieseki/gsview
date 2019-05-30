/* Copyright (C) 1993-2006, Ghostgum Software Pty Ltd.  All rights reserved.
  
  This file is part of GSview.
   
  This program is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  Refer to the GSview Licence (the "Licence") 
  for full details.
   
  Every copy of GSview must include a copy of the Licence, normally in a 
  plain ASCII text file named LICENCE.  The Licence grants you the right 
  to copy, modify and redistribute GSview, but only under certain conditions 
  described in the Licence.  Among other things, the Licence requires that 
  the copyright notice and this notice be preserved on all copies.
*/

/* gvceps.h */
/* Common data structures for EPS manipulation */

/* because Windows and PM define the same bitmap structures  */
/* with different names we use our own common structures */
/* BITMAP1 = Windows BITMAPCORE and PM BITMAPINFO */
/* BITMAP2 = Windows BITMAPINFO and PM BITMAPINFO2 */
/* RGB3 = Windows RGBTRIPLE and PM RGB */
/* RGB4 = Windows RGBQUAD and PM RGB2 */

/* We used to pack structures to byte boundaries, but this is
 * no longer required.  All EPS handling should be unaffected
 * by alignment/packing and endian order.
 */

/* WARNING - these structures might not have byte packing. */
/* When you read BMP files, do not use sizeof(structure). */
/* Use RGB3_LENGTH instead of sizeof(RGB3) */

#ifndef BITMAP1AND2
#define BITMAP1AND2
typedef struct tagRGB3
{
    BYTE    rgbtBlue;
    BYTE    rgbtGreen;
    BYTE    rgbtRed;
} RGB3;
typedef RGB3 * LPRGB3;

#define RGB3_BLUE	0
#define RGB3_GREEN	1
#define RGB3_RED	2
#define RGB3_LENGTH	3

typedef struct tagRGB4
{
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGB4;
typedef RGB4 * LPRGB4;

#define RGB4_BLUE	0
#define RGB4_GREEN	1
#define RGB4_RED	2
#define RGB4_EXTRA	3
#define RGB4_LENGTH	4

typedef struct tagBITMAP1
{
    DWORD   bcSize;
    short   bcWidth;
    short   bcHeight;
    WORD    bcPlanes;
    WORD    bcBitCount;
} BITMAP1;
typedef BITMAP1 * LPBITMAP1;

#define BITMAP1_WIDTH 4
#define BITMAP1_HEIGHT 6
#define BITMAP1_PLANES 8
#define BITMAP1_BITCOUNT 10
#define BITMAP1_LENGTH 12

typedef struct tagBITMAP2
{
    DWORD   biSize;
    LONG    biWidth;
    LONG    biHeight;
    WORD    biPlanes;
    WORD    biBitCount;
    DWORD   biCompression;
    DWORD   biSizeImage;
    LONG    biXPelsPerMeter;
    LONG    biYPelsPerMeter;
    DWORD   biClrUsed;
    DWORD   biClrImportant;
} BITMAP2;
typedef BITMAP2 * LPBITMAP2;

#define BITMAP2_WIDTH 4
#define BITMAP2_HEIGHT 8
#define BITMAP2_PLANES 12
#define BITMAP2_BITCOUNT 14
#define BITMAP2_CLRUSED 32
#define BITMAP2_LENGTH 40

typedef struct tagBITMAP2INFO
{
	BITMAP2 bmp2;
	RGB4 rgb4[256];
} BITMAP2INFO;
typedef BITMAP2INFO * LPBITMAP2INFO;

typedef struct tagBITMAPFILE
{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILE;
typedef BITMAPFILE * LPBITMAPFILE;
#define BITMAPFILE_LENGTH 14
#endif /*  BITMAP1AND2 */


struct eps_header_s {
    char id[4];
    DWORD ps_begin;
    DWORD ps_length;
    DWORD mf_begin;
    DWORD mf_length;
    DWORD tiff_begin;
    DWORD tiff_length;
    WORD  checksum;
};
#define EPS_HEADER_SIZE 30

#ifdef __EMX__
#pragma pack()
#endif

/* a structure for holding details of a bitmap used for constructing */
/* an EPS preview */
typedef struct tagPREBMAP {
    int  width;
    int  height;
    int  depth;
    int  bytewidth;	/* length of each scan line in bytes */
    BYTE * bits;
    BOOL os2;
    BOOL topleft;
} PREBMAP;


typedef struct tagPSBBOX {
	int	llx;
	int	lly;
	int	urx;
	int	ury;
	int	valid;
} PSBBOX;
extern PSBBOX bbox;

/* in gvpeps.c or gvweps.c */
LPBITMAP2 get_bitmap(void);
void release_bitmap(void);

/* in gvceps.c */
unsigned long dib_bytewidth(unsigned char *pbitmap);
unsigned int dib_pal_colors(unsigned char *pbitmap);
int make_eps_tiff(int type, BOOL calc_bbox, const char *epsname);
int make_eps_interchange(BOOL calc_bbox, const char *epiname);
int make_eps_user(void);
int make_eps_metafile(BOOL calc_bbox, const char *outname);
void extract_doseps(int command);
void copy_bbox_header(FILE *f);
int scan_pbmplus(PREBMAP *ppbmap, unsigned char *pbitmap);
int scan_dib(PREBMAP *ppbmap, unsigned char *pbitmap);
void scan_bbox(PREBMAP *pprebmap, PSBBOX *devbbox);
void write_dword(DWORD val, FILE *f);
void write_word(WORD val, FILE *f);

void ps_copy(FILE *outfile, GFile *infile, long begin, long end);
int ps_fgets(char *s, int n, GFile *stream);
BOOL ps_copy_find(FILE *outfile, GFile *infile, long end, 
	char *s, int n, const char *comment);
