/* Copyright (C) 1993-2005, Ghostgum Software Pty Ltd.  All rights reserved.
  
  This file is part of GSview.
   
  This program is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  Refer to the GSview Licence (the "Licence") 
  for full details.
   
  Every copy of GSview must include a copy of the Licence, normally in a 
  plain ASCII text file named LICENCE.  The Licence grants you the right 
  to copy, modify and redistribute GSview, but only under certain conditions 
  described in the Licence.  Among other things, the Licence requires that 
  the copyright notice and this notice be preserved on all copies.
*/

/* gvcdll.c */
/* GS DLL associated routines */
#include "gvc.h"
#include <stdarg.h>

#define GS_UNSAFE 703

GSDLL gsdll;		/* the main DLL structure */
IMAGE image;		/* display device */
VIEW view;
PENDING pending;	/* operations that must wait */
int execute_code;	/* return code from gsapi_run_string_continue */
BOOL in_pstotext = FALSE;


/* forward declarations */
#ifndef VIEWONLY
int gs_process_pstotext(void);
#endif
int gsview_page_orientation(int page);
void d_calc_resize(int pagenum, float *pwidth, float *pheight, 
    int *pxoffset, int *pyoffset);
int d_resize(int pagenum);
int d_save(void);
int d_restore(void);
int d_init1(void);
int d_init2(void);
int d_permitread(void);
int d_pdf_page(int pagenum);
int get_gs_input(char *buf, int blen);
int send_trailer(void);
int send_document(void);
BOOL gs_process_trailer(void);
int gs_process_prepare_input(PENDING *ppend);
int gs_process_loop2(PENDING *ppend);
int gs_process_pending(PENDING *lpending);
int gs_process_loop1(void);

int
gs_execute(const char *str, int len)
{
    int exit_code;
    if (gsdll.run_string_continue == NULL)
	return 255;
    execute_code = gsdll.run_string_continue(gsdll.minst, str, len, 
	0, &exit_code);
    if (execute_code == e_NeedInput)
	execute_code = 0;	/* normal return */
#ifdef NOTUSED
    if (debug & DEBUG_GENERAL) {
	char buf[MAXSTR];
	gs_addmessf("\n----Begin %d bytes----\n", len);
	gs_addmess_count(str, len);
	gs_addmessf("\n----End of %d bytes----\n", len);
/*
*/
	sprintf(buf, "gsdll.run_string_continue returns %d\n", execute_code);
	gs_addmess(buf);
    }
#endif
    return execute_code;
}

/* returns error code from GS */
int
gs_printf(const char *fmt, ...)
{
va_list args;
int count;
int code;
char buf[2048];
	if (strlen(fmt) > sizeof(buf) - 256) {
	    message_box(TEXT("gs_printf buffer overrun\n"), 0);
	    return 0;
	}
	va_start(args,fmt);
	count = vsprintf(buf,fmt,args);
	if (debug & DEBUG_GENERAL)
	    gs_addmess(buf);
	code = gs_execute(buf, count);
	va_end(args);
	if (count >= (int)sizeof(buf)) {
	    debug |= DEBUG_GENERAL | DEBUG_LOG;
	    gs_addmess("gs_printf buffer overrun\n");
#ifndef VIEWONLY
	    gs_addmess(buf);
	    message_box(TEXT("Please send c:\\gsview.txt to the author"), 0);
#endif
	}
	return code;
}

/* private funtions */

/* Get auto orientation for specified page */
int
gsview_page_orientation(int page)
{
    CDSC *dsc = psfile.dsc;
    int orientation = IDM_PORTRAIT;

    if (psfile.ispdf)
	return pdf_orientation(page);

    if (dsc == (CDSC *)NULL)
	return IDM_PORTRAIT;

    if (dsc->page_orientation == CDSC_LANDSCAPE)
	orientation = IDM_LANDSCAPE;
    if ((page >=1) && (page <= (int)dsc->page_count)) {
	/* check for page orientation */
	if (dsc->page[map_page(page-1)].orientation == CDSC_PORTRAIT)
	    orientation = IDM_PORTRAIT;
	if (dsc->page[map_page(page-1)].orientation == CDSC_LANDSCAPE)
	    orientation = IDM_LANDSCAPE;
    }
    return orientation;
}

typedef struct viewing_orient_s {
    int orientation;
    CDSCCTM viewing;
} viewing_orient_t;

const viewing_orient_t viewing_orients[] = {
    {IDM_PORTRAIT, {1,0,0,1}},
    {IDM_LANDSCAPE, {0,1,-1,0}},
    {IDM_UPSIDEDOWN, {-1,0,0,-1}},
    {IDM_SEASCAPE, {0,-1,1,0}}
};

static int find_viewing_orientation(CDSCCTM *viewing)
{
    int i;
    if (viewing == NULL)
	return 0;
    for (i=0; i<sizeof(viewing_orients)/sizeof(viewing_orients[0]); i++) {
	if  ((viewing->xx == viewing_orients[i].viewing.xx) &&
	     (viewing->xy == viewing_orients[i].viewing.xy) &&
	     (viewing->yx == viewing_orients[i].viewing.yx) &&
	     (viewing->yy == viewing_orients[i].viewing.yy))
	    return viewing_orients[i].orientation;    
    }
    return 0;
}

/* Get auto viewing orientation for specified page */
/* Return 0 if unknown, of IDM_PORTRAIT etc if known */
static int
gsview_viewing_orientation(int page)
{
    CDSC *dsc = psfile.dsc;
    int viewing = 0;
    
    if (psfile.ispdf)
	return viewing;	/* do nothing */

    if (dsc == (CDSC *)NULL)
	return viewing;

    if ((page >=1) && (page <= (int)dsc->page_count)) {
	viewing = find_viewing_orientation(
			dsc->page[map_page(page-1)].viewing_orientation);
    }
    if (viewing == 0)
	viewing = find_viewing_orientation(dsc->viewing_orientation);
    return viewing;
}

/* get orientation for display of given page */
int
d_orientation(int pagenum)
{
int orientation;
int viewing = CDSC_ORIENT_UNKNOWN;
    if (option.auto_orientation == TRUE) {
	orientation = gsview_page_orientation(pagenum);
	viewing = gsview_viewing_orientation(pagenum);
	if (viewing != 0)
	    orientation = viewing;
    }
    else
	orientation = option.orientation;
    switch (orientation) {
	case IDM_LANDSCAPE:
	    if (option.swap_landscape && !viewing)
		return 1;
	    return 3;
	case IDM_SEASCAPE:
	    if (option.swap_landscape && !viewing)
		return 3;
	    return 1;
	case IDM_PORTRAIT:
	    return 0;
	case IDM_UPSIDEDOWN:
	    return 2;
    }
    return 0;
}

