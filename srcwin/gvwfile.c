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

/* gvwfile.cpp */

/* GFile is similar but not identical to MFC CFile. */
/* This implementation uses Windows APIs */

#define STRICT
#include <windows.h>
#include <stdio.h>

#include "gvcfile.h"

/* These are the private bits */
struct GFile_s {
#ifdef MEMORYFILE
	/* Read from a memory buffer */
	const char *m_pBase;
	long m_nOffset;
	long m_nLen;
#else
#ifdef _Windows
	/* Windows API */
	int m_hFile;
#else
	FILE *m_file;
#endif /* !_Windows */
#endif /* !MEMORYFILE */
};

static void gfile_assert(const char *file, int len);

#ifndef ASSERT
#ifdef DEBUG
#define ASSERT(f) if (!(f)) gfile_assert(__FILE__, __LINE__)
#else
#define ASSERT(f)
#endif
#endif


LONG gfile_get_length(GFile *gf)
{
    BY_HANDLE_FILE_INFORMATION fi;
    ASSERT(gf != NULL);
    GetFileInformationByHandle((HANDLE)gf->m_hFile, &fi);
    return fi.nFileSizeLow;
}

BOOL gfile_get_datetime(GFile *gf, UINT *pdt_low, UINT *pdt_high)
{
    FILETIME datetime;
    BOOL flag;
    ASSERT(gf != NULL);
    flag = GetFileTime((HANDLE)gf->m_hFile, NULL, NULL, &datetime);
    *pdt_low = datetime.dwLowDateTime;
    *pdt_high = datetime.dwHighDateTime;
    return flag;
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
    GFile *gf = (GFile *)malloc(sizeof(GFile));
    if (gf == NULL) {
	CloseHandle((HANDLE)hFile);
	return NULL;
    }
    memset(gf, 0, sizeof(GFile));
    gf->m_hFile = hFile;
    return gf;
}

GFile *gfile_open(LPCSTR lpszFileName, UINT nOpenFlags)
{
    GFile *gf;
    DWORD dwAccess = GENERIC_READ;
    DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
    DWORD dwCreate = OPEN_EXISTING;
    HANDLE hFile;
    ASSERT(nOpenFlags == GENERIC_READ);
    if ((nOpenFlags & 0xf) == gfile_modeRead)
	dwAccess = GENERIC_READ;
    if ((nOpenFlags & 0xf) == gfile_modeWrite)
	dwAccess = GENERIC_WRITE;
    if ((nOpenFlags & 0xf0) == gfile_shareDenyWrite)
	dwShareMode = FILE_SHARE_READ;
    if ((nOpenFlags & 0xf0) == gfile_shareExclusive)
	dwShareMode = 0;
    if ((nOpenFlags & 0xf00) == gfile_modeCreate)
	dwCreate = CREATE_ALWAYS;

    hFile = CreateFileA(lpszFileName, dwAccess,
	dwShareMode, NULL, dwCreate, FILE_ATTRIBUTE_NORMAL,
	NULL);
    if (hFile == INVALID_HANDLE_VALUE)
	return NULL;
    gf = (GFile *)malloc(sizeof(GFile));
    if (gf == NULL) {
	CloseHandle(hFile);
	return NULL;
    }
    memset(gf, 0, sizeof(GFile));
    gf->m_hFile = (int)hFile;
    return gf;
}

void gfile_close(GFile *gf)
{
    ASSERT(gf != NULL);
    ASSERT(gf->m_hFile != 0);
    CloseHandle((HANDLE)gf->m_hFile);
    gf->m_hFile = 0;
    free(gf);
}


UINT gfile_read(GFile *gf, void *lpBuf, UINT nCount)
{
    DWORD nBytesRead;
    ASSERT(gf != NULL);
    ASSERT(gf->m_hFile != 0);
    if (ReadFile((HANDLE)gf->m_hFile, lpBuf, nCount, &nBytesRead, NULL))
	return nBytesRead;
    return 0;
}

UINT gfile_write(GFile *gf, void *lpBuf, UINT nCount)
{
    DWORD nBytesWritten;
    ASSERT(gf != NULL);
    ASSERT(gf->m_hFile != 0);
    if (WriteFile((HANDLE)gf->m_hFile, lpBuf, nCount, &nBytesWritten, NULL))
	return nBytesWritten;
    return 0;
}

LONG gfile_seek(GFile *gf, LONG lOff, UINT nFrom)
{
    DWORD dwMoveMethod;
    ASSERT(gf != NULL);
    ASSERT(gf->m_hFile != 0);
    switch(nFrom) {
	default:
	case gfile_begin:
	    dwMoveMethod = FILE_BEGIN;
	    break;
	case gfile_current:
	    dwMoveMethod = FILE_CURRENT;
	    break;
	case gfile_end:
	    dwMoveMethod = FILE_END;
	    break;
    }
    // return value on error is 0xffffffff
    return SetFilePointer((HANDLE)gf->m_hFile, lOff, NULL, dwMoveMethod);
}

LONG gfile_get_position(GFile *gf)
{
    ASSERT(gf != NULL);
    ASSERT(gf->m_hFile != 0);
    return SetFilePointer((HANDLE)gf->m_hFile, 0, NULL, FILE_CURRENT); 
}

static void gfile_assert(const char *file, int line) 
{
  printf("Assert failed in file %s at line %d\n", file, line);
}
