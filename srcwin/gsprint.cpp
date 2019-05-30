/* Copyright (C) 2000-2006, Ghostgum Software Pty Ltd.  All rights reserved.
  
  This file is part of GSview.
  
  This program is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  Refer to the GSview Free Public Licence 
  (the "Licence") for full details.
  
  Every copy of GSview must include a copy of the Licence, normally in a 
  plain ASCII text file named LICENCE.  The Licence grants you the right 
  to copy, modify and redistribute GSview, but only under certain conditions 
  described in the Licence.  Among other things, the Licence requires that 
  the copyright notice and this notice be preserved on all copies.
*/

// gsprint.cpp - An external printer driver for Windows Ghostscript
//
// Tells Ghostscript to output using a BMP device and write 
// to a pipe using -sOutputFile="%handle%XXXXXXXX".
// We read from the pipe and write GDI commands to the
// actual printer.


#include "gvcver.h"	// pick up BETA settings

#define STRICT
#include <windows.h>
#include <commdlg.h>
#include <winspool.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#ifdef BETA
#include <time.h>
#endif
extern "C" {
#include "gvcfile.h"
BOOL find_gs(char *gspath, int len, int minver, BOOL bDLL); /* gvwgsver.cpp */
}
#include "gvwdib.h"
#include "gvwpdib.h"


HANDLE hPipeRd; 	/* We read printer output from this one */
HANDLE hPipeWr;
#ifdef NOTUSED
PROCESS_INFORMATION piProcInfo;
#endif
BOOL global_debug;

#define COPYRIGHT TEXT("Copyright (C) 2003-2006, Ghostgum Software Pty Ltd.  All Rights Reserved.\n")
#define VERSION TEXT("2006-02-24 gsprint 1.9\n")

#define MAXSTR 256

typedef struct tagGSPRINT_OPTION {
    int colour;	// one of the following
#define MONO 0
#define GREY 1
#define COLOUR 2

    int orientation; // one of the following
#define DEFAULT 0
#define PORTRAIT 1
#define LANDSCAPE 2

    int duplex;
#define DUPLEX_UNKNOWN 0
#define DUPLEX_SIMPLEX 1
#define DUPLEX_VERTICAL 2
#define DUPLEX_HORIZONTAL 3

    int copies;

    BOOL query;		// true if printer setup to be shown
    BOOL printer;	// printer specified
    char printer_name[MAXSTR];
    char printer_port[MAXSTR];	// optional

    char args[4096];	// all arguments for gsprint and gs
    int args_end;	// character index to end of args
    char gs[MAXSTR];	// Path and filename of command line Ghostscript
    char options[2048];	// options for gs
    char filenames[2048]; // filenames for gs

    int from;		// 0 = first
    int to;		// 0 = last
    int even_odd;	// one of the following
#define ALL_PAGES 0
#define ODD_PAGES 1
#define EVEN_PAGES 2

    BOOL twoup;

    char document_name[MAXSTR];
    BOOL debug;
    BOOL debug_gdi;
} GSPRINT_OPTION;


#ifdef BETA
int
beta_expired(void)
{
  time_t today = time(NULL);
  struct tm *t;
  t = localtime(&today);
  if (t->tm_year+1900 < BETA_YEAR)
    return 0;
  if (t->tm_year+1900 > BETA_YEAR)
    return 1;    /* beta copy has expired */
  if (t->tm_mon+1 < BETA_MONTH)
    return 0;
  if (t->tm_mon+1 > BETA_MONTH)
    return 1;    /* beta copy has expired */
  if (t->tm_mday < BETA_DAY)
    return 0;
  return 1;    /* beta copy has expired */
}

int beta(void)
{
  if (beta_expired()) {
	fprintf(stdout, "\nThis TEST version has expired.\n");
    return 1;
  }
  return 0;
}
#endif /* BETA */

void print_id()
{
    fputs(COPYRIGHT, stdout);
	fputs(VERSION, stdout);
#ifdef BETA
	fprintf(stdout, "This a TEST version of gsprint.  It will disable on %04d-%02d-%02d\n", BETA_YEAR, BETA_MONTH, BETA_DAY);
#endif
}

void print_help()
{
    fprintf(stdout, "\
Usage:  gsprint [options] filename\n\
 -mono                  Render in monochrome as 1bit/pixel\n\
 -grey or -gray         Render in greyscale as 8bits/pixel\n\
 -colour or -color      Render in colour as 24bits/pixel\n\
 -query                 Show printer setup dialog\n\
 -printer \042name\042        Print to the specified printer\n\
 -port \042name\042           Print to the specified printer port\n\
 -ghostscript \042name\042    Path and filename of command line Ghostscript\n\
 -option \042name\042    Option to pass to ghostscript\n\
 -config \042name\042         Read options from this file instead of gsprint.cfg\n\
 -odd                   Print only odd pages\n\
 -even                  Print only even pages\n\
 -all                   Print all pages\n\
 -from NN               First page to print is NN\n\
 -to NN                 Last page to print is NN\n\
 -twoup                 2 pages per sheet\n\
 -portrait              Portrait orientation\n\
 -landscape             Landscape orientation\n\
 -duplex_simplex        No duplex\n\
 -duplex_vertical       Duplex (binding on long edge)\n\
 -duplex_horizontal     Duplex (binding on short edge)\n\
 -copies NN             Print NN copies (if supported)\n\
 \042filename\042             The PostScript/PDF file to print\n\
");
}

