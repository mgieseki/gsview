/* pstotxtd.c */
/* OS/2 and Win32 Command line interface to pstotxt[23].dll */
/* 8086 MS-DOS command line EXE. */
/* Russell Lang */

/* derived from main.c */
/* Copyright (C) 1995, Digital Equipment Corporation.         */
/* All rights reserved.                                       */
/* See the file pstotext.txt for a full description.          */
/* Last modified on Fri Jan  9 21:11:00 AEST 2004 by rjl      */
/*      modified on Sat Mar 11 09:16:00 AEST 2000 by rjl      */
/*      modified on Fri Oct 16 16:30:54 PDT 1998 by mcjones   */
/*      modified on Thu Nov 16 13:33:13 PST 1995 by deutsch   */

/* Modifications by rjl
 *  Fixed compiler warnings.
 */

#ifndef MSDOS
#ifdef _Windows
#include <windows.h>
#include <io.h>
#ifndef __BORLANDC__
#define mktemp(t) _mktemp(t)
#endif
#else
#define INCL_DOS
#include <os2.h>
#endif
#endif

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef MSDOS
#include <io.h>
#include <ctype.h>
#include "bundle.h"
#include "ocr.h"
#include "rot270.h"
#include "rot90.h"
typedef int HMODULE;
#endif

#include "ptotdll.h"


#define BOOLEAN int
#define FALSE 0
#define TRUE 1

#define LINELEN 2000 /* longest allowable line from gs ocr.ps output */

/* resource IDs for pstotxt3.dll */
#define OCR_PROLOG 1
#define ROT270_PROLOG 2
#define ROT90_PROLOG 3

static int cleanup(void);
static void do_it(char *path);
#define strcasecmp stricmp
#define MAXPATHLEN 256
static int debug = FALSE;
static int cork = FALSE;
#ifdef MSDOS
static char *gscommand = "gs386.exe";
#else
#ifdef _Windows
static char *gscommand = "gswin32c.exe";
#else
static char *gscommand = "gsos2.exe";
#endif
#endif
static char *outfile = "";

static char *cmd; /* = argv[0] */

static enum {
  portrait,
  landscape,
  landscapeOther} orientation = portrait;

static int bboxes = FALSE;

static int explicitFiles = 0; /* count of explicit file arguments */

void usage(void) {
  fprintf(stderr, "pstotext 1.8i of 2003-01-08\n");
  fprintf(stderr, "Copyright (C) 1995-1998, Digital Equipment Corporation.\n");
  fprintf(stderr, "Modified by Ghostgum Software Pty Ltd for Ghostscript 6.0.\n");
  fprintf(stderr, "Comments to {mcjones,birrell}@pa.dec.com.\n\n");
  fprintf(stderr, "Usage: %s [option|file]...\n", cmd);
  fprintf(stderr, "Options:\n");
  fprintf(stderr, "  -cork            Assume Cork encoding for dvips output\n");
  fprintf(stderr, "  -landscape       rotate 270 degrees\n");
  fprintf(stderr, "  -landscapeOther  rotate 90 degrees\n");
  fprintf(stderr, "  -portrait        don't rotate (default)\n");
  fprintf(stderr, "  -bboxes          output one word per line with bounding box\n");
  fprintf(stderr, "  -debug           show Ghostscript output and error messages\n");
  fprintf(stderr, "  -gs \042command\042    Ghostscript command\n");
  fprintf(stderr, "  -output file     output results to \042file\042 (default is stdout)\n");
  fprintf(stderr, "  -                read from stdin (default if no files specified)\n");
}

#ifndef _Windows
#define WINAPI 		/* nothing for OS/2 or MSDOS */
#endif

typedef int (WINAPI *PFN_pstotextInit)(void **instance);
typedef int (WINAPI *PFN_pstotextFilter)(void *instance, char *instr, 
    char **pre, char **word, char **post,
    int *llx, int *lly, int *urx, int *ury);
