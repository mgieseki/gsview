/* Copyright (C) 1993-2011, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwinit.c */
/* Initialisation routines for Windows GSview */
#include "gvwin.h"

static void init_displays(void);

/* Open/Save File Dialog Box */
OPENFILENAME ofn;
TCHAR szOFilename[MAXSTR];	/* filename for OFN */
/* buttons */
WNDPROC lpfnButtonWndProc;	/* default button WndProc */
struct buttonlist {
   HWND hbutton;
   struct buttonlist *next;
};
struct buttonlist *buttonhead, *buttontail;
BOOL dde_exit = FALSE;		/* exit after sending DDE command */

BOOL parse_args(GSVIEW_ARGS *pargs);

/* convert gs version integer to string */
/* buf must be 6 chars or longer */
void gsver_string(int ver, char *buf)
{
    /* make sure length including null never exceeds 6 */
    if (ver >= 9999)
	strcpy(buf, "0.0");
    sprintf(buf, "%d.%02d", ver / 100, ver % 100);
}

/* convert gs version string to integer */
int gsver_int(char *buf)
{
    int ver;
    if (strlen(buf) == 4)
	ver = (buf[0]-'0')*100 + (buf[2]-'0')*10 + (buf[3]-'0');
    else if (strlen(buf) == 3)
	ver = (buf[0]-'0')*100 + (buf[2]-'0')*10;
    else
	ver = GS_REVISION;
    return ver;
}

void
drop_filename(HWND hwnd, char *str)
{
    /* Send the message to the main window */
    if (strlen(str) != 0) {
	/* open file specified on command line */
	HGLOBAL hglobal;
	LPSTR szFile;
	hglobal = GlobalAlloc(GHND | GMEM_SHARE, strlen(str)+1);
	if (hglobal) {
	    szFile = (LPSTR)GlobalLock(hglobal);
	    strcpy(szFile, str);
	    GlobalUnlock(hglobal);
	    PostMessage(hwnd, WM_COMMAND, IDM_DROP, (LPARAM)hglobal);
	}
    }
}

/* Don't start another instance - use previous instance */
/* This is never used in Win95/NT */
void
gsview_init0(LPSTR lpszCmdLine)
{
	HWND hwnd = FindWindow(szClassName, NULL);
	BringWindowToTop(hwnd);
#ifdef NOTUSED
#if __BORLANDC__ == 0x452
	/* avoid bug in BC++ 4.0 */
	/* skip over EXE name */
	while ( *lpszCmdLine && (*lpszCmdLine!=' ')) 
		lpszCmdLine++;
	while ( *lpszCmdLine && (*lpszCmdLine==' ')) 
		lpszCmdLine++;
#endif
#endif
	drop_filename(hwnd, lpszCmdLine);
}


char workdir[MAXSTR];

/* This code allows GSview to automatically find the language DLLs.
 * This allows languages to be added without having to alter the
 * source code of the GSview executable.
 */

/* At startup, we look for language DLLs in the GSview EXE directory
 * which have the following name.  The ?? is replaced by the two
 * letter Internet country code.
 */
#ifdef _WIN64
const TCHAR lang_pattern[] = TEXT("gsvw64??.dll");
#else
const TCHAR lang_pattern[] = TEXT("gsvw32??.dll");
#endif
const int lang_offset = 6;  /* offset to ?? */

/* We load each of these DLLs and if they contain a version
 * string that matches the GSview EXE, we consider them to
 * be a valid language DLL.  We store the details about each
 * in the following structure.  
 * The id is allocated automatically.
 * We refer to each language in the INI file using the twocc code, 
 * which matches the two letter Internet country code.
 * Each language DLL provides a bitmap of the language name,
 * to allow it to be displayed on systems without that language
 * installed.
 * The required ANSI codepage is in codepage.  If any code page is
 * acceptable (e.g. English) then this may be 0.
 */
typedef struct lang_s {
    int id;
    char twocc[3];
    TCHAR dllname[MAXSTR];
    HBITMAP bitmap;
    int codepage;
} lang_t;

/* The list of language DLLs available */
#define MAXLANG 19
lang_t lang[MAXLANG];
int nlang;

int
language_find(void)
{
    int i;
    HANDLE hff = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA wfd;
    TCHAR dirname[MAXSTR];
    TCHAR pattern[1024];
    int twocc_index;
    HINSTANCE hInstance;

    for (i=0; i<MAXLANG; i++) {
	lang[i].id = 0;
	lang[i].twocc[0] = '\0';
	lang[i].dllname[0] = '\0';
	lang[i].bitmap = NULL;
    }

    /* First language is always English */
    nlang = 0;
    lang[nlang].id = IDM_LANGMENU + nlang + 1;
    lang[nlang].twocc[0] = 'e';
    lang[nlang].twocc[1] = 'n';
    lang[nlang].twocc[2] = '\0';
    lang[nlang].dllname[0] = '\0';
    lang[nlang].bitmap = LoadBitmap(phInstance, MAKEINTRESOURCE(ID_LANG));
    nlang++;

    /* Now search for language DLLs */
    GetModuleFileName(phInstance, dirname, sizeof(dirname));
    for (i=lstrlen(dirname)-1; i>0; i--) {
	if (dirname[i] == '\\') {
	    dirname[i+1] = '\0';
	    break;
	}
    }
    lstrcpy(pattern, dirname);
    lstrcat(pattern, lang_pattern);
    twocc_index = lstrlen(pattern - 6);
    memset(&wfd, 0, sizeof(wfd));

    hff = FindFirstFile(pattern, &wfd);
    while (hff != INVALID_HANDLE_VALUE) {
        lang[nlang].id = IDM_LANGMENU + nlang + 1;
	lang[nlang].twocc[0] = wfd.cFileName[lang_offset];
	lang[nlang].twocc[1] = wfd.cFileName[lang_offset+1];
	lang[nlang].twocc[2] = '\0';
	if (lstrlen(dirname) + lstrlen(wfd.cFileName) < 
	    sizeof(lang[nlang].dllname)/sizeof(TCHAR)-1) {
	    lstrcpy(lang[nlang].dllname, dirname); 
	    lstrcat(lang[nlang].dllname, wfd.cFileName); 
 	}
	else
	    lang[nlang].id = 0;	/* can't do it */
	lang[nlang].bitmap = NULL;

	/* Try opening DLL */
        hInstance = LoadLibrary(lang[nlang].dllname);
        if (hInstance >= (HINSTANCE)HINSTANCE_ERROR) {
	    TCHAR vbuf[MAXSTR];
	    /* Make sure DLL version matches EXE */
	    memset(vbuf, 0, sizeof(vbuf));
	    LoadString(hInstance, IDS_GSVIEWVERSION, vbuf, sizeof(vbuf));
	    if (lstrcmp(TEXT(GSVIEW_DOT_VERSION), vbuf) != 0)
		lang[nlang].id = 0;
	    else  {
		/* Load bitmap of language name */
		lang[nlang].bitmap = LoadBitmap(hInstance, 
			MAKEINTRESOURCE(ID_LANG));
		/* Get required code page */
		LoadString(hInstance, IDS_CODEPAGE, vbuf, sizeof(vbuf));
#ifdef UNICODE
		lang[nlang].codepage = _wtoi(vbuf);
#else
		lang[nlang].codepage = atoi(vbuf);
#endif
	    }
	    FreeLibrary(hInstance);
	}

	if (lang[nlang].id != 0)
	    nlang++;	/* found a valid language */

	if (!FindNextFile(hff, &wfd)) {
	    FindClose(hff);
	    hff = INVALID_HANDLE_VALUE;
	}
    }

    return 0;
}

int language_free(void)
{
    int i;
    for (i=0; i<nlang; i++) {
	lang[i].id = 0;
	lang[i].twocc[0] = '\0';
	lang[i].dllname[0] = '\0';
	if (lang[i].bitmap)
	    DeleteObject((HGDIOBJ)lang[i].bitmap);
    }
    return 0;
}

int language_id(const char *str)
{
    int i;
    for (i=0; i<nlang; i++) {
	if (strcmp(str, lang[i].twocc) == 0)
	    return lang[i].id;
    }
    return 0;
}

int language_lookup(int id)
{
    int i;
    for (i=0; i<nlang; i++) {
	if (id == lang[i].id)
	    return i;
    }
    return 0;
}

const char * language_twocc(int id)
{
    int i;
    for (i=0; i<nlang; i++) {
	if (id == lang[i].id)
	    return lang[i].twocc;
    }
    return "en";
}

const TCHAR * language_dllname(int id)
{
    int i;
    for (i=0; i<nlang; i++) {
	if (id == lang[i].id)
	    return lang[i].dllname;
    }
    return TEXT("");
}


/* load language dependent resources */
/* returns TRUE if language change successful */
BOOL
load_language(int language)
{
TCHAR langdll[MAXSTR];
HINSTANCE hInstance;
    if (language == IDM_LANGEN) {
	if ((hlanguage != (HINSTANCE)NULL) && (hlanguage != phInstance))
	    FreeLibrary(hlanguage);
	/* Don't load a DLL */
	hlanguage = phInstance;
	return TRUE;
    }

    lstrcpy(langdll, language_dllname(language));
    hInstance = LoadLibrary(langdll);
    if (hInstance >= (HINSTANCE)HINSTANCE_ERROR) {
	if ((hlanguage != (HINSTANCE)NULL) && (hlanguage != phInstance))
	    FreeLibrary(hlanguage);
	hlanguage = hInstance;

	load_string(IDS_GSVIEWVERSION, langdll, sizeof(langdll));
	if (lstrcmp(TEXT(GSVIEW_DOT_VERSION), langdll) != 0)
	    message_box(TEXT("Language resources version doesn't match GSview EXE"), 0);

	return TRUE;
    }
    
    return FALSE;
}

