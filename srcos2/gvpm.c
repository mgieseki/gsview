/* Copyright (C) 1993-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvpm.c */
/* Main routines for PM GSview */
#define INCL_WINSTDDRAG
#include "gvpm.h"
#ifdef __EMX__
#include "sys/ioctl.h"
#endif

#include <signal.h>
#include <setjmp.h>

char szAppName[MAXSTR] = GSVIEW_PRODUCT;  /* application name - for title bar */
int nHelpTopic;
char szWait[MAXSTR];
char szExePath[MAXSTR];
char szHelpName[MAXSTR];
char szFindText[MAXSTR];
char szIniFile[MAXSTR];
unsigned char szMMini[MAXSTR];
char previous_filename[MAXSTR];
char selectname[MAXSTR];
const char szScratch[] = "gv";	/* temporary filename prefix */
char *szSpoolPrefix = "\\\\spool\\";
HMTX hmutex_ps;
HAB hab;		/* Anchor Block */
HMQ hand_mq;		/* message queue */
ULONG os_version;
BOOL multithread;
HWND hwnd_frame;	/* main window */
HWND hwnd_bmp;		/* client area of main window */
HWND hwnd_image;	/* full screen popup or child window */
HWND hwnd_fullscreen;	/* full screen popup */
HWND hwnd_status;
HWND hwnd_button;
HWND hwnd_help;
HWND hwnd_modeless;	/* any modeless dialog box */
HWND hwnd_measure;		/* measure modeless dialog box */
HWND hptr_crosshair;
HWND hptr_hand;
HWND hwnd_menu;
HACCEL haccel;
HMODULE hlanguage;
POINTL buttonbar;
POINTL statusbar;
POINTL info_file;
POINTL info_page;
POINTL scroll_pos;
RECTL info_coord;
int on_link;			/* TRUE if we were or are over link */
int on_link_page;		/* page number of link target */
const char *on_link_action;	/* action of link target */
long gsbytes_size;		/* number of bytes for this page */
long gsbytes_done;		/* number of byte written */
BOOL fit_page_enabled = FALSE;	/* next WM_SIZE is allowed to resize window */
BOOL quitnow = FALSE;		/* Used to cause exit from nested message loops */

int percent_done;		/* percentage of document processed */
int percent_pending;		/* TRUE if WM_GSPERCENT is pending */

#define MAXARGS 64
int nargc;
char *nargv[MAXARGS];
GSVIEW_ARGS args;		/* Parsed arguments */
PSFILE psfile;		/* Postscript file structure */
BMAP bitmap;		/* Bitmap structure */
OPTIONS option;		/* GSview options (saved in INI file) */
DISPLAY display;	/* Display parameters */
PRINTER printer;	/* Printer GS parameters */
char last_files[4][MAXSTR];	/* last 4 files used */
int last_files_count;		/* number of files known */
HISTORY history;		/* history of pages displayed */
BOOL fullscreen = FALSE;

int page_skip = 5;		/* number of pages to skip in IDM_NEXTSKIP or IDM_PREVSKIP */
BOOL zoom = FALSE;		/* true if display zoomed */
BOOL print_silent = FALSE;	/* /P or /F command line option used */
BOOL print_exit = FALSE;	/* exit on completion of printing */
int print_count = 0;		/* number of current print jobs */
				/* It is safe to exit GSview when this is 0 */
int portable_app = 0;		/* /A command line option used */
int debug = 0;			/* /D command line option used */
struct sound_s sound[NUMSOUND] = {
	{"SoundOutputPage", IDS_SNDPAGE, ""},
	{"SoundNoPage", IDS_SNDNOPAGE, BEEP},
	{"SoundNoNumbering", IDS_SNDNONUMBER, ""},
	{"SoundNotOpen", IDS_SNDNOTOPEN, ""},
	{"SoundError", IDS_SNDERROR, BEEP},
	{"SoundStart", IDS_SNDSTART, ""},
	{"SoundExit", IDS_SNDEXIT, ""},
	{"SoundBusy", IDS_SNDBUSY, BEEP},
};
PFN_MciPlayFile pfnMciPlayFile;
USERMEDIA usermedia[IDM_USERSIZE13 - IDM_USERSIZE1 + 1];

/* button bar */
void delete_buttons(void);
struct button *buttonhead, *buttontail;
BOOL button_down;
struct button *button_current;
struct button *button_info;
POINTL button_shift;
POINTL button_size;

