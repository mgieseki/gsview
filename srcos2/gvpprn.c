/* Copyright (C) 1993-2000, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvpprn.c */
/* Printer routines for PM GSview */
#include "gvpm.h"

char not_defined[] = "[Not defined]";

#define PRINT_BUF_SIZE 16384u

// Nasty globals
int print_gdi_width;
int print_gdi_height;
int print_gdi_xdpi;
int print_gdi_ydpi;
ULONG print_gdi_read_handle;
ULONG print_gdi_write_handle;

MRESULT EXPENTRY
SpoolDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
int notify_message;
int i, j;
char *entry;
    switch(msg) {
      case WM_INITDLG:
	entry = (char *)mp2;
	j = 0;
	i = 0;
	while (*entry) {
	    if ( strcmp(entry, option.printer_queue) == 0 )
		j = i;
	    entry += strlen(entry)+1; /* skip to queue comment */
	    if (*entry) {
	        WinSendMsg( WinWindowFromID(hwnd, SPOOL_PORT),
	    	    LM_INSERTITEM, MPFROMLONG(LIT_END), MPFROMP(entry));
	        entry += strlen(entry)+1;
	    }
	    i++;
	}
	WinSendMsg( WinWindowFromID(hwnd, SPOOL_PORT),
	    	LM_SELECTITEM, MPFROMLONG(j), MPFROMLONG(TRUE) );
	break;
    case WM_CONTROL:
	notify_message = SHORT2FROMMP(mp1);
	switch (notify_message) {
	    case LN_ENTER:
	        if (SHORT1FROMMP(mp1) == SPOOL_PORT)
		    WinPostMsg(hwnd, WM_COMMAND, (MPARAM)DID_OK, MPFROM2SHORT(CMDSRC_OTHER, TRUE));
		break;
	}
	break;
    case  WM_COMMAND:
      switch(LOUSHORT(mp1)) {
	case ID_HELP:
	    get_help();
	    return (MRESULT)TRUE;
        case DID_OK:
	    WinDismissDlg(hwnd, 1+(int)WinSendMsg(WinWindowFromID(hwnd, SPOOL_PORT), 
		LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0));
            return (MRESULT)TRUE;
	case DID_CANCEL:
	    WinDismissDlg(hwnd, 0);
	    return (MRESULT)TRUE;
      }
      break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

char *
get_ports(char *buf, int len)
{
/* get port list from SplEnumQueue */
/* port list consists of entries formatted as follows */
/*    queue_name,'\0',queue_comment,'\0' */
/* terminated by double null */
/* We need to use queue_name, user needs to see queue_comment */

    SPLERR splerr;
    ULONG  cbBuf ;
    ULONG  cTotal;
    ULONG  cReturned ;
    ULONG  cbNeeded ;
    ULONG  ulLevel ;
    ULONG  i ;
    PSZ    pszComputerName ;
    PBYTE  pBuf ;
    PPRQINFO3 prq ;
    PBYTE pdrivbuf;
    int used;
    char *p;
 
    used = 0;
    p = buf;
    ulLevel = 3L;
    pszComputerName = (PSZ)NULL ;
    splerr = SplEnumQueue(pszComputerName, ulLevel, pBuf, 0L, /* cbBuf */
                          &cReturned, &cTotal,
                          &cbNeeded, NULL);
    if ( splerr == ERROR_MORE_DATA || splerr == NERR_BufTooSmall ) {
       if (!DosAllocMem( (PPVOID)&pBuf, cbNeeded,
                         PAG_READ|PAG_WRITE|PAG_COMMIT) ) {
          cbBuf = cbNeeded ;
          splerr = SplEnumQueue(pszComputerName, ulLevel, pBuf, cbBuf,
                                  &cReturned, &cTotal,
                                  &cbNeeded, NULL);
          if ( (splerr == NO_ERROR) &&
             !DosAllocMem((PPVOID)&pdrivbuf, 1024, PAG_READ|PAG_WRITE|PAG_COMMIT))
	  {
             /* Set pointer to point to the beginning of the buffer.           */
             prq = (PPRQINFO3)pBuf;
             /* cReturned has the count of the number of PRQINFO3 structures.  */
             for (i=0;i < cReturned ; i++) {
		/* Don't list if connected to FILE */
	        splerr = SplQueryDevice(pszComputerName, prq->pszPrinters, 3, 
			pdrivbuf, 1024L, &cbNeeded);
		if ((splerr != NO_ERROR) || 
		    (strcmp("FILE", (char *)(((PPRDINFO3)pdrivbuf)->pszLogAddr)) != 0))
		{
		    used += strlen((char *)(prq->pszName)) + 1;
		    used += strlen((char *)(prq->pszComment)) + 1;
		    if (used < len) {
			strcpy(p, (char *)(prq->pszName));
			p += strlen(p)+1;
			strcpy(p, (char *)(prq->pszComment));
			p += strlen(p)+1;
		    }
		}
                prq++;
             }/*endfor cReturned */
             DosFreeMem((PVOID)pdrivbuf) ;
          }
          DosFreeMem((PVOID)pBuf) ;
       }
    } /* end if Q level given */
    else {
	*p++ = '\0';
        *p = '\0';
	return NULL;
    }
    *p = '\0';
    return buf;
}


/* return TRUE if portname available */
/* return FALSE if cancelled or error */
/* if port non-NULL, use as suggested port */
BOOL
get_portname(char *portname, char *port)
{
char *buffer;
char *p;
int i, iport;
	if ((buffer = (char *)malloc(PRINT_BUF_SIZE)) == (char *)NULL)
	    return FALSE;
	/* get list of ports */
	get_ports(buffer, PRINT_BUF_SIZE);
	if (port != (char *)NULL) {
	    /* use specified port */
	    p = buffer;
	    while (strlen(p) != 0) {
		if (strcmp(p, port)==0)
		    break;
		p += strlen(p)+1;	/* skip queue name */
		if (*p)
		    p += strlen(p)+1;	/* skip queue comment */
	    }
	    if (*p) {
		strcpy(portname, "\\\\spool\\");
		strcat(portname, p);
		strcpy(option.printer_queue, p);
	    }
	    else
	        port = NULL;	/* couldn't find it, so prompt for valid port */
	}
	if (port == (char *)NULL) {
	    /* select a port */
	    nHelpTopic = IDS_TOPICSPOOL;
	    iport = WinDlgBox(HWND_DESKTOP, hwnd_frame, SpoolDlgProc, hlanguage, IDD_SPOOL, buffer);
	    if (!iport || iport == 65536) {
		free(buffer);
		return FALSE;
	    }
	    p = buffer;
	    for (i=2; i<iport+iport && strlen(p)!=0; i++)
		p += strlen(p)+1;
	    strcpy(portname, "\\\\spool\\");
	    strcat(portname, p);
	    strcpy(option.printer_queue, p);
	}

	if (strlen(portname) == 0)
	    return FALSE;
	free(buffer);
	return TRUE;
}

/* If strlen(queue_name)==0, return default queue and driver name */
/* If queue_name supplied, return driver_name */
/* returns 0 if OK, non-zero for error */
int
spl_find_queue(char *queue_name, char *driver_name)
{
    SPLERR splerr;
    ULONG  cbBuf;
    ULONG  cTotal;
    ULONG  cReturned;
    ULONG  cbNeeded;
    ULONG  ulLevel;
    ULONG  i;
    PSZ    pszComputerName;
    PBYTE  pBuf;
    PPRQINFO3 prq;
 
    ulLevel = 3L;
    pszComputerName = (PSZ)NULL ;
    splerr = SplEnumQueue(pszComputerName, ulLevel, pBuf, 0L, /* cbBuf */
                          &cReturned, &cTotal,
                          &cbNeeded, NULL);
    if ( splerr == ERROR_MORE_DATA || splerr == NERR_BufTooSmall ) {
       if (!DosAllocMem( (PPVOID)&pBuf, cbNeeded,
                         PAG_READ|PAG_WRITE|PAG_COMMIT) ) {
          cbBuf = cbNeeded ;
          splerr = SplEnumQueue(pszComputerName, ulLevel, pBuf, cbBuf,
                                  &cReturned, &cTotal,
                                  &cbNeeded, NULL);
          if (splerr == NO_ERROR) {
             /* Set pointer to point to the beginning of the buffer.           */
             prq = (PPRQINFO3)pBuf ;
 
             /* cReturned has the count of the number of PRQINFO3 structures.  */
             for (i=0;i < cReturned ; i++) {
		    /* find queue name and return driver name */
		    if (strlen(queue_name)==0) {  /* use default queue */
		        if ( prq->fsType & PRQ3_TYPE_APPDEFAULT )
			    strcpy(queue_name, (char *)(prq->pszName));
		    }
		    if (strcmp((char *)(prq->pszName), queue_name) == 0) {
			char *p;
			for (p=(char *)(prq->pszDriverName); *p && (*p!='.'); p++)
			    /* do nothing */ ;
			*p = '\0';  /* truncate at '.' */
			if (driver_name != NULL)
			    strcpy(driver_name, (char *)(prq->pszDriverName));
			DosFreeMem((PVOID)pBuf) ;
			return 0;
		    }
                prq++;
             }/*endfor cReturned */
          }
          DosFreeMem((PVOID)pBuf) ;
       }
    } /* end if Q level given */
    else {
       /* If we are here we had a bad error code. Print it and some other info.*/
       gserror(0, "SplEnumQueue failed", MB_ICONEXCLAMATION, SOUND_ERROR);
    }
    if (splerr)
        return splerr;
    return -1;
}


/* Spool file to queue */
/* queue==NULL means prompt for port with dialog box */
/* return TRUE if successful, FALSE if error */
int
gp_printfile(char *filename, char *queue)
{
HSPL hspl;
PDEVOPENSTRUC pdata;
PSZ  pszToken = (PBYTE)"*";
BOOL rc;
char queue_name[256];
char driver_name[256];
char port_name[256];
char *buffer;
FILE *f;
int count;
    if (!get_portname(port_name, queue))
	return FALSE;
    strcpy(queue_name, port_name+8);	/* ignore \\spool\ prefix */
    spl_find_queue(queue_name, driver_name);

    if ((buffer = (char *)malloc(PRINT_BUF_SIZE)) == (char *)NULL) {
	gserror(0, "Out of memory in gp_printfile", MB_ICONEXCLAMATION, SOUND_ERROR);
	return FALSE;
    }
    if ((f = fopen(filename, "rb")) == (FILE *)NULL) {
	gserror(IDS_NOTEMP, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	free(buffer);
	return FALSE;
    }

    /* Allocate memory for pdata */
    if ( !DosAllocMem( (PPVOID)&pdata,sizeof( DEVOPENSTRUC ),
         (PAG_READ|PAG_WRITE|PAG_COMMIT ) ) ) {
        /* Initialize elements of pdata */
        pdata->pszLogAddress      = (PSZ)queue_name;
        pdata->pszDriverName      = (PSZ)driver_name;
        pdata->pdriv              = NULL;
        pdata->pszDataType        = (PSZ)"PM_Q_RAW";
        pdata->pszComment         = (PSZ)"GSview";
        pdata->pszQueueProcName   = NULL;
        pdata->pszQueueProcParams = NULL;
        pdata->pszSpoolerParams   = NULL;
        pdata->pszNetworkParams   = NULL;
 
        hspl = SplQmOpen( pszToken,5L,( PQMOPENDATA )pdata );
        if ( hspl == SPL_ERROR ) {
	    gserror(0, "SplQMOpen failed", MB_ICONEXCLAMATION, SOUND_ERROR);
	    DosFreeMem((PVOID)pdata);
	    free(buffer);
	    fclose(f);
	    return FALSE;	/* failed */
        }

	rc = SplQmStartDoc(hspl, (PBYTE)filename);
	if (!rc) {
	    gserror(0, "SplQMStartDoc failed", MB_ICONEXCLAMATION, SOUND_ERROR);
	    DosFreeMem((PVOID)pdata);
	    free(buffer);
	    fclose(f);
	    return FALSE;
	}
	
	/* loop, copying file to queue */
	while (rc && (count = fread(buffer, 1, PRINT_BUF_SIZE, f)) != 0 ) {
	    rc = SplQmWrite(hspl, count, buffer);
	    if (!rc)
	        gserror(0, "SplQMWrite failed", MB_ICONEXCLAMATION, SOUND_ERROR);
  	}
	free(buffer);
	fclose(f);

	if (!rc) {
	  gserror(0, "Aborting Spooling", MB_ICONEXCLAMATION, SOUND_ERROR);
          SplQmAbort(hspl);
        }
	else {
	    SplQmEndDoc(hspl);
	    rc = SplQmClose(hspl);
	    if (!rc)
	        gserror(0, "SplQMClose failed", MB_ICONEXCLAMATION, SOUND_ERROR);
	}
    }
    else
	rc = FALSE;	/* no memory */
    return rc;
}


void
strip_spaces(char *s)
{
char *d = s;
   while (*s) {
	if (*s != ' ')
	    *d++ = *s;
	s++;
   }
   *d = '\0';
}

char editpropname[MAXSTR];

/* dialog for adding for editing properties */
MRESULT EXPENTRY
EditPropDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static char device[MAXSTR];
    switch (msg) {
      case WM_INITDLG:
	strcpy(device, (char *)mp2);	/* initialise device name */
	WinSendMsg( WinWindowFromID(hwnd, EDITPROP_NAME),
	    EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	WinSendMsg( WinWindowFromID(hwnd, EDITPROP_VALUE),
	    EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	if (*editpropname) {
	  PROFILE *prf;
	  char section[MAXSTR];
	  char buf[MAXSTR];
	  if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
	    if (*editpropname == 's')
		WinSendMsg( WinWindowFromID(hwnd, EDITPROP_STRING),
		    BM_SETCHECK, MPFROMSHORT(1), MPFROMLONG(0));
	    else
		WinSendMsg( WinWindowFromID(hwnd, EDITPROP_NUMBER),
		    BM_SETCHECK, MPFROMSHORT(1), MPFROMLONG(0));
	    WinSetWindowText(WinWindowFromID(hwnd, EDITPROP_NAME), (PBYTE)(editpropname+1));
	    strcpy(section, device);
	    strcat(section, " values");
	    profile_read_string(prf, section, editpropname, "", buf, sizeof(buf)-2);
	    WinSetWindowText(WinWindowFromID(hwnd, EDITPROP_VALUE), (PCSZ)buf);
	    profile_close(prf);
	  }
	}
	else {
	    WinSendMsg( WinWindowFromID(hwnd, EDITPROP_NUMBER),
	        BM_SETCHECK, MPFROMSHORT(1), MPFROMLONG(0));
	}
	break;
      case WM_CONTROL:
	break;
      case WM_COMMAND:
	switch(LOUSHORT(mp1)) {
	    case ID_HELP:
		get_help();
		return (MRESULT)TRUE;
	    case EDITPROP_DEL:
	    	{PROFILE *prf;
/* need to reopen profile file - this is wasteful */
		  if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		    char name[MAXSTR];
		    char section[MAXSTR];
		    if (WinSendMsg(WinWindowFromID(hwnd, EDITPROP_STRING), BM_QUERYCHECK,  
			MPFROMLONG(0), MPFROMLONG(0)) > 0) {
			strcpy(name, "s");
		    }
		    else
			strcpy(name, "d");
	    	    WinQueryWindowText(WinWindowFromID(hwnd, EDITPROP_NAME), sizeof(name)-2, (PBYTE)(name+1));
		    strip_spaces(name);
		    if (strlen(name)>1) {
			strcpy(section, device);
			strcat(section, " values");
			profile_write_string(prf, section, name, NULL);
			profile_write_string(prf, device, name, NULL);
		    }
		    profile_close(prf);
		  }
		}
		WinDismissDlg(hwnd, DID_OK);
            	return (MRESULT)TRUE;
	    case DID_OK:
	    	{PROFILE *prf;
/* need to reopen profile file - this is wasteful */
		  if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		    char name[MAXSTR];
		    char value[MAXSTR];
		    char section[MAXSTR];
		    if (WinSendMsg(WinWindowFromID(hwnd, EDITPROP_STRING), BM_QUERYCHECK,  
			MPFROMLONG(0), MPFROMLONG(0)) > 0) {
			strcpy(name, "s");
		    }
		    else
			strcpy(name, "d");
	    	    WinQueryWindowText(WinWindowFromID(hwnd, EDITPROP_NAME), sizeof(name)-2, (PBYTE)(name+1));
	    	    WinQueryWindowText(WinWindowFromID(hwnd, EDITPROP_VALUE), sizeof(value), (PBYTE)value);
		    strip_spaces(name);
		    strip_spaces(value);
		    if ((strlen(name)>1) && strlen(value)) {
			strcpy(section, device);
			strcat(section, " values");
			profile_write_string(prf, section, name, value);
			strtok(value, ",");
			profile_write_string(prf, device, name, value);
		    }
		    profile_close(prf);
		  }
		}
		WinDismissDlg(hwnd, DID_OK);
            	return (MRESULT)TRUE;
	    case DID_CANCEL:
		WinDismissDlg(hwnd, DID_CANCEL);
		return (MRESULT)TRUE;
	}
	break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


/* dialog box for selecting printer properties */
MRESULT EXPENTRY
PropDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	char buf[128];
	int iprop;
	int ivalue;
	char *p;
	char *value;
	static char notdef[128];
	static char device[MAXSTR];	/* contains printer device name */
	static struct prop_item_s* propitem;
	char section[MAXSTR];

    switch (msg) {
      case WM_INITDLG:
	strcpy(device, (char *)mp2);	/* initialise device name */
	load_string(IDS_NOTDEFTAG, notdef, sizeof(notdef));
	propitem = get_properties(device);
	if (propitem == (struct prop_item_s *)NULL) {
	    WinDismissDlg(hwnd, FALSE);
	    return (MRESULT)TRUE;
	}
	WinSendMsg(WinWindowFromID(hwnd, PROP_NAME), LM_DELETEALL, (MPARAM)0, (MPARAM)0);
	WinSendMsg(WinWindowFromID(hwnd, PROP_VALUE), LM_DELETEALL, (MPARAM)0, (MPARAM)0);
	WinSetWindowText(WinWindowFromID(hwnd, PROP_NAME), (PCSZ)"");
	WinSetWindowText(WinWindowFromID(hwnd, PROP_VALUE), (PCSZ)"");
	for (iprop=0; propitem[iprop].name[0]; iprop++) {
	    WinSendMsg( WinWindowFromID(hwnd, PROP_NAME),
	    	LM_INSERTITEM, MPFROMLONG(LIT_END), MPFROMP(propitem[iprop].name+1));
	}
        WinEnableWindow(WinWindowFromID(hwnd, PROP_NAME), (iprop != 0));
        WinEnableWindow(WinWindowFromID(hwnd, PROP_VALUE), (iprop != 0));
        WinEnableWindow(WinWindowFromID(hwnd, PROP_EDIT), (iprop != 0));

	WinSendMsg( WinWindowFromID(hwnd, PROP_NAME),
	    	LM_SELECTITEM, MPFROMLONG(0), MPFROMLONG(TRUE) );
/*
	WinSendMsg(hwnd, WM_CONTROL, MPFROM2SHORT(PROP_NAME, CBN_LBSELECT),
	    	MPFROMLONG(WinWindowFromID(hwnd, PROP_NAME)));
*/
	{
	    PROFILE *prf;
	    strcpy(section, device);
	    strcat(section, " Options");
  /* need to reopen profile file - this is wasteful */
	    if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		profile_read_string(prf, section, "Xoffset", "0", buf, sizeof(buf)-2);
	        WinSendMsg( WinWindowFromID(hwnd, PROP_XOFFSET),
	    	    EM_SETTEXTLIMIT, MPFROM2SHORT(sizeof(buf)-2, 0), MPFROMLONG(0) );
		WinSetWindowText(WinWindowFromID(hwnd, PROP_XOFFSET), (PCSZ)buf);
		profile_read_string(prf, section, "Yoffset", "0", buf, sizeof(buf)-2);
	        WinSendMsg( WinWindowFromID(hwnd, PROP_YOFFSET),
	    	    EM_SETTEXTLIMIT, MPFROM2SHORT(sizeof(buf)-2, 0), MPFROMLONG(0) );
		WinSetWindowText(WinWindowFromID(hwnd, PROP_YOFFSET), (PCSZ)buf);
		profile_close(prf);
	    }
	}
	break;
      case WM_CONTROL:
	if (mp1 == MPFROM2SHORT(PROP_NAME, CBN_LBSELECT)) {
	    iprop = (int)WinSendMsg(WinWindowFromID(hwnd, PROP_NAME), LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0);
	    if (iprop == LIT_NONE)
	        return FALSE;
	    /* now look up entry in gsview.ini */
	    /* and update PROP_VALUE list box */
	    strcpy(section, device);
	    strcat(section, " values");
	    {PROFILE *prf;
/* need to reopen profile file - this is wasteful */
		if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
	    	    profile_read_string(prf, section, propitem[iprop].name, "", buf, sizeof(buf)-2);
		    profile_close(prf);
		}
		else
		    buf[0] = '\0';
	    }
	    while ((*buf) && (buf[strlen(buf)-1]==' '))
		buf[strlen(buf)-1] = '\0';    /* remove trailing spaces */
	    buf[strlen(buf)+1] = '\0';	/* put double NULL at end */
	    WinSendMsg(WinWindowFromID(hwnd, PROP_VALUE), LM_DELETEALL, (MPARAM)0, (MPARAM)0);
	    WinSendMsg( WinWindowFromID(hwnd, PROP_VALUE),
	    	        LM_INSERTITEM, MPFROMLONG(LIT_END), MPFROMP(notdef) );
	    p = buf;
	    if (*p != '\0') {
	      WinEnableWindow(WinWindowFromID(hwnd, PROP_VALUE), TRUE);
	      while (*p!='\0') {
		value = p;
		while ((*p!='\0') && (*p!=','))
		    p++;
		*p++ = '\0';
	    	WinSendMsg( WinWindowFromID(hwnd, PROP_VALUE),
	    	        LM_INSERTITEM, MPFROMLONG(LIT_END), MPFROMP(value) );
	      }
	    }
	    strcpy(buf, propitem[iprop].value);
	    if (strcmp(buf, not_defined)==0)
		strcpy(buf, notdef);
	    iprop = (int)WinSendMsg( WinWindowFromID(hwnd, PROP_VALUE),
	    	    LM_SEARCHSTRING, MPFROM2SHORT(LSS_CASESENSITIVE, LIT_FIRST),
		    MPFROMP(buf) );
	    if ((iprop == LIT_ERROR) || (iprop == LIT_NONE)) {
		iprop = 0;
	        WinSetWindowText( WinWindowFromID(hwnd, PROP_VALUE), (PCSZ)buf);
	    }
	    else {
		WinSendMsg( WinWindowFromID(hwnd, PROP_VALUE),
		    LM_SELECTITEM, MPFROMLONG(iprop), MPFROMLONG(TRUE) );
	    }
	}
	if (mp1 == MPFROM2SHORT(PROP_VALUE, CBN_LBSELECT)) {
	    iprop = (int)WinSendMsg(WinWindowFromID(hwnd, PROP_NAME), LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0);
	    if (iprop == LIT_NONE)
	        return FALSE;
	    ivalue = (int)WinSendMsg(WinWindowFromID(hwnd, PROP_VALUE), LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0);
	    if (ivalue == LIT_NONE)
	        return FALSE;
	    WinSendMsg(WinWindowFromID(hwnd, PROP_VALUE), LM_QUERYITEMTEXT,  
		MPFROM2SHORT(ivalue, sizeof(propitem->value)), 
		MPFROMP(propitem[iprop].value));
	    if (strcmp(propitem[iprop].value, notdef)==0)
		strcpy(propitem[iprop].value, not_defined);
	}
	if (mp1 == MPFROM2SHORT(PROP_VALUE, CBN_EFCHANGE)) {
	    iprop = (int)WinSendMsg(WinWindowFromID(hwnd, PROP_NAME), LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0);
	    if (iprop == LIT_NONE)
	        return FALSE;
	    WinQueryWindowText(WinWindowFromID(hwnd, PROP_VALUE), 
			 sizeof(propitem->value), (PBYTE)(propitem[iprop].value));
	    if (strcmp(propitem[iprop].value, notdef)==0)
		strcpy(propitem[iprop].value, not_defined);
	}
	break;
    case WM_COMMAND:
	switch(LOUSHORT(mp1)) {
	    case PROP_EDIT:
	        nHelpTopic = IDS_TOPICEDITPROP;
	        iprop = (int)WinSendMsg(WinWindowFromID(hwnd, PROP_NAME), LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0);
		editpropname[0] = '\0';
		if (iprop != LIT_NONE)
		    strcpy(editpropname, propitem[iprop].name);
		WinDlgBox(HWND_DESKTOP, hwnd, EditPropDlgProc, hlanguage, IDD_EDITPROP, device);
		free((char *)propitem);
		WinSendMsg(hwnd, WM_INITDLG, MPFROMLONG(hwnd), MPFROMP(device));
	        nHelpTopic = IDS_TOPICPROP;
		return (MRESULT)TRUE;
	    case PROP_NEW:
	        nHelpTopic = IDS_TOPICEDITPROP;
		editpropname[0] = '\0';
		WinDlgBox(HWND_DESKTOP, hwnd, EditPropDlgProc, hlanguage, IDD_EDITPROP, device);
		free((char *)propitem);
		WinSendMsg(hwnd, WM_INITDLG, MPFROMLONG(hwnd), MPFROMP(device));
	        nHelpTopic = IDS_TOPICPROP;
		return (MRESULT)TRUE;
	    case ID_HELP:
		get_help();
		return (MRESULT)TRUE;
	    case DID_OK:
	    	{PROFILE *prf;
/* need to reopen profile file - this is wasteful */
		  if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		    for (iprop=0; propitem[iprop].name[0]; iprop++) {
			profile_write_string(prf, device, propitem[iprop].name, propitem[iprop].value);
		    }
		    strcpy(section, device);
		    strcat(section, " Options");
	    	    WinQueryWindowText(WinWindowFromID(hwnd, PROP_XOFFSET), sizeof(buf)-2, (PBYTE)(buf));
		    profile_write_string(prf, section, "Xoffset", buf);
	    	    WinQueryWindowText(WinWindowFromID(hwnd, PROP_YOFFSET), sizeof(buf)-2, (PBYTE)(buf));
		    profile_write_string(prf, section, "Yoffset", buf);
		    profile_close(prf);
		  }
		}
		free((char *)propitem);
		WinDismissDlg(hwnd, DID_OK);
            	return (MRESULT)TRUE;
	    case DID_CANCEL:
		free((char *)propitem);
		WinDismissDlg(hwnd, DID_CANCEL);
		return (MRESULT)TRUE;
	}
	break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