// add one argument to opt->args
BOOL add_arg(GSPRINT_OPTION *opt, char *str)
{
    int i;
    if (strlen(str) + 2 < sizeof(opt->args) - opt->args_end) {
	strcpy(&opt->args[opt->args_end], str);
	// remove trailing spaces
	i = opt->args_end + strlen(str) - 1;
	while (i>=0 && opt->args[i] == ' ')
	    i--;
	i++;
	opt->args[i] = '\0';
	opt->args_end = i+1;
	opt->args[opt->args_end] = '\0';	// double trailing null
	return TRUE;
    }
    fprintf(stdout, "Too many arguments\n");
    return FALSE;
}

// read options from config file
BOOL read_config(GSPRINT_OPTION *opt, FILE *f)
{
    char buf[MAXSTR];
    while (fgets(buf, sizeof(buf)-1, f)) {
	// remove trailing \n
	if (strlen(buf))
	    buf[strlen(buf)-1] = '\0';
	if (strcmp(buf, "-config") == 0) {
	    fprintf(stdout, "recursive config files not permitted\n");
	    return FALSE;
	}
	if (!add_arg(opt, buf))
	    return FALSE;
    }

    return TRUE;
}

// collect all arguments together in opt->args
BOOL collect_args(GSPRINT_OPTION *opt, int argc, char *argv[])
{
    int i;
    for (i=1; i < argc; i++) {
	if (strcmp(argv[i], "-config") == 0) {
	    if (i+1 < argc) {
		i++;
		FILE *f = fopen(argv[i], "r");
		if (f == (FILE *)NULL) {
		    fprintf(stdout, "Can't open config file \042%s\042\n", argv[i]);
		    return FALSE;
		}
		if (!read_config(opt, f)) {
		    fclose(f);
		    return FALSE;
		}
		fclose(f);
	    }
	    else {
		fprintf(stdout, "missing -config filename\n");
		return FALSE;
	    }
	}
	else {
	    if (!add_arg(opt, argv[i]))
		return FALSE;
	}
    }
    return TRUE;
}

void missing_arg(char *thisarg)
{
    fprintf(stdout, "Missing %s argument\n", thisarg);
}

char enable_twoup[] = 
" -c \042\
 /gsview_twoup_dict 8 dict def\
 gsview_twoup_dict begin\
 currentpagedevice /PageSize get dup\
 /twidth exch 0 get def\
 /theight exch 1 get def\
 /tland twidth theight gt def\
 twidth theight tland {exch} if\
 2 copy 1.414214 div gt {2 div exch div}{div}ifelse\
 /tscale exch def\
 tland\
 {\
 theight tscale div twidth sub 2 div /tx2 exch def /tx1 tx2 def\
 twidth 2 div tscale div theight sub 2 div /ty2 exch def\
 /ty1 twidth 2 div tscale div ty2 add def\
 }\
 {\
 twidth tscale div theight sub 2 div /ty1 exch def /ty2 ty1 def\
 theight 2 div tscale div twidth sub 2 div /tx1 exch def\
 /tx2 theight 2 div tscale div tx1 add def\
 }\
 ifelse\
 end\
 <<\
 /BeginPage{gsview_twoup_dict begin 90 rotate 0 twidth neg translate\
 tscale tscale scale 1 and 0 eq{tx1 ty1}{tx2 ty2}ifelse translate end}\
 /EndPage{2 eq {pop false}{1 and 0 ne}ifelse}\
 >> setpagedevice\042 -f ";

char disable_twoup[] =
" -c \042<< /BeginPage{} /EndPage{} >> setpagedevice\042 -f ";

void add_twoup(GSPRINT_OPTION *opt)
{
    char *p = opt->twoup ? enable_twoup : disable_twoup;
    if (strlen(p) + strlen(opt->options) + 3 < sizeof(opt->options))
	strcat(opt->options, p);
    else
	fprintf(stdout, "Argument too long while adding twoup\n");
}

/* Determine if an argument needs to be quoted.
 * Assume that all argument need to be quoted,
 * except those that already contain a ".
 */
BOOL quote_it(const char *arg)
{
    const char *p;
    for (p=arg; *p; p++)
	if (*p == '\042')
	    return FALSE;
    return TRUE;
}

