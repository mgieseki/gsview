/* Copyright (C) 1993-2004, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvcpdf.c */
/* Code to display PDF files */

#include "gvc.h"

/* This code will work in single thread mode, but won't check message */
/* queue */

char pdf_pages_tag[] = "%GSVIEW_PDF_PAGES: ";
char pdf_page_tag[] = "%GSVIEW_PDF_PAGE: ";
char pdf_media_tag[] = "%GSVIEW_PDF_MEDIA: ";
char pdf_crop_tag[] = "%GSVIEW_PDF_CROP: ";
char pdf_rotate_tag[] = "%GSVIEW_PDF_ROTATE: ";
char pdf_done_tag[] = "%GSVIEW_PDF_DONE: ";
char pdf_mark_tag[] = "%GSVIEW_PDF_MARK: ";
void pdf_add_link(PDFLINK link);

#define MAX_TAG_LEN 4096
char pdf_tag_line[MAX_TAG_LEN];

char* pdf_parse_mark(char *str, char **key, char **value);
int pdf_process_tag(char *line);

int
pdf_head(void)
{
int code;
char filename[MAXSTR];
char *p, *s;
    pdf_tag_line[0] = '\0';

    p = filename;
    for (s = psfile_name(&psfile); *s; s++) {
	if ((*s == '\\') || (*s == '(') || (*s == ')'))
	    *p++ = '\\';
	*p++ = *s;
    }
    *p = '\0';

    /* Define our routine for preparing to show a page. */
    /* This writes out some tags which we capture in the */
    /* callback to obtain the page size and orientation. */
    code = gs_printf("/GSview_PDFpage {\n\
(%s) print dup == flush\n\
pdfgetpage /Page exch store\n\
Page /MediaBox pget\n\
 { (%s) print == flush\n\
 }\n\
if\n\
Page /CropBox pget\n\
 { (%s) print == flush\n\
 }\n\
if\n\
Page /Rotate pget not { 0 } if\n\
   (%s) print == flush\n\
} def\n", pdf_page_tag, pdf_media_tag, pdf_crop_tag, pdf_rotate_tag);


    /* we will need to update this pdfmark code to handle */
    /* embedded dictionaries */
    if (!code)
	code = pdf_add_pdfmark();

    /* put these in userdict so we can write to them later */
    if (!code)
	code = gs_printf("/Page null def\n/Page# 0 def\n/PDFSave null def\n/DSCPageCount 0 def\n");
    /* open PDF support dictionaries */
    if (!code)
        code = gs_printf("GS_PDF_ProcSet begin\npdfdict begin\n");
    /* open PDF file */
    if (!code)
	code = gs_printf("(%s) (r) file pdfopen begin\n", filename);
    if (!code)
	code = gs_printf("/FirstPage where { pop FirstPage } { 1 } ifelse\n ");
    if (!code)
	code = gs_printf("/LastPage where { pop LastPage } { pdfpagecount } ifelse\n");
    /* flush stdout and then send PDF page marker to stdout */
    /* we capture the page numbers in the DLL callback */
    if (!code)
	code = gs_printf("flush (%s) print exch =only ( ) print =only (\n) print flush\n", pdf_pages_tag);
    /* page numbers should now be captured and CDSC object created */
    return code;
}

/* we will need to update this pdfmark code to handle */
/* embedded dictionaries */
int 
pdf_add_pdfmark(void)
{
/* pdfmark operates in user space, not in default user space */
/* Within a PDF file however, annotations use default user space */
/* Based on code provided by Valeriy Ushakov */
        return gs_printf("\
userdict /pdfmark { \
(%s) print ==only \
 counttomark 2 idiv \
 { exch dup /Rect eq \
  { ( ) print ==only ( ) print dup length 4 eq not \
   {==only} \
   {aload \
    5 -2 roll transform matrix defaultmatrix itransform \
    5 -2 roll transform matrix defaultmatrix itransform \
    5 -1 roll astore ==only \
   } \
   ifelse \
  } \
  { dup /Border eq \
   { ( ) print ==only ( ) print dup length 4 eq \
    {aload \
     5 -2 roll dtransform matrix defaultmatrix idtransform \
     5 -1 roll dup dtransform matrix defaultmatrix idtransform pop \
     5 -1 roll \
      1 1 dtransform matrix defaultmatrix idtransform pop \
      exch dup length 0 exch 1 exch 1 sub \
      { 1 index 1 index get 3 index mul 2 index 3 1 roll put } \
      for exch pop \
     5 -1 roll astore ==only \
    } \
    {aload \
     4 -2 roll dtransform matrix defaultmatrix idtransform \
     4 -1 roll dup dtransform matrix defaultmatrix idtransform pop \
     4 -1 roll astore ==only \
    } \
    ifelse \
   } \
   { dup dup /Action eq exch /A eq or \
     { dup type /nametype eq \
       { \
         ( ) print ==only ( ) print ==only \
       } \
       { ( ) print ==only ( <<) print \
         { ( ) print exch ==only ( ) print ==only } forall \
         ( >>) print \
       } \
       ifelse \
     } \
     { ( ) print ==only ( ) print ==only \
     } \
     ifelse \
   } \
   ifelse \
  } \
  ifelse \
 } repeat pop (\n) print flush \
} bind put \
\n", pdf_mark_tag);
}

