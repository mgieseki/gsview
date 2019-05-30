/* Copyright (C) 1993-2006, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwprn.c */
/* Printer routines for Windows GSview */
#include "gvwin.h"

#ifndef DS_FIXEDSYS
/* as defined in Windows 2000 April SDK */
#define DS_FIXEDSYS 0x0008L
#endif
#ifndef DS_SHELLFONT
/* as defined in Windows 2000 April SDK */
#define DS_SHELLFONT (DS_SETFONT | DS_FIXEDSYS)
#endif


/* documented in Device Driver Adaptation Guide */
/* Prototypes taken from print.h */
DECLARE_HANDLE(HPJOB);

HPJOB   WINAPI OpenJob(LPSTR, LPSTR, HPJOB);
int     WINAPI StartSpoolPage(HPJOB);
int     WINAPI EndSpoolPage(HPJOB);
int     WINAPI WriteSpool(HPJOB, LPSTR, int);
int     WINAPI CloseJob(HPJOB);
int     WINAPI DeleteJob(HPJOB, int);
int     WINAPI WriteDialog(HPJOB, LPSTR, int);
int     WINAPI DeleteSpoolPage(HPJOB);

char not_defined[] = "[Not defined]";
char * get_ports(void);
char * get_queues(void);
#define PORT_BUF_SIZE 4096

PRINTDLG pd;	/* global */

DLGRETURN CALLBACK _export
NewDeviceDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam);

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

/* dialog for adding or editing properties */
DLGRETURN CALLBACK _export
EditPropDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
static char device[MAXSTR];	/* contains printer device name */
PROFILE *prf;
    switch (wmsg) {
	case WM_INITDIALOG:
	    /* initialise device name */
	    strncpy(device, (LPSTR)lParam, sizeof(device)-1);	
	    if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
	      if (*editpropname) {
		  char section[MAXSTR];
		  char buf[MAXSTR];
		  if (*editpropname == 's')
		      SendDlgItemMessage(hDlg, EDITPROP_STRING, BM_SETCHECK, 
			      (WPARAM)1, 0L);
		  else
		      SendDlgItemMessage(hDlg, EDITPROP_NUMBER, BM_SETCHECK, 
			      (WPARAM)1, 0L);
		  SetDlgItemTextA(hDlg, EDITPROP_NAME, editpropname+1);
		  strncpy(section, device, sizeof(section)-1);
		  strncat(section, " values", sizeof(section)-strlen(section)-1);
		  profile_read_string(prf, section, editpropname, "", 
			buf, sizeof(buf)-2);
		  SetDlgItemTextA(hDlg, EDITPROP_VALUE, buf);
	      }
	      profile_close(prf);
	    }
	    else
	        SendDlgItemMessage(hDlg, EDITPROP_NUMBER, BM_SETCHECK, (WPARAM)1, 0L);
	    return TRUE;
	case WM_COMMAND:
	  switch (LOWORD(wParam)) {
	    case EDITPROP_DEL:
		{
		char name[MAXSTR];
		char section[MAXSTR];
		if ( SendDlgItemMessage(hDlg, EDITPROP_STRING, 
		    BM_GETCHECK, (WPARAM)1, 0L) > 0) {
		    strcpy(name, "s");
		}
		else
		    strcpy(name, "d");
		GetDlgItemTextA(hDlg, EDITPROP_NAME, name+1, sizeof(name)-2);
		strip_spaces(name);
		if (strlen(name)>1) {
		    strcpy(section, device);
		    strcat(section, " values");
		    if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		      profile_write_string(prf, section, name, NULL);
		      profile_write_string(prf, device, name, NULL);
		      profile_close(prf);
		    }
		}
		EndDialog(hDlg, TRUE);
		}
		return TRUE;
	    case ID_HELP:
		get_help();
		return(FALSE);
	    case IDOK:
		{
		char name[MAXSTR];
		char value[MAXSTR];
		char section[MAXSTR];
		if ( SendDlgItemMessage(hDlg, EDITPROP_STRING, 
		    BM_GETCHECK, (WPARAM)1, 0L) > 0) {
		    strcpy(name, "s");
		}
		else
		    strcpy(name, "d");
		GetDlgItemTextA(hDlg, EDITPROP_NAME, name+1, sizeof(name)-2);
		GetDlgItemTextA(hDlg, EDITPROP_VALUE, value, sizeof(value)-1);
		strip_spaces(name);
		strip_spaces(value);
		if ((strlen(name)>1) && strlen(value)) {
		    strcpy(section, device);
		    strcat(section, " values");
		    if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
			profile_write_string(prf, section, name, value);
			strtok(value, ",");
			profile_write_string(prf, device, name, value);
			profile_close(prf);
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

/* dialog box for selecting printer properties */
DLGRETURN CALLBACK _export
PropDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
    char buf[128];
    TCHAR wbuf[128];
    int iprop;
    int ivalue;
    WORD notify_message;
    char *p;
    char *value;
    static TCHAR notdef[128];
    static char device[MAXSTR];	/* contains printer device name */
    static struct prop_item_s* propitem;
    char section[MAXSTR];
    PROFILE *prf;

    switch (wmsg) {
	case WM_INITDIALOG:
	    strcpy(device, (LPSTR)lParam);	/* initialise device name */
	    load_string(IDS_NOTDEFTAG, notdef, sizeof(notdef));
	    propitem = get_properties(device);
	    SendDlgItemMessage(hDlg, PROP_NAME, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	    SendDlgItemMessage(hDlg, PROP_VALUE, CB_RESETCONTENT, (WPARAM)0, (LPARAM)0);
	    for (iprop=0; propitem[iprop].name[0]; iprop++) {
		SendDlgItemMessageA(hDlg, PROP_NAME, CB_ADDSTRING, 0, 
		    (LPARAM)((LPSTR)propitem[iprop].name+1));
	    }
	    SendDlgItemMessage(hDlg, PROP_NAME, CB_SETCURSEL, 0, 0L);
	    /* force update of PROP_VALUE */
	    SendDlgNotification(hDlg, PROP_NAME, CBN_SELCHANGE);
	    EnableWindow(GetDlgItem(hDlg, PROP_NAME), (iprop != 0));
	    EnableWindow(GetDlgItem(hDlg, PROP_VALUE), (iprop != 0));
	    EnableWindow(GetDlgItem(hDlg, PROP_EDIT), (iprop != 0));

	    if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		strcpy(section, device);
		strcat(section, " Options");
		profile_read_string(prf, section, "Xoffset", "0", 
		    buf, sizeof(buf)-2);
		SetDlgItemTextA(hDlg, PROP_XOFFSET, buf);
		profile_read_string(prf, section, "Yoffset", "0", 
		    buf, sizeof(buf)-2);
		SetDlgItemTextA(hDlg, PROP_YOFFSET, buf);
		profile_close(prf);
	    }

	    SetFocus(GetDlgItem(hDlg, IDOK));
	    return TRUE;
	case WM_COMMAND:
	    notify_message = GetNotification(wParam,lParam);
	    switch (LOWORD(wParam)) {
		case ID_HELP:
		    nHelpTopic = IDS_TOPICPROP;
		    get_help();
		    return(FALSE);
		case PROP_NAME:
		    if (notify_message != CBN_SELCHANGE) {
			    return FALSE;
		    }
		    iprop = (int)SendDlgItemMessage(hDlg, PROP_NAME, CB_GETCURSEL, 0, 0L);
		    if (iprop == CB_ERR) {
			return FALSE;
		    }
		    /* now look up entry in gsview.ini */
		    /* and update PROP_VALUE list box */
		    SendDlgItemMessage(hDlg, PROP_VALUE, CB_RESETCONTENT, 0, 0L);
		    SendDlgItemMessageL(hDlg, PROP_VALUE, CB_ADDSTRING, 0, 
			(LPARAM)((LPSTR)notdef));
		    strcpy(section, device);
		    strcat(section, " values");
		    if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		      profile_read_string(prf, section, propitem[iprop].name, 
			"", buf, sizeof(buf)-2);
		      profile_close(prf);
		    }
		    buf[strlen(buf)+1] = '\0';	/* put double NULL at end */
		    p = buf;
		    if (*p != '\0') {
		      EnableWindow(GetDlgItem(hDlg, PROP_VALUE), TRUE);
		      while (*p!='\0') {
			value = p;
			while ((*p!='\0') && (*p!=','))
			    p++;
			*p++ = '\0';
			SendDlgItemMessageA(hDlg, PROP_VALUE, CB_ADDSTRING, 0, 
			    (LPARAM)((LPSTR)value));
		      }
		    }

		    strcpy(buf, propitem[iprop].value);
		    if (strcmp(buf, not_defined)==0) {
			lstrcpy(wbuf, notdef);
		    }
		    else {
			convert_multibyte(wbuf, buf, 
			    sizeof(wbuf)/sizeof(TCHAR));
		    }
		    SendDlgItemMessageL(hDlg, PROP_VALUE, CB_SELECTSTRING, -1, 
			(LPARAM)(LPTSTR)wbuf);
		    SetDlgItemTextL(hDlg, PROP_VALUE, wbuf);
		    return FALSE;
		case PROP_VALUE:
		    if (notify_message == CBN_SELCHANGE) {
			iprop = (int)SendDlgItemMessage(hDlg, PROP_NAME, CB_GETCURSEL, 0, 0L);
			if (iprop == CB_ERR)
			    return FALSE;
			ivalue = (int)SendDlgItemMessage(hDlg, PROP_VALUE, CB_GETCURSEL, 0, 0L);
			if (ivalue == CB_ERR)
			    return FALSE;
			SendDlgItemMessageLGetString(hDlg, 
			    PROP_VALUE, CB_GETLBTEXT, 
			    ivalue, (LPARAM)(LPTSTR)wbuf);
			if (lstrcmp(wbuf, notdef)==0)
			    strcpy(propitem[iprop].value, not_defined);
			else
			    convert_widechar(propitem[iprop].value, wbuf,
				sizeof(propitem[iprop].value)/sizeof(TCHAR)-1);
		    }
		    if (notify_message == CBN_EDITCHANGE) {
			iprop = (int)SendDlgItemMessage(hDlg, PROP_NAME, CB_GETCURSEL, 0, 0L);
			if (iprop == CB_ERR)
			    return FALSE;
			GetDlgItemTextL(hDlg, PROP_VALUE, 
			    wbuf, sizeof(wbuf)/sizeof(TCHAR)-1);
			if (lstrcmp(wbuf, notdef)==0)
			    strcpy(propitem[iprop].value, not_defined);
			else
			    convert_widechar(propitem[iprop].value, wbuf,
				sizeof(propitem[iprop].value)/sizeof(TCHAR)-1);
		    }
		    return FALSE;
		case PROP_EDIT:
		    nHelpTopic = IDS_TOPICEDITPROP;
		    iprop = (int)SendDlgItemMessage(hDlg, PROP_NAME, CB_GETCURSEL, 0, 0L);
		    editpropname[0] = '\0';
		    if (iprop != CB_ERR)
			strcpy(editpropname, propitem[iprop].name);
		    DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_EDITPROP), 
			hDlg, EditPropDlgProc, (LPARAM)device);
		    free((char *)propitem);
		    SendMessage(hDlg, WM_INITDIALOG, (WPARAM)hDlg, (LPARAM)device);
		    SendDlgItemMessage(hDlg, IDOK, BM_SETSTYLE, 
			    (WPARAM)BS_DEFPUSHBUTTON, MAKELPARAM(TRUE, 0));
		    SendDlgItemMessage(hDlg, PROP_EDIT, BM_SETSTYLE, 
			    (WPARAM)BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));
		    nHelpTopic = IDS_TOPICPROP;
		    return FALSE;
		case PROP_NEW:
		    nHelpTopic = IDS_TOPICEDITPROP;
		    editpropname[0] = '\0';
		    DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_EDITPROP), 
			hDlg, EditPropDlgProc, (LPARAM)device);
		    free((char *)propitem);
		    SendMessage(hDlg, WM_INITDIALOG, (WPARAM)hDlg, (LPARAM)device);
		    SendDlgItemMessage(hDlg, IDOK, BM_SETSTYLE, 
			    (WPARAM)BS_DEFPUSHBUTTON, MAKELPARAM(TRUE, 0));
		    SendDlgItemMessage(hDlg, PROP_NEW, BM_SETSTYLE, 
			    (WPARAM)BS_PUSHBUTTON, MAKELPARAM(TRUE, 0));
		    nHelpTopic = IDS_TOPICPROP;
		    return FALSE;
		case IDOK:
		    if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
			for (iprop=0; propitem[iprop].name[0]; iprop++) {
			    profile_write_string(prf, device, 
				propitem[iprop].name, propitem[iprop].value);
		      }
		      strcpy(section, device);
		      strcat(section, " Options");
		      GetDlgItemTextA(hDlg, PROP_XOFFSET, buf, sizeof(buf)-2);
		      profile_write_string(prf, section, "Xoffset", buf);
		      GetDlgItemTextA(hDlg, PROP_YOFFSET, buf, sizeof(buf)-2);
		      profile_write_string(prf, section, "Yoffset", buf);
		      profile_close(prf);
		    }
		    free((char *)propitem);
		    EndDialog(hDlg, TRUE);
		    return TRUE;
		case IDCANCEL:
		    free((char *)propitem);
		    EndDialog(hDlg, FALSE);
		    return TRUE;
	    }
	    break;
    }
    return FALSE;
}


LPSTR
GetUPPname(HWND hwnd, LPSTR uppname, int upplen)
{
    LPSTR p, q;
    uppname[0] = '\0';
    GetDlgItemTextA(GetParent(hwnd), 
	    DEVICE_OPTIONS, (LPSTR)uppname, upplen);
    p = uppname;
    if (p[0] == '"') {
	/* remove quotes around configuration file */
	p++;
	q = strchr(p, '"');
	if (q != (LPSTR)NULL)
	    *q = '\0';
    }
    if (p[0] == '@')
	p++;
    if (strlen(p))
        memmove(uppname, p, strlen(p)+1);
    return uppname;
}