BOOL process_args(GSPRINT_OPTION *opt)
{
    char *thisarg = opt->args;
    char *nextarg;
    while (*thisarg) {
        nextarg = thisarg + strlen(thisarg) + 1;
	if (strcmp(thisarg, "-help") == 0) {
	    print_help();
	    return FALSE;
	}
	else if (strcmp(thisarg, "-debug") == 0) {
	    opt->debug = TRUE;
	}
	else if (strcmp(thisarg, "-debug_gdi") == 0) {
	    opt->debug_gdi = TRUE;
	}
	else if (strcmp(thisarg, "-mono") == 0) {
	    opt->colour = MONO;
	}
	else if (strcmp(thisarg, "-grey") == 0) {
	    opt->colour = GREY;
	}
	else if (strcmp(thisarg, "-gray") == 0) {
	    opt->colour = GREY;
	}
	else if (strcmp(thisarg, "-colour") == 0) {
	    opt->colour = COLOUR;
	}
	else if (strcmp(thisarg, "-color") == 0) {
	    opt->colour = COLOUR;
	}
	else if (strcmp(thisarg, "-duplex") == 0) {
	    /* for backward compatibility */
	    opt->duplex = DUPLEX_VERTICAL;
	}
	else if (strcmp(thisarg, "-duplex_unknown") == 0) {
	    opt->duplex = DUPLEX_UNKNOWN;
        }
	else if (strcmp(thisarg, "-duplex_simplex") == 0) {
	    opt->duplex = DUPLEX_SIMPLEX;
	}
	else if (strcmp(thisarg, "-duplex_vertical") == 0) {
	    opt->duplex = DUPLEX_VERTICAL;
	}
	else if (strcmp(thisarg, "-duplex_horizontal") == 0) {
	    opt->duplex = DUPLEX_HORIZONTAL;
	}
	else if (strcmp(thisarg, "-portrait") == 0) {
	    opt->orientation = PORTRAIT;
	}
	else if (strcmp(thisarg, "-landscape") == 0) {
	    opt->orientation = LANDSCAPE;
	}
	else if (strcmp(thisarg, "-copies") == 0) {
	    if (*nextarg) {
		opt->copies = atoi(nextarg);
		thisarg = nextarg;
	    }
	    else {
		missing_arg(thisarg);
		return FALSE;
	    }
	}
	else if (strcmp(thisarg, "-all") == 0) {
	    opt->even_odd = ALL_PAGES;
	    opt->from = 0;
	    opt->to = 0;
	}
	else if (strcmp(thisarg, "-odd") == 0) {
	    opt->even_odd = ODD_PAGES;
	}
	else if (strcmp(thisarg, "-even") == 0) {
	    opt->even_odd = EVEN_PAGES;
	}
	else if (strcmp(thisarg, "-from") == 0) {
	    if (*nextarg) {
		opt->from = atoi(nextarg);
		thisarg = nextarg;
	    }
	    else {
		missing_arg(thisarg);
		return FALSE;
	    }
	}
	else if (strcmp(thisarg, "-to") == 0) {
	    if (*nextarg) {
		opt->to = atoi(nextarg);
		thisarg = nextarg;
	    }
	    else {
		missing_arg(thisarg);
		return FALSE;
	    }
	}
	else if (strcmp(thisarg, "-twoup") == 0) {
	    opt->twoup = TRUE;
	    add_twoup(opt);
	}
	else if (strcmp(thisarg, "-notwoup") == 0) {
	    BOOL old_twoup = opt->twoup;
	    opt->twoup = FALSE;
	    if (old_twoup)
		add_twoup(opt);
	}
	else if (strcmp(thisarg, "-ghostscript") == 0) {
	    if (*nextarg) {
		if (strlen(thisarg) + 1 < sizeof(opt->gs)) {
		    strcpy(opt->gs, nextarg);
		    thisarg = nextarg;
		}
		else  {
		    fprintf(stdout, "Argument of -ghostscript is too long\n");
		    return FALSE;
		}
	    }
	    else {
		missing_arg(thisarg);
		return FALSE;
	    }
	}
	else if (strcmp(thisarg, "-printer") == 0) {
	    if (*nextarg) {
		if (strlen(thisarg) + 1 < sizeof(opt->printer_name)) {
		    strcpy(opt->printer_name, nextarg);
		    opt->printer = TRUE;
		    thisarg = nextarg;
		}
		else  {
		    fprintf(stdout, "Argument of -printer is too long\n");
		    return FALSE;
		}
	    }
	    else {
		missing_arg(thisarg);
		return FALSE;
	    }
	}
	else if (strcmp(thisarg, "-noprinter") == 0) {
	    opt->printer = FALSE;
	}
	else if (strcmp(thisarg, "-port") == 0) {
	    if (*nextarg) {
		if (strlen(thisarg) + 1 < sizeof(opt->printer_port)) {
		    strcpy(opt->printer_port, nextarg);
		    thisarg = nextarg;
		}
		else  {
		    fprintf(stdout, "Argument of -port is too long\n");
		    return FALSE;
		}
	    }
	    else {
		missing_arg(thisarg);
		return FALSE;
	    }
	}
	else if (strcmp(thisarg, "-noport") == 0) {
	    opt->printer_port[0] = '\0';
	}
	else if (strcmp(thisarg, "-query") == 0) {
	    opt->query = TRUE;
	}
	else if (strcmp(thisarg, "-noquery") == 0) {
	    opt->query = FALSE;
	}
	else if (strcmp(thisarg, "-option") == 0) {
	    if (*nextarg) {
		if (strlen(thisarg) + 5 < 
		    sizeof(opt->options) - strlen(opt->options)) {
		    thisarg = nextarg;
		    strcat(opt->options, " ");
		    if (quote_it(thisarg)) {
			/* option, not quoted */
			strcat(opt->options,"\042");
			strcat(opt->options, thisarg);
			strcat(opt->options,"\042");
		    }
		    else 
			strcat(opt->options, thisarg);

		}
		else  {
		    fprintf(stdout, "Argument of -option is too long\n");
		    return FALSE;
		}
	    }
	    else {
		missing_arg(thisarg);
		return FALSE;
	    }
	}
	else {
	    BOOL is_option = FALSE;
	    // Something for Ghostscript
	    if (thisarg[0] == '-')
		is_option = TRUE;	/* Looks like an option */
	    if ((thisarg[0] == '\042') && (thisarg[1] == '-'))
		is_option = TRUE;	/* Looks like an option */
	    if (opt->filenames[0])	/* But options after filenames */
		is_option = FALSE;	/* must remain after the filename */
	    if (is_option) {
		if (strlen(thisarg) + 5 < 
		    sizeof(opt->options) - strlen(opt->options) ) {
		    strcat(opt->options, " ");
		    if (quote_it(thisarg)) {
			strcat(opt->options,"\042");
			strcat(opt->options, thisarg);
			strcat(opt->options,"\042");
		    }
		    else 
			strcat(opt->options, thisarg);
		}
		else  {
		    fprintf(stdout, "Argument too long: \042%s\042\n", thisarg);
		    return FALSE;
		}
	    }
	    else {
		if (strlen(thisarg) + 5 < 
		    sizeof(opt->filenames) - strlen(opt->filenames) ) {
		    strcat(opt->filenames, " ");
		    if (quote_it(thisarg)) {
			strcat(opt->filenames,"\042");
			strcat(opt->filenames, thisarg);
			strcat(opt->filenames,"\042");
		    }
		    else 
			strcat(opt->filenames, thisarg);
		}
		else  {
		    fprintf(stdout, "Argument too long: \042%s\042\n", thisarg);
		    return FALSE;
		}
	    }
	    if ( ((thisarg[0] == '\042') && (thisarg[1] != '-'))
		    || (thisarg[0] != '-') ) {
		// probably a filename - use as the document name
		if (strlen(thisarg) < sizeof(opt->document_name)-1)
		    strcpy(opt->document_name, thisarg);
	    }
	}
	
	if (*thisarg)
	    thisarg = thisarg + strlen(thisarg) + 1;
    }
    return TRUE; 
} 