/* Create a CDSC object for PDF file */
int 
pdf_makedoc(int first, int last)
{
CDSC *dsc;
char filename[MAXSTR];
char textname[MAXSTR];
char tname[MAXSTR];
    strcpy(filename, psfile.name);	/* remember filename */
    strcpy(textname, psfile.text_name);	/* remember filename */
    strcpy(tname, psfile.tname);	/* remember filename */
    psfile.text_name[0] = '\0';		/* hide filename so it doesn't get deleted */
    psfile.tname[0] = '\0';		/* hide filename so it doesn't get deleted */
    psfile_free(&psfile);
    strcpy(psfile.name, filename);
    strcpy(psfile.text_name, textname);
    strcpy(psfile.tname, tname);
    psfile.dsc = dsc_init(NULL);
    if (psfile.dsc == (CDSC *)NULL)
	return FALSE;
    dsc = psfile.dsc;
    dsc_set_debug_function(psfile.dsc, dsc_addmess);
    if (last != 0) {
	int i;
	char buf[32];
	for (i=first; i<=last; i++) {
	   sprintf(buf, "%d", i);
	   if (dsc_add_page(psfile.dsc, i, buf) == CDSC_ERROR)
		return FALSE;
	}
	psfile.page_list.select = 
		(BOOL *)malloc( dsc->page_count * sizeof(BOOL) );
    }
    psfile.ispdf = TRUE;
    return TRUE;
}

int
pdf_trailer(void)
{
    pdf_free_link();
    return gs_printf("currentdict pdfclose\nend\nend\nend\n");
}

int
pdf_page_init(int pagenum)
{
    /* Prepare to show a page */
    /* This obtains the page size and orientation */
    pdf_free_link();
    view.img->ignore_sync = TRUE;	/* ignore next SYNC callback */
    return gs_printf("%d GSview_PDFpage\n", pagenum);
}

int
pdf_page(void)
{
    /* Display a page, assuming page size and orientation already correct */
    /* We avoid using pdfshowpage becuase this would call */
    /* pdf_showpage_setpage and undo our display pagesize setup */
    return gs_printf("Page pdfshowpage_init pdfshowpage_finish\n");
}

/* Get next key/value pair */ 
/* Return pointer to just beyond value */
char*
pdf_parse_mark(char *str, char **key, char **value)
{
char *p;
int level;
    p = str;
    while (*p) {
	if (*p == '/')
	    break;
	p++;
    }
    if (*p == '\0')
	return NULL;
    if (*p == ']')
	return NULL;
    *key = p;
    while (*p) {	/* search for a space */
	if (*p == ' ') {
	    *p = '\0';
	    p++;
	    while (*p) {  /* skip over remaining spaces */
		if (*p != ' ')
		    break;
		p++;
	    }
	    break;
	}
	p++;
    }
    *value = p;
    /* the following isn't robust */
    /* strings within arrays containing ] will confuse it */
    if (*p == '[') {  /* skip over arrays */
	p++;
	level = 1;
	while (*p) {
	    if (*p == '[')
		level++;
	    if (*p == ']')
		level--;
	    p++;
	    if (level == 0)
		break;
	}
	if (*p) {
	    *p = '\0';
	    p++;
	}
    }
    else if (*p == '(') {  /* skip over strings */
	p++;
	level = 1;
	while (*p) {
	    if (*p == '\\') {
		p++;
		if (*p == '\0')
		    p--;	/* backup, \ at EOL */
	    }
	    else {
	        if (*p == '(')
		    level++;
		if (*p == ')')
		    level--;
	    }
	    p++;
	    if (level == 0)
		break;
	}
	if (*p) {
	    *p = '\0';
	    p++;
	}
    }
    else if (strncmp(p, "<<", 2) == 0) {  /* skip over dictionaries */
	p+=2;
	level = 1;
	while (*p) {
	    if (strncmp(p, "<<", 2) == 0) {
		level++;
		p++;
	    }
	    if (strncmp(p, ">>", 2) == 0) {
		level--;
		p++;
	    }
	    p++;
	    if (level == 0)
		break;
	}
	if (*p) {
	    *p = '\0';
	    p++;
	}
    }
    else {
	while (*p) {
	    if (*p == ' ') {
		*p = '\0';
		p++;
		while (*p) {  /* skip over remaining spaces */
		    if (*p != ' ')
			break;
		    p++;
		}
		break;
	    }
	    p++;
	}
    }
    return p;
}

