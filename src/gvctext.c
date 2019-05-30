/* Copyright (C) 1993-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvctext.c */
/* Text Extract and Search module of PM and Windows GSview */

#include "gvc.h"


/* forward declarations */
void gsview_text_extract_slow(void);
void gsview_text_extract_quick(void);
void gsview_text_findnext_slow(void);
void gsview_text_findnext_quick(void);

char toupper_latin1(char ch);
char tolower_latin1(char ch);
int text_extract_line(char *dest, FILE *inf);
void text_extract(FILE *outf, FILE *inf, unsigned long end);
char * text_find_string(char *str, char *find, int flen);
char * text_find_section(FILE *inf, unsigned long end, char *str);
char * text_grab_word(char *line);
void text_extract_slow(FILE *outfile, FILE *infile, BOOL all);
BOOL text_find_slow(FILE *infile, char *str, BOOL all);
BOOL in_word(PSBBOX *bbox, int x, int y);

char
toupper_latin1(char ch)
{
    if ((ch >= 'a') && (ch <= 'z'))
	return (char)(ch - ('a' - 'A'));
    if ((ch >= '\340') && (ch <= '\377'))   /* Latin-1 accented chars */
	return (char)(ch - ('\340' - '\300'));
    return ch;
}

char
tolower_latin1(char ch)
{
    if ((ch >= 'A') && (ch <= 'Z'))
	return (char)(ch + ('a' - 'A'));
    if ((ch >= '\300') && (ch <= '\337'))   /* Latin-1 accented chars */
	return (char)(ch + ('\340' - '\300'));
    return ch;
}


/* extract text from next line of ps file */
/* return count of characters written to dest */
int
text_extract_line(char *dest, FILE *inf)
{
char linebuf[DSC_LINE_LENGTH];
unsigned int count = 0;
char *p;
char ch;
int instring;
	*dest = '\0';
	if ( (p=fgets(linebuf, sizeof(linebuf), inf)) == (char *)NULL )
	    return 0;
	/* skip over binary sections */
	if (strncmp(linebuf, "%%BeginBinary:",14)==0) {
	    unsigned long lcount = 0;
	    int read;
	    char buf[1024];
	    if (sscanf(linebuf+14, "%ld", &lcount) != 1)
		lcount = 0;
	    while (lcount) {
		read = fread(buf, 1, min(lcount, sizeof(buf)), inf);
		lcount -= read;
		if (read == 0)
		    lcount = 0;
	    }
	    if ( (p=fgets(linebuf, sizeof(linebuf), inf)) == (char *)NULL )
	        return 0;
	}
	if (strncmp(linebuf, "%%BeginData:",12)==0) {
	    unsigned long lcount;
	    int read;
	    char buf[DSC_LINE_LENGTH];
	    if (sscanf(linebuf+12, "%ld %*s %s", &lcount, buf) != 2)
		lcount = 0;
	    if (strncmp(buf, "Lines", 5) == 0) {
		while (lcount) {
		    lcount--;
		    if (fgets(buf, sizeof(buf), inf) == (char *)NULL)
			lcount = 0;
		}
	    }
	    else {
		while (lcount) {
		    read = fread(buf, 1, min(lcount, (unsigned)sizeof(buf)), inf);
		    lcount -= read;
		    if (read == 0)
			lcount = 0;
		}
	    }
	    if ( (p=fgets(linebuf, sizeof(linebuf), inf)) == (char *)NULL )
	        return 0;
	}

	instring = FALSE;
	while ((ch = *p)!='\0') {
	    	if (!instring && (ch=='%'))
	    	    break;	/* comment until EOL */
	    	if (ch == '(') {
	    	    if (instring) {
			*dest++ = ch;
			count++;
		    }
	    	    instring++;
	    	}
	        else if (ch == ')') {
	            instring--;
	    	    if (instring) {
			*dest++ = ch;
			count++;
		    }
	    	    else {
			*dest++ = ' ';  /* may need to be changed */
			count++;
		    }
	        }
	        else if (instring && (ch == '\\')) {
	            ch = *++p;
	            if (ch == '\0') {
	                p--;
	            }
	            else {
			if ((ch != '(') && (ch != ')') && (ch !='\\')) {
			    *dest++ = '\\';
			    count++;
			}
			*dest++ = ch;
			count++;
		    }
	        }
	        else if (instring) {
		    *dest++ = ch;
		    count++;
		}
	    	p++;
	}
	*dest = '\0';
	return count;
}

