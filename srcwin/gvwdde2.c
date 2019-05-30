/* Copyright (C) 2001, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwdde2.c */
/* DDE server and client for XTYP_EXECUTE transactions */
#include "gvwin.h"
#include <ddeml.h>

/* NOT Unicode */

const char szServiceName[] = "GSview";
const char szTopicName[] = "GSview";

void dde_server_execute(HDDEDATA hData);

/* globals, to be accessed from this file only */
DWORD idInst = 0L;
FARPROC lpDdeProc;
HSZ hszService, hszTopic;
BOOL server_enabled = FALSE;

#ifdef __BORLANDC__
#pragma argsused	/* ignore warning for next function */
#endif
HDDEDATA CALLBACK
GSviewDdeCallback(UINT type, UINT fmt, HCONV hconv,
    HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1, DWORD dwData2)
{
char szBuffer[32];
  if (server_enabled) {
      switch (type) {
	case XTYP_CONNECT:
	    DdeQueryStringA(idInst, hsz2, (LPSTR)szBuffer, sizeof(szBuffer), 0);
	    if (strcmp(szBuffer, szServiceName) != 0)
		return FALSE;
	    DdeQueryStringA(idInst, hsz1, (LPSTR)szBuffer, sizeof(szBuffer), 0);
	    if (strcmp(szBuffer, szTopicName) != 0)
		return FALSE;
	    return (HDDEDATA)TRUE;
	case XTYP_EXECUTE:
	    dde_server_execute(hData);
	    return (HDDEDATA)DDE_FACK;
      }
  }
  return (HDDEDATA)NULL;
}


/* This should be during startup in WinMain */
BOOL
dde_initialise()
{
    lpDdeProc = MakeProcInstance((FARPROC)GSviewDdeCallback, phInstance);
    if (DdeInitialize(&idInst, (PFNCALLBACK)lpDdeProc, CBF_FAIL_POKES, 0L)) {
	return FALSE;
    }
    hszService = DdeCreateStringHandleA(idInst, (LPSTR)szServiceName, 0);
    hszTopic = DdeCreateStringHandleA(idInst, (LPSTR)szTopicName, 0);
    return TRUE;
}

/* This should be during shutdown in WinMain */
void
dde_uninitialise()
{
    dde_enable_server(FALSE);
    DdeFreeStringHandle(idInst, hszService);
    DdeFreeStringHandle(idInst, hszTopic);
    DdeUninitialize(idInst);
    idInst = 0;
}

void
dde_enable_server(BOOL enable) 
{
    if (enable && !server_enabled) {
	server_enabled = TRUE;
	DdeNameService(idInst, hszService, NULL, DNS_REGISTER);
    }
    else if (!enable && server_enabled) {
	server_enabled = FALSE;
	DdeNameService(idInst, hszService, NULL, DNS_UNREGISTER);
    }
}

/* Send a command string to server */
/* Assume dde_initialise has already been called */
BOOL
dde_execute(char *str)
{
HSZ hszServName;
HSZ hszTopicName;
HCONV hConv;
DWORD dwResult = DDE_FNOTPROCESSED;

    /* Make connection to server */
    hszServName = DdeCreateStringHandleA(idInst, (LPSTR)szServiceName, 
	CP_WINANSI);
    hszTopicName = DdeCreateStringHandleA(idInst, (LPSTR)szTopicName, 
	CP_WINANSI);
    hConv = DdeConnect(idInst, hszServName, hszTopicName, (PCONVCONTEXT)NULL);
    if (hConv == NULL) {
	DdeFreeStringHandle(idInst, hszServName);
	DdeFreeStringHandle(idInst, hszTopicName);
	if (debug)
	    gs_addmess("DdeConnect failed\r\n");
	return FALSE;
    }

    /* send command */
    DdeClientTransaction((LPBYTE)str, strlen(str)+1, hConv,
	NULL, CF_TEXT, XTYP_EXECUTE, 10000, &dwResult);

    /* Disconnect from server */
    DdeDisconnect(hConv);
    DdeFreeStringHandle(idInst, hszServName);
    DdeFreeStringHandle(idInst, hszTopicName);
    if (debug) {
	gs_addmess("Sent XTYP_EXECUTE: ");
	gs_addmess(str);
	gs_addmess("\r\n");
    }
    return (dwResult == DDE_FACK);
}

