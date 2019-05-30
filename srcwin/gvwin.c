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

/* gvwin.c */
/* Main routines for Windows GSview */
#include "gvwin.h"

TCHAR szAppName[MAXSTR] = TEXT(GSVIEW_PRODUCT);  /* application name - for title bar */
TCHAR szExePath[MAXSTR];
char szIniFile[MAXSTR];
TCHAR szWait[MAXSTR];
char szFindText[MAXSTR];
char previous_filename[MAXSTR];
char selectname[MAXSTR];
const TCHAR szClassName[] = TEXT("gsview_class");
const TCHAR szImgClassName[] = TEXT("gsview_img_class");
const char szScratch[] = "gsview";	/* temporary filename prefix */
char *szSpoolPrefix = "\\\\spool\\";

HWND hwndimg;			/* gsview main window */
HWND hDlgModeless;		/* any modeless dialog box */
HWND hwnd_measure;		/* measure modelss dialog box */
HWND hwnd_fullscreen;		/* full screen popup of child window */
HWND hwnd_image;		/* full screen or image child window */	
HWND hwndimgchild;		/* gswin image child window */
HWND hwndspl;			/* window handle of gsv16spl.exe */
HINSTANCE phInstance;		/* instance of gsview */
HINSTANCE hlanguage;		/* instance of language resources */
UINT nCodePageLanguage;		/* Code page of the GSview language */
UINT nCodePageSystem;		/* Code page of the system */

#define MAXARGS 64
int nargc;
char *nargv[MAXARGS];
GSVIEW_ARGS args;		/* Parsed arguments */
PSFILE psfile;		/* Postscript file structure */
PRINTER printer;	/* Ghostscript printer structure */
BOOL win32s_printer_pending = FALSE;
BMAP bitmap;		/* information about display bitmap */
OPTIONS option;		/* GSview options (saved in INI file) */
DISPLAY display;	/* Display parameters */
char last_files[4][MAXSTR];	/* last 4 files used */
int last_files_count;		/* number of files known */
HISTORY history;		/* history of pages displayed */
BOOL fullscreen = FALSE;
HCURSOR hCursorArrow;
HPEN hpen_btnshadow;		/* button shadow */
HPEN hpen_btnhighlight;		/* button highlight */
HBRUSH hbrush_window;		/* Window background */
HBRUSH hbrush_menu;		/* menu background */

struct sound_s sound[NUMSOUND] = {
	{"SoundOutputPage", IDS_SNDPAGE, TEXT("")},
	{"SoundNoPage", IDS_SNDNOPAGE, TEXT(BEEP)},
	{"SoundNoNumbering", IDS_SNDNONUMBER, TEXT("")},
	{"SoundNotOpen", IDS_SNDNOTOPEN, TEXT("")},
	{"SoundError", IDS_SNDERROR, TEXT(BEEP)},
	{"SoundStart", IDS_SNDSTART, TEXT("")},
	{"SoundExit", IDS_SNDEXIT, TEXT("")},
	{"SoundBusy", IDS_SNDBUSY, TEXT(BEEP)},
};
USERMEDIA usermedia[IDM_USERSIZE13 - IDM_USERSIZE1 + 1];

/* initialised in init.c */
BOOL is_winnt = FALSE;		/* To allow selective use of Windows NT features */
BOOL is_win95 = FALSE;		/* To allow selective use of Windows 95 features */
BOOL is_win98 = FALSE;		/* To allow selective use of Windows 98 features */
BOOL is_win32s = FALSE;		/* To allow selective use of Win32s misfeatures */
BOOL is_win4;			/* To allow selective use of Windows 4.0 features */
BOOL multithread = FALSE;
CRITICAL_SECTION crit_sec;	/* for thread synchronization */
HANDLE hmutex_ps;		/* for protecting psfile and pending */
TCHAR szHelpName[MAXSTR];	/* buffer for building help filename */
int nHelpTopic;			/* topic for OFN_SHOWHELP */
UINT help_message;		/* message sent by OFN_SHOWHELP */
HMENU hmenu;			/* main menu */
HACCEL haccel;			/* menu accelerators */
HCURSOR hcWait;
HCURSOR hcCrossHair;
HCURSOR hcHand;
POINT img_offset;		/* offset to gswin child window */
HFONT info_font;		/* font for info line */
HFONT hFontAnsi;		/* ANSI (Western European) font */
HFONT hFontGreek;		/* cp 1253 / Greek font */
HFONT hFontEastEurope;		/* ISO-Latin2 / cp1250 font */
HFONT hFontCyrillic;		/* Windows Cyrillic / cp1251 font */
POINT info_file;		/* position of file information */
POINT info_page;		/* position of page information */
RECT  info_rect;		/* position and size of brief info area */
RECT  info_coord;		/* position and size of coordinate information */
RECT  button_rect;		/* position and size of button area */
int on_link;			/* TRUE if we were or are over link */
int on_link_page;		/* page number of link target */
const char *on_link_action;	/* action of link target */
long gsbytes_size;		/* number of bytes for this page */
long gsbytes_done;		/* number of byte written */
BOOL fit_page_enabled = FALSE;	/* next WM_SIZE is allowed to resize window */

BOOL prev_in_child;		/* true if cursor previously in gswin child window */
int page_skip = 5;		/* number of pages to skip in IDM_NEXTSKIP or IDM_PREVSKIP */
BOOL zoom = FALSE;		/* true if display zoomed */
int debug = 0;			/* /D command line option used */
BOOL print_silent = FALSE;	/* /P or /F command line option used */
BOOL print_exit = FALSE;	/* exit on completion of printing */
int print_count = 0;		/* number of current print jobs */
				/* It is safe to exit GSview when this is 0 */
int portable_app = 0;		/* /A command line option used */
HINSTANCE hlib_mmsystem;	/* DLL containing sndPlaySound function */
FPSPS lpfnSndPlaySound;		/* pointer to sndPlaySound function if loaded */
BOOL quitnow = FALSE;		/* Used to cause exit from nested message loops */

int percent_done;		/* percentage of document processed */
int percent_pending;		/* TRUE if WM_GSPERCENT is pending */

int number_of_displays = 1; /* number of active displays */
DISPLAY_INFO win_display[4];


/* GSview on Windows now uses HtmlHelp, and topics are accessed by 
 * HTML filename within the compressed HTML help file.
 * The names are the same for all languages, as below.
 * The topic names in gvclang.rc are not used by Windows
 * but may still be used by other platforms.
 */
struct help_entry_s {
	int id;			/* Help topic identifier */
	const TCHAR *name;	/* profile entry */
};

struct help_entry_s help_entry[] = {
	/* Help topics */
	/* These need to match -filenames in XX\gvclang.txt */
	/* and need to be the same for all languages */
	{IDS_TOPICROOT,  "Overview"},
	{IDS_TOPICOPEN,  "Open"},
	{IDS_TOPICPRINT, "Print"},
	{IDS_TOPICPROP, "Properties"},
	{IDS_TOPICEDITPROP, "Edit_Properties"},
	{IDS_TOPICCONVERT, "Convert"},
	{IDS_TOPICCLIP, "Clipboard"},
	{IDS_TOPICPREVIEW, "EPS_Preview"},
	{IDS_TOPICSOUND, "Sounds"},
	{IDS_TOPICDSET, "Display_Settings"},
	{IDS_TOPICMEDIA, "Media"},
	{IDS_TOPICPSTOEPS, "PS_to_EPS"},
	{IDS_TOPICGOTO, "Page_Selection"},
	{IDS_TOPICINSTALL, "Installation"},
	{IDS_TOPICTEXT, "Text_Extract_and_Find"},
	{IDS_TOPICKEYS, "Keys"},
	{IDS_TOPICMESS, "Show_Messages"},
	{IDS_TOPICSPOOL, "Spooler"},
	{IDS_TOPICZLIB, "zlib"},
	{IDS_TOPICBZIP2, "bzip2"},
	{IDS_TOPICPSTOEDIT, "PStoEdit"},
	{IDS_TOPICMEASURE, "Measure"},
	{IDS_TOPICEASYCFG, "Easy_Configure"},
	{IDS_TOPICADVANCEDCFG, "Advanced_Configure"},
	{IDS_TOPICDOWNLOAD, "Obtaining_Ghostscript"},
	{IDS_TOPICDSCWARN, "DSC_Warnings"},
	{IDS_TOPICREG, "Registration"},
	{0, NULL}
};


#if (WINVER < 0x0400)
/* Windows 4.0 scroll bar extras */
#define SIF_RANGE           0x01
#define SIF_PAGE            0x02
#define SIF_POS             0x04
#define SIF_DISABLENOSCROLL 0x08
#define SIF_TRACKPOS        0x10
#define SIF_ALL             (SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS)
#define SBM_SETSCROLLINFO 0x00E9
#define SBM_GETSCROLLINFO 0x00EA

typedef struct tagSCROLLINFO {
    UINT cbSize;
    UINT fMask;
    int nMin;
    int nMax;
    UINT nPage;
    int nPos;
    int nTrackPos;
} SCROLLINFO;
typedef SCROLLINFO FAR *LPSCROLLINFO;
/*
WINUSERAPI int WINAPI SetScrollInfo(HWND, int, LPSCROLLINFO, BOOL);
WINUSERAPI int WINAPI GetScrollInfo(HWND, int, LPSCROLLINFO);
*/
#endif

typedef int (WINAPI *PFN_SetScrollInfo)(HWND, int, LPSCROLLINFO, BOOL);
PFN_SetScrollInfo pSetScrollInfo;
HMODULE hmodule_user32;

BOOL
load_SetScrollInfo(void)
{
    /* Instead of linking to SetScrollInfo at load time,
     * we instead do it at run time.
     * This allows us to produce an EXE that will run under
     * Windows 3.1 and Windows 4.0
     */
    if (!is_win4)
	return FALSE;
    hmodule_user32 = LoadLibrary(TEXT("USER32.DLL"));
    if (hmodule_user32 < (HINSTANCE)HINSTANCE_ERROR) {
	hmodule_user32 = (HINSTANCE)NULL;
        return FALSE;
    }
    pSetScrollInfo = (PFN_SetScrollInfo) GetProcAddress(hmodule_user32, "SetScrollInfo");
    if (!pSetScrollInfo)
        return FALSE;
    return TRUE;
}

void
free_SetScrollInfo(void) 
{
    if (!hmodule_user32)
	return;
    pSetScrollInfo = (PFN_SetScrollInfo)NULL;
    FreeLibrary(hmodule_user32);
    hmodule_user32 = (HINSTANCE)NULL;
}


/* local functions */
BOOL draw_button(DRAWITEMSTRUCT FAR *lpdis);
BOOL in_child_client_area(void);
BOOL in_client_area(void);
BOOL in_info_area(void);
void info_paint(HWND, HDC);
void cursorpos_paint(HDC hdc);
void gsview_close(void);
BOOL query_close(void);
void update_scroll_bars(void);
void gs_thread(void *arg);
void map_pt_to_pixel(float *x, float *y);
void enable_menu_item(int menuid, int itemid, BOOL enabled);
void end_button_help(void);

#ifndef VIEWONLY
void highlight_words(HDC hdc, int first, int last);
BOOL text_marking = FALSE;
int text_mark_first = -1;
int text_mark_last = -1;
#endif

void highlight_links(HDC hdc);
void info_link(void);
int win_get_args(int *argc, char **argv, int maxarg, char *cmdline);

#ifndef MEMORYFILE
int PASCAL 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int cmdShow)
{
    LPSTR pcmdline = GetCommandLineA();
    return gsview_main(hInstance, pcmdline);
}
#endif