void
d_calc_resize(int pagenum, float *pwidth, float *pheight, int *pxoffset, int *pyoffset)
{
    CDSC *dsc;

    float	width;		  /* page width in 1/72" */
    float	height;		  /* page height in 1/72" */
    int		xoffset;	  /* page origin offset in 1/72" */
    int		yoffset;	  /* page origin offset in 1/72" */

    dsc = psfile.dsc;
    if (dsc != (CDSC *)NULL) {
	if (pagenum < 0)
	    pagenum = psfile.pagenum;
	if (dsc->page_count && (pagenum > (int)dsc->page_count))
	     pagenum = dsc->page_count;
	if (pagenum < 1)
	    pagenum = 1;
    }

    /* calculate new size */
    if ( (option.xdpi == 0.0) || (option.ydpi == 0.0) )
	    option.xdpi = option.ydpi = DEFAULT_RESOLUTION;
    display.epsf_clipped = FALSE;

    display.xdpi = option.xdpi;
    display.ydpi = option.ydpi;

    if (zoom && (dsc!=(CDSC *)NULL)) {
	if (debug & DEBUG_GENERAL)
	    gs_addmess("d_calc_resize: zoomed\n");
	display.xdpi = option.zoom_xdpi;
	display.ydpi = option.zoom_ydpi;
	width = (float)(display.width * 72.0 / display.xdpi);
	height = (float)(display.height * 72.0 / display.ydpi);
	xoffset = display.zoom_xoffset;
	yoffset = display.zoom_yoffset;
    }
    else if ( (dsc != (CDSC *)NULL) && 
	       ((dsc->epsf || psfile.ispdf) && option.epsf_clip) ) {
	if (debug & DEBUG_GENERAL)
	    gs_addmess("d_calc_resize: cropped EPS or PDF\n");
	display.epsf_clipped = TRUE;
	if (psfile.ispdf) {
	    /* PDF crop box is stored in page bbox */
	    CDSCBBOX *bbox = NULL;
	    /* PDF media size is stored in page media mediabox */
	    CDSCBBOX *mediabox = NULL;
	    if ( (pagenum >= 1) &&
	         (pagenum <= (int)psfile.dsc->page_count) ) {
		bbox = psfile.dsc->page[pagenum-1].bbox;
	        if (psfile.dsc->page[pagenum-1].media)
	            mediabox = psfile.dsc->page[pagenum-1].media->mediabox;
	    }
	    if (bbox) {
		width = (float)(bbox->urx - bbox->llx);
		height = (float)(bbox->ury - bbox->lly);
		xoffset = bbox->llx;
		yoffset = bbox->lly;
	    }
	    else if (mediabox) {
		width = (float)(mediabox->urx - mediabox->llx);
		height = (float)(mediabox->ury - mediabox->lly);
		xoffset = mediabox->llx;
		yoffset = mediabox->lly;
	    }
	    else {
		width = (float)get_paper_width();
		height = (float)get_paper_height();
		xoffset = 0;
		yoffset = 0;
	    }
	}
	else {
	    if (dsc->bbox != (CDSCBBOX *)NULL) {
		width = (float)(dsc->bbox->urx - dsc->bbox->llx);
		height = (float)(dsc->bbox->ury - dsc->bbox->lly);
		xoffset = dsc->bbox->llx;
		yoffset = dsc->bbox->lly;
	    }
	    else {
		width = (float)get_paper_width();
		height = (float)get_paper_height();
		xoffset = 0;
		yoffset = 0;
	    }
	}
	if (width <= 0)
	    display.epsf_clipped = FALSE;
	if (height <= 0)
	    display.epsf_clipped = FALSE;
    }
    else if ((dsc != (CDSC *)NULL) && psfile.ispdf) {
	CDSCBBOX *mediabox = NULL;
	if (debug & DEBUG_GENERAL)
	    gs_addmess("d_calc_resize: PDF\n");
	/* PDF media size is stored in page media mediabox */
	if ( (pagenum >= 1) &&
	     (pagenum <= (int)psfile.dsc->page_count) &&
	     (psfile.dsc->page[pagenum-1].media) &&
	     (psfile.dsc->page[pagenum-1].media->mediabox) )
	    mediabox = psfile.dsc->page[pagenum-1].media->mediabox;

	if (mediabox) {
	    width = (float)(mediabox->urx - mediabox->llx);
	    height = (float)(mediabox->ury - mediabox->lly);
	    xoffset = mediabox->llx;
	    yoffset = mediabox->lly;
	}
	else {
	    width = (float)get_paper_width();
	    height = (float)get_paper_height();
	    xoffset = 0;
	    yoffset = 0;
	}
    }
    else {
	/* !zooming && !display.epsf_clipped && !ispdf */
	if (debug & DEBUG_GENERAL)
	    gs_addmess("d_resize: other\n");
	xoffset = 0;
	yoffset = 0;
	width = (float)get_paper_width();
	height = (float)get_paper_height();
    }

    *pwidth = width;
    *pheight = height;
    *pxoffset = xoffset;
    *pyoffset = yoffset;
}

/* calculate new size then set it */
int
d_resize(int pagenum)
{
int code = 0;
float	width;		  /* page width in 1/72" */
float	height;		  /* page height in 1/72" */
int	xoffset;	  /* page origin offset in 1/72" */
int	yoffset;	  /* page origin offset in 1/72" */

    d_calc_resize(pagenum, &width, &height, &xoffset, &yoffset);
    if (debug & DEBUG_GENERAL)
	gs_addmessf("d_resize: width=%f height=%f\n", width, height);

    display.orientation = d_orientation(psfile.pagenum);

    display.width  = (unsigned int)(width  * display.xdpi / 72.0 + 0.5);
    display.height = (unsigned int)(height * display.ydpi / 72.0 + 0.5);

    display.xoffset = (unsigned int)(xoffset * display.xdpi / 72.0);
    display.yoffset = (unsigned int)(yoffset * display.ydpi / 72.0);

    /* set new size */

    /* There is a local/global allocation problem in GS 7.04,
     * 7.05, 8.00 which we try to avoid by always calling setuserparams, 
     * .locksafe and setpagedevice with global objects.  
     * The problem is that you can insert a local object with 
     * setpagedevice, but if you later call currentpagedevice while 
     * in global allocation, it will fail trying to put the local 
     * object into a global dictionary.
     */
    if (!code) 	/* local/global bug in GS, see note above */
	code = gs_printf("currentglobal true setglobal\n");
    if (!code)
	code = gs_printf("GSview /HWResolution [%f %f] put\n", display.xdpi, display.ydpi);
    if (!code)   /* need to improve this for PDF - need page bbox */
	code = gs_printf("GSview /PageSize [%f %f] put\n", width, height);
    if ((xoffset!=0) || (yoffset!=0)) {
	if (!code)
	    code = gs_printf("GSview /ImagingBBox [%d %d %f %f] put\n", 
		xoffset, yoffset, xoffset + width, yoffset + height);
    }
    else {
	if (!code)
	    code = gs_printf("GSview /ImagingBBox null put\n"); 
    }
    if (!code)
	code = gs_printf("GSview /Orientation %d put\n", display.orientation);
    if (!code)
	code = gs_printf("GSview /TextAlphaBits %d put\n", real_depth(option.depth) >= 8 ? option.alpha_text : 1);
    if (!code)
	code = gs_printf("GSview /GraphicsAlphaBits %d put\n", real_depth(option.depth) >= 8 ?  option.alpha_graphics : 1);
    if (!code) 	/* local/global bug in GS, see note above */
	code = gs_printf("setglobal\n");

    return code;
}


/* save GSview state */
int
d_save(void)
{
int code;
    code = gs_printf("/GSview_Save save def\n");
    display.saved = TRUE;
    return code;
}

/* restore GS */
/* assume restore is quick - don't check message loop */
int
d_restore(void)
{
int code;
    code = gs_printf("userdict /gsview_eps_countcheck known {gsview_eps_countcheck} if flush\n");
    if (!code)
        code = gs_printf("//systemdict /clear get exec //systemdict /cleardictstack get exec\n");
#ifdef NOTUSED
    /* Don't do a restore, because we are within a stopped and it won't work */
    if (!code)
        code = gs_printf("GSview_Save restore\n");
#endif
    display.saved = FALSE;
    return code;
}


/* create GSview dictionary */
int
d_init1(void)
{
    /* create GSview dictionary */
    /* local/global bug in GS, see note above */
    return gs_printf("currentglobal true setglobal\n\
/GSview 8 dict def GSview begin\n/PageSize [612 792] def\n\
/ImagingBBox null def\n/Orientation 0 def\n/HWResolution [96.0 96.0] def\n\
/Size PageSize def\n/PageOffset [0 0] def\n\
/TextAlphaBits 1 def /GraphicsAlphaBits 1 def\nend\n\
setglobal\n");
}

#ifdef UNIX
long dmode1 = 
     DISPLAY_COLORS_GRAY | DISPLAY_ALPHA_NONE | DISPLAY_DEPTH_8 | 
     DISPLAY_BIGENDIAN | DISPLAY_TOPFIRST; /* not used */
long dmode4 = 
     DISPLAY_COLORS_GRAY | DISPLAY_ALPHA_NONE | DISPLAY_DEPTH_8 | 
     DISPLAY_BIGENDIAN | DISPLAY_TOPFIRST; /* not used */
long dmode8 = 
     DISPLAY_COLORS_NATIVE | DISPLAY_ALPHA_NONE | DISPLAY_DEPTH_8 | 
     DISPLAY_BIGENDIAN | DISPLAY_TOPFIRST;
long dmode24 = 
     DISPLAY_COLORS_RGB | DISPLAY_ALPHA_NONE | DISPLAY_DEPTH_8 | 
     DISPLAY_BIGENDIAN | DISPLAY_TOPFIRST;
#else
long dmode1 = 
     DISPLAY_COLORS_NATIVE | DISPLAY_ALPHA_NONE | DISPLAY_DEPTH_1 | 
     DISPLAY_LITTLEENDIAN | DISPLAY_BOTTOMFIRST;
long dmode4 = 
     DISPLAY_COLORS_NATIVE | DISPLAY_ALPHA_NONE | DISPLAY_DEPTH_8 | 
     DISPLAY_LITTLEENDIAN | DISPLAY_BOTTOMFIRST;
long dmode8 = 
     DISPLAY_COLORS_NATIVE | DISPLAY_ALPHA_NONE | DISPLAY_DEPTH_8 | 
     DISPLAY_LITTLEENDIAN | DISPLAY_BOTTOMFIRST;
long dmode24 = 
     DISPLAY_COLORS_RGB | DISPLAY_ALPHA_NONE | DISPLAY_DEPTH_8 | 
     DISPLAY_LITTLEENDIAN | DISPLAY_BOTTOMFIRST;
#endif

