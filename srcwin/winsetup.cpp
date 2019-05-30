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

/* winsetup.cpp */
/* MS-Windows installation program for GSview */

#ifndef __WIN32__
#error Win16 is NOT supported.
#endif
// You also can't compile this with BC++ 4.5, because it
// doesn't include the explorer shell interface

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <objbase.h>
#include <shlobj.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#include <direct.h>
#else
#include <dir.h>
#endif
#include <ctype.h>
#include <io.h>

#ifdef MAX_PATH
#define MAXSTR MAX_PATH
#else
#define MAXSTR 256
#endif

#ifdef _WIN64
#define unlink _unlink
#define mktemp _mktemp
#define stricmp _stricmp
#define strnicmp _strnicmp
#endif

#include "dwinst.h"

extern "C" {
#include "gvcver.h"
#include "gvcbeta.h"
#include "gvwgsver.h"
int message_box(const char *str, int icon);
int load_string(int id, char *str, int len);
typedef HRESULT (WINAPI *PFN_SHGetFolderPath)(
  HWND hwndOwner,
  int nFolder,
  HANDLE hToken,
  DWORD dwFlags,
  LPSTR pszPath);

typedef BOOL (WINAPI *PFN_SHGetSpecialFolderPath)(
  HWND hwndOwner,
  LPTSTR lpszPath,
  int nFolder,
  BOOL fCreate);
}
#include "gvcrc.h"
#include "winsetup.h"
#include "gvclang.h"


#define UNINSTALLPROG "uninstgs.exe"

#ifdef _MSC_VER
#define _export
#endif


/////////////////////////////////
// Globals

CInstall cinst;

// TRUE = Place Start Menu items in All Users.
// FALSE = Current User
BOOL g_bUseCommon;

// TRUE = Destination is the same as Source, so don't copy files.
BOOL g_bNoCopy;

// Source directory, usually a temporary directory created by
// unzip self extractor.
CHAR g_szSourceDir[MAXSTR];

// Target directory for program
// Default loaded from resources
CHAR g_szTargetDir[MAXSTR];

// Target Group for shortcut.
// Default loaded from resources
CHAR g_szTargetGroup[MAXSTR];
// Create group if checkbox selected
BOOL g_bCreateFolder = TRUE;

// Register file associations for these
BOOL g_bAssocPS;
BOOL g_bAssocPDF;

// Setup application name, loaded from resources
CHAR g_szAppName[MAXSTR];

BOOL g_bInstallFonts = TRUE;
BOOL g_bAllUsers = FALSE;


HWND g_hMain = HWND_DESKTOP;		// Main install dialog
HWND g_hWndText;	// Install log dialog
HINSTANCE g_hInstance;
HINSTANCE g_hLanguage;


// If a directory is listed on the command line, g_bBatch will
// be TRUE and a silent install will occur.
BOOL g_bBatch = FALSE;	

BOOL g_bQuit = FALSE;	// TRUE = Get out of message loop.
BOOL g_bError = FALSE;	// TRUE = Install was not successful
int is_win32s;
int is_win4;
BOOL is_winnt = FALSE;	// Disable "All Users" if not NT.

BOOL install_autoexec= TRUE;		/* update autoexec.bat or config.sys */
BOOL install_autoexecbak = TRUE;	/* keep backup of old autoexec.bak */
char error_message[MAXSTR];
char get_string_answer[MAXSTR];
char autoexec_bak[MAXSTR];
BOOL installing;


// prototypes
BOOL init();
void check_language(void);
BOOL make_filelist(int argc, char *argv[]);
#ifdef _WIN64
#define DLGRETURN INT_PTR
#else
#define DLGRETURN BOOL
#endif
DLGRETURN CALLBACK _export ModelessDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
DLGRETURN CALLBACK _export MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void gs_addmess_update(void);



#ifdef _WIN64
char szIniName[]="gsview64.ini";
#else
char szIniName[]="gsview32.ini";
#endif
const char * const bootdrive = "c:";

/* early versions of Win32s don't support lstrcpyn */
#undef lstrcpyn
#define lstrcpyn(d,s,n) strncpy(d,s,n)

HWND hwnd_current;
HWND hwnd_previous;

typedef struct tagWIZPAGE {
   int id;		/* resource ID */
   int prev;		/* resource id of previous page */
   int next;		/* resource id of next page */
   int (*func)(HWND);	/* function to run on exit from page */
   HWND hwnd;		/* window handle of dialog */
} WIZPAGE;

WIZPAGE * find_page_from_id(int id);
void next_page(HWND hwnd);
int init_temp(void);
int check_dir(HWND hwnd);
int make_dir(HWND hwnd);
int done(HWND hwnd);
int finish(HWND hwnd);
void goto_page(HWND hwnd, int id);
int gs_chdir(char *dirname);
BOOL update_config(void);
BOOL update_ini(char *ininame);

WIZPAGE pages[]={
	{IDD_INTRO, IDD_INTRO, IDD_COPYRIGHT, NULL, 0},
	{IDD_COPYRIGHT, IDD_INTRO, IDD_ASSOC, NULL, 0},
	{IDD_ASSOC, IDD_COPYRIGHT, IDD_DIR, NULL, 0},
	{IDD_DIR, IDD_ASSOC, IDD_MKDIR, check_dir, 0},
	{IDD_MKDIR, IDD_DIR, IDD_CONFIG, make_dir, 0},
	{IDD_CONFIG, IDD_DIR, IDD_FINISH, NULL, 0},
	{IDD_FINISH, IDD_DIR, IDD_TEXTWIN, finish, 0},
	{IDD_TEXTWIN, IDD_TEXTWIN, IDD_TEXTWIN, NULL, 0},
	{IDD_DONE, IDD_TEXTWIN, IDD_DONE, done, 0},
	{IDD_FAILED, IDD_TEXTWIN, IDD_FAILED, done, 0},
	{0, 0, 0, NULL, 0}
};


//////////////////////////////////////////////////////////////////////
// Entry point
//////////////////////////////////////////////////////////////////////

#ifdef __BORLANDC__
#pragma argsused
#endif
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	g_hInstance = g_hLanguage = hInstance;

	/* Get source directory.
	 * This used to be the current directory, but is now
	 * the executable directory.
	 * GetCurrentDirectory(sizeof(g_szSourceDir), g_szSourceDir);
	 */
	{   int i;
	    GetModuleFileName(g_hInstance, g_szSourceDir, 
		sizeof(g_szSourceDir));
	    for (i=lstrlen(g_szSourceDir)-1; i>0; i--) {
		if (g_szSourceDir[i] == '\\') {
		    g_szSourceDir[i] = '\0';
		    break;
		}
	    }
	}

	LoadString(g_hInstance, IDS_APPNAME, g_szAppName, sizeof(g_szAppName));
        if (!beta_warn()) {
	    if (!init()) {
		    MessageBox(HWND_DESKTOP, "Initialisation failed", 
			    g_szAppName, MB_OK);
		    return 1;
	    }
	    
	    if (!g_bBatch) {
		    while (GetMessage(&msg, (HWND)NULL, 0, 0)) {
			    if (!IsDialogMessage(hwnd_current, &msg) && 
				    !IsDialogMessage(g_hMain, &msg)) {
				    TranslateMessage(&msg);
				    DispatchMessage(&msg);
			    }
		    }
		    DestroyWindow(g_hMain);
	    }
	}
	
	if (g_hLanguage != g_hInstance)
	    FreeLibrary(g_hLanguage);

	return (g_bError ? 1 : 0);
}



//////////////////////////////////////////////////////////////////////
// Browse dialog box 
//////////////////////////////////////////////////////////////////////

// nasty GLOBALS
char szFolderName[MAXSTR];
char szDirName[MAXSTR];