int gsview_main(HINSTANCE hInstance, LPSTR lpszCmdLine)
{
    MSG msg;
    DWORD dwHelpCookie = 0;
    char *cmdline;

    /* copy the hInstance into a variable so it can be used */
    phInstance = hInstance;

    dde_initialise();
    cmdline = (char *)malloc(strlen(lpszCmdLine)+1);
    if (cmdline == NULL)
	return 0;
    strcpy(cmdline, lpszCmdLine);
    win_get_args(&nargc, nargv, MAXARGS, cmdline);

    if (!gsview_init1(nargc, nargv)) {
	dde_uninitialise();
	return FALSE;
    }

    /* don't start the DDE server until after we have parsed the 
     * command line, otherwise we could end up talking to ourselves
     * instead of a separate copy of GSview
     */
    dde_enable_server(TRUE);

    load_SetScrollInfo();
    if (print_silent) {
	ShowWindow(hwndimg, SW_SHOWMINNOACTIVE);
    }
    else
    {   STARTUPINFO sti;
	GetStartupInfo(&sti);
	ShowWindow(hwndimg, option.img_max && (sti.wShowWindow == SW_SHOWNORMAL) 
	    ? SW_SHOWMAXIMIZED : SW_SHOWDEFAULT);
    }
    info_wait(IDS_NOWAIT);
    if (gsview_changed())
	PostQuitMessage(0);
    else if (!print_silent)
	// before changing following line, please see gvcreg.cpp
	registration_check();

    if (multithread) {
	/* start thread for displaying */
	display.tid = _beginthread(gs_thread, 131072, NULL);
    }
#ifdef USE_HTMLHELP
    HtmlHelp(hwndimg, NULL, HH_INITIALIZE, (DWORD)&dwHelpCookie);
#endif
    
    while (!(!multithread && quitnow)
	     && GetMessage(&msg, (HWND)NULL, 0, 0)) {
	if ( ((hDlgModeless == 0) || !IsDialogMessage(hDlgModeless, &msg)) 
	  && ((hwnd_measure == 0) || !IsDialogMessage(hwnd_measure, &msg)) 
	   ) {
#ifdef USE_HTMLHELP
	    if (!HtmlHelp(NULL, NULL, HH_PRETRANSLATEMESSAGE, (DWORD)(&msg))) { 
#endif
		if (!TranslateAccelerator(hwndimg, haccel, &msg)) {
		    TranslateMessage(&msg);
		    DispatchMessage(&msg);
		}
#ifdef USE_HTMLHELP
	    }
#endif
	}
	if (multithread) {
	    /* release other thread if needed */
	    if (pending.unload || pending.now || pending.next || quitnow)
		SetEvent(display.event);
	}
	else 
	{
	    if (pending.now) {
		if (is_win95 || is_winnt)
		    gs_process();	/* start Ghostscript */
		else {
		    /* Win32s can't start gs_process while printing */
		    /* Check if gvwgs.exe is running */
		    HWND hwndprn = FindWindow(NULL, TEXT("GSview Print"));
		    if (IsWindow(hwndprn)) {
			pending.now = FALSE;
			gserror(0, "Busy printing.  Win32s can't use Ghostscript for displaying while it is being used for printing. Try again when 'GSview Print' has finished.", MB_ICONEXCLAMATION, SOUND_ERROR);
		    }
		    if (pending.now)
			gs_process();	/* start Ghostscript */
		}
		update_scroll_bars();
	    }
	}
#ifndef VIEWONLY
	if (
	    is_win32s && 
	    win32s_printer_pending) {
	    /* Win32s can't load GS DLL twice */
	    /* so we must run it while display GS DLL is unloaded */
	    start_gvwgs();
	    win32s_printer_pending = FALSE;
	    /* We can't stop the user attempting to display while */
	    /* printing since we don't know when printer finished */
	    /* We'll see how much of a problem this causes */
	}
#endif
    }

    dde_uninitialise();

    play_sound(SOUND_EXIT);
    gsview_close();
    DestroyWindow(hwndimg);
    hwndimg = HWND_DESKTOP;
#ifndef VIEWONLY
    printer_cleanup();
#endif
    delete_buttons();
    free_SetScrollInfo();
#ifdef USE_HTMLHELP
    HtmlHelp(hwndimg,szHelpName, HH_UNINITIALIZE, (DWORD)dwHelpCookie);
#else
    WinHelp(hwndimg,szHelpName,HELP_QUIT,(DWORD)NULL);
#endif
    if (hlib_mmsystem != (HINSTANCE)NULL)
	FreeLibrary(hlib_mmsystem);
    if ((hlanguage != (HINSTANCE)NULL) && (hlanguage != phInstance))
	FreeLibrary(hlanguage);

#ifdef DEBUG_MALLOC
    if (malloc_file)
	fclose(malloc_file);
#endif

    if (debug & DEBUG_MEM) {
	while (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE))
	    /* clear quit message */;
	debug_memory_report();
    }
    return 0;
}



