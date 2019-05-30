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


// DIB.cpp: implementation of the CDIB class.

// To change this back to using MFC:
// Replace GFile with CFile
// Exception handling around CFile::Read
// CDC not HDC
// delete palette, brush etc, not DeleteObject

#define STRICT
#include <windows.h>
#include <windowsx.h>
extern "C" {
#include "gvcfile.h"
}
#include "gvwdib.h"



#ifdef NOTUSED
#include "stdafx.h"
#include "afxmt.h"
//#include "GSview.h"
#include "DIB.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#endif


// This code assumes that structures elements are PACKED 
// on byte boundaries.

static unsigned long dsc_arch = 0x00000001;  // to determine if we are little endian


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDIB::CDIB(void)
{
    m_palette = NULL;
    m_bitmap = NULL;
    m_colors = NULL;
    m_bits = NULL;
    m_valid = FALSE;
}

CDIB::~CDIB(void)
{
    CleanupAll();
}

DWORD
CDIB::GetDword(LPBYTE pbyte)
{
    if (m_little_endian)
        return (*(LPDWORD)pbyte);
    else
         return ( pbyte[0] | (pbyte[1]<<8) | (pbyte[2]<<16) | (pbyte[3]<<24) );
}

WORD
CDIB::GetWord(LPBYTE pbyte)
{
    if (m_little_endian)
        return (*(LPWORD)pbyte);
    else
        return (WORD)( pbyte[0] | (pbyte[1]<<8) );
}


void
CDIB::Draw(HDC hdc)
{
    Draw(hdc, 0, 0);
}

