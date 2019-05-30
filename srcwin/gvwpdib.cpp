/* Copyright (C) 2000, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

// gvwpdib.cpp: implementation of the CPrintDIB class.

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include "gvcfile.h"
#include "gvwdib.h"
#include "gvwpdib.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrintDIB::CPrintDIB()
{
    ResetPrintLine();
    m_iBlackPalIndex = -1;
    m_iWhitePalIndex = -1;
    m_iWhiteAltPalIndex = -1;
}

CPrintDIB::~CPrintDIB()
{
    if (m_bits)
	    delete [] m_bits;
}


BOOL CPrintDIB::ReadHeader(GFile *cf)
{
    if (m_bits != NULL)
	    delete [] m_bits;
    m_bits = NULL;

    if (!CDIB::ReadHeader(cf))
	return FALSE;
    // copy the bitmap header to keep a copy that refers to entire page
    memcpy(&m_PageBmp, &m_bmp, sizeof(m_PageBmp));

    ResetPrintLine();				// no scan lines yet
    m_bits = new BYTE[MAX_PRINT_BITMAP];	// GlobalAlloc??
    GetBlackWhiteIndex();			// Which palette entry is white?
    return TRUE;
}

// Find palette indices for black and white, and the all white byte
BOOL CPrintDIB::GetBlackWhiteIndex()
{
    if (m_bmp.bmp2.biBitCount > 8)
	return FALSE;

    // Only makes sense for palette bitmaps
    int palcount = PalCount();
    m_iBlackPalIndex = -1;
    m_iWhitePalIndex = -1;
    m_iWhiteAltPalIndex = -1;
    m_chWhite = 0;
    m_chWhiteAlt = 0;
    LPPALETTEENTRY lpe = new PALETTEENTRY[palcount];
    LPPALETTEENTRY pe;
    GetPaletteEntries(m_palette, 0, palcount, lpe);
    // figure out which palette entry is white
    int i;
    for (i=0; i<palcount; i++) {
	pe = &(lpe[i]);
	if ( (pe->peRed == 0xff) && (pe->peGreen == 0xff) &&
	    (pe->peBlue == 0xff) ) {
	    // remember up to two white indexes
	    if (m_iWhitePalIndex < 0)
		m_iWhitePalIndex = i;
	    else if (m_iWhiteAltPalIndex < 0)
		m_iWhiteAltPalIndex = i;
	}
	if ( (pe->peRed == 0) && (pe->peGreen == 0) &&
	     (pe->peBlue == 0) ) {
	    if (m_iBlackPalIndex < 0)
		m_iBlackPalIndex = i;
	}
    }
    delete [] lpe;

    if (m_iWhitePalIndex < 0)
	return FALSE;	// White isn't in palette!

    switch (m_bmp.bmp2.biBitCount) {
	case 1:
	    m_chWhite = (BYTE)(m_iWhitePalIndex ? 0xff : 0);
	    m_chWhiteAlt = m_chWhite;
	    break;
	case 4:
	    m_chWhite = (BYTE)(m_iWhitePalIndex + (i << 4));
	    m_chWhiteAlt = (BYTE)((m_iWhiteAltPalIndex < 0) ?
		    m_chWhite : (m_iWhiteAltPalIndex + (i << 4)));
	    break;
	case 8:
	    m_chWhite = (BYTE)(m_iWhitePalIndex);
	    m_chWhiteAlt = (BYTE)((m_iWhiteAltPalIndex < 0) ?
		    m_chWhite : m_iWhiteAltPalIndex);
	    break;
	default:
	    ASSERT(FALSE);
    }
    return TRUE;
}


// Look for all white lines in a monochrome bitmap
// Returns TRUE if line all white
BOOL CPrintDIB::ScanForWhite(LPBYTE pLine)
{
    ASSERT(m_bmp.bmp2.biBitCount == 1);

    BOOL bWhite = TRUE;
    if (m_iWhitePalIndex < 0)		// white isn't in palette
	return FALSE;
    for (UINT i = 0; i < m_bytewidth; i++) {
	if (pLine[i] != m_chWhite) {
	    bWhite = FALSE;
	    if (i < m_nLeftMark)
		m_nLeftMark = i;
	    if (i > m_nRightMark)
		m_nRightMark = i;
	}
    }
    return bWhite;
}

// Look for all white lines in a grey (8 bit/pixel) bitmap
BOOL CPrintDIB::ScanForGrey(LPBYTE pLine)
{
    ASSERT(m_bmp.bmp2.biBitCount == 8);

    UINT width = m_PageBmp.bmp2.biWidth;
    BOOL bWhite = TRUE;
    if (m_iWhitePalIndex < 0)		// white isn't in palette
	return FALSE;
    for (UINT i = 0; i < width; i++) {
	if ((pLine[i] != m_chWhite) && (pLine[i] != m_chWhiteAlt)) {
	    bWhite = FALSE;
	    if (i < m_nLeftMark)
		m_nLeftMark = i;
	    if (i > m_nRightMark)
		m_nRightMark = i;
	    if (pLine[i] != m_iBlackPalIndex)
		m_bGrey = TRUE;  // found something other than black or white
	}
    }
    return bWhite;
}

// Look for all white lines in a colour (24 bit/pixel) bitmap
BOOL CPrintDIB::ScanForColour(LPBYTE pLine)
{
    BYTE r, g, b;

    BOOL bWhite = TRUE;
    ASSERT(m_bmp.bmp2.biBitCount == 24);
    UINT width = m_PageBmp.bmp2.biWidth * 3;
    for (UINT i = 0; i < width; i+=3) {
	b = *pLine++;
	g = *pLine++;
	r = *pLine++;
	if ((r!=0xff) || (g!=0xff) || (b!=0xff)) {
	    bWhite = FALSE;
	    if (i < m_nLeftMark)
		m_nLeftMark = i;
	    if (i > m_nRightMark)
		m_nRightMark = i;
	    if ((r!=0) || (g!=0) || (b!=0))
		m_bGrey = TRUE;
	    if ((r!=g) || (g!=b))
		m_bColour = TRUE;
	}
    }
    return bWhite;
}


// The output buffer is now empty
void CPrintDIB::ResetPrintLine()
{
    // Revert back to page settings
    // Copy PageBmp header to CDIB header
    memcpy(&m_bmp, &m_PageBmp, sizeof(m_bmp));
    m_bytewidth = ((m_PageBmp.bmp2.biWidth * 
	    m_PageBmp.bmp2.biBitCount + 31) & (~31)) >> 3;

    m_bmp.bmp2.biHeight = 0;	// no scan lines yet
    m_nPrintCount = 0;
    m_yOrigin = 0;		// redundant

    m_bPrevNotWhite = FALSE;
    m_bColour = FALSE;
    m_bGrey = FALSE;
    m_nLeftMark = m_bytewidth;
    m_nRightMark = 0;
}


// Add a scan line to the output buffer
void CPrintDIB::AddPrintLine(HDC hdc, UINT row, LPBYTE pByte)
{
    ASSERT(m_bmp.bmp2.biBitCount != 4);
    ASSERT(m_bmp.bmp2.biBitCount != 16);
    ASSERT(m_bmp.bmp2.biBitCount != 32);
    // Support only 1, 8 (preferably grey) and 24 bits per pixel

    if (m_nPrintCount + m_bytewidth > MAX_PRINT_BITMAP)
	FlushPrintBitmap(hdc);

    // Check for blank lines, B/W lines and colour lines
    BOOL bWhite;
    switch (m_bmp.bmp2.biBitCount) {
	case 1:
	    bWhite = ScanForWhite(pByte);
	    break;
	case 8:
	    bWhite = ScanForGrey(pByte);
	    break;
	case 24:
	    bWhite = ScanForColour(pByte);
	    break;
	default:
	    ASSERT(FALSE);
    }

    if (bWhite) {
	// Don't print all white lines
	if (m_bPrevNotWhite) {
	    // Copy out all previous non-white lines
	    FlushPrintBitmap(hdc);
	    ResetPrintLine();
	}
    }
    else {
	if (!m_bPrevNotWhite) {
	    // First non-blank line, so remember y coordinate
	    m_yOrigin = row;
	}
	CopyMemory(m_bits + m_nPrintCount, pByte, m_bytewidth);
	m_bmp.bmp2.biHeight++;
	m_nPrintCount += m_bytewidth;
	m_bPrevNotWhite = TRUE;
    }
}

void CPrintDIB::TrimBitmap()
{
    if (m_nLeftMark > m_nRightMark) {
	ASSERT(FALSE);
	// Should never be asked to print blank lines
	m_xOrigin = 0;
	return;
    }

    UINT bpp = m_bmp.bmp2.biBitCount;
    if (bpp <= 8)
	m_nRightMark++;
    else
	m_nRightMark += bpp / 8;
    
    if ((m_nLeftMark == 0) && (m_nRightMark == m_bytewidth)) {
	// nothing to trim
	m_xOrigin = 0;
	return;
    }

    // new width is set by non-white pixels
    int new_width = m_nRightMark * 8 / bpp;
    // make sure new width doesn't exceed original width
    if (new_width > m_bmp.bmp2.biWidth)
	new_width = m_bmp.bmp2.biWidth;
    // remove left white pixels
    new_width -= m_nLeftMark * 8 / bpp;

    UINT new_bytewidth = 
	((new_width * bpp + 31) & (~31)) >> 3;

    LPBYTE s = m_bits + m_nLeftMark;
    LPBYTE d = m_bits;
    int i;
    for (i=0; i<m_bmp.bmp2.biHeight; i++) {
	MoveMemory(d, s, new_bytewidth);
	s += m_bytewidth;
	d += new_bytewidth;
    }
    m_bmp.bmp2.biWidth = new_width;
    m_bytewidth = new_bytewidth;

    m_xOrigin = m_nLeftMark * 8 / bpp;
    return;
}

void CPrintDIB::MakePaletteMono()
{
    m_bmp.bmp2.biCompression = BI_RGB;
    m_bmp.bmp2.biBitCount = 1;
    m_bmp.bmp2.biClrUsed = 0;
    m_bmp.bmp2.biClrImportant = 0;

    LPBYTE p = m_colors;

    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    if (!m_old)
	*p++ = 0;
    *p++ = 255;
    *p++ = 255;
    *p++ = 255;
    if (!m_old)
	*p   = 0;
    
    MakePalette();
}

void CPrintDIB::ConvertGreyToMono()
{
    ASSERT(m_bmp.bmp2.biBitCount == 8);

    UINT new_bpp = 1;
    UINT new_bytewidth = 
	((m_bmp.bmp2.biWidth * new_bpp + 31) & (~31)) >> 3;

    int x, y;
    int ch;
    LPBYTE s, d;
    BYTE pixelbyte;
    int mask;
    int width = m_bmp.bmp2.biWidth;
    for (y=0; y<m_bmp.bmp2.biHeight; y++) {
	s = m_bits + (y * m_bytewidth);
	d = m_bits + (y * new_bytewidth);
	pixelbyte = 0xff;	// white
	mask = 0x80;
	for (x=0; x<width; x++) {
	    ch = *s++;
	    if ((ch != m_chWhite) && (ch != m_chWhiteAlt)) // 8 bit grey
		pixelbyte &= (BYTE)(~mask);	// non-white pixels are black
	    //else
	    //	pixelbyte |= mask;
	    mask >>= 1;
	    if ((x & 7) == 7) {
		*d++ = pixelbyte;
		pixelbyte = 0xff;
		mask = 0x80;
	    }
	}
	while (x < (int)(new_bytewidth * 8)) {
	    // write out remaining bits and fill extra bytes with white
	    *d++ = pixelbyte;
	    pixelbyte = 0xff;
	    x += 8;
	}
    }

    m_bmp.bmp2.biCompression = BI_RGB;
    m_bytewidth = new_bytewidth;
    m_bmp.bmp2.biSizeImage = m_bmp.bmp2.biHeight * m_bytewidth;
    MakePaletteMono();
}

void CPrintDIB::ConvertColourToMono()
{
    ASSERT(m_bmp.bmp2.biBitCount == 24);

    UINT new_bpp = 1;
    UINT new_bytewidth = 
	((m_bmp.bmp2.biWidth * new_bpp + 31) & (~31)) >> 3;
    int x, y;
    LPBYTE s, d;
    BYTE pixelbyte;
    int mask;
    BYTE r, g, b;	// colour
    int width = m_bmp.bmp2.biWidth;
    for (y=0; y<m_bmp.bmp2.biHeight; y++) {
	s = m_bits + (y * m_bytewidth);
	d = m_bits + (y * new_bytewidth);
	pixelbyte = 0xff;	// white
	mask = 0x80;
	for (x=0; x<width; x++) {
	    b = *s++;		// colour
	    g = *s++;		// colour
	    r = *s++;		// colour
	    if ((r!=0xff) || (g!=0xff) || (b!=0xff))	// colour
		pixelbyte &= (BYTE)(~mask);	// non-white pixels are black
	    //else
	    //	pixelbyte |= 0x01;
	    mask >>= 1;
	    if ((x & 7) == 7) {
		*d++ = pixelbyte;
		pixelbyte = 0xff;
		mask = 0x80;
	    }
	}
	while (x < (int)(new_bytewidth * 8)) {
	    // write out remaining bits and fill extra bytes with white
	    *d++ = pixelbyte;
	    pixelbyte = 0xff;
	    x += 8;
	}
    }

    m_bmp.bmp2.biCompression = BI_RGB;
    m_bytewidth = new_bytewidth;
    m_bmp.bmp2.biSizeImage = m_bmp.bmp2.biHeight * m_bytewidth;
    MakePaletteMono();
}

// TrimBitmap() and ConvertXxxToMono() change m_bmp, m_bytewidth
// and the palette.  These need to be restored in ResetPrintLine()
void CPrintDIB::FlushPrintBitmap(HDC hdc)
{
    if (m_nPrintCount == 0) {
	    ResetPrintLine();
	    return;
    }


    // Compact the bitmap to remove white at left and right
    TrimBitmap();

    if ( (m_bmp.bmp2.biBitCount == 8) && !m_bGrey ) {
	// convert 8 bit grey scale to B/W
	ConvertGreyToMono();
    }
    if ( (m_bmp.bmp2.biBitCount == 24) && !m_bGrey && !m_bColour ) {
	// convert 24 bit colour to B/W
	ConvertColourToMono();
    }


    // Invert y axis and adjust for margins
    int x = -GetDeviceCaps(hdc, PHYSICALOFFSETX);
    x += m_xOrigin;
    int y = GetDeviceCaps(hdc, PHYSICALHEIGHT);
    y -= GetDeviceCaps(hdc, PHYSICALOFFSETY);
    y -= m_yOrigin;
    y -= m_bmp.bmp2.biHeight-1;

    // Draw bitmap to device
    Draw(hdc, x, y);
    
    if (debug) {
	// Draw RED rectangle around the bitmap, so we can tell 
	// where the bitmaps are located.
	HPEN new_pen = CreatePen(PS_SOLID, 1, RGB(255,0,0));
	HPEN old_pen = SelectPen(hdc, new_pen);
	MoveToEx(hdc, x, y, NULL);
	LineTo(hdc, x+m_bmp.bmp2.biWidth, y);
	LineTo(hdc, x+m_bmp.bmp2.biWidth, y+m_bmp.bmp2.biHeight);
	LineTo(hdc, x, y+m_bmp.bmp2.biHeight);
	LineTo(hdc, x, y);
	SelectPen(hdc, old_pen);
	DeletePen(new_pen);
    }

    // Empty bitmap
    ResetPrintLine();
    MakePalette();
}


