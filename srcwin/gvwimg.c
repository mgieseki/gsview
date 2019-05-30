/* Copyright (C) 2000-2001, Ghostgum Software Pty Ltd.  All rights reserved.
  
  This file is part of GSview.
  
  This program is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  Refer to the GSview Free Public Licence 
  (the "Licence") for full details.
  
  Every copy of GSview must include a copy of the Licence, normally in a 
  plain ASCII text file named LICENCE.  The Licence grants you the right 
  to copy, modify and redistribute GSview, but only under certain conditions 
  described in the Licence.  Among other things, the Licence requires that 
  the copyright notice and this notice be preserved on all copies.
*/

/* gvwimg.cpp */
/* Image routines for Ghostscript display device */

#include "gvc.h"

void
image_lock(IMAGE *img)
{
    if (debug & img->lock_count)
	gs_addmess("Image is locked\n");
    if (multithread)
	WaitForSingleObject(img->hmutex, 120000);
    if (debug) {
	if (img->lock_count)
	    gs_addmess("Attempted to lock image twice\n");
	img->lock_count++;
    }
}

void
image_unlock(IMAGE *img)
{
    if (multithread)
	ReleaseMutex(img->hmutex);
    if (debug) {
	if (img->lock_count == 0)
	    gs_addmess("Attempted to unlock unlocked image\n");
	else 
	    img->lock_count--;
    }
}

HPALETTE 
image_create_palette(IMAGE *img)
{
    int i;
    int nColors;
    HPALETTE palette = NULL;

    nColors = img->bmih.biClrUsed;
    if (nColors) {
	LPLOGPALETTE logpalette;
	logpalette = (LPLOGPALETTE) malloc(sizeof(LOGPALETTE) + 
	    nColors * sizeof(PALETTEENTRY));
	if (logpalette == (LPLOGPALETTE) NULL)
	    return (HPALETTE)0;
	logpalette->palVersion = 0x300;
	logpalette->palNumEntries = (WORD)img->bmih.biClrUsed;
	for (i = 0; i < nColors; i++) {
	    logpalette->palPalEntry[i].peFlags = 0;
	    image_color(img->format, i,
		&logpalette->palPalEntry[i].peRed,
		&logpalette->palPalEntry[i].peGreen,
		&logpalette->palPalEntry[i].peBlue);
	}
	palette = CreatePalette(logpalette);
	free(logpalette);
    }
    return palette;
}

int 
image_preclose(IMAGE *img)
{
    return 0;
}

int
image_presize(IMAGE *img, int width, int height, int raster,
    unsigned int format)
{
    return 0;	/* OK */
}