void 
text_extract(FILE *outf, FILE *inf, unsigned long end)
{
char outline[DSC_LINE_LENGTH];
	while (ftell(inf) < (long)end) {
	    if (text_extract_line(outline, inf)) {
		fputs(outline, outf);
		fputc('\n', outf);
	    }
	}
}


void
gsview_text_extract_quick()
{
    FILE *f;
    FILE *infile;
    static char output[MAXSTR];

    if (!get_filename(output, TRUE, FILTER_TXT, 0, IDS_TOPICTEXT))
	    return;

    if ((f = fopen(output, "w")) == (FILE *)NULL) {
	    return;
    }
    if ( (infile = fopen(psfile_name(&psfile), "rb")) == (FILE *)NULL ) {
	fclose(f);
	return;
    }

    info_wait(IDS_WAITWRITE);
    if (psfile.dsc == (CDSC *)NULL) {
	/* scan whole document */
	unsigned long end;
	fseek(infile, 0L, SEEK_END);
	end = ftell(infile);
	fseek(infile, 0L, SEEK_SET);
	text_extract(f, infile, end);
	dfclose();
    }
    else {
      if (psfile.dsc->page_count != 0) {
	int i;
	for (i = 0; i < (int)psfile.dsc->page_count; i++) {
	    if (psfile.page_list.select[map_page(i)])  {

		fseek(infile, psfile.dsc->page[map_page(i)].begin, SEEK_SET);
		text_extract(f, infile, psfile.dsc->page[map_page(i)].end);
		fputc('\f', f);
		fputc('\n', f);
	    }
	}
      }
      else {
	long end = psfile.dsc->begincomments;
	fseek(infile, psfile.dsc->begincomments, SEEK_SET);
	if (psfile.dsc->endcomments)
	   end = psfile.dsc->endcomments;
	if (psfile.dsc->enddefaults)
	   end = psfile.dsc->enddefaults;
	if (psfile.dsc->endprolog)
	   end = psfile.dsc->endprolog;
	if (psfile.dsc->endsetup)
	   end = psfile.dsc->endsetup;
	if (psfile.dsc->endtrailer)
	   end = psfile.dsc->endtrailer;
	text_extract(f, infile, end);
      }
    }

    fclose(infile);
    fclose(f);

    info_wait(IDS_NOWAIT);
    return;
}



void
gsview_text_extract()
{
    int thispage = psfile.pagenum;
    if (psfile.ispdf && (option.pstotext == 0)) {
	TCHAR buf[MAXSTR];
	load_string(IDS_NOPDFQUICKTEXT, buf, sizeof(buf)-1);
	message_box(buf, 0);
	return;
    }
    if (psfile.name[0] == '\0') {
	gserror(IDS_NOTOPEN, NULL, MB_ICONEXCLAMATION, SOUND_NOTOPEN);
	return;
    }

    if (pstotextOutfile != (FILE *)NULL) {
	play_sound(SOUND_BUSY);
	return;	/* busy creating text index file */
    }

    nHelpTopic = IDS_TOPICTEXT;
    if ((psfile.dsc != (CDSC *)NULL) && (psfile.dsc->page_count != 0))
	if (!get_page(&thispage, TRUE, FALSE))
	    return;

    if (option.pstotext == 0) {
	if (dfreopen() != 0)
	    return;
	gsview_text_extract_quick();
	dfclose();
    }
    else {
	/* check if text_name exists, create if necessary */
	if (psfile.text_name[0] == '\0') {
	    pending.text = TRUE;
	    pending.now = TRUE;
	    /* set flag to come back here after text_name created */
	    psfile.text_extract = TRUE;
	    return;
	}
	else {
	    gsview_text_extract_slow();
	}
    }
}



