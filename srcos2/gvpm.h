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

/* gvpm.h */
/* Header includes for PM GSview */

#define INCL_DOS
#define INCL_WIN
#define INCL_GPI
#define INCL_SPL
#define INCL_SPLDOSPRINT
#define INCL_SPLERRORS
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#ifdef __BORLANDC__
#include <dir.h>
#include <process.h>
#endif
#define NeedFunctionPrototypes 1
#include "gsvver.h"
#include "gvcrc.h"

#ifndef RC_INVOKED

typedef unsigned short WORD;
typedef unsigned long DWORD;


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

#define MAXSTR 256	/* maximum file name length and general string length */
#define PROFILE_SIZE 2048
#define DEVICENAME "display"
#define DEFAULT_GSCOMMAND "gsos2.exe"
#define DEFAULT_RESOLUTION 96.0
#define DEFAULT_ZOOMRES 300.0
#define INISECTION "Options"
#define DEVSECTION "Devices"
#define CONVERTSECTION "Convert"
#define EOLSTR "\r\n"
#define CW_USEDEFAULT 32768
#define COPY_BUF_SIZE 4096
#define PATHSEP "\\"
#define LPSTR char *
#define TCHAR char
#define LPTSTR TCHAR *
#ifndef LPCTSTR
#define LPCTSTR const TCHAR *
#endif
#define TEXT(x) (x)
#define convert_multibyte(s, t, len) strncpy((s),(t),(len))
#define convert_widechar(s, t, len) strncpy((s),(t),(len))
#define lstrlen(s) strlen(s)
#define lstrcpy(s,t) strcpy((s),(t))
#define wsprintf sprintf
#define IDYES MBID_YES
#define IDNO MBID_NO
#define IDOK  MBID_OK
#define IDCANCEL  MBID_CANCEL
#define HINSTANCE HMODULE
#define POINT POINTL

#define ID_STATUSBAR   100
#define ID_BUTTONBAR   101
#define SB_TOP 20
#define SB_BOTTOM 21

#ifndef min
#define min(x,y)  ( (x) < (y) ? (x) : (y) )
#endif
#ifndef max
#define max(x,y)  ( (x) > (y) ? (x) : (y) )
#endif

#include "gvceps.h"
#include "gvcprf.h"
#include "gvctype.h"

/* program details */
typedef struct tagPROG {
    BOOL	valid;
    ULONG	session_id;
    PID		process_id;
} PROG;

/* bitmap details */
typedef struct tagBM {
    BOOL	old_bmp;	/* bitmap type */
    PBITMAPINFO2 pbmi;		/* pointer to bitmap info */
    PBYTE	bits;		/* pointer to bitmap bits */
    int		width;
    int		height;
    int		planes;
    int		depth;
    int		palsize;
    int 	palimportant;
    int		old_width;
    int		old_height;
    int		old_planes;
    int		old_depth;
    int		old_palsize;
    int		old_palimportant;
} BMAP;

/* main structure with info about the GS DLL */
#include "cdll.h"
#include "cimg.h"
#include "cview.h"


typedef struct tagDISPLAY {
	int	width;
	int	height;
	LONG	planes;
	LONG	bitcount;
	BOOL	show_find;	/* highlight found text */
	BOOL	epsf_clipped;	/* display is clipped to bbox */
	int	zoom_xoffset;	/* in 1/72" */
	int	zoom_yoffset;
	POINTL	offset;	
	LONG	hasPalMan;	/* Palette Manager */
	BOOL	hpal_exists;
	HPAL	hpal;
	/* new ones for DLL */
	float	xdpi;
	float	ydpi;
	int	xoffset;
	int	yoffset;
	int	orientation;
	BOOL	init;		/* viewer initialised */
	BOOL	saved;		/* interpreter state saved */
	BOOL	need_header;
	BOOL	need_trailer;
	HEV	event;
	TID	tid;
} DISPLAY;

typedef struct tagPRINTER {
	PROG	prog;		 /* Ghostscript program doing printing */
	char	optname[MAXSTR]; /* file storing command line options */
	char	psname[MAXSTR];	 /* file storing temporary postscript */
} PRINTER;


/* button bar */
struct button {
	int	id;
	RECTL	rect;
	HBITMAP	hbitmap;
	char	str[16];
	struct button *next;
};
extern struct button *buttonhead, *buttontail;
extern BOOL button_down;
extern struct button *button_current;
extern POINTL button_shift;
extern POINTL button_size;

/* for pstotext DLL */
extern HMODULE pstotextModule;
extern FILE *pstotextOutfile;
extern void *pstotextInstance;
extern char pstotextLine[2048];
extern int pstotextCount;

#ifdef __cplusplus
extern "C" {
#endif
typedef int (GSDLLAPI *PFN_pstotextInit)(void **instance);
typedef int (GSDLLAPI *PFN_pstotextFilter)(void *instance, char *instr, 
    char **pre, char **word, char **post,
    int *llx, int *lly, int *urx, int *ury);
typedef int (GSDLLAPI *PFN_pstotextExit)(void *instance);
typedef int (GSDLLAPI *PFN_pstotextSetCork)(void *instance, int value);

extern PFN_pstotextInit pstotextInit;
extern PFN_pstotextFilter pstotextFilter;
extern PFN_pstotextExit pstotextExit;
extern PFN_pstotextSetCork pstotextSetCork;
#ifdef __cplusplus
}
#endif