int
d_permitread(void)
{
int code = 0;
char filename[MAXSTR+MAXSTR];
int i;
const char *p;
    /* Allow document files to be opened */
    if (option.safer && (gsdll.revision_number > GS_UNSAFE)) {
 	/* local/global bug in GS, see note above */
	code = gs_printf( "currentglobal true setglobal\n");
	if (!code)
	    code = gs_printf("<<\n");
	if (!code)
	    code = gs_printf(" /PermitFileReading [\n");
	if (!code && psfile.name[0]) {
	    for (p=psfile.name, i=0; *p && i < sizeof(filename)-2; p++) {
		if (*p == '\\') {
		    filename[i++] = '\\';
		    filename[i++] = '\\';
		    filename[i++] = '\\';
		}
		if ((*p == '(') || (*p == ')'))
		    filename[i++] = '\\';
		filename[i++] = *p;
	    }
	    filename[i] = '\0';
	    code = gs_printf("  (%s)\n", filename);
	    while (--i) {
		if ((filename[i] == '\\') || (filename[i] == '/')) {
		    filename[++i] = '*';
		    filename[++i] = '\0';
		    break;
		}
	    }
	    if (i)
	        code = gs_printf("  (%s)\n", filename);
	}
	if (!code && psfile.tname[0]) {
	    for (p=psfile.tname, i=0; *p && i < sizeof(filename)-2; p++) {
		if (*p == '\\') {
		    filename[i++] = '\\';
		    filename[i++] = '\\';
		    filename[i++] = '\\';
		}
		if ((*p == '(') || (*p == ')'))
		    filename[i++] = '\\';
		filename[i++] = *p;
	    }
	    filename[i] = '\0';
	    code = gs_printf("  (%s)\n", filename);
	}

	if (!code)
	    code = gs_printf(" ]\n");
	if (!code)
	    code = gs_printf(" /PermitFileWriting []\n");
	if (!code)
	    code = gs_printf(" /PermitFileControl []\n");
	if (!code)
	    code = gs_printf(" >> setuserparams\n");
	if (!code) 	/* local/global bug in GS, see note above */
	    code = gs_printf( "setglobal\n");
    }
    if (code)
	gs_addmessf("Failed to setuserparams for SAFER\n");
    return code;
}


/* open device and install viewer hooks */
int
d_init2(void)
{
int code = 0;
int depth;
long dmode;

    if (!code)
        code = d_permitread();

    /* calculate depth */
    depth = real_depth(option.depth);

    /* set the device depth and size */
    /* open device */
    view.img->ignore_sync = TRUE;	 /* ignore GSDLL_SYNC from this setpagedevice */
    switch (depth) {
	case 1:
	    dmode = dmode1;
	    break;
	case 4:
	    dmode = dmode4;
	    break;
	case 8:
	    dmode = dmode8;
	    break;
	default:
	case 24:
	    dmode = dmode24;
	    break;
    }

    if (!code) 	/* local/global bug in GS, see note above */
	code = gs_printf("currentglobal true setglobal\n");
    if (!code) {
	/* 64-bit support required changed from -dDisplayFormat
	 * to -sDisplayFormat
	 */
#ifdef _Windows
#define HEX64 "%I64x"
#else
#define HEX64 "%llx"
#endif
	if ( (gsdll.revision_number >= 851) ||
	  ((gsdll.revision_number >= 816) && (gsdll.revision_number < 830)) ||
	  (sizeof(&view) == 8)
	) {
	    char fmt[MAXSTR];
	    sprintf(fmt, "<< /OutputDevice /%%s /DisplayFormat 16#%%lx /DisplayHandle (16#%s)\n", sizeof(&view) == 4 ? "%lx" : HEX64);
	    code = gs_printf(fmt, DEVICENAME, dmode, &view);
	}
	else 
	    code = gs_printf("<< /OutputDevice /%s /DisplayFormat 16#%lx /DisplayHandle %ld\n",
		DEVICENAME, dmode, &view);
#undef HEX64
    }
    if (!code && option.safer && (gsdll.revision_number > GS_UNSAFE))
	code = gs_printf("/.LockSafetyParams true\n");
    if (!code)
        code = send_prolog(IDR_VIEWER);
    if (!code)
        code = gs_printf(">> setpagedevice\n");
    if (!code) 	/* local/global bug in GS, see note above */
	code = gs_printf("setglobal\n");

    if (!code && option.safer && (gsdll.revision_number > GS_UNSAFE)) {
	if (gsdll.revision_number == 704) {
	    /* There is a local/global allocation problem in GS 7.04 */
	    /* which we try to avoid with the following */
	    code = gs_printf(
		"currentglobal true setglobal .locksafe setglobal\n");

	    /* .locksafe disabled ViewerPreProcess */
	    /* Put it back again */
	    if (!code)
		code = gs_printf("currentglobal true setglobal\n");
	    if (!code)
		code = gs_printf(
		    "currentpagedevice /ViewerPreProcess known not { <<\n");
	    if (!code)
		code = send_prolog(IDR_VIEWER);
	    if (!code)
		code = gs_printf(">> setpagedevice } if\n");
	    if (!code)
		code = gs_printf("setglobal\n");
        }
	else  {
	    /* code = gs_printf(".locksafe\n"); */
	    /* local/global bug in GS, see note above */
	    code = gs_printf(
		"currentglobal true setglobal .locksafe setglobal\n");
/* FIX: .locksafe in 7.06 pre-release is buggy and leaves items on the stack */
	    if (!code)
		code = gs_printf("clear\n");
	}
    }

    if (code) {
	char buf[256];
	sprintf(buf,"Failed to open device or install ViewerPreProcess hook: returns %d\n", code);
	gs_addmess(buf);
	pending.unload = TRUE;
	if ( (code == e_limitcheck)
	     || (code == e_invalidexit)
	     || (code == e_VMerror)
	   ) {
	    gs_addmess("Page size may have been too large or resolution too high.\nResetting page size and resolution\n");
	    if (option.xdpi > DEFAULT_RESOLUTION)
		option.xdpi = option.ydpi = DEFAULT_RESOLUTION;
	    if (zoom)
		option.zoom_xdpi = option.zoom_ydpi = 300;
	    post_img_message(WM_COMMAND, IDM_A4);
	}
    }

    /* We need to have one level of save after opening the display device
     * with setpagedevice, to avoid problems with grestoreall.
     * We can't do a restore because we are inside the stopped,
     * but that doesn't matter because we are closing ghostscript
     * anyway at that stage.
     */
    d_save();

    if (!code)
	/* run Ghostscript with a stopped */
	code = gs_printf("\
/GSview_stopped\n\
{\n\
 { currentfile run } stopped\n\
 { $error /errorname get /VMerror eq\n\
  { $error begin command errorname end signalerror }\n\
  { handleerror quit }\n\
  ifelse\n\
 }\n\
 if\n\
}\n\
def\n\
GSview_stopped\n");

    if (!code)
	display.init = TRUE;
    return code;
}

int
d_pdf_page(int pagenum)
{
    int code;
    code = pdf_page_init(pagenum);
    if (code)
	gs_addmess("pdf_page_init failed\n");
    if (!zoom) {
	if (!code) {
	    code = d_resize(pagenum);
	    if (code)
		gs_addmess("d_resize failed\n");
	}
	if (!code) {
	    /* local/global bug in GS, see note above */
	    code = gs_printf("currentglobal true setglobal\n");
	    if (!code) {
		if (in_pstotext)
		    code = gs_printf("<< >> setpagedevice\n");
		else
		    code = gs_printf("<< >> //systemdict /setpagedevice get exec\n");
	    }
	    if (!code) 	/* local/global bug in GS, see note above */
		code = gs_printf("setglobal\n");
	    if (code)
		gs_addmess("setpagedevice failed\n");
	}
    }
    if (!code) {
	code = pdf_page();
	if (code)
	    gs_addmess("pdf_page failed\n");
    }
    return code;
}

/* input for Ghostscript DLL */
/* get_gs_input() copies/reads from gs_input[] to the supplied buffer */
/* copy/read at most blen bytes */
/* returns number of bytes copied to buffer */
int
get_gs_input(char *buf, int blen)
{
unsigned long len;
GSINPUT *entry;
GSDLL_INPUT *input = &view.input;


    if (input->index >= input->count) {
	return 0; /* no more */
    }

    entry = &input->section[input->index];
    len = entry->end - entry->ptr;
    len = min(len, (unsigned long)blen);

    if (entry->seek) {
	gfile_seek(psfile.file, entry->ptr, gfile_begin);
	entry->seek = FALSE;
    }

    len = gfile_read(psfile.file, buf, (int)len);
    entry->ptr += len;

    /* check if we need to move to next item */
    if (entry->ptr >= entry->end) {
        input->index++;
	/* do initial seek */
        if (input->index < input->count) {
	    gfile_seek(psfile.file, input->section[input->index].ptr, 
		gfile_begin);
	    input->section[input->index].seek = FALSE;
	}
    }
    return (int)len;
}


