/* Copyright (C) 1993-2007, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwdlg.c */
/* Dialog boxes for Windows GSview */
#include "gvwin.h"

/* Find which display is used by the window at origin x,y */
/* Return -1 if we can't find it */
int
find_display(int x, int y)
{
    int i;
    for (i=0; i<number_of_displays; i++) {
	if ((x >= win_display[i].left) && 
	    (x <  win_display[i].left + win_display[i].width) &&
	    (y >= win_display[i].top) && 
	    (y <  win_display[i].top + win_display[i].height))
	    return i;
    }
    return -1;
}


void
centre_dialog(HWND hwnd)
{
    /* centre dialog on screen */
    int left = 0;
    int top = 0;
    int width = GetSystemMetrics(SM_CXFULLSCREEN);
    int height = GetSystemMetrics(SM_CYFULLSCREEN);
    RECT rect;
    int disp;

    /* Find display of parent */
    GetWindowRect(GetParent(hwnd), &rect);
    disp = find_display((rect.left+rect.right)/2, (rect.top+rect.bottom)/2);
    if (disp < 0)
        disp = find_display(rect.left, rect.top);
    if (disp >= 0) {
	/* Put it on the first display */
	left =   win_display[disp].left;
	top =    win_display[disp].top;
	width =  win_display[disp].width;
	height = win_display[disp].height;
    }

    /* Centre on parent */
    GetWindowRect(hwnd, &rect);
    MoveWindow(hwnd, left+(width - (rect.right - rect.left))/2,
	    top+(height - (rect.bottom - rect.top))/2,
	    (rect.right - rect.left),
	    (rect.bottom - rect.top), FALSE);
}

BOOL
get_filename(char *filename, BOOL save, int filter, int title, int help)
{
LPTSTR old_lpstrFile;
LPCTSTR old_lpstrTitle;
TCHAR szFileName[MAXSTR];
TCHAR szTitle[MAXSTR];
BOOL flag;
LPCTSTR old_lpstrFilter;
TCHAR szFilter[1024];		/* filter for OFN */
int i;
TCHAR cReplace;

#if defined(VIEWONLY)
	save = FALSE;
#endif
	if (help)
	    nHelpTopic = help;
	old_lpstrTitle = ofn.lpstrTitle;
	if (title) {
	    load_string(title, szTitle, sizeof(szTitle));
	    ofn.lpstrTitle = szTitle;
	}
	old_lpstrFile = ofn.lpstrFile;
	if (filename != (LPSTR)NULL) {
	    convert_multibyte(szFileName, filename, 
		sizeof(szFileName)/sizeof(TCHAR));
	    ofn.lpstrFile = szFileName;
	}
	/* Get filter types */
	old_lpstrFilter = ofn.lpstrFilter;
	ofn.nFilterIndex = 0;
	if (load_string(IDS_FILTER_BASE+filter, szFilter, sizeof(szFilter)-1)) {
	    cReplace = szFilter[lstrlen(szFilter)-1];
	    for (i=0; szFilter[i] != '\0'; i++)
	        if (szFilter[i] == cReplace)
		    szFilter[i] = '\0';
	    ofn.lpstrFilter = szFilter;
	    ofn.nFilterIndex = 0;
	}
	/* call the common dialog box */
#ifdef UNICODE
	if (save)
	    flag = GetSaveFileName(&ofn);
	else
	    flag = GetOpenFileName(&ofn);
#else
	flag = GetOpenSaveFileNameL(&ofn, save);
#endif
	if (flag)
	    convert_widechar(filename, ofn.lpstrFile, MAXSTR-1);
	ofn.lpstrTitle = old_lpstrTitle;
	ofn.lpstrFile = old_lpstrFile;
	ofn.lpstrFilter = old_lpstrFilter;
	ofn.nFilterIndex = 0;


	if ( save && flag && 
	     (psfile.name[0]!='\0') && (strcmp(filename, psfile.name) == 0) ) {
	    gserror(IDS_NOTDFNAME, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	    flag = FALSE;
	}
	/* GetOpenFileName() should change current directory */
	/* Deal with broken Win32 that doesn't do this */
	if (flag)
	    make_cwd(filename);
	return flag;
}

/* Input Dialog Box structures */
LPCTSTR input_prop = TEXT("input_prop");
struct input_param {
	LPCTSTR prompt;
	TCHAR answer[MAXSTR];
};


/* input string dialog box */
DLGRETURN CALLBACK _export
InputDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
	    {
	      HLOCAL hlocal;
	      LPTSTR *panswer;
	      struct input_param *pparam = (struct input_param *)lParam;
	      SetDlgItemText(hDlg, ID_PROMPT, pparam->prompt);
	      SetDlgItemText(hDlg, ID_ANSWER, pparam->answer);
	      /* save address of answer string in property list */
	      hlocal = LocalAlloc(LHND, sizeof(pparam->answer));
	      panswer = (LPTSTR *)LocalLock(hlocal);
	      if (panswer != (LPTSTR *)NULL) {
	        *panswer = pparam->answer;
		LocalUnlock(hlocal);
	        SetProp(hDlg, input_prop, (HANDLE)hlocal);
	      }
	    }
            return( TRUE);
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case ID_HELP:
		    get_help();
		    return(FALSE);
                case ID_ANSWER:
                    return(TRUE);
		case IDOK:
		    {
		      HLOCAL hlocal = (HLOCAL)GetProp(hDlg, input_prop); 
		      LPTSTR *panswer;
	              panswer = (LPTSTR *)LocalLock(hlocal);
	              if (panswer != (LPTSTR *)NULL) {
		        GetDlgItemText(hDlg, ID_ANSWER, *panswer, MAXSTR);
			LocalUnlock(hlocal);
		      }
		      LocalFree(hlocal);
		      RemoveProp(hDlg, input_prop);
		    }
                    EndDialog(hDlg, TRUE);
                    return(TRUE);
                case IDCANCEL:
		    {
		      HLOCAL hlocal = (HLOCAL)GetProp(hDlg, input_prop); 
		      LocalFree(hlocal);
		      RemoveProp(hDlg, input_prop);
		    }
                    EndDialog(hDlg, FALSE);
                    return(TRUE);
                default:
                    return(FALSE);
            }
        default:
            return(FALSE);
    }
}

