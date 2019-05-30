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

/* echogsv.c */
/* A subset of the Ghostscript echogs.c */

/*
 * -w file  write to file
 * -a file  append to file
 * -x XX   hexadecimal character 
 * -n      don't append linefeed
 * -       treat next argument as string
 * other   string
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int omit_nl = 0;
FILE *f;

int main(int argc, char *argv[])
{
    int i;
    char *p;
    f = stdout;
    for (i=1; i<argc; i++) {
	p = argv[i];
	if (strcmp(p, "-w")==0) {
	    i++;
	    p = argv[i];
	    if (i < argc)
		f = fopen(p, "w");
	    continue;
	}
	else if (strcmp(p, "-a")==0) {
	    i++;
	    p = argv[i];
	    if (i < argc)
		f = fopen(p, "a");
	    continue;
	}
	else if (strcmp(p, "-x")==0) {
	    i++;
	    p = argv[i];
	    if (i < argc) {
		int ch;
		int val;
		int alphaoffset = 'A' - '9' - 1;
		ch = *p++;
		if (isalpha(ch))
		    ch = toupper(ch);
		ch -= '0';
		if (ch > 9)
		    ch -= alphaoffset;
		val = ch<<4;
		ch = *p;
		if (isalpha(ch))
		    ch = toupper(ch);
		ch -= '0';
		if (ch > 9)
		    ch -= alphaoffset;
		val |= ch;
		fputc(val, f);
	    }
	    continue;
	}
	else if (strcmp(p, "-n")==0) {
	    if (i+1 < argc)
		omit_nl = 1;
	    continue;
	}
	else if (strcmp(p, "-")==0) {
	    /* next argument literal, even if it starts with - */
	    if (i+1 < argc) {
		i++;
		fputs(argv[i], f);
	    }
	}
	else {
	   /* literal */
	    fputs(p, f);
	}
    }
    if (!omit_nl)
	fputs("\n", f);
    if (f != stdout)
	fclose(f);
    return 0;
}