typedef int (WINAPI *PFN_pstotextExit)(void *instance);
typedef int (WINAPI *PFN_pstotextSetCork)(void *instance, int value);
HMODULE pstotextModule;
void *pstotextInstance;
PFN_pstotextInit dllfn_pstotextInit;
PFN_pstotextFilter dllfn_pstotextFilter;
PFN_pstotextExit dllfn_pstotextExit;
PFN_pstotextSetCork dllfn_pstotextSetCork;

#ifdef _Windows
int
load_pstotext(void)
{
char dllname[256];
char *p;
    /* get path to EXE */
    GetModuleFileName(0, dllname, sizeof(dllname));
    if ((p = strrchr(dllname,'\\')) != (char *)NULL)
	p++;
    else
	p = dllname;
    *p = '\0';
#ifdef __WIN32__
#ifdef DECALPHA
    strcat(dllname, "pstotxta.dll");
#else
    strcat(dllname, "pstotxt3.dll");
#endif
#else
    strcat(dllname, "pstotxt1.dll");
#endif
    if (debug) {
	fputs(dllname, stdout);
	fputc('\n', stdout);
    }


    /* load pstotext DLL */
    pstotextModule = LoadLibrary(dllname);
    if (pstotextModule < (HINSTANCE)HINSTANCE_ERROR) {
	fprintf(stderr, "Can't load %s\n", dllname);
	return 1;
    }
    dllfn_pstotextInit = (PFN_pstotextInit) GetProcAddress(pstotextModule, "pstotextInit");
    if (dllfn_pstotextInit == (PFN_pstotextInit)NULL) {
	fprintf(stderr, "Can't find pstotextInit() in %s\n", dllname);
	FreeLibrary(pstotextModule);
	return 1;
    }
    dllfn_pstotextFilter = (PFN_pstotextFilter) GetProcAddress(pstotextModule, "pstotextFilter");
    if (dllfn_pstotextFilter == (PFN_pstotextFilter)NULL) {
	fprintf(stderr, "Can't find pstotextFilter() in %s\n", dllname);
	FreeLibrary(pstotextModule);
	return 1;
    }
    dllfn_pstotextExit = (PFN_pstotextExit) GetProcAddress(pstotextModule, "pstotextExit");
    if (dllfn_pstotextExit == (PFN_pstotextExit)NULL) {
	fprintf(stderr, "Can't find pstotextExit() in %s\n", dllname);
	FreeLibrary(pstotextModule);
	return 1;
    }
    dllfn_pstotextSetCork = (PFN_pstotextSetCork) GetProcAddress(pstotextModule, "pstotextSetCork");
    if (dllfn_pstotextSetCork == (PFN_pstotextSetCork)NULL) {
	fprintf(stderr, "Can't find pstotextSetCork() in %s\n", dllname);
	FreeLibrary(pstotextModule);
	return 1;
    }

    dllfn_pstotextInit(&pstotextInstance);

    return 0;
}

int
unload_pstotext(void)
{
    if (pstotextInstance)
        dllfn_pstotextExit(pstotextInstance);
    pstotextInstance = NULL;
    FreeLibrary(pstotextModule);
    pstotextModule = NULL;
    return 0;
}

void
send_prolog(FILE *f, int resource)
{
    HGLOBAL hglobal;
    LPSTR prolog;
    hglobal = LoadResource(pstotextModule, 
	FindResource(pstotextModule, (LPSTR)resource, RT_RCDATA));
    if ( (prolog = (LPSTR)LockResource(hglobal)) != (LPSTR)NULL) {
	fputs(prolog, f);
	FreeResource(hglobal);
    }
}

#else /* !_Windows */
#ifdef MSDOS
int
load_pstotext(void)
{
    dllfn_pstotextInit = pstotextInit;
    dllfn_pstotextFilter = pstotextFilter;
    dllfn_pstotextExit = pstotextExit;
    dllfn_pstotextSetCork = pstotextSetCork;
    dllfn_pstotextInit(&pstotextInstance);
    return 0;
}

int
unload_pstotext(void)
{
    return 0;
}

void
send_prolog(FILE *f, int resource)
{
    switch (resource) {
	case OCR_PROLOG:
	    putbundle(ocr, f);
	    break;
	case ROT270_PROLOG:
	    putbundle(rot270, f);
	    break;
	case ROT90_PROLOG:
	    putbundle(rot90, f);
	    break;
    }
}

