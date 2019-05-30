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

/* gvcdisp.c */
/* Display GSview routines common to Windows and PM */
#include "gvc.h"

FILE *debug_file;

void gs_puts(char *str, FILE *f);
void gs_copy(FILE *from, FILE *to, long begin, long end);
int get_paper_size_index(void);
int get_paper_user_size_index(void);
void rotate_last_files(int count);
void update_last_files(char *filename);
BOOL dsc_gunzip(PSFILE *psf);
BOOL dsc_bunzip2(PSFILE *psf);
void dsc_dump(PSFILE *psf);
int show_dsc_error(void *caller_data, CDSC *dsc, unsigned int explanation, 
	const char *line, unsigned int line_len);

void
gs_puts(char *str, FILE *f)
{
    if (str != NULL) {
	fputs(str, f);
	if (debug_file != (FILE *)NULL)
	   fputs(str, debug_file);
    }
}

void 
gs_copy(FILE *from, FILE *to, long begin, long end)
{
    char *buf;
    int count;
    buf = (char *)malloc(COPY_BUF_SIZE);
    if (buf == (char *)NULL)
	return;
    if (begin >= end)
	return;
    if (begin >= 0)
	fseek(from, begin, SEEK_SET);
    begin = ftell(from);
    while (begin < end) {
        count = min(end-begin, COPY_BUF_SIZE);
	if ((count = fread(buf, 1, count, from)) > 0) {
	    fwrite(buf, 1, count, to);
	    if (debug_file != (FILE *)NULL)
		fwrite(buf, 1, count, debug_file);
	    begin += count;
	}
	else
	    begin = end;	/* EOF or error */
    }
    free(buf);
}

/* transform cursor position from device coordinates to points */
/* taking into account rotated pages */
void
transform_cursorpos(float *x, float *y)
{
	  if (zoom) {
            /* first figure out number of pixels to zoom origin point */
	    *x = (float)(*x * 72.0/option.xdpi);
	    *y = (float)(*y * 72.0/option.ydpi);
	    transform_point(x,y);
	    *x = (float)(*x * option.xdpi/72);
	    *y = (float)(*y * option.ydpi/72);
	    /* now convert to pts and offset it */
	    *x = (float)(*x * 72/option.zoom_xdpi + display.zoom_xoffset);
	    *y = (float)(*y * 72/option.zoom_ydpi + display.zoom_yoffset);
	  }
	  else {
            int xoffset = (int)(display.xoffset * 72.0 / display.xdpi);
            int yoffset = (int)(display.yoffset * 72.0 / display.ydpi);
	    *x = (float)(*x * 72.0/option.xdpi);
	    *y = (float)(*y * 72.0/option.ydpi);
	    transform_point(x,y);
	    *x = (float)(*x + xoffset);
	    *y = (float)(*y + yoffset);
	  }
}


/* transform point from coordinates relative to bottom left
 * corner of paper to bottom left corner of rotated coordinate
 * system
 */
void
transform_point(float *x, float *y)
{
float oldx, oldy;
int real_orientation;
int width, height;
	oldx = *x;
	oldy = *y;
	width  = (unsigned int)(display.width  * 72.0 / option.xdpi);
	height = (unsigned int)(display.height * 72.0 / option.ydpi);

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

	switch (real_orientation) {
	    case IDM_PORTRAIT:
		break;
	    case IDM_LANDSCAPE:
	    	*x = width - oldy;	/* display.width = bitmap.height */
	    	*y = oldx;
	    	break;
	    case IDM_UPSIDEDOWN:
	    	*x = width - oldx;
	    	*y = height - oldy;
		break;
	    case IDM_SEASCAPE:
	    	*x = oldy;
	    	*y = height - oldx;
	    	break;
	}
	return;
}

/* inverse transform point from coordinates relative 
 * to bottom left corner of rotated coordinate system
 * to bottom left corner of paper 
 */
void
itransform_point(float *x, float *y)
{
float oldx, oldy;
int real_orientation;
int width, height;
	oldx = *x;
	oldy = *y;
	width  = (unsigned int)(display.width  * 72.0 / option.xdpi);
	height = (unsigned int)(display.height * 72.0 / option.ydpi);

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

	switch (real_orientation) {
	    case IDM_PORTRAIT:
		break;
	    case IDM_LANDSCAPE:
	    	*y = width - oldx;
	    	*x = oldy;
	    	break;
	    case IDM_UPSIDEDOWN:
	    	*x = width - oldx;
	    	*y = height - oldy;
		break;
	    case IDM_SEASCAPE:
	    	*y = oldx;
	    	*x = height - oldy;
	    	break;
	}
	return;
}

/* calculate depth */
int
real_depth(int depth)
{
    if (depth == 0)
        depth = display.planes * display.bitcount;
    if (depth > 8)
	depth = 24;
    else if (depth >=8)
	depth = 8;
    else if (depth >=4)
	depth = 4;
    else 
	depth = 1;
    return depth;
}

/* get current media index to dsc_known_media[], or -1 if no match */
int
get_paper_size_index(void)
{
int i;
    for (i=0; dsc_known_media[i].name != (char *)NULL; i++) {
	if (!stricmp(dsc_known_media[i].name, option.medianame))
	    return i;
    }
    return -1;
}

