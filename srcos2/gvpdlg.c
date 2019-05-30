/* Copyright (C) 1993-1998, Ghostgum Software Pty Ltd.  All rights reserved.G
  
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

/* gvpdlg.c */
/* Dialog boxes for PM GSview */
#include "gvpm.h"

BOOL get_string_busy;
char get_string_prompt[MAXSTR];
char get_string_answer[MAXSTR];
void listbox_getpath(HWND hwnd, char *path, int listbox);
void listbox_directory(HWND hwnd, char *path, int listbox, int stext);

MRESULT EXPENTRY 
InputDlgProc(HWND hwnd, ULONG mess, MPARAM mp1, MPARAM mp2)
{
    switch(mess) {
	case WM_INITDLG:
	    WinSendMsg( WinWindowFromID(hwnd, ID_ANSWER),
	    	EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	    WinSetWindowText( WinWindowFromID(hwnd, ID_ANSWER),
	    	(PBYTE)get_string_answer );
	    WinSetWindowText( WinWindowFromID(hwnd, ID_PROMPT),
	    	(PBYTE)get_string_prompt );
	    WinSendMsg( WinWindowFromID(hwnd, ID_ANSWER),
	    	EM_SETSEL, MPFROM2SHORT(0, strlen(get_string_answer)), MPFROMLONG(0) );
	    WinSetFocus(HWND_DESKTOP, WinWindowFromID(hwnd, ID_ANSWER));
    	    break;
        case WM_COMMAND:
            switch(SHORT1FROMMP(mp1)) {
                case DID_OK:
                    WinEnableWindow(WinWindowFromID(hwnd, DID_OK), FALSE);
                    WinQueryWindowText(WinWindowFromID(hwnd, ID_ANSWER),
                    	MAXSTR, (PBYTE)get_string_answer);
                    WinDismissDlg(hwnd, DID_OK);
		    return (MRESULT)TRUE;
		case ID_HELP:
		    get_help();
		    return (MRESULT)TRUE;
            }
            break;
    }
    return WinDefDlgProc(hwnd, mess, mp1, mp2);
}

BOOL
query_string(const char *prompt, char *answer)
{
	if (get_string_busy) {
	    message_box("get_string is busy",0);
	    return FALSE;
	}
	get_string_busy = TRUE;
	strncpy(get_string_prompt, prompt, MAXSTR);
	strncpy(get_string_answer, answer, MAXSTR);
	
	if (WinDlgBox(HWND_DESKTOP, hwnd_frame, InputDlgProc, hlanguage, IDD_INPUT, NULL)
	   == DID_OK) {
		strncpy(answer, get_string_answer, MAXSTR);
		get_string_busy = FALSE;
		return TRUE;
	}
	get_string_busy = FALSE;
	return FALSE;
}


MRESULT EXPENTRY 
SubFileDlgProc(HWND hwnd, ULONG mess, MPARAM mp1, MPARAM mp2)
{
static char filter[80];
static char filename[MAXSTR];
static BOOL is_wild;
char *p;
int i;
    switch(mess) {
	case WM_INITDLG:
	    is_wild = FALSE;
	    filename[0] = '\0';
	    filter[0] = '\0';
	    strcpy(filter, (char *)((PFILEDLG)WinQueryWindowULong(hwnd, QWL_USER))->pszIType);
	    for (p=filter; *p; p++) {
	        if (*p == ';')
		    *p = '\0';
	    }
	    p++;
	    *p = '\0';	/* double trailing NULL */
	    break;
    	case WM_CONTROL:
	    if (mp1 == MPFROM2SHORT(DID_FILENAME_ED, EN_CHANGE)) {
		WinQueryWindowText(WinWindowFromID(hwnd, DID_FILENAME_ED),
                    	sizeof(filename), (PBYTE)filename);
		is_wild = (strchr(filename, '*') || strchr(filename, '?'));
	    }
	    if (mp1 == MPFROM2SHORT(DID_FILTER_CB, CBN_LBSELECT)) {
		i = (int)WinSendMsg(WinWindowFromID(hwnd, DID_FILTER_CB), LM_QUERYSELECTION, (MPARAM)0, (MPARAM)0);
		if (i == LIT_NONE)
		    return FALSE;
		WinSendMsg(WinWindowFromID(hwnd, DID_FILTER_CB), LM_QUERYITEMTEXT,  MPFROM2SHORT(i, sizeof(filter)), MPFROMP(filter));
		if (filter[0] == '<')
		    strcpy(filter, "*.*");
		for (p=filter; *p; p++) {
		    if (*p == ';')
			*p = '\0';
		}
		p++;
		*p = '\0';	/* double trailing NULL */
	    }
	    break;
	case FDM_VALIDATE:
	    if ( (((PFILEDLG)WinQueryWindowULong(hwnd, QWL_USER))->fl) & 
		FDS_OPEN_DIALOG ) {
		/* check that file exists */
		FILE *f;
		if ( (f=fopen((char *)mp1, "rb")) != (FILE *)NULL ) {
		    fclose(f);
		    return (MRESULT)TRUE;
		}
		play_sound(SOUND_ERROR);
		return (MRESULT)FALSE;
	    }
	    /* return FALSE if Open dialog box and file doesn't exist */
	    return (MRESULT)TRUE;	/* dismiss dialog */
	case FDM_FILTER:
	    if (is_wild) {
		if (wildmatch(filename, (char *)mp1))
		    return (MRESULT)TRUE;	/* add to list box */
	    }
	    else {
		p = filter;
		while (*p) {
		    if (wildmatch(p, (char *)mp1))
			return (MRESULT)TRUE;	/* add to list box */
		    p += strlen(p) + 1;
		}
	    }
	    return (MRESULT)FALSE;	/* ignore file */
    }
    return WinDefFileDlgProc(hwnd, mess, mp1, mp2);
}

#define MAXFILTER 10
char *file_filter[MAXFILTER+1];
char file_filter_buffer[1024];

BOOL 
get_filename(char *filename, BOOL save, int filter, int title, int help)
{
FILEDLG FileDlg;
FILE *f;
char szTitle[MAXSTR];
char *p;
int i;
	memset(&FileDlg, 0, sizeof(FILEDLG));
	FileDlg.cbSize = sizeof(FILEDLG);
	if (save)
	    FileDlg.fl = FDS_CENTER | FDS_SAVEAS_DIALOG;
	else
	    FileDlg.fl = FDS_CENTER | FDS_OPEN_DIALOG;
	if (help) {
	    nHelpTopic = help;
	    FileDlg.fl |= FDS_HELPBUTTON;
	}
	if (title) {
	    load_string(title, szTitle, sizeof(szTitle));
	    FileDlg.pszTitle = (PBYTE)szTitle;
	}
	if (*filename) {
	    strcpy(FileDlg.szFullFile, filename);
	}
	else {
	    gs_getcwd(FileDlg.szFullFile, sizeof(FileDlg.szFullFile));
	    for (p=FileDlg.szFullFile; *p; p++) {
		if (*p == '/')
		    *p = '\\';
	    }
	    i = strlen(FileDlg.szFullFile);
	    if (i && FileDlg.szFullFile[i-1]!='\\') {
		strcat(FileDlg.szFullFile, "\\");
		i++;
	    }
	}

	load_string(IDS_FILTER_BASE+filter, file_filter_buffer, sizeof(file_filter_buffer));
	i = 0;
	p=strtok(file_filter_buffer,"|");
	FileDlg.pszIType = (PBYTE)p;		/* default is first */
	while (p && (i<MAXFILTER)) {
	    p = strtok(NULL, "|");
	    file_filter[i] = p;
	    i++;
	}
	FileDlg.papszITypeList = (PAPSZ)file_filter;

	/* sub class window to alter "List files of Type" behaviour */
	FileDlg.pfnDlgProc = SubFileDlgProc;

	WinFileDlg(HWND_DESKTOP, hwnd_frame, &FileDlg);
	if (FileDlg.lReturn == DID_OK) {
	    f = (FILE *)NULL;
	    if ( !save && ((f = fopen(FileDlg.szFullFile, "rb")) == (FILE *)NULL) ) {
		gserror(IDS_FILENOTFOUND, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
		return FALSE;
	    }
	    if (f)
	        fclose(f);
	    strncpy(filename, FileDlg.szFullFile, MAXSTR);
	    p = strrchr(FileDlg.szFullFile, '\\');
	    if (p) {
		*p = '\0';
		gs_chdir(FileDlg.szFullFile);
	    }
	    return TRUE;
	}
	return FALSE;
}


/* information about document dialog box */
MRESULT EXPENTRY 
InfoDlgProc(HWND hwnd, ULONG mess, MPARAM mp1, MPARAM mp2)
{
    switch(mess) {
        case WM_INITDLG:
	    info_init(hwnd);
	    break;
        case WM_COMMAND:
            switch(SHORT1FROMMP(mp1)) {
                case DID_OK:
                    WinEnableWindow(WinWindowFromID(hwnd, DID_OK), FALSE);
                    WinQueryWindowText(WinWindowFromID(hwnd, ID_ANSWER),
                    	MAXSTR, (PBYTE)get_string_answer);
                    WinDismissDlg(hwnd, DID_OK);
		    return (MRESULT)TRUE;
            }
            break;
    }
    return WinDefDlgProc(hwnd, mess, mp1, mp2);
}


/* show info about ps file */
void
show_info()
{
	WinDlgBox(HWND_DESKTOP, hwnd_frame, InfoDlgProc, hlanguage, IDD_INFO, NULL);
}


/* About Dialog Box */
MRESULT EXPENTRY AboutDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  switch(msg) {
    case WM_INITDLG:
/*
	WinSetWindowText( WinWindowFromID(hwnd, ABOUT_VERSION),
	    	GSVIEW_DOT_VERSION );
*/
	WinSetWindowText( WinWindowFromID(hwnd, NAG_NAME),
	    (PBYTE)registration_name );
	if (registration_receipt != 0) {
	    char buf[16];
	    sprintf(buf, "%u", registration_receipt);
	    WinSetWindowText( WinWindowFromID(hwnd, NAG_RECEIPT),
		(PBYTE)buf );
	}
	break;
    case WM_BUTTON1DBLCLK:
	{POINTL pt;
	RECTL rect;
	HPS hps;
	HBITMAP hbm;
	rect.xLeft = 8; rect.xRight = rect.xLeft+216;
	rect.yBottom = 28 /* 28 */; rect.yTop = rect.yBottom + 10;
	pt.x = SHORT1FROMMP(mp1);  pt.y = SHORT2FROMMP(mp1);
	WinMapDlgPoints(hwnd, &pt, 1, FALSE);
	if (WinPtInRect(hab, &rect, &pt)) {
	    hps = WinGetPS(hwnd);
	    hbm = GpiLoadBitmap(hps, NULLHANDLE, IDM_MISC, 64, 64);
	    pt.x = 250; pt.y = 12;
	    WinMapDlgPoints(hwnd, &pt, 1, TRUE);
	    WinDrawBitmap(hps, hbm, NULL, &pt, CLR_BLACK, CLR_WHITE, DBM_NORMAL);
	    GpiDeleteBitmap(hbm);
	    WinReleasePS(hps);
	}
	}
	break;
    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1)) {
        case DID_OK:
          WinDismissDlg(hwnd, TRUE);
          return (MRESULT)TRUE;
      }
      break;
  }
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
}	