void CDIB::Draw(HDC hdc, int xOffset, int yOffset)
{
    if (!m_valid) {
	RECT rect;
	GetClipBox(hdc, &rect);
	FillRect(hdc, &rect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
	return;
    }

    Lock();

    ASSERT(m_valid);

    int palcount = PalCount();
    UINT fuColorUse = DIB_RGB_COLORS;
    DWORD caps = GetDeviceCaps(hdc, RASTERCAPS);

    if (!(caps & RC_STRETCHDIB)) {
	LPTSTR message = TEXT("Device doesn't support StretchDIBits");
	TextOut(hdc, 10, 10, message, lstrlen(message));
	Unlock();
	return;
    }

    if (caps & RC_PALETTE) {
	// palette device
	if ((palcount != 0) && m_palette) {
	    // bitmap has palette, so use it
	    fuColorUse = DIB_PAL_COLORS;
	}
    }
    
    if (fuColorUse == DIB_PAL_COLORS) {
	// device and bitmap both use a palette
	// create a special bitmap header with palette indices
	struct bmi_s {
	    BITMAPINFOHEADER h;
	    WORD pal_index[256];
	} bmi;
	HPALETTE old_palette;
	int i;

	bmi.h.biSize = sizeof(bmi.h);
	bmi.h.biWidth = m_bmp.bmp2.biWidth;
	bmi.h.biHeight = m_bmp.bmp2.biHeight;
	bmi.h.biPlanes = 1;
	bmi.h.biBitCount = m_bmp.bmp2.biBitCount;
	bmi.h.biCompression = 0;
	bmi.h.biSizeImage = 0;      /* default */
	bmi.h.biXPelsPerMeter = 0;  /* default */
	bmi.h.biYPelsPerMeter = 0;  /* default */
	bmi.h.biClrUsed = palcount;
	bmi.h.biClrImportant = palcount;
	for (i = 0; i < palcount; i++)
		bmi.pal_index[i] = (WORD)i;

	old_palette = SelectPalette(hdc, m_palette, FALSE);
	RealizePalette(hdc);
	
	SetStretchBltMode(hdc, COLORONCOLOR);
	StretchDIBits(hdc, xOffset, yOffset, 
	      m_bmp.bmp2.biWidth, m_bmp.bmp2.biHeight,
	      0, 0, m_bmp.bmp2.biWidth, m_bmp.bmp2.biHeight, 
	      m_bits, (BITMAPINFO *)&bmi, fuColorUse, SRCCOPY);

	SelectPalette(hdc, old_palette, FALSE);
    }
    else {
	// either device doesn't use palettes
	// or bitmap does use a colour table
	SetStretchBltMode(hdc, COLORONCOLOR);
// may need to modify target dimensions to allow print preview to work.
// At present, preview should work if bitmap is at printer resolution.
	StretchDIBits(hdc, xOffset, yOffset, 
	      m_bmp.bmp2.biWidth, m_bmp.bmp2.biHeight,
	      0, 0, m_bmp.bmp2.biWidth, m_bmp.bmp2.biHeight, 
	      m_bits, (BITMAPINFO *)&m_bmp, fuColorUse, SRCCOPY);
    }
    Unlock();
}

#ifdef NOTUSED
#ifdef _DEBUG
void CDIB::Dump(CDumpContext& dc) const
{
	dc << "DIB:\n";
	dc << " HeaderSize " << m_bmp.bmp2.biSize << "\n";
	dc << " Width " << m_bmp.bmp2.biWidth << "\n";
	dc << " Height " << m_bmp.bmp2.biHeight << "\n";
	dc << " Planes " << m_bmp.bmp2.biPlanes << "\n";
	dc << " BitCount " << m_bmp.bmp2.biBitCount << "\n";
	dc << " SizeImage " << m_bmp.bmp2.biSizeImage << "\n";
	dc << " XDPI " << m_bmp.bmp2.biXPelsPerMeter << "\n";
	dc << " YDPI " << m_bmp.bmp2.biYPelsPerMeter << "\n";
	dc << " ClrUsed " << m_bmp.bmp2.biClrUsed << "\n";
	dc << " ClrImportant " << m_bmp.bmp2.biClrImportant << "\n";
}
#endif
#endif

void
CDIB::DrawDump(HDC hdc)
{
    TEXTMETRIC tm;
    LOGFONT lf;
    HFONT font;
    int cyHeight;
    int cxLeftMargin;
    int x, y;
    TCHAR str[256];

    Lock();

    memset(&lf, 0, sizeof(lf));
    lf.lfHeight = 8;
    lstrcpy(lf.lfFaceName, TEXT("MS Sans Serif"));
    font = CreateFontIndirect(&lf);
    SelectObject(hdc, font);
    GetTextMetrics(hdc, &tm);
    cyHeight = tm.tmHeight + tm.tmExternalLeading;
    cxLeftMargin = tm.tmAveCharWidth * 2;
    x = cxLeftMargin;
    y = cyHeight;
    wsprintf(str, 
	TEXT("HeaderSize=%d   Width=%d   Height=%d   Planes=%d   BitCount=%d"),
	m_bmp.bmp2.biSize, m_bmp.bmp2.biWidth, m_bmp.bmp2.biHeight,
	(int)m_bmp.bmp2.biPlanes, (int)m_bmp.bmp2.biBitCount);
    TextOut(hdc,x, y, str, lstrlen(str));
    y+= cyHeight;
    wsprintf(str, 
        TEXT("SizeImage=%d   XDPI=%d   YDPI=%d   ClrUsed=%d   ClrImportant=%d"),
	m_bmp.bmp2.biSizeImage, 
	m_bmp.bmp2.biXPelsPerMeter, m_bmp.bmp2.biYPelsPerMeter,
	m_bmp.bmp2.biClrUsed, m_bmp.bmp2.biClrImportant);
    TextOut(hdc, x, y, str, lstrlen(str));
    Unlock();
}


BOOL
CDIB::MakePalette(void)
{
    // Make a palette if needed
    int i;
    int pcount = PalCount();
    LPLOGPALETTE logpalette;
    LPBYTE p = m_colors;
    if (m_palette) {
	DeletePalette(m_palette);
	m_palette = NULL;
    }

    if (pcount == 0)
	return TRUE;

    logpalette = new LOGPALETTE[pcount];
    logpalette->palVersion = 0x300;
    logpalette->palNumEntries = (WORD)pcount;
    for (i = 0; i < pcount; i++) {
	m_bmp.rgb4[i].rgbReserved = logpalette->palPalEntry[i].peFlags = 0;
	m_bmp.rgb4[i].rgbBlue  = logpalette->palPalEntry[i].peBlue  = *p++;
	m_bmp.rgb4[i].rgbGreen = logpalette->palPalEntry[i].peGreen = *p++;
	m_bmp.rgb4[i].rgbRed   = logpalette->palPalEntry[i].peRed   = *p++;
	if (!m_old)
	    p++;
    }
    
    m_palette = CreatePalette(logpalette);
    delete [] logpalette;
    return (m_palette != NULL);
}

void CDIB::CleanupPalette(void)
{
    if (m_palette) {
	DeletePalette(m_palette);
	m_palette = NULL;
	m_valid = FALSE;
    }
}

void CDIB::CleanupBitmap(void)
{
    if (m_bitmap) {
	HGLOBAL hglobal = GlobalHandle(m_bitmap);
	GlobalUnlock(hglobal);
	GlobalFree(hglobal);
	m_bitmap = NULL;
	m_valid = FALSE;
    }
}

void CDIB::CleanupAll(void)
{
    m_valid = FALSE;
    CleanupPalette();
    CleanupBitmap();
}


void CDIB::Release(void)
{
    Lock();
    CleanupAll();
    Unlock();
}

void CDIB::Init(void)
{
    Lock();
    CleanupAll();
    Unlock();
}


BOOL CDIB::Init(HGLOBAL hglobal)
{
    Lock();
    CleanupAll();
    // We are now responsible for freeing this memory
    m_bitmap = (LPBYTE)GlobalLock(hglobal);
    BOOL flag = Init(m_bitmap);
    Unlock();
    return flag;
}

BOOL 
CDIB::Init(LPBYTE pbmp, LPBYTE pbits)
{
    Lock();
    BOOL flag = Init(pbmp);
    m_bits =  pbits;
    Unlock();
    return flag;
}

BOOL 
CDIB::Init(LPBYTE pbmp)
{
    Lock();
    
    CleanupPalette();

    m_little_endian = (*((char *)(&dsc_arch))) == 1;
    m_bmp.bmp2.biSize = GetDword(pbmp);
    pbmp += 4;
    if (m_bmp.bmp2.biSize > 12) {
	// new bitmap
	m_old = FALSE;
	m_bmp.bmp2.biWidth = GetDword(pbmp);
        pbmp += 4;
	m_bmp.bmp2.biHeight = GetDword(pbmp);
        pbmp += 4;
	m_bmp.bmp2.biPlanes = GetWord(pbmp);
        pbmp += 2;
	m_bmp.bmp2.biBitCount = GetWord(pbmp);
        pbmp += 2;
	m_bmp.bmp2.biCompression = GetDword(pbmp);
        pbmp += 4;
	m_bmp.bmp2.biSizeImage = GetDword(pbmp);
        pbmp += 4;
	m_bmp.bmp2.biXPelsPerMeter = GetDword(pbmp);
        pbmp += 4;
	m_bmp.bmp2.biYPelsPerMeter = GetDword(pbmp);
        pbmp += 4;
	m_bmp.bmp2.biClrUsed = GetDword(pbmp);
        pbmp += 4;
	m_bmp.bmp2.biClrImportant = GetDword(pbmp);
        pbmp += 4;
	m_bytewidth = (( m_bmp.bmp2.biWidth * m_bmp.bmp2.biBitCount + 31) 
			& ~31) >> 3;
        m_colors = pbmp;
	pbmp += PalLength();
	m_bits =  pbmp;
    }
    else {
	// old bitmap
	m_old = TRUE;
	m_bmp.bmp2.biWidth = GetWord(pbmp);
        pbmp += 2;
	m_bmp.bmp2.biHeight = GetWord(pbmp);
        pbmp += 2;
	m_bmp.bmp2.biPlanes = GetWord(pbmp);
        pbmp += 2;
	m_bmp.bmp2.biBitCount = GetWord(pbmp);
        pbmp += 2;
	m_bmp.bmp2.biCompression = 0;
	m_bmp.bmp2.biSizeImage = 0;
	m_bmp.bmp2.biXPelsPerMeter = 72;
	m_bmp.bmp2.biYPelsPerMeter = 72;
	m_bmp.bmp2.biClrUsed = 0;
	m_bmp.bmp2.biClrImportant = 0;
	m_bytewidth = (( m_bmp.bmp2.biWidth * m_bmp.bmp2.biBitCount + 31) 
	 		   & ~31) >> 3;
        m_colors = pbmp;
	pbmp += PalLength();
	m_bits =  pbmp;
    }
    m_valid = MakePalette();

    Unlock();
    return m_valid;
}


/* number of colours in colour table */
UINT
CDIB::PalCount(void)
{
    if (m_old) {
	// old bitmap
	if (m_bmp.bmp2.biBitCount == 24)
		return 0;
	return 1 << (m_bmp.bmp2.biBitCount * m_bmp.bmp2.biPlanes);
    }

    // new bitmap
    switch (m_bmp.bmp2.biBitCount) {
	case 16:
	case 24:
	case 32:
	    return 0;
	case 1:
	case 4:
	case 8:
	    return (m_bmp.bmp2.biClrUsed > 0 ? (UINT)m_bmp.bmp2.biClrUsed :
		1 << (m_bmp.bmp2.biBitCount * m_bmp.bmp2.biPlanes));
    }
    // unsupported bitmap format
    return 0;	// should throw an exception
}


// length of colour table in bytes
UINT 
CDIB::PalLength(void)
{
    if ((m_bmp.bmp2.biBitCount == 16) || (m_bmp.bmp2.biBitCount == 32)) {
	if (m_bmp.bmp2.biCompression == BI_BITFIELDS)
	    return 12;	// 3 * DWORD
	return 0;	/* default bit fields */
    }
    return PalCount() * (m_old ? 3 : 4);
}


// pointer to bitmap bits
LPBYTE 
CDIB::GetBits(void)
{
    ASSERT(m_valid);
    if (!m_valid)
	return 0;
    return m_bits;
}

// width of bitmap row in bytes
UINT 
CDIB::ByteWidth(void)
{
    ASSERT(m_valid);
    if (!m_valid)
	return 0;
    return m_bytewidth;
}

// width of bitmap in bits
int 
CDIB::Width(void)
{
    ASSERT(m_valid);
    if (!m_valid)
	return 0;
    return m_bmp.bmp2.biWidth;
}

// height of bitmap in lines
int 
CDIB::Height(void)
{
    ASSERT(m_valid);
    if (!m_valid)
	return 0;
    return m_bmp.bmp2.biHeight;
}

// get size of bitmap
void 
CDIB::GetRect(LPRECT prect)
{
    ASSERT(m_valid);
    if (prect == (LPRECT)NULL)
	   return;
    prect->top = 0;
    prect->left = 0;
    if (m_valid) {
	prect->bottom = Height();
	prect->right = Width();
    }
    else {
	prect->bottom = 0;
	prect->right = 0;
    }
}

BOOL 
CDIB::Read(LPCSTR lpszFileName)
{
    GFile *gf;
    if ( (gf = gfile_open(lpszFileName, gfile_modeRead | gfile_shareDenyWrite))
	== NULL )
	return FALSE;
    BOOL flag = Read(gf);
    gfile_close(gf);
    return flag;
}

// read bitmap to memory
BOOL 
CDIB::Read(GFile *pFile)
{
    BITMAPFILE bmf;
    BYTE bh[SIZEOF_BITMAPFILE];
    LPBYTE p;
    DWORD length;
    DWORD dwRead;

    Lock();
    CleanupBitmap();

    m_valid = FALSE;
    // MFC version of CFile would throw exceptions
    // TRY {
	if (gfile_read(pFile, &bh, SIZEOF_BITMAPFILE) != SIZEOF_BITMAPFILE) {
		Unlock();
		return FALSE;
	}
	p = bh;
	bmf.bfType = GetWord(p);
	p+=2;
	if (bmf.bfType != (('M'<<8) | 'B')) {
		Unlock();
		return FALSE;	// not a DIB bitmap
	}
	bmf.bfSize = GetDword(p);
	p+=4;
	bmf.bfReserved1 = GetWord(p);
	p+=2;
	bmf.bfReserved2 = GetWord(p);
	p+=2;
	bmf.bfOffBits = GetDword(p);
	length = bmf.bfSize - SIZEOF_BITMAPFILE;
	ASSERT(length < 64000000);
	
	// We are now responsible for freeing this memory
	m_bitmap = (LPBYTE)GlobalLock(GlobalAlloc(GHND, length));
	ASSERT(m_bitmap != NULL);
	
	p = m_bitmap;
	while (length && (dwRead = gfile_read(pFile, p, length)) != 0) {
	    length -= dwRead;
	    p += dwRead;
	}

//    } CATCH(CFileException, pE) {
//	if (pE->m_lOsError == ERROR_BROKEN_PIPE)
//	    return FALSE;  // Someone probably closed the other end of the pipe
//	pE->ReportError();
//    }
//    END_CATCH
    BOOL flag = Init(m_bitmap);
    Unlock();
    return flag;
}

// read bitmap header and palette to memory, but not bits
BOOL 
CDIB::ReadHeader(GFile *pFile)
{
    BITMAPFILE bmf;
    BYTE bh[SIZEOF_BITMAPFILE];
    LPBYTE p;
    DWORD length;
    DWORD dwRead;
    HGLOBAL hglobal;

    Lock();
    CleanupBitmap();

    m_valid = FALSE;
    // MFC version of CFile would throw exceptions
    // TRY {
	if (gfile_read(pFile, &bh, SIZEOF_BITMAPFILE) != SIZEOF_BITMAPFILE) {
	    Unlock();
	    return FALSE;
	}
	p = bh;
	bmf.bfType = GetWord(p);
	p+=2;
	if (bmf.bfType != (('M'<<8) | 'B')) {
	    Unlock();
	    return FALSE;	// not a DIB bitmap
	}
	bmf.bfSize = GetDword(p);
	p+=4;
	bmf.bfReserved1 = GetWord(p);
	p+=2;
	bmf.bfReserved2 = GetWord(p);
	p+=2;
	bmf.bfOffBits = GetDword(p);
//		ASSERT(bmf.bfSize - SIZEOF_BITMAPFILE < 64000000);
	
	// instead of loading the entire bitmap, only read the header
	ASSERT(bmf.bfOffBits != 0);
	length = bmf.bfOffBits - SIZEOF_BITMAPFILE;
	ASSERT(length >= 40);

	// make sure we have enough space to add a palette,
	// in case we want to change the bits per pixel of
	// the bitmap.
	// We are now responsible for freeing this memory
	hglobal = GlobalAlloc(GHND, SIZEOF_BITMAP2INFO + 64);
	m_bitmap = (LPBYTE)GlobalLock(hglobal);
	ASSERT(m_bitmap != NULL);
	
	p = m_bitmap;
	while (length && (dwRead = gfile_read(pFile, p, length)) != 0) {
		length -= dwRead;
		p += dwRead;
	}
//    } CATCH(CFileException, pE) {
//	if (pE->m_lOsError == ERROR_BROKEN_PIPE)
//	    return FALSE;  // Someone probably closed the other end of the pipe
//	pE->ReportError();
//    }
//    END_CATCH
    BOOL flag = Init(m_bitmap, NULL);
    Unlock();
    return flag;
}



// ============================




BOOL CDIB::Valid()
{
    return m_valid;
}

// return string containing information about the bitmap
LPTSTR CDIB::DIBinfo()
{

    // create a text message, suitable for printing
    // or use in a dialog box MLE.
#define BUFSIZE 16384
    Lock();
    LPTSTR str = new TCHAR[BUFSIZE];
    LPTSTR p = str;
    int i;
    ASSERT(str != NULL);
    if (str == NULL)
	return NULL;
    wsprintf(p, TEXT("BMP info\r\n"));
    p += lstrlen(p);
    if (!m_valid) {
	wsprintf(p, TEXT(" Bitmap not valid\r\n"));
	Unlock();
	return str;
    }
    wsprintf(p, TEXT(" biSize=%d\r\n biWidth=%d\r\n biHeight=%d\r\n biPlanes=%d\r\n biBitCount=%d\r\n"),
	m_bmp.bmp2.biSize, m_bmp.bmp2.biWidth, m_bmp.bmp2.biHeight,
	(int)m_bmp.bmp2.biPlanes, (int)m_bmp.bmp2.biBitCount);
    p += lstrlen(p);
    wsprintf(p, TEXT(" biSizeImage=%d\r\n biXPelsPerMeter=%d\r\n biYPelsPerMeter=%d\r\n"),
	m_bmp.bmp2.biSizeImage, m_bmp.bmp2.biXPelsPerMeter, m_bmp.bmp2.biYPelsPerMeter);
    p += lstrlen(p);
    wsprintf(p, TEXT(" biClrUsed=%d\r\n biClrImportant=%d\r\n"),
	m_bmp.bmp2.biClrUsed, m_bmp.bmp2.biClrImportant);
    p += lstrlen(p);
    int palcount = PalCount();
    wsprintf(p, TEXT("palcount=%d\r\npallength=%d\r\nm_bytewidth=%d\r\n"),
	palcount, PalLength(), m_bytewidth);
    p += lstrlen(p);

    if (palcount) {
	LPBYTE pp = m_colors;
	int r, g, b;
	for (i = 0; i < palcount; i++) {
	    r = *pp++;
	    g = *pp++;
	    b = *pp++;
	    if (!m_old)
		    pp++;
	    wsprintf(p, TEXT("Color %d: %d %d %d\r\n"), i, r, g, b);
	    p += lstrlen(p);
	}
    }

    if ( ((m_bmp.bmp2.biBitCount == 16) || (m_bmp.bmp2.biBitCount == 32)) 
	&& (m_bmp.bmp2.biCompression == BI_BITFIELDS) ) {
	LPDWORD pd = (LPDWORD)m_colors;
	for (i = 0; i < 3; i++) {
		wsprintf(p, TEXT("Bitfield %d: 0x%04x\r\n"), *pd++);
		p += lstrlen(p);
	}
    }
    
    wsprintf(p, TEXT("End of BMP info\r\n"));

    ASSERT((p+lstrlen(p)-str) < BUFSIZE);
    
    Unlock();
    return str;
#undef BUFSIZE
}


BOOL CDIB::Lock(void)
{
//    return m_critical_section.Lock();
    return TRUE;
}

BOOL CDIB::Unlock(void)
{
//    return m_critical_section.Unlock();
    return TRUE;
}

// Copy bytes into a buffer, advancing buffer pointer
void CDIB::PutDWORD(LPBYTE &d, DWORD dw)
{
    PutWORD(d, (WORD)dw);
    PutWORD(d, (WORD)(dw >> 16));
}

void CDIB::PutWORD(LPBYTE &d, WORD w)
{
    PutBYTE(d, (BYTE)(w));
    PutBYTE(d, (BYTE)(w >> 8));
}

void CDIB::PutBYTE(LPBYTE &d, BYTE b)
{
    *d++ = b;
}


BOOL CDIB::NewHeader(LPBYTE pHeader, UINT *pLen)
{
    if (!Valid())
	return FALSE;

    UINT len = SIZEOF_BITMAPFILE + SIZEOF_BITMAP2 + PalLength();
    if (pLen == NULL)
	return FALSE;
    if ( (pHeader == NULL) || (*pLen < len) ) {
	*pLen = len;
	return FALSE;
    }
    if (m_bmp.bmp2.biSizeImage == 0)
	m_bmp.bmp2.biSizeImage = m_bytewidth * m_bmp.bmp2.biHeight;

    LPBYTE p = pHeader;
    // Create BITMAPFILEHEADER
    PutBYTE(p, 'B');				// bfType
    PutBYTE(p, 'M');
    PutDWORD(p, len + m_bmp.bmp2.biSizeImage);	// bfSize
    PutWORD(p, 0);				// bfReserved1
    PutWORD(p, 0);				// bfReserved2
    PutDWORD(p, len);				// bfOffsetBits

    // Create BITMAPINFOHEADER
    PutDWORD(p, m_bmp.bmp2.biSize);
    PutDWORD(p, (DWORD)m_bmp.bmp2.biWidth);
    PutDWORD(p, (DWORD)m_bmp.bmp2.biHeight);
    PutWORD(p, m_bmp.bmp2.biPlanes);
    PutWORD(p, m_bmp.bmp2.biBitCount);
    PutDWORD(p, m_bmp.bmp2.biCompression);
    PutDWORD(p, m_bmp.bmp2.biSizeImage);
    PutDWORD(p, (DWORD)m_bmp.bmp2.biXPelsPerMeter);
    PutDWORD(p, (DWORD)m_bmp.bmp2.biYPelsPerMeter);
    PutDWORD(p, m_bmp.bmp2.biClrUsed);
    PutDWORD(p, m_bmp.bmp2.biClrImportant);

    // Create palette
    switch (m_bmp.bmp2.biBitCount) {
	case 1:
	case 4:
	case 8:
	    {
	    // Two methods we don't use are:
	    //  pColours: may be RGB triples from an old BITMAP1
	    //  m_bmp.bmiColors: may be palette indices, not colour values
	    ASSERT(m_bmp.bmp2.biCompression == BI_RGB);
	    int palcount = PalCount();
	    LPPALETTEENTRY lpe = new PALETTEENTRY[palcount];
	    GetPaletteEntries(m_palette, 0, palcount, lpe);
	    for (int i=0; i<palcount; i++) {
		    // Write RGB quads
		    PutBYTE(p, lpe[i].peBlue);
		    PutBYTE(p, lpe[i].peGreen);
		    PutBYTE(p, lpe[i].peRed);
		    PutBYTE(p, 0);
	    }
	    delete [] lpe;
	    break;
	    }
	case 24:
	    // no palette
	    ASSERT(m_bmp.bmp2.biCompression == BI_RGB);
	    break;
	case 16:
	case 32:
	    if (m_bmp.bmp2.biCompression == BI_RGB) {
		// no palette
	    }
	    else if (m_bmp.bmp2.biCompression == BI_BITFIELDS) {
		LPDWORD pdw = (LPDWORD)(&m_bmp.rgb4[0]);
		PutDWORD(p, pdw[0]);
		PutDWORD(p, pdw[1]);
		PutDWORD(p, pdw[2]);
	    }
	    else {
		ASSERT(FALSE);
	    }
	    break;
    }

    // make sure we calculated things correctly
    ASSERT( (UINT)(p-pHeader) == len);
    *pLen = len;
    return TRUE;
}


BOOL CDIB::Write(GFile *gf) {
    UINT len = 0;
    NewHeader(NULL, &len);	// get header size
    LPBYTE p = new BYTE[len];
    ASSERT(p != NULL);
    if (!NewHeader(p, &len)) {
	    delete p;
	    return FALSE;
    }
    // Write the headers and palette
    gfile_write(gf, p, len);
    delete p;

    // Write the bits
    ASSERT(m_bits != NULL);
    gfile_write(gf, m_bits, m_bmp.bmp2.biSizeImage);
    return TRUE;
}


BOOL CDIB::Write(LPCSTR lpszFileName)
{
    // not implemented
    GFile *gf;
    if ( (gf = gfile_open(lpszFileName, gfile_modeCreate | 
	gfile_modeWrite | gfile_shareExclusive)) == NULL ) {
	ASSERT(FALSE);
	return FALSE;
    }
    BOOL flag = Write(gf);
    gfile_close(gf);
    return flag;
}


// Return a bitmap suitable for placing on the clipboard 
HGLOBAL CDIB::MakeGlobalDIB()
{
    // Allocate memory for header and bits
    UINT len = 0;
    if (!NewHeader(NULL, &len)) {		// get header size
	ASSERT(FALSE);
	return NULL;
    }
    HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,
	len + m_bmp.bmp2.biSizeImage);
    ASSERT(hglobal);
    if (hglobal == NULL)
	return NULL;
    LPBYTE p = (LPBYTE)GlobalLock(hglobal);
    ASSERT(p);
    if (p == NULL) {
	GlobalFree(hglobal);
	return NULL;
    }

    // Copy the header and palette
    if (!NewHeader(p, &len)) {
	GlobalUnlock(hglobal);
	GlobalFree(hglobal);
	ASSERT(FALSE);
	return NULL;
    }

    // Copy the bits
    CopyMemory(p + len, m_bits, m_bmp.bmp2.biSizeImage);

    GlobalUnlock(hglobal);
    return hglobal;
}