#else /* !_Windows && !MSDOS */
/* OS/2 */
int
load_pstotext(void)
{
char dllname[256];
char buf[256];
char *p;
APIRET rc;
PTIB pptib;
PPIB pppib;

    if ( (rc = DosGetInfoBlocks(&pptib, &pppib)) != 0 ) {
	fprintf(stderr,"Couldn't get pid, rc = \n", rc);
	return rc;
    }

    /* get path to EXE */
    if ( (rc = DosQueryModuleName(pppib->pib_hmte, sizeof(dllname), dllname)) != 0 ) {
	fprintf(stderr,"Couldn't get module name, rc = %d\n", rc);
	return rc;
    }
    if ((p = strrchr(dllname,'\\')) != (PCHAR)NULL) {
	p++;
	*p = '\0';
    }
    strcat(dllname, "pstotxt2.dll");
    if (debug) {
	fputs(dllname, stdout);
	fputc('\n', stdout);
    }

    /* load pstotext DLL */
    if (DosLoadModule(buf, sizeof(buf), dllname, &pstotextModule)) {
	fprintf(stderr, "Can't load %s\n", dllname);
	return 1;
    }
    if ((rc = DosQueryProcAddr(pstotextModule, 0, "pstotextInit", (PFN *)(&dllfn_pstotextInit))) !=0) {
	fprintf(stderr, "Can't find pstotextInit() in %s\n", dllname);
	DosFreeModule(pstotextModule);
	pstotextModule = (HMODULE)NULL;
	return 1;
    }
    if ((rc = DosQueryProcAddr(pstotextModule, 0, "pstotextFilter", (PFN *)(&dllfn_pstotextFilter))) !=0) {
	fprintf(stderr, "Can't find pstotextFilter() in %s\n", dllname);
	DosFreeModule(pstotextModule);
	pstotextModule = (HMODULE)NULL;
	return 1;
    }
    if ((rc = DosQueryProcAddr(pstotextModule, 0, "pstotextExit", (PFN *)(&dllfn_pstotextExit))) !=0) {
	fprintf(stderr, "Can't find pstotextExit() in %s\n", dllname);
	DosFreeModule(pstotextModule);
	pstotextModule = (HMODULE)NULL;
	return 1;
    }
    if ((rc = DosQueryProcAddr(pstotextModule, 0, "pstotextSetCork", (PFN *)(&dllfn_pstotextSetCork))) !=0) {
	fprintf(stderr, "Can't find pstotextSetCork() in %s\n", dllname);
	DosFreeModule(pstotextModule);
	pstotextModule = (HMODULE)NULL;
	return 1;
    }

    dllfn_pstotextInit(&pstotextInstance);

    return 0;
}

int
unload_pstotext(void)
{
    if (pstotextInstance)
        dllfn_pstotextExit(pstotextInstance);
    pstotextInstance = NULL;
    if (pstotextModule)
        DosFreeModule(pstotextModule);
    pstotextModule = (HMODULE)NULL;
    return 0;
}


int
send_prolog(FILE *f, int resource)
{  
char *prolog, *p;
APIRET rc;
int code = -1;
	rc = DosGetResource(pstotextModule, RT_RCDATA, resource, (PPVOID)&prolog);
	if (!rc && (prolog != (char *)NULL) ) {
	    code = 0;
	    p = prolog;
	    while (*p) {
		if (!code)
		    fputs(p, f);
		p += strlen(p)+1;
	    }
	    DosFreeResource(prolog);
	}
	else {
	    fprintf(stderr, "Failed to load pstotext resource %d\n", resource);
	}
	return code;
}
#endif /* OS/2   (!MSDOS) */
#endif /* (!_Windows) */