char *
text_find_string(char *str, char *find, int flen)
{
char *p, *last;
int mcount = 0;
	last = p = str;
	while (*p) {
	    if (*p == ' ') {
		p++;
		continue;	/* ignore white space */
	    }
	    if (mcount) {
	        if (toupper_latin1(*p) == find[mcount])
		    mcount++;	/* matched one more character */
		else {
		    mcount = 0;
		    p = last+1;	/* retrace to just past last partial match */
		}
	    }
	    else {
		if (toupper_latin1(*p) == *find) {
		    last = p;
		    mcount++;	/* start of partial match */
		}
	    }
	    if (mcount == flen)
		return last;
	    p++;
	}
	return (char *)NULL;
}

/* if str found return malloc'd string containing match */
char *
text_find_section(FILE *inf, unsigned long end, char *str)
{
char dbuf[DSC_LINE_LENGTH+DSC_LINE_LENGTH];
char sbuf[DSC_LINE_LENGTH/4];
int dlength;
int slength;
int count;

	/* copy str to uppercase, removing spaces */
	slength = 0;
	for (count=0; str[count]; count++) {
	    if (slength > DSC_LINE_LENGTH/4)
		return NULL;
	    if (str[count] != ' ')			/* ignore spaces */
	        sbuf[slength++] = (char)toupper_latin1(str[count]);	/* searches are case insensitive */
	}
	sbuf[slength] = '\0';
	if (slength==0)
	    return NULL;
	dlength = 0;
	while (ftell(inf) < (long)end) {
	    while ((ftell(inf) < (long)end) && (dlength < DSC_LINE_LENGTH)) {
		count = text_extract_line(dbuf+dlength, inf);
	    	dlength += count;
		if (count) { /* separate lines by spaces */
		    dbuf[dlength++] = ' ';
		    dbuf[dlength] = '\0';
		}
	    }
	    if (text_find_string(dbuf, sbuf, slength)) {
		str = (char *)malloc(dlength+1);
		if (str)
		    strcpy(str, dbuf);
		return str;
	    }
	    if (dlength > slength) {
	       memmove(dbuf, dbuf+dlength-slength, slength+1);
	       dlength = slength;
	    }
	    else 
	    	dlength = 0;
        }
        return NULL;
}


void
gsview_text_find()
{
TCHAR prompt[MAXSTR];		/* input dialog box prompt and message box string */
char answer[MAXSTR];		/* input dialog box answer string */
int thispage = psfile.pagenum;
    /* can relax some of these restrictions if don't display */
    if (not_dsc())
	return;
    if (psfile.ispdf && (option.pstotext == 0)) {
	TCHAR buf[MAXSTR];
	load_string(IDS_NOPDFQUICKTEXT, buf, sizeof(buf)-1);
	message_box(buf, 0);
	return;
    }
    if (order_is_special())
	return;
    if (pstotextOutfile != (FILE *)NULL) {
	play_sound(SOUND_BUSY);
	return;	/* busy creating text index file */
    }
    if (psfile.dsc->page_count == 0) {
	gserror(IDS_NOPAGE, NULL, MB_ICONEXCLAMATION, SOUND_NONUMBER);
	return;
    }
    load_string(IDS_TEXTFIND, prompt, sizeof(prompt));
    strcpy(answer, szFindText);
    nHelpTopic = IDS_TOPICTEXT;

    if (!query_string(prompt,answer))
	return;
    strcpy(szFindText, answer);
    if (!get_page(&thispage, TRUE, TRUE))	/* search all pages */
	    return;

    if (option.pstotext == 0) {
	gsview_text_findnext_quick();
    }
    else {
	psfile.text_offset = 0;
	psfile.text_page = 0;
	/* check if text_name exists, create if necessary */
	if (psfile.text_name[0] == '\0') {
	    pending.text = TRUE;
	    pending.now = TRUE;
	    /* set flag to come back here after text_name created */
	    psfile.text_extract = FALSE;
	    return;
	}
	else {
	    gsview_text_findnext_slow();
	}
    }
}