MRESULT EXPENTRY ClientWndProc(HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY FrameWndProc(HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY StatusWndProc(HWND, ULONG, MPARAM, MPARAM);
MRESULT EXPENTRY ButtonWndProc(HWND, ULONG, MPARAM, MPARAM);
PFNWP OldFrameWndProc;

BOOL scan_bitmap(BMAP *pbm);
HBITMAP make_bitmap(BMAP *, ULONG, ULONG, ULONG, ULONG, ULONG);
void cursorpos_paint(HPS hps);
void map_pt_to_pixel(float *x, float *y);
MRESULT DragOver(PDRAGINFO);
MRESULT Drop(PDRAGINFO);
void highlight_words(HPS ps, int first, int last);
BOOL text_marking = FALSE;
int text_mark_first = -1;
int text_mark_last = -1;
void highlight_links(HPS ps);
void info_link(void);

void
update_scroll_bars(void)
{
    /* Cause update of scroll bars etc. */
    SWP swp;
    WinQueryWindowPos(hwnd_image, &swp);
    WinSendMsg(hwnd_image, WM_SIZE, MPFROM2SHORT(swp.cx, swp.cy), MPFROM2SHORT(swp.cx, swp.cy));
}



/* Return TRUE if is OK to exit */
BOOL
query_close(void)
{
    /* tell GS DLL to unload */
    request_mutex();
    pending.unload = TRUE;
    pending.abort = TRUE;
    quitnow = TRUE;
    release_mutex();
    if (multithread)
        DosPostEventSem(display.event);	/* unblock display thread */
    return TRUE;
}

void
exit_func(void)
{
    psfile_free(&psfile);
    if (option.settings && !print_silent)
	write_profile();
    else
	write_profile_last_files();	/* always save MRU files */
    unload_zlib();
    unload_pstoedit();
    if (debug & DEBUG_MEM) {
	/* flush message queue of quit message */
        QMSG q_mess;		/* queue message */
	WinPeekMsg(hab, &q_mess, 0L, 0, 0, PM_REMOVE);
	WinPeekMsg(hab, &q_mess, 0L, 0, 0, PM_REMOVE);
	WinPeekMsg(hab, &q_mess, 0L, 0, 0, PM_REMOVE);
	WinPeekMsg(hab, &q_mess, 0L, 0, 0, PM_REMOVE);
	debug_memory_report();
    }
    language_free();
}


/* code to protect GSview against Ghostscript memory violations */
jmp_buf sig_env;

void
sig_handler(int code)
{
	signal(code, SIG_ACK); 
	longjmp(sig_env, 1);
}

void
gs_sig_process()
{
    if (setjmp(sig_env) == 0) {
	/* got here on direct call */
	signal(SIGSEGV, sig_handler);
	signal(SIGFPE, sig_handler);
        gs_process();	/* start Ghostscript */
    }
    else {
	/* got here by calling longjmp */
	pending.now = FALSE;
	delayed_message_box(IDS_SIGSEGV, 0);
    }
    signal(SIGFPE, SIG_DFL);	/* restore default handler */
    signal(SIGSEGV, SIG_DFL);	/* restore default handler */
}


/* Thread which loads Ghostscript DLL for display */
void 
gs_thread(void *arg)
{
HAB hab;
    hab = WinInitialize(0);
    while (!quitnow) {
	if (!pending.now)
	    wait_event();
	if (!quitnow)
	    gs_sig_process();
    }

    WinTerminate(hab);
    /* signal that we have finished */
    display.tid = 0;
}


int
main(int argc, char *argv[])
{
  QMSG q_mess;		/* queue message */
  APIRET rc = 0;

  hab = WinInitialize(0);	/* Get the Anchor Block */

  hand_mq = WinCreateMsgQueue(hab, 0); /* start a queue */

  rc = gsview_init(argc, argv);
/*
  atexit(exit_func);
*/

  if (!rc) {
	SWP swp;
	WinQueryWindowPos(hwnd_frame, &swp);
	if (print_silent) {
	    /* show minimized */
	    swp.fl &= (~SWP_MAXIMIZE);
	    swp.fl &= (~SWP_RESTORE);
	    swp.fl |= SWP_MINIMIZE | SWP_ACTIVATE | SWP_SHOW;;
	}
	else {
	    swp.fl |= SWP_ACTIVATE | SWP_SHOW;
	}
	WinSetWindowPos(hwnd_frame, HWND_TOP, swp.x, swp.y, swp.cx, swp.cy, swp.fl);
	haccel = WinQueryAccelTable(hab, hwnd_frame);
  }

  if (multithread) {
      /* start thread for displaying */
      display.tid = _beginthread(gs_thread, NULL, 131072, NULL);
  }

  play_sound(SOUND_START);
  info_wait(IDS_NOWAIT);
  if (gsview_changed())
      WinPostMsg(hwnd_bmp, WM_CLOSE, MPFROMLONG(0), MPFROMLONG(0));
  else if (!print_silent)
      /* before changing following line, please see gvcreg.cpp */
      registration_check();

  /* message loop */
  while (!rc && !quitnow && WinGetMsg(hab, &q_mess, 0L, 0, 0)) {
      WinDispatchMsg(hab, &q_mess);
      if (multithread) {
	  /* release other thread if needed */
	  if (pending.unload || pending.now || pending.next)
		DosPostEventSem(display.event);
      }
      else {
	  if (pending.now)
	    gs_sig_process();	/* start Ghostscript */
      }
  }

  if (WinIsWindow(hab, hwnd_frame))	/* don't play sound if closed using system menu */
      play_sound(SOUND_EXIT);
  if (multithread && display.tid) {
      int i = 100;	/* 10 seconds */
      pending.unload = TRUE;
      quitnow = TRUE;
      DosPostEventSem(display.event);	/* unblock display thread */
      while (i && display.tid) {
	  /* wait for thread to close */
	  DosSleep(100);
          DosPostEventSem(display.event);	/* unblock display thread */
	  i--;
	  peek_message();
      }
  }

  /* Shut down the application window and queue */
  if (hwnd_help)
      WinDestroyHelpInstance(hwnd_help);
  DosCloseMutexSem(hmutex_ps);
  WinDestroyWindow(hwnd_frame);
  hwnd_frame = (HWND)NULL;
  delete_buttons();
  WinDestroyMsgQueue(hand_mq);
  WinTerminate(hab);

  exit_func();

  return rc;
}

#define MAX_PAL_SIZE 256
void
make_palette(BMAP *pbm)
{
ULONG tbl[MAX_PAL_SIZE];
PRGB2 palptr = (PRGB2) ((PBYTE)(pbm->pbmi) + pbm->pbmi->cbFix);
RGB *old_palptr = (RGB *)palptr;
int palcount = pbm->palimportant;
int i;
BOOL old_bmp = (pbm->pbmi->cbFix == sizeof(BITMAPINFOHEADER));
    if (old_bmp) {
	for (i=0; i<palcount; i++) {
	    tbl[i] = (old_palptr->bRed<<16) + (old_palptr->bGreen<<8) + (old_palptr->bBlue);
	    old_palptr++;
	}
    }
    else {
	for (i=0; i<palcount; i++) {
	    tbl[i] = (palptr->bRed<<16) + (palptr->bGreen<<8) + (palptr->bBlue);
	    palptr++;
	}
    }
    if (display.hpal_exists)
	GpiDeletePalette(display.hpal);
    display.hpal = GpiCreatePalette(hab, 0L, LCOLF_CONSECRGB, palcount, tbl);
    display.hpal_exists = TRUE;
}


/* scan bitmap */
/* update bitmap structure */
/* return value is TRUE if bitmap dimension has changed */
BOOL
scan_bitmap(BMAP *pbm)
{
PBITMAPINFO2 pbmi;
PBITMAPINFO old_pbmi;
BOOL old_bmp;

    if (!pbm->pbmi)
	return TRUE;

    pbmi = pbm->pbmi;
    old_pbmi = (PBITMAPINFO)pbmi;
    old_bmp = (pbmi->cbFix == sizeof(BITMAPINFOHEADER));

    if (old_bmp) {
  	/* it is a BITMAPINFO */
  	switch(old_pbmi->cBitCount) {
  	  case 24:
  	    pbm->palsize = 0;
  	    break;
  	  case 8:
  	    pbm->palsize = 256;
  	    break;
  	  case 4:
  	    pbm->palsize = 16;
  	    break;
  	  case 1:
  	    pbm->palsize = 2;
  	    break;
  	  default:
  	    error_message("scan_bitmap: wrong number of bits"); /* panic */
  	    return FALSE;
	}
	pbm->palimportant = pbm->palsize;
	pbm->palsize = pbm->palsize * sizeof(RGB);
        pbm->bits   = (PBYTE)old_pbmi + old_pbmi->cbFix + pbm->palsize;
	pbm->width  = old_pbmi->cx;
	pbm->height = old_pbmi->cy;
	pbm->planes = old_pbmi->cPlanes;
	pbm->depth  = old_pbmi->cBitCount;
    }
    else {
 	/* it is a BITMAPINFO2 */
  	switch(pbmi->cBitCount) {
  	  case 24:
  	    pbm->palsize = 0;
  	    break;
  	  case 8:
  	    pbm->palsize = 256;
  	    break;
  	  case 4:
  	    pbm->palsize = 16;
  	    break;
  	  case 1:
  	    pbm->palsize = 2;
  	    break;
  	  default:
  	    error_message("scan_bitmap: wrong number of bits"); /* panic */
  	    return FALSE;
	}
	if ( (pbmi->cbFix > (&(pbmi->cclrUsed) - &(pbmi->cbFix)))
		&& (pbmi->cclrUsed != 0) && (pbmi->cBitCount != 24) )
	    pbm->palsize = pbmi->cclrUsed;
	pbm->palimportant = pbm->palsize;
	if ( (pbmi->cbFix > (&(pbmi->cclrImportant) - &(pbmi->cbFix)))
		&& (pbmi->cclrImportant != 0) && (pbmi->cBitCount != 24) )
	    pbm->palimportant = pbmi->cclrImportant;
	pbm->palsize = pbm->palsize * sizeof(RGB2);
        pbm->bits   = (PBYTE)pbmi + pbmi->cbFix + pbm->palsize;
	pbm->width  = pbmi->cx;
	pbm->height = pbmi->cy;
	pbm->planes = pbmi->cPlanes;
	pbm->depth  = pbmi->cBitCount;
    }

    if ((pbm->palsize != pbm->old_palsize) || (pbm->palimportant != pbm->old_palimportant)) {
	if ( (pbm->depth == 8) && display.hasPalMan )
	    make_palette(pbm);
	pbm->old_palimportant = pbm->palimportant;
    }

    if ( (pbm->width   == pbm->old_width) && 
    	 (pbm->height  == pbm->old_height) &&
	 (pbm->planes  == pbm->old_planes) && 
	 (pbm->depth   == pbm->old_depth) &&
	 (pbm->palsize == pbm->old_palsize) &&
	 (pbm->old_bmp == old_bmp) )
	return FALSE;

    /* bitmap has changed */
    pbm->old_width   = pbm->width;
    pbm->old_height  = pbm->height;
    pbm->old_planes  = pbm->planes;
    pbm->old_depth   = pbm->depth;
    pbm->old_palsize = pbm->palsize;
    pbm->old_bmp     = old_bmp;
    return TRUE;
}

/* copy bitmap to the clipboard */
void
copy_clipboard(void)
{
HBITMAP hbmp;
    if (!image.open) {
	gserror(0, "Cannot copy to clipboard:\nNo Bitmap displayed", MB_ICONEXCLAMATION, SOUND_ERROR);
	return;
    }
    if ( text_index && (text_mark_first != -1) && (text_mark_last != -1)) {
	/* copy text, not image */
	int first, last, line;
	int length;
	int i;
	LPSTR data;
	PVOID mem;
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
	if (DosAllocSharedMem(&mem, NULL, length, PAG_READ | PAG_WRITE | PAG_COMMIT | OBJ_GIVEABLE)) {
	    message_box("out of memory", 0);
	    return;
	}
	data = (LPSTR)mem;
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
        if (WinOpenClipbrd(hab)) {
	    WinEmptyClipbrd(hab);
	    WinSetClipbrdData(hab, (ULONG)mem, CF_TEXT, CFI_POINTER);
	    WinCloseClipbrd(hab);
	}
	return;
    }

    if (WinOpenClipbrd(hab)) {
	/* get bmp mutex to stop gs.exe changing bitmap while we copy it */
	image_lock(&image);
	if (image.open && scan_bitmap(&bitmap)) {
	    /* bitmap has changed */
	    update_scroll_bars();
	}
	hbmp = make_bitmap(&bitmap, 0, 0, bitmap.width, bitmap.height, bitmap.depth);
	if (hbmp) {
	    WinEmptyClipbrd(hab);
	    WinSetClipbrdData(hab, (ULONG)hbmp, CF_BITMAP, CFI_HANDLE);
	}
	image_unlock(&image);
	WinCloseClipbrd(hab);
    }
}

HBITMAP
make_bitmap(BMAP *pbm, ULONG left, ULONG bottom, ULONG right, ULONG top, ULONG depth)
{
HDC hdcMem = DEV_ERROR;
HPS hps = GPI_ERROR;
HBITMAP hbmp = GPI_ERROR, hbmr = HBM_ERROR;
SIZEL sizePS;
BITMAPINFOHEADER2 bmih;

	if ( (left == right) || (bottom == top) )
		return (HBITMAP)NULL;

	if ((int)right > pbm->width)
		right = pbm->width;
	if ((int)left > pbm->width)
		left = 0;
	if ((int)top > pbm->height)
		top = pbm->height;
	if ((int)bottom > pbm->height)
		bottom = 0;
		
	memset(&bmih, 0, sizeof(bmih));
	bmih.cbFix = sizeof(BITMAPINFOHEADER2);
	bmih.cx = right - left;
	bmih.cy = top - bottom;
	bmih.cPlanes = 1;
	bmih.cBitCount = depth;

	/* create memory DC compatible with screen */
	hdcMem = DevOpenDC(hab, OD_MEMORY, (unsigned char *)"*", 0L, NULL, NULLHANDLE);

	sizePS.cx = right - left;
	sizePS.cy = top - bottom;
	if (hdcMem != DEV_ERROR)
	    hps = GpiCreatePS(hab, hdcMem, &sizePS, 
		PU_PELS | GPIF_DEFAULT | GPIT_MICRO | GPIA_ASSOC );

	if (hps != GPI_ERROR)
		hbmp = GpiCreateBitmap(hps, &bmih, 0L, NULL, NULL);

	if (hbmp != GPI_ERROR)
		hbmr = GpiSetBitmap(hps, hbmp);


	if (hbmr != HBM_ERROR) {
		LONG rc;
		ERRORID eid;
  	    	POINTL apts[4];
		/* target is inclusive */
		apts[0].x = 0;
		apts[0].y = 0;
		apts[1].x = right - left - 1;
		apts[1].y = top - bottom - 1;
		/* source is not inclusive of top & right borders */
		apts[2].x = left;
		apts[2].y = bottom;
		apts[3].x = right;
		apts[3].y = top;

		rc = 0;
		eid = WinGetLastError(hab);
		if ((bitmap.depth == 8) && display.hasPalMan && display.hpal_exists)
		    GpiSelectPalette(hps, display.hpal);
	    	rc = GpiDrawBits(hps, pbm->bits, pbm->pbmi, 4, apts, 
			(bitmap.depth != 1) ? ROP_SRCCOPY : ROP_NOTSRCCOPY, 0);
		if ((bitmap.depth == 8) && display.hasPalMan && display.hpal_exists)
		    GpiSelectPalette(hps, (HPAL)NULL);
		if (rc==0) {
			char buf[MAXSTR];
			eid = WinGetLastError(hab);
			sprintf(buf,"make_bitmap: GpiDrawBits rc = %08x, eid = %08x",(int)rc, (int)eid);
			message_box(buf, 0);
		}
	}
	
	if (hbmr != HBM_ERROR)
		GpiSetBitmap(hps, (ULONG)0);
	if (hps != GPI_ERROR)
		GpiDestroyPS(hps);
	if (hdcMem != DEV_ERROR)
		DevCloseDC(hdcMem);

	if ( (hbmr == HBM_ERROR) || (hdcMem == DEV_ERROR) ||
		(hbmp == GPI_ERROR) || (hps == GPI_ERROR) ) {
		if (hbmp != GPI_ERROR)
			GpiDeleteBitmap(hbmp);
		return 0;
	}
	return hbmp;
}

MRESULT
paint_bitmap(HPS ps, PRECTL prect, int scrollx, int scrolly)
{
    POINTL apts[4];
    RECTL rect;
    int wx, wy;
    HRGN hrgn, hrgnold;
    HPOINTER hptr;
    LONG background_colour = SYSCLR_DIALOGBACKGROUND;
    if (WinIsRectEmpty(hab, prect))
	return 0;

/* KLUDGE BEGIN */
    /* remove pointer to cope with buggy display drivers */
    hptr = WinQueryPointer(HWND_DESKTOP);
    WinSetPointer(HWND_DESKTOP, 0);
/* KLUDGE END */

    /* source is not inclusive of top & right borders */
    wx = prect->xRight - prect->xLeft;   /* update width */
    wy = prect->yTop -   prect->yBottom; /* update height */
    if (prect->xLeft < display.offset.x)
	apts[2].x = 0;
    else
	apts[2].x = (prect->xLeft-display.offset.x)   + scrollx;
    if (prect->yBottom < display.offset.y)
        apts[2].y = 0;
    else
	apts[2].y = (prect->yBottom-display.offset.y) + scrolly;
    if (apts[2].x > bitmap.width)
	    apts[2].x = bitmap.width;
    if (apts[2].x + wx > bitmap.width) {
	    wx = bitmap.width - apts[2].x;
    }
    apts[3].x = apts[2].x + wx;
    if (apts[2].y > bitmap.height)
	    apts[2].y = bitmap.height;
    if (apts[2].y + wy > bitmap.height) {
	    wy = bitmap.height - apts[2].y;
    }
    apts[3].y = apts[2].y + wy;
    /* target is inclusive */
    if (prect->xLeft < display.offset.x)
	apts[0].x = display.offset.x;
    else 
	apts[0].x = prect->xLeft;
    if (prect->yBottom < display.offset.y)
 	apts[0].y = display.offset.y;
    else
	apts[0].y = prect->yBottom;
    apts[1].x = apts[0].x + wx - 1;
    apts[1].y = apts[0].y + wy - 1;

    if ( (option.drawmethod == IDM_DRAWWIN) || 
         ((option.drawmethod == IDM_DRAWDEF) && (display.bitcount == 4)) ) /* standard VGA is buggy */
        {
	/* slow code to dodge OS/2 bugs */
	/* this code double buffers the bitmap and works on a standard VGA
	 * but didn't work on an ATI Ultra Graphics Pro in 8514 emulation
	 */
	/* This won't work for OS/2 2.11, S3 or ATI GU, 8bit/pixel display, 8bit/pixel bitmap */
	HBITMAP hbmp;
	/* create a bitmap */
	hbmp = make_bitmap(&bitmap, apts[2].x, apts[2].y, apts[3].x, apts[3].y, bitmap.depth);
	/* Draw it to the display */
	if (hbmp) {
	    WinDrawBitmap(ps, hbmp, NULL, &apts[0], CLR_BLACK, CLR_WHITE, DBM_NORMAL);
	    GpiDeleteBitmap(hbmp);
	}
    }
    else {   /* (option.drawmethod == IDM_DRAWGPI) || (default and non buggy display) */
	/* fast code which doesn't always work */
	/* This code works on the Trident SVGA and 8514 in 256 color mode,
 	 * but GpiDrawBits fails with a SYS3175 on the standard VGA.
	 */
	/* This won't work for version 2.11, S3 or ATI GU, 8bit/pixel display, 1bit/pixel bitmap */
	GpiDrawBits(ps, bitmap.bits, bitmap.pbmi, 4, apts, 
		(bitmap.depth != 1) ? ROP_SRCCOPY : ROP_NOTSRCCOPY, 0);
    }


    if (fullscreen)
	background_colour = CLR_BLACK;

    /* Fill areas around page */
    if (prect->yBottom < display.offset.y) {	/* bottom centre */
	rect.yBottom = prect->yBottom;
	rect.yTop = display.offset.y;
	rect.xLeft = apts[0].x;
	rect.xRight = rect.xLeft + wx;
	WinFillRect(ps, &rect, background_colour);
    }
    if (prect->yTop > bitmap.height + display.offset.y) { /* top centre */
	rect.yBottom = bitmap.height + display.offset.y;
	rect.yTop = prect->yTop;
	rect.xLeft = apts[0].x;
	rect.xRight = rect.xLeft + wx;
	WinFillRect(ps, &rect, background_colour);
    }
    if (prect->xLeft < display.offset.x) { /* left */
	rect.yBottom = prect->yBottom;
	rect.yTop = prect->yTop;
	rect.xLeft = prect->xLeft;
	rect.xRight = display.offset.x;
	WinFillRect(ps, &rect, background_colour);
    }
    if (prect->xRight > bitmap.width + display.offset.x) { /* right */
	rect.yBottom = prect->yBottom;
	rect.yTop = prect->yTop;
	rect.xLeft = bitmap.width + display.offset.x;
	rect.xRight = prect->xRight;
	WinFillRect(ps, &rect, background_colour);
    }

    /* clip other drawing commands to update rectangle */
    hrgn = GpiCreateRegion(ps, 1, prect);
    GpiSetClipRegion(ps, hrgn, &hrgnold);
    if (hrgnold)
	GpiDestroyRegion(ps, hrgnold);

    /* draw bounding box */
    if ((psfile.dsc != (CDSC *)NULL) && 
	(psfile.dsc->bbox != (CDSCBBOX *)NULL) &&
	option.show_bbox) {
        POINTL pt;
	float x, y;
	/* map bounding box to device coordinates */
	x = psfile.dsc->bbox->llx;
	y = psfile.dsc->bbox->lly;
	map_pt_to_pixel(&x, &y);
	rect.xLeft   = (int)x;
	rect.yBottom = (int)y;
	x = psfile.dsc->bbox->urx;
	y = psfile.dsc->bbox->ury;
	map_pt_to_pixel(&x, &y);
	rect.xRight  = (int)x;
	rect.yTop    = (int)y;

	/* draw it inverted */
	GpiSetColor(ps, CLR_TRUE);
	GpiSetLineType(ps, LINETYPE_SHORTDASH);
	GpiSetMix(ps, FM_XOR);
	pt.x = rect.xLeft; pt.y = rect.yBottom;
	GpiMove(ps, &pt);
	pt.x = rect.xRight; /* might be better to use GpiPolyLine */
	GpiLine(ps, &pt);
	pt.y = rect.yTop;
	GpiLine(ps, &pt);
	pt.x = rect.xLeft;
	GpiLine(ps, &pt);
	pt.y = rect.yBottom;
	GpiLine(ps, &pt);
	GpiSetLineType(ps, LINETYPE_DEFAULT);
	GpiSetMix(ps, FM_DEFAULT);
	GpiSetColor(ps, CLR_TRUE);
    }

    /* highlight found search word */
    if (image.open && display.show_find) {
	float x, y;
	/* map bounding box to device coordinates */
	x = psfile.text_bbox.llx;
	y = psfile.text_bbox.lly;
	map_pt_to_pixel(&x, &y);
	rect.xLeft   = (int)x;
	rect.yBottom = (int)y;
	x = psfile.text_bbox.urx;
	y = psfile.text_bbox.ury;
	map_pt_to_pixel(&x, &y);
	rect.xRight  = (int)x;
	rect.yTop    = (int)y;
	if (rect.yTop < rect.yBottom) {
	    int temp = rect.yTop;
	    rect.yTop = rect.yBottom;
	    rect.yBottom = temp;
	}

	/* invert text */
	WinInvertRect(ps, &rect);
    }

    /* highlight marked words */
    highlight_words(ps, text_mark_first, text_mark_last);

    /* GS 6.50 highlights links itself for PDF files */
    if ((option.gsversion < 650) || !psfile.ispdf)
	highlight_links(ps);

    GpiSetClipRegion(ps, NULLHANDLE, &hrgnold);
    if (hrgnold)
	GpiDestroyRegion(ps, hrgnold);

/* KLUDGE BEGIN */
    /* restore pointer after we removed it */
    WinSetPointer(HWND_DESKTOP, hptr);
/* KLUDGE END */

    return 0;
}

/* enable or disable a menu item */
void
enable_menu_item(int menuid, int itemid, BOOL enabled)
{
HWND hwndMenu;
MENUITEM mi;
	hwndMenu = WinWindowFromID(hwnd_frame, FID_MENU);
	WinSendMsg(hwndMenu, MM_QUERYITEM, 
		MPFROM2SHORT(menuid, TRUE), MPFROMP(&mi));
	WinSendMsg(mi.hwndSubMenu, MM_SETITEMATTR, MPFROMLONG(itemid),
		MPFROM2SHORT(MIA_DISABLED, enabled ? 0 : MIA_DISABLED));
}


void
init_menu(int menuid)
{
BOOL idle;
BOOL addeps;
    idle = (gsdll.state != GS_BUSY);
    switch (menuid) {
	case IDM_FILEMENU:
	    enable_menu_item(IDM_FILEMENU, IDM_PRINT, idle);
	    enable_menu_item(IDM_FILEMENU, IDM_PRINTTOFILE, idle);
	    enable_menu_item(IDM_FILEMENU, IDM_EXTRACT, idle);
	    enable_menu_item(IDM_FILEMENU, IDM_PSTOEPS, idle);
	    /* recent files */
	    {	HWND hwndMenu;
		MENUITEM mi;
		MENUITEM mii;
		char buf[MAXSTR];
		int i;
		hwndMenu = WinWindowFromID(hwnd_frame, FID_MENU);
		WinSendMsg(hwndMenu, MM_QUERYITEM, 
			MPFROM2SHORT(IDM_FILEMENU, TRUE), MPFROMP(&mi));
		WinSendMsg(mi.hwndSubMenu, MM_DELETEITEM, 
			MPFROM2SHORT(IDM_LASTFILE1, TRUE), (MPARAM)NULL);
		WinSendMsg(mi.hwndSubMenu, MM_DELETEITEM, 
			MPFROM2SHORT(IDM_LASTFILE2, TRUE), (MPARAM)NULL);
		WinSendMsg(mi.hwndSubMenu, MM_DELETEITEM, 
			MPFROM2SHORT(IDM_LASTFILE3, TRUE), (MPARAM)NULL);
		WinSendMsg(mi.hwndSubMenu, MM_DELETEITEM, 
			MPFROM2SHORT(IDM_LASTFILE4, TRUE), (MPARAM)NULL);
		mii.iPosition = 15;
		mii.afStyle = MIS_TEXT;
		mii.afAttribute = 0;
		mii.hwndSubMenu = (HWND)NULL;
		mii.hItem = 0;
		for (i=last_files_count; i>0; i--) {
		    sprintf(buf, "~%d %s", i, last_files[i-1]);
		    if (strlen(buf)>36) {
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
		    mii.id = IDM_LASTFILE1+i-1;
		    WinSendMsg(mi.hwndSubMenu, MM_INSERTITEM, 
			(MPARAM)&mii, (MPARAM)buf);
		}
	    }
	    break;
	case IDM_EDITMENU:
	    request_mutex();
	    enable_menu_item(IDM_EDITMENU, IDM_COPYCLIP, image.open);
	    enable_menu_item(IDM_EDITMENU, IDM_PASTETO, image.open);
    	    addeps =  (psfile.dsc != (CDSC *)NULL) && psfile.dsc->epsf && idle;
	    enable_menu_item(IDM_EDITMENU, IDM_ADDEPSMENU, addeps);
	    enable_menu_item(IDM_ADDEPSMENU, IDM_MAKEEPSU, addeps);
    	    addeps =  addeps && image.open;
	    enable_menu_item(IDM_ADDEPSMENU, IDM_MAKEEPSI, addeps);
	    enable_menu_item(IDM_ADDEPSMENU, IDM_MAKEEPST4, addeps);
	    enable_menu_item(IDM_ADDEPSMENU, IDM_MAKEEPST6U, addeps);
	    enable_menu_item(IDM_ADDEPSMENU, IDM_MAKEEPST6P, addeps);
	    enable_menu_item(IDM_EDITMENU, IDM_EXTEPSMENU, 
		    ((psfile.preview == IDS_EPST) || (psfile.preview == IDS_EPSW)) && idle);
	    enable_menu_item(IDM_EDITMENU, IDM_TEXTEXTRACT, idle);
	    enable_menu_item(IDM_EDITMENU, IDM_TEXTFIND, idle);
	    enable_menu_item(IDM_EDITMENU, IDM_TEXTFINDNEXT, idle);
	    release_mutex();
	    break;
    }
}

/* This is the window function */
MRESULT EXPENTRY ClientWndProc(HWND hwnd, ULONG mess, 
			MPARAM mp1, MPARAM mp2)
{
  char buf[MAXSTR];

  static int cxClient, cyClient;
  static int cxAdjust, cyAdjust;
  static int nHscrollMax, nHscrollPos;
  static int nVscrollMax, nVscrollPos;
  int nHscrollInc;
  int nVscrollInc;
  HWND hwndScroll;
  HPS hps;
  RECTL rect;
  ULONG ulclr;

    switch(mess) {
	case WM_CREATE:
	    break;
	case WM_ERASEBACKGROUND:
	    /* by returning TRUE, the Presentation Manager automatically clears
	     * the window each time the window is resized or moved.
	     */
	    if (!image.open) {
		WinFillRect((HPS)mp1, (PRECTL)mp2, SYSCLR_DIALOGBACKGROUND);
		return (MRESULT)FALSE;	/* say we have done it */
	    }
	    return (MRESULT)TRUE;
	case WM_GSSYNC:
	    if (!fullscreen) {
		if (!WinInvalidateRect(hwnd_image, (PRECTL)NULL, TRUE))
			error_message("error invalidating rect");
		if (!WinUpdateWindow(hwnd_image))
			error_message("error updating window");
	    }
	    return 0;
	case WM_GSPAGE:
	    play_sound(SOUND_PAGE);
	    if (display.show_find)
		scroll_to_find();
	    if (!WinInvalidateRect(hwnd_image, (PRECTL)NULL, TRUE))
		    error_message("error invalidating rect");
	    if (!WinUpdateWindow(hwnd_image))
		    error_message("error updating window");
	    return 0;
	case WM_GSTILE:
	    /* draw a tile that has changed */
	    if (fullscreen)
		return 0;
	    image_lock(&image);
	    if (image.open) {
		/* calculate area */
		DATETIME dt1, dt2;
		int td;
		RECTL rect;
		ULONG dw = LONGFROMMP(mp1);
		int tx1 = (dw >> 24) & 0xff;
		int ty1 = (dw >> 16) & 0xff;
		int tx2 = (dw >> 8) & 0xff;
		int ty2 = (dw) & 0xff;
		rect.xLeft = (tx1 * image.tile_width) 
		    + display.offset.x - scroll_pos.x;
		rect.xRight = rect.xLeft + 
		    (tx2 - tx1 + 1) * image.tile_width + 1;
		rect.yBottom = (ty1 * image.tile_height)
		    + display.offset.y - scroll_pos.y;
		rect.yTop = rect.yBottom + 
		    (ty2 - ty1 + 1) * image.tile_height + 1;
		image_unlock(&image);

		DosGetDateTime(&dt1);
		WinInvalidateRect(hwnd_image, (PRECTL)&rect, TRUE);
		WinUpdateWindow(hwnd_image);
		DosGetDateTime(&dt2);
		td = ((dt2.minutes - dt1.minutes) * 6000 +
		    (dt2.seconds - dt1.seconds) * 100 +
		    (dt2.hundredths - dt1.hundredths)) * 10;
		image_lock(&image);
		if (image.open && (td * 5 > image.tile_interval)) {
		    image.tile_interval += td + 100;
		    image.tile_verydirty_threshold += 256;
		    image.tile_threshold += image.tile_threshold;
		}
		image_unlock(&image);
		/* don't update immediately */
	    }
	    else {
		image_unlock(view.img);
	    }
	    return 0;
	case WM_GSDEVICE:
	    if (fullscreen) /* don't flash between pages */
		return 0;
	    if (!image.open) {
		if (fullscreen)
		    gsview_fullscreen_end();
	        WinInvalidateRect(hwnd, NULL, FALSE);
		if (!psfile.name[0])
		    update_scroll_bars();
	    }
	    return 0;
	case WM_GSSIZE:
	    if (image.open && scan_bitmap(&bitmap)) {
		/* bitmap has changed */
		update_scroll_bars();
	    }
	    return 0;
	case WM_GSMESSBOX:
	    /* delayed message box, usually from other thread */
	    load_string((int)mp1, buf, sizeof(buf));
	    message_box(buf, (int)mp2);
	    return 0;
	case WM_GSSHOWMESS:
	    /* delayed show message, usually from other thread */
	    gs_showmess();
	    return 0;
	case WM_GSREDISPLAY:
	    WinPostMsg(hwnd_image, WM_COMMAND, MPFROMSHORT(IDM_REDISPLAY), 
			MPFROMLONG(0));
	    return 0;
	case WM_GSTITLE:
	    /* update title */
	    request_mutex();
	    if (psfile.name[0] != '\0') {
		char buf[256];
		char *p;
		p = strrchr(psfile.name, '\\');
		if (p)
		    p++;
		else
		    p = psfile.name;
		sprintf(buf, "%s - %s", szAppName, p);
		WinSetWindowText(hwnd_frame, (unsigned char *)buf);
	    }
	    else
		WinSetWindowText(hwnd_frame, (unsigned char *)szAppName);
	    release_mutex();
	    return 0;
	case WM_GSPERCENT:
	    percent_pending = FALSE;
	    WinInvalidateRect(hwnd_status, (PRECTL)NULL, FALSE);
	    WinUpdateWindow(hwnd_status);
	    return 0;
	case WM_GSTEXTINDEX:
	    make_text_index();
	    text_marking = FALSE;
	    text_mark_first = text_mark_last = -1;
	    return 0;
	case WM_GSWAIT:
	    info_wait((int)mp1);
	    return 0;
	case WM_INITMENU:
	    init_menu((int)mp1);
	    WinSetAccelTable(hab, NULLHANDLE, hwnd_frame);	/* disable keyboard accelerators */
	    break;
	case WM_MENUEND:
	    WinSetAccelTable(hab, haccel, hwnd_frame);	/* reenable keyboard accelerators */
	    break;
	case WM_COMMAND:
	    if (LONGFROMMP(mp1) == IDM_ARGS) {
		/* delayed processing of command line arguments */
		GSVIEW_ARGS *pargs = (GSVIEW_ARGS *)mp2;
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
		else  if (pargs->filename[0]) {
		    FILE *f;
		    /* Only display file if it exists, because starting 
		     * GSview from Desktop menu sets filename d:\Desktop
		     */
		    if ( (f=fopen(pargs->filename,"rb")) != (FILE *)NULL ) {
			fclose(f);
			make_cwd(pargs->filename);
			gsview_displayfile(pargs->filename);
		    }
		}
	    }
	    else if (LONGFROMMP(mp1) == IDM_DROP) {
		char *cmd = (char *)mp2;
		FILE *f;
		/* Only display file if it exists, because starting 
		 * GSview from Desktop menu sets cmd to d:\Desktop
		 */
		if ( (f=fopen(cmd,"rb")) != (FILE *)NULL ) {
		    fclose(f);
		    make_cwd(cmd);
		    gsview_displayfile(cmd);
		}
		free(cmd);
	    }
	    else {
		if (LONGFROMMP(mp1) == IDM_PSTOEPS) {
		    if (psfile.name[0] == '\0')	/* need open file */
			WinSendMsg(hwnd, WM_COMMAND, MPFROMSHORT(IDM_OPEN), 
				MPFROM2SHORT(CMDSRC_OTHER,FALSE));
		}
		gsview_command(LONGFROMMP(mp1));
	    }
	    break;
	case WM_REALIZEPALETTE:
	    if ((bitmap.depth == 8) && display.hasPalMan && display.hpal_exists) {
		APIRET rc;
		hps = WinGetPS(hwnd);
		GpiSelectPalette(hps, display.hpal);
	        if ( (rc = WinRealizePalette(hwnd, hps, &ulclr)) > 0)
		    WinInvalidateRect(hwnd, NULL, FALSE);
		GpiSelectPalette(hps, (HPAL)NULL);
		WinReleasePS(hps);
	        return 0;
	    }
	    break;	/* use default processing */
	case WM_PAINT:
	    if (!image.open || quitnow) {
	        hps = WinBeginPaint(hwnd, (ULONG)0, &rect);
		WinFillRect(hps, &rect, SYSCLR_DIALOGBACKGROUND);
		WinEndPaint(hwnd);
		return 0;
	    }

	    /* Refresh the window each time the WM_PAINT message is received */

	    /* get bmp mutex to stop gs.exe changing bitmap while we paint */
	    image_lock(&image);
	    if (image.open && scan_bitmap(&bitmap)) {
		update_scroll_bars(); /* bitmap has changed */
	    }

	    if (!image.open) {
		image_unlock(&image);
	        hps = WinBeginPaint(hwnd, (ULONG)0, &rect);
		WinFillRect(hps, &rect, CLR_BACKGROUND);
		WinEndPaint(hwnd);
		return 0;
	    }

	    request_mutex();
	    hps = WinBeginPaint(hwnd, (HPS)NULL, &rect);
	    if ((bitmap.depth == 8) && display.hasPalMan && display.hpal_exists) {
	        GpiSelectPalette(hps, display.hpal);
                WinRealizePalette(hwnd, hps, &ulclr);
	        paint_bitmap(hps, &rect, nHscrollPos, nVscrollMax - nVscrollPos);
	        GpiSelectPalette(hps, (HPAL)NULL);
	    }
	    else
	        paint_bitmap(hps, &rect, nHscrollPos, nVscrollMax - nVscrollPos);
	    WinEndPaint(hwnd);
	    release_mutex();
	    if (hwnd == hwnd_fullscreen)
		WinSetPointer(HWND_DESKTOP, 0);
	    image_unlock(&image);
	    return 0;
	case WM_MOVE:
	    /* don't interrogate the window location immediately since */
	    /* it causes the Diamond Stealth VL24 with IBM S3 drivers */
	    /* to corrupt the display */
	    DosSleep(50);
	    if (hwnd_frame) {
		SWP swp;
		WinQueryWindowPos(WinQueryWindow(hwnd, QW_PARENT), &swp);
		if (!(swp.fl & SWP_MINIMIZE)) {
		    if (!(swp.fl & SWP_MAXIMIZE)) {
		        option.img_origin.x = swp.x;
		        option.img_origin.y = swp.y;
		    }
		    option.img_max = ((swp.fl & SWP_MAXIMIZE) != 0);
		}
	    }
	    break;
	case WM_SIZE:
		cyClient = SHORT2FROMMP(mp2);
		cxClient = SHORT1FROMMP(mp2);

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
			/* We just got a GSDLL_SIZE and option.fitpage was TRUE */
			/* enlarge window to smaller of image height */
			/* and height if client extended to bottom of screen */
		        SWP swp;
			DosSleep(50);  /* see note below */
		        WinQueryWindowPos(WinQueryWindow(hwnd, QW_PARENT), &swp);
			cyAdjust = min(image.height, cyClient + swp.y)
			    - cyClient;
		    }
		    else
			cyAdjust = 0;
		}
		cyClient += cyAdjust;
#endif

		nVscrollMax = max(0, image.height - cyClient);
		nVscrollPos = min(nVscrollPos, nVscrollMax);
		scroll_pos.y = nVscrollMax - nVscrollPos;

		if (!image.open)
			cyClient = cyAdjust = nVscrollMax = nVscrollPos = 0;

		if (fullscreen) {
		    /* we don't have scroll bars */
		}
		else {
		    hwndScroll = WinWindowFromID(
			WinQueryWindow(hwnd, QW_PARENT), FID_VERTSCROLL);
		    WinSendMsg(hwndScroll, SBM_SETSCROLLBAR, 
			MPFROMLONG(nVscrollPos), MPFROM2SHORT(0, nVscrollMax));
		    if (image.open)
			WinSendMsg(hwndScroll, SBM_SETTHUMBSIZE, 
			    MPFROM2SHORT(cyClient, image.height),
			    MPFROMLONG(0));
		    else
			WinSendMsg(hwndScroll, SBM_SETTHUMBSIZE, 
				MPFROM2SHORT(1, 1), MPFROMLONG(0));
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
			/* enlarge window to smaller of image width */
			/* and width if client extended to right of screen */
		        SWP swp;
			DosSleep(50);  /* see note below */
		        WinQueryWindowPos(WinQueryWindow(hwnd, QW_PARENT), &swp);
			cxAdjust = min(image.width, 
			    cxClient + WinQuerySysValue(HWND_DESKTOP, SV_CXFULLSCREEN) -
				(swp.x + swp.cx) /* Windows uses rect.right */)
			    - cxClient;
		    }
		    else
			cxAdjust = 0;
		}
		cxClient += cxAdjust;
#endif

		nHscrollMax = max(0, image.width - cxClient);
		nHscrollPos = min(nHscrollPos, nHscrollMax);
		scroll_pos.x = nHscrollPos;

		if (!image.open)
			cxClient = cxAdjust = nHscrollMax = nHscrollPos = 0;

		if (fullscreen) {
		    /* we don't have scroll bars */
		}
		else {
		    hwndScroll = WinWindowFromID(
			WinQueryWindow(hwnd, QW_PARENT), FID_HORZSCROLL);
		    WinSendMsg(hwndScroll, SBM_SETSCROLLBAR, 
			MPFROMLONG(nHscrollPos), MPFROM2SHORT(0, nHscrollMax));
		    if (image.open)
			WinSendMsg(hwndScroll, SBM_SETTHUMBSIZE, 
			    MPFROM2SHORT(cxClient, image.width), 
			    MPFROMLONG(0));
		    else
			WinSendMsg(hwndScroll, SBM_SETTHUMBSIZE, 
			    MPFROM2SHORT(1, 1), MPFROMLONG(0));
		}

		if (!fullscreen && option.fit_page && image.open &&
			(cxAdjust!=0 || cyAdjust!=0) ) {
		        SWP swp;
			/* don't interrogate the window location immediately since */
			/* it causes the Diamond Stealth VL24 with IBM S3 drivers */
			/* to corrupt the display */
			DosSleep(50);
		        WinQueryWindowPos(WinQueryWindow(hwnd, QW_PARENT), &swp);
		        WinSetWindowPos(WinQueryWindow(hwnd, QW_PARENT), 0, 
			    swp.x, swp.y - cyAdjust,
			    swp.cx + cxAdjust, swp.cy + cyAdjust, SWP_SIZE | SWP_MOVE);
		        cxAdjust = cyAdjust = 0;
		}
		display.offset.x = -cxAdjust/2;
		display.offset.y = -cyAdjust/2;
	    	if (!fullscreen && hwnd_frame) {
		    SWP swp;
		    WinQueryWindowPos(WinQueryWindow(hwnd, QW_PARENT), &swp);
		    if (!(swp.fl & SWP_MINIMIZE)) {
			if (!(swp.fl & SWP_MAXIMIZE)) {
		            option.img_size.x = swp.cx;
		            option.img_size.y = swp.cy;
			}
		        option.img_max = ((swp.fl & SWP_MAXIMIZE) != 0);
		    }
		}
		fit_page_enabled = FALSE;
		break;
	case WM_VSCROLL:
	    if (!image.open)
		break;
	    switch(SHORT2FROMMP(mp2)) {
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
#ifdef NOTUSED
		case SB_SLIDERTRACK:
		    /* only do this for fast redraw modes */
		    /* these are 8bit/pixel and 1bit/pixel */
	            if ( (bitmap.depth == 1) ||
		       ((bitmap.depth == 8) && display.hasPalMan && display.hpal_exists) ) {
			nVscrollInc = SHORT1FROMMP(mp2) - nVscrollPos;
		    }
		    else
			nVscrollInc = 0;
#endif
		case SB_SLIDERPOSITION:
			nVscrollInc = SHORT1FROMMP(mp2) - nVscrollPos;
			break;
		case SB_TOP:
			nVscrollInc = -nVscrollPos;
			break;
		case SB_BOTTOM:
			nVscrollInc = nVscrollMax - nVscrollPos;
			break;
		case SB_FIND:
			nVscrollInc = (short)SHORT1FROMMP(mp2);
			break;
		default:
			nVscrollInc = 0;
	    }
	    if ((nVscrollInc = max(-nVscrollPos, 
		min(nVscrollInc, nVscrollMax - nVscrollPos)))!=0) {
		LONG lComplexity;
	        HPOINTER hptr;
		hwndScroll = WinWindowFromID(WinQueryWindow(hwnd, QW_PARENT), FID_VERTSCROLL);
		nVscrollPos += nVscrollInc;
		scroll_pos.y = nVscrollMax - nVscrollPos;
/* KLUDGE BEGIN */
		/* remove pointer to cope with buggy display drivers */
		hptr = WinQueryPointer(HWND_DESKTOP);
		WinSetPointer(HWND_DESKTOP, 0);
/* KLUDGE END */
		lComplexity = WinScrollWindow(hwnd, 0, nVscrollInc, (PRECTL)NULL, (PRECTL)NULL, 
			(HRGN)NULLHANDLE, (PRECTL)&rect, 0);
/* KLUDGE BEGIN */
		/* restore pointer after we removed it */
		WinSetPointer(HWND_DESKTOP, hptr);
/* KLUDGE END */
		WinSendMsg(hwndScroll, SBM_SETPOS, MPFROMLONG(nVscrollPos), 0);
		if (lComplexity != RGN_RECT) {
		    WinInvalidateRect(hwnd, (PRECTL)NULL, FALSE);
		    WinUpdateWindow(hwnd);
		}
		else {
		    /* redraw exposed area */
		    hps = WinGetPS(hwnd);
	            if ((bitmap.depth == 8) && display.hasPalMan && display.hpal_exists) {
	                GpiSelectPalette(hps, display.hpal);
                        WinRealizePalette(hwnd, hps, &ulclr);
		        paint_bitmap(hps, &rect, nHscrollPos, nVscrollMax - nVscrollPos);
	                GpiSelectPalette(hps, (HPAL)NULL);
	            }
		    else
		        paint_bitmap(hps, &rect, nHscrollPos, nVscrollMax - nVscrollPos);
		    WinValidateRect(hwnd, &rect, FALSE);
		    WinReleasePS(hps);
		}
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
		switch(SHORT2FROMMP(mp2)) {
		    case SB_PAGEUP:
		      if ((psfile.dsc != (CDSC *)NULL)
			    && (psfile.pagenum != 1)) {
			WinSendMsg(hwnd, WM_VSCROLL, MPFROMLONG(0), 
				MPFROM2SHORT(0, SB_BOTTOM));
			gsview_command(IDM_PREV);
		      }
		      break;
		    case SB_PAGEDOWN:
		      if ((psfile.dsc == (CDSC *)NULL)
			    || (psfile.pagenum < numpages)) {
			WinSendMsg(hwnd, WM_VSCROLL, MPFROMLONG(0), 
				MPFROM2SHORT(0, SB_TOP));
			gsview_command(IDM_NEXT);
		      }
		      break;
		}
	    }
	    break;
	case WM_HSCROLL:
	    if (!image.open)
		break;
	    switch(SHORT2FROMMP(mp2)) {
		case SB_LINELEFT:
			nHscrollInc = -cxClient/16;
			break;
		case SB_LINERIGHT:
			nHscrollInc = cyClient/16;
			break;
		case SB_PAGELEFT:
			nHscrollInc = min(-1,-cxClient);
			break;
		case SB_PAGERIGHT:
			nHscrollInc = max(1,cxClient);
			break;
#ifdef NOTUSED
		case SB_SLIDERTRACK:
		    /* only do this for fast redraw modes */
		    /* these are 8bit/pixel and 1bit/pixel */
	            if ( (bitmap.depth == 1) ||
		       ((bitmap.depth == 8) && display.hasPalMan && display.hpal_exists) ) {
			nHscrollInc = SHORT1FROMMP(mp2) - nHscrollPos;
		    }
		    else
			nHscrollInc = 0;
#endif
		case SB_SLIDERPOSITION:
			nHscrollInc = SHORT1FROMMP(mp2) - nHscrollPos;
			break;
		case SB_TOP:
			nHscrollInc = -nHscrollPos;
			break;
		case SB_BOTTOM:
			nHscrollInc = nHscrollMax - nHscrollPos;
			break;
		case SB_FIND:
			nHscrollInc = (short)SHORT1FROMMP(mp2);
			break;
		default:
			nHscrollInc = 0;
	    }
	    if ((nHscrollInc = max(-nHscrollPos, 
		min(nHscrollInc, nHscrollMax - nHscrollPos)))!=0) {
		LONG lComplexity;
		HPOINTER hptr;
		hwndScroll = WinWindowFromID(WinQueryWindow(hwnd, QW_PARENT), FID_HORZSCROLL);
		nHscrollPos += nHscrollInc;
		scroll_pos.x = nHscrollPos;
/* KLUDGE BEGIN */
		/* remove pointer to cope with buggy display drivers */
		hptr = WinQueryPointer(HWND_DESKTOP);
		WinSetPointer(HWND_DESKTOP, 0);
/* KLUDGE END */
		lComplexity = WinScrollWindow(hwnd, -nHscrollInc, 0, (PRECTL)NULL, (PRECTL)NULL, 
			(HRGN)NULLHANDLE, (PRECTL)&rect, 0);
/* KLUDGE BEGIN */
		/* restore pointer after we removed it */
		WinSetPointer(HWND_DESKTOP, hptr);
/* KLUDGE END */
		/* need to send next message BEFORE redrawing, otherwise S3 driver screws up */
		WinSendMsg(hwndScroll, SBM_SETPOS, MPFROMLONG(nHscrollPos), 0);
		if (lComplexity != RGN_RECT) {
		    WinInvalidateRect(hwnd, (PRECTL)NULL, FALSE);
		    WinUpdateWindow(hwnd);
		}
		else {
		    /* redraw exposed area */
		    hps = WinGetPS(hwnd);
	            if ((bitmap.depth == 8) && display.hasPalMan && display.hpal_exists) {
	                GpiSelectPalette(hps, display.hpal);
                        WinRealizePalette(hwnd, hps, &ulclr);
		        paint_bitmap(hps, &rect, nHscrollPos, nVscrollMax - nVscrollPos);
	                GpiSelectPalette(hps, (HPAL)NULL);
	            }
		    else
		        paint_bitmap(hps, &rect, nHscrollPos, nVscrollMax - nVscrollPos);
		    WinValidateRect(hwnd, &rect, FALSE);
		    WinReleasePS(hps);
		}
	    }
	    break;
	case WM_CHAR:	/* process keystrokes here */
	    if (!(SHORT1FROMMP(mp1) & KC_KEYUP) && 
		(SHORT1FROMMP(mp1) & KC_VIRTUALKEY)) {
	        /* Process virtual keys only on presses, not key releases */
		USHORT vkey = SHORT2FROMMP(mp2);
		/* Note that the cursor keys these scroll hwnd_image, not hwnd.
		 * This is needed for fullscreen because the key presses
		 * are sent to the frame child window, not the fullscreen
		 * window which has the focus!
		 */ 
		switch(vkey) {
		    case VK_ESC:
			if (fullscreen)
			    gsview_fullscreen_end();
			break;
		    case VK_HOME:
	    		if (SHORT1FROMMP(mp1) & KC_CTRL)
			    WinSendMsg(hwnd_image, WM_HSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_TOP));
			else
			    WinSendMsg(hwnd_image, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_TOP));
		    	break;
		    case VK_END:
	    		if (SHORT1FROMMP(mp1) & KC_CTRL)
			    WinSendMsg(hwnd_image, WM_HSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_BOTTOM));
			else
			    WinSendMsg(hwnd_image, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_BOTTOM));
		    	break;
		    case VK_UP:
	    		if (SHORT1FROMMP(mp1) & KC_CTRL)
		    	    WinSendMsg(hwnd_image, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_PAGEUP));
			else
			    WinSendMsg(hwnd_image, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_LINEUP));
		    	break;
		    case VK_DOWN:
	    		if (SHORT1FROMMP(mp1) & KC_CTRL)
		    	    WinSendMsg(hwnd_image, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_PAGEDOWN));
			else
			    WinSendMsg(hwnd_image, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_LINEDOWN));
		    	break;
		    case VK_PAGEUP:
	    		if (SHORT1FROMMP(mp1) & KC_CTRL)
		    	    WinSendMsg(hwnd_image, WM_HSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_PAGELEFT));
		 	else
		    	    WinSendMsg(hwnd_image, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_PAGEUP));
		    	break;
		    case VK_PAGEDOWN:
	    		if (SHORT1FROMMP(mp1) & KC_CTRL)
		    	    WinSendMsg(hwnd_image, WM_HSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_PAGERIGHT));
		 	else
		    	    WinSendMsg(hwnd_image, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_PAGEDOWN));
		    	break;
		    case VK_LEFT:
	    		if (SHORT1FROMMP(mp1) & KC_CTRL)
		    	    WinSendMsg(hwnd_image, WM_HSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_PAGELEFT));
		  	else
		    	    WinSendMsg(hwnd_image, WM_HSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_LINELEFT));
		    	break;
		    case VK_RIGHT:
	    		if (SHORT1FROMMP(mp1) & KC_CTRL)
		    	    WinSendMsg(hwnd_image, WM_HSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_PAGERIGHT));
		    	else
		    	    WinSendMsg(hwnd_image, WM_HSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_LINERIGHT));
		    	break;
		}
	    }
	    break;
	case WM_BUTTON1DOWN:
		if (hwnd_modeless) 
		    WinPostMsg(hwnd_modeless, WM_COMMAND, MPFROMSHORT(BB_CLICK), MPFROMLONG(0));
		else {
		    int iword ;
		    float x, y;
		    PDFLINK link;
		    BOOL full = fullscreen && (hwnd == hwnd_fullscreen);
		    if (get_cursorpos(&x, &y)) {
			if (!full) {
			    HPS hps = WinGetPS(hwnd);
			    if ( (iword = word_find((int)x, (int)y)) >= 0 ) {
				/* remove any current selection */
				highlight_words(hps, text_mark_first, text_mark_last);
				/* mark new selection */
				text_mark_first = text_mark_last = iword;
				text_marking = TRUE;
				highlight_words(hps, text_mark_first, text_mark_last);
				WinSetCapture(HWND_DESKTOP, hwnd);
			    }
			    else {
				/* remove selection */
				highlight_words(hps, text_mark_first, text_mark_last);
				text_mark_first = text_mark_last = -1;
				WinSetCapture(HWND_DESKTOP, NULLHANDLE);
			    }
			    WinReleasePS(hps);
			}
		    }
		    /* pdfmark link */
		    if (is_link(x, y, &link)) {
			/* found link */
			if (link.page == 0) {
			    if (strcmp(link.action, "GoBack")==0)
				history_back();
			    else
				gserror(IDS_NOLINKTARGET, NULL, 0, SOUND_ERROR);
			}
			else {
			    gsview_unzoom();
			    pending.pagenum = link.page;
			    history_add(pending.pagenum);
			    pending.now = TRUE;
			}
		    }
		    else if (full) {
			WinPostMsg(hwnd_image, WM_COMMAND, 
			    MPFROMSHORT(IDM_NEXT), MPFROMLONG(0));
		    }
		    measure_setpoint(x, y);
		}
		break;
	case WM_BUTTON2DOWN:
		if (fullscreen && (hwnd == hwnd_fullscreen)) {
		    WinPostMsg(hwnd_image, WM_COMMAND, MPFROMSHORT(IDM_PREV), 
			MPFROMLONG(0));
		}
		else { 
		    float x, y;
		    int zwidth, zheight;
		    if (hwnd_modeless) {
			play_sound(SOUND_BUSY);
			break;
		    }
		    request_mutex();
		    if (get_cursorpos(&x, &y)) {
		        zoom = !zoom;
		        display.zoom_xoffset = (int)x;
		        display.zoom_yoffset = (int)y;
			if (cxClient > image.width)
			    zwidth = image.width;
			else
			    zwidth = cxClient;
			if (cyClient > image.height)
			    zheight = image.height;
			else
			    zheight = cyClient;
			x = (scroll_pos.x+zwidth/2)*72.0/option.xdpi;
			y = (scroll_pos.y+zheight/2)*72.0/option.ydpi;
			transform_point(&x, &y);
			x *= option.xdpi/72.0;
			y *= option.ydpi/72.0;
			display.zoom_xoffset -= (int)(x*72.0/option.zoom_xdpi);
			display.zoom_yoffset -= (int)(y*72.0/option.zoom_ydpi);
		    }
		    else {
		        zoom = FALSE;
		    }
		    release_mutex();
		    WinPostMsg(hwnd_bmp, WM_COMMAND, (MPARAM)IDM_ZOOM, MPFROMLONG(0));
		}
		break;
	case WM_MOUSEMOVE:
		/* track cursor and display coordinates */
		request_mutex();
		hps = WinGetPS(hwnd_status);
		WinFillRect(hps, &info_coord, SYSCLR_BUTTONMIDDLE);
		cursorpos_paint(hps);
		WinReleasePS(hps);
		release_mutex();
		/* extend marked text */
		if (text_marking) {
		    int iword ;
		    float x, y;
		    RECTL rect;
		    POINTL pt;
		    do { 
			if (!WinQueryPointerPos(HWND_DESKTOP, &pt))
			    return FALSE;

			WinMapWindowPoints(HWND_DESKTOP, hwnd_bmp, &pt, 1);
			WinQueryWindowRect(hwnd_bmp, &rect);
			if (pt.x > rect.xRight)
		    	    WinSendMsg(hwnd, WM_HSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_LINERIGHT));
			if (pt.x < rect.xLeft)
		    	    WinSendMsg(hwnd, WM_HSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_LINELEFT));
			if (pt.y < rect.yBottom)
		    	    WinSendMsg(hwnd, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_LINEDOWN));
			if (pt.y > rect.yTop)
		    	    WinSendMsg(hwnd, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_LINEUP));
			DosSleep(100);
		    } while ( (pt.x > rect.xRight) || (pt.x < rect.xLeft)
			   || (pt.y > rect.yTop)   || (pt.y < rect.yBottom) );
		    if (get_cursorpos(&x, &y)) {
			if ( (iword = word_find((int)x, (int)y)) >= 0 ) {
			    if (iword != text_mark_last) {
				HPS hps = WinGetPS(hwnd);
				int first, last;
				if ((text_mark_last-text_mark_first >= 0) != (iword-text_mark_first >= 0)) {
				    /* changing direction */
				    /* clear everything */
				    highlight_words(hps, text_mark_first, text_mark_last);
				    /* reinstate first word */
				    text_mark_last = text_mark_first;
				    highlight_words(hps, text_mark_first, text_mark_last);
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
				  highlight_words(hps, first, last);
				  text_mark_last = iword;
				}
				WinReleasePS(hps);
			    }
			}
		    }
		}
		/* update cursor */
		if (fullscreen && (hwnd == hwnd_fullscreen)) {
		    if (szWait[0] != '\0')
    		        WinSetPointer(HWND_DESKTOP, 0);
		    else if (image.open) {
			float x, y;
			PDFLINK link;
			info_link();
			if (get_cursorpos(&x, &y) && is_link(x, y, &link))
			    WinSetPointer(HWND_DESKTOP, hptr_hand);
			else
    		            WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(
				HWND_DESKTOP, SPTR_ARROW, FALSE));
		    }
		    else
    		        WinSetPointer(HWND_DESKTOP, 
			  WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));
		}
		else if (szWait[0] != '\0')
    		    WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE));
		else {
		    float x, y;
		    PDFLINK link;
		    info_link();
		    if (get_cursorpos(&x, &y) && is_link(x, y, &link)) {
			WinSetPointer(HWND_DESKTOP, hptr_hand);
		    }
		    else if (!image.open)
    		        WinSetPointer(HWND_DESKTOP, WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));
		    else
    		        WinSetPointer(HWND_DESKTOP, hptr_crosshair);
		}
		return (MRESULT)TRUE;	/* cursor has been changed */
	    case WM_BUTTON1UP:
		WinSetCapture(HWND_DESKTOP, NULLHANDLE);
		text_marking = FALSE;
		break;
	    case WM_CLOSE:
		if (!query_close()) {
		     return (MRESULT)0;
		}
		break;
	    case WM_DESTROY:
		hwnd_image = hwnd_bmp;
		fullscreen = FALSE;
		if ((hwnd == hwnd_fullscreen) && image.open) {
		    WinShowWindow(hwnd_frame, TRUE);
		    update_scroll_bars();
		    WinInvalidateRect(hwnd_image, NULL, FALSE);
		    WinUpdateWindow(hwnd_image);
		    if (debug)
			gs_addmess("Full Screen finished\r\n");
		}
		hwnd_fullscreen = (HWND)NULL;
		break;
	    case DM_DRAGOVER:
		return DragOver((PDRAGINFO)mp1);
	    case DM_DROP:
		return Drop((PDRAGINFO)mp1);
  }
  /* All messages not handled by the ClientWndProc must be passed */
  /* along to the Presentation Manager for default processing */
  return WinDefWindowProc(hwnd, mess, mp1, mp2);
}