/* child image window */
LRESULT CALLBACK _export
WndImgChildProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{	
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    int nVscrollInc, nHscrollInc;
    static int cxAdjust, cyAdjust;
    static int cxClient, cyClient;
    static int nHscrollPos, nHscrollMax;
    static int nVscrollPos, nVscrollMax;

    switch(message) {
	    case WM_CREATE:
		    cxClient = cyClient = 0;
		    nHscrollPos = nHscrollMax = 0;
		    nVscrollPos = nVscrollMax = 0;
		    break;
	    case WM_DESTROY:
		    hwnd_image = hwndimgchild;
		    fullscreen = FALSE;
		    if ((hwnd == hwnd_fullscreen) && image.open) {
			ShowWindow(hwnd_image, SW_SHOWNA);
			update_scroll_bars();
			InvalidateRect(hwnd_image, (LPRECT)NULL, FALSE);
			UpdateWindow(hwnd_image);
			if (debug)
			    gs_addmess("Full Screen finished\r\n");
		    }
		    hwnd_fullscreen = (HWND)NULL;
		    break;
	    case WM_SIZE:
		    if (wParam == SIZE_MINIMIZED)
			    return(0);
		    cyClient = HIWORD(lParam);
		    cxClient = LOWORD(lParam);

#ifdef OLD
		    cyAdjust = min(image.height, cyClient) - cyClient;
		    cyClient += cyAdjust;
#else
		    if (image.height < cyClient) {
			/* shrink window */
			cyAdjust = image.height - cyClient;
		    }
		    else {
			if (!fullscreen && fit_page_enabled) {
			    /* We just got a GSDLL_SIZE and option.fitpage 
			     * was TRUE enlarge window to smaller of bitmap 
			     * height and height if client extended to 
			     * bottom of screen
			     */
			    GetWindowRect(GetParent(hwnd),&rect);
			    cyAdjust = min(image.height, 
				cyClient + GetSystemMetrics(SM_CYFULLSCREEN) + 
				GetSystemMetrics(SM_CYCAPTION) - rect.bottom)
				- cyClient;
			}
			else
			    cyAdjust = 0;
		    }
		    cyClient += cyAdjust;
#endif

		    nVscrollMax = max(0, image.height - cyClient);
		    nVscrollPos = min(nVscrollPos, nVscrollMax);

		    if (!image.open)
			nVscrollMax = nVscrollPos = 0;

		    if (fullscreen) {
			/* never display scroll bars */
			SetScrollRange(hwnd, SB_VERT, 0, 0, TRUE);
		    }
		    else if (is_win4 && pSetScrollInfo) {
			SCROLLINFO si;
			si.cbSize = sizeof(si);
			si.fMask = SIF_ALL;
			si.nMin = 0;
			/* Win32 docs say use nMax = nVscrollMax + (nPage-1) */
			/* Overview example says use nMax = nVscrollMax */
			if (nVscrollMax) {
			    si.nPage = cyClient;
			    si.nMax = nVscrollMax + (cyClient - 1);
			}
			else {
			    si.nMax = si.nPage = 0;
			}
			si.nPos = nVscrollPos;
			si.nTrackPos = 0;
			pSetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		    }
		    else 
		    {
			SetScrollRange(hwnd, SB_VERT, 0, nVscrollMax, FALSE);
			SetScrollPos(hwnd, SB_VERT, nVscrollPos, TRUE);
		    }

#ifdef OLD
		    cxAdjust = min(image.width,  cxClient) - cxClient;
		    cxClient += cxAdjust;
#else
		    if (image.width < cxClient) {
			/* shrink window */
			cxAdjust = image.width - cxClient;
		    }
		    else {
			if (fit_page_enabled) {
			    /* We just got a GSDLL_SIZE and option.fitpage was TRUE */
			    /* enlarge window to smaller of bitmap width */
			    /* and width if client extended to right of screen */
			    GetWindowRect(GetParent(hwnd),&rect);
			    cxAdjust = min(image.width, 
				cxClient + GetSystemMetrics(SM_CXFULLSCREEN)-rect.right)
				- cxClient;
			}
			else
			    cxAdjust = 0;
		    }
		    cxClient += cxAdjust;
#endif

		    nHscrollMax = max(0, image.width - cxClient);
		    nHscrollPos = min(nHscrollPos, nHscrollMax);

		    if (!image.open)
			nHscrollMax = nHscrollPos = 0;

		    if (fullscreen) {
			/* never display scroll bars */
			SetScrollRange(hwnd, SB_HORZ, 0, 0, TRUE);
		    }
		    else if (is_win4 && pSetScrollInfo) {
			SCROLLINFO si;
			si.cbSize = sizeof(si);
			si.fMask = SIF_ALL;
			si.nMin = 0;
			if (nHscrollMax) {
			    si.nPage = cxClient;
			    si.nMax = nHscrollMax + (cxClient - 1);
			}
			else {
			    si.nMax = si.nPage = 0;
			}
			si.nPos = nHscrollPos;
			si.nTrackPos = 0;
			pSetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		    }
		    else 
		    {
			SetScrollRange(hwnd, SB_HORZ, 0, nHscrollMax, FALSE);
			SetScrollPos(hwnd, SB_HORZ, nHscrollPos, TRUE);
		    }

		    bitmap.scrollx = nHscrollPos;
		    bitmap.scrolly = nVscrollPos;

		    if (!fullscreen && option.fit_page &&
			(wParam==SIZE_RESTORED) && !IsZoomed(hwndimg) &&
			image.open && (cxAdjust!=0 || cyAdjust!=0) ) {
			GetWindowRect(GetParent(hwnd),&rect);
			MoveWindow(GetParent(hwnd),rect.left,rect.top,
			    rect.right-rect.left+cxAdjust,
			    rect.bottom-rect.top+cyAdjust, TRUE);
			cxAdjust = cyAdjust = 0;
		    }
		    fit_page_enabled = FALSE;

		    /* centre the bitmap if smaller than client window */
		    GetClientRect(hwnd, &rect);
		    cxClient = rect.right - rect.left;
		    cyClient = rect.bottom - rect.top;
		    if (image.width < cxClient)
			display.offset.x = (cxClient - image.width) / 2;
		    else
			display.offset.x = 0;
		    if (image.height < cyClient)
			display.offset.y = (cyClient - image.height) / 2;
		    else
			display.offset.y = 0;

		    return(0);
	    case WM_VSCROLL:
		    switch(LOWORD(wParam)) {
			case SB_TOP:
				nVscrollInc = -nVscrollPos;
				break;
			case SB_BOTTOM:
				nVscrollInc = nVscrollMax - nVscrollPos;
				break;
			case SB_LINEUP:
				nVscrollInc = -cyClient/16;
				break;
			case SB_LINEDOWN:
				nVscrollInc = cyClient/16;
				break;
			case SB_PAGEUP:
				nVscrollInc = min(-1,-cyClient);
				break;
			case SB_PAGEDOWN:
				nVscrollInc = max(1,cyClient);
				break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				nVscrollInc = HIWORD(wParam) - nVscrollPos;
				break;
			case SB_FIND:
				/* non standard */
				nVscrollInc = (short)HIWORD(wParam);
				break;
			default:
				nVscrollInc = 0;
		    }
		    if ((nVscrollInc = max(-nVscrollPos, 
			    min(nVscrollInc, nVscrollMax - nVscrollPos)))!=0) {
			    nVscrollPos += nVscrollInc;
			    ScrollWindow(hwnd,0,-nVscrollInc,NULL,NULL);
			    SetScrollPos(hwnd,SB_VERT,nVscrollPos,TRUE);
			    bitmap.scrollx = nHscrollPos;
			    bitmap.scrolly = nVscrollPos;
			    UpdateWindow(hwnd);
		    }
		    else if (gsdll.state != GS_IDLE) {
			/* We are at the top or bottom of the 
			 * scroll range.  Change page if 
			 * PageUp or PageDown pressed. */
			int numpages = 0;
			request_mutex();
			if (psfile.dsc != (CDSC *)NULL)
			    numpages = psfile.dsc->page_count;
			release_mutex();
			switch(LOWORD(wParam)) {
			    case SB_PAGEUP:
			      if ((psfile.dsc != (CDSC *)NULL)
				    && (psfile.pagenum != 1)) {
				PostMessage(hwnd ,WM_VSCROLL, SB_BOTTOM,0L);
				gsview_command(IDM_PREV);
			      }
			      break;
			    case SB_PAGEDOWN:
			      if ((psfile.dsc == (CDSC *)NULL)
				    || (psfile.pagenum < numpages)) {
				PostMessage(hwnd ,WM_VSCROLL, SB_TOP,0L);
				gsview_command(IDM_NEXT);
			      }
			      break;
			}
		    }

		    return(0);
	    case WM_HSCROLL:
		    switch(LOWORD(wParam)) {
			case SB_TOP:
				nHscrollInc = -nHscrollPos;
				break;
			case SB_BOTTOM:
				nHscrollInc = nHscrollMax - nHscrollPos;
				break;
			case SB_LINEUP:
				nHscrollInc = -cxClient/16;
				break;
			case SB_LINEDOWN:
				nHscrollInc = cyClient/16;
				break;
			case SB_PAGEUP:
				nHscrollInc = min(-1,-cxClient);
				break;
			case SB_PAGEDOWN:
				nHscrollInc = max(1,cxClient);
				break;
			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:
				nHscrollInc = HIWORD(wParam) - nHscrollPos;
				break;
			case SB_FIND:
				/* non standard */
				nHscrollInc = (short)HIWORD(wParam);
				break;
			default:
				nHscrollInc = 0;
		    }

		    if ((nHscrollInc = max(-nHscrollPos, 
			    min(nHscrollInc, nHscrollMax - nHscrollPos)))!=0) {
			    nHscrollPos += nHscrollInc;
			    ScrollWindow(hwnd,-nHscrollInc,0,NULL,NULL);
			    SetScrollPos(hwnd,SB_HORZ,nHscrollPos,TRUE);
			    bitmap.scrollx = nHscrollPos;
			    bitmap.scrolly = nVscrollPos;
			    UpdateWindow(hwnd);
		    }

		    return(0);
	    case WM_KEYDOWN:
		    end_button_help();
		    switch(LOWORD(wParam)) {
			case VK_HOME:
			    SendMessage(hwnd,WM_VSCROLL,SB_TOP,0L);
			    break;
			case VK_END:
			    SendMessage(hwnd,WM_VSCROLL,SB_BOTTOM,0L);
			    break;
			case VK_PRIOR:
			    SendMessage(hwnd,WM_VSCROLL,SB_PAGEUP,0L);
			    break;
			case VK_NEXT:
			    SendMessage(hwnd,WM_VSCROLL,SB_PAGEDOWN,0L);
			    break;
			case VK_UP:
			    if (GetKeyState(VK_CONTROL) & 0x8000)
			      SendMessage(hwnd,WM_VSCROLL,SB_PAGEUP,0L);
			    else
			      SendMessage(hwnd,WM_VSCROLL,SB_LINEUP,0L);
			    break;
			case VK_DOWN:
			    if (GetKeyState(VK_CONTROL) & 0x8000)
			      SendMessage(hwnd,WM_VSCROLL,SB_PAGEDOWN,0L);
			    else
			      SendMessage(hwnd,WM_VSCROLL,SB_LINEDOWN,0L);
			    break;
			case VK_LEFT:
			    if (GetKeyState(VK_CONTROL) & 0x8000)
			      SendMessage(hwnd,WM_HSCROLL,SB_PAGEUP,0L);
			    else
			      SendMessage(hwnd,WM_HSCROLL,SB_LINEUP,0L);
			    break;
			case VK_RIGHT:
			    if (GetKeyState(VK_CONTROL) & 0x8000)
			      SendMessage(hwnd,WM_HSCROLL,SB_PAGEDOWN,0L);
			    else
			      SendMessage(hwnd,WM_HSCROLL,SB_LINEDOWN,0L);
			    break;
			case VK_ESCAPE:
			    gsview_fullscreen_end();
			    break;
		    }
		    return(0);
	    case WM_KEYUP:
		    end_button_help();
		    break;
	    case WM_SETCURSOR:
		    if (fullscreen && (hwnd == hwnd_fullscreen)) {
			if (szWait[0] != '\0')
			    SetCursor(NULL);
			else
			    SetCursor(hCursorArrow);
			if (image.open) {
			    float x, y;
			    if (get_cursorpos(&x, &y)) {
				PDFLINK link;
				if (is_link(x, y, &link)) {
				    SetCursor(hcHand);
				    return TRUE;
				}
			    }
			}
			return TRUE;
		    }
		    break;
	    case WM_RBUTTONDOWN:
		    if (fullscreen && (hwnd == hwnd_fullscreen)) {
			SendMessage(hwndimg, WM_COMMAND, IDM_PREV, 0L);
			break;
		    }
		    break;
	    case WM_LBUTTONDOWN:
#ifndef VIEWONLY
		    if (hDlgModeless && in_child_client_area())
			SendMessage(hDlgModeless, WM_COMMAND, BB_CLICK, lParam);
		    else {
			int iword ;
			float x, y;
			PDFLINK link;
			BOOL full = fullscreen && (hwnd == hwnd_fullscreen);
			if (get_cursorpos(&x, &y)) {
			    if (!full) {
				HDC hdc = GetDC(hwnd);
				if ( (iword = word_find((int)x, (int)y)) >= 0){
				    /* remove any current selection */
				    highlight_words(hdc, text_mark_first, 
					text_mark_last);
				    /* mark new selection */
				    text_mark_first = text_mark_last = iword;
				    text_marking = TRUE;
				    highlight_words(hdc, text_mark_first, 
					text_mark_last);
				    SetCapture(hwnd);
				}
				else {
				    /* remove selection */
				    highlight_words(hdc, text_mark_first, 
					text_mark_last);
				    text_mark_first = text_mark_last = -1;
				    ReleaseCapture();
				}
				ReleaseDC(hwnd, hdc);
			    }
			    /* pdfmark link */
			    if (is_link(x, y, &link)) {
				/* found link */
				if (link.page == 0) {
				    if (strcmp(link.action, "GoBack")==0)
					history_back();
				    else
				        gserror(IDS_NOLINKTARGET, NULL, 0, 
					    SOUND_ERROR);
				}
				else {
				    gsview_unzoom();
				    pending.pagenum = link.page;
				    history_add(pending.pagenum);
				    pending.now = TRUE;
				}
			    }
			    else if (full) {
				SendMessage(hwndimg, WM_COMMAND, IDM_NEXT, 0L);
				break;
			    }
			}
			/* set last point for measuring distances */
			measure_setpoint(x, y);
		    }
#endif
		    break;
#ifndef VIEWONLY
	    case WM_MOUSEMOVE:
		    if (text_marking) {
			int iword ;
			float x, y;
			while (!in_child_client_area()) {
			    RECT rect;
			    POINT pt;
			    GetCursorPos(&pt);
			    GetClientRect(hwnd, &rect);
			    ScreenToClient(hwnd, &pt);
			    if (pt.x > rect.right)
				SendMessage(hwnd, WM_HSCROLL,SB_LINEDOWN,0L);
			    if (pt.x < rect.left)
				SendMessage(hwnd, WM_HSCROLL,SB_LINEUP,0L);
			    if (pt.y > rect.bottom)
				SendMessage(hwnd, WM_VSCROLL,SB_LINEDOWN,0L);
			    if (pt.y < rect.top)
				SendMessage(hwnd, WM_VSCROLL,SB_LINEUP,0L);
			    Sleep(100);
			}
			if (get_cursorpos(&x, &y)) {
			    if ( (iword = word_find((int)x, (int)y)) >= 0 ) {
				if (iword != text_mark_last) {
				    HDC hdc = GetDC(hwnd_image);
				    int first, last;
				    if ((text_mark_last-text_mark_first >= 0) != (iword-text_mark_first >= 0)) {
					/* changing direction */
					/* clear everything */
					highlight_words(hdc, text_mark_first, text_mark_last);
					/* reinstate first word */
					text_mark_last = text_mark_first;
					highlight_words(hdc, text_mark_first, text_mark_last);
				    }
				    if (iword != text_mark_last) {
				      if (iword >= text_mark_first) {
					if (iword > text_mark_last)
					    first=text_mark_last+1, last=iword;
					else
					    first=iword+1, last=text_mark_last;
				      }
				      else {
					if (iword > text_mark_last)
					    first=text_mark_last, last=iword-1;
					else
					    first=iword, last=text_mark_last-1;
				      }
				      highlight_words(hdc, first, last);
				      text_mark_last = iword;
				    }
				    ReleaseDC(hwnd_image, hdc);
				}
			    }
			}
		    }
		    break;
	    case WM_LBUTTONUP:
		    ReleaseCapture();
		    text_marking = FALSE;
		    break;
#endif
	    case WM_GSTILE:
		    /* draw a tile that has changed */
		    image_lock(&image);
		    if (image.open) {
			/* calculate area */
			RECT rect;
			SYSTEMTIME t1, t2;
			int td;
			int tx1 = HIBYTE(HIWORD(wParam));
			int ty1 = LOBYTE(HIWORD(wParam));
			int tx2 = HIBYTE(LOWORD(wParam));
			int ty2 = LOBYTE(LOWORD(wParam));
			rect.left = (tx1 * image.tile_width) 
			    + display.offset.x - nHscrollPos;
			rect.right = rect.left + 
				(tx2 - tx1 + 1) * image.tile_width + 1;
			rect.top = 
			    (image.height - 1 - ((ty2+1) * image.tile_height)) 
			    + display.offset.y - nVscrollPos + 1;
			rect.bottom =  rect.top +
			    (ty2 - ty1 + 1) * image.tile_height + 1;
			GetSystemTime(&t1);
		        image_unlock(&image);
			InvalidateRect(hwnd, &rect, FALSE);
		        UpdateWindow(hwnd);
			GetSystemTime(&t2);
			/* make sure we don't spend too long doing updates */
			td = (t2.wHour - t1.wHour) * 3600000 + 
			    (t2.wMinute - t1.wMinute ) * 60000 +
			    (t2.wSecond - t1.wSecond ) * 1000 + 
			    (t2.wMilliseconds - t1.wMilliseconds);
			image_lock(&image);
			if (image.open && (td * 5 > image.tile_interval)) {
			    /* Backup off time and require to be done
			     * between updates.
			     */
			    image.tile_interval += td + 100;
			    image.tile_verydirty_threshold += 256;
			    image.tile_threshold += image.tile_threshold;
			}
		        image_unlock(&image);
		    }
		    else {
		        image_unlock(&image);
		    }
		    break;
	    case WM_PAINT:
		    if (quitnow) {
			hdc = BeginPaint(hwnd, &ps);
			rect = ps.rcPaint;
			FillRect(hdc, &rect, hbrush_window);
			EndPaint(hwnd, &ps);
			return 0;
		    }
		    image_lock(view.img);
		    request_mutex();
		    {
		    int wx,wy;
		    RECT source, dest, fillrect;
		    CDSC *dsc = psfile.dsc;
		    HBRUSH hbrush = hbrush_window;
		    hdc = BeginPaint(hwnd, &ps);
		    rect = ps.rcPaint;
		    SetMapMode(hdc, MM_TEXT);
		    SetBkMode(hdc,OPAQUE);
		    if (hwnd == hwnd_fullscreen)
			hbrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
		    if (image.open && 
			    (image.width > 1) && (image.height > 1)) {

			wx = rect.right-rect.left; /* width */
			wy = rect.bottom-rect.top;
			if (rect.left < display.offset.x)
				source.left = 0;
			else
				source.left = rect.left - display.offset.x + nHscrollPos;
			if (rect.top < display.offset.y)
				source.top = 0;
			else
				source.top = rect.top - display.offset.y + nVscrollPos;
			if (source.left > image.width)
				source.left = image.width;
			if (source.left + wx > image.width)
				wx = image.width - source.left;
			source.right = source.left + wx;
			if (source.top > image.height)
				source.top = image.height;
			if (source.top + wy > image.height)
				wy = image.height - source.top;
			source.bottom = source.top + wy;

			if (rect.left < display.offset.x)
				dest.left = display.offset.x;
			else
				dest.left = rect.left;
			if (rect.top < display.offset.y)
				dest.top = display.offset.y;
			else
				dest.top = rect.top;
			dest.right = dest.left + wx;
			dest.bottom = dest.top + wy;
			    
			if (wx && wy) {
			    image_draw(&image, hdc, dest.left, dest.top,
				wx, wy, source.left, source.top);
			}
			/* Fill areas around page */
			/* not sure if display.offset.y is from top or bottom */
			/* code here assumes top (opposite to OS/2) */
			if (rect.bottom > image.height + display.offset.y) {	/* bottom centre */
			    fillrect.bottom = rect.bottom; 
			    fillrect.top = image.height + display.offset.y;
			    fillrect.left = dest.left;
			    fillrect.right = dest.right;
			    FillRect(hdc, &fillrect, hbrush);
			}
			if (rect.top < display.offset.y) { /* top centre */
			    fillrect.bottom = display.offset.y;
			    fillrect.top = rect.top;
			    fillrect.left = dest.left;
			    fillrect.right = dest.right;
			    FillRect(hdc, &fillrect, hbrush);
			}
			if (rect.left < display.offset.x) { /* left */
			    fillrect.bottom = rect.bottom;
			    fillrect.top = rect.top;
			    fillrect.left = rect.left;
			    fillrect.right = display.offset.x;
			    FillRect(hdc, &fillrect, hbrush);
			}
			if (rect.right > image.width + display.offset.x) { /* right */
			    fillrect.bottom = rect.bottom;
			    fillrect.top = rect.top;
			    fillrect.left = image.width + display.offset.x;
			    fillrect.right = rect.right;
			    FillRect(hdc, &fillrect, hbrush);
			}
		    }
		    else {
			FillRect(hdc, &rect, hbrush);
		    }
		    /* draw bounding box */
		    if (image.open && (dsc != (CDSC *)NULL) &&
			(dsc->bbox != (CDSCBBOX *)NULL) &&
			option.show_bbox) {
			float x, y;
			HPEN hpen, hpen_old;
			/* map bounding box to device coordinates */
			x = (float)dsc->bbox->llx;
			y = (float)dsc->bbox->lly;
			map_pt_to_pixel(&x, &y);
			rect.left   = (int)x;
			rect.bottom = (int)y;
			x = (float)dsc->bbox->urx;
			y = (float)dsc->bbox->ury;
			map_pt_to_pixel(&x, &y);
			rect.right  = (int)x;
			rect.top    = (int)y;

			hpen = CreatePen(PS_DOT, 1, RGB(0,0,0));
			hpen_old = SelectPen(hdc, hpen);
			SelectPen(hdc, hpen);
			SetROP2(hdc, R2_XORPEN);
			MoveTo(hdc, rect.left, rect.bottom);
			LineTo(hdc, rect.right, rect.bottom);
			LineTo(hdc, rect.right, rect.top);
			LineTo(hdc, rect.left, rect.top);
			LineTo(hdc, rect.left, rect.bottom);
			SetROP2(hdc, R2_COPYPEN);
			SelectPen(hdc, hpen_old);
			DeletePen(hpen);
		    }
		    /* highlight found search word */
		    if (image.open && display.show_find) {
			float x, y;
			/* map bounding box to device coordinates */
			x = (float)psfile.text_bbox.llx;
			y = (float)psfile.text_bbox.lly;
			map_pt_to_pixel(&x, &y);
			rect.left   = (int)x;
			rect.bottom = (int)y;
			x = (float)psfile.text_bbox.urx;
			y = (float)psfile.text_bbox.ury;
			map_pt_to_pixel(&x, &y);
			rect.right  = (int)x;
			rect.top    = (int)y;
			if (rect.top > rect.bottom) {
			    int temp = rect.top;
			    rect.top = rect.bottom;
			    rect.bottom = temp;
			}

			/* invert text */
			InvertRect(hdc, &rect);

		    }

#ifndef VIEWONLY
		    /* highlight marked words */
		    highlight_words(hdc, text_mark_first, text_mark_last);
#endif

		    /* GS 6.50 highlights links itself for PDF files */
		    if ((option.gsversion < 650) || !psfile.ispdf)
			highlight_links(hdc);

		    EndPaint(hwnd, &ps);
		    release_mutex();
		    if (hwnd == hwnd_fullscreen)
			SetCursor(NULL);
		    image_unlock(view.img);
		    return 0;
		    }
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#define WHEEL_DELTA 120
#endif

/* parent overlapped window */
LRESULT CALLBACK _export
WndImgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
RECT rect;

    if (message == help_message) {
#ifdef USE_HTMLHELP
	TCHAR buf[MAXSTR];
	char strh[MAXSTR];
	char strb[MAXSTR];
	int i;
	for (i=0; help_entry[i].id != 0; i++) {
	    if (nHelpTopic == help_entry[i].id)
		break;
	}
	if (help_entry[i].name != NULL) {
	    lstrcpy(buf, help_entry[i].name);
	    lstrcat(buf, TEXT(".htm"));
	}

	HtmlHelp(hwndimg,szHelpName,HH_DISPLAY_TOPIC,(DWORD)buf);

convert_widechar(strh, szHelpName, sizeof(strh)-1);
convert_widechar(strb, buf, sizeof(strh)-1);
gs_addmessf("HtmlHelp: %s HH_DISPLAY_TOPIC %s\n", strh, strb);
#else
	TCHAR buf[MAXSTR];
	load_string(nHelpTopic, buf, sizeof(buf)/sizeof(TCHAR)-1);
	WinHelp(hwndimg,szHelpName,HELP_KEY,(DWORD)buf);
#endif
	return 0;
    } else
    switch(message) {
	case WM_MOUSEWHEEL:
	    /* If Wheel Mice become available with a step size < WHEEL_DELTA,
	     * this code will need to be rewritten */
	    if (LOWORD(wParam) == MK_SHIFT) {
		/* change page */
		int numpages = 0;
		int skip = HIWORD(wParam);
		if (skip > 0x7fff)
		   skip = skip - 0x10000L;
		skip = -skip / WHEEL_DELTA;
		request_mutex();
		if (psfile.dsc != (CDSC *)NULL)
		    numpages = psfile.dsc->page_count;
		release_mutex();
	    
		if ( (skip < 0)  && 
		     (psfile.dsc != (CDSC *)NULL) && 
		     (psfile.pagenum != 1)) {
			PostMessage(hwnd ,WM_VSCROLL, SB_BOTTOM,0L);
			gs_page_skip(skip);
		}
		else if ( (skip > 0) && 
		    ((psfile.dsc == (CDSC *)NULL)
		    || (psfile.pagenum < numpages)) ) {
			PostMessage(hwnd ,WM_VSCROLL, SB_TOP,0L);
			if (psfile.dsc == (CDSC *)NULL)
			    skip = 1;
			gs_page_skip(skip);
		}
	    }
	    else if (LOWORD(wParam) ==  MK_CONTROL) {
		float scale = 1.0;
		int scroll_increment;
		scroll_increment = HIWORD(wParam);
		if (scroll_increment > 0x7fff)
		    scroll_increment -= 0x10000L;
		scroll_increment = -scroll_increment;
		if (scroll_increment > 0) {
		    while (scroll_increment > 0) {
			scale = (float)(scale * 1.20);
			scroll_increment -= WHEEL_DELTA;
		    }
		    gs_magnify(scale);
		}
		else if (scroll_increment < 0) {
		    while (scroll_increment < 0) {
			scale = (float)(scale/1.20);
			scroll_increment += WHEEL_DELTA;
		    }
		    gs_magnify(scale);
		}
	    }
	    else if (LOWORD(wParam) == MK_LBUTTON) {
		/* horizontal scroll */
		int scroll_increment;
		GetClientRect(hwnd, &rect);
		scroll_increment = HIWORD(wParam);
		if (scroll_increment > 0x7fff)
		    scroll_increment -= 0x10000L;
		scroll_increment = -scroll_increment 
		    * ((rect.right-rect.left)/16)
		    / WHEEL_DELTA;
		PostMessage(hwnd_image, WM_HSCROLL, 
		    MAKELONG(SB_FIND, scroll_increment), 0);
	    }
	    else {
		/* vertical scroll */
		int scroll_increment;
		GetClientRect(hwnd, &rect);
		scroll_increment = HIWORD(wParam);
		if (scroll_increment > 0x7fff)
		    scroll_increment -= 0x10000L;
		scroll_increment = -scroll_increment
		    * ((rect.bottom-rect.top)/16) 
		    / WHEEL_DELTA;
		PostMessage(hwnd_image, WM_VSCROLL, 
		    MAKELONG(SB_FIND, scroll_increment), 0);
	    }
	    return 0;
	case WM_GSV16SPL:
	    hwndspl = (HWND)lParam;	   /* gsv16spl.c window handle */
	    return 0;
	case WM_GSDEVICE:
	    if (fullscreen)	/* don't flash between pages */
		return 0;
	    /* hide window if closed */
	    if (!image.open) {
		ShowWindow(hwndimgchild, SW_HIDE);
		if (fullscreen) 
		    ShowWindow(hwnd_fullscreen, SW_HIDE);
	    }
	    bitmap.changed = TRUE;
	    return 0;
	case WM_GSSIZE:
	    if (image.open) {
		/* bitmap has changed */
		update_scroll_bars();
	    }
	    return 0;
	case WM_GSSYNC:
	    if (debug & DEBUG_GENERAL)
		gs_addmess("WM_GSSYNC\n");
	    hwnd_image = fullscreen ? hwnd_fullscreen : hwndimgchild;
	    if ( fullscreen &&
		(!IsWindowVisible(hwnd_fullscreen) || bitmap.changed) &&
		(image.width > 1) && (image.height > 1)) {
		SetFocus(hwndimg);
		ShowWindow(hwndimgchild, SW_HIDE);
		ShowWindow(hwnd_fullscreen, SW_SHOWNA);
		update_scroll_bars();
		bitmap.changed = FALSE;
	    }
	    else if ( (!IsWindowVisible(hwndimgchild) || bitmap.changed) &&
		(image.width > 1) && (image.height > 1)) {
		ShowWindow(hwndimgchild, SW_SHOWNA);
		update_scroll_bars();
		bitmap.changed = FALSE;
	    }
	    if ( !IsIconic(hwndimg) && !fullscreen) {  /* redraw child window */
		if (image.open) {
		    /* don't erase background - the bitmap will cover it anyway */
		    InvalidateRect(hwnd_image, (LPRECT)NULL, FALSE);
		    UpdateWindow(hwnd_image);
		}
	    }
	    return 0;
	case WM_GSPAGE:
	    if (debug & DEBUG_GENERAL)
		gs_addmess("WM_GSPAGE\n");
	    hwnd_image = fullscreen ? hwnd_fullscreen : hwndimgchild;
	    if ( fullscreen &&
		(!IsWindowVisible(hwnd_fullscreen) || bitmap.changed) &&
		(image.width > 1) && (image.height > 1)) {
		SetFocus(hwndimg);
		ShowWindow(hwndimgchild, SW_HIDE);
		ShowWindow(hwnd_fullscreen, SW_SHOWNA);
		update_scroll_bars();
		bitmap.changed = FALSE;
	    }
	    else if ( (!IsWindowVisible(hwndimgchild) || bitmap.changed) &&
		(image.width > 1) && (image.height > 1)) {
		ShowWindow(hwndimgchild, SW_SHOWNA);
		update_scroll_bars();
		bitmap.changed = FALSE;
	    }
	    /* showpage has just been called */
	    play_sound(SOUND_PAGE);
	    if (IsIconic(hwndimg))    /* useless as an Icon so fix it */
		ShowWindow(hwndimg, SW_SHOWNORMAL);
	    if ( !IsIconic(hwndimg) ) {  /* redraw child window */
		if (image.open) {
		    if (display.show_find) {
			scroll_to_find();
		    }
		    /* don't erase background - the bitmap will cover it anyway */
		    InvalidateRect(hwnd_image, (LPRECT)NULL, FALSE);
		    UpdateWindow(hwnd_image);
		}
	    }
	    info_link();
	    return 0;
	case WM_GSTILE:
	    /* Must post this to the image window, since only it has 
	     * the scroll position
	     */
	    if (debug & DEBUG_GENERAL)
		gs_addmess("WM_GSTILE\n");
	    hwnd_image = fullscreen ? hwnd_fullscreen : hwndimgchild;
	    if ( !IsIconic(hwndimg) && image.open && !fullscreen)
	    	    PostMessage(hwnd_image, WM_GSTILE, wParam, lParam);
	    return 0;
	case WM_GSMESSBOX:
	    /* delayed message box, usually from other thread */
	    {TCHAR buf[MAXSTR];
	    load_string((int)wParam, buf, sizeof(buf)/sizeof(TCHAR));
	    message_box(buf, (int)lParam);
	    }
	    return 0;
	case WM_GSSHOWMESS:
	    /* delayed message box, usually from other thread */
	    /* delayed show message, usually from other thread */
	    gs_showmess();
	    return 0;
	case WM_GSREDISPLAY:
	    PostMessage(hwnd, WM_COMMAND, IDM_REDISPLAY, 0);
	    return 0;
	case WM_GSTITLE:
	    /* update title */
	    request_mutex();
	    if (psfile.name[0] != '\0') {
		TCHAR buf[256];
		TCHAR fname[MAXSTR];
		convert_multibyte(fname, psfile.name, 
		    sizeof(fname)/sizeof(TCHAR));
		GetFileTitle(fname, buf, (WORD)(sizeof(buf)/sizeof(TCHAR)));
		wsprintf(buf+lstrlen(buf), TEXT(" - %s"), szAppName);
		SetWindowText(hwnd, buf);
	    }
	    else
		SetWindowText(hwnd, szAppName);
	    release_mutex();
	    return 0;
	case WM_GSPERCENT:
	    percent_pending = FALSE;
	    InvalidateRect(hwndimg, (LPRECT)&info_rect, FALSE);
	    UpdateWindow(hwndimg);
	    return 0;
	case WM_GSTEXTINDEX:
#ifndef VIEWONLY
	    make_text_index();
	    text_marking = FALSE;
	    text_mark_first = text_mark_last = -1;
#endif
	    return 0;
	case WM_GSWAIT:
	    info_wait(wParam);
	    return 0;
	case WM_ACTIVATE:
	    {
#ifdef WIN32
	     HWND hwnd_activate = (HWND)lParam;
#else
	     HWND hwnd_activate = (HWND)LOWORD(lParam);
#endif
	    /* We seem to be activated whenever fActive is non-zero
	     * and hwnd_activate == NULL
	     */
	    if ( (LOWORD(wParam) != WA_INACTIVE) &&
		 ( (hwnd_activate == (HWND)NULL) || 
		   (hwnd_activate == hwndimg) ||
		   (hwnd_activate == hwndimgchild) ||
		   (hwnd_activate == hwnd_image)
		 )
               )
	        if (dfchanged() != 0)
		    PostMessage(hwnd, WM_COMMAND, IDM_REDISPLAY, 0);
	    }
	    break;
	case WM_CREATE:
	    hwndimg = hwnd;
	    gsview_create();
	    /* Enable Drag Drop */
	    DragAcceptFiles(hwnd, TRUE);
	    break;
	case WM_CLOSE:
	    quitnow = TRUE;		 	/* exit from nested message loops */
	    pending.unload = TRUE;
	    pending.abort = TRUE;
	    if (multithread)
		SetEvent(display.event);	/* unblock display thread */
	    if (gsdll.state != GS_UNINIT)
		return 0;			/* don't close yet */
	    PostQuitMessage(0);
	    break;
	case WM_DESTROY:
	    /* tell GS DLL to unload */
	    quitnow = TRUE;
	    pending.unload = TRUE;

	    /* disable Drag Drop */
	    DragAcceptFiles(hwnd, FALSE);
	    PostQuitMessage(0);			/* redundant */
	    break;
/*
	case WM_ENDSESSION:
	    if (wParam)
		gsview_close();
	    return 0;
*/
	case WM_DROPFILES:
	    {
		LPSTR szFile;
		HGLOBAL hglobal;
		int i, cFiles, length;
		HDROP hdrop = (HDROP)wParam;
		cFiles = DragQueryFileA(hdrop, 0xffffffff, (LPSTR)NULL, 0);
		for (i=0; i<cFiles; i++) {
		    length = DragQueryFileA(hdrop, i, (LPSTR)NULL, 0);
		    hglobal = GlobalAlloc(GHND | GMEM_SHARE, length+1);
		    if (hglobal) {
			    szFile = (LPSTR)GlobalLock(hglobal);
			    DragQueryFileA(hdrop, i, szFile, MAXSTR);
			    GlobalUnlock(hglobal);
			    /* it doesn't work if we call gsview_display directly */
			    PostMessage(hwnd, WM_COMMAND, IDM_DROP, (LPARAM)hglobal);
		    }
		}
		DragFinish(hdrop);
	    }
	    break;
	case WM_INITMENU:
	    if (hmenu == (HMENU)wParam) {
		HMENU hmenuedit = GetSubMenu(hmenu,1);
		BOOL idle;
		BOOL addeps;
	        idle = (gsdll.state != GS_BUSY);
#ifndef VIEWONLY
		enable_menu_item(IDM_EDITMENU, IDM_COPYCLIP, image.open);
		if (OpenClipboard(hwnd)) {
		    enable_menu_item(IDM_EDITMENU, IDM_PASTETO, IsClipboardFormatAvailable(CF_DIB));
		    enable_menu_item(IDM_EDITMENU, IDM_CONVERT, 
			IsClipboardFormatAvailable(CF_DIB) || 
			IsClipboardFormatAvailable(CF_BITMAP));
		    CloseClipboard();
		}

		/* IDM_ADDEPSMENU */
		addeps =  (psfile.dsc != (CDSC *)NULL) && psfile.dsc->epsf && idle;
		if (addeps) {
		    EnableMenuItem(hmenuedit, 5, MF_BYPOSITION | MF_ENABLED);
		}
		else {
		    EnableMenuItem(hmenuedit, 5, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		}
		/* Extract EPS sub menu */
		if ( (psfile.preview == IDS_EPST) || (psfile.preview == IDS_EPSW) && idle)
		    EnableMenuItem(hmenuedit, 6, MF_BYPOSITION | MF_ENABLED);
		else
		    EnableMenuItem(hmenuedit, 6, MF_BYPOSITION | MF_DISABLED | MF_GRAYED);
		enable_menu_item(IDM_ADDEPSMENU, IDM_MAKEEPSU, addeps);
		addeps =  addeps && image.open;
		enable_menu_item(IDM_ADDEPSMENU, IDM_MAKEEPSI, addeps);
		enable_menu_item(IDM_ADDEPSMENU, IDM_MAKEEPST4, addeps);
		enable_menu_item(IDM_ADDEPSMENU, IDM_MAKEEPST6U, addeps);
		enable_menu_item(IDM_ADDEPSMENU, IDM_MAKEEPST6P, addeps);

		enable_menu_item(IDM_EDITMENU, IDM_PSTOEDIT, idle);
		enable_menu_item(IDM_EDITMENU, IDM_TEXTEXTRACT, idle);
		enable_menu_item(IDM_EDITMENU, IDM_TEXTFIND, idle);
		enable_menu_item(IDM_EDITMENU, IDM_TEXTFINDNEXT, idle);
#endif


#ifndef MEMORYFILE
		/* Recent files */
		{   char buf[MAXSTR];
		    int i;
#ifndef VIEWONLY
		    int offset = 15;
#else
		    int offset = 7;
#endif
		    HMENU hmenufile = GetSubMenu(hmenu,0);
		    RemoveMenu(hmenufile, IDM_LASTFILE1, MF_BYCOMMAND);
		    RemoveMenu(hmenufile, IDM_LASTFILE2, MF_BYCOMMAND);
		    RemoveMenu(hmenufile, IDM_LASTFILE3, MF_BYCOMMAND);
		    RemoveMenu(hmenufile, IDM_LASTFILE4, MF_BYCOMMAND);
		    for (i=last_files_count; i>0; i--) {
		        sprintf(buf, "&%d %s", i, last_files[i-1]);
			if (strlen(buf)>64) {
			    int j;
			    for (j=strlen(buf); j>0; j--)
				if ((buf[j] == '/') || (buf[j] == '\\'))
				    break;
			    if (strlen(buf) - j > 28)
				memmove(buf+3, buf+strlen(buf)+1-30, 30);
			    else {
				buf[5] = buf[6] = buf[7] = '.';
				memmove(buf+8, buf+j, strlen(buf)+1-j);
			    }
			}
			InsertMenuA(hmenufile, offset, MF_BYPOSITION | MF_STRING, IDM_LASTFILE1+i-1, buf);
		    }
		}
#endif /* !MEMORYFILE */
		return 0;
	    }
	    break;
	case WM_COMMAND:
	    request_mutex();
	    if (LOWORD(wParam) == IDM_ARGS) {
		GSVIEW_ARGS *pargs = (GSVIEW_ARGS *)lParam;
#ifndef VIEWONLY
		/* delayed processing of command line arguments */
		if (pargs->print || pargs->convert) {
		    make_cwd(pargs->filename);
		    gsview_selectfile(pargs->filename);
		    if (pargs->device[0]) {
			if (pargs->print)
			    strcpy(option.printer_device, pargs->device);
			else
			    strcpy(option.convert_device, pargs->device);
		    }
		    if ((pargs->queue[0]) && pargs->print)
			strcpy(option.printer_queue, pargs->queue);
		    if (dfreopen() != 0) {
			release_mutex();
		        return 0;
		    }
		    if (psfile.name[0] != '\0') {
		        option.print_to_file = FALSE;
		        gsview_print(!pargs->print);
		    }
		    dfclose();
		}
		else if (pargs->spool) {
		    if (*pargs->filename) {
			gs_addmess("Spooling \042");
			gs_addmess(pargs->filename);
			gs_addmess("\042 to printer \042");
			if (pargs->queue)
			    gs_addmess(pargs->queue);
			gs_addmess("\042\n");
			gsview_spool(pargs->filename, pargs->queue);
			gsview_command(IDM_EXIT);
			break;
		    }
		}
		else  
#endif
		if (pargs->filename[0]) {
		    make_cwd(pargs->filename);
		    gsview_displayfile(pargs->filename);
		}
	    }
	    else if (LOWORD(wParam) == IDM_DROP) {
		LPSTR szFile = (LPSTR)GlobalLock((HGLOBAL)lParam);
		make_cwd(szFile);
		gsview_displayfile(szFile);
		GlobalUnlock((HGLOBAL)lParam);
		GlobalFree((HGLOBAL)lParam);
	    }
	    else {
		if (GetNotification(wParam,lParam) != BN_DOUBLECLICKED) {
		    if (hDlgModeless) {
			play_sound(SOUND_ERROR);
			release_mutex();
			return 0;	/* obtaining Bounding Box so ignore commands */
		    }
#ifdef NOTUSED
		    if (gsdll.state == GS_BUSY) {
			/* With DLL, more than the following will be safe */
			switch(LOWORD(wParam)) {
			    case IDM_INFO:
			    case IDM_CLOSE_DONE:
			    case IDM_SAVEDIR:
			    case IDM_SETTINGS:
			    case IDM_SAVESETTINGS:
			    case IDM_SOUNDS:
			    case IDM_HELPCONTENT:
			    case IDM_HELPSEARCH:
			    case IDM_ABOUT:
			    case IDM_GSMESS:
			    case IDM_EXIT:
				/* these are safe to use when busy */
				break;
			    default:
				play_sound(SOUND_ERROR);
				release_mutex();
				return 0;	/* Command not permitted now */
			}
		    }
#endif
		    gsview_command(LOWORD(wParam));
		}
	    }
	    release_mutex();
	    return 0;
	case WM_KEYDOWN:
	case WM_KEYUP:
	    end_button_help();
	    /* pass on key presses so that child window scroll bars work */
	    SendMessage(hwnd_image, message, wParam, lParam);
	    return 0;
	case WM_SIZE:
	    /* make child window fill client area */
	    {
	    int cx, cy;
	    calc_info_button_areas(LOWORD(lParam), HIWORD(lParam));
	    cx = LOWORD(lParam)-img_offset.x;
	    cy = info_rect.top - img_offset.y;
	    if ((wParam != SIZE_MINIMIZED) && hwndimgchild !=(HWND)NULL)
		SetWindowPos(hwndimgchild, (HWND)NULL, 
		    img_offset.x, img_offset.y, cx, cy, 
		    SWP_NOZORDER | SWP_NOACTIVATE);
	    /* save window size for INIFILE */
	    if (wParam == SIZE_RESTORED) {
		    GetWindowRect(hwnd,&rect);
		    option.img_size.x = rect.right-rect.left;
		    option.img_size.y = rect.bottom-rect.top;
	    }
	    if (IsWindowVisible(hwnd))
		option.img_max = (wParam == SIZE_MAXIMIZED);
	    }
	    return 0;
	case WM_MOVE:
	    /* save window position for INIFILE */
	    if (!IsIconic(hwnd) && !IsZoomed(hwnd)) {
		    GetWindowRect(hwnd,&rect);
		    option.img_origin.x = rect.left;
		    option.img_origin.y = rect.top;
	    }
	    return 0;
	case WM_SETCURSOR:
	    /* if waiting, display hourglass cursor over our window */
	    if (szWait[0] != '\0') {
		if (in_child_client_area()) {
		    SetCursor(hcWait);
		    return TRUE;
		}
	    }
	    /* track cursor and display coordinates if in child window */
	    if (image.open) {
		float x, y;
		if (in_child_client_area() || prev_in_child) {
		    /* update coordinate info */
		    HFONT old_hfont;
		    HDC hdc = GetDC(hwnd);
		    if (info_font)
			old_hfont = (HFONT)SelectObject(hdc, info_font);
		    cursorpos_paint(hdc);
		    if (info_font)
			SelectObject(hdc, old_hfont);
		    ReleaseDC(hwnd, hdc);
		}
		prev_in_child = in_child_client_area();
		if (get_cursorpos(&x, &y)) {
		    PDFLINK link;
		    info_link();
		    if (is_link(x, y, &link)) {
			SetCursor(hcHand);
		        return TRUE;
		    }
		}
	    }
	    break;
	case WM_PARENTNOTIFY:
/*
	    if (wParam == WM_LBUTTONDOWN) {
		if (hDlgModeless && in_child_client_area())
		    SendMessage(hDlgModeless, WM_COMMAND, BB_CLICK, lParam);
		return 0;
	    }
*/
	    if (wParam == WM_RBUTTONDOWN) {
		float x, y;
		RECT rect;
		int zwidth, zheight;
		if (hDlgModeless) {
		    play_sound(SOUND_BUSY);
		    break;
		}
		request_mutex();
		GetWindowRect(hwnd_image,&rect);
		if (get_cursorpos(&x, &y)) {
		    int scrollx, scrolly;
		    zoom = !zoom;
		    display.zoom_xoffset = (int)x;
		    display.zoom_yoffset = (int)y;
		    scrollx = bitmap.scrollx;
		    scrolly = bitmap.scrolly;
		    if (rect.right - rect.left > image.width)
		        zwidth = image.width;
		    else
		        zwidth = rect.right - rect.left;
		    if (rect.bottom - rect.top > image.height)
		        zheight = image.height;
		    else
		        zheight = rect.bottom - rect.top;
		    x = (float)((scrollx + zwidth/2)*72.0/option.xdpi);
		    y = (float)(((image.height-1) - 
			(scrolly + zheight/2))*72.0/option.ydpi);
		    transform_point(&x, &y);
		    x = (float)(x * option.xdpi/72.0);
		    y = (float)(y * option.ydpi/72.0);
		    display.zoom_xoffset -= (int)(x*72.0/option.zoom_xdpi);
		    display.zoom_yoffset -= (int)(y*72.0/option.zoom_ydpi);
		}
		else {
		    zoom = FALSE;
		}
		release_mutex();
		PostMessage(hwndimg, WM_COMMAND, IDM_ZOOM, (LPARAM)0);
	    }
	    break;
	case WM_SYSCOLORCHANGE:
	    system_colours();
	    break;
	case WM_PAINT:
	    {
	    HDC hdc;
	    PAINTSTRUCT ps;
	    hdc = BeginPaint(hwnd, &ps);
	    /* draw info area at bottom */
	    info_paint(hwnd, hdc);
	    /* buttons at top */
	    /* draw button background */
	    GetClientRect(hwnd, &rect);
	    rect.top = button_rect.top;
	    rect.left = button_rect.left;
	    rect.bottom = button_rect.bottom;
	    if (rect.bottom - rect.top > 2)
		FillRect(hdc, &rect, hbrush_menu);
	    SelectPen(hdc, hpen_btnshadow);
	    MoveTo(hdc, rect.left, rect.top);
	    LineTo(hdc, rect.right, rect.top);

	    if (rect.bottom - rect.top > 2) {
		MoveTo(hdc, rect.left, rect.bottom);
		LineTo(hdc, rect.right, rect.bottom);
		SelectPen(hdc, hpen_btnhighlight);
		MoveTo(hdc, rect.left, rect.top+1);
		LineTo(hdc, rect.right, rect.top+1);
	    }
	    EndPaint(hwnd, &ps);
	    }
	    return 0;
	case WM_MEASUREITEM:
	    return 1;
	case WM_DRAWITEM:
	    return draw_button((DRAWITEMSTRUCT FAR *)lParam);
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

/* return TRUE if button drawn */
BOOL
draw_button(DRAWITEMSTRUCT FAR *lpdis)
{
HBRUSH hbrush;
HPEN hpen_highlight, hpen_shadow, hpen_old;
HDC hdc = lpdis->hDC;
RECT rect;
HICON hicon;
HBITMAP hbitmap_old, hbitmap;
BITMAP bm;
int i;
TCHAR buf[20];
    rect = lpdis->rcItem;
    if (lpdis->CtlType != ODT_BUTTON)
	return FALSE;
    switch (lpdis->itemAction) {
	case ODA_DRAWENTIRE:
	    if ((hbitmap = LoadBitmap(phInstance,MAKEINTRESOURCE(lpdis->CtlID)))
	      != (HBITMAP)NULL) {
		HDC hdcsrc;
		COLORREF fgColour;
		COLORREF bgColour;
		GetObject(hbitmap, sizeof(BITMAP),&bm);
		if ( (rect.right-rect.left > bm.bmWidth) ||
		     (rect.bottom-rect.top > bm.bmHeight) ) {
		    hbrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		    FillRect(hdc, &rect, hbrush);
		    DeleteBrush(hbrush);
		}
		/* When painting from monochrome bitmap to a colour DC,
		 * the text foreground and background colours are used
		 */
		hdcsrc = CreateCompatibleDC(hdc);
		fgColour = SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));
		bgColour = SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
		hbitmap_old = (HBITMAP)SelectObject(hdcsrc,hbitmap);
		BitBlt(hdc, (rect.left+rect.right-bm.bmWidth)/2,
		   (rect.top+rect.bottom-bm.bmHeight)/2,
		   bm.bmWidth,bm.bmHeight,hdcsrc,0,0,SRCCOPY);
		SetTextColor(hdc, fgColour);
		SetBkColor(hdc, bgColour);
		SelectObject(hdcsrc,hbitmap_old);
		DeleteDC(hdcsrc);
		DeleteObject(hbitmap);
	    }
	    else {
		hbrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
		FillRect(hdc, &rect, hbrush);
		DeleteBrush(hbrush);
		if ((i = LoadString(phInstance, lpdis->CtlID, buf, sizeof(buf)))
		    != 0) {
		    COLORREF text_colour;
		    SIZE sz;
		    GetTextExtentPoint(hdc, buf, i, &sz);
		    text_colour = SetTextColor(hdc, GetSysColor(COLOR_MENUTEXT));
		    SetBkMode(hdc, TRANSPARENT);
		    TextOut(hdc, (rect.left+rect.right-sz.cx)/2,
			(rect.top+rect.bottom-sz.cy)/2, buf, i);
		    SetTextColor(hdc, text_colour);
		}
		else if ( (hicon = LoadIcon(phInstance, MAKEINTRESOURCE(lpdis->CtlID)))
		    != (HICON)NULL )  {
		    DrawIcon(hdc, (rect.left+rect.right-32)/2, 
			(rect.top+rect.bottom-32)/2, hicon);
		    DestroyIcon(hicon);
		}
	    }
	    hpen_old = SelectPen(hdc, hpen_btnshadow);
	    MoveTo(hdc, rect.left, rect.top);
	    LineTo(hdc, rect.right-1, rect.top);
	    LineTo(hdc, rect.right-1, rect.bottom-1);
	    LineTo(hdc, rect.left, rect.bottom-1);
	    LineTo(hdc, rect.left, rect.top-1);
	    SelectPen(hdc, hpen_old);
	    /* fall thru */
	case ODA_FOCUS:
	case ODA_SELECT:
	    if (lpdis->itemState & ODS_SELECTED) {
		hpen_highlight = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
		hpen_shadow = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNFACE));
	    }
	    else {
		hpen_highlight = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
		hpen_shadow = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	    }
	    hpen_old = SelectPen(hdc, hpen_highlight);
	    MoveTo(hdc, rect.left+1, rect.bottom-3);
	    LineTo(hdc, rect.left+1, rect.top+1);
	    LineTo(hdc, rect.right-2, rect.top+1);
	    MoveTo(hdc, rect.right-3, rect.top+2);
	    LineTo(hdc, rect.left+2, rect.top+2);
	    LineTo(hdc, rect.left+2, rect.bottom-4);
	    SelectPen(hdc, hpen_shadow);
	    MoveTo(hdc, rect.left+1, rect.bottom-2);
	    LineTo(hdc, rect.right-2, rect.bottom-2);
	    LineTo(hdc, rect.right-2, rect.top+1);
	    MoveTo(hdc, rect.right-3, rect.top+2);
	    LineTo(hdc, rect.right-3, rect.bottom-3);
	    LineTo(hdc, rect.left+2, rect.bottom-3);
	    SelectPen(hdc, hpen_old);
	    DeleteObject(hpen_highlight);
	    DeleteObject(hpen_shadow);
	    return TRUE;
    }
    return FALSE;
}