/* dialog box for selecting uniprint configuration file */
#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
UniDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
    WORD notify_message;
    LPSTR p, q;
    char uppname[MAXSTR];	/* contains printer device name */
    static LPSTR ubuf;
    LPSTR desc;
    int i;

    switch (wmsg) {
	case WM_INITDIALOG:
	    ubuf = NULL;
	    /* Delay initialization of the list box until 
	     * after it is displayed, because searching for 
	     * configuration files takes several seconds.
	     */
	    SendDlgItemMessage(hDlg, UPP_LIST, LB_RESETCONTENT, 0, (LPARAM)0);
	    { TCHAR wbuf[MAXSTR];
	      load_string(IDS_WAIT, wbuf, sizeof(wbuf));
	      SendDlgItemMessageL(hDlg, UPP_LIST, LB_ADDSTRING, 0, 
		(LPARAM)wbuf);
	    }
	    EnableWindow(GetDlgItem(hDlg, UPP_LIST), FALSE);
	    uppname[0] = uppname[1] = '\0';
	    GetUPPname(hDlg, uppname, sizeof(uppname));
	    SetDlgItemTextA(hDlg, UPP_NAME, uppname);
	    PostMessage(hDlg, WM_COMMAND, WM_USER, 0L);
	    return TRUE;
	case WM_COMMAND:
	    notify_message = GetNotification(wParam,lParam);
	    switch (LOWORD(wParam)) {
		case WM_USER:
		    /* time consuming initialization */
		    ShowWindow(hDlg, SW_SHOW);
		    InvalidateRect(hDlg, NULL, FALSE);
		    UpdateWindow(hDlg);
		    desc = NULL;
		    uppname[0] = uppname[1] = '\0';
		    i = enum_upp_path(option.gsinclude, NULL, 0);
		    if ((ubuf = (LPSTR)malloc(i)) == (char *)NULL) {
			play_sound(SOUND_ERROR);
			PostMessage(hDlg, WM_COMMAND, IDCANCEL, 0L);
			return TRUE;	/* no memory */
		    }
		    enum_upp_path(option.gsinclude, ubuf, i);
		    GetUPPname(hDlg, uppname, sizeof(uppname));
		    SendDlgItemMessage(hDlg, UPP_LIST, LB_RESETCONTENT, 
			0, (LPARAM)0);
		    for (p = ubuf; *p; p += strlen(p) + 1) {
			q = p + strlen(p) + 1;
			if (strcmp(p, uppname) == 0)
			    desc = q;
			SendDlgItemMessageA(hDlg, UPP_LIST, LB_ADDSTRING, 
				0, (LPARAM)q);
			p = q;
		    }
		    if (desc != (LPSTR)NULL)
			SendDlgItemMessageA(hDlg, UPP_LIST, LB_SELECTSTRING, 
			    0, (LPARAM)desc);
		    SetDlgItemTextA(hDlg, UPP_NAME, uppname);
		    EnableWindow(GetDlgItem(hDlg, UPP_LIST), TRUE);
		    return TRUE;	/* we processed the message */
		case ID_HELP:
		    get_help();
		    return FALSE;
		case UPP_LIST:
		    if (notify_message == LBN_SELCHANGE) {
			char dname[MAXSTR];
			if (ubuf == NULL)
			    return FALSE;
			i = (int)SendDlgItemMessage(hDlg, UPP_LIST, 
			    LB_GETCURSEL, 0, 0L);
			if (i == LB_ERR)
			    return FALSE;
			if (SendDlgItemMessage(hDlg, UPP_LIST, LB_GETTEXTLEN, 
			      i, (LPARAM)0) + 1 > (int)sizeof(dname))
			    return FALSE;
			SendDlgItemMessageA(hDlg, UPP_LIST, LB_GETTEXT, 
			    i, (LPARAM)(LPSTR)dname);
			SetDlgItemTextA(hDlg, UPP_NAME, 
			    uppmodel_to_name(ubuf, dname));
		    }
		    else if (notify_message == LBN_DBLCLK) {
			PostMessage(hDlg, WM_COMMAND, IDOK, (LPARAM)0);
		    }
		    return FALSE;
		case IDOK:
		    /* select uniprint device in parent dialog */
		    {char buf[MAXSTR];
		      GetDlgItemTextA(GetParent(hDlg), DEVICE_NAME, 
			buf, sizeof(buf));
		      if (strcmp(buf, "uniprint") != 0) {
		        /* select uniprint device */
		        if (SendDlgItemMessageA(GetParent(hDlg), DEVICE_NAME, 
			    CB_SELECTSTRING, 0, 
			    (LPARAM)(LPSTR)"uniprint")
			      == CB_ERR) {
			  play_sound(SOUND_ERROR);
			  return FALSE;	/* can't select uniprint */
		        }
		        SendDlgNotification(GetParent(hDlg), DEVICE_NAME, 
		  	  CBN_SELCHANGE);
		      }
		      SetDlgItemTextA(GetParent(hDlg), DEVICE_RES, "");
		    }
		    /* select uniprint option in parent dialog */
		    if (GetDlgItemTextA(hDlg, UPP_NAME, uppname+2, 
		      sizeof(uppname)-3) != 0) {
		      uppname[0] = '"';
		      uppname[1] = '@';
		      strcat(uppname, "\042");
		      SetDlgItemTextA(GetParent(hDlg), 
			    DEVICE_OPTIONS, uppname);
		    }
		    if (ubuf)
		 	free(ubuf);
		    EndDialog(hDlg, TRUE);
		    return TRUE;
		case IDCANCEL:
		    if (ubuf)
		 	free(ubuf);
		    EndDialog(hDlg, FALSE);
		    return TRUE;
	    }
	    break;
    }
    return FALSE;
}

/* dialog box for selecting PostScript prolog/epilog and Ctrl+D */
#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
AdvPSDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{

    switch (wmsg) {
	case WM_INITDIALOG:
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
#ifndef cmb4
#define cmb4 0x0473
#endif
	    GetDlgItemTextA(GetParent(hDlg), cmb4, section, sizeof(section));
	    if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		profile_read_string(prf, section, "PreCtrlD", "0", buf, 
		    sizeof(buf)-2);
		if (sscanf(buf, "%d", &prectrld) != 1)
		    prectrld = 0;
		SendDlgItemMessage(hDlg, ADVPS_PRECTRLD, BM_SETCHECK, 
		    prectrld ? 1 : 0, 0);
		profile_read_string(prf, section, "PostCtrlD", "0", buf, 
		    sizeof(buf)-2);
		if (sscanf(buf, "%d", &postctrld) != 1)
		    postctrld = 0;
		SendDlgItemMessage(hDlg, ADVPS_POSTCTRLD, BM_SETCHECK, 
		    postctrld ? 1 : 0, 0);
		profile_read_string(prf, section, "Prolog", "", buf, 
		   sizeof(buf)-2);
		SetDlgItemTextA(hDlg, ADVPS_PROLOG, buf);
		profile_read_string(prf, section, "Epilog", "", buf, 
		   sizeof(buf)-2);
		SetDlgItemTextA(hDlg, ADVPS_EPILOG, buf);
		profile_close(prf);
	    }
	    }
	    return TRUE;
	case WM_COMMAND:
	    switch (LOWORD(wParam)) {
		case ADVPS_PROLOGBROWSE:
		    {   char buf[MAXSTR];
			GetDlgItemTextA(hDlg, ADVPS_PROLOG, buf, sizeof(buf)-1);
			if (get_filename(buf, FALSE, FILTER_ALL, 
			       0, IDS_TOPICPRINT)) {
			    SetDlgItemTextA(hDlg, ADVPS_PROLOG, buf);
			}
		    }
		    return FALSE;
		case ADVPS_EPILOGBROWSE:
		    {   char buf[MAXSTR];
			GetDlgItemTextA(hDlg, ADVPS_EPILOG, buf, sizeof(buf)-1);
			if (get_filename(buf, FALSE, FILTER_ALL, 
			       0, IDS_TOPICPRINT)) {
			    SetDlgItemTextA(hDlg, ADVPS_EPILOG, buf);
			}
		    }
		    return FALSE;
		case ID_HELP:
		    get_help();
		    return FALSE;
		case IDOK:
		    { 
		    char buf[MAXSTR];
		    char section[MAXSTR];
		    int prectrld;
		    int postctrld;
		    PROFILE *prf;
		    /* save settings */
		    if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		      GetDlgItemTextA(GetParent(hDlg), cmb4, 
			section, sizeof(section));
		      prectrld = (int)SendDlgItemMessage(hDlg, ADVPS_PRECTRLD, 
			  BM_GETCHECK, 0, 0);
		      profile_write_string(prf, section, "PreCtrlD", 
			  prectrld ? "1" : "0");
		      postctrld = (int)SendDlgItemMessage(hDlg, ADVPS_POSTCTRLD, 
			  BM_GETCHECK, 0, 0);
		      profile_write_string(prf, section, "PostCtrlD", 
			  postctrld ? "1" : "0");
		      GetDlgItemTextA(hDlg, ADVPS_PROLOG, buf, sizeof(buf)-1);
		      profile_write_string(prf, section, "Prolog", buf);
		      GetDlgItemTextA(hDlg, ADVPS_EPILOG, buf, sizeof(buf)-1);
		      profile_write_string(prf, section, "Epilog", buf);
		      profile_close(prf);
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


char *device_queue_list;
int device_queue_index;


BOOL
get_device(void)
{
int result;
#define DEVICE_BUF_SIZE 4096
    device_queue_list = get_queues();
    if (device_queue_list == (char *)NULL)
	return FALSE;

    nHelpTopic = IDS_TOPICPRINT;
    result = DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_PRINTGS), hwndimg, 
	NewDeviceDlgProc, (LPARAM)NULL);
    free(device_queue_list);
    device_queue_list = NULL;
    if (result != IDOK)
	return FALSE;
    return TRUE;
}




#ifdef __BORLANDC__
#pragma argsused
#endif
/* Modeless dialog box - Cancel printing */
DLGRETURN CALLBACK _export
CancelDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
	case WM_INITDIALOG:
	    SetWindowText(hDlg, szAppName);
	    return TRUE;
	case WM_COMMAND:
	    switch(LOWORD(wParam)) {
		case IDCANCEL:
		    DestroyWindow(hDlg);
		    hDlgModeless = 0;
		    EndDialog(hDlg, 0);
		    return TRUE;
	    }
    }
    return FALSE;
}

/* Dialog box to select printer port */
/* For Win32s this selects a port */
/* For Win95 or WinNT, this selects a queue */
DLGRETURN CALLBACK _export
SpoolDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
LPSTR entry;
    switch(message) {
	case WM_INITDIALOG:
	    entry = (LPSTR)lParam;
	    while (*entry) {
		SendDlgItemMessageA(hDlg, SPOOL_PORT, LB_ADDSTRING, 0, (LPARAM)entry);
		entry += strlen(entry)+1;
	    }
	    if ( (is_win32s ? (*option.printer_port=='\0') : (*option.printer_queue=='\0') ) ||
		(SendDlgItemMessageA(hDlg, SPOOL_PORT, LB_SELECTSTRING, 0, (LPARAM)(LPSTR)(is_win32s ? option.printer_port : option.printer_queue))
		    == LB_ERR) )
	        SendDlgItemMessage(hDlg, SPOOL_PORT, LB_SETCURSEL, 0, (LPARAM)0);
	    return TRUE;
	case WM_COMMAND:
	    switch(LOWORD(wParam)) {
		case SPOOL_PORT:
		    if (HIWORD(wParam) == LBN_DBLCLK)
			PostMessage(hDlg, WM_COMMAND, IDOK, 0L);
		    return FALSE;
		case IDOK:
		    SendDlgItemMessageA(hDlg, SPOOL_PORT, LB_GETTEXT, 
			(int)SendDlgItemMessage(hDlg, SPOOL_PORT, LB_GETCURSEL, 0, 0L),
			(LPARAM)(LPSTR)(is_win32s ? option.printer_port : option.printer_queue));
		    EndDialog(hDlg, 1+(int)SendDlgItemMessage(hDlg, SPOOL_PORT, LB_GETCURSEL, 0, 0L));
		    return TRUE;
		case IDCANCEL:
		    EndDialog(hDlg, 0);
		    return TRUE;
	    }
    }
    return FALSE;
}

char *
get_queues(void)
{
int i;
DWORD count, needed;
PRINTER_INFO_1 *prinfo;
char *enumbuffer;
char *buffer;
char *p;
char *s;
    if (is_win32s) {
	if ((buffer = (char *)malloc(PORT_BUF_SIZE)) == (char *)NULL)
	    return NULL;
	GetProfileStringA("Devices", NULL, "", buffer, PORT_BUF_SIZE);
	return buffer;
    }

    /* EnumPrintersW is not supported MSLU, so use ANSI version.
     * Because PRINTER_INFO_1 is still Unicode, we need to cast
     * the string pointers back to ANSI.
     */

    /* enumerate all available printers */
    EnumPrintersA(PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL, 
	NULL, 1, NULL, 0, &needed, &count);
    if (needed == 0) {
	/* no printers */
	enumbuffer = (char *)malloc(4);
	if (enumbuffer == (char *)NULL)
	    return NULL;
	memset(enumbuffer, 0, 4);
	return enumbuffer;	
    }
    enumbuffer = (char *)malloc(needed);
    if (enumbuffer == (char *)NULL)
	return NULL;
    if (!EnumPrintersA(PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL, 
	NULL, 1, (LPBYTE)enumbuffer, needed, &needed, &count)) {
	char buf[256];
	free(enumbuffer);
	sprintf(buf, "EnumPrintersA() failed, error code = %d", GetLastError());
	gserror(0, buf, MB_ICONHAND, SOUND_ERROR);
	return NULL;
    }
    prinfo = (PRINTER_INFO_1 *)enumbuffer;
    if ((buffer = (char *)malloc(PORT_BUF_SIZE)) == (char *)NULL) {
	free(enumbuffer);
	return NULL;
    }
    /* copy printer names to single buffer */
    p = buffer;
    for (i=0; i<(int)count; i++) {
	/* We used ANSI version of EnumPrinters, so cast string back to ANSI */
	s = (char *)prinfo[i].pName;
	if ((int)(strlen(s) + 1) < (PORT_BUF_SIZE- (p-buffer))) {
	    strncpy(p, s, (PORT_BUF_SIZE- (p-buffer)));
	    p += strlen(p) + 1;
	}
    }
    *p = '\0';	/* double null at end */
    free(enumbuffer);
    return buffer;
}

