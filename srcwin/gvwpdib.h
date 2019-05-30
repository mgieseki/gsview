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

// gvwpdib.h: interface for the CPrintDIB class.


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// We derive from CDIB, and build up a bitmap in the CDIB,
// keeping the bitmap size less than 1 Mbyte.
#define MAX_PRINT_BITMAP 1048576

class CPrintDIB : public CDIB  
{
public:
    CPrintDIB();
    virtual ~CPrintDIB();

    // We keep a second BITMAP2 header for the full page
    // and refer to it occasionally.
    BITMAP2INFO m_PageBmp;		// Bitmap header for a full page

    // To minimise the output bitmap size, we try to break up
    // the page bitmap into smaller bitmaps.
    // 1. Break the bitmap into chunks that do not include blank lines.
    // 2. Break the bitmap into chunks smaller than 1Mbyte.
    // 3. Remove blank pixels from left and right
    // 4. Convert bitmaps to monochrome where possible
    // The following member variables assist us with this.
    UINT m_nPrintCount;		// bytes written to pBits
    UINT m_xOrigin;		// origin for bitmap being built
    UINT m_yOrigin;
    UINT m_nLeftMark;		// Left most pixel that is not white
    UINT m_nRightMark;		// Right most pixel that is not white
				// If left > right, all pixels are white
    BOOL m_bPrevNotWhite;	// Previous scan line was not white
    BOOL m_bColour;		// Bitmap contains colour
    BOOL m_bGrey;		// Bitmap contains greys (other than B/W)
    int	m_iBlackPalIndex;	// Index of black palette entry, negative if none
    int m_iWhitePalIndex;	// Index of white palette entry, negative if none
    int m_iWhiteAltPalIndex;	// Alternative white index
    BYTE m_chWhite;		// Byte for all white pixels (1 or 8 bpp)
    BYTE m_chWhiteAlt;		// Alternate byte for all white pixels, where
				// there are two palette entries for white.
    BOOL debug;

    // implementation
    BOOL ReadHeader(GFile *cf);
    BOOL GetBlackWhiteIndex();
    BOOL ScanForWhite(LPBYTE pLine);
    BOOL ScanForGrey(LPBYTE pLine);
    BOOL ScanForColour(LPBYTE pLine);
    void ResetPrintLine();
//    void AddPrintLine(CDC *pDC, UINT row, LPBYTE pByte);
    void AddPrintLine(HDC hdc, UINT row, LPBYTE pByte);
    void TrimBitmap();
    void MakePaletteMono();
    void ConvertGreyToMono();
    void ConvertColourToMono();
//    void FlushPrintBitmap(CDC *pDC);
    void FlushPrintBitmap(HDC hdc);


};