void
change_language(void)
{
TCHAR *p;
HMENU hMenuOptions;
HMENU hMenuLang;
HMENU hMenuMedia;
int i;

    nCodePageSystem = GetACP();
    nCodePageLanguage = nCodePageSystem;

    nCodePageLanguage = lang[language_lookup(option.language)].codepage;
    if (nCodePageLanguage == 0)
	nCodePageLanguage = nCodePageSystem;

    if (nCodePageLanguage == 1253)
       info_font = hFontGreek;
    else if (nCodePageLanguage == 1250)
       info_font = hFontEastEurope;
    else if (nCodePageLanguage == 1251)
       info_font = hFontCyrillic;
    else
       info_font = hFontAnsi;

    hmenu = LoadMenu(hlanguage, TEXT("gsview_menu"));
    /* Add extra languages to menu */
#ifdef VIEWONLY
    hMenuOptions = GetSubMenu(hmenu, 1);
#else
    hMenuOptions = GetSubMenu(hmenu, 2);
#endif
    hMenuLang = GetSubMenu(hMenuOptions, 4);
    for (i=1; i<nlang; i++) {
	AppendMenu(hMenuLang, MF_BITMAP, lang[i].id, (LPTSTR)lang[i].bitmap);
	if (!is_winnt && (nCodePageSystem != lang[i].codepage))
	    enable_menu_item(IDM_LANGMENU, lang[i].id, FALSE);
    }

    /* User media */
#ifdef VIEWONLY
    hMenuMedia = GetSubMenu(hmenu, 4);
#else
    hMenuMedia = GetSubMenu(hmenu, 5);
#endif
    for (i=0; i<sizeof(usermedia)/sizeof(USERMEDIA)-1; i++) {
	if (usermedia[i].name[0])
	    AppendMenuA(hMenuMedia, MF_STRING, 
		usermedia[i].id, (LPSTR)usermedia[i].name);
    }

    SetMenu(hwndimg, hmenu);

    haccel = LoadAccelerators(hlanguage, TEXT("gsview_accel"));

#ifdef USE_HTMLHELP
    HtmlHelp(NULL, NULL, HH_CLOSE_ALL, (DWORD)NULL);
#else
    WinHelp(hwndimg,szHelpName,HELP_QUIT,(DWORD)NULL);
#endif
    /* get path to help file */
    lstrcpy(szHelpName, szExePath);
    p = szHelpName + lstrlen(szHelpName);
    load_string(IDS_HELPFILE, p, 
	sizeof(szHelpName)/sizeof(TCHAR) - (int)(p-szHelpName));
    if (debug)
	gs_addmessf("Help file is %s\n", szHelpName);

    nHelpTopic = IDS_TOPICROOT;
    init_check_menu();
    InvalidateRect(hwndimg, (LPRECT)NULL, FALSE);
    if (hwnd_measure)
	PostMessage(hwnd_measure, WM_COMMAND, IDCANCEL, 0L);
}


#ifdef __BORLANDC__
#pragma argsused
#endif
/* language dialog box */
DLGRETURN CALLBACK _export
LanguageDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    int i;
    switch(message) {
	case WM_INITDIALOG:
	    /* Display language names as bitmaps, to make them
	     * work even if we don't have the correct fonts.
	     */
	    for (i=0; i<nlang; i++) {
		CreateWindow(TEXT("button"), NULL,
		    BS_BITMAP | WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP,
		    20, 8 + i * 32, 80, 24,
		    hDlg, (HMENU)lang[i].id,
		    phInstance, NULL);
		SendDlgItemMessage(hDlg, lang[i].id, BM_SETIMAGE, IMAGE_BITMAP, 
		    (LPARAM)lang[i].bitmap);
		if (!is_winnt && lang[i].codepage &&
		    (nCodePageSystem != lang[i].codepage))
		    EnableWindow(GetDlgItem(hDlg, lang[i].id), FALSE);
	    }
	    SetWindowPos(hDlg, NULL, 0, 0, 
	       20+80+20 + 2 * GetSystemMetrics(SM_CXDLGFRAME), 
	       16 + nlang * 32 + 2 * GetSystemMetrics(SM_CYDLGFRAME) + 
		+ GetSystemMetrics(SM_CYCAPTION),
	       SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	    centre_dialog(hDlg);

	    return TRUE;
        case WM_COMMAND:
	    if ((LOWORD(wParam) >  IDM_LANGMENU)
		&& (LOWORD(wParam) <= IDM_LANGLAST)) {
		EndDialog(hDlg, LOWORD(wParam));
		return(TRUE);
	    }
            switch(LOWORD(wParam)) {
		case IDOK:
                case IDCANCEL:
                    EndDialog(hDlg, 0);
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
char buf[MAXSTR];
int count;
char *p;
int language;
    /* if Window language doesn't match GSview language */
    GetProfileStringA("Intl", "sLanguage", "ENG", winlang, sizeof(winlang));
    LoadStringA(hlanguage, IDS_SLANGUAGE, buf, sizeof(buf)-1);
    /* buf contains valid language code and length */
    p = strtok(buf, ",");
    p = strtok(NULL, ",");
    count = atoi(p);
    if (count > 3)
	count = 3;

    if (strnicmp(winlang, buf, count) != 0) {
	language = DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_LANG), 
	    hwndimg, LanguageDlgProc, (LPARAM)NULL);
	if (language) 
	    gsview_language(language);
    }
}

void
post_args(void)
{
    PostMessage(hwndimg, WM_COMMAND, IDM_ARGS, (LPARAM)&args); 
}

void
system_colours(void)
{
LOGBRUSH lb;
    if (hpen_btnshadow)
	DeleteBrush(hpen_btnshadow);
    hpen_btnshadow = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
    if (hpen_btnhighlight)
	DeleteBrush(hpen_btnhighlight);
    hpen_btnhighlight = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
    lb.lbStyle = BS_SOLID;
    lb.lbHatch = 0;
    lb.lbColor = GetSysColor(COLOR_WINDOW);
    if (lb.lbColor == RGB(255,255,255))	/* don't allow white background */
	lb.lbColor = GetSysColor(COLOR_MENU);
    if (lb.lbColor == RGB(255,255,255))	/* don't allow white background */
	lb.lbColor = GetSysColor(COLOR_APPWORKSPACE);
    if (lb.lbColor == RGB(255,255,255))	/* don't allow white background */
	lb.lbColor = RGB(192,192,192);
    if (hbrush_window)
	DeleteBrush(hbrush_window);
    hbrush_window = CreateBrushIndirect(&lb);
    lb.lbColor = GetSysColor(COLOR_MENU);
    if (hbrush_menu)
	DeleteBrush(hbrush_menu);
    hbrush_menu =  CreateBrushIndirect(&lb);

    /* change class background brush of main and image windows */
#ifdef _WIN64
    SetClassLongPtr(hwndimg, GCLP_HBRBACKGROUND, (LONG_PTR)hbrush_window);
    SetClassLongPtr(hwndimgchild, GCLP_HBRBACKGROUND, (LONG_PTR)hbrush_window);
#else
    SetClassLong(hwndimg, GCL_HBRBACKGROUND, (LONG)hbrush_window);
    SetClassLong(hwndimgchild, GCL_HBRBACKGROUND, (LONG)hbrush_window);
#endif
}