/* Check stdout for tag giving page range, pdfmarks etc. */
int
pdf_process_tag(char *line)
{
int i, first, last;
float x0, x1, y0, y1;
float temp;
int rotate;
unsigned int len = strlen(line);
static int pdf_page_number;
static int pdf_page_first;

    if ( (len < 1) || (*line != '%') )
	return FALSE;

    if (psfile.ispdf && (len >= sizeof(pdf_pages_tag)) &&
	(strncmp(line, pdf_pages_tag, strlen(pdf_pages_tag)) == 0) ) {
	i = sscanf(line+strlen(pdf_pages_tag), "%d %d", &first, &last);
        pdf_page_first = 1;
	if (i==2) {
	    if (debug)
		gs_addmess("Found GSVIEW_PDF_PAGES tag\n");
	    if (psfile.dsc == (CDSC *)NULL) {
		pdf_page_first = first;
		if (!pdf_makedoc(first, last)) {
		    /* stop processing ASAP */
		    request_mutex();
		    pending.abort = TRUE;
		    pending.now = FALSE;
		    release_mutex();
		    post_img_message(WM_COMMAND, IDM_GSMESS);
		}
	    }
	    return TRUE;
	}
    }
    if (psfile.ispdf && (len >= sizeof(pdf_page_tag)-1) &&
	(strncmp(line, pdf_page_tag, strlen(pdf_page_tag)) == 0) ) {
        pdf_page_number = 0;
	i = sscanf(line+strlen(pdf_page_tag), "%d", &pdf_page_number);
	if (i==1) {
	    pdf_page_number -= pdf_page_first;
	    if (debug)
		gs_addmess("Found GSVIEW_PDF_PAGE tag\n");
	    return TRUE;
	}
    }
    if (psfile.ispdf && (len >= sizeof(pdf_media_tag)) &&
	(strncmp(line, pdf_media_tag, strlen(pdf_media_tag)) == 0) ) {
	i = sscanf(line+strlen(pdf_media_tag), "[%f %f %f %f]", &x0, &y0, &x1, &y1);
	if (i==4) {
	    if (debug)
		gs_addmess("Found GSVIEW_PDF_MEDIA tag\n");
	    /* PDF page size is stored in page media mediabox */
	    if (x0 > x1) {
		temp = x0;
		x0 = x1;
		x1 = temp;
	    }
	    if (y0 > y1) {
		temp = y0;
		y0 = y1;
		y1 = temp;
	    }
	    if (psfile.dsc != (CDSC *)NULL) {
		/* try to find page size in existing media */
		char buf[MAXSTR];
		CDSCBBOX bbox;
		CDSCMEDIA *m = NULL;
		bbox.llx = (int)x0;
		bbox.lly = (int)y0;
		bbox.urx = (int)(x1+0.5);
		bbox.ury = (int)(y1+0.5);
		sprintf(buf, "%d,%d,%d,%d", bbox.llx, bbox.lly, 
			bbox.urx, bbox.ury);
		for (i=0; i < (int)psfile.dsc->media_count; i++) {
		    if ( psfile.dsc->media[i]->name &&
			(strcmp(buf, psfile.dsc->media[i]->name)==0) )
			m = psfile.dsc->media[i];
		}
		if (m == NULL) {
		    /* add new media */
		    CDSCMEDIA lm;
		    lm.name = buf;
		    lm.width = x1 - x0;
		    lm.height = y1 - y0;
		    lm.weight = 80.0;
		    lm.colour = "";
		    lm.type = "";
		    lm.mediabox = &bbox;
		    dsc_add_media(psfile.dsc, &lm);
		    if (debug & DEBUG_GENERAL)
			gs_addmessf("pdf_process_tag: adding media %s\n", buf);
		}
		else {
		    /* known page size */
		    if (debug & DEBUG_GENERAL)
			gs_addmessf("pdf_process_tag: already know about media %s\n",
			     buf);
		}
		/* find page size again, and attach to this page */
		for (i=0; i < (int)psfile.dsc->media_count; i++) {
		    if ( psfile.dsc->media[i]->name &&
		      (strcmp(buf, psfile.dsc->media[i]->name)==0) &&
		      (pdf_page_number < (int)psfile.dsc->page_count) &&
		      (psfile.dsc->page[pdf_page_number].media == NULL)) {
			psfile.dsc->page[pdf_page_number].media = 
				psfile.dsc->media[i];
			break;
		    }
		}
	    }
	    return TRUE;
	}
    }
    if (psfile.ispdf && (len >= sizeof(pdf_crop_tag)) &&
	(strncmp(line, pdf_crop_tag, strlen(pdf_crop_tag)) == 0) ) {
	i = sscanf(line+strlen(pdf_crop_tag), "[%f %f %f %f]", &x0, &y0, &x1, &y1);
	if (i==4) {
	    if (debug)
		gs_addmess("Found GSVIEW_PDF_CROP tag\n");
	    /* PDF crop box size is stored in page bbox */
	    if (x0 > x1) {
		temp = x0;
		x0 = x1;
		x1 = temp;
	    }
	    if (y0 > y1) {
		temp = y0;
		y0 = y1;
		y1 = temp;
	    }
	    if ((psfile.dsc != NULL) &&
	        (pdf_page_number < (int)psfile.dsc->page_count) &&
	        (psfile.dsc->page[pdf_page_number].bbox == NULL)) {
		dsc_set_page_bbox(psfile.dsc, pdf_page_number, 
			(int)x0, (int)y0, (int)x1, (int)y1);
	    }
	    return TRUE;
	}
    }
    if (psfile.ispdf && (len >= sizeof(pdf_rotate_tag)) &&
	(strncmp(line, pdf_rotate_tag, strlen(pdf_rotate_tag)) == 0) ) {
	i = sscanf(line+strlen(pdf_rotate_tag), "%d", &rotate);
	if (i==1) {
	    int pdf_rotate;
	    if (debug)
		gs_addmess("Found GSVIEW_PDF_ROTATE tag\n");
	    while (rotate < 0)
		rotate += 360;
	    while (rotate >= 360)
		rotate -= 360;
	    switch (rotate) {
		case 90:
		    pdf_rotate = CDSC_LANDSCAPE;
		    break;
		case 180:
		    pdf_rotate = CDSC_UPSIDEDOWN;
		    break;
		case 270:
		    pdf_rotate = CDSC_SEASCAPE;
		    break;
		default:
		    pdf_rotate = CDSC_PORTRAIT;
		    break;
	    }
	    /* pdf_rotate is used if orientation is auto */
	    if ((psfile.dsc != NULL) &&
	        (pdf_page_number < (int)psfile.dsc->page_count)) {
		psfile.dsc->page[pdf_page_number].orientation = pdf_rotate;
	    }
	    return TRUE;
	}
    }
    if ((len >= sizeof(pdf_mark_tag)) &&
	(strncmp(line, pdf_mark_tag, strlen(pdf_mark_tag)) == 0) ) {
	char *p, *key, *value;
	PDFLINK link;
	BOOL possibly_a_link = FALSE;
	BOOL is_a_link = FALSE;
	int code = FALSE;
	memset(&link, 0, sizeof(link));
	link.border_width = 1;
	p = line+strlen(pdf_mark_tag);
        if (strncmp(p, "/LNK ", 5) == 0) {
	    p += 5;
	    possibly_a_link = TRUE;
	    is_a_link = TRUE;
	}
        if (strncmp(p, "/ANN ", 5) == 0) {
	    p += 5;
	    possibly_a_link = TRUE;
	}
	if (possibly_a_link) {
	    p = pdf_parse_mark(p, &key, &value);
	    while (p) {
		if (strcmp(key, "/Rect") == 0) {
		    float fllx, flly, furx, fury;
		    if (sscanf(value+1, "%f %f %f %f", 
			&fllx, &flly, &furx, &fury) == 4) {
			link.bbox.llx = (int)fllx;
			link.bbox.lly = (int)flly;
			link.bbox.urx = (int)(furx + 0.5);
			link.bbox.ury = (int)(fury + 0.5);
			code = TRUE;
		    }
		    else if (debug)
			gs_addmess("Invalid /Rect\n");
		}
		if (strcmp(key, "/Page") == 0) {
		    if (strcmp(value, "/NextPage") == 0) {
			link.page = psfile.pagenum+1;
			code = TRUE;
		    }
		    else if (strcmp(value, "/PrevPage") == 0) {
			link.page = psfile.pagenum-1;
			code = TRUE;
		    }
		    if (strcmp(value, "/FirstPage") == 0) {
			link.page = 1;
			code = TRUE;
		    }
		    else if (strcmp(value, "/LastPage") == 0) {
			if (psfile.dsc)
			    link.page = psfile.dsc->page_count;
			else
			    link.page = 0;
			code = TRUE;
		    }
		    else if (sscanf(value, "%d", &link.page) == 1) {
			code = TRUE;
		    }
		}
		if (strcmp(key, "/Border") == 0) {
		    if (sscanf(value+1, "%f %f %f", &link.border_xr, 
			&link.border_yr, &link.border_width) == 3)
			code = TRUE;
		    else if (debug)
			gs_addmess("Invalid /Border\n");
		}
		if (strcmp(key, "/Color") == 0) {
		    if (sscanf(value+1, "%f %f %f", &link.colour_red, 
			&link.colour_green, &link.colour_blue) == 3) {
			code = TRUE;
			link.colour_valid = TRUE;
		    }
		    else if (debug)
			gs_addmess("Invalid /Color\n");
		}
		if (strcmp(key, "/Subtype") == 0) {
		    if (strcmp(value, "/Link") == 0) {
			is_a_link = TRUE;
			code = TRUE;
		    }
		}
		if ((strcmp(key, "/Action") == 0) ||
		    (strcmp(key, "/A") == 0)) {
		    if (strcmp(value, "/GoTo") == 0) {
			is_a_link = TRUE;
			code = TRUE;
		    }
		    else if (strncmp(value, "<<", 2) == 0) {
			/* probably a named desination */
			int valid = TRUE;
			char *s = value;
			s += 2;
			while (*s && *s == ' ')
			    s++;
			if (strncmp(s, "/Subtype ", 9) == 0)
			    s += 9;
			else if (strncmp(s, "/S ", 3) == 0)
			    s += 3;
			else
			    valid = FALSE;
			while (*s && *s == ' ')
			    s++;
			if (valid && (strncmp(s, "/Named ", 7) == 0)) {
			    s += 7;
			    while (*s && *s == ' ')
				s++;
			    if (valid && (strncmp(s, "/N ", 3) == 0))
				s += 3;
			    else
				valid = FALSE;
			    while (*s && *s == ' ')
				s++;
			    if (valid && (*s == '/')) {
				s++;
				if (strlen(s) < sizeof(link.action)-1) {
				    strncpy(link.action, s, sizeof(link.action));
				    s = link.action;
				    while (*s && (*s != ' '))
					s++;
				    if (*s == ' ')
					*s = '\0';
				}
				is_a_link = TRUE;
				code = TRUE;
			    }
			}
			else if (strncmp(s, "/URI ", 5) == 0) {
			    s += 5;
			    while (*s && *s == ' ')
				s++;
			    if (strncmp(s, "/URI ", 5) == 0)
				s += 5;
			    else
				valid = FALSE;
			    while (*s && *s == ' ')
				s++;
			    if (valid && (*s == '(')) {
				/* URL */
				s++;
				if (strlen(s) < sizeof(link.action)-1) {
				    strncpy(link.action, s,sizeof(link.action));
				    s = link.action;
				    while (*s && (*s != ')'))
					s++;
				    if (*s == ')')
					*s = '\0';
				}
				is_a_link = TRUE;
				code = TRUE;
			    }
			}
			else
			    valid = FALSE;
		    }
		}
		p = pdf_parse_mark(p, &key, &value);
	    }
	    if (code && is_a_link)
		pdf_add_link(link);
	    return code;
	}
    }
    return FALSE;
}


