/* Copyright (C) 1993-1998, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* os2setup.c */
/* OS/2 installation program for GSview and Ghostscript */

#define INCL_DOS
#define INCL_WIN
#define INCL_GPI
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <ctype.h>

#define IDOK DID_OK
#define IDYES MBID_YES
#define IDCANCEL MBID_CANCEL

#ifndef OS2
#define OS2
#endif
#define MAXSTR 256
#define LPSTR PCHAR

#include "gvcver.h"
#include "gvcbeta.h"
#include "gvcprf.h"
#include "gvcrc.h"
#include "os2setup.h"
#include "gvclang.h"
#include "setupc.h"



MRESULT EXPENTRY ModelessDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY MainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

HAB hab;
HINSTANCE phInstance;	/* = hab */
HMODULE hlanguage;
HWND hwnd_dlg;
char szUnzipDll[]="unzip2.dll";
char szIniName[]="gvpm.ini";

/* these should be removed */
char groupname[MAXSTR];
char groupfile[MAXSTR];

char no_error[] = "";
int emx;
int use_os2_fonts;

/* returns TRUE if language change successful */
BOOL
load_language(int language)
{   /* load language dependent resources */
char langdll[MAXSTR];
char buf[MAXSTR];
HMODULE hmodule;
APIRET rc;
    /* load language dependent resources */
    strcpy(langdll, sourcedir);
    strcat(langdll, "setup2");
    switch (language) {
	case IDM_LANGCT:
	    strcat(langdll, "ct");
	    break;
	case IDM_LANGDE:
	    strcat(langdll, "de");
	    break;
	case IDM_LANGES:
	    strcat(langdll, "es");
	    break;
	case IDM_LANGFR:
	    strcat(langdll, "fr");
	    break;
	case IDM_LANGGR:
	    strcat(langdll, "gr");
	    break;
	case IDM_LANGIT:
	    strcat(langdll, "it");
	    break;
	case IDM_LANGNL:
	    strcat(langdll, "nl");
	    break;
	case IDM_LANGRU:
	    strcat(langdll, "ru");
	    break;
	case IDM_LANGSE:
	    strcat(langdll, "se");
	    break;
	case IDM_LANGSK:
	    strcat(langdll, "sk");
	    break;
	case IDM_LANGEN:
	default:
	    hlanguage = 0;
	    return TRUE;
    }
    strcat(langdll, ".dll");
    rc = DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)langdll, &hmodule);
    if (!rc) {
	hlanguage = hmodule;
	return TRUE;
    }

    hlanguage = 0;
    return FALSE;
}

MRESULT EXPENTRY 
LanguageDlgProc(HWND hwnd, ULONG mess, MPARAM mp1, MPARAM mp2)
{
    switch(mess) {
        case WM_COMMAND:
            switch(SHORT1FROMMP(mp1)) {
		case DID_CANCEL:
                case DID_OK:
                    WinDismissDlg(hwnd, 0);
                    break;
		case IDM_LANGEN:
		case IDM_LANGCT:
		case IDM_LANGDE:
		case IDM_LANGES:
		case IDM_LANGFR:
		case IDM_LANGGR:
		case IDM_LANGIT:
		case IDM_LANGNL:
		case IDM_LANGRU:
		case IDM_LANGSE:
		case IDM_LANGSK:
                    WinDismissDlg(hwnd, SHORT1FROMMP(mp1));
            }
            break;
    }
    return WinDefDlgProc(hwnd, mess, mp1, mp2);
}

void 
check_language(void)
{
int language;
COUNTRYCODE pcc;
COUNTRYINFO pci;
ULONG pcbActual;
    pcc.country = 0;	/* ask about default country */
    pcc.codepage = 0;
    if (DosQueryCtryInfo(sizeof(pci), &pcc, &pci, &pcbActual) != 0)
	return;	/* give up */

   if (pcbActual == 0)
	return;

    if (  
	  !((pci.country == 99) || (pci.country == 61) ||
	    (pci.country == 44) || (pci.country == 1))
#ifdef BETA
	|| 1
#endif
       )
    {	/* language isn't English */
	language = WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, LanguageDlgProc, hlanguage, IDD_LANG, NULL);
	switch (language) {
	    case IDM_LANGEN:
	    case IDM_LANGCT:
	    case IDM_LANGDE:
	    case IDM_LANGES:
	    case IDM_LANGFR:
	    case IDM_LANGGR:
	    case IDM_LANGIT:
	    case IDM_LANGNL:
	    case IDM_LANGRU:
	    case IDM_LANGSE:
	    case IDM_LANGSK:
		load_language(language);
	}
    }
}