void
gsview_text_findnext()
{
    if (option.pstotext == 0)
	gsview_text_findnext_quick();
    else
	gsview_text_findnext_slow();
}

void
gsview_text_findnext_quick()
{
int i;
char *p;
FILE *infile;
    if (not_dsc())
	return;
    if (psfile.ispdf && (option.pstotext == 0)) {
	TCHAR buf[MAXSTR];
	load_string(IDS_NOPDFQUICKTEXT, buf, sizeof(buf)-1);
	message_box(buf, 0);
	return;
    }
    if (strlen(szFindText)==0) {
	gserror(IDS_TEXTNOTFIND, NULL, MB_ICONEXCLAMATION, 0);
	return;
    }
    if ( (infile = fopen(psfile_name(&psfile), "rb")) == (FILE *)NULL )
	return;

    info_wait(IDS_WAITSEARCH);
    for (i = 0; i < (int)psfile.dsc->page_count; i++) {
	if (psfile.page_list.select[map_page(i)])  {
	    psfile.page_list.select[map_page(i)] = FALSE;
	    fseek(infile, psfile.dsc->page[map_page(i)].begin, SEEK_SET);
	    p = text_find_section(infile, 
		psfile.dsc->page[map_page(i)].end, szFindText);
	    if (p) {	/* found it */
		info_wait(IDS_NOWAIT);
		free(p);
		dfclose();
		request_mutex();
		pending.pagenum = i+1;
		history_add(pending.pagenum);
		pending.now = TRUE;
		release_mutex();
		return;
	    }
	}
    }
    fclose(infile);
    info_wait(IDS_NOWAIT);
    gserror(IDS_TEXTNOTFIND, NULL, MB_ICONEXCLAMATION, 0);
}


/***************************************************/
/* text extraction and searching based on pstotext */

/* This code assumes that pstotext has produced an output file
 * named psfile.text_name.
 * This file contains one line per word in the following format
 *   llx lly urx ury word 
 * A new line is identified by a blank line 
 * A new page is identified by a formfeed/newline
 *
 */

char *
text_grab_word(char *line)
{
char *p, *q;
    p = line;
    while (*p && ((*p == ' ') || (*p == '\t')))
      p++;
    /* skip llx */
    while (*p && !((*p == ' ') || (*p == '\t')))
      p++;
    while (*p && ((*p == ' ') || (*p == '\t')))
      p++;
    /* skip lly */
    while (*p && !((*p == ' ') || (*p == '\t')))
      p++;
    while (*p && ((*p == ' ') || (*p == '\t')))
      p++;
    /* skip urx */
    while (*p && !((*p == ' ') || (*p == '\t')))
      p++;
    while (*p && ((*p == ' ') || (*p == '\t')))
      p++;
    /* skip ury */
    while (*p && !((*p == ' ') || (*p == '\t')))
      p++;
    while (*p && ((*p == ' ') || (*p == '\t')))
      p++;
    /* remove trailing newline */
    q = p;
    while (*q) {
	if ( (*q == '\r') || (*q == '\n') )
	    *q = '\0';
	else
	    q++;
    }
    return p;
}

