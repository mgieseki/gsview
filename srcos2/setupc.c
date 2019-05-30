/* Copyright (C) 1996-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* setupc.c */
#ifdef OS2
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
#define LPSTR PCHAR
#else
#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _MSC_VER
#include <direct.h>
#else
#include <dir.h>
#endif
#include <ctype.h>
#endif

#include "gvcver.h"
#include "gvcbeta.h"
#include "gvcrc.h"
#include "os2setup.h"
#include "setupc.h"

char szAppName[]="GSview Setup";

/* flags for items we need to do */
BOOL install_gs = TRUE;
BOOL install_gsview = TRUE;
BOOL install_autoexec= TRUE;		/* update autoexec.bat or config.sys */
BOOL install_autoexecbak = TRUE;	/* keep backup of old autoexec.bak */
BOOL install_group = TRUE;		/* add program manager group */

int batch;		/* don't display windows */
BOOL failed;		/* TRUE if something went wrong */
BOOL cancelling;	/* TRUE if cancel request */
BOOL installing;	/* TRUE if we are currently installing */

/* variables that affect where we install things */
int gsver = GS_REVISION;
char gs_basedir[16];
char gs_zipprefix[16];
char gsviewbase[MAXSTR];

char workdir[MAXSTR];
char bootdrive[MAXSTR];
char sourcedir[MAXSTR];
char destdir[MAXSTR];
char autoexec_bak[MAXSTR];

char unzipname[MAXSTR];
char unzipprefix[MAXSTR];
FILE *unziplogfile;

char get_string_answer[MAXSTR];
char error_message[MAXSTR];

int check_gsver(HWND hwnd);
int check_dir(HWND hwnd);
int make_dir(HWND hwnd);
int done(HWND hwnd);
int finish(HWND hwnd);

HWND hMain = HWND_DESKTOP;
HWND hwnd_current;
HWND hwnd_previous;

WIZPAGE pages[]={
	{IDD_INTRO, IDD_INTRO, IDD_COPYRIGHT, NULL, 0},
	{IDD_COPYRIGHT, IDD_INTRO, IDD_GSVER, NULL, 0},
	{IDD_GSVER, IDD_COPYRIGHT, IDD_DIR, check_gsver, 0},
	{IDD_DIR, IDD_GSVER, IDD_MKDIR, check_dir, 0},
	{IDD_MKDIR, IDD_DIR, IDD_CONFIG, make_dir, 0},
	{IDD_CONFIG, IDD_DIR, IDD_FINISH, NULL, 0},
	{IDD_FINISH, IDD_DIR, IDD_TEXTWIN, finish, 0},
	{IDD_TEXTWIN, IDD_TEXTWIN, IDD_TEXTWIN, NULL, 0},
	{IDD_DONE, IDD_TEXTWIN, IDD_DONE, done, 0},
	{IDD_FAILED, IDD_TEXTWIN, IDD_FAILED, done, 0},
	{0, 0, 0, NULL, 0}
};

/* text window for unzipping */
char twbuf[TWLENGTH];
int twend;

/* Add string for Ghostscript message window */
void
gs_addmess_count(LPSTR str, int count)
{
LPSTR p;
int i, lfcount;
    /* we need to add \r after each \n, so count the \n's */
    lfcount = 0;
    p = str;
    for (i=0; i<count; i++) {
	if (*p == '\n')
	    lfcount++;
	p++;
    }
    if (count + lfcount >= TWSCROLL)
	return;		/* too large */
    if (count + lfcount + twend >= TWLENGTH-1) {
	/* scroll buffer */
	twend -= TWSCROLL;
	memmove(twbuf, twbuf+TWSCROLL, twend);
    }
    p = twbuf+twend;
    for (i=0; i<count; i++) {
	if (*str == '\n') {
	    *p++ = '\r';
	}
	*p++ = *str++;
    }
    twend += (count + lfcount);
    *(twbuf+twend) = '\0';
}

void
gs_addmess(LPSTR str)
{
    gs_addmess_count(str, strlen(str));
}

WIZPAGE *
find_page_from_id(int id)
{
WIZPAGE *page;
    for (page=pages; page->id; page++) {
	if (page->id == id)
	    return page;
    }
    return NULL;
}

void
next_page(HWND hwnd)
{
WIZPAGE *page;
int id;
    for (page=pages; page->id; page++) {
	if (page->hwnd == hwnd) {
	    if (page->func) {
		/* need to execute a function before continuing */
		if ( (id = page->func(hwnd)) != 0) {
		    goto_page(hwnd, id);
		    return;
		}
	    }
	    goto_page(hwnd, page->next);
	    return;
	}
    }
}

