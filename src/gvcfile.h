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

/* gvcfile.h */

/* GFile is similar but to MFC CFile but is implemented as C, not C++. */

#if defined(STDIO) || defined(MEMORYFILE) || !defined(_Windows) || defined(OS2)
#ifndef UINT
#define UINT unsigned int
#endif
#ifndef BOOL
#define BOOL int
#endif
#ifndef LONG
#define LONG long
#endif
#ifndef DWORD
#define DWORD unsigned long
#endif
#ifndef LPCTSTR
#define LPCTSTR const char *
#endif
#ifndef LPCSTR
#define LPCSTR const char *
#endif
#ifndef GENERIC_READ
#define GENERIC_READ (0x80000000L)
#endif
#ifndef FILE_SHARE_READ
#define FILE_SHARE_READ 0x00000001
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE (!FALSE)
#endif
#endif


typedef struct GFile_s GFile;

/* for gfile_open nOpenFlags */
enum OpenFlags {gfile_modeRead = 0x0000, gfile_modeWrite = 0x0001,
    gfile_shareExclusive=0x0010, gfile_shareDenyWrite=0x0020, 
    gfile_modeCreate=0x1000};

/* for gfile_seek nFrom */
enum {gfile_begin, gfile_current, gfile_end};

GFile *gfile_open_handle(int hFile);
GFile *gfile_open(LPCSTR lpszFileName, UINT nOpenFlags);
void gfile_close(GFile *gf);
UINT gfile_read(GFile *gf, void *lpBuf, UINT nCount);
UINT gfile_write(GFile *gf, void *lpBuf, UINT nCount);
LONG gfile_seek(GFile *gf, LONG lOff, UINT nFrom);
LONG gfile_get_position(GFile *gf);
LONG gfile_get_length(GFile *gf);
BOOL gfile_get_datetime(GFile *gf, UINT *pdt_low, UINT *pdt_high);
BOOL gfile_changed(GFile *gf, LONG length, UINT dt_low, UINT dt_high);
void gfile_set_memory(GFile *gf, const char *base, long len);