void
write_error(DWORD err)
{
LPVOID lpMessageBuffer;
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM,
	NULL, err,
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* user default language */
	(LPTSTR) &lpMessageBuffer, 0, NULL);
    if (lpMessageBuffer) {
	fputs((LPTSTR)lpMessageBuffer, stdout);
	LocalFree(LocalHandle(lpMessageBuffer));
	fputs("\r\n", stdout);
    }
}


/* Start child program with redirected standard input and output */
BOOL exec_prog(LPCSTR command)
{
    STARTUPINFO siStartInfo;
#ifndef NOTUSED
    PROCESS_INFORMATION piProcInfo;
#endif
    LPVOID env;

    /* Now create the child process. */

    /* Set up members of STARTUPINFO structure. */

    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.lpReserved = NULL;
    siStartInfo.lpDesktop = NULL;
    siStartInfo.lpTitle = NULL;  /* use executable name as title */
    siStartInfo.dwX = siStartInfo.dwY = CW_USEDEFAULT;		/* ignored */
    siStartInfo.dwXSize = siStartInfo.dwYSize = CW_USEDEFAULT;	/* ignored */
    siStartInfo.dwXCountChars = 80;
    siStartInfo.dwYCountChars = 25;
    siStartInfo.dwFillAttribute = 0;			/* ignored */
    siStartInfo.dwFlags = 0;
    siStartInfo.wShowWindow = SW_SHOWNORMAL;		/* ignored */
    siStartInfo.cbReserved2 = 0;
    siStartInfo.lpReserved2 = NULL;
    siStartInfo.hStdInput = NULL;
    siStartInfo.hStdOutput = NULL;
    siStartInfo.hStdError = NULL;

    env = NULL;

    /* Create the child process. */

    if (!CreateProcess(NULL,
        (char *)command,  /* command line                       */
        NULL,          /* process security attributes        */
        NULL,          /* primary thread security attributes */
        TRUE,          /* handles are inherited              */
        0,             /* creation flags                     */
        env,           /* environment                        */
        NULL,          /* use parent's current directory     */
        &siStartInfo,  /* STARTUPINFO pointer                */
        &piProcInfo))  /* receives PROCESS_INFORMATION  */
	  return FALSE;

    WaitForInputIdle(piProcInfo.hProcess, 5000);
#ifndef NOTUSED
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);
#endif

    return TRUE;
}

