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

/* gvwin.h */
/* Header includes for Windows GSview */

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <commdlg.h>
#include <shellapi.h>
#include <mmsystem.h>
#define USE_HTMLHELP /* FIX */
#ifdef USE_HTMLHELP
#include <htmlhelp.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _MSC_VER
#include <direct.h>
#define _export
#else
#include <dir.h>
#include <dirent.h>
#endif
#include <sys/stat.h>
#include <io.h>
#include <time.h>
#include <process.h>
#include "gsvver.h"
#include "gvcrc.h"
#ifdef _MSC_VER
#define DLLEXPORT __declspec(dllimport)
#define GSDLLEXPORT __declspec(dllimport)
#else
#define DLLEXPORT _import
#define GSDLLEXPORT _export
#endif

#ifndef RC_INVOKED

#define P0() void
#define P1(t1) t1
#define P2(t1,t2) t1,t2
#define P3(t1,t2,t3) t1,t2,t3
#define P4(t1,t2,t3,t4) t1,t2,t3,t4
#define P5(t1,t2,t3,t4,t5) t1,t2,t3,t4,t5
#define P6(t1,t2,t3,t4,t5,t6) t1,t2,t3,t4,t5,t6

#include "errors.h"
#include "iapi.h"
#include "gdevdsp.h"

#include "dscparse.h"
#include "gvcfile.h"

#ifndef NODEBUG_MALLOC
void * debug_malloc(size_t size);
void  * debug_realloc(void *block, size_t size);
void debug_free(void *block);
void debug_memory_report(void);
#define malloc(size) debug_malloc(size)
#define calloc(nitems, size) debug_calloc(nitems, size)
#define realloc(block, size) debug_realloc(block, size)
#define free(block) debug_free(block)
extern long allocated_memory;
#endif

#ifdef DEBUG_MALLOC
extern FILE *malloc_file;
#endif

/* for gsv16spl.exe 16-bit spooler interface for Win32s */
#define WM_GSV16SPL WM_USER+1
extern HWND hwndspl;	/* window handle of gsv16spl.exe */

#define PROFILE_SIZE 2048
#define MAXSTR 256	/* maximum file name length and general string length */
#define DEVICENAME "display"
#ifdef _WIN64
#define DEFAULT_GSCOMMAND "gswin64.exe"
#else
#define DEFAULT_GSCOMMAND "gswin32.exe"
#endif
#define DEFAULT_RESOLUTION 96.0
#define DEFAULT_ZOOMRES 300.0
#define INISECTION "Options"
#define DEVSECTION "Devices"
#define CONVERTSECTION "Convert"
#define EOLSTR "\r\n"
#define COPY_BUF_SIZE 4096
#define PATHSEP "\\"

#include "gvceps.h"
#include "gvcprf.h"
#include "gvctype.h"

/* program details */
typedef struct tagPROG {
    BOOL	valid;
    HINSTANCE	hinst;
} PROG;

typedef struct tagPRINTER {
	PROG	prog;		/* Ghostscript program doing printing */
	char	optname[MAXSTR];/* file storing command line options */
	char	psname[MAXSTR]; /* file storing temporary postscript */
} PRINTER;

/* bitmap details */
typedef struct tagBM {
    int		scrollx;
    int		scrolly;
    BOOL	changed;	/* if width or height changed by GS */
} BMAP;

typedef struct tagDISPLAY {
	int	width;		/* in pixels */
	int	height;
	int	planes;
	int	bitcount;
	BOOL	show_find;	/* highlight found text */
	BOOL	epsf_clipped;	/* display is clipped to bbox */
	int	zoom_xoffset;	/* in 1/72" */
	int	zoom_yoffset;
	POINT	offset;		/* offset of child window */
	float	xdpi;
	float	ydpi;
	int	xoffset;	/* offset of page in pixels */
	int	yoffset;
	int	orientation;	/* 0-3 = 0, 90, 180 or 270 degrees */
	BOOL	init;		/* viewer initialised */
	BOOL	saved;		/* interpreter state saved */
	BOOL	need_header;
	BOOL	need_trailer;
	HANDLE	event;
	unsigned long tid;
} DISPLAY;

/* main structure with info about the GS DLL */
#include "cdll.h"
#include "cimg.h"
#include "cview.h"

/* all the external DLL use "C", not C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* for pstotext DLL */
typedef int (GSDLLAPI *PFN_pstotextInit)(void **instance);
typedef int (GSDLLAPI *PFN_pstotextFilter)(void *instance, char *instr, 
    char **pre, char **word, char **post,
    int *llx, int *lly, int *urx, int *ury);
