/* Copyright (C) 1993-2003, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvceps.c */
/* EPS file manipulation module of PM and Windows GSview */

#ifdef EPSTOOL
#include "epstool.h"
#else	/* GSview */
#include "gvc.h"
#endif

PSBBOX bbox;
DWORD get_dword(unsigned char *buf);
WORD get_word(unsigned char *buf);

#define tiff_long(val, f) write_dword(val, f)
#define tiff_short(val, f) write_word_as_dword(val, f)
#define tiff_word(val, f) write_word(val, f)

void write_dword(DWORD val, FILE *f);
void write_word_as_dword(WORD val, FILE *f);
void write_word(WORD val, FILE *f);

void write_doseps_header(struct eps_header_s *peps_header, FILE *outfile);
void copy_bbox_header(FILE *f);
void copy_trailer_without_bbox(FILE *f);
int scan_dib(PREBMAP *ppbmap, unsigned char *pbitmap);
void shift_preview(unsigned char *preview, int bwidth, int offset);
void scan_bbox(PREBMAP *pprebmap, PSBBOX *psbbox);
void get_dib_line(BYTE *line, unsigned char *preview, int width, int bitcount);
BYTE * get_dib_bits(unsigned char *pbitmap);
void make_bmp_info(LPBITMAP2 newpbm, PREBMAP *ppbmap, unsigned char *pbitmap);
int packbits(BYTE *comp, BYTE *raw, int length);
int write_tiff(FILE *f, unsigned char *pbitmap, BOOL tiff4, BOOL use_packbits, 
    BOOL calc_bbox);
int write_interchange(FILE *f, unsigned char *pbitmap, BOOL calc_bbox);


void 
ps_copy(FILE *outfile, GFile *infile, long begin, long end)
{
    char *buf;
    int count;
    buf = (char *)malloc(COPY_BUF_SIZE);
    if (buf == (char *)NULL)
	return;
    if (begin >= 0)
	gfile_seek(infile, begin, gfile_begin);
    begin = gfile_get_position(infile);
    while (begin < end) {
        count = min(end-begin, COPY_BUF_SIZE);
	if ((count = gfile_read(infile, buf, count)) > 0) {
	    fwrite(buf, 1, count, outfile);
	    begin += count;
	}
	else
	    begin = end;	/* EOF or error */
    }
    free(buf);
}

/* Like fgets, but allows any combination of EOL characters
 * and returns the count of bytes, not the string pointer
 */
int ps_fgets(char *s, int n, GFile *stream)
{
    char ch;
    int not_eof = 0;
    char *p = s;
    int count = 0;	/* bytes written to buffer */
    /* copy until first EOL character */
    while ( (count < n) &&  ((not_eof = gfile_read(stream, &ch, 1)) != 0)
	&& (ch != '\r') && (ch != '\n') ) {
	*p++ = (char)ch;
	count++;
    }

    if ((count < n) && not_eof) {
        /* check for extra EOL characters */
	if (ch == '\r') {
	    *p++ = (char)ch;
	    count++;
	    /* check if MS-DOS \r\n is being used */
	    if ((count < n) && ((not_eof = gfile_read(stream, &ch, 1)) != 0)) {
		if (ch == '\n') {
		    /* Yes, MS-DOS */
		    *p++ = (char)ch;
		    count++;
		}
		else {
		    /* No, Macintosh */
		    gfile_seek(stream, -1, gfile_current);
		}
	    }
	}
	else {
	    /* must have been '\n' */
	    *p++ = (char)ch;
	    count++;
	}
    }
    if (count < n)
	*p = '\0';

    if ( (!not_eof) && (p == s) )
	return 0;
    return count;
}


/* Copy DSC until a particular comment is found */
/* Do not copy the comment */
/* Stop if file offset exceeds end */
/* Assume that end does not occur in the middle of a line */
/* return TRUE if comment found, FALSE if not found */
BOOL ps_copy_find(FILE *outfile, GFile *infile, long end, 
	char *s, int n, const char *comment)
{
    int count;
    while ((gfile_get_position(infile) < end) && 
	((count = ps_fgets(s, n-1, infile))!=0)) {
	s[n-1] = '\0';
	if (strncmp(s, comment, strlen(comment)) == 0)
	    return TRUE;
	fwrite(s, 1, count, outfile);
    }
    return FALSE;
}


