/*  Copyright (C) 1993-2001, Ghostgum Software Pty Ltd.  All rights reserved.

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

/* gvpdll.c */
/* GS DLL associated routines for PM */
#include "gvpm.h"

void 
post_img_message(int message, int param)
{
    WinPostMsg(hwnd_bmp, message, (MPARAM)param, (MPARAM)0);
}

void
view_wait_message(VIEW *view)
{
    /* process messsages until pending changes */
    int rc;
    QMSG q_mess;		/* queue message */
    while (!pending.next && !pending.now && !pending.unload &&
	!quitnow && ((rc = WinGetMsg(hab, &q_mess, 0L, 0, 0)) != 0))
	    WinDispatchMsg(hab, &q_mess);
}

/******************************************************************/

int
gsdll_open(GSDLL *dll, const char *name)
{
APIRET rc;
char buf[MAXSTR+40];

char fullname[1024];
const char *shortname;
char *p;

    if (debug)
	gs_addmessf("Trying to load %s\n", name);

    memset(buf, 0, sizeof(buf));
    rc = DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)name, &dll->hmodule);
    if (rc) {
	/* failed */
	/* try once more - which bug are we dodging? */
	rc = DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)name, &dll->hmodule);
    }

    if (rc) {
	/* failed */
	/* try again, with path of EXE */

	PTIB pptib;
	PPIB pppib;
	if ( (rc = DosGetInfoBlocks(&pptib, &pppib)) != 0 ) {
	    gs_addmessf("gsdll_open: Couldn't get pid, rc = %ld\n", rc);
	    return_error(-1);
	}
	/* get path to EXE */
	if ( (rc = DosQueryModuleName(pppib->pib_hmte, 
	    sizeof(fullname), fullname)) != 0 ) {
	    gs_addmessf("gsdll_open: Couldn't get module name, rc = %ld\n", rc);
	    return_error(-1);
	}
	if ((p = strrchr(fullname,'\\')) != (char *)NULL) {
	    p++;
	    *p = '\0';
	}

	if ((shortname = strrchr((char *)name, '\\')) == (const char *)NULL)
	    shortname = name;
	else
	    shortname++;

	strcat(fullname, shortname);
	if (debug)
	    gs_addmessf("Trying to load %s\n", fullname);
	rc = DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)fullname, 
	    &dll->hmodule);
	if (rc) {
	    /* failed again */
	    /* try once more, this time on system search path */
	    if (debug)
		gs_addmessf("Trying to load %s\n", shortname);
	    rc = DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)shortname, 
		&dll->hmodule);
	}
    }
    if (rc)
	return_error(-1);

    return 0;
}

void *
gsdll_sym(GSDLL *dll, const char *name)
{
    char uname[MAXSTR];
    int i;
    PFN fn;

    for (i=0; name[i]; i++)
	uname[i] = toupper(name[i]);
    uname[i] = '\0';

    if (DosQueryProcAddr(dll->hmodule, 0, (PCSZ)uname, &fn) != 0)
	return NULL;

    return (void *)fn;
}

int
gsdll_close(GSDLL *dll)
{
    DosFreeModule(dll->hmodule);
    return 0;
}

/*********************************************************************/



static int poll(void)
{
    if (!multithread)
	peek_message();
    if (pending.abort)
	return -100;	/* signal an error if we want to abort */
    return 0;
}


/* Poll the caller for cooperative multitasking. */
/* If this function is NULL, polling is not needed */
int GSDLLCALL gsdll_poll(void *handle)
{
    return poll();
}

/*********************************************************************/

static int image_palette_size(int format)
{
    int palsize = 0;
    switch (format & DISPLAY_COLORS_MASK) {
	case DISPLAY_COLORS_NATIVE:
	    switch (format & DISPLAY_DEPTH_MASK) {
		case DISPLAY_DEPTH_1:
		    palsize = 2;
		    break;
		case DISPLAY_DEPTH_4:
		    palsize = 16;
		    break;
		case DISPLAY_DEPTH_8:
		    palsize = 96;
		    break;
	    }
	    break;
	case DISPLAY_COLORS_GRAY:
	    switch (format & DISPLAY_DEPTH_MASK) {
		case DISPLAY_DEPTH_1:
		    palsize = 2;
		    break;
		case DISPLAY_DEPTH_4:
		    palsize = 16;
		    break;
		case DISPLAY_DEPTH_8:
		    palsize = 256;
		    break;
	    }
	    break;
    }
    return palsize;
}


int 
image_preclose(IMAGE *img)
{
    return 0;
}