/* get current media index to usermedia, or -1 if no match */
/* usermedia is initialised from the INI file */
int 
get_paper_user_size_index(void)
{
int i;
    for (i=0; i <= (int)(sizeof(usermedia)/sizeof(USERMEDIA)); i++) {
	if (!stricmp(usermedia[i].name, option.medianame))
	    return i;
    }
    return -1;
}

int get_paper_width(void)
{
    int width;
    int i = get_paper_size_index();
    if (i < 0) {
	i = get_paper_user_size_index();
	if (i < 0) {
	    if (option.media_rotate)
		width = option.user_height;
	    else
		width = option.user_width;
	}
	else {
	    if (option.media_rotate)
		width = (int)usermedia[i].height;
	    else
		width = (int)usermedia[i].width;
	}
    }
    else {
	if (option.media_rotate)
	    width = (int)dsc_known_media[i].height;
	else
	    width = (int)dsc_known_media[i].width;
    }
    return width;
}

int get_paper_height(void)
{
    int height;
    int i = get_paper_size_index();
    if (i < 0) {
	i = get_paper_user_size_index();
	if (i < 0) {
	    if (option.media_rotate)
		height = option.user_width;
	    else
		height = option.user_height;
	}
	else {
	    if (option.media_rotate)
		height = (int)usermedia[i].width;
	    else
		height = (int)usermedia[i].height;
	}
    }
    else {
	if (option.media_rotate)
	    height = (int)dsc_known_media[i].width;
	else
	    height = (int)dsc_known_media[i].height;
    }
    return height;
}


/* change the size of the gs image if open */
void
gs_resize(void)
{
	pending.resize = TRUE;

	if ( gsdll.hmodule &&  (psfile.dsc==(CDSC *)NULL) && 
	    (gsdll.state != GS_IDLE) )
	    /* don't know where we are so close and reopen */
	    pending.abort = TRUE;

	if (option.redisplay && (gsdll.state == GS_PAGE)) {
	    if (psfile.dsc != (CDSC *)NULL)
	        pending.now = TRUE;
	    else {
		pending.abort = TRUE;	/* must restart from page 1 */
		pending.now = TRUE;
	    }
	}
	if (option.redisplay && (gsdll.state == GS_IDLE)
	   && (psfile.dsc != (CDSC *)NULL)) {
	     /* zero page EPS file */
	    pending.now = TRUE;
	}
}

void
gs_magnify(float scale)
{
int xtemp, ytemp;
	xtemp = (int)(option.xdpi * scale + 0.5);
	ytemp = (int)(option.ydpi * scale + 0.5);
	if ( (xtemp == option.xdpi) && (scale > 1.0) ) {
	    option.xdpi++;	/* force magnification if requested */
	    option.ydpi++;
	}
	else {
	    option.xdpi = (float)xtemp;
	    option.ydpi = (float)ytemp;
	}
	zoom = FALSE;
	gs_resize();
}

void
gsview_orientation(int new_orientation)
{
	if (new_orientation == IDM_AUTOORIENT) {
	    check_menu_item(IDM_ORIENTMENU, option.orientation, option.auto_orientation);
	    option.auto_orientation = !option.auto_orientation;
	    check_menu_item(IDM_ORIENTMENU, IDM_AUTOORIENT, option.auto_orientation);
	    zoom = FALSE;
	    gs_resize();
	    return;
	}

	if (option.auto_orientation && (new_orientation != IDM_SWAPLANDSCAPE)) {
	    option.auto_orientation = FALSE;
	    check_menu_item(IDM_ORIENTMENU, IDM_AUTOORIENT, option.auto_orientation);
	} 
  	else if (new_orientation == option.orientation)
		return;

	if (new_orientation == IDM_SWAPLANDSCAPE) {
	    option.swap_landscape = !option.swap_landscape;
	    if (option.swap_landscape) 
	        check_menu_item(IDM_ORIENTMENU, IDM_SWAPLANDSCAPE, TRUE);
	    else
	        check_menu_item(IDM_ORIENTMENU, IDM_SWAPLANDSCAPE, FALSE);
	    if ((option.orientation != IDM_LANDSCAPE) && 
		(option.orientation != IDM_SEASCAPE) && 
		(option.auto_orientation == FALSE))
	        return;
	}
	else {
	    check_menu_item(IDM_ORIENTMENU, option.orientation, FALSE);
	    option.orientation = new_orientation;
	    check_menu_item(IDM_ORIENTMENU, option.orientation, TRUE);
	}
        zoom = FALSE;
	gs_resize();
	return;
}

void
gsview_media(int new_media)
{
	char *d, *s;
	if ( (new_media == option.media) && (new_media != IDM_USERSIZE) )
		return;
	check_menu_item(IDM_MEDIAMENU, option.media, FALSE);
	option.media = new_media;
	check_menu_item(IDM_MEDIAMENU, option.media, TRUE);
	get_menu_string(IDM_MEDIAMENU, option.media, option.medianame, sizeof(option.medianame));
	for (d=s=option.medianame; *s; s++) {
	    /* Get rid of shortcut characters */
	    *d = *s;
	    if ((*s != '_') && (*s != '&') && (*s !='~'))
		d++;
	}
	*d = '\0';
	gs_resize();
        zoom = FALSE;
	return;
}

