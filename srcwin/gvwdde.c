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

/* gvwdde.c */
/* Program Manager interface for Windows GSview */

#include "gvwin.h"
#include <ddeml.h>

/* NOT Unicode */


#define GSVIEW_NAME "GSview"

#ifdef __BORLANDC__
#pragma argsused	/* ignore warning for next function */
#endif
HDDEDATA CALLBACK 
DdeCallback(UINT type, UINT fmt, HCONV hconv,
    HSZ hsz1, HSZ hsz2, HDDEDATA hData, DWORD dwData1, DWORD dwData2)
{
  switch (type) {
    default:
	return (HDDEDATA)NULL;
  }
}

char *
group_token(char **s)
{
char *p, *q;
    q = p = *s;
    while (*p && (*p != '\r') && (*p != '\n') && (*p != ','))
	p++;
    if (*p) {
	*p++ = '\0';
	*s = p;
    }
    return q;
}

void
add_group_item(FILE *f, char *line)
{
char *cmdline, *name, *iconpath;
char *iconindex, *xpos, *ypos;
char *defdir, *hotkey, *minimize;
char *p;
int i;
TCHAR wbuf[64];
char buf[64];
char untitle[128];
    p = line;
    name = group_token(&p);
    cmdline = group_token(&p);
    defdir = group_token(&p);
    iconpath = group_token(&p);
    xpos = group_token(&p);
    ypos = group_token(&p);
    iconindex = group_token(&p);
    hotkey = group_token(&p);
    minimize = group_token(&p);
    if (is_win4) {
	i = strlen(cmdline);
	if ( (i>2) && (cmdline[i-2]=='\042') && (cmdline[i-1]=='\042')) {
	    /* Need to massage cmdline to be acceptable to AddItem */
	    /* Need quotes separately around command and arguments */
	    /* find quote at start of arguments */
	    p = cmdline+1;
	    while (*p && *p != '\042')
		p++;
	    /* move arguments back one character and insert quote */
	    if ((int)(p-cmdline) > 2) {
		p--;	 	/* point at space before quote */
		memmove(p+1, p, strlen(p)-1);
		*p = '\042';	/* place quote after EXE name */
	    }
	}
    }

    load_string(IDS_UNINSTALLITEM, wbuf, sizeof(wbuf)/sizeof(TCHAR));
    convert_widechar(buf, wbuf, lstrlen(wbuf));
    sprintf(untitle, "\042%s\042", buf);
    sprintf(buf, "\042%s\042", GSVIEW_NAME);
    if (   (strcmp(name, buf) == 0)
	|| (strcmp(name, "\042GSview README\042") == 0)
	|| (strcmp(name, "\042Ghostscript\042") == 0)
	|| (strcmp(name, "\042Ghostscript README\042") == 0)
	|| (strcmp(name, untitle) == 0) )
	fprintf(f, "[AddItem(%s,%s,%s,%s,%s,%s,%s,%s,%s)]\n",
	    cmdline, name, iconpath, iconindex, xpos, ypos, 
	    defdir, hotkey, minimize);
}