/* main initialisation */
BOOL
gsview_init1(int argc, char *argv[])
{
WNDCLASS wndclass;
DWORD version = GetVersion();
int length = 64;
int badarg;
int ndisp;
	getcwd(workdir, sizeof(workdir));

	while (length && !SetMessageQueue(length))
	    length--;	/* reduce size and try again */
	if (length == 0)
	    exit(0);	/* panic */
	
	/* figure out which version of Windows */
	/* Win32s: bit 15 HIWORD is 1 and bit 14 is 0 */
	/* Win95:  bit 15 HIWORD is 1 and bit 14 is 1 */
	/* WinNT:  bit 15 HIWORD is 0 and bit 14 is 0 */
	/* WinNT with Win95 shell recognised by WinNT + LOBYTE(LOWORD) >= 4 */
	/* check if Windows NT */
	if ((HIWORD(version) & 0x8000)==0)
	    is_winnt = TRUE;
	/* check if Windows 95 (Windows 4.0) */
	if ( ((HIWORD(version) & 0x8000)!=0) && ((HIWORD(version) & 0x4000)!=0) )
	    is_win95 = TRUE;
	/* Win32s */
	if ( ((HIWORD(version) & 0x8000)!=0) && ((HIWORD(version) & 0x4000)==0) )
	    is_win32s = TRUE;
	/* Windows 4.0 */
	if (LOBYTE(LOWORD(version)) >= 4)
	    is_win4 = TRUE;
	if (is_win95 && is_win4 && HIBYTE(LOWORD(version)) >= 10)
	    is_win98 = TRUE;

	if (is_win95 || is_winnt)
	    args.multithread = TRUE;

        init_displays();

	badarg = parse_argv(&args, argc, argv);
	parse_args(&args);

	if (args.exit_existing) {
	    dde_execute("[FileExit()]");
	    dde_exit = TRUE;
	}
	if (args.existing) {
	    int i;
	    LPSTR line;
	    int len = 0;
	    for (i=args.existing+1; i<argc; i++)
		len += strlen(argv[i]) + 3; /* "\042arg\042 " */
	    line = (LPSTR)malloc(len+1);
	    if (line == NULL)
		return FALSE;
	    line[0] = '\0';
	    for (i=args.existing+1; i<argc; i++) {
		strcat(line, "\042");
		strcat(line, argv[i]);
		strcat(line, "\042 ");
	    }
	    if (dde_execute_line(line))
		dde_exit = TRUE;
	    dde_execute("[ShowWindow(1)]");
	    free(line);
	    args.existing = FALSE;
	}

	if (dde_exit)
	    return FALSE;

	if (is_win32s) {
	    /* don't allow multiple copies under Win32s */
	    HWND hwnd = FindWindow(szClassName, NULL);
	    if (hwnd != (HWND)NULL) {
		gsview_init0(GetCommandLineA());
		return FALSE;
	    }
	}
	if (is_win95 || is_winnt) {
	    if (multithread) {
		display.event = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (!display.event) {
		    multithread = FALSE;
		    error_message("Failed to create display.event object");
		}
		if (multithread)
		    InitializeCriticalSection(&crit_sec);

		if (multithread)
		    hmutex_ps = CreateMutex(NULL, FALSE, NULL);
		if (hmutex_ps == NULL) {
		    error_message("Failed to create mutex");
		    multithread = FALSE;
		}

		if (multithread)
		    image.hmutex = CreateMutex(NULL, FALSE, NULL);
		if (image.hmutex == NULL) {
		    error_message("Failed to create mutex");
		    multithread = FALSE;
		}
	    }
	}

	/* get path to EXE */
	GetModuleFileName(phInstance, szExePath, sizeof(szExePath));
	{ int i;
	  for (i=lstrlen(szExePath)-1; i>0; i--)
	    if (szExePath[i] == '\\') {
		szExePath[i+1] = '\0';
		break;
	    }
	}

	/* Find available language DLLs */
        language_find();

	/* get path to INI file */
	szIniFile[0] = '\0';
	if (portable_app) {
	    /* Store INI file in same directory as executable */
	    strcpy(szIniFile, szExePath);
	}
	else if (is_win4) {
	    /* allow for user profiles */
	    LONG rc;
	    HKEY hkey;
	    DWORD keytype;
	    DWORD cbData;
	    DWORD fa;
	    /* Find the user profile directory */
	    rc = RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\ProfileReconciliation", 0, KEY_READ, &hkey);
	    if (rc == ERROR_SUCCESS) {
		cbData = sizeof(szIniFile)-sizeof(INIFILE);
		keytype =  REG_SZ;
		rc = RegQueryValueExA(hkey, "ProfileDirectory", 0, &keytype, (LPBYTE)szIniFile, &cbData);
		RegCloseKey(hkey);
	    }
	    if (rc == ERROR_SUCCESS) {
		fa = GetFileAttributesA(szIniFile);
		if ((fa != 0xffffffff) && (fa & FILE_ATTRIBUTE_DIRECTORY))
		    strcat(szIniFile, "\\");
		else
		    szIniFile[0] = '\0';
	    }
	    else {
		    /* If we didn't succeed, use the Windows directory */
		    szIniFile[0] = '\0';
	    }
	}
	if (szIniFile[0] == '\0') {
	    DWORD fa;
	    /* If we didn't succeed, try %USERPROFILE% */
	    char *p = getenv("USERPROFILE");
	    if (p && *p) {
		strcpy(szIniFile, p);
#ifdef __BORLANDC__
		OemToCharBuff(szIniFile, szIniFile, lstrlen(szIniFile));
#endif
		p = szIniFile + strlen(szIniFile) - 1;
		if ((*p == '\\') || (*p == '/'))
		    *p = '\0';
		/* check if USERPROFILE contains a directory name */
		fa = GetFileAttributesA(szIniFile);
		if ((fa != 0xffffffff) && (fa & FILE_ATTRIBUTE_DIRECTORY))
		    strcat(szIniFile, "\\");
		else
		    szIniFile[0] = '\0';
	    }
	}
	if (szIniFile[0] == '\0') {
	    TCHAR tbuf[MAXSTR];
	    GetWindowsDirectory(tbuf, 
		sizeof(tbuf)/sizeof(TCHAR)-2-sizeof(INIFILE));
	    convert_widechar(szIniFile, tbuf, sizeof(szIniFile));
	    strcat(szIniFile, "\\");
	}
	strcat(szIniFile, INIFILE);

	/* defaults if entry not in gsview.ini */
 	init_options();
	/* read entries from gsview.ini */
	read_profile(szIniFile);

 	use_args(&args);

	if (debug) {
	    gs_addmess("INI file is \042");
	    gs_addmess(szIniFile);
	    gs_addmess("\042\n");
	}

	if (!load_language(option.language)) {
	    message_box(TEXT("Couldn't load language specific resources.  Resetting to English."), 0);
	    option.language = IDM_LANGEN;
	    if (!load_language(option.language))
		message_box(TEXT("Couldn't load English resources.  Please reinstall GSview"), 0);
	}

	system_colours();

        view_init(&view);

	/* register the child image window class */
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndImgChildProc;
	wndclass.cbClsExtra = 0;
#ifdef _WIN64
	wndclass.cbWndExtra = sizeof(LONG_PTR);
#else
	wndclass.cbWndExtra = sizeof(LONG);
#endif
	wndclass.hInstance = phInstance;
	wndclass.hIcon = LoadIcon(phInstance,MAKEINTRESOURCE(ID_GSVIEW));
/*
	wndclass.hCursor = LoadCursor((HINSTANCE)NULL, IDC_CROSS);
*/
	wndclass.hCursor = hcCrossHair = LoadCursor(phInstance,MAKEINTRESOURCE(IDP_CROSSHAIR)); 
	wndclass.hbrBackground =  hbrush_window;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szImgClassName;
	RegisterClass(&wndclass);
	
	/* register the parent window class */
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndImgProc;
	wndclass.cbClsExtra = 0;
#ifdef _WIN64
	wndclass.cbWndExtra = sizeof(LONG_PTR);
#else
	wndclass.cbWndExtra = sizeof(LONG);
#endif
	wndclass.hInstance = phInstance;
	wndclass.hIcon = LoadIcon(phInstance,MAKEINTRESOURCE(ID_GSVIEW));
	wndclass.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);
	wndclass.hbrBackground =  hbrush_window;
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szClassName;
	RegisterClass(&wndclass);

	/* Make sure window origin is on a current display */
	ndisp = find_display(option.img_origin.x, option.img_origin.y);
	if (ndisp < 0) {
	    option.img_origin.x = win_display[0].left;
	    option.img_origin.y = win_display[0].top;
	    option.img_size.x = CW_USEDEFAULT;
	    option.img_size.y = CW_USEDEFAULT;
	}

	/* create parent window */
	hwndimg = CreateWindow(szClassName, szAppName,
		  WS_OVERLAPPEDWINDOW,
		  option.img_origin.x, option.img_origin.y, 
		  option.img_size.x, option.img_size.y, 
		  NULL, NULL, phInstance, (void FAR *)NULL);

	/* help message for GetOpenFileName Dialog Box */
	help_message = RegisterWindowMessage(HELPMSGSTRING);

	change_language();
        load_string(IDS_WAIT, szWait, sizeof(szWait));	/* generic wait message */

	/* load DLL for sounds */
	/* MMSYSTEM.DLL requires Windows 3.1, so to allow gsview to run
	   under Windows 3.0 we can't use the import library */
	hlib_mmsystem = LoadLibrary(TEXT("WINMM.DLL"));
	if (hlib_mmsystem != NULL) {
#ifdef UNICODE
	    lpfnSndPlaySound = (FPSPS)GetProcAddress(hlib_mmsystem, 
		"sndPlaySoundW");
#else
	    lpfnSndPlaySound = (FPSPS)GetProcAddress(hlib_mmsystem, 
		"sndPlaySoundA");
#endif
	}
	else {
	    gserror(IDS_SOUNDNOMM, NULL, MB_ICONEXCLAMATION, -1);
	    hlib_mmsystem = (HINSTANCE)NULL;
	}


	gsview_initc(argc, argv);

	if (badarg) {
	    gs_addmessf("Unknown argument %d: %s\n", badarg, argv[badarg]);
	    gserror(IDS_PARSEERROR, NULL, 0, SOUND_ERROR);
	}
	return TRUE;
}

/* Platform specific preprocessing of arguments */
BOOL
parse_args(GSVIEW_ARGS *args)
{
    char *filename = args->filename;
    debug = args->debug;
    multithread = args->multithread;
    portable_app = args->portable;
    if (is_win32s)
	multithread = FALSE;
    if (args->print || args->convert) {
	print_silent = TRUE;
	print_exit = TRUE;
    }
    if (filename[0]) {
	/* make sure filename contains full path */
	char fullname[MAXSTR+MAXSTR];
	if (filename[0]=='\\' || filename[0]=='/') {
	    if (filename[1] && (filename[1]=='\\' ||  filename[1]=='/')) {
		/* UNC name */
		/* do nothing */
	    }
	    else {
		/* referenced from root directory, so add drive */
		fullname[0] = workdir[0];
		fullname[1] = workdir[1];
		strncpy(fullname+2, filename, sizeof(fullname)-2);
		strncpy(args->filename, fullname, sizeof(args->filename)-1);
	    }
	}
	else if ( ! (isalpha(*filename) && filename[1]==':')) {
	    /* Doesn't include drive code, so add work dir */
	    int j;
	    strcpy(fullname, workdir);
	    j = strlen(workdir) - 1;
	    if ((j >= 0) && !((fullname[j] == '\\') || (fullname[j] == '/')))
		strcat(fullname, "\\");
	    strcat(fullname, filename);
	    strncpy(args->filename, fullname, sizeof(args->filename)-1);
	}
	else {
	    /* contains full path */
	    /* make this the work dir */
	    char *t;
	    char filedir[MAXSTR];
	    strcpy(filedir, filename);
	    if ( (t = strrchr(filedir, '\\')) != (char *)NULL ) {
		*(++t) = '\0';
#ifdef __BORLANDC__
		if (isalpha(filedir[0]) && (filedir[1]==':'))
		    (void) setdisk(toupper(filedir[0])-'A');
		if (!((strlen(filedir)==2) && isalpha(filedir[0]) && 
		    (filedir[1]==':')))
#endif
		    gs_chdir(filedir);
	    }
	}
    }


    return TRUE;
}