BOOL
dde_execute_line(char *str)
{
BOOL result;
char *cmd = (char *)malloc(strlen(str) + 64);
    if (cmd == (char *)NULL)
	return FALSE;
    sprintf(cmd, "[Command(%s)]", str);
    result = dde_execute(cmd);
    free(cmd);
    return result; 
}


char *
skiparg(char *p)
{
    while (*p != ')') {
	/* skip over argument */
	if (*p == '\042') {
	    /* skip over argument, until closing quote */
	    p++;
	    while (*p && (*p != '\042'))
		p++;
	    if (*p) {
		*p = '\0';	/* remove closing quote */
		p++;
	    }
	}
	else 
	    p++;
    }
    if (*p) {
	/* remove closing ) */
	*p = '\0';
	p++;
    }
    return p;
}

/* This is called whenever someone sends us XTYP_EXECUTE */
void 
dde_server_execute(HDDEDATA hData)
{
char buffer[256];
char *p;
char *q;
    DdeGetData(hData, (LPBYTE)buffer, sizeof(buffer), 0);
    DdeFreeDataHandle(hData);

    if (debug) {
	gs_addmess("XTYP_EXECUTE: ");
	gs_addmess(buffer);
	gs_addmess("\r\n");
    }


    p = buffer;
    while (*p) {
	while (*p && *p!= '[')
	    p++;
        if (*p)
	    p++;
	q = p;

	if (strncmp(p, "Command(", 8) == 0) {
	    p+= 8;
	    q = skiparg(p);
	    if (*p == '\042')
		p++;
	    drop_filename(hwndimg, p);
	}
	else if (strncmp(p, "FileOpen(", 9) == 0) {
	    p+= 9;
	    q = skiparg(p);
	    if (*p == '\042')
		p++;
	    drop_filename(hwndimg, p);
	}
	else if (strncmp(p, "FileExit(", 9) == 0) {
	    p+= 9;
	    q = skiparg(p);
	    if (print_count)
		/* if busy printing, exit after finished */
		print_exit = TRUE;
	    else
		/* otherwise exit now */
	        gsview_command(IDM_EXIT);
	}
	else if (strncmp(p, "NextPage(", 9) == 0) {
	    p+= 9;
	    q = skiparg(p);
	    gsview_command(IDM_NEXT);
	}
	else if (strncmp(p, "PrevPage(", 9) == 0) {
	    p+= 9;
	    q = skiparg(p);
	    gsview_command(IDM_PREV);
	}
	else if (strncmp(p, "GoBack(", 7) == 0) {
	    p+= 7;
	    q = skiparg(p);
	    gsview_command(IDM_GOBACK);
	}
	else if (strncmp(p, "GotoPage(", 9) == 0) {
	    int page;
	    p+= 9;
	    q = skiparg(p);
	    page = atoi(p);
	    gsview_goto_page(page);
	}
	else if (strncmp(p, "ShowWindow(", 11) == 0) {
 	    int nCmdShow;
	    p+= 11;
	    q = skiparg(p);
	    nCmdShow = atoi(p);
	    ShowWindow(hwndimg, nCmdShow);
	    if ((nCmdShow == SW_SHOW) || (nCmdShow == SW_SHOWMAXIMIZED) 
		|| (nCmdShow == SW_SHOWNORMAL))
	        SetForegroundWindow(hwndimg);
	}
	else if (strncmp(p, "FullScreen(", 11) == 0) {
	    p+= 11;
	    q = skiparg(p);
	    gsview_fullscreen();
	}
	
	p = q;
	while (*p && *p!= ']')
	    p++;
    }
}

