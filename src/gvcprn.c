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

/* gvcprn.c */
/* Printer module of PM and Windows GSview */

#include "gvc.h"

#ifndef _MSC_VER  /* Brain damaged MSVC++ 5.0 doesn't support POSIX dirent.h */
#include <dirent.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

void copy_setup(FILE *f, GFile *infile, int copies) ;
int upp_add_list(char *name, char *buffer, int len, int offset);
int enum_upp(char *path, char *buffer, int len, int offset);
char * uppname_to_model(char *buffer, char *name);
void psfile_epsf_print(FILE *f, int copies);
BOOL gsview_copygfile(FILE *outfile, GFile *infile);
BOOL gsview_copyfile(FILE *outfile, FILE *infile);

void add_copies(FILE *f, int copies)
{
   if (copies >= 1) {
      fprintf(f, "[{\n%%%%BeginFeature: *NumCopies %d\n", copies);
      fprintf(f, "<< /NumCopies %d >> setpagedevice\n", copies);
      fprintf(f, "%%%%EndFeature\n} stopped cleartomark\n");
   }
}

struct prop_item_s *
get_properties(const char *device)
{
char *entries, *p, *q;
int i, numentry;
struct prop_item_s *proplist;
PROFILE *prf;
	entries = (char *)malloc(PROFILE_SIZE);
	if (entries == (char *)NULL)
	   return NULL;
	if ( (prf = profile_open(szIniFile)) == (PROFILE *)NULL) {
	    free(entries);
	    return NULL;
	}
	profile_read_string(prf, device, NULL, "", entries, PROFILE_SIZE);
/*
	if (strlen(entries) == 0) {
	    profile_close(prf);
	    free(entries);
	    return NULL;
	}
*/
	p = entries;
	for (numentry=0; p!=(char *)NULL && strlen(p)!=0; numentry++)
	    p += strlen(p) + 1;
	proplist = (struct prop_item_s *)malloc((numentry+1) * sizeof(struct prop_item_s));
	if (proplist == (struct prop_item_s *)NULL) {
	    profile_close(prf);
	    free(entries);
	    return NULL;
	}
	p = entries;
	for (i=0; i<numentry; i++) {
	    strcpy(proplist[i].name, p);
	    profile_read_string(prf, device, p, "", proplist[i].value, sizeof(proplist->value));
	    q = proplist[i].value;
	    while ((*q) && (q[strlen(q)-1]==' '))
		q[strlen(q)-1] = '\0';    /* remove trailing spaces */
	    p += strlen(p) + 1;
	}
	proplist[numentry].name[0] = '\0';
	proplist[numentry].value[0] = '\0';
	profile_close(prf);
	free(entries);
	return proplist;
}

char *
get_devices(BOOL convert)
{
char *p;
PROFILE *prf;
const char *section = convert ? CONVERTSECTION: DEVSECTION ;
	if ( (prf = profile_open(szIniFile)) == (PROFILE *)NULL)
	    return (char *)NULL;

	if ( (p = (char *)malloc(PROFILE_SIZE)) == (char *)NULL) {
	    profile_close(prf);
	    return (char *)NULL;
	}

	profile_read_string(prf, section, NULL, "", p, PROFILE_SIZE);
	if (strlen(p) == 0) {
	    /* [Devices] section doesn't exist.  Initialise from resources */
	    profile_create_section(prf, section, IDR_DEVICES);
	}
	profile_read_string(prf, section, NULL, "", p, PROFILE_SIZE);
	profile_close(prf);
	return p;
}


/* get a filename and spool it for printing */
void
gsview_spool(char *fname, char *port)
{
	static char filename[MAXSTR];

	if (fname == (char *)NULL) {
	    if (!get_filename(filename, FALSE, FILTER_ALL, IDS_PRINTFILE, IDS_TOPICPRINT))
		return;
	}
	else {
	    while (*fname && *fname==' ')
	        fname++;
	    strncpy(filename, fname, MAXSTR);
	}

	if (!gp_printfile(filename, port)) {
		play_sound(SOUND_ERROR);
		return;
	}
}


/* save entire file */
/* added to save files when GSview used as a WWW viewer */
void
gsview_saveas()
{
FILE *f;
char output[MAXSTR];
FILE *infile;
UINT count;
char *buffer;
int filter;

    memset(output, 0, sizeof(output));
    if (psfile.name[0] == '\0') {
	    gserror(IDS_NOTOPEN, NULL, MB_ICONEXCLAMATION, SOUND_NOTOPEN);
	    return;
    }
    strncpy(output, psfile.name, sizeof(output)-1);

    filter = psfile.ispdf ? FILTER_PDF :
	( psfile.dsc && psfile.dsc->epsf ? FILTER_EPS : FILTER_PS );
    nHelpTopic = IDS_TOPICOPEN;
    if (!get_filename(output, TRUE, filter, 0, IDS_TOPICOPEN))
	    return;

    if ((f = fopen(output, "wb")) == (FILE *)NULL) {
	    return;
    }

    /* create buffer for PS file copy */
    buffer = (char *)malloc(COPY_BUF_SIZE);
    if (buffer == (char *)NULL) {
	play_sound(SOUND_ERROR);
	fclose(f);
	unlink(output);
	return;
    }

    /* don't use dfreopen, since that wouldn't work for PDF files */
    infile = fopen(psfile_name(&psfile), "rb");
    if (infile == (FILE *)NULL) {
	play_sound(SOUND_ERROR);
	free(buffer);
	fclose(f);
	unlink(output);
	return;
    }

    info_wait(IDS_WAITWRITE);

    while ( (count = fread(buffer, 1, COPY_BUF_SIZE, infile)) != 0 ) {
	fwrite(buffer, 1, count, f);
    }
    free(buffer);
    fclose(infile);
    fclose(f);

    info_wait(IDS_NOWAIT);
    return;
}