void
prev_page(HWND hwnd)
{
WIZPAGE *page;
    for (page=pages; page->id; page++) {
	if (page->hwnd == hwnd) {
	    goto_page(hwnd, page->prev);
	}
    }
}

int
init_temp(void)
{
    if (getenv("TEMP")) {
	install_autoexec = FALSE;
	return 0;	/* assume TEMP is in autoexec.bat */
    }
    return 1;
}

int
add_gsver(HWND hwnd, int offset)
{
char buf[MAXSTR];
int ver;
    GetDlgItemText(hwnd, IDD_GSVER_TEXT, (PBYTE)buf, sizeof(buf));
    if (strlen(buf) == 4)
	ver = (buf[0]-'0')*100 + (buf[2]-'0')*10 + (buf[3]-'0');
    else if (strlen(buf) == 3)
	ver = (buf[0]-'0')*100 + (buf[2]-'0')*10;
    else
	return GS_REVISION;
    ver += offset;
    if (ver > GS_REVISION_MAX)
       ver = GS_REVISION_MAX;
    if (ver < GS_REVISION_MIN)
       ver = GS_REVISION_MIN;
    return ver;
}

int
check_gsver(HWND hwnd)
{
char buf[MAXSTR];
int ver = GS_REVISION;
BOOL fixit = FALSE;
    /* should allow edit field to be changed  */
    /* then make sure it is within range */
    GetDlgItemText(hwnd, IDD_GSVER_TEXT, (PBYTE)buf, sizeof(buf));
    if (strlen(buf) == 4) {
	ver = (buf[0]-'0')*100 + (buf[2]-'0')*10 + (buf[3]-'0');
	if ( (ver > GS_REVISION_MAX) || (ver < GS_REVISION_MIN) )
	    fixit = TRUE;
    }
    else if (strlen(buf) == 3) {
	ver = (buf[0]-'0')*100 + (buf[2]-'0')*10;
	if ( (ver > GS_REVISION_MAX) || (ver < GS_REVISION_MIN) )
	    fixit = TRUE;
    }
    else
	fixit = TRUE;
    if (fixit) {
	ver = GS_REVISION;
	sprintf(buf, "%d.%02d", ver / 100, ver % 100);
	SetDlgItemText(hwnd, IDD_GSVER_TEXT, (PCSZ)buf);
	/* don't move until it is valid */
	return IDD_GSVER;
    }
    
    gsver = ver;
    install_init();
    SetDlgItemText((find_page_from_id(IDD_DIR)->hwnd), IDD_DIR_GSVER, (PCSZ)gs_basedir);
    return 0;
}


