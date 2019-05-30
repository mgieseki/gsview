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

/* gvdoc.c */
/* Preprocess doc file
 *
 * usage:  gvdoc W infile.doc outfile.doc
 *
 *   where infile.doc is a GSview superset of a Gnuplot .DOC file 
 *   supporting conditionals and outfile.doc is a gnuplot style .DOC file.
 *
 * typical usage for GSview:
 *
 *   for Windows:
 *     gvdoc W gvc.doc gsview.doc
 *     doc2rtf gsview.doc gsview.rtf
 *     hc31 gsview.hpj
 *
 *   for OS/2:
 *     gvdoc P gvc.doc gsview.doc
 *     doc2ipf gsview.doc gvpm.ipf
 *     ipfc gvpm.ipf
 *     ipfc /INF gvpm.ipf
 *   for Linux/X11:
 *     gvdoc X gvc.doc gsview.doc
 *     doc2html gvx.txt GSview.htm
 * 
 *     
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME_LEN	256
#define MAX_LINE_LEN	256
#define TRUE 1
#define FALSE 0

typedef int boolean;

char start;
char end = 'E';
boolean ignore = FALSE;
char line[MAX_LINE_LEN];

int
main(int argc, char *argv[])
{
FILE * infile;
FILE * outfile;
	infile = stdin;
	outfile = stdout;
	if (argc != 4) {
		fprintf(stderr,"Usage: %s [W][P][X] infile outfile\n", argv[0]);
		return 1;
	}
	start = argv[1][0];
	if ( (infile = fopen(argv[2],"r")) == (FILE *)NULL) {
		fprintf(stderr,"%s: Can't open %s for reading\n",
			argv[0], argv[2]);
		return 1;
	}
	if ( (outfile = fopen(argv[3],"w")) == (FILE *)NULL) {
		fprintf(stderr,"%s: Can't open %s for writing\n",
			argv[0], argv[3]);
		return 1;
	}
	
	while (fgets(line, MAX_LINE_LEN, infile)) {
	    if (isalpha((int)(line[0]))) {
		if (strchr(line, start) || (line[0] == end))
		    ignore = FALSE;
		else
		    ignore = TRUE;
	    }
	    else if (!ignore)
		fputs(line, outfile);
	}
	fclose(outfile);
	return 0;
}
