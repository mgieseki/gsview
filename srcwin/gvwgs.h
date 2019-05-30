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

/* gvwgs.h */

#define STRICT
#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include "gvcrc.h"

#define GVWGS_VERSION  "0.3"

#ifndef DS_3DLOOK
#define DS_3DLOOK 0x0004L      /* for Windows 95 look */
#endif

#ifndef RC_INVOKED

#include <dos.h>
#ifdef _MSC_VER
#define GSDLLEXPORT __declspec(dllimport)
#endif
#define P0() void
#define P1(t1) t1
#define P2(t1,t2) t1,t2
#define P3(t1,t2,t3) t1,t2,t3
#define P4(t1,t2,t3,t4) t1,t2,t3,t4
#define P5(t1,t2,t3,t4,t5) t1,t2,t3,t4,t5
#define P6(t1,t2,t3,t4,t5,t6) t1,t2,t3,t4,t5,t6
#include "errors.h"
#include "iapi.h"

#define WM_TEXTUPDATE	WM_USER+1
#define WM_PCUPDATE	WM_USER+2

#define MAXSTR 256

#ifdef _WIN64
#define unlink _unlink
#endif

#endif