/* assume this is quick - don't check message loop */
int
send_trailer(void)
{
char buf[MAXSTR];
unsigned long len;
unsigned long ptr;
int code = 0;
int prevlen = 0;
CDSC *dsc = psfile.dsc;
    if (psfile.file == NULL)
	return 0;	/* file has been deleted */
    ptr = dsc->begintrailer;
    gfile_seek(psfile.file, ptr, gfile_begin);
    len = dsc->endtrailer - ptr;
    len = min(len, sizeof(buf));
    if (debug & DEBUG_GENERAL)
	gs_addmess("send_trailer\n");
    while (!code && len && (len = gfile_read(psfile.file, buf, (int)len))!=0) {
	code = gs_execute(buf, (int)len);
	prevlen = (int)len;
        ptr += len;
	len = dsc->endtrailer - ptr;
	len = min(len, sizeof(buf));
    }
#ifndef VIEWONLY
    if (code) {
	gs_addmess("\n---Error in document trailer---\n-------\n");
        gs_addmess_count(buf, prevlen);
	gs_addmess("\n-------\n");
    }
#endif
    return code;
}

/* This doesn't actually send anything to Ghostscript */
/* Instead it prepares a list of what will be sent */
int
send_document(void)
{
char filename[MAXSTR];
char buf[MAXSTR+50];
char *p;
CDSC *dsc = psfile.dsc;
GSDLL_INPUT *input = &view.input;
    if (debug & DEBUG_GENERAL)
	gs_addmess("send_document:\n");
    if (psfile.file == NULL)
	return -1;
    strcpy(filename, psfile.name);
    for (p=filename; *p; p++)
	if (*p == '\\')
	    *p = '/';

    if (dsc != (CDSC *)NULL) {
	if (dsc->page_count && (dsc->page != 0)) {
	    int i, page;
	    i = 0;
	    page = map_page(psfile.pagenum-1);
	    if (display.need_header) {
		sprintf(buf, "Displaying DSC file %s\n", filename);
		gs_addmess(buf);
		/* add header */
		if (dsc->endcomments - dsc->begincomments != 0) {
		    if (debug & DEBUG_GENERAL) {
			sprintf(buf, "adding header %ld %ld\n", 
			    (long)(dsc->begincomments), (long)(dsc->endcomments));
		 	gs_addmess(buf);
		    }
		    input->section[i].ptr = dsc->begincomments;
		    input->section[i].end = dsc->endcomments;
		    input->section[i].seek = TRUE;
		    i++;
		}
		/* add defaults */
		if (dsc->enddefaults - dsc->begindefaults != 0) {
		    if (debug & DEBUG_GENERAL) {
			sprintf(buf, "adding defaults %ld %ld\n", 
			    (long)(dsc->begindefaults), (long)(dsc->enddefaults));
		 	gs_addmess(buf);
		    }
		    input->section[i].ptr = dsc->begindefaults;
		    input->section[i].end = dsc->enddefaults;
		    input->section[i].seek = TRUE;
		    i++;
		}
		/* add prolog */
		if (dsc->endprolog - dsc->beginprolog != 0) {
		    if (debug & DEBUG_GENERAL) {
			sprintf(buf, "adding prolog %ld %ld\n", 
			    (long)(dsc->beginprolog), (long)(dsc->endprolog));
		 	gs_addmess(buf);
		    }
		    input->section[i].ptr = dsc->beginprolog;
		    input->section[i].end = dsc->endprolog;
		    input->section[i].seek = TRUE;
		    i++;
		}
		/* add setup */
		if (dsc->endsetup - dsc->beginsetup != 0) {
		    if (debug & DEBUG_GENERAL) {
			sprintf(buf, "adding setup %ld %ld\n", 
			    (long)(dsc->beginsetup), (long)(dsc->endsetup));
		 	gs_addmess(buf);
		    }
		    input->section[i].ptr = dsc->beginsetup;
		    input->section[i].end = dsc->endsetup;
		    input->section[i].seek = TRUE;
		    i++;
		}
		display.need_header = FALSE;
		/* remember if we need trailer later */
		if (dsc->endtrailer - dsc->begintrailer != 0)
		    display.need_trailer = TRUE;
	    }
	    /* add page */
	    sprintf(buf, "Displaying page %d\n", psfile.pagenum);
	    gs_addmess(buf);
	    if (debug & DEBUG_GENERAL) {
		sprintf(buf, "adding page %d %ld %ld\n", page,
		    (long)(dsc->page[page].begin), (long)(dsc->page[page].end));
		gs_addmess(buf);
	    }
	    input->section[i].ptr = dsc->page[page].begin;
	    input->section[i].end = dsc->page[page].end;
	    if (dsc->epsf && !dsc->dcs2) {
		int j;
		/* add everything except trailer */
		input->section[i].ptr = dsc->page[0].begin;
		for (j=0; j<(int)dsc->page_count; j++)
		    if (dsc->page[j].end)
			input->section[i].end = dsc->page[j].end;
		
		display.need_trailer = FALSE;
		if (dsc->endtrailer - dsc->begintrailer != 0)
		    display.need_trailer = TRUE;
	    }
	    input->section[i].seek = TRUE;
	    i++;
	    input->count = i;
	    input->index = 0;
	}
	else {
	    /* add complete file */
	    int i = 0;
	    sprintf(buf, "Displaying DSC file %s without pages\n", filename);
	    gs_addmess(buf);
	    /* add header */
	    if (dsc->endcomments - dsc->begincomments != 0) {
		if (debug & DEBUG_GENERAL) {
		    sprintf(buf, "adding header %ld %ld\n", 
			(long)(dsc->begincomments), (long)(dsc->endcomments));
		    gs_addmess(buf);
		}
		input->section[i].ptr = dsc->begincomments;
		input->section[i].end = dsc->endcomments;
		input->section[i].seek = TRUE;
		i++;
	    }
	    /* add defaults */
	    if (dsc->enddefaults - dsc->begindefaults != 0) {
		if (debug & DEBUG_GENERAL) {
		    sprintf(buf, "adding defaults %ld %ld\n", 
			(long)(dsc->begindefaults), (long)(dsc->enddefaults));
		    gs_addmess(buf);
		}
		input->section[i].ptr = dsc->begindefaults;
		input->section[i].end = dsc->enddefaults;
		input->section[i].seek = TRUE;
		i++;
	    }
	    /* add prolog */
	    if (dsc->endprolog - dsc->beginprolog != 0) {
		if (debug & DEBUG_GENERAL) {
		    sprintf(buf, "adding prolog %ld %ld\n", 
			(long)(dsc->beginprolog), (long)(dsc->endprolog));
		    gs_addmess(buf);
		}
		input->section[i].ptr = dsc->beginprolog;
		input->section[i].end = dsc->endprolog;
		input->section[i].seek = TRUE;
		i++;
	    }
	    /* add setup */
	    if (dsc->endsetup - dsc->beginsetup != 0) {
		if (debug & DEBUG_GENERAL) {
		    sprintf(buf, "adding setup %ld %ld\n", 
			(long)(dsc->beginsetup), (long)(dsc->endsetup));
		    gs_addmess(buf);
		}
		input->section[i].ptr = dsc->beginsetup;
		input->section[i].end = dsc->endsetup;
		input->section[i].seek = TRUE;
		i++;
	    }
	    /* add trailer */
	    if (dsc->endtrailer - dsc->begintrailer != 0) {
		if (debug & DEBUG_GENERAL) {
		    sprintf(buf, "adding trailer %ld %ld\n", 
			(long)(dsc->begintrailer), (long)(dsc->endtrailer));
		    gs_addmess(buf);
		}
		input->section[i].ptr = dsc->begintrailer;
		input->section[i].end = dsc->endtrailer;
		input->section[i].seek = TRUE;
		i++;
	    }
	    display.need_header = FALSE;
	    display.need_trailer = FALSE;
	    input->count = i;
	    input->index = 0;
	}
    }
    else {
	/* add complete file */
        sprintf(buf, "Displaying non DSC file %s\n", filename);
	gs_addmess(buf);
	input->section[0].ptr = 0;
	input->section[0].end = gfile_get_length(psfile.file);
	input->section[0].seek = TRUE;
	if (debug & DEBUG_GENERAL) {
	    sprintf(buf, "adding complete file %ld %ld\n", 
		(long)0, (long)(input->section[0].end));
	    gs_addmess(buf);
	}
	input->count = 1;
	input->index = 0;
        display.need_trailer = FALSE;
    }
/* this is done by get_gs_input
    gfile_seek(psfile.file, input->section[0].ptr, gfile_begin);
*/
    return 0;
}