void
text_extract_slow(FILE *outfile, FILE *infile, BOOL all)
{
    char line[2048];
    int page = 0;
    BOOL thispage;
    char *p;

    /* pages are in correct order - no reversal needed */
    thispage = (all || psfile.page_list.select[page]);
    while (fgets(line, sizeof(line)-1, infile)) {
	if (thispage) {
	    if ( (*line == '\r') || (*line == '\n') || (*line == '\f') )
		fputs(line, outfile);
	    else {
		p = text_grab_word(line);
		fputs(p, outfile);
		fputc(' ', outfile);
	    }
	}
	if (*line == '\f') {
	    page++;
	    thispage = (all || psfile.page_list.select[page]);
	}
    }
}

/* extract text from a range of pages */
void
gsview_text_extract_slow()
{
    FILE *f;
    FILE *infile;
    static char output[MAXSTR];

    if (!get_filename(output, TRUE, FILTER_TXT, 0, IDS_TOPICTEXT))
	return;

    if ((f = fopen(output, "wb")) == (FILE *)NULL)
	return;

    if (psfile.text_name[0] == '\0')
	return;

    /* Must read in binary mode, since TeX can use ^Z as a character code. */
    if ((infile = fopen(psfile.text_name, "rb")) == (FILE *)NULL) {
	message_box(TEXT("pstotext text extraction file is missing"), 0);
	return;
    }

    info_wait(IDS_WAITWRITE);

    if (psfile.dsc == (CDSC *)NULL) {
	text_extract_slow(f, infile, TRUE);	/* all pages */
    }
    else {
	if (psfile.dsc->page_count != 0) {
	    text_extract_slow(f, infile, FALSE);	/* selected pages */
	}
	else {
	    text_extract_slow(f, infile, TRUE);	/* all pages */
	}
    }

    fclose(f);
    fclose(infile);

    info_wait(IDS_NOWAIT);
    return;
}

/* compare string w with string s */
/* case insensitive */
/* w may contain wildcards * and ? in any position */
/* return TRUE if match */
BOOL
wildmatch(const char *w, const char *s)
{
const char *lastw = "";	/* location of last possible '*' */
const char *lasts = s;	/* location of character after the last matched by '*' */
BOOL nomatch;
    while (*s && *w) {
	nomatch = FALSE;
	if (*w == '*') {
	    lastw = w;
	    w++;		/* look at character after '*' */
	    while (*w && (*w == '*'))
		w++;		/* '**' is same as '*' */
	    if (*w == '\0')
		return TRUE;	/* matches all until end of word */
	    while (*s) {
		if (*w == '?') {
		    break;	/* '*?' is same as '?' */
		}
		else if (tolower_latin1(*w) == tolower_latin1(*s)) {
		    lasts = s;	/* remember location in case we need to extend wildcard match */
		    break;	/* break loop when match found */
		}
		s++;
	    }
	    if (*s == '\0')
		break;		/* end of s without matching next in w */
	}
	else if (*w != '?') {  /* '?' always matches */
	    if (tolower_latin1(*w) != tolower_latin1(*s))
		nomatch = TRUE;
	}
	if (nomatch && *lastw && *lastw=='*' && *lasts) {
	    /* try extending the length matched by the '*' wildcard */
	    w = lastw;
	    s = lasts + 1;
	}
	else if (nomatch) {
	    break;
	}
	else {
	    w++;
	    s++;
	}
    }

    /* skip over trailing '*' */
    while (*w && (*w == '*'))
	w++;		/* '**' is same as '*' */

    if ((*s == '\0') && (*w == '\0'))
	return TRUE;		/* at end of both strings, so matched */

    return FALSE;
}

#define MAX_FIND_WORD 10