/* returns true if cursor in client area of Ghostscript image window */
BOOL
in_child_client_area()
{
RECT rect;
POINT pt;
HWND hwnd;
    GetCursorPos(&pt);
    hwnd = WindowFromPoint(pt);
    if ((hwnd != hwndimg) && !IsChild(hwndimg,hwnd))
	    return 0;
    GetClientRect(hwnd_image, &rect);
    ScreenToClient(hwnd_image, &pt);
    return PtInRect(&rect, pt);
}

/* returns true if cursor in client area of GSview window */
BOOL
in_client_area()
{
RECT rect;
POINT pt;
HWND hwnd;
    GetCursorPos(&pt);
    hwnd = WindowFromPoint(pt);
    if ((hwnd != hwndimg) && !IsChild(hwndimg,hwnd))
	    return 0;
    GetClientRect(hwndimg, &rect);
    ScreenToClient(hwndimg, &pt);
    return PtInRect(&rect, pt);
}

/* returns true if cursor in info area or button area of GSview windows */
BOOL
in_info_area()
{
RECT rect;
POINT pt;
HWND hwnd;
    GetCursorPos(&pt);
    hwnd = WindowFromPoint(pt);
    if ((hwnd != hwndimg) && !IsChild(hwndimg,hwnd))
	    return 0;
    ScreenToClient(hwndimg, &pt);

    GetClientRect(hwndimg, &rect);
    rect.bottom = img_offset.y;
    if (PtInRect(&rect, pt))
	    return TRUE;
    GetClientRect(hwndimg, &rect);
    rect.right = img_offset.x;
    return PtInRect(&rect, pt);
}