void
show_about()
{
		WinDlgBox(HWND_DESKTOP, hwnd_frame, AboutDlgProc, hlanguage, IDD_ABOUT, 0);
}

/* Return TRUE if a contiguous block of pages (>2) is found */
/* Store start and end of this range in first and last */
/* If not contiguous, store entire page range in first and last */
BOOL
contiguous_range(HWND hwnd, int *first, int *last)
{
int i;
BOOL gap = FALSE;	/* TRUE is gap found after block */
BOOL contiguous = TRUE;
int block = 0;		/* number of pages set in a contiguous block */
BOOL selected;
    for (i=0; i<(int)psfile.dsc->page_count; i++) {
	selected = (i == (int)WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
		LM_QUERYSELECTION, MPFROMSHORT(i-1), (MPARAM)0));
	if (selected && contiguous) {
	    if (gap)
		contiguous = FALSE;
	    else {
		if (block == 0)
		    *first = i;
		else
		    *last = i+1;
		block++;
	    }
	}
	else if (!selected && block)
	    gap = TRUE;
    }
    if (block < 2)
	contiguous = FALSE;
    if (!contiguous) {
	*first = 0;
	*last = psfile.dsc->page_count;
    }
    return contiguous;
}

MRESULT EXPENTRY PageDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
int i;
int notify_message;
static BOOL ecdisable;
    switch(msg) {
    case  WM_INITDLG:
	{char buf[MAXSTR];
	for (i=0; i<(int)psfile.dsc->page_count; i++) {
	    page_ordlabel(buf, i);
	    WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST),
	    	LM_INSERTITEM, MPFROMLONG(LIT_END), 
		MPFROMP(buf) );
	}
	WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST),
	    	LM_SELECTITEM, MPFROMLONG(psfile.page_list.current), MPFROMLONG(TRUE) );
/*
	if (psfile.page_list.current > 5)
	    WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST),
		LM_SETTOPINDEX, MPFROMLONG(psfile.page_list.current - 5), (MPARAM)0 );
*/
	WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST), LM_QUERYITEMTEXT, 
		MPFROM2SHORT(psfile.page_list.current, sizeof(buf)), 
		MPFROMP(buf) );
	ecdisable = TRUE;
	WinSetWindowText( WinWindowFromID(hwnd, PAGE_EDIT), (PBYTE)buf);
	ecdisable = FALSE;
	WinSendMsg( WinWindowFromID(hwnd, PAGE_EDIT), EM_SETSEL, 
		MPFROM2SHORT(0, 255), (MPARAM)0);
	}
	break;
    case WM_CONTROL:
	notify_message = SHORT2FROMMP(mp1);
	if (SHORT1FROMMP(mp1) == PAGE_LIST)
	  switch (notify_message) {
	    case LN_ENTER:
	        if (SHORT1FROMMP(mp1) == PAGE_LIST)
		    WinPostMsg(hwnd, WM_COMMAND, (MPARAM)DID_OK, MPFROM2SHORT(CMDSRC_OTHER, TRUE));
		break;
	    case LN_SELECT:
		i = (int)WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
		    LM_QUERYSELECTION, (MPARAM)0, (MPARAM)0);
		if (i != LIT_NONE) {
		    char buf[MAXSTR];
		    WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST), LM_QUERYITEMTEXT, 
			MPFROM2SHORT(i, sizeof(buf)), MPFROMP(buf) );
		    /* Update edit field, but stop edit field from
		     * from altering list box selection */
		    ecdisable = TRUE;
		    WinSetWindowText( WinWindowFromID(hwnd, PAGE_EDIT), (PBYTE)buf);
		    ecdisable = FALSE;
		}
		break;
	}
	else if ( !ecdisable && (SHORT1FROMMP(mp1) == PAGE_EDIT)
	   && (notify_message = EN_CHANGE) ) {
	    char buf[MAXSTR];
	    WinQueryWindowText(WinWindowFromID(hwnd, PAGE_EDIT), 
		sizeof(buf), (PBYTE)buf);
	    i = (int)WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST), LM_SEARCHSTRING, 
		MPFROM2SHORT(LSS_CASESENSITIVE, LIT_FIRST),
		MPFROMP(buf) );
	    if (i != LIT_NONE) {
		WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST),
		    LM_SELECTITEM, MPFROMLONG(i), MPFROMLONG(TRUE) );