int
dialog(int resource, PFNWP dlgproc)
{
    return WinDlgBox(HWND_DESKTOP, HWND_DESKTOP, dlgproc, hlanguage, resource, 0);
}

int message_box(const char *str, int icon)
{
    return WinMessageBox(HWND_DESKTOP, HWND_DESKTOP,
                 (PCSZ)str, (PCSZ)szAppName, 0, icon);
}


int 
gs_chdir(char *dirname)
{
#ifdef __BORLANDC__
	if (isalpha(dirname[0]) && (dirname[1]==':'))
		(void) setdisk(toupper(dirname[0])-'A');
	if (!((strlen(dirname)==2) && isalpha(dirname[0]) && (dirname[1]==':')))
		chdir(dirname);
	return TRUE;
#else
#ifdef __IBMC__
	if (isalpha(dirname[0]) && (dirname[1]==':'))
	    if (_chdrive(toupper(dirname[0])-'A'+1))
		return -1;
	if (!((strlen(dirname)==2) && isalpha(dirname[0]) && (dirname[1]==':')))
	    return _chdir(dirname);
#else
	if (isalpha((int)(dirname[0])) && (dirname[1]==':'))
	    if (_chdrive(dirname[0]))
		return -1;
	return _chdir2(dirname);
#endif
#endif
}

int
load_string(int id, char *str, int len)
{
	return WinLoadString(hab, hlanguage, id, len, (PBYTE)str);
}

/* Update MLE to match twbuf */
void
gs_addmess_update(void)
{
IPT ipt = 0;
HWND hwndmess;
HWND hwnd_textwin = find_page_from_id(IDD_TEXTWIN)->hwnd;

    if (hwnd_textwin == (HWND)NULL)
	return;
    hwndmess = WinWindowFromID(hwnd_textwin, TEXTWIN_MLE);
    if (hwndmess == (HWND)NULL)
	return;

    WinEnableWindowUpdate(hwndmess, FALSE);
    WinSendMsg( hwndmess, MLM_DISABLEREFRESH, 0, 0);
    /* delete current contents */
    ipt = (IPT)WinSendMsg( hwndmess, MLM_QUERYTEXTLENGTH, 0, 0); /* get current length */
    WinSendMsg( hwndmess, MLM_SETSEL, (MPARAM)0, (MPARAM)ipt);
    WinSendMsg( hwndmess, MLM_CLEAR, (MPARAM)0, (MPARAM)0);
    /* add udpated buffer */
    ipt = 0;
    WinSendMsg(hwndmess, MLM_SETIMPORTEXPORT, (MPARAM)twbuf, (MPARAM)sizeof(twbuf));
    WinSendMsg( hwndmess, MLM_IMPORT, (MPARAM)&ipt, (MPARAM)twend);
    WinSendMsg( hwndmess, MLM_SETSEL, (MPARAM)twend, (MPARAM)twend);
    WinSendMsg( hwndmess, MLM_ENABLEREFRESH, 0, 0);
    WinEnableWindowUpdate(hwndmess, TRUE);
    WinInvalidateRect(hwndmess, (PRECTL)NULL, TRUE);
    WinUpdateWindow(hwndmess);
}


void
goto_page(HWND hwnd, int id)
{
WIZPAGE *page;
HWND hbutton;
    page = find_page_from_id(id);
    if (page) {
	WinShowWindow(hwnd, FALSE);		/* hide window */
	WinShowWindow(page->hwnd, TRUE);	/* show window */
	hwnd_current = page->hwnd;
	if (WinIsWindowEnabled(WinWindowFromID(page->hwnd, IDNEXT)))
	    hbutton = WinWindowFromID(page->hwnd, IDNEXT);
	else
	    hbutton = WinWindowFromID(page->hwnd, IDCANCEL);
	WinSendMsg(hbutton, BM_SETDEFAULT, (MPARAM)TRUE , (MPARAM)0);
	WinSetFocus(HWND_DESKTOP, hbutton);
	return;
    }
}