/* extract a range of pages for later printing */
void
gsview_extract()
{
    FILE *f;
    static char output[MAXSTR];
    int thispage = psfile.pagenum;
    CDSC *dsc = psfile.dsc;

    if (psfile.name[0] == '\0') {
	gserror(IDS_NOTOPEN, NULL, MB_ICONEXCLAMATION, SOUND_NOTOPEN);
	return;
    }

    if (dsc == (CDSC *)NULL) {
	gserror(IDS_NOPAGE, NULL, MB_ICONEXCLAMATION, SOUND_NONUMBER);
	return;
    }
    
    if (psfile.ispdf && (dsc->page_count == 0)) {
	TCHAR buf[MAXSTR];
	load_string(IDS_PDFNOPAGE, buf, sizeof(buf));
	message_box(buf, MB_ICONASTERISK | MB_OK);
	return;
    }

    if (psfile.ispdf) {
	TCHAR buf[MAXSTR];
	load_string(IDS_USEPDFWRITE, buf, sizeof(buf));
	message_box(buf, MB_ICONASTERISK | MB_OK);
	return;
    }

    nHelpTopic = IDS_TOPICOPEN;
    if (dsc->page_count != 0)
	if (!get_page(&thispage, TRUE, FALSE))
	    return;


    if (!get_filename(output, TRUE, FILTER_PS, 0, IDS_TOPICOPEN))
	return;

    if ((f = fopen(output, "wb")) == (FILE *)NULL)
	return;

    load_string(IDS_WAITWRITE, szWait, sizeof(szWait));
    info_wait(IDS_WAITWRITE);
    if (dfreopen() != 0) {
	fclose(f);
	unlink(output);
	gserror(0, "Couldn't reopen document", MB_ICONEXCLAMATION, SOUND_NOTOPEN);
	return;
    }
    if (dsc->page_count != 0)
	psfile_extract(f, 1);
    else {
	ps_copy(f, psfile.file, dsc->begincomments, dsc->endcomments);
	ps_copy(f, psfile.file, dsc->begindefaults, dsc->enddefaults);
	ps_copy(f, psfile.file, dsc->beginprolog, dsc->endprolog);
	ps_copy(f, psfile.file, dsc->beginsetup, dsc->endsetup);
	ps_copy(f, psfile.file, dsc->begintrailer, dsc->endtrailer);
    }
    dfclose();

    fclose(f);

    info_wait(IDS_NOWAIT);
    return;
}


void
copy_setup(FILE *f, GFile *infile, int copies) 
{
    char line[DSC_LINE_LENGTH+1];
    if (copies > 1) {
	if (psfile.dsc->beginsetup != psfile.dsc->endsetup) {
	    gfile_seek(infile, psfile.dsc->beginsetup, gfile_begin);
	    /* copy up to, but no including %%EndSetup */
	    ps_copy_find(f, infile, psfile.dsc->endsetup,
		line, sizeof(line)-1, "%%EndSetup");
	    /* insert code for multiple copies */
	    add_copies(f, copies);
	    /* copy rest of setup section */
	    if (strlen(line))
		fputs(line, f);
	    ps_copy(f, infile, -1, psfile.dsc->endsetup);
	}
	else {
	    /* setup section was missing - add our own. */
	    fputs("%%BeginSetup\n", f);
	    add_copies(f, copies);
	    fputs("%%EndSetup\n", f);
	}
    }
    else
        ps_copy(f, infile, psfile.dsc->beginsetup, psfile.dsc->endsetup);
}