/* map from a coordinate in points, to a coordinate in pixels */
/* This is the opposite of the transform part of get_cursorpos */
/* Used when showing bbox */
void
map_pt_to_pixel(float *x, float *y)
{
	if (zoom) {
	    /* WARNING - this doesn't cope with EPS Clip */
	    *x = (*x - display.zoom_xoffset) * option.zoom_xdpi / 72.0;
	    *y = (*y - display.zoom_yoffset) * option.zoom_ydpi / 72.0;
	    *x = (*x * 72.0 / option.xdpi);
	    *y = (*y * 72.0 / option.ydpi);
	    itransform_point(x, y);
	    *x = (*x * option.xdpi / 72.0) - scroll_pos.x + display.offset.x;
	    *y = (*y * option.ydpi / 72.0) - scroll_pos.y + display.offset.y;
	}
	else {
	    int xoffset = display.xoffset / display.xdpi * 72.0 + 0.5;
	    int yoffset = display.yoffset / display.ydpi * 72.0 + 0.5;
	    *x = *x - xoffset;
	    *y = *y - yoffset;
	    itransform_point(x, y);
	    *x = *x * option.xdpi/72.0
		  - scroll_pos.x + display.offset.x;
	    *y = *y * option.ydpi/72.0
		  - scroll_pos.y + display.offset.y;
	}
}


