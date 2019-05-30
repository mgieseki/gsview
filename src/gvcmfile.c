/* Copyright (C) 2001-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvcmfile.cpp */

/* GFile is similar but not identical to MFC CFile. */
/* This implementation uses a memory buffer, NOT files. */


#define STRICT
#include <windows.h>
#include <stdio.h>
#include "gvcfile.h"

static const char *g_pBase;
static long g_nLen;

/* These are the private bits */
struct GFile_s {
	/* Read from a memory buffer */
	const char *m_pBase;
	long m_nOffset;
	long m_nLen;
};

void gfile_set_memory(const char *base, long len)
{
    g_pBase = base;
    g_nLen = len;
}

LONG gfile_get_length(GFile *gf)
{
    ASSERT(gf != NULL);
    return gf->m_nLen;
}

BOOL gfile_get_datetime(GFile *gf, UINT *pdt_low, UINT *pdt_high)
{
    ASSERT(gf != NULL);
    *pdt_low = 0;
    *pdt_high = 0;
    return TRUE;
}

BOOL gfile_changed(GFile *gf, LONG length, UINT dt_low, UINT dt_high)
{
    UINT this_dt_low, this_dt_high;
    DWORD this_length = gfile_get_length(gf);
    gfile_get_datetime(gf, &this_dt_low, &this_dt_high);
    return ( (this_length != length) ||
	(this_dt_low != dt_low) || (this_dt_high != dt_high));
}

GFile *gfile_open_handle(int hFile)
{
    return NULL;
}

GFile *gfile_open(LPCTSTR lpszFileName, UINT nOpenFlags)
{
    GFile *gf;
    if (g_pBase == NULL)
	return NULL;
    gf = (GFile *)malloc(sizeof(GFile));
    if (gf == NULL)
	return NULL;
    memset(gf, 0, sizeof(GFile));
    m_pBase = g_pBase;
    m_nLen = g_nLen;
    m_nOffset = 0;
    return gf;
}


void gfile_close(GFile *gf)
{
    ASSERT(gf != NULL);
    free(gf);
}


UINT gfile_read(GFile *gf, void *lpBuf, UINT nCount)
{
    ASSERT(gf != NULL);
    ASSERT(gf->m_pBase != NULL);
    int count;
    if (gf->m_pBase == NULL)
	return 0;
    if (gf->m_nLen == 0)
	return 0;
    count = min(nCount, gf->m_nLen - gf->m_nOffset);
    memcpy(lpBuf, gf->m_pBase + gf->m_nOffset, count); 
    gf->m_nOffset += count;
    return count;
}


/* only works with reading */
LONG gfile_seek(GFile *gf, LONG lOff, UINT nFrom)
{
    ASSERT(gf != NULL);
    ASSERT(gf->m_pBase != NULL);
    switch(nFrom) {
	default:
	case begin:
	    gf->m_nOffset = lOff;
	    break;
	case current:
	    gf->m_nOffset += lOff;
	    break;
	case end:
	    gf->m_nOffset = gf->m_nLen + lOff;
	    break;
    }
    if (gf->m_nOffset < 0)
	gf->m_nOffset = 0;
    if (gf->m_nOffset > gf->m_nLen)
	gf->m_nOffset = gf->m_nLen;
    return gf->m_nOffset;
}

LONG gfile_get_position(GFile *gf)
{
    ASSERT(gf != NULL);
    return gf->m_nOffset;
}


