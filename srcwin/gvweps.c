/* Copyright (C) 1993-2001, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvweps.c */
/* EPS file manipulation module for Windows GSview */

#include "gvwin.h"

static HGLOBAL get_bitmap_hglobal;

LPBITMAP2
get_bitmap()
{
    LPBITMAPINFOHEADER lpBitmap;
    get_bitmap_hglobal = image_copy_dib(&image);
    if (get_bitmap_hglobal == (HGLOBAL)NULL) {
	gserror(0, "not enough memory to copy bitmap", 
		MB_ICONEXCLAMATION, SOUND_ERROR);
	return NULL;
    }
    lpBitmap = (LPBITMAPINFOHEADER)GlobalLock(get_bitmap_hglobal);
    return (LPBITMAP2)lpBitmap;
}

void
release_bitmap()
{
	if (get_bitmap_hglobal) {
	    GlobalUnlock(get_bitmap_hglobal);
	    GlobalFree(get_bitmap_hglobal);
	    get_bitmap_hglobal = 0;
	}
}