/* Copy the headers, marked pages, and trailer to f */
/* Reverse the page order if needed and possible */
BOOL
psfile_extract(FILE *f, int copies)
{
    char line[DSC_LINE_LENGTH+1];
    BOOL pages_written = FALSE;
    BOOL pageorder_written = FALSE;
    int pages = 0;
    int page;
    int i;
    unsigned long position;
    CDSC *dsc = psfile.dsc;
    int neworder = dsc->page_order;
    BOOL reverse = psfile.page_list.reverse;
    BOOL end_header;
    BOOL line_written;

    if (neworder == CDSC_ORDER_UNKNOWN)	/* No page order so assume ASCEND */
	neworder = CDSC_ASCEND;
    /* Don't touch SPECIAL pageorder */

    /* reverse means new page order to be DESCEND */
    if (reverse) {
	if (neworder == CDSC_ASCEND)
	    neworder = CDSC_DESCEND;
	else if (neworder == CDSC_DESCEND) {
	    /* neworder = DESCEND;*/	/* unchanged */
	    reverse = FALSE;	/* already reversed, don't do it again */
	}
    }
    else {
	if (neworder == CDSC_DESCEND) {
	    neworder = CDSC_ASCEND;
	    reverse = TRUE;	/* reverse it to become ascending */
	}
    }
    /* neworder = page order of the extracted document */
    /* reverse = reverse the current page order */

    for (i=0; i<(int)dsc->page_count; i++) {
	    if (psfile.page_list.select[i]) pages++;
    }

    /* copy header, fixing up %%Pages: and %%PageOrder:
     * Write a DSC 3.0 %%Pages: or %%PageOrder: in header,
     * even if document was DSC 2.x.
     * Remove %%Pages: and %%PageOrder from trailer.
     */
    position = gfile_seek(psfile.file, dsc->begincomments, gfile_begin);
    while ( position < dsc->endcomments ) {
	if (ps_fgets(line, sizeof(line), psfile.file) == 0)
	    return FALSE;
	position = gfile_seek(psfile.file, 0, gfile_current);
	end_header = (strncmp(line, "%%EndComments", 13) == 0);
	if ((line[0] != '%') && (line[0] != ' ') && (line[0] != '+')
	 && (line[0] != '\t') && (line[0] != '\r') && (line[0] != '\n'))
	    end_header = TRUE;
	line_written = FALSE;
	if (end_header || strncmp(line, "%%Pages:", 8) == 0) {
	    if (!pages_written) {
		fprintf(f, "%%%%Pages: %d\r\n", pages);
		pages_written = TRUE;
	    }
	    line_written = !end_header;
	}
	if (end_header || strncmp(line, "%%PageOrder:", 12) == 0) {
	    if (!pageorder_written) {
		if (neworder == CDSC_ASCEND)
		    fputs("%%PageOrder: Ascend\r\n", f);
		else if (neworder == CDSC_DESCEND)
		    fputs("%%PageOrder: Descend\r\n", f);
		else 
		    fputs("%%PageOrder: Special\r\n", f);
		pageorder_written = TRUE;
	    }
	    line_written = !end_header;
	}
	if (!line_written) {
	    fputs(line, f);	
	}
    }
    if (dsc->begincomments != dsc->endcomments) {
	if (!pages_written) {
	    fprintf(f, "%%%%Pages: %d\r\n", pages);
	    /* pages_written = TRUE; */
	}
	if (!pageorder_written) {
	    if (neworder == CDSC_ASCEND)
		fputs("%%PageOrder: Ascend\r\n", f);
	    else if (neworder == CDSC_DESCEND)
		fputs("%%PageOrder: Descend\r\n", f);
	    else 
		fputs("%%PageOrder: Special\r\n", f);
	    /* pageorder_written = TRUE; */
	}
    }

    ps_copy(f, psfile.file, dsc->beginpreview, dsc->endpreview);
    ps_copy(f, psfile.file, dsc->begindefaults, dsc->enddefaults);
    ps_copy(f, psfile.file, dsc->beginprolog, dsc->endprolog);
    copy_setup(f, psfile.file, copies);

    page = 1;
    i = reverse ? dsc->page_count - 1 : 0;
    while ( reverse ? (i >= 0)  : (i < (int)dsc->page_count) ) {
	if (psfile.page_list.select[map_page(i)])  {
	    char buf[MAXSTR];
	    /* modify ordinal of %%Page: comment */
	    position = gfile_seek(psfile.file, dsc->page[i].begin, gfile_begin);
	    if (ps_fgets(buf, sizeof(buf)-1, psfile.file) == 0)
	        return FALSE;

	    /* reached end of %%Page: line */
	    if (dsc->page[i].label)
		fprintf(f, "%%%%Page: %s %d\r\n", 
		    dsc->page[i].label, page);
	    else
		fprintf(f, "%%%%Page: %d %d\r\n", page, page);
	    page++;
	    ps_copy(f, psfile.file, -1, dsc->page[i].end);
	}
        i += reverse ? -1 : 1;
    }

    /* copy trailer, removing %%Pages: and %%PageOrder: */
    position = gfile_seek(psfile.file, dsc->begintrailer, gfile_begin);
    while ( position < dsc->endtrailer ) {
	if (ps_fgets(line, sizeof(line), psfile.file) == 0)
	    return FALSE;
        position = gfile_seek(psfile.file, 0, gfile_current);
	if (strncmp(line, "%%Pages:", 8) == 0) {
	    continue;	/* has already been written in header */
	}
	else if (strncmp(line, "%%PageOrder:", 12) == 0) {
	    continue;	/* has already been written in header */
	}
	else {
	    fputs(line, f);	
	}
    }
    return TRUE;
}

