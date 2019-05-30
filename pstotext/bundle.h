/* Copyright (C) 1995, Digital Equipment Corporation.         */
/* All rights reserved.                                       */
/* See the file pstotext.txt for a full description.          */
/* Last modified on Fri Jan 09 21:15:00 AEST 2004 by rjl   */
/*      modified on Fri Oct 11 15:35:24 PDT 1996 by mcjones   */

/* rjl: Fix compiler warnings */
typedef const char *BUNDLE[];

extern void putbundle(BUNDLE b, FILE *f);
/* Write bundle "b" to file "f".

   "b" should have been constructed from "b.ps" by the ".ps.h" rule in
   the pstotext Makefile. */