/* map from a coordinate in points, to a coordinate in pixels */
/* This is the opposite of the transform part of get_cursorpos */
/* Used when showing bbox */
void
map_pt_to_pixel(float *x, float *y)
{
    if (zoom) {
	/* WARNING - this doesn't cope with EPS Clip */
	*x = (float)((*x - display.zoom_xoffset) * option.zoom_xdpi / 72.0);
	*y = (float)((*y - display.zoom_yoffset) * option.zoom_ydpi / 72.0);
	*x = (float)(*x * 72.0 / option.xdpi);
	*y = (float)(*y * 72.0 / option.ydpi);
	itransform_point(x, y);
	*x = (float)((*x * option.xdpi / 72.0) - 
		bitmap.scrollx + display.offset.x);
	*y = (float)(-(*y * option.ydpi / 72.0) 
		+ (image.height-1 - bitmap.scrolly) 
		+ display.offset.y);
    }
    else {
	int xoffset = (int)(display.xoffset / display.xdpi * 72.0 + 0.5);
	int yoffset = (int)(display.yoffset / display.ydpi * 72.0 + 0.5);
	*x = *x - xoffset;
	*y = *y - yoffset;
	itransform_point(x, y);
	*x = (float)(*x * option.xdpi/72.0
	      - bitmap.scrollx + display.offset.x);
	*y = (float)(-(*y * option.ydpi/72.0)
	      + (image.height-1 - bitmap.scrolly) + display.offset.y);
    }
}

