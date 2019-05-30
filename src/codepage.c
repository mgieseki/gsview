/* Copyright (C) 2001-2003, Ghostgum Software Pty Ltd.  All rights reserved.
 
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
/* codepage.c */

/* Convert between code pages.  This is quick to go from a codepage
 * to Unicode, but slower from Unicode to a codepage.
 * Assumes that all code pages have ASCII from 0-127.
 */

#include <stdio.h>
#include <string.h>
#ifdef _MSC_VER
#include <io.h>
#include <fcntl.h>
#endif

#include "gvccp.h"

int main(int argc, char *argv[])
{
    int i, j;
    FILE *infile;
    unsigned short *in_cp;
    int in_ucs2 = 0;
    int in_utf8 = 0;
    FILE *outfile;
    unsigned short *out_cp;
    int out_ucs2 = 0;
    int out_utf8 = 0;
    unsigned short uch;
    int ch;
    unsigned char chbuf[3];
    int chcnt;

    in_cp = NULL;
    out_cp = NULL;

    if (argc == 5) {
	for (i=0; cptable[i].name; i++) {
	    if (strcmp(argv[1], cptable[i].name)==0) {
		in_cp = cptable[i].table;
		in_ucs2 = cptable[i].ucs2;
		in_utf8 = cptable[i].utf8;
		break;
	    }
	}

	for (i=0; cptable[i].name; i++) {
	    if (strcmp(argv[2], cptable[i].name)==0) {
		out_cp = cptable[i].table;
		out_ucs2 = cptable[i].ucs2;
		out_utf8 = cptable[i].utf8;
		break;
	    }
	}
    }
   
    if ((in_cp == NULL) || (out_cp == NULL) || (argc != 5)) {
	fprintf(stderr, "Usage: codepage in_cp out_cp in_file out_file\n");
	fprintf(stderr, "in_cp and out_cp are one of the following codepages:\n");
	for (i=0; cptable[i].name; i++) {
	    j = 8 - strlen(cptable[i].name);
	    fprintf(stderr, "%s", cptable[i].name);
	    while (j>0) {
		fputc(' ', stderr);
		j--;
	    }
	    fprintf(stderr, "%s\n", cptable[i].description);
	}
	if (in_cp == NULL)
	   fprintf(stderr, "Input codepage is invalid\n");
	if (out_cp == NULL)
	   fprintf(stderr, "Output codepage is invalid\n");
	return 1;
    }

    infile = fopen(argv[3], "rb"); 
    if (infile == NULL) {
	fprintf(stderr, "Can't open input file '%s'\n", argv[3]);
	return 1;
    }
   
   
    outfile = fopen(argv[4], "wb"); 
    if (outfile == NULL) {
	fprintf(stderr, "Can't open output file '%s'\n", argv[4]);
	fclose(infile);
	return 1;
    }


#ifdef _MSC_VER
    _setmode(fileno(infile), _O_BINARY);
    _setmode(fileno(outfile), _O_BINARY);
#endif

    if (out_ucs2) {
	fputc(0xff, outfile);
	fputc(0xfe, outfile);
    }
    if (in_ucs2) {
	ch = fgetc(infile);
	ch = fgetc(infile);
    }
    while ((ch = fgetc(infile)) != EOF) {
	/* Convert to one UCS-2 character */
	if (in_ucs2) {
	    uch = (unsigned short)ch;
    	    if ((ch = fgetc(infile)) != EOF)
		uch |= (ch << 8);
	}
	else if (in_utf8) {
	    if ((ch & 0x80) == 0)
	        uch = (unsigned short)ch;
	    else {
		/* multibyte UTF-8 character */
		if ((ch & 0xe0) == 0xc0)
		   chcnt = 2;
		else if ((ch & 0xf0) == 0xe0)
		   chcnt = 3;
		else
		   chcnt = 0;	/* can't be put in UCS-2 */
		chbuf[0] = (unsigned char)ch;
		for (i=1; i<chcnt; i++) {
    	            ch = fgetc(infile);
		    if (ch != EOF)
			chbuf[i] = (unsigned char)ch;
		    else
			chbuf[i] = '0';
		}
		utf8_to_ucs2(&uch, 1, chbuf, chcnt);
	    }
	}
	else {
	    uch = in_cp[(unsigned int)ch];
	}

	if (out_ucs2) {
	    fputc(uch & 0xff, outfile);
	    fputc((uch>>8) & 0xff, outfile);
	}
	else if (out_utf8) {
	    chcnt = ucs2_to_utf8(chbuf, sizeof(chbuf), &uch, 1);
	    for (i=0; i<chcnt; i++)
		fputc(chbuf[i], outfile);
	}
	else {
	    fputc(tocp(out_cp, uch), outfile);
	}
    }
    fclose(infile);
    fclose(outfile);
    return 0;
}