/*
		if (i > 5) 
		    WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST),
			LM_SETTOPINDEX, MPFROMLONG(i - 5), (MPARAM)0 );
*/
	    }
	}
	break;
    case  WM_COMMAND:
      switch(LOUSHORT(mp1)) {
        case DID_OK:
	    i = (int)WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
		LM_QUERYSELECTION, (MPARAM)0, (MPARAM)0);
	    if (i == LIT_NONE) {
                WinDismissDlg(hwnd, DID_CANCEL);
	    }
	    psfile.page_list.current = i;
	    for (i=0; i<(int)psfile.dsc->page_count; i++) {
	        psfile.page_list.select[i] = 0;
	    }
	    psfile.page_list.select[psfile.page_list.current] = TRUE;
            WinDismissDlg(hwnd, DID_OK);
            return (MRESULT)TRUE;
	case DID_CANCEL:
	    WinDismissDlg(hwnd, DID_CANCEL);
	    return (MRESULT)TRUE;
	case ID_HELP:
	    get_help();
	    return (MRESULT)TRUE;
      }
      break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

MRESULT EXPENTRY PageMultiDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
int i;
int notify_message;
    switch(msg) {
    case  WM_INITDLG:
	{char buf[MAXSTR];
	if (psfile.page_list.reverse)
	    WinSendMsg( WinWindowFromID(hwnd, PAGE_REVERSE),
		BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
	for (i=0; i<(int)psfile.dsc->page_count; i++) {
	    page_ordlabel(buf, i);
	    WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST),
	    	LM_INSERTITEM, MPFROMLONG(LIT_END), 
		MPFROMP(buf) );
	}
	for (i=0; i<(int)psfile.dsc->page_count; i++) {
	    if (psfile.page_list.select[i])
	        WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST), LM_SELECTITEM, 
		    MPFROMLONG(i), MPFROMLONG(TRUE) );
	}
	WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST),
	    	LM_SELECTITEM, MPFROMLONG(psfile.page_list.current), MPFROMLONG(TRUE) );
	if (psfile.page_list.current > 5)
	    WinSendMsg( WinWindowFromID(hwnd, PAGE_LIST),
		LM_SETTOPINDEX, MPFROMLONG(psfile.page_list.current - 5), (MPARAM)0 );
	if (!psfile.page_list.multiple) {
		WinEnableWindow(WinWindowFromID(hwnd, PAGE_ALL), FALSE);
		WinEnableWindow(WinWindowFromID(hwnd, PAGE_ODD), FALSE);
		WinEnableWindow(WinWindowFromID(hwnd, PAGE_EVEN), FALSE);
	}
	}
	break;
    case WM_CONTROL:
	notify_message = SHORT2FROMMP(mp1);
	switch (notify_message) {
	    case LN_ENTER:
	        if (SHORT1FROMMP(mp1) == PAGE_LIST)
		    WinPostMsg(hwnd, WM_COMMAND, (MPARAM)DID_OK, MPFROM2SHORT(CMDSRC_OTHER, TRUE));
		break;
	}
	break;
    case  WM_COMMAND:
      switch(LOUSHORT(mp1)) {
	case DID_CANCEL:
	    WinDismissDlg(hwnd, DID_CANCEL);
	    return (MRESULT)TRUE;
        case DID_OK:
	    psfile.page_list.reverse = 
		(int)WinSendMsg( WinWindowFromID(hwnd, PAGE_REVERSE), 
		    BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));
	    i = (int)WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
		LM_QUERYSELECTION, (MPARAM)0, (MPARAM)0);
	    psfile.page_list.current = (i == LIT_NONE) ? 0 : i;
	    for (i=0; i<(int)psfile.dsc->page_count; i++) {
	        psfile.page_list.select[i] = 0;
	    }
	    if (psfile.page_list.multiple) {
	        i = LIT_FIRST;
	        while ( (i = (int)WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
			LM_QUERYSELECTION, (MPARAM)i, (MPARAM)0))
		    != LIT_NONE )
		    psfile.page_list.select[i] = TRUE;
	    }
	    else
		psfile.page_list.select[psfile.page_list.current] = TRUE;
            WinDismissDlg(hwnd, DID_OK);
            return (MRESULT)TRUE;
	case PAGE_ALL:
	    WinEnableWindowUpdate(WinWindowFromID(hwnd, PAGE_LIST), FALSE);
	    for (i=(int)WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
		LM_QUERYITEMCOUNT, (MPARAM)0, (MPARAM)0)-1; i>=0; i--)
		WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
		    LM_SELECTITEM, (MPARAM)i, (MPARAM)TRUE);
	    WinEnableWindowUpdate(WinWindowFromID(hwnd, PAGE_LIST), TRUE);
            return (MRESULT)TRUE;
	case PAGE_ODD:
	    WinEnableWindowUpdate(WinWindowFromID(hwnd, PAGE_LIST), FALSE);
	    { int first, last;
		contiguous_range(hwnd, &first, &last);
		for (i=(int)WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
		    LM_QUERYITEMCOUNT, (MPARAM)0, (MPARAM)0)-1; i>=0; i--)
		    if (i >= first && i < last)
		        WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
			    LM_SELECTITEM, (MPARAM)i, (MPARAM)!(i&1));
		    WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
			LM_SETTOPINDEX, MPFROMSHORT(first), (MPARAM)0);
	    }
	    WinEnableWindowUpdate(WinWindowFromID(hwnd, PAGE_LIST), TRUE);
            return (MRESULT)TRUE;
	case PAGE_EVEN:
	    WinEnableWindowUpdate(WinWindowFromID(hwnd, PAGE_LIST), FALSE);
	    { int first, last;
		contiguous_range(hwnd, &first, &last);
		for (i=(int)WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
		    LM_QUERYITEMCOUNT, (MPARAM)0, (MPARAM)0)-1; i>=0; i--)
		    if (i >= first && i < last)
			WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
			    LM_SELECTITEM, (MPARAM)i, (MPARAM)(i&1));
		    WinSendMsg(WinWindowFromID(hwnd, PAGE_LIST), 
			LM_SETTOPINDEX, MPFROMSHORT(first), (MPARAM)0);
	    }
	    WinEnableWindowUpdate(WinWindowFromID(hwnd, PAGE_LIST), TRUE);
            return (MRESULT)TRUE;
	case ID_HELP:
	    get_help();
	    return (MRESULT)TRUE;
      }
      break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/* Get page number from dialog box and store in ppage */
/* multiple is TRUE if multiple pages may be selected */
/* allpages is TRUE if all pages should be initially selected */
BOOL
get_page(int *ppage, BOOL multiple, BOOL allpages)
{
int flag;
int i;
	if (psfile.dsc == (CDSC *)NULL)
		return FALSE;
	if (psfile.dsc->page_count == 0) {
		gserror(IDS_NOPAGE, NULL, MB_ICONEXCLAMATION, SOUND_NONUMBER);
		return FALSE;
	}
	/* Make psfile.page_list.reverse sticky */
        /* psfile.page_list.reverse = FALSE; */
	psfile.page_list.current = *ppage - 1;
	psfile.page_list.multiple = multiple;
	if (psfile.page_list.select == (BOOL *)NULL)
		return FALSE;

	memset(psfile.page_list.select, 0, psfile.dsc->page_count * sizeof(BOOL) );
	if (multiple && allpages) {
	    for (i=0; i< (int)psfile.dsc->page_count; i++)
		psfile.page_list.select[i] = TRUE;
	}
	else
		psfile.page_list.select[psfile.page_list.current] = TRUE;

	if (psfile.page_list.multiple)
	    flag = WinDlgBox(HWND_DESKTOP, hwnd_frame, PageMultiDlgProc, hlanguage, IDD_MULTIPAGE, NULL);
	else
	    flag = WinDlgBox(HWND_DESKTOP, hwnd_frame, PageDlgProc, hlanguage, IDD_PAGE, NULL);
	if ((flag == DID_OK) && (psfile.page_list.current >= 0))
		*ppage = psfile.page_list.current + 1;
	return (flag == DID_OK);
}

