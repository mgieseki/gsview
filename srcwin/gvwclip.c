/* Copyright (C) 1993-1996, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwclip.c */
/* Clipboard module for Windows GSview */
#include "gvwin.h"

void paste_to_file(void);
void clip_convert(void);
void clip_add_palette(void);
void clip_add_ddb(void);
void clip_add_dib(void);

/* copy a DIB from the clipboard to a file */
void
paste_to_file(void)
{
HGLOBAL hglobal;
LPBITMAPINFOHEADER pbmih;
BITMAPFILEHEADER bmfh;
UINT palcolors;
UINT palsize;
DWORD bitmap_size;
BYTE *lpBits;
static char output[MAXSTR];
HFILE hfile;
	if (!OpenClipboard(hwndimg)) {
		play_sound(SOUND_ERROR);
		return;
	}
	if (!IsClipboardFormatAvailable(CF_DIB)) {
		CloseClipboard();
		play_sound(SOUND_ERROR);
		return;
	}
	hglobal = (HGLOBAL)GetClipboardData(CF_DIB);
	pbmih = (LPBITMAPINFOHEADER)GlobalLock(hglobal);
	palcolors = dib_pal_colors((unsigned char *)pbmih);
	if (pbmih->biSize == sizeof(BITMAPCOREHEADER))
		palsize = palcolors * sizeof(RGBTRIPLE); 
	else
		palsize = palcolors * sizeof(RGBQUAD);
	bitmap_size = (DWORD)pbmih->biHeight *  dib_bytewidth((unsigned char *)pbmih);

	bmfh.bfType = ('M'<<8) | 'B';
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + pbmih->biSize + palsize;
	bmfh.bfSize = bmfh.bfOffBits + bitmap_size;

	if ( get_filename(output, TRUE, FILTER_BMP, 0, IDS_TOPICCLIP)
	    && ((hfile = _lcreat(output, 0)) != HFILE_ERROR) ) {
		_hwrite(hfile, (const char *)&bmfh, sizeof(BITMAPFILEHEADER));
		_hwrite(hfile, (const char *)pbmih, pbmih->biSize + palsize);
		lpBits =  ((BYTE *)pbmih) + pbmih->biSize + palsize;
		_hwrite(hfile, (const char *)lpBits, bitmap_size);
		_lclose(hfile);
	}
	GlobalUnlock(hglobal);
	CloseClipboard();
}

/* convert bitmap (DIB or DDB) in clipboard to */
/* CF_DIB, CF_BITMAP and CF_PALETTE */
void
clip_convert(void)
{
	if (!OpenClipboard(hwndimg)) {
	    play_sound(SOUND_ERROR);
	    return;
	}
	if (IsClipboardFormatAvailable(CF_DIB)) {
	    if (!IsClipboardFormatAvailable(CF_PALETTE))
		clip_add_palette();
	    if (!IsClipboardFormatAvailable(CF_BITMAP))
	        clip_add_ddb();
	}
	else {
	    if (IsClipboardFormatAvailable(CF_BITMAP)) {
		clip_add_dib();
	        if (!IsClipboardFormatAvailable(CF_PALETTE))
		    clip_add_palette();
	    }
	    else 
		play_sound(SOUND_ERROR);
	}
	CloseClipboard();
}