/* return TRUE if queuename available */
/* return FALSE if cancelled or error */
/* if queue non-NULL, use as suggested queue */
BOOL
get_queuename(char *portname, char *queue)
{
char *buffer;
char *p;
int i, iport;

    buffer = get_queues();
    if (buffer == (char *)NULL) 
	return FALSE;
    if ( (queue == (char *)NULL) || (strlen(queue)==0) ) {
	/* select a queue */
	iport = DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_QUEUE), hwndimg, 
		SpoolDlgProc, (LPARAM)buffer);
	if (!iport) {
	    free(buffer);
	    return FALSE;
	}
	p = buffer;
	for (i=1; i<iport && strlen(p)!=0; i++)
	    p += strlen(p)+1;
	/* prepend \\spool\ which is used by Ghostscript to distinguish */
	/* real files from queues */
	strcpy(portname, "\\\\spool\\");
	strcat(portname, p);
    }
    else {
	strcpy(portname, "\\\\spool\\");
	strcat(portname, queue);
    }

    free(buffer);
    return TRUE;
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
char filename[MAXSTR];
char device[MAXSTR];
	if (is_win95 || is_winnt)
	    return get_queuename(portname, port);

	if (port && strlen(port)) {
	    /* check if it is a queue name */
	    GetProfileStringA("Devices", port, "", device, sizeof(device));
	    if (strlen(device)) {
		/* map it to a port name */
		strtok(device, ",");
		port = strtok(NULL, ",");
	    }
	}

        buffer = get_ports();
	if ( (port == (char *)NULL) || (strlen(port)==0) ) {
	    if (buffer == (char *)NULL)
		return FALSE;
	    /* select a port */
	    iport = DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_SPOOL), 
		hwndimg, SpoolDlgProc, (LPARAM)buffer);
	    if (!iport) {
	        free(buffer);
	        return FALSE;
	    }
	    p = buffer;
	    for (i=1; i<iport && strlen(p)!=0; i++)
	        p += strlen(p)+1;
	    strcpy(portname, p);
	}
	else
	    strcpy(portname, port);

	if (strlen(portname) == 0)
	    return FALSE;
	if (strcmp(portname,"FILE:") == 0) {
	    strcpy(filename, "*.prn");
	    if (!get_filename(filename, TRUE, FILTER_ALL, IDS_PRINTFILE, IDS_TOPICPRINT)) {
	        free(buffer);
	        return FALSE;
	    }
	    strcpy(portname, filename);
	}
	free(buffer);
	return TRUE;
}


char *
get_ports(void)
{
char *buffer;
	if (is_win95 || is_winnt)
	    return get_queues();

	if ((buffer = (char *)malloc(PORT_BUF_SIZE)) == (char *)NULL)
	    return NULL;
	GetProfileStringA("ports", NULL, "", buffer, PORT_BUF_SIZE);
	return buffer;
}




/******************************************************************/
/* Print File to port or queue */
/* port==NULL means prompt for port or queue with dialog box */
int gp_printfile(char *filename, char *port);

/* This is messy because Microsoft changed the spooler interface */
/* between Window 3.1 and Windows 95/NT */
/* and didn't provide the spooler interface in Win32s */

/* This code requires several different versions */
/* Win16: Use OpenJob etc. */
int gp_printfile_win16(char *filename, char *port);

/* Win95, WinNT: Use OpenPrinter, WritePrinter etc. */
int gp_printfile_win32(char *filename, char *port);

/* Win32s: Pass to Win16 spooler via gsv16spl.exe */
int gp_printfile_gsv16spl(char *filename, char *port);

int gp_printfile(char *filename, char *port)
{
    gs_addmess("Printing file \042");
    if (filename)
        gs_addmess(filename);
    gs_addmess("\042 to printer \042");
    if (port)
        gs_addmess(port);
    gs_addmess("\042\n");

    if (is_win95 || is_winnt)
	return gp_printfile_win32(filename, port);
    return gp_printfile_gsv16spl(filename, port);
}

#define PRINT_BUF_SIZE 16384u

/* True Win32 method, using OpenPrinter, WritePrinter etc. */
int 
gp_printfile_win32(char *filename, char *port)
{
DWORD count;
char *buffer;
char portname[MAXSTR];
FILE *f;
HANDLE printer;
DOC_INFO_1 di;
DWORD written;
TCHAR wbuf[MAXSTR];

    if (!get_portname(portname, port))
	return FALSE;
    port = portname + 8;	/* skip over \\spool\ */

    if ((buffer = (char *)malloc(PRINT_BUF_SIZE)) == (char *)NULL)
        return FALSE;
	
    if ((f = fopen(filename, "rb")) == (FILE *)NULL) {
	free(buffer);
	return FALSE;
    }


    /* open a printer */
    if (!OpenPrinterA(port, &printer, NULL)) {
	char buf[256];
	sprintf(buf, "OpenPrinter() failed for \042%s\042, error code = %d", port, GetLastError());
	gserror(0, buf, MB_ICONHAND, SOUND_ERROR);
	free(buffer);
	return FALSE;
    }
    /* from here until ClosePrinter, should AbortPrinter on error */

    convert_multibyte(wbuf, filename, sizeof(wbuf)/sizeof(TCHAR));
    di.pDocName = wbuf;
    di.pOutputFile = NULL;
    di.pDatatype = TEXT("RAW");  /* for available types see EnumPrintProcessorDatatypes */
    if (!StartDocPrinter(printer, 1, (LPBYTE)&di)) {
	char buf[256];
	sprintf(buf, "StartDocPrinter() failed, error code = %d", GetLastError());
	gserror(0, buf, MB_ICONHAND, SOUND_ERROR);
	AbortPrinter(printer);
	free(buffer);
	return FALSE;
    }
    if (!StartPagePrinter(printer)) {
	char buf[256];
	sprintf(buf, "StartPagePrinter() failed, error code = %d", GetLastError());
	gserror(0, buf, MB_ICONHAND, SOUND_ERROR);
	AbortPrinter(printer);
	free(buffer);
	return FALSE;
    }
   

    while ((count = fread(buffer, 1, PRINT_BUF_SIZE, f)) != 0 ) {
	if (!WritePrinter(printer, (LPVOID)buffer, count, &written)) {
	    free(buffer);
	    fclose(f);
	    AbortPrinter(printer);
	    return FALSE;
	}
    }
    fclose(f);
    free(buffer);

    if (!EndPagePrinter(printer)) {
	char buf[256];
	sprintf(buf, "EndPagePrinter() failed, error code = %d", GetLastError());
	gserror(0, buf, MB_ICONHAND, SOUND_ERROR);
	AbortPrinter(printer);
	return FALSE;
    }
    if (!EndDocPrinter(printer)) {
	char buf[256];
	sprintf(buf, "EndDocPrinter() failed, error code = %d", GetLastError());
	gserror(0, buf, MB_ICONHAND, SOUND_ERROR);
	AbortPrinter(printer);
	return FALSE;
    }

    if (!ClosePrinter(printer)) {
	char buf[256];
	sprintf(buf, "ClosePrinter() failed, error code = %d", GetLastError());
	gserror(0, buf, MB_ICONHAND, SOUND_ERROR);
	return FALSE;
    }
    return TRUE;
}


/* Start a 16-bit application gsv16spl.exe and pass printer data in */
/* global memory.  gsv16spl.exe then uses 16-bit spooler functions. */
/* Only works under Win16 and Win32s */
/* Intended for Win32s where 16-bit spooler functions are not available */
/* and Win32 spooler functions are not implemented. */
int
gp_printfile_gsv16spl(char *filename, char *port)
{
/* Get printer port list from win.ini */
char *buffer;
char portname[MAXSTR];
FILE *f;
unsigned int count;
int error = FALSE;
long lsize;
long ldone;
char pcdone[20];
MSG msg;
HINSTANCE hinst;
char command[MAXSTR];
HGLOBAL hmem;
LPBYTE data;

	if (!get_portname(portname, port))
	    return FALSE;

	if ((buffer = (char *)malloc(PRINT_BUF_SIZE)) == (char *)NULL)
	    return FALSE;

	hmem = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, PRINT_BUF_SIZE+sizeof(WORD));
	if (hmem == (HGLOBAL)NULL) {
	    free(buffer);
	    gserror(0, "Can't allocate global memory for gsv16spl", 0, SOUND_ERROR);
	    return FALSE;
	}

	if ((f = fopen(filename, "rb")) == (FILE *)NULL) {
	    free(buffer);
	    GlobalFree(hmem);
	    return FALSE;
	}
	fseek(f, 0L, SEEK_END);
	lsize = ftell(f);
	if (lsize <= 0)
	    lsize = 1;
	fseek(f, 0L, SEEK_SET);

	data = (LPBYTE)GlobalLock(hmem);
	strcpy(((LPSTR)data)+2, portname);
	*((LPWORD)data) = (WORD)(strlen(portname)+1);
	GlobalUnlock(hmem);
	
	convert_widechar(command, szExePath, sizeof(command)-1);
	strcat(command, "gsv16spl.exe");
	sprintf(command+strlen(command), " %lu", (unsigned long)hwndimg);
	hinst = (HINSTANCE)WinExec(command, SW_SHOWMINNOACTIVE);
	if (hinst == NULL)
	{
	    fclose(f);
	    free(buffer);
	    GlobalFree(hmem);
	    gserror(IDS_CANNOTRUN, command, MB_ICONSTOP, SOUND_ERROR);
	    return FALSE;
	}

	if (hwndspl == (HWND)NULL) {
	    fclose(f);
	    free(buffer);
	    GlobalFree(hmem);
	    gserror(0, "gsv16spl didn't give us a window handle", 0, SOUND_ERROR);
	    return FALSE;	/* gsv16spl didn't SendMessage to us */
	}

	if (!SendMessage(hwndspl, WM_GSV16SPL, 0, (LPARAM)hmem)) {
	    fclose(f);
	    free(buffer);
	    GlobalFree(hmem);
	    gserror(0, "gsv16spl couldn't start printer job", 0, SOUND_ERROR);
	    return FALSE;
	}


	hDlgModeless = CreateDialogParamL(hlanguage, 
	    MAKEINTRESOURCE(IDD_CANCEL), hwndimg, CancelDlgProc, 0L);
	ldone = 0;

	data = (LPBYTE)GlobalLock(hmem);
	while (!error && hDlgModeless 
	  && (count = fread(buffer, 1, PRINT_BUF_SIZE, f)) != 0 ) {
	    *((LPWORD)data) = (WORD)count;
	    memcpy(((LPSTR)data)+2, buffer, count);
	    GlobalUnlock(hmem);
	    if (!SendMessage(hwndspl, WM_GSV16SPL, 0, (LPARAM)hmem))
		error = TRUE;
	    ldone += count;
	    sprintf(pcdone, "%d %%done", (int)(ldone * 100 / lsize));
	    SetWindowTextA(GetDlgItem(hDlgModeless, CANCEL_PCDONE), pcdone);
	    while (PeekMessage(&msg, hDlgModeless, 0, 0, PM_REMOVE)) {
		if ((hDlgModeless == 0) || !IsDialogMessage(hDlgModeless, &msg)) {
		    TranslateMessage(&msg);
		    DispatchMessage(&msg);
  		}
  	    }
	    data = (LPBYTE)GlobalLock(hmem);
  	}
	fclose(f);

	if (!hDlgModeless)
	    error=TRUE;

	if (error)
	    *((LPWORD)data) = 0xffff;	/* abort */
	else
	    *((LPWORD)data) = 0;	/* EOF */
	GlobalUnlock(hmem);
        SendMessage(hwndspl, WM_GSV16SPL, 0, (LPARAM)hmem);
	GlobalFree(hmem);
	free(buffer);

	DestroyWindow(hDlgModeless);
	hDlgModeless = 0;
	return !error;
}