void
delete_buttons(void)
{
    struct buttonlist *bp = buttonhead;
    while (bp) {
	buttonhead = bp->next;
	free(bp);
	bp = buttonhead;
    }
}

POINT button_size, button_shift;

void
calc_info_button_areas(int width, int height)
{
    HDC hdc;
    TEXTMETRIC tm;
    LOGFONT lf;
    HFONT old_hfont;
    POINT char_size;		/* size of default text characters */

    /* get default text size */
    hdc = GetDC(hwndimg);
    memset(&lf, 0, sizeof(LOGFONT));
    lf.lfHeight = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    lstrcpy(lf.lfFaceName, TEXT("MS Shell Dlg"));
    lf.lfCharSet = ANSI_CHARSET;
    hFontAnsi = CreateFontIndirect(&lf);
    lf.lfCharSet = GREEK_CHARSET;
    hFontGreek = CreateFontIndirect(&lf);
    lf.lfCharSet = EASTEUROPE_CHARSET;
    hFontEastEurope = CreateFontIndirect(&lf);
    lf.lfCharSet = RUSSIAN_CHARSET;
    hFontCyrillic = CreateFontIndirect(&lf);
    old_hfont = (HFONT)SelectObject(hdc, hFontAnsi);
    GetTextMetrics(hdc,(LPTEXTMETRIC)&tm);
    display.planes = GetDeviceCaps(hdc, PLANES);
    display.bitcount = GetDeviceCaps(hdc, BITSPIXEL);
    SelectObject(hdc, old_hfont);
    ReleaseDC(hwndimg,hdc);
    char_size.x = tm.tmAveCharWidth;
    char_size.y = tm.tmHeight;

    if (nCodePageLanguage == 1253)
       info_font = hFontGreek;
    else if (nCodePageLanguage == 1250)
       info_font = hFontEastEurope;
    else if (nCodePageLanguage == 1251)
       info_font = hFontCyrillic;
    else
       info_font = hFontAnsi;

    button_size.x = 24;
    button_size.y = 24;
    info_rect.left = 0;
    info_rect.right = info_rect.left + 96 * char_size.x;
    info_rect.bottom = height;
    info_rect.top = info_rect.bottom - char_size.y - 4;
    // buttons at top
    button_shift.x = button_size.x - 1;
    button_shift.y = 0;
    button_rect.top = 0;
    button_rect.left = 0;
    button_rect.right = 0;	/* don't care */
    button_rect.bottom = button_size.y+1;	
    if (!option.button_show)
	button_rect.bottom = 0;

    img_offset.x = 0;
    img_offset.y = button_rect.bottom + 1;

    info_file.x = info_rect.left + 2;
    info_file.y = info_rect.top + 3;
    info_coord.left = info_rect.left + 42 * char_size.x;
    info_coord.right = info_rect.left + 62 * char_size.x;
    info_coord.top = info_rect.top + 3;
    info_coord.bottom = info_coord.top + char_size.y+2;
    info_page.x = info_rect.left + 64 * char_size.x + 2;
    info_page.y = info_rect.top + 3;
}

/* create gsview window menu bar, buttons and child window */
void
gsview_create()
{
int i;
WNDCLASS wndclass;
HGLOBAL hglobal;
short *pButtonID;
HWND hbutton;
WNDPROC	lpfnMenuButtonProc;
RECT rect;
int x, y;

	/* setup OPENFILENAME struct */
	ofn.lpstrFilter = (LPTSTR)NULL;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwndimg;
	ofn.nFilterIndex = FILTER_PS;
	ofn.lpstrFile = szOFilename;
	ofn.nMaxFile = sizeof(szOFilename);
	ofn.lpstrFileTitle = (LPTSTR)NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrTitle = (LPTSTR)NULL;
	ofn.lpstrInitialDir = (LPTSTR)NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_SHOWHELP;
	nHelpTopic = IDS_TOPICROOT;

	/* set size of info area, buttons and offset to child window */
	GetClientRect(hwndimg, &rect);
	calc_info_button_areas(rect.right-rect.left, rect.bottom-rect.top);

	hcWait = LoadCursor((HINSTANCE)NULL, IDC_WAIT);
	hcHand = LoadCursor(phInstance,MAKEINTRESOURCE(IDP_HAND)); 

	/* add buttons */
	lpfnMenuButtonProc = (WNDPROC)MakeProcInstance((FARPROC)MenuButtonProc, phInstance);
	/* get default button class info */
	GetClassInfo((HINSTANCE)NULL, TEXT("button"), &wndclass);
	lpfnButtonWndProc = wndclass.lpfnWndProc;
	
	hglobal = LoadResource(phInstance, FindResource(phInstance, MAKEINTRESOURCE(IDR_BUTTON), RT_RCDATA));
	if ( (pButtonID = (short *)LockResource(hglobal)) == (short *)NULL)
		return;
	
	x = button_rect.left;
        y = button_rect.top+1;

	for (i=0; pButtonID[i]; i++) {
	    if (pButtonID[i] < 100) {
		/* not a button, but a spacer */
		x += pButtonID[i];
		    
	    }
	    else {
		hbutton = CreateWindow(TEXT("button"), NULL,
		    WS_CHILD | BS_OWNERDRAW | 
		    (option.button_show ? WS_VISIBLE : 0),
		    x, y, button_size.x, button_size.y,
		    hwndimg, (HMENU)(int)pButtonID[i],
		    phInstance, NULL);
#ifdef _WIN64
		SetWindowLongPtr(hbutton, GWLP_WNDPROC, (LONG_PTR)lpfnMenuButtonProc);
#else
		SetWindowLong(hbutton, GWL_WNDPROC, (LONG)lpfnMenuButtonProc);
#endif
		if (hbutton) {
		    if (buttonhead == (struct buttonlist *)NULL)
			buttontail = buttonhead = (struct buttonlist *)
			    malloc(sizeof(struct buttonlist));
		    else {
			buttontail->next = (struct buttonlist *)
			    malloc(sizeof(struct buttonlist)); 
			buttontail = buttontail->next;
		    }
		    buttontail->hbutton = hbutton;
		    buttontail->next = NULL;
		}
		x += button_shift.x;
		y += button_shift.y;
	    }
	}
	FreeResource(hglobal);

	/* create child window */
	GetClientRect(hwndimg, &rect);
	hwnd_image = hwndimgchild = 
		  CreateWindow(szImgClassName, szAppName,
		  WS_CHILD /* | WS_VISIBLE */,
		  rect.left, rect.top,
		  rect.right-rect.left, rect.bottom-rect.top,
		  hwndimg, NULL, phInstance, (void FAR *)NULL);

}

void
show_buttons(void)
{
struct buttonlist *bp = buttonhead;
RECT rect;
	GetClientRect(hwndimg, &rect);
	calc_info_button_areas(rect.right - rect.left, rect.bottom - rect.top);
	SetWindowPos(hwndimgchild, (HWND)NULL, 
		rect.left+img_offset.x, rect.top+img_offset.y,
		rect.right-img_offset.x, info_rect.top-img_offset.y, 
		SWP_NOZORDER | SWP_NOACTIVATE);
	while (bp) {
	    ShowWindow(bp->hbutton, option.button_show ? SW_SHOWNA : SW_HIDE);
	    bp = bp->next;
	}
	InvalidateRect(hwndimg, &rect, TRUE);
	UpdateWindow(hwndimg);
}


char hkey_root[]="HKEY_CLASSES_ROOT";
char reg_win32s_error[]="ERROR: You can't set named values under Win32s\n";

void
reg_quote(char *d, char *s)
{
    while (*s) {
	if (*s == '\\')
	    *d++ = '\\';
	*d++ = *s++;
    }
    *d = *s;
}

/* Open the key. If it doesn't exist, create it */
BOOL
reg_open_key(FILE *newfile, FILE *oldfile, char *name, HKEY *hkey)
{
LONG lrc; 
    lrc = RegOpenKeyA(HKEY_CLASSES_ROOT, name, hkey);
    if (lrc == ERROR_SUCCESS) {
	if (oldfile) 
	    fprintf(oldfile, "\n[%s\\%s]\n", hkey_root, name);
    }
    else {
	lrc = RegCreateKeyA(HKEY_CLASSES_ROOT, name, hkey);
    }
    if (newfile)
	fprintf(newfile, "\n[%s\\%s]\n", hkey_root, name);
    if (lrc != ERROR_SUCCESS)
        *hkey = HKEY_CLASSES_ROOT;
    return (lrc == ERROR_SUCCESS);
}