typedef int (GSDLLAPI *PFN_pstotextExit)(void *instance);
typedef int (GSDLLAPI *PFN_pstotextSetCork)(void *instance, int value);
extern HMODULE pstotextModule;
extern FILE *pstotextOutfile;
extern void *pstotextInstance;
extern PFN_pstotextInit pstotextInit;
extern PFN_pstotextFilter pstotextFilter;
extern PFN_pstotextExit pstotextExit;
extern PFN_pstotextSetCork pstotextSetCork;
extern char pstotextLine[2048];
extern int pstotextCount;


/* for zlib gunzip decompression */
extern HINSTANCE zlib_hinstance;
typedef void *gzFile ;
typedef gzFile (*PFN_gzopen)(const char *path, const char *mode);
typedef int (*PFN_gzread)(gzFile file, void *buf, unsigned len);
typedef int (*PFN_gzclose)(gzFile file);
extern PFN_gzopen gzopen;
extern PFN_gzread gzread;
extern PFN_gzclose gzclose;

/* for bzip2 decompression */
extern HINSTANCE bzip2_hinstance;
typedef void *bzFile ;
typedef bzFile (WINAPI *PFN_bzopen)(const char *path, const char *mode);
typedef int (WINAPI *PFN_bzread)(bzFile file, void *buf, unsigned len);
typedef int (WINAPI *PFN_bzclose)(bzFile file);
extern PFN_bzopen bzopen;
extern PFN_bzread bzread;
extern PFN_bzclose bzclose;

#ifdef __cplusplus
}
#endif


extern BOOL print_silent;	/* /P or /F command line option used */
extern BOOL print_exit;		/* exit on completion of printing */
extern int print_count;		/* number of current print jobs */
				/* It is safe to exit GSview when this is 0 */
extern int portable_app;	/* /A command line option used */

extern int debug;		/* /D command line option used */
extern FILE *debug_file;	/* for gs input logging */

#define SOUND_PAGE 0
#define SOUND_NOPAGE 1
#define SOUND_NONUMBER 2
#define SOUND_NOTOPEN 3
#define SOUND_ERROR 4
#define SOUND_START 5
#define SOUND_EXIT 6
#define SOUND_BUSY 7
#define NUMSOUND 8
struct sound_s {
	char *entry;		/* profile entry */
	int title;		/* Resource string */
	TCHAR file[MAXSTR];	/* empty, "beep", or .WAV sound to play */
};
extern struct sound_s sound[NUMSOUND];
#define BEEP "beep"		/* profile entry for a speaker beep */
typedef BOOL (WINAPI *FPSPS)(LPCTSTR, UINT);
extern HINSTANCE hlib_mmsystem;	/* DLL containing sndPlaySound function */
extern FPSPS lpfnSndPlaySound;	/* pointer to sndPlaySound function if loaded */

extern const TCHAR szClassName[];
extern const TCHAR szImgClassName[];
extern const char szScratch[];  /* temporary filename prefix */
extern char *szSpoolPrefix;	/* usually \\spool\ */
extern TCHAR szAppName[MAXSTR];
extern int nHelpTopic;
extern TCHAR szWait[MAXSTR];
extern TCHAR szExePath[MAXSTR];
extern char szIniFile[MAXSTR];
extern char szFindText[MAXSTR];
extern TCHAR szHelpName[MAXSTR];		/* buffer for building help filename */
extern char previous_filename[MAXSTR];	/* to remember name between file dlg boxes */
extern char selectname[MAXSTR];		/* for IDM_SELECT */
extern UINT help_message;		/* message sent by OFN_SHOWHELP */
extern GSVIEW_ARGS args;		/* Parsed arguments */
extern HWND hwndimg;			/* gsview main window */
extern HWND hDlgModeless;		/* any modeless dialog box */
extern HWND hwnd_measure;		/* measure modeless dialog box */
extern HWND hwndimgchild;		/* gswin image child window */
extern HWND hwnd_fullscreen;		/* full screen popup of child window */
extern HWND hwnd_image;			/* full screen or image child window */	
extern HINSTANCE phInstance;		/* instance of gsview */
extern HINSTANCE hlanguage;		/* instance of language resources */
extern UINT nCodePageLanguage;		/* Code page of the GSview language */
extern UINT nCodePageSystem;		/* Code page of the system */
extern BOOL is_win31;			/* To allow selective use of win 3.1 features */
extern BOOL is_winnt;			/* To allow selective use of Windows NT features */
extern BOOL is_win95;			/* To allow selective use of Windows 95 features */
extern BOOL is_win98;			/* To allow selective use of Windows 98 features */
extern BOOL is_win32s;			/* To allow selective use of Win32s misfeatures */
extern BOOL is_win4;			/* To allow selective use of Windows 4.0 features */
extern BOOL multithread;		/* TRUE if running multithreaded */
extern CRITICAL_SECTION crit_sec;	/* for thread synchronization */
extern HANDLE hmutex_ps;		/* for protecting psfile and pending */
extern HMENU hmenu;			/* main menu */
extern HACCEL haccel;			/* menu accelerators */
extern HCURSOR hcWait;
extern HCURSOR hcCrossHair;
extern HCURSOR hcHand;
extern HPEN hpen_btnshadow;		/* button shadow */
extern HPEN hpen_btnhighlight;		/* button highlight */
extern HBRUSH hbrush_window;		/* Window background */
extern HBRUSH hbrush_menu;		/* menu background */
extern int bitmap_scrollx;	/* offset from bitmap to origin of child window */
extern int bitmap_scrolly;
extern HFONT info_font;
extern HFONT hFontAnsi;			/* ANSI (Western European) font */
extern HFONT hFontGreek;		/* cp 1253 / Greek font */
extern HFONT hFontEastEurope;		/* ISO-Latin2 / cp1250 font */
extern HFONT hFontCyrillic;		/* ISO-Latin5 / cp1251 font */
extern POINT img_offset;		/* offset to gswin child window */
extern POINT info_file;		/* position of file information */
extern POINT info_page;		/* position of page information */
extern RECT  info_rect;		/* position and size of brief info area */
extern RECT  info_coord;		/* position and size of coordinate information */
extern RECT  button_rect;		/* position and size of button area */
extern int on_link;			/* TRUE if we were or are over link */
extern int on_link_page;		/* page number of link target */
extern long gsbytes_size;		/* number of bytes for this page */
extern long gsbytes_done;		/* number of byte written */
extern OPENFILENAME ofn;
extern WNDPROC lpfnButtonWndProc;
extern BOOL quitnow;	/* Used to cause exit from nested message loops */
extern int percent_done;		/* percentage of document processed */
extern int percent_pending;		/* TRUE if WM_GSPERCENT is pending */
extern BOOL fit_page_enabled;		/* next WM_SIZE is allowed to resize window */

