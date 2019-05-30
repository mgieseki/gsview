/* Copyright (C) 2007, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwport.c */
/* Launcher for GSview as a portable application */

/* The addition of the /a command line option causes GSview
 * to locate ghostscript in an adjacent directory, to use
 * an INI file in the GSview directory, and to avoid writing
 * to the registry.
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int PASCAL 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int cmdShow)
{
    int i;
    char szExePath[1024]; 
    LPSTR pcmdline = GetCommandLineA();
    LPSTR pargs;
    LPSTR command = NULL;

    /* get path to EXE */
    GetModuleFileNameA(hInstance, szExePath, sizeof(szExePath));
    for (i=lstrlen(szExePath)-1; i>0; i--)
	if (szExePath[i] == '\\') {
	    szExePath[i+1] = '\0';
	    break;
    }

    /* find args in old command line */
    if (pcmdline[0] == '\042') {
	pargs=pcmdline+1;
	while (*pargs && (*pargs != '\042'))
	    pargs++;
	if (*pargs)
	  pargs++;
    }
    else {
	pargs=pcmdline;
	while (*pargs && (*pargs != ' '))
	    pargs++;
	if (*pargs)
	  pargs++;
    }

    /* Create a new command line and run GSview */
    command = malloc(strlen(szExePath) + strlen(pcmdline) + 1 + 22 + 5);
    if (command != NULL) {
	strcpy(command, "\042");
        strcat(command, szExePath);
        strcat(command, "gsview\\gsview32.exe\042");
	strcat(command, " /a ");
	strcat(command, pargs);
	WinExec(command, SW_SHOWNORMAL);
    }

    return 0;
}
