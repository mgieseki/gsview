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

/* gvwmisc.c */
/* Miscellaneous Windows GSview routines */

#include "gvwin.h"

/* SetDlgItemText is a Windows API */

void
post_img_message(int message, int param)
{
    PostMessage(hwndimg, message, (WPARAM)param, (LPARAM)0);
}

void
get_help(void)
{
    SendMessage(hwndimg, help_message, 0, 0L);
}

/* display message */
int message_box(LPCTSTR str, int icon)
{
    return MessageBoxL(hwndimg, str, szAppName, icon | MB_OK);
}

/* display message */
int message_box_a(LPCSTR str, int icon)
{
    TCHAR wbuf[MAXSTR];
    convert_multibyte(wbuf, str, sizeof(wbuf)/sizeof(TCHAR)-1);
    return MessageBoxL(hwndimg, wbuf, szAppName, icon | MB_OK);
}

void
delayed_message_box(int id, int icon)
{
    PostMessage(hwndimg, WM_GSMESSBOX, (WPARAM)id, (LPARAM)icon);
}

#ifdef __BORLANDC__
#pragma argsused
#endif
/* change menu item checkmark */
void
check_menu_item(int menuid, int itemid, BOOL checked)
{
        CheckMenuItem(hmenu, itemid, MF_BYCOMMAND | (checked ? MF_CHECKED : MF_UNCHECKED));
}

#ifdef __BORLANDC__
#pragma argsused
#endif
/* get text of menu item */
int
get_menu_string(int menuid, int itemid, char *str, int len)
{
    return GetMenuStringA(hmenu, itemid, str, len, MF_BYCOMMAND);
}

int
load_string(int id, LPTSTR str, int len)
{
    return LoadString(hlanguage, id, str, len);
}

int
load_string_a(int id, LPSTR str, int len)
{
    return LoadStringA(hlanguage, id, str, len);
}

int
load_resource(int resource, char *str, int len)
{  
HGLOBAL hglobal;
LPSTR prolog;
int code = -1;
	hglobal = LoadResource(phInstance, 
	    FindResource(phInstance, MAKEINTRESOURCE(resource), RT_RCDATA));
	if ( (prolog = (LPSTR)LockResource(hglobal)) != (LPSTR)NULL) {
	    if ((int)(strlen(prolog) + 1) < len)
		strncpy(str, prolog, len);
	    FreeResource(hglobal);
	}
	return code;
}


void
play_sound(int num)
{
	if (lstrlen(sound[num].file)==0)
		return;
	if (lstrcmp(sound[num].file,TEXT(BEEP))==0) {
		MessageBeep(-1);
		return;
	}
	if (lpfnSndPlaySound != (FPSPS)NULL) 
	    lpfnSndPlaySound(sound[num].file, SND_SYNC);
	else
	    MessageBeep(-1);
}


/* display or remove 'wait' message */
void
info_wait(int id)
{
HWND hwnd;
POINT pt;
	if (id)
    	    load_string(id, szWait, sizeof(szWait));
	else 
	    szWait[0] = '\0';

	InvalidateRect(hwndimg, (LPRECT)&info_rect, FALSE);
	UpdateWindow(hwndimg);

	if (szWait[0] != '\0') {
            GetCursorPos(&pt);
	    hwnd = WindowFromPoint(pt);
	    if ((hwnd == hwndimg) || IsChild(hwndimg,hwnd))
		SetCursor(hcWait);
	}
	else {
	    /* set cursor to that of active window */
	    hwnd = GetActiveWindow();
	    if (hwnd == hwnd_fullscreen) {
	        SetCursor(NULL);
		return;
	    }
	    if ( (image.open)
	      && ((hwnd == hwndimg) || (hwnd == hwndimgchild)) ) {
		if (in_child_client_area()) {
		    SetCursor(GetClassCursor(hwndimgchild));
		    return;
		}
	    }
	    SetCursor(GetClassCursor(hwnd));
	}
}

/* change directory and drive */
int
gs_chdir(char *dirname)
{
    return !SetCurrentDirectoryA(dirname);
}

char * 
gs_getcwd(char *dirname, int size)
{
    GetCurrentDirectoryA(size, dirname);
    return dirname;
}


int
send_prolog(int resource)
{  
HGLOBAL hglobal;
LPSTR prolog;
int code = -1;
	hglobal = LoadResource(phInstance, 
	    FindResource(phInstance, MAKEINTRESOURCE(resource), RT_RCDATA));
	if ( (prolog = (LPSTR)LockResource(hglobal)) != (LPSTR)NULL) {
	    code = gs_execute(prolog, strlen(prolog));
	    FreeResource(hglobal);
	}
	return code;
}

void
profile_create_section(PROFILE *prf, const char *section, int id)
{  
HGLOBAL hglobal;
LPSTR entry, value;
char name[MAXSTR];
char val[256];
	hglobal = LoadResource(phInstance, 
	    FindResource(phInstance, MAKEINTRESOURCE(id), RT_RCDATA));
	if ( (entry = (LPSTR)LockResource(hglobal)) == (LPSTR)NULL)
	    return;
	while (strlen(entry)!=0) {
	    for ( value = entry; 
		  (*value!='\0') && (*value!=',') && (*value!='='); 
		  value++)
		/* nothing */;
	    strncpy(name, entry, (int)(value-entry));
	    name[(int)(value-entry)] = '\0';
	    value++;
	    strncpy(val, value, sizeof(val));
	    profile_write_string(prf, section, name, val);
	    entry = value + strlen(value) + 1;
	}
	FreeResource(hglobal);
}
