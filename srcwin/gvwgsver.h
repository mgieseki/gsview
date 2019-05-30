/* Copyright (C) 2001-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this file ("Software"), to deal in the Software without 
  restriction, including without limitation the rights to use, copy, 
  modify, merge, publish, distribute, sublicense, and/or sell copies of 
  this Software, and to permit persons to whom this file is furnished to 
  do so, subject to the following conditions: 

  This Software is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  

  The above copyright notice and this permission notice shall be included
  in all copies or substantial portions of the Software.
*/

/* This file is part of GSview.  The above notice applies only 
 * to this file.  It does NOT apply to any other file in GSview
 * unless that file includes the above copyright notice.
 */


/* gvgsver.h */
/* Functions for finding Ghostscript versions */
BOOL get_gs_versions(int *pver);
BOOL get_gs_string(int gs_revision, const char *name, char *ptr, int len);
BOOL find_gs(char *gspath, int len, int minver, BOOL bDLL);