/* return coordinates in pts from origin of page */
/* For Portrait, this is the bottom left */
/* For rotated pages, this is NOT the bottom left */
BOOL
get_cursorpos(float *x, float *y)
{
RECTL rect;
POINTL pt;
	if (!image.open)
	    return FALSE;
	if (!WinQueryPointerPos(HWND_DESKTOP, &pt))
	    return FALSE;
	WinMapWindowPoints(HWND_DESKTOP, hwnd_image, &pt, 1);
	WinQueryWindowRect(hwnd_image, &rect);
	if (!WinPtInRect(hab, &rect, &pt))
	    return FALSE;
	*x = scroll_pos.x+pt.x-display.offset.x;
	*y = scroll_pos.y+pt.y-display.offset.y;
	transform_cursorpos(x, y);
	return TRUE;
}

void
cursorpos_paint(HPS hps)
{
float x, y;
POINTL pt;
char buf[64];
char fmt[64];
int digits = option.unitfine ? 2 : 0;
    request_mutex();
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
	GpiSetTextAlignment(hps, TA_RIGHT, TA_NORMAL_VERT);
	pt.x = info_coord.xRight - 1;
	pt.y = info_page.y;
	GpiCharStringAt(hps, &pt, strlen(buf), (PBYTE)buf);

        measure_paint(x, y);
    }
    release_mutex();
}