#ifdef _MSC_VER
/* Brain damaged Microsoft C doesn't support POSIX directory operations.
 * Implement a subset of these ourselves to keep the uniprint
 * enumeration code happy.
 */
struct dirent
{
    char        d_name[260];
};

typedef struct
{
    BOOL finished;
    HANDLE hff;
    WIN32_FIND_DATA wfd;
    TCHAR pattern[1024];
    struct dirent de;
} DIR;

DIR * opendir(const char *dirname);
struct dirent *readdir(DIR *dir);
int closedir (DIR *dir);

DIR * opendir(const char *dirname)
{
DIR *dp = (DIR *)malloc(sizeof(DIR));
int i;
TCHAR *p;
    if (dp == NULL)
	return NULL;
    memset((char *)dp, 0, sizeof(DIR));
    convert_multibyte(dp->pattern, dirname, 
	sizeof(dp->pattern)/sizeof(TCHAR)-1);
    i = lstrlen(dp->pattern);
    for (p = dp->pattern; *p; p++)
	if (*p == '/')
	    *p = '\\';
    if (i && dp->pattern[i-1]!='\\')
	lstrcat(dp->pattern, TEXT("\\"));
    lstrcat(dp->pattern, TEXT("*"));
    dp->finished = FALSE;
    dp->hff = NULL;
    return dp;
}

int closedir (DIR *dir)
{
    if ((dir->hff) && (dir->hff != INVALID_HANDLE_VALUE))
	FindClose(dir->hff);
    free(dir);
    return 0;
}

struct dirent *readdir(DIR *dir)
{
    if (dir->finished)
	return NULL;

    if (dir->hff == NULL) {
	dir->hff = FindFirstFile(dir->pattern, &dir->wfd);
        if (dir->hff == INVALID_HANDLE_VALUE) {
	    dir->finished = TRUE;
    	    return NULL;
	}
    }
    else {
	if (!FindNextFile(dir->hff, &dir->wfd)) {
	    dir->finished = TRUE;
	    return NULL;
	}
    }
    convert_widechar(dir->de.d_name, dir->wfd.cFileName, 
	sizeof(dir->de.d_name)-1); 
    return &(dir->de);
}
#endif

/* Add the file name and description to the list
 * Return new offset.
 * If not enough space, don't copy name/description but
 * still return offset as if data was copied.
 * This allows caller to work out what buffer size is required.
 */
int
upp_add_list(char *name, char *buffer, int len, int offset)
{
char *pd;
FILE *f;
char desc[MAXSTR];
int needed, remaining;
    if ( (f = fopen(name, "r")) != (FILE *)NULL ) {
	if (fgets(desc, sizeof(desc)-1, f)) {
	    strtok(desc, "\042\n");
	    pd = strtok(NULL, "\042\n");
	    if (pd && strlen(pd)) {
	        needed = strlen(name) + strlen(pd) + 3;
	        remaining = len - offset;
		if (needed < remaining)  {
		    strcpy(buffer+offset, name);
		    offset += strlen(name) + 1;
		    strcpy(buffer+offset, pd);
		    offset += strlen(pd) + 1;
		    buffer[offset] = '\0';  /* double trailing null */
		}
		else {
		    /* don't copy data, but tell caller how much */
		    /* space it needs  */
		    offset += strlen(name) + 1;
		    offset += strlen(pd) + 1;
		}
	    }
	}
	fclose(f);
    }
    return offset;
}

/* search path for any uniprint configuration files (*.upp),
 * appending the filename and description of any found to
 * buffer at offset.  Return updated offset.
 * If offset > len, then not all data was placed in buffer.
 */
int 
enum_upp(char *path, char *buffer, int len, int offset)
{    
DIR *dirp;
struct dirent* de;
#ifdef _WIN64
struct _stat st;
#else
struct stat st;
#endif
char name[MAXSTR];
char *p;
    dirp = opendir(path);
    if (dirp == NULL)
	return 2;
    while ( (de = readdir(dirp)) != NULL) {
	if (strlen(path) + strlen(de->d_name) + 1 < MAXSTR) {
	    strcpy(name, path);
	    strcat(name, PATHSEP);
	    strcat(name, de->d_name);
	    if (stat(name, &st) != -1) {
		if  (st.st_mode & S_IFDIR) {
		    /* don't recurse into subdirectories */
		}
		else {
		    /* an ordinary file */
		    p = strrchr(name,'.');
		    if (p && (stricmp(p, ".upp") == 0)) {
			offset = upp_add_list(name, buffer, len, offset);
		    }
		}
	    }
	}
    }
    closedir(dirp);
    return offset;
}