// Return a devmode and devnames structure for the specified printer.
// If device == NULL, return for the default printer.
BOOL get_devmode(GSPRINT_OPTION *opt, HANDLE *hdevmode, HANDLE *hdevnames)
{
    char devicebuf[256];
    char driverbuf[512];
    char *device;
    char *driver;
    char *output;
    int length, offset;
    HANDLE hglobal;
    LPDEVNAMES lpdevnames;

    if (opt->printer)
	device = opt->printer_name;
    else {
	// no device specified - use default
	GetProfileString("windows", "device", "", devicebuf, sizeof(devicebuf));
	strtok(devicebuf, ",");
	device = devicebuf;
    }
    
    // Get the information needed for the DEVNAMES structure
    GetProfileString("Devices", device, "", driverbuf, sizeof(driverbuf));
    if (strlen(driverbuf) == 0)
	return FALSE;	// printer doesn't exist

    driver = strtok(driverbuf, ",");
    output = strtok(NULL, ",");

    // Build the DEVNAMES structure
    length = sizeof(DEVNAMES) + 
	strlen(device) + 1 + 
	strlen(driver) + 1 +
	strlen(output) + 2;
    if (length < 1024)
	length = 1024;
    hglobal = GlobalAlloc(GMEM_MOVEABLE, length);
    if (hglobal == NULL)
	return FALSE;
    lpdevnames = (LPDEVNAMES)GlobalLock(hglobal);
    memset(lpdevnames, 0, length);
    offset = sizeof(DEVNAMES);
    lpdevnames->wDriverOffset = offset;
    strcpy((char *)lpdevnames + offset, driver);
    offset += strlen(driver) + 1;
    lpdevnames->wDeviceOffset = offset;
    strcpy((char *)lpdevnames + offset, device);
    offset += strlen(device) + 1;
    lpdevnames->wOutputOffset = offset;
    strcpy((char *)lpdevnames + offset, output);
    GlobalUnlock(hglobal);
    lpdevnames = NULL;
    *hdevnames = hglobal;

    // here we should try to merge requests for page size,
    // duplex, orientation etc. by obtaining and modifying
    // DEVMODE.
    LPDEVMODE podevmode, pidevmode;
    HANDLE hprinter;

    if (!OpenPrinter(device, &hprinter, NULL))
	return FALSE;
    length = DocumentProperties(NULL, hprinter, device, NULL, NULL, 0);

    hglobal = GlobalAlloc(GMEM_MOVEABLE, length);
    if (hglobal == NULL) {
	ClosePrinter(hprinter);
	return FALSE;
    }
    podevmode = (LPDEVMODE)GlobalLock(hglobal);
    memset(podevmode, 0, length);
	    
    if ((pidevmode = (LPDEVMODE)malloc(length)) == (LPDEVMODE) NULL) {
	GlobalUnlock(hglobal);
	GlobalFree(hglobal);
	ClosePrinter(hprinter);
	return FALSE;
    }
    DocumentProperties(NULL, hprinter, device, podevmode, NULL, DM_OUT_BUFFER);

    memcpy(pidevmode, podevmode, length);

    pidevmode->dmFields = 0;

    if (opt->duplex == DUPLEX_SIMPLEX) {
	pidevmode->dmFields |= DM_DUPLEX;
	pidevmode->dmDuplex = DMDUP_SIMPLEX;
    }
    else if (opt->duplex == DUPLEX_VERTICAL) {
	pidevmode->dmFields |= DM_DUPLEX;
	pidevmode->dmDuplex = DMDUP_VERTICAL;
    }
    else if (opt->duplex == DUPLEX_HORIZONTAL) {
	pidevmode->dmFields |= DM_DUPLEX;
	pidevmode->dmDuplex = DMDUP_HORIZONTAL;
    }

    if (opt->copies) {
	pidevmode->dmFields |= DM_COPIES;
	pidevmode->dmCopies = opt->copies;
    }
    if (opt->orientation == PORTRAIT) {
	pidevmode->dmFields |= DM_ORIENTATION;
	pidevmode->dmOrientation = DMORIENT_PORTRAIT;
    } else if (opt->orientation == LANDSCAPE) {
	pidevmode->dmFields |= DM_ORIENTATION;
	pidevmode->dmOrientation = DMORIENT_LANDSCAPE;
    }

    // merge the entries
    DocumentProperties(NULL, hprinter, device, podevmode, pidevmode, 
	DM_IN_BUFFER | DM_OUT_BUFFER);
    ClosePrinter(hprinter);


    free(pidevmode);
    GlobalUnlock(hglobal);
    *hdevmode = hglobal;
    return TRUE;
}

void print_devmode(HANDLE hDevMode)
{
    DWORD dw;
    if ((hDevMode == NULL) || (hDevMode == INVALID_HANDLE_VALUE))
	return;
    LPDEVMODE dm = (LPDEVMODE)GlobalLock(hDevMode);
    fprintf(stdout, "DevMode:\n");
    fprintf(stdout, " dmDeviceName=\042%s\042\n", dm->dmDeviceName);
    fprintf(stdout, " dmFields=0x%x\n", dm->dmFields);
    dw = dm->dmFields;
    if (dw & DM_ORIENTATION)
	fprintf(stdout, "  DM_ORIENTATION\n");
    if (dw & DM_PAPERSIZE)
	fprintf(stdout, "  DM_PAPERSIZE\n");
    if (dw & DM_PAPERLENGTH)
	fprintf(stdout, "  DM_PAPERLENGTH\n");
    if (dw & DM_PAPERWIDTH)
	fprintf(stdout, "  DM_PAPERWIDTH\n");
    if (dw & DM_SCALE)
	fprintf(stdout, "  DM_SCALE\n");
    if (dw & DM_COPIES)
	fprintf(stdout, "  DM_COPIES\n");
    if (dw & DM_DEFAULTSOURCE)
	fprintf(stdout, "  DM_DEFAULTSOURCE\n");
    if (dw & DM_PRINTQUALITY)
	fprintf(stdout, "  DM_PRINTQUALITY\n");
    if (dw & DM_COLOR)
	fprintf(stdout, "  DM_COLOR\n");
    if (dw & DM_DUPLEX)
	fprintf(stdout, "  DM_DUPLEX\n");
    if (dw & DM_YRESOLUTION)
	fprintf(stdout, "  DM_YRESOLUTION\n");
    if (dw & DM_TTOPTION)
	fprintf(stdout, "  DM_TTOPTION\n");
    if (dw & DM_COLLATE)
	fprintf(stdout, "  DM_COLLATE\n");
    if (dw & DM_FORMNAME)
	fprintf(stdout, "  DM_FORMNAME\n");
    fprintf(stdout, " dmOrientation=%d\n", dm->dmOrientation);
    fprintf(stdout, " dmPaperSize=%d\n", dm->dmPaperSize);
    fprintf(stdout, " dmPaperLength=%d\n", dm->dmPaperLength);
    fprintf(stdout, " dmPaperWidth=%d\n", dm->dmPaperWidth);
    fprintf(stdout, " dmScale=%d\n", dm->dmScale);
    fprintf(stdout, " dmCopies=%d\n", dm->dmCopies);
    fprintf(stdout, " dmDefaultSource=%d\n", dm->dmDefaultSource);
    fprintf(stdout, " dmPrintQuality=%d\n", dm->dmPrintQuality);
    fprintf(stdout, " dmColor=%d\n", dm->dmColor);
    fprintf(stdout, " dmDuplex=%d\n", dm->dmDuplex);
    fprintf(stdout, " dmYResolution=%d\n", dm->dmYResolution);
    fprintf(stdout, " dmTTOption=%d\n", dm->dmTTOption);
    fprintf(stdout, " dmCollate=%d\n", dm->dmCollate);
    fprintf(stdout, " dmFormName=\042%s\042\n", dm->dmFormName);
    fprintf(stdout, " dmLogPixels=%d\n", dm->dmLogPixels);
    fprintf(stdout, " dmBitsPerPel=%d\n", dm->dmBitsPerPel);
    fprintf(stdout, " dmPelsWidth=%d\n", dm->dmPelsWidth);
    fprintf(stdout, " dmPelsHeight=%d\n", dm->dmPelsHeight);
    fprintf(stdout, " dmDisplayFlags=%d\n", dm->dmDisplayFlags);
    fprintf(stdout, " dmDisplayFrequency=%d\n", dm->dmDisplayFrequency);
    GlobalUnlock(hDevMode);
}