BOOL
query_string(LPCTSTR prompt, char *answer)
{
struct input_param param;
BOOL flag;
        convert_multibyte(param.answer, answer, 
		sizeof(param.answer)/sizeof(TCHAR));
	param.prompt = prompt;
	flag = DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_INPUT), 
	    hwndimg, InputDlgProc, (LPARAM)&param);
	if (flag)
	    convert_widechar(answer, param.answer, MAXSTR-1);
	return flag;
}


void
wwait(void)
{
MSG msg;
DWORD start = GetTickCount();
DWORD end = start + 70;
	while ( (GetTickCount() <= end) && (GetTickCount() >= start) ) {
		while (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)) {
			 TranslateMessage(&msg);
			 DispatchMessage(&msg);
		}
	}
}

/* copyright dialog box */
DLGRETURN CALLBACK _export
AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
            SetDlgItemText(hDlg, ABOUT_VERSION, TEXT(GSVIEW_DOT_VERSION));
            SetDlgItemText(hDlg, NAG_NAME, registration_name);
	    if (registration_receipt != 0) {
		TCHAR buf[16];
		wsprintf(buf, TEXT("%u"), registration_receipt);
		SetDlgItemText(hDlg, NAG_RECEIPT, buf);
	    }
            return( TRUE);
	  case WM_LBUTTONDOWN:
	    {
	    HWND hwicon = GetDlgItem(hDlg, ABOUT_ICON);
	    HICON hicon1 = LoadIcon(phInstance, MAKEINTRESOURCE(ID_GSVIEW));
	    HICON hicon2 = LoadIcon(phInstance, MAKEINTRESOURCE(ID_GSVIEW2));
	    HICON hicon3 = LoadIcon(phInstance, MAKEINTRESOURCE(ID_GSVIEW3));
	    HDC hdc = GetDC(hwicon);
	    RECT rect; POINT pt;
		pt.x = LOWORD(lParam); pt.y = HIWORD(lParam);
		ClientToScreen(hDlg, &pt);
		GetWindowRect(hwicon, &rect);
		if (PtInRect(&rect,pt)) {
			DrawIcon(hdc, 0, 0, hicon2);
			wwait();
			DrawIcon(hdc, 0, 0, hicon3);
			wwait();
			wwait();
			DrawIcon(hdc, 0, 0, hicon2);
			wwait();
			DrawIcon(hdc, 0, 0, hicon1);
		}
		DestroyIcon(hicon1);
		DestroyIcon(hicon2);
		DestroyIcon(hicon3);
		ReleaseDC(hwicon, hdc);
	    }
	    return FALSE;
	case WM_LBUTTONDBLCLK:
	    {DWORD dwUnit = GetDialogBaseUnits();
	    RECT rect; POINT pt;
	    pt.x = LOWORD(lParam); pt.y = HIWORD(lParam);
		/* this is for 8pt dialog fonts */
		rect.left   =   8 * LOWORD(dwUnit) / 5;
		rect.top    = 166 * HIWORD(dwUnit) / 10;
		rect.right  = 240 * LOWORD(dwUnit) / 5 + rect.left;
		rect.bottom =  10 * HIWORD(dwUnit) / 10 + rect.top;
#ifdef NOTUSED
		/* this is for 10pt dialog fonts */
		rect.left   =   8 * LOWORD(dwUnit) / 4;
		rect.top    = 166 * HIWORD(dwUnit) / 8;
		rect.right  = 240 * LOWORD(dwUnit) / 4 + rect.left;
		rect.bottom =   8 * HIWORD(dwUnit) / 8 + rect.top;
#endif

	    if (PtInRect(&rect,pt)) {
		BITMAP bm;
		HBITMAP hbitmap_old;
		HBITMAP hbitmap = LoadBitmap(phInstance,TEXT("gsview_bitmap"));
		HDC hdc = GetDC(hDlg);
		HDC hdcsrc = CreateCompatibleDC(hdc);
		hbitmap_old = (HBITMAP)SelectObject(hdcsrc,hbitmap);
		GetObject(hbitmap, sizeof(BITMAP),&bm);
		BitBlt(hdc, rect.right-bm.bmWidth,rect.bottom-bm.bmHeight,
		   bm.bmWidth,bm.bmHeight,hdcsrc,0,0,SRCCOPY);
		SelectObject(hdcsrc,hbitmap_old);
		DeleteObject(hbitmap);
		DeleteDC(hdcsrc);
		ReleaseDC(hDlg,hdc);
	    }
	    }
	    return FALSE;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case IDCANCEL:
                case IDOK:
                    EndDialog(hDlg, TRUE);
                    return(TRUE);
                default:
                    return(FALSE);
            }
        default:
            return(FALSE);
    }
}

void
show_about()
{
    DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_ABOUT), hwndimg, 
	AboutDlgProc, (LPARAM)NULL);
}

#ifdef __BORLANDC__
#pragma argsused
#endif
/* information about document dialog box */
DLGRETURN CALLBACK _export
InfoDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
	    SendDlgItemMessage(hDlg, INFO_FILE, WM_SETFONT, (WPARAM)info_font, 1);
	    info_init(hDlg);
	    SendDlgItemMessage(hDlg, INFO_FILE, WM_SETFONT, (WPARAM)info_font, 1);
            return( TRUE);
	case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDOK:
                    EndDialog(hDlg, TRUE);
                    return(TRUE);
                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return(TRUE);
                default:
                    return(FALSE);
            }
        default:
            return(FALSE);
    }
}

/* show info about ps file */
void
show_info()
{
    DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_INFO), 
	hwndimg, InfoDlgProc, (LPARAM)NULL);
}



