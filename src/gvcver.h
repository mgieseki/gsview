/* Copyright (C) 1993-2012, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* version strings for GSview and Ghostscript */
/* These strings are used by both code and resources */

/* Version number changes must be made to
 *   src/gvcver.h
 *   src/gvcver.mak
 *   src/wincom.mak (Copyright year)
 *   Readme.htm
 *   FILE_ID.DIZ
 *   src/gvcnews.txt
 *   srcunx/gsview.spec
 *   epstool.htm
 *   language/gvclang.txt
 *   language/gvclang.h    (for Copyright dates)
 */


/* undefine BETA for a final release */
/*
#define BETA
#define BETA_YEAR    2011
#define BETA_MONTH   12
#define BETA_DAY     30
*/


/* GSview */
#include "gsvver.h"

#define GSVIEW_PRODUCT "GSview"

#ifdef _Windows
#define GSVIEW_BASEDIR "gsview"
#define GSVIEW_ZIP     "win32.zip"
#else
#define GSVIEW_ZIP     "os2.zip"
#define GSVIEW_BASEDIR "gsview2"
#endif


/* Ghostscript */
#define GS_PRODUCT  "GPL Ghostscript"
#define GS_REVISION_MIN   704
#define GS_REVISION	  704
#define GS_REVISION_MAX   999

#ifdef _Windows
#ifdef _WIN64
#define GSVIEW_EXENAME "gsview64.exe"
#define GS_EXENAME  "gswin64c.exe"
#define GS_DLLNAME  "gsdll64.dll"
#define INIFILE "gsview64.ini"
#else
#define GSVIEW_EXENAME "gsview32.exe"
#define GS_EXENAME  "gswin32c.exe"
#define GS_DLLNAME  "gsdll32.dll"
#define INIFILE "gsview32.ini"
#endif
#else
#if X11
#define INIFILE "gsview.ini"
#define GS_EXENAME  "gs"
#define GS_DLLNAME  "libgs.so"
#else
#define GS_EXENAME  "gsos2.exe"
#define GS_DLLNAME  "gsdll2.dll"
#define INIFILE "gvpm.ini"
#endif
#endif

/* General */

#define EMX_NEEDED "0.9d"
#define INSTALL_DIR "\\gs"