void
gsview_unit(int new_unit)
{
	check_menu_item(IDM_UNITMENU, option.unit, FALSE);
	option.unit = new_unit;
	check_menu_item(IDM_UNITMENU, option.unit, TRUE);
	return;
}

/* free a PSFILE and contents */
/* Do NOT use this if you have just copied PSFILE to psfile */
void
e_free_psfile(PSFILE *ppsfile)
{
    psfile_free(ppsfile);
    free(ppsfile);
}


/* open a new document */
PSFILE *
gsview_openfile(char *filename)
{
int i;
PSFILE *tpsfile;
    history_reset();
    tpsfile = (PSFILE *)malloc(sizeof(PSFILE));
    if (tpsfile == NULL)
	return NULL;
    memset((char *)tpsfile, 0, sizeof(PSFILE));

    strncpy(tpsfile->name, filename, MAXSTR-1);
    tpsfile->pagenum = 1;
    info_wait(IDS_WAITREAD);
    if (dsc_scan(tpsfile)) {
	if ((tpsfile->dsc != (CDSC *)NULL) && !tpsfile->ispdf){
	    CDSC *dsc = tpsfile->dsc;
	    /* found DSC comments */
	    if (dsc->page_media != (CDSCMEDIA *)NULL) {
		char thismedia[20];
		int found = 0;
		for (i=0; i<(int)(sizeof(usermedia)/sizeof(USERMEDIA)); i++) {
		    if (!stricmp(usermedia[i].name, dsc->page_media->name)) {
			gsview_media(i+IDM_USERSIZE1);
			found = 1;
			break;
		    }
		}
		if (!found) {
		      for (i=IDM_MEDIAFIRST; i<IDM_MEDIALAST; i++) {
			get_menu_string(IDM_MEDIAMENU, i, thismedia, 
			sizeof(thismedia));
			if (!stricmp(thismedia, dsc->page_media->name)) {
			    gsview_media(i);
			    found = 1;
			    break;
			}
		      }
		}
		if (!found) {
		    gsview_media(IDM_USERSIZE);
		    option.user_width  = (int)dsc->page_media->width;
		    option.user_height = (int)dsc->page_media->height;
		    gsview_check_usersize();
		}
	    }
	}
    }
    else {
	/* failed to open file */
	e_free_psfile(tpsfile);
	info_wait(IDS_NOWAIT);
	return NULL;
    }
    return tpsfile;
}


void
rotate_last_files(int count)
{
int i;
char buf[MAXSTR];
    strncpy(buf, last_files[count], MAXSTR-1);
    for (i=count; i>0; i--)
	strncpy(last_files[i], last_files[i-1], MAXSTR-1);
    strncpy(last_files[0], buf, MAXSTR-1);
}

void
update_last_files(char *filename)
{
int i;
    for (i=0; i<last_files_count; i++) {
	if (strcmp(filename, last_files[i]) == 0)
	    break;
    }
    if (i < last_files_count) {
	/* already in list */
	rotate_last_files(i);
	return;
    }
    if (last_files_count < 4)
        last_files_count++;
    rotate_last_files(last_files_count-1);
    strncpy(last_files[0], filename, MAXSTR-1);
}

/* get filename then open new file for printing or extract */
void 
gsview_select()
{
char buf[MAXSTR];
	strncpy(buf, psfile.name, MAXSTR-1);
	if (get_filename(buf, FALSE, FILTER_PSALL, 0, IDS_TOPICOPEN))
		gsview_selectfile(buf);
}

/* open new file for printing or extract */
void
gsview_selectfile(char *filename)
{
	while (*filename && *filename==' ')
	     filename++;

	update_last_files(filename);

	if (gsdll.open && (gsdll.state!=GS_UNINIT)) {
	    /* remember name for later */
	    strncpy(selectname, filename, sizeof(selectname));
	    /* close file and wait for notification */
	    post_img_message(WM_COMMAND, IDM_CLOSE);
	}
	else {
	    /* open it ourselves */
	    PSFILE *tpsfile = gsview_openfile(filename);
	    if (tpsfile) {
		psfile_free(&psfile);
		psfile = *tpsfile;
		post_img_message(WM_GSTITLE, 0);
		free(tpsfile);	/* Do NOT free doc and page_list.select */
	    }
	}
	info_wait(IDS_NOWAIT);
}

/* get filename then open a new document and display it */
void 
gsview_display()
{
char buf[MAXSTR];
	strncpy(buf, psfile.name, MAXSTR-1);
	buf[sizeof(buf)-1] = '\0';
	if (get_filename(buf, FALSE, FILTER_PSALL, 0, IDS_TOPICOPEN))
		gsview_displayfile(buf);
}

/* open a new document and display it */
void
gsview_displayfile(char *filename)
{
PSFILE *tpsfile;
	tpsfile = gsview_openfile(filename);
	if (!tpsfile)
	    return;

	update_last_files(filename);

	if (pending.psfile) {
	    message_box_a("pending.psfile is already set", 0);
	    e_free_psfile(tpsfile);
	    return;
	}
	pending.psfile = tpsfile;
	if ( gsdll.hmodule &&  (psfile.dsc==(CDSC *)NULL) && 
	    (gsdll.state != GS_IDLE) )
	    /* don't know where we are so close and reopen */
	    pending.abort = TRUE;
	pending.now = TRUE;
	history_add(1);
}