extern PROG gsprog;
extern BMAP bitmap;
extern OPTIONS option;
extern PSFILE psfile;
extern PRINTER printer;
extern BOOL win32s_printer_pending;
extern DISPLAY display;
/* extern PSDOC *doc;	/* points to psfile->doc */
extern int page_skip;
extern BOOL changed_version;
extern BOOL zoom;

extern PSBBOX bbox;

extern TCHAR registration_name[MAXSTR];
extern unsigned int registration_receipt;

/* Display dialog boxes on the first display, instead of centred.
 * Display full screen on the last display.
 * So we need to find out where the display are located...
 */
typedef struct tagDISPLAY_INFO {
    int left;
    int top;
    int width;
    int height;
} DISPLAY_INFO;
extern int number_of_displays; /* number of active displays */
extern DISPLAY_INFO win_display[4];	/* active displays */

/* PRINT_GDI */
extern int print_gdi_width;
extern int print_gdi_height;
extern int print_gdi_xdpi;
extern int print_gdi_ydpi;
extern HANDLE print_gdi_read_handle;
extern HANDLE print_gdi_write_handle;

#define MoveTo(hdc,x,y) MoveToEx((hdc),(x),(y),(LPPOINT)NULL)
#define SetWindowOrg(hdc, x, y) SetWindowOrgEx(hdc, x, y, (LPPOINT)NULL)
#define	SetWindowExt(hdc, x, y) SetWindowExtEx(hdc, x, y, (LPSIZE)NULL)
#define GetNotification(wParam,lParam) (HIWORD(wParam))
#define SendDlgNotification(hwnd, id, notice) \
    SendMessage((hwnd), WM_COMMAND, MAKELONG((id),(notice)), (LPARAM)GetDlgItem((hwnd),(id)))
#ifdef _WIN64
#define SetClassCursor(hwnd, hcursor) SetClassLongPtr((hwnd), GCLP_HCURSOR, (LONG)(hcursor))
#define GetClassCursor(hwnd) ((HCURSOR)GetClassLongPtr((hwnd), GCLP_HCURSOR))
#else
#define SetClassCursor(hwnd, hcursor) SetClassLong((hwnd), GCL_HCURSOR, (LONG)(hcursor))
#define GetClassCursor(hwnd) ((HCURSOR)GetClassLong((hwnd), GCL_HCURSOR))
#define unlink(s) _unlink(s)
#endif

/* Dialogs have different return types in Win32 and Win64 */
#ifdef _WIN64
#define DLGRETURN INT_PTR
#else
#define DLGRETURN BOOL
#endif


int load_string_a(int id, LPSTR str, int len);
int message_box_a(LPCSTR str, int icon);
int convert_multibyte(LPTSTR str, LPCSTR mbstr, int len);
int convert_widechar(LPSTR mbstr, LPCTSTR str, int len);