/* Check stdout for tag giving page range */
int
pdf_checktag(const char *str, int len)
{
char *p;
BOOL quote_next;
BOOL inparen;
BOOL found_eol;
int code = FALSE;
unsigned int tag_len;
int count;

    while (len) {
	/* append to local copy */
	tag_len = strlen(pdf_tag_line);
	count = min(len, (int)(sizeof(pdf_tag_line) - 1 - tag_len));
	len -= count;
	if (count) {
	    memcpy(pdf_tag_line+tag_len, str, count);
	    tag_len += count;
	    pdf_tag_line[tag_len] = '\0';
        }
	if (tag_len == 0)
	    return 0;

	do {
	    if (tag_len == 0)
		return 0;
	    tag_len = strlen(pdf_tag_line);
	    /* check if line is complete */
	    quote_next = FALSE;
	    inparen = FALSE;
	    found_eol = FALSE;
	    for (p = pdf_tag_line; *p; p++) {
		if (quote_next) {
		    quote_next = FALSE;
		    continue;
		}

		if (*p == '\\')
		   quote_next = TRUE;
		else if (*p == '(') 
		   inparen = TRUE;
		else if (*p == ')') 
		   inparen = FALSE;
		else if ((*p == '\n') && !inparen) {
		    found_eol = TRUE;
		    break;
		}
	    }


	    if (found_eol) {
		if (*p)
		    *p++ = '\0';
		if (*pdf_tag_line == '%') {
		    code = pdf_process_tag(pdf_tag_line);
	 	}
		memmove(pdf_tag_line, p, 
			pdf_tag_line+tag_len - pdf_tag_line + 1);
	    }
	    tag_len = strlen(pdf_tag_line);

	    if (!found_eol && (tag_len >= sizeof(pdf_tag_line)-1)) {
		/* Very long line without an EOL.
		 * This can't be a pdf tag, so just ignore it
		 */
		pdf_tag_line[0] = '\0';
		tag_len = 0;
	    }
	} while (found_eol);
    }

    return code;
}
	