unsigned char *
GetUPPname(HWND hwnd, unsigned char *uppname, int upplen)
{
    unsigned char *p, *q;
    uppname[0] = '\0';
    WinQueryWindowText(WinWindowFromID(WinQueryWindow(hwnd, QW_OWNER),
	DEVICE_OPTIONS), upplen, uppname);
    p = uppname;
    if (p[0] == '"') {
	/* remove quotes around configuration file */
	p++;
	q = (unsigned char *)strrchr((char *)p, '"');
	if (q != NULL)
	    *q = '\0';
    }
    if (p[0] == '@')
	p++;
    if (strlen((char *)p))
        memmove(uppname, p, strlen((char *)p)+1);
    return uppname;
}


MRESULT EXPENTRY
UniDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    static char *ubuf;
    BYTE uppname[MAXSTR];	/* contains printer device name */
    int i;

    switch (msg) {
      case WM_INITDLG:
	    ubuf = NULL;
	    /* Delay initialization of the list box until 
	     * after it is displayed, because searching for 
	     * configuration files takes several seconds.
	     */
	    load_string(IDS_WAIT, (char *)uppname, sizeof(uppname));
	    WinSendMsg(WinWindowFromID(hwnd, UPP_LIST), LM_DELETEALL, 
		(MPARAM)0, (MPARAM)0);
	    WinSendMsg( WinWindowFromID(hwnd, UPP_LIST), LM_INSERTITEM, 
		MPFROMLONG(LIT_END), MPFROMP(uppname) );
	    WinEnableWindow(WinWindowFromID(hwnd, UPP_LIST), FALSE);
	    uppname[0] = uppname[1] = '\0';
	    GetUPPname(hwnd, uppname, sizeof(uppname));
	    WinSetWindowText(WinWindowFromID(hwnd, UPP_NAME), uppname);
	    WinPostMsg(hwnd, WM_COMMAND, (MPARAM)WM_USER, 
		MPFROM2SHORT(CMDSRC_OTHER, TRUE));
	    return (MRESULT)TRUE;
      case WM_CONTROL:
	if (mp1 == MPFROM2SHORT(UPP_LIST, LN_SELECT)) {
	    char dname[MAXSTR];
	    if (ubuf == NULL)
		return FALSE;
	    i = (int)WinSendMsg(WinWindowFromID(hwnd, UPP_LIST), 
		LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0);
	    if (i == LIT_NONE)
	        return FALSE;
	    WinSendMsg(WinWindowFromID(hwnd, UPP_LIST), LM_QUERYITEMTEXT,  
		MPFROM2SHORT(i, sizeof(dname)), MPFROMP(dname));
	    WinSetWindowText(WinWindowFromID(hwnd, UPP_NAME), 
		(PCSZ)uppmodel_to_name((char *)ubuf, (char *)dname));
	}
	if (mp1 == MPFROM2SHORT(UPP_LIST, LN_ENTER)) {
	    WinPostMsg(hwnd, WM_COMMAND, (MPARAM)IDOK, 
		MPFROM2SHORT(CMDSRC_OTHER, TRUE));
	}
	break;
    case WM_COMMAND:
	switch(LOUSHORT(mp1)) {
	    case WM_USER:
		/* time consuming initialization */
		{int needed;
		char *desc, *p, *q;
		/* draw window immediately */
		WinShowWindow(hwnd, TRUE);
		WinInvalidateRect(hwnd, (PRECTL)NULL, TRUE);
		WinUpdateWindow(hwnd);

		desc = NULL;
		uppname[0] = uppname[1] = '\0';
		i = 8192;	/* a guess */
		if ((ubuf = (char *)malloc(i)) == (char *)NULL) {
		    play_sound(SOUND_ERROR);
		    WinPostMsg(hwnd, WM_COMMAND, (MPARAM)DID_CANCEL, 
			MPFROM2SHORT(CMDSRC_OTHER, TRUE));
		    return (MRESULT)TRUE;	/* no memory */
		}
		needed = enum_upp_path(option.gsinclude, ubuf, i);
		if (needed > i) {
		    /* our guess wasn't big enough */
		    free(ubuf);
		    if ((ubuf = (char *)malloc(needed)) == (char *)NULL) {
			play_sound(SOUND_ERROR);
			WinPostMsg(hwnd, WM_COMMAND, (MPARAM)DID_CANCEL, 
			    MPFROM2SHORT(CMDSRC_OTHER, TRUE));
			return (MRESULT)TRUE;	/* no memory */
		    }
		    enum_upp_path(option.gsinclude, ubuf, needed);
		}

		GetUPPname(hwnd, uppname, sizeof(uppname));
		WinSendMsg(WinWindowFromID(hwnd, UPP_LIST), LM_DELETEALL, 
		    (MPARAM)0, (MPARAM)0);
		for (p = ubuf; *p!='\0'; p += strlen(p) + 1) {
		    q = p + strlen(p) + 1;
		    if (strcmp(p, (char *)uppname) == 0)
			desc = q;
		    WinSendMsg( WinWindowFromID(hwnd, UPP_LIST), LM_INSERTITEM, 
			MPFROMLONG(LIT_SORTASCENDING), MPFROMP(q) );
		    p = q;
		}
		if (desc != (LPSTR)NULL) {
		    i = (int)WinSendMsg( WinWindowFromID(hwnd, UPP_LIST),
			LM_SEARCHSTRING, 
			MPFROM2SHORT(LSS_CASESENSITIVE, LIT_FIRST), 
			MPFROMP(desc) );
		    if ((i != LIT_ERROR) && (i != LIT_NONE)) {
			WinSendMsg( WinWindowFromID(hwnd, UPP_LIST),
			    LM_SELECTITEM, MPFROMLONG(i), MPFROMLONG(TRUE) );
		        WinSendMsg(WinWindowFromID(hwnd, UPP_LIST), 
			    LM_SETTOPINDEX, MPFROMLONG(i), (MPARAM)0);
		    }
		}
		WinSetWindowText(WinWindowFromID(hwnd, UPP_NAME), uppname);
		WinEnableWindow(WinWindowFromID(hwnd, UPP_LIST), TRUE);
		}
		return (MRESULT)TRUE;
	    case ID_HELP:
		get_help();
		return (MRESULT)TRUE;
	    case DID_OK:
		if (WinQueryWindowText(WinWindowFromID(hwnd, UPP_NAME), 
			sizeof(uppname)-3, uppname+2)) {
		  uppname[0] = '"';
		  uppname[1] = '@';
		  strcat((char *)uppname, "\042");
		  WinSetWindowText(WinWindowFromID(WinQueryWindow(hwnd, 
		      QW_OWNER), DEVICE_OPTIONS), uppname);
		}
		if (ubuf)
		    free(ubuf);
		WinDismissDlg(hwnd, DID_OK);
            	return (MRESULT)TRUE;
	    case DID_CANCEL:
		if (ubuf)
		    free(ubuf);
		WinDismissDlg(hwnd, DID_CANCEL);
		return (MRESULT)TRUE;
	}
	break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/* dialog box for selecting PostScript prolog/epilog and Ctrl+D */
MRESULT EXPENTRY
AdvPSDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{

  switch (msg) {
    case WM_INITDLG:
	{
	/* for PostScript printers, provide options for sending
	 * Ctrl+D before and after job, and sending a prolog
	 * and epilog file.
	 * These are set using the Advanced button on the Printer
	 * Setup dialog, only enabled for PostScript printer.
	 */
	PROFILE *prf;
	char buf[MAXSTR];
	char section[MAXSTR];
	int prectrld=0;
	int postctrld=0;
	int i = (int)WinSendMsg(
	  WinWindowFromID(WinQueryWindow(hwnd, QW_OWNER), SPOOL_PORT), 
	    LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0);
	  WinSendMsg(WinWindowFromID(WinQueryWindow(hwnd, QW_OWNER), SPOOL_PORT), 
	    LM_QUERYITEMTEXT, MPFROM2SHORT(i, sizeof(section)), 
	    MPFROMP(section));
	  WinSendMsg( WinWindowFromID(hwnd, ADVPS_PROLOG),
	    	EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	  WinSendMsg( WinWindowFromID(hwnd, ADVPS_EPILOG),
	    	EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	  if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
	    profile_read_string(prf, section, "PreCtrlD", "0", buf, 
		sizeof(buf)-2);
	    if (sscanf(buf, "%d", &prectrld) != 1)
		prectrld = 0;
	    WinSendMsg( WinWindowFromID(hwnd, ADVPS_PRECTRLD),
		BM_SETCHECK, MPFROMLONG(prectrld ? 1 : 0), MPFROMLONG(0));
	    profile_read_string(prf, section, "PostCtrlD", "0", buf, 
		sizeof(buf)-2);
	    if (sscanf(buf, "%d", &postctrld) != 1)
		postctrld = 0;
	    WinSendMsg( WinWindowFromID(hwnd, ADVPS_POSTCTRLD),
		BM_SETCHECK, MPFROMLONG(postctrld ? 1 : 0), MPFROMLONG(0));
	    profile_read_string(prf, section, "Prolog", "", buf, 
	       sizeof(buf)-2);
	    WinSetWindowText(WinWindowFromID(hwnd, ADVPS_PROLOG), (PCSZ)buf);
	    profile_read_string(prf, section, "Epilog", "", buf, 
	       sizeof(buf)-2);
	    WinSetWindowText(WinWindowFromID(hwnd, ADVPS_EPILOG), (PCSZ)buf);
	    profile_close(prf);
	  }
	}
	break;
    case WM_COMMAND:
	  switch(LOUSHORT(mp1)) {
	    case ADVPS_PROLOGBROWSE:
		{   char buf[MAXSTR];
		    WinQueryWindowText(WinWindowFromID(hwnd, ADVPS_PROLOG), 
			sizeof(buf)-1, (PBYTE)buf);
		    if (get_filename(buf, FALSE, FILTER_ALL, 
			   0, IDS_TOPICPRINT)) {
			WinSetWindowText(WinWindowFromID(hwnd, 
			    ADVPS_PROLOG), (PCSZ)buf);
		    }
		}
		return (MRESULT)TRUE;
	    case ADVPS_EPILOGBROWSE:
		{   char buf[MAXSTR];
		    WinQueryWindowText(WinWindowFromID(hwnd, ADVPS_EPILOG), 
			sizeof(buf)-1, (PBYTE)buf);
		    if (get_filename(buf, FALSE, FILTER_ALL, 
			   0, IDS_TOPICPRINT)) {
			WinSetWindowText(WinWindowFromID(hwnd, 
			    ADVPS_EPILOG), (PCSZ)buf);
		    }
		}
		return (MRESULT)TRUE;
	    case ID_HELP:
		get_help();
		return (MRESULT)TRUE;
	    case DID_OK:
		{ PROFILE *prf;
		  int i;
		  char buf[MAXSTR];
		  char section[MAXSTR];
		  i = (int)WinSendMsg(
		    WinWindowFromID(WinQueryWindow(hwnd, QW_OWNER), SPOOL_PORT), 
		    LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0);
		  WinSendMsg(WinWindowFromID(WinQueryWindow(hwnd, QW_OWNER), SPOOL_PORT), 
		    LM_QUERYITEMTEXT, MPFROM2SHORT(i, sizeof(section)), 
		    MPFROMP(section));
		  if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		    i = (int)WinSendMsg( WinWindowFromID(hwnd, ADVPS_PRECTRLD),
	    	        BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));
		    profile_write_string(prf, section, "PreCtrlD", 
			i ? "1" : "0");
		    i = (int)WinSendMsg( WinWindowFromID(hwnd, ADVPS_POSTCTRLD),
	    	        BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));
		    profile_write_string(prf, section, "PostCtrlD", 
			i ? "1" : "0");
		    WinQueryWindowText(WinWindowFromID(hwnd, ADVPS_PROLOG), 
			sizeof(buf)-1, (PBYTE)buf);
		    profile_write_string(prf, section, "Prolog", buf);
		    WinQueryWindowText(WinWindowFromID(hwnd, ADVPS_EPILOG), 
			sizeof(buf)-1, (PBYTE)buf);
		    profile_write_string(prf, section, "Epilog", buf);
		    profile_close(prf);
		  }
		}
		WinDismissDlg(hwnd, DID_OK);
            	return (MRESULT)TRUE;
	    case DID_CANCEL:
		WinDismissDlg(hwnd, DID_CANCEL);
		return (MRESULT)TRUE;
	}
	break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

void init_fixed_media(HWND hwnd, int id, int fixed_media)
{
    char buf[MAXSTR];
    // Fill in Page Size combo box
    strcpy(buf, "Variable");
    load_string(IDS_PAGESIZE_VARIABLE, buf, sizeof(buf));
    WinSendMsg( WinWindowFromID(hwnd, id),
	LM_INSERTITEM, MPFROMLONG(LIT_END), MPFROMP(buf) );
    strcpy(buf, "Fixed");
    load_string(IDS_PAGESIZE_FIXED, buf, sizeof(buf));
    WinSendMsg( WinWindowFromID(hwnd, id),
	LM_INSERTITEM, MPFROMLONG(LIT_END), MPFROMP(buf) );
    strcpy(buf, "Shrink");
    load_string(IDS_PAGESIZE_SHRINK, buf, sizeof(buf));
    WinSendMsg( WinWindowFromID(hwnd, id),
	LM_INSERTITEM, MPFROMLONG(LIT_END), MPFROMP(buf) );
    WinSendMsg( WinWindowFromID(hwnd, id),
	LM_SELECTITEM, MPFROMLONG(fixed_media), MPFROMLONG(TRUE) );
}


char *device_queue_list;
int device_queue_index;


/* dialog box for selecting printer device and resolution */
MRESULT EXPENTRY
DeviceDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    char buf[128];
    int idevice;
    int i;
    char *p;
    char *res;
    char *s;
    int numentry;
    char entry[MAXSTR];
    struct prop_item_s *proplist;
    static BOOL bConvert;

    switch (msg) {
      case WM_INITDLG:
	bConvert = (BOOL)mp2;
	WinSendMsg( WinWindowFromID(hwnd, DEVICE_OPTIONS),
	    EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	/* fill in device, resolution list boxes */
	p = get_devices(bConvert);
	res = p;	/* save for free() */
	idevice = -1;
	s = bConvert ? option.convert_device : option.printer_device; 
	if (strlen(s) == 0)
	   idevice = 0;
	for (numentry=0; p!=(char *)NULL && strlen(p)!=0; numentry++) {
	    WinSendMsg( WinWindowFromID(hwnd, DEVICE_NAME),
		LM_INSERTITEM, MPFROMLONG(LIT_END), MPFROMP(p) );
	    if (strcmp(p, s) == 0)
		idevice = numentry;
	    p += strlen(p) + 1;
	}
	if (res)
	    free(res);
	if (idevice < 0)
	    WinSetWindowText(WinWindowFromID(hwnd, DEVICE_NAME), 
		(unsigned char *)(s));
	else 
	    WinSendMsg( WinWindowFromID(hwnd, DEVICE_NAME),
		LM_SELECTITEM, MPFROMLONG(idevice), MPFROMLONG(TRUE) );
	/* force update of DEVICE_RES */
	s = bConvert ? option.convert_resolution : option.printer_resolution;
	WinSendMsg(hwnd, WM_CONTROL, MPFROM2SHORT(DEVICE_NAME, CBN_LBSELECT),
	    MPFROMLONG(WinWindowFromID(hwnd, DEVICE_NAME)));
	i = (int)WinSendMsg( WinWindowFromID(hwnd, DEVICE_RES),
		LM_SEARCHSTRING, MPFROM2SHORT(LSS_CASESENSITIVE, LIT_FIRST),
		MPFROMP(s) );
	if ((i == LIT_ERROR) || (i == LIT_NONE))
	    WinSetWindowText(WinWindowFromID(hwnd, DEVICE_RES), 
		(unsigned char *)(s));
	else
	    WinSendMsg( WinWindowFromID(hwnd, DEVICE_RES),
	    LM_SELECTITEM, MPFROMLONG(i), MPFROMLONG(TRUE) );

	if (!bConvert) {
	    /* fill in queue list box */
	    p = device_queue_list;
	    device_queue_index = 0;
	    i = 0;
	    if (strlen(p)==0) {
		/* no printers, so force Print to File */
		option.print_to_file = TRUE;
		option.print_method = PRINT_GS;
		WinSendMsg( WinWindowFromID(hwnd, SPOOL_TOFILE),
		    BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
		WinEnableWindow(WinWindowFromID(hwnd, SPOOL_TOFILE), FALSE);
		WinEnableWindow(WinWindowFromID(hwnd, SPOOL_PORT), FALSE);
		WinEnableWindow(WinWindowFromID(hwnd, SPOOL_PORTTEXT), FALSE);
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_PSPRINT), FALSE);
	    }
	    while (*p) {
		if ( strcmp(p, option.printer_queue) == 0 )
		    device_queue_index = i;
		p += strlen(p)+1; /* skip to queue comment */
		if (*p) {
		    WinSendMsg( WinWindowFromID(hwnd, SPOOL_PORT),
			LM_INSERTITEM, MPFROMLONG(LIT_END), MPFROMP(p));
		    p += strlen(p)+1;
		}
		i++;
	    }
	    if (option.print_method == PRINT_PS) {
		WinSendMsg( WinWindowFromID(hwnd, DEVICE_PSPRINT),
			BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_NAMETEXT), FALSE);
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_NAME), FALSE);
		WinSendMsg(hwnd, WM_CONTROL, 
		    MPFROM2SHORT(DEVICE_NAME, CBN_LBSELECT),
		    MPFROMLONG(WinWindowFromID(hwnd, DEVICE_NAME)));
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_PROP), FALSE);
		WinEnableWindow(WinWindowFromID(hwnd, SPOOL_TOFILE), FALSE);
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_OPTIONSTEXT), 
		    FALSE);
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_OPTIONS), FALSE);
		WinSendMsg( WinWindowFromID(hwnd, SPOOL_PORT),
		    LM_SELECTITEM, MPFROMLONG(device_queue_index), 
		    MPFROMLONG(TRUE) );
	    }
	    else {
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_ADVPS), FALSE);
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_UNIPRINT), TRUE);
		/* set Print to File check box */
		if (option.print_to_file) {
		    WinSendMsg( WinWindowFromID(hwnd, SPOOL_TOFILE),
			BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
		    WinEnableWindow(WinWindowFromID(hwnd, SPOOL_PORT), FALSE);
		    WinEnableWindow(WinWindowFromID(hwnd, SPOOL_PORTTEXT), 
			FALSE);
		}
		else {
		    WinSendMsg( WinWindowFromID(hwnd, SPOOL_PORT),
			LM_SELECTITEM, MPFROMLONG(device_queue_index), 
			MPFROMLONG(TRUE) );
		}
	    }
	}
	/* fill in page list box */
	if ( (psfile.dsc != (CDSC *)NULL) && (psfile.dsc->page_count != 0)) {
	    psfile.page_list.current = psfile.pagenum-1;
	    psfile.page_list.multiple = TRUE;
	    for (i=0; i< (int)psfile.dsc->page_count; i++)
		psfile.page_list.select[i] = FALSE;
	    psfile.page_list.select[psfile.page_list.current] = TRUE;
	    psfile.page_list.reverse = option.print_reverse;
	    PageMultiDlgProc(hwnd, msg, mp1, mp2);
	}
	else {
	    psfile.page_list.multiple = FALSE;
	    WinEnableWindow(WinWindowFromID(hwnd, PAGE_ALL), FALSE);
	    WinEnableWindow(WinWindowFromID(hwnd, PAGE_ODD), FALSE);
	    WinEnableWindow(WinWindowFromID(hwnd, PAGE_EVEN), FALSE);
	    WinEnableWindow(WinWindowFromID(hwnd, PAGE_REVERSE), FALSE);
	    load_string(IDS_ALL, buf, sizeof(buf));
	    for (p=buf, i=0; *p; p++) {
		if (*p != '~')
		    buf[i++] = *p;
	    }
	    buf[i] = '\0';
	    WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST),
		LM_INSERTITEM, MPFROMLONG(LIT_END), MPFROMP(buf) );
	    WinEnableWindow(WinWindowFromID(hwnd, PAGE_LIST), FALSE);
	}
	init_fixed_media(hwnd, DEVICE_FIXEDMEDIA, option.print_fixed_media);
	break;
      case WM_CONTROL:
	if (mp1 == MPFROM2SHORT(DEVICE_NAME, CBN_LBSELECT)) {
	    idevice = (int)WinSendMsg(WinWindowFromID(hwnd, DEVICE_NAME), 
		LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0);
	    if (idevice == LIT_NONE)
		return FALSE;
	    WinSendMsg(WinWindowFromID(hwnd, DEVICE_NAME), LM_QUERYITEMTEXT,  
		MPFROM2SHORT(idevice, sizeof(entry)), MPFROMP(entry));
	    if ( (proplist = get_properties(entry)) 
		!= (struct prop_item_s *)NULL ) {
		free((char *)proplist);
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_PROP), TRUE);
	    }
	    else
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_PROP), FALSE);
	    /* now look up entry in gvpm.ini */
	    /* and update DEVICE_RES list box */
	    {PROFILE *prf;
	     char section[MAXSTR];
/* need to reopen profile file - this is wasteful */
	      if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		/* update printer options */
		strcpy(section, entry);
		strcat(section, " Options");
		profile_read_string(prf, section, "Options", "", 
			buf, sizeof(buf)-2);
		WinSetWindowText(WinWindowFromID(hwnd, DEVICE_OPTIONS), 
			(PCSZ)buf);
		/* now look up resolutions */
		profile_read_string(prf, 
		    bConvert ? CONVERTSECTION : DEVSECTION, 
		    entry, "", buf, sizeof(buf)-2);
		profile_close(prf);
	      }
	      else
		buf[0] = '\0';
	    }
	    while ((*buf) && (buf[strlen(buf)-1]==' '))
	    buf[strlen(buf)-1] = '\0';    /* remove trailing spaces */
	    buf[strlen(buf)+1] = '\0';	/* double NULL at end */
	    WinSendMsg(WinWindowFromID(hwnd, DEVICE_RES), LM_DELETEALL, 
		(MPARAM)0, (MPARAM)0);
	    p = buf;
	    if (p==(char *)NULL || *p == '\0') {
		/* no resolutions can be set */
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_RES), FALSE);
		WinEnableWindow(WinWindowFromID(hwnd, DEVICE_RESTEXT), FALSE);
	    }
	    else {
	      WinEnableWindow(WinWindowFromID(hwnd, DEVICE_RES), TRUE);
	      WinEnableWindow(WinWindowFromID(hwnd, DEVICE_RESTEXT), TRUE);
	      while (p && *p!='\0') {
		res = p;
		while ((*p!='\0') && (*p!=','))
		    p++;
		*p++ = '\0';
		WinSendMsg( WinWindowFromID(hwnd, DEVICE_RES),
		    LM_INSERTITEM, MPFROMLONG(LIT_END), MPFROMP(res) );
	      }
	    }
	    WinSendMsg( WinWindowFromID(hwnd, DEVICE_RES),
		LM_SELECTITEM, MPFROMLONG(0), MPFROMLONG(TRUE) );
	    if ((int)WinSendMsg(WinWindowFromID(hwnd, DEVICE_RES), 
		LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0)
		    != LIT_NONE)
		WinSetWindowText(WinWindowFromID(hwnd, DEVICE_RES), (PCSZ)buf);
	}
	switch (SHORT2FROMMP(mp1)) {
	    case LN_ENTER:
		if (SHORT1FROMMP(mp1) == PAGE_LIST)
		    WinPostMsg(hwnd, WM_COMMAND, (MPARAM)DID_OK, 
			MPFROM2SHORT(CMDSRC_OTHER, TRUE));
		if (SHORT1FROMMP(mp1) == SPOOL_PORT)
		    WinPostMsg(hwnd, WM_COMMAND, (MPARAM)DID_OK, 
			MPFROM2SHORT(CMDSRC_OTHER, TRUE));
		break;
	    case BN_CLICKED:
	       if (SHORT1FROMMP(mp1) == SPOOL_TOFILE) {
		    i = (int)WinSendMsg( WinWindowFromID(hwnd, SPOOL_TOFILE),
		    BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));
		    /* toggle state */
		    i = (i == 0) ? 1 : 0;
		    WinSendMsg( WinWindowFromID(hwnd, SPOOL_TOFILE),
			BM_SETCHECK, MPFROMLONG(i), MPFROMLONG(0));
		    if (i) {  /* save selection */
			device_queue_index = (int)WinSendMsg(WinWindowFromID(
			   hwnd, SPOOL_PORT), LM_QUERYSELECTION, 
			    MPFROMSHORT(LIT_FIRST), (MPARAM)0);
		    }
		    WinSendMsg( WinWindowFromID(hwnd, SPOOL_PORT),
			LM_SELECTITEM, MPFROMLONG(device_queue_index), 
			MPFROMLONG(i ? FALSE : TRUE));
		    WinEnableWindow(WinWindowFromID(hwnd, SPOOL_PORTTEXT), 
			(i ? FALSE : TRUE) );
		    WinEnableWindow(WinWindowFromID(hwnd, SPOOL_PORT), 
			(i ? FALSE : TRUE) );
		    
	       }
	       else if (SHORT1FROMMP(mp1) == DEVICE_PSPRINT) {
		    i = (int)WinSendMsg( WinWindowFromID(hwnd, DEVICE_PSPRINT),
		    BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));
		    /* toggle state */
		    i = (i == 0) ? 1 : 0;
		    WinSendMsg( WinWindowFromID(hwnd, DEVICE_PSPRINT),
			BM_SETCHECK, MPFROMLONG(i), MPFROMLONG(0));
		    i = !i;
		    WinEnableWindow(WinWindowFromID(hwnd, DEVICE_NAMETEXT), i);
		    WinEnableWindow(WinWindowFromID(hwnd, DEVICE_NAME), i);
		    WinEnableWindow(WinWindowFromID(hwnd, DEVICE_OPTIONSTEXT), i);
		    WinEnableWindow(WinWindowFromID(hwnd, DEVICE_OPTIONS), i);
		    WinEnableWindow(WinWindowFromID(hwnd, DEVICE_UNIPRINT), i);
		    WinEnableWindow(WinWindowFromID(hwnd, DEVICE_ADVPS), !i);
		    if (i)
			WinSendMsg(hwnd, WM_CONTROL, 
			    MPFROM2SHORT(DEVICE_NAME, CBN_LBSELECT),
			    MPFROMLONG(WinWindowFromID(hwnd, DEVICE_NAME)));
		    else 
			WinEnableWindow(WinWindowFromID(hwnd, DEVICE_RESTEXT), 
				i);
			WinEnableWindow(WinWindowFromID(hwnd, DEVICE_RES), i);
		    WinEnableWindow(WinWindowFromID(hwnd, DEVICE_PROP), i);
		    if (!i && (int)WinSendMsg( 
			WinWindowFromID(hwnd, SPOOL_TOFILE),
			BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0)) )
			WinSendMsg(hwnd, WM_CONTROL, MPFROM2SHORT(SPOOL_TOFILE,
			    BN_CLICKED), MPFROMLONG(0));
		    WinEnableWindow(WinWindowFromID(hwnd, SPOOL_TOFILE), i);
		}
	    }
	    return FALSE;
      case WM_COMMAND:
	switch(LOUSHORT(mp1)) {
	    case DID_OK:
		/* save device name and resolution */
		if (bConvert) {
		    WinQueryWindowText(WinWindowFromID(hwnd, DEVICE_NAME), 
			 sizeof(option.convert_device)-1, 
			(PBYTE)(option.convert_device));
		    WinQueryWindowText(WinWindowFromID(hwnd, DEVICE_RES), 
			sizeof(option.convert_resolution)-1, 
			(PBYTE)(option.convert_resolution));
		}
		else {
		    WinQueryWindowText(WinWindowFromID(hwnd, DEVICE_NAME), 
			 sizeof(option.printer_device)-1, 
			(PBYTE)(option.printer_device));
		    WinQueryWindowText(WinWindowFromID(hwnd, DEVICE_RES), 
			sizeof(option.printer_resolution)-1, 
			(PBYTE)(option.printer_resolution));
		    /* get PostScript Printer state */
		    option.print_method = (int)WinSendMsg( 
			WinWindowFromID(hwnd, DEVICE_PSPRINT),
			BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0))
			? PRINT_PS : PRINT_GS;
		    /* get Print to File state */
		    option.print_to_file = (int)WinSendMsg( 
			WinWindowFromID(hwnd, SPOOL_TOFILE),
			BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));
		    if (!option.print_to_file) {
			/* save queue name */
			device_queue_index = 1+(int)WinSendMsg(
			    WinWindowFromID(hwnd, SPOOL_PORT), 
			    LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), 
			    (MPARAM)0);
			p = device_queue_list;
			for (i=2; i<device_queue_index+device_queue_index && 
				  strlen(p)!=0; i++)
			    p += strlen(p)+1;
			strcpy(option.printer_queue, p);
		    }
		}
		/* Fixed Media */
		option.print_fixed_media = (int)WinSendMsg( 
		    WinWindowFromID(hwnd, DEVICE_FIXEDMEDIA),
		    LM_QUERYSELECTION, MPFROMLONG(LIT_FIRST), MPFROMLONG(0));
		/* save pages numbers */
		if ( (psfile.dsc != (CDSC *)NULL) 
		    && (psfile.dsc->page_count != 0)) {
		    PageMultiDlgProc(hwnd, msg, mp1, mp2);
		    option.print_reverse = psfile.page_list.reverse;
		}

		/* get options */
		{PROFILE *prf;
		  char section[MAXSTR];
		  if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
/* need to reopen profile file - this is wasteful */
		    /* update printer options */
		    strcpy(section, bConvert ? option.convert_device :
				option.printer_device);
		    strcat(section, " Options");
		    WinQueryWindowText(WinWindowFromID(hwnd, DEVICE_OPTIONS), 
			sizeof(buf), (PBYTE)buf);
		    profile_write_string(prf, section, "Options", buf);
		    profile_close(prf);
		  }
		}
		WinDismissDlg(hwnd, DID_OK);
		return (MRESULT)TRUE;
	    case ID_HELP:
		get_help();
		return (MRESULT)TRUE;
	    case DEVICE_UNIPRINT:
		WinQueryWindowText(WinWindowFromID(hwnd, DEVICE_NAME), 
		     sizeof(buf), (PBYTE)buf);
		if (strcmp(buf, "uniprint") != 0) {
		  /* select uniprint device */
		    i = (int)WinSendMsg( WinWindowFromID(hwnd, DEVICE_NAME),
			LM_SEARCHSTRING, 
			MPFROM2SHORT(LSS_CASESENSITIVE, LIT_FIRST),
			MPFROMP("uniprint") );
		    if ((i == LIT_ERROR) || (i == LIT_NONE)) {
			play_sound(SOUND_ERROR);
			return FALSE;	/* can't select uniprint */
		    }
		    WinSendMsg( WinWindowFromID(hwnd, DEVICE_NAME),
			LM_SELECTITEM, 
			MPFROMLONG(i), MPFROMLONG(TRUE) );
		    /* force update of DEVICE_RES */
		    WinSendMsg(hwnd, WM_CONTROL, 
			MPFROM2SHORT(DEVICE_NAME, CBN_LBSELECT),
			MPFROMLONG(WinWindowFromID(hwnd, DEVICE_NAME)));
		}
		nHelpTopic = IDS_TOPICPRINT;
		WinDlgBox(HWND_DESKTOP, hwnd, UniDlgProc, hlanguage, 
		    IDD_UNIPRINT, NULL);
		return (MRESULT)TRUE;
	    case DEVICE_ADVPS:
		nHelpTopic = IDS_TOPICPRINT;
		WinDlgBox(HWND_DESKTOP, hwnd, AdvPSDlgProc, hlanguage, 
		    IDD_ADVPS, NULL);
		return (MRESULT)TRUE;
	    case PAGE_ALL:
	    case PAGE_EVEN:
	    case PAGE_ODD:
		return (MRESULT)PageMultiDlgProc(hwnd, msg, mp1, mp2);
	    case DEVICE_PROP:
		idevice = (int)WinSendMsg(WinWindowFromID(hwnd, DEVICE_NAME), 
		    LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), (MPARAM)0);
		if (idevice == LIT_NONE)
		    return (MRESULT)TRUE;
		WinSendMsg(WinWindowFromID(hwnd, DEVICE_NAME), 
		    LM_QUERYITEMTEXT,  MPFROM2SHORT(idevice, sizeof(entry)), 
		    MPFROMP(entry));
		if ( (proplist = get_properties(entry)) != (struct prop_item_s *)NULL ) {
			free((char *)proplist);
			nHelpTopic = IDS_TOPICPROP;
			WinDlgBox(HWND_DESKTOP, hwnd, PropDlgProc, hlanguage, IDD_PROP, entry);
			nHelpTopic = IDS_TOPICPRINT;
		}
		else
		    play_sound(SOUND_ERROR);
		return (MRESULT)TRUE;
	}
	break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