/* returns TRUE if match found */
/* infile is text file from pstotext */
/*   is assumed to have been positioned by fseek */
/* str is string to match */
/* all is TRUE if all pages to be searched */
BOOL
text_find_slow(FILE *infile, char *str, BOOL all)
{
char line[MAXSTR];
char *lineword;
char sbuf[MAXSTR];
const char *strword;
BOOL thispage;
char *words[MAX_FIND_WORD+1];
int i;
    /* break up search string into words */
    strcpy(sbuf, str);
    words[0] = strtok(sbuf, " ");
    i = 1;
    while ( ((words[i] = strtok(NULL, " ")) != NULL) && (i<MAX_FIND_WORD) )
	i++;

    /* point at first word */
    i = 0;
    strword = words[i];

    thispage = (all || psfile.page_list.select[psfile.text_page]);
    while (fgets(line, sizeof(line)-1, infile)) {
	if (thispage && (*line != '\n') && (*line != '\f')) {
	    /* get start of word */
	    lineword = text_grab_word(line);
	    if (wildmatch(strword, lineword)) {
		/* matched first word */
		psfile.text_offset = ftell(infile); /* remember location */
		/* remember bounding box of word */
		sscanf(line, "%d %d %d %d",
		   &psfile.text_bbox.llx, &psfile.text_bbox.lly, 
		   &psfile.text_bbox.urx, &psfile.text_bbox.ury);
		/* now check remaining words */
    		strword = words[++i];
		while (strword) {
		    if (!fgets(line, sizeof(line)-1, infile))
			return FALSE;	/* match failed at EOF */
		    if ((*line != '\n') && (*line != '\f')) {
			lineword = text_grab_word(line);
			if (!wildmatch(strword, lineword)) {
			    /* partial match failed */
			    /* restart search from next word */
			    fseek(infile, psfile.text_offset, SEEK_SET);
			    i = 0;
			    strword = words[i];
			    break;
			}
			strword = words[++i];
		        /* extend bbox ?? */
		    }
		}
		/* all words matched */
		if (!strword)
		    return TRUE;
	    }
	}
	if (*line == '\f') {
	    psfile.text_page++;
	    thispage = (all || psfile.page_list.select[psfile.text_page]);
	}
    }
    return FALSE;	/* reached EOF without match */
}

void
gsview_text_findnext_slow()
{
FILE *infile;
char find_text[MAXSTR];
	if (not_dsc())
	    return;
	if (strlen(szFindText)==0) {
	    gserror(IDS_TEXTNOTFIND, NULL, MB_ICONEXCLAMATION, 0);
	    return;
	}

	if (psfile.text_name[0] == '\0') {
	    gserror(IDS_TEXTNOTFIND, NULL, MB_ICONEXCLAMATION, 0);
	    return;
	}

	if ((infile = fopen(psfile.text_name, "rb")) == (FILE *)NULL) {
	    message_box(TEXT("pstotext text extraction file is missing"), 0);
	    return;
	}
	
	/* add wildcard to begin and end */
	if ((szFindText[0] == '*') || (szFindText[0] == '?'))
	    strcpy(find_text, szFindText);
	else {
	    strcpy(find_text, "*");
	    strcat(find_text, szFindText);
	}
	if ( (find_text[strlen(find_text)-1] != '*') &&
	     (find_text[strlen(find_text)-1] != '?') )
	    strcat(find_text, "*");

	info_wait(IDS_WAITSEARCH);
	fseek(infile, psfile.text_offset, SEEK_SET);
	if (text_find_slow(infile, find_text, FALSE)) {
	    /* found it */
	    fclose(infile);
	    info_wait(IDS_NOWAIT);
	    /* signal that BBOX is valid and should be highlighted */
	    if ((psfile.pagenum == psfile.text_page+1) 
		&& (gsdll.state == GS_PAGE)) {
		/* on correct page */
		display.show_find = TRUE;
		scroll_to_find();
	        post_img_message(WM_GSSYNC, 0); /* redraw */
	    }
	    else {
		/* move to correct page */
		request_mutex();
		psfile.text_bbox.valid = TRUE;
		pending.pagenum = psfile.text_page+1;
		history_add(pending.pagenum);
		pending.now = TRUE;
		release_mutex();
		/* scroll_to_find occurs after page is displayed */
	    }
	    return;
	}

	fclose(infile);
        info_wait(IDS_NOWAIT);
	gserror(IDS_TEXTNOTFIND, NULL, MB_ICONEXCLAMATION, 0);
}