BOOL
get_cursorpos(float *x, float *y)
{
RECT rect;
POINT pt;
    if (image.open) {
	GetClientRect(hwnd_image, &rect);
	GetCursorPos(&pt);
	ScreenToClient(hwnd_image, &pt);
	if (PtInRect(&rect, pt)) {
	    *x = (float)(bitmap.scrollx+pt.x - display.offset.x);
	    *y = (float)(image.height-1 - 	
		(bitmap.scrolly+pt.y) + display.offset.y);
	    transform_cursorpos(x, y);
	    return TRUE;
	}
    }
    return FALSE;
}

void
cursorpos_paint(HDC hdc)
{
/* Not Unicode because wsprintf doesn't support floating point */
float x, y;
char buf[32];
char fmt[32];
COLORREF text_colour;
int digits = option.unitfine ? 2 : 0;
    request_mutex();
    SetBkMode(hdc, TRANSPARENT);
    text_colour = SetTextColor(hdc, GetSysColor(COLOR_MENUTEXT));
    FillRect(hdc, &info_coord, hbrush_menu);
    /* show coordinate */
    if (get_cursorpos(&x, &y)) {
	switch(option.unit) {
	   case IDM_UNITPT:   
	      sprintf(fmt, "%%.%df, %%.%dfpt", digits, digits);
	      sprintf(buf, fmt, x, y);
	      break;
	   case IDM_UNITMM:   
	      sprintf(fmt, "%%.%df, %%.%dfmm", digits, digits);
	      sprintf(buf, fmt, x/72*25.4, y/72*25.4);
	      break;
	   case IDM_UNITINCH:   
	      digits += 1;
	      sprintf(fmt, "%%.%df, %%.%dfin", digits, digits);
	      sprintf(buf, fmt, x/72, y/72);
	      break;
	}
	SetTextAlign(hdc, TA_RIGHT);
	TextOutA(hdc, info_coord.right-1, info_coord.top, buf, strlen(buf));
	SetTextColor(hdc, text_colour);
#ifndef VIEWONLY
	measure_paint(x, y);
#endif
    }
    release_mutex();
}