#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK 
DirDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD notify_message;
	
	switch(message) {
	case WM_INITDIALOG:
		DlgDirList(hwnd, szDirName, IDC_FILES, IDC_FOLDER, 
			DDL_DRIVES | DDL_DIRECTORY);
		SetDlgItemText(hwnd, IDC_TARGET, szFolderName);
		return FALSE;
    case WM_COMMAND:
		notify_message = HIWORD(wParam);
		switch (LOWORD(wParam)) {
		case IDC_FILES:
			if (notify_message == LBN_DBLCLK) {
				CHAR szPath[MAXSTR];
				DlgDirSelectEx(hwnd, szPath, sizeof(szPath), IDC_FILES);
				DlgDirList(hwnd, szPath, IDC_FILES, IDC_FOLDER, 
					DDL_DRIVES | DDL_DIRECTORY);
			}
			return FALSE;
		case IDOK:
			GetDlgItemText(hwnd, IDC_FOLDER, szDirName, sizeof(szDirName));
			GetDlgItemText(hwnd, IDC_TARGET, szFolderName, sizeof(szFolderName));
			EndDialog(hwnd, TRUE);
			return TRUE;
		case IDCANCEL:
			EndDialog(hwnd, FALSE);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////
// text window for log
//////////////////////////////////////////////////////////////////////
#define TWLENGTH 16384
#define TWSCROLL 1024
char twbuf[TWLENGTH];
int twend;

/* Add string for Ghostscript message window */
void
gs_addmess_count(const char *str, int count)
{
    const char *s;
    char *p;
    int i, lfcount;
    MSG msg;

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
	*p++ = *str++;
    }
    twend += (count + lfcount);
    *(twbuf+twend) = '\0';

	
    // Update the dialog box
    if (g_bBatch)
	    return;
    
    gs_addmess_update();
    while (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)) {
	    if (!IsDialogMessage(g_hWndText, &msg) && 
		    !IsDialogMessage(g_hMain, &msg)) {
		    TranslateMessage(&msg);
		    DispatchMessage(&msg);
	    }
    }
}

void
gs_addmess(const char *str)
{
    gs_addmess_count(str, strlen(str));
}

//////////////////////////////////////////////////////////////////////

int
message_box(const char *str, int icon)
{
    return MessageBox(g_hMain, str, g_szAppName, icon);
}

void
message_box(const char *str)
{
    MessageBox(g_hMain, str, g_szAppName, MB_OK);
}

//////////////////////////////////////////////////////////////////////

WIZPAGE *
find_page_from_id(int id)
{
WIZPAGE *page;
    for (page=pages; page->id; page++) {
	if (page->id == id)
	    return page;
    }
    return NULL;
}

void
next_page(HWND hwnd)
{
WIZPAGE *page;
int id;
    for (page=pages; page->id; page++) {
	if (page->hwnd == hwnd) {
	    if (page->func) {
		/* need to execute a function before continuing */
		if ( (id = page->func(hwnd)) != 0) {
		    goto_page(hwnd, id);
		    return;
		}
	    }
	    goto_page(hwnd, page->next);
	    return;
	}
    }
}

void
prev_page(HWND hwnd)
{
WIZPAGE *page;
    for (page=pages; page->id; page++) {
	if (page->hwnd == hwnd) {
	    goto_page(hwnd, page->prev);
	}
    }
}

int
init_temp(void)
{
    if (getenv("TEMP")) {
	install_autoexec = FALSE;
	return 0;	/* assume TEMP is in autoexec.bat */
    }
    return 1;
}