/* Read DIB from the clipboard, create PALETTE and add to clipboard */
void
clip_add_palette(void)
{
HGLOBAL hglobal;
LPBITMAPINFOHEADER pbmih;
UINT palcolors;
UINT palsize;
int i;
LPLOGPALETTE logpalette;
HPALETTE hpalette;
RGBQUAD FAR *prgbquad;
RGBTRIPLE FAR *prgbtriple;

	if (!IsClipboardFormatAvailable(CF_DIB)) {
		play_sound(SOUND_ERROR);
		return;
	}
	hglobal = (HGLOBAL)GetClipboardData(CF_DIB);
	pbmih = (LPBITMAPINFOHEADER)GlobalLock(hglobal);
	palcolors = dib_pal_colors((unsigned char *)pbmih);
	if (pbmih->biSize == sizeof(BITMAPCOREHEADER))
		palsize = palcolors * sizeof(RGBTRIPLE); 
	else
		palsize = palcolors * sizeof(RGBQUAD);
	if (palsize) {
	    /* create palette to match DIB */
	    logpalette = (LPLOGPALETTE) malloc( sizeof(LOGPALETTE) + 
		palcolors * sizeof(PALETTEENTRY) );
	    if (logpalette == (LPLOGPALETTE)NULL) {
		GlobalUnlock(hglobal);
		play_sound(SOUND_ERROR);
		return;
	    }
	    logpalette->palVersion = 0x300;
	    logpalette->palNumEntries = (WORD)palcolors;
	    prgbquad = (RGBQUAD FAR *)(((BYTE *)pbmih) + pbmih->biSize);
	    if (pbmih->biSize == sizeof(BITMAPCOREHEADER)) {
		/* OS2 format */
	        prgbtriple = (RGBTRIPLE FAR *)prgbquad;
	        for (i=0; i<(int)palcolors; i++) {
	            logpalette->palPalEntry[i].peFlags = 0;
		    logpalette->palPalEntry[i].peRed   = prgbtriple[i].rgbtRed;
		    logpalette->palPalEntry[i].peGreen = prgbtriple[i].rgbtGreen;
		    logpalette->palPalEntry[i].peBlue  = prgbtriple[i].rgbtBlue;
	        }
	    }
	    else {
		/* Windows Format */
	        for (i=0; i<(int)palcolors; i++) {
	            logpalette->palPalEntry[i].peFlags = 0;
		    logpalette->palPalEntry[i].peRed   = prgbquad[i].rgbRed;
		    logpalette->palPalEntry[i].peGreen = prgbquad[i].rgbGreen;
		    logpalette->palPalEntry[i].peBlue  = prgbquad[i].rgbBlue;
	        }
	    }
	    hpalette = CreatePalette(logpalette);
	    free((void *)logpalette);
	    SetClipboardData(CF_PALETTE, hpalette);
	}
	GlobalUnlock(hglobal);
}


/* Read DIB from the clipboard, convert to DDB and add to clipboard */
void
clip_add_ddb(void)
{
HGLOBAL hglobal;
LPBITMAPINFOHEADER pbmih;
UINT palcolors;
UINT palsize;
HPALETTE hpalette;
HDC hdc;
HBITMAP hbitmap;

	hglobal = (HGLOBAL)GetClipboardData(CF_DIB);
	pbmih = (LPBITMAPINFOHEADER)GlobalLock(hglobal);
	palcolors = dib_pal_colors((unsigned char *)pbmih);
	if (pbmih->biSize == sizeof(BITMAPCOREHEADER))
		palsize = palcolors * sizeof(RGBTRIPLE); 
	else
		palsize = palcolors * sizeof(RGBQUAD);

	hdc = GetDC(hwndimg);
	hpalette = (HPALETTE)GetClipboardData(CF_PALETTE);
	if (hpalette) {
	    SelectPalette(hdc,hpalette,FALSE);
	    RealizePalette(hdc);
	}
	hbitmap = CreateDIBitmap(hdc, pbmih, CBM_INIT,
		((BYTE *)pbmih) + pbmih->biSize + palsize,
		(LPBITMAPINFO)pbmih, DIB_RGB_COLORS);
	ReleaseDC(hwndimg, hdc);
	GlobalUnlock(hglobal);
	SetClipboardData(CF_BITMAP, hbitmap);
}