BOOL
gs_process_trailer(void)
{
    if (display.need_trailer) {
	/* assume trailer is short, don't check message loop */	
	post_img_message(WM_GSWAIT, IDS_WAIT);
	if (!psfile.ispdf && (dfreopen() != 0)) {
	    pending.abort = TRUE;
	    return TRUE;
	}
	if (psfile.ispdf) {
	    if (pdf_trailer())
		return FALSE;
	}
	else {
	    BOOL old_now = pending.now;
	    pdf_free_link();
	    /* don't wait if we execute showpage in trailer */
	    pending.now = TRUE;
	    if (send_trailer()) {
		dfclose();
		pending.now = old_now;
		return FALSE;
	    }
	    dfclose();
	    pending.now = old_now;
	}
	display.need_trailer = FALSE;
    }
    return FALSE;
}


/* Prepare input to be written to Ghostscript (Unix)
 * or write header and prepare input (Windows)
 * Return code is 0 for OK, +ve for OK but exit enclosing loop,
 * -ve for Ghostscript error
 */
int
gs_process_prepare_input(PENDING *ppend)
{
    int code;
    int i;
    char buf[MAXSTR];
    GSDLL_INPUT *input = &view.input;
    code = dfreopen();
    if (code < 0) {
	/* file could not be opened */
	/* beep only */
	return 1;	/* error */
    }
    else if (code > 0) {
	/* document changed, so force a rescan */
	/* Cause the rescan to occur on main thread */
	request_mutex();
	pending.now = FALSE;
	pending.redisplay = TRUE; /* cause a redisplay after abort complete */
	pending.abort = TRUE;
	release_mutex();
	return 1;
    }

    code = 0;	/* GS return code */

    /* move to desired page */
    if (psfile.dsc != (CDSC *)NULL) {
	if (ppend->pagenum < 0)
	    ppend->pagenum = psfile.pagenum;
	if (psfile.dsc->page_count 
		&& (ppend->pagenum > (int)psfile.dsc->page_count))
	     ppend->pagenum = psfile.dsc->page_count;
	if (ppend->pagenum < 0)
	    ppend->pagenum = 1;
    }
    if (ppend->pagenum > 0)
	psfile.pagenum = ppend->pagenum;
    ppend->pagenum = 0;

    if (!display.init) {
	if (!code)
	    code = d_init1();	/* create GSview dict */
	if (!code)
	    code = d_resize(psfile.pagenum);  /* Set GSview dict entries */
	if (!code)
	    code = d_init2();	/* add ViewerPreProcess then setpagedevice */
    }
    else {
	if (ppend->resize) {
	    if (!code)
		code = d_resize(psfile.pagenum);
	    view.img->ignore_sync = TRUE;		/* ignore next sync */
	    if (!code) 	/* local/global bug in GS, see note above */
		code = gs_printf("currentglobal true setglobal\n");
	    if (!code) {
		if (in_pstotext)
		    code = gs_printf("<< >> setpagedevice\n");
		else
		    code = gs_printf("<< >> //systemdict /setpagedevice get exec\n");
	    }
	    if (!code) 	/* local/global bug in GS, see note above */
		code = gs_printf("setglobal\n");
	}
    }

    if (!code && option.epsf_warn)
	code = send_prolog(IDR_EPSFWARN);

    if (code) {
	/* error initializing GS */
	if (!psfile.ispdf)
	    dfclose();
	return code;
    }

    /* highlight search word if needed */
    if (psfile.text_bbox.valid) {
	display.show_find = TRUE;
	psfile.text_bbox.valid = FALSE;	/* don't do it on following pages */
    }
    else
	display.show_find = FALSE;

    if (psfile.ispdf) {
	post_img_message(WM_GSTEXTINDEX, 0);
	{
	    if (display.need_header) {
		gs_addmess("Scanning PDF file\n");
		if ( (code = pdf_head()) != 0 )
		    return code;
	    }
	    display.need_header = FALSE;
	    display.need_trailer = TRUE;
	    if (ppend->pagenum > 0)
		psfile.pagenum = ppend->pagenum;
	    ppend->pagenum = 0;
	    sprintf(buf, "Displaying PDF page %d\n", psfile.pagenum);
	    gs_addmess(buf);
	    if ( (code = d_pdf_page(psfile.pagenum)) != 0)
		return code;
	}
    } else {
	post_img_message(WM_GSTEXTINDEX, 0);

	pdf_free_link();
	if (display.need_header)
	    pdf_add_pdfmark();

        /* calculate document length */
        /* prepare list of PostScript sections to send */
	if ( (code = send_document()) != 0 ) {
	    dfclose();
	    return code;
	}

	gsbytes_done = 0;
	gsbytes_size = 0;
	for (i=0; i<input->count; i++)
	    gsbytes_size += (input->section[i].end - input->section[i].ptr);
	percent_pending = FALSE;
	percent_done = 0;
    }

    return code;
}

/* process current page or non-DSC file */
/* returns 0 if OK, else GS error code */
int
gs_process_loop2(PENDING *ppend)
{
char buf[1024];
int len;
int code;
int pcdone;

    if ((code = gs_process_prepare_input(ppend)) > 0) {
	/* Need to rescan file */
	return 0;
    }
	
    if (code)
	return code;

    if (!psfile.ispdf) {
	/* send postscript */
	post_img_message(WM_GSWAIT, IDS_WAITDRAW_PC);
	while (!pending.abort && !pending.unload
	    && ((len = get_gs_input(buf, sizeof(buf)))!=0)) {
	    code = gs_execute(buf, len);
	    if (code) {
		if (psfile.file)
		    dfclose();
		if (pending.abort) {
		    gs_addmess("\n--- Aborted ---\n");
		    return 0;
		}
		else {
#ifndef VIEWONLY
		    gs_addmess("\n--- Begin offending input ---\n");
		    gs_addmess_count(buf, len);
		    gs_addmess("\n--- End offending input ---\n");
		    /* a file offset of 0 really means that it reached the end of the data */
		    /* need to fix this eventually */
		    sprintf(buf, "file offset = %ld\n", view.input.section[view.input.index].ptr);
		    gs_addmess(buf);
#endif
		    sprintf(buf, "gsapi_run_string_continue returns %d\n", code);
		    gs_addmess(buf);
		}
		return code;
	    }
	    gsbytes_done += len;
	    if (gsbytes_size > 100)
	        pcdone = (int)(gsbytes_done / (gsbytes_size / 100));
	    else
		pcdone = 0;
	    if ((pcdone != percent_done) && !percent_pending) {
		percent_done = pcdone;
		percent_pending = TRUE;
		post_img_message(WM_GSPERCENT, 0);
	    }
	    if (!multithread) {
		/* peek message loop */
		peek_message();
	    }
	}
	gs_printf("\n");
	dfclose();
    }

    if (psfile.dsc == (CDSC *)NULL) {
	/* We are at the end of the file */
	/* Do not allow more pages to be displayed */
	request_mutex();
	pending.pagenum = 0;
	pending.restart = TRUE;
	release_mutex();
    }
    if (pending.abort)
	gs_addmess("\n--- Aborted ---\n");

    return 0;
}


/* Find out exactly what is pending.
 * Copy and reset pending.
 * Return copy of what is needed in lpending
 */
int
gs_process_pending(PENDING *lpending)
{
    int code = 0;

    /* block other threads while we take a snapshort of pending */
    request_mutex();
    *lpending = pending;
    pending.now = FALSE;
    pending.next = FALSE;
    pending.abort = FALSE;
    pending.psfile = NULL;
    pending.pagenum = 0;
    pending.resize = FALSE;
    pending.text = FALSE;
    pending.restart = FALSE;
    release_mutex();

    if (option.auto_orientation && !lpending->psfile && lpending->pagenum) {
	/* If moving to another page in the same document
	 * and new page orientation doesn't match the current,
	 * then we need to resize.
	 */
	if (display.orientation != d_orientation(lpending->pagenum))
	    lpending->resize = TRUE;

	/* If we are changing file then resize will occur automatically.
	 * If we are redisplaying the current page then we don't need
	 * to force a resize.
	 */
    }

    if (lpending->psfile && display.init)
	lpending->restart = TRUE;

    if (lpending->resize && display.init)
	lpending->restart = TRUE;

    if (lpending->text && display.init)
	lpending->restart = TRUE;


    if (lpending->restart || (lpending->resize && !psfile.ispdf) 
	  || lpending->psfile) {
	/* if size or document changed, need to restart document */
	if (display.need_trailer)
	    gs_process_trailer();	/* send trailer to clean up */
	display.need_header = TRUE;
	if (display.saved)
	    code = d_restore();
	if (code)
	    lpending->restart = TRUE;
    }

    if (lpending->psfile) {
	request_mutex();
	zoom = FALSE;
	psfile_free(&psfile);
	psfile = *(lpending->psfile);
	post_img_message(WM_GSTITLE, 0);
	free(lpending->psfile);
	release_mutex();
    }
    return code;
}