int
check_dir(HWND hwnd)
{
int i;
WIZPAGE *page;
    GetDlgItemText(hwnd, ID_ANSWER, g_szTargetDir, sizeof(g_szTargetDir));
    page = find_page_from_id(IDD_MKDIR);
    if (page)
	SetDlgItemText((page->hwnd), IDD_MKDIR_TEXT, g_szTargetDir);

    /* check if directory exists */
    i = strlen(g_szTargetDir) - 1;
    if ((i >= 2) && (g_szTargetDir[i] == '\\') )
       g_szTargetDir[i] = '\0';
    if (strlen(g_szTargetDir)==0)
	return IDD_DIR;

    if (!gs_chdir(g_szTargetDir)) {
	if (init_temp())
            return IDD_CONFIG;
	else
	    return IDD_FINISH;
    }

    /* if it does not, continue to IDD_MKDIR */
    return 0;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
int
make_dir(HWND hwnd)
{
char buf[MAXSTR];
    if (cinst.MakeDir(g_szTargetDir)) {
	/* succeeded */
	if (init_temp())
            return 0;
	else
	    return IDD_FINISH;
    }

    /* failed */ 
    load_string(IDS_MKDIRFAIL, buf, sizeof(buf)-1);
    message_box(buf);
    return IDD_DIR;
}

void
centre_dialog(HWND hwnd)
{
    // centre dialog on screen
    int width = GetSystemMetrics(SM_CXFULLSCREEN);
    int height = GetSystemMetrics(SM_CYFULLSCREEN);
    RECT rect;
    GetWindowRect(hwnd, &rect);
    if (width / height >= 2)
	width = width / 2;	/* dual monitors */
    MoveWindow(hwnd, (width - (rect.right - rect.left))/2,
	    (height - (rect.bottom - rect.top))/2,
	    (rect.right - rect.left),
	    (rect.bottom - rect.top), FALSE);
}

int
dialog(int resource, DLGPROC dlgproc) 
{
int flag;
    flag = DialogBoxParam(g_hLanguage, MAKEINTRESOURCE(resource), g_hMain, dlgproc, (LPARAM)NULL);
    return flag;
}



/* change directory and drive */
int
gs_chdir(char *dirname)
{
    return !SetCurrentDirectory(dirname);
}

/* extern "C" */ int
load_string(int id, char *str, int len)
{
	return LoadString(g_hLanguage, id, str, len);
}

void
gs_addmess_update(void)
{
  HWND hwndmess = find_page_from_id(IDD_TEXTWIN)->hwnd;

  if (IsWindow(hwndmess)) {
    HWND hwndtext = GetDlgItem(hwndmess, TEXTWIN_MLE);
    DWORD linecount;
    SendMessage(hwndtext, WM_SETREDRAW, FALSE, 0);
    SetDlgItemText(hwndmess, TEXTWIN_MLE, twbuf);
    /* EM_SETSEL, followed by EM_SCROLLCARET doesn't work */
    linecount = SendDlgItemMessage(hwndmess, TEXTWIN_MLE, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
    SendDlgItemMessage(hwndmess, TEXTWIN_MLE, EM_LINESCROLL, (WPARAM)0, (LPARAM)linecount-14);
    SendMessage(hwndtext, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hwndtext, (LPRECT)NULL, TRUE);
    UpdateWindow(hwndtext);
  }
}

void
goto_page(HWND hwnd, int id)
{
WIZPAGE *page;
HWND hbutton;
    page = find_page_from_id(id);
    if (page) {
	ShowWindow(hwnd, SW_HIDE);
	ShowWindow(page->hwnd, SW_SHOW);
	hwnd_current = page->hwnd;
	if (IsWindowEnabled(GetDlgItem(page->hwnd, IDNEXT)))
	    hbutton = GetDlgItem(page->hwnd, IDNEXT);
	else
	    hbutton = GetDlgItem(page->hwnd, IDCANCEL);
	SetFocus(hbutton);
	SendMessage(hbutton, BM_SETSTYLE, (WPARAM)BS_DEFPUSHBUTTON, TRUE);
	return;
    }
}

#ifdef __BORLANDC__
#pragma argsused
#endif
/* exit from program */
int
done(HWND hwnd)
{
    PostQuitMessage(0);
    return 0;
}

/* commence installation */
int
finish(HWND hwnd)
{
WIZPAGE *page;
    page = find_page_from_id(IDD_ASSOC);
    g_bAssocPS = (BOOL)SendDlgItemMessage(page->hwnd, IDC_CFG41, 
	BM_GETCHECK, (WPARAM)0, (LPARAM)0);
    g_bAssocPDF = (BOOL)SendDlgItemMessage(page->hwnd, IDC_CFG42, 
	BM_GETCHECK, (WPARAM)0, (LPARAM)0);

    page = find_page_from_id(IDD_CONFIG);
    install_autoexec = (BOOL)SendDlgItemMessage(page->hwnd, IDM_AUTOEXECBAT, BM_GETCHECK, 
		    (WPARAM)0, (LPARAM)0);
    install_autoexecbak = (BOOL)SendDlgItemMessage(page->hwnd, IDM_AUTOEXECBAK, BM_GETCHECK, 
		    (WPARAM)0, (LPARAM)0);

    page = find_page_from_id(IDD_DIR);
    GetDlgItemText(page->hwnd, ID_ANSWER, g_szTargetDir, sizeof(g_szTargetDir));

    page = find_page_from_id(IDD_FINISH);
    GetDlgItemText(page->hwnd, IDM_PROGMAN2, g_szTargetGroup, sizeof(g_szTargetGroup));
    g_bAllUsers = SendDlgItemMessage(page->hwnd, IDC_ALLUSERS, 
	BM_GETCHECK, 0, 0) == BST_CHECKED;
    g_bCreateFolder = SendDlgItemMessage(page->hwnd, IDC_CREATEFOLDER, 
	BM_GETCHECK, 0, 0) == BST_CHECKED;

    PostMessage(hwnd, WM_COMMAND, (WPARAM)IDFINISH, (LPARAM)0);
    return 0;
}

BOOL
update_config(void)
{
FILE *infile, *outfile;
char inname[MAXSTR], outname[MAXSTR];
char buf[MAXSTR];
char line[1024];
char tempname[MAXSTR];
int i;
BOOL autoexec_exists = TRUE;
    
    strcpy(inname, bootdrive);
    strcat(inname, "\\autoexec.bat");

    strcpy(tempname, bootdrive);
    strcat(tempname, "\\GSXXXXXX");
    if (mktemp(tempname) == (char *)NULL) {
	load_string(IDS_CANTCREATETEMPFILE, error_message, sizeof(error_message));
	return FALSE;
    }

    if ( (infile = fopen(inname, "r")) == (FILE *)NULL) {
	/* autoexec.bat doesn't exist */
	load_string(IDS_CANTOPENREAD, buf, sizeof(buf));
	sprintf(error_message, buf, inname);
	gs_addmess("autoexec.bat already exists\n");
	autoexec_exists = FALSE;
	/* not an error */
    }
    if ( (outfile = fopen(tempname, "w")) == (FILE *)NULL)  {
	load_string(IDS_CANTOPENWRITE, buf, sizeof(buf));
	sprintf(error_message, buf, tempname);
	return FALSE;
    }
    if (infile != (FILE *)NULL) {
	while (fgets(line, sizeof(line), infile)) {
	    if (strnicmp(line, "SET TEMP=", 9)==0) {
		/* it has been added recently */
		fclose(outfile);
		fclose(infile);
		unlink(tempname);
		return TRUE;
	    }
	    fputs(line, outfile);
	}
    }
    sprintf(line, "SET TEMP=%s\\\n", bootdrive);
    fputs(line, outfile);

    fclose(outfile);
    if (infile != (FILE *)NULL)
	fclose(infile);

    if (autoexec_exists) {
	if (install_autoexecbak) {
	    for (i=0; i<=999; i++) {
		sprintf(outname, "%s\\autoexec.%03d", bootdrive, i);
		if ( (infile = fopen(outname, "r")) != (FILE *)NULL)
		    fclose(infile);
		else
		    break;   /* found a suitable name */
	    }
	    if (rename(inname, outname)) {
		load_string(IDS_ERRORRENAME, buf, sizeof(buf));
		sprintf(error_message, buf, inname, outname);
		return FALSE;
	    }
	    strcpy(autoexec_bak, outname);
	}
	else
	    unlink(inname);
    }

    if (rename(tempname, inname)) {
	load_string(IDS_ERRORRENAME, buf, sizeof(buf));
	sprintf(error_message, buf, tempname, inname);
	return FALSE;
    }

    return TRUE;
}

BOOL
update_ini(char *ininame)
{
char buf[16];
char secver[64];
    sprintf(secver, "GSview-%s", GSVIEW_DOT_VERSION);
    sprintf(buf, "%3d", GS_REVISION);
    WritePrivateProfileString(secver, "Configured", "0", ininame);
    WritePrivateProfileString(secver, "GSversion", buf, ininame);
    return TRUE;
}

int
create_object(void)
{

/* MAY NEED TO ADD MORE CODE HERE */

    /* tell user what we have done */
    /*
    SetDlgItemText(find_page_from_id(IDD_DONE)->hwnd, IDD_DONE_GROUP, "Installed OK");
    */

    return 0;
}


DLGPROC lpMainDlgProc;


BOOL
create_dialog(void)
{
WIZPAGE *page;
char buf[MAXSTR];
    // main dialog box
    g_hMain = CreateDialogParam(g_hLanguage, MAKEINTRESOURCE(IDD_MAIN), (HWND)NULL, MainDlgProc, (LPARAM)NULL);
    centre_dialog(g_hMain);
    
    // initialize targets
    cinst.SetMessageFunction(message_box);
    if (!cinst.Init(g_szSourceDir, "filelist.txt")) {
	if (g_hMain != (HWND)NULL)
	    DestroyWindow(g_hMain);
	return FALSE;
    }

    if (init_temp()) {
        page = find_page_from_id(IDD_CONFIG);
	SendDlgItemMessage(page->hwnd, IDM_AUTOEXECBAT, BM_SETCHECK, 
		    (WPARAM)1, (LPARAM)0);
	SendDlgItemMessage(page->hwnd, IDM_AUTOEXECBAK, BM_SETCHECK, 
		    (WPARAM)1, (LPARAM)0);
	find_page_from_id(IDD_FINISH)->prev = IDD_CONFIG; 
    }

    sprintf(buf, "%d.%02d - %d.%02d", 
	GS_REVISION_MIN / 100, GS_REVISION_MIN % 100,
	GS_REVISION_MAX / 100, GS_REVISION_MAX % 100);
    SetDlgItemText(find_page_from_id(IDD_INTRO)->hwnd,
	IDD_INTRO_T3, buf);


    page = find_page_from_id(IDD_ASSOC);
    SendDlgItemMessage(page->hwnd, IDC_CFG41, BM_SETCHECK, 
	    g_bAssocPS ? BST_CHECKED : BST_UNCHECKED, 0);
    SendDlgItemMessage(page->hwnd, IDC_CFG42, BM_SETCHECK, 
	    g_bAssocPDF ? BST_CHECKED : BST_UNCHECKED, 0);

    page = find_page_from_id(IDD_FINISH);
    SetDlgItemText(page->hwnd, IDM_PROGMAN2, g_szTargetGroup); 
    SendDlgItemMessage(page->hwnd, IDC_CREATEFOLDER, BM_SETCHECK, 
	    g_bCreateFolder ? BST_CHECKED : BST_UNCHECKED, 0);
    EnableWindow(GetDlgItem(page->hwnd, IDC_ALLUSERS), is_winnt);
    SendDlgItemMessage(page->hwnd, IDC_ALLUSERS, BM_SETCHECK, 
	    g_bAllUsers ? BST_CHECKED : BST_UNCHECKED, 0);
    
    page = find_page_from_id(IDD_DIR);
    SetDlgItemText(page->hwnd, ID_ANSWER, g_szTargetDir);


    ShowWindow(g_hMain, SW_SHOWNORMAL);

    return TRUE;
}

BOOL get_inipath(char *buf, int len)
{
	/* get path to INI file */
	buf[0] = '\0';
	/* allow for user profiles */
	if (is_win4) {
	    LONG rc;
	    HKEY hkey;
	    DWORD keytype;
	    DWORD cbData;
	    DWORD fa;
	    /* Find the user profile directory */
	    rc = RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\ProfileReconciliation", 0, KEY_READ, &hkey);
	    if (rc == ERROR_SUCCESS) {
		cbData = len - sizeof(szIniName);
		keytype =  REG_SZ;
		rc = RegQueryValueEx(hkey, "ProfileDirectory", 0, &keytype, (LPBYTE)buf, &cbData);
		RegCloseKey(hkey);
	    }
	    if (rc == ERROR_SUCCESS) {
		fa = GetFileAttributes(buf);
		if ((fa != 0xffffffff) && (fa & FILE_ATTRIBUTE_DIRECTORY))
		    strcat(buf, "\\");
		else
		    buf[0] = '\0';
	    }
	    else {
		    /* If we didn't succeed, use the Windows directory */
		    buf[0] = '\0';
	    }
	}
	if (buf[0] == '\0') {
	    DWORD fa;
	    /* If we didn't succeed, try %USERPROFILE% */
	    char *p = getenv("USERPROFILE");
	    if (p && *p) {
		strcpy(buf, p);
#ifdef __BORLANDC__
		OemToCharBuff(buf, buf, lstrlen(buf));
#endif
		p = buf + strlen(buf) - 1;
		if ((*p == '\\') || (*p == '/'))
		    *p = '\0';
		/* check if USERPROFILE contains a directory name */
		fa = GetFileAttributes(buf);
		if ((fa != 0xffffffff) && (fa & FILE_ATTRIBUTE_DIRECTORY))
		    strcat(buf, "\\");
		else
		    buf[0] = '\0';
	    }
	}
	if (strlen(buf) + strlen(szIniName) + 1 >= len)
	   buf[0] = '\0';
	strcat(buf, szIniName);
	return TRUE;
}

BOOL
create_registry_type(const char *keyname, const char *program, 
    const char *description)
{
    const char shellsubkey[]= "\\shell";
    const char opensubkey[] = "\\open";
    const char printsubkey[] = "\\print";
    const char commandsubkey[] = "\\command";
    char kbuf[MAXSTR];
    char buf[MAXSTR];

    BOOL flag = TRUE;

    if (flag)
	flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, keyname);
    if (flag)
	flag = cinst.RegistrySetValue(NULL, description);

    strcpy(kbuf, keyname);
    strcat(kbuf, shellsubkey);
    if (flag)
	flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, kbuf);

    strcat(kbuf, opensubkey);
    if (flag)
	flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, kbuf);

    strcat(kbuf, commandsubkey);
    if (flag)
	flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, kbuf);
    sprintf(buf, "\042%s\042 \042%%1\042", program);
    if (flag)
	flag = cinst.RegistrySetValue(NULL, buf);

    strcpy(kbuf, keyname);
    strcat(kbuf, shellsubkey);
    strcat(kbuf, printsubkey);
    if (flag)
	flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, kbuf);

    strcat(kbuf, commandsubkey);
    if (flag)
	flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, kbuf);
    sprintf(buf, "\042%s\042 /p \042%%1\042", program);
    if (flag)
	flag = cinst.RegistrySetValue(NULL, buf);

    strcpy(kbuf, keyname);
    strcat(kbuf, "\\DefaultIcon");
    if (flag)
	flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, kbuf);
    sprintf(buf, "%s,3", program);
    if (flag)
	flag = cinst.RegistrySetValue(NULL, buf);

    cinst.RegistryCloseKey();

    return flag;
}