/* paint brief info area */
void
info_paint(HWND hwnd, HDC hdc)
{
RECT rect;
int i;
TCHAR buf[MAXSTR];
TCHAR fmt[MAXSTR];
TCHAR fname[MAXSTR];
HFONT old_hfont;
CDSC *dsc;
COLORREF text_colour;
    request_mutex();
    dsc = psfile.dsc;
    SetBkMode(hdc, TRANSPARENT);
    text_colour = SetTextColor(hdc, GetSysColor(COLOR_MENUTEXT));
    if (info_font)
	old_hfont = (HFONT)SelectObject(hdc, info_font);
    if (info_rect.bottom) {
	GetClientRect(hwnd, &rect);
	rect.top = info_rect.top;
	rect.left = info_rect.left;
	rect.bottom = info_rect.bottom;
	FillRect(hdc, &rect, hbrush_menu);
	SelectPen(hdc, hpen_btnshadow);
	MoveTo(hdc, rect.left, rect.bottom);
	LineTo(hdc, rect.right, rect.bottom);
	if (is_win4) {
	    SelectPen(hdc, hpen_btnshadow);
	    MoveTo(hdc, rect.left, rect.top);
	    LineTo(hdc, rect.right, rect.top);
	    SelectPen(hdc, hpen_btnhighlight);
	    MoveTo(hdc, rect.left, rect.top+1);
	    LineTo(hdc, rect.right, rect.top+1);
	}
    }
    /* write file information */
    if (psfile.name[0] != '\0') {
	i = load_string(IDS_FILE, buf, sizeof(buf)/sizeof(TCHAR));
	convert_multibyte(fname, psfile.name, sizeof(fname)/sizeof(TCHAR));
	i = GetFileTitle(fname, buf+i, (WORD)(sizeof(buf)/sizeof(TCHAR)-i));
	if (lstrlen(buf) > 36) {
	    memmove(buf+3, buf+lstrlen(buf)+1-32, 32*sizeof(TCHAR));
	    buf[0] = buf[1] = buf[2] = '.';
	}
	TextOut(hdc, info_file.x, info_file.y, buf, lstrlen(buf));
	if (szWait[0] != '\0') {
	    wsprintf(buf, szWait, percent_done);
	    TextOut(hdc, info_page.x, info_page.y, buf, lstrlen(buf));
	}
	else {
	  if (dsc!=(CDSC *)NULL) {
	    int n = map_page(psfile.pagenum - 1);
	    load_string(IDS_PAGEINFO, fmt, sizeof(fmt)/sizeof(TCHAR));
	    if (on_link) {
		if ((on_link_page == 0) && on_link_action[0]) {
		    convert_multibyte(buf, on_link_action, 
			    sizeof(buf)/sizeof(TCHAR)-1);
		}
		else {
		    load_string(IDS_LINKPAGE, fmt, sizeof(fmt)/sizeof(TCHAR));
		    wsprintf(buf, fmt, on_link_page);
		}
	    }
	    else {
		TCHAR lbuf[MAXSTR];
		if (dsc->page_count && dsc->page[n].label)
		    convert_multibyte(lbuf, dsc->page[n].label, 
			    sizeof(lbuf)/sizeof(TCHAR)-1);
		else
		    lstrcpy(lbuf, TEXT(" "));
		wsprintf(buf, fmt, lbuf ,psfile.pagenum, dsc->page_count);
	    }
	    if (zoom)
		load_string(IDS_ZOOMED, buf+lstrlen(buf), 
		sizeof(buf)/sizeof(TCHAR)-lstrlen(buf));
	    TextOut(hdc, info_page.x, info_page.y, buf, lstrlen(buf));
	  }
	  else {
	    if (gsdll.state == GS_IDLE)
		load_string(IDS_NOMORE, buf, sizeof(buf)/sizeof(TCHAR));
	    else {
		 load_string(IDS_PAGE, buf, sizeof(buf)/sizeof(TCHAR));
		wsprintf(buf+i, TEXT("%d"), psfile.pagenum);
	    }
	    TextOut(hdc, info_page.x, info_page.y, buf, lstrlen(buf));
	  }
	  /* show coordinate */
	  cursorpos_paint(hdc);
	}
    }
    else {
	load_string(IDS_NOFILE, buf, sizeof(buf)/sizeof(TCHAR));
	TextOut(hdc, info_file.x, info_file.y, buf, lstrlen(buf));
	if (szWait[0] != '\0') {
	    wsprintf(buf, szWait, percent_done);
	    TextOut(hdc, info_page.x, info_page.y, buf, lstrlen(buf));
	}
    }
    if (info_font)
	SelectObject(hdc, old_hfont);
    SetTextColor(hdc, text_colour);
    release_mutex();
}


HWND hbutton_info;

void
end_button_help(void)
{
    if (hbutton_info) {
	ReleaseCapture();
	hbutton_info = (HWND)NULL;
	SetFocus(hwndimg);
	InvalidateRect(hwndimg, &info_rect, FALSE);
	UpdateWindow(hwndimg);
    }
}

/* subclass button WndProc to give focus back to parent window */
LRESULT CALLBACK _export
MenuButtonProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
	case WM_LBUTTONUP:
	    {
	    RECT rect;
	    POINT pt;
	    end_button_help();
	    GetWindowRect(hwnd, &rect);
	    GetCursorPos(&pt);
	    SetFocus(GetParent(hwnd));
	    if (PtInRect(&rect, pt))
		SendMessage(GetParent(hwnd), WM_COMMAND, GetWindowID(hwnd), 0L);
	    }
	    break;
	case WM_MOUSEMOVE:
	    {
		POINT pt;
	        HDC hdc; 
	        HFONT old_hfont;
		RECT rect;
		HWND hwnd_cursor;
		TCHAR buf[MAXSTR];
		if (GetActiveWindow() != hwndimg)
		    break;  /* ignore if GSview not the top window */
		GetCursorPos(&pt);
		hwnd_cursor = WindowFromPoint(pt);
		if (hwnd != hwnd_cursor) {
		    end_button_help();
		}
		else if (hbutton_info != hwnd) {
		    COLORREF text_colour;
		    hbutton_info = hwnd;
		    SetCapture(hwnd);
		    hdc = GetDC(hwndimg);
		    load_string(GetWindowID(hwnd), buf, sizeof(buf)/sizeof(TCHAR));
		    SetBkMode(hdc, TRANSPARENT);
		    text_colour = SetTextColor(hdc, GetSysColor(COLOR_MENUTEXT));
		    if (info_rect.bottom) {
			GetClientRect(hwnd, &rect);
			rect.top = info_rect.top+2;
			rect.left = info_rect.left;
			rect.bottom = info_rect.bottom-1;
			rect.right = info_rect.right;
			FillRect(hdc, &rect, hbrush_menu);
		    }
		    if (info_font)
			old_hfont = (HFONT)SelectObject(hdc, info_font);
		    TextOut(hdc, info_file.x, info_file.y, buf, lstrlen(buf));
		    if (info_font)
			SelectObject(hdc, old_hfont);
		    SetTextColor(hdc, text_colour);
		    ReleaseDC(hwndimg, hdc);
		}
	    }
	    break;
    }
    return CallWindowProc(lpfnButtonWndProc, hwnd, message, wParam, lParam);
}

void
update_scroll_bars(void)
{
    /* Cause update of scroll bars etc. */
    RECT rect;
    GetClientRect(hwnd_image, &rect);
    SendMessage(hwnd_image, WM_SIZE, SIZE_RESTORED, 
	MAKELONG(rect.right-rect.left, rect.bottom-rect.top));
}

/* Thread which loads Ghostscript DLL for display */
#ifdef __BORLANDC__
#pragma argsused
#endif
void 
gs_thread(void *arg)
{
    while (!quitnow) {
	if (!pending.now)
	    wait_event();
	if (!quitnow)
	    gs_process();
    }

    /* signal that we have finished */
    display.tid = 0;
    post_img_message(WM_QUIT, 0);   /* shut down application */
}


/* Return TRUE if is OK to exit */
BOOL
query_close(void)
{
    /* tell GS DLL to unload */
    quitnow = TRUE;
    pending.unload = TRUE;
    if (multithread)
        SetEvent(display.event);	/* unblock display thread */
    return TRUE;
}


/* remove temporary files etc. */
void
gsview_close()
{
    psfile_free(&psfile);
    pdf_free_link();
    if (option.settings && !print_silent)
	write_profile(); 
    else
	write_profile_last_files();	/* always save MRU files */
    SetCursor(GetClassCursor((HWND)NULL));
    if (hFontAnsi)
	DeleteObject(hFontAnsi);
    if (hFontGreek)
	DeleteObject(hFontGreek);
    if (hFontEastEurope)
	DeleteObject(hFontEastEurope);
    if (hcCrossHair)
	DestroyCursor(hcCrossHair);
    if (hcHand)
	DestroyCursor(hcHand);
    if (multithread) {
	CloseHandle(display.event);
	CloseHandle(hmutex_ps);
	CloseHandle(image.hmutex);
	DeleteCriticalSection(&crit_sec);
    }
#ifndef VIEWONLY
    unload_zlib();
    unload_bzip2();
    unload_pstoedit();
#endif
    if (hpen_btnshadow)
	DeletePen(hpen_btnshadow);
    if (hpen_btnhighlight)
	DeletePen(hpen_btnhighlight);
    if (hbrush_window)
	DeleteBrush(hbrush_window);
    if (hbrush_menu)
	DeleteBrush(hbrush_menu);
    language_free();
    return;
}


#ifndef VIEWONLY
void
copy_clipboard()
{
HGLOBAL hglobal;
HPALETTE hpalette;
    if ( text_index && (text_mark_first != -1) && (text_mark_last != -1)) {
	/* copy text, not image */
	int first, last, line;
	int length;
	int i;
	LPSTR data;
	first = text_mark_first;
	last = text_mark_last;
	if (first > last) {
	    first = text_mark_last;
	    last = text_mark_first;
	}
	line = text_index[first].line;
	length = 1;
	for (i=first; i<=last; i++) {
	    if (text_index[i].line != line) {
	        line = text_index[i].line;
		length += 2;
	    }
	    length += strlen( text_words + text_index[i].word ) + 1;
	}
	if ((hglobal = GlobalAlloc(GHND | GMEM_SHARE, length)) == (HGLOBAL)NULL) {
	    message_box(TEXT("out of memory"), 0);
	    return;
	}
	if ((data = (LPSTR)GlobalLock(hglobal)) == (LPSTR)NULL) {
	    message_box(TEXT("out of memory"), 0);
	    return;
	}
	line = text_index[first].line;
	for (i=first; i<=last; i++) {
	    if (text_index[i].line != line) {
	        line = text_index[i].line;
		strcpy(data, "\r\n");
		data += strlen(data);
	    }
	    strcpy(data, text_words + text_index[i].word);
	    strcat(data, " ");
	    data += strlen(data);
	}
	GlobalUnlock(hglobal);
	OpenClipboard(hwndimg);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hglobal);
	CloseClipboard();
	return;
    }

    image_lock(view.img);
    if (image.open) {
	hglobal = image_copy_dib(&image);
	hpalette = image_create_palette(&image);
	if (hglobal == (HGLOBAL)NULL) {
	    MessageBox(hwndimg, TEXT("Not enough memory to Copy to Clipboard"), 
		szAppName, MB_OK | MB_ICONEXCLAMATION);
	    return;
	}
	OpenClipboard(hwndimg);
	EmptyClipboard();
	SetClipboardData(CF_DIB, hglobal);
	if (hpalette)
	    SetClipboardData(CF_PALETTE, hpalette);
	CloseClipboard();
    }
    image_unlock(view.img);
}
#endif


#ifdef __BORLANDC__
#pragma argsused
#endif
/* enable or disable a menu item */
void
enable_menu_item(int menuid, int itemid, BOOL enabled)
{
	if (enabled)
	    EnableMenuItem(hmenu, itemid, MF_ENABLED);
	else
	    EnableMenuItem(hmenu, itemid, MF_DISABLED | MF_GRAYED);
}

/* if found word is not visible, scroll window to make it visible */
void
scroll_to_find(void)
{
    RECT rect, rect_client;
    float x, y;

    request_mutex();
    SendMessage(hwnd_image, WM_SETREDRAW, FALSE, 0);
    /* first translate found box to window coordinates */
    x = (float)psfile.text_bbox.llx;
    y = (float)psfile.text_bbox.lly;
    map_pt_to_pixel(&x, &y);
    rect.left   = (int)x;
    rect.bottom = (int)y;
    x = (float)psfile.text_bbox.urx;
    y = (float)psfile.text_bbox.ury;
    map_pt_to_pixel(&x, &y);
    rect.right  = (int)x;
    rect.top    = (int)y;

    GetClientRect(hwnd_image, &rect_client);

    /* scroll to bring the middle left to the centre of the window */
    if ((rect.left < rect_client.left) || (rect.right > rect_client.right))
	PostMessage(hwnd_image, WM_HSCROLL, MAKELONG(SB_FIND, rect.left - ((rect_client.right-rect_client.left)/2)), 0);

    if ((rect.top < rect_client.top) || (rect.bottom > rect_client.bottom))
	PostMessage(hwnd_image, WM_VSCROLL, MAKELONG(SB_FIND, (rect.bottom+rect.top - rect_client.bottom-rect_client.top)/2), 0);
    SendMessage(hwnd_image, WM_SETREDRAW, TRUE, 0);
    release_mutex();
}