/* loop while Ghostscript loaded, document unchanged and no errors */
/* return 0 if OK, or GS error code */
int
gs_process_loop1(void)
{
PENDING lpending;
int code;
    while (!pending.unload && !pending.abort) {
	if (!pending.now) {
	    if (szWait[0] != '\0')
	        post_img_message(WM_GSWAIT, IDS_NOWAIT);
	    if (multithread)
		wait_event();	/* wait for event semaphore */
	    else
		get_message();	/* process one message */
	}

	if (pending.now && !pending.unload && !pending.abort) {
	    gsdll.state = GS_BUSY;
	    post_img_message(WM_GSWAIT, IDS_WAIT);

	    code = gs_process_pending(&lpending);
	    if (code)
		return code;

	    if (psfile.name[0] == '\0') {
		/* no new file */
		gsdll.state = GS_IDLE;
		post_img_message(WM_GSWAIT, IDS_NOWAIT);
		/* notify in case CLOSE was caused by IDM_SELECT */
		post_img_message(WM_COMMAND, IDM_CLOSE_DONE);
		break;
	    }

	    if (lpending.restart && display.init) {
		request_mutex();
		/* restore pending operations */
		pending.pagenum = lpending.pagenum;
		pending.abort = TRUE;		/* ignore errors */
		pending.text = lpending.text;
		pending.now = TRUE;
		release_mutex();
		return 0;	/* restart interpreter */
	    }

#ifndef VIEWONLY
	    if (lpending.text) {
		code = gs_process_pstotext();
		gsdll.state = GS_IDLE;
		return code;
	    }
#endif

	    if ( (code = gs_process_loop2(&lpending)) != 0 ) {
		gsdll.state = GS_IDLE;
		if (psfile.dsc == (CDSC *)NULL)
		    psfile.pagenum = 1;
		post_img_message(WM_GSWAIT, IDS_NOWAIT);
		return code;
	    }

	    if (pending.abort)
		return 0;

	    if (pending.unload) {
		post_img_message(WM_GSWAIT, IDS_WAITGSCLOSE);
	    }
	    else {
		gsdll.state = GS_IDLE;
		if (psfile.dsc == (CDSC *)NULL)
		    psfile.pagenum = 1;
		post_img_message(WM_GSWAIT, IDS_NOWAIT);
	    }
	    post_img_message(WM_GSSYNC, 0);	/* redraw display */
	}
    }
    return 0;
}


/********************************************************/
/* public functions */

/* In single thread mode, the main message loop is only used 
 * when the GS DLL is unloaded */

/* This is the second message loop, or second thread */
/* It loads the GS DLL and returns when GS DLL is unloaded */
/* The third message loop is in the GS DLL callback gsdll.state = GS_PAGE */

void
gs_process(void)
{
int code;
int exit_code;

#ifndef VIEWONLY
    if (pending.pstoedit) {
	post_img_message(WM_GSWAIT, IDS_WAIT);
	gsdll_free(&gsdll);	/* pstoedit is going to load it */
	if (debug & DEBUG_GENERAL)
	    gs_addmess("Ghostscript unloaded\n");
	process_pstoedit(NULL);
	post_img_message(WM_GSWAIT, IDS_NOWAIT);
	pending.pstoedit = FALSE;
	pending.now = FALSE;
	return;
    }
#endif /* !VIEWONLY */

    /* gsdll.state = UNLOADED; */
    gsdll.state = GS_BUSY;		/* skip IDLE state */
    pending.abort = FALSE;

    post_img_message(WM_GSWAIT, IDS_WAITGSOPEN);

    if (gsdll_load(&gsdll, option.gsdll)) {
        gsdll.state = GS_UNINIT;
	request_mutex();
	pending.now = FALSE;
	pending.next = FALSE;
	pending.unload = FALSE;
	pending.abort = FALSE;
	pending.redisplay = FALSE;
	pending.pstoedit = FALSE;
	pending.text = FALSE;
	if (pending.psfile) {
	    psfile_free(&psfile);
	    psfile = *pending.psfile;
	    free(pending.psfile);
	    pending.psfile = NULL;
	}
	release_mutex();
	post_img_message(WM_GSWAIT, IDS_NOWAIT);
        post_img_message(WM_GSSHOWMESS, 0);
	return;
    }
/*
    post_img_message(WM_GSWAIT, IDS_WAIT);
*/

    /* send stuff to Ghostscript */
    while (!pending.unload) {
        display.saved = FALSE;
	display.init = FALSE;
	display.need_header = TRUE;
	display.need_trailer = FALSE;
	pending.restart = FALSE;

	if (!pending.now) {
	    if (szWait[0] != '\0')
	        post_img_message(WM_GSWAIT, IDS_NOWAIT);
	    if (multithread)
		wait_event();	/* wait for event semaphore */
	    else
		get_message();	/* process one message */
	}

	if ( (code = gs_dll_init(NULL)) != 0 ) {
/*
	    delayed_message_box(IDS_PROCESS_INIT_FAIL, 0);
*/
	    if ((pending.abort == TRUE) && (code == e_interrupt))
	        gs_addmess("Ignoring error on abort\n");
	    else
		post_img_message(WM_GSSHOWMESS, 0);
	    pending.unload = TRUE;
	    break;
	}

	if (!code) {
	    if (gsdll.run_string_begin == NULL)
		break;
	    if ( (code = gsdll.run_string_begin(
		    gsdll.minst, 0, &exit_code)) != 0 ) {
		char buf[256];
		sprintf(buf,"gsdll.run_string_begin returns %d\n", code);
		gs_addmess(buf);
		pending.unload = TRUE;
		post_img_message(WM_GSSHOWMESS, 0);
	    }
	    execute_code = 0;
	}

        if (!code) {
	    code = gs_process_loop1();
	    if (code) {
		if (!pending.abort) {
		    pending.now = FALSE;
		    pending.resize = FALSE;
		    pending.next = FALSE;
		    post_img_message(WM_GSSHOWMESS, 0);
		    post_img_message(WM_GSWAIT, IDS_NOWAIT); /* shouldn't be needed */
		}
	    }
	    if (execute_code >= 0)
		gsdll.run_string_end(gsdll.minst, 0, &exit_code);
	}
	pending.abort = FALSE;	/* don't interrupt cleanup */
	if (gsdll.exit != NULL) {
	    char buf[256];
	    code = gsdll.exit(gsdll.minst);
	    if ((debug  & DEBUG_GENERAL)|| code) {
	        sprintf(buf,"gsapi_exit returns %d\n", code);
	        gs_addmess(buf);
	    }
	    gs_addmess("\n\n");
	}
	if (pending.redisplay) {
	    pending.redisplay = FALSE;
	    post_img_message(WM_GSREDISPLAY, 0);	/* will set pending.now */
	}
	pending.abort = FALSE;
    }

    post_img_message(WM_GSWAIT, IDS_WAITGSCLOSE);
    gsdll.state = GS_UNINIT;
    gsdll_free(&gsdll);
    if (debug & DEBUG_GENERAL)
	gs_addmess("Ghostscript unloaded\n");
    display.need_header = TRUE;
    display.need_trailer = FALSE;
    display.saved = FALSE;
    pending.now = FALSE;
    pending.next = FALSE;
    pending.unload = FALSE;
    pending.abort = FALSE;
    if (pending.psfile) {
	request_mutex();
	psfile_free(&psfile);
        psfile = *pending.psfile;
	free(pending.psfile);
	pending.psfile = NULL;
	release_mutex();
    }
    /* close document also */
    gsview_unzoom();

    if (pending.pstoedit)
	pending.now = TRUE;

    post_img_message(WM_GSWAIT, IDS_NOWAIT);
    post_img_message(WM_GSTITLE, 0);
}


/* next page */
int
gs_page_skip(int skip)
{
CDSC *dsc = psfile.dsc;
    if (dsc == (CDSC *)NULL) {
	/* NOT DSC, can only go forward one page at a time */
	if (skip != 1) {
	    play_sound(SOUND_NOPAGE);
	    return FALSE;
	}
	if (gsdll.state == GS_IDLE) {
	    /* at end of file */
	    play_sound(SOUND_NOPAGE);
	    return FALSE;	/* nothing to do */
	}
	else  {
	    request_mutex();
	    gsview_unzoom();	/* should never be zoomed anyway */
	    pending.next = TRUE;
	    release_mutex();
	}
	return TRUE;
    }

    /* Must be DSC */
    if ( (skip == 0) 
        || ((skip > 0) && (psfile.pagenum == (int)dsc->page_count))
        || ((skip < 0) && (psfile.pagenum == 1))
	|| (dsc->page_count == 0) ) {
	    play_sound(SOUND_NOPAGE);
	    return FALSE;
    }

    request_mutex();
    pending.pagenum = psfile.pagenum + skip;
    if (pending.pagenum > (int)dsc->page_count)
	 pending.pagenum = dsc->page_count;
    if (pending.pagenum < 1)
	pending.pagenum = 1;
    gsview_unzoom();
    pending.now = TRUE;
    release_mutex();

    /* add to history */
    history_add(pending.pagenum);

    return TRUE;
}
     
