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

/* gvwcp.c */
/* Code page access to resources */
#include "gvwin.h"

#ifndef UNICODE
/* 
 * The system code page is nCodePageSystem.
 * The GSview language uses nCodePageLanguage.
 * If the two differ and we are using NT, then use
 * wide versions of the APIs to allow the foreign
 * language stuff to display correctly.
 *
 * This isn't needed if the system code page matches the 
 * GSview language which will usually be the case if someone
 * is using GSview in their native language, but it is not
 * the case when the author of GSview is testing languages
 * such as Greek.
 */


/* Use a Unicode message box if Windows NT and GSview language
 * doesn't match system code page.
 */
int MessageBoxL(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
    if (is_winnt && (nCodePageLanguage != nCodePageSystem)) {
	WCHAR wtext[MAXSTR+MAXSTR];
	WCHAR wcaption[MAXSTR+MAXSTR];
	MultiByteToWideChar(nCodePageLanguage, 0,
	    lpText, strlen(lpText)+1,
	    wtext, sizeof(wtext)/sizeof(WCHAR));
	MultiByteToWideChar(nCodePageLanguage, 0,
	    lpCaption, strlen(lpCaption)+1,
	    wcaption, sizeof(wcaption)/sizeof(WCHAR));
	return MessageBoxW(hWnd, wtext, wcaption, uType);
    }
    return MessageBox(hWnd, lpText, lpCaption, uType);
}

BOOL GetOpenSaveFileNameL(LPOPENFILENAME pofn, BOOL save)
{
    BOOL flag;
    if (is_winnt && (nCodePageLanguage != nCodePageSystem)) {
	OPENFILENAMEW wofn;
	WCHAR wszFile[MAXSTR];
	WCHAR wszTitle[MAXSTR];
	WCHAR wszFilter[1024];		/* filter for OFN */
	memcpy(&wofn, pofn, sizeof(wofn));
	/* Convert lpstrTitle, lpstrFilter, lpstrFile */
	if (pofn->lpstrTitle) {
	    MultiByteToWideChar(nCodePageLanguage, 0,
		pofn->lpstrTitle, strlen(pofn->lpstrTitle)+1,
		wszTitle, sizeof(wszTitle)/sizeof(WCHAR));
	    wofn.lpstrTitle = wszTitle;
	}
	if (pofn->lpstrFilter) {
	    int len = 0;
	    while (pofn->lpstrFilter[len])
		len += strlen(pofn->lpstrFilter+len)+1;
	    MultiByteToWideChar(nCodePageLanguage, 0,
		pofn->lpstrFilter, len+1,
		wszFilter, sizeof(wszFilter)/sizeof(WCHAR));
	    wofn.lpstrFilter = wszFilter;
	}
	MultiByteToWideChar(nCodePageLanguage, 0,
	    pofn->lpstrFile, strlen(pofn->lpstrFile)+1,
	    wszFile, sizeof(wszFile)/sizeof(WCHAR));
	wofn.lpstrFile = wszFile;
	if (save)
	    flag = GetSaveFileNameW(&wofn);
	else
	    flag = GetOpenFileNameW(&wofn);
	WideCharToMultiByte(nCodePageLanguage, 0,
		wofn.lpstrFile, wcslen(wofn.lpstrFile)+1,
		pofn->lpstrFile, MAXSTR-1, NULL, NULL);
	return flag;
    }
    if (save)
	flag = GetSaveFileName(&ofn);
    else
	flag = GetOpenFileName(&ofn);
    return flag;
}
/* Use Unicode dialog box if Windows NT and GSview language
 * doesn't match system code page.
 */
BOOL DialogBoxParamL(HINSTANCE hInstance, LPCTSTR lpTemplateName,
    HWND hWndParent, DLGPROC lpDialogProc, LPARAM dwInitParam)
{
    if (is_winnt && (nCodePageLanguage != nCodePageSystem))
        return DialogBoxParamW(hlanguage, (LPCWSTR)lpTemplateName,
	    hWndParent, lpDialogProc, dwInitParam);
    return DialogBoxParamA(hlanguage, lpTemplateName,
        hWndParent, lpDialogProc, dwInitParam);
}

HWND CreateDialogParamL(HINSTANCE hInstance, LPCTSTR lpTemplateName,
    HWND hWndParent, DLGPROC lpDialogProc, LPARAM dwInitParam)
{
    if (is_winnt && (nCodePageLanguage != nCodePageSystem))
        return CreateDialogParamW(hlanguage, (LPCWSTR)lpTemplateName,
	    hWndParent, lpDialogProc, dwInitParam);
    return CreateDialogParamA(hlanguage, lpTemplateName,
        hWndParent, lpDialogProc, dwInitParam);
}


/* We can't set font of a dialog control to hGreekFont because
 * Windows ignores the GREEK_CHARSET of the font and uses the
 * system default code page instead.
 * To work around this we convert text back to Unicode and
 * use the wide version for setting text.
 */
BOOL SetDlgItemTextL(HWND hDlg, int nlDDlgItem, LPCTSTR lpString)
{
    if (is_winnt && (nCodePageLanguage != nCodePageSystem)) {
	WCHAR wbuf[MAXSTR+MAXSTR];
	MultiByteToWideChar(nCodePageLanguage, 0,
	    lpString, strlen(lpString)+1,
	    wbuf, sizeof(wbuf)/sizeof(WCHAR));
        return SetDlgItemTextW(hDlg, nlDDlgItem, wbuf);
    }
    return SetDlgItemTextA(hDlg, nlDDlgItem, lpString);
}