BOOL
install_prog()
{
	BOOL flag;
	char *regkey1 = "Ghostgum";
	char *regkey2 = "GSview";
	char buf[MAXSTR];
	char szProgram[MAXSTR];
	char szReadme[MAXSTR];
	char szArguments[MAXSTR];
	char szDescription[MAXSTR];
#ifdef _WIN64
	char *szExeName = "gsview64.exe";
#else
	char *szExeName = "gsview32.exe";
#endif
	
	if (g_bQuit)
		return FALSE;
	
	cinst.SetMessageFunction(gs_addmess);
	cinst.SetTargetDir(g_szTargetDir);
	cinst.SetTargetGroup(g_szTargetGroup);
	cinst.SetAllUsers(g_bAllUsers);
	if (!cinst.Init(g_szSourceDir, "filelist.txt"))
		return FALSE;
	
	// Uninstall any previous version in same directory
	gs_addmess("Uninstalling previous version...\n");
	cinst.Uninstall(UNINSTALLPROG, g_bBatch);

	// Get GS version number
	gs_addmess("Installing GSview...\n");

	// copy files
	if (!cinst.InstallFiles(g_bNoCopy, &g_bQuit)) {
		gs_addmess("Install failed\n");
		return FALSE;
	}
	
	if (g_bQuit)
		return FALSE;

	// Set up program EXE name, used by registry and start menu
	strcpy(szProgram, g_szTargetDir);
	strcat(szProgram, "\\");
	strcat(szProgram, cinst.GetMainDir());
	strcat(szProgram, "\\");
	strcat(szProgram, szExeName);
	strcpy(szArguments, "");
	
	// write registry entries
	gs_addmess("Updating Registry\n");
	if (!cinst.RegistryBegin()) {
	    gs_addmess("Failed to begin registry update\n");
	    return FALSE;
	}

	sprintf(buf, "SOFTWARE\\%s", regkey1);
	if (!cinst.RegistryOpenKey(HKEY_LOCAL_MACHINE, buf)) {
	    gs_addmess("Failed to open/create registry application key\n");
	    return FALSE;
	}

	sprintf(buf, "SOFTWARE\\%s\\%s", regkey1, regkey2);
	if (!cinst.RegistryOpenKey(HKEY_LOCAL_MACHINE, buf)) {
	    gs_addmess("Failed to open/create registry application key\n");
	    return FALSE;
	}

	if (!cinst.RegistrySetValue(GSVIEW_DOT_VERSION, g_szTargetDir)) {
	    gs_addmess("Failed to add registry value\n");
	    return FALSE;
	}

	sprintf(buf, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\%s", szExeName);
        

	cinst.RegistryCloseKey();

	// Write App Paths to registry
	sprintf(buf, 
	    "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\%s",
		szExeName);
	flag = cinst.RegistryOpenKey(HKEY_LOCAL_MACHINE, buf);
	if (flag) {
	    flag = cinst.RegistrySetValue(NULL, szProgram);
	    if (flag) {
		strcpy(buf, g_szTargetDir);
		strcat(buf, "\\");
		strcat(buf, cinst.GetMainDir());
		flag = cinst.RegistrySetValue("Path", buf);
	    }
	    if (!flag)
	        gs_addmess("Failed to open/create registry App Paths values\n");
	    cinst.RegistryCloseKey();
	}
	else
	    gs_addmess("Failed to open/create registry App Paths key\n");
	if (!flag)
	   return flag;


	// now put in assocations

	const char *psmime="application/postscript";
	const char *pdfmime="application/pdf";
	const char *contentname="Content Type";
	const char *extension="Extension";
	const char *pskey="psfile";
	const char *pdfkey="pdffile";
	const char *psext=".ps";
	const char *epsext=".eps";
	const char *pdfext=".pdf";

	if (g_bAssocPS) {
	    BOOL flag = TRUE;
	    if (flag)
	        flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, psext);
	    if (flag)
		flag = cinst.RegistrySetValue(NULL, pskey);
	    if (flag)
		flag = cinst.RegistrySetValue(contentname, psmime);

	    if (flag)
		flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, epsext);
	    if (flag)
		flag = cinst.RegistrySetValue(NULL, pskey);
	    if (flag)
		flag = cinst.RegistrySetValue(contentname, psmime);

	    sprintf(buf, "MIME\\Database\\%s\\%s", contentname, psmime);
	    if (flag)
		flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, buf);
	    if (flag)
		flag = cinst.RegistrySetValue(extension, psext);

	    if (flag)
		flag = create_registry_type(pskey, szProgram, "PostScript");
	    if (!flag) {
		gs_addmess("Failed to add PS file association\n");
		return FALSE;
	    }
	}

	if (g_bAssocPDF) {
	    BOOL flag = TRUE;
	    if (flag)
	        flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, pdfext);
	    if (flag)
		flag = cinst.RegistrySetValue(NULL, pdfkey);
	    if (flag)
		flag = cinst.RegistrySetValue(contentname, pdfmime);

	    sprintf(buf, "MIME\\Database\\%s\\%s", contentname, pdfmime);
	    if (flag)
		flag = cinst.RegistryOpenKey(HKEY_CLASSES_ROOT, buf);
	    if (flag)
		flag = cinst.RegistrySetValue(extension, pdfext);

	    if (flag)
		flag = create_registry_type(pdfkey, szProgram, "Portable Document Format");
	    if (!flag) {
		gs_addmess("Failed to add PDF file association\n");
		return FALSE;
	    }
	}


	if (!cinst.RegistryEnd()) {
		gs_addmess("Failed to end registry update\n");
		return FALSE;
	}
	if (g_bQuit)
		return FALSE;
	
	if (g_bCreateFolder) {
	    // Add Start Menu items
	    gs_addmess("Adding Start Menu items\n");
	    if (!cinst.StartMenuBegin()) {
		    gs_addmess("Failed to begin Start Menu update\n");
		    return FALSE;
	    }
	    // szProgram is already set
	    sprintf(szDescription, "GSview %s", GSVIEW_DOT_VERSION);
	    if (!cinst.StartMenuAdd(szDescription, szProgram, szArguments)) {
		    gs_addmess("Failed to add Start Menu item\n");
		    return FALSE;
	    }
	    strcpy(szReadme, g_szTargetDir);
	    strcat(szReadme, "\\");
	    strcat(szReadme, cinst.GetMainDir());
	    strcat(szReadme, "\\Readme.htm");
	    sprintf(szDescription, "GSview Readme %s", GSVIEW_DOT_VERSION);
	    if (!cinst.StartMenuAdd(szDescription, szReadme, NULL)) {
		    gs_addmess("Failed to add Start Menu item\n");
		    return FALSE;
	    }
	    if (!cinst.StartMenuEnd()) {
		    gs_addmess("Failed to end Start Menu update\n");
		    return FALSE;
	    }
	}
	else {
	    gs_addmess("NOT Adding Start Menu items\n");
	}
	
	// consolidate logs into one uninstall file
	if (cinst.MakeLog()) {
		// add uninstall entry for "Add/Remove Programs"
		gs_addmess("Adding uninstall program\n");
		cinst.SetUninstallDescription(
		    "http://www.ghostgum.com.au/",
		    NULL, 
		    "http://www.ghostgum.com.au/",
		    "Ghostgum Software Pty Ltd",
		    GSVIEW_DOT_VERSION);
		if (!cinst.WriteUninstall(UNINSTALLPROG, g_bNoCopy)) {
			gs_addmess("Failed to write uninstall entry\n");
			return FALSE;
		}
	}
	else {
		gs_addmess("Failed to write uninstall log\n");
		// If batch install, files might be on a server
		// in a write protected directory.
		// Don't return an error for batch install.
		if (g_bBatch)
			return TRUE;
		return FALSE;
	}

	get_inipath(buf, sizeof(buf)-1);
	gs_addmess("Updating INI file ");
	gs_addmess(buf);
	gs_addmess("\n");
	if (!update_ini(buf))
	    return FALSE;

	if (g_bAllUsers) {
	    /* Write a gsview32.ini file to allow use of GSview
	     * by any user, not just the one who installed GSview.
	     * This INI file will cause auto-configuration if
	     * the user runs GSview for the first time, or was
	     * previously using a different version of GSview.
	     * If Ghostscript isn't already installed, don't bother.
	     */
	    int gsver = 0;
	    int count;
	    int *ver;

	    count = 1;
	    get_gs_versions(&count);
	    if (count >= 1) {
		ver = (int *)malloc((count+1)*sizeof(int));
		if (ver == (int *)NULL)
		    return FALSE;
		ver[0] = count+1;
		if (get_gs_versions(ver)) {
		    for (int i=1; i<=ver[0]; i++) {
			if (ver[i] > gsver)
			    gsver = ver[i];
		    }
		}
		free(ver);
	    }
	    if (gsver == 0) {
		gs_addmess("If installing for all users, you must install Ghostscript first.\n");
	    }
	    else {
		FILE *f;
		char buf[256];
		const char *p = cinst.GetUninstallName();

		strcpy(szProgram, g_szTargetDir);
		strcat(szProgram, "\\");
		strcat(szProgram, cinst.GetMainDir());
		strcat(szProgram, "\\");
		strcat(szProgram, szIniName);
		gs_addmess("Writing ");
		gs_addmess(szProgram);
		gs_addmess("\n");
		if ((f = fopen(szProgram, "w")) == (FILE *)NULL) {
		    gs_addmess("Failed\n");
		    return FALSE;
		}

		/* Skip over "GSview 3.6" to get to version number */
		while (*p && *p != ' ')
		    p++;
		while (*p && *p == ' ')
		    p++;
		fprintf(f, "[GSview-%s]\n", p);
		fprintf(f, "Version=%s\n", p);

		fprintf(f, "GSversion=%d\n", gsver);

		if (!get_gs_string(gsver, "GS_DLL", buf, sizeof(buf))) {
		    return FALSE;
		    fclose(f);
		}
		fprintf(f, "GhostscriptDLL=%s\n", buf);
		if (!get_gs_string(gsver, "GS_LIB", buf, sizeof(buf))) {
		    return FALSE;
		    fclose(f);
		}
		fprintf(f, "GhostscriptInclude=%s\n", buf);
		fprintf(f, "GhostscriptOther=-dNOPLATFONTS -sFONTPATH=\042c:\\psfonts\042\n");
		fprintf(f, "Configured=1\n");
		fclose(f);
	    }
	}

	if (install_autoexec)
	    if (!update_config()) {
		gs_addmess(error_message);
		gs_addmess("\n");
		return FALSE;
	    }
	
	gs_addmess("Program install successful\n");
	return TRUE;
}