#define MAX_SYSTEM_SOUND 16
TCHAR *system_sounds;
TCHAR *sound_entry[MAX_SYSTEM_SOUND];
TCHAR szNone[32];
TCHAR szSpeaker[32];
int system_num;
DLGRETURN CALLBACK _export SoundDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam);

int
load_sounds(void)
{
	/* add our two sounds */
	sound_entry[0] = TEXT("");
	sound_entry[1] = TEXT(BEEP);
	load_string(IDS_NONE, szNone, sizeof(szNone));
	load_string(IDS_SPKR, szSpeaker, sizeof(szSpeaker));
#ifndef UNICODE
	if (is_win32s) {
	    /* get list of system sounds */
	    char *p;
	    int j;
	    system_sounds = (char *)malloc(PROFILE_SIZE);
	    if (system_sounds != (char *)NULL) {
		GetProfileString("sounds", NULL, "", system_sounds, 
		PROFILE_SIZE);
	    }
	    p = system_sounds;
	    for (j=2; p!=(char *)NULL && j<MAX_SYSTEM_SOUND && strlen(p)!=0; 
		j++) {
		/* Windows NT uses "Enable=1" in the sounds section */
		/* We need to prevent this from appearing in the */
		/* list of sounds */
		if (strcmp(p, "Enable") == 0)
		    j--;
		else
		    sound_entry[j] = p;	
		p += strlen(p) + 1;
	    }
	    system_num = j;
	}
	else 
#endif
	{
	    /* It is difficult to get the names of Windows system sounds */
	    /* for Windows 4 and later */
	    system_num = 2;
	}
	return system_num;
}

TCHAR *
get_sound_entry(int index)
{
	return (sound_entry[index]);
}

TCHAR *
get_sound_name(int index)
{
static char buf[64];
TCHAR *p;
	if (index==0)
		return szNone;
	if (index==1)
		return szSpeaker;
#ifndef UNICODE
	if (is_win32s) {
	    GetProfileString("sounds", sound_entry[index], ",", 
		buf, sizeof(buf));
	    p = strchr(buf,',');
	    if (p != (char *)NULL)
		return p+1;
	}
#endif
	return (TCHAR *)NULL;
}

int 
find_sound_name(TCHAR *entry)
{
int i;
	for (i=0; i<system_num; i++) {
	    if (lstrcmp(entry, sound_entry[i])==0)
	        return i;
	}
	return -1;	/* no find */
}

void
add_sounds(HWND hDlg)
{
int ifile;
TCHAR *p;
    for (ifile=system_num-1; ifile>=0; ifile--) {
	p = get_sound_name(ifile);
	if (p != (TCHAR *)NULL)
	    SendDlgItemMessageL(hDlg, SOUND_FILE, LB_INSERTSTRING, 0,
		(LPARAM)(LPTSTR)p);
    }
}

void
free_sounds(void)
{
    if (system_sounds != (TCHAR *)NULL)
	free(system_sounds);
}

void
change_sounds(void)
{
    nHelpTopic = IDS_TOPICSOUND;
    DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_SOUND), hwndimg, 
	SoundDlgProc, (LPARAM)NULL);
}