#pragma argsused
/* exit from program */
int
done(HWND hwnd)
{
    WinPostMsg(hwnd, WM_QUIT, (MPARAM)0, (MPARAM)0);
    return 0;
}

/* commence installation */
int
finish(HWND hwnd)
{
WIZPAGE *page;
char groupname[MAXSTR];
    page = find_page_from_id(IDD_GSVER);
    install_gsview = (BOOL)WinSendMsg( WinWindowFromID(page->hwnd, PARTIAL_GSVIEW),
	BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));
    install_gs =     (BOOL)WinSendMsg( WinWindowFromID(page->hwnd, PARTIAL_GS),
	BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));

    /* assume gsver is already correct */

    page = find_page_from_id(IDD_CONFIG);
    install_autoexec = (BOOL)WinSendMsg( WinWindowFromID(page->hwnd, IDM_AUTOEXECBAT),
	BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));
    install_autoexecbak = (BOOL)WinSendMsg( WinWindowFromID(page->hwnd, IDM_AUTOEXECBAK),
	BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));
    /* assume destdir is already correct */

    page = find_page_from_id(IDD_FINISH);
    install_group = (BOOL)WinSendMsg( WinWindowFromID(page->hwnd, IDM_PROGMAN1),
	BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0));
    WinQueryWindowText( WinWindowFromID(page->hwnd, IDM_PROGMAN2), sizeof(groupname), (PBYTE)groupname);

    WinPostMsg(hwnd, WM_COMMAND, (MPARAM)IDFINISH, (MPARAM)0);
    return 0;
}



int
cleanup(void)
{
    return 0;
}


int
update_config(void)
{
FILE *infile, *outfile;
char inname[MAXSTR], outname[MAXSTR];
char line[1024];
char tempname[MAXSTR];
char buf[MAXSTR];
BOOL got_temp = FALSE;
BOOL changed = FALSE;
int i;
    
    strcpy(inname, bootdrive);
    strcat(inname, "\\config.sys");

    strcpy(tempname, bootdrive);
    strcat(tempname, "\\GSXXXXXX");
    if (mktemp(tempname) == (char *)NULL) {
	load_string(IDS_CANTCREATETEMPFILE, error_message, sizeof(error_message));
	return 1;
    }

    if ( (infile = fopen(inname, "r")) == (FILE *)NULL) {
	load_string(IDS_CANTOPENREAD, buf, sizeof(buf));
	sprintf(error_message, buf, inname);
	return 1;
    }
    if ( (outfile = fopen(tempname, "w")) == (FILE *)NULL)  {
	load_string(IDS_CANTOPENWRITE, buf, sizeof(buf));
	sprintf(error_message, buf, outname);
	return 1;
    }
    while (fgets(line, sizeof(line), infile)) {
	if (strnicmp(line, "SET TEMP=", 9)==0) {
	    /* it has been added recently */
	    got_temp = TRUE;
	}
        if (strncmp(line, "LIBPATH=", 8) == 0) {
	    if (!emx) {
		sprintf(line+strlen(line)-1, ";%s\\emx\\dll\n", bootdrive);
		changed = TRUE;
	    }
	}
        if (strncmp(line, "SET PATH=", 9) == 0) {
	    if (!emx) {
		sprintf(line+strlen(line)-1, ";%s\\emx\\bin\n", bootdrive);
		changed = TRUE;
	    }
	}
	fputs(line, outfile);
    }
    if (!got_temp) {
	sprintf(line, "SET TEMP=%s\\\n", bootdrive);
	changed = TRUE;
    }
    fputs(line, outfile);

    fclose(outfile);
    fclose(infile);

    if (!changed) {
	/* no changes were necessary, so bail out */
	unlink(tempname);
	return 0;
    }

    if (install_autoexecbak) {
	for (i=0; i<=999; i++) {
	    sprintf(outname, "%s\\config.%03d", bootdrive, i);
	    if ( (infile = fopen(outname, "r")) != (FILE *)NULL)
		fclose(infile);
	    else
		break;   /* found a suitable name */
	}
        if (rename(inname, outname)) {
	    load_string(IDS_ERRORRENAME, buf, sizeof(buf));
	    sprintf(error_message, buf, inname, outname);
	    return 1;
	}
	strcpy(autoexec_bak, outname);
    }
    else
	unlink(inname);

    if (rename(tempname, inname)) {
	load_string(IDS_ERRORRENAME, buf, sizeof(buf));
	sprintf(error_message, buf, tempname, inname);
	return 1;
    }

    return 0;
}