// install program and files
BOOL
install_all()
{
	gs_addmess("Source Directory=");
	gs_addmess(g_szSourceDir);
	gs_addmess("\n");
	gs_addmess("Target Directory=");
	gs_addmess(g_szTargetDir);
	gs_addmess("\n");
	gs_addmess("Target Shell Folder=");
	gs_addmess(g_szTargetGroup);
	gs_addmess("\n");
	gs_addmess(g_bAllUsers ? "  All users\n" : "  Current user\n");
	
	if (stricmp(g_szSourceDir, g_szTargetDir) == 0) {
		// Don't copy files
		if (!g_bBatch)
			if (::MessageBox(g_hWndText, "Install location is the same as the current file location.  No files will be copied.", g_szAppName, MB_OKCANCEL) 
				!= IDOK) {
				return FALSE;
			}
		g_bNoCopy = TRUE;
	}

	if (g_bQuit)
		return FALSE;
	
	if (!install_prog()) {
		cinst.CleanUp();
		g_bError = TRUE;
		return FALSE;
	}
	
	gs_addmess("Install successful\n");
	
	// show start menu folder
	if (!g_bBatch) {
		char szFolder[MAXSTR];
		szFolder[0] = '\0';
		cinst.GetPrograms(g_bAllUsers, szFolder, sizeof(szFolder));
		strcat(szFolder, "\\");
		strcat(szFolder, g_szTargetGroup);
		ShellExecute(HWND_DESKTOP, "open", szFolder, 
			NULL, NULL, SW_SHOWNORMAL);
	}
	
#ifdef DEBUG
	return FALSE;
#endif

	return TRUE;
}

/* Simplified from gvwreg.cpp */
#define REG_KEY_NAME "Software\\Ghostgum\\GSview"
#define REGISTRATION_RECEIPT "Receipt"
#define REGISTRATION_NUMBER "Number"
#define REGISTRATION_NAME "Name"
BOOL
write_registration(unsigned int reg_receipt, unsigned int reg_number,
  char *reg_name)
{
    LONG rc;
    HKEY hkey;
    DWORD dwValue;
    HKEY root;
    char *name;
    char *value;
   
    root = HKEY_LOCAL_MACHINE;
    name = REG_KEY_NAME;
    value = NULL;
    if ((rc = RegOpenKeyEx(root, name, 0, 
	    KEY_ALL_ACCESS, &hkey)) != ERROR_SUCCESS) {
	/* failed to open key, so try to create it */
	rc = RegCreateKey(root, name, &hkey);
    }

    if (rc == ERROR_SUCCESS) {
	dwValue = (DWORD)reg_receipt;
	value = REGISTRATION_RECEIPT;
	rc = RegSetValueEx(hkey, value, 0, REG_DWORD,
		    (CONST BYTE *)&dwValue, sizeof(DWORD));

	dwValue = (DWORD)(reg_number ^ 0xffff);
	if (rc == ERROR_SUCCESS) {
	    value = REGISTRATION_NUMBER;
	    rc = RegSetValueEx(hkey, value, 0, REG_DWORD,
		    (CONST BYTE *)&dwValue, sizeof(DWORD));
	}

	if (rc == ERROR_SUCCESS) {
	    value = REGISTRATION_NAME;
	    rc = RegSetValueEx(hkey, value, 0, REG_SZ,
		    (CONST BYTE *)reg_name, lstrlen(reg_name)+1);
	}
	RegCloseKey(hkey);
    }
    
    if (rc != ERROR_SUCCESS) {
/*
	registry_error(root, name, value, FALSE, rc);
*/
	return FALSE;
    }
    return TRUE;
}

#ifndef CSIDL_PROGRAM_FILES
#define CSIDL_PROGRAM_FILES 0x0026
#endif
#ifndef CSIDL_FLAG_CREATE
#define CSIDL_FLAG_CREATE 0x8000
#endif
#ifndef SHGFP_TYPE_CURRENT
#define SHGFP_TYPE_CURRENT 0
#endif