/* Get the Unicode text and convert back to GSview language */
BOOL GetDlgItemTextL(HWND hDlg, int nlDDlgItem, LPSTR lpString, int nMaxCount)
{
    if (is_winnt && (nCodePageLanguage != nCodePageSystem)) {
	BOOL flag;
	WCHAR wbuf[MAXSTR];
        flag = GetDlgItemTextW(hDlg, nlDDlgItem, wbuf, MAXSTR-1);
	if (flag) {
	    WideCharToMultiByte(nCodePageLanguage, 0,
		wbuf, lstrlenW(wbuf)+1, lpString, nMaxCount, NULL, NULL);
	}
	return flag;
    }
    return GetDlgItemTextA(hDlg, nlDDlgItem, lpString, nMaxCount);
}


/* Strings are stored in resources in Unicode.
 * As they are loaded they are converted to the system
 * default code page.  If we are trying to display
 * Greek (ACP 1253) on a system with the Western 
 * European code page (ACP 1252) as default, the 
 * the characters which are not in ACP 1252 will
 * be replaced by question marks.
 * To workaround this we load the string as Unicode
 * then convert it to ACP 1253 outselves.
 * We will then display it in a suitable font.
 * We can display it ourselves by creaing a font with
 * lfCharSet=GREEK_CHARSET, but we can't display it
 * in a dialog box control that way.  There is a bug
 * in Windows NT4 and 2000 that causes it to ignore
 * the character set and always use the system code
 * page when the text is displayed in a dialog box.
 * Instead we need to convert it back to Unicode
 * to set the text of a dialog control.
 */
BOOL LoadStringL(HINSTANCE hlanguage, UINT id, LPTSTR str, int len)
{
    if (is_winnt && (nCodePageLanguage != nCodePageSystem)) {
	WCHAR wcBuf[MAXSTR*4];
 	int cchWideChar;
	int cchMultiByte;
	cchWideChar = LoadStringW(hlanguage, id, wcBuf, 
	    sizeof(wcBuf)/sizeof(WCHAR)-1);
	if (cchWideChar) {
	    cchMultiByte = WideCharToMultiByte(nCodePageLanguage, 0,
		wcBuf, cchWideChar+1, str, len, NULL, NULL);
	}
	if (cchMultiByte > 0)
	    return cchMultiByte-1;
	return 0;
    }
    return LoadStringA(hlanguage, id, str, len);
}

/* Convert lParam string to Unicode and call unicode API */
/* Used with CB_ADDSTRING, LB_ADDSTRING, LB_INSERTSTRING, CB_SELECTSTRING */
LONG
SendDlgItemMessageL(HWND hDlg, int id, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (is_winnt && (nCodePageLanguage != nCodePageSystem)) {
	LPSTR lpString = (LPSTR)lParam;
	WCHAR wbuf[MAXSTR];
	MultiByteToWideChar(nCodePageLanguage, 0,
	    lpString, strlen(lpString)+1,
	    wbuf, sizeof(wbuf)/sizeof(WCHAR));
        return SendDlgItemMessageW(hDlg, id, msg, wParam, (LPARAM)wbuf);
    }
    return SendDlgItemMessageA(hDlg, id, msg, wParam, lParam);
}

/* Convert Unicode API then convert Unicode lParam string MultiByte */
/* Used with CB_GETLBTEXT */
LONG
SendDlgItemMessageLGetString(HWND hDlg, int id, UINT msg, WPARAM wParam, 
  LPARAM lParam)
{
    if (is_winnt && (nCodePageLanguage != nCodePageSystem)) {
	LPSTR lpString = (LPSTR)lParam;
	WCHAR wbuf[MAXSTR];
 	int cchWideChar;
	int cchMultiByte;
	cchWideChar = SendDlgItemMessageW(hDlg, id, msg, wParam, (LPARAM)wbuf);
	cchMultiByte = WideCharToMultiByte(nCodePageLanguage, 0,
		wbuf, cchWideChar+1, lpString, MAXSTR, NULL, NULL);
        return cchMultiByte;
    }
    return SendDlgItemMessageA(hDlg, id, msg, wParam, lParam);
}

#endif /* !UNICODE */

/* Convert a multibyte string str to a wide or multibyte string */
int
convert_multibyte(LPTSTR str, LPCSTR mbstr, int len)
{
#ifdef UNICODE
    MultiByteToWideChar(nCodePageLanguage, 0,
	mbstr, strlen(mbstr)+1, str, len);
    return lstrlen(str);
#else
    strncpy(str, mbstr, len);
    return strlen(str);
#endif
}

/* Convert a wide or multibyte string to multibyte string */
int
convert_widechar(LPSTR mbstr, LPCTSTR str, int len)
{
#ifdef UNICODE
    return WideCharToMultiByte(nCodePageLanguage, 0,
	str, lstrlen(str)+1, mbstr, len, NULL, NULL);
#else
    strncpy(mbstr, str, len);
    return strlen(str);
#endif
}