BOOL
reg_set_value(FILE *newfile, FILE *oldfile, HKEY hkey, char *name, char *value)
{
DWORD keytype;
DWORD cbData;
char buf[MAXSTR];
char qbuf[MAXSTR];
LONG lenbuf;
    if (hkey == HKEY_CLASSES_ROOT)
	return FALSE;
    if (oldfile) {
        lenbuf = sizeof(buf);
	if (name==(char *)NULL) {
	    if (RegQueryValueA(hkey, (LPSTR)name, (LPSTR)buf, &lenbuf)
		== ERROR_SUCCESS) {
		if (strlen(buf)) {
		    reg_quote(qbuf, buf);
		    fprintf(oldfile, "@=\042%s\042\n", qbuf);
		}
	    }
	}
	else if (!is_win32s) {
	    cbData = sizeof(buf);
	    keytype =  REG_SZ;
	    if (RegQueryValueExA(hkey, name, 0, &keytype, 
		(LPBYTE)buf, &cbData) == ERROR_SUCCESS) {
	        reg_quote(qbuf, buf);
	        fprintf(oldfile, "\042%s\042=\042%s\042\n", name, qbuf);
	    }
	}
	else {
	    fprintf(oldfile, reg_win32s_error);
	    return FALSE;
	}
    }
    if (name==(char *)NULL) {
	reg_quote(qbuf, value);
	if (newfile)
	    fprintf(newfile, "@=\042%s\042\n", qbuf);
	if (RegSetValueA(hkey, NULL, REG_SZ, 
	    value, strlen(value)) != ERROR_SUCCESS)
	    return FALSE;
    }
    else if (!is_win32s) {
	reg_quote(qbuf, value);
	if (newfile)
	    fprintf(newfile, "\042%s\042=\042%s\042\n", name, qbuf);
	if (RegSetValueExA(hkey, name, 0, REG_SZ, 
	    (CONST BYTE *)value, strlen(value)+1) != ERROR_SUCCESS)
	    return FALSE;
    }
    else {
	if (newfile)
	    fprintf(newfile, reg_win32s_error);
	return FALSE;
    }
    return TRUE; 
}

void
reg_close_key(HKEY *hkey)
{
    RegCloseKey(*hkey);
    *hkey = HKEY_CLASSES_ROOT;
}

BOOL
create_registry_type(FILE *newfile, FILE *oldfile, 
	char *keyname, char *description)
{
HKEY hkey;
char buf[MAXSTR];
char kbuf[MAXSTR];
const char shellsubkey[]= "\\shell";
const char opensubkey[] = "\\open";
const char printsubkey[] = "\\print";
const char commandsubkey[] = "\\command";
BOOL flag = TRUE;

    if (flag)
	flag = reg_open_key(newfile, oldfile, keyname, &hkey);
    if (flag) {
	flag = reg_set_value(newfile, oldfile, hkey, NULL, description);
	reg_close_key(&hkey);
    }

    strcpy(kbuf, keyname);
    strcat(kbuf, shellsubkey);
    if (flag)
	flag = reg_open_key(newfile, oldfile, kbuf, &hkey);
    if (flag)
	reg_close_key(&hkey);

    strcat(kbuf, opensubkey);
    if (flag)
	flag = reg_open_key(newfile, oldfile, kbuf, &hkey);
    if (flag)
	reg_close_key(&hkey);
    strcat(kbuf, commandsubkey);
    if (flag)
	flag = reg_open_key(newfile, oldfile, kbuf, &hkey);
    if (!is_win32s)
        sprintf(buf, "\042%s%s\042 \042%%1\042", szExePath, GSVIEW_EXENAME);
    else
        sprintf(buf, "%s%s %%1", szExePath, GSVIEW_EXENAME);
    if (flag) {
	flag = reg_set_value(newfile, oldfile, hkey, NULL, buf);
	reg_close_key(&hkey);
    }

    strcpy(kbuf, keyname);
    strcat(kbuf, shellsubkey);
    strcat(kbuf, printsubkey);
    if (flag)
	flag = reg_open_key(newfile, oldfile, kbuf, &hkey);
    if (flag)
	reg_close_key(&hkey);
    strcat(kbuf, commandsubkey);
    if (flag)
	flag = reg_open_key(newfile, oldfile, kbuf, &hkey);
    if (!is_win32s)
        sprintf(buf, "\042%s%s\042 /p \042%%1\042", szExePath, GSVIEW_EXENAME);
    else
        sprintf(buf, "%s%s /p %%1", szExePath, GSVIEW_EXENAME);
    if (flag) {
	flag = reg_set_value(newfile, oldfile, hkey, NULL, buf);
	reg_close_key(&hkey);
    }

    if (is_win4) {
	strcpy(kbuf, keyname);
	strcat(kbuf, "\\DefaultIcon");
	if (flag)
	    flag = reg_open_key(newfile, oldfile, kbuf, &hkey);
	sprintf(buf, "%s%s,3", szExePath, GSVIEW_EXENAME);
	if (flag) {
	    flag = reg_set_value(newfile, oldfile, hkey, NULL, buf);
	    reg_close_key(&hkey);
	}
    }

    return flag;
}


int
update_registry(BOOL ps, BOOL pdf)
{
char *psmime="application/postscript";
char *pdfmime="application/pdf";
char *contentname="Content Type";
char *extension="Extension";
char buf[MAXSTR];
HKEY hkey;
char *pskey="psfile";
char *pdfkey="pdffile";
char *psext=".ps";
char *epsext=".eps";
char *pdfext=".pdf";
char *p;
FILE *oldfile, *newfile;
BOOL flag = TRUE;
const char regheader[]="REGEDIT4\n";

    if (portable_app)
	return 0;	/* never write to registry when portable */

    if (!ps && !pdf)
	return 0;

    convert_widechar(buf, szExePath, sizeof(buf));
    strcat(buf, GSVIEW_ZIP);
    p = strrchr(buf, '.');
    /* Write the old registry file, but only if it doesn't exist */
    strcpy(p, "old.reg");
    oldfile = fopen(buf, "r");
    if (oldfile == (FILE *)NULL) {
        oldfile = fopen(buf, "w");
	/* If we failed to open the file, the destination is probably 
	 * read only.  Don't worry, just don't write to the log file.
	 */
    }
    else {
	fclose(oldfile);
	oldfile = (FILE *)NULL;
    }

    /* Write the new registry file */
    strcpy(p, "new.reg");
    newfile = fopen(buf, "w");

    if (oldfile != (FILE *)NULL)
	fprintf(oldfile, regheader);
    if (newfile != (FILE *)NULL)
	fprintf(newfile, regheader);

    if (ps) {
	if (flag)
	    flag = reg_open_key(newfile, oldfile, psext, &hkey);
	if (flag) {
	    flag = reg_set_value(newfile, oldfile, hkey, NULL, pskey);
	    if (flag && !is_win32s)
		reg_set_value(newfile, oldfile, hkey, contentname, psmime);
	    reg_close_key(&hkey);
	}

	if (flag)
	    flag = reg_open_key(newfile, oldfile, epsext, &hkey);
	if (flag) {
	    flag = reg_set_value(newfile, oldfile, hkey, NULL, pskey);
	    if (flag && !is_win32s)
		flag = reg_set_value(newfile, oldfile, hkey, 
		    contentname, psmime);
	    reg_close_key(&hkey);
	}


	/* Don't bother with undelete information for these */
	if (!is_win32s) {
	    sprintf(buf, "MIME\\Database\\%s\\%s", contentname, psmime);
	    if (flag)
		flag = reg_open_key(newfile, oldfile, buf, &hkey);
	    if (flag) {
		flag = reg_set_value(newfile, oldfile, hkey, extension, psext);
		reg_close_key(&hkey);
	    }
	}
	if (flag) 
	  flag = create_registry_type(newfile, oldfile, pskey, "PostScript");
    }

    if (pdf) {
	if (flag)
	    flag = reg_open_key(newfile, oldfile, pdfext, &hkey);
	if (flag) {
	    flag = reg_set_value(newfile, oldfile, hkey, NULL, pdfkey);
	    if (flag && !is_win32s)
		reg_set_value(newfile, oldfile, hkey, contentname, pdfmime);
	    reg_close_key(&hkey);
	}

	/* Don't bother with undelete information for these */
	if (!is_win32s) {
	    sprintf(buf, "MIME\\Database\\%s\\%s", contentname, pdfmime);
	    if (flag)
		flag = reg_open_key(newfile, oldfile, buf, &hkey);
	    if (flag) {
		flag = reg_set_value(newfile, oldfile, hkey, extension, pdfext);
		reg_close_key(&hkey);
	    }
	}

	if (flag)
	    flag = create_registry_type(newfile, oldfile, pdfkey, "Portable Document Format");
    }

    if (oldfile)
	fclose(oldfile);

    if (newfile)
	fclose(newfile);

    return !flag;
}


int
gsview_create_objects(char *groupname)
{
char gspath[MAXSTR];
char *p;
int rc;
char exepath[MAXSTR];
    convert_widechar(exepath, szExePath, sizeof(exepath)-1);
    strcpy(gspath, option.gsdll);
    if ((p = strrchr(gspath,'\\')) != (char *)NULL)
	p++;
    else
	p = gspath;
    *p = '\0';

    rc = gsview_progman(groupname, exepath,
		option.gsversion, gspath, option.gsinclude);
    if (rc)
	gserror(IDS_NOPROGMAN, NULL, 0, SOUND_ERROR);
    return rc;
}

/***************************/

HINSTANCE zlib_hinstance;
PFN_gzopen gzopen;
PFN_gzread gzread;
PFN_gzclose gzclose;

void
unload_zlib(void)
{
    if (zlib_hinstance == (HINSTANCE)NULL)
	return;
    FreeLibrary(zlib_hinstance);
    zlib_hinstance = NULL;
    gzopen = NULL;
    gzread = NULL;
    gzclose = NULL;
}