/* mouse text selection */


#define TEXT_INDEX_CHUNK 1000
TEXTINDEX *text_index;
unsigned int text_index_count;	/* number of words in index */
unsigned int text_index_size;	/* number of words storage allocated */
char *text_words;
unsigned int text_words_count;	/* location for next word */
unsigned int text_words_size;	/* maximum character count per page */

void
free_text_index(void)
{
    if (text_index)
	free(text_index);
    text_index = NULL;
    text_index_count = 0;
    text_index_size = 0;
    if (text_words)
        free(text_words);
    text_words = NULL;
    text_words_count = 0;
    text_words_size = 0;
    return;
}

BOOL 
make_text_index(void)
{
FILE *f;
int thispage;
int thisline;
char buf[2048];
TEXTINDEX *text;
int page = psfile.pagenum;

    free_text_index();

    if (psfile.text_name[0] == '\0')
	return FALSE;
    
    text_words_size = TEXT_INDEX_CHUNK * 8;
    text_words = (char *)malloc(text_words_size);
    if (text_words == (char *)NULL) {
	free_text_index();
	return FALSE;
    }
    text_words_count = 0;

    text_index_size = TEXT_INDEX_CHUNK;
    text_index = (TEXTINDEX *)malloc(text_index_size * sizeof(TEXTINDEX));
    if (text_index == (TEXTINDEX *)NULL) {
	free_text_index();
	return FALSE;
    }
    text_index_count = 0;

    /* find page */
    f = fopen(psfile.text_name, "rb");
    if (f == (FILE *)NULL) {
	free_text_index();
	return FALSE;
    }
    thispage = 1;
    while ((thispage != page) && fgets(buf, sizeof(buf), f))
	if (*buf == '\f') 
	    thispage++;
    if (thispage != page) {
	free_text_index();
	fclose(f);
	return FALSE;
    }
    thisline = 0;
    while ((thispage == page) && fgets(buf, sizeof(buf), f))  {
	if (*buf == '\f') {
	    thispage++;
	}
	else if (*buf == '\n') {
	    thisline++;
	}
	else {
	    if (text_index_count >= text_index_size) {
		text_index_size += TEXT_INDEX_CHUNK;
		text_index = (TEXTINDEX *)realloc(text_index, 
			text_index_size * sizeof(TEXTINDEX));
		if (text_index == (TEXTINDEX *)NULL) {
		    free_text_index();
		    fclose(f);
		    return FALSE;
		}
	    }
	    text = &text_index[text_index_count];
	    if (text_words_count + strlen(buf) + 1 > text_words_size) {
		text_words_size += TEXT_INDEX_CHUNK * 8;
		text_words = (char *)realloc(text_words, text_words_size);
		if (text_words == (char *)NULL) {
		    free_text_index();
		    fclose(f);
		    return FALSE;
		}
	    }
	    text->word = text_words_count;
	    sscanf(buf, "%d %d %d %d %s",
		       &text->bbox.llx, &text->bbox.lly,
		       &text->bbox.urx, &text->bbox.ury,
		       text_words + text->word);
	    text->line = thisline;
	    text_words_count += strlen(text_words + text_words_count) + 1;
	    text_index_count++;
	}
    }
    fclose(f);
    return TRUE;
}

/* return TRUE if x,y is in bbox */
BOOL
in_word(PSBBOX *bbox, int x, int y)
{
    return ( (bbox->llx <= x) && (x <= bbox->urx)
	  && (bbox->lly <= y) && (y <= bbox->ury) );
}

/* search for word containing x,y */
/* if found, return pointer */
/* if no match, return NULL */
int
word_find(int x, int y)
{
unsigned int i;
    for (i=0; i<text_index_count; i++)
	if (in_word(&text_index[i].bbox, x, y))
	    return i;
    return -1;
}