/* make a DIB from a BITMAP in the clipboard */
/* GetDIBits won't work for 4 plane or 4 bit/pixel bitmaps */
/* clipboard must be open */
HGLOBAL 
make_dib(void)
{
LPBITMAPINFOHEADER pbmih;
LPBITMAPINFO pbmi;
BYTE FAR *lpBits;
HBITMAP hbitmap;
UINT palcolors;
UINT palsize;
UINT byte_width;
DWORD bitmap_size;
HGLOBAL hglobal;
HDC hdc;
HDC hdc_bit;
BITMAP bm;
PALETTEENTRY *pe;
int i;
	hbitmap = (HBITMAP)GetClipboardData(CF_BITMAP);
	hdc = GetDC((HWND)NULL);
	hdc_bit = CreateCompatibleDC(hdc);
	ReleaseDC((HWND)NULL,hdc);
	GetObject(hbitmap, sizeof(BITMAP), &bm);
	if (bm.bmPlanes == 4) {
	    HBITMAP hbitmap_new, hbitmap_old;
	    HDC hdc_new;
	    /* convert format to 1 plane, 1 bit/pixel */ 
	    bm.bmPlanes = 1;
	    bm.bmBitsPixel = 1;
	    hdc_new = CreateCompatibleDC(hdc_bit);
	    hbitmap_new = CreateBitmap(bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel, NULL);
	    SelectBitmap(hdc_new, hbitmap_new);
	    hbitmap_old = SelectBitmap(hdc_bit, hbitmap);
	    BitBlt(hdc_new, 0, 0, bm.bmWidth, bm.bmHeight, hdc_bit, 0, 0, SRCCOPY);
	    SelectBitmap(hdc_bit, hbitmap_old);
	    DeleteDC(hdc_new);
	    hbitmap = hbitmap_new;
	}
 	byte_width = (((bm.bmWidth * bm.bmBitsPixel + 31) & ~31) >> 3);
	bitmap_size = (DWORD)bm.bmHeight *  byte_width;
	palcolors = 1<<(bm.bmBitsPixel * bm.bmPlanes);
	palsize = palcolors * sizeof(RGBQUAD);
	hglobal = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + palsize + bitmap_size);
	if (hglobal != (HGLOBAL)NULL) {
	    lpBits = (BYTE FAR *)GlobalLock(hglobal);
	    pbmi = (LPBITMAPINFO)lpBits;
	    pbmih = (LPBITMAPINFOHEADER)lpBits;
	    lpBits += sizeof(BITMAPINFOHEADER) + palsize;
	    pbmih->biSize = sizeof(BITMAPINFOHEADER);
	    pbmih->biWidth = bm.bmWidth;
	    pbmih->biHeight = bm.bmHeight;
	    pbmih->biPlanes = 1;
	    pbmih->biBitCount = (WORD)(bm.bmBitsPixel * bm.bmPlanes);
	    pbmih->biCompression = BI_RGB;
	    pbmih->biSizeImage = bitmap_size;
	    pbmih->biXPelsPerMeter = (int)(option.xdpi / 25.4 * 1000);
	    pbmih->biYPelsPerMeter = (int)(option.ydpi / 25.4 * 1000);
	    pbmih->biClrUsed = palcolors;
	    pbmih->biClrImportant = palcolors;
	    /* create colour table from system palette */
	    pe = (PALETTEENTRY *)malloc(palcolors * sizeof(PALETTEENTRY));
	    if (IsClipboardFormatAvailable(CF_PALETTE)) {
		HPALETTE hpalette = (HPALETTE)GetClipboardData(CF_PALETTE);
		i = GetObject(hpalette, sizeof(int), pe);
		GetPaletteEntries(hpalette, 0, i, pe);
	    }
	    else
	        GetSystemPaletteEntries(hdc_bit, 0, palcolors, pe);
	    for (i=0; i<(int)palcolors; i++) {
		pbmi->bmiColors[i].rgbRed = pe[i].peRed;
		pbmi->bmiColors[i].rgbGreen = pe[i].peGreen;
		pbmi->bmiColors[i].rgbBlue = pe[i].peBlue;
		pbmi->bmiColors[i].rgbReserved = 0;
	    }
	    free((void *)pe);
	    i = GetDIBits(hdc_bit, hbitmap, 0, (UINT)pbmih->biHeight,
		lpBits, pbmi, DIB_RGB_COLORS);
	    GlobalUnlock(hglobal);
	    if (i == 0) {
		GlobalFree(hglobal);
		hglobal = NULL;
	    }
	}
	DeleteDC(hdc_bit);
	if (hbitmap != GetClipboardData(CF_BITMAP))
		DeleteBitmap(hbitmap);
	return hglobal;
}

/* Read DDB from the clipboard, convert to DIB and add to clipboard */
void
clip_add_dib(void)
{
HGLOBAL hglobal;
	hglobal = make_dib();
	if (hglobal != (HGLOBAL)NULL)
	        SetClipboardData(CF_DIB, hglobal);
}

#ifdef NOT_USED
/* not used */
/* Read bitmap (CF_DIB or CF_BITMAP) from the clipboard */
/* convert to Metafile Picture and add to clipboard */
void
clip_add_metafile(void)
{
LPMETAFILEPICT lpmfp;
HGLOBAL hglobal;
HMETAFILE hmf;
	if ( (hmf = make_metafile()) == (HMETAFILE)NULL ) {
		return;
	}

	hglobal = GlobalAlloc(GHND | GMEM_SHARE, sizeof(METAFILEPICT)); 
	lpmfp = GlobalLock(hglobal);
	lpmfp->mm = MM_ANISOTROPIC;
	if (bitmap_width)
	    lpmfp->xExt = (int)(bitmap_width / xdpi * 2540);
	else 
	    lpmfp->xExt = 21000;	/* A4 */
	if (bitmap_height)
	    lpmfp->yExt = (int)(bitmap_height / ydpi * 2540);
	else
	    lpmfp->yExt = 29700;	/* A4 */
	lpmfp->hMF = hmf;
	GlobalUnlock(hglobal);
	SetClipboardData(CF_METAFILEPICT, hglobal);
	return;
}
#endif