/* load zlib DLL for gunzip */
BOOL
load_zlib(void)
{   
    char buf[MAXSTR];
    char exepath[MAXSTR];
#ifdef _WIN64
    char zlibname[] = "zlib64.dll";
#else
    char zlibname[] = "zlib32.dll";
#endif

    convert_widechar(exepath, szExePath, sizeof(exepath)-1);
    if (zlib_hinstance != (HINSTANCE)NULL)
	return TRUE;	/* already loaded */

    /* first look in GSview directory */
    strcpy(buf, exepath);
    strcat(buf, zlibname);
    gs_addmess("Attempting to load ");
    gs_addmess(buf);
    gs_addmess("\n");
    zlib_hinstance = LoadLibraryA(buf);
    if (zlib_hinstance < (HINSTANCE)HINSTANCE_ERROR) {
	/* if that fails, use the system search path */
	strcpy(buf, zlibname);
	gs_addmess("Attempting to load ");
	gs_addmess(buf);
	gs_addmess("\n");
	zlib_hinstance = LoadLibraryA(buf);
    }
    if (zlib_hinstance >= (HINSTANCE)HINSTANCE_ERROR) {
        gzopen = (PFN_gzopen) GetProcAddress(zlib_hinstance, "gzopen");
	if (gzopen == NULL) {
	    unload_zlib();
	}
	else {
	    gzread = (PFN_gzread) GetProcAddress(zlib_hinstance, "gzread");
	    if (gzread == NULL) {
		unload_zlib();
	    }
	    else {
		gzclose = (PFN_gzclose) GetProcAddress(zlib_hinstance, "gzclose");
		if (gzclose == NULL) {
		    unload_zlib();
		}
	    }
	}
    }
    else
	zlib_hinstance = NULL;

    if (zlib_hinstance == NULL) {
	TCHAR wbuf[MAXSTR];
	load_string(IDS_ZLIB_FAIL, wbuf, sizeof(wbuf)/sizeof(TCHAR)-1);
	if (message_box(wbuf, MB_OKCANCEL) == IDOK) {
	    nHelpTopic = IDS_TOPICZLIB;
	    get_help();
	}
	return FALSE;
    }
    
    return TRUE;
}

/***************************/

HINSTANCE bzip2_hinstance;
PFN_bzopen bzopen;
PFN_bzread bzread;
PFN_bzclose bzclose;

void
unload_bzip2(void)
{
    if (bzip2_hinstance == (HINSTANCE)NULL)
	return;
    FreeLibrary(bzip2_hinstance);
    bzip2_hinstance = NULL;
    bzopen = NULL;
    bzread = NULL;
    bzclose = NULL;
}

/* load bzip2 DLL for gunzip */
BOOL
load_bzip2(void)
{   
    char buf[MAXSTR];
    char exepath[MAXSTR];
    char bzip2name[] = "libbz2.dll";

    convert_widechar(exepath, szExePath, sizeof(exepath)-1);
    if (bzip2_hinstance != (HINSTANCE)NULL)
	return TRUE;	/* already loaded */

    /* first look in GSview directory */
    strcpy(buf, exepath);
    strcat(buf, bzip2name);
    gs_addmess("Attempting to load ");
    gs_addmess(buf);
    gs_addmess("\n");
    bzip2_hinstance = LoadLibraryA(buf);
    if (bzip2_hinstance < (HINSTANCE)HINSTANCE_ERROR) {
	/* if that fails, use the system search path */
	strcpy(buf, bzip2name);
	bzip2_hinstance = LoadLibraryA(buf);
	gs_addmess("Attempting to load ");
	gs_addmess(buf);
	gs_addmess("\n");
    }
    if (bzip2_hinstance >= (HINSTANCE)HINSTANCE_ERROR) {
        bzopen = (PFN_bzopen) GetProcAddress(bzip2_hinstance, "BZ2_bzopen");
	if (bzopen == NULL) {
	    unload_bzip2();
	}
	else {
	    bzread = (PFN_bzread) GetProcAddress(bzip2_hinstance, "BZ2_bzread");
	    if (bzread == NULL) {
		unload_bzip2();
	    }
	    else {
		bzclose = (PFN_bzclose) GetProcAddress(bzip2_hinstance, "BZ2_bzclose");
		if (bzclose == NULL) {
		    unload_bzip2();
		}
	    }
	}
    }
    else
	bzip2_hinstance = NULL;

    if (bzip2_hinstance == NULL) {
	TCHAR wbuf[MAXSTR];
	load_string(IDS_BZIP2_FAIL, wbuf, sizeof(wbuf)/sizeof(TCHAR)-1);
	if (message_box(wbuf, MB_OKCANCEL) == IDOK) {
	    nHelpTopic = IDS_TOPICBZIP2;
	    get_help();
	}
	return FALSE;
    }
    
    return TRUE;
}


/****************************************************/
/* Easy Configure */

#ifdef __BORLANDC__
#pragma argsused
#endif
/* easy configure dialog box */
DLGRETURN CALLBACK _export
EasyConfigureDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    WORD notify_message;
    switch(message) {
	case WM_INITDIALOG:
	    {
		int *gsver = (int *)lParam;
		int i;
		int n=0;
		char buf[16];
	        for (i=1; i<=gsver[0]; i++) {
		    if ((gsver[i] >= GS_REVISION_MIN) &&
		        (gsver[i] <= GS_REVISION_MAX)) {
			n++;
			gsver_string(gsver[i], buf);
			/* put string in list box */
			SendDlgItemMessageA(hDlg, IDC_GSVER, LB_ADDSTRING, 
			    0, (LPARAM)((LPSTR)buf));
		    }
		}
		SendDlgItemMessage(hDlg, IDC_GSVER, LB_SETCURSEL, 
		    n-1, 0L);
	    }
	    return TRUE;
        case WM_COMMAND:
	    notify_message = GetNotification(wParam,lParam);
            switch(LOWORD(wParam)) {
		case IDOK:
		    {
		    char buf[16];
		    int i = (int)SendDlgItemMessage(hDlg, IDC_GSVER, 
			LB_GETCURSEL, 0, 0L);
		    SendDlgItemMessage(hDlg, IDC_GSVER, LB_GETTEXT, 
			i, (LPARAM)(LPSTR)buf);
                    EndDialog(hDlg, gsver_int(buf));
		    }
                    return(TRUE);
                case IDCANCEL:
                    EndDialog(hDlg, 0);
                    return(TRUE);
		case ID_HELP:
		    get_help();
		    return(FALSE);
		case IDC_GSVER:
		    if (notify_message == LBN_DBLCLK)
			PostMessage(hDlg, WM_COMMAND, IDOK, 0);
		    return(FALSE);
                default:
                    return(FALSE);
            }
    }
    return(FALSE);
}

int
config_easy(BOOL bVerbose)
{
#ifndef __WIN32__
#error Win16 is no longer supported
#endif
	int result;
	int *gsver;
	int gs_count = 0;
	get_gs_versions(&gs_count);
	if (gs_count == 0)
	   return 1;
	gsver = (int *)malloc(sizeof(int) * (gs_count + 1));
	if (gsver == (int *)NULL)
	    return 1;
	gsver[0] = gs_count+1;
	nHelpTopic = IDS_TOPICEASYCFG;
	if (get_gs_versions(gsver)) {
	    if (!bVerbose && (gsver[0] == 1)) {
		/* Only one copy of Ghostscript installed */
		/* Don't prompt user */
		result = gsver[1];
	    }
	    else {
		/* Multiple copies of Ghostscript installed */
		/* Ask user to choose one */
		result = DialogBoxParamL(hlanguage, 
			MAKEINTRESOURCE(IDD_EASYCFG), hwndimg, 
			EasyConfigureDlgProc, (LPARAM)gsver);
	    }
	}
	free(gsver);

	if (result == 0)
	    return 0;	/* don't configure and don't warn */

	option.gsversion= result;
	get_gs_string(option.gsversion, "GS_DLL", option.gsdll, 
	    sizeof(option.gsdll));
	get_gs_string(option.gsversion, "GS_LIB", option.gsinclude, 
	    sizeof(option.gsinclude));
	strcpy(option.gsother, "-dNOPLATFONTS -sFONTPATH=\042c:\\psfonts\042");

	/* copy printer.ini */
	gsview_printer_profiles();

	option.configured = TRUE;

	write_profile();

	if (gsdll.open)
	    pending.unload = TRUE;

	return 0; /* success */
}



/***************************/
/* configure dialog wizard */

HWND hWiz = HWND_DESKTOP;
int gsver = GS_REVISION;

int wiz_exit(HWND hwnd);
int check_gsver(HWND hwnd);
int config_finish(HWND hwnd);
DLGRETURN CALLBACK _export CfgChildDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
DLGRETURN CALLBACK _export CfgMainDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

/* hDlgModeless */
typedef struct tagWIZPAGE {
   int id;		/* resource ID */
   int prev;		/* resource id of previous page */
   int next;		/* resource id of next page */
   int (*func)(HWND);	/* function to run on exit from page */
   HWND hwnd;		/* window handle of dialog */
} WIZPAGE;

WIZPAGE pages[]={
	{IDD_CFG1, IDD_CFG1, IDD_CFG2, NULL, 0},
	{IDD_CFG2, IDD_CFG1, IDD_CFG3, check_gsver, 0},
	{IDD_CFG3, IDD_CFG2, IDD_CFG4, NULL, 0},
	{IDD_CFG4, IDD_CFG3, IDD_CFG5, NULL, 0},
	{IDD_CFG5, IDD_CFG4, IDD_CFG6, NULL, 0},
	{IDD_CFG6, IDD_CFG5, IDD_CFG7, config_finish, 0},
	{IDD_CFG7, IDD_CFG7, IDD_CFG7, wiz_exit, 0},
	{0, 0, 0, NULL, 0}
};