#ifdef UNICODE
#define MessageBoxL MessageBoxW
#define DialogBoxParamL DialogBoxParamW
#define CreateDialogParamL CreateDialogParamW
#define SendDlgItemMessageL SendDlgItemMessageW
#define SendDlgItemMessageLGetString SendDlgItemMessageW
#define GetDlgItemTextL GetDlgItemTextW
#define SetDlgItemTextL SetDlgItemTextW
#else
/* Some redirection stuff to allow NT to display text that
 * doesn't match the system code page
 */
int MessageBoxL(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
BOOL GetOpenSaveFileNameL(LPOPENFILENAME pofn, BOOL save);
BOOL DialogBoxParamL(HINSTANCE hInstance, LPCTSTR lpTemplateName,
    HWND hWndParent, DLGPROC lpDialogProc, LPARAM dwInitParam);
HWND CreateDialogParamL(HINSTANCE hInstance, LPCTSTR lpTemplateName,
    HWND hWndParent, DLGPROC lpDialogProc, LPARAM dwInitParam);
BOOL SetDlgItemTextL(HWND hDlg, int nlDDlgItem, LPCTSTR lpString);
BOOL GetDlgItemTextL(HWND hDlg, int nlDDlgItem, LPSTR lpString, 
    int nMaxCount);
BOOL LoadStringL(HINSTANCE hlanguage, UINT id, LPTSTR str, int len);
LONG SendDlgItemMessageL(HWND hDlg, int id, UINT msg, WPARAM wParam, 
    LPARAM lParam);
LONG SendDlgItemMessageLGetString(HWND hDlg, int id, UINT msg, WPARAM wParam, 
  LPARAM lParam);
#undef SetDlgItemText
#define SetDlgItemText(hwnd, id, str) SetDlgItemTextL((hwnd), (id), (str))
#undef CreateDialogParam
#define CreateDialogParam(hinst, tmpl, hwnd, dlgproc, param) \
  CreateDialogParamL((hinst), (tmpl), (hwnd), (dlgproc), (param))
#undef DialogBoxParam
#define DialogBoxParam(hinst, tmpl, hwnd, dlgproc, param) \
    DialogBoxParamL((hinst), (tmpl), (hwnd), (dlgproc), (param))
#undef LoadString
#define LoadString(hlanguage, id, str, len) \
    LoadStringL((hlanguage), (id), (str), (len))
/* early versions of Win32s don't support lstrcpyn */
#undef lstrcpyn
#define lstrcpyn(d,s,n) strncpy(d,s,n)
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#include "gvcfn.h"    /* common function prototypes */
#include "gvcbeta.h"  /* common function prototypes */
#include "gvwgsver.h" /* common function prototypes */

/* in gvwin.c */
int gsview_main(HINSTANCE hInstance, LPSTR lpszCmdLine);
LRESULT CALLBACK _export MenuButtonProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK _export WndImgProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK _export WndImgChildProc(HWND, UINT, WPARAM, LPARAM);
BOOL in_child_client_area(void);
void enable_menu_item(int menuid, int itemid, BOOL enabled);

/* in gvwinit.c */
void gsview_init0(LPSTR lpszCmdLine);
BOOL gsview_init1(int argc, char *argv[]);
void gsview_create(void);

/* in gvwdisp.c */
void do_message(void);
BOOL exec_pgm(char *name, char *arg, PROG* prog);
void stop_pgm(PROG* prog);
void cleanup_pgm(PROG *prog);

/* in gvwdlg.c */
DLGRETURN CALLBACK _export PageDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam);
DLGRETURN CALLBACK _export PageMultiDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam);

/* in gvwprn.c */
BOOL get_portname(char *portname, char *port);
int gp_printfile(char *filename, char *port);
extern char not_defined[];
void start_gvwgs(void);

/* gvwimg.cpp */
void image_color(unsigned int format, int index, 
    unsigned char *r, unsigned char *g, unsigned char *b);
HPALETTE image_create_palette(IMAGE *img);
HGLOBAL image_copy_dib(IMAGE *img);
void image_draw(IMAGE *img, HDC hdc, int dx, int dy, int wx, int wy,
    int sx, int sy);

#ifdef _WIN64
#define unlink(cs) _unlink(cs)
#define stricmp(cs,ct) _stricmp(cs,ct)
#define strnicmp(cs,ct,n) _strnicmp(cs,ct,n)
#define mktemp(s) _mktemp(s)
#define getcwd(s,n) _getcwd(s,n)
#define putenv(cs) _putenv(cs)
#define stat(cs,st) _stat(cs,st)
#endif

#endif