MRESULT EXPENTRY BoundingBoxDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static int bboxindex;
float x, y;
char buf[MAXSTR];
    switch(msg) {
	case WM_INITDLG:
	    bboxindex = 0;
	    load_string(IDS_BBPROMPT, buf, sizeof(buf));
	    WinSetDlgItemText(hwnd, BB_PROMPT, (PBYTE)buf);
	    break;
	case WM_COMMAND:
            switch(SHORT1FROMMP(mp1)) {
		case BB_CLICK:
		    if (!get_cursorpos(&x, &y)) {
			WinDestroyWindow(hwnd);
			hwnd_modeless = 0;
		    }
		    switch(bboxindex) {
			case 0:
			    bbox.llx = (int)x;
			    break;
			case 1:
			    bbox.lly = (int)y;
			    break;
			case 2:
			    bbox.urx = (int)x;
			    break;
			case 3:
			    bbox.ury = (int)y;
			    bbox.valid = TRUE;
			    break;
		    }
		    bboxindex++;
		    if (bboxindex <= 3) {
	    		load_string(IDS_BBPROMPT+bboxindex, buf, sizeof(buf));
	    	        WinSetDlgItemText(hwnd, BB_PROMPT, (PBYTE)buf);
			WinSetFocus(HWND_DESKTOP, hwnd_modeless);
			return (MRESULT)FALSE;
		    }
		    /* all corners have been obtained so close dialog box */
		    WinDestroyWindow(hwnd);
		    hwnd_modeless = 0;
		    return (MRESULT)TRUE;
	    }
	case WM_CLOSE:
	    WinDestroyWindow(hwnd);
	    hwnd_modeless = 0;
	    return (MRESULT)TRUE;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

BOOL
get_bbox(void)
{
QMSG q_mess;		/* queue message */
	bbox.valid = FALSE;
	bbox.llx = bbox.lly = bbox.urx = bbox.ury = 0;
	if ((gsdll.state != GS_PAGE) && (gsdll.state != GS_IDLE)) {
	    gserror(IDS_EPSNOBBOX, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	    return FALSE;
	}
	hwnd_modeless = WinLoadDlg(HWND_DESKTOP, hwnd_frame, BoundingBoxDlgProc, hlanguage, IDD_BBOX, NULL);
	WinSetWindowPos(hwnd_modeless, HWND_TOP, 0, 0, 0, 0, SWP_ZORDER | SWP_ACTIVATE);
	while (hwnd_modeless) {
	    /* wait for bounding box to be obtained */
  	    if (WinGetMsg(hab, &q_mess, 0L, 0, 0))
	        WinDispatchMsg(hab, &q_mess);
	}
	return bbox.valid;
}

/* dialog box for warning PSTOEPS warning and auto/manual bbox selection */
MRESULT EXPENTRY
PSTOEPSDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
    switch (msg) {
	case WM_INITDLG:
		if (option.auto_bbox)
		    WinSendMsg( WinWindowFromID(hwnd, PSTOEPS_AUTOBBOX),
			BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
		break;
	case WM_COMMAND:
            switch(SHORT1FROMMP(mp1)) {
		case ID_HELP:
		    nHelpTopic = IDS_TOPICPSTOEPS;
		    get_help();
		    return (MRESULT)TRUE;
                case DID_OK:
                case IDYES:
		    /* get Print to File status */
		    option.auto_bbox = (int)WinSendMsg( WinWindowFromID(hwnd, PSTOEPS_AUTOBBOX), BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));
                    WinDismissDlg(hwnd, IDYES);
		    return (MRESULT)TRUE;
		case IDNO:
		    nHelpTopic = IDS_TOPICPSTOEPS;
		    get_help();
		    WinDismissDlg(hwnd, IDNO);
		    return (MRESULT)TRUE;
		case DID_CANCEL:
		    WinDismissDlg(hwnd, DID_CANCEL);
		    return (MRESULT)TRUE;
	    }
	    break;
    }
    return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

BOOL
pstoeps_warn(void)
{
    return (WinDlgBox(HWND_DESKTOP, hwnd_frame, PSTOEPSDlgProc, hlanguage, IDD_PSTOEPS, NULL) == IDYES);
}

/* sounds stuff */

#define MAX_SYSTEM_SOUND 16
char *system_sounds;
char *sound_entry[MAX_SYSTEM_SOUND];
char szNone[32];
char szSpeaker[32];
int system_num;
MRESULT EXPENTRY SoundDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

int
load_sounds(void)
{
char *p;
int j;
HINI hini;
	/* add our two sounds */
	sound_entry[0] = "";
	sound_entry[1] = BEEP;
	load_string(IDS_NONE, szNone, sizeof(szNone));
	load_string(IDS_SPKR, szSpeaker, sizeof(szSpeaker));
	/* get list of system sounds */
	system_sounds = (char *)malloc(PROFILE_SIZE);
	memset(system_sounds, 0, PROFILE_SIZE);
	if (system_sounds != (char *)NULL) {
 	    if ( (hini = PrfOpenProfile(hab, szMMini)) != NULLHANDLE ) {
	        PrfQueryProfileString(hini, (PCSZ)"MMPM2_AlarmSounds", NULL, (PCSZ)"\0\0", system_sounds, PROFILE_SIZE-1);
	        PrfCloseProfile(hini);
	    }
	}
	p = system_sounds;
	for (j=2; p!=(char *)NULL && j<MAX_SYSTEM_SOUND && strlen(p)!=0; j++) {
	    if (atoi(p) > 100)
	        p += strlen(p) + 1;	/* ignore <Try it> */
	    if (strlen(p)==0)
		j--;
	    else {
	        sound_entry[j] = p;	
	        p += strlen(p) + 1;
	    }
	}
	system_num = j;
	return system_num;
}

char *
get_sound_entry(int index)
{
	return (sound_entry[index]);
}

char *
get_sound_name(int index)
{
static char buf[64];
char *p, *q;
HINI hini;
	if (index==0)
		return szNone;
	if (index==1)
		return szSpeaker;
	/* get human readable name for system sound */
	if ( (hini = PrfOpenProfile(hab, szMMini)) == NULLHANDLE )
	    return "";
	PrfQueryProfileString(hini, (PCSZ)"MMPM2_AlarmSounds", (PCSZ)sound_entry[index], (PCSZ)"##", buf, sizeof(buf));
	PrfCloseProfile(hini);
    	p = strchr(buf,'#');
    	if (p != (char *)NULL) {
    	    q = strchr(++p,'#');
	    if (q != (char *)NULL) {
	        *q = '\0';
		return p;
	    }
	}
	return "";
}

int 
find_sound_name(char *entry)
{
int i;
	for (i=0; i<system_num; i++) {
	    if (strcmp(entry, get_sound_entry(i))==0)
	        return i;
	}
	return -1;	/* no find */
}

void
add_sounds(HWND hwnd)
{
int ifile;
	for (ifile=system_num-1; ifile>=0; ifile--)
	    WinSendDlgItemMsg(hwnd, SOUND_FILE, LM_INSERTITEM, MPFROMSHORT(0),
	         MPFROMP(get_sound_name(ifile)));
}

void
free_sounds(void)
{
	if (system_sounds != (char *)NULL)
		free(system_sounds);
}

void
change_sounds(void)
{
	nHelpTopic = IDS_TOPICSOUND;
	WinDlgBox(HWND_DESKTOP, hwnd_frame, SoundDlgProc, hlanguage, IDD_SOUND, NULL);
}

MRESULT EXPENTRY SoundDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
	char buf[MAXSTR];
	int ievent, ifile;
	static struct sound_s dsound[NUMSOUND];	/* copy of sound[] */
	char *szWaveFilter = "*.wav";
	static char szPath[MAXSTR];
	static int file_start;
	char *p;

	switch (msg) {
	    case WM_INITDLG:
		file_start = load_sounds();
		for (ievent=0; ievent<NUMSOUND; ievent++) {
		    strcpy(dsound[ievent].file, sound[ievent].file);
		    load_string(sound[ievent].title, buf, sizeof(buf));
		    WinSendDlgItemMsg(hwnd, SOUND_EVENT, LM_INSERTITEM, 
			MPFROMLONG(LIT_END), MPFROMP(buf));
		}
		ievent = 0;
		WinSendDlgItemMsg(hwnd, SOUND_EVENT, LM_SELECTITEM, MPFROMLONG(ievent), MPFROMLONG(TRUE));
		/* force update of SOUND_FILE */
		WinSendMsg(WinWindowFromID(hwnd, SOUND_EVENT), WM_CONTROL,
			MPFROM2SHORT(SOUND_EVENT, LN_SELECT),  MPFROMLONG(0));
		break;
	    case WM_CONTROL:
		if (mp1 == MPFROM2SHORT(SOUND_EVENT, LN_SELECT)) {
			ievent = (int)WinSendDlgItemMsg(hwnd, SOUND_EVENT, LM_QUERYSELECTION,
				MPFROMSHORT(LIT_FIRST), MPFROMLONG(0));
			if (ievent == LIT_NONE) {
			    WinEnableWindow(WinWindowFromID(hwnd, SOUND_TEST), FALSE);
			    break;
			}
			ifile = find_sound_name(dsound[ievent].file);
			if (ifile >= 0) {
			    strcpy(buf, get_sound_name(ifile));
			    szPath[0] = '\0';
			    WinEnableWindow(WinWindowFromID(hwnd, SOUND_TEST), ifile!=0);
			}
			else {
			    /* must be WAVE file */
			    strcpy(szPath, dsound[ievent].file);
			    p = strrchr(szPath, '\\');
			    if (p != (char *)NULL) {
			        strcpy(buf,++p);
			        *p = '\0';
			    }
			    else {
			        strcpy(buf, szPath);
			    }
			    WinEnableWindow(WinWindowFromID(hwnd, SOUND_TEST), TRUE);
			}
			strcat(szPath, szWaveFilter);
			WinEnableWindowUpdate(WinWindowFromID(hwnd, SOUND_FILE), FALSE);
			WinSendDlgItemMsg(hwnd, SOUND_FILE, LM_DELETEALL, MPFROMLONG(0), MPFROMLONG(0));
			listbox_directory(hwnd, szPath, SOUND_FILE, SOUND_PATH);
			add_sounds(hwnd);
			WinEnableWindowUpdate(WinWindowFromID(hwnd, SOUND_FILE), TRUE);
			WinShowWindow(WinWindowFromID(hwnd, SOUND_FILE), TRUE);
			ifile = (int)WinSendDlgItemMsg(hwnd, SOUND_FILE, LM_SEARCHSTRING, 
				MPFROM2SHORT(0,LIT_FIRST), MPFROMP(buf));
			if (ifile != LIT_NONE)
			    WinSendDlgItemMsg(hwnd, SOUND_FILE, LM_SELECTITEM, MPFROMLONG(ifile), MPFROMLONG(TRUE));
			else
			    WinSendDlgItemMsg(hwnd, SOUND_FILE, LM_SELECTITEM, MPFROMLONG(0), MPFROMLONG(TRUE));
		}
		if (mp1 == MPFROM2SHORT(SOUND_FILE, LN_SELECT)) {
		    ifile = (int)WinSendDlgItemMsg(hwnd, SOUND_FILE, LM_QUERYSELECTION,
			MPFROMSHORT(LIT_FIRST), MPFROMLONG(0));
		    WinSendDlgItemMsg(hwnd, SOUND_FILE, LM_QUERYITEMTEXT, 
			MPFROM2SHORT(ifile,sizeof(buf)), MPFROMP(buf));
		    ievent = (int)WinSendDlgItemMsg(hwnd, SOUND_EVENT, LM_QUERYSELECTION,
			MPFROMSHORT(LIT_FIRST), MPFROMLONG(0));
		    if (ifile >= file_start) {
			if (buf[0] == '[') { /* selected a directory */
			    WinEnableWindow(WinWindowFromID(hwnd, SOUND_TEST), FALSE);
			}
			else { /* selected a WAVE file */
			    int i = WinQueryDlgItemText(hwnd, SOUND_PATH, MAXSTR, (PBYTE)dsound[ievent].file);
			    if (dsound[ievent].file[i-1] != '\\') {
				dsound[ievent].file[i++] = '\\';
				dsound[ievent].file[i] = '\0';
			    }
			    listbox_getpath(hwnd, dsound[ievent].file + i, SOUND_FILE);
			    WinEnableWindow(WinWindowFromID(hwnd, SOUND_TEST), TRUE);
			}
		    }
		    else {
			WinEnableWindow(WinWindowFromID(hwnd, SOUND_TEST), ifile!=0);
			strcpy(dsound[ievent].file,get_sound_entry(ifile));
		    }
		}
		if (mp1 == MPFROM2SHORT(SOUND_FILE, LN_ENTER)) {
		    ifile = (int)WinSendDlgItemMsg(hwnd, SOUND_FILE, LM_QUERYSELECTION,
			MPFROMSHORT(LIT_FIRST), MPFROMLONG(0));
		    WinSendDlgItemMsg(hwnd, SOUND_FILE, LM_QUERYITEMTEXT, 
			MPFROM2SHORT(ifile,sizeof(buf)), MPFROMP(buf));
		    if (buf[0] == '[') {
		        WinQueryDlgItemText(hwnd, SOUND_PATH, sizeof(szPath), (PBYTE)szPath);
			listbox_getpath(hwnd, szPath, SOUND_FILE);
			strcat(szPath, szWaveFilter);
			listbox_directory(hwnd, szPath, SOUND_FILE, SOUND_PATH);
			add_sounds(hwnd);
		    }
		    else {
		        WinSendMsg(hwnd, WM_COMMAND, MPFROMSHORT(SOUND_TEST), MPFROMLONG(0));
		    }
		}
		break;
	    case WM_COMMAND:
		switch (SHORT1FROMMP(mp1)) {
		    case ID_HELP:
			get_help();
		        return (MRESULT)FALSE;
		    case SOUND_EVENT:
			return (MRESULT)FALSE;
		    case SOUND_FILE:
			return (MRESULT)FALSE;
		    case SOUND_TEST:
		        ievent = (int)WinSendDlgItemMsg(hwnd, SOUND_EVENT, LM_QUERYSELECTION,
			    MPFROMSHORT(LIT_FIRST), MPFROMLONG(0));
			if (strlen(dsound[ievent].file)==0)
			    return FALSE;
			if (!(pfnMciPlayFile) || strcmp(dsound[ievent].file,BEEP)==0) {
			    DosBeep(200,200);
			    return FALSE;
			}
			if (isdigit((int)(*dsound[ievent].file)))
			    play_system_sound(dsound[ievent].file);
			else {
			    buf[0] = '\042';
			    strcpy(buf+1, dsound[ievent].file);
			    strcat(buf, "\042");
			    if ((*pfnMciPlayFile)(hwnd_frame, (PBYTE)buf, 0, 0, 0))
			        DosBeep(200,200);
			}
			return (MRESULT)FALSE;
		    case IDOK:
			for (ievent=0; ievent<NUMSOUND; ievent++)
				strcpy(sound[ievent].file, dsound[ievent].file);
			free_sounds();
			WinDismissDlg(hwnd, DID_OK);
			return (MRESULT)TRUE;
		    case DID_CANCEL:
			free_sounds();
			WinDismissDlg(hwnd, DID_CANCEL);
			return (MRESULT)TRUE;
		}
		break;
	}
    	return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/* update path with current selection */
/* returned path has a trailing \ if it is a directory */
void
listbox_getpath(HWND hwnd, char *path, int listbox)
{
int i;
char buf[MAXSTR];
char *p;
ULONG len;
	i = (int)WinSendDlgItemMsg(hwnd, listbox, LM_QUERYSELECTION,
	    MPFROMSHORT(LIT_FIRST), MPFROMLONG(0));
	WinSendDlgItemMsg(hwnd, listbox, LM_QUERYITEMTEXT, 
	    MPFROM2SHORT(i,sizeof(buf)), MPFROMP(buf));
	if (buf[0] == '[') {
	    /* directory or drive */
	    if (buf[1] == '-') { /* drive */
		len = MAXSTR;
		path[0] = toupper(buf[2]);
		path[1] = ':';
		path[2] = '\\';
		DosQueryCurrentDir(buf[2]+1-'a', (PBYTE)path+3, &len);
	    }
	    else {
		if (strcmp(buf, "[..]")==0) {
		    if ((p = strrchr(path,'\\')) != (char *)NULL)
		        *(++p)='\0';
		}
		else {
	            if (path[strlen(path)-1] != '\\')
		        strcat(path,"\\");
		    if ((p = strchr(buf, ']')) != (char *)NULL)
		        *p = '\0';
		    strcat(path, buf+1);
		    strcat(path, "\\");
		}
	    }
	}
	else {
	    if (path[strlen(path)-1] != '\\')
		strcat(path,"\\");
	    strcat(path, buf);
	}
}

/* empty list box, then fill it with files and subdirectories given */
/* by path and stext */
/* if path doesn't contain directory, use directory in stext */
/* update stext to point to directory */
void
listbox_directory(HWND hwnd, char *path, int listbox, int stext)
{
APIRET rc;
FILEFINDBUF3 findbuf;
HDIR hdir;
ULONG cFilenames = 1;
char *p;
char dpath[MAXSTR];
char spath[MAXSTR];
int i;
ULONG drivenum, drivemap;
	if (strrchr(path, '\\') == (char *)NULL) {
	    WinQueryWindowText(WinWindowFromID(hwnd, stext), MAXSTR, (PBYTE)dpath);
	    if (strlen(dpath)==0)
		strcpy(dpath, szExePath);
	    strcpy(spath, dpath);
	    if (spath[strlen(spath)-1] != '\\')
		strcat(spath,"\\");
	    strcat(spath, path);
	}
	else {
	    strcpy(spath, path);
	    strcpy(dpath, path);
	    p = strrchr(dpath,'\\');
	    *(++p)='\0';
	    if (strlen(dpath) > 3)
	        *(--p)='\0';
	}
	if ( (strlen(dpath) != 3) &&(dpath[strlen(dpath)-1] == '\\') )
	        dpath[strlen(dpath)-1] = '\0';
	WinSetWindowText(WinWindowFromID(hwnd, stext), (PCSZ)dpath);
	strcat(dpath, (strlen(dpath) > 3) ? "\\*" : "*");

	/* stuff in filenames */
	WinSendDlgItemMsg(hwnd, listbox, LM_DELETEALL, MPFROMLONG(0), MPFROMLONG(0));
	hdir = HDIR_CREATE;
	cFilenames = 1;
	rc = DosFindFirst((PBYTE)spath, &hdir, FILE_NORMAL,
		&findbuf, sizeof(findbuf), &cFilenames, FIL_STANDARD);
	while (!rc) {
	    WinSendDlgItemMsg(hwnd, listbox, LM_INSERTITEM, 
		MPFROMLONG(LIT_SORTASCENDING), MPFROMP(findbuf.achName));
	    cFilenames = 1;
	    rc = DosFindNext(hdir, &findbuf, sizeof(findbuf), &cFilenames);
	}
	DosFindClose(hdir);

	/* stuff in subdirectory names */
	hdir = HDIR_CREATE;
	cFilenames = 1;
	rc = DosFindFirst((PBYTE)dpath, &hdir, MUST_HAVE_DIRECTORY,
		&findbuf, sizeof(findbuf), &cFilenames, FIL_STANDARD);
	while (!rc) {
	    strcpy(spath,"[");
	    strcat(spath,findbuf.achName);
	    strcat(spath,"]");
	    if (strcmp(findbuf.achName, ".") != 0)
	      WinSendDlgItemMsg(hwnd, listbox, LM_INSERTITEM, 
		MPFROMLONG(LIT_SORTASCENDING), MPFROMP(spath));
	    cFilenames = 1;
	    rc = DosFindNext(hdir, &findbuf, sizeof(findbuf), &cFilenames);
	}
	DosFindClose(hdir);

	DosQueryCurrentDisk(&drivenum, &drivemap); 
	/* stuff in drive list */
	for (i=1; i<=26; i++) {
	  if (drivemap & 1) {
	    sprintf(spath,"[-%c-]",i-1+'a');
	    WinSendDlgItemMsg(hwnd, listbox, LM_INSERTITEM, 
		MPFROMLONG(LIT_END), MPFROMP(spath));
	  }
	  drivemap >>= 1;
	}
}


MRESULT EXPENTRY 
InstallDlgProc(HWND hwnd, ULONG mess, MPARAM mp1, MPARAM mp2)
{
    switch(mess) {
	case WM_INITDLG:
	    WinSendMsg( WinWindowFromID(hwnd, INSTALL_DLL),
	    	EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	    WinSetWindowText( WinWindowFromID(hwnd, INSTALL_DLL),
	    	(PCSZ)option.gsdll );
	    WinSendMsg( WinWindowFromID(hwnd, INSTALL_INCLUDE),
	    	EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	    WinSetWindowText( WinWindowFromID(hwnd, INSTALL_INCLUDE),
	    	(PCSZ)option.gsinclude );
	    WinSendMsg( WinWindowFromID(hwnd, INSTALL_OTHER),
	    	EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	    WinSetWindowText( WinWindowFromID(hwnd, INSTALL_OTHER),
	    	(PCSZ)option.gsother );
	    WinSetFocus(HWND_DESKTOP, WinWindowFromID(hwnd, INSTALL_INCLUDE));
    	    break;
        case WM_COMMAND:
            switch(SHORT1FROMMP(mp1)) {
		case ID_DEFAULT:
		    install_default(hwnd);
		    return(FALSE);
                case DID_OK:
                    WinEnableWindow(WinWindowFromID(hwnd, DID_OK), FALSE);
                    WinQueryWindowText(WinWindowFromID(hwnd, INSTALL_DLL),
                    	MAXSTR, (PBYTE)option.gsdll);
                    WinQueryWindowText(WinWindowFromID(hwnd, INSTALL_INCLUDE),
                    	MAXSTR, (PBYTE)option.gsinclude);
                    WinQueryWindowText(WinWindowFromID(hwnd, INSTALL_OTHER),
                    	MAXSTR, (PBYTE)option.gsother);
                    WinDismissDlg(hwnd, DID_OK);
		    return (MRESULT)TRUE;
		case ID_HELP:
		    get_help();
		    return (MRESULT)TRUE;
            }
            break;
    }
    return WinDefDlgProc(hwnd, mess, mp1, mp2);
}

BOOL
install_gsdll(void)
{
	nHelpTopic = IDS_TOPICADVANCEDCFG;
	if (WinDlgBox(HWND_DESKTOP, hwnd_frame, InstallDlgProc, hlanguage, IDD_INSTALL, NULL)
	   == DID_OK) {
		option.configured = TRUE;
		return TRUE;
	}
	return FALSE;
}

char *depthlist[] =    {"Default", "1", "4", "8", "24"};
int index_to_depth[] = { 0,         1,   4,   8,   24};
int depth_to_index(int depth)
{
int i;
    for (i=0; i<(int)(sizeof(index_to_depth)/sizeof(int)); i++)
	if (index_to_depth[i] == depth)
	    return i;
    return 0;
}

char *alphalist[] =    {"1", "2", "4"};
int index_to_alpha[] = { 1,   2,   4};
int alpha_to_index(int alpha)
{
unsigned int i;
    for (i=0; i<sizeof(index_to_alpha)/sizeof(int); i++)
	if (index_to_alpha[i] == alpha)
	    return i;
    return 0;
}

char *drawlist[] =    {"Default", "GpiDrawBits", "WinDrawBitmap"};
int index_to_draw[] = { IDM_DRAWDEF,  IDM_DRAWGPI, IDM_DRAWWIN };
int draw_to_index(int draw)
{
unsigned int i;
    for (i=0; i<sizeof(index_to_draw)/sizeof(int); i++)
	if (index_to_draw[i] == draw)
	    return i;
    return 0;
}

void
enable_alpha(HWND hwnd)
{
    int i;
    i = (int)WinSendMsg( WinWindowFromID(hwnd, DSET_DEPTH),
	    	LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), MPFROMLONG(0) );
    if (i == LIT_NONE)
	return;
    i = real_depth(index_to_depth[i]);
    i = (i >= 8);
    WinEnableWindow(WinWindowFromID(hwnd, DSET_TALPHA), i);
    WinEnableWindow(WinWindowFromID(hwnd, DSET_GALPHA), i);
}


MRESULT EXPENTRY 
DisplaySettingsDlgProc(HWND hwnd, ULONG mess, MPARAM mp1, MPARAM mp2)
{
    char buf[128];
    unsigned int i;
    switch(mess) {
	case WM_INITDLG:
	    WinSendMsg( WinWindowFromID(hwnd, DSET_RES),
	    	EM_SETTEXTLIMIT, MPFROM2SHORT(sizeof(buf)-1, 0), MPFROMLONG(0) );
	    if (option.xdpi == option.ydpi)
		sprintf(buf,"%g", option.xdpi);
	    else 
		sprintf(buf,"%g %g", option.xdpi, option.ydpi);
	    SetDlgItemText(hwnd, DSET_RES, buf);
	    WinSendMsg( WinWindowFromID(hwnd, DSET_ZOOMRES),
	    	EM_SETTEXTLIMIT, MPFROM2SHORT(sizeof(buf)-1, 0), MPFROMLONG(0) );
	    if (option.zoom_xdpi == option.zoom_ydpi)
		sprintf(buf,"%g", option.zoom_xdpi);
	    else 
		sprintf(buf,"%g %g", option.zoom_xdpi, option.zoom_ydpi);
	    SetDlgItemText(hwnd, DSET_ZOOMRES, buf);
	    WinSendMsg( WinWindowFromID(hwnd, DSET_DEPTH),
	    	LM_DELETEALL, MPFROMLONG(0), MPFROMLONG(0) );
	    for (i=0; i<sizeof(depthlist)/sizeof(char *); i++) {
		strcpy(buf, depthlist[i]);
		if (strcmp(buf, "Default")==0)
		    load_string(IDS_DEFAULT, buf, sizeof(buf)-1);
	        WinSendMsg( WinWindowFromID(hwnd, DSET_DEPTH),
	    	    LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(buf) );
	    }
	    WinSendMsg( WinWindowFromID(hwnd, DSET_DEPTH),
		LM_SELECTITEM, MPFROMSHORT(depth_to_index(option.depth)), MPFROMSHORT(TRUE));
	    WinSendMsg( WinWindowFromID(hwnd, DSET_TALPHA),
	    	LM_DELETEALL, MPFROMLONG(0), MPFROMLONG(0) );
	    WinSendMsg( WinWindowFromID(hwnd, DSET_TALPHA),
	    	LM_DELETEALL, MPFROMLONG(0), MPFROMLONG(0) );
	    for (i=0; i<sizeof(alphalist)/sizeof(char *); i++) {
	        WinSendMsg( WinWindowFromID(hwnd, DSET_TALPHA),
	    	    LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(alphalist[i]) );
	        WinSendMsg( WinWindowFromID(hwnd, DSET_GALPHA),
	    	    LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(alphalist[i]) );
	    }
	    WinSendMsg( WinWindowFromID(hwnd, DSET_TALPHA),
		LM_SELECTITEM, MPFROMSHORT(alpha_to_index(option.alpha_text)), MPFROMSHORT(TRUE));
	    WinSendMsg( WinWindowFromID(hwnd, DSET_GALPHA),
		LM_SELECTITEM, MPFROMSHORT(alpha_to_index(option.alpha_graphics)), MPFROMSHORT(TRUE));
	    enable_alpha(hwnd);
	    WinSendMsg( WinWindowFromID(hwnd, DSET_DRAWMETHOD),
	    	LM_DELETEALL, MPFROMLONG(0), MPFROMLONG(0) );
	    for (i=0; i<sizeof(drawlist)/sizeof(char *); i++) {
		strcpy(buf, drawlist[i]);
		if (strcmp(buf, "Default")==0)
		    load_string(IDS_DEFAULT, buf, sizeof(buf)-1);
	        WinSendMsg( WinWindowFromID(hwnd, DSET_DRAWMETHOD),
	    	    LM_INSERTITEM, MPFROMSHORT(LIT_END), MPFROMP(buf) );
	    }
	    WinSendMsg( WinWindowFromID(hwnd, DSET_DRAWMETHOD),
		LM_SELECTITEM, MPFROMSHORT(draw_to_index(option.drawmethod)), MPFROMSHORT(TRUE));
    	    break;
	case WM_CONTROL:
	    if (SHORT2FROMMP(mp1) == LN_SELECT)
		enable_alpha(hwnd);
	    break;
        case WM_COMMAND:
            switch(SHORT1FROMMP(mp1)) {
                case DID_OK:
		    {
		    BOOL unzoom = FALSE;
		    BOOL resize = FALSE;
	            BOOL restart = FALSE;
		    float x, y;
                    WinQueryWindowText(WinWindowFromID(hwnd, DSET_RES),
                    	sizeof(buf)-2, (PBYTE)buf);
		    switch (sscanf(buf,"%f %f", &x, &y)) {
		      case EOF:
		      case 0:
			break;
		      case 1:
			y = x;
		      case 2:
			if (x==0.0)
			    x= DEFAULT_RESOLUTION;
			if (y==0.0)
			    y= DEFAULT_RESOLUTION;
			if ( (x != option.xdpi) || (y != option.ydpi) ) {
			    option.xdpi = x;
			    option.ydpi = y;
			    resize = TRUE; 
			    unzoom = TRUE;
			}
		    }
                    WinQueryWindowText(WinWindowFromID(hwnd, DSET_ZOOMRES),
                    	sizeof(buf)-2, (PBYTE)buf);
		    switch (sscanf(buf,"%f %f", &x, &y)) {
		      case EOF:
		      case 0:
			break;
		      case 1:
			y = x;
		      case 2:
			if (x==0.0)
			    x= DEFAULT_RESOLUTION;
			if (y==0.0)
			    y= DEFAULT_RESOLUTION;
			if ( (x != option.zoom_xdpi) || (y != option.zoom_ydpi) ) {
			    option.zoom_xdpi = x;
			    option.zoom_ydpi = y;
			    resize = TRUE; 
			    unzoom = TRUE;
			}
		    }
		    i = (unsigned int)WinSendMsg( WinWindowFromID(hwnd, DSET_DEPTH),
			LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), MPFROMLONG(0) );
		    i = index_to_depth[i];
		    if (i != (unsigned int)option.depth) {
			option.depth = i;
			restart = TRUE;
			resize = TRUE; 
			unzoom = TRUE;
		    }
		    i = (unsigned int)WinSendMsg( WinWindowFromID(hwnd, DSET_TALPHA),
			LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), MPFROMLONG(0) );
		    i = index_to_alpha[i];
		    if (i != (unsigned int)option.alpha_text) {
			option.alpha_text = i;
			restart = TRUE;
			resize = TRUE; 
			unzoom = TRUE;
		    }
		    i = (unsigned int)WinSendMsg( WinWindowFromID(hwnd, DSET_GALPHA),
			LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), MPFROMLONG(0) );
		    i = index_to_alpha[i];
		    if (i != (unsigned int)option.alpha_graphics) {
			option.alpha_graphics = i;
			/* restart = TRUE; */
			resize = TRUE; 
			unzoom = TRUE;
		    }
		    i = (unsigned int)WinSendMsg( WinWindowFromID(hwnd, DSET_DRAWMETHOD),
			LM_QUERYSELECTION, MPFROMSHORT(LIT_FIRST), MPFROMLONG(0) );
		    i = index_to_draw[i];
		    if (i != (unsigned int)option.drawmethod) {
			option.drawmethod = i;
			/* strictly all we need to do is redraw the screen */
			/* but we force this making GS redraw */
			resize = TRUE; 	
		    }
		    if (resize) {
			if (unzoom)
			    gsview_unzoom();
			if (gsdll.state != GS_UNINIT) {
/* gs_resize has this check
			    if (option.redisplay && (gsdll.state == GS_PAGE) && (psfile.doc != (CDSC *)NULL))
*/
				gs_resize();
			    /* for those that can't be changed with a */
			    /* postscript command so must close gs */
			    if (restart)
				pending.restart = TRUE;
			}
		    }
		    }
                    WinDismissDlg(hwnd, DID_OK);
		    return (MRESULT)TRUE;
		case DID_CANCEL:
		    WinDismissDlg(hwnd, DID_CANCEL);
		    return (MRESULT)TRUE;
		case ID_HELP:
		    nHelpTopic = IDS_TOPICDSET;
		    get_help();
		    return (MRESULT)TRUE;
            }
            break;
    }
    return WinDefDlgProc(hwnd, mess, mp1, mp2);
}


void
display_settings(void)
{
	WinDlgBox(HWND_DESKTOP, hwnd_frame, DisplaySettingsDlgProc, hlanguage, IDD_DSET, NULL);
}


/* Text Window for Ghostscript Messages */
/* uses OS/2 MLE control */


#define TWLENGTH 16384
#define TWSCROLL 1024
char twbuf[TWLENGTH];
int twend;

MRESULT EXPENTRY 
TextDlgProc(HWND hwnd, ULONG mess, MPARAM mp1, MPARAM mp2)
{
    switch(mess) {
	case WM_INITDLG:
	    {IPT ipt = 0;
	    int len = strlen(twbuf);
	    WinSendMsg( WinWindowFromID(hwnd, TEXTWIN_MLE),
	    	MLM_SETIMPORTEXPORT, (MPARAM)twbuf, (MPARAM)len);
	    WinSendMsg( WinWindowFromID(hwnd, TEXTWIN_MLE),
	    	MLM_IMPORT, (MPARAM)&ipt, (MPARAM)len);
	    WinSendMsg( WinWindowFromID(hwnd, TEXTWIN_MLE),
	    	MLM_SETSEL, (MPARAM)len, (MPARAM)len);
	    }
    	    break;
        case WM_COMMAND:
            switch(SHORT1FROMMP(mp1)) {
                case DID_OK:
                    WinEnableWindow(WinWindowFromID(hwnd, DID_OK), FALSE);
                    WinDismissDlg(hwnd, DID_OK);
		    return (MRESULT)TRUE;
		case ID_HELP:
		    get_help();
		    return (MRESULT)TRUE;
		case TEXTWIN_COPY:
		    {MRESULT mr; 
	    	    mr = WinSendMsg( WinWindowFromID(hwnd, TEXTWIN_MLE),
	    		MLM_QUERYSEL, (MPARAM)MLFQS_MINMAXSEL, (MPARAM)0);
		    if (SHORT1FROMMP(mr) == SHORT2FROMMP(mr)) /* no selection so select all */
	    		WinSendMsg( WinWindowFromID(hwnd, TEXTWIN_MLE),
	    		    MLM_SETSEL, 0, (MPARAM)strlen(twbuf));
	    	    WinSendMsg( WinWindowFromID(hwnd, TEXTWIN_MLE),
	    		MLM_COPY, (MPARAM)0, (MPARAM)0);
		    WinSendMsg( WinWindowFromID(hwnd, TEXTWIN_MLE),
		        MLM_SETSEL, (MPARAM)strlen(twbuf), (MPARAM)strlen(twbuf));
		    }
		    return (MRESULT)TRUE;
            }
            break;
    }
    return WinDefDlgProc(hwnd, mess, mp1, mp2);
}

/* Show Ghostscript messages window */
void
gs_showmess(void)
{
        nHelpTopic = IDS_TOPICMESS;
	WinDlgBox(HWND_DESKTOP, hwnd_frame, TextDlgProc, hlanguage, IDD_TEXTWIN, NULL);
}

/* Add string for Ghostscript message window */
void
gs_addmess_count(const char *str, int count)
{
    /* if debugging, write to a log file */
    if (debug & DEBUG_LOG) {
	FILE *f = fopen("c:\\gsview.txt", "a");
	if (f != (FILE *)NULL) {
	    fwrite(str, 1, count, f);
	    fclose(f);
	}
    }

    if (count >= TWSCROLL)
	return;		/* too large */
    if (count + twend >= TWLENGTH-1) {
	/* scroll buffer */
	twend -= TWSCROLL;
	memmove(twbuf, twbuf+TWSCROLL, twend);
    }
    memmove(twbuf+twend, str, count);
    twend += count;
    *(twbuf+twend) = '\0';
}

void
gs_addmess(const char *str)
{
    gs_addmess_count(str, strlen(str));
}


MRESULT EXPENTRY 
DSCErrorDlgProc(HWND hwnd, ULONG mess, MPARAM mp1, MPARAM mp2)
{
    switch(mess) {
	case WM_INITDLG:
	    {BYTE buf[MAXSTR];
	    unsigned int i;
	    unsigned int j = 0;
	    PBYTE mess = (PBYTE)mp2;
	    /* place one line per control */
	    while (*mess) {
		for (i=0; i<sizeof(buf)-1; i++) {
		    buf[i] = *mess++;
		    if (buf[i] == '\n') {
			buf[i] = '\0';
			break;
		    }
		}
		WinSetWindowText(WinWindowFromID(hwnd, DSC_STATIC_TEXT+j), buf);
		j++;
	    }
	    }
    	    break;
        case WM_COMMAND:
            switch(SHORT1FROMMP(mp1)) {
                case DID_OK:
                    WinDismissDlg(hwnd, CDSC_RESPONSE_OK);
		    return (MRESULT)TRUE;
                case DID_CANCEL:
                    WinDismissDlg(hwnd, CDSC_RESPONSE_CANCEL);
		    return (MRESULT)TRUE;
                case DSC_IGNORE_ALL:
                    WinDismissDlg(hwnd, CDSC_RESPONSE_IGNORE_ALL);
		    return (MRESULT)TRUE;
		case ID_HELP:
		    get_help();
		    return (MRESULT)TRUE;
            }
            break;
    }
    return WinDefDlgProc(hwnd, mess, mp1, mp2);
}


int 
get_dsc_response(char *message)
{
    nHelpTopic = IDS_TOPICDSCWARN;
    return WinDlgBox(HWND_DESKTOP, hwnd_frame, DSCErrorDlgProc, 
	hlanguage, IDD_DSCERROR, message);
}