#ifdef __BORLANDC__
#pragma argsused
#endif
int wiz_exit(HWND hwnd)
{
    PostMessage(hWiz, WM_COMMAND, (WPARAM)IDOK, (LPARAM)0);
    return 0;
}

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
goto_page(HWND hwnd, int id)
{
WIZPAGE *page;
HWND hbutton;
    page = find_page_from_id(id);
    if (page) {
	ShowWindow(hwnd, SW_HIDE);
	ShowWindow(page->hwnd, SW_SHOW);
	hDlgModeless = page->hwnd;
	if (IsWindowEnabled(GetDlgItem(page->hwnd, IDNEXT)) )
	    hbutton = GetDlgItem(page->hwnd, IDNEXT);
	else
	    hbutton = GetDlgItem(page->hwnd, IDCANCEL);
	SetFocus(hbutton);
	SendMessage(hbutton, BM_SETSTYLE, (WPARAM)BS_DEFPUSHBUTTON, TRUE);
	return;
    }
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
add_gsver(HWND hwnd, int offset)
{
char buf[MAXSTR];
int ver;
    GetDlgItemTextA(hwnd, IDC_CFG20, buf, sizeof(buf));
    if (strlen(buf) == 4)
	ver = (buf[0]-'0')*100 + (buf[2]-'0')*10 + (buf[3]-'0');
    else if (strlen(buf) == 3)
	ver = (buf[0]-'0')*100 + (buf[2]-'0')*10;
    else
	return GS_REVISION;
    ver += offset;
    if (ver > GS_REVISION_MAX)
       ver = GS_REVISION_MAX;
    if (ver < GS_REVISION_MIN)
       ver = GS_REVISION_MIN;
    return ver;
}

int
check_gsver(HWND hwnd)
{
char buf[MAXSTR];
int ver = GS_REVISION;
BOOL fixit = FALSE;
    /* should allow edit field to be changed  */
    /* then make sure it is within range */
    GetDlgItemTextA(hwnd, IDC_CFG20, buf, sizeof(buf));
    if (strlen(buf) == 4) {
	ver = (buf[0]-'0')*100 + (buf[2]-'0')*10 + (buf[3]-'0');
	if ( (ver > GS_REVISION_MAX) || (ver < GS_REVISION_MIN) )
	    fixit = TRUE;
    }
    else
	fixit = TRUE;
    if (fixit) {
	ver = GS_REVISION;
	sprintf(buf, "%d.%02d", ver / 100, ver % 100);
	SetDlgItemTextA(hwnd, IDC_CFG20, buf);
	/* don't move until it is valid */
	return IDD_CFG2;
    }
    
    gsver = ver;
    return 0;
}

/* update GS directory edit field when version number changes */
void
gsdir_fix(HWND hwnd, char *verstr)
{
char buf[MAXSTR];
char *p;
    GetDlgItemTextA(hwnd, IDC_CFG22, buf, sizeof(buf));
    if (strlen(buf) < 6)
	return;
    p = buf + strlen(buf) - 4;
    if (isdigit((int)(p[0])) && (p[1]=='.') && 
	isdigit((int)(p[2])) && isdigit((int)(p[3]))) {
	strcpy(p, verstr);
        SetDlgItemTextA(hwnd, IDC_CFG22, buf);
    }
    else {
	p = buf + strlen(buf) - 3;
	if (isdigit((int)(p[0])) && (p[1]=='.') && isdigit((int)(p[2]))) {
	    strcpy(p, verstr);
	    SetDlgItemTextA(hwnd, IDC_CFG22, buf);
	}
    }
}

int
config_now(void)
{
BOOL assoc_ps;
BOOL assoc_pdf;
char buf[MAXSTR];
TCHAR wbuf[MAXSTR];
WIZPAGE *page;
FILE *f;
char *p;

    /* get info from wizard */
    page = find_page_from_id(IDD_CFG2);
    option.gsversion = add_gsver(page->hwnd, 0);
    GetDlgItemTextA(page->hwnd, IDC_CFG22, buf, sizeof(buf));
    if (option.gsversion >= 593) {
        sprintf(option.gsdll, "%s\\bin\\%s", buf, GS_DLLNAME);
    }
    else {
        sprintf(option.gsdll, "%s\\%s", buf, GS_DLLNAME);
    }
    default_gsinclude_from_path(option.gsinclude, buf);
    strcpy(option.gsother, "-dNOPLATFONTS ");
    GetDlgItemTextA(page->hwnd, IDC_CFG23, buf, sizeof(buf));
    if (strlen(buf)) {
	strcat(option.gsother, "-sFONTPATH=\042");
	strcat(option.gsother, buf);
	strcat(option.gsother, "\042");
    }

    /* check if Ghostscript really has been installed */
    /* first look for the DLL */
    if ( (f = fopen(option.gsdll, "rb")) == (FILE *)NULL ) {
	load_string(IDS_GSNOTINSTALLED, wbuf, sizeof(wbuf)/sizeof(TCHAR)-1);
	SetDlgItemText(find_page_from_id(IDD_CFG7)->hwnd, IDC_CFG71,
	    wbuf);
	return 1;
    }
    fclose(f);

    /* next look for gs_init.ps */
    strcpy(buf, option.gsinclude);
    p = strchr(buf, ';');	/* remove trailing paths */
    if (p)
	*p = '\0';
    strcat(buf, "\\gs_init.ps");
    if ( (f = fopen(buf, "rb")) == (FILE *)NULL ) {
	load_string(IDS_GSLIBNOTINSTALLED, wbuf, sizeof(wbuf));
	SetDlgItemText(find_page_from_id(IDD_CFG7)->hwnd, IDC_CFG71,
	    wbuf);
	return 1;
    }
    fclose(f);
    /* at this stage we don't look for fonts, but maybe we should */

    assoc_ps = (BOOL)SendDlgItemMessage(find_page_from_id(IDD_CFG4)->hwnd, 
	    IDC_CFG41, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
    assoc_pdf = (BOOL)SendDlgItemMessage(find_page_from_id(IDD_CFG4)->hwnd, 
	    IDC_CFG42, BM_GETCHECK, (WPARAM)0, (LPARAM)0);
    if (update_registry(assoc_ps, assoc_pdf)) {
	return 1;
    }

    GetDlgItemTextA(find_page_from_id(IDD_CFG5)->hwnd, 
	IDC_CFG52, buf, sizeof(buf));
    if (SendDlgItemMessage(find_page_from_id(IDD_CFG5)->hwnd, 
	    IDC_CFG51, BM_GETCHECK, (WPARAM)0, (LPARAM)0)
	&& gsview_create_objects(buf)) {
	return 1;
    }
    
    if (SendDlgItemMessage(find_page_from_id(IDD_CFG3)->hwnd, 
	    IDC_CFG32, BM_GETCHECK, (WPARAM)0, (LPARAM)0))
	gsview_printer_profiles();

    option.configured = TRUE;

    write_profile();

    return 0;
}


int
config_finish(HWND hwnd)
{
    EnableWindow(GetDlgItem(hwnd, IDNEXT), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDPREV), FALSE);
    EnableWindow(GetDlgItem(hwnd, IDCANCEL), FALSE);
    if (config_now())
    {	TCHAR buf[MAXSTR];
	load_string(IDS_CFG73, buf, sizeof(buf));
	SetDlgItemText(find_page_from_id(IDD_CFG7)->hwnd, IDC_CFG70, buf);
    }
    return 0;
}


#ifdef __BORLANDC__
#pragma argsused
#endif
/* Download GS dialog box */
DLGRETURN CALLBACK _export
DownloadGSDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case IDOK:
                    EndDialog(hDlg, TRUE);
                    return(TRUE);
                case IDCANCEL:
                    EndDialog(hDlg, FALSE);
                    return(TRUE);
		case ID_HELP:
		    get_help();
		    return(FALSE);
                default:
                    return(FALSE);
            }
    }
    return(FALSE);
}


int
config_wizard(BOOL bVerbose)
{
    /* We don't use a configure wizard anymore - this is done in
     * the setup program.
     * Instead we have several options:
     * 1. GS is installed on hard disk - offer the easy configure
     *    which relies on the setup program having written entries
     *    to the registry.
     * 2. If GS not installed, or 1. fails, look for 
     *      ..\gsN.NN\bin\gsdll32.dll 
     *    If this exists, configure silently since we are either
     *    running from CD-ROM or network drive.
     * 3. Tell user to download GS
     */   
    int gscount;
    char basedir[MAXSTR];
    char gsdir[MAXSTR];
    char gsdll[MAXSTR];
    int gsver;
    char *p;
    FILE *f;

    if (!portable_app) {
	/* 1. If GS installed, easy configure */
	gscount = 0;
	get_gs_versions(&gscount);
	if (gscount > 0) {
	    if (config_easy(bVerbose) == 0)
		return 0;	/* success */
	}
    }

    /* 2. GS not installed.  Look for GS in adajacent directory */
    convert_widechar(basedir, szExePath, sizeof(basedir));
    p = strrchr(basedir, '\\');	/* remove trailing backslash */
    if (p)
	*p = '\0';
    p = strrchr(basedir, '\\');	/* remove trailing gsview */
    if (p)
	*(++p) = '\0';
    strcpy(gsdir, basedir);

    p = gsdir + strlen(gsdir);


    if (portable_app)
        gs_addmess("GSview Portable\n");
    else
        gs_addmess("Ghostscript registry entries not present.\n");

    gsver = GS_REVISION;
    while (gsver <= GS_REVISION_MAX) {
	sprintf(p, "gs%d.%02d", gsver / 100, gsver % 100);

	strcpy(gsdll, gsdir);
#ifdef _WIN64
	if (gsver <= 900)
	    strcat(gsdll, "\\bin\\gsdll32.dll");
	else
	    strcat(gsdll, "\\bin\\gsdll64.dll");
#else
	strcat(gsdll, "\\bin\\gsdll32.dll");
#endif

	if ( (f = fopen(gsdll, "rb")) != (FILE *)NULL ) {
	    /* GS DLL exists. Configure GSview */
	    fclose(f);
	    gs_addmess("Found ");
	    gs_addmess(gsdll);
	    gs_addmess("\n");
	    option.gsversion = gsver;
	    strcpy(option.gsdll, gsdll);
	    sprintf(option.gsinclude, "%s\\lib;%sfonts", gsdir, basedir);
	    strcpy(option.gsother, "-dNOPLATFONTS -sFONTPATH=\042c:\\psfonts\042");
	    gsview_printer_profiles();
	    option.configured = TRUE;
	    write_profile();
	    return 0;	/* success */
	}
	gsver++;
    }

    
    nHelpTopic = IDS_TOPICDOWNLOAD;
    if (DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_DOWNLOAD), hwndimg, 
		DownloadGSDlgProc, (LPARAM)0)) {
	/* download now */
	ShellExecute(hwndimg, NULL, 
	    TEXT("http://www.cs.wisc.edu/~ghost/index.htm"),
	    NULL, NULL, SW_SHOWNORMAL);
    }
    
    return 1;	/* failed */
}