void
start_gvwgs(void)
{
    BOOL flag;
    char progname[MAXSTR];
    char command[MAXSTR+MAXSTR];

    if (!is_win32s)
        sprintf(command,"%s \042%s\042 \042%s\042 \042%s\042", debug ? "/d" : "",
	    option.gsdll, printer.optname, printer.psname);
    else
        sprintf(command,"%s %s %s %s", debug ? "/d" : "",
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

    info_wait(IDS_WAIT);
    convert_widechar(progname, szExePath, sizeof(progname)-1);
#ifdef DECALPHA
    strcat(progname, "gvwgsda.exe");
#else
#ifdef _WIN64
    strcat(progname, "gvwgs64.exe");
#else
    strcat(progname, "gvwgs32.exe");
#endif
#endif
    flag = exec_pgm(progname, command, &printer.prog);
    if (!flag || !printer.prog.valid) {
	    cleanup_pgm(&printer.prog);
	    gserror(IDS_CANNOTRUN, progname, MB_ICONHAND, SOUND_ERROR);
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

    /* if printing from command line, close GSview */
    if (print_exit && (print_count==0) && !(debug & DEBUG_GDI))
	gsview_command(IDM_EXIT);
    return;
}



#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
PageRangeDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
    int i;
    switch (wmsg) {
      case WM_INITDIALOG:
	{int print_method = (int)lParam;
	/* page reverse only for DSC and not ignoredsc */
	if ((psfile.dsc == (CDSC *)NULL) || psfile.print_ignoredsc) {
	    EnableWindow(GetDlgItem(hDlg, PAGE_REVERSE), FALSE);
	}
	else {
	    if (psfile.page_list.reverse)
		SendDlgItemMessage(hDlg, PAGE_REVERSE, BM_SETCHECK, 1, 0);
	}
	/* ignore DSC available only with DSC and PRINT_GDI */
	if ((psfile.dsc == (CDSC *)NULL) || print_method!=PRINT_GDI)
	    EnableWindow(GetDlgItem(hDlg, IDC_IGNOREDSC), FALSE);
	else
	    EnableWindow(GetDlgItem(hDlg, IDC_IGNOREDSC), TRUE);
	/* select all/even/odd */
	switch (psfile.print_oddeven) {
	    default:
	    case ALL_PAGES:
		i = IDC_ALL;
		break;
	    case ODD_PAGES:
		i = IDC_ODD;
		break;
	    case EVEN_PAGES:
		i = IDC_EVEN;
		break;
	}
	if ((psfile.dsc==(CDSC *)NULL) && print_method!=PRINT_GDI) {
	    i = IDC_ALL;
	    EnableWindow(GetDlgItem(hDlg, IDC_EVEN), FALSE);
	    EnableWindow(GetDlgItem(hDlg, IDC_ODD), FALSE);
	}
	SendDlgItemMessage(hDlg, i, BM_SETCHECK, 1, 0);

	/* page range */
	if ((psfile.dsc!=(CDSC *)NULL) || print_method==PRINT_GDI) {
	    char buf[32];
	    sprintf(buf, "%d", psfile.print_from);
	    SetDlgItemTextA(hDlg, IDC_FROM, buf);
	    sprintf(buf, "%d", psfile.print_to);
	    SetDlgItemTextA(hDlg, IDC_TO, buf);
	}
	else {
	    EnableWindow(GetDlgItem(hDlg, IDC_FROM), FALSE);
	    EnableWindow(GetDlgItem(hDlg, IDC_TO), FALSE);
	}
	}
	return TRUE;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	    case IDC_IGNOREDSC:
		i = SendDlgItemMessage(hDlg, IDC_IGNOREDSC, BM_GETCHECK, 0, 0);
		if (i || (psfile.dsc == (CDSC *)NULL)) {
		  SendDlgItemMessage(hDlg, PAGE_REVERSE, BM_SETCHECK, 0, 0);
		  EnableWindow(GetDlgItem(hDlg, PAGE_REVERSE), FALSE);
		}
		else {
		  EnableWindow(GetDlgItem(hDlg, PAGE_REVERSE), TRUE);
		}
		return FALSE;
	    case IDOK:
		psfile.print_ignoredsc = SendDlgItemMessage(hDlg, 
			IDC_IGNOREDSC, BM_GETCHECK, 0, 0);
		psfile.page_list.reverse = SendDlgItemMessage(hDlg, 
			PAGE_REVERSE, BM_GETCHECK, 0, 0);
		psfile.print_from = GetDlgItemInt(hDlg, IDC_FROM, NULL, FALSE);
		psfile.print_to = GetDlgItemInt(hDlg, IDC_TO, NULL, FALSE);
		if (SendDlgItemMessage(hDlg, IDC_ODD, BM_GETCHECK, 0, 0))
		    psfile.print_oddeven = ODD_PAGES;
		else if (SendDlgItemMessage(hDlg, IDC_EVEN, BM_GETCHECK, 0, 0))
		    psfile.print_oddeven = EVEN_PAGES;
		else
		    psfile.print_oddeven = ALL_PAGES;
		EndDialog(hDlg, TRUE);
		return TRUE;
	    case IDCANCEL:
		EndDialog(hDlg, FALSE);
		return TRUE;
	    case ID_HELP:
		get_help();
		return TRUE;
	}
	break;
    }
    return FALSE;
}


void
set_page_range(void)
{
    if (psfile.dsc != (CDSC *)NULL) {
	BOOL print_it;
	int i;
	for (i=0; i< (int)(psfile.dsc->page_count); i++) {
	    print_it = FALSE;
	    if (((i+1) >= psfile.print_from) && ((i+1) <= psfile.print_to)) {
		if (psfile.print_oddeven == ALL_PAGES)
		    print_it = TRUE;
		else if ((psfile.print_oddeven == ODD_PAGES) && ((i%2)==0))
		    print_it = TRUE;
		else if ((psfile.print_oddeven == EVEN_PAGES) && ((i%2)==1))
		    print_it = TRUE;
	    }
	    psfile.page_list.select[i] = print_it;
	}
    }
}


/* Select pages to print or convert */
BOOL
get_page_range(HWND hwnd, int method)
{
    if (psfile.dsc != (CDSC *)NULL) {
	int i;
	psfile.page_list.current = psfile.pagenum;
	psfile.page_list.multiple = TRUE;
	if (psfile.page_list.select == (BOOL *)NULL)
	    return FALSE;
	for (i=0; i<(int)(psfile.dsc->page_count); i++)
	    if (psfile.page_list.select[i]) {
	        psfile.page_list.current = i;
		break;
	    }
	return DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_MULTIPAGE), 
		hwnd, (DLGPROC)PageMultiDlgProc, (LPARAM)NULL);
    }

    if ((psfile.dsc == (CDSC *)NULL) || psfile.dsc->page_count == 0 ) {
	/* all pages */
	psfile.print_from = 1;
	psfile.print_to = 999999;
    }
    else {
	psfile.print_from = 1;
	psfile.print_to = psfile.dsc->page_count;
    }
    psfile.print_oddeven = ALL_PAGES;
    /* pfile.print_ignoredsc = FALSE;	   enabled only if DSC && PrintGDI */
    /* psfile.page_list.reverse = FALSE;   enabled only if DSC && GS device */
    /* option.print_reverse set when printing and psfile.page_list.reverse */
    if (DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_PAGERANGE), hwnd, 
	PageRangeDlgProc, (LPARAM)method) == IDOK) {
	set_page_range();
	return TRUE;
    }
    return FALSE;
}


void init_fixed_media(HWND hDlg, int id, int fixed_media)
{
    TCHAR buf[MAXSTR];
    /* Fill in Page Size combo box */
    lstrcpy(buf, TEXT("Variable"));
    load_string(IDS_PAGESIZE_VARIABLE, buf, sizeof(buf));
    SendDlgItemMessageL(hDlg, id, CB_ADDSTRING, 0, 
	    (LPARAM)((LPTSTR)buf));
    lstrcpy(buf, TEXT("Fixed"));
    load_string(IDS_PAGESIZE_FIXED, buf, sizeof(buf));
    SendDlgItemMessageL(hDlg, id, CB_ADDSTRING, 0, 
	    (LPARAM)((LPTSTR)buf));
    lstrcpy(buf, TEXT("Shrink"));
    load_string(IDS_PAGESIZE_SHRINK, buf, sizeof(buf));
    SendDlgItemMessageL(hDlg, id, CB_ADDSTRING, 0, 
	    (LPARAM)((LPTSTR)buf));
    SendDlgItemMessage(hDlg, id, CB_SETCURSEL, fixed_media, 0L);
}

/* dialog box for selecting print/convert device and resolution */
DLGRETURN CALLBACK _export
NewDeviceDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
    char buf[128];
    int i, idevice;
    WORD notify_message;
    char *p;
    char *res;
    char *s;
    char entry[MAXSTR];
    struct prop_item_s *proplist;
    static BOOL bConvert;  /* TRUE if convert, FALSE if printing */
    PROFILE *prf;

    switch (wmsg) {
      case WM_INITDIALOG:
	bConvert = (BOOL)lParam;
	p = get_devices(bConvert);
	res = p;	/* save for free() */
	while ( p!=(char *)NULL && strlen(p)!=0) {
	    SendDlgItemMessageA(hDlg, DEVICE_NAME, CB_ADDSTRING, 0, 
		(LPARAM)((LPSTR)p));
	    p += strlen(p) + 1;
	}
	free(res);
	s = bConvert ? option.convert_device : option.printer_device;
	if (SendDlgItemMessageA(hDlg, DEVICE_NAME, CB_SELECTSTRING, 0, 
	    (LPARAM)(LPSTR)(s)) == CB_ERR) {
	    if (strlen(s))
		SetDlgItemTextA(hDlg, DEVICE_NAME, s);
	    else
		SendDlgItemMessageA(hDlg, DEVICE_NAME, CB_SETCURSEL, 0, 0L);
	}
	/* force update of DEVICE_RES */
	s = bConvert ? option.convert_resolution : option.printer_resolution;
	SendDlgNotification(hDlg, DEVICE_NAME, CBN_SELCHANGE);
	if (SendDlgItemMessageA(hDlg, DEVICE_RES, CB_SELECTSTRING, 0, 
	    (LPARAM)(LPSTR)(s)) == CB_ERR) {
	    if (strlen(s))
		SetDlgItemTextA(hDlg, DEVICE_RES, s);
	    else
		SendDlgItemMessage(hDlg, DEVICE_RES, CB_SETCURSEL, 0, 0L);
	}

	/* Fill in Page Size combo box */
	init_fixed_media(hDlg, DEVICE_FIXEDMEDIA, 
	    bConvert ? option.convert_fixed_media : option.print_fixed_media);

	/* Uniprint button only when printing */
	if (bConvert)
	    ShowWindow(GetDlgItem(hDlg, DEVICE_UNIPRINT), FALSE);

	/* fill in page list box */
	if (bConvert) {
	  if ( (psfile.dsc != (CDSC *)NULL) && (psfile.dsc->page_count != 0)) {
	    psfile.page_list.current = psfile.pagenum-1;
	    psfile.page_list.multiple = TRUE;
	    for (i=0; i< (int)psfile.dsc->page_count; i++)
		psfile.page_list.select[i] = TRUE;
	    psfile.page_list.select[psfile.page_list.current] = TRUE;
	    psfile.page_list.reverse = option.print_reverse;
	    if (GetDlgItem(hDlg, PAGE_LIST) != (HWND)NULL)
	        PageMultiDlgProc(hDlg, wmsg, wParam, lParam);
	  }
	  else {
	    TCHAR tbuf[MAXSTR];
	    LPTSTR t;
	    psfile.page_list.multiple = FALSE;
	    EnableWindow(GetDlgItem(hDlg, PAGE_ALL), FALSE);
	    EnableWindow(GetDlgItem(hDlg, PAGE_ODD), FALSE);
	    EnableWindow(GetDlgItem(hDlg, PAGE_EVEN), FALSE);
	    EnableWindow(GetDlgItem(hDlg, PAGE_REVERSE), FALSE);
	    load_string(IDS_ALL, tbuf, sizeof(tbuf));
	    for (t=tbuf, i=0; *t; t++) {
		if (*t != '&')
		    tbuf[i++] = *t;
	    }
	    tbuf[i] = '\0';
	    SendDlgItemMessage(hDlg, PAGE_LIST, LB_ADDSTRING, 0, 
		(LPARAM)((LPTSTR)tbuf));
	    EnableWindow(GetDlgItem(hDlg, PAGE_LISTTEXT), FALSE);
	    EnableWindow(GetDlgItem(hDlg, PAGE_LIST), FALSE);
	  }
	}
	return TRUE;
      case WM_COMMAND:
	notify_message = GetNotification(wParam,lParam);
	switch (LOWORD(wParam)) {
	    case ID_HELP:
		get_help();
		return FALSE;
	    case PAGE_LIST:
		if (notify_message == LBN_DBLCLK)
		    PostMessage(hDlg, WM_COMMAND, IDOK, 0L);
		return FALSE;
	    case DEVICE_NAME:
		if (notify_message != CBN_SELCHANGE) {
		    return FALSE;
		}
		idevice = (int)SendDlgItemMessage(hDlg, DEVICE_NAME, 
			CB_GETCURSEL, 0, 0L);
		if (idevice == CB_ERR)
		    return FALSE;

	        if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		    char section[MAXSTR];
		    SendDlgItemMessageA(hDlg, DEVICE_NAME, CB_GETLBTEXT, 
			idevice, (LPARAM)(LPSTR)entry);
		    if ( (proplist = get_properties(entry)) != 
			(struct prop_item_s *)NULL ) {
			free((char *)proplist);
			EnableWindow(GetDlgItem(hDlg, DEVICE_PROP), TRUE);
		    }
		    else
			EnableWindow(GetDlgItem(hDlg, DEVICE_PROP), FALSE);
		    /* now look up entry in gsview.ini */
		    /* and update DEVICE_RES list box */
		    profile_read_string(prf, 
			bConvert ? CONVERTSECTION : DEVSECTION,
			entry, "", buf, sizeof(buf)-2);
		    buf[strlen(buf)+1] = '\0';	/* double NULL at end */
		    SendDlgItemMessage(hDlg, DEVICE_RES, CB_RESETCONTENT, 0, 0L);
		    p = buf;
		    if (*p == '\0') {
			/* no resolutions can be set */
			EnableWindow(GetDlgItem(hDlg, DEVICE_RES), FALSE);
			EnableWindow(GetDlgItem(hDlg, DEVICE_RESTEXT), FALSE);
		    }
		    else {
		      EnableWindow(GetDlgItem(hDlg, DEVICE_RES), TRUE);
		      EnableWindow(GetDlgItem(hDlg, DEVICE_RESTEXT), TRUE);
		      while (*p!='\0') {
			res = p;
			while ((*p!='\0') && (*p!=','))
			    p++;
			*p++ = '\0';
			SendDlgItemMessageA(hDlg, DEVICE_RES, CB_ADDSTRING, 0, 
			    (LPARAM)((LPSTR)res));
		      }
		    }
		    SendDlgItemMessage(hDlg, DEVICE_RES, CB_SETCURSEL, 0, 0L);
		    if (SendDlgItemMessageA(hDlg, DEVICE_RES, CB_GETLBTEXT, 
			    0, (LPARAM)(LPSTR)buf) != CB_ERR)
			SetDlgItemTextA(hDlg, DEVICE_RES, buf);

		    /* update printer options */
		    strcpy(section, entry);
		    strcat(section, " Options");
		    profile_read_string(prf, section, "Options", "", 
			    buf, sizeof(buf)-2);
		    SetDlgItemTextA(hDlg, DEVICE_OPTIONS, buf);
	            profile_close(prf);
		}
		return FALSE;
	    case DEVICE_RES:
		/* don't have anything to do */
		return FALSE;
	    case DEVICE_PROP:
		SendDlgItemMessageA(hDlg, DEVICE_NAME, WM_GETTEXT, 
			sizeof(entry)-1, (LPARAM)(LPSTR)entry);
		if (strlen(entry) == 0)
		    return FALSE;
		if ( (proplist = get_properties(entry)) 
			!= (struct prop_item_s *)NULL ) {
		    free((char *)proplist);
		    nHelpTopic = IDS_TOPICPROP;
		    DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_PROP),
			hDlg, PropDlgProc, (LPARAM)entry);
		}
		else
		    play_sound(SOUND_ERROR);
	        nHelpTopic = bConvert ? IDS_TOPICCONVERT : IDS_TOPICPRINT;
		return FALSE;
	    case DEVICE_UNIPRINT:
		DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_UNIPRINT), hDlg, 
			UniDlgProc, (LPARAM)NULL);
		return FALSE;
	    case PAGE_ALL:
	    case PAGE_EVEN:
	    case PAGE_ODD:
		PageMultiDlgProc(hDlg, wmsg, wParam, lParam);
		return FALSE;
	    case IDOK:
		/* save device name and resolution */
		if (bConvert) {
		    GetDlgItemTextA(hDlg, DEVICE_NAME, 
			option.convert_device, sizeof(option.convert_device));
		    GetDlgItemTextA(hDlg, DEVICE_RES, option.convert_resolution, 
			sizeof(option.convert_resolution));
		    option.convert_fixed_media = (int)SendDlgItemMessage(
			hDlg, DEVICE_FIXEDMEDIA, CB_GETCURSEL, 0, 0);
		}
		else {
		    GetDlgItemTextA(hDlg, DEVICE_NAME, 
			option.printer_device, sizeof(option.printer_device));
		    GetDlgItemTextA(hDlg, DEVICE_RES, option.printer_resolution, 
			sizeof(option.printer_resolution));
		    option.print_fixed_media = (int)SendDlgItemMessage(
			hDlg, DEVICE_FIXEDMEDIA, CB_GETCURSEL, 0, 0);
		}
		option.print_reverse = psfile.page_list.reverse = 
		    (int)SendDlgItemMessage(hDlg, PAGE_REVERSE, 
			BM_GETCHECK, 0, 0);

		{ /* get options */
		  char section[MAXSTR];
		  if (bConvert)
		      strcpy(section, option.convert_device);
		  else 
		      strcpy(section, option.printer_device);
		  strcat(section, " Options");
		  GetDlgItemTextA(hDlg, DEVICE_OPTIONS, buf, sizeof(buf)-2);
	          if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		      profile_write_string(prf, section, "Options", buf);
	              profile_close(prf);
		  }
		}

		if (GetDlgItem(hDlg, PAGE_LIST) != (HWND)NULL) {
		    /* Do this for Convert dialog, but not
		     * Windows 4 dialog which doesn't have a
		     * PAGE_LIST on the dialog
		     */
	  	    if ( (psfile.dsc != (CDSC *)NULL) && 
		         (psfile.dsc->page_count != 0))
		        PageMultiDlgProc(hDlg, wmsg, wParam, lParam);
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


#define GDI_ENTRY "GDI"

/* dialog box for GDI print settings*/
DLGRETURN CALLBACK _export
GDIDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
    char buf[128];
    char entry[MAXSTR];
    struct prop_item_s *proplist;
    PROFILE *prf;

    switch (wmsg) {
      case WM_INITDIALOG:
	    if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		char section[MAXSTR];
		SendDlgItemMessage(hDlg, option.print_gdi_depth, 
		    BM_SETCHECK, 1, 0);
		init_fixed_media(hDlg, DEVICE_FIXEDMEDIA, 
		    option.print_gdi_fixed_media);
		strcpy(section, GDI_ENTRY);
		strcat(section, " Options");
		profile_read_string(prf, section, "Options", "", 
			buf, sizeof(buf)-2);
		SetDlgItemTextA(hDlg, DEVICE_OPTIONS, buf);
	        profile_close(prf);
	    }
	    return TRUE;
      case WM_COMMAND:
	switch (LOWORD(wParam)) {
	    case ID_HELP:
		get_help();
		return FALSE;
	    case DEVICE_PROP:
/* should save help topic */
		strncpy(entry, GDI_ENTRY, sizeof(entry)); 
		if ( (proplist = get_properties(entry)) 
			!= (struct prop_item_s *)NULL ) {
		    free((char *)proplist);
		    nHelpTopic = IDS_TOPICPROP;
		    DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_PROP), 
			hDlg, PropDlgProc, (LPARAM)entry);
		}
		else
		    play_sound(SOUND_ERROR);
		nHelpTopic = IDS_TOPICPRINT;
		return FALSE;
	    case IDOK:
		/* get depth */
		if (SendDlgItemMessage(hDlg, IDC_COLOUR, BM_GETCHECK, 0, 0))
		    option.print_gdi_depth = IDC_COLOUR;
		else if (SendDlgItemMessage(hDlg, IDC_GREY, BM_GETCHECK, 0, 0))
		    option.print_gdi_depth = IDC_GREY;
		else
		    option.print_gdi_depth = IDC_MONO;

		option.print_gdi_fixed_media = (int)SendDlgItemMessage(
			hDlg, DEVICE_FIXEDMEDIA, CB_GETCURSEL, 0, 0);

	        if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
		  /* get options */
		  char section[MAXSTR];
		  strcpy(section, GDI_ENTRY);
		  strcat(section, " Options");
		  GetDlgItemTextA(hDlg, DEVICE_OPTIONS, buf, sizeof(buf)-2);
		  profile_write_string(prf, section, "Options", buf);
		  profile_close(prf);
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