int
gsview_progman(char *groupname, char *gsviewpath, int gsver, char *gspath, char *gsargs)
{
DWORD idInst = 0L;
FARPROC lpDdeProc;
HSZ hszServName;
HSZ hszSysTopic;
HSZ hszGroupsItem;
HCONV hConv;
HDDEDATA hdata = NULL;
char setup[MAXSTR+MAXSTR];
DWORD dwResult;
char groupfile[MAXSTR];
int i;
char *s, *d;
FILE *ddefile;
char gspathbuf[MAXSTR];
char gsviewpathbuf[MAXSTR];
char gsdocbuf[MAXSTR];

    strncpy(gspathbuf, gspath, sizeof(gspathbuf)/sizeof(TCHAR));
    strncpy(gsdocbuf, gsargs, sizeof(gsdocbuf)/sizeof(TCHAR));
    d = strchr(gsdocbuf, ';');
    if (d)
       *d = '\0';
    if (gsver >= 593) {
	d = strrchr(gsdocbuf, '\\');
	if (d) {
	    d++;
	    strcpy(d, "doc\\");
	}
    }
    else {
	strcat(gsdocbuf, "\\");
    }
    strncpy(gsviewpathbuf, gsviewpath, sizeof(gsviewpathbuf));
    if (!is_win32s) {
	/* The DDE interface isn't reliable with long names */
	/* Convert everything to short names */
	GetShortPathNameA(gspath, gspathbuf, sizeof(gspathbuf));
	GetShortPathNameA(gsviewpath, gsviewpathbuf, sizeof(gsviewpathbuf));
    }

    /* Open ProgMan DDE undo file if it doesn't exist */
    strcpy(setup, gsviewpathbuf);
    strcat(setup, GSVIEW_ZIP);
    d = strrchr(setup, '.');
    strcpy(d, "dde.log");
    ddefile = fopen(setup, "r");
    if (ddefile != (FILE *)NULL) {
	/* We found a previous ProgMan DDE undo file. */
	/* Don't touch it since we want to keep the original record */
	/* of the ProgMan state before GSview was installed */ 
	fclose(ddefile);
	ddefile = (FILE *)NULL;
    }
    else {
        ddefile = fopen(setup, "w");
	/* If we fail to open the file for writing, the destination is probably 
	 * read only.  Don't worry, just don't write to the log file.
	 */
    }

    /* derive group filename from group name */
    for (i=0, s=groupname, d=groupfile; i<8 && *s; s++) {
	if (isalpha((int)(*s)) || isdigit((int)(*s))) {
	    *d++ = *s;
	    i++;
	} 
    }
    *d = '\0';
    if (strlen(groupfile)==0)
	strcpy(groupfile, "gstools");

    lpDdeProc = MakeProcInstance((FARPROC)DdeCallback, phInstance);
    if (DdeInitialize(&idInst, (PFNCALLBACK)lpDdeProc, CBF_FAIL_POKES, 0L)) {
	return 1;
    }
    hszServName = DdeCreateStringHandleA(idInst, "PROGMAN", CP_WINANSI);
    hszSysTopic = DdeCreateStringHandleA(idInst, "PROGMAN", CP_WINANSI);
    hConv = DdeConnect(idInst, hszServName, hszSysTopic, (PCONVCONTEXT)NULL);
    if (hConv == NULL) {
	DdeFreeStringHandle(idInst, hszServName);
	DdeFreeStringHandle(idInst, hszSysTopic);
	return 1;
    }

    if (ddefile) {
	/* Find out if group existed */
	hszGroupsItem = DdeCreateStringHandleA(idInst, groupname, CP_WINANSI);
	hdata = DdeClientTransaction((LPBYTE)NULL, 0, hConv,\
	    hszGroupsItem, CF_TEXT, XTYP_REQUEST, 5000, &dwResult);
	DdeFreeStringHandle(idInst, hszGroupsItem);
    }

#define DDEEXECUTE(str)\
    DdeClientTransaction((LPBYTE)str, strlen(str)+1, hConv,\
	NULL, CF_TEXT, XTYP_EXECUTE, 5000, &dwResult)

    sprintf(setup, "[CreateGroup(\042%s\042,%s.grp)][ShowGroup(\042%s\042,1)]",
	groupname, groupfile, groupname);  /* display, active */
    DDEEXECUTE(setup);
    if (ddefile)	/* display, no active */
        fprintf(ddefile, "[ShowGroup(\042%s\042,8)]\n",groupname);
    sprintf(setup, "[ReplaceItem(\042%s\042)]", GSVIEW_NAME);
    DDEEXECUTE(setup);
#ifdef _WIN64
#define GSVIEW_ICON "gsview64.ico"
#else
#define GSVIEW_ICON "gsview32.ico"
#endif
    if (!is_win4)
       sprintf(setup, "[AddItem(\042%s%s\042,\042%s\042, \042%s%s\042)]", 
	  gsviewpathbuf, GSVIEW_EXENAME, GSVIEW_NAME, gsviewpathbuf, 
	  GSVIEW_ICON);
    else
       sprintf(setup, "[AddItem(\042%s%s\042,\042%s\042)]", 
	  gsviewpathbuf, GSVIEW_EXENAME, GSVIEW_NAME);
    DDEEXECUTE(setup);
    if (ddefile)
        fprintf(ddefile, "[DeleteItem(\042%s\042)]\n", GSVIEW_NAME);

/* Win3.1 documentation says you must put quotes around names */
/* with embedded spaces. */
/* In Win95, it appears you must put quotes around the EXE name */
/* and options separately */

    sprintf(setup, "[ReplaceItem(\042GSview README\042)]");
    DDEEXECUTE(setup);
#ifdef NOTUSED_IN_GSVIEW28
    if (!is_win4)
	sprintf(setup, "[AddItem(\042notepad.exe %sREADME.TXT\042,\042GSview README\042)]", 
	    gsviewpathbuf);
    else
	sprintf(setup, "[AddItem(\042notepad.exe\042 \042%sREADME.TXT\042,\042GSview README\042,\042notepad.exe\042,1)]", 
	    gsviewpathbuf);
#endif
    sprintf(setup, "[AddItem(\042%sReadme.htm\042,\042GSview README\042)]", 
	    gsviewpathbuf);
    DDEEXECUTE(setup);
    if (ddefile)
        fprintf(ddefile, "[DeleteItem(\042%s\042)]\n", "GSview README");

    sprintf(setup, "[ReplaceItem(\042Ghostscript\042)]");
    DDEEXECUTE(setup);
    if (!is_win4)
        sprintf(setup, "[AddItem(\042%s%s -I%s\042,\042Ghostscript\042, \042%sgstext.ico\042)]", 
	    gspathbuf, GS_EXENAME, gsargs, gspathbuf);
    else
        sprintf(setup, "[AddItem(\042%s%s\042 \042-I%s\042,\042Ghostscript\042)]", 
	    gspathbuf, GS_EXENAME, gsargs);
    DDEEXECUTE(setup);
    if (ddefile)
        fprintf(ddefile, "[DeleteItem(\042%s\042)]\n", "Ghostscript");

    sprintf(setup, "[ReplaceItem(\042Ghostscript README\042)]");
    DDEEXECUTE(setup);
    if (gsver >= 540) {
	    sprintf(setup, 
		"[AddItem(\042%sReadme.htm\042,\042Ghostscript README\042)]", 
		 gsdocbuf);
    }
    else {
	if (!is_win4)
	    sprintf(setup, "[AddItem(\042notepad.exe %sREADME.\042,\042Ghostscript README\042)]", 
		 gsdocbuf);
	else
	    sprintf(setup, "[AddItem(\042notepad.exe\042 \042%sREADME.\042,\042Ghostscript README\042, \042notepad.exe\042,1)]", 
		 gsdocbuf);
    }
    DDEEXECUTE(setup);
    if (ddefile)
        fprintf(ddefile, "[DeleteItem(\042%s\042)]\n", "Ghostscript README");


#undef DDEXECUTE

    /* Now remember the way things were */
    if (ddefile) {
      if (hdata) {
	DWORD dlen;
	BYTE FAR *lpData = DdeAccessData(hdata, &dlen);
	LPSTR p, q;
	/* skip first line */
	q = (LPSTR)lpData;
	while (*q && (*q != '\r') && (*q != '\n'))
	    q++;
	while (*q && ((*q == '\r') || (*q == '\n')))
	    q++;
	p = q;
	/* for each group item */
	while (*p) {
	    /* skip to end of line */
	    while (*q && (*q != '\r') && (*q != '\n'))
		q++;
	    strncpy(setup, p, (int)(q-p)+1);
	    add_group_item(ddefile, setup);
	    /* skip to start of next group name */
	    while (*q && ((*q == '\r') || (*q == '\n')))
		q++;
	    p = q;
	}
	if (ddefile)		/* display, no active */
	    fprintf(ddefile, "[ShowGroup(\042%s\042,8)]\n",groupname);
	DdeUnaccessData(hdata);
	DdeFreeDataHandle(hdata);
      }
      else {
	/* group didn't exist before, so delete it */
        fprintf(ddefile, "[DeleteGroup(\042%s\042)]\n", groupname);
      }
      fclose(ddefile);
    }

    DdeDisconnect(hConv);
    DdeFreeStringHandle(idInst, hszServName);
    DdeFreeStringHandle(idInst, hszSysTopic);
    DdeUninitialize(idInst);

    return 0;
}