BOOL 
GetProgramFiles(LPTSTR path) 
{
    PFN_SHGetSpecialFolderPath PSHGetSpecialFolderPath = NULL;
    PFN_SHGetFolderPath PSHGetFolderPath = NULL;
    HMODULE hModuleShell32 = NULL;
    HMODULE hModuleShfolder = NULL;
    BOOL fOk = FALSE;
    hModuleShfolder = LoadLibrary("shfolder.dll");
    hModuleShell32 = LoadLibrary("shell32.dll");

    if (hModuleShfolder) {
	PSHGetFolderPath = (PFN_SHGetFolderPath)
	    GetProcAddress(hModuleShfolder, "SHGetFolderPathA");
	if (PSHGetFolderPath) {
	    fOk = (PSHGetFolderPath(HWND_DESKTOP, 
		CSIDL_PROGRAM_FILES | CSIDL_FLAG_CREATE, 
		NULL, SHGFP_TYPE_CURRENT, path) == S_OK);
	}
    }

    if (!fOk && hModuleShell32) {
	PSHGetFolderPath = (PFN_SHGetFolderPath)
	    GetProcAddress(hModuleShell32, "SHGetFolderPathA");
	if (PSHGetFolderPath) {
	    fOk = (PSHGetFolderPath(HWND_DESKTOP, 
		CSIDL_PROGRAM_FILES | CSIDL_FLAG_CREATE, 
		NULL, SHGFP_TYPE_CURRENT, path) == S_OK);
	}
    }

    if (!fOk && hModuleShell32) {
	PSHGetSpecialFolderPath = (PFN_SHGetSpecialFolderPath)
	    GetProcAddress(hModuleShell32, "SHGetSpecialFolderPathA");
	if (PSHGetSpecialFolderPath) {
	    fOk = PSHGetSpecialFolderPath(HWND_DESKTOP, path,
		CSIDL_PROGRAM_FILES, TRUE);
	}
    }

    if (!fOk) {
	/* If all else fails (probably Win95), try the registry */
	LONG rc;
	HKEY hkey;
	DWORD cbData;
	DWORD keytype;
	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
	    "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", 0, KEY_READ, &hkey);
	if (rc == ERROR_SUCCESS) {
	    cbData = MAX_PATH;
	    keytype =  REG_SZ;
	    if (rc == ERROR_SUCCESS)
		rc = RegQueryValueEx(hkey, "ProgramFilesDir", 0, &keytype, 
		    (LPBYTE)path, &cbData);
	    RegCloseKey(hkey);
	}
	fOk = (rc == ERROR_SUCCESS);
    }
    return fOk;
}


BOOL
init()
{
    DWORD dwVersion = GetVersion();
    char *szRegName = NULL;
    int nRegReceipt = 0;
    int nRegNumber = 0;

    /* find out if we are running under Win32s */
    /* Win32s */
    if ( ((HIWORD(dwVersion) & 0x8000)!=0) && 
	((HIWORD(dwVersion) & 0x4000)==0) )
	    is_win32s = TRUE;
    /* Windows 4.0 */
    if (LOBYTE(LOWORD(dwVersion)) >= 4)
	is_win4 = TRUE;
    /* Windows NT */
    if ( (HIWORD(dwVersion) & 0x8000) == 0)
	    is_winnt = TRUE;

    if (is_winnt)
	g_bAllUsers = TRUE;

    // load strings
    LoadString(g_hInstance, IDS_TARGET_GROUP, 
	    g_szTargetGroup, sizeof(g_szTargetGroup));

    // other defaults
    g_bAssocPS = TRUE;
    g_bAssocPDF = FALSE;
    
    if (LOBYTE(LOWORD(dwVersion)) < 4) {
    MessageBox(HWND_DESKTOP, 
		    "This install program needs Windows 4.0 or later",
		    g_szAppName, MB_OK);
	    return FALSE;
    }
    
    
    cinst.SetMessageFunction(message_box);

#define MAXCMDTOKENS 128

    int argc;
    LPSTR argv[MAXCMDTOKENS];
    LPSTR p;
    char command[256];
    char *args;
    char *d, *e;
    int i;
 
    p = GetCommandLine();

    argc = 0;
    args = (char *)malloc(lstrlen(p)+1);
    if (args == (char *)NULL)
	    return 1;
   
    // Parse command line handling quotes.
    d = args;
    while (*p) {
	    // for each argument

	    if (argc >= MAXCMDTOKENS - 1)
		    break;

	    e = d;
	    while ((*p) && (*p != ' ')) {
		    if (*p == '\042') {
			    // Remove quotes, skipping over embedded spaces.
			    // Doesn't handle embedded quotes.
			    p++;
			    while ((*p) && (*p != '\042'))
				    *d++ =*p++;
		    }
		    else 
			    *d++ = *p;
		    if (*p)
			    p++;
	    }
	    *d++ = '\0';
	    argv[argc++] = e;

	    while ((*p) && (*p == ' '))
		    p++;	// Skip over trailing spaces
    }
    argv[argc] = NULL;

    if (strlen(argv[0]) == 0) {
	    GetModuleFileName(g_hInstance, command, sizeof(command)-1);
	    argv[0] = command;
    }

    if ((argc > 2) && (strcmp("-filelist", argv[1])==0)) {
	    // Probably creating filelist.txt
	    return make_filelist(argc, argv);
    }

    for (i=1; i<argc; i++) {
	if (strcmp(argv[i], "-name") == 0) {
	    i++;
	    szRegName = argv[i];
	}
	else if (strcmp(argv[i], "-number") == 0) {
	    char *n;
	    i++;
	    n = argv[i];
	    while (*n && *n != '-')
		n++;
	    if (*n == '-')
		n++;
	    nRegReceipt = atoi(argv[i]);
	    nRegNumber = atoi(n);
	}
	else {
	    // Directory specified, so batch mode requested
	    strncpy(g_szTargetDir, argv[i], sizeof(g_szTargetDir));
	    g_bBatch = TRUE;
	    init_temp();	/* find out if TEMP is defined */
	}
    }

    if (szRegName && nRegReceipt)
	 write_registration(nRegReceipt, nRegNumber, szRegName);

    if (g_bBatch) {
	    if (!install_all()) {
		    // display log showing error
		    g_bBatch = FALSE;
		    create_dialog();
		    goto_page(hwnd_current, IDD_TEXTWIN);
		    WIZPAGE *page = find_page_from_id(IDD_TEXTWIN);
		    page->next = IDD_FAILED;	/* KLUDGE */
		    EnableWindow(GetDlgItem(page->hwnd, IDNEXT), TRUE);
		    gs_addmess_update();
	    }
	    return TRUE;
    }
    
    // Interactive setup
    check_language();
    if (!GetProgramFiles(g_szTargetDir))
	strcpy(g_szTargetDir, "C:\\Program Files");
    strcat(g_szTargetDir, "\\");
    LoadString(g_hInstance, IDS_TARGET_DIR, 
	g_szTargetDir+strlen(g_szTargetDir), 
	sizeof(g_szTargetDir)-strlen(g_szTargetDir));
    
    // main dialog box
    if (!create_dialog())
	    return FALSE;

    return (g_hMain != (HWND)NULL); /* success */
}