/* Create and open a scratch file with a given name prefix. */
/* Write the actual file name at fname. */
FILE *
gp_open_scratch_file(const char *prefix, char *fname, const char *mode)
{	char *temp;
#if defined(UNIX) || defined(OS2)
        int fd;
#endif
	if ( (temp = getenv("TEMP")) == NULL )
#ifdef UNIX
		strcpy(fname, "/tmp");
#else
		gs_getcwd(fname, MAXSTR-1);
#endif
	else
		strncpy(fname, temp, MAXSTR-1);

	for ( temp = fname; *temp; temp++ ) {
#if defined(_Windows) || defined(OS2)
		if (*temp == '/')
		    *temp = '\\';
#endif
	}
	if ( strlen(fname) && (fname[strlen(fname)-1] != PATHSEP[0]) )
		strcat(fname, PATHSEP);

	strncat(fname, prefix, MAXSTR-1-strlen(fname));
	strncat(fname, "XXXXXX", MAXSTR-1-strlen(fname));
#ifdef __IBMC__
	{ char *p;
	_tempnam(NULL, fname);
	strncpy(fname, p, MAXSTR-1);
	free(p);
	}
	return fopen(fname, mode);
#else
#if defined(UNIX) || defined(OS2)
	fd = mkstemp(fname);
	return fdopen(fd, mode);
#else
	mktemp(fname);
	return fopen(fname, mode);
#endif
#endif
}

/* This is triggered by WM_ACTIVATE.
 * If the file has changed when we are activated, and Auto
 * redisplay is set, cause the file to be reloaded and 
 * redisplayed.
 * returns 0 is no change, -1 if file deleted, 
 *    1 if changed and should redisplay.
 */
int
dfchanged()
{
char *filename;
GFile *gf;
BOOL changed = FALSE;
int code = 0;
	request_mutex();
	if (psfile.locked) {
	    release_mutex();	/* someone else has it */
	    return 0;
	}
	psfile.locked = TRUE;	/* stop others using it */
	release_mutex();

	filename = psfile_name(&psfile);

	if (filename[0] != '\0') {
	    if ((gf = gfile_open(filename, gfile_modeRead)) != NULL) {
		changed = gfile_changed(gf, psfile.length, 
		    psfile.filetimel, psfile.filetimeh);
		gfile_close(gf);
	    }
	    else 
	        code = -1;
	}
	else
	    code = 0;

        psfile.locked = FALSE;
	if (code < 0)
	    return code;

        if (changed && option.redisplay)
	    return 1;
        return 0;
}


/* reopen psfile */
/* psfile will then be locked until closed */
/* return 0 if OK */
/* if psfile time/date or length has changed, return 1 */
/* return -1 if file can not be opened */
int
dfreopen(void)
{
char *filename;
	if (debug & DEBUG_GENERAL)
	    gs_addmess("dfreopen:\n");
	request_mutex();
	if (psfile.locked) {
	    release_mutex();	/* someone else has it */
	    delayed_message_box(IDS_DEBUG_DFISLOCKED, 0);
	    return -1;
	}
	psfile.locked = TRUE;	/* stop others using it */
	release_mutex();

	filename = psfile_name(&psfile);

	if (psfile.file != NULL) {	/* should never happen */
	    delayed_message_box(IDS_DEBUG_DFISOPEN, 0);
	}

	if (filename[0] == '\0') {
	    psfile.locked = FALSE;
	    delayed_message_box(IDS_NOTOPEN, 0);
	    return -1;
	}

	if ((psfile.file = gfile_open(filename, gfile_modeRead))
	    == (GFile *)NULL) {
	    if (debug)
	        delayed_message_box(IDS_DEBUG_DFISMISSING, 0);
	    filename[0] = '\0';
	    psfile.locked = FALSE;
	    return -1;
	}
	if (gfile_changed(psfile.file, psfile.length, 
	        psfile.filetimel, psfile.filetimeh)) {
	    /* doesn't cope with pdf file changing */
	    dfclose();
	    if (debug)
	        delayed_message_box(IDS_DEBUG_DFCHANGED, 0);
	    return 1;
	}
        if (psfile.ispdf) {
	    /* We needed to open the PDF file to check for changes */
	    /* but we don't need it open for displaying. */
	    dfclose();
	}
	return 0;
}

void
dfclose()
{
	if (debug & DEBUG_GENERAL)
	    gs_addmess("dfclose:\n");
	if (debug) {
	    if ((psfile.file==NULL) && !psfile.ispdf) 
		delayed_message_box(IDS_DEBUG_DFISCLOSED, 0);
	}
	if (psfile.file)
	    gfile_close(psfile.file);
 	psfile.file = NULL;
	psfile.locked = FALSE;
}