BOOL
get_device(void)
{
int result;
    if ((device_queue_list=(char *)malloc(PRINT_BUF_SIZE)) == (char *)NULL)
	return FALSE;
    get_ports(device_queue_list, PRINT_BUF_SIZE);
    nHelpTopic = IDS_TOPICPRINT;
    result = WinDlgBox(HWND_DESKTOP, hwnd_frame, DeviceDlgProc, 
	hlanguage, IDD_DEVICE, (MPARAM)FALSE);
    free(device_queue_list);
    if (result != DID_OK)
	return FALSE;
    return TRUE;
}

/* print a range of pages using a Ghostscript device */
void
gsview_print(BOOL convert)
{
    /* silent printing not yet implemented */
    int i;
    int flag;
    char command[MAXSTR+MAXSTR];
    char progname[MAXSTR];
    
    if (psfile.name[0] == '\0') {
	gserror(IDS_NOTOPEN, NULL, MB_ICONEXCLAMATION, SOUND_NOTOPEN);
	return;
    }
    

    if (convert) {
	if (print_silent) {
	    if (psfile.dsc != (CDSC *)NULL)
		for (i=0; i< (int)psfile.dsc->page_count; i++)
		    psfile.page_list.select[i] = TRUE;
	}
	else {
	    nHelpTopic = IDS_TOPICCONVERT;
	    if (WinDlgBox(HWND_DESKTOP, hwnd_frame, DeviceDlgProc, 
		    hlanguage, IDD_CONVERT, (MPARAM)TRUE) != DID_OK)
		return;
	}
    }
    else {
	if (print_silent) {
	    /* don't prompt */
	    int i;
	    psfile.print_copies = 1;
	    psfile.print_ignoredsc = FALSE;
	    if ( (psfile.dsc != (CDSC *)NULL) && 
		 (psfile.dsc->page_count != 0)) {
		psfile.page_list.current = 0;
		psfile.page_list.multiple = TRUE;
		for (i=0; i< (int)psfile.dsc->page_count; i++)
		    psfile.page_list.select[i] = TRUE;
		psfile.page_list.reverse = option.print_reverse;
	    }
	}
	else if (!get_device())
	    return;
    }

// PRINT_GDI not yet implemented
    if (option.print_method == PRINT_GDI)
	option.print_method = PRINT_GS;

    if (!gsview_cprint(printer.psname, printer.optname, convert)) {
	if (print_exit && (print_count==0) && !(debug & DEBUG_GDI))
	    gsview_command(IDM_EXIT);
	return;
    }

    info_wait(IDS_WAITPRINT);

    strcpy(progname, "gvpgs.exe");

    sprintf(command,"%s \042%s\042 \042%s\042 \042%s\042", debug ? "/d" : "", 
       option.gsdll, printer.optname, printer.psname);

    if (strlen(command) > MAXSTR-1) {
	    /* command line too long */
	    gserror(IDS_TOOLONG, command, MB_ICONHAND, SOUND_ERROR);
	    if (!debug)
		unlink(printer.psname);
	    printer.psname[0] = '\0';
	    if (!debug)
		unlink(printer.optname);
	    printer.optname[0] = '\0';
	    return;
    }

    flag = exec_pgm(progname, command, &printer.prog);
    if (!flag || !printer.prog.valid) {
	    cleanup_pgm(&printer.prog);
	    gserror(IDS_CANNOTRUN, command, MB_ICONHAND, SOUND_ERROR);
	    if (!debug)
		unlink(printer.psname);
	    printer.psname[0] = '\0';
	    if (!debug)
		unlink(printer.optname);
	    printer.optname[0] = '\0';
	    info_wait(IDS_NOWAIT);
	    return;
    }

    info_wait(IDS_NOWAIT);
    
    if (print_exit && (print_count==0) && !(debug & DEBUG_GDI))
	gsview_command(IDM_EXIT);

    return;
}