void print_devnames(HANDLE hDevNames)
{
    if ((hDevNames == NULL) || (hDevNames == INVALID_HANDLE_VALUE))
	return;
    LPDEVNAMES lpdevnames = (LPDEVNAMES)GlobalLock(hDevNames);
    fprintf(stdout, "DevNames:\n");
    fprintf(stdout, " Device=\042%s\042\n", 
	((char *)lpdevnames) + lpdevnames->wDeviceOffset);
    fprintf(stdout, " Driver=\042%s\042\n", 
	((char *)lpdevnames) + lpdevnames->wDriverOffset);
    fprintf(stdout, " Output=\042%s\042\n", 
	((char *)lpdevnames) + lpdevnames->wOutputOffset);
    GlobalUnlock(hDevNames);
}



HDC query_printer(GSPRINT_OPTION *opt)
{
    PRINTDLG pd;
    memset(&pd, 0, sizeof(pd));
    pd.lStructSize = sizeof(pd);
    pd.hwndOwner = HWND_DESKTOP;
    pd.Flags = PD_RETURNDC;
    if (!get_devmode(opt, &pd.hDevMode, &pd.hDevNames))
	return (HDC)NULL;
    pd.Flags |= PD_NOSELECTION;
    pd.nMinPage = 1;
    pd.nMaxPage = (unsigned int)-1;
    if (opt->from != 0) {
	pd.nFromPage = opt->from;
	pd.nToPage = 999;
	pd.Flags |= PD_PAGENUMS;
    }
    if (opt->to != 0) {
	if (pd.nFromPage == 0)
		pd.nFromPage = 1;
	pd.nToPage = opt->to;
	pd.Flags |= PD_PAGENUMS;
    }

    if (!PrintDlg(&pd))
	return (HDC)NULL;

    if (pd.Flags & PD_PAGENUMS) {
	opt->from = pd.nFromPage;
	opt->to = pd.nToPage;
    }
    if (opt->debug)
	print_devnames(pd.hDevNames);
    if (opt->debug)
	print_devmode(pd.hDevMode);
    GlobalFree(pd.hDevMode);
    GlobalFree(pd.hDevNames);
    pd.hDevMode = NULL;
    pd.hDevNames = NULL;
    return pd.hDC;
}

HDC open_printer(GSPRINT_OPTION *opt)
{
    char *device;
    char *driver;
    char *output;
    HANDLE hDevMode;
    HANDLE hDevNames;
    HDC hdc;

    if (!get_devmode(opt, &hDevMode, &hDevNames))
	    return (HDC)NULL;
    
    LPDEVNAMES lpdevnames = (LPDEVNAMES)GlobalLock(hDevNames);
    device = ((char *)lpdevnames) + lpdevnames->wDeviceOffset;
    driver = ((char *)lpdevnames) + lpdevnames->wDriverOffset;
    output = ((char *)lpdevnames) + lpdevnames->wOutputOffset;

    LPDEVMODE lpdevmode = (LPDEVMODE)GlobalLock(hDevMode);

    hdc = CreateDC(driver, device, NULL, lpdevmode);

    GlobalUnlock(hDevMode);
    GlobalUnlock(hDevNames);

    if (opt->debug)
	print_devnames(hDevNames);
    if (opt->debug)
	print_devmode(hDevMode);

    GlobalFree(hDevMode);
    GlobalFree(hDevNames);

    return hdc;
}

