/* Copyright (C) 2000-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* dscutil.c - text messages for DSC parser and standalone test code */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(STANDALONE) && !defined(UNIX)
#include <io.h>
#endif

#include "dscparse.h"

#define MAXSTR 256

int dsc_error_fn(void *caller_data, CDSC *dsc, unsigned int explanation, 
	const char *line, unsigned int line_len);
void dsc_display(CDSC *dsc, void (*dfn)(void *ptr, const char *str));


/* We always include the English version of this text in
 * an array for debug reporting.  Language specific versions
 * are stored in resources.
 */ 

const char * const dsc_message[] = {
/* CDSC_MESSAGE_BBOX */
 "This line is incorrect.\n\
The bounding box must be in integer coordinates,\n\
not floating point. A floating point bounding box should\n\
be specified using %%HiResBoundingBox:\n\
A %%BoundingBox: should still be provided with integer values.\n\
\n\
'OK' will convert the coordinates to integers,\n\
rounding them outwards.\n\
'Cancel' will ignore this line\n",

/* CDSC_MESSAGE_EARLY_TRAILER */
"The trailer is normally at the end of a document.\n\
This line was found more than 32k bytes from the end\n\
of the file.  Trailers are not usually this long.\n\
It is more likely that another PostScript file has been included,\n\
without enclosing it in %%BeginDocument / %%EndDocument.\n\
\n\
'OK' will assume this is part of an included file.\n\
'Cancel' will assume this is the trailer\n",

/* CDSC_MESSAGE_EARLY_EOF */
"This line normally occurs at the end of a document.\n\
This line was found more than 100 bytes from the end\n\
of the file.\n\
It is more likely that another PostScript file has been included,\n\
without enclosing it in %%BeginDocument / %%EndDocument.\n\
\n\
'OK' will assume this is part of an included file.\n\
'Cancel' will assume this is part of the trailer\n",

/* CDSC_MESSAGE_PAGE_IN_TRAILER */
"This %%Page: line occurred in the trailer, which is not legal.\n\
EPS files should be encapsulated in %%BeginDocument / %%EndDocument.\n\
If is possible that an EPS file was incorrectly encapsulated,\n\
and that we have been confused by the %%Trailer in an EPS file.\n\
\n\
'OK' will process more pages.\n\
'Cancel' will ignore this page comment\n",

/* CDSC_MESSAGE_PAGE_ORDINAL */
"This line is incorrect.\n\
The two arguments should be a label and an ordinal.\n\
The ordinal should be 1 for the first page and should\n\
increase by 1 for each following page.\n\
The page ordinal is missing or is not in sequence.\n\
\n\
It is likely that this line is part of an included file that\n\
was not enclosed in %%BeginDocument / %%EndDocument.\n\
\n\
'OK' will ignore this page, assuming it is part of an\n\
incorrectly encapsulated EPS file.\n\
'Cancel' will treat this as a valid page\n",

/* CDSC_MESSAGE_PAGES_WRONG */
"%%Pages: doesn't match number of %%Page:\n\
\n\
'OK' will adjust the page count to match the number of %%Page:\n\
comments found\n\
'Cancel' will set the page count from %%Pages:\n",

/* CDSC_MESSAGE_EPS_NO_BBOX */
"This EPS file is missing the required %%BoundingBox:\n\
See the GSview help for details of how to add/adjust the\n\
bounding box.\n\
\n\
'OK' will assume this as an EPS file\n\
'Cancel' will assume this is NOT an EPS file.\n",

/* CDSC_MESSAGE_EPS_PAGES */
"EPS files may have 0 or 1 pages.  This 'EPS' file has\n\
more than this and so is not an EPS file.\n\
You cannot use `Print To` `Encapsulated PostScript File` for\n\
printing multipage files.  The correct method is to connect\n\
the printer to FILE: or to select 'Print to File'.\n\
\n\
'OK' will assume this as an EPS file\n\
'Cancel' will assume this is NOT an EPS file.\n",

/* CDSC_MESSAGE_NO_MEDIA */
"Media was specified with %%DocumentMedia:, but no default\n\
media was specified with %%PageMedia:.\n\
\n\
'OK' will set the default media to the first listed media.\n\
'Cancel' will not set a default media.\n",

/* CDSC_MESSAGE_ATEND */
"This line is incorrect\n\
To defer a value, you must use '(atend)', not 'atend'\n\
\n\
'OK' will assume that (atend) was meant.\n\
'Cancel' will ignore this line.\n",

/* CDSC_MESSAGE_DUP_COMMENT */
"This comment is duplicated in the header, which is unnecessary.\n\
The first occurence of a header line takes precedence.\n\
\n\
'OK' or 'Cancel' will ignore the duplicate line.\n",

/* CDSC_MESSAGE_DUP_TRAILER */
"This comment is duplicated in the trailer, which is unnecessary.\n\
The last occurence of a trailer line takes precedence.\n\
\n\
'OK' or 'Cancel' will use the duplicate line.\n",

/* CDSC_MESSAGE_BEGIN_END */
"The number of Begin and End comments do not match.\n\
\n\
'OK' or 'Cancel' will ignore this mismatch.\n",

/* CDSC_MESSAGE_BAD_SECTION */
"This line is incorrect because it should not occur\n\
within a page.  This probably indicates that an EPS file\n\
was incorrectly encapsulated.  The line will be ignored.\n\
\n\
'OK' or 'Cancel' will ignore this line.\n",

/* CDSC_MESSAGE_LONG_LINE */
"\n\nLines in DSC documents must be shorter than 255 characters.\n",

/* CDSC_MESSAGE_INCORRECT_USAGE */
"This DSC comment line is incorrect.\n\
Please refer to the DSC specification.\n\
'OK' or 'Cancel' will ignore this mismatch.\n",

   NULL
};