#ifndef EPSTOOL
/* At present only allows bounding box to be specified */
void
ps_to_eps(void)
{
char output[MAXSTR];
FILE *f;
char *buffer;
UINT count;
FILE *infile;
time_t t;
char *now;
char text[DSC_LINE_LENGTH];
long here;
CDSC *dsc = psfile.dsc;

	if (!pstoeps_warn()) {
	    nHelpTopic = IDS_TOPICPSTOEPS;
	    get_help();
	    return;
	}

	if ((gsdll.state != GS_PAGE) && (gsdll.state != GS_IDLE)) {
	    gserror(IDS_EPSNOBBOX, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	    return;
	}

	if ((dsc != (CDSC *)NULL) && (dsc->page_count > 1)) {
	    gserror(IDS_EPSONEPAGE, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	    return;
	}
	if (psfile.ispdf) {
	    gserror(IDS_EPSONEPAGE, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	    return;
	}
	if (dsc == (CDSC *)NULL) {
	    TCHAR mess[MAXSTR];
	    load_string(IDS_EPSQPAGES, mess, sizeof(mess));
	    if (message_box(mess, MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;
	}

	if (option.auto_bbox) {
	    unsigned char *pbitmap;
	    PREBMAP prebmap;
	    PSBBOX devbbox;	/* in pixel units */
	    if (option.orientation != IDM_PORTRAIT) {
		gserror(IDS_MUSTUSEPORTRAIT, 0, MB_ICONEXCLAMATION, 0); 
		return;
	    }
	    image_lock(view.img);
	    if ( (pbitmap = (unsigned char *)get_bitmap()) 
		== (unsigned char *)NULL) {
		    image_unlock(view.img);
		play_sound(SOUND_ERROR);
		return;
	    }
	    if (scan_dib(&prebmap, pbitmap))
	 	return;
	    devbbox.valid = FALSE;
	    bbox.valid = FALSE;
	    scan_bbox(&prebmap, &devbbox);
	    release_bitmap();
	    image_unlock(view.img);

	    if (devbbox.valid) {
		bbox.llx = (int)(devbbox.llx / option.xdpi * 72 - 0.5);
		bbox.lly = (int)(devbbox.lly / option.ydpi * 72 - 0.5);
		bbox.urx = (int)(devbbox.urx / option.xdpi * 72 + 1.5);
		bbox.ury = (int)(devbbox.ury / option.ydpi * 72 + 1.5);
		if (display.epsf_clipped) {
		    /* correct for clipped page */
		    int xoffset = 0;
		    int yoffset = 0;
		    if (dsc->bbox != (CDSCBBOX *)NULL) {
			xoffset = dsc->bbox->llx;
			yoffset = dsc->bbox->lly;
		    }
		    bbox.llx += xoffset;
		    bbox.lly += yoffset;
		    bbox.urx += xoffset;
		    bbox.ury += yoffset;
		}
		bbox.valid = TRUE;
	    }
	    if (!bbox.valid) {
		play_sound(SOUND_ERROR);
		return;
	    }
	}
	else {
	    if (!get_bbox()) {
		play_sound(SOUND_ERROR);
		return;
	    }
	}

	output[0] = '\0';
	if (!get_filename(output, TRUE, FILTER_EPS, 0, IDS_TOPICPSTOEPS))
	    return;

	if ((f = fopen(output, "wb")) == (FILE *)NULL) {
	    play_sound(SOUND_ERROR);
	    return;
	}

	if (dsc == (CDSC *)NULL) {
 	    char appname[MAXSTR];
	    convert_widechar(appname, szAppName, sizeof(appname)-1);
	    info_wait(IDS_WAITWRITE);
	    fputs("%!PS-Adobe-3.0 EPSF-3.0\r\n",f);
	    /* if this is not a single page document then gsview has just lied */
	    fprintf(f, "%%%%BoundingBox: %u %u %u %u\r\n",
		bbox.llx,bbox.lly,bbox.urx,bbox.ury);
	    fprintf(f,"%%%%Title: %s\r\n",psfile.name);
	    fprintf(f,"%%%%Creator: %s from %s\r\n",appname,psfile.name);
	    t = time(NULL);
	    now = ctime(&t);
	    now[strlen(now)-1] = '\0';	/* remove trailing \n */
	    fprintf(f,"%%%%CreationDate: %s\r\n",now);
	    fputs("%%Pages: 1\r\n",f);
	    fputs("%%EndComments\r\n",f);

	    fputs("%%Page: 1 1\r\n",f);
	    fprintf(f,"%%%%BeginDocument: %s\r\n",psfile.name);

	    /* create buffer for PS file copy */
	    buffer = (char *)malloc(COPY_BUF_SIZE);
	    if (buffer == (char *)NULL) {
	        play_sound(SOUND_ERROR);
	        fclose(f);
		unlink(output);
	        return;
	    }

	    infile = fopen(psfile_name(&psfile), "rb");
	    if (infile == (FILE *)NULL) {
	        play_sound(SOUND_ERROR);
	        fclose(f);
		unlink(output);
	        return;
	    }

            while ( (count = fread(buffer, 1, COPY_BUF_SIZE, infile)) != 0 ) {
	        fwrite(buffer, 1, count, f);
	    }
	    free(buffer);
	    fclose(infile);

	    fputs("\r\n%%EndDocument\r\n",f);
	    fputs("%%Trailer\r\n",f);
	    fclose(f);
	    info_wait(IDS_NOWAIT);
	}
	else {
	    /* document already has DSC comments */
	    char eol[3];
	    char *p;
	    info_wait(IDS_WAITWRITE);
	    gfile_seek(psfile.file, dsc->begincomments, gfile_begin);
	    ps_fgets(text, sizeof(text), psfile.file);
 	    p = text;
	    while (*p && (*p != '\r') && (*p != '\n'))
		p++;	/* find EOL used in DSC file */
	    memset(eol, 0, sizeof(eol));
	    strncpy(eol, p, sizeof(eol)-1);
	    if (dsc->epsf)
	        fputs(text,f);
	    else
	        fprintf(f, "%%!PS-Adobe-3.0 EPSF-3.0%s", eol);
	    if (dsc->bbox != (CDSCBBOX *)NULL) {
		ps_copy_find(f, psfile.file, dsc->endcomments,
		    text, sizeof(text), "%%BoundingBox:");
	    }
	    fprintf(f, "%%%%BoundingBox: %d %d %d %d%s",
		bbox.llx, bbox.lly, bbox.urx, bbox.ury, eol);
	    here = gfile_get_position(psfile.file);
	    ps_copy(f, psfile.file, here, dsc->endcomments);
	    ps_copy(f, psfile.file, dsc->begindefaults, dsc->enddefaults);
	    ps_copy(f, psfile.file, dsc->beginprolog, dsc->endprolog);
	    ps_copy(f, psfile.file, dsc->beginsetup, dsc->endsetup);
	    if (dsc->page_count)
	        ps_copy(f, psfile.file, dsc->page[0].begin, dsc->page[0].end);
	    copy_trailer_without_bbox(f);
	    fclose(f);
	    info_wait(IDS_NOWAIT);
	}
}
#endif


typedef struct tagWINRECT {
	WORD	left;
	WORD	top;
	WORD	right;
	WORD	bottom;
} WINRECT;

typedef struct {
    DWORD	key;
    WORD 	hmf;
    WINRECT 	bbox;
    WORD	inch;
    DWORD	reserved;
    WORD	checksum;
} METAFILEHEADER;

void
write_doseps_header(struct eps_header_s *peps_header, FILE *outfile)
{
    fputc(peps_header->id[0], outfile);
    fputc(peps_header->id[1], outfile);
    fputc(peps_header->id[2], outfile);
    fputc(peps_header->id[3], outfile);
    write_dword(peps_header->ps_begin, outfile);
    write_dword(peps_header->ps_length, outfile);
    write_dword(peps_header->mf_begin, outfile);
    write_dword(peps_header->mf_length, outfile);
    write_dword(peps_header->tiff_begin, outfile);
    write_dword(peps_header->tiff_length, outfile);
    write_word(peps_header->checksum, outfile);
}

/* extract EPS or TIFF or WMF file from DOS EPS file */
void 
extract_doseps(int command)
{
unsigned long pos;
unsigned long len;
unsigned int count;
char *buffer;
FILE* epsfile;
BOOL is_meta = TRUE;
char outname[MAXSTR];
FILE *outfile;
DWORD key;
#ifndef EPSTOOL
int filter;
#endif
CDSC *dsc = psfile.dsc;
	if ((dsc == (CDSC *)NULL) || (dsc->doseps == (CDSCDOSEPS *)NULL)) {
	    gserror(IDS_NOPREVIEW, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	    return;
	}
	epsfile = fopen(psfile_name(&psfile),"rb");
	pos = dsc->doseps->ps_begin;
	len = dsc->doseps->ps_length;
	if (command == IDM_EXTRACTPRE) {
	    pos = dsc->doseps->wmf_begin;
	    len = dsc->doseps->wmf_length;
	    if (pos == 0L) {
	        pos = dsc->doseps->tiff_begin;
	        len = dsc->doseps->tiff_length;
	        is_meta = FALSE;
	    }
	}
	if (pos == 0L) {
	    fclose(epsfile);
	    gserror(IDS_NOPREVIEW, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	    return;
	}
	fseek(epsfile, pos, SEEK_SET);	/* seek to section to extract */


#ifdef EPSTOOL
	/* assume outname already exists */
	strncpy(outname, oname, MAXSTR-1);
	if (*outname!='\0')
	    outfile = fopen(outname,"wb");
	else
	    outfile = stdout;
#else
	/* create postscript or preview file */
	outname[0] = '\0';
	if (command == IDM_EXTRACTPRE) {
	    if (is_meta)
	        filter = FILTER_WMF;
	    else
	        filter = FILTER_TIFF;
	}
	else
	    filter = FILTER_PS;
	if (!get_filename(outname, TRUE, filter, 0, IDS_TOPICPREVIEW)) {
	    fclose(epsfile);
	    return;
	}
	outfile = fopen(outname, "wb");
#endif
	if (outfile == (FILE *)NULL) {
	    play_sound(SOUND_ERROR);
	    fclose(epsfile);
	    return;
	}
	
	/* create buffer for file copy */
	buffer = (char *)malloc(COPY_BUF_SIZE);
	if (buffer == (char *)NULL) {
	    play_sound(SOUND_ERROR);
	    fclose(epsfile);
	    if (*outname!='\0')
	        fclose(outfile);
	    return;
	}

	if ((command == IDM_EXTRACTPRE) && is_meta) {
	    /* check if metafile already contains header */
	    char keybuf[4];
	    DWORD doseps_key = 0x9ac6cdd7UL;
	    fread(keybuf, 1, 4, epsfile);
	    key = keybuf[0] + (keybuf[1]<<8) + 
	          (keybuf[2]<<16) + (keybuf[3]<<24);
	    fseek(epsfile, pos, SEEK_SET);	/* seek to section to extract */
	    if ( key != doseps_key ) {
	        /* write placeable Windows Metafile header */
	        METAFILEHEADER mfh;
	        int i, temp;
	        unsigned short *pw;
	        mfh.key = doseps_key;
	        mfh.hmf = 0;
		/* guess the location - this might be wrong */
	        mfh.bbox.left = 0;
	        mfh.bbox.top = 0;
		if (dsc->bbox != (CDSCBBOX *)NULL) {
		    temp = (dsc->bbox->urx - dsc->bbox->llx);
		    /* double transfer to avoid GCC Solaris bug */
		    mfh.bbox.right = (WORD)temp;	
		    mfh.bbox.bottom = (WORD)(dsc->bbox->ury - dsc->bbox->lly);
		    temp = (dsc->bbox->ury - dsc->bbox->lly);
		    mfh.bbox.bottom = (WORD)temp;
		}
	  	else {
		    /* bbox missing, assume A4 */
		    mfh.bbox.right = 595;
		    mfh.bbox.bottom = 842;
		}
	        mfh.inch = 72;	/* PostScript points */
	        mfh.reserved = 0L;
	        mfh.checksum =  0;
	        pw = (WORD *)&mfh;
		temp = 0;
	        for (i=0; i<10; i++) {
	    	    temp ^= *pw++;
	        }
		mfh.checksum = (WORD)temp;
	        write_dword(mfh.key, outfile);
	        write_word(mfh.hmf, outfile);
	        write_word(mfh.bbox.left,   outfile);
	        write_word(mfh.bbox.top,    outfile);
	        write_word(mfh.bbox.right,  outfile);
	        write_word(mfh.bbox.bottom, outfile);
	        write_word(mfh.inch, outfile);
	        write_dword(mfh.reserved, outfile);
	        write_word(mfh.checksum, outfile);
	    }
	}

        while ( (count = (unsigned int)min(len,COPY_BUF_SIZE)) != 0 ) {
	    count = fread(buffer, 1, count, epsfile);
	    fwrite(buffer, 1, count, outfile);
	    if (count == 0)
		len = 0;
	    else
		len -= count;
	}
	free(buffer);
	fclose(epsfile);
	if (*outname!='\0')
	    fclose(outfile);
}


/* These routines deal with a PBM bitmap under Unix */
/* and a BMP under OS/2 or MS-DOS */

void shift_preview(unsigned char *preview, int bwidth, int offset);
BOOL iswhitespace(char c);
void reverse_triples(unsigned char *line2, int n);


char isblack[256];	/* each byte is non-zero if that colour is black */

BOOL
iswhitespace(char c)
{
	return (c==' ' || c=='\t' || c=='\r' || c=='\n');
}

/* reverse RGB triples to BGR triples, or vice versa */
void
reverse_triples(unsigned char *line2, int n)
{
unsigned char temp;
int i;
    for (i=0; i<n; i++) {
        temp = line2[0];
        line2[0] = line2[2];
	line2[2] = temp;
	line2 += 3;
    }
}

/* this doesn't do a good job of scanning pbm format */
/* instead it makes assumptions about the way Ghostscript writes pbm files */
int
scan_pbmplus(PREBMAP *ppbmap, unsigned char *pbitmap)
{
unsigned char *p;
int i;
    if (pbitmap[0] == 'P' && (pbitmap[1] == '4' || pbitmap[1] == '5' || pbitmap[1] == '6')) {
	/* pbmraw */
	p = pbitmap+3;
	while (*p!='\n')
	    p++;	/* skip comment line */
	p++;
	ppbmap->width = atoi((const char *)p);
	while (isdigit((int)(*p)))
	    p++;
	while (iswhitespace(*p))
	    p++;
	ppbmap->height = atoi((const char *)p);
	while (isdigit((int)(*p)))
	    p++;
	if (pbitmap[1] == '4') {	/* pbmraw */
	    ppbmap->depth = 1;
	    isblack[0] = 0;
	    isblack[1] = 1;
	}
	else if (pbitmap[1] == '5') {	/* pgmraw */
	    ppbmap->depth = 8;
	    while (iswhitespace(*p))
	        p++;
	    /* ignore max value */
	    while (isdigit((int)(*p)))
	        p++;
	    for (i=0; i < 256; i++)
		isblack[i] = 1;
	    isblack[255] = 0;
	}
	else {				/* ppmraw */
	    ppbmap->depth = 24;
	    while (iswhitespace(*p))
	        p++;
	    /* ignore max value */
	    while (isdigit((int)(*p)))
	        p++;
	}
	ppbmap->bits = ((BYTE *)p) +1;
        ppbmap->bytewidth = (( ppbmap->width * ppbmap->depth + 7) & ~7) >> 3;
	ppbmap->topleft = TRUE;
    }
    else {
	gserror(0, "Unknown bitmap format", MB_ICONEXCLAMATION, SOUND_ERROR);
	return 1;
    }
    return 0;
}

void scan_colors(PREBMAP *ppbmap, unsigned char *pbitmap);

int
scan_dib(PREBMAP *ppbmap, unsigned char *pbitmap)
{
    int size =  get_dword(pbitmap);
    if (size == BITMAP1_LENGTH) {
	ppbmap->width = (short)get_word(pbitmap+BITMAP1_WIDTH);
	ppbmap->height = (short)get_dword(pbitmap+BITMAP1_HEIGHT);
	ppbmap->depth = get_word(pbitmap+BITMAP1_BITCOUNT);
	ppbmap->bits =  (((BYTE *)pbitmap) + size)
			+ dib_pal_colors(pbitmap) * RGB3_LENGTH; 
	ppbmap->os2 = TRUE;
    }
    else {
	ppbmap->width = (LONG)get_dword(pbitmap+BITMAP2_WIDTH);
	ppbmap->height = (LONG)get_dword(pbitmap+BITMAP2_HEIGHT);
	ppbmap->depth = get_word(pbitmap+BITMAP2_BITCOUNT);
	ppbmap->bits =  (((BYTE *)pbitmap) + size)
			+ dib_pal_colors(pbitmap) * RGB4_LENGTH; 
	ppbmap->os2 = FALSE;
    }

    ppbmap->bytewidth = (( ppbmap->width * ppbmap->depth + 31) & ~31) >> 3;
    ppbmap->topleft = FALSE;

    scan_colors(ppbmap, pbitmap);
    return 0;
}

/* return number of bytes per line, rounded up to multiple of 4 bytes */
unsigned long
dib_bytewidth(unsigned char *pbitmap)
{
unsigned int width, bitcount;
    if (get_dword(pbitmap) == BITMAP1_LENGTH) {
	width = get_word(pbitmap + BITMAP1_WIDTH);
	bitcount = get_word(pbitmap + BITMAP1_BITCOUNT);
    }
    else {
	width = get_dword(pbitmap + BITMAP2_WIDTH);
	bitcount = get_word(pbitmap + BITMAP2_BITCOUNT);
    }
    return (((width * bitcount + 31) & ~31) >> 3);
}

/* return number of colors in color table */
unsigned int
dib_pal_colors(unsigned char *pbitmap)
{
unsigned int bitcount, planes, clrused;
    if (get_dword(pbitmap) == BITMAP1_LENGTH) {
	planes = get_word(pbitmap + BITMAP1_PLANES);
	bitcount = get_word(pbitmap + BITMAP1_BITCOUNT);
	if (bitcount != 24)
	    return 1<<(bitcount * planes);
    }
    else {
	planes = get_word(pbitmap + BITMAP2_PLANES);
	bitcount = get_word(pbitmap + BITMAP2_BITCOUNT);
	clrused = get_word(pbitmap + BITMAP2_CLRUSED);
	if (bitcount != 24)
	    return clrused ? clrused : 1<<(bitcount * planes);
    }
    if (bitcount != 24)
	    return 1<<(bitcount * planes);
    return 0;
}


#ifdef __BORLANDC__
#pragma argsused
#endif
void
scan_colors(PREBMAP *ppbmap, unsigned char *pbitmap)
{
	unsigned char *prgb;
	unsigned char rr;
	unsigned char gg;
	unsigned char bb;
	int clrtablesize;
	int i;
	unsigned int size = get_dword(pbitmap);

	prgb   = pbitmap + size;
	/* read in the color table */
	clrtablesize = dib_pal_colors(pbitmap);
	for (i = 0; i < clrtablesize; i++) {
		if (ppbmap->os2) {
		    bb = prgb[i*RGB3_LENGTH+RGB3_BLUE];
		    gg = prgb[i*RGB3_LENGTH+RGB3_GREEN];
		    rr = prgb[i*RGB3_LENGTH+RGB3_RED];
		}
		else {
		    bb = prgb[i*RGB4_LENGTH+RGB4_BLUE];
		    gg = prgb[i*RGB4_LENGTH+RGB4_GREEN];
		    rr = prgb[i*RGB4_LENGTH+RGB4_RED];
		}
		isblack[i] = (unsigned char)((rr < 0xff) || (gg < 0xff) || (bb < 0xff));
	}
}

/* return pointer to bitmap bits */
BYTE *
get_dib_bits(unsigned char *pbitmap)
{
BYTE *lpDibBits;
	unsigned int size = get_dword(pbitmap);
	lpDibBits = (((BYTE *)pbitmap) + size);
	if (size == BITMAP1_LENGTH)
	    lpDibBits += dib_pal_colors(pbitmap) * RGB3_LENGTH; 
	else
	    lpDibBits += dib_pal_colors(pbitmap) * RGB4_LENGTH; 
	return lpDibBits;
}

/* get line from DIB and store as 1 bit/pixel in preview */
/* also works for PBM (pnmraw) bitmap */
/* preview has 0=black, 1=white */
void
get_dib_line(BYTE *line, unsigned char *preview, int width, int bitcount)
{
int bwidth = ((width + 7) & ~7) >> 3; /* byte width with 1 bit/pixel */
unsigned char omask;
int oroll;
unsigned char c = 0;
int j;
	memset(preview,0xff,bwidth);
	omask = 0x80;
	oroll = 7;
	if (bitcount == 1) {
	    if (isblack[0])
		for (j = 0; j < bwidth ; j++)
			preview[j] = line[j];
	    else
		for (j = 0; j < bwidth ; j++)
			preview[j] = (unsigned char)~line[j];
	    preview[bwidth-1] |= (unsigned char)(width & 7 ? (1<<(8-(width&7)))-1 : 0);	/* mask for edge of bitmap */
	}
	else {
	    for (j = 0; j < width; j++) {
		switch (bitcount) {
			case 4:
				c = line[j>>1];
				if (!(j&1))
					c >>= 4;
				c = isblack[ c & 0x0f ];
				break;
			case 8:
				c = isblack[ (int)(line[j]) ];
				break;
			case 24:
				c = (unsigned char)(
				    (line[j*3] < 0xff) || 
				    (line[j*3+1] < 0xff) || 
				    (line[j*3+2] < 0xff) );
				break;
		}
		if (c) 
		    preview[j/8] &= (unsigned char)(~omask);
		else
		    preview[j/8] |= omask;
		oroll--;
		omask >>= 1;
		if (oroll < 0) {
		    omask = 0x80;
		    oroll = 7;
		}
	    }
	}
}


/* make a BMP header and palette from a PBMPLUS or BMP bitmap */
/* WARNING: The newpbm structure might not be packed, so it
 * should only be used by write_bitmap_info(), and not written
 * out directly. newpbm->biSize=40 if packed, larger otherwise. */
void
make_bmp_info(LPBITMAP2 newpbm, PREBMAP *ppbmap, unsigned char *pbitmap)
{
unsigned char *prgb;
int clrtablesize;
int i;
	if (*pbitmap == 'P') {
	    newpbm->biSize = sizeof(BITMAP2); /* WARNING - MAY NOT BE PACKED */
	    newpbm->biWidth = ppbmap->width;
	    newpbm->biHeight = ppbmap->height;
	    newpbm->biPlanes = 1;
	    newpbm->biBitCount = (WORD)ppbmap->depth;
	    newpbm->biCompression = 0;
	    newpbm->biSizeImage = 0;
	    newpbm->biXPelsPerMeter = (long)(1000 * option.xdpi / 25.4);
	    newpbm->biYPelsPerMeter = (long)(1000 * option.ydpi / 25.4);
	    newpbm->biClrUsed = 0;
	    newpbm->biClrImportant = 0;
	    /* write palette */
	    prgb = ((BYTE *)newpbm) + sizeof(BITMAP2);
	    switch (ppbmap->depth) {
		case 24:
		    break;	/* no palette */
		case 8:
	    	    for (i = 0; i < 256; i++) {
			*prgb++ = (unsigned char)i;	/* blue */
			*prgb++ = (unsigned char)i;	/* green */
			*prgb++ = (unsigned char)i;	/* red */
			*prgb++ = 0;
		    }
		    break;
		default:
		    *prgb++ = 255;	/* blue */
		    *prgb++ = 255;	/* green */
		    *prgb++ = 255;	/* red */
		    *prgb++ = 0;
		    *prgb++ = 0;	/* blue */
		    *prgb++ = 0;	/* green */
		    *prgb++ = 0;	/* red */
		    *prgb   = 0;
	    }
	}
	else {
	    /* already a BMP, so easy */
	    unsigned int size = get_dword(pbitmap);
	    unsigned char *poldrgb = pbitmap + size;

	    if (size == BITMAP1_LENGTH) {
		newpbm->biSize = sizeof(BITMAP2);
		newpbm->biWidth = ppbmap->width;
		newpbm->biHeight = ppbmap->height;
		newpbm->biPlanes = 1;
		newpbm->biBitCount = (WORD)ppbmap->depth;
		newpbm->biCompression = 0;
		newpbm->biSizeImage = 0;
		newpbm->biXPelsPerMeter = (long)(1000 * option.xdpi / 25.4);
		newpbm->biYPelsPerMeter = (long)(1000 * option.ydpi / 25.4);
		newpbm->biClrUsed = 0;
		newpbm->biClrImportant = 0;
	    }
	    else {
		/* BITMAP2 - just copy it */
	        memmove((char *)newpbm, pbitmap, size);
	    }

	    /* copy palette */
	    prgb = ((unsigned char *)newpbm) + newpbm->biSize;
	    /* read in the color table */
	    if (ppbmap->depth == 24)
		clrtablesize = 0;
	    else
		clrtablesize = 1 << ppbmap->depth;
	    for (i = 0; i < clrtablesize; i++) {
		if (ppbmap->os2) {
		    *prgb++ = poldrgb[i*RGB3_LENGTH + RGB3_BLUE];
		    *prgb++ = poldrgb[i*RGB3_LENGTH + RGB3_GREEN];
		    *prgb++ = poldrgb[i*RGB3_LENGTH + RGB3_RED];
		    *prgb++ = 0;
		}
		else {
		    *prgb++ = poldrgb[i*RGB4_LENGTH + RGB4_BLUE];
		    *prgb++ = poldrgb[i*RGB4_LENGTH + RGB4_GREEN];
		    *prgb++ = poldrgb[i*RGB4_LENGTH + RGB4_RED];
		    *prgb++ = 0;
		}
	    }
	}
}



#define TIFF_BYTE 1
#define TIFF_ASCII 2
#define TIFF_SHORT 3
#define TIFF_LONG 4
#define TIFF_RATIONAL 5

struct rational_s {
	DWORD numerator;
	DWORD denominator;
};
#define TIFF_RATIONAL_SIZE 8

struct ifd_entry_s {
	WORD tag;
	WORD type;
	DWORD length;
	DWORD value;
};
#define TIFF_IFD_SIZE 12

struct tiff_head_s {
	WORD order;
	WORD version;
	DWORD ifd_offset;
};
#define TIFF_HEAD_SIZE 8

DWORD
get_dword(unsigned char *buf)
{
	DWORD dw;
    dw = (DWORD)buf[0];
    dw += ((DWORD)buf[1])<<8;
    dw += ((DWORD)buf[2])<<16;
    dw += ((DWORD)buf[3])<<24;
    return dw;
}

WORD
get_word(unsigned char *buf)
{
    WORD w;
    w = (WORD)buf[0];
    w |= (WORD)(buf[1]<<8);
    return w;
}


/* write DWORD as DWORD */
void
write_dword(DWORD val, FILE *f)
{
    fputc((unsigned char)( val      & 0xff), f);
    fputc((unsigned char)((val>>8)  & 0xff), f);
    fputc((unsigned char)((val>>16) & 0xff), f);
    fputc((unsigned char)((val>>24) & 0xff), f);
}

/* write WORD as DWORD */
void
write_word_as_dword(WORD val, FILE *f)
{
    fputc((unsigned char)( val      & 0xff), f);
    fputc((unsigned char)((val>>8)  & 0xff), f);
    fputc('\0', f);
    fputc('\0', f);
}

/* write WORD as WORD */
void
write_word(WORD val, FILE *f)
{
    fputc((unsigned char)( val      & 0xff), f);
    fputc((unsigned char)((val>>8)  & 0xff), f);
}

int
packbits(BYTE *comp, BYTE *raw, int length)
{
BYTE *cp;
int literal = 0;	/* number of literal bytes */
int prevlit = 0;	/* length of previous literal */
int repeat = 0;		/* number of repeat bytes - 1*/
int start = 0;		/* start of block to be coded */
BYTE previous = raw[0];
int i, j;
    cp = comp;
    for (i=0; i<length; i++) {
	if (literal == 128) {
	    /* code now */
	    *cp++ = (BYTE)(literal-1);
	    for (j=0; j<literal; j++)
		*cp++ = (BYTE)raw[start+j];
	    prevlit = 0; 		/* because we can't add to it */
	    literal = 0;
	    /* repeat = 0; */	/* implicit */
	    start = i;
	    previous = raw[i];
	}
	if (repeat == 128) {
	    /* write out repeat block */
	    *cp++ = (BYTE)-127;	/* repeat count 128 */
	    *cp++ = previous;
	    repeat = 0;
	    literal = 0;
	    start = i;
	    prevlit = 0;
	    previous = raw[i];
	}
	if (raw[i] == previous) {
	    if (literal == 1) {
		/* replace by repeat */
		repeat = 1;
		literal = 0;
	    }
	    else if (literal) {
		/* write out existing literal */
		literal--;	/* remove repeat byte from literal */
		*cp++ = (BYTE)(literal-1);
		for (j=0; j<literal; j++)
		    *cp++ = raw[start+j];
		if (literal < 126)
		    prevlit = literal; 
		else
		    prevlit = 0;	/* we won't be able to add to it */
		/* repeat = 0; */	/* implicit */
		start = i-1;
		repeat = 1;
		literal = 0;
	    }
	    repeat++;
	}
	else {
	    /* doesn't match previous byte */
	    if (repeat) {
		/* write out repeat block, or merge with literal */
		if (repeat == 1) {
		    /* repeats must be 2 bytes or more, so code as literal */
		    literal = repeat;
		    repeat = 0;
		} else if (repeat == 2) {	/* 2 byte repeat */
		    /* code 2 byte repeat as repeat */
		    /* except when preceeded by literal */
		    if ( (prevlit) && (prevlit < 126) ) {
			/* previous literal and room to combine */
			start -= prevlit;  /* continue previous literal */
			cp -= prevlit+1;
			literal = prevlit + 2;
			prevlit = 0;
			repeat = 0;
		    }
		    else {
			/* code as repeat */
			*cp++ = (BYTE)(-repeat+1);
			*cp++ = previous;
			start = i;
			prevlit = 0;
			/* literal = 0; */	/* implicit */
			repeat = 0;
		    }
		    /* literals will be coded later */
		}
		else {
		    /* repeat of 3 or more bytes */
		    *cp++ = (BYTE)(-repeat+1);
		    *cp++ = previous;
		    start = i;
		    repeat = 0;
		    prevlit = 0;
		}
	    }
	    literal++;
	}
        previous = raw[i];
    }
    if (repeat == 1) {
	/* can't code repeat 1, use literal instead */
	literal = 1;
	repeat = 0;
    }
    if (literal) {
	/* code left over literal */
	*cp++ = (BYTE)(literal-1);
	for (j=0; j<literal; j++)
	    *cp++ = raw[start+j];
    }
    if (repeat) {
	/* code left over repeat */
	*cp++ = (BYTE)(-repeat+1);
	*cp++ = previous;
    }
    return (int)(cp - comp);	/* number of code bytes */
}

/* Write tiff file from DIB bitmap */
/* Since this will be used by a DOS EPS file, we write an Intel TIFF file */
int
write_tiff(FILE *f, unsigned char *pbitmap, BOOL tiff4, BOOL use_packbits, BOOL calc_bbox)
{
#define IFD_MAX_ENTRY 12
WORD ifd_length;
DWORD ifd_next;
DWORD tiff_end, end;
int i;
unsigned char *preview;
BYTE *line;
int source_bwidth, bwidth;
BOOL soft_extra = FALSE;
PREBMAP prebmap;
PSBBOX devbbox;	/* in pixel units */
int width, height;	/* size of preview */
int code;
int bitoffset;
WORD *comp_length=NULL;	/* lengths of compressed lines */
BYTE *comp_line=NULL;	/* compressed line buffer */
int rowsperstrip;
int stripsperimage;
int strip, is;
int lastrow;
char appname[MAXSTR];
	
	convert_widechar(appname, szAppName, sizeof(appname)-1);
	if (*pbitmap == 'P')
	    code = scan_pbmplus(&prebmap, pbitmap);
	else {
	    code = scan_dib(&prebmap, pbitmap);
	}
	if (code)
	    return code;

	if (calc_bbox) {
	    scan_bbox(&prebmap, &devbbox);
	    if (devbbox.valid) {
	    	/* copy to global bbox as if obtained by PS to EPS */
	    	bbox.llx = (int)(devbbox.llx * 72.0 / option.xdpi - 0.5);
	    	bbox.lly = (int)(devbbox.lly * 72.0 / option.ydpi - 0.5);
	    	bbox.urx = (int)(devbbox.urx * 72.0 / option.xdpi + 1.5);
	    	bbox.ury = (int)(devbbox.ury * 72.0 / option.ydpi + 1.5);
	    	bbox.valid = TRUE;
	    }
	    else {
		devbbox.urx = prebmap.width;
		devbbox.ury = prebmap.height;
		devbbox.llx = devbbox.lly = 0;
	    }
	}
	else {
#ifdef EPSTOOL
	    devbbox.urx = prebmap.width;
	    devbbox.ury = prebmap.height;
	    devbbox.llx = devbbox.lly = 0;
#else
	    if (display.epsf_clipped || (psfile.dsc->bbox==(CDSCBBOX *)NULL)) {
		/* copy all of display bitmap */
		devbbox.urx = prebmap.width;
		devbbox.ury = prebmap.height;
		devbbox.llx = devbbox.lly = 0;
	    }
	    else {
		/* copy only part of display bitmap */
		devbbox.llx = (int)(psfile.dsc->bbox->llx * option.xdpi / 72.0);
		devbbox.lly = (int)(psfile.dsc->bbox->lly * option.ydpi / 72.0);
		devbbox.urx = (int)(psfile.dsc->bbox->urx * option.xdpi / 72.0 + 0.5);
		devbbox.ury = (int)(psfile.dsc->bbox->ury * option.ydpi / 72.0 + 0.5);
#define LIMIT(z, limit) if (z < 0) z = 0; else if (z > limit) z = limit
		LIMIT(devbbox.llx, prebmap.width);
		LIMIT(devbbox.lly, prebmap.height);
		LIMIT(devbbox.urx, prebmap.width);
		LIMIT(devbbox.ury, prebmap.height);
#undef LIMIT
	    }
#endif
	}

	width  = devbbox.urx - devbbox.llx;	/* width  of dest bitmap */
	height = devbbox.ury - devbbox.lly;	/* height of dest bitmap */
	/* byte width of preview */
	if (tiff4)	/* always monochrome */
	    bwidth = (width + 7) >> 3;
	else
	    bwidth = (width * prebmap.depth + 7) >> 3;

	/* byte width of source bitmap */
	if (tiff4)
	    source_bwidth = ((prebmap.width + 7) & ~7) >> 3; /* byte width with 1 bit/pixel */
	else
	    source_bwidth = ((prebmap.width * prebmap.depth + 7) & ~7) >> 3; /* byte width */

	if (tiff4)
	    bitoffset = devbbox.llx;
	else
	    bitoffset = devbbox.llx * prebmap.depth;

	if (tiff4)
	    rowsperstrip = 1; /* make TIFF 4 very simple */
	else {
	    /* work out RowsPerStrip, to give < 8k compressed */
	    /* or uncompressed data per strip */
	    rowsperstrip = (8192 - 256) / bwidth;
	    if (rowsperstrip == 0)
		rowsperstrip = 1;	/* strips are larger than 8k */
	}
	stripsperimage = (height + rowsperstrip - 1) / rowsperstrip;
	if (stripsperimage == 1)
	    rowsperstrip = height;

	preview = (unsigned char *) malloc(prebmap.bytewidth);
	if (preview == NULL)
	    return 1;
	memset(preview,0xff,prebmap.bytewidth);

	/* compress bitmap, throwing away result, to find out compressed size */
	if (use_packbits) {
	    comp_length = (WORD *)malloc(stripsperimage * sizeof(WORD));
	    if (comp_length == NULL) {
		free(preview);
		return 1;
	    }
	    comp_line = (BYTE *)malloc(bwidth + bwidth/64 + 1);
	    if (comp_line == NULL) {
		free(preview);
		free(comp_length);
		return 1;
	    }
	    if (prebmap.bits) {
		if (prebmap.topleft)
		    line = (BYTE *)prebmap.bits + ((long)prebmap.bytewidth * (prebmap.height - devbbox.ury));
		else
		    line = (BYTE *)prebmap.bits + ((long)prebmap.bytewidth * (devbbox.ury-1));
	    }
	    else {
		return 1;
	    }
	    /* process each strip */
	    for (strip = 0; strip < stripsperimage; strip++) {
		is = strip * rowsperstrip;
		lastrow = min( rowsperstrip, height - is);
		comp_length[strip] = 0;
		/* process each line within strip */
		for (i = 0; i< lastrow; i++) {
		    if (tiff4 || prebmap.depth==1)
			get_dib_line(line, preview, prebmap.width, prebmap.depth);
		    else
			memmove(preview,  line, prebmap.bytewidth);
		    if (bitoffset)
			shift_preview(preview, source_bwidth, bitoffset);
		    if ( !tiff4 && (*pbitmap != 'P') && (prebmap.depth==24) )
			reverse_triples(preview, width);
		    comp_length[strip] += (WORD)packbits(comp_line, preview, bwidth);
		    if (prebmap.bits) {
			if (prebmap.topleft)
			    line += prebmap.bytewidth;
			else
			    line -= prebmap.bytewidth;
		    }
		}
	    }
	}
	 

	/* write header */
	tiff_end = TIFF_HEAD_SIZE;
	tiff_word(0x4949, f);	/* Intel = little endian */
	tiff_word(42, f);
	tiff_long(tiff_end, f);

	/* write count of ifd entries */
	tiff_end += 2 /* sizeof(ifd_length) */;
	if (tiff4)
	    ifd_length = 10;
	else {
	    switch (prebmap.depth) {
		case 24:
		    /* extras are BitsPerPixel, SamplesPerPixel */
	            ifd_length = 15;
		    break;
		case 8:
		case 4:
		    /* extras are BitsPerPixel, ColorMap */
	            ifd_length = 15;
		    break;
		default:	/* bi-level */
		    ifd_length = 13;
	    }
	}
	tiff_word(ifd_length, f);

	tiff_end += ifd_length * TIFF_IFD_SIZE + 4 /* sizeof(ifd_next) */;
	ifd_next = 0;

	/* write each of the ifd entries */
	if (tiff4) {
	    tiff_word(0xff, f);	    /* SubfileType */
	    tiff_word(TIFF_SHORT, f);  /* value type */
	    tiff_long(1, f);		    /* length */
	    tiff_short(0, f);		    /* value */
	}
	else {
	    tiff_word(0xfe, f);	/* NewSubfileType */
	    tiff_word(TIFF_LONG, f);
	    tiff_long(1, f);		    /* length */
	    tiff_long(0, f);		    /* value */
	}

	tiff_word(0x100, f);	/* ImageWidth */
	if (tiff4) {
	    tiff_word(TIFF_SHORT, f);
	    tiff_long(1, f);
	    tiff_short((short)width, f);
	}
	else {
	    tiff_word(TIFF_LONG, f);
	    tiff_long(1, f);
	    tiff_long(width, f);
	}

	tiff_word(0x101, f);	/* ImageHeight */
	if (tiff4) {
	    tiff_word(TIFF_SHORT, f);
	    tiff_long(1, f);
	    tiff_short((short)height, f);
	}
	else {
	    tiff_word(TIFF_LONG, f);
	    tiff_long(1, f);
	    tiff_long(height, f);
	}

	if (!tiff4 && prebmap.depth>1) {
	    tiff_word(0x102, f);	/* BitsPerSample */
	    tiff_word(TIFF_SHORT, f);
	    if (prebmap.depth == 24) {
	        tiff_long(3, f);
		tiff_long(tiff_end, f);
		tiff_end += 6;
	    }
	    else {
	        tiff_long(1, f);
		tiff_short((WORD)prebmap.depth, f);
	    }
	}

	tiff_word(0x103, f);	/* Compression */
	tiff_word(TIFF_SHORT, f);
	tiff_long(1, f);
	if (use_packbits)
	    tiff_short(32773U, f);	/* packbits compression */
	else
	    tiff_short(1, f);		/* no compression */

	tiff_word(0x106, f);	/* PhotometricInterpretation */
	tiff_word(TIFF_SHORT, f);
	tiff_long(1, f);
	if (tiff4 || prebmap.depth==1)
	    tiff_short(1, f);		/* black is zero */
	else if (prebmap.depth==24)
	    tiff_short(2, f);		/* RGB */
	else /* prebmap.depth == 4 or 8 */
	    tiff_short(3, f);		/* Palette Color */

	tiff_word(0x111, f);	/* StripOffsets */
	tiff_word(TIFF_LONG, f);
	if (stripsperimage == 1) {
	    /* This is messy and fragile */
	    int len = 0;
	    tiff_long(1, f);
	    len += TIFF_RATIONAL_SIZE * 2;		/* resolutions */
	    if (!tiff4) {
		len += ((strlen(appname)+2)&~1) + 20;	/* software and date */
		if (prebmap.depth == 4 || prebmap.depth == 8)
		    len += 2 * 3*(1<<prebmap.depth);	/* palette */
	    }
	    tiff_long(tiff_end + len, f);
	}
	else {
	    tiff_long(stripsperimage, f);
	    tiff_long(tiff_end, f);
	    tiff_end += (stripsperimage * 4 /* sizeof(DWORD) */);
	}

	if (!tiff4 && (prebmap.depth==24)) {
	    tiff_word(0x115, f);	/* SamplesPerPixel */
	    tiff_word(TIFF_SHORT, f);
	    tiff_long(1, f);
	    tiff_short(3, f);		/* 3 components */
	}

	tiff_word(0x116, f);	/* RowsPerStrip */
	tiff_word(TIFF_LONG, f);
	tiff_long(1, f);
	tiff_long(rowsperstrip, f);

	tiff_word(0x117, f);	/* StripByteCounts */
	tiff_word(TIFF_LONG, f);
	if (stripsperimage == 1) {
	    tiff_long(1, f);
	    if (use_packbits)
		tiff_long(comp_length[0], f);
	    else
		tiff_long(bwidth * rowsperstrip, f);
	}
	else {
	    tiff_long(stripsperimage, f);
	    tiff_long(tiff_end, f);
	    tiff_end += (stripsperimage * 4 /* sizeof(DWORD) */);
	}

	tiff_word(0x11a, f);	/* XResolution */
	tiff_word(TIFF_RATIONAL, f);
	tiff_long(1, f);
	tiff_long(tiff_end, f);
	tiff_end += TIFF_RATIONAL_SIZE;

	tiff_word(0x11b, f);	/* YResolution */
	tiff_word(TIFF_RATIONAL, f);
	tiff_long(1, f);
	tiff_long(tiff_end, f);
	tiff_end += TIFF_RATIONAL_SIZE;

	if (!tiff4) {
	    tiff_word(0x128, f);	/* ResolutionUnit */
	    tiff_word(TIFF_SHORT, f);
	    tiff_long(1, f);
	    tiff_short(2, f);		/* inches */

	    tiff_word(0x131, f);	/* Software */
	    tiff_word(TIFF_ASCII, f);
	    i = strlen(appname) + 1;
	    tiff_long(i, f);
	    tiff_long(tiff_end, f);
	    tiff_end += i;
	    if (tiff_end & 1) { /* pad to word boundary */
	        soft_extra = TRUE;
	        tiff_end++;
	    }

	    tiff_word(0x132, f);	/* DateTime */
	    tiff_word(TIFF_ASCII, f);
	    tiff_long(20, f);
	    tiff_long(tiff_end, f);
	    tiff_end += 20;

	    if (prebmap.depth==4 || prebmap.depth==8) {
		int palcount = 1<<prebmap.depth;
		tiff_word(0x140, f);	/* ColorMap */
		tiff_word(TIFF_SHORT, f);
		tiff_long(3*palcount, f);  /* size of ColorMap */
		tiff_long(tiff_end, f);
		tiff_end += 2 * 3*palcount;
	    }
	}


	/* write end of ifd tag */
	tiff_long(ifd_next, f);

	/* BitsPerSample for 24 bit colour */
	if (!tiff4 && (prebmap.depth==24)) {
	    tiff_word(8, f);
	    tiff_word(8, f);
	    tiff_word(8, f);
	}

	/* strip offsets */
	end = tiff_end;
	if (stripsperimage > 1) {
	    int stripwidth = bwidth * rowsperstrip;
	    for (i=0; i<stripsperimage; i++) {
		tiff_long(end, f);
		if (use_packbits)
		    end += comp_length[i];
		else
		    end += stripwidth;
	    }
	}

	/* strip byte counts (after compression) */
	if (stripsperimage > 1) {
	    for (i=0; i<stripsperimage; i++) {
		if (use_packbits)
		    tiff_long(comp_length[i], f);
		else {
	    	    is = i * rowsperstrip;
	    	    lastrow = min( rowsperstrip, height - is);
		    tiff_long(lastrow * bwidth, f);
		}
	    }
	}

	/* XResolution rational */
	tiff_long((int)option.xdpi, f);
	tiff_long(1, f);
	/* YResolution rational */
	tiff_long((int)option.ydpi, f);
	tiff_long(1, f);

	/* software and time strings */
	if (!tiff4) {
	    time_t t;
	    char now[20];
	    struct tm* dt;
	    fwrite(appname, 1, strlen(appname)+1, f);
	    if (soft_extra)
	        fputc('\0',f);
	    t = time(NULL);
	    dt = localtime(&t);
	    sprintf(now, "%04d:%02d:%02d %02d:%02d:%02d",
		dt->tm_year+1900, dt->tm_mon+1, dt->tm_mday,
		dt->tm_hour, dt->tm_min, dt->tm_sec);
	    fwrite(now, 1, 20, f);
	}

	/* Palette */
	if (!tiff4 && ((prebmap.depth==4) || (prebmap.depth==8))) {
	    int palcount = 1<<prebmap.depth;
#define PALVAL(x) ((WORD)((x<< 8) | x))
	    if (*pbitmap == 'P') {
		for (i=0; i<palcount; i++)
		    tiff_word(PALVAL(i), f);
		for (i=0; i<palcount; i++)
		    tiff_word(PALVAL(i), f);
		for (i=0; i<palcount; i++)
		    tiff_word(PALVAL(i), f);
	    }
	    else {
		unsigned char *prgb = pbitmap + get_dword(pbitmap);
		for (i=0; i<palcount; i++)
		    tiff_word(PALVAL(prgb[i*RGB4_LENGTH + RGB4_RED]), f);
		for (i=0; i<palcount; i++)
		    tiff_word(PALVAL(prgb[i*RGB4_LENGTH + RGB4_GREEN]), f);
		for (i=0; i<palcount; i++)
		    tiff_word(PALVAL(prgb[i*RGB4_LENGTH + RGB4_BLUE]), f);
	    }
#undef PALVAL
	}

	memset(preview,0xff,prebmap.bytewidth);

	if (prebmap.bits) {
	    if (prebmap.topleft)
		line = (BYTE *)prebmap.bits + ((long)prebmap.bytewidth * (prebmap.height - devbbox.ury));
	    else
		line = (BYTE *)prebmap.bits + ((long)prebmap.bytewidth * (devbbox.ury-1));
	}
	else {
	    return 1;
	}
        /* process each strip of bitmap */
	for (strip = 0; strip < stripsperimage; strip++) {
	    int len;
	    is = strip * rowsperstrip;
	    lastrow = min( rowsperstrip, height - is);
            /* process each row of strip */
	    for (i = 0; i < lastrow; i++) {
		if (tiff4 || prebmap.depth==1)
		    get_dib_line(line, preview, prebmap.width, prebmap.depth);
		else
		    memmove(preview,  line, prebmap.bytewidth);
		if (bitoffset)
		    shift_preview(preview, source_bwidth, bitoffset);
		if ( !tiff4 && (*pbitmap != 'P') && (prebmap.depth==24) )
		    reverse_triples(preview, width);
		if (use_packbits) {
		    len = (WORD)packbits(comp_line, preview, bwidth);
		    fwrite(comp_line, 1, len, f);
		}
		else
		    fwrite(preview, 1, bwidth, f);
		if (prebmap.bits) {
		    if (prebmap.topleft)
			line += prebmap.bytewidth;
		    else
			line -= prebmap.bytewidth;
		}
	    }
	}
	if (use_packbits) {
	    free(comp_length);
	    free(comp_line);
	}
	free(preview);
	return 0;
}


/* make a PC EPS file with a TIFF Preview */
/* from a PS file and a bitmap */
int
make_eps_tiff(int type, BOOL calc_bbox, const char *epsname)
{
unsigned char *pbitmap;
char *buffer;
unsigned int count;
FILE *epsfile;
FILE *tiff_file;
char tiffname[MAXSTR];
#if defined(__EMX__) || defined (_MSC_VER)
#pragma pack(1)
#endif
struct eps_header_s eps_header;
#if defined(__EMX__) || defined (_MSC_VER)
#pragma pack()
#endif
FILE *tpsfile;
char tpsname[MAXSTR];
int code;
	
	if ( (pbitmap = (unsigned char *)get_bitmap()) == (unsigned char *)NULL) {
	    play_sound(SOUND_ERROR);
	    return 1;
	}

	if ( (tiff_file = gp_open_scratch_file(szScratch, tiffname, "wb")) == (FILE *)NULL) {
	    play_sound(SOUND_ERROR);
	    release_bitmap();
	    return 1;
	}
	code = write_tiff(tiff_file, pbitmap, (type == IDM_MAKEEPST4), (type == IDM_MAKEEPST6P), calc_bbox);

	fclose(tiff_file);
	release_bitmap();
	if (code) {
	    unlink(tiffname);
	    return code;
	}

	if (calc_bbox) {
	    CDSC *dsc = psfile.dsc;
	    /* we need to copy the psfile to a temporary file */
	    /* because we will be changing the %%BoundingBox line */
	    if ( (tpsfile = gp_open_scratch_file(szScratch, tpsname, "wb")) == (FILE *)NULL) {
		play_sound(SOUND_ERROR);
		return 1;
	    }
	    copy_bbox_header(tpsfile); /* adjust %%BoundingBox: comment */
	    ps_copy(tpsfile, psfile.file, dsc->begindefaults, dsc->enddefaults);
	    ps_copy(tpsfile, psfile.file, dsc->beginprolog, dsc->endprolog);
	    ps_copy(tpsfile, psfile.file, dsc->beginsetup, dsc->endsetup);
	    if (dsc->page_count)
	        ps_copy(tpsfile, psfile.file, dsc->page[0].begin, dsc->page[0].end);
	    copy_trailer_without_bbox(tpsfile);
	    fclose(tpsfile);
	    if ( (tpsfile = fopen(tpsname, "rb")) == (FILE *)NULL) {
		play_sound(SOUND_ERROR);
		return 1;
	    }
	}
	else {
	    if ( (tpsfile = fopen(psfile_name(&psfile), "rb"))
	  	== (FILE *)NULL) {
		unlink(tiffname);
		return 1;
	    }
	}

#ifdef EPSTOOL
	if (*epsname!='\0')
	    epsfile = fopen(epsname,"wb");
	else
	    epsfile = stdout;
#else
	/* create EPS file */
	epsfile = fopen(epsname,"wb");
#endif
	if (epsfile == (FILE *)NULL) {
	    play_sound(SOUND_ERROR);
	    release_bitmap();
	    return 1;
	}

	/* write DOS EPS binary header */
	eps_header.id[0] = (char) 0xc5;
	eps_header.id[1] = (char) 0xd0;
	eps_header.id[2] = (char) 0xd3;
	eps_header.id[3] = (char) 0xc6;
	eps_header.ps_begin = EPS_HEADER_SIZE;
	if (calc_bbox) {
	    fseek(tpsfile, 0, SEEK_END);
	    eps_header.ps_length = ftell(tpsfile);
	}
	else if (psfile.dsc->dcs2 && psfile.dsc->page_count) {
	    /* add all pages */
	    int i;
 	    unsigned long begin = psfile.dsc->page[0].begin;
	    unsigned long end = begin;
	    for (i=0; i<(int)psfile.dsc->page_count; i++) {
		if (psfile.dsc->page[i].end > end)
	            end = psfile.dsc->page[i].end;
	    }
	    eps_header.ps_length = end - begin;
	}
	else {
	    /* don't use ftell(), because we may already have an eps preview */
	    unsigned long end = psfile.dsc->begincomments;
	    if (psfile.dsc->endcomments > end)
	       end = psfile.dsc->endcomments;
	    if (psfile.dsc->enddefaults > end)
	       end = psfile.dsc->enddefaults;
	    if (psfile.dsc->endprolog > end)
	       end = psfile.dsc->endprolog;
	    if (psfile.dsc->endsetup > end)
	       end = psfile.dsc->endsetup;
	    if (psfile.dsc->page_count && (psfile.dsc->page[0].end > end))
	        end = psfile.dsc->page[0].end;
	    if (psfile.dsc->endtrailer > end)
	       end = psfile.dsc->endtrailer;
	    eps_header.ps_length = end - psfile.dsc->begincomments;
	}
	eps_header.mf_begin = 0;
	eps_header.mf_length = 0;
	eps_header.tiff_begin = eps_header.ps_begin + eps_header.ps_length;
	tiff_file = fopen(tiffname,"rb");
	fseek(tiff_file, 0, SEEK_END);
	eps_header.tiff_length = ftell(tiff_file);
	eps_header.checksum = 0xffff;
	write_doseps_header(&eps_header, epsfile);

	buffer = (char *)malloc(COPY_BUF_SIZE);
	if (buffer == (char *)NULL) {
	    play_sound(SOUND_ERROR);
	    fclose(epsfile);
	    unlink(epsname);
	    fclose(tiff_file);
	    unlink(tiffname);
	    fclose(tpsfile);
	    if (calc_bbox)
		unlink(tpsname);
	    return 1;
	}

	/* copy EPS file */
	if (calc_bbox) {
	    rewind(tpsfile);
	    while ( (count = fread(buffer, 1, COPY_BUF_SIZE, tpsfile)) != 0 )
		fwrite(buffer, 1, count, epsfile);
	}
	else if (psfile.dsc->dcs2 && psfile.dsc->page_count) {
	    /* add all pages */
	    ps_copy(epsfile, psfile.file, psfile.dsc->page[0].begin, 
		eps_header.ps_length + psfile.dsc->page[0].begin);
	}
	else {
	    CDSC *dsc = psfile.dsc;
	    ps_copy(epsfile, psfile.file, dsc->begincomments, dsc->endcomments);
	    ps_copy(epsfile, psfile.file, dsc->begindefaults, dsc->enddefaults);
	    ps_copy(epsfile, psfile.file, dsc->beginprolog, dsc->endprolog);
	    ps_copy(epsfile, psfile.file, dsc->beginsetup, dsc->endsetup);
	    if (dsc->page_count)
	        ps_copy(epsfile, psfile.file, dsc->page[0].begin, dsc->page[0].end);
	    ps_copy(epsfile, psfile.file, dsc->begintrailer, dsc->endtrailer);
	}
	
	/* copy tiff file */
	rewind(tiff_file);
        while ( (count = fread(buffer, 1, COPY_BUF_SIZE, tiff_file)) != 0 ) 
	{
	    fwrite(buffer, 1, count, epsfile);
	}

	free(buffer);
	fclose(tiff_file);
	unlink(tiffname);
        fclose(tpsfile);
	if (calc_bbox)
	    unlink(tpsname);
	if (*epsname!='\0')
	   fclose(epsfile);
	return 0;
}

static char hex[17] = "0123456789ABCDEF";

/* write interchange preview to file f */
int
write_interchange(FILE *f, unsigned char *pbitmap, BOOL calc_bbox)
{
	int i, j;
	unsigned char *preview;
	BYTE *line;
	int preview_width, bwidth;
	int lines_per_scan;
	PREBMAP prebmap;
	PSBBOX devbbox;	/* in pixel units */
	int code;
	CDSC *dsc = psfile.dsc;
	
	if (*pbitmap == 'P')
	    code = scan_pbmplus(&prebmap, pbitmap);
	else {
	    code = scan_dib(&prebmap, pbitmap);
	}
	if (code)
	    return code;
	
	if (calc_bbox) {
	    scan_bbox(&prebmap, &devbbox);
	    if (devbbox.valid) {
	    	/* copy to global bbox as if obtained by PS to EPS */
	    	bbox.llx = (int)(devbbox.llx * 72.0 / option.xdpi - 0.5);
	    	bbox.lly = (int)(devbbox.lly * 72.0 / option.ydpi - 0.5);
	    	bbox.urx = (int)(devbbox.urx * 72.0 / option.xdpi + 1.5);
	    	bbox.ury = (int)(devbbox.ury * 72.0 / option.ydpi + 1.5);
	    	bbox.valid = TRUE;
	    }
	    copy_bbox_header(f); /* adjust %%BoundingBox: comment */
	}
	else {
#ifdef EPSTOOL
	    devbbox.urx = prebmap.width;
	    devbbox.ury = prebmap.height;
	    devbbox.llx = devbbox.lly = 0;
#else
	    if (display.epsf_clipped || (psfile.dsc->bbox==(CDSCBBOX *)NULL)) {
		/* copy all of display bitmap */
		devbbox.urx = prebmap.width;
		devbbox.ury = prebmap.height;
		devbbox.llx = devbbox.lly = 0;
	    }
	    else {
		/* copy only part of display bitmap */
		devbbox.llx = (int)(psfile.dsc->bbox->llx * option.xdpi / 72.0);
		devbbox.lly = (int)(psfile.dsc->bbox->lly * option.ydpi / 72.0);
		devbbox.urx = (int)(psfile.dsc->bbox->urx * option.xdpi / 72.0 + 0.5);
		devbbox.ury = (int)(psfile.dsc->bbox->ury * option.ydpi / 72.0 + 0.5);
#define LIMIT(z, limit) if (z < 0) z = 0; else if (z > limit) z = limit
		LIMIT(devbbox.llx, prebmap.width);
		LIMIT(devbbox.lly, prebmap.height);
		LIMIT(devbbox.urx, prebmap.width);
		LIMIT(devbbox.ury, prebmap.height);
#undef LIMIT
	    }
#endif
	    ps_copy(f, psfile.file, dsc->begincomments, dsc->endcomments);
	}

	bwidth = (((devbbox.urx-devbbox.llx) + 7) & ~7) >> 3; /* byte width with 1 bit/pixel */
	preview_width = ((prebmap.width + 7) & ~7) >> 3; /* byte width with 1 bit/pixel */

	preview = (unsigned char *) malloc(preview_width);

	lines_per_scan = ((bwidth-1) / 32) + 1;
	fprintf(f,"%%%%BeginPreview: %u %u 1 %u",(devbbox.urx-devbbox.llx), (devbbox.ury-devbbox.lly), 
	    (devbbox.ury-devbbox.lly)*lines_per_scan);
	fputs(EOLSTR, f);

	if (prebmap.bits) {
	    if (prebmap.topleft)
		line = (BYTE *)prebmap.bits + ((long)prebmap.bytewidth * (prebmap.height - devbbox.ury));
	    else
		line = (BYTE *)prebmap.bits + ((long)prebmap.bytewidth * (devbbox.ury-1));
	}
	else {
	    return FALSE;
	}
	/* process each line of bitmap */
	for (i = 0; i < (devbbox.ury-devbbox.lly); i++) {
	    get_dib_line(line, preview, prebmap.width, prebmap.depth);
	    if (devbbox.llx)
		shift_preview(preview, preview_width, devbbox.llx);
	    fputs("% ",f);
	    for (j=0; j<bwidth; j++) {
		if (j && ((j & 31) == 0)) {
		    fputs(EOLSTR, f);
		    fputs("% ",f);
		}
		fputc(hex[15-((preview[j]>>4)&15)],f);
		fputc(hex[15-((preview[j])&15)],f);
	    }
	    fputs(EOLSTR, f);
	    if (prebmap.bits) {
		if (prebmap.topleft)
		    line += prebmap.bytewidth;
		else 
		    line -= prebmap.bytewidth;
	    }
	}

	fputs("%%EndPreview",f);
	fputs(EOLSTR, f);
	free(preview);
	ps_copy(f, psfile.file, dsc->begindefaults, dsc->enddefaults);
	ps_copy(f, psfile.file, dsc->beginprolog, dsc->endprolog);
	ps_copy(f, psfile.file, dsc->beginsetup, dsc->endsetup);
	if (dsc->page_count)
	    ps_copy(f, psfile.file, dsc->page[0].begin, dsc->page[0].end);
	if (calc_bbox)
	    copy_trailer_without_bbox(f);
	else
	    ps_copy(f, psfile.file, dsc->begintrailer, dsc->endtrailer);
	return 0;
}

/* make an EPSI file with an Interchange Preview */
/* from a PS file and a bitmap */
int
make_eps_interchange(BOOL calc_bbox, const char *epiname)
{
FILE *epifile;
LPBITMAP2 pbm;
int code;

	if ( (pbm = get_bitmap()) == (LPBITMAP2)NULL) {
	    play_sound(SOUND_ERROR);
	    return 1;
	}

#ifdef EPSTOOL
	if (*epiname!='\0')
	    epifile = fopen(epiname,"wb");
	else
	    epifile = stdout;
#else
	/* create EPI file */
	epifile = fopen(epiname,"wb");
#endif

	if (epifile == (FILE *)NULL) {
	    play_sound(SOUND_ERROR);
	    release_bitmap();
	    return 1;
	}

	gfile_seek(psfile.file, 0, gfile_begin);
	code = write_interchange(epifile, (unsigned char *)pbm, calc_bbox);
	if (*epiname!='\0')
	    fclose(epifile);
	release_bitmap();
	if (code)
	    unlink(epiname);
	return code;
}


/* scan bitmap and return bbox measured in pixels */
void
scan_bbox(PREBMAP *pprebmap, PSBBOX *devbbox)
{
	unsigned char *preview;
	BYTE *line;
	int bwidth = ((pprebmap->width + 7) & ~7) >> 3; /* byte width with 1 bit/pixel */
	int i, j, k, l;
	int x;
	BYTE ch;
	BYTE *chline;
	unsigned char omask;

	devbbox->llx = pprebmap->width;
	devbbox->lly = pprebmap->height;
	devbbox->urx = devbbox->ury = 0;
	devbbox->valid = FALSE;

	preview = (unsigned char *) malloc(bwidth);
	memset(preview,0xff,bwidth);

	if (pprebmap->bits) {
	    if (pprebmap->topleft)
		line = (BYTE *)pprebmap->bits + ((long)pprebmap->bytewidth * (pprebmap->height-1));
	    else
		line = (BYTE *)pprebmap->bits;
	}
	else {
	    devbbox->llx = 0;
	    devbbox->lly = 0;
	    devbbox->urx = pprebmap->width;
	    devbbox->ury = pprebmap->height;
	    devbbox->valid = FALSE;
	    return;
	}
        /* process each line of bitmap */
	for (i = 0; i < pprebmap->height; i++) {
	    /* get 1bit/pixel line, 0=black, 1=white */
	    get_dib_line(line, preview, pprebmap->width, pprebmap->depth);
	    chline = preview;
	    ch = 0;
	    for (j=0; j<bwidth; j++)
	        ch |= (BYTE)(~(*chline++));	/* check for black pixels */
	    if (ch) {
		/* adjust y coordinates of bounding box */
		if (i < devbbox->lly)
		    devbbox->lly = i;
		if (i+1 > devbbox->ury)
		    devbbox->ury = i+1;
		/* scan for x coordinates of black pixels */
	        chline = preview;
		for (k=0; k<bwidth; k++) {
		    if (~(*chline) & 0xff) { /* a pixel is black */
			omask = 0x80;
			for (l=0; l<8; l++) {
			    if ( ~*chline & omask ) {
			    	x = k*8 + l;
			    	if (x < devbbox->llx)
			    	    devbbox->llx = x;
			    	if (x+1 > devbbox->urx)
			    	    devbbox->urx = x+1;
			    }
		            omask >>= 1;
			}
		    }
		    chline++;
		}
	    }
	    if (pprebmap->bits) {
		if (pprebmap->topleft)
		    line -= pprebmap->bytewidth;
		else
		    line += pprebmap->bytewidth;
	    }
	}
	free(preview);
	if ( (devbbox->lly < devbbox->ury) && (devbbox->llx < devbbox->urx) )
	    devbbox->valid = TRUE;
	
	if (debug)
	{   char buf[256];
	    sprintf(buf, "scan_bbox=[%d %d %d %d] in pixels\n", devbbox->llx, devbbox->lly, devbbox->urx, devbbox->ury);
	    gs_addmess(buf);
	}
}

/* shift preview by offset bits to the left */
/* width is in bytes */
/* fill exposed bits with 1's */
void
shift_preview(unsigned char *preview, int bwidth, int offset)
{
int bitoffset;
int byteoffset;
int newwidth;
int shifter;
int i;
	if (offset == 0)
	    return;
	byteoffset = offset / 8;
	newwidth = bwidth - byteoffset;
	/* first remove byte offset */
	memmove(preview, preview+byteoffset, newwidth);
	memset(preview+newwidth, 0xff, bwidth-newwidth);
	/* next remove bit offset */
	bitoffset = offset - byteoffset*8;
	if (bitoffset==0)
	    return;
	bitoffset = 8 - bitoffset;
	for (i=0; i<newwidth; i++) {
	   shifter = preview[i] << 8;
	   if (i==newwidth-1)
	       shifter += 0xff;	/* can't access preview[bwidth] */
	   else
	       shifter += preview[i+1];  
	   preview[i] = (unsigned char)(shifter>>bitoffset);
	}
}

/* Copy the header to file f */
/* change bbox line if present, or add bbox line */
void
copy_bbox_header(FILE *f)
{
    char text[DSC_LINE_LENGTH+1];
    BOOL bbox_written = FALSE;
    long position;
    CDSC *dsc = psfile.dsc;
    char eol[3];
    char *p;
    memset(eol, 0, sizeof(eol));

    gfile_seek(psfile.file, dsc->begincomments, gfile_begin);
    if (dsc->bbox != (CDSCBBOX *)NULL) {
      while ( ps_copy_find(f, psfile.file, dsc->endcomments,
		text, sizeof(text), "%%BoundingBox:") ) {
	if (!bbox_written) {
	    p = text;
	    while (*p && (*p != '\r') && (*p != '\n'))
		p++;	/* find EOL used in DSC file */
	    strncpy(eol, p, sizeof(eol)-1);
	    fprintf(f, "%%%%BoundingBox: %d %d %d %d%s",
		bbox.llx, bbox.lly, bbox.urx, bbox.ury, eol);
	    bbox_written = TRUE;
	}
      }
    }
    else {
	ps_fgets(text, sizeof(text), psfile.file);
        fputs(text,f);
	p = text;
	while (*p && (*p != '\r') && (*p != '\n'))
	    p++;	/* find EOL used in DSC file */
	strncpy(eol, p, sizeof(eol)-1);
	fprintf(f, "%%%%BoundingBox: %d %d %d %d%s",
	    bbox.llx, bbox.lly, bbox.urx, bbox.ury, eol);
	position = gfile_get_position(psfile.file);
	ps_copy(f, psfile.file, position, dsc->endcomments);
    }
}

/* Copy the trailer to file f, removing bbox line if present */
void
copy_trailer_without_bbox(FILE *f)
{
    char text[DSC_LINE_LENGTH+1];
    CDSC *dsc = psfile.dsc;

    gfile_seek(psfile.file, dsc->begintrailer, gfile_begin);
    while ( ps_copy_find(f, psfile.file, dsc->endtrailer,
		text, sizeof(text), "%%BoundingBox:") ) {
	/* do NOT copy the bounding box */
    }
}

/* make a PC EPS file */
/* from a PS file and a user supplied preview */
/* preview may be WMF or TIFF */
/* returns 0 on success */
int
make_eps_user(void)
{
char epsname[MAXSTR];
char *buffer;
unsigned int count;
FILE *epsfile;
FILE *preview_file;
char preview_name[MAXSTR];
unsigned long preview_length;
#if defined(__EMX__) || defined (_MSC_VER)
#pragma pack(1)
#endif
struct eps_header_s eps_header;
#if defined(__EMX__) || defined (_MSC_VER)
#pragma pack()
#endif
int type = 0;
#define TIFF 1
#define WMF 2
char id[4];
CDSC *dsc = psfile.dsc;
long end;

	/* get user supplied preview */
#ifdef EPSTOOL
	strncpy(preview_name, upname, MAXSTR-1);
#else
	preview_name[0] = '\0';
	if (!get_filename(preview_name, FALSE, FILTER_ALL, IDS_EPSUSERTITLE, IDS_TOPICPREVIEW))
	    return 1; /* failure */
#endif

	/* open preview, determine length and type */
	preview_file = fopen(preview_name, "rb");
	if (preview_file == (FILE *)NULL) {
	 
	    play_sound(SOUND_ERROR);
	    return 1;
	}

	id[0] = (char)fgetc(preview_file);
	id[1] = (char)fgetc(preview_file);
	id[2] = (char)fgetc(preview_file);
	id[3] = (char)fgetc(preview_file);
	fseek(preview_file, 0, SEEK_END);
	preview_length = ftell(preview_file);
	fseek(preview_file, 0, SEEK_SET);

	if ((id[0] == 'I') && (id[1] == 'I'))
	    type = TIFF;
	if ((id[0] == 'M') && (id[1] == 'M'))
	    type = TIFF;
	if ((id[0] == (char)0x01) && (id[1] == (char)0x00) && (id[2] == (char)0x09) && (id[3] == (char)0x00))
	    type = WMF;
	if ((id[0] == (char)0xd7) && (id[1] == (char)0xcd) && (id[2] == (char)0xc6) && (id[3] == (char)0x9a)) {
	    type = WMF;
	    preview_length -= 22;	/* skip over placeable metafile header */
	    fseek(preview_file, 22, SEEK_SET);
	}

	if (type == 0) {
	    gserror(IDS_EPSUSERINVALID, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	    fclose(preview_file);
	    return 1;
	}

#ifdef EPSTOOL
	strncpy(epsname, oname, MAXSTR-1);
	if (*epsname!='\0')
	    epsfile = fopen(epsname,"wb");
	else
	    epsfile = stdout;
#else
	/* create EPS file */
	epsname[0] = '\0';
	if (!get_filename(epsname, TRUE, FILTER_EPS, 0, IDS_TOPICPREVIEW)) {
	    fclose(preview_file);
	    return 1;
	}
	epsfile = fopen(epsname,"wb");
#endif
	if (epsfile == (FILE *)NULL) {
	    play_sound(SOUND_ERROR);
	    fclose(preview_file);
	    return 1;
	}

	/* write DOS EPS binary header */
	eps_header.id[0] = (char) 0xc5;
	eps_header.id[1] = (char) 0xd0;
	eps_header.id[2] = (char) 0xd3;
	eps_header.id[3] = (char) 0xc6;
	eps_header.ps_begin = EPS_HEADER_SIZE;
	end = psfile.dsc->begincomments;
	if (dsc->endcomments)
	   end = dsc->endcomments;
	if (dsc->enddefaults)
	   end = dsc->enddefaults;
	if (dsc->endprolog)
	   end = dsc->endprolog;
	if (dsc->endsetup)
	   end = dsc->endsetup;
	if (dsc->page_count && dsc->page[0].end)
	    end = dsc->page[0].end;
	if (dsc->endtrailer)
	   end = dsc->endtrailer;
	eps_header.ps_length = end - psfile.dsc->begincomments;
	if (type == WMF) {
	    eps_header.mf_begin = eps_header.ps_begin + eps_header.ps_length;
	    eps_header.mf_length = preview_length;
	    eps_header.tiff_begin = 0;
	    eps_header.tiff_length = 0;
	}
	else {
	    eps_header.mf_begin = 0;
	    eps_header.mf_length = 0;
	    eps_header.tiff_begin = eps_header.ps_begin + eps_header.ps_length;
	    eps_header.tiff_length = preview_length;
	}
	eps_header.checksum = 0xffff;

	write_doseps_header(&eps_header, epsfile);

	ps_copy(epsfile, psfile.file, dsc->begincomments, dsc->endcomments);
	ps_copy(epsfile, psfile.file, dsc->begindefaults, dsc->enddefaults);
	ps_copy(epsfile, psfile.file, dsc->beginprolog, dsc->endprolog);
	ps_copy(epsfile, psfile.file, dsc->beginsetup, dsc->endsetup);
	if (dsc->page_count)
	    ps_copy(epsfile, psfile.file, dsc->page[0].begin, dsc->page[0].end);
	ps_copy(epsfile, psfile.file, dsc->begintrailer, dsc->endtrailer);
	
	/* copy preview file */
	buffer = (char *)malloc(COPY_BUF_SIZE);
	if (buffer == (char *)NULL) {
	    play_sound(SOUND_ERROR);
	    fclose(epsfile);
	    unlink(epsname);
	    fclose(preview_file);
	    return 1;
	}
        while ( (count = fread(buffer, 1, COPY_BUF_SIZE, preview_file)) != 0 )
	    fwrite(buffer, 1, count, epsfile);
	free(buffer);
	fclose(preview_file);
	if (*epsname!='\0')
	   fclose(epsfile);
	return 0; /* success */
#undef TIFF
#undef WMF
}



typedef struct tagMFH {
    WORD type;
    WORD headersize;
    WORD version;
    DWORD size;
    WORD nobj;
    DWORD maxrec;
    WORD noparam;
} MFH;

int metafile_init(unsigned char *pbitmap, BOOL calc_bbox, PSBBOX *pdevbbox, 
    MFH* mf);
void write_bitmap_info(LPBITMAP2 pbmi, int palcount, FILE *f);
int write_metafile(FILE *f, unsigned char *pbitmap, PSBBOX *pdevbbox, MFH *mf);


/* A metafile object must not be larger than 64k */
/* Metafile bitmap object contains metafile header, */
/* bitmap header, palette and bitmap bits */
#define MAX_METAFILE_BITMAP 64000L	/* max size of bitmap bits */

int
metafile_init(unsigned char *pbitmap, BOOL calc_bbox, PSBBOX *pdevbbox, MFH* mf)
{
PREBMAP prebmap;
int wx, wy;
int ny, nylast;
int complete, partial;
int bytewidth;
int palcount;
int code;
unsigned long size;

	if (*pbitmap == 'P')
	    code = scan_pbmplus(&prebmap, pbitmap);
	else {
	    code = scan_dib(&prebmap, pbitmap);
	}
	if (code)
	    return code;

	if (calc_bbox) {
	    scan_bbox(&prebmap, pdevbbox);
	    if (pdevbbox->valid) {
	    	/* copy to global bbox as if obtained by PS to EPS */
	    	bbox.llx = (int)(pdevbbox->llx * 72.0 / option.xdpi - 0.5);
	    	bbox.lly = (int)(pdevbbox->lly * 72.0 / option.ydpi - 0.5);
	    	bbox.urx = (int)(pdevbbox->urx * 72.0 / option.xdpi + 1.5);
	    	bbox.ury = (int)(pdevbbox->ury * 72.0 / option.ydpi + 1.5);
	    	bbox.valid = TRUE;
	    }
	}
	else {
#ifdef EPSTOOL
	    pdevbbox->urx = prebmap.width;
	    pdevbbox->ury = prebmap.height;
	    pdevbbox->llx = pdevbbox->lly = 0;
#else
	    if (display.epsf_clipped || (psfile.dsc->bbox==(CDSCBBOX *)NULL)) {
		/* copy all of display bitmap */
		pdevbbox->urx = prebmap.width;
		pdevbbox->ury = prebmap.height;
		pdevbbox->llx = pdevbbox->lly = 0;
	    }
	    else {
		/* copy only part of display bitmap */
		pdevbbox->llx = (int)(psfile.dsc->bbox->llx * option.xdpi / 72.0);
		pdevbbox->lly = (int)(psfile.dsc->bbox->lly * option.ydpi / 72.0);
		pdevbbox->urx = (int)(psfile.dsc->bbox->urx * option.xdpi / 72.0 + 0.5);
		pdevbbox->ury = (int)(psfile.dsc->bbox->ury * option.ydpi / 72.0 + 0.5);
#define LIMIT(z, limit) if (z < 0) z = 0; else if (z > limit) z = limit
		LIMIT(pdevbbox->llx, prebmap.width);
		LIMIT(pdevbbox->lly, prebmap.height);
		LIMIT(pdevbbox->urx, prebmap.width);
		LIMIT(pdevbbox->ury, prebmap.height);
#undef LIMIT
	    }
#endif
	}

	wx = pdevbbox->urx - pdevbbox->llx;
	wy = pdevbbox->ury - pdevbbox->lly;
        bytewidth = (( wx * prebmap.depth + 31) & ~31) >> 3;
	ny = (int)(MAX_METAFILE_BITMAP / bytewidth);
	if (prebmap.depth == 24)
	    palcount = 0;
	else
	    palcount = 1<<prebmap.depth;

	complete = wy / ny;
	nylast = wy % ny;
	partial = nylast ? 1 : 0;
	
	mf->type = 1;			/* metafile in file */
	mf->headersize = 9;		/* 9 WORDs */
	mf->version = 0x300;		/* Windows 3.0 */
	mf->size = 			/* sizes in WORDs */
	    9UL +			/* header */
	    5 +				/* SetWindowOrg */
	    5; 				/* SetWindowExt */
	/* complete StretchDIBits */
	mf->size += 14*complete;
	size = (40L + palcount*4L + (unsigned long)ny*(unsigned long)bytewidth)/2L;
	mf->size += size * (unsigned long)complete;
	/* partial StretchDIBits */
	mf->size += 14*partial;
	size = (40L + palcount*4L + (unsigned long)nylast*(unsigned long)bytewidth)/2L;
	mf->size += size * (unsigned long)partial;
	mf->size += 3;			/* end marker */

	mf->nobj = 0;
	size = complete ? 
	   (40L + palcount*4L + (unsigned long)ny*(unsigned long)bytewidth)/2L
	 : (40L + palcount*4L + (unsigned long)nylast*(unsigned long)bytewidth)/2L;
	mf->maxrec = 14L + size;
	mf->noparam = 0;
	return 0;
}


/* WARNING: pbmi might not be packed.
 * pbmi->biSize=40 if packed, larger otherwise.
 * The palette will be at offset pbmi->biSize, not BITMAP2_LENGTH
 */
void
write_bitmap_info(LPBITMAP2 pbmi, int palcount, FILE *f)
{
int i;
unsigned char *prgb;
    /* write bitmap info */
    write_dword(BITMAP2_LENGTH, f);
    write_dword(pbmi->biWidth, f);
    write_dword(pbmi->biHeight, f);
    write_word(pbmi->biPlanes, f);
    write_word(pbmi->biBitCount, f);
    write_dword(pbmi->biCompression, f);
    write_dword(pbmi->biSizeImage, f);
    write_dword(pbmi->biXPelsPerMeter, f);
    write_dword(pbmi->biYPelsPerMeter, f);
    write_dword(pbmi->biClrUsed, f);
    write_dword(pbmi->biClrImportant, f);

    prgb = ((BYTE *)pbmi) + pbmi->biSize;
    for (i=0; i<palcount; i++) {
	fputc(*prgb++, f);
	fputc(*prgb++, f);
	fputc(*prgb++, f);
	fputc(*prgb++, f);
    }
}

/* convert the display bitmap to a metafile picture */
int
write_metafile(FILE *f, unsigned char *pbitmap, PSBBOX *pdevbbox, MFH *mf)
{
PREBMAP prebmap;
int i;
int wx;
int ny, sy, dy, wy;
BYTE *line;
BYTE *line2;
LPBITMAP2 pbmi;
int bsize;
int bitoffset;
int bytewidth, activewidth;
int palcount;
int code;
unsigned long size;

	if (*pbitmap == 'P')
	    code = scan_pbmplus(&prebmap, pbitmap);
	else {
	    code = scan_dib(&prebmap, pbitmap);
	}
	if (code)
	    return code;

	dy = 0;
	wx = pdevbbox->urx - pdevbbox->llx;
	sy = pdevbbox->lly;
	wy = pdevbbox->ury - pdevbbox->lly;
	bitoffset = (pdevbbox->llx * prebmap.depth);
        bytewidth = (( wx * prebmap.depth + 31) & ~31) >> 3;
	activewidth = (( wx * prebmap.depth + 7) & ~7) >> 3;
	ny = (int)(MAX_METAFILE_BITMAP / bytewidth);
	if (prebmap.depth == 24)
	    palcount = 0;
	else
	    palcount = 1<<prebmap.depth;

	/* create and initialize a BITMAPINFO for the <64k bitmap */
	bsize = sizeof(BITMAP2) + palcount * sizeof(RGB4); 
	pbmi = (LPBITMAP2)malloc(bsize);
	if (pbmi == NULL) {
	   gserror(0, "not enough memory to copy bitmap", MB_ICONEXCLAMATION, SOUND_ERROR);
	   return 1;
	}
	memset((char *)pbmi, 0, bsize);
	make_bmp_info(pbmi, &prebmap, pbitmap);
	pbmi->biClrUsed = 0;		/* write out full palette */
	pbmi->biClrImportant = 0;

	line2 = (BYTE *)malloc(prebmap.bytewidth);
	if (line2 == (BYTE *)NULL) {
	   gserror(0, "not enough memory to copy bitmap", MB_ICONEXCLAMATION, SOUND_ERROR);
	   return 1;
	}
        pbmi->biWidth = wx;

	if (prebmap.bits) {
	    if (prebmap.topleft)
		line = (BYTE *)prebmap.bits + ((long)prebmap.bytewidth * (prebmap.height - pdevbbox->lly -1));
	    else
		line = (BYTE *)prebmap.bits + ((long)prebmap.bytewidth * (pdevbbox->lly));
	}
	else {
	    free((char *)pbmi);
	    free(line2);
	    return 1;
	}


	/* write metafile header */
	write_word(mf->type, f);
	write_word(mf->headersize, f);
	write_word(mf->version, f);
	write_dword(mf->size, f);
	write_word(mf->nobj, f);
	write_dword(mf->maxrec, f);
	write_word(mf->noparam, f);

	/* write SetWindowOrg */
	write_dword(5, f);
	write_word(0x20b, f);
	write_word(0, f);
	write_word(0, f);

	/* write SetWindowExt */
	write_dword(5, f);
	write_word(0x20c, f);
	write_word((WORD)wy, f);
	write_word((WORD)wx, f);

	/* copy in chunks < 64k */
        for ( ; wy > ny; dy += ny, wy -= ny, sy += ny ) {
	    pbmi->biHeight = ny;

	    size = bsize + (unsigned long)bytewidth * (unsigned long)ny;
	    /* write StretchDIBits header */
	    write_dword(14 + size/2L, f);
	    write_word(0x0f43, f);
	    write_dword(0x00cc0020L, f);	/* SRC_COPY */
	    write_word(0, f);			/* DIB_RGB_COLORS */
	    write_word((WORD)ny, f);		/* Source cy */
	    write_word((WORD)wx, f);		/* Source cx */
	    write_word(0, f);			/* Source y */
	    write_word(0, f);			/* Source x */
	    write_word((WORD)ny, f);		/* Dest   cy */
	    write_word((WORD)wx, f);		/* Dest   cx */
	    write_word((WORD)(pdevbbox->ury - pdevbbox->lly - ny - dy), f); /* Dest   y */
	    write_word(0, f);			/* Dest   x */

	    /* write bitmap header */
	    write_bitmap_info(pbmi, palcount, f);

	    /* write bitmap rows */
	    for (i=0; i<ny; i++) {
	        memmove(line2,  line, prebmap.bytewidth);
		shift_preview(line2, prebmap.bytewidth, bitoffset);
		if (activewidth < bytewidth)
		    memset(line2+activewidth, 0xff, bytewidth-activewidth);
		if ( (*pbitmap == 'P') && (prebmap.depth==24) )
		    reverse_triples(line2, wx);
		fwrite(line2, 1, bytewidth, f);
		if (prebmap.bits) {
		    if (prebmap.topleft)
			line -= prebmap.bytewidth;
		    else
			line += prebmap.bytewidth;
		}
	    }
	    
	}

	/* write StretchDIBits header */
	pbmi->biHeight = wy;
	size = bsize + (unsigned long)bytewidth * (unsigned long)wy;
	write_dword(14 + size/2L, f);
	write_word(0x0f43, f);
	write_dword(0x00cc0020L, f);	/* SRC_COPY */
	write_word(0, f);		/* DIB_RGB_COLORS */
	write_word((WORD)wy, f);	/* Source cy */
	write_word((WORD)wx, f);	/* Source cx */
	write_word(0, f);		/* Source y */
	write_word(0, f);		/* Source x */
	write_word((WORD)wy, f);	/* Dest   cy */
	write_word((WORD)wx, f);	/* Dest   cx */
	write_word((WORD)(pdevbbox->ury - pdevbbox->lly - wy - dy), f);	/* Dest   y */
	write_word(0, f);		/* Dest   x */

	/* write bitmap header */
	write_bitmap_info(pbmi, palcount, f);

	/* copy last chunk */
	for (i=0; i<wy; i++) {
	    memmove(line2,  line, prebmap.bytewidth);
	    shift_preview(line2, prebmap.bytewidth, bitoffset);
	    if (activewidth < bytewidth)
		memset(line2+activewidth, 0xff, bytewidth-activewidth);
	    if ( (*pbitmap == 'P') && (prebmap.depth==24) )
		reverse_triples(line2, wx);
	    fwrite(line2, 1, bytewidth, f);
	    if (prebmap.bits) {
		if (prebmap.topleft)
		    line -= prebmap.bytewidth;
		else
		    line += prebmap.bytewidth;
	    }
	}

	/* write end marker */
	write_dword(3, f);
	write_word(0, f);

        free((char *)pbmi);
	free(line2);

	return 0;
}


/* make a PC EPS file with a Windows Metafile Preview */
/* from a PS file and a bitmap */
int
make_eps_metafile(BOOL calc_bbox, const char *outname)
{
char *buffer;
UINT count;
FILE *outfile;
FILE *tpsfile;
char tpsname[MAXSTR];
struct eps_header_s eps_header;
unsigned char *pbitmap;
PSBBOX devbbox;
MFH mf;
int code;
CDSC *dsc = psfile.dsc;

	if ( (pbitmap = (unsigned char *)get_bitmap()) == (unsigned char *)NULL) {
	    return 1;
	}
	/* scan bbox, make metafile header */
	code = metafile_init(pbitmap, calc_bbox, &devbbox, &mf);
	if (code) {
	    release_bitmap();
	    play_sound(SOUND_ERROR);
	    return code;
	}
	

	if (calc_bbox) {
	    /* we need to copy the psfile to a temporary file */
	    /* because we will be changing the %%BoundingBox line */
	    if ( (tpsfile = gp_open_scratch_file(szScratch, tpsname, "wb")) == (FILE *)NULL) {
		release_bitmap();
		play_sound(SOUND_ERROR);
		return 1;
	    }
	    copy_bbox_header(tpsfile); /* adjust %%BoundingBox: comment */
	    ps_copy(tpsfile, psfile.file, dsc->begindefaults, dsc->enddefaults);
	    ps_copy(tpsfile, psfile.file, dsc->beginprolog, dsc->endprolog);
	    ps_copy(tpsfile, psfile.file, dsc->beginsetup, dsc->endsetup);
	    if (dsc->page_count)
		ps_copy(tpsfile, psfile.file, dsc->page[0].begin, dsc->page[0].end);
	    copy_trailer_without_bbox(tpsfile);
	    fclose(tpsfile);
	    if ( (tpsfile = fopen(tpsname, "rb")) == (FILE *)NULL) {
		release_bitmap();
		play_sound(SOUND_ERROR);
		return 1;
	    }
	}
	else {
	    if ( (tpsfile = fopen(psfile_name(&psfile), "rb"))
	  	== (FILE *)NULL) {
		release_bitmap();
		return 1;
	    }
	}

	/* create buffer for PS file copy */
	buffer = (char *)malloc(COPY_BUF_SIZE);
	if (buffer == (char *)NULL) {
	    release_bitmap();
	    gserror(0, "not enough memory to copy bitmap", MB_ICONEXCLAMATION, SOUND_ERROR);
	    return 1;
	}

#ifdef EPSTOOL
	/* assume outname already exists */
	if (*outname!='\0')
	    outfile = fopen(outname,"wb");
	else
	    outfile = stdout;
#else
	/* create EPS file */
        outfile = fopen(outname,"wb");
#endif
	if (outfile == (FILE *)NULL) {
	    release_bitmap();
	    return 1;
	}

	/* write DOS EPS binary header */
	eps_header.id[0] = (char) 0xc5;	/* "EPSF" with bit 7 set */
	eps_header.id[1] = (char) 0xd0;
	eps_header.id[2] = (char) 0xd3;
	eps_header.id[3] = (char) 0xc6;
	eps_header.ps_begin = EPS_HEADER_SIZE;
	fseek(tpsfile, 0, SEEK_END);
	eps_header.ps_length = ftell(tpsfile);
	eps_header.mf_begin = eps_header.ps_begin + eps_header.ps_length;
	eps_header.mf_length = mf.size*2;
	eps_header.tiff_begin = 0;
	eps_header.tiff_length = 0;
	eps_header.checksum = 0xffff;
	write_doseps_header(&eps_header, outfile);

	/* copy PS file */
        rewind(tpsfile);
	while ( (count = fread(buffer, 1, COPY_BUF_SIZE, tpsfile)) != 0 )
	    fwrite(buffer, 1, count, outfile);
	free(buffer);

	/* copy metafile */
	code = write_metafile(outfile, pbitmap, &devbbox, &mf);

	release_bitmap();

	fclose(tpsfile);
	if (calc_bbox)
	    unlink(tpsname);
	if (*outname!='\0')
	    fclose(outfile);
	return code;
}