int
image_presize(IMAGE *img, int width, int height, int raster,
    unsigned int format)
{
    int color = format & DISPLAY_COLORS_MASK;
    int depth = format & DISPLAY_DEPTH_MASK;
    int alpha = format & DISPLAY_ALPHA_MASK;
    img->format_known = FALSE;
    if ( ((color == DISPLAY_COLORS_NATIVE) || 
	  (color == DISPLAY_COLORS_GRAY))
	     &&
	 ((depth == DISPLAY_DEPTH_1) ||
	  (depth == DISPLAY_DEPTH_4) ||
	  (depth == DISPLAY_DEPTH_8)) )
	img->format_known = TRUE;
    if ((color == DISPLAY_COLORS_RGB) && (depth == DISPLAY_DEPTH_8) &&
	(alpha == DISPLAY_ALPHA_NONE))
	img->format_known = TRUE;
    if (!img->format_known) {
	gs_addmessf("display_presize: format %d = 0x%x is unsupported\n", format, format);
	return_error(DISPLAY_ERROR);
    }

    /* remember parameters so we can figure out where to allocate bitmap */
    img->width = width;
    img->height = height;
    img->raster = raster;
    img->format = format;
    return 0;	/* OK */
}

/* Convert the raster format if needed */
int
image_sync(IMAGE *img)
{
    return 0;
}

int image_update_time(IMAGE *img)
{
    DATETIME dt;
    int now_ms;
    DosGetDateTime(&dt);
    now_ms = dt.hours * 86400000 + dt.minutes * 3600000 +
	    dt.seconds * 1000 + dt.hundredths * 10;
    if (now_ms < img->tile_time)
	img->tile_time = now_ms;
    if (now_ms - img->tile_time < img->tile_interval)
	return 1;	/* too frequent */
    img->tile_time = now_ms;
    return 0;
}


/* resize image */
int
image_size(IMAGE *img)
{
    PBITMAPINFO2 bmi;
    int i, nColors;
    bmi = (PBITMAPINFO2) img->bitmap;

    /* write BMP header including palette */
    bmi = (PBITMAPINFO2) img->bitmap;
    bmi->cbFix = BITMAP2_LENGTH;
    bmi->cx = img->width;
    bmi->cy = img->height;
    bmi->cPlanes = 1;
    bmi->cBitCount = 24;
    bmi->ulCompression = BCA_UNCOMP;
    bmi->cbImage = 0;
    bmi->cxResolution = 0;
    bmi->cyResolution = 0;
    bmi->cclrUsed = bmi->cclrImportant = image_palette_size(img->format);

    switch (img->format & DISPLAY_DEPTH_MASK) {
	default:
	case DISPLAY_DEPTH_1:
	    bmi->cBitCount = 1;
	    break;
	case DISPLAY_DEPTH_4:
	    bmi->cBitCount = 4;
	    break;
	case DISPLAY_DEPTH_8:
	    if ((img->format & DISPLAY_COLORS_MASK) == DISPLAY_COLORS_NATIVE)
		bmi->cBitCount = 8;
	    else if ((img->format & DISPLAY_COLORS_MASK) == DISPLAY_COLORS_GRAY)
		bmi->cBitCount = 8;
	    else
		bmi->cBitCount = 24;
	    break;
    }

    /* add palette if needed */
    nColors = bmi->cclrUsed;
    if (nColors) {
	unsigned char *p;
	p = img->bitmap + BITMAP2_LENGTH;
	for (i = 0; i < nColors; i++) {
	    image_color(img->format, i, p+2, p+1, p);
	    *(p+3) = 0;
	    p += 4;
	}
    }

#ifdef DISPLAY_DEBUG
    if (debug) {
    	fprintf(stdout, "\nBMP dump\n");
    	fprintf(stdout, " bitmap=%lx\n", (long)img->bitmap);
    	fprintf(stdout, " cx=%ld\n", bmi->cx);
    	fprintf(stdout, " cy=%ld\n", bmi->cy);
    	fprintf(stdout, " cPlanes=%d\n", bmi->cPlanes);
    	fprintf(stdout, " cBitCount=%d\n", bmi->cBitCount);
    	fprintf(stdout, " ulCompression=%ld\n", bmi->ulCompression);
    	fprintf(stdout, " cbImage=%ld\n", bmi->cbImage);
    	fprintf(stdout, " cxResolution=%ld\n", bmi->cxResolution);
    	fprintf(stdout, " cyResolution=%ld\n", bmi->cyResolution);
    	fprintf(stdout, " cclrUsed=%ld\n", bmi->cclrUsed);
    	fprintf(stdout, " cclrImportant=%ld\n", bmi->cclrImportant);
    }
#endif

    memset(&bitmap, 0, sizeof(bitmap));
    bitmap.pbmi = (BITMAPINFO2 *)image.bitmap;

    /* allow window to be resized without user control */
    fit_page_enabled = option.fit_page;

    return 0;
}


/* We allocate the memory ourselves, so we can place the
 * BMP header before it
 */
void *display_memalloc(void *handle, void *device, unsigned long size)
{
    unsigned long header;
    void *mem = NULL;

#ifdef DISPLAY_DEBUG
    gs_addmesssf("display_memalloc(0x%x 0x%x %ld)\n", 
	(int)handle, (int)device, size);
#endif

    if (image.bitmap) {
	char buf[256];
	sprintf(buf, "display_memalloc: panic, bitmap still allocated\n");
	gs_addmess(buf);
    }

    header = BITMAP2_LENGTH + image_palette_size(image.format) * 4;
    image.bitmap = (unsigned char *)malloc(size+header);
    if (image.bitmap)
	mem = image.bitmap + header;
    
    return mem;
}