#ifndef VIEWONLY
/* gunzip to temporary file */
BOOL
dsc_gunzip(PSFILE *psf)
{
FILE *outfile;
gzFile infile;
char *buffer;
int count;
    
    if (!load_zlib())
	return FALSE;

    /* create buffer for file copy */
    buffer = (char *)malloc(COPY_BUF_SIZE);
    if (buffer == (char *)NULL) {
	play_sound(SOUND_ERROR);
	unload_zlib();
	return FALSE;
    }

    if ((infile = gzopen(psf->name, "rb")) == (gzFile)NULL) {
	play_sound(SOUND_ERROR);
	unload_zlib();
	free(buffer);
	return FALSE;
    }

    if ( (outfile = gp_open_scratch_file(szScratch, psf->tname, "wb")) == (FILE *)NULL) {
	gserror(IDS_NOTEMP, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	gzclose(infile);
	unload_zlib();
	free(buffer);
	return FALSE;
    }

    gs_addmess("Uncompressing ");
    gs_addmess(psf->name);
    gs_addmess(" to ");
    gs_addmess(psf->tname);
    gs_addmess("\n");
	
    while ( (count = gzread(infile, buffer, COPY_BUF_SIZE)) > 0 ) {
	fwrite(buffer, 1, count, outfile);
    }
    free(buffer);
    gzclose(infile);
    fclose(outfile);
    /* unload_zlib(); */
    if (count < 0)
	return FALSE;
    return TRUE;
}

#if defined(_Windows) || defined(UNIX)
/* Uncompress bzip2 to temporary file */
BOOL
dsc_bunzip2(PSFILE *psf)
{
FILE *outfile;
bzFile infile;
char *buffer;
int count;
    
    if (!load_bzip2())
	return FALSE;

    /* create buffer for file copy */
    buffer = (char *)malloc(COPY_BUF_SIZE);
    if (buffer == (char *)NULL) {
	play_sound(SOUND_ERROR);
	unload_bzip2();
	return FALSE;
    }

    if ((infile = bzopen(psf->name, "rb")) == (bzFile)NULL) {
	play_sound(SOUND_ERROR);
	unload_bzip2();
	free(buffer);
	return FALSE;
    }

    if ( (outfile = gp_open_scratch_file(szScratch, psf->tname, "wb")) == (FILE *)NULL) {
	gserror(IDS_NOTEMP, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	bzclose(infile);
	unload_bzip2();
	free(buffer);
	return FALSE;
    }
    gs_addmess("Uncompressing ");
    gs_addmess(psf->name);
    gs_addmess(" to ");
    gs_addmess(psf->tname);
    gs_addmess("\n");
	
    while ( (count = bzread(infile, buffer, COPY_BUF_SIZE)) > 0 ) {
	fwrite(buffer, 1, count, outfile);
    }
    free(buffer);
    bzclose(infile);
    fclose(outfile);
    /* unload_bzip2(); */
    if (count < 0)
	return FALSE;
    return TRUE;
}
#endif
#endif /* !VIEWONLY */


/* Debug for DSC comments */
void
dsc_dump(PSFILE *psf)
{
char buf[MAXSTR];
    sprintf(buf, "DSC dump for %.200s\n", psf->name);
    gs_addmess(buf);
    dsc_display(psf->dsc, dsc_addmess);

    sprintf(buf, "End of DSC dump\n");
    gs_addmess(buf);
}

int 
show_dsc_error(void *caller_data, CDSC *dsc, unsigned int explanation, 
	const char *line, unsigned int line_len)
{
    int response = CDSC_RESPONSE_CANCEL;
    int severity;
    char buf[MAXSTR];
    int len;
    char title[MAXSTR];
    char linefmt[MAXSTR];
    int i;
    char *p;
    int count;

    if (explanation > dsc->max_error)
	return CDSC_RESPONSE_OK;

    severity = dsc->severity[explanation];

    /* If debug function provided, copy messages there */
    if (dsc->debug_print_fn) {
	switch (severity) {
	    case CDSC_ERROR_INFORM:
		dsc_debug_print(dsc, "\nDSC Information");
		break;
	    case CDSC_ERROR_WARN:
		dsc_debug_print(dsc, "\nDSC Warning");
		break;
	    case CDSC_ERROR_ERROR:
		dsc_debug_print(dsc, "\nDSC Error");
	}
	dsc_debug_print(dsc, "\n");
	if (explanation <= dsc->max_error) {
	    if (line && line_len) {
		int length = min(line_len, sizeof(buf)-1);
		sprintf(buf, "At line %d:\n", dsc->line_count);
		dsc_debug_print(dsc, buf);
		strncpy(buf, line, length);
		buf[length]='\0';
		dsc_debug_print(dsc, "  ");
		dsc_debug_print(dsc, buf);
	    }
	    dsc_debug_print(dsc, dsc_message[explanation]);
	}
    }

    /* Here you could prompt user for OK, Cancel, Ignore ALL DSC */
    if (option.dsc_warn == IDM_DSC_OFF)
	return response;
    else if ((option.dsc_warn == IDM_DSC_ERROR)
	&& (severity < CDSC_ERROR_ERROR))
	return response;
    else if ((option.dsc_warn == IDM_DSC_WARN)
	&& (severity < CDSC_ERROR_WARN))
	return response;
    
    switch (severity) {
	case CDSC_ERROR_INFORM:
	    i = IDS_DSC_INFO;
	    break;
	case CDSC_ERROR_WARN:
	    i = IDS_DSC_WARN;
	    break;
	case CDSC_ERROR_ERROR:
	    i = IDS_DSC_ERROR;
	    break;
	default:
	    i = -1;
    }
    if (i != -1)
        load_string_a(i, title, sizeof(title));
    else
	title[0] = '\0';

    /* build up string  */
#define MSGBUFLEN 4096
    p = (char *)malloc(MSGBUFLEN);
    if (p == (char *)NULL)
	return response;

    if (line) {
        load_string_a(IDS_DSC_LINEFMT, linefmt, sizeof(linefmt));
        sprintf(p, linefmt, title, dsc->line_count);
	strcat(p, "\n   ");
	len = strlen(p);
	count = min((int)line_len, MSGBUFLEN-len-2);
	strncpy(p+len, line, count);
        p[len+count] = '\0';
    }
    else {
	strcpy(p, title);
	strcat(p, "\n");
    }
    len = strlen(p);
    load_string_a(CDSC_RESOURCE_BASE+(explanation*4), p+len, 4096-len);
    len = strlen(p);
    load_string_a(CDSC_RESOURCE_BASE+(explanation*4)+1, p+len, 4096-len);
    len = strlen(p);
    load_string_a(CDSC_RESOURCE_BASE+(explanation*4)+2, p+len, 4096-len);
    
    response = get_dsc_response(p);

    free(p);

    if (dsc->debug_print_fn) {
	switch (response) {
	    case CDSC_RESPONSE_OK:
		dsc_debug_print(dsc, "Response = OK\n");
		break;
	    case CDSC_RESPONSE_CANCEL:
		dsc_debug_print(dsc, "Response = Cancel\n");
		break;
	    case CDSC_RESPONSE_IGNORE_ALL:
		dsc_debug_print(dsc, "Response = Ignore All DSC\n");
		break;
	}
    }

    return response;
}

void dsc_addmess(void *caller_data, const char *str)
{
    gs_addmess(str);
}


/* scan file for PostScript Document Structuring Conventions */
/* return FALSE if error */
/* On return, psf->dsc is non-zero if DSC */
BOOL
dsc_scan(PSFILE *psf)
{
char line[MAXSTR];
unsigned long file_length;
CDSC *dsc = NULL;
	if (psf->file) {
	    message_box(TEXT("dsc_scan: file is open but shouldn't be"), 0);
	    gfile_close(psf->file);
	    psf->file = NULL;
	}

	if (psf->locked) {
	    TCHAR buf[MAXSTR];
	    load_string(IDS_DEBUG_DFISLOCKED, buf, sizeof(buf));
	    message_box(buf, 0);
	    return FALSE;
	}
	psf->locked = TRUE;	/* stop others using it */
	if ( (psf->file = gfile_open(psf->name, gfile_modeRead)) == NULL) {
	    char buf[MAXSTR+MAXSTR];
	    sprintf(buf, "File '%.200s' does not exist", psf->name);
	    message_box_a(buf, 0);
	    psf->locked = FALSE;
	    return FALSE;
	}

	/* these shouldn't be needed */
	if (psf->page_list.select)
	    free(psf->page_list.select);
	psf->page_list.select = NULL;
	if (psf->dsc)
	    dsc_free(psf->dsc);
	psf->preview = 0;
	
	/* get first line to look for magic numbers */
	gfile_read(psf->file, line, sizeof(line)-1);
	gfile_seek(psf->file, 0, gfile_begin);

#ifndef VIEWONLY
	/* check for gzip */
	psf->gzip = FALSE;
	psf->bzip2 = FALSE;
	if ( (line[0]=='\037') && (line[1]=='\213') ) { /* 1F 8B */
	    psf->gzip = TRUE;
	    gfile_close(psf->file);
	    psf->file = NULL;
	    if (!dsc_gunzip(psf)) {
/* ENGLISH */
		message_box(TEXT("Failed to gunzip file"), 0);
		psf->locked = FALSE;
		return FALSE;
	    }
	    if ( (psf->file = gfile_open(psfile_name(psf), gfile_modeRead)) 
		== NULL) {
		char buf[MAXSTR+MAXSTR];
		sprintf(buf, "File '%.200s' does not exist", psfile_name(psf));
		message_box_a(buf, 0);
		psf->locked = FALSE;
		return FALSE;
	    }
	    gfile_read(psf->file, line, sizeof(line)-1);
	    gfile_seek(psf->file, 0, gfile_begin);
	}

	/* check for bzip2 */
	if ( (line[0]=='B') && (line[1]=='Z') && (line[2]=='h')) { /* "BZh */
	    psf->bzip2 = TRUE;
	    gfile_close(psf->file);
	    psf->file = NULL;
#if defined(_Windows) || defined(UNIX)
	    if (!dsc_bunzip2(psf)) {
/* ENGLISH */
		message_box(TEXT("Failed to uncompress bzip2 file"), 0);
		psf->locked = FALSE;
		return FALSE;
	    }
#else
	    message_box(TEXT("This is a bzip2 file.  Please uncompress it first."), 0);
	    psf->locked = FALSE;
	    return FALSE;
#endif
	    if ( (psf->file = gfile_open(psfile_name(psf), gfile_modeRead)) 
		== NULL) {
		char buf[MAXSTR+MAXSTR];
		sprintf(buf, "File '%.200s' does not exist", psfile_name(psf));
		message_box_a(buf, 0);
		psf->locked = FALSE;
		return FALSE;
	    }
	    gfile_read(psf->file, line, sizeof(line)-1);
	    gfile_seek(psf->file, 0, gfile_begin);
	}
#endif /* !VIEWONLY */

	/* save file date and length */
	psf->length = gfile_get_length(psf->file);
	gfile_get_datetime(psf->file, &psf->filetimel, &psf->filetimeh);

	/* check for PDF */
	psf->ispdf = FALSE;
	if ( strncmp("%PDF-", line, 5) == 0 ) {
	    gfile_close(psf->file);
	    psf->file = NULL;
	    psf->locked = FALSE;
	    psf->ispdf = TRUE;
	    return TRUE;	/* we don't know how many pages yet */
	}

	/* check for PCL */
	if ((line[0]=='\033') && (line[1]=='E') && (line[2]=='\033')) {
	    TCHAR buf[MAXSTR];
	    load_string(IDS_PROBABLY_PCL, buf, sizeof(buf));
	    if (message_box(buf, MB_ICONEXCLAMATION | MB_YESNO) != IDYES) {
		gfile_close(psf->file);
		psf->file = NULL;
		psf->locked = FALSE;
		return FALSE;
	    }
	}



	/* check for documents that start with Ctrl-D */
	psf->ctrld = (line[0] == '\004');
	/* check for HP LaserJet prologue */
	psf->pjl = FALSE;
	if (strncmp("\033%-12345X", line, 9) == 0)
	    psf->pjl = TRUE;
	if (option.ignore_dsc)
	    psf->dsc = (CDSC *)NULL;
	else  {
	    int code = 0;
	    int count;
	    char *d;
	    psf->dsc = NULL;
	    if ( (d = (char *) malloc(COPY_BUF_SIZE)) == NULL)
		return FALSE;
   
	    gfile_seek(psf->file, 0, gfile_begin);
	    psf->dsc = dsc_init(NULL);
	    dsc_set_debug_function(psf->dsc, dsc_addmess);
	    dsc_set_error_function(psf->dsc, show_dsc_error);
	    dsc_set_length(psf->dsc, gfile_get_length(psf->file));
	    while ((count = gfile_read(psf->file, d, COPY_BUF_SIZE))!=0) {
		code = dsc_scan_data(psf->dsc, d, count);
	 	if ((code == CDSC_ERROR) || (code == CDSC_NOTDSC)) {
		    /* not DSC or an error */
		    break;
		}
	    }
	    free(d);
	    d = NULL;
	    if ((code == CDSC_ERROR) || (code == CDSC_NOTDSC)) {
		dsc_free(psf->dsc);
		psf->dsc = NULL;
	    }
	    else {
		dsc_fixup(psf->dsc);
		if (debug & DEBUG_GENERAL)
		    dsc_display(psf->dsc, dsc_addmess);
	    }
	}

	file_length = gfile_get_length(psf->file);
	gfile_close(psf->file);
	psf->file = NULL;
	psf->locked = FALSE;

	/* check for DSC comments */
	dsc = psf->dsc;
	if (dsc == (CDSC *)NULL)
	    return TRUE;	/* OK, but not DSC */

	if (dsc->doseps) {
	    BOOL bad_header = FALSE;
	    /* check what sort of preview is present */
	    if (dsc->doseps->tiff_begin)
		psf->preview = IDS_EPST;
	    if (dsc->doseps->wmf_begin)
		psf->preview = IDS_EPSW;
	    /* check for errors in header */
	    if (dsc->doseps->ps_begin > file_length)
		bad_header = TRUE;
	    if (dsc->doseps->ps_begin + dsc->doseps->ps_length > file_length)
		bad_header = TRUE;
	    if (dsc->doseps->wmf_begin > file_length)
		bad_header = TRUE;
	    if (dsc->doseps->wmf_begin + dsc->doseps->wmf_length > file_length)
		bad_header = TRUE;
	    if (dsc->doseps->tiff_begin > file_length)
		bad_header = TRUE;
	    if (dsc->doseps->tiff_begin + dsc->doseps->tiff_length > file_length)
		bad_header = TRUE;
	    if (bad_header) {
		TCHAR buf[MAXSTR];
	        load_string(IDS_BAD_DOSEPS_HEADER, buf, sizeof(buf));
	        message_box(buf, 0);
		/* Ignore the bad information */
		dsc_free(psf->dsc);
		psf->dsc = (CDSC *)NULL;
		return FALSE;
	    }
	}
	if (!psf->preview && (dsc->beginpreview != dsc->endpreview))
	    psf->preview = IDS_EPSI;
	if (dsc->page_count) {
	    psf->page_list.select = 
		(BOOL *)malloc(dsc->page_count * sizeof(BOOL));
	    if (psf->page_list.select)
	        memset(psf->page_list.select, 0, 
			dsc->page_count * sizeof(BOOL));
	}
	if (dsc->epsf) {
	    /* warn if bounding box off the page */
	    int width, height;
	    width = get_paper_width();
	    height = get_paper_height();
	    if ( !option.epsf_clip &&
		(dsc->bbox != (CDSCBBOX *)NULL) &&
	        ((dsc->bbox->llx > width) || 
		 (dsc->bbox->lly > height) ||
	         (dsc->bbox->urx < 0) || 
		 (dsc->bbox->ury < 0)) )
	    {
		TCHAR buf[MAXSTR];
	        load_string(IDS_EPS_OFF_PAGE, buf, sizeof(buf));
	        message_box(buf, 0);
	    }
	    if ( option.epsf_clip && 
		((dsc->bbox == (CDSCBBOX *)NULL) || 
		 (dsc->bbox->llx > dsc->bbox->urx) || 
		 (dsc->bbox->lly > dsc->bbox->ury))
	       )
	    {
		TCHAR buf[MAXSTR];
	        load_string(IDS_EPS_BAD_BBOX, buf, sizeof(buf));
	        message_box(buf, 0);
	    }
	}

	if (debug)
	    dsc_dump(psf);

	return TRUE;
}



/* reverse zero based page number if needed */
int
map_page(int page)
{
    if (psfile.dsc != (CDSC *)NULL)
        if (psfile.dsc->page_order == CDSC_DESCEND) 
	    return (psfile.dsc->page_count - 1) - page;
    return page;
}

/* Generate string for page list box, as either
 *   ordinal
 *   ordinal    "label"
 */
void
page_ordlabel(char *buf, int page_number)
{
    const char *label;
    int ordinal;
    label = psfile.dsc->page[map_page(page_number)].label;
    ordinal = psfile.dsc->page[map_page(page_number)].ordinal;
    sprintf(buf, "%d", ordinal);
    if (strcmp(buf, label) != 0) {
	/* label and ordinal don't match, so use composite */
	sprintf(buf, "%d  \042%s\042", ordinal, label);
    }
}

void
psfile_free(PSFILE *psf)
{
    if (psf == (PSFILE *)NULL)
	return;

    psf->name[0] = '\0';

    /* same as dfclose() */
    if (psf->file)
	gfile_close(psf->file);
    psf->file = NULL;
    psf->locked = FALSE;

    if ((psf->tname[0] != '\0') && (!debug))
	unlink(psf->tname);
    psf->tname[0] = '\0';


    if (psf->page_list.select)
	free(psf->page_list.select);
    psf->page_list.select = NULL;
    if (psf->dsc)
	dsc_free(psf->dsc);
    psf->dsc = (CDSC *)NULL;
#ifndef VIEWONLY
    if (psf->text_name) {
	if (!debug)
	    unlink(psf->text_name);
        psf->text_name[0] = '\0';
	free_text_index();
    }
#endif
}

char *
psfile_name(PSFILE *psf)
{
    /* if original file was gzipped, give name of gunzipped file */
    if ((psf->tname[0]!='\0') && ((psf->gzip) || psf->bzip2))
	return psf->tname;
    /* otherwise return original file name */
    return psf->name;
}

/* #define DEBUG_HISTORY */
#ifdef DEBUG_HISTORY
void history_debug(void)
{
char buf[256];
int i;
    gs_addmess("history_debug: ");
    for (i=0; i<history.count; i++) {
	if (i == history.index)
	    gs_addmess("[");
        sprintf(buf, "%d", history.pages[i]);
	gs_addmess(buf);
	if (i == history.index)
	    gs_addmess("]");
	gs_addmess(" ");
    }
    gs_addmess("\n");
}
#define HISTORY_DEBUG history_debug()
#else
#define HISTORY_DEBUG
#endif

void
history_add(int pagenum)
{
    /* scroll if history full */
    if (history.index >= HISTORY_MAX) {
	memmove(&history.pages[0], &history.pages[1], 
 	    (HISTORY_MAX-1)*sizeof(history.pages[0]));
	history.index--;
	history.count = history.index;
    }

    if ((history.index > 1) && (history.pages[history.index -1] == pagenum)) {
	/* Don't insert duplicate page */
	return;
    }
    if (history.pages[history.index] != pagenum) {
	/* If we are following a different path to last time,
	 * truncate history
	 */ 
	history.count = history.index;
    }

    history.pages[history.index] = pagenum;

    history.index++;
    if (history.index > history.count)
	history.count = history.index;
    HISTORY_DEBUG;
}


void
history_reset(void)
{
    history.index = history.count = 0;
    HISTORY_DEBUG;
}


void
history_back(void)
{
    if (history.index < 2)
	return;	/* can't do anything */

    history.index--;	/* point to current page */

    request_mutex();
    pending.pagenum = history.pages[history.index - 1];
    if (pending.pagenum > (int)psfile.dsc->page_count)
	 pending.pagenum = psfile.dsc->page_count;
    if (pending.pagenum < 1)
	pending.pagenum = 1;
    gsview_unzoom();
    pending.now = TRUE;
    release_mutex();
    HISTORY_DEBUG;
}

void
history_forward(void)
{
    if (history.index >= history.count) {
	gs_page_skip(1);
	return;
    }

    request_mutex();
    pending.pagenum = history.pages[history.index];
    if (pending.pagenum > (int)psfile.dsc->page_count)
	 pending.pagenum = psfile.dsc->page_count;
    if (pending.pagenum < 1)
	pending.pagenum = 1;
    gsview_unzoom();
    pending.now = TRUE;
    release_mutex();

    history.index++;
    HISTORY_DEBUG;
}
