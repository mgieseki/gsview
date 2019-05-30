/* mkrch.c */
/* Make resource script header */
/* Created by Russell Lang, 1996-10-11 */

#include <stdio.h>
#include <string.h>

int 
usage(void)
{
    fprintf(stderr, "Usage: mkbundle psfile hdrfile resource_id\n");
    return 1;
}

int
main(int argc, char *argv[])
{
FILE *psfile, *hdrfile;
char inbuf[256], outbuf[256];
char *s, *d;
    if (argc!=4)
	return usage();
    if ( (psfile = fopen(argv[1], "r")) == (FILE *)NULL )
	return usage();
    if ( (hdrfile = fopen(argv[2], "w")) == (FILE *)NULL ) {
	fclose(psfile);
	return usage();
    }
    strcpy(inbuf, argv[1]);
    strtok(inbuf, ".");
    fputs("\n", hdrfile);
#ifdef __EMX__
    fputs("RCDATA ", hdrfile);
    fputs(argv[3], hdrfile);
    fputs("\nBEGIN\n", hdrfile);
#else
    fputs(argv[3], hdrfile);
    fputs(" RCDATA\nBEGIN\n", hdrfile);
#endif
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
	fputs("\042\n", hdrfile);
    }
    fputs("  \042\\0\\0\042\nEND\n\n", hdrfile);
    fclose(psfile);
    fclose(hdrfile);
    return 0;
}