/* create an empty temporary file and return its name */
static char *scratch_file(void) {
    FILE *f;
    char *temp;
    char *path = malloc(256);
    if (path == NULL)
	return NULL;
    if ( (temp = getenv("TEMP")) != NULL )
	strcpy(path, temp);
    else if ( (temp = getenv("TMP")) != NULL )
	strcpy(path, temp);
    else
	strcpy(path, "c:\\");

    /* Prevent X's in path from being converted by mktemp. */
    for ( temp = path; *temp; temp++ ) {
	    *temp = (char)tolower(*temp);
	    if (*temp == '/')
		*temp = '\\';
    }
    if ( strlen(path) && (path[strlen(path)-1] != '\\') )
	    strcat(path, "\\");

    strcat(path, "ptXXXXXX");
    mktemp(path);
    f = fopen(path, "w");
    if (f==NULL) {perror(cmd); exit(1);}
    fclose(f);
    return path;
}

static char *make_temp(int resource) {
  /* Return pathname of temporary file containing prolog from resources.  Caller
     should unlink file (and, technically, free pathname). */
    FILE *f;
    char *path = scratch_file();
    if (path == NULL) {perror(cmd); cleanup(); exit(1);}
	
    f = fopen(path, "w");
    if (f==NULL) {perror(cmd); cleanup(); exit(1);}

    send_prolog(f, resource);
    fclose(f);
    return path;
}


static char *ocr_path = NULL, *rotate_path = NULL;
static FILE *gs = NULL;
char *gstemp = NULL;
static void *instance; /* pstotext state */

static int cleanup(void) {
  int status = 0;
  unload_pstotext();
  if (gs!=NULL) {
#if defined(_Windows) || defined(MSDOS)
    fclose(gs);
#else
    pclose(gs);
#endif
  }
  if (gstemp!=NULL && !debug)
    unlink(gstemp);
  if (rotate_path!=NULL && strcmp(rotate_path, "")!=0 && !debug) 
    unlink(rotate_path);
  if (ocr_path!=NULL && !debug) 
     unlink(ocr_path);
  return status;
}

static void handler(int code) {
  int status = code; /* suppress unreference 'code' warning */
  status = cleanup();
  if (status!=0)
    exit(status);
  exit(2);
}