/* returns TRUE if language change successful */
BOOL
load_language(int language)
{   /* load language dependent resources */
char langdll[MAXSTR];
HINSTANCE hInstance;
    /* load language dependent resources */
    strcpy(langdll, g_szSourceDir);
#ifdef DECALPHA
    strcat(langdll, "\\setpda");
#else
    strcat(langdll, "\\setp32");
#endif
    switch (language) {
	case IDM_LANGCT:
	    strcat(langdll, "ct");
	    break;
	case IDM_LANGDE:
	    strcat(langdll, "de");
	    break;
	case IDM_LANGES:
	    strcat(langdll, "es");
	    break;
	case IDM_LANGFR:
	    strcat(langdll, "fr");
	    break;
	case IDM_LANGGR:
	    strcat(langdll, "gr");
	    break;
	case IDM_LANGIT:
	    strcat(langdll, "it");
	    break;
	case IDM_LANGNL:
	    strcat(langdll, "nl");
	    break;
	case IDM_LANGRU:
	    strcat(langdll, "ru");
	    break;
	case IDM_LANGSE:
	    strcat(langdll, "se");
	    break;
	case IDM_LANGSK:
	    strcat(langdll, "sk");
	    break;
	case IDM_LANGEN:
	default:
	    g_hLanguage = g_hInstance;
	    return TRUE;
    }
    strcat(langdll, ".dll");
    hInstance = LoadLibrary(langdll);
    if (hInstance >= (HINSTANCE)HINSTANCE_ERROR) {
	g_hLanguage = hInstance;
	return TRUE;
    }
    
    g_hLanguage = g_hInstance;
    return FALSE;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
/* language dialog box */
DLGRETURN CALLBACK _export
LanguageDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
	case WM_INITDIALOG:
	    centre_dialog(hDlg);
	    ShowWindow(hDlg, SW_SHOWNORMAL);
            return( TRUE);
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    return(TRUE);
                case IDM_LANGEN:
                case IDM_LANGCT:
                case IDM_LANGDE:
                case IDM_LANGES:
                case IDM_LANGFR:
                case IDM_LANGGR:
                case IDM_LANGIT:
                case IDM_LANGNL:
                case IDM_LANGRU:
                case IDM_LANGSE:
                case IDM_LANGSK:
                    EndDialog(hDlg, LOWORD(wParam));
                    return(TRUE);
                default:
                    return(FALSE);
            }
    }
    return(FALSE);
}

/* prompt to change language if Windows language doesn't match */
/* GSview language */
void 
check_language(void)
{
char winlang[MAXSTR];
int language;
    GetProfileString("Intl", "sLanguage", "ENG", winlang, sizeof(winlang));
    /* if Window language isn't English */
    if ( strnicmp(winlang, "EN", 2)
#ifdef BETA
      || TRUE
#endif
	)
    {
	language = DialogBoxParam(g_hLanguage, MAKEINTRESOURCE(IDD_LANG), HWND_DESKTOP, LanguageDlgProc, (LPARAM)NULL);
	switch (language) {
	    case IDM_LANGEN:
	    case IDM_LANGCT:
	    case IDM_LANGDE:
	    case IDM_LANGES:
	    case IDM_LANGFR:
	    case IDM_LANGGR:
	    case IDM_LANGIT:
	    case IDM_LANGNL:
	    case IDM_LANGRU:
	    case IDM_LANGSE:
	    case IDM_LANGSK:
		load_language(language);
	}
    }
}


#ifdef __BORLANDC__
#pragma argsused	/* ignore warning for next function */
#endif
/* input string dialog box */
DLGRETURN CALLBACK _export
InputDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
	    SetDlgItemText(hDlg, ID_ANSWER, get_string_answer);
            return( TRUE);
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case IDOK:
		    GetDlgItemText(hDlg, ID_ANSWER, get_string_answer, sizeof(get_string_answer));
                    EndDialog(hDlg, IDOK);
                    return(TRUE);
                case IDCANCEL:
                    EndDialog(hDlg, IDCANCEL);
                    return(TRUE);
                default:
                    return(FALSE);
            }
    }
    return(FALSE);
}

#ifdef __BORLANDC__
#pragma argsused	/* ignore warning for next function */
#endif
/* Modeless Dialog Box */
DLGRETURN CALLBACK _export
MainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
static BOOL initialised;
    switch(message) {
        case WM_INITDIALOG:
	    /* create child dialog windows */
	    if (!initialised) {
		WIZPAGE *page;
		initialised=TRUE;
		for (page=pages; page->id; page++) {
		    page->hwnd = CreateDialogParam(g_hLanguage, MAKEINTRESOURCE(page->id), hDlg, ModelessDlgProc, (LPARAM)NULL);
		    ShowWindow(page->hwnd, SW_HIDE);
		}
		ShowWindow(pages[0].hwnd, SW_SHOW);
		SetFocus(GetDlgItem(pages[0].hwnd, IDNEXT));
		SendDlgItemMessage(pages[0].hwnd, IDNEXT, BM_SETSTYLE, 
		    (WPARAM)BS_DEFPUSHBUTTON, TRUE);
		hwnd_current = pages[0].hwnd;
	    }
            return FALSE;	/* we decide the focus */
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDCANCEL:
                case IDOK:
		    PostQuitMessage(0);
                    return(TRUE);
                default:
                    return(FALSE);
            }
	case WM_CLOSE:
	    PostQuitMessage(0);
	    return TRUE;
    }
    return FALSE;
}


#ifdef __BORLANDC__
#pragma argsused	/* ignore warning for next function */
#endif
/* Modeless Dialog Box */
DLGRETURN CALLBACK _export
ModelessDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
            return( TRUE);
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case IDOK:
                case IDNEXT:
		    next_page(hDlg);
                    return(TRUE);
                case IDPREV:
		    SendDlgItemMessage(hDlg, IDPREV, BM_SETSTYLE, 
		        (WPARAM)0, TRUE);	/* remove default style */
		    prev_page(hDlg);
                    return(TRUE);
		case IDFINISH:
		    installing = TRUE;
		    if (!install_all() || g_bQuit) {
			WIZPAGE *page;
			/* FAILED */
		        page = find_page_from_id(IDD_TEXTWIN);
			page->next = IDD_FAILED;	/* KLUDGE */
			EnableWindow(GetDlgItem(page->hwnd, IDNEXT), TRUE);
			SetDlgItemText(find_page_from_id(IDD_FAILED)->hwnd,
			   IDD_FAILED_REASON, error_message); 
			gs_addmess(error_message);
			gs_addmess_update();
		    }
		    else {
			WIZPAGE *page;
		        page = find_page_from_id(IDD_TEXTWIN);
			page->next = IDD_DONE;	/* KLUDGE */
			EnableWindow(GetDlgItem(page->hwnd, IDNEXT), TRUE);
			gs_addmess_update();
		    }
		    if (autoexec_bak[0] != '\0') {
			char buf1[MAXSTR], buf2[MAXSTR];
			load_string(IDS_DONEBAK, buf1, sizeof(buf2));
			sprintf(buf2, buf1, autoexec_bak);
			SetDlgItemText(find_page_from_id(IDD_DONE)->hwnd,
			   IDD_DONE_BAK, buf2); 
		    }
		    installing = FALSE;
		    PostMessage(find_page_from_id(IDD_TEXTWIN)->hwnd,
			WM_COMMAND, (WPARAM)IDNEXT, (LPARAM)0);
                    return(TRUE);
                case IDCANCEL:
		    g_bQuit = TRUE;
		    if (installing)
		        gs_addmess("\ncancelling\n");
		    else 
		         PostMessage(GetParent(hDlg), WM_COMMAND, 
				(WPARAM)IDCANCEL, (LPARAM)0);
                    return(TRUE);
		case IDC_CREATEFOLDER:
		    if (HIWORD(wParam) == BN_CLICKED) {
			BOOL bFolder;
			bFolder = SendDlgItemMessage(hDlg, IDC_CREATEFOLDER, 
			    BM_GETCHECK, 0, 0) == BST_CHECKED;
			EnableWindow(GetDlgItem(hDlg, IDM_PROGMAN2), bFolder);
			EnableWindow(GetDlgItem(hDlg, IDC_ALLUSERS), bFolder);
			EnableWindow(GetDlgItem(hDlg, IDC_BROWSE_GROUP), bFolder);
		    }
                    return(TRUE);
		case IDC_VIEWREADME:
		    {
		    char szReadmeName[MAXSTR];
		    strcpy(szReadmeName, g_szSourceDir);
		    strcat(szReadmeName, "\\gsview\\Readme.htm");
		    ShellExecute(hDlg, "open", szReadmeName, 
			NULL, NULL, SW_SHOWNORMAL);
		    }
                    return(TRUE);
		case IDC_VIEWHELP:
		    {
		    char szHelpName[MAXSTR];
		    strcpy(szHelpName, g_szSourceDir);
		    strcat(szHelpName, "\\gsview\\");
		    load_string(IDS_HELPFILE, szHelpName+strlen(szHelpName),
			sizeof(szHelpName) - strlen(szHelpName) - 1);
		    ShellExecute(hDlg, "open", szHelpName, 
			NULL, NULL, SW_SHOWNORMAL);
		    }
                    return(TRUE);
		case IDC_BROWSE_DIR:
		    { char dir[MAXSTR];
		    char *p;
		    GetDlgItemText(find_page_from_id(IDD_DIR)->hwnd, 
			ID_ANSWER, dir, sizeof(dir));
		    strcpy(szDirName, dir);
		    if ( (p = strrchr(szDirName, '\\')) != (char *)NULL ) {
			    strcpy(szFolderName, p+1);
			    if (p == szDirName+2)
				    p++;	// step over c:\   //
			    *p = '\0';
		    }
		    else {
			    strcpy(szDirName, "c:\\");
			    strcpy(szFolderName, dir);
		    }
		    if (DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIRDLG), 
			    hDlg, DirDlgProc)) {
			    strcpy(dir, szDirName);
			    if (strlen(dir) && (dir[strlen(dir)-1] != '\\'))
				    strcat(dir, "\\");
			    strcat(dir, szFolderName);
			    SetDlgItemText(find_page_from_id(IDD_DIR)->hwnd, 
				ID_ANSWER, dir);
		    }
		    }
		    return(TRUE);
		case IDC_BROWSE_GROUP:
		    { char dir[MAXSTR];
		    char programs[MAXSTR];
		    char *p;
		    GetDlgItemText(find_page_from_id(IDD_FINISH)->hwnd, 
			IDM_PROGMAN2, dir, sizeof(dir));
		    cinst.GetPrograms(
			    SendDlgItemMessage(hDlg, IDC_ALLUSERS,
			    BM_GETCHECK, 0, 0) == BST_CHECKED,
			    programs, sizeof(programs));
		    strcpy(szDirName, programs);
		    strcpy(szFolderName, dir);
		    if (DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_DIRDLG), 
			    hDlg, DirDlgProc)) {
			    strcpy(dir, szFolderName);
			    p = szDirName;
			    if (strnicmp(szDirName, programs, 
				    strlen(programs)) == 0) {
				    p += strlen(programs);
				    if (*p == '\\')
					    p++;
				    strcpy(dir, p);
				    if (strlen(dir) && 
					    (dir[strlen(dir)-1] != '\\'))
					    strcat(dir, "\\");
				    strcat(dir, szFolderName);
			    }
			    SetDlgItemText(find_page_from_id(IDD_FINISH)->hwnd,
				IDM_PROGMAN2, dir);
		    }
		    }
		    return TRUE;
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
			MessageBeep(~0);
			return(FALSE);
		    }
		    p = (LPSTR)GlobalLock(hglobal);
		    if (p == (LPSTR)NULL) {
			MessageBeep(~0);
			return(FALSE);
		    }
		    lstrcpyn(p, twbuf+start, end-start);
		    GlobalUnlock(hglobal);
		    OpenClipboard(hDlg);
		    EmptyClipboard();
		    SetClipboardData(CF_TEXT, hglobal);
		    CloseClipboard();
		    }
                default:
                    return(FALSE);
            }
	case WM_CLOSE:
	    PostMessage(GetParent(hDlg), WM_COMMAND, (WPARAM)IDCANCEL, (LPARAM)0);
	    return TRUE;
    }
    return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Create file list
