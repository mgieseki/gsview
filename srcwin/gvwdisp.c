/* Copyright (C) 1993-1998, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwdisp.c */
/* Display GSview routines for Windows */
#include "gvwin.h"

/* handle messages while we are waiting */
void
do_message(void)
{
    MSG msg;
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
	if ((hDlgModeless == 0) || !IsDialogMessage(hDlgModeless, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}




char *
text_filetime(LPFILETIME pft)
{
   static char buf[MAXSTR];
   FILETIME ft;
   SYSTEMTIME st;
   FileTimeToLocalFileTime(pft, &ft);
   FileTimeToSystemTime(&ft, &st);
   sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d",
	st.wYear, st.wMonth, st.wDay,
	st.wHour, st.wMinute, st.wSecond);
   return buf;
}


BOOL 
exec_pgm(char *name, char *arg, PROG* prog)
{
char command[MAXSTR*2];
	prog->valid = FALSE;
	command[0] = '\0';
	if (!is_win32s)
	    strcat(command, "\042");
	strcat(command, name);
	if (!is_win32s)
	    strcat(command, "\042");
	strcat(command, " ");
	strcat(command, arg);
	prog->hinst = (HINSTANCE)WinExec(command, SW_SHOWMINNOACTIVE);
	if (prog->hinst > (HINSTANCE)31)
	    prog->valid = TRUE;
	return prog->valid;
}

/* stop specified program */
void
stop_pgm(PROG* prog)
{
	if (!prog->valid) {
	    cleanup_pgm(prog);
	    return;
	}
/* should really stop program by sending it a WM_CLOSE */
/* figuring out which window is messy */
	{
	    int i = 0;
	    TerminateProcess(prog->hinst, 1);
	    while (prog->valid && (i < 100)) {
		/* wait for termination message to cause cleanup_pgm() to be called */
		Sleep(100);
		peek_message();
		i++;
	    }
	    if (i >= 100)
		gserror(0, "can't stop program", MB_ICONHAND, SOUND_ERROR);
	}
/* cleanup should already have occurred */
	cleanup_pgm(prog);
}


/* cleanup after program has stopped */
void
cleanup_pgm(PROG* prog)
{
	prog->valid = FALSE;
	prog->hinst = 0;
}

