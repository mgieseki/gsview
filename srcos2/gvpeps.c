/* Copyright (C) 1993-1998, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvpeps.c */
/* EPS file manipulation module for PM GSview */

#include "gvpm.h"

LPBITMAP2
get_bitmap()
{
	return (LPBITMAP2)bitmap.pbmi;
}

void
release_bitmap()
{
	/* no action for PM */
}

/* Write out a BMP file */
void
paste_to_file(void)
{
	LPBITMAP1 pbm1 = (LPBITMAP1)bitmap.pbmi;
	BITMAPFILEHEADER2 bmfh;
	UINT bmfh_length = sizeof(BITMAPFILEHEADER2) - sizeof(BITMAPINFOHEADER2);
	UINT offset = 0;
	UINT length = 0;	/* bitmap length */
	ULONG fh;		/* file handle */
	ULONG action;
	ULONG count;
	char output[MAXSTR];
	strcpy(output, "*.bmp");
	if (!get_filename(output, TRUE, FILTER_BMP, 0, IDS_TOPICCLIP))
		return;

	bmfh.usType = 0x4d42;	/* "BM" */
        if (bitmap.pbmi->cbFix == sizeof(BITMAP1)) {
	    offset = pbm1->bcSize + sizeof(RGB3) * dib_pal_colors((unsigned char *)bitmap.pbmi);
	    length = offset + ( dib_bytewidth((unsigned char *)bitmap.pbmi) * pbm1->bcHeight );
	}
	else {
	    offset = bitmap.pbmi->cbFix + sizeof(RGB2) * dib_pal_colors((unsigned char *)bitmap.pbmi);
	    length = offset + ( dib_bytewidth((unsigned char *)bitmap.pbmi) * bitmap.pbmi->cy );
	}
	bmfh.cbSize = bmfh_length + length;
	bmfh.xHotspot = bmfh.yHotspot = 0;
	bmfh.offBits = bmfh_length + offset;
	if (DosOpen((PCSZ)output,	/* filename */
		&fh,		/* pointer to handle */
		&action,	/* pointer to result */
		0,		/* initial length */
		FILE_NORMAL,	/* normal file */
		OPEN_ACTION_CREATE_IF_NEW | OPEN_ACTION_REPLACE_IF_EXISTS,
		OPEN_ACCESS_WRITEONLY | OPEN_SHARE_DENYREADWRITE,
		0)) {
		gserror(0, "Error opening BMP file", MB_ICONEXCLAMATION, SOUND_ERROR);
		return;
	}
	if (DosWrite(fh, (PBYTE)&bmfh, bmfh_length, &count))
		gserror(0, "Error writing BMP header", MB_ICONEXCLAMATION, SOUND_ERROR);
	if (DosWrite(fh, (PBYTE)bitmap.pbmi, length, &count))
		gserror(0, "Error writing BMP body", MB_ICONEXCLAMATION, SOUND_ERROR);
	if (DosClose(fh))
		gserror(0, "Error closing BMP file", MB_ICONEXCLAMATION, SOUND_ERROR);
}

void
clip_convert()
{
	not_implemented();
}