static void do_it(char *path) {
  /* If "path" is NULL, then "stdin" should be processed. */
  char gs_cmd[2*MAXPATHLEN];
  char input[MAXPATHLEN];
  int status;
  FILE *fileout;
#ifdef MSDOS
  char *gsargtemp;
  FILE *gsargfile;
#endif

  fileout = stdout;
  if (strlen(outfile) != 0) {
    fileout = fopen(outfile, "w");
    if (fileout == NULL) {perror(cmd); exit(1);}
  }

  signal(SIGINT, handler);
  signal(SIGTERM, handler);

  status = load_pstotext();
  if (status!=0) {
    fprintf(stderr, "%s: internal error %d\n", cmd, status);
    exit(5);
  }

  if (cork) {
    status = dllfn_pstotextSetCork(pstotextInstance, TRUE);
    if (status!=0) {
      fprintf(stderr, "%s: internal error %d\n", cmd, status);
      exit(5);
    }
  }

  ocr_path = make_temp(OCR_PROLOG);

  switch (orientation) {
  case portrait: rotate_path = ""; break;
  case landscape: rotate_path = make_temp(ROT270_PROLOG); break;
  case landscapeOther: rotate_path = make_temp(ROT90_PROLOG); break;
  }

  if (path==NULL) strcpy(input, "-");
  else {strcpy(input, "-- "); strcat(input, path);}

#if defined(_Windows) || defined(MSDOS)
  /* don't support pipes, so write gs output to a temporary file */
  if ( (gstemp = scratch_file()) == NULL) {
	cleanup();
	exit(1);
  }
#endif

#ifdef MSDOS
  /* MSDOS has command line length problems */
  if ( (gsargtemp = scratch_file()) == NULL) {
	cleanup();
	exit(1);
  }
  if ( (gsargfile = fopen(gsargtemp, "w")) == (FILE *)NULL) {
	cleanup();
	exit(1);
  }
  fprintf(gsargfile, "-r72 -dNODISPLAY -dFIXEDMEDIA -dDELAYBIND -dWRITESYSTEMDICT %s -dNOPAUSE\n",
    (debug ? "" : "-q"));
  fputs(rotate_path, gsargfile);
  fputs("\n", gsargfile);
  fputs(ocr_path, gsargfile);
  fputs("\n", gsargfile);
  fputs(input, gsargfile);
  fputs("\n", gsargfile);
  fclose(gsargfile);
  
  sprintf(gs_cmd, "%s @%s %s %s", gscommand, gsargtemp, 
#if defined(_Windows) || defined(MSDOS)
    "> ", gstemp
#else
    "", ""
#endif
    );

#else   /* !MSDOS */
  sprintf(gs_cmd, "%s -r72 -dNODISPLAY -dFIXEDMEDIA -dDELAYBIND -dWRITESYSTEMDICT %s -dNOPAUSE %s %s %s %s %s",
    gscommand,
    (debug ? "" : "-q"),
    ocr_path,
    rotate_path,
    input,
#if defined(_Windows) || defined(MSDOS)
    "> ", gstemp
#else
    "", ""
#endif
    );
#endif

  if (debug) {
    fputs(gs_cmd, stdout);
    fputc('\n', stdout);
  }

#if defined(_Windows) || defined(MSDOS)
  if (system(gs_cmd)) {
    fprintf(stderr,"\nCan't run (errno=%d):\n   %s\n", errno, gs_cmd);
    cleanup();
    exit(1);
  }

  gs = fopen(gstemp, "r");
#else
  gs = popen(gs_cmd, "r");
#endif

#ifdef MSDOS
  if (!debug)
     unlink(gsargtemp);
  free(gsargtemp);
#endif

  if( gs==NULL ) {perror(cmd); cleanup(); exit(1);}

  while (gs != NULL) {   /* while TRUE */
    char line[LINELEN];
    char *pre, *word, *post;
    int llx, lly, urx, ury;
    if (fgets(line, LINELEN, gs)==NULL) break;
    if (debug)
	fputs(line, stdout);
    status = dllfn_pstotextFilter(
      pstotextInstance, line, &pre, &word, &post, &llx, &lly, &urx, &ury);
    if (status!=0) {
      fprintf(stderr, "%s: internal error %d\n", cmd, status);
      cleanup();
      exit(5);
    }
    if (word!=NULL)
      if (!bboxes) {
        fputs(pre, fileout); fputs(word, fileout); fputs(post, fileout);
	if (debug)
	    fputc('\n', stdout);
      }
      else {
	if (pre) {
	    if (*pre == ' ')
		pre++;
	    fputs(pre, fileout);
	}
        fprintf(fileout, "%6d\t%6d\t%6d\t%6d\t%s\n", llx, lly, urx, ury, word);
	if (post)
	    fputs(post, fileout);
      }
  }
  if (fileout != stdout) fclose(fileout);
  status = cleanup();
  if (status!=0) exit(status);
}

int main(int argc, char *argv[]) {
  int i;
  char *arg;
  cmd = argv[0];
  for (i = 1; i<argc; i++) {
    arg = argv[i];
    if (strcasecmp(arg, "-landscape")==0) orientation = landscape;
    else if (strcasecmp(arg, "-cork")==0) cork = TRUE;
    else if (strcasecmp(arg, "-landscapeOther")==0) orientation = landscapeOther;
    else if (strcasecmp(arg, "-portrait")==0) orientation = portrait;
    else if (strcasecmp(arg, "-bboxes")==0) bboxes = TRUE;
    else if (strcasecmp(arg, "-debug")==0) debug = TRUE;
    else if (strcasecmp(arg, "-gs")==0) {
	i++;
	if (i<argc)
	    gscommand = argv[i];
    }
    else if (strcasecmp(arg, "-output") == 0) {
	i++;
	if (i>=argc) {usage(); exit(1);}
	outfile = argv[i];
    }
    else if (strcmp(arg, "-")==0) do_it(NULL);
    else if (arg[0] == '-') {usage(); exit(1);}
    else /* file */ {
      explicitFiles++;
      do_it(arg);
    }
  }
  if (explicitFiles==0) do_it(NULL);
  return 0;
}
