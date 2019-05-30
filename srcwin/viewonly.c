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

#include "gvc.h"
/* viewonly.cpp */


HINSTANCE hDllInstance;

/* DLL entry point for Borland C++ */
__declspec(dllexport) BOOL WINAPI
DllEntryPoint(HINSTANCE hInst, DWORD fdwReason, LPVOID lpReserved)
{
    hDllInstance = hInst;
    return TRUE;
}

/* DLL entry point for Microsoft Visual C++ */
__declspec(dllexport) BOOL WINAPI
DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpReserved)
{
    return DllEntryPoint(hInst, fdwReason, lpReserved);
}


/* Exported API */
__declspec(dllexport) BOOL WINAPI
gsview_display_memory(const char *buffer, int len)
{
    psfile.file.SetMemory(buffer, len);
    return gsview_main(hDllInstance, "gsview dummy");
}