void
show_available_printers(void)
{
    LPBYTE data = NULL;
    DWORD needed, returned;
    int rc;
    unsigned int i;
    PRINTER_INFO_2 *pri2;
    rc = EnumPrinters(PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL, 
	  NULL, 2, data, 0, &needed, &returned);
    if (rc == 0) {
	data = (LPBYTE)malloc(needed);
	rc = EnumPrinters(PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL, 
	      NULL, 2, data, needed, &needed, &returned);
    }
    pri2 = (PRINTER_INFO_2 *)data;
    fprintf(stderr, "Available printers:\n");
    if (rc) {
	for (i=0; i<returned; i++) {
           fprintf(stderr, "  \042%s\042\n", pri2[i].pPrinterName);
	}
    }
    else
	printf("EnumPrinters() failed\n");
    if (data)
	free(data);
}


#ifdef NOTUSED
void CheckProcess( void *dummy )
{
    DWORD exit_status;

    while (GetExitCodeProcess(piProcInfo.hProcess, &exit_status)
    && (exit_status == STILL_ACTIVE)) {
	    Sleep(1000);
	    if (global_debug)
		fprintf(stderr, ".");
    }
    if (global_debug)
	fprintf(stderr, "CheckProcess exiting\n");
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    // We still have a handle to the write end of the pipe
    // Reading the read end of the pipe won't return EOF 
    // until both the child copy of the write handle and
    // our copy of the write handle are closed.
    CloseHandle(hPipeWr);
}
#endif