void
set_check_method(HWND hDlg, int method)
{
    BOOL bPageSelection = 
	(psfile.dsc != (CDSC *)NULL) || (method == PRINT_GDI);
    SendDlgItemMessage(hDlg, IDC_DEVICE_GDI, BM_SETCHECK,
	(method == PRINT_GDI) ? 1 : 0, 0);
    SendDlgItemMessage(hDlg, IDC_DEVICE_GS, BM_SETCHECK, 
	(method == PRINT_GS) ? 1 : 0, 0);
    SendDlgItemMessage(hDlg, IDC_DEVICE_PS, BM_SETCHECK,
	(method == PRINT_PS) ? 1 : 0, 0);
    /* Printer properties can only be set for GDI printing */
    EnableWindow(GetDlgItem(hDlg, 1025), method == PRINT_GDI);
    /* We can only select a page range if DSC or PRINT_GDI */
    EnableWindow(GetDlgItem(hDlg, IDC_SELECT_PAGES), bPageSelection);
    EnableWindow(GetDlgItem(hDlg, 1057), bPageSelection);
    EnableWindow(GetDlgItem(hDlg, 1058), bPageSelection);
    EnableWindow(GetDlgItem(hDlg, 1089), bPageSelection);
    EnableWindow(GetDlgItem(hDlg, 1090), bPageSelection);
    EnableWindow(GetDlgItem(hDlg, 1152), bPageSelection);
    EnableWindow(GetDlgItem(hDlg, 1153), bPageSelection);
    
}

int
get_method(HWND hDlg)
{
    int print_method;
    if (SendDlgItemMessage(hDlg, IDC_DEVICE_PS, 
	BM_GETCHECK, 0, 0)) 
	print_method = PRINT_PS;
    else if (SendDlgItemMessage(hDlg, IDC_DEVICE_GS, 
	BM_GETCHECK, 0, 0))
	print_method = PRINT_GS;
    else
	print_method = PRINT_GDI;
    return print_method;
}

#ifdef _WIN64
UINT_PTR
#else
UINT 
#endif
APIENTRY 
PrintHookProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_INITDIALOG) {
	/* Initialise our extra dialog entries from resource strings */
	TCHAR buf[MAXSTR];
	load_string(IDS_PRN_PRINTMETHOD, buf, sizeof(buf)); 
	SetDlgItemText(hDlg, IDC_DEVICE_GROUP, buf);
	load_string(IDS_PRN_WINDOWSGDI, buf, sizeof(buf)); 
	SetDlgItemText(hDlg, IDC_DEVICE_GDI, buf);
	load_string(IDS_PRN_GSDEVICE, buf, sizeof(buf)); 
	SetDlgItemText(hDlg, IDC_DEVICE_GS, buf);
	load_string(IDS_PRN_PSPRINTER, buf, sizeof(buf)); 
	SetDlgItemText(hDlg, IDC_DEVICE_PS, buf);
	load_string(IDS_PRN_SETTINGS, buf, sizeof(buf)); 
	SetDlgItemText(hDlg, IDC_GS_WINDOWS, buf);
	SetDlgItemText(hDlg, IDC_GS_DEVNAME, buf);
	SetDlgItemText(hDlg, IDC_GS_ADVPS, buf);
	load_string(IDS_PRN_HELP, buf, sizeof(buf)); 
	SetDlgItemText(hDlg, ID_HELP, buf);
	load_string(IDS_PRN_SELECTPAGES, buf, sizeof(buf)); 
	SetDlgItemText(hDlg, IDC_SELECT_PAGES, buf);

	set_check_method(hDlg, option.print_method);
	SetDlgItemTextA(hDlg, IDC_GS_DEVNAME, option.printer_device);
	/* We can't collate. */
	EnableWindow(GetDlgItem(hDlg, 1041), FALSE);

	if ((psfile.page_list.select != (BOOL *)NULL) && psfile.dsc) {
	    int i;
	    for (i=0; i< (int)(psfile.dsc->page_count); i++)
		psfile.page_list.select[i] = TRUE;
        }

	return TRUE;
    }
    else if (message == WM_COMMAND) {
        switch(LOWORD(wParam)) {
	    case IDC_DEVICE_GDI:
	    case IDC_DEVICE_GS:
	    case IDC_DEVICE_PS:
		set_check_method(hDlg, get_method(hDlg));
	 	return FALSE;	/* not processed */
	    case IDOK:
		option.print_method = get_method(hDlg);
		return FALSE;	/* not processed */
	    case IDC_SELECT_PAGES:
		if (get_page_range(hDlg, get_method(hDlg)))
		    SendDlgNotification(hDlg, 1057, BN_CLICKED);
		return TRUE;
	    case IDC_GS_WINDOWS:
		if (DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_PRINTGDI), 
		    hDlg, GDIDlgProc, (LPARAM)NULL))
		    set_check_method(hDlg, PRINT_GDI);
		return TRUE;
	    case IDC_GS_DEVNAME:
		device_queue_list = get_queues();
		if (device_queue_list == (char *)NULL)
		    return TRUE;
		if (DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_PRINTGS), 
		    hDlg, NewDeviceDlgProc, (LPARAM)NULL))
		    set_check_method(hDlg, PRINT_GS);
		free(device_queue_list);
		device_queue_list = NULL;
		SetDlgItemTextA(hDlg, IDC_GS_DEVNAME, option.printer_device);
		return TRUE;
	    case IDC_GS_ADVPS:
		if (DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_ADVPS), 
		    hDlg, AdvPSDlgProc, (LPARAM)NULL))
		    set_check_method(hDlg, PRINT_PS);
		return TRUE;
	    case ID_HELP:
		get_help();
		return(FALSE);
	}
    }
    return 0;	/* default handler */
}

void print_devmode(void (*pfn)(const char *str), HANDLE hDevMode)
{
    char buf[512];
    DWORD dw;
    LPDEVMODE dm;
    if ((hDevMode == NULL) || (hDevMode == INVALID_HANDLE_VALUE))
	return;
    dm = (LPDEVMODE)GlobalLock(hDevMode);
    sprintf(buf, "DevMode:\n");
    (*pfn)(buf);
#ifdef UNICODE
    strcpy(buf, " dmDeviceName=\042");
    convert_widechar(buf+strlen(buf), dm->dmDeviceName, 
	sizeof(buf)-strlen(buf)-1);
    strcat(buf, "\042\n");
#else
    sprintf(buf, " dmDeviceName=\042%s\042\n", dm->dmDeviceName);
#endif
    (*pfn)(buf);
    sprintf(buf, " dmFields=0x%x\n", dm->dmFields);
    (*pfn)(buf);
    dw = dm->dmFields;
    if (dw & DM_ORIENTATION) {
	sprintf(buf, "  DM_ORIENTATION\n");
	(*pfn)(buf);
    }
    if (dw & DM_PAPERSIZE) {
	sprintf(buf, "  DM_PAPERSIZE\n");
	(*pfn)(buf);
    }
    if (dw & DM_PAPERLENGTH) {
	sprintf(buf, "  DM_PAPERLENGTH\n");
	(*pfn)(buf);
    }
    if (dw & DM_PAPERWIDTH) {
	sprintf(buf, "  DM_PAPERWIDTH\n");
	(*pfn)(buf);
    }
    if (dw & DM_SCALE) {
	sprintf(buf, "  DM_SCALE\n");
	(*pfn)(buf);
    }
    if (dw & DM_COPIES) {
	sprintf(buf, "  DM_COPIES\n");
	(*pfn)(buf);
    }
    if (dw & DM_DEFAULTSOURCE) {
	sprintf(buf, "  DM_DEFAULTSOURCE\n");
	(*pfn)(buf);
    }
    if (dw & DM_PRINTQUALITY) {
	sprintf(buf, "  DM_PRINTQUALITY\n");
	(*pfn)(buf);
    }
    if (dw & DM_COLOR) {
	sprintf(buf, "  DM_COLOR\n");
	(*pfn)(buf);
    }
    if (dw & DM_DUPLEX) {
	sprintf(buf, "  DM_DUPLEX\n");
	(*pfn)(buf);
    }
    if (dw & DM_YRESOLUTION) {
	sprintf(buf, "  DM_YRESOLUTION\n");
	(*pfn)(buf);
    }
    if (dw & DM_TTOPTION) {
	sprintf(buf, "  DM_TTOPTION\n");
	(*pfn)(buf);
    }
    if (dw & DM_COLLATE) {
	sprintf(buf, "  DM_COLLATE\n");
	(*pfn)(buf);
    }
    if (dw & DM_FORMNAME) {
	sprintf(buf, "  DM_FORMNAME\n");
	(*pfn)(buf);
    }
    sprintf(buf, " dmOrientation=%d\n", dm->dmOrientation);
    (*pfn)(buf);
    sprintf(buf, " dmPaperSize=%d\n", dm->dmPaperSize);
    (*pfn)(buf);
    sprintf(buf, " dmPaperLength=%d\n", dm->dmPaperLength);
    (*pfn)(buf);
    sprintf(buf, " dmPaperWidth=%d\n", dm->dmPaperWidth);
    (*pfn)(buf);
    sprintf(buf, " dmScale=%d\n", dm->dmScale);
    (*pfn)(buf);
    sprintf(buf, " dmCopies=%d\n", dm->dmCopies);
    (*pfn)(buf);
    sprintf(buf, " dmDefaultSource=%d\n", dm->dmDefaultSource);
    (*pfn)(buf);
    sprintf(buf, " dmPrintQuality=%d\n", dm->dmPrintQuality);
    (*pfn)(buf);
    sprintf(buf, " dmColor=%d\n", dm->dmColor);
    (*pfn)(buf);
    sprintf(buf, " dmDuplex=%d\n", dm->dmDuplex);
    (*pfn)(buf);
    sprintf(buf, " dmYResolution=%d\n", dm->dmYResolution);
    (*pfn)(buf);
    sprintf(buf, " dmTTOption=%d\n", dm->dmTTOption);
    (*pfn)(buf);
    sprintf(buf, " dmCollate=%d\n", dm->dmCollate);
    (*pfn)(buf);
#ifdef UNICODE
    strcpy(buf, " dmFormName=\042");
    convert_widechar(buf+strlen(buf), dm->dmFormName, 
	sizeof(buf)-strlen(buf)-1);
    strcat(buf, "\042\n");
#else
    sprintf(buf, " dmFormName=\042%s\042\n", dm->dmFormName);
#endif
    (*pfn)(buf);
    sprintf(buf, " dmLogPixels=%d\n", dm->dmLogPixels);
    (*pfn)(buf);
    sprintf(buf, " dmBitsPerPel=%d\n", dm->dmBitsPerPel);
    (*pfn)(buf);
    sprintf(buf, " dmPelsWidth=%d\n", dm->dmPelsWidth);
    (*pfn)(buf);
    sprintf(buf, " dmPelsHeight=%d\n", dm->dmPelsHeight);
    (*pfn)(buf);
    sprintf(buf, " dmDisplayFlags=%d\n", dm->dmDisplayFlags);
    (*pfn)(buf);
    sprintf(buf, " dmDisplayFrequency=%d\n", dm->dmDisplayFrequency);
    (*pfn)(buf);
    GlobalUnlock(hDevMode);
}

