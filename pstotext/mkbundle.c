/* mkbundle.c */
/* Created by Russell Lang, 1996-10-11 */
/* Updated by Russell Lang, 2004-01-09 to fix compiler warnings */

#include <stdio.h>
#include <string.h>

int 
usage(void)
{
    fprintf(stderr, "Usage: mkbundle psfile hdrfile\n");
    return 1;
}

int
main(int argc, char *argv[])
{
FILE *psfile, *hdrfile;
char inbuf[256], outbuf[256];
char *s, *d;
    if (argc!=3)
	return usage();
    if ( (psfile = fopen(argv[1], "r")) == (FILE *)NULL )
	return usage();
    if ( (hdrfile = fopen(argv[2], "w")) == (FILE *)NULL ) {
	fclose(psfile);
	return usage();
    }
    strcpy(inbuf, argv[1]);
    strtok(inbuf, ".");
    fputs("const char *", hdrfile);
    fputs(inbuf, hdrfile);
    fputs("[] = {\n", hdrfile);
    while ( fgets(inbuf, sizeof(inbuf)-1, psfile) ) {
	d = outbuf;
	for (s=inbuf; *s; s++) {
	    if (*s == '"') {
		*d++ = '\\';
		*d++ = '0';
		*d++ = '4';
		*d++ = '2';
	    }
	    else {
		if (*s != '\n')
		    *d++ = *s;
		else {
		    *d++ = '\\';
		    *d++ = '0';
		    *d++ = '1';
		    *d++ = '2';
		}
	    }
	}
	*d = '\0';
	fputs("  \042", hdrfile);
	fputs(outbuf, hdrfile);
	fputs("\042,\n", hdrfile);
    }
    fputs("  0\n};\n\n", hdrfile);
    fclose(psfile);
    fclose(hdrfile);
    return 0;
}