int
check_dir(HWND hwnd)
{
int i;
WIZPAGE *page;
    GetDlgItemText(hwnd, ID_ANSWER, (PBYTE)destdir, sizeof(destdir));
    page = find_page_from_id(IDD_MKDIR);
    if (page)
	SetDlgItemText((page->hwnd), IDD_MKDIR_TEXT, (PCSZ)destdir);

    /* check if directory exists */
    i = strlen(destdir) - 1;
    if ((i >= 2) && (destdir[i] == '\\') )
       destdir[i] = '\0';
    if (strlen(destdir)==0)
	return IDD_DIR;

    if (!gs_chdir(destdir)) {
	if (init_temp())
            return IDD_CONFIG;
	else
	    return IDD_FINISH;
    }

    /* if it does not, continue to IDD_MKDIR */
    return 0;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
int
make_dir(HWND hwnd)
{
char buf[MAXSTR];
    if (!mkdirall(destdir)) {
	/* succeeded */
	if (init_temp())
            return 0;
	else
	    return IDD_FINISH;
    }

    /* failed */ 
    load_string(IDS_MKDIRFAIL, buf, sizeof(buf)-1);
    message_box(buf, MB_OK);
    return IDD_DIR;
}

void
zip_message(char *str, int count)
{
int n = 0;
int i;
    gs_addmess_count(str, count);
    gs_addmess_update();
    if (unziplogfile != (FILE *)NULL) {
	/* keep log of installed files, for later uninstalling */
	if (strncmp(str, "  inflating: ", 13)==0)
	    n = 13;
	if (strncmp(str, "  extracting: ", 14)==0)
	    n = 14;
	if (strncmp(str, " extracting: ", 13)==0)
	    n = 13;
	if (n!=0) {
	    fprintf(unziplogfile, "%s\\", unzipprefix);
	    for (i=n; i<count; i++) {
		if (str[i]!='/')
		    fputc(str[i], unziplogfile);
	 	else
		    fputc('\\', unziplogfile);
	    }
	    fwrite("\n", 1, 1, unziplogfile);
	}
    }
    return;
}

int
unzip_to_dir(char *filename, char *destination)
{
    /* start unzip session */  
    char fullname[256];
    FILE *f;
    int file_exists = 0;
    char cwd[256];
    int len;
    int rc;

    /* prompt for disk to be installed */
    strcpy(fullname, sourcedir);
    strcat(fullname, filename);
    while (!file_exists) {
	gs_addmess("Unzipping ");
	gs_addmess(fullname);
	gs_addmess("\n");
        if ( (f = fopen(fullname, "r")) == (FILE *)NULL ) {
	    char buf[256], mess[256];
	    gs_addmess_update();
	    load_string(IDS_INSERTDISK, mess, sizeof(mess));
	    sprintf(buf, mess, fullname);
	    strcpy(get_string_answer, fullname);
	    if (dialog(IDD_FILE, InputDlgProc) != DID_OK) {
		load_string(IDS_ZIPNOTFOUND, error_message, sizeof(error_message));
		return 1;
	    }
	    strcpy(fullname, get_string_answer);
	}
	else {
	    file_exists = TRUE;
	    fclose(f);
	}
    }

    getcwd(cwd, sizeof(cwd));
    gs_chdir(destination);
    strcpy(unzipprefix, destination);
    len = strlen(unzipprefix);
    if (len && (unzipprefix[len-1] == '\\'))
	unzipprefix[len-1] = '\0';
    rc = unzip(fullname);
    gs_chdir(cwd);

    if (!rc) {
        if (cancelling) {
	    load_string(IDS_UNZIPCANCELLED, error_message, sizeof(error_message));
	    return -1;
	}
    }
    return rc;
}


int
do_install(void)
{
int rc=0;
char buf[MAXSTR];
char gsviewdir[MAXSTR];
char gstoolsdir[MAXSTR];
    install_init();

    gs_addmess("Destination ");
    gs_addmess(destdir);
    gs_addmess("\n");
    gs_addmess("Installing GSview: ");
    if (install_gsview)
        gs_addmess("true\n");
    else
        gs_addmess("false\n");
    gs_addmess("Installing Ghostscript: ");
    if (install_gs)
        gs_addmess("true\n");
    else
        gs_addmess("false\n");

    if (!rc) {
	char logname[MAXSTR];
	char *p;

	strcpy(gsviewdir, destdir);
	if (strlen(gsviewdir) == 2)
	    strcat(gsviewdir, "\\");	/* is root directory */
	strcpy(gstoolsdir, gsviewdir);
	if (strlen(gsviewdir) && (gsviewdir[strlen(gsviewdir)-1] != '\\'))
	    strcat(gsviewdir, "\\");
	strcat(gsviewdir, gsviewbase);
	mkdirall(gsviewdir);

	strcpy(logname, gsviewdir);
	strcat(logname, "\\");
	strcat(logname, GSVIEW_ZIP);
	p = strrchr(logname, '.');
	strcpy(p, ".log");
	unziplogfile = fopen(logname, "w");	/* don't append */
	if (unziplogfile == (FILE *)NULL) {
	    load_string(IDS_CANTOPENWRITE, buf, sizeof(buf)); 
	    sprintf(error_message, buf, logname);
	    rc = 1;
	}
    }

    if (install_gs || install_gsview) {
	if (!rc) {
	    /* copy unzip DLL so we don't demand load it from floppy */
	    strcpy(unzipname, gsviewdir);
	    strcat(unzipname, "\\");
	    strcat(unzipname, szUnzipDll);
	    strcpy(buf, sourcedir);
	    strcat(buf, szUnzipDll);
	    rc = copyfile(unzipname, buf);
	}

	if (cancelling)
	    rc = 1;

	/* unzip GSview and Ghostscript */
	if (!rc) {
	    char zipname[MAXSTR];
	    load_unzip(unzipname, phInstance, 
		find_page_from_id(IDD_TEXTWIN)->hwnd, (HWND)NULL);

	    if (!rc && install_gsview) {
		rc = unzip_to_dir(GSVIEW_ZIP, gsviewdir);
		if (cancelling)
		    rc = 1;
	    }
#ifdef OS2
	    if (!rc) {
		/* install EMX if needed */
	        char emxbuf[MAXSTR];
		sprintf(emxbuf, "%s\\", bootdrive);
		if (!emx)
		    rc = unzip_to_dir(EMXZIP, emxbuf);
		if (cancelling)
		    rc = 1;
	    }
#endif

	    if (!rc && install_gs) {
		if (gsver < 600) {
		    sprintf(zipname, "%sini.zip", gs_zipprefix);
		    if (!rc)
			rc = unzip_to_dir(zipname, gstoolsdir);
		    if (cancelling)
			rc = 1;
		}
#ifdef OS2
		sprintf(zipname, "%sos2.zip", gs_zipprefix);
#else
		sprintf(zipname, "%sw32.zip", gs_zipprefix);
#endif
		if (!rc)
		    rc = unzip_to_dir(zipname, gstoolsdir);
		if (cancelling)
		    rc = 1;
		if (gsver < 600) {
		    sprintf(zipname, "%sfn1.zip", gs_zipprefix);
		    if (!rc) {
			strcpy(buf, gstoolsdir);
			if (strlen(buf) && (buf[strlen(buf)-1] != '\\'))
			    strcat(buf, "\\");
			strcat(buf, gs_basedir);
			rc = unzip_to_dir(zipname, buf);
			if (cancelling)
			    rc = 1;
		    }
		}
	    }

	    gs_chdir(workdir);
	    free_unzip();
	}

	/* remove unneeded unzip DLL */
	unlink(unzipname);
    }

    if (!rc && install_autoexec)
	rc = update_config();
    if (cancelling)
	rc = 1;

    if (!rc && install_group)
	rc = create_object();
    if (cancelling)
	rc = 1;

    if (!rc) {
#ifdef OS2
	char szIniName[MAXSTR];
	strcpy(szIniName, bootdrive);
	strcat(szIniName, "\\os2\\gvpm.ini");
#endif
	/* This doesn't work for Windows per user profiles */
	rc = update_ini(szIniName);
    }
    if (cancelling)
	rc = 1;

    if (unziplogfile!=(FILE *)NULL)
	fclose(unziplogfile);

    return rc;
}


/* recursive mkdir */
/* requires a full path to be specified, so ignores root \ */
/* apart from root \, must not contain trailing \ */
/* Examples:
 *  c:\          (OK, but useless)
 *  c:\gstools   (OK)
 *  c:\gstools\  (incorrect)
 *  c:gstools    (incorrect)
 *  gstools      (incorrect)
 * The following UNC names should work,
 * but didn't under Win3.1 because gs_chdir wouldn't accept UNC names
 * Needs to be tested under Windows 95.
 *  \\server\sharename\gstools    (OK)
 *  \\server\sharename\           (OK, but useless)
 */
int
mkdirall(char *dirname)
{
char newdir[MAXSTR];
char *p;
    if (strlen(dirname) < 3)
	return -1;

    if (isalpha((int)(dirname[0])) && dirname[1]==':' && dirname[2]=='\\') {
	/* drive mapped path */
	p = dirname+3;
    }
    else if (dirname[1]=='\\' && dirname[1]=='\\') {
	/* UNC path */
	p = strchr(dirname+2, '\\');	/* skip servername */
	if (p == NULL)
	    return -1;
 	p++;
	p = strchr(p, '\\');		/* skip sharename */
	if (p == NULL)
	    return -1;
    }
    else {
	/* not full path so error */
	return -1;
    }

    while (1) {
	strncpy(newdir, dirname, (int)(p-dirname));
	newdir[(int)(p-dirname)] = '\0';
	if (gs_chdir(newdir)) {
	    if (mkdir(newdir
#ifdef __EMX__
			   , 0
#endif
                              ))
		return -1;
	}
	p++;
        if (p >= dirname + strlen(dirname))
	    break;		/* all done */
	p = strchr(p, '\\');
 	if (p == NULL)
	    p = dirname + strlen(dirname);
    }

    return gs_chdir(dirname);
}



int copyfile(char *dname, char *sname)
{
FILE *dfile, *sfile;
char *buffer;
char mess[MAXSTR];
int count;
#define COPY_BUF_SIZE 16384
    sfile = fopen(sname, "rb");
    if (sfile == (FILE *)NULL) {
	load_string(IDS_CANTOPENREAD, mess, sizeof(mess));
	sprintf(error_message, mess, sname);
	return 1;
    }
    dfile = fopen(dname, "wb");
    if (dfile == (FILE *)NULL) {
	load_string(IDS_CANTOPENWRITE, mess, sizeof(mess));
	sprintf(error_message, mess, dname);
	fclose(sfile);
	return 1;
    }
    if ( (buffer = (char *)malloc(COPY_BUF_SIZE)) == (char *)NULL ) {
	fclose(sfile);
	fclose(dfile);
	load_string(IDS_CANTALLOCBUF, error_message, sizeof(error_message));
	return 1;
    }

    while ( (count = fread(buffer, 1, COPY_BUF_SIZE, sfile)) != 0 )
	fwrite(buffer, 1, count, dfile);

    free(buffer);
    fclose(dfile);
    fclose(sfile);
    return 0;
}