/* resize image */
int
image_size(IMAGE *img)
{
    int bytewidth;

    /* create a BMP header for the bitmap */
    img->bmih.biSize = sizeof(BITMAPINFOHEADER);
    img->bmih.biWidth = img->width;
    img->bmih.biHeight = img->height;
    img->bmih.biPlanes = 1;
    switch (img->format & DISPLAY_COLORS_MASK) {
	case DISPLAY_COLORS_NATIVE:
	    switch (img->format & DISPLAY_DEPTH_MASK) {
		case DISPLAY_DEPTH_1:
		    img->bmih.biBitCount = 1;
		    img->bmih.biClrUsed = 2;
		    img->bmih.biClrImportant = 2;
		    break;
		case DISPLAY_DEPTH_4:
		    /* Fixed color palette */
		    img->bmih.biBitCount = 4;
		    img->bmih.biClrUsed = 16;
		    img->bmih.biClrImportant = 16;
		    break;
		case DISPLAY_DEPTH_8:
		    /* Fixed color palette */
		    img->bmih.biBitCount = 8;
		    img->bmih.biClrUsed = 96;
		    img->bmih.biClrImportant = 96;
		    break;
		case DISPLAY_DEPTH_16:
		    /* RGB bitfields */
		    /* Bit fields */
		    if ((img->format & DISPLAY_ENDIAN_MASK)
			== DISPLAY_BIGENDIAN) {
			/* convert to 24BGR */
			img->bmih.biBitCount = 24;
			img->bmih.biClrUsed = 0;
			img->bmih.biClrImportant = 0;
		    }
		    else {
			img->bmih.biBitCount = 16;
			img->bmih.biClrUsed = 0;
			img->bmih.biClrImportant = 0;
		    }
		    break;
		default:
		    return_error(DISPLAY_ERROR);
	    }
	    break;
	case DISPLAY_COLORS_GRAY:
	    switch (img->format & DISPLAY_DEPTH_MASK) {
		case DISPLAY_DEPTH_1:
		    img->bmih.biBitCount = 1;
		    img->bmih.biClrUsed = 2;
		    img->bmih.biClrImportant = 2;
		    break;
		case DISPLAY_DEPTH_4:
		    /* Fixed gray palette */
		    img->bmih.biBitCount = 4;
		    img->bmih.biClrUsed = 16;
		    img->bmih.biClrImportant = 16;
		    break;
		case DISPLAY_DEPTH_8:
		    /* Fixed gray palette */
		    img->bmih.biBitCount = 8;
		    img->bmih.biClrUsed = 256;
		    img->bmih.biClrImportant = 256;
		    break;
		default:
		    return_error(DISPLAY_ERROR);
	    }
	    break;
	case DISPLAY_COLORS_RGB:
	    if ((img->format & DISPLAY_DEPTH_MASK) != DISPLAY_DEPTH_8)
		return_error(DISPLAY_ERROR);
	    if (((img->format & DISPLAY_ALPHA_MASK) == DISPLAY_UNUSED_LAST) &&
		((img->format & DISPLAY_ENDIAN_MASK) == DISPLAY_LITTLEENDIAN)) {
		/* use bitfields to display this */
		img->bmih.biBitCount = 32;
		img->bmih.biClrUsed = 0;
		img->bmih.biClrImportant = 0;
	    }
	    else {
		/* either native BGR, or we need to convert it */
		img->bmih.biBitCount = 24;
		img->bmih.biClrUsed = 0;
		img->bmih.biClrImportant = 0;
	    }
	    break;
	case DISPLAY_COLORS_CMYK:
	    /* we can't display this natively */
	    /* we will convert it just before displaying */
	    img->bmih.biBitCount = 24;
	    img->bmih.biClrUsed = 0;
	    img->bmih.biClrImportant = 0;
	    break;
    }

    img->bmih.biCompression = 0;
    img->bmih.biSizeImage = 0;
    img->bmih.biXPelsPerMeter = 0;
    img->bmih.biYPelsPerMeter = 0;

    /* adjust width until raster is correct */
    img->align_width = img->width;
    while ( (bytewidth = 
	((img->align_width * img->bmih.biBitCount + 31 ) & ~31) >> 3)
	< img->raster ) {
	img->align_width++;
    }
    /* img->bmih.biWidth = img->align_width */

    if (img->palette)
	DeleteObject(img->palette);
    img->palette = image_create_palette(img);

    /* allow window to be resized without user control */
    fit_page_enabled = option.fit_page;

    return 0;
}

/* Convert the raster format if needed */
int
image_sync(IMAGE *img)
{
    return 0;
}