void print_devnames(void (*pfn)(const char *str), HANDLE hDevNames)
{
    char buf[512];
    LPDEVNAMES lpdevnames;
    if ((hDevNames == NULL) || (hDevNames == INVALID_HANDLE_VALUE))
	    return;
    lpdevnames = (LPDEVNAMES)GlobalLock(hDevNames);
    sprintf(buf, "DevNames:\n");
    (*pfn)(buf);
    sprintf(buf, " Device=\042%s\042\n", 
	((char *)lpdevnames) + lpdevnames->wDeviceOffset);
    (*pfn)(buf);
    sprintf(buf, " Driver=\042%s\042\n", 
	((char *)lpdevnames) + lpdevnames->wDriverOffset);
    (*pfn)(buf);
    sprintf(buf, " Output=\042%s\042\n", 
	((char *)lpdevnames) + lpdevnames->wOutputOffset);
    (*pfn)(buf);
    GlobalUnlock(hDevNames);
}

/* Return a devnames structure for the specified printer.
 * If device == NULL, return for the default printer.
 * MSLU doesn't support DocumentProperties, so call ANSI version.
 * Since DEVNAMES may be used with the Unicode CreateDC, we
 * need to support Unicode strings.
 */

BOOL get_devnames(LPSTR device, HANDLE *hdevnames)
{
    char devicebuf[256];
    char driverbuf[512];
    char *driver;
    char *output;
    int length, offset;
    HANDLE hglobal;
    LPDEVNAMES lpdevnames;

    if ((device == NULL) || (strlen(device)==0)) {
	/* no device specified - use default */
	GetProfileStringA("windows", "device", "", 	
	    devicebuf, sizeof(devicebuf));
	strtok(devicebuf, ",");
	device = devicebuf;
    }
    if (strlen(device)==0)
	return FALSE;
    
    /* Get the information needed for the DEVNAMES structure */
    GetProfileStringA("Devices", device, "", driverbuf, sizeof(driverbuf));
    if (strlen(driverbuf) == 0)
	return FALSE;	/* printer doesn't exist */

    driver = strtok(driverbuf, ",");
    output = strtok(NULL, ",");

    /* Build the DEVNAMES structure */
    /* Must support Unicode */
    if (hdevnames) {
#ifdef UNICODE
	TCHAR *t;
	length = sizeof(DEVNAMES) + sizeof(TCHAR) * 
	   (strlen(device) + 1 + strlen(driver) + 1 + strlen(output) + 2);

	if (length < 1024)
	    length = 1024;
	hglobal = GlobalAlloc(GMEM_MOVEABLE, length);
	if (hglobal == NULL)
	    return FALSE;
	lpdevnames = (LPDEVNAMES)GlobalLock(hglobal);
	memset(lpdevnames, 0, length);
	offset = sizeof(DEVNAMES) / sizeof(TCHAR);
	lpdevnames->wDriverOffset = (WORD)offset;
	t = (TCHAR *)lpdevnames + offset;
	convert_multibyte(t, driver, strlen(driver)+1);
	offset += lstrlen(t) + 1;
	lpdevnames->wDeviceOffset = (WORD)offset;
	t = (TCHAR *)lpdevnames + offset;
	convert_multibyte(t, device, strlen(device)+1);
	offset += lstrlen(t) + 1;
	lpdevnames->wOutputOffset = (WORD)offset;
	t = (TCHAR *)lpdevnames + offset;
	convert_multibyte(t, output, strlen(output)+1);
	GlobalUnlock(hglobal);
	lpdevnames = NULL;
	*hdevnames = hglobal;
#else
	length = sizeof(DEVNAMES) + 
	   (strlen(device) + 1 + strlen(driver) + 1 + strlen(output) + 2);
	if (length < 1024)
	    length = 1024;
	hglobal = GlobalAlloc(GMEM_MOVEABLE, length);
	if (hglobal == NULL)
	    return FALSE;
	lpdevnames = (LPDEVNAMES)GlobalLock(hglobal);
	memset(lpdevnames, 0, length);
	offset = sizeof(DEVNAMES);
	lpdevnames->wDriverOffset = (WORD)offset;
	strncpy((char *)lpdevnames + offset, driver, strlen(driver)+1);
	offset += strlen((char *)lpdevnames + offset) + 1;
	lpdevnames->wDeviceOffset = (WORD)offset;
	strncpy((char *)lpdevnames + offset, device, strlen(device)+1);
	offset += strlen((char *)lpdevnames + offset) + 1;
	lpdevnames->wOutputOffset = (WORD)offset;
	strncpy((char *)lpdevnames + offset, output, strlen(output)+1);
	GlobalUnlock(hglobal);
	lpdevnames = NULL;
	*hdevnames = hglobal;
#endif
    }

    return TRUE;
}

/* open GSview default printer */
/* no prompts */
BOOL open_printer(void)
{
    TCHAR *device;
    TCHAR *driver;
    LPDEVNAMES lpdevnames;

    if (!get_devnames(option.printer_queue, &pd.hDevNames))
	return FALSE;

    lpdevnames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
    device = ((TCHAR *)lpdevnames) + lpdevnames->wDeviceOffset;
    driver = ((TCHAR *)lpdevnames) + lpdevnames->wDriverOffset;

    /*
    char *output = ((char *)lpdevnames) + lpdevnames->wOutputOffset;
    */

    pd.hDC = CreateDC(driver, device, NULL, NULL);

    GlobalUnlock(pd.hDevNames);

    if (debug & DEBUG_GDI)
	print_devnames(gs_addmess, pd.hDevNames);

    psfile.print_from = 1;
    psfile.print_to = 999999;
    psfile.print_oddeven = ALL_PAGES;
    psfile.print_copies = 1;
    psfile.print_ignoredsc = FALSE;
    set_page_range();

    return TRUE;
}




/* We don't release hDevMode and hDevNames after calls to */
/* query_printer, so we must provide a way to free them */
/* when exiting GSview. */
void 
printer_cleanup(void)
{
    if ((pd.hDevMode != 0) && (pd.hDevMode != INVALID_HANDLE_VALUE))
        GlobalFree(pd.hDevMode);
    if ((pd.hDevNames != 0) && (pd.hDevNames != INVALID_HANDLE_VALUE))
	GlobalFree(pd.hDevNames);
    pd.hDevMode = NULL;
    pd.hDevNames = NULL;
}

typedef struct {
    WORD      dlgVer;
    WORD      signature;
    DWORD     helpID;
    DWORD     exStyle;
    DWORD     style;
    WORD      cDlgItems;
    short     x;
    short     y;
    short     cx;
    short     cy;
    WORD /* sz_Or_Ord */ menu;
    WORD /* sz_Or_Ord */ windowClass; 
    WCHAR     title[1 /* titleLen */]; 
/* The following members exist only if the style member is  */
/* set to DS_SETFONT or DS_SHELLFONT. */
    WORD     pointsize;
    WORD     weight; 
    BYTE     italic;
    BYTE     charset;
    WCHAR    typeface[1 /* stringLen */];  
} DLGTEMPLATEEX; 
typedef DLGTEMPLATEEX *LPDLGTEMPLATEEX;

typedef struct { 
  DWORD  helpID; 
  DWORD  exStyle; 
  DWORD  style; 
  short  x; 
  short  y; 
  short  cx; 
  short  cy; 
  WORD   id; 
  WORD /* sz_Or_Ord */ windowClass; 
  WORD /* sz_Or_Ord */ title; 
  WORD   extraCount; 
} DLGITEMTEMPLATEEX; 
typedef DLGITEMTEMPLATEEX *LPDLGITEMTEMPLATEEX;


WORD *skip_unicode_string(WORD *pw)
{
    char buf[MAXSTR];
    int i=0;
    while ((*pw) && (i < MAXSTR-1)) {
	if (*pw < 0xff)
	    buf[i] = (char)*pw;	/* assume ISO-Latin1 subset */
	i++;
	pw++;
    }
    if (*pw == 0)
	pw++;
    buf[i] = 0;
    if (debug & DEBUG_GDI)
	gs_addmess(buf);
    return pw;
}

/* returns length of dlgtemplateex */
int
parse_dlgtemplateex(LPDLGTEMPLATEEX pt, LPDLGITEMTEMPLATEEX *pitem, int item_id)
{
    LPWORD pw;
    LPDLGITEMTEMPLATEEX pit;
    int i;
    int fixup;
    char buf[MAXSTR];
    BOOL debug_gdi = debug & DEBUG_GDI;

    /* for debugging, dump to gs messages */
    if (debug_gdi) {
	gs_addmess("=== Dump of dialog template ex ===\n");
	sprintf(buf, "dlgVer=%d signature=0x%x helpID=%d exStyle=0x%x style=0x%x cDlgItems=%d x=%d y=%d cx=%d cy=%d\n",
	    (int)pt->dlgVer, (int)pt->signature,
	    (int)pt->helpID,
	     (int)pt->style, (int)pt->exStyle,
	    (int)pt->cDlgItems, (int)pt->x, (int)pt->y,
	    (int)pt->cx, (int)pt->cy);
	gs_addmess(buf);
    }
    pw = (LPWORD)((LPBYTE)pt + 26 /* sizeof(DLGTEMPLATEEX) up to cy */);
    if (*pw == 0x0000) {
	pw++;
	if (debug_gdi)
	    gs_addmess("No menu\n");
    }
    else if (*pw == 0xffff) {
	pw++;
	if (debug_gdi) {
	    sprintf(buf, "Menu id %d\n", (int)*pw);
	    gs_addmess(buf);
	}
	pw++;
    }
    else {
	if (debug_gdi)
	    gs_addmess("Menu name \042");
	/* unicode string */
	pw = skip_unicode_string(pw);
	if (debug_gdi)
	    gs_addmess("\042\n");
    }
    /* might have class array */
    if (*pw == 0x0000) {
	pw++;
	if (debug_gdi)
	    gs_addmess("No class\n");
    }
    else if (*pw == 0xffff) {
	pw++;
	if (debug_gdi) {
	    sprintf(buf, "Class id %d\n", (int)*pw);
	    gs_addmess(buf);
	}
	pw++;
    }
    else {
	if (debug_gdi)
	    gs_addmess("Class name \042");
	pw = skip_unicode_string(pw);
	if (debug_gdi)
	    gs_addmess("\042\n");
    }
    /* might have title array */
    if (*pw == 0x0000) {
	pw++;
	if (debug_gdi)
	    gs_addmess("No title\n");
    }
    else {
	if (debug_gdi)
	    gs_addmess("Title name \042");
	pw = skip_unicode_string(pw);
	if (debug_gdi)
	    gs_addmess("\042\n");
    }
    /* might have font size/name */
    if ( ((pt->style & DS_SETFONT) == DS_SETFONT) ||
	 ((pt->style & DS_SHELLFONT) == DS_SHELLFONT) ) {
	if (debug_gdi) {
	    sprintf(buf, "Font size = %d\n", (int)*pw);
	    gs_addmess(buf);
	}
	pw++;
	if (debug_gdi) {
	    sprintf(buf, "Font weight = %d\n", (int)*pw);
	    gs_addmess(buf);
	}
	pw++;
	if (debug_gdi) {
	    sprintf(buf, "Font italic = %d\n", ((unsigned char *)pw)[0]);
	    gs_addmess(buf);
	    sprintf(buf, "Font charset = %d\n", ((unsigned char *)pw)[1]);
	    gs_addmess(buf);
	}
	pw++;
	if (debug_gdi)
	    gs_addmess("Font name ");
	pw = skip_unicode_string(pw);
	if (debug_gdi)
	    gs_addmess("\n");
    }
    else if (debug_gdi)
	gs_addmess("No font\n");
 
    for (i=0; i<pt->cDlgItems; i++) {
        pit = (LPDLGITEMTEMPLATEEX)pw;
	/* realign to DWORD boundary */
	fixup = ((LPBYTE)pit - (LPBYTE)pt) % 4;
	if (fixup != 0)
	    pit = (LPDLGITEMTEMPLATEEX)((LPBYTE)pit + (4-fixup));
	if (debug_gdi) {
	    sprintf(buf, "Dialog item ex %d\n", i);
	    gs_addmess(buf);
	    sprintf(buf, "  helpID=%d exStyle=0x%x style=0x%x x=%d y=%d cx=%d cy=%d id=%d\n",
		(int)pit->helpID,
		(int)pit->exStyle, (int)pit->style, 
		(int)pit->x, (int)pit->y,
		(int)pit->cx, (int)pit->cy, (int)pit->id);
	    gs_addmess(buf);
	}

	if ((pit->id == item_id) && (pitem != NULL))
	    *pitem = pit;

	pw = (LPWORD)((LPBYTE)pit + 22);
/* for some reason, there is an extra null byte always present */
pw++;
	/* sizeof(DLGITEMTEMPLATEEX) less windowClass, title, extraCount */
	/* might have class array */
	if (*pw == 0xffff) {
	    char *pc;
	    pw++;
	    switch ((int)*pw) {
		case 0x0080:
		    pc = "Button\n";
		    break;
		case 0x0081:
		    pc = "Edit\n";
		    break;
		case 0x0082:
		    pc = "Static\n";
		    break;
		case 0x0083:
		    pc = "List box\n";
		    break;
		case 0x0084:
		    pc = "Scroll bar\n";
		    break;
		case 0x0085:
		    pc = "Combo box\n";
		    break;
		default:
		    sprintf(buf, " Control class id %d\n", (int)*pw);
		    pc = buf;
	    }
	    pw++;
	    if (debug_gdi) {
		gs_addmess("  ");
		gs_addmess(pc);
	    }
	}
	else {
	    if (debug_gdi)
		gs_addmess("  Control class name \042");
	    pw = skip_unicode_string(pw);
	    if (debug_gdi)
		gs_addmess("\042\n");
	}
	/* might have title array */
	if (*pw == 0xffff) {
	    pw++;
	    if (debug_gdi)
		sprintf(buf, "  Title resource id %d\n", (int)*pw);
	    pw++;
	}
	else {
	    if (debug_gdi)
		gs_addmess("  Title name \042");
	    pw = skip_unicode_string(pw);
	    if (debug_gdi)
		gs_addmess("\042\n");
	}
	/* might have creation data */
	if (*pw) {
	    /* skip *pw bytes of data, realigning to word boundary */
	    pw += ((*pw + 1) / sizeof(WORD)) ;	/* skip creation data */
	}
	pw++;	/* skip creation data count */
    }

    fixup = (LPBYTE)pw - (LPBYTE)pt;	/* length of template */
    if (debug_gdi) {
	sprintf(buf, "Processed %d bytes of DLGTEMPLATEEX\n", fixup);
	gs_addmess(buf);
	gs_addmess("=== End of dialog template ===\n");
    }
    return fixup;
}