void
info_paint(HWND hwnd)
{
HPS ps;
RECTL rect;
POINTL pt;
int i;
char *p;
char buf[MAXSTR];
char fmt[MAXSTR];
	request_mutex();
        ps = WinBeginPaint(hwnd, (ULONG)0, &rect);
	/* draw background */
	WinFillRect(ps, &rect, SYSCLR_BUTTONMIDDLE);
	GpiSetColor(ps, SYSCLR_BUTTONDARK);
	pt.x = 0; pt.y = 0;
	GpiMove(ps, &pt);
	pt.x = statusbar.x - 1;
	GpiLine(ps, &pt);
	pt.y = statusbar.y - 1;
	GpiLine(ps, &pt);
	GpiSetColor(ps, SYSCLR_BUTTONLIGHT);
	pt.x = 0;
	GpiLine(ps, &pt);
	pt.y = 0;
	GpiLine(ps, &pt);
	GpiSetColor(ps, SYSCLR_BUTTONDEFAULT);

	/* write file information */
	if (psfile.name[0] != '\0') {
	    i = load_string(IDS_FILE, buf, sizeof(buf));
	    if ( (p = strrchr(psfile.name, '\\')) == (char *)NULL )
		p = psfile.name;
	    else
		p++;
	    strcat(buf, p);
	    GpiCharStringAt(ps, &info_file, strlen(buf), (PBYTE)buf);
	    if (szWait[0] != '\0') {
		sprintf(buf, szWait, percent_done);
	        GpiCharStringAt(ps, &info_page, strlen(buf), (PBYTE)buf);
	    }
	    else {
	      if (psfile.dsc!=(CDSC *)NULL) {
		if (on_link) {
		    if ((on_link_page == 0) && on_link_action[0]) {
			strncpy(buf, on_link_action, sizeof(buf)-1);
		    }
		    else {
			load_string(IDS_LINKPAGE, fmt, sizeof(fmt));
			sprintf(buf, fmt, on_link_page);
		    }
		}
		else {
		    int n = map_page(psfile.pagenum - 1);
		    i = load_string(IDS_PAGEINFO, fmt, sizeof(fmt));
		    if ((psfile.dsc->page_count != 0) && (psfile.dsc->page))
			sprintf(buf, fmt, psfile.dsc->page[n].label ? psfile.dsc->page[n].label : " ",psfile.pagenum,  psfile.dsc->page_count);
		    else
			sprintf(buf, fmt, " " ,psfile.pagenum,  psfile.dsc->page_count);
		}
		if (zoom)
		    load_string(IDS_ZOOMED, buf+strlen(buf), sizeof(buf)-strlen(buf));
	        GpiCharStringAt(ps, &info_page, strlen(buf), (PBYTE)buf);
	      }
	      else {
		if (gsdll.state == GS_IDLE)
		    i = load_string(IDS_NOMORE, buf, sizeof(buf));
		else {
		    i = load_string(IDS_PAGE, buf, sizeof(buf));
		    sprintf(buf+i, "%d", psfile.pagenum);
		}
	        GpiCharStringAt(ps, &info_page, strlen(buf), (PBYTE)buf);
	      }
	      /* show coordinate */
	      cursorpos_paint(ps);
	    }
	}
	else {
	    i = load_string(IDS_NOFILE, buf, sizeof(buf));
	    GpiCharStringAt(ps, &info_file, strlen(buf), (PBYTE)buf);
	    if (szWait[0] != '\0') {
		sprintf(buf, szWait, percent_done);
	        GpiCharStringAt(ps, &info_page, strlen(buf), (PBYTE)buf);
	    }
	}
	WinEndPaint(hwnd);
	release_mutex();
}


