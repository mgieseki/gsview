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

/* gvcfile.cpp */

/* GFile is similar but not identical to MFC CFile, but is plain C. */
/* This implementation uses C file streams */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifndef __WIN32__
#include <unistd.h>
#endif
#include "gvcfile.h"

/* These are the private bits */
struct GFile_s {
	FILE *m_file;
	time_t	m_filetime;	/* time/date of selected file */
	long m_length;		/* length of selected file */
};

#ifndef ASSERT
#ifdef DEBUG
static void gfile_assert(const char *file, int len);
#define ASSERT(f) if (!(f)) gfile_assert(__FILE__, __LINE__)
#else
#define ASSERT(f)
#endif
#endif


LONG gfile_get_length(GFile *gf)
{
    struct stat fstatus;
    fstat(fileno(gf->m_file), &fstatus);
    return fstatus.st_size;
}

BOOL gfile_get_datetime(GFile *gf, UINT *pdt_low, UINT *pdt_high)
{
    struct stat fstatus;
    ASSERT(gf != NULL);
    fstat(fileno(gf->m_file), &fstatus);
    *pdt_low = fstatus.st_mtime;
    *pdt_high = 0;
    return TRUE;
}

BOOL gfile_changed(GFile *gf, LONG length, UINT dt_low, UINT dt_high)
{
    UINT this_dt_low, this_dt_high;
    LONG this_length = gfile_get_length(gf);
    gfile_get_datetime(gf, &this_dt_low, &this_dt_high);
    return ( (this_length != length) ||
	(this_dt_low != dt_low) || (this_dt_high != dt_high));
}

GFile *gfile_open_handle(int hFile)
{
    GFile *gf = (GFile *)malloc(sizeof(GFile));
    if (gf == NULL)
	return NULL;
    memset(gf, 0, sizeof(GFile));
    gf->m_file = fdopen(hFile, "rb");
    if (gf->m_file == NULL) {
	free(gf);
	gf = NULL;
    }
    return gf;
}

GFile *gfile_open(LPCTSTR lpszFileName, UINT nOpenFlags)
{
    GFile *gf;
    FILE *f;
    const char *access = "rb";
    if ((nOpenFlags & 0xf) == gfile_modeWrite)
	access = "wb";
 
    f = fopen(lpszFileName, access);
    if (f == (FILE *)NULL)
	return NULL;

    gf = (GFile *)malloc(sizeof(GFile));
    if (gf == NULL) {
	fclose(f);
	return NULL;
    }
    memset(gf, 0, sizeof(GFile));
    gf->m_file = f;
    return gf;
}

void gfile_close(GFile *gf)
{
    ASSERT(gf != NULL);
    ASSERT(gf->m_file != 0);
    fclose(gf->m_file);
    gf->m_file = NULL;
    free(gf);
}


UINT gfile_read(GFile *gf, void *lpBuf, UINT nCount)
{
    ASSERT(gf != NULL);
    ASSERT(gf->m_file != 0);
    return fread(lpBuf, 1, nCount, gf->m_file);
}

UINT gfile_write(GFile *gf, void *lpBuf, UINT nCount)
{
    ASSERT(gf != NULL);
    ASSERT(gf->m_file != 0);
    return fwrite(lpBuf, 1, nCount, gf->m_file);
}

/* only works with reading */
LONG gfile_seek(GFile *gf, LONG lOff, UINT nFrom)
{
    int origin;
    ASSERT(gf != NULL);
    ASSERT(gf->m_file != 0);

    switch(nFrom) {
	default:
	case gfile_begin:
	    origin = SEEK_SET;
	    break;
	case gfile_current:
	    origin = SEEK_CUR;
	    break;
	case gfile_end:
	    origin = SEEK_END;
	    break;
    }
    if ((origin == SEEK_SET) && (lOff == 0))
	rewind(gf->m_file);
    fseek(gf->m_file, lOff, origin);
    return ftell(gf->m_file);
}

LONG gfile_get_position(GFile *gf)
{
    ASSERT(gf != NULL);
    ASSERT(gf->m_file != 0);
    return ftell(gf->m_file);
}