/* returns length of dlgtemplate */
int
parse_dlgtemplate(LPDLGTEMPLATE pt, LPDLGITEMTEMPLATE *pitem, int item_id)
{
    LPWORD pw;
    LPDLGITEMTEMPLATE pit;
    int i;
    int fixup;
    char buf[MAXSTR];
    BOOL debug_gdi = debug & DEBUG_GDI;

    /* for debugging, dump to gs messages */
    if (debug_gdi) {
	gs_addmess("=== Dump of dialog template ===\n");
	sprintf(buf, "style=0x%x exstyle=0x%x cdit=%d x=%d y=%d cx=%d cy=%d\n",
	    (int)pt->style, (int)pt->dwExtendedStyle,
	    (int)pt->cdit, (int)pt->x, (int)pt->y,
	    (int)pt->cx, (int)pt->cy);
	gs_addmess(buf);
    }
    pw = (LPWORD)((LPBYTE)pt + sizeof(DLGTEMPLATE));
    if (*pw == 0x0000) {
	pw++;
	if (debug_gdi)
	    gs_addmess("No menu\n");
    }
    else if (*pw == 0xffff) {
	pw++;
	if (debug_gdi) {
	    sprintf(buf, "Menu id %d\n", (int)*pw);
	    gs_addmess(buf);
	}
	pw++;
    }
    else {
	if (debug_gdi)
	    gs_addmess("Menu name \042");
	/* unicode string */
	pw = skip_unicode_string(pw);
	if (debug_gdi)
	    gs_addmess("\042\n");
    }
    /* might have class array */
    if (*pw == 0x0000) {
	pw++;
	if (debug_gdi)
	    gs_addmess("No class\n");
    }
    else if (*pw == 0xffff) {
	pw++;
	if (debug_gdi) {
	    sprintf(buf, "Class id %d\n", (int)*pw);
	    gs_addmess(buf);
	}
	pw++;
    }
    else {
	if (debug_gdi)
	    gs_addmess("Class name \042");
	pw = skip_unicode_string(pw);
	if (debug_gdi)
	    gs_addmess("\042\n");
    }
    /* might have title array */
    if (*pw == 0x0000) {
	pw++;
	if (debug_gdi)
	    gs_addmess("No title\n");
    }
    else {
	if (debug_gdi)
	    gs_addmess("Title name \042");
	pw = skip_unicode_string(pw);
	if (debug_gdi)
	    gs_addmess("\042\n");
    }
    /* might have font size/name */
    if (pt->style & DS_SETFONT) {
	if (debug_gdi)
	    sprintf(buf, "Font size = %d\n", (int)*pw);
	pw++;
	if (debug_gdi)
	    gs_addmess("Font name ");
	pw = skip_unicode_string(pw);
	if (debug_gdi)
	    gs_addmess("\n");
    }
    else if (debug_gdi)
	gs_addmess("No font\n");
 
    for (i=0; i<pt->cdit; i++) {
        pit = (LPDLGITEMTEMPLATE)pw;
	/* realign to DWORD boundary */
	fixup = ((LPBYTE)pit - (LPBYTE)pt) % 4;
	if (fixup != 0)
	    pit = (LPDLGITEMTEMPLATE)((LPBYTE)pit + (4-fixup));
	if (debug_gdi) {
	    sprintf(buf, "Dialog item %d\n", i);
	    gs_addmess(buf);
	    sprintf(buf, "  style=0x%x exstyle=0x%x x=%d y=%d cx=%d cy=%d id=%d\n",
		(int)pit->style, (int)pit->dwExtendedStyle, 
		(int)pit->x, (int)pit->y,
		(int)pit->cx, (int)pit->cy, (int)pit->id);
	    gs_addmess(buf);
	}

	if ((pit->id == item_id) && (pitem != NULL))
	    *pitem = pit;

	pw = (LPWORD)((LPBYTE)pit + sizeof(DLGITEMTEMPLATE));
	/* might have class array */
	if (*pw == 0xffff) {
	    char *pc;
	    pw++;
	    switch ((int)*pw) {
		case 0x0080:
		    pc = "Button\n";
		    break;
		case 0x0081:
		    pc = "Edit\n";
		    break;
		case 0x0082:
		    pc = "Static\n";
		    break;
		case 0x0083:
		    pc = "List box\n";
		    break;
		case 0x0084:
		    pc = "Scroll bar\n";
		    break;
		case 0x0085:
		    pc = "Combo box\n";
		    break;
		default:
		    sprintf(buf, " Control class id %d\n", (int)*pw);
		    pc = buf;
	    }
	    pw++;
	    if (debug_gdi) {
		gs_addmess("  ");
		gs_addmess(pc);
	    }
	}
	else {
	    if (debug_gdi)
		gs_addmess("  Control class name \042");
	    pw = skip_unicode_string(pw);
	    if (debug_gdi)
		gs_addmess("\042\n");
	}
	/* might have title array */
	if (*pw == 0xffff) {
	    pw++;
	    if (debug_gdi)
		sprintf(buf, "  Title resource id %d\n", (int)*pw);
	    pw++;
	}
	else {
	    if (debug_gdi)
		gs_addmess("  Title name \042");
	    pw = skip_unicode_string(pw);
	    if (debug_gdi)
		gs_addmess("\042\n");
	}
	/* might have creation data */
	if (*pw) {
	    /* skip *pw bytes of data, realigning to word boundary */
	    pw += ((*pw + 1) / sizeof(WORD)) ;	/* skip creation data */
	}
	pw++;	/* skip creation data count */
    }

    fixup = (LPBYTE)pw - (LPBYTE)pt;	/* length of template */
    if (debug_gdi) {
	sprintf(buf, "Processed %d bytes of DLGTEMPLATE\n", fixup);
	gs_addmess(buf);
	gs_addmess("=== End of dialog template ===\n");
    }
    return fixup;
}

/* returns number of WORDs added to structure */
int dlg_add_control(LPDLGTEMPLATE pt, LPWORD pw, 
    LPDLGITEMTEMPLATE dit, WORD control, BOOL bDlgEx)
{
    int fixup;
    LPWORD newpw;
    if (bDlgEx) {
	LPDLGTEMPLATEEX ptex = (LPDLGTEMPLATEEX)pt;
	LPDLGITEMTEMPLATEEX pitex = (LPDLGITEMTEMPLATEEX)pw;
	ptex->cDlgItems += 1;
	pitex->helpID = 0;
	pitex->exStyle = dit->dwExtendedStyle;
	pitex->style = dit->style;
	pitex->x = dit->x;
	pitex->y = dit->y;
	pitex->cx = dit->cx;
	pitex->cy = dit->cy;
	pitex->id = dit->id;
	/* newpw = (LPWORD)((LPBYTE)pw + 22); */
	/* documentation says structure should be 22 bytes, but 24 is needed */
	newpw = (LPWORD)((LPBYTE)pw + 24);
    }
    else {
	LPDLGITEMTEMPLATE pit = (LPDLGITEMTEMPLATE)pw;
	pt->cdit += 1;
	pit->style = dit->style;
	pit->dwExtendedStyle = dit->dwExtendedStyle;
	pit->x = dit->x;
	pit->y = dit->y;
	pit->cx = dit->cx;
	pit->cy = dit->cy;
	pit->id = dit->id;
	newpw = (LPWORD)((LPBYTE)pw + sizeof(DLGITEMTEMPLATE));
    }
    *newpw++ = 0xffff;	/* windowClass */
    *newpw++ = control;	/*   button */
    *newpw++ = 0x0000;	/* title */
    *newpw++ = 0;	/* creation data length */
    fixup = ((LPBYTE)newpw - (LPBYTE)pt) & 3;
    if (fixup == 2)
	*newpw++ = 0;
    return ((LPBYTE)newpw - (LPBYTE)pw) / sizeof(WORD);
}