/* Status Bar Window Proc */
MRESULT EXPENTRY StatusWndProc(HWND hwnd, ULONG mess, 
			MPARAM mp1, MPARAM mp2)
{
  HPS ps;
  switch (mess) {
    case WM_CREATE:
	{
	FONTMETRICS fm;
	POINTL char_size;
	ps = WinGetPS(hwnd);
	GpiQueryFontMetrics(ps, sizeof(FONTMETRICS), &fm);
	char_size.y = fm.lMaxAscender + fm.lMaxDescender + fm.lExternalLeading + 2;
	char_size.x = fm.lAveCharWidth;
	statusbar.y = char_size.y + 2;
	info_file.x = 2;
	info_file.y = fm.lMaxDescender + 3;
	info_coord.xLeft = 32 * char_size.x;
	info_coord.xRight = 52 * char_size.x;
	info_coord.yTop = char_size.y + 1;
	info_coord.yBottom = 1;
	info_page.x = 54 * char_size.x + 2;
	info_page.y = fm.lMaxDescender + 3;
/*
	WinFillRect(ps, SYSCLR_BUTTONMIDDLE);
*/
	WinReleasePS(ps);
	}
	break;
    case WM_PAINT:
	info_paint(hwnd);
	return 0;
  }
  return WinDefWindowProc(hwnd, mess, mp1, mp2);
}

void
delete_buttons(void)
{
    struct button *bp = buttonhead;
    while (bp) {
	buttonhead = bp->next;
	free(bp);
	bp = buttonhead;
    }
}

void
shade_button(HPS hps, struct button *bp, BOOL down)
{
POINTL pt;
	/* draw button shading */
	if (down)
	    GpiSetColor(hps, SYSCLR_BUTTONLIGHT);
	else
	    GpiSetColor(hps, SYSCLR_BUTTONDARK);
	pt.x = bp->rect.xLeft; 
	pt.y = bp->rect.yBottom;
	GpiMove(hps, &pt);
	pt.x = bp->rect.xRight - 1;
	GpiLine(hps, &pt);
	pt.y = bp->rect.yTop - 1;
	GpiLine(hps, &pt);
	if (down)
	    GpiSetColor(hps, SYSCLR_BUTTONDARK);
	else
	    GpiSetColor(hps, SYSCLR_BUTTONLIGHT);
	pt.x = bp->rect.xLeft;
	GpiLine(hps, &pt);
	pt.y = bp->rect.yBottom;
	GpiLine(hps, &pt);
	GpiSetColor(hps, SYSCLR_BUTTONDEFAULT);
}