int 
dsc_error_fn(void *caller_data, CDSC *dsc, unsigned int explanation, 
	const char *line, unsigned int line_len)
{
    int response;
    int severity;
    char buf[MAXSTR];
    if (explanation > dsc->max_error)
	return CDSC_RESPONSE_OK;

    if (caller_data == NULL)	 	/* ignore unused parameter */
	dsc_debug_print(dsc, "");	/* do nothing */

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
    /* In this version, we treat DSC as being correct and return Cancel */
    response = CDSC_RESPONSE_CANCEL;

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

void 
dsc_display(CDSC *dsc, void (*dfn)(void *ptr, const char *str))
{
    unsigned int i;
    const char *p;
    char buf[MAXSTR];
    void *ptr = dsc->caller_data;
    CDCS2 *pdcs = dsc->dcs2;
    CDSCCOLOUR *colour = dsc->colours;
    char fmtbuf[MAXSTR];
    const char *offset_fmt = DSC_OFFSET_FORMAT;
    (*dfn)(ptr, "== DSC dump ==\n");
    if (dsc->dsc)
        (*dfn)(ptr, "DSC ");
    if (dsc->epsf)
        (*dfn)(ptr, "EPSF ");
    if (dsc->doseps)
        (*dfn)(ptr, "DOSEPS ");
    if (dsc->macbin)
        (*dfn)(ptr, "MacBinary ");
    if (dsc->pdf)
        (*dfn)(ptr, "PDF ");
    if (dsc->pjl)
        (*dfn)(ptr, "PJL ");
    if (dsc->ctrld)
        (*dfn)(ptr, "CTRLD ");
    if (dsc->dcs2)
        (*dfn)(ptr, "DCS2.0 ");
    (*dfn)(ptr, "\n");

    if (dsc->dsc_version) {
	(*dfn)(ptr, "version ");
	(*dfn)(ptr, dsc->dsc_version);
	(*dfn)(ptr, "\n");
    }
    if (dsc->dsc_title) {
	(*dfn)(ptr, "title ");
	(*dfn)(ptr, dsc->dsc_title);
	(*dfn)(ptr, "\n");
    }
    if (dsc->dsc_creator) {
	(*dfn)(ptr, "creator ");
	(*dfn)(ptr, dsc->dsc_creator);
	(*dfn)(ptr, "\n");
    }
    if (dsc->dsc_date) {
	(*dfn)(ptr, "date ");
	(*dfn)(ptr, dsc->dsc_date);
	(*dfn)(ptr, "\n");
    }
    if (dsc->dsc_for) {
	(*dfn)(ptr, "for ");
	(*dfn)(ptr, dsc->dsc_for);
	(*dfn)(ptr, "\n");
    }

    if (dsc->doseps) {
	sprintf(buf, "doseps: ps=%ld %ld  wmf=%ld %ld  tiff=%ld %ld\n",
	    dsc->doseps->ps_begin, dsc->doseps->ps_length,
	    dsc->doseps->wmf_begin, dsc->doseps->wmf_length,
	    dsc->doseps->tiff_begin, dsc->doseps->tiff_length);
	(*dfn)(ptr, buf);
    }
    if (dsc->macbin) {
	sprintf(buf, "macbin ps=%ld %ld  resource=%ld %ld\n",
	    dsc->macbin->data_begin, dsc->macbin->data_length,
	    dsc->macbin->resource_begin, dsc->macbin->resource_length);
	(*dfn)(ptr, buf);
    }
    if (dsc->bbox) {
	sprintf(buf, "boundingbox %d %d %d %d\n",
	    dsc->bbox->llx, dsc->bbox->lly, dsc->bbox->urx, dsc->bbox->ury);
        (*dfn)(ptr, buf);
    }
    if (dsc->hires_bbox) {
	sprintf(buf, "hiresboundingbox %f %f %f %f\n",
	    dsc->hires_bbox->fllx, dsc->hires_bbox->flly, 
	    dsc->hires_bbox->furx, dsc->hires_bbox->fury);
        (*dfn)(ptr, buf);
    }
    if (dsc->crop_box) {
	sprintf(buf, "cropbox %f %f %f %f\n",
	    dsc->crop_box->fllx, dsc->crop_box->flly, 
	    dsc->crop_box->furx, dsc->crop_box->fury);
        (*dfn)(ptr, buf);
    }
    if (dsc->language_level != 0) {
        sprintf(buf, "language level %d\n", dsc->language_level);
        (*dfn)(ptr, buf);
    }
    if (dsc->preview != CDSC_NOPREVIEW) {
	switch (dsc->preview) {
	    case CDSC_NOPREVIEW:
		    p = "None";
		    break;
	    case CDSC_EPSI:
		    p = "Interchange";
		    break;
	    case CDSC_TIFF:
		    p = "TIFF";
		    break;
	    case CDSC_WMF:
		    p = "Windows MetaFile";
		    break;
	    case CDSC_PICT:
		    p = "Mac PICT";
		    break;
	    default:
		    p = "Unknown";
	}
	sprintf(buf, "preview type %s\n", p);
	(*dfn)(ptr, buf);
    }

    if (dsc->document_data != CDSC_DATA_UNKNOWN) {
	switch (dsc->document_data) {
	    case CDSC_CLEAN7BIT:
		    p = "Clean7Bit";
		    break;
	    case CDSC_CLEAN8BIT:
		    p = "Clean8Bit";
		    break;
	    case CDSC_BINARY:
		    p = "Binary";
		    break;
	    default:
		    p = "Unknown";
	}
	sprintf(buf, "document data %s\n", p);
	(*dfn)(ptr, buf);
    }


    if (dsc->page_order != CDSC_ORDER_UNKNOWN) {
	switch (dsc->page_order) {
	    case CDSC_ASCEND:
		    p = "Ascend";
		    break;
	    case CDSC_DESCEND:
		    p = "Descend";
		    break;
	    case CDSC_SPECIAL:
		    p = "Special";
		    break;
	    default:
		    p = "Unknown";
	}
	sprintf(buf, "page order %s\n", p);
	(*dfn)(ptr, buf);
    }

    if (dsc->page_orientation != CDSC_ORIENT_UNKNOWN) {
	switch (dsc->page_orientation) {
	    case CDSC_PORTRAIT:
		    p = "Portrait";
		    break;
	    case CDSC_LANDSCAPE:
		    p = "Landscape";
		    break;
	    default:
		    p = "Unknown";
	}
	sprintf(buf, "page orientation %s\n", p);
	(*dfn)(ptr, buf);
    }
    if (dsc->page_bbox) {
	sprintf(buf, "default page boundingbox %d %d %d %d\n",
	    dsc->page_bbox->llx, dsc->page_bbox->lly, 
	    dsc->page_bbox->urx, dsc->page_bbox->ury);
        (*dfn)(ptr, buf);
    }

    for (i=0; dsc->media && (i<dsc->media_count); i++) {
	if (dsc->media[i]) {
	    sprintf(buf, "media %d   (%.50s) %g %g %g (%.50s) (%.50s)\n", i,
		dsc->media[i]->name ? dsc->media[i]->name : "", 
		dsc->media[i]->width, dsc->media[i]->height, 
		dsc->media[i]->weight, 
		dsc->media[i]->colour ? dsc->media[i]->colour : "", 
		dsc->media[i]->type ? dsc->media[i]->type : "");
	    (*dfn)(ptr, buf);
	    if (dsc->media[i]->mediabox) {
	        sprintf(buf, "  mediabox %d %d %d %d\n",
		    dsc->media[i]->mediabox->llx,
		    dsc->media[i]->mediabox->llx,
		    dsc->media[i]->mediabox->llx,
		    dsc->media[i]->mediabox->llx);
		    (*dfn)(ptr, buf);
		}
	}
    }
    if (dsc->page_media) {
	sprintf(buf, "default page media %.50s\n", dsc->page_media->name);
        (*dfn)(ptr, buf);
    }

    if (dsc->viewing_orientation) {
	sprintf(buf, "default viewing orientation %f %f %f %f\n",
	    dsc->viewing_orientation->xx,
	    dsc->viewing_orientation->xy,
	    dsc->viewing_orientation->yx,
	    dsc->viewing_orientation->yy);
        (*dfn)(ptr, buf);
    }

    sprintf(fmtbuf, "comments %%%s %%%s\n", offset_fmt, offset_fmt);
    sprintf(buf, fmtbuf, dsc->begincomments, dsc->endcomments);
    (*dfn)(ptr, buf);
    sprintf(fmtbuf, "preview %%%s %%%s\n", offset_fmt, offset_fmt);
    sprintf(buf, fmtbuf, dsc->beginpreview, dsc->endpreview);
    (*dfn)(ptr, buf);
    sprintf(fmtbuf, "defaults %%%s %%%s\n", offset_fmt, offset_fmt);
    sprintf(buf, fmtbuf, dsc->begindefaults, dsc->enddefaults);
    (*dfn)(ptr, buf);
    sprintf(fmtbuf, "prolog %%%s %%%s\n", offset_fmt, offset_fmt);
    sprintf(buf, fmtbuf, dsc->beginprolog, dsc->endprolog);
    (*dfn)(ptr, buf);
    sprintf(fmtbuf, "setup %%%s %%%s\n", offset_fmt, offset_fmt);
    sprintf(buf, fmtbuf, dsc->beginsetup, dsc->endsetup);
    (*dfn)(ptr, buf);
    sprintf(buf, "pages %d\n", dsc->page_pages);
    (*dfn)(ptr, buf);
    for (i=0; i<dsc->page_count; i++) {
        sprintf(fmtbuf, "page %%.20s %%d  %%%s %%%s\n", offset_fmt, offset_fmt);
	sprintf(buf, fmtbuf,
	    dsc->page[i].label, dsc->page[i].ordinal, 
	    dsc->page[i].begin, dsc->page[i].end);
        (*dfn)(ptr, buf);
	if (dsc->page[i].media) {
	    sprintf(buf, "  page media %s\n", dsc->page[i].media->name);
            (*dfn)(ptr, buf);
	}
	if (dsc->page[i].orientation != CDSC_ORIENT_UNKNOWN) {
	    switch (dsc->page[i].orientation) {
		case CDSC_PORTRAIT:
			p = "Portrait";
			break;
		case CDSC_LANDSCAPE:
			p = "Landscape";
			break;
		default:
			p = "Unknown";
	    }
	    sprintf(buf, "  page orientation %s\n", p);
	    (*dfn)(ptr, buf);
	}
	if (dsc->page[i].viewing_orientation) {
	    sprintf(buf, "  viewing orientation %f %f %f %f\n",
		dsc->page[i].viewing_orientation->xx,
		dsc->page[i].viewing_orientation->xy,
		dsc->page[i].viewing_orientation->yx,
		dsc->page[i].viewing_orientation->yy);
	    (*dfn)(ptr, buf);
	}
	if (dsc->page[i].crop_box) {
	    sprintf(buf, "pagecropbox %f %f %f %f\n",
		dsc->page[i].crop_box->fllx, dsc->page[i].crop_box->flly, 
		dsc->page[i].crop_box->furx, dsc->page[i].crop_box->fury);
	    (*dfn)(ptr, buf);
	}
    }
    sprintf(fmtbuf, "trailer %%%s %%%s\n", offset_fmt, offset_fmt);
    sprintf(buf, fmtbuf, dsc->begintrailer, dsc->endtrailer);
    (*dfn)(ptr, buf);
    if (pdcs) {
	if (dsc->dcs1)
	    sprintf(buf, "DCS 1.0 separations\n");
	else
	    sprintf(buf, "DCS 2.0 separations\n");
	(*dfn)(ptr, buf);
    }
    while (pdcs) {
	if (pdcs->location && (dsc_stricmp(pdcs->location, "Local") == 0))
	    sprintf(buf, " %s %s\n", pdcs->colourname, pdcs->filename);
	else {
            sprintf(fmtbuf, " %%s #%%%s %%%s\n", offset_fmt, offset_fmt);
	    sprintf(buf, fmtbuf, pdcs->colourname, pdcs->begin, pdcs->end);
	}
        (*dfn)(ptr, buf);
	pdcs = pdcs->next;
    }
    if (colour) {
	sprintf(buf, "colours\n");
	(*dfn)(ptr, buf);
    }
    while (colour) {
	const char *type;
	switch (colour->type) {
	    case CDSC_COLOUR_PROCESS:
		type = "Process";
		break;
	    case CDSC_COLOUR_CUSTOM:
		type = "Custom";
		break;
	    default:
		type = "Unknown";
	}
	sprintf(buf, " %s %s", colour->name, type);
        (*dfn)(ptr, buf);
	switch (colour->custom) {
	    case CDSC_CUSTOM_COLOUR_CMYK:
		sprintf(buf, " CMYK %g %g %g %g\n", colour->cyan, 
		    colour->magenta, colour->yellow, colour->black);
		break;
	    case CDSC_CUSTOM_COLOUR_RGB:
		sprintf(buf, " RGB %g %g %g\n", 
		    colour->red, colour->green, colour->blue);
		break;
	    default:
		sprintf(buf, "\n");
	}
        (*dfn)(ptr, buf);
	colour = colour->next;
    }
    for (i=0; dsc->media && (i<dsc->media_count); i++) {
	if (dsc->media[i]) {
	    sprintf(buf, "media %d   (%.50s) %g %g %g (%.50s) (%.50s)\n", i,
		dsc->media[i]->name ? dsc->media[i]->name : "", 
		dsc->media[i]->width, dsc->media[i]->height, 
		dsc->media[i]->weight, 
		dsc->media[i]->colour ? dsc->media[i]->colour : "", 
		dsc->media[i]->type ? dsc->media[i]->type : "");
	    (*dfn)(ptr, buf);
	    if (dsc->media[i]->mediabox) {
	        sprintf(buf, "  mediabox %d %d %d %d\n",
		    dsc->media[i]->mediabox->llx,
		    dsc->media[i]->mediabox->llx,
		    dsc->media[i]->mediabox->llx,
		    dsc->media[i]->mediabox->llx);
		    (*dfn)(ptr, buf);
		}
	}
    }
    (*dfn)(ptr, "== END DSC dump ==\n");
}


#ifdef STANDALONE
/****************************************************************/
/* TESTING CODE */
/****************************************************************/

#include <fcntl.h>


void dump_fn(void *caller_data, const char *str)
{
    fputs(str, stdout);
}

#define COPY_BUF_SIZE 4096
void 
ps_copy(FILE *outfile, FILE *infile, long begin, long end)
{
    char *buf;
    int count;
    buf = (char *)malloc(COPY_BUF_SIZE);
    if (buf == (char *)NULL)
	return;
    if (begin >= 0)
	fseek(infile, begin, SEEK_SET);
    begin = ftell(infile);
    while (begin < end) {
        count = min(end-begin, COPY_BUF_SIZE);
	if ((count = fread(buf, 1, count, infile)) > 0) {
	    fwrite(buf, 1, count, outfile);
	    begin += count;
	}
	else
	    begin = end;	/* EOF or error */
    }
    free(buf);
}

long mem_allocated = 0;
long mem_signature = 0x5aa5f00f;
FILE *mem_file = NULL;
#define MEM_FILE "c:/gsview.txt"


void *debug_memalloc(size_t size, void *closure_data)
{
    void *p = malloc(size + 3 * sizeof(long));
    long *pl = (long *)p;
    if (pl) {
	pl[0] = mem_signature;
	pl[1] = (long)closure_data;
	pl[2] = size;
	mem_allocated += size;
	p = (void *)(pl + 3);
    }

    if (mem_file == (FILE *)NULL)
	mem_file = fopen(MEM_FILE, "w");
    if (mem_file != (FILE *)NULL) {
	fprintf(mem_file, "memalloc(%d) allocated=%ld ptr=0x%lx\n", 
	    size, mem_allocated, (long)p);
	fflush(mem_file);
    }
    return p;
}

void debug_memfree(void *ptr, void *closure_data)
{
    long *pl = (long *)ptr;
    long oldsize = 0;
    int corrupted = 0;
    int closure_mismatch = 0;
    if (pl) {
	pl -= 3;
	oldsize = pl[2];
	mem_allocated -= oldsize;
	if (pl[1] != (long)closure_data)
	    closure_mismatch = 1;
	if (pl[0] != mem_signature)
	    corrupted = 1;
	else
	    free((void *)pl);
    }

    if (mem_file == (FILE *)NULL)
	mem_file = fopen(MEM_FILE, "w");
    if (mem_file != (FILE *)NULL) {
	fprintf(mem_file, "memfree(%ld) allocated=%ld ptr=0x%lx\n", 
	    oldsize, mem_allocated, (long)ptr);
	if (closure_mismatch)
	    fprintf(mem_file, "memsfree closure_data MISMATCH\n");
	if (corrupted)
	    fprintf(mem_file, "memfree HEAP IS CORRUPTED\n");
	fflush(mem_file);
    }
}

int main(int argc, char *argv[])
{
    CDSC *dsc;
    FILE *infile;
    char buf[256];
    char bigbuf[4096];
    int count;
    int code;
    unsigned int i;
    const char *filename = NULL;
	
    if (argc >= 2)
	filename = argv[1];
    if (filename == NULL)
	filename = "t.ps";
    /* file must be opened in BINARY mode */
    infile = fopen(filename, "rb");

    if (infile == NULL)
	return 1;

    /* test 1 - write single byte at a time */
    fprintf(stdout, "TEST 1 - SINGLE BYTE\n");
    fseek(infile, 0, SEEK_SET);
    dsc = dsc_init(NULL);
    dsc_set_debug_function(dsc, dump_fn);
    dsc_set_error_function(dsc, dsc_error_fn);
    while ((count = fread(buf, 1, 1, infile))!=0) {
	dsc_scan_data(dsc, buf, count);
    }
    dsc_fixup(dsc);
    dsc_display(dsc, dump_fn);
    dsc_free(dsc);

    /* test 2 - write multiple bytes at a time */
    fprintf(stdout, "TEST 2 - %d BYTES\n", sizeof(buf));
    fseek(infile, 0, SEEK_SET);
    dsc = dsc_init(NULL);
    dsc_set_debug_function(dsc, dump_fn);
    dsc_set_error_function(dsc, dsc_error_fn);
    while ((count = fread(buf, 1, sizeof(buf), infile))!=0) {
	dsc_scan_data(dsc, buf, count);
    }
    dsc_fixup(dsc);
    dsc_display(dsc, dump_fn);
    dsc_free(dsc);

    /* test 3 - write one line at a time a time */
    /* Only works if the file does not contain null characters */
    fprintf(stdout, "TEST 3 - LINE\n");
    fseek(infile, 0, SEEK_SET);
    dsc = dsc_init(NULL);
    dsc_set_debug_function(dsc, dump_fn);
    dsc_set_error_function(dsc, dsc_error_fn);
    memset(buf, 0, sizeof(buf));
    while (fgets(buf, sizeof(buf)-1, infile) != NULL) {
	code  = dsc_scan_data(dsc, buf, strlen(buf));
	if (code != CDSC_OK)
	    fprintf(stdout, "Line %d, DSC %d\n", dsc->line_count, code);
    }
    dsc_fixup(dsc);
    dsc_display(dsc, dump_fn);


    fflush(stdout);

#ifndef UNIX
    setmode(fileno(stdout), O_BINARY);
#endif
    fputs("==== COMMENTS ====\r\n", stdout);
    ps_copy(stdout, infile, dsc->begincomments, dsc->endcomments);
    fputs("==== PREVIEW ====\r\n", stdout);
    ps_copy(stdout, infile, dsc->beginpreview, dsc->endpreview);
    fputs("==== DEFAULTS ====\r\n", stdout);
    ps_copy(stdout, infile, dsc->begindefaults, dsc->enddefaults);
    fputs("==== PROLOG ====\r\n", stdout);
    ps_copy(stdout, infile, dsc->beginprolog, dsc->endprolog);
    fputs("==== SETUP ====\r\n", stdout);
    ps_copy(stdout, infile, dsc->beginsetup, dsc->endsetup);
    for (i=0; i<dsc->page_count; i++) {
	fprintf(stdout, "==== PAGE %d ====\r\n", i+1);
	ps_copy(stdout, infile, dsc->page[i].begin, dsc->page[i].end);
    }
    fputs("==== TRAILER ====\r\n", stdout);
    ps_copy(stdout, infile, dsc->begintrailer, dsc->endtrailer);
    fputs("==== EOF ====\r\n", stdout);
    dsc_free(dsc);

    /* test 4 - write multiple bytes at a time, debug memalloc */
    fprintf(stdout, "TEST 4 - %d BYTES, DEBUG MALLOC\n", sizeof(buf));
    fseek(infile, 0, SEEK_SET);
    dsc = dsc_init_with_alloc(NULL, debug_memalloc, debug_memfree, NULL);
    dsc_set_debug_function(dsc, dump_fn);
    dsc_set_error_function(dsc, dsc_error_fn);
    while ((count = fread(buf, 1, sizeof(buf), infile))!=0) {
	dsc_scan_data(dsc, buf, count);
    }
    dsc_fixup(dsc);
    dsc_display(dsc, dump_fn);
    dsc_free(dsc);
    if (mem_file)
	fclose(mem_file);

    /* test 5 - write multiple bytes at a time */
    fprintf(stdout, "TEST 5 - %d BYTES\n", sizeof(bigbuf));
    fseek(infile, 0, SEEK_SET);
    dsc = dsc_init(NULL);
    dsc_set_debug_function(dsc, dump_fn);
    dsc_set_error_function(dsc, dsc_error_fn);
    while ((count = fread(bigbuf, 1, sizeof(bigbuf), infile))!=0) {
	dsc_scan_data(dsc, bigbuf, count);
    }
    dsc_fixup(dsc);
    dsc_display(dsc, dump_fn);
    dsc_free(dsc);

    dsc = NULL;
    return 0;
}
#endif
