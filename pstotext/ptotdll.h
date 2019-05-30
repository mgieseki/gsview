/* Copyright (C) 1996, Digital Equipment Corporation.         */
/* All rights reserved.                                       */
/* See the file pstotext.txt for a full description.          */
/* Last modified on Fri Jan 09 21:17:00 AEST 2004 by rjl   */
/*      modified on Sun Oct 13 08:46:00 PDT 1996 by mcjones   */
/*      modified on Mon Jul 29 14:29:00 UTC 1996 by rjl       */

/* Interface to ptotdll.c, which is based on OCR_PS.m3, a module of
   the Virtual Paper project at the DEC Systems Research Center:
   http://www.research.digital.com/SRC/virtualpaper/  */

/* Modifications by rjl
 *   Fixed compiler warnings
 */

#ifndef _PTOTDLL_H
#define _PTOTDLL_H
#endif

#ifdef __DLL__
#ifdef _Windows
#include <windows.h>
#ifdef __BORLANDC__
#define DLLEXPORT CALLBACK _export	/* Win32 Borland C++ */
#else
#define DLLEXPORT __declspec(dllexport) CALLBACK /* Win32 MS VC++ */
#endif
#else
#include <os2.h>
#define DLLEXPORT			 /* OS/2 */
#endif
#else
#define DLLEXPORT			/* all others */
#endif

/* DLL exported functions */

int DLLEXPORT pstotextInit(void **instance);
/* Allocate and initialize the instance data, and set *instance to
   the address of that data.  Return zero if successful, or
   PSTOTEXT_INIT_MALLOC if not enough storage could be allocated. */

int DLLEXPORT pstotextSetCork(void *instance, int value);
/* Assume the Cork encoding for dvips-produced fonts if value==1, or
   the old text layout if value==0. (Default is value==0.) */

int DLLEXPORT pstotextFilter(
  /* input parameters: */
  void *instance,
  char *instr,
  /* output parameters:  */
  /* rjl: "char **" made const in 1.9 */
  const char **pre, const char **word, const char **post,
  int *llx, int *lly, int *urx, int *ury
);
/* Process *instr, a null-terminated line of GS ocr.ps output, and
   set the output parameters as follows

     if ( no output available this call ) *word = 0;
     else {
       *pre = "" or " " or "\n";
       *word = null-terminated word (not equal to 0);
       (*llx, *lly, *urx, *ury) = bounding box for *word;
       *post = "" or "\f\n" or "\n\f\n";
     }

  The strings assigned to *pre, *word, and *post are only valid until the next
  call of pstotextFilter or pstotextExit.

  pstotextFilter normally returns 0, but a nonzero result implies an
  error: */

int DLLEXPORT pstotextExit(void *instance);
/* Free the storage pointed to by instance. */

/* return codes from pstotextInit and pstotextFilter */
#define PSTOTEXT_INIT_MALLOC 100
#define PSTOTEXT_FILTER_BADENCODINGNUMBER   200
#define PSTOTEXT_FILTER_TOOMANYGLYPHINDEXES 201
#define PSTOTEXT_FILTER_BADFONTNUMBER       202
#define PSTOTEXT_FILTER_BADMETRICNUMBER     203
#define PSTOTEXT_FILTER_BADQS               204
#define PSTOTEXT_FILTER_BADGLYPHINDEX       205