void
size_buttons(PPOINTL ppt)
{
  struct button *bp;
  int i = 0;
  if (option.button_show) {
    for (bp = buttonhead; bp != (struct button *)NULL; bp = bp->next) {
	bp->rect.xLeft = i * button_shift.x;
	bp->rect.yBottom = ppt->y - button_size.y - i * button_shift.y;
	bp->rect.xRight = bp->rect.xLeft + button_size.x;
	bp->rect.yTop = bp->rect.yBottom + button_size.y;
        i++;
    }
  }
}

/* Button Bar Window Proc */
MRESULT EXPENTRY ButtonWndProc(HWND hwnd, ULONG mess, 
			MPARAM mp1, MPARAM mp2)
{
  HPS hps;
  RECTL rect;
  POINTL pt;
  struct button *bp;
  switch (mess) {
    case WM_SIZE:
	/* reposition buttons */
	pt.x = SHORT1FROMMP(mp2);
	pt.y = SHORT2FROMMP(mp2);
        size_buttons(&pt);
	WinInvalidateRect(hwnd, (PRECTL)NULL, FALSE);
	break;
    case WM_BUTTON1DOWN:
	pt.x = SHORT1FROMMP(mp1);
	pt.y = SHORT2FROMMP(mp1);
	for (bp = buttonhead; bp != (struct button *)NULL; bp = bp->next) {
	    if (WinPtInRect(hab, &(bp->rect), &pt)) {
		button_current = bp;
		button_down = TRUE;
		hps = WinGetPS(hwnd);
		shade_button(hps, bp, TRUE);
		WinReleasePS(hps);
	        WinSetCapture(HWND_DESKTOP, hwnd);
		break;
	    }
	}
	break;
    case WM_BUTTON1UP:
	pt.x = SHORT1FROMMP(mp1);
	pt.y = SHORT2FROMMP(mp1);
	bp = button_current;
	if (bp != (struct button *)NULL) {
	    hps = WinGetPS(hwnd);
	    shade_button(hps, bp, FALSE);
	    WinReleasePS(hps);
	    if (WinPtInRect(hab, &(bp->rect), &pt)) {
	    	WinPostMsg(hwnd_bmp, WM_COMMAND, MPFROMSHORT(bp->id), MPFROMLONG(0));
	    }
	}
	WinSetCapture(HWND_DESKTOP, NULLHANDLE);
	WinInvalidateRect(hwnd_status, NULL, FALSE);
	WinUpdateWindow(hwnd_status);
	button_info = NULL;
	button_current = NULL;
	button_down = FALSE;
	break;
    case WM_MOUSEMOVE:
	pt.x = SHORT1FROMMP(mp1);
	pt.y = SHORT2FROMMP(mp1);
	bp = button_current;
	if (bp != (struct button *)NULL) {
	    if (button_down && !WinPtInRect(hab, &(bp->rect), &pt)) {
		button_down = FALSE;
		hps = WinGetPS(hwnd);
		shade_button(hps, bp, FALSE);
		WinReleasePS(hps);
	    }
	    else if (!button_down && WinPtInRect(hab, &(bp->rect), &pt)) {
		button_down = TRUE;
		hps = WinGetPS(hwnd);
		shade_button(hps, bp, TRUE);
		WinReleasePS(hps);
	    }
	}
	else {
	    /* track cursor and display button function */
	    char buf[64];
	    /* find button */
	    for (bp = buttonhead; bp != (struct button *)NULL; bp = bp->next) {
	      if (WinPtInRect(hab, &(bp->rect), &pt)) {
		break;
	      }
	    }
	    if (bp != button_info) {
	      button_info = bp;
	      if (bp != (struct button *)NULL) {
		if (WinQueryCapture(HWND_DESKTOP) != hwnd)
	            WinSetCapture(HWND_DESKTOP, hwnd); /* track all mouse movements */
	        /* erase page area of status bar */
	        hps = WinGetPS(hwnd_status);
	        rect.xLeft = info_file.x;
	        rect.xRight = statusbar.x - 1;
	        rect.yBottom = info_coord.yBottom;
	        rect.yTop = info_coord.yTop;
	        WinFillRect(hps, &rect, SYSCLR_BUTTONMIDDLE);
		load_string(bp->id, buf, sizeof(buf));
                GpiCharStringAt(hps, &info_file, strlen(buf), (PBYTE)buf);
	        WinReleasePS(hps);
	      }
	      else {
	        WinSetCapture(HWND_DESKTOP, NULLHANDLE);  /* release capture */
		/* put statusbar back to normal */
	        WinInvalidateRect(hwnd_status, NULL, FALSE);
  		WinUpdateWindow(hwnd_status);
	      }
	    }
	}
	break;
    case WM_PAINT:
	hps = WinBeginPaint(hwnd, (ULONG)0, &rect);
	/* draw background */
	WinFillRect(hps, &rect, SYSCLR_BUTTONMIDDLE);
	GpiSetColor(hps, SYSCLR_BUTTONDARK);
	pt.x = buttonbar.x - 1; pt.y = 0;
	GpiMove(hps, &pt);
	pt.y = buttonbar.y - 1;
	GpiLine(hps, &pt);
	GpiSetColor(hps, SYSCLR_BUTTONLIGHT);
	pt.x = 0;
	GpiLine(hps, &pt);
	pt.y = 0;
	GpiLine(hps, &pt);
	GpiSetColor(hps, SYSCLR_BUTTONDEFAULT);
	for (bp = buttonhead; bp != (struct button *)NULL; bp = bp->next) {
	    if (bp->hbitmap) {
		/* draw bitmap */
		pt.x = bp->rect.xLeft;
		pt.y = bp->rect.yBottom;
		WinDrawBitmap(hps, bp->hbitmap, NULL, &pt, 0, 0, DBM_NORMAL);
	    }
	    else {
		/* draw button text */
		pt.x = (bp->rect.xLeft + bp->rect.xRight)/2;
		pt.y = (bp->rect.yBottom + bp->rect.yTop)/2;
	        GpiSetTextAlignment(hps, TA_CENTER, TA_HALF);
                GpiCharStringAt(hps, &pt, strlen(bp->str), (PBYTE)(bp->str));
	    }
	    shade_button(hps, bp, FALSE);
	}
	WinEndPaint(hwnd);
	return 0;
  }
  return WinDefWindowProc(hwnd, mess, mp1, mp2);
}

/* subclassed Frame Window Procedure */
MRESULT EXPENTRY FrameWndProc(HWND hwnd, ULONG mess, 
			MPARAM mp1, MPARAM mp2)
{
MRESULT mr;
int sCount, i;
PSWP pswpNew, pswpClient, pswp;

  switch(mess) {
    case WM_HELP:
	/* HM_DISPLAY_HELP is unreliable for HM_PANELNAME so we lookup */
	/* the id ourselves and use HM_RESOURCEID */
	{   char *prolog, *p;
	    PSHORT pw;
	    APIRET rc;
	    char buf[MAXSTR];
	    load_string(nHelpTopic, buf, sizeof(buf)-1);
	    rc = DosGetResource(hlanguage, RT_RCDATA, IDR_HELP, (PPVOID)&prolog);
	    if (!rc && (prolog != (char *)NULL) ) {
		p = prolog;
		while (*(PSHORT)p) {
		    pw = (PSHORT)p;
		    p += sizeof(SHORT);
		    if (strcmp(p, buf)==0) {
			WinSendMsg(hwnd_help, HM_DISPLAY_HELP, MPFROMLONG(*pw), MPFROMSHORT(HM_RESOURCEID));
			break;
		    }
		    p += strlen(p)+1;
		}
		DosFreeResource(prolog);
	    }
	}
	return 0;
    case WM_CALCFRAMERECT:
	mr =  (*OldFrameWndProc)(hwnd, mess, mp1, mp2);
	/* calculate position of client rectangle */
	if (mr && mp2) {
	    ((PRECTL)mp1)->yTop -= statusbar.y;
	    if (option.button_show)
	        ((PRECTL)mp1)->xLeft += buttonbar.x;
	}
	return mr;
    case WM_FORMATFRAME:
	/* reformat frame to make room for status bar */
	mr =  (*OldFrameWndProc)(hwnd, mess, mp1, mp2);
	sCount = (int)mr;
	pswp = (PSWP)mp1;
	/* find client window */
	pswpClient = ((PSWP)mp1) + (FID_CLIENT - FID_SYSMENU);  /* guess */
	for (i=0; i<sCount; i++)
	    if (pswp[i].hwnd == WinWindowFromID(hwnd, FID_CLIENT)) {
		pswpClient = pswp+i;
		break;
	    }
	pswpNew = ((PSWP)mp1) + sCount;
	*pswpNew = *pswpClient;
	pswpNew->hwnd = WinWindowFromID(hwnd, ID_STATUSBAR);
	pswpNew->cy = statusbar.y;
	pswpClient->cy -= pswpNew->cy;
	pswpNew->y = pswpClient->y + pswpClient->cy;
	/* find vertical scroll bar */
	for (i=0; i<sCount; i++)
	    if (pswp[i].hwnd == WinWindowFromID(hwnd, FID_VERTSCROLL))
		break;
	pswp[i].cy -= pswpNew->cy;	/* reduce height of vert scroll bar */
	/* find menu bar */
	for (i=0; i<sCount; i++)
	    if (pswp[i].hwnd == WinWindowFromID(hwnd, FID_MENU))
		break;
	/* set status bar dimensions from menu bar */
	pswpNew->x = pswp[i].x;
	pswpNew->cx = pswp[i].cx;
	statusbar.x = pswpNew->cx;
	sCount++;
	/* reformat frame to make room for button bar */
	pswpNew++;
	*pswpNew = *pswpClient;
	pswpNew->hwnd = WinWindowFromID(hwnd, ID_BUTTONBAR);
	pswpNew->x = pswpClient->x;
        pswpNew->cx = buttonbar.x;
	pswpNew->y = pswpClient->y;
	pswpNew->cy = pswpClient->cy;
	buttonbar.y = pswpNew->cy;
	pswpNew->fl &= ~(SWP_SHOW | SWP_HIDE);
	sCount++;
	if (option.button_show) {
	    pswpClient->x += buttonbar.x;
	    pswpClient->cx -= buttonbar.x;
	    pswpNew->fl |= SWP_SHOW;
	    /* following is a kludge because ButtonWndProc is receiving WM_SIZE
	       messages in reverse order to WM_FORMATFRAME here! */
	    WinPostMsg(hwnd_button, WM_SIZE, 
		MPFROM2SHORT(pswpNew->cx, pswpNew->cy), 
		MPFROM2SHORT(pswpNew->cx, pswpNew->cy));
	}
	else {
	    pswpNew->fl |= SWP_HIDE;
	}
	return (MRESULT)sCount;
    case WM_QUERYFRAMECTLCOUNT:
	mr =  (*OldFrameWndProc)(hwnd, mess, mp1, mp2);
    	sCount = (int)mr;
    	sCount+=2;
	return (MRESULT)sCount;
    case WM_ACTIVATE:
	if (SHORT1FROMMP(mp1) == TRUE) {	/* activation */
	    if (dfchanged() != 0)
	        gsview_command(IDM_REDISPLAY);
	}
	break;
  }
  return (*OldFrameWndProc)(hwnd, mess, mp1, mp2);
}