int main(int argc, char *argv[])
{
    GSPRINT_OPTION opt;

    print_id();
#ifdef BETA
    if (beta())
	return 1;
#endif

    memset(&opt, 0, sizeof(opt));
    opt.args_end = 0;
    opt.args[0] = '\0';
    opt.args[1] = '\0';

    find_gs(opt.gs, sizeof(opt.gs)-1, 600, FALSE);

    // try reading the default config file
    char buf[1024];
    if (!GetModuleFileName(NULL, buf, sizeof(buf)-1))
	return 1;
    char *s = strrchr(buf, '\\');
    if (s) {
	s++;
	strcpy(s, "gsprint.cfg");
    }
    FILE *f = fopen(buf, "r");
    if (f != (FILE *)NULL) {
	if (!read_config(&opt, f)) {
	    fclose(f);
	    return 1;
	}
	fclose(f);
    }

    if (!collect_args(&opt, argc, argv))
       return 1;
    if (!process_args(&opt))
       return 1;

    if (strlen(opt.gs) == 0) {
       fprintf(stdout, "You must use -ghostscript\n");
       return 1;
    }

    if (opt.debug) {
	fprintf(stdout, "Options:\n");
	fprintf(stdout, " Colour=%d\n", opt.colour);
	fprintf(stdout, " Orientation=%d\n", opt.orientation);
	fprintf(stdout, " Duplex=%d\n", opt.duplex);
	fprintf(stdout, " Copies=%d\n", opt.copies);
	fprintf(stdout, " From=%d\n", opt.from);
	fprintf(stdout, " To=%d\n", opt.to);
	fprintf(stdout, " Even/Odd=%d\n", opt.even_odd);
	fprintf(stdout, " Twoup=%d\n", opt.twoup);
	fprintf(stdout, " Query=%d\n", opt.query);
	fprintf(stdout, " Printer=%d\n", opt.printer);
	fprintf(stdout, " Printer Name=\042%s\042\n", opt.printer_name);
	fprintf(stdout, " Ghostscript=\042%s\042\n", opt.gs);
	fprintf(stdout, " Ghostscript Options=\042%s\042\n", opt.options);
	fprintf(stdout, " Document: \042%s\042\n", opt.document_name);
    }

    // Get a printer handle
    HDC hdc = NULL;
    if (opt.query) {
	hdc = query_printer(&opt);
    }
    else {
	hdc = open_printer(&opt);
    }
    if (hdc == (HDC)NULL) {
	fprintf(stderr, "Couldn't open Windows GDI printer driver\n");
	if (!opt.query) {
	    fprintf(stderr, "Requested printer: \042%s\042\n", opt.printer_name);
	    show_available_printers();
	}
	return 1;
    }
    
    // create the pipe for capturing printer output
    /* Set the bInheritHandle flag so pipe handles are inherited. */
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    HANDLE hPipeTemp;
    if (!CreatePipe(&hPipeTemp, &hPipeWr, &saAttr, 0)) {
	fprintf(stderr, "failed to open printer pipe\n");
	    return 1;
    }
    /* make the read handle non-inherited */
    if (!DuplicateHandle(GetCurrentProcess(), hPipeTemp,
	    GetCurrentProcess(), &hPipeRd, 0,
	    FALSE,       /* not inherited */
	    DUPLICATE_SAME_ACCESS)) {
	return 1;
    }
    CloseHandle(hPipeTemp);

    // open printer, get size and resolution
    CPrintDIB printdib;
    LPBYTE p;
    LPBYTE pLine = NULL;
    int length;
    DWORD dwRead;
    DOCINFO di;
    memset(&di, 0, sizeof(DOCINFO));
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = strlen(opt.document_name) ? opt.document_name : "gsprint";
    di.lpszOutput = NULL;
    if (opt.printer_port[0])
	di.lpszOutput = opt.printer_port; /* use specified port */
    if (StartDoc(hdc, &di) == SP_ERROR) {
	DWORD err = GetLastError();
	fprintf(stderr, "StartDoc failed, error code %ld\n", err);
	write_error(err);
	DeleteDC(hdc);
	return 1;
    }

    int width = GetDeviceCaps(hdc, PHYSICALWIDTH);
    int height = GetDeviceCaps(hdc, PHYSICALHEIGHT);
    int xdpi = GetDeviceCaps(hdc, LOGPIXELSX);
    int ydpi = GetDeviceCaps(hdc, LOGPIXELSY);
    int xoff = GetDeviceCaps(hdc, PHYSICALOFFSETX);
    int yoff = GetDeviceCaps(hdc, PHYSICALOFFSETY);
    int hres = GetDeviceCaps(hdc, HORZRES);
    int vres = GetDeviceCaps(hdc, VERTRES);

    if (opt.debug) {
	fprintf(stdout, "PHYSICALWIDTH=%d\n", width);
	fprintf(stdout, "PHYSICALHEIGHT=%d\n", height);
	fprintf(stdout, "PHYSICALOFFSETX=%d\n", xoff);
	fprintf(stdout, "PHYSICALOFFSETY=%d\n", yoff);
	fprintf(stdout, "HORZRES=%d\n", hres);
	fprintf(stdout, "VERTRES=%d\n", vres);
	fprintf(stdout, "LOGPIXELSX=%d\n", xdpi);
	fprintf(stdout, "LOGPIXELSY=%d\n", ydpi);
    }

    // copy all the command line arguments to a buffer
    char command[8192];
    int i;
    if (quote_it(opt.gs)) {
	strcpy(command, "\042");
        strcat(command, opt.gs);
        strcat(command, "\042");
    }
    else
        strcpy(command, opt.gs);
    switch (opt.colour) {
	case MONO:
	    strcat(command, " -sDEVICE=bmpmono");
	    break;
	case GREY:
	    strcat(command, " -sDEVICE=bmpgray");
	    break;
	case COLOUR:
	    strcat(command, " -sDEVICE=bmp16m");
	    break;
    }
    strcat(command, " -dNOPAUSE");
    sprintf(command + strlen(command), " -g%dx%d -r%dx%d",
	width, height, xdpi, ydpi);
    sprintf(command + strlen(command), " -sOutputFile=%%handle%%%08x", hPipeWr);

    strcat(command, opt.options);

   { /* Set the margins so that PDFFitPAge and EPSFitPage work better.
      * This may cause problems here.  It would be better to place it
      * just before the filename, not before other the ghostscript
      * options.
      */
 	char margin[256];
   	sprintf(margin, 
	    " -c \042<< /.HWMargins [%lg %lg %lg %lg] >> setpagedevice\042 -f",
	    (double)xoff / xdpi * 72.0,  			/* left */
	    (double)yoff / ydpi * 72.0,				/* top */
	    (double)(width - xoff - hres) / xdpi * 72.0,	/* right */
	    (double)(height - yoff - vres) / ydpi * 72.0);	/* bottom */
	strcat(command, margin);
    }

    strcat(command, opt.filenames);

    if (opt.twoup) 	// add an extra showpage to eject last odd page
	strcat(command, " -c showpage -f");
    strcat(command, " -c quit");

    if (opt.twoup) {
	// page counts need to be halved for twoup
	if (opt.from)
	    opt.from = opt.from / 2;
	if (opt.to)
	    opt.to = (opt.to+1) / 2;
    }

    if (opt.debug) {
	fprintf(stdout, " Command: %s\n", command);
    }

    // start the program
    if (!exec_prog(command)) {
	fprintf(stderr, "Failed to exec program\n  %s\n",command);
	CloseHandle(hPipeWr);
	CloseHandle(hPipeRd);
	return 1;
    }

    global_debug = opt.debug;
#ifdef NOTUSED
    _beginthread(CheckProcess, 32768, NULL);
#endif

    GFile *pFile = gfile_open_handle((int)hPipeRd);

    // now that program is running, we can close our copy of
    // the pipe write handle
    CloseHandle(hPipeWr);
    hPipeWr = INVALID_HANDLE_VALUE;
  
    int page = 0;
    BOOL print_it;
    printdib.debug = opt.debug_gdi;

    while (printdib.ReadHeader(pFile)) {
	page++;
	print_it = TRUE;
	if ((opt.even_odd == EVEN_PAGES) && ((page & 1) == 1))
	    print_it = FALSE;
	else if ((opt.even_odd == ODD_PAGES) && ((page & 1) == 0))
	    print_it = FALSE;
	if ((opt.from > 0) && (page < opt.from))
	    print_it = FALSE;
	if ((opt.to > 0) && (page > opt.to))
	    print_it = FALSE;
	
	fprintf(stdout, "Page %d, %s\n", page, 
		print_it ? "PRINT" : "ignore");
	if (print_it)
	    StartPage(hdc);
	length = printdib.m_bytewidth;
	pLine = new BYTE[length];
	
	for (i=0; i < printdib.m_PageBmp.bmp2.biHeight; i++) {
	    // read a scan line
	    length = printdib.m_bytewidth;
	    p = pLine;
	    while (length && (dwRead = gfile_read(pFile, p, length)) != 0) {
		length -= dwRead;
		p += dwRead;
	    }
	    if (print_it)
		printdib.AddPrintLine(hdc, i, pLine);
	}
	if (print_it) {
	    printdib.FlushPrintBitmap(hdc);
	    EndPage(hdc);
	}
	delete pLine;
    }

    EndDoc(hdc);
    DeleteDC(hdc);
    gfile_close(pFile);
    Sleep(2000);

    return 0;
}