int
pdf_orientation(int page)
{
    if (psfile.dsc == (CDSC *)NULL)
	return IDM_PORTRAIT;
    if ((page < 1) || (page > (int)psfile.dsc->page_count))
	return IDM_PORTRAIT;
    switch (psfile.dsc->page[page-1].orientation) {
	case CDSC_LANDSCAPE:
	   return IDM_LANDSCAPE;
	case CDSC_UPSIDEDOWN:
	   return IDM_UPSIDEDOWN;
	case CDSC_SEASCAPE:
	   return IDM_SEASCAPE;
	
    }
    return IDM_PORTRAIT;
}

#ifndef VIEWONLY
/* Create DSC file to print selected pages of PDF file */
int
pdf_extract(FILE *f, int copies)
{
char filename[MAXSTR];
char *p, *s;
int i, page, pages;
CDSC *dsc = psfile.dsc;
BOOL reverse = psfile.page_list.reverse;
    p = filename;
    for (s = psfile_name(&psfile); *s; s++) {
	if ((*s == '\\') || (*s == '(') || (*s == ')'))
	    *p++ = '\\';
	*p++ = *s;
    }
    *p = '\0';

    if (dsc->page_count == 0) {
	/* unknown number of pages */
	TCHAR buf[MAXSTR];
	load_string(IDS_PDFEXTRACTALL, buf, sizeof(buf));
	if (message_box(buf, MB_ICONASTERISK | MB_OKCANCEL) == IDCANCEL)
	    return FALSE;
	fputs("%!\n(", f);
	fputs(filename, f);
	fputs(") run\n", f);
	return TRUE;
    }

    /* count pages */
    pages = 0;
    for (i=0; i< (int)dsc->page_count; i++) {
	if (psfile.page_list.select[i]) pages++;
    }

    /* Send header */
    fputs("%!PS-Adobe 3.0\r\n%%Creator: GSview\r\n", f);
    fputs("%%Title: Ghostscript wrapper for ", f);
    fputs(psfile_name(&psfile), f);
    fputs("\r\n%%Pages: ", f);
    fprintf(f, "%d\r\n", pages);
    fprintf(f, "%%%%PageOrder: %s\r\n", reverse ? "Descend" : "Ascend");
    fputs("%%EndComments\r\n", f);
    fputs("%%BeginProlog\r\n", f);
    fputs("\
/Page null def\r\n\
/Page# 0 def\r\n\
/PDFSave null def\r\n\
/DSCPageCount 0 def\r\n\
/DoPDFPage {\r\n\
  (Page ) print dup == flush\r\n\
  dup /Page# exch store\r\n\
  pdfgetpage pdfshowpage\r\n\
} def\r\n\
GS_PDF_ProcSet begin\r\n\
pdfdict begin\r\n\
", f);
    fputs("%%EndProlog\r\n", f);
    fputs("%%BeginSetup\r\n", f);
    if (copies > 1)
	add_copies(f, copies);
    fprintf(f, "(%s) (r) file pdfopen begin\r\n", filename);
    if (option.safer)
	fprintf(f, "systemdict /.setsafe known { .setsafe } if\n");
    fputs("%%EndSetup\r\n", f);

    /* Send each page */
    page = 1;
    i = reverse ? dsc->page_count - 1 : 0;
    while ( reverse ? (i >= 0)  : (i < (int)dsc->page_count) ) {
	if (psfile.page_list.select[i])  {
	    fprintf(f, "%%%%Page: %s %d\r\n", dsc->page[i].label, page);
	    fprintf(f, "%d DoPDFPage\r\n", i+1);
	    page++;
	}
        i += reverse ? -1 : 1;
    }

    /* Send trailer */
    fputs("%%Trailer\r\n", f);
    fputs("currentdict pdfclose\r\nend\r\nend\r\nend\r\n%%EOF\r\n", f);
    return TRUE;
}
#endif