/* return number of bytes needed */
int
enum_upp_path(char *path, char *buffer, int len)
{
char pbuf[1024];
char *p, *q, *r;
int offset = 0;
    if (buffer == (char *)NULL)
	len = 0;
    if (len >= 2) {
	buffer[0] = '\0';
	buffer[1] = '\0';
    }
    p = path;
    while (p) {
	q = strchr(p, ';');
	if (q) {
	    strncpy(pbuf, p, (int)(q-p));
	    pbuf[(int)(q-p)] = '\0';
	}
	else
	    strcpy(pbuf, p);
	if (strlen(pbuf)) {
	    r = pbuf + strlen(pbuf) - 1;
	    if ( (*r == '\\') || (*r == '/') )
		*r = '\0';	/* trailing slash will be added later */
	    offset = enum_upp(pbuf, buffer, len, offset);
	}
	if (q)
	    p = q+1;
	else
	    p = NULL;
    }
    return offset + 2;
}


char * 
uppmodel_to_name(char *buffer, char *model)
{
char *p, *desc;
    for (p=buffer; *p; p+=strlen(p)+1) {
	desc = p + strlen(p) + 1;
	if (strcmp(desc, model) == 0)
	    return p;
	p = desc;
    }
    return NULL;
}

char * 
uppname_to_model(char *buffer, char *name)
{
char *p, *desc;
    for (p=buffer; *p; p+=strlen(p)+1) {
	desc = p + strlen(p) + 1;
	if (strcmp(p, name) == 0)
	    return desc;
	p = desc;
    }
    return NULL;
}

void
psfile_epsf_print(FILE *f, int copies)
{
int llx, lly, urx, ury;
int original_llx, original_lly;
float scale = 1.0;
int rescale = FALSE;
int width, height;
int j;
CDSC *dsc = psfile.dsc;

    width = get_paper_width();
    height = get_paper_height();

    if (psfile.dsc->bbox != (CDSCBBOX *)NULL) {
	llx = psfile.dsc->bbox->llx;
	lly = psfile.dsc->bbox->lly;
	urx = psfile.dsc->bbox->urx;
	ury = psfile.dsc->bbox->ury;
    }
    else {
	llx = lly = 0;
	urx = width;
	ury = height;
    }
    original_llx = llx;
    original_lly = lly;

    /* If EPS file isn't going to fit on the page, rescale it */
    if ( (llx < 0) || (llx > width)  ||
	 (lly < 0) || (lly > height) ||
	 (urx < 0) || (urx > width)  ||
	 (ury < 0) || (lly > height) )
	rescale = TRUE;


#define MARGIN 36	/* 12.5mm gap */
    if (rescale) {
	/* leave minimum 12.5mm gap on each side */
	scale = min( ((float)(width  - 2 * MARGIN)/(urx - llx)), 
		     ((float)(height - 2 * MARGIN)/(ury - lly)) );
	urx = MARGIN + (int)((urx - llx)*scale + 0.999999);
	ury = MARGIN + (int)((ury - lly)*scale + 0.999999);
	llx = MARGIN;
	lly = MARGIN;
    }


    fprintf(f, "%%!PS-Adobe-3.0 EPSF-3.0\r\n");
    fprintf(f, "%%%%BoundingBox: %d %d %d %d\r\n", llx, lly, urx, ury);
    fprintf(f, "%%%%Pages: 1\r\n%%%%EndComments\r\n%%%%BeginProlog\r\n");
    fprintf(f, "%%%%EndProlog\r\n");
    if (copies > 1) {
	fputs("%%BeginSetup\r\n", f);
	add_copies(f, copies);
	fputs("%%EndSetup\r\n", f);
    }
    fprintf(f, "%%%%Page: 1 1\r\n");
    fprintf(f, " /EPSTOOL_save save def\r\n /showpage {} def\r\n");
    fprintf(f, " count /EPSTOOL_count exch def\r\n");
    fprintf(f, " /EPSTOOL_countdictstack countdictstack def\r\n");
    if (rescale)
        fprintf(f, " %d %d translate\r\n %g %g scale\r\n %d %d translate\r\n", 
	    MARGIN, MARGIN,		/* add new offset */
	    scale, scale, 
	    -original_llx, -original_lly /* remove old offset */ );
    fprintf(f, "%%%%BeginDocument: %s\r\n", psfile.name);
    ps_copy(f, psfile.file, dsc->begincomments, dsc->endcomments);
    ps_copy(f, psfile.file, dsc->begindefaults, dsc->enddefaults);
    ps_copy(f, psfile.file, dsc->beginprolog, dsc->endprolog);
    ps_copy(f, psfile.file, dsc->beginsetup, dsc->endsetup);
    for (j=0; j<(int)dsc->page_count; j++)
	    ps_copy(f, psfile.file, dsc->page[j].begin, dsc->page[j].end);
    ps_copy(f, psfile.file, dsc->begintrailer, dsc->endtrailer);
    fprintf(f, "\r\n%%%%EndDocument\r\n");
    fprintf(f, " count EPSTOOL_count sub {pop} repeat\r\n");
    fprintf(f, " countdictstack EPSTOOL_countdictstack sub {end} repeat\r\n");
    fprintf(f," EPSTOOL_save restore\r\n showpage\r\n%%%%Trailer\r\n%%%%EOF\r\n");
#undef MARGIN
}