/* Parse argline looking for argument
 * If argument found, copy to arg and return pointer to next argument.
 * If no argument found, return NULL.
 * To get next argument, call again with return value as argline.
 * If copyquotes is TRUE, quotes will copied to the output, otherwise
 * they will be omitted.
 */
char *
gs_argnext(char *argline, char *arg, BOOL copyquotes)
{
    /* quotes may be put around embedded spaces and are copied */
    while ((*argline) && (*argline==' '))
	argline++;	/* skip over leading spaces */
    if (*argline=='\0')
	return NULL;	/* no more arguments */

    while (*argline) {
	if (*argline == ' ') {
	    /* end of argument */
	    *arg = '\0';
	    while (*argline && (*argline==' '))
		argline++;
	    return argline;
	}
	else if (*argline == '"') {
	    /* quoted argument */
	    /* copy until closing quote or end of string */
	    if (copyquotes)
	        *arg++ = *argline; /* copy opening quote */
	    argline++;
	    while ((*argline) && (*argline != '"')) {
		*arg++ = *argline;
		argline++;
	    }
	    if ((*argline) && (*argline == '"')) {
		/* copy closing quote */
		if (copyquotes)
		    *arg++ = *argline;
		argline++;
	    }
	}
	else {
	    *arg++ = *argline;
	    argline++;
	}
    }
    *arg = '\0';
    return argline;
}


/*********************************************************************/
/* stdio functions */
static int GSDLLCALL
gsdll_stdin(void *instance, char *buf, int len)
{
    char mess[MAXSTR];
    sprintf(mess,"stdin callback not supported: %p %d\n", buf, len);
    gs_addmess(mess);
    return 0; /* EOF */
}

static int GSDLLCALL
gsdll_stdout(void *instance, const char *str, int len)
{
#ifndef VIEWONLY
#ifndef UNIX
    if (callback_pstotext(str, len))
	return len;
#endif
#endif
    if (!pending.abort)
	pdf_checktag(str, len);
    gs_addmess_count(str, len);
    return len;
}

static int GSDLLCALL
gsdll_stderr(void *instance, const char *str, int len)
{
    gs_addmess_count(str, len);
    return len;
}
/*********************************************************************/

/* called from gs_process */
int
gs_dll_init(char *devname)
{
char buf[1024];
char *p, *q;
int code;
int depth;
int i, argc;
char **argv;

	/* build options list */
	p = buf;
	buf[0] = '\0';
	buf[1] = '\0';

	/* add include path */
	if (option.gsinclude[0] != '\0') {
	    strcpy(buf, "-I");
	    strcat(buf, option.gsinclude);
	    p += strlen(p)+1;
	    *p = '\0';
	}

	/* add initial device */
	if (devname) {
	    strcpy(p, "-sDEVICE=");
	    p += strlen(p);
	    strcpy(p, devname);
	    p += strlen(p)+1;
	    *p = '\0';
	    /* restrict depth to values supported by Ghostscript */
	    depth = display.planes * display.bitcount;
	    if (depth > 8)
		depth = 24;
	    else if (depth >=8)
		depth = 8;
	    else if (depth >=4)
		depth = 4;
	    else 
		depth = 1;
	    strcpy(p, "-dBitsPerPixel=");
	    p += strlen(p);
	    sprintf(p, "%d", option.depth ? option.depth : depth);
	    p += strlen(p)+1;
	    *p = '\0';
	}
	else {
	    /* no device at all */
	    strcpy(p, "-dNODISPLAY");
	    p += strlen(p)+1;
	    *p = '\0';
	}

	strcpy(p, "-dNOPAUSE");
	p += strlen(p)+1;
	*p = '\0';

        if (gsdll.revision_number > GS_UNSAFE) {
	    /* after selecting the initial device, we need to execute 
	     * .locksafe
	     */
	    strcpy(p, "-dNOSAFER");
	    p += strlen(p)+1;
	    *p = '\0';
	}
	else {
	    if (option.safer) {
		strcpy(p, "-dSAFER");
		p += strlen(p)+1;
		*p = '\0';
	    }
	}

	if (option.alpha_text > 1) {
	    strcpy(p, "-dNOPLATFONTS");
	    p += strlen(p)+1;
	    *p = '\0';
	}

	/* Ghostscript 7.0 won't execute pdfmark for links unless
	 * the device has /pdfmark as a parameter.  This isn't the
	 * case for any device other than pdfwrite, so we need
	 * to force it to procedd pdfmarks.
	 */
	strcpy(p, "-dDOPDFMARKS");
	p += strlen(p)+1;
	*p = '\0';

	if (pending.text) {
	    /* pstotext needs to make some changes to the systemdict */
	    strcpy(p, "-dDELAYBIND");
	    p += strlen(p)+1;
	    *p = '\0';
 	    strcpy(p, "-dWRITESYSTEMDICT");
	    p += strlen(p)+1;
	    *p = '\0';
	    strcpy(p, "-q");
	    p += strlen(p)+1;
	    *p = '\0';
	    /* GS 5.50 incorrectly allows the page size of nullpage */
	    /* device to be changed.  We must disable this. */
	    strcpy(p, "-dFIXEDMEDIA");
	    p += strlen(p)+1;
	    *p = '\0';
	}

	/* add other options */
	/* option.gsother must have options separated by one or more spaces */
	/* quotes may be put around embedded spaces, but are not copied */
	q = option.gsother;
	while ((q = gs_argnext(q, p, FALSE)) != NULL) {
	    p += strlen(p)+1;
	    *p = '\0';
	}

/* convert buf back to argv */
for (argc=1, p=buf; *p; argc++)
	p += strlen(p)+1;
argv = (char **)malloc((argc+2) * sizeof(char *));
argv[0] = option.gsdll;
for (i=1, p=buf; i<=argc; i++) {
    argv[i] = p;
    p+=strlen(p)+1;
}
argv[i] = NULL;

	if (debug & DEBUG_GENERAL) {
	    gs_addmess("Ghostscript options are:\n");
	    for (i=1; i<=argc; i++) {
		gs_addmess("  ");
		gs_addmess(argv[i]);
		gs_addmess("\n");
	    }
	}

	if (gsdll.minst == NULL) {
	    code = gsdll.new_instance(&gsdll.minst, NULL);
	    if (code) {
		sprintf(buf,"gsapi_new_instance returns %d\n", code);
		gs_addmess(buf);
		return code;
	    }
	    if (image.open) {
		if (debug & DEBUG_GENERAL)
		    gs_addmess("Ghostscript didn't close display when it exited.\n");
	    }
	    gsdll.set_stdio(gsdll.minst, gsdll_stdin, gsdll_stdout, gsdll_stderr);
	    gsdll.set_poll(gsdll.minst, gsdll_poll);
	    gsdll.set_display_callback(gsdll.minst, &gsdisplay);
	}
	code = gsdll.init_with_args(gsdll.minst, argc, argv);
free((void *)argv);
	if (code) {
	    sprintf(buf,"gsapi_init_with_args returns %d\n", code);
	    gs_addmess(buf);
	}

/*
	zoom = FALSE;
*/
	display.epsf_clipped = FALSE;
	gsdll.open = TRUE;
	return code;
}

#ifndef UNIX
/******************************************************************/
/* for pstotext */
HMODULE pstotextModule;
FILE *pstotextOutfile;
void *pstotextInstance;
#ifdef __cplusplus
extern "C" {
#endif
PFN_pstotextInit pstotextInit;
PFN_pstotextFilter pstotextFilter;
PFN_pstotextExit pstotextExit;
PFN_pstotextSetCork pstotextSetCork;
#ifdef __cplusplus
}
#endif
char pstotextLine[2048];
int pstotextCount;