int display_memfree(void *handle, void *device, void *mem)
{
#ifdef DISPLAY_DEBUG
    gs_addmessf("display_memfree(0x%x, 0x%x, 0x%x)\n", 
	(int)handle, (int)device, (int)mem);
#endif
    if (image.bitmap) {
	free(image.bitmap);
	image.bitmap = NULL;
    }
    return 0;
}



/* DUMMY TESTING FUNCTIONS */
int
get_message(void)
{
int rc;
  QMSG q_mess;		/* queue message */
  if ( (rc = WinGetMsg(hab, &q_mess, 0L, 0, 0)) != 0)
      WinDispatchMsg(hab, &q_mess);
    return rc;
}


int
peek_message()
{
int rc;
  QMSG q_mess;		/* queue message */
  while ( (rc = WinPeekMsg(hab, &q_mess, 0L, 0, 0, PM_REMOVE)) != 0)
      WinDispatchMsg(hab, &q_mess);
    return rc;
}

void
wait_event(void)
{
ULONG count;
    DosResetEventSem(display.event, &count);
    DosWaitEventSem(display.event, (ULONG)-1);
}

void 
request_mutex(void)
{
    if (multithread)
	DosRequestMutexSem(hmutex_ps, 120000);
}

void 
release_mutex(void)
{
    if (multithread)
	DosReleaseMutexSem(hmutex_ps);
}

void
image_lock(IMAGE *img)
{
    if (debug & img->lock_count)
	gs_addmess("Image is locked\n");
    if (multithread)
	DosRequestMutexSem(img->hmutex, 120000);
    if (debug) {
	if (img->lock_count)
	    gs_addmess("Attempted to lock image twice\n");
	img->lock_count++;
    }
}

void
image_unlock(IMAGE *img)
{
    if (multithread)
	DosReleaseMutexSem(img->hmutex);
    if (debug) {
	if (img->lock_count == 0)
	    gs_addmess("Attempted to unlock unlocked image\n");
	else 
	    img->lock_count--;
    }
}



/* for pstotext */

int
load_pstotext(void)
{
char dllname[MAXSTR];
char buf[MAXSTR];
APIRET rc;
    /* load pstotext DLL */
    strcpy(dllname, szExePath);
    strcat(dllname, "pstotxt2.dll");
    if (DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)dllname, &pstotextModule)) {
	gs_addmess("Can't load ");
        gs_addmess(dllname);
        gs_addmess("\n");
	gs_addmess("Please select Options | PStoText | Disable\n");
	return 1;
    }
    if ((rc = DosQueryProcAddr(pstotextModule, 0, (PCSZ)"pstotextInit", (PFN *)(&pstotextInit))) !=0) {
	sprintf(buf, "Can't find pstotextInit in %s, rc = %ld\n", dllname, rc);
        gs_addmess(buf);
	DosFreeModule(pstotextModule);
	return 1;
    }
    if ((rc = DosQueryProcAddr(pstotextModule, 0, (PCSZ)"pstotextFilter", (PFN *)(&pstotextFilter))) !=0) {
	sprintf(buf, "Can't find pstotextFilter in %s, rc = %ld\n", dllname, rc);
        gs_addmess(buf);
	DosFreeModule(pstotextModule);
	return 1;
    }
    if ((rc = DosQueryProcAddr(pstotextModule, 0, (PCSZ)"pstotextExit", (PFN *)(&pstotextExit))) !=0) {
	sprintf(buf, "Can't find pstotextExit in %s, rc = %ld\n", dllname, rc);
        gs_addmess(buf);
	DosFreeModule(pstotextModule);
	return 1;
    }
    if ((rc = DosQueryProcAddr(pstotextModule, 0, (PCSZ)"pstotextSetCork", (PFN *)(&pstotextSetCork))) !=0) {
	sprintf(buf, "Can't find pstotextSetCork in %s, rc = %ld\n", dllname, rc);
        gs_addmess(buf);
	DosFreeModule(pstotextModule);
	return 1;
    }

    pstotextInit(&pstotextInstance);
    pstotextCount = 0;

    return 0;
}

int
unload_pstotext(void)
{
    if (pstotextOutfile)
	fclose(pstotextOutfile);
    pstotextOutfile = (FILE *)NULL;
    if (pstotextInstance)
        pstotextExit(pstotextInstance);
    pstotextInstance = NULL;
    DosFreeModule(pstotextModule);
    pstotextModule = (HMODULE)NULL;
    pstotextCount = 0;
    return 0;
}


int
send_pstotext_prolog(HINSTANCE hmodule, int resource)
{  
char *prolog, *p;
APIRET rc;
int code = -1;
	rc = DosGetResource(hmodule, RT_RCDATA, resource, (PPVOID)&prolog);
	if (!rc && (prolog != (char *)NULL) ) {
	    code = 0;
	    p = prolog;
	    while (*p) {
		if (!code)
	            code = gs_execute(p, strlen(p));
		p += strlen(p)+1;
	    }
	    DosFreeResource(prolog);
	}
	else {
	    gs_addmess("error: failed to load pstotext resource\n");
	}
	return code;
}