#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
SoundDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR buf[MAXSTR];
	int ievent, ifile;
	static struct sound_s dsound[NUMSOUND];	/* copy of sound[] */
	WORD notify_message;
	const TCHAR *szWaveFilter = TEXT("*.wav");
	static TCHAR szPath[MAXSTR];
	static int file_start;
	TCHAR *p;

	switch (wmsg) {
	    case WM_INITDIALOG:
		file_start = load_sounds();
		for (ievent=0; ievent<NUMSOUND; ievent++) {
		    lstrcpy(dsound[ievent].file, sound[ievent].file);
		    load_string(sound[ievent].title, buf, sizeof(buf));
		    SendDlgItemMessageL(hDlg, SOUND_EVENT, LB_ADDSTRING, 0, 
			(LPARAM)((LPTSTR)buf));
		}
		ievent = 0;
		SendDlgItemMessage(hDlg, SOUND_EVENT, LB_SETCURSEL, ievent, 0L);
		/* force update of SOUND_FILE */
		SendDlgNotification(hDlg, SOUND_EVENT, LBN_SELCHANGE);
		return TRUE;
	    case WM_COMMAND:
		notify_message = GetNotification(wParam,lParam);
		switch (LOWORD(wParam)) {
		    case ID_HELP:
			get_help();
		        return(FALSE);
		    case SOUND_EVENT:
			if (notify_message != LBN_SELCHANGE) {
				return FALSE;
			}
			ievent = (int)SendDlgItemMessage(hDlg, SOUND_EVENT, LB_GETCURSEL, 0, 0L);
			if (ievent == LB_ERR) {
			    EnableWindow(GetDlgItem(hDlg, SOUND_TEST), FALSE);
			    return FALSE;
			}
			ifile = find_sound_name(dsound[ievent].file);
			if (ifile >= 0) {
			    lstrcpy(buf, get_sound_name(ifile));
			    szPath[0] = '\0';
			    EnableWindow(GetDlgItem(hDlg, SOUND_TEST), ifile!=0);
			}
			else {
			    /* must be WAVE file */
			    int i;
			    lstrcpy(szPath, dsound[ievent].file);
			    p = NULL;
			    for (i=lstrlen(szPath)-1; i >= 0; i--) {
				if (szPath[i] == '\\') {
				    p = szPath+i;
				    break;
				}
			    }
			    if (p != (TCHAR *)NULL) {
			        lstrcpy(buf,++p);
			        *p = '\0';
			    }
			    else {
			        lstrcpy(buf, szPath);
			    }
			    EnableWindow(GetDlgItem(hDlg, SOUND_TEST), TRUE);
			}
			lstrcat(szPath, szWaveFilter);
			DlgDirList(hDlg, szPath, SOUND_FILE, SOUND_PATH, DDL_DRIVES | DDL_DIRECTORY);
			add_sounds(hDlg);
			SendDlgItemMessage(hDlg, SOUND_FILE, LB_SELECTSTRING, file_start, (LPARAM)(LPTSTR)buf);
			return FALSE;
		    case SOUND_FILE:
			if (notify_message == LBN_SELCHANGE) {
			    ifile = (int)SendDlgItemMessage(hDlg, SOUND_FILE, LB_GETCURSEL, 0, 0L);
			    SendDlgItemMessage(hDlg, SOUND_FILE, LB_GETTEXT, ifile, (LPARAM)(LPSTR)buf);
			    ievent = (int)SendDlgItemMessage(hDlg, SOUND_EVENT, LB_GETCURSEL, 0, 0L);
			    if (ifile >= file_start) {
				if (buf[0] == '[') { /* selected a directory */
				    EnableWindow(GetDlgItem(hDlg, SOUND_TEST), FALSE);
			        }
				else { /* selected a WAVE file */
		                    int i = GetDlgItemText(hDlg, SOUND_PATH, dsound[ievent].file, MAXSTR);
			            if (dsound[ievent].file[i-1] != '\\')
			                dsound[ievent].file[i++] = '\\';
		                    DlgDirSelectEx(hDlg, dsound[ievent].file + i, sizeof(dsound[ievent].file), SOUND_FILE);
				    EnableWindow(GetDlgItem(hDlg, SOUND_TEST), TRUE);
				}
			    }
			    else {
				EnableWindow(GetDlgItem(hDlg, SOUND_TEST), ifile!=0);
				lstrcpy(dsound[ievent].file,get_sound_entry(ifile));
			    }
			}
			if (notify_message == LBN_DBLCLK) {
			    ifile = (int)SendDlgItemMessage(hDlg, SOUND_FILE, LB_GETCURSEL, 0, 0L);
			    SendDlgItemMessage(hDlg, SOUND_FILE, LB_GETTEXT, ifile, (LPARAM)(LPSTR)buf);
			    if (buf[0] == '[') {
		                DlgDirSelectEx(hDlg, szPath, sizeof(szPath), SOUND_FILE);
			        lstrcat(szPath, szWaveFilter);
		                DlgDirList(hDlg, szPath, SOUND_FILE, SOUND_PATH, DDL_DRIVES | DDL_DIRECTORY);
				add_sounds(hDlg);
			    }
			    else {
				SendDlgNotification(hDlg, SOUND_TEST, BN_CLICKED);
			    }
			}
			return FALSE;
		    case SOUND_TEST:
			ievent = (int)SendDlgItemMessage(hDlg, SOUND_EVENT, LB_GETCURSEL, 0, 0L);
			if (lstrlen(dsound[ievent].file)==0)
				return FALSE;
			if (lstrcmp(dsound[ievent].file, TEXT(BEEP))==0) {
				MessageBeep(-1);
				return FALSE;
			}
			if (lpfnSndPlaySound != (FPSPS)NULL) 
			    lpfnSndPlaySound(dsound[ievent].file, SND_SYNC);
			else
			    MessageBeep(-1);
			return FALSE;
		    case IDOK:
			for (ievent=0; ievent<NUMSOUND; ievent++)
			    lstrcpy(sound[ievent].file, dsound[ievent].file);
			free_sounds();
			EndDialog(hDlg, TRUE);
			return TRUE;
		    case IDCANCEL:
			free_sounds();
			EndDialog(hDlg, FALSE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

/* Return TRUE if a contiguous block of pages (>2) is found */
/* Store start and end of this range in first and last */
/* If not contiguous, store entire page range in first and last */
BOOL
contiguous_range(HWND hDlg, int *first, int *last)
{
int i;
BOOL gap = FALSE;	/* TRUE is gap found after block */
BOOL contiguous = TRUE;
int block = 0;		/* number of pages set in a contiguous block */
BOOL selected;
    for (i=0; i<(int)(psfile.dsc->page_count); i++) {
	selected = (int)SendDlgItemMessage(hDlg, PAGE_LIST, LB_GETSEL, i, 0L);
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

#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
PageDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
	int i;
	WORD notify_message;
	static BOOL ecdisable;
	switch (wmsg) {
	    case WM_INITDIALOG:
		{TCHAR buf[MAXSTR];
		char cbuf[MAXSTR];
		for (i=0; i<(int)(psfile.dsc->page_count); i++) {
		    page_ordlabel(cbuf, i);
		    SendDlgItemMessageA(hDlg, PAGE_LIST, LB_ADDSTRING, 0, 
			(LPARAM)((LPSTR)cbuf));
		}
		SendDlgItemMessage(hDlg, PAGE_LIST, LB_SETCURSEL, 
		    psfile.page_list.current, 0L);
		SendDlgItemMessage(hDlg, PAGE_LIST, LB_GETTEXT, 
		    psfile.page_list.current, (LPARAM)buf);
		ecdisable = TRUE;
		SetDlgItemText(hDlg, PAGE_EDIT, buf);
		ecdisable = FALSE;
		}
		return TRUE;
	    case WM_COMMAND:
		notify_message = GetNotification(wParam,lParam);
		switch (LOWORD(wParam)) {
		    case PAGE_EDIT:
			if (!ecdisable && (notify_message == EN_CHANGE)) {
			    TCHAR buf[MAXSTR];
			    GetDlgItemText(hDlg, PAGE_EDIT, buf, 
				sizeof(buf)/sizeof(TCHAR));
			    if ((i = (int)SendDlgItemMessage(hDlg, PAGE_LIST, 
				    LB_FINDSTRING, -1, (LPARAM)buf))
				!= LB_ERR) {
				SendDlgItemMessage(hDlg, PAGE_LIST, 
				    LB_SETCURSEL, i, 0L);
			    }
			}
			return TRUE;
		    case PAGE_LIST:
			if (notify_message == LBN_DBLCLK)
				PostMessage(hDlg, WM_COMMAND, IDOK, 0L);
		        else if (notify_message == LBN_SELCHANGE) {
			    TCHAR buf[MAXSTR];
			    i = (int)SendDlgItemMessage(hDlg, PAGE_LIST, 
				LB_GETCURSEL, 0, 0L);
			    if (i != LB_ERR) {
				SendDlgItemMessage(hDlg, PAGE_LIST, LB_GETTEXT, 
				    i, (LPARAM)buf);
				/* Update edit field, but stop edit field from
			         * from altering list box selection */
				ecdisable = TRUE;
				SetDlgItemText(hDlg, PAGE_EDIT, buf);
				ecdisable = FALSE;
			    }
			}
			return FALSE;
		    case IDOK:
			i = (int)SendDlgItemMessage(hDlg, PAGE_LIST, 
				LB_GETCURSEL, 0, 0L);
			if (i == LB_ERR)
			    EndDialog(hDlg, FALSE);
			else {
			    psfile.page_list.current = i;
			    /* if available, also save in selection array */
			    if (psfile.page_list.select) {
			      for (i=0; i<(int)(psfile.dsc->page_count); i++) {
				  psfile.page_list.select[i] = FALSE;
			      }
			      psfile.page_list.select[psfile.page_list.current]
				 = TRUE;
			    }
			    EndDialog(hDlg, TRUE);
			}
			return TRUE;
		    case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
PageMultiDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
	int i;
	WORD notify_message;
	switch (wmsg) {
	    case WM_INITDIALOG:
		{
		char cbuf[MAXSTR];
		if (psfile.page_list.reverse)
		    SendDlgItemMessage(hDlg, PAGE_REVERSE, BM_SETCHECK, 1, 0);
		for (i=0; i<(int)(psfile.dsc->page_count); i++) {
		    page_ordlabel(cbuf, i);
		    SendDlgItemMessageA(hDlg, PAGE_LIST, LB_ADDSTRING, 0, 
			(LPARAM)((LPSTR)cbuf));
		}
		if (psfile.page_list.multiple) {
		    /* multiple selection list box */
		    for (i=0; i<(int)(psfile.dsc->page_count); i++)
			if (psfile.page_list.select[i]) 
			    SendDlgItemMessage(hDlg, PAGE_LIST, LB_SETSEL, TRUE, MAKELPARAM(i,0));
		    SendDlgItemMessage(hDlg, PAGE_LIST, LB_SETSEL, TRUE, MAKELPARAM(psfile.page_list.current, 0));
		}
		else {
		    /* single selection list box */
		    SendDlgItemMessage(hDlg, PAGE_LIST, LB_SETSEL, TRUE, MAKELPARAM(psfile.page_list.current, 0));
		    SendDlgItemMessage(hDlg, PAGE_LIST, LB_SETCURSEL, psfile.page_list.current, 0L);
		    EnableWindow(GetDlgItem(hDlg, PAGE_ALL), FALSE);
		    EnableWindow(GetDlgItem(hDlg, PAGE_ODD), FALSE);
		    EnableWindow(GetDlgItem(hDlg, PAGE_EVEN), FALSE);
		}
		}
		return TRUE;
	    case WM_COMMAND:
		notify_message = GetNotification(wParam,lParam);
		switch (LOWORD(wParam)) {
		    case PAGE_LIST:
			if (notify_message == LBN_DBLCLK)
				PostMessage(hDlg, WM_COMMAND, IDOK, 0L);
			return FALSE;
		    case PAGE_ALL:
			SendDlgItemMessage(hDlg, PAGE_LIST, LB_SELITEMRANGE, TRUE, 
				MAKELPARAM(0,psfile.dsc->page_count-1));
			return FALSE;
		    case PAGE_ODD:
			{
			int first, last;
			contiguous_range(hDlg, &first, &last);
			for (i=(int)SendDlgItemMessage(hDlg, PAGE_LIST, 
				LB_GETCOUNT, 0, 0L)-1; i>=0; i--)
			    if (i >= first && i < last)
			        SendDlgItemMessage(hDlg, PAGE_LIST, LB_SETSEL, 
				    !(i&1), MAKELPARAM(i,0));
			SendDlgItemMessage(hDlg, PAGE_LIST, LB_SETTOPINDEX, 
			    first, 0L);
			}
			return FALSE;
		    case PAGE_EVEN:
			{
			int first, last;
			contiguous_range(hDlg, &first, &last);
			for (i=(int)SendDlgItemMessage(hDlg, PAGE_LIST, 
				LB_GETCOUNT, 0, 0L)-1; i>=0; i--)
			    if (i >= first && i < last)
			        SendDlgItemMessage(hDlg, PAGE_LIST, LB_SETSEL, 
				    (i&1), MAKELPARAM(i,0));
			SendDlgItemMessage(hDlg, PAGE_LIST, LB_SETTOPINDEX, 
			    first, 0L);
			}
			return FALSE;
		    case IDOK:
			psfile.page_list.reverse = 
			    (int)SendDlgItemMessage(hDlg, PAGE_REVERSE, 
				BM_GETCHECK, 0, 0);
			i = (int)SendDlgItemMessage(hDlg, PAGE_LIST, LB_GETCURSEL, 0, 0L);
			psfile.page_list.current = (i == LB_ERR) ? -1 : i;
			for (i=0; i<(int)(psfile.dsc->page_count); i++) {
			  psfile.page_list.select[i] =
			    (int)SendDlgItemMessage(hDlg, PAGE_LIST, LB_GETSEL, i, 0L);
			}
			EndDialog(hDlg, TRUE);
			return TRUE;
		    case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

/* Get page number from dialog box and store in ppage */
/* multiple is TRUE if multiple pages may be selected */
/* allpages is TRUE if all pages should be initially selected */
BOOL
get_page(int *ppage, BOOL multiple, BOOL allpages)
{
DLGPROC lpProc;
BOOL flag;
LPTSTR dlgname;
int i;
	if (psfile.dsc == (CDSC *)NULL)
		return FALSE;
	if (psfile.dsc->page_count == 0) {
		gserror(IDS_NOPAGE, NULL, MB_ICONEXCLAMATION, SOUND_NONUMBER);
		return FALSE;
	}
	// Make psfile.page_list.reverse sticky
        // psfile.page_list.reverse = FALSE;
	psfile.page_list.current = *ppage - 1;
	psfile.page_list.multiple = multiple;
	if (psfile.page_list.select == (BOOL *)NULL)
		return FALSE;

	memset(psfile.page_list.select, 0, psfile.dsc->page_count * sizeof(BOOL) );
	if (multiple) {
	    for (i=0; i< (int)(psfile.dsc->page_count); i++)
		psfile.page_list.select[i] = allpages;
	}
	psfile.page_list.select[psfile.page_list.current] = TRUE;

	if (psfile.page_list.multiple) {
	    dlgname = MAKEINTRESOURCE(IDD_MULTIPAGE);
	    lpProc = (DLGPROC)PageMultiDlgProc;
	}
	else {
	    dlgname = MAKEINTRESOURCE(IDD_PAGE);
	    lpProc = (DLGPROC)PageDlgProc;
	}
	flag = DialogBoxParamL(hlanguage, dlgname, hwndimg, lpProc, (LPARAM)NULL);
	if (flag && (psfile.page_list.current >= 0))
		*ppage = psfile.page_list.current + 1;
	return flag;
}


#ifndef VIEWONLY
#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
BoundingBoxDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
static int bboxindex;
float x, y;
TCHAR buf[MAXSTR];
    switch(message) {
	case WM_INITDIALOG:
	    bboxindex = 0;
	    load_string(IDS_BBPROMPT, buf, sizeof(buf));
	    SetDlgItemText(hDlg, BB_PROMPT, buf);
	    return TRUE;
	case WM_COMMAND:
	    switch(LOWORD(wParam)) {
		case BB_CLICK:
		    if (!get_cursorpos(&x, &y)) {
			DestroyWindow(hDlg);
			hDlgModeless = 0;
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
	    	        SetDlgItemText(hDlg, BB_PROMPT, buf);
			return FALSE;
		    }
		case IDCANCEL:
		    DestroyWindow(hDlg);
		    hDlgModeless = 0;
		    return TRUE;
	    }
	case WM_CLOSE:
	    DestroyWindow(hDlg);
	    hDlgModeless = 0;
	    return TRUE;
    }
    return FALSE;
}

BOOL
get_bbox(void)
{
	bbox.valid = FALSE;
	bbox.llx = bbox.lly = bbox.urx = bbox.ury = 0;
	if ((gsdll.state != GS_PAGE) && (gsdll.state != GS_IDLE)) {
	    gserror(IDS_EPSNOBBOX, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	    return FALSE;
	}
	hDlgModeless = CreateDialogParamL(hlanguage, MAKEINTRESOURCE(IDD_BBOX),
	    hwndimg, BoundingBoxDlgProc, (LPARAM)NULL);
	while (hDlgModeless) {
	    do_message();	/* wait for bounding box to be obtained */
	}
	return bbox.valid;
}

/* dialog box for warning PSTOEPS warning and auto/manual bbox selection */
#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
PSTOEPSDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
	switch (wmsg) {
	    case WM_INITDIALOG:
		if (option.auto_bbox)
		    SendDlgItemMessage(hDlg, PSTOEPS_AUTOBBOX, BM_SETCHECK, 1, 0);
		return TRUE;
	    case WM_COMMAND:
		switch (LOWORD(wParam)) {
		    case ID_HELP:
			get_help();
		        return FALSE;
		    case IDOK:
		    case IDYES:
			/* get Print to File status */
			option.auto_bbox = (int)SendDlgItemMessage(hDlg, PSTOEPS_AUTOBBOX, BM_GETCHECK, 0, 0);
			EndDialog(hDlg, IDYES);
			return TRUE;
		    case IDNO:
			get_help();
		    case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;
		}
		break;
	}
	return FALSE;
}


BOOL
pstoeps_warn(void)
{
int flag;
    nHelpTopic = IDS_TOPICPSTOEPS;
    flag = DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_PSTOEPS), hwndimg, 
	PSTOEPSDlgProc, (LPARAM)NULL);
    return (flag == IDYES);
}
#endif /* !VIEWONLY */


#ifdef __BORLANDC__
#pragma argsused
#endif
/* input string dialog box */
DLGRETURN CALLBACK _export
InstallDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
	    SetDlgItemTextA(hDlg, INSTALL_DLL, option.gsdll);
	    SetDlgItemTextA(hDlg, INSTALL_INCLUDE, option.gsinclude);
	    SetDlgItemTextA(hDlg, INSTALL_OTHER, option.gsother);
	    if (portable_app) {
    		EnableWindow(GetDlgItem(hDlg, IDC_CONFIGADV_START), FALSE);
    		EnableWindow(GetDlgItem(hDlg, IDC_CONFIGADV_PS), FALSE);
    		EnableWindow(GetDlgItem(hDlg, IDC_CONFIGADV_PDF), FALSE);
	    }
            return( TRUE);
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case ID_DEFAULT:
		    install_default(hDlg);
		    return(FALSE);
		case ID_HELP:
		    get_help();
		    return(FALSE);
		case IDOK:
		    /* do sanity check on the following strings */
		    GetDlgItemTextA(hDlg, INSTALL_DLL, option.gsdll, MAXSTR);
		    GetDlgItemTextA(hDlg, INSTALL_INCLUDE, option.gsinclude, MAXSTR);
		    GetDlgItemTextA(hDlg, INSTALL_OTHER, option.gsother, MAXSTR);
		    if (SendDlgItemMessage(hDlg, IDC_CONFIGADV_PRINTER, 
			BM_GETCHECK, 0, 0))
			gsview_printer_profiles();
		    if (SendDlgItemMessage(hDlg, IDC_CONFIGADV_START, 
			BM_GETCHECK, 0, 0))
			gsview_create_objects("Ghostgum");
		    {BOOL ps, pdf;
			ps = SendDlgItemMessage(hDlg, IDC_CONFIGADV_PS, 
			BM_GETCHECK, 0, 0);
			pdf = SendDlgItemMessage(hDlg, IDC_CONFIGADV_PDF, 
			BM_GETCHECK, 0, 0);
			if (ps || pdf)
			    update_registry(ps, pdf);
		    }
                    EndDialog(hDlg, TRUE);
                    return(TRUE);
                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return(TRUE);
                default:
                    return(FALSE);
            }
        default:
            return(FALSE);
    }
}

BOOL
install_gsdll(void)
{
BOOL flag;
/*
	nHelpTopic = IDS_TOPICINSTALL;
*/
	nHelpTopic = IDS_TOPICADVANCEDCFG;
	flag = DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_INSTALL), 
	    hwndimg, InstallDlgProc, (LPARAM)NULL);
	if (flag)
	    option.configured = TRUE;
	return flag;
}


TCHAR *depthlist[] =    {TEXT("Default"), TEXT("1"), TEXT("4"), 
	TEXT("8"), TEXT("24")};
int index_to_depth[] = { 0,         1,   4,   8,   24};
int depth_to_index(int depth)
{
int i;
    for (i=0; i<sizeof(index_to_depth)/sizeof(int); i++)
	if (index_to_depth[i] == depth)
	    return i;
    return 0;
}

TCHAR *alphalist[] =    {TEXT("1"), TEXT("2"), TEXT("4")};
int index_to_alpha[] = { 1,   2,   4};
int alpha_to_index(int alpha)
{
int i;
    for (i=0; i<sizeof(index_to_alpha)/sizeof(int); i++)
	if (index_to_alpha[i] == alpha)
	    return i;
    return 0;
}

void
enable_alpha(HWND hDlg)
{
    int i;
    i = (int)SendDlgItemMessage(hDlg, DSET_DEPTH, CB_GETCURSEL, 0, 0L);
    if (i == CB_ERR)
	return;
    i = real_depth(index_to_depth[i]);
    i = (i >= 8);
    EnableWindow(GetDlgItem(hDlg, DSET_TALPHA), i);
    EnableWindow(GetDlgItem(hDlg, DSET_GALPHA), i);
}


/* dialog box for display settings */
#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
DisplaySettingsDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
    char buf[128];
    TCHAR wbuf[128];
    int i;
    WORD notify_message;

    switch (wmsg) {
	case WM_INITDIALOG:
	    if (option.xdpi == option.ydpi)
		sprintf(buf,"%g", option.xdpi);
	    else 
		sprintf(buf,"%g %g", option.xdpi, option.ydpi);
	    SetDlgItemTextA(hDlg, DSET_RES, buf);
	    if (option.zoom_xdpi == option.zoom_ydpi)
		sprintf(buf,"%g", option.zoom_xdpi);
	    else 
		sprintf(buf,"%g %g", option.zoom_xdpi, option.zoom_ydpi);
	    SetDlgItemTextA(hDlg, DSET_ZOOMRES, buf);
	    SendDlgItemMessage(hDlg, DSET_DEPTH, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	    for (i=0; i<sizeof(depthlist)/sizeof(TCHAR *); i++) {
		lstrcpy(wbuf, depthlist[i]);
		if (lstrcmp(wbuf, TEXT("Default"))==0)
		    load_string(IDS_DEFAULT, wbuf, 
			sizeof(wbuf)/sizeof(TCHAR)-1);
	        SendDlgItemMessageL(hDlg, DSET_DEPTH, CB_ADDSTRING, 0, 
		    (LPARAM)((LPTSTR)wbuf));
	    }
    	    SendDlgItemMessage(hDlg, DSET_DEPTH, CB_SETCURSEL, depth_to_index(option.depth), (LPARAM)0);
	    SendDlgItemMessage(hDlg, DSET_TALPHA, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	    SendDlgItemMessage(hDlg, DSET_GALPHA, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	    for (i=0; i<sizeof(alphalist)/sizeof(TCHAR *); i++) {
	        SendDlgItemMessageL(hDlg, DSET_TALPHA, CB_ADDSTRING, 0, 
		    (LPARAM)((LPSTR)alphalist[i]));
	        SendDlgItemMessageL(hDlg, DSET_GALPHA, CB_ADDSTRING, 0, 
		    (LPARAM)((LPSTR)alphalist[i]));
	    }
	    enable_alpha(hDlg);
    	    SendDlgItemMessage(hDlg, DSET_TALPHA, CB_SETCURSEL, alpha_to_index(option.alpha_text), (LPARAM)0);
    	    SendDlgItemMessage(hDlg, DSET_GALPHA, CB_SETCURSEL, alpha_to_index(option.alpha_graphics), (LPARAM)0);
	    return TRUE;
	case WM_COMMAND:
	    notify_message = GetNotification(wParam,lParam);
	    switch (LOWORD(wParam)) {
		case ID_HELP:
		    nHelpTopic = IDS_TOPICDSET;
		    get_help();
		    return FALSE;
		case DSET_DEPTH:
		    if (notify_message == CBN_SELCHANGE)
			enable_alpha(hDlg);
		    return FALSE;
		case IDOK:
		    {
		    BOOL unzoom = FALSE;
		    BOOL resize = FALSE;
	            BOOL restart = FALSE;
		    float x, y;
		    GetDlgItemTextA(hDlg, DSET_RES, buf, sizeof(buf)-2);
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
		    GetDlgItemTextA(hDlg, DSET_ZOOMRES, buf, sizeof(buf)-2);
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
    		    i = (int)SendDlgItemMessage(hDlg, DSET_DEPTH, CB_GETCURSEL, 0, 0L);
		    i = index_to_depth[i];
		    if (i != option.depth) {
			option.depth = i;
			restart = TRUE;
			resize = TRUE; 
			unzoom = TRUE;
		    }
    		    i = (int)SendDlgItemMessage(hDlg, DSET_TALPHA, CB_GETCURSEL, 0, 0L);
		    i = index_to_alpha[i];
		    if (i != option.alpha_text) {
			option.alpha_text = i;
			restart = TRUE;
			resize = TRUE; 
			unzoom = TRUE;
		    }
    		    i = (int)SendDlgItemMessage(hDlg, DSET_GALPHA, CB_GETCURSEL, 0, 0L);
		    i = index_to_alpha[i];
		    if (i != option.alpha_graphics) {
			option.alpha_graphics = i;
			/* restart = TRUE; */
			resize = TRUE; 
			unzoom = TRUE;
		    }
		    if (resize) {
			if (unzoom)
			    gsview_unzoom();
			if (gsdll.state != GS_UNINIT) {
/* gs_resize has this check 
			    if (option.redisplay && (gsdll.state == GS_PAGE) && (psfile.dsc != (CDSC *)NULL))
*/
				gs_resize();
			    /* for those that can't be changed with a */
			    /* postscript command so must close gs */
			    if (restart)
				pending.restart = TRUE;
			}
		    }
		    EndDialog(hDlg, TRUE);
		    }
		    return TRUE;
		case IDCANCEL:
		    EndDialog(hDlg, FALSE);
		    return TRUE;
	    }
	    break;
    }
    return FALSE;
}

void
display_settings()
{
	DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_DSET), hwndimg, 
	    DisplaySettingsDlgProc, (LPARAM)NULL);
}



/* Text Window for Ghostscript Messages */
/* uses MS-Windows multiline edit field */


#ifdef __WIN32__
#define TWLENGTH 61440
#else
#define TWLENGTH 16384
#endif
#define TWSCROLL 2048
char twbuf[TWLENGTH];
int twend;


#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
TextDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
	    /* Never translate this one */
            SetDlgItemTextA(hDlg, TEXTWIN_MLE, twbuf);
	    {
	    DWORD linecount;
	    /* EM_SETSEL, followed by EM_SCROLLCARET doesn't work */
	    linecount = SendDlgItemMessage(hDlg, TEXTWIN_MLE, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
	    SendDlgItemMessage(hDlg, TEXTWIN_MLE, EM_LINESCROLL, (WPARAM)0, (LPARAM)linecount-18);
	    }
            return(TRUE);
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return(TRUE);
                case IDOK:
                    EndDialog(hDlg, TRUE);
                    return(TRUE);
		case ID_HELP:
		    get_help();
		    return(FALSE);
		case TEXTWIN_COPY:
		    {HGLOBAL hglobal;
		    LPSTR p;
		    DWORD result;
		    int start, end;
		    result = SendDlgItemMessage(hDlg, TEXTWIN_MLE, EM_GETSEL, (WPARAM)0, (LPARAM)0);
		    start = LOWORD(result);
		    end   = HIWORD(result);
		    if (start == end) {
			start = 0;
			end = twend;
		    }
		    hglobal = GlobalAlloc(GHND | GMEM_SHARE, end-start+1);
		    if (hglobal == (HGLOBAL)NULL) {
			MessageBeep(-1);
			return(FALSE);
		    }
		    p = (LPSTR)GlobalLock(hglobal);
		    if (p == (LPSTR)NULL) {
			MessageBeep(-1);
			return(FALSE);
		    }
		    strncpy(p, twbuf+start, end-start);
		    GlobalUnlock(hglobal);
		    OpenClipboard(hwndimg);
		    EmptyClipboard();
		    SetClipboardData(CF_TEXT, hglobal);
		    CloseClipboard();
		    }
                default:
                    return(FALSE);
            }
        default:
            return(FALSE);
    }
}