//////////////////////////////////////////////////////////////////////

FILE *fList;

typedef int (*PFN_dodir)(const char *name);

/* Called once for each directory */
#ifdef __BORLANDC__
#pragma argsused
#endif
int
dodir(const char *filename)
{
    return 0;
}

/* Called once for each file */
int
dofile(const char *filename)
{
    if (fList != (FILE *)NULL) {
		fputs(filename, fList);
		fputs("\n", fList);
    }
	
    return 0;
}


/* Walk through directory 'path', calling dodir() for given directory
 * and dofile() for each file.
 * If recurse=1, recurse into subdirectories, calling dodir() for
 * each directory.
 */
int 
dirwalk(char *path, int recurse, PFN_dodir dodir, PFN_dodir dofile)
{    
	WIN32_FIND_DATA find_data;
	HANDLE find_handle;
	char pattern[MAXSTR];	/* orig pattern + modified pattern */
	char base[MAXSTR];
	char name[MAXSTR];
	BOOL bMore = TRUE;
	char *p;
	
	
	if (path) {
		strcpy(pattern, path);
		if (strlen(pattern) != 0)  {
			p = pattern + strlen(pattern) -1;
			if (*p == '\\')
				*p = '\0';		// truncate trailing backslash
		}
		
		strcpy(base, pattern);
		if (strchr(base, '*') != NULL) {
			// wildcard already included
			// truncate it from the base path
			if ( (p = strrchr(base, '\\')) != NULL )
				*(++p) = '\0';
		}
		else if (isalpha(pattern[0]) && 
			pattern[1]==':' && pattern[2]=='\0')  {
			strcat(pattern, "\\*");		// search entire disk
			strcat(base, "\\");
		}
		else {
			// wildcard NOT included
			// check to see if path is a directory
			find_handle = FindFirstFile(pattern, &find_data);
			if (find_handle != INVALID_HANDLE_VALUE) {
				FindClose(find_handle);
				if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					strcat(pattern, "\\*");		// yes, search files 
					strcat(base, "\\");
				}
				else {
					dofile(path);				// no, return just this file
					return 0;
				}
			}
			else
				return 1;	// path invalid
		}
	}
	else {
		base[0] = '\0';
		strcpy(pattern, "*");
	}
	
	find_handle = FindFirstFile(pattern,  &find_data);
	if (find_handle == INVALID_HANDLE_VALUE)
		return 1;
	
	while (bMore) {
		strcpy(name, base);
		strcat(name, find_data.cFileName);
		if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if ( strcmp(find_data.cFileName, ".") && 
				strcmp(find_data.cFileName, "..") ) {
				dodir(name);
				if (recurse)
					dirwalk(name, recurse, dodir, dofile);
			}
		}
		else {
			dofile(name);
		}
		bMore = FindNextFile(find_handle, &find_data);
	}
	FindClose(find_handle);
	
	return 0;
}



// This is used when creating a file list.

BOOL make_filelist(int argc, char *argv[])
{
    char *title = NULL;
    char *dir = NULL;
    char *list = NULL;
    int i;
    g_bBatch = TRUE;	// Don't run message loop
	
    for (i=1; i<argc; i++) {
		if (strcmp(argv[i], "-filelist") == 0) {
		    /* ignore */
		}
		if (strcmp(argv[i], "-title") == 0) {
			i++;
			title = argv[i];
		}
		else if (strcmp(argv[i], "-dir") == 0) {
			i++;
			dir = argv[i];
		}
		else if (strcmp(argv[i], "-list") == 0) {
			i++;
			list = argv[i];
		}
		else {
		    if ((title == NULL) || (strlen(title) == 0) ||
			(dir == NULL) || (strlen(dir) == 0) ||
			(list == NULL) || (strlen(list) == 0)) {
			message_box("Usage: setupgs -filelist -title \042AFPL Ghostscript #.##\042 -dir \042gs#.##\042 -list \042filelist.txt\042 spec1 spec2 specn\n");
			return FALSE;
		    }
		    if (fList == (FILE *)NULL) {
			    if ( (fList = fopen(list, "w")) == (FILE *)NULL ) {
					message_box("Can't write list file\n");
					return FALSE;
			    }
			    fputs(title, fList);
			    fputs("\n", fList);
			    fputs(dir, fList);
			    fputs("\n", fList);
		    }
		    if (argv[i][0] == '@') {
			// Use @filename with list of files/directories
			// to avoid DOS command line limit
			FILE *f;
			char buf[MAXSTR];
			int j;
			if ( (f = fopen(&(argv[i][1]), "r")) != (FILE *)NULL) {
			    while (fgets(buf, sizeof(buf), f)) {
				// remove trailing newline and spaces
				while ( ((j = strlen(buf)-1) >= 0) &&
				    ((buf[j] == '\n') || (buf[j] == ' ')) )
				    buf[j] = '\0';
			        dirwalk(buf, TRUE, dodir, dofile);
			    }
			    fclose(f);
			}
			else {
				wsprintf(buf, "Can't open @ file \042%s\042",
				    &argv[i][1]);
				message_box(buf);
			}
		    }
		    else
		        dirwalk(argv[i], TRUE, dodir, dofile);
		}
    }
	
    if (fList != (FILE *)NULL) {
        fclose(fList);
	fList = NULL;
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////