/* This makes a copy of the bitmap in global memory, suitable for clipboard */
/* Do not put 16 or 32-bit per pixels on the clipboard because */
/* ClipBook Viewer (NT4) can't display them */
HGLOBAL 
image_copy_dib(IMAGE *img)
{
    int bitsperpixel;
    int bytewidth;
    int bitmapsize;
    int palcount;
    HGLOBAL hglobal;
    BYTE *pBits;
    BYTE *pLine;
    BYTE *pDIB;
    BITMAPINFOHEADER *pbmih;
    RGBQUAD *pColors;
    int i;
    BOOL directcopy = FALSE;

    /* Allocates memory for the clipboard bitmap */
    if (img->bmih.biBitCount <= 1)
	bitsperpixel = 1;
    else if (img->bmih.biBitCount <= 4)
	bitsperpixel = 4;
    else if (img->bmih.biBitCount <= 8)
	bitsperpixel = 8;
    else
	bitsperpixel = 24;
    bytewidth = ((img->bmih.biWidth * bitsperpixel + 31 ) & ~31) >> 3;
    bitmapsize = bytewidth * img->bmih.biHeight;
    if (bitsperpixel > 8)
	palcount = 0;	/* 24-bit BGR */
    else
	palcount = img->bmih.biClrUsed;

    hglobal = GlobalAlloc(GHND | GMEM_SHARE, sizeof(BITMAPINFOHEADER)
			  + sizeof(RGBQUAD) * palcount + bitmapsize);
    if (hglobal == (HGLOBAL) NULL)
	return (HGLOBAL) NULL;
    pDIB = (BYTE *) GlobalLock(hglobal);
    if (pDIB == (BYTE *) NULL)
	return (HGLOBAL) NULL;


    /* initialize the clipboard bitmap */
    pbmih = (BITMAPINFOHEADER *) (pDIB);
    pColors = (RGBQUAD *) (pDIB + sizeof(BITMAPINFOHEADER));
    pBits = (BYTE *) (pDIB + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * palcount);
    pbmih->biSize = sizeof(BITMAPINFOHEADER);
    pbmih->biWidth = img->bmih.biWidth;
    pbmih->biHeight = img->bmih.biHeight;
    pbmih->biPlanes = 1;
    pbmih->biBitCount = bitsperpixel;
    pbmih->biCompression = 0;
    pbmih->biSizeImage = 0;	/* default */
    pbmih->biXPelsPerMeter = 0;
    pbmih->biYPelsPerMeter = 0;
    pbmih->biClrUsed = palcount;
    pbmih->biClrImportant = palcount;

    for (i = 0; i < palcount; i++) {
	image_color(img->format, i, &pColors[i].rgbRed, 
	    &pColors[i].rgbGreen, &pColors[i].rgbBlue);
	pColors[i].rgbReserved = 0;
    }

    /* find out if the format needs to be converted */
    switch (img->format & DISPLAY_COLORS_MASK) {
	case DISPLAY_COLORS_NATIVE:
	    switch (img->format & DISPLAY_DEPTH_MASK) {
		case DISPLAY_DEPTH_1:
		case DISPLAY_DEPTH_4:
		case DISPLAY_DEPTH_8:
		    directcopy = TRUE;
	    }
	    break;
	case DISPLAY_COLORS_GRAY:
	    switch (img->format & DISPLAY_DEPTH_MASK) {
		case DISPLAY_DEPTH_1:
		case DISPLAY_DEPTH_4:
		case DISPLAY_DEPTH_8:
		    directcopy = TRUE;
	    }
	    break;
	case DISPLAY_COLORS_RGB:
	    if (((img->format & DISPLAY_DEPTH_MASK) == DISPLAY_DEPTH_8) &&
		((img->format & DISPLAY_ALPHA_MASK) == DISPLAY_ALPHA_NONE) &&
		((img->format & DISPLAY_ENDIAN_MASK) == DISPLAY_LITTLEENDIAN))
		directcopy = TRUE;
    }

    pLine = pBits;
    if (directcopy) {
	for (i = 0; i < img->bmih.biHeight; i++) {
	    memcpy(pLine, img->image + i * img->raster, bytewidth);
	    pLine += bytewidth;
	}
    }
    else {
	/* we need to convert the format to 24BGR */
	for (i = 0; i < img->bmih.biHeight; i++) {
	    image_to_24BGR(img, pLine, img->image + i * img->raster);
	    pLine += bytewidth;
	}
    }

    GlobalUnlock(hglobal);

    return hglobal;
}