int
update_ini(char *ininame)
{
PROFILE *prf;
char buf[16];
char secver[64];
    prf = profile_open(ininame);
    if (prf == (PROFILE *)NULL) {
	char mess[MAXSTR];
	load_string(IDS_CANTOPENREAD, mess, sizeof(mess));
	sprintf(error_message, mess, ininame);
	return 1;
    }
    sprintf(secver, "GSview-%s", GSVIEW_DOT_VERSION);
    profile_write_string(prf, secver, "Configured", "0");
    sprintf(buf, "%3d", gsver);
    profile_write_string(prf, secver, "GSversion", buf);
    profile_close(prf);
    return 0;
}

int
create_object(void)
{
char buf[MAXSTR];
char setup[MAXSTR];
APIRET rc;
    strcpy(buf, destdir);
    strcat(buf, "\\");
    strcat(buf, gsviewbase);
    strcat(buf, "\\gvpm.exe");
    sprintf(setup, "EXENAME=%s;ASSOCFILTER=*.ps,*.eps,*.pdf", buf);
    rc = !WinCreateObject((PCSZ)"WPProgram", (PCSZ)"GSview", (PCSZ)setup, 
	(PCSZ)"<WP_DESKTOP>", CO_REPLACEIFEXISTS);
    if (rc)
	load_string(IDS_PROGRAMOBJECTFAILED, error_message, sizeof(error_message));
    else {
	load_string(IDS_PROGMANGROUP6, setup, sizeof(setup));
	sprintf(buf, setup, groupname);
	SetDlgItemText(find_page_from_id(IDD_DONE)->hwnd, IDD_DONE_GROUP, (PCSZ)buf);
    }
    return rc;
}