#ifndef VIEWONLY
int
callback_pstotext(const char *str, unsigned long count)
{
    if (pstotextInstance) {
	if (debug & DEBUG_GENERAL)
	    gs_addmess_count(str, (int)count);
	if (pstotextOutfile) {
	    char *d, *e;
	    char *pre, *post;
	    int llx, lly, urx, ury;
	    int status;
	    char ch;
	    if (sizeof(pstotextLine) > count + pstotextCount) { 
		memcpy(pstotextLine+pstotextCount, str, (int)count);
		pstotextCount += (int)count;
		pstotextLine[pstotextCount] = '\0';
		/* can't use strchr, because line may include null char */
		e = (char *)memchr(pstotextLine, '\n', pstotextCount);
		while ( e != NULL ) {
		    ch = *(++e);	/* save character after \n */
		    *e = '\0';
		    d = pre = post = (char *)NULL;
		    llx = lly = urx = ury = 0;
		    status = pstotextFilter(pstotextInstance, pstotextLine, 
			&pre, &d, &post,
			&llx, &lly, &urx, &ury);
		    *e = ch;	/* restore character after \n */
		    memmove(pstotextLine, e, (int)(pstotextCount - (e-pstotextLine)));
		    pstotextCount -= (int)(e-pstotextLine);
		    pstotextLine[pstotextCount] = '\0';
		    /* can't use strchr, because line may include null char */
		    e = (char *)memchr(pstotextLine, '\n', pstotextCount);
		    if (status) {
			char buf[MAXSTR];
			sprintf(buf, "\npstotextFilter error %d\n", status);
			gs_addmess(buf);
			return 1;
		    }
		    if (d) {
			if (pre) {
			    if (*pre == ' ')
				pre++;
			    fputs(pre, pstotextOutfile);
			}
		        fprintf(pstotextOutfile, "%d %d %d %d %s\n",
			    llx, lly, urx, ury, d);
			if (post)
			    fputs(post, pstotextOutfile);
		    }
		}
	    }
	    else
		pstotextCount = 0;	/* buffer overflow */
	}
        return 1;
    }
    return 0;
}
#endif

void
gs_error_code(int code)
{
char buf[MAXSTR];
    sprintf(buf, "Ghostscript returns error code %d\n", code);
    gs_addmess(buf);
}

#ifndef VIEWONLY
/* This handles extracting text from PS or PDF file */
int 
gs_process_pstotext(void)
{
int code;
char buf[MAXSTR];
int angle = 0;
int len;
long lsize, ldone;
int pcdone;
int real_orientation;
    if (load_pstotext())
	return 1;
  
    in_pstotext = TRUE;
    if (option.pstotext == IDM_PSTOTEXTCORK - IDM_PSTOTEXTMENU - 1)
        pstotextSetCork(pstotextInstance, TRUE);

    gs_addmess("Extracting text using pstotext...\n");

    /* open output file */
    if ( (pstotextOutfile = gp_open_scratch_file(szScratch, psfile.text_name, "w")) == (FILE *)NULL) {
	gs_addmess("Can't open temporary file for text extraction\n");
        in_pstotext = FALSE;
	unload_pstotext();
	return 1;
    }
    if (debug & DEBUG_GENERAL)
        gs_addmessf("Writing text index to \042%s\042\n", psfile.text_name);

    percent_pending = FALSE;
    percent_done = 0;
    post_img_message(WM_GSWAIT, IDS_WAITTEXT);

    switch(d_orientation(psfile.pagenum)) {
	default:
	case 0:
	    real_orientation = IDM_PORTRAIT;
	    break;
	case 1:
	    real_orientation = IDM_SEASCAPE;
	    break;
	case 2:
	    real_orientation = IDM_UPSIDEDOWN;
	    break;
	case 3:
	    real_orientation = IDM_LANDSCAPE;
	    break;
    }

    if (psfile.ispdf)
	real_orientation = pdf_orientation(psfile.pagenum);

    switch (real_orientation) {
	case IDM_LANDSCAPE:
	    angle = 270;
	    break;
	case IDM_SEASCAPE:
	    angle = 90;
	    break;
	case IDM_PORTRAIT:
	    angle = 0;
	    break;
	case IDM_UPSIDEDOWN:
	    angle = 180;
	    break;
    }

    /* send pstotext prolog to GS */
    if ((angle==270) && (code = send_pstotext_prolog(pstotextModule, 2)) != 0 ) {
	gs_addmess("Error processing rot270 prolog\n");
	gs_error_code(code);
        in_pstotext = FALSE;
	unload_pstotext();
	return code;
    }
    if ((angle==90) && (code = send_pstotext_prolog(pstotextModule, 3)) != 0 ) {
	gs_addmess("Error processing rot90 prolog\n");
	gs_error_code(code);
        in_pstotext = FALSE;
	unload_pstotext();
	return code;
    }

    if ( (code = send_pstotext_prolog(pstotextModule, 1)) != 0 ) {
	gs_addmess("Error processing ocr prolog\n");
	gs_error_code(code);
        in_pstotext = FALSE;
	unload_pstotext();
	return code;
    }

    /* Don't let anyone stuff around with the page size now */
    gs_printf("/setpagedevice { pop } def\n");

    if (option.safer && (gsdll.revision_number > GS_UNSAFE)) {
	d_permitread();
	gs_printf(".locksafe\n");
    }

    if (psfile.ispdf) {
	int i;
	if ( (code = pdf_head()) != 0 ) {
	    gs_addmess("PDF prolog failed\n");
	    gs_error_code(code);
	    in_pstotext = FALSE;
	    unload_pstotext();
	    return code;
	}
	if ( psfile.dsc == (CDSC *)NULL ) {
	    gs_addmess("Couldn't get PDF page count\n");
	    in_pstotext = FALSE;
	    unload_pstotext();
	    return 1;
	}
	if ( (code = d_init1()) != 0) {
	    gs_addmess("Error creating GSview dictionary\n");
	    gs_error_code(code);
	    in_pstotext = FALSE;
	    unload_pstotext();
	    return 1;
	}

	lsize = psfile.dsc->page_count;
	i = 1;
	while (!pending.abort && !pending.unload && i <= (int)psfile.dsc->page_count) {
	    if ( (code = d_pdf_page(i)) != 0 ) {
		gs_error_code(code);
		in_pstotext = FALSE;
		unload_pstotext();
		return code;
	    }
	    pcdone = (int)(i * 100 / lsize);
	    if ((pcdone != percent_done) && !percent_pending) {
		percent_done = pcdone;
		percent_pending = TRUE;
		post_img_message(WM_GSPERCENT, 0);
	    }
	    i++;
	    if (!multithread) {
		/* peek message loop */
		peek_message();
	    }
	    if (pending.now)
		pending.abort = TRUE;	/* user wants to do something else */
	}
	if ((code = pdf_trailer()) != 0) {
	    gs_addmess("Error in PDF trailer\n");
	    gs_error_code(code);
	    in_pstotext = FALSE;
	    unload_pstotext();
	    return code;
	}
    }
    else {
	if (dfreopen() != 0) {
	    gs_addmess("File changed or missing\n");
	    in_pstotext = FALSE;
	    unload_pstotext();
	    return 1;
	}

	/* get file length */
	lsize = gfile_get_length(psfile.file);
	if (lsize <= 0)
	    lsize = 1;
	gfile_seek(psfile.file, 0, gfile_begin);
	ldone = 0;

	/* send document to GS */
	while (!pending.abort && !pending.unload
	    && ((len = gfile_read(psfile.file, buf, sizeof(buf)))!=0)) {
	    code = gs_execute(buf, len);
	    if (code) {
		dfclose();
	        gs_error_code(code);
		in_pstotext = FALSE;
		unload_pstotext();
		unlink(psfile.text_name);
		psfile.text_name[0] = '\0';
		if (pending.abort) {
		    gs_addmess("\n--- Aborted ---\n");
		    return 0;
		}
		else {
		    gs_addmess("\n--- Begin offending input ---\n");
		    gs_addmess_count(buf, len);
		    gs_addmess("\n--- End offending input ---\n");
		    sprintf(buf, "gsapi_run_string_continue returns %d\n", code);
		    gs_addmess(buf);
		}
		return code;
	    }
	    ldone += len;
	    pcdone = (int)(ldone * 100.0 / lsize);
	    if ((pcdone != percent_done) && !percent_pending) {
		percent_done = pcdone;
		percent_pending = TRUE;
		post_img_message(WM_GSPERCENT, 0);
	    }
	    if (!multithread) {
		/* peek message loop */
		peek_message();
	    }
	    if (pending.now)
		pending.abort = TRUE;	/* user wants to do something else */
	}
	dfclose();
    }


    /* close output file */
    fclose(pstotextOutfile);

    /* unload pstotext DLL */
    in_pstotext = FALSE;
    unload_pstotext();

    /* if successful, restart extract or find */
    if (!pending.unload && !pending.abort) {
        gs_addmess("\npstotext successful\n");
	if (psfile.text_extract)
	    post_img_message(WM_COMMAND, IDM_TEXTEXTRACT_SLOW);
	else
	    post_img_message(WM_COMMAND, IDM_TEXTFINDNEXT);
    }
    else {
        gs_addmess("\npstotext aborted\n");
	unlink(psfile.text_name);
	psfile.text_name[0] = '\0';
    }
    pending.abort = TRUE;    /* ignore errors */
    return 0;
}
#endif
#endif /* !VIEWONLY */