/* for zlib gunzip decompression */
extern HMODULE zlib_hmodule;
typedef void *gzFile ;
typedef gzFile (*PFN_gzopen)(const char *path, const char *mode);
typedef int (*PFN_gzread)(gzFile file, void *buf, unsigned len);
typedef int (*PFN_gzclose)(gzFile file);
extern PFN_gzopen gzopen;
extern PFN_gzread gzread;
extern PFN_gzclose gzclose;

extern BOOL print_silent;	/* /P or /F command line option used */
extern BOOL print_exit;		/* exit on completion of printing */
extern int print_count;		/* number of current print jobs */
				/* It is safe to exit GSview when this is 0 */
extern int portable_app;	/* /A command line option used */

extern int debug;			/* /D command line option used */
extern FILE *debug_file;		/* for gs input logging */

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
	char file[MAXSTR];	/* empty, "beep", or .WAV sound to play */
};
extern struct sound_s sound[NUMSOUND];
#define BEEP "beep"		/* profile entry for a speaker beep */
typedef ULONG (* PFN_MciPlayFile)(HWND hwndOwner, PSZ pszFile, ULONG ulFlags,
	PSZ pszTitle, HWND hwndViewport);
extern PFN_MciPlayFile pfnMciPlayFile;

extern char szAppName[MAXSTR];
extern int nHelpTopic;
extern char szExePath[MAXSTR];
extern char szHelpName[MAXSTR];
extern char szWait[MAXSTR];
extern char szFindText[MAXSTR];
extern char szIniFile[MAXSTR];
extern unsigned char szMMini[MAXSTR];
extern char previous_filename[MAXSTR];	/* to remember name between file dlg boxes */
extern char selectname[MAXSTR];		/* for IDM_SELECT */
extern const char szScratch[];	/* temporary filename prefix */
extern char *szSpoolPrefix;	/* usually \\spool\ */
extern ULONG os_version;
extern BOOL multithread;		/* TRUE if running multithreaded */
extern GSVIEW_ARGS args;		/* Parsed arguments */
extern HMTX hmutex_ps;
extern HAB hab;
extern HMQ hand_mq;		/* message queue */
extern HWND hwnd_frame;
extern HWND hwnd_bmp;
extern HWND hwnd_image;
extern HWND hwnd_fullscreen;
#define hwndimg hwnd_bmp		/* to look like Windows */
extern HWND hwnd_status;
extern HWND hwnd_button;
extern HWND hwnd_help;
extern HWND hwnd_modeless;		/* any modeless dialog box */
extern HWND hwnd_measure;		/* measure modeless dialog box */
extern HWND hptr_crosshair;
extern HWND hptr_hand;
extern HWND hwnd_menu;
extern HACCEL haccel;
extern HMODULE hlanguage;
extern POINTL buttonbar;
extern POINTL statusbar;
extern POINTL info_file;
extern POINTL info_page;
extern RECTL info_coord;
extern int on_link;			/* TRUE if we were or are over link */
extern int on_link_page;		/* page number of link target */
extern long gsbytes_size;		/* number of bytes for this page */
extern long gsbytes_done;		/* number of byte written */
MRESULT EXPENTRY ClientWndProc(HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY FrameWndProc(HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY StatusWndProc(HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY ButtonWndProc(HWND, ULONG, MPARAM, MPARAM);
extern PFNWP OldFrameWndProc;
extern BOOL quitnow;	/* Used to cause exit from nested message loops */
extern int percent_done;		/* percentage of document processed */
extern int percent_pending;		/* TRUE if WM_GSPERCENT is pending */
extern BOOL fit_page_enabled;		/* next WM_SIZE is allowed to resize window */


extern PROG pdfconv;
extern OPTIONS option;
extern PSFILE psfile; 
extern DISPLAY display;
extern PRINTER printer;
extern BMAP bitmap;
extern int page_extra;
extern int page_skip;
extern BOOL changed_version;
extern BOOL zoom;

extern PSBBOX bbox;

extern char registration_name[MAXSTR];
extern unsigned int registration_receipt;

/* PRINT_GDI - NOT IMPLEMENTED FOR OS/2 */
extern int print_gdi_width;
extern int print_gdi_height;
extern int print_gdi_xdpi;
extern int print_gdi_ydpi;
extern ULONG print_gdi_read_handle;
extern ULONG print_gdi_write_handle;

#include "gvcfn.h"	/* common function prototypes */
#include "gvcbeta.h"    /* common function prototypes */

/* in gvpm.c */
void update_scroll_bars(void);
void enable_menu_item(int menuid, int itemid, BOOL enabled);

/* gvpdlg.c */
MRESULT EXPENTRY PageDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY PageMultiDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

/* in gvpinit.c */
APIRET gsview_init(int argc, char *argv[]);

/* in gvpmisc.c */
#define load_string_a load_string
#define message_box_a message_box
#define SetDlgItemTextA SetDlgItemText
BOOL SetDlgItemText(HWND hwnd, int id, char *str);

/* in gvpdisp.c */
BOOL exec_pgm(char *name, char *arg, PROG* prog);
void stop_pgm(PROG* prog);
void cleanup_pgm(PROG *prog);
BOOL gs_open(void);
BOOL gs_close(void);

/* in gvpprn.c */
#ifndef NERR_BufTooSmall
#define NERR_BufTooSmall 2123	/* For SplEnumQueue */
#endif
BOOL get_portname(char *portname, char *port);
int gp_printfile(char *filename, char *port);
extern char not_defined[];

#endif