/* display dialog box with multiline edit control */
void 
gs_showmess(void)
{
    nHelpTopic = IDS_TOPICMESS;
    DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_TEXTWIN), hwndimg, 
	TextDlgProc, (LPARAM)NULL);
}


/* Add string for Ghostscript message window */
void
gs_addmess_count(const char *str, int count)
{
char *p;
const char *s;
int i, lfcount;
    /* if debugging, write to a log file */
    if (debug & DEBUG_LOG) {
	FILE *f = fopen("c:\\gsview.txt", "a");
	if (f != (FILE *)NULL) {
	    fwrite(str, 1, count, f);
	    fclose(f);
	}
    }

    /* we need to add \r after each \n, so count the \n's */
    lfcount = 0;
    s = str;
    for (i=0; i<count; i++) {
	if (*s == '\n')
	    lfcount++;
	s++;
    }
    if (count + lfcount >= TWSCROLL)
	return;		/* too large */
    if (count + lfcount + twend >= TWLENGTH-1) {
	/* scroll buffer */
	twend -= TWSCROLL;
	memmove(twbuf, twbuf+TWSCROLL, twend);
    }
    p = twbuf+twend;
    for (i=0; i<count; i++) {
	if (*str == '\n') {
	    *p++ = '\r';
	}
	if (*str == '\0') {
	    *p++ = ' ';	/* ignore null characters */
	    str++;
	}
	else
	    *p++ = *str++;
    }
    twend += (count + lfcount);
    *(twbuf+twend) = '\0';
}

void
gs_addmess(const char *str)
{
    gs_addmess_count(str, strlen(str));
}


#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
DSCErrorDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
            SetDlgItemTextA(hDlg, DSC_STATIC_TEXT, (char *)lParam);
	    centre_dialog(hDlg);
            return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case IDOK:
                    EndDialog(hDlg, CDSC_RESPONSE_OK);
                    return(TRUE);
                case IDCANCEL:
                    EndDialog(hDlg, CDSC_RESPONSE_CANCEL);
                    return(TRUE);
                case DSC_IGNORE_ALL:
                    EndDialog(hDlg, CDSC_RESPONSE_IGNORE_ALL);
                    return(TRUE);
		case ID_HELP:
		    get_help();
		    return(FALSE);
                default:
                    return(FALSE);
            }
        default:
            return(FALSE);
    }
}


int 
get_dsc_response(char *message)
{
    nHelpTopic = IDS_TOPICDSCWARN;
    return DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_DSCERROR), hwndimg, 
	DSCErrorDlgProc, (LPARAM)message);
}