MRESULT
DragOver(PDRAGINFO pDragInfo)
{
int cItems, i;
PDRAGITEM pditem;
USHORT usDrop, usDefaultOp;
char dtype[MAXSTR];
char drmf[MAXSTR];
	if (!DrgAccessDraginfo(pDragInfo))
	    return MRFROM2SHORT(DOR_NODROPOP, 0);
	switch(pDragInfo->usOperation) {
	    case DO_DEFAULT:
	    case DO_COPY:
		usDrop = DOR_DROP;
		usDefaultOp = DO_COPY;
		break;
	    default:
	    case DO_UNKNOWN:
	    case DO_MOVE:
		DrgFreeDraginfo(pDragInfo);
		return MRFROM2SHORT(DOR_NODROPOP, 0);
	}
	
	cItems = DrgQueryDragitemCount(pDragInfo);
	for (i=0; i<cItems; i++) {
	    pditem = DrgQueryDragitemPtr(pDragInfo, i);
	    DrgQueryStrName(pditem->hstrType, sizeof(dtype)-1, (PBYTE)dtype);
	    DrgQueryStrName(pditem->hstrRMF, sizeof(drmf)-1, (PBYTE)drmf);
	    if (pditem->fsSupportedOps & DO_COPYABLE)  {
	        if ( DrgVerifyRMF(pditem, (PBYTE)"DRM_OS2FILE", NULL) )	/* changed from "DRF_TEXT" to NULL */
	            usDrop = DOR_DROP;
		else
	            usDrop = DOR_NEVERDROP;
	    }
	    else {
		usDrop = DOR_NODROPOP;
	    }
	}
	DrgFreeDraginfo(pDragInfo);
	return MRFROM2SHORT(usDrop, usDefaultOp);
}

MRESULT
Drop(PDRAGINFO pDragInfo)
{
int cItems, i;
DRAGITEM dragitem;
char dirname[MAXSTR];
char filename[MAXSTR];
char *dragname;
int length;
	if (!DrgAccessDraginfo(pDragInfo))
	    return MRFROM2SHORT(DOR_NODROPOP, 0);
	cItems = DrgQueryDragitemCount(pDragInfo);
	for (i=0; i<cItems; i++) {
	    DrgQueryDragitem(pDragInfo, sizeof(dragitem), &dragitem, i);
	    length = DrgQueryStrName(dragitem.hstrContainerName, 
		sizeof(dirname), (PBYTE)dirname);
	    length += DrgQueryStrName(dragitem.hstrSourceName, 
		sizeof(filename), (PBYTE)filename);
	    dragname = (char *)malloc(length+1);
	    strcpy(dragname, dirname);
	    strcat(dragname, filename);
	    WinPostMsg(hwnd_bmp, WM_COMMAND, (MPARAM)IDM_DROP, MPFROMP(dragname));	/* mp2 is not strictly correct */
	}
	DrgDeleteDraginfoStrHandles(pDragInfo);
	DrgFreeDraginfo(pDragInfo);
	return (MRESULT)0;
}

/* if found word is not visible, scroll window to make it visible */
void
scroll_to_find(void)
{
    RECTL rect, rect_client;
    float x, y;

    request_mutex();
    WinEnableWindowUpdate(hwnd_image, FALSE);
    /* first translate found box to window coordinates */
    x = psfile.text_bbox.llx;
    y = psfile.text_bbox.lly;
    map_pt_to_pixel(&x, &y);
    rect.xLeft   = (int)x;
    rect.yBottom = (int)y;
    x = psfile.text_bbox.urx;
    y = psfile.text_bbox.ury;
    map_pt_to_pixel(&x, &y);
    rect.xRight  = (int)x;
    rect.yTop    = (int)y;

    WinQueryWindowRect(hwnd_image, &rect_client);

    /* scroll to bring the bottom left to the centre of the window */
    if ((rect.xLeft < rect_client.xLeft) || (rect.xRight > rect_client.xRight))
	WinPostMsg(hwnd_image, WM_HSCROLL, 0, MPFROM2SHORT(((rect_client.xRight-rect_client.xLeft)/2) - rect.xLeft, SB_FIND));

    if ((rect.yTop > rect_client.yTop) || (rect.yBottom < rect_client.yBottom))
	WinPostMsg(hwnd_image, WM_VSCROLL, 0, MPFROM2SHORT(((rect_client.yTop-rect_client.yBottom)/2) - ((rect.yBottom+rect.yTop)/2) , SB_FIND));
    WinEnableWindowUpdate(hwnd_image, TRUE);
    release_mutex();
}

/* highlight words from first to last inclusive */
/* first may be > last */
/* word = -1 means nothing to mark */
void
highlight_words(HPS ps, int first, int last)
{
    RECTL rect;
    float x, y;
    TEXTINDEX *text;
    int i;
    if ((first == -1) || (last == -1))
	return;


    if (((unsigned)first > text_index_count) || ((unsigned)last > text_index_count)) {
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
	x = text->bbox.llx;
	y = text->bbox.lly;
	map_pt_to_pixel(&x, &y);
	rect.xLeft   = (int)x;
	rect.yBottom = (int)y;
	x = text->bbox.urx;
	y = text->bbox.ury;
	map_pt_to_pixel(&x, &y);
	rect.xRight  = (int)x;
	rect.yTop    = (int)y;
	if (rect.xRight < rect.xLeft) {
	    int temp = rect.xRight;
	    rect.xRight = rect.xLeft;
	    rect.xLeft = temp;
	}
	if (rect.yTop < rect.yBottom) {
	    int temp = rect.yTop;
	    rect.yTop = rect.yBottom;
	    rect.yBottom = temp;
	}


	/* invert text */
	WinInvertRect(ps, &rect);
    }
}

void
highlight_links(HPS hps)
{
PDFLINK link;
int i = 0;
float x, y;
RECTL rect;
    
    while ( pdf_get_link(i, &link) ) {
	i++;
	if (link.border_width) {
	    /* map bounding box to device coordinates */
	    x = link.bbox.llx;
	    y = link.bbox.lly;
	    map_pt_to_pixel(&x, &y);
	    rect.xLeft   = (int)x;
	    rect.yBottom = (int)y;
	    x = link.bbox.urx;
	    y = link.bbox.ury;
	    map_pt_to_pixel(&x, &y);
	    rect.xRight  = (int)x;
	    rect.yTop    = (int)y;
	    if (rect.yTop < rect.yBottom) {
		int temp = rect.yTop;
		rect.yTop = rect.yBottom;
		rect.yBottom = temp;
	    }
	    if (rect.xLeft > rect.xRight) {
		int temp = rect.xRight;
		rect.xRight = rect.xLeft;
		rect.xLeft = temp;
	    }
	    /* draw border */
	    if (link.colour_valid) {
		GpiSetColor(hps, 
		    GpiQueryColorIndex(hps, 0, 
		    (int)(link.colour_red*255 +0.5) * 65536 +
		        (int)(link.colour_green*255 +0.5) * 256 +
			(int)(link.colour_blue*255 +0.5)) );
		GpiSetMix(hps, FM_OVERPAINT);
	    }
	    else {
		if ((bitmap.depth == 8) && display.hasPalMan && display.hpal_exists)
		    GpiSetColor(hps, 0xf);	/* CYAN (RRGGBB) */
		else
		    GpiSetColor(hps, CLR_CYAN);	/* CYAN index */
		GpiSetMix(hps, FM_XOR);
	    }
	    if ((link.border_width <= 1) && (link.border_xr == 0) && (link.border_yr == 0)) {
		    POINTL pt;
		    pt.x = rect.xLeft;  pt.y = rect.yBottom;
		    GpiMove(hps, &pt);
		    pt.x = rect.xRight;
		    GpiLine(hps, &pt);
		    pt.y = rect.yTop;
		    GpiLine(hps, &pt);
		    pt.x = rect.xLeft;
		    GpiLine(hps, &pt);
		    pt.y = rect.yBottom;
		    GpiLine(hps, &pt);
	    }
	    else {
#define FIXED1 65536
	        POINTL pt;
		ARCPARAMS arc;
		int w2 = (int)((link.border_width+0.5)/2);
		int xr = (int)(link.border_xr+0.5);
		int yr = (int)(link.border_yr+0.5);
		arc.lP = xr;
		arc.lQ = yr;
		arc.lR = 0;
		arc.lS = 0;
		GpiSetArcParams(hps, &arc);
		GpiSetLineWidth(hps, (long)(FIXED1*link.border_width));

		/* rectangle with thick lines and/or rounded corners */
		pt.x = rect.xLeft-w2+xr;
		pt.y = rect.yBottom-w2;
		GpiMove(hps, &pt);
		pt.x = rect.xRight+w2-xr;
		pt.y = rect.yBottom-w2+yr;
		GpiPartialArc(hps, &pt, FIXED1, 270*FIXED1, 90*FIXED1);
		pt.y = rect.yTop+w2-yr;
		GpiPartialArc(hps, &pt, FIXED1, 0*FIXED1, 90*FIXED1);
		pt.x = rect.xLeft-w2+xr;
		GpiPartialArc(hps, &pt, FIXED1, 90*FIXED1, 90*FIXED1);
		pt.y = rect.yBottom-w2+yr;
		GpiPartialArc(hps, &pt, FIXED1, 180*FIXED1, 90*FIXED1);
	    }
	}
    }
}


void
info_link(void)
{
float x, y;
PDFLINK link;
    if (get_cursorpos(&x, &y)) {
	if (is_link(x, y, &link)) {
	    on_link = TRUE;
	    on_link_page = link.page;
	    on_link_action = link.action;
	    WinInvalidateRect(hwnd_status, (PRECTL)NULL, FALSE);
	    WinUpdateWindow(hwnd_status);
	}
	else if (on_link)
	{
	    on_link = FALSE;
	    WinInvalidateRect(hwnd_status, (PRECTL)NULL, FALSE);
	    WinUpdateWindow(hwnd_status);
	}
    }
}

void
gsview_fullscreen_end(void)
{
    if (fullscreen) {
	gs_addmess("Full Screen ending\r\n");
	WinDestroyWindow(hwnd_fullscreen);
    }
}

void
gsview_fullscreen(void)
{
    unsigned char szClass[] = "gvBmpFullscreenClass";  /* class name */
    RECTL rect;
    static BOOL class_registered;

    if (!image.open)
	return;

    if (fullscreen && (hwnd_fullscreen != (HWND)NULL))
	return;

    if (!class_registered) {
	if (!WinRegisterClass(	/* register this window class */
	    hab,			/* anchor block */
	    (PSZ)szClass,		/* class name */
	    (PFNWP) ClientWndProc,	/* window function */
	    CS_SIZEREDRAW |		/* window style */
	    CS_MOVENOTIFY,		
	    0))			/* no storage */
	    return;
	class_registered = TRUE;
    }

    WinQueryWindowRect(HWND_DESKTOP, &rect);
    fullscreen = TRUE;
    hwnd_fullscreen = WinCreateWindow(
    	HWND_DESKTOP,	/* parent */
    	szClass,
    	(PBYTE)"GSview fullscreen client",
	0,		/* not visible */
	rect.xLeft, rect.yBottom, rect.xRight, rect.yTop,
	HWND_DESKTOP,	/* owner */
	HWND_TOP,	/* insert behind */
	0,		/* ID */
	NULL,		/* control data */
	NULL);		/* presentation parameters */

    if (hwnd_fullscreen) {
	hwnd_image = hwnd_fullscreen;
	update_scroll_bars();
	WinShowWindow(hwnd_fullscreen, TRUE);
	gs_addmess("Full Screen started\r\n");
    }
    else {
	gs_addmess("Full Screen failed\r\n");
	fullscreen = FALSE;
    }
}

/* Set the current resolution to fill the window.
 * If neither width nor height match, fit whole page
 * into window.  If either width or height match
 * the window size, fit the height or width respectively.
 */
void 
gsview_fitwin(void) 
{
RECTL rect;
int width, height;
float dpi, xdpi, ydpi;
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
    
    /* get client window size */
    WinQueryWindowRect(hwnd_image, &rect);

    xdpi = (rect.xRight - rect.xLeft) * 72.0 / width;
    ydpi = (rect.yTop - rect.yBottom) * 72.0 / height;

    /* don't need to worry about scroll bars - these are always
     * present in a normal window but outside the client area,
     * and in fullscreen mode (if ever implemented) are not present
     */

    if (display.orientation & 1) {
	/* page is rotated 90 degrees */
	float ftemp;
	ftemp = xdpi;
	xdpi = ydpi;
	ydpi = ftemp;
    }

    if ( ((xdpi + 0.5) > option.xdpi) && (xdpi - 0.5) < option.xdpi) {
	/* Width matches. Set size based on height. */
	dpi = ydpi;
    }
    else if ( ((ydpi + 0.5) > option.ydpi) && (ydpi - 0.5) < option.ydpi) {
	/* Height matches. Set size based on width. */
	dpi = xdpi;
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
    sprintf(buf, "dpi=%f %f\n", xdpi, ydpi);
    gs_addmess(buf);
    sprintf(buf, "final dpi=%f\n", dpi);
    gs_addmess(buf);
    }
#endif
    option.xdpi = option.ydpi = dpi;
    gs_resize();
}