#ifdef __BORLANDC__
#pragma argsused	/* ignore warning for next function */
#endif
/* Modeless Dialog Box */
DLGRETURN CALLBACK _export
CfgMainDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
	    /* create child dialog windows */
	    {
		WIZPAGE *page;
		TCHAR buf[MAXSTR];
		TCHAR gsdir[MAXSTR];
		int i;
		for (page=pages; page->id; page++) {
		    page->hwnd = CreateDialogParamL(hlanguage, 
			MAKEINTRESOURCE(page->id), hwnd, CfgChildDlgProc, 
			(LPARAM)NULL);
		    ShowWindow(page->hwnd, SW_HIDE);
		}
		ShowWindow(pages[0].hwnd, SW_SHOW);
		SetFocus(GetDlgItem(pages[0].hwnd, IDNEXT));
		SendDlgItemMessage(pages[0].hwnd, IDNEXT, BM_SETSTYLE, 
		    (WPARAM)BS_DEFPUSHBUTTON, TRUE);
		hDlgModeless = pages[0].hwnd;

		/* initialize GS version */
		page = find_page_from_id(IDD_CFG2);
		if (page) {
		    wsprintf(buf, TEXT("%d.%02d"), option.gsversion / 100, 
			option.gsversion % 100);
		    SetDlgItemText(page->hwnd, IDC_CFG20, buf);
		    SetDlgItemText(page->hwnd, IDC_CFG22, szExePath);
		    SetDlgItemText(page->hwnd, IDC_CFG23, TEXT("c:\\psfonts"));
		}

		/* assume that GS is in the adjacent directory */
		wsprintf(buf, TEXT("%d.%02d"), option.gsversion / 100, 
			option.gsversion % 100);
		lstrcpy(gsdir, szExePath);
		/* remove trailing \ */
		for (i=lstrlen(gsdir); i>0; i++)
		    if (gsdir[i] == '\\') {
			gsdir[i] = '\0';
			break;
		    }	
		/* remove trailing gsview */
		for (i=lstrlen(gsdir); i>0; i++)
		    if (gsdir[i] == '\\') {
			gsdir[i+1] = '\0';
			break;
		    }	
		lstrcat(gsdir, TEXT("gs"));
		lstrcat(gsdir, buf);
		SetDlgItemText(page->hwnd, IDC_CFG22, gsdir);
		SetDlgItemText(page->hwnd, IDC_CFG23, TEXT("c:\\psfonts"));

		SendDlgItemMessage(find_page_from_id(IDD_CFG3)->hwnd, IDC_CFG32, BM_SETCHECK, 
			    (WPARAM)1, (LPARAM)0);
		SendDlgItemMessage(find_page_from_id(IDD_CFG4)->hwnd, IDC_CFG41, BM_SETCHECK, 
			    (WPARAM)1, (LPARAM)0);
		SendDlgItemMessage(find_page_from_id(IDD_CFG4)->hwnd, IDC_CFG42, BM_SETCHECK, 
			    (WPARAM)0, (LPARAM)0);   /* PDF is NOT the default */

		/* program group */
		load_string(IDS_PROGMANGROUP4, buf, sizeof(buf));
		page = find_page_from_id(IDD_CFG5);
		if (page) {
		    SendDlgItemMessage(page->hwnd, IDC_CFG51, BM_SETCHECK, 
				(WPARAM)1, (LPARAM)0);
		    SetDlgItemText(page->hwnd, IDC_CFG52, buf);
		}

	    }
            return FALSE;	/* we decide the focus */
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDCANCEL:
                case IDOK:
		    EnableWindow(hwndimg, TRUE);
		    DestroyWindow(hwnd);
		    hDlgModeless = NULL;
		    post_args();
		    /* should post message to main window to delete thunks */
                    return(TRUE);
                default:
                    return(FALSE);
            }
	case WM_CLOSE:
	    EnableWindow(hwndimg, TRUE);
	    DestroyWindow(hwnd);
	    hDlgModeless = NULL;
	    post_args();
	    return TRUE;
    }
    return FALSE;
}


#ifdef __BORLANDC__
#pragma argsused	/* ignore warning for next function */
#endif
/* Modeless Dialog Box */
DLGRETURN CALLBACK _export
CfgChildDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
            return( TRUE);
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case IDOK:
                case IDNEXT:
		    next_page(hwnd);
                    return(TRUE);
                case IDPREV:
		    SendDlgItemMessage(hwnd, IDPREV, BM_SETSTYLE, 
		        (WPARAM)0, TRUE);	/* remove default style */
		    prev_page(hwnd);
                    return(TRUE);
                case IDCANCEL:
		    {	TCHAR buf[MAXSTR];
			load_string(IDS_CFG74, buf, sizeof(buf));
			SetDlgItemText(find_page_from_id(IDD_CFG7)->hwnd, IDC_CFG70, buf);
			goto_page(hwnd, IDD_CFG7);
		    }
                    return(TRUE);
		case IDC_CFG20:
		    if (GetNotification(wParam,lParam) == EN_CHANGE)
		    { int ver;
		      char buf[16];
		      ver = add_gsver(hwnd, 0);
		      sprintf(buf, "%d.%02d", ver / 100, ver % 100);
		      /* don't use touch IDC_CFG20 - this would be recursive */
		      gsdir_fix(hwnd, buf);
		    }
                    return(TRUE);
                default:
                    return(FALSE);
            }
	case WM_VSCROLL:
	    { int ver;
	      char buf[16];
		switch(LOWORD(wParam)) {
		    case SB_LINEUP:
			ver = add_gsver(hwnd, 1);
			break;
		    case SB_LINEDOWN:
			ver = add_gsver(hwnd, -1);
			break;
		    default:
			ver = add_gsver(hwnd, 0);
			break;
		}
		sprintf(buf, "%d.%02d", ver / 100, ver % 100);
		SetDlgItemTextA(hwnd, IDC_CFG20, buf);
	    }
	    return TRUE;
	case WM_CLOSE:
	    PostMessage(GetParent(hwnd), WM_COMMAND, (WPARAM)IDCANCEL, (LPARAM)0);
	    return TRUE;
    }
    return FALSE;
}


/* In case we have multiple monitors, get the size of the first and 
 * last displays.  This allows us to put dialog boxes on the the 
 * first display (instead of centred and split across two displays),
 * and to put the fullscreen window on the last display.
 */ 
typedef BOOL (WINAPI
	*LUENUMDISPLAYDEVICES)(HDC,DWORD,PDISPLAY_DEVICE,DWORD);

static void 
init_displays(void)
{
    DISPLAY_DEVICE dd;
    DEVMODE dm;
    int dev = 0; /* device index */
    int ndisp = 0;
 
    /* Get the function's address. We could use the new platform SDK, but 
     * this code also works with the old one and on older Windozes (NT/95)
     */
    HINSTANCE huser32=LoadLibrary("user32.dll");
    LUENUMDISPLAYDEVICES pEnumDisplayDevices=0;
    memset(&dd, 0, sizeof(dd));
    dd.cb = sizeof(dd);

    /* Defaults if we don't have or support multiple monitors */
    number_of_displays = 1;
    win_display[0].left   = 0;
    win_display[0].top    = 0;
    win_display[0].width  = GetSystemMetrics(SM_CXSCREEN);
    win_display[0].height = GetSystemMetrics(SM_CYSCREEN);

#ifdef _MSC_VER /* cygwin doesn't have the necessary header files */
    if (huser32!=NULL)
	pEnumDisplayDevices=(LUENUMDISPLAYDEVICES)
		GetProcAddress(huser32,"EnumDisplayDevicesA");

    /* If we support multiple monitors, get dimensions of first few */
    if (pEnumDisplayDevices!=NULL) {
	ndisp = 0;
	while ((*pEnumDisplayDevices)(0, dev, &dd, 0)) {
	    if (!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)) {
		memset(&dm, 0, sizeof(dm));
		dm.dmSize = sizeof(dm);
		if ((dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) && 
		    (EnumDisplaySettings(dd.DeviceName, 
			ENUM_CURRENT_SETTINGS, &dm) == TRUE)) {
		    /*  Don't use this because it sometimes gives garbage
		        on Windows 98/ME
			EnumDisplaySettings(dd.DeviceName, 
			    ENUM_REGISTRY_SETTINGS, &dm);
		    */
		    if (ndisp < sizeof(win_display)/sizeof(win_display[0])) {
			win_display[ndisp].left = dm.dmPosition.x;
			win_display[ndisp].top = dm.dmPosition.y;
			win_display[ndisp].width =  dm.dmPelsWidth;
			win_display[ndisp].height = dm.dmPelsHeight;
			ndisp++;
		    }
		}
	    }
	    dev++;
	}
    }
#endif
    if (ndisp == 0)
	ndisp = 1;
    number_of_displays = ndisp;

    if (huser32 != NULL)
	FreeLibrary(huser32);
}