/* Return a modified printer common dialog template */
/* This template should be released with GlobalFree(); */
HGLOBAL fix_prn_dlg(void)
{
    LPDLGTEMPLATE pt;
    LPWORD pw;
    int length;
    int fixup;
    char buf[MAXSTR];
    HGLOBAL hglobal;
    LPDLGTEMPLATE ptemplate;
    BOOL bDlgEx = FALSE;
    DLGITEMTEMPLATE dit;
    HRSRC hrsrc;
    DWORD dwSize;
    int extra_size;
    HGLOBAL hglobal_new;
    /* int cx; */
    int cy;
    int selpagex;
    int selpagey;

    HMODULE hCommDlg = phInstance;
    if (!is_win32s)  {
	hCommDlg = LoadLibraryA("COMDLG32.DLL");
	if (hCommDlg < (HINSTANCE)HINSTANCE_ERROR) {
	    gs_addmess("comdlg32.dll load failed\n");
	    return NULL;
	}
    }

    hrsrc = FindResource(hCommDlg, MAKEINTRESOURCE(1538), RT_DIALOG);
    dwSize = SizeofResource(hCommDlg, hrsrc);
    hglobal = LoadResource(hCommDlg, hrsrc);

    if ( (ptemplate = (LPDLGTEMPLATE)LockResource(hglobal)) 
	    == (LPDLGTEMPLATE)NULL) {
	gs_addmess("Failed to find print dialog resource 1538\n");
	FreeLibrary(hCommDlg);
	return NULL;
    }

    if (debug & DEBUG_GDI) {
	sprintf(buf, "size of dialog template from resources=%ld\n", dwSize);
	gs_addmess(buf);
    }

    /* Calculate how large our extra items will be */
    /* extra items are 1 group box,  */
    /* 3 auto radio buttons,  3 settings pushbuttons, */
    /* 1 select page pushbutton,  */
    /* 1 help pushbutton */
    if (*(DWORD *)ptemplate == 0xffff0001) {
	extra_size = sizeof(WORD)	/* in case original needs padding */
		+ 9 * (24	/* sizeof(DLGITEMTEMPLATEEX) */
				/*   less windowClass, title, extraCount */
				/*   plus 2 (unknown why this is needed) */
		+ 2 * sizeof(WORD) 	/* class id */
		+ sizeof(WORD)		/* empty string */
		+ sizeof(WORD)		/* creation data length */
		+ sizeof(WORD));	/* padding */
    }
    else {
	extra_size = sizeof(WORD)	/* in case original needs padding */
		+ 9 * (sizeof(DLGITEMTEMPLATE)  /* 18 bytes */
		+ 2 * sizeof(WORD) 	/* class id */
		+ sizeof(WORD)		/* empty string */
		+ sizeof(WORD)		/* creation data length */
		+ sizeof(WORD));	/* padding */
    }
    /* We set the titles of each control in WM_INITDIALOG */
    /* from resource strings. */

    /* Make a copy of the DLGTEMPLATE with extra space for our controls */
    hglobal_new = GlobalAlloc(GPTR, dwSize + extra_size);
    pt = (LPDLGTEMPLATE)GlobalLock(hglobal_new);
    if (pt == NULL) {
	gs_addmess("Failed to allocate space for print dialog template\n");
	FreeLibrary(hCommDlg);
	return NULL;
    }
    memcpy(pt, ptemplate, dwSize);
    if (!is_win32s)
	FreeLibrary(hCommDlg);

    if (debug & DEBUG_GDI) {
	sprintf(buf, "new template size=%ld\n", dwSize + extra_size);
	gs_addmess(buf);
    }

    selpagex = 0;
    selpagey = 0;
    /* parse DLGTEMPLATE and find Page Range groupbox  */
    /* and Selection radio button */
    if (*(DWORD *)pt == 0xffff0001) {
	/* DLGTEMPLATEEX */
	LPDLGITEMTEMPLATEEX pit_pagerange = NULL;
	LPDLGITEMTEMPLATEEX pit_selection = NULL;
	bDlgEx = TRUE;
	length = parse_dlgtemplateex((LPDLGTEMPLATEEX)pt, &pit_pagerange, 1072);
	parse_dlgtemplateex((LPDLGTEMPLATEEX)pt, &pit_selection, 1057);
/*	cx = ((LPDLGTEMPLATEEX)pt)->cx; */
	cy = ((LPDLGTEMPLATEEX)pt)->cy;
	if ((pit_pagerange != 0) && (pit_selection != NULL)) {
	    /* location of Select Pages button */
	    selpagex = pit_pagerange->x + pit_pagerange->cx;
	    selpagey = pit_selection->y + pit_selection->cy/2;
	}
    }
    else {
	/* older DLGTEMPLATE */
	LPDLGITEMTEMPLATE pit_pagerange = NULL;
	LPDLGITEMTEMPLATE pit_selection = NULL;
	length = parse_dlgtemplate(pt, &pit_pagerange, 1072);
	parse_dlgtemplate(pt, &pit_selection, 1057);
/*	cx = pt->cx; */
	cy = pt->cy;
	if ((pit_pagerange != 0) && (pit_selection != NULL)) {
	    /* location of Select Pages button */
	    selpagex = pit_pagerange->x + pit_pagerange->cx;
	    selpagey = pit_selection->y + pit_selection->cy/2;
	}
    }

    pw = (LPWORD)((LPBYTE)pt + length);
    fixup = ((LPBYTE)pw - (LPBYTE)pt) & 3;
    if (fixup == 2)
	*pw++ = 0;

    /* START OF ADDITIONS */
    /* some extra space at bottom of dialog */
    cy += 72;
    if (is_win32s)
	cy += 8;
    if (bDlgEx)
	((LPDLGTEMPLATEEX)pt)->cy = cy;
    else
	pt->cy = cy;

#define ORD_BUTTON 0x0080
    /* "Print Method" groupbox */
    dit.style = BS_GROUPBOX | WS_GROUP | WS_CHILD | WS_VISIBLE;
    dit.dwExtendedStyle = 0;
    dit.x = 8;
    dit.y = cy - 80;
    dit.cx = 164;
    dit.cy = 72;
    dit.id = IDC_DEVICE_GROUP;
    pw += dlg_add_control(pt, pw, &dit, ORD_BUTTON, bDlgEx);

    /* "Windows GDI Printer" auto radio button */
    dit.style = BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE;
    dit.dwExtendedStyle = 0;
    dit.x = 16;
    dit.y = cy - 70;
    dit.cx = 96;
    dit.cy = 12;
    dit.id = IDC_DEVICE_GDI;
    pw += dlg_add_control(pt, pw, &dit, ORD_BUTTON, bDlgEx);

    /* "Ghostscript device" auto radio button */
    dit.style = BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE;
    dit.dwExtendedStyle = 0;
    dit.x = 16;
    dit.y = cy - 50;
    dit.cx = 96;
    dit.cy = 12;
    dit.id = IDC_DEVICE_GS;
    pw += dlg_add_control(pt, pw, &dit, ORD_BUTTON, bDlgEx);

    /* "PostScript printer" auto radio button */
    dit.style = BS_AUTORADIOBUTTON | WS_CHILD | WS_VISIBLE;
    dit.dwExtendedStyle = 0;
    dit.x = 16;
    dit.y = cy - 30;
    dit.cx = 96;
    dit.cy = 12;
    dit.id = IDC_DEVICE_PS;
    pw += dlg_add_control(pt, pw, &dit, ORD_BUTTON, bDlgEx);

    /* "Settings" for PRINT_GDI pushbutton */
    dit.style = BS_PUSHBUTTON | WS_TABSTOP | WS_GROUP | WS_CHILD | WS_VISIBLE;
    dit.dwExtendedStyle = 0;
    dit.x = 114;
    dit.y = cy - 70;
    dit.cx = 50;
    dit.cy = 14;
    dit.id = IDC_GS_WINDOWS;
    pw += dlg_add_control(pt, pw, &dit, ORD_BUTTON, bDlgEx);

    /* "Settings" for PRINT_GS pushbutton */
    dit.style = BS_PUSHBUTTON | WS_TABSTOP | WS_GROUP | WS_CHILD | WS_VISIBLE;
    dit.dwExtendedStyle = 0;
    dit.x = 114;
    dit.y = cy - 50;
    dit.cx = 50;
    dit.cy = 14;
    dit.id = IDC_GS_DEVNAME;
    pw += dlg_add_control(pt, pw, &dit, ORD_BUTTON, bDlgEx);

    /* "Settings" for PRINT_PS pushbutton */
    dit.style = BS_PUSHBUTTON | WS_TABSTOP | WS_GROUP | WS_CHILD | WS_VISIBLE;
    dit.dwExtendedStyle = 0;
    dit.x = 114;
    dit.y = cy - 30;
    dit.cx = 50;
    dit.cy = 14;
    dit.id = IDC_GS_ADVPS;
    pw += dlg_add_control(pt, pw, &dit, ORD_BUTTON, bDlgEx);

    /* "Help" pushbutton */
    dit.style = BS_PUSHBUTTON | WS_TABSTOP | WS_GROUP | WS_CHILD | WS_VISIBLE;
    dit.dwExtendedStyle = 0;
    dit.x = 180;
    dit.y = cy - 70;
    dit.cx = 48;
    if (is_win32s)
	dit.cx = 40;
    dit.cy = 14;
    dit.id = ID_HELP;
    pw += dlg_add_control(pt, pw, &dit, ORD_BUTTON, bDlgEx);

    /* "Select Pages" pushbutton */
    dit.style = BS_PUSHBUTTON | WS_TABSTOP | WS_GROUP | WS_CHILD | WS_VISIBLE;
    dit.dwExtendedStyle = 0;
    dit.cx = 62;
    dit.cy = 14;
    /* Place this near "Selection" radio button */
    /* Do this by placing it vertically aligned with selection radio button */
    /* and right aligned with the page range groupbox */
    if ((selpagex==0) || (selpagey==0)) {
	gs_addmess(
	    "Error: can't find Print Range groupbox or Selection button\n");
	/* place it below the help button instead */
	dit.x = 180;
	dit.y = cy - 50;
    }
    else {
	dit.x = selpagex - dit.cx - 6;
	dit.y = selpagey  - dit.cy / 2;
    }
    dit.id = IDC_SELECT_PAGES;
    pw += dlg_add_control(pt, pw, &dit, ORD_BUTTON, bDlgEx);
/* END OF ADDITIONS */

    if (debug & DEBUG_GDI) {	/* dump it again with our additions */
	if (bDlgEx)
	    parse_dlgtemplateex((LPDLGTEMPLATEEX)pt, NULL, 0);
	else
	    parse_dlgtemplate(pt, NULL, 0);
    }

    GlobalUnlock(hglobal_new);
    return hglobal_new;
}

BOOL query_printer(void)
{
    HGLOBAL hglobal;
    pd.lStructSize = sizeof(pd);
    pd.hwndOwner = hwndimg;
    pd.Flags = PD_RETURNDC;

    pd.nCopies = 1;
    pd.nMinPage = 1;
    pd.nMaxPage = -1;
    pd.nFromPage = 1;
    pd.nToPage = 1;
    /* pd.Flags |= PD_PAGENUMS; */
    if ((psfile.dsc != (CDSC *)NULL) && (psfile.dsc->page_count > 0))
	pd.nToPage = psfile.dsc->page_count;

    /* custom print dialog box */
    /* We get the system default dialog, then make a few changes. */
    hglobal = fix_prn_dlg();
    if (hglobal == NULL)
	return FALSE;
    pd.Flags |= PD_ENABLEPRINTTEMPLATEHANDLE;
    pd.hPrintTemplate = (HANDLE)hglobal;

    pd.Flags |= PD_ENABLEPRINTHOOK;
    pd.lpfnPrintHook = PrintHookProc;
    pd.lCustData = (DWORD)NULL;	/* WM_INITDIALOG lParam */
    pd.Flags |= PD_USEDEVMODECOPIES;

    if ((pd.hDevNames == NULL) && (strlen(option.printer_queue)))
	get_devnames(option.printer_queue, &pd.hDevNames);

    if (!PrintDlg(&pd)) {
	GlobalFree(hglobal);
	return FALSE;
    }

    GlobalFree(hglobal);

    /* Retrieve number of copies from devmode */
    psfile.print_copies = 1;
    if ((pd.hDevMode != NULL) && (pd.hDevMode != INVALID_HANDLE_VALUE)) {
        LPDEVMODE dm = (LPDEVMODE)GlobalLock(pd.hDevMode);
	if (dm->dmFields | DM_COPIES)
	    psfile.print_copies = dm->dmCopies;
        GlobalUnlock(pd.hDevMode);
    }
    if (option.print_method == PRINT_GDI)
	psfile.print_copies = 1;	/* handled by print driver */

    if (pd.Flags & PD_PAGENUMS) {
	psfile.print_from = pd.nFromPage;
	psfile.print_to = pd.nToPage;
	psfile.print_oddeven = ALL_PAGES;
        set_page_range();
    }
    else if (pd.Flags & PD_SELECTION) {
	/* do nothing, we have set psfile.print_* already */
    }
    else {
	psfile.print_from = 1;
	psfile.print_to = 999999;
	psfile.print_oddeven = ALL_PAGES;
        set_page_range();
    }

    if (debug)
	print_devnames(gs_addmess, pd.hDevNames);
    if (debug)
	print_devmode(gs_addmess, pd.hDevMode);
    return TRUE;
}


/* print a range of pages using a Ghostscript device */
void
gsview_print(BOOL convert)
{
    int i;
    if (psfile.name[0] == '\0') {
	gserror(IDS_NOTOPEN, NULL, MB_ICONEXCLAMATION, SOUND_NOTOPEN);
	return;
    }

    if (is_win32s && option.print_method == PRINT_GDI)
	option.print_method = PRINT_GS;
    
    if (convert) {
	if (print_silent) {
	    if (psfile.dsc != (CDSC *)NULL)
		for (i=0; i< (int)psfile.dsc->page_count; i++)
		    psfile.page_list.select[i] = TRUE;
	}
	else {
	    nHelpTopic = IDS_TOPICCONVERT;
	    if (DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_CONVERT), 
		hwndimg, NewDeviceDlgProc, (LPARAM)TRUE) != IDOK)
		return;
	}
    }
    else {
	nHelpTopic = IDS_TOPICPRINT;
	if (print_silent) {
	    if (!open_printer())
		return;
	}
	else {
	    if (!query_printer())
	       return;
	}
	if (option.print_method == PRINT_GDI) {
	    /* create pipe */
	    if (!init_print_gdi(pd.hDC)) {
		DeleteDC(pd.hDC);
		pd.hDC = (HDC)NULL;
		play_sound(SOUND_ERROR);
		return;
	    }
	}
	else {
	    /* PRINT_GS and PRINT_PS don't need device context */
	    DeleteDC(pd.hDC);
	    pd.hDC = (HDC)NULL;
	}
    }
    option.print_to_file = FALSE;
    if (!convert && pd.Flags == PD_PRINTTOFILE)
	option.print_to_file = TRUE;
    if (!convert && (pd.hDevNames != NULL) && 
	(pd.hDevNames != INVALID_HANDLE_VALUE)) {
	LPDEVNAMES lpdevnames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
#ifdef UNICODE
	convert_widechar(option.printer_queue, 
		(TCHAR *)lpdevnames+lpdevnames->wDeviceOffset,
		sizeof(option.printer_queue)-1);
	convert_widechar(option.printer_port, 
		(TCHAR *)lpdevnames+lpdevnames->wOutputOffset,
		sizeof(option.printer_port)-1);
#else
	strncpy(option.printer_queue, 
		(char *)lpdevnames+lpdevnames->wDeviceOffset,
		sizeof(option.printer_queue)-1);
	strncpy(option.printer_port, 
		(char *)lpdevnames+lpdevnames->wOutputOffset,
		sizeof(option.printer_port)-1);
#endif
	if (strcmp(option.printer_port, "FILE:")==0)
	    option.print_to_file = TRUE;
	GlobalUnlock(pd.hDevNames);
    }

    if (!convert)
	option.print_reverse = psfile.page_list.reverse;

    if (!gsview_cprint(printer.psname, printer.optname, convert)) {
	if (print_exit && (print_count==0) && !(debug & DEBUG_GDI))
	    gsview_command(IDM_EXIT);
	return;
    }

    if (is_win32s) 
    {
	/* Win16 and Win32s can't load GS DLL twice */
	/* We must unload the current GS DLL */
	if (gsdll.open)
	    pending.unload = TRUE;
	/* printer_pending will cause start_gvwgs() to be run */
	/* from main message loop, after displaying GS DLL */
	/* has unloaded */
	win32s_printer_pending = TRUE;
	return;
    }

    /* If PRINT_GDI, must use our own special process */
    if (!convert && (option.print_method == PRINT_GDI)) {
	if (!start_gvwgs_with_pipe(pd.hDC)) {
	    DeleteDC(pd.hDC);
	    pd.hDC = NULL;
	}
    }
    else
	start_gvwgs();
    return;
}

