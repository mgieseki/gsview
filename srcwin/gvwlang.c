/* Copyright (C) 1996-1998, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwlang.c */
/* put some code in the DLL to keep the linker happy */

#include <windows.h>
#ifdef _MSC_VER
#define _export
#endif

#ifdef __BORLANDC__
#pragma argsused
#endif
BOOL WINAPI _export
DllEntryPoint(HINSTANCE hInst, DWORD fdwReason, LPVOID lpReserved)
{
	return TRUE;
}