int
create_dialog(void)
{
WIZPAGE *page;
char buf[MAXSTR];
    /* main dialog box */
    hMain = WinLoadDlg(HWND_DESKTOP, HWND_DESKTOP, MainDlgProc, hlanguage, IDD_MAIN, 0);

    sprintf(buf, "%d.%02d - %d.%02d", 
	GS_REVISION_MIN / 100, GS_REVISION_MIN % 100,
	GS_REVISION_MAX / 100, GS_REVISION_MAX % 100);
    WinSetWindowText( WinWindowFromID(find_page_from_id(IDD_INTRO)->hwnd, IDD_INTRO_T3), (PCSZ)buf);

    /* initialize GS version */
    page = find_page_from_id(IDD_GSVER);
    if (page) {
	sprintf(buf, "%d.%02d", gsver / 100, gsver % 100);
        WinSetWindowText( WinWindowFromID(page->hwnd, IDD_GSVER_TEXT), (PCSZ)buf);
	WinSendMsg( WinWindowFromID(page->hwnd, PARTIAL_GSVIEW),
	    BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
	WinSendMsg( WinWindowFromID(page->hwnd, PARTIAL_GS),
	    BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
    }
    { /* put scroll thumb in middle to enable both arrows */
	HWND hwndScroll = WinWindowFromID(page->hwnd, IDD_GSVER_SCROLL);
	WinSendMsg(hwndScroll, SBM_SETSCROLLBAR, MPFROMLONG(1), 
		    MPFROM2SHORT(0, 2));
    }

    /* initialize destination directory */
    strcpy(destdir, bootdrive);
    strcat(destdir, INSTALL_DIR);
    WinSetWindowText( WinWindowFromID(find_page_from_id(IDD_DIR)->hwnd, ID_ANSWER), (PCSZ)destdir);

    if (!emx || init_temp()) {
        page = find_page_from_id(IDD_CONFIG);
	if (page) {
	    WinSendMsg( WinWindowFromID(page->hwnd, IDM_AUTOEXECBAT),
		BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
	    WinSendMsg( WinWindowFromID(page->hwnd, IDM_AUTOEXECBAT),
		BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
	    WinSendMsg( WinWindowFromID(page->hwnd, IDM_AUTOEXECBAK),
		BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
	}
	find_page_from_id(IDD_FINISH)->prev = IDD_CONFIG; 
    }

    /* program group */
    load_string(IDS_PROGMANGROUP4, buf, sizeof(buf));
    page = find_page_from_id(IDD_FINISH);
    if (page) {
	WinSendMsg( WinWindowFromID(page->hwnd, IDM_PROGMAN1),
	    BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
        WinSetWindowText( WinWindowFromID(page->hwnd, IDM_PROGMAN2), (PCSZ)buf);
    }

    /* try again to set the focus where we want it */
    WinSetFocus(HWND_DESKTOP, WinWindowFromID(pages[0].hwnd, IDNEXT));

    return 0; /* success */
}

void
install_init(void)
{
int i;
char *s, *d;
    sprintf(gs_basedir, "gs%d.%02d", gsver / 100, gsver % 100);
    sprintf(gs_zipprefix, "gs%3d", gsver);

    /* derive group filename from group name */
    for (i=0, s=groupname, d=groupfile; i<8 && *s; s++) {
	if (isalpha((int)(*s)) || isdigit((int)(*s))) {
	    *d++ = *s;
	    i++;
	} 
    }
    *d = '\0';
    if (strlen(groupfile)==0)
	strcpy(groupfile, "gstools");
}

BOOL gszip_exists(int ver)
{
    FILE *f;
    char name[256];
    sprintf(name, "%sgs%03dos2.zip", sourcedir, ver);
    if ( (f = fopen(name, "r")) != (FILE *)NULL ) {
	fclose(f);
	return TRUE;
    }
    return FALSE;
}

/* Try to find if we are being asked to install a different
 * version of Ghostscript than GS_REVISION.
 * Do this by trying to open gsNNNos2.zip for other version
 * numbers.
 */ 
void
find_gszip(void)
{
    int i;
    gsver = GS_REVISION;

    /* try to find a Ghostscript OS/2 zip file */
    for (i=gsver; i <= GS_REVISION_MAX; i++) {
	if (gszip_exists(i)) {
	    gsver = i;
	    return;	/* got one */
	}
    }

    /* try a couple of earlier versions */
    if (gszip_exists(550)) {
	gsver = 550;
	return;
    }
    if (gszip_exists(510)) {
	gsver = 510;
	return;
    }

}


int
init_setup(LPSTR lpszCmdLine)
{
char *p;
LPSTR d, s;

char buf[256];
APIRET rc = 0;

PTIB pptib;
PPIB pppib;
HMODULE hmodule;

    if (lpszCmdLine[0] != '\0') {
	d = destdir;
	s = lpszCmdLine;
	if (*s == '\042')
	    s++; 		/* don't copy quotes */
	while (*s) {
	    if (*s == '\042')
		s++; 	/* don't copy quotes */
	    else
		*d++ = *s++;
	    if (d - destdir > sizeof(destdir) - 1) {
	        *d = '\0';
		break;
	    }
	}
	batch = TRUE;
    }

    /* get path to EXE */
    if ( (rc = DosGetInfoBlocks(&pptib, &pppib)) != 0 ) {
	sprintf(error_message, "DosGetInfoBlocks rc=%ld", rc);
	return rc;
    }

    if ( (rc = DosQueryModuleName(pppib->pib_hmte, sizeof(sourcedir), sourcedir)) != 0 ) {
	sprintf(error_message, "DosQueryModuleName rc=%ld", rc);
	return rc;
    }

    if ((p = strrchr(sourcedir,'\\')) != (char *)NULL)
	p++;
    else
	p = sourcedir;
    *p = '\0';

    /* Inspect system, get boot drive */
    getcwd(workdir, sizeof(workdir));	/* remember the working directory */
    strcpy(bootdrive, getenv("USER_INI"));
    bootdrive[2] = '\0';

    if ( !(rc = DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)"EMX", &hmodule)) ) {
	emx = TRUE;
	DosFreeModule(hmodule);
    }

    /* Set the initial Ghostscript revision (gsver) */
    find_gszip();

    return 0;
}

MRESULT EXPENTRY InputDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  switch(msg) {
    case WM_INITDLG:
	WinSendMsg( WinWindowFromID(hwnd, ID_ANSWER),
	    	EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	WinSetWindowText( WinWindowFromID(hwnd, ID_ANSWER),
	  	(PCSZ)get_string_answer );
    	break;
    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1)) {
        case DID_OK:
	  WinEnableWindow(WinWindowFromID(hwnd, DID_OK), FALSE);
	  WinQueryWindowText(WinWindowFromID(hwnd, ID_ANSWER),
		MAXSTR, (PBYTE)get_string_answer);
          WinDismissDlg(hwnd, DID_OK);
          return (MRESULT)TRUE;
	case DID_CANCEL:
          WinDismissDlg(hwnd, DID_CANCEL);
          return (MRESULT)TRUE;
      }
      break;
  }
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
}	


MRESULT EXPENTRY MainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
static BOOL initialised = FALSE;
  switch(msg) {
    case WM_INITDLG:
	/* create child dialog windows */
	if (!initialised) {
	    WIZPAGE *page;
	    initialised=TRUE;
	    for (page=pages; page->id; page++) {
		page->hwnd = WinLoadDlg(hwnd, hwnd, ModelessDlgProc,
		    hlanguage, page->id, 0);
		WinShowWindow(page->hwnd, FALSE);
	    }
	    page = find_page_from_id(IDD_DIR);
	    if (page) {
	        WinSendMsg( WinWindowFromID(page->hwnd, ID_ANSWER),
		    EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	    }
	    page = find_page_from_id(IDD_FINISH);
	    if (page) {
	        WinSendMsg( WinWindowFromID(page->hwnd, IDM_PROGMAN2),
		    EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	    }
	    WinShowWindow(pages[0].hwnd, TRUE);
	    WinSetFocus(HWND_DESKTOP, WinWindowFromID(pages[0].hwnd, IDNEXT));
	    WinSendMsg(WinWindowFromID(pages[0].hwnd, IDNEXT), 
		    BM_SETDEFAULT, (MPARAM)TRUE , (MPARAM)0);
	    hwnd_current = pages[0].hwnd;
	}
    	break;
    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1)) {
        case DID_OK:
	case DID_CANCEL:
	  WinPostMsg(hwnd, WM_QUIT, (MPARAM)0, (MPARAM)0);
          return (MRESULT)TRUE;
      }
      break;
    case WM_CLOSE:
      WinPostMsg(hwnd, WM_QUIT, (MPARAM)0, (MPARAM)0);
      return (MRESULT)TRUE;
      break;
  }
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
}	


MRESULT EXPENTRY ModelessDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  switch(msg) {
    case WM_INITDLG:
    	break;
    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1)) {
	case IDOK:
	case IDNEXT:
	    next_page(hwnd);
	    return(MRESULT)TRUE;
	case IDPREV:
	    /* remove default style */
	    WinSendMsg(WinWindowFromID(hwnd, IDPREV), BM_SETDEFAULT, 
		(MPARAM)FALSE , (MPARAM)0);
	    prev_page(hwnd);
	    return(MRESULT)TRUE;
	case IDFINISH:
	    installing = TRUE;
	    if (do_install() || cancelling) {
		WIZPAGE *page;
		/* FAILED */
		failed = TRUE;
		page = find_page_from_id(IDD_TEXTWIN);
		page->next = IDD_FAILED;	/* KLUDGE */
		WinEnableWindow(WinWindowFromID(page->hwnd, IDNEXT), TRUE);
		WinSetWindowText( WinWindowFromID(find_page_from_id(IDD_FAILED)->hwnd, 
		   IDD_FAILED_REASON), (PCSZ)error_message); 
		gs_addmess(error_message);
		gs_addmess_update();
	    }
	    else {
		WIZPAGE *page;
		page = find_page_from_id(IDD_TEXTWIN);
		page->next = IDD_DONE;	/* KLUDGE */
		WinEnableWindow(WinWindowFromID(page->hwnd, IDNEXT), TRUE);
	    }
	    installing = FALSE;
	    if (autoexec_bak[0] != '\0') {
		char buf1[MAXSTR], buf2[MAXSTR];
		load_string(IDS_DONEBAK, buf1, sizeof(buf2));
		sprintf(buf2, buf1, autoexec_bak);
		WinSetWindowText( WinWindowFromID(find_page_from_id(IDD_DONE)->hwnd, 
		   IDD_DONE_BAK), (PCSZ)buf2); 
	    }
	    WinPostMsg(find_page_from_id(IDD_TEXTWIN)->hwnd,
		WM_COMMAND, MPFROMSHORT(IDNEXT), (MPARAM)0);
	    return(MRESULT)TRUE;
	case IDCANCEL:
	    if (installing) {
		gs_addmess("\ncancelling\n");
		cancelling = 1;
	    }
	    else
		WinPostMsg(WinQueryWindow(hwnd, QW_PARENT), WM_COMMAND, MPFROMSHORT(IDCANCEL), (MPARAM)0);
	    return(MRESULT)TRUE;
	case TEXTWIN_COPY:
	    {MRESULT mr; 
	    mr = WinSendMsg( WinWindowFromID(hwnd, TEXTWIN_MLE),
		MLM_QUERYSEL, (MPARAM)MLFQS_MINMAXSEL, (MPARAM)0);
	    if (SHORT1FROMMP(mr) == SHORT2FROMMP(mr)) /* no selection so select all */
		WinSendMsg( WinWindowFromID(hwnd, TEXTWIN_MLE),
		    MLM_SETSEL, 0, (MPARAM)strlen(twbuf));
	    WinSendMsg( WinWindowFromID(hwnd, TEXTWIN_MLE),
		MLM_COPY, (MPARAM)0, (MPARAM)0);
	    WinSendMsg( WinWindowFromID(hwnd, TEXTWIN_MLE),
		MLM_SETSEL, (MPARAM)strlen(twbuf), (MPARAM)strlen(twbuf));
	    }
	    return(MRESULT)TRUE;
	default:
	    return(MRESULT)FALSE;
      }
      break;
    case WM_VSCROLL:
      /* handle scrolling GS version */
      { int ver;
	char buf[16];
	  switch(SHORT2FROMMP(mp2)) {
	      case SB_LINEUP:
		  ver = add_gsver(hwnd, 1);
		  break;
	      case SB_LINEDOWN:
		  ver = add_gsver(hwnd, -1);
		  break;
	      default:
		  ver = add_gsver(hwnd, 0);
		  break;
	  }
	  sprintf(buf, "%d.%02d", ver / 100, ver % 100);
	  WinSetWindowText( WinWindowFromID(hwnd, IDD_GSVER_TEXT), (PCSZ)buf); 
      }
      return(MRESULT)TRUE;
  }
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
}	


int
main(int argc, char *argv[])
{
    HMQ hand_mq;		/* message queue */
    QMSG q_mess;		/* queue message */
    APIRET rc = 0;

    hab = WinInitialize(0);	/* Get the Anchor Block */
    phInstance = hab;
    hlanguage = 0;

    hand_mq = WinCreateMsgQueue(hab, 0); /* start a queue */

    if (argc==2)
        init_setup(argv[1]);
    else
        init_setup("");

    check_language();

    load_string(IDS_GSVIEWBASE, gsviewbase, sizeof(gsviewbase));
    load_string(IDS_PROGMANGROUP4, groupname, sizeof(groupname));

    if (!beta_warn()) {
	if (!rc) {
	    if (batch) {
		rc = do_install();
	    }
	    else
		rc = create_dialog();
	}

	if (!batch) {
	    while (!rc && WinGetMsg(hab, &q_mess, 0L, 0, 0))
		WinDispatchMsg(hab, &q_mess);
	    WinDestroyWindow(hMain);
	}
    }

    if (hlanguage)
	DosFreeModule(hlanguage);

    WinDestroyMsgQueue(hand_mq);
    WinTerminate(hab);
    return rc;
}