/* Repaint a section of the window. */
void
image_draw(IMAGE *img, HDC hdc, int dx, int dy, int wx, int wy,
		int sx, int sy)
{
    HPALETTE oldpalette;
    struct bmi_s {
	BITMAPINFOHEADER h;
	unsigned short pal_index[256];
    } bmi;
    int i;
    UINT which_colors;
    unsigned char *line = NULL;
    long ny;
    unsigned char *bits;
    BOOL directcopy = FALSE;

    memset(&bmi.h, 0, sizeof(bmi.h));
    
    bmi.h.biSize = sizeof(bmi.h);
    bmi.h.biWidth = img->bmih.biWidth;
    bmi.h.biHeight = wy;
    bmi.h.biPlanes = 1;
    bmi.h.biBitCount = img->bmih.biBitCount;
    bmi.h.biCompression = 0;
    bmi.h.biSizeImage = 0;	/* default */
    bmi.h.biXPelsPerMeter = 0;	/* default */
    bmi.h.biYPelsPerMeter = 0;	/* default */
    bmi.h.biClrUsed = img->bmih.biClrUsed;
    bmi.h.biClrImportant = img->bmih.biClrImportant;
    
    if (img->bmih.biClrUsed) {
	/* palette colors */
	for (i = 0; i < (int)img->bmih.biClrUsed; i++)
	    bmi.pal_index[i] = i;
	which_colors = DIB_PAL_COLORS;
    } 
    else if (bmi.h.biBitCount == 16) {
	DWORD* bmi_colors = (DWORD*)(&bmi.pal_index[0]);
	bmi.h.biCompression = BI_BITFIELDS;
	which_colors = DIB_RGB_COLORS;
	if ((img->format & DISPLAY_555_MASK) == DISPLAY_NATIVE_555) {
	    /* 5-5-5 RGB mode */
	    bmi_colors[0] = 0x7c00;
	    bmi_colors[1] = 0x03e0;
	    bmi_colors[2] = 0x001f;
	}
	else {
	    /* 5-6-5 RGB mode */
	    bmi_colors[0] = 0xf800;
	    bmi_colors[1] = 0x07e0;
	    bmi_colors[2] = 0x001f;
	}
    }
    else if (bmi.h.biBitCount == 32) {
	unsigned int alpha = img->format & DISPLAY_ALPHA_MASK;
	DWORD* bmi_colors = (DWORD*)(&bmi.pal_index[0]);
	bmi.h.biCompression = BI_BITFIELDS;
	which_colors = DIB_RGB_COLORS;
	if ((img->format & DISPLAY_ENDIAN_MASK) == DISPLAY_BIGENDIAN) {
	    if ((alpha == DISPLAY_ALPHA_FIRST) || 
		(alpha == DISPLAY_UNUSED_FIRST)) {
		/* Mac mode */
		bmi_colors[0] = 0x0000ff00;
		bmi_colors[1] = 0x00ff0000;
		bmi_colors[2] = 0xff000000;
	    }
	    else {
		bmi_colors[0] = 0x000000ff;
		bmi_colors[1] = 0x0000ff00;
		bmi_colors[2] = 0x00ff0000;
	    }
	}
	else {
	    if ((alpha == DISPLAY_ALPHA_FIRST) || 
		(alpha == DISPLAY_UNUSED_FIRST)) {
		/* ignore alpha */
		bmi_colors[0] = 0xff000000;
		bmi_colors[1] = 0x00ff0000;
		bmi_colors[2] = 0x0000ff00;
	    }
	    else {
		/* Windows mode */
		/* ignore alpha */
		bmi_colors[0] = 0x00ff0000;
		bmi_colors[1] = 0x0000ff00;
		bmi_colors[2] = 0x000000ff;
	    }
	}
    } else {
	bmi.h.biClrUsed = 0;
	bmi.h.biClrImportant = 0;
	which_colors = DIB_RGB_COLORS;
    }

    if (img->raster <= 0)
	return;

    /* Determine if the format is native and we can do a direct copy */
    switch (img->format & DISPLAY_COLORS_MASK) {
	case DISPLAY_COLORS_NATIVE:
	    switch (img->format & DISPLAY_DEPTH_MASK) {
		case DISPLAY_DEPTH_1:
		case DISPLAY_DEPTH_4:
		case DISPLAY_DEPTH_8:
		    directcopy = TRUE;
		    break;
		case DISPLAY_DEPTH_16:
		    if ((img->format & DISPLAY_ENDIAN_MASK)
			== DISPLAY_LITTLEENDIAN)
			directcopy = TRUE;
		    break;
	    }
	    break;
	case DISPLAY_COLORS_GRAY:
	    switch (img->format & DISPLAY_DEPTH_MASK) {
		case DISPLAY_DEPTH_1:
		case DISPLAY_DEPTH_4:
		case DISPLAY_DEPTH_8:
		    directcopy = TRUE;
	    }
	    break;
	case DISPLAY_COLORS_RGB:
	    if (((img->format & DISPLAY_DEPTH_MASK) == DISPLAY_DEPTH_8) &&
	        ((img->format & DISPLAY_ENDIAN_MASK) == DISPLAY_LITTLEENDIAN) &&
	        ((img->format & DISPLAY_ALPHA_MASK) == DISPLAY_ALPHA_NONE))
		directcopy = TRUE;	/* BGR24 */
	    if (((img->format & DISPLAY_DEPTH_MASK) == DISPLAY_DEPTH_8) &&
	        ((img->format & DISPLAY_ENDIAN_MASK) == DISPLAY_LITTLEENDIAN) &&
	        ((img->format & DISPLAY_ALPHA_MASK) == DISPLAY_UNUSED_LAST))
		directcopy = TRUE;	/* 32-bit */
	    break;
    }


    if (which_colors == DIB_PAL_COLORS) {
	oldpalette = SelectPalette(hdc, img->palette, FALSE);
	RealizePalette(hdc);
    }

    /*
     * Windows apparently limits the size of a single transfer
     * to 2 Mb, which can be exceeded on 24-bit displays.
     */
    ny = 2000000 / img->raster;

    if (img->width != img->align_width)	/* not 32-bit architecture */
	ny = 1;

    /* If color format not native, convert it line by line */
    /* This is slow, but these formats aren't normally used */
    if (!directcopy) {
        int bytewidth = ((img->width * 24 + 31 ) & ~31) >> 3;
	ny = 1;
	line = (unsigned char *)malloc(bytewidth);
	if (line == NULL)
	    return;
    }

    for (; wy; dy += ny, wy -= ny, sy += ny) {
	ny = min(ny, wy);
	if (directcopy) {
	    bits = img->image + img->raster * (img->bmih.biHeight - (sy + ny));
	}
	else {
	    image_to_24BGR(img, line,
		img->image + img->raster * (img->bmih.biHeight - (sy + ny)));
	    bits = line;
	}
	SetDIBitsToDevice(hdc, dx, dy, wx, ny, sx, 0, 0, ny, bits,
		  (BITMAPINFO *) & bmi, which_colors);
    }
    
    if (which_colors == DIB_PAL_COLORS)
	SelectPalette(hdc, oldpalette, FALSE);

    if (line)
	free(line);
}

int image_update_time(IMAGE *img)
{
    SYSTEMTIME now;
    int now_ms;
    GetSystemTime(&now);
    now_ms = now.wHour * 36000000 + now.wMinute * 60000 +
	    now.wSecond * 1000 + now.wMilliseconds;
    if (now_ms < img->tile_time)
	img->tile_time = now_ms;
    if (now_ms - img->tile_time < img->tile_interval)
	return 1;	/* less than 100ms since last update */
    img->tile_time = now_ms;
    return 0;
}

