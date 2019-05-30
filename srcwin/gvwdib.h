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

// DIB.h: interface for the CDIB class.


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/* Because Windows and PM define the same bitmap structures
** with different names we use our own common structures.
** This also allows us to reuse the code on Unix systems.
** BITMAP1 = Windows BITMAPCORE and PM BITMAPINFO
** BITMAP2 = Windows BITMAPINFO and PM BITMAPINFO2
** RGB3 = Windows RGBTRIPLE and PM RGB
** RGB4 = Windows RGBQUAD and PM RGB2
*/
#if defined(__EMX__) || defined(_MSC_VER)
#pragma pack(1)		/* align structures to byte boundaries */
#endif

#ifndef BITMAP1AND2
#define BITMAP1AND2

typedef struct tagRGB3
{
    BYTE    rgbtBlue;
    BYTE    rgbtGreen;
    BYTE    rgbtRed;
} RGB3;
typedef RGB3 * LPRGB3;

typedef struct tagRGB4
{
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGB4;
typedef RGB4 * LPRGB4;
#define SIZEOF_RGB4 4

typedef struct tagBITMAP1
{
    DWORD   bcSize;
    short   bcWidth;
    short   bcHeight;
    WORD    bcPlanes;
    WORD    bcBitCount;
} BITMAP1;
typedef BITMAP1 * LPBITMAP1;

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
#define SIZEOF_BITMAP2 40

typedef struct tagBITMAP2INFO
{
	BITMAP2 bmp2;
	RGB4 rgb4[256];
} BITMAP2INFO;
typedef BITMAP2INFO * LPBITMAP2INFO;
#define SIZEOF_BITMAP2INFO (SIZEOF_BITMAP2 + SIZEOF_RGB4 * 256)

typedef struct tagBITMAPFILE
{
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILE;
typedef BITMAPFILE * LPBITMAPFILE;
#define SIZEOF_BITMAPFILE 14

#endif /*  BITMAP1AND2 */


//////////////////////////////////////////////////////////////////////

class CDIB  
{
public:
	CDIB();
	virtual ~CDIB();

	BOOL Valid(void);		// True if we have a valid bitmap

	LPTSTR DIBinfo(void);		// Get text description of DIB header
	void Init(void);
	BOOL Init(HGLOBAL hglobal);	// read a bitmap in global memory
	void Release();

	BOOL Read(GFile *pFile);	// read a bitmap from file
//	BOOL Read(UINT resource);	// read a bitmap from resources
	BOOL Read(LPCSTR lpszFilename);
	BOOL ReadHeader(GFile *pFile);	// read bitmap header and palette

	BOOL Write(LPCSTR lpszFilename);
	BOOL Write(GFile *cf);
	HGLOBAL MakeGlobalDIB(void);

#ifdef NOTUSED
#ifdef _DEBUG
	void Dump(CDumpContext& dc);
#endif
#endif

//	void DrawDump(CDC *pDC);	// draw BMP details
//	void Draw(CDC *pDC);
//	void Draw(CDC *pDC, int xOffset, int yOffset);

	void DrawDump(HDC hdc);		// draw BMP details
	void Draw(HDC hdc);		// draw bitmap
	void Draw(HDC hdc, int xOffset, int yOffset);
	int Width(void);		// width of bitmap in bits
	int Height(void);		// height of bitmap in lines
	void GetRect(LPRECT prect);	// get size of bitmap

	BOOL Lock(void);	// Stop another thread from changing this object
	BOOL Unlock(void);	//

//private:
	void CleanupPalette(void);
	void CleanupAll(void);
	void CleanupBitmap(void);
	BOOL Init(LPBYTE pbmp, LPBYTE pbits);
	BOOL Init(LPBYTE pbmp);
	UINT PalCount(void);	// number of entries in palette
	UINT PalLength(void);	// byte length of palette
	UINT ByteWidth(void);	// width of bitmap row in bytes
	LPBYTE GetBits(void);	// pointer to bitmap bits
	WORD GetWord(LPBYTE pbyte);
	DWORD GetDword(LPBYTE pbyte);
	BOOL MakePalette(void);

	void PutDWORD(LPBYTE &d, DWORD dw);
	void PutWORD(LPBYTE &d, WORD w);
	void PutBYTE(LPBYTE &d, BYTE b);
	BOOL NewHeader(LPBYTE pHeader, UINT *pLen);


	LPBYTE m_bitmap;	// bitmap memory allocated with GlobalAlloc
	LPBYTE m_colors;	// pointer to palette table in DIB
	LPBYTE m_bits;		// pointer to bits in DIB
	BITMAP2INFO m_bmp;	// local storage of bitmap info 
	UINT m_bytewidth;

	BOOL m_little_endian;	// True if little endian architecture
	BOOL m_old;		// True if old (used by OS/2) bitmap
	BOOL m_valid;
//	CPalette *m_palette;
	HPALETTE m_palette;
//	CCriticalSection m_critical_section;	
};

#ifndef ASSERT
#define ASSERT(f)
#endif