#ifndef VIEWONLY
/* highlight words from first to last inclusive */
/* first may be > last */
/* word = -1 means nothing to mark */
void
highlight_words(HDC hdc, int first, int last)
{
    RECT rect;
    float x, y;
    TEXTINDEX *text;
    int i;
    if ((first == -1) || (last == -1))
	return;


    if ((first > (int)text_index_count) || (last > (int)text_index_count)) {
	gs_addmess("\nhighlight_words called with invalid arguments\n");
	return;
    }
    if (first > last) {
        int temp = first;
	first = last;
	last = temp;
    }

    for (i = first; i<=last; i++) {
	text = &text_index[i];
	/* highlight found word */
	/* map bounding box to device coordinates */
	x = (float)text->bbox.llx;
	y = (float)text->bbox.lly;
	map_pt_to_pixel(&x, &y);
	rect.left   = (int)x;
	rect.bottom = (int)y;
	x = (float)text->bbox.urx;
	y = (float)text->bbox.ury;
	map_pt_to_pixel(&x, &y);
	rect.right  = (int)x;
	rect.top    = (int)y;
	if (rect.top > rect.bottom) {
	    int temp = rect.top;
	    rect.top = rect.bottom;
	    rect.bottom = temp;
	}
	if (rect.left > rect.right) {
	    int temp = rect.right;
	    rect.right = rect.left;
	    rect.left = temp;
	}

	/* invert text */
	InvertRect(hdc, &rect);
    }
}
#endif


void
highlight_links(HDC hdc)
{
PDFLINK link;
int i = 0;
float x, y;
RECT rect;
LOGBRUSH lb;
HBRUSH hbrush, hbrush_old;
HPEN hpen, hpen_old;
int w2;
    
    while ( pdf_get_link(i, &link) ) {
	i++;
	if (link.border_width) {
	    /* map bounding box to device coordinates */
	    x = (float)link.bbox.llx;
	    y = (float)link.bbox.lly;
	    map_pt_to_pixel(&x, &y);
	    rect.left   = (int)x;
	    rect.bottom = (int)y;
	    x = (float)link.bbox.urx;
	    y = (float)link.bbox.ury;
	    map_pt_to_pixel(&x, &y);
	    rect.right  = (int)x;
	    rect.top    = (int)y;
	    if (rect.top > rect.bottom) {
		int temp = rect.top;
		rect.top = rect.bottom;
		rect.bottom = temp;
	    }
	    if (rect.left > rect.right) {
		int temp = rect.right;
		rect.right = rect.left;
		rect.left = temp;
	    }
	    /* draw border */
	    SetROP2(hdc, R2_COPYPEN);
	    if (link.colour_valid) {
	        hpen = CreatePen(PS_SOLID, (int)(link.border_width+0.5), 
		    RGB((int)(link.colour_red*255 +0.5),
		        (int)(link.colour_green*255 +0.5),
			(int)(link.colour_blue*255 +0.5)));
	    }
	    else {
	        hpen = CreatePen(PS_SOLID, (int)(link.border_width+0.5), RGB(0,255,255));
	        SetROP2(hdc, R2_XORPEN);
	    }
	    hpen_old = SelectPen(hdc, hpen);
	    SelectPen(hdc, hpen);
	    lb.lbStyle = BS_NULL;	/* hollow = transparent */
	    lb.lbColor = 0;		/* ignored */
	    lb.lbHatch = 0;		/* ignored */
	    hbrush = CreateBrushIndirect(&lb);
	    hbrush_old = SelectBrush(hdc, hbrush);
	    w2 = (int)((link.border_width+0.5)/2);
	    RoundRect(hdc, rect.left-w2, rect.top-w2, 
		    rect.right+w2, rect.bottom+w2, 
		    2 * ((int)(link.border_xr+0.5)),
		    2 * ((int)(link.border_yr+0.5)));
	    SelectBrush(hdc, hbrush_old);
	    DeleteBrush(hbrush);
	    SelectPen(hdc, hpen_old);
	    DeletePen(hpen);
	}
    }
}


void
info_link(void)
{
float x, y;
PDFLINK link;
HFONT old_hfont;
HDC hdc;
    if (get_cursorpos(&x, &y)) {
	if (is_link(x, y, &link)) {
	    on_link = TRUE;
	    on_link_page = link.page;
	    on_link_action = link.action;
	    hdc = GetDC(hwndimg);
	    if (info_font)
		old_hfont = (HFONT)SelectObject(hdc, info_font);
	    info_paint(hwndimg, hdc);
	    if (info_font)
		SelectObject(hdc, old_hfont);
	    ReleaseDC(hwndimg, hdc);
	}
	else if (on_link)
	{
	    on_link = FALSE;
	    hdc = GetDC(hwndimg);
	    if (info_font)
		old_hfont = (HFONT)SelectObject(hdc, info_font);
	    info_paint(hwndimg, hdc);
	    if (info_font)
		SelectObject(hdc, old_hfont);
	    ReleaseDC(hwndimg, hdc);
	}
    }
}

void
gsview_fullscreen_end(void)
{
    if (fullscreen) {
	gs_addmess("Full Screen ending\r\n");
        DestroyWindow(hwnd_fullscreen);
	hwnd_fullscreen = (HWND)NULL;
	hwnd_image = hwndimgchild;
    }
}

void
gsview_fullscreen(void)
{
WNDCLASS wndclass;
int width, height;
TCHAR class_name[MAXSTR];
static BOOL class_registered;
DISPLAY_INFO *di;
int ndisp;
RECT rect;

	if (!image.open)
	    return;
        if (!IsWindow(hwnd_fullscreen)) {
	    lstrcpy(class_name, szAppName);
	    lstrcat(class_name, TEXT("_fullscreen"));
	    if (!class_registered) {
		/* register the window class */
		wndclass.style = 0;
		wndclass.lpfnWndProc = WndImgChildProc;
		wndclass.cbClsExtra = 0;
		wndclass.cbWndExtra = 0;
		wndclass.hInstance = phInstance;
		wndclass.hIcon = NULL;
		wndclass.hCursor = NULL;
		hCursorArrow = LoadCursor((HINSTANCE)NULL, IDC_ARROW);
		wndclass.hbrBackground =  (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = class_name;
		RegisterClass(&wndclass);
		class_registered = TRUE;
	    }

/* OLD */
/*
	    width = GetSystemMetrics(SM_CXSCREEN);
	    height = GetSystemMetrics(SM_CYSCREEN);
*/
	    /* Put full screen on current display */
	    GetWindowRect(hwndimg, &rect);
	    ndisp = find_display((rect.left+rect.right)/2, 
		(rect.top+rect.bottom)/2);
	    if (ndisp < 0)
		ndisp = find_display(rect.left, rect.top);
	    if (ndisp < 0)
		ndisp = 0;
 	    di = &win_display[ndisp];

	    fullscreen = TRUE;
	    hwnd_fullscreen = CreateWindow(class_name, szAppName,
		      WS_POPUP,
		      di->left, di->top, 
		      di->width, di->height,
		      NULL /* parent = desktop */, 
		      NULL, phInstance, (void FAR *)NULL);

	    if (hwnd_fullscreen && IsWindow(hwnd_fullscreen)) {
		hwnd_image = hwnd_fullscreen;
		ShowWindow(hwnd_fullscreen, SW_SHOWNORMAL);
		BringWindowToTop(hwnd_fullscreen);
		gs_addmess("Full Screen started\r\n");
	    }
	    else {
		gs_addmess("Full Screen failed\r\n");
		fullscreen = FALSE;
	    }
        }
	else {
	    hwnd_image = hwnd_fullscreen;
	    ShowWindow(hwnd_fullscreen, SW_SHOWNORMAL);
	    SetFocus(hwndimg);
	    BringWindowToTop(hwnd_fullscreen);
	    gs_addmess("Full Screen restarted\r\n");
	}

	return;
}


/* Set the current resolution to fill the window.
 * If neither width nor height match, fit whole page
 * into window.  If either width or height match
 * the window size, fit the height or width respectively.
 */
void 
gsview_fitwin(void) 
{
RECT rect;
int width, height;
float dpi, xdpi, ydpi, xdpi2, ydpi2;
    if (psfile.ispdf) {
	CDSCBBOX *mediabox = NULL;
	CDSCBBOX *cropbox = NULL;
        int page = psfile.pagenum;
	if ((page > 0) && (page <= (int)psfile.dsc->page_count))  {
	    if (psfile.dsc->page[page-1].media)
		mediabox = psfile.dsc->page[page-1].media->mediabox;
	    cropbox = psfile.dsc->page[page-1].bbox;
	}
	if (option.epsf_clip && (cropbox != (CDSCBBOX *)NULL)) {
	    width = cropbox->urx - cropbox->llx;
	    height = cropbox->ury - cropbox->lly;
	}
	else {
	    if (mediabox) {
		width = mediabox->urx - mediabox->llx;
		height = mediabox->ury - mediabox->lly;
	    }
	    else {
		width = get_paper_width();
		height = get_paper_height();
	    }
	}
    }
    else {
	width = get_paper_width();
	height = get_paper_height();
    }

    if (display.orientation & 1) {
	/* page is rotated 90 degrees */
	int temp = width;
	width = height;
	height = temp;
    }


    if (fullscreen)
	GetClientRect(hwnd_image, &rect);
    else {
	/* get size including scroll bars area */
	GetClientRect(hwndimg, &rect);
	rect.left += img_offset.x;
	rect.top += img_offset.y;
	/* substract height of status bar */
	rect.bottom -= (info_rect.bottom - info_rect.top);
    }
    xdpi = (float)((rect.right - rect.left) * 72.0 / width);
    ydpi = (float)((rect.bottom - rect.top) * 72.0 / height);
    if (fullscreen) {
	xdpi2 = xdpi;
	ydpi2 = ydpi;
    }
    else {
	/* These are the resolutions allowing for a scroll bar */
	xdpi2 = (float)((rect.right - rect.left - 
	    GetSystemMetrics(SM_CXVSCROLL)) * 72.0 / width);
	ydpi2 = (float)((rect.bottom - rect.top - 
	    GetSystemMetrics(SM_CYHSCROLL)) * 72.0 / height);
    }

    if (display.orientation & 1) {
	/* page is rotated 90 degrees */
	float ftemp;
	ftemp = xdpi;
	xdpi = ydpi;
	ydpi = ftemp;
	ftemp = xdpi2;
	xdpi2 = ydpi2;
	ydpi2 = ftemp;
    }

    if ( ((xdpi + 0.5) > option.xdpi) && (xdpi - 0.5) < option.xdpi) {
	/* Width matches. Set size based on height. */
	if (fullscreen || (ydpi <= xdpi))
	    dpi = ydpi;
	else
	    dpi = ydpi2;
    }
    else if ( ((ydpi + 0.5) > option.ydpi) && (ydpi - 0.5) < option.ydpi) {
	/* Height matches. Set size based on width. */
	if (fullscreen || (xdpi <= ydpi))
	    dpi = xdpi;
	else
	    dpi = xdpi2;
    }
    else  {
	/* Neither width nor height match.  Fit the whole page. */
	if (xdpi > ydpi)
		dpi = ydpi;
	else
		dpi = xdpi;
    }
#ifdef DEBUG
    {
    char buf[MAXSTR];
    sprintf(buf, "\nrect=%d %d %d %d\n", 
    rect.left, rect.top, rect.right, rect.bottom);
    gs_addmess(buf);
    sprintf(buf, "size=%d %d\n", width, height);
    gs_addmess(buf);
    sprintf(buf, "old dpi=%f %f\n", option.xdpi, option.ydpi);
    gs_addmess(buf);
    sprintf(buf, "dpi=%f %f %f %f\n", xdpi, ydpi, xdpi2, ydpi2);
    gs_addmess(buf);
    sprintf(buf, "final dpi=%f\n", dpi);
    gs_addmess(buf);
    }
#endif
    option.xdpi = option.ydpi = dpi;
    gs_resize();
}

/* Split up cmdline into argc/argv */
/* cmdline must be a buffer that can be modified */
int
win_get_args(int *argc, char **argv, int maxarg, char *cmdline)
{
    int i;
    if (maxarg < 1)
	return -1;

    /* skip over program name */
    argv[0] = cmdline;
    while (*cmdline && *cmdline != ' ') {
	if (*cmdline == '\042') {
	    /* skip over name, until closing quote */
	    cmdline++;
	    while (*cmdline && (*cmdline != '\042'))
		cmdline++;
	    if (*cmdline)
		cmdline++;
	}
	else 
	    cmdline++;
    }
    if (*cmdline == ' ')
	*cmdline++ = '\0';
    while (*cmdline == ' ')
	cmdline++;

    /* now grab individual arguments */
    for (i = 1; i < maxarg; i++) {
	argv[i] = cmdline;
	if (*cmdline == '\042') {
	    /* skip over name, until closing quote */
	    cmdline++;
	    argv[i] = cmdline;
	    while (*cmdline && (*cmdline != '\042'))
		cmdline++;
	    if (*cmdline)
		*cmdline++ = '\0';
	}
	else {
            while (*cmdline && *cmdline != ' ')
		cmdline++;
	}
	if (*cmdline == ' ')
	    *cmdline++ = '\0';
	while (*cmdline == ' ')
	    cmdline++;
	if (*cmdline == '\0')
	    break;
    }

    *argc = i+1;
    return 0;
}