BOOL
gsview_copygfile(FILE *outfile, GFile *infile)
{
    char *buffer;
    int count;
    /* create buffer for PS file copy */
    buffer = (char *)malloc(COPY_BUF_SIZE);
    if (buffer == (char *)NULL)
	return FALSE;
    while ( (count = gfile_read(infile, buffer, COPY_BUF_SIZE)) != 0 ) {
	fwrite(buffer, 1, count, outfile);
    }
    free(buffer);
    return TRUE;
}

BOOL
gsview_copyfile(FILE *outfile, FILE *infile)
{
    char *buffer;
    int count;
    /* create buffer for PS file copy */
    buffer = (char *)malloc(COPY_BUF_SIZE);
    if (buffer == (char *)NULL)
	return FALSE;
    while ( (count = fread(buffer, 1, COPY_BUF_SIZE, infile)) != 0 ) {
	fwrite(buffer, 1, count, outfile);
    }
    free(buffer);
    return TRUE;
}

BOOL
copy_for_printer(FILE *pcfile, BOOL convert)
{
    int copies = 1;
    if (!convert && (psfile.print_copies > 1))
	copies = psfile.print_copies;

    if (psfile.dsc == (CDSC *)NULL) {
	/* copy non-DSC file */
	if (copies > 1) {
	    fputs("%!\n", pcfile);
	    add_copies(pcfile, copies);
	}
	if (psfile.ispdf) {
	    /* PDF file hasn't been displayed so we don't know the page count */
	    /* Print/convert all pages */
	    char *p = psfile_name(&psfile);
	    fputc('(', pcfile);
	    for (; *p; p++)
		if (*p == '\\')
		   fputc('/', pcfile);
		else if ((*p == '(') || (*p == ')')) {
		   fputc('\\', pcfile);
		   fputc(*p, pcfile);
		}
		else
		   fputc(*p, pcfile);
	    fputs(") run\n", pcfile);
	}
	else if (!gsview_copygfile(pcfile, psfile.file)) {
	    play_sound(SOUND_ERROR);
	    fclose(pcfile);
	    return FALSE;
	}
    }
    else {
	if (psfile.ispdf) {
	    if (!pdf_extract(pcfile, copies)) {
		fclose(pcfile);
		return FALSE;
	    }
	}
	else  {
	    if (psfile.dsc->epsf && !psfile.dsc->dcs2) {
		/* Copy EPSF file, making sure it fits on the page
		 * and includes one and only one showpage
		 */
		 psfile_epsf_print(pcfile, copies);
	    }
	    else {
		/* copy DSC file */
		if ((psfile.dsc->page_count == 0) || 
		    (!convert && psfile.print_ignoredsc)) {
			CDSC *dsc = psfile.dsc;
		        ps_copy(pcfile, psfile.file, dsc->begincomments, 
			    dsc->endcomments);
			ps_copy(pcfile, psfile.file, dsc->begindefaults, 
			    dsc->enddefaults);
			ps_copy(pcfile, psfile.file, dsc->beginprolog, 
			    dsc->endprolog);
			copy_setup(pcfile, psfile.file, copies);
			ps_copy(pcfile, psfile.file, dsc->begintrailer, 
			    dsc->endtrailer);
		    }
		else
		    if (!psfile_extract(pcfile, copies))
		        return FALSE;
	    }
	}
    }
    return TRUE;
}