PDFLINK *pdf_link_head;

void
pdf_free_link(void)
{
PDFLINK *next, *thislink;
    request_mutex();
    thislink = pdf_link_head;
    while (thislink != (PDFLINK *)NULL) {
        next = thislink->next;
	free(thislink);
	thislink = next;
    }
    pdf_link_head = (PDFLINK *)NULL;
    release_mutex();
}

void
pdf_add_link(PDFLINK newlink)
{
PDFLINK *thislink;
int temp;
    if (newlink.bbox.llx > newlink.bbox.urx) {
	temp = newlink.bbox.llx;
	newlink.bbox.llx = newlink.bbox.urx;
	newlink.bbox.urx = temp;
    }
    if (newlink.bbox.lly > newlink.bbox.ury) {
	temp = newlink.bbox.lly;
	newlink.bbox.lly = newlink.bbox.ury;
	newlink.bbox.ury = temp;
    }
	
    if (debug) {
	char buf[MAXSTR];
	sprintf(buf, "Adding link /Page %d /Rect [%d %d %d %d] /Border [%g %g %g]",
	    newlink.page,
	    newlink.bbox.llx, newlink.bbox.lly,
	    newlink.bbox.urx, newlink.bbox.ury,
	    newlink.border_xr, newlink.border_yr, newlink.border_width);
	gs_addmess(buf);
	if (newlink.colour_valid) {
	    sprintf(buf, " /Color [%g %g %g]",
	      newlink.colour_red, newlink.colour_green, newlink.colour_blue);
	    gs_addmess(buf);
	}
        gs_addmess("\n");
    }

    request_mutex();
    thislink = (PDFLINK *)malloc(sizeof(PDFLINK));
    if (thislink) {
	*thislink = newlink;
	thislink->next = pdf_link_head;
	pdf_link_head = thislink;
    }
    release_mutex();
}

BOOL
pdf_get_link(int index, PDFLINK *link)
{
int i;
PDFLINK *thislink = pdf_link_head;
int code = FALSE;
    request_mutex();
    for (i=0; (thislink!=NULL) && i < index; i++)
	thislink = thislink->next;
    if (thislink) {
	*link = *thislink;
	code = TRUE;
    }
    release_mutex();
    return code;
}

BOOL
is_link(float x, float y, PDFLINK *link)
{
int i = 0;
    while ( pdf_get_link(i, link) ) {
	i++;
	if (   (link->bbox.llx < x) && (link->bbox.urx > x)
	    && (link->bbox.lly < y) && (link->bbox.ury > y) ) {
	    /* found link */
	    return TRUE;
	}
    }
    return FALSE;
}