/* common printer/convert code */
BOOL
gsview_cprint(char *psname, char *optname, BOOL convert)
{
char buf[MAXSTR];
int i;
float print_xdpi, print_ydpi;
int width, height;
int widthpt, heightpt;
struct prop_item_s *proplist;
FILE *optfile;
FILE *pcfile;
char *p;
static char output[MAXSTR]; /* output filename for printing */
static char queue[MAXSTR];  /* output queue if not printing to file */
BOOL printtofile=FALSE;
float xoffset = 0;
float yoffset = 0;
char section[MAXSTR];
PROFILE *prf;
int prectrld=0;
int postctrld=0;
char psprolog[MAXSTR];
char psepilog[MAXSTR];
const char *device;
const char *resolution;
int method = option.print_method;
    if (convert)
	method = PRINT_CONVERT;

    /*  ASSUMES psfile.file is open */

    if (convert || 
	(((method == PRINT_GS) || (method == PRINT_PS)) && 
	 (option.print_to_file || (strcmp(option.printer_queue, "FILE:")==0)))
       ) {
	printtofile = TRUE;
	if (!get_filename(output, TRUE, FILTER_ALL, IDS_OUTPUTFILE, IDS_TOPICPRINT))
	    return FALSE;
    }
    else {
	strcpy(queue, szSpoolPrefix);
	strcat(queue, option.printer_queue);
    }

    /* create (temporary) file containing pages to print */
    psname[0] = '\0';
    if ((method == PRINT_PS) && printtofile && !psfile.ispdf) {
	/* we want to write the PostScript file directly. */
	if ((pcfile = fopen(output, "wb")) == (FILE *)NULL)
	    return FALSE;
	strcpy(psname, output);
    }
    else if ( (pcfile = gp_open_scratch_file(szScratch, psname, "wb")) 
	== (FILE *)NULL) {
	gserror(IDS_NOTEMP, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	play_sound(SOUND_ERROR);
	return FALSE;
    }

    if ((method == PRINT_PS) && !psfile.ispdf) {
	/* For PostScript printers, provide options for sending
         * Ctrl+D before and after job, and sending a prolog
	 * and epilog file.
	 * These are set using the Advanced button on the Printer
	 * Setup dialog, only enabled for PostScript printer
	 * when document is PostScript.
	 */
	strcpy(section, option.printer_queue);
	if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
	    profile_read_string(prf, section, "PreCtrlD", "0", buf, 
		sizeof(buf)-2);
	    if (sscanf(buf, "%d", &prectrld) != 1)
		prectrld = 0;
	    profile_read_string(prf, section, "PostCtrlD", "0", buf, 
		sizeof(buf)-2);
	    if (sscanf(buf, "%d", &postctrld) != 1)
		postctrld = 0;
	    profile_read_string(prf, section, "Prolog", "", psprolog, 
	       sizeof(psprolog)-2);
	    profile_read_string(prf, section, "Epilog", "", psepilog, 
	       sizeof(psepilog)-2);
	    profile_close(prf);
	}
	if (prectrld)
	   fputc('\004', pcfile);
	if (strlen(psprolog)!=0) {
	    FILE *infile = fopen(psprolog, "rb");
	    if (infile != (FILE *)NULL) {
	        if (!gsview_copyfile(pcfile, infile)) {
		    play_sound(SOUND_ERROR);
		    return FALSE;
		}
		fclose(infile);
	    }
	    else
		play_sound(SOUND_ERROR);
	}
    }

    if (!copy_for_printer(pcfile, convert)) {
	gs_addmess("Failed to copy document to temporary file for printing\n");
	unlink(psname);
	return FALSE;
    }

    if ((method == PRINT_PS) && !psfile.ispdf) {
	if (strlen(psepilog)!=0) {
	    FILE *infile = fopen(psepilog, "rb");
	    if (infile != (FILE *)NULL) {
	        if (!gsview_copyfile(pcfile, infile)) {
		    play_sound(SOUND_ERROR);
		    return FALSE;
		}
		fclose(infile);
	    }
	    else
		play_sound(SOUND_ERROR);
	}
	if (postctrld)
	   fputc('\004', pcfile);
    }

    fclose(pcfile);

    if ((method == PRINT_PS) && printtofile && !psfile.ispdf) {
	/* finished - not an error */
	/* Don't continue with Ghostscript */
	return FALSE;
    }

    if ((method == PRINT_PS) && !psfile.ispdf) {
	/* Send PostScript files direct to printer */
	/* If PDF, we must use Ghostscript to convert to PS */
	if (!gp_printfile(psname, option.printer_queue)) {
	    play_sound(SOUND_ERROR);
	    return FALSE;
	}
	unlink(psname);
	/* finished - not an error */
	/* Don't continue with Ghostscript */
	return FALSE;
    }

    if (convert) {
	device = option.convert_device;
	resolution = option.convert_resolution;
    }
    else if (method == PRINT_GDI) {
	switch (option.print_gdi_depth) {
	    case IDC_COLOUR:
		device = "bmp16m";
		break;
	    case IDC_GREY:
		device = "bmpgray";
		break;
	    default:
		device = "bmpmono";
		break;
	}
	resolution = "300";	/* fix later */
    }
    else if (method == PRINT_PS) {
	device = "pswrite";
	resolution = "600";	/* a guess */
    }
    else {
	device = option.printer_device;
	resolution = option.printer_resolution;
    }
	
    /* calculate image size */
    switch (sscanf(resolution, "%fx%f", &print_xdpi, &print_ydpi)) {
	case EOF:
	case 0:
	    print_xdpi = print_ydpi = DEFAULT_RESOLUTION;
	    break;
	case 1:
	    print_ydpi = print_xdpi;
    }
    widthpt = get_paper_width();
    heightpt = get_paper_height();
    width  = (unsigned int)(widthpt  / 72.0 * print_xdpi + 0.5);
    height = (unsigned int)(heightpt / 72.0 * print_ydpi + 0.5);

    if (method == PRINT_GDI) {
	/* Use values obtained from device context */
    	width = print_gdi_width;
    	height = print_gdi_height;
    	print_xdpi = (float)print_gdi_xdpi;
    	print_ydpi = (float)print_gdi_ydpi;
    }

    /* create options file */
#ifdef UNIX
    if ((optname[0] != '\0') && !debug)
	    unlink(optname);
#endif
    optname[0] = '\0';
    if ( (optfile = gp_open_scratch_file(szScratch, optname, "w")) == (FILE *)NULL) {
	    play_sound(SOUND_ERROR);
	    return FALSE;
    }
    if (option.gsinclude[0])
	fprintf(optfile, "-I\042%s\042\n", option.gsinclude);
    fprintf(optfile, "-dNOPAUSE\n");
    /* If using a PDF file, we set SAFER later after we have opened
     * the PDF file.
     */
    if (option.safer && (!psfile.ispdf || (option.gsversion < 704)))
	fprintf(optfile, "-dSAFER\n");
    if (strcmp(device, "uniprint") == 0) {
	/* uniprint sets the device name and resolution in */
	/* a configuration file */
	/* Since we don't know the resolution, set the page size in points */
	fprintf(optfile, "-dDEVICEWIDTHPOINTS=%u\n", widthpt);
	fprintf(optfile, "-dDEVICEHEIGHTPOINTS=%u\n", heightpt);
    }
    else {
	fprintf(optfile, "-sDEVICE=%s\n",device);
	fprintf(optfile, "-dDEVICEXRESOLUTION=%g\n", (double)print_xdpi);
        fprintf(optfile, "-dDEVICEYRESOLUTION=%g\n", (double)print_ydpi);
	fprintf(optfile, "-dDEVICEWIDTH=%u\n", width);
	fprintf(optfile, "-dDEVICEHEIGHT=%u\n", height);
    }

    if (method == PRINT_GDI) {
	fprintf(optfile, "-sOutputFile=\042%%handle%%%08lx\042\n",
		(unsigned long)print_gdi_write_handle);
    }
    else {
	fprintf(optfile, "-sOutputFile=\042");
	for (p=(printtofile) ? output : queue; *p != '\0'; p++)
	    fputc(*p,optfile);
	fputc('\042',optfile);
	fputc('\n',optfile);
    }

    if ((proplist = get_properties(device)) != (struct prop_item_s *)NULL) {
	/* output current property selections */
	for (i=0; proplist[i].name[0]; i++) {
	    if (strcmp(proplist[i].value, not_defined) != 0)
		fprintf(optfile,"-%s=%s\n", proplist[i].name, proplist[i].value);
	}
	free((char *)proplist);
    }
    p = option.gsother;
    while ((p = gs_argnext(p, buf, TRUE)) != NULL)
        fprintf(optfile, "%s\n", buf);

    /* Delay all things that call setpagedevice until the end */
    strcpy(section, (method == PRINT_GDI) ? "GDI" : device);
    strcat(section, " Options");
    if ( (prf = profile_open(szIniFile)) != (PROFILE *)NULL ) {
	/* Options */
	profile_read_string(prf, section, "Options", "", buf, sizeof(buf)-2);
	if (strlen(buf) > 0) {
	    if (buf[0] == '@') {
		/* STUPID Windows *sometimes* removes the quotes.
		 * If the profile string contains quotes at the
		 * the start *and* end, Windows will remove them.
		 * Otherwise, quotes will be copied intact.
		 * The quotes are important, so we have to put
		 * them back in.
		 */
		fprintf(optfile, "\042%s\042\n", buf);
	    }
	    else
		fprintf(optfile, "%s\n", buf);
 	}

        /* PageOffset */
	profile_read_string(prf, section, "Xoffset", "0", buf, sizeof(buf)-2);
	if (sscanf(buf, "%f", &xoffset) != 1)
	    xoffset = 0;
	profile_read_string(prf, section, "Yoffset", "0", buf, sizeof(buf)-2);
	if (sscanf(buf, "%f", &yoffset) != 1)
	    yoffset = 0;
	if ((xoffset != 0) || (yoffset != 0))
	    fprintf(optfile, "-c \042<< /PageOffset [%g %g] >> setpagedevice\042\n-f\n", 
	    (double)xoffset, (double)yoffset);

	profile_close(prf);
    }



    if (   ((method==PRINT_GS) && option.print_fixed_media)
        || ((method==PRINT_GDI) && option.print_gdi_fixed_media)
	|| ((method==PRINT_CONVERT) && option.convert_fixed_media) ) {
	/* Force page size to remain unchanged and let the
	 * GS page matching code rotate the pages as needed.
	 * Must do this after the device is opened.
	 */
	int fixed_media = option.print_fixed_media;
	if (method == PRINT_GDI)
	    fixed_media = option.print_gdi_fixed_media;
	if (method == PRINT_CONVERT)
	    fixed_media = option.convert_fixed_media;
    	fprintf(optfile, 
	    "-c \042<< /Policies << /PageSize %d >> /PageSize [%d %d] ",
	    (fixed_media == 1) ? 5 : 3, widthpt, heightpt);
	fprintf(optfile, "/InputAttributes currentpagedevice \
/InputAttributes get mark exch {1 index /Priority eq not \
 {pop << /PageSize [%d %d] >>} if } forall >> \
>> setpagedevice\042 -f\n", widthpt, heightpt);
    }

    if (strcmp(device, "pdfwrite")==0) {
	if (option.gsversion >= 650)
	    fprintf(optfile, "-c .setpdfwrite -f\n");
	else
	    fprintf(optfile, "-c save pop -f\n");
    }

    fclose(optfile);
    return TRUE;
}

