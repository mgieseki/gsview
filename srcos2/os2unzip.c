/* Copyright (C) 1996-1998, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* os2unzip.c */

#define INCL_DOS
#define INCL_WIN
#define INCL_GPI
#define INCL_DOSERRORS
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LPSTR PCHAR

#ifdef __cplusplus
extern "C" {
#endif
#include "unzip2.h"
#ifdef __cplusplus
}
#endif
#include "setupc.h"
#include "gvcrc.h"

extern HAB hab;
HINSTANCE unzip_hinstance;

#ifdef __cplusplus
extern "C" {
#endif
typedef int (*PFN_UzpAltMain)(int argc, char **argv, UzpInit *init);
typedef UzpVer*  (*PFN_UzpVersion)(void);
PFN_UzpAltMain UnzipAltMain;
PFN_UzpVersion UnzipVersion;
UzpVer *pVersion;
UzpInit unzip_init;
#ifdef __cplusplus
}
#endif

int dot_position;	/* KLUDGE */

int
MessagePrnt(zvoid *pG, uch *buffer, ulg size, int flag)
{
#ifdef OLD
    /* The OS/2 unzip 5.3 DLL has a bug in the msgfn hook. */
    /* It should have been called with _System, but wasn't */
    /* so the arguments are in registers which is wrong for */
    /* a publicly accessible callback function. */

    /* Until the unzip 5.3 bug is fixed, just print a . */
    QMSG q_mess;		/* queue message */
    gs_addmess(". ");			/* KLUDGE */
    dot_position++;			/* KLUDGE */
    if (dot_position >= 32) {		/* KLUDGE */
	dot_position = 0;		/* KLUDGE */
	gs_addmess("\n");		/* KLUDGE */
    }
    gs_addmess_update();		/* KLUDGE */
    while (WinPeekMsg(hab, &q_mess, 0L, 0, 0, PM_REMOVE))
	WinDispatchMsg(hab, &q_mess);
    return size;
#else
    QMSG q_mess;		/* queue message */
    zip_message((char *)buffer, size);
    while (WinPeekMsg(hab, &q_mess, 0L, 0, 0, PM_REMOVE))
	WinDispatchMsg(hab, &q_mess);
    return size;
#endif
}

int free_unzip(void)
{
    if (unzip == (HINSTANCE)NULL)
	return 0;
    DosFreeModule(unzip_hinstance);
    unzip_hinstance = (HINSTANCE)NULL;
    return 0;
}

/* load unzip DLL for gunzip */
int load_unzip(LPSTR lpszDllName, HINSTANCE hInstance, HWND hmain, HWND hlist)
{   
char buf[MAXSTR];
char buf2[MAXSTR];
APIRET rc;
    if (unzip_hinstance != (HINSTANCE)NULL)
	return 0;	/* already loaded */

    strcpy(buf, lpszDllName);
    memset(buf2, 0, sizeof(buf2));
    rc = DosLoadModule((PBYTE)buf2, sizeof(buf2), (PCSZ)buf, &unzip_hinstance);
    if (rc==0) {
	if ((rc = DosQueryProcAddr(unzip_hinstance, 0, (PCSZ)"UzpAltMain", (PFN *)(&UnzipAltMain)))!=0) {
	    free_unzip();
	}
	else {
	    if ((rc = DosQueryProcAddr(unzip_hinstance, 0, (PCSZ)"UzpVersion", (PFN *)(&UnzipVersion)))!=0) {
		free_unzip();
	    }
	}
    }
    else
	unzip_hinstance = (HINSTANCE)NULL;

    if (unzip_hinstance == (HINSTANCE)NULL) {
	char mess[256];
	load_string(IDS_CANTLOAD, mess, sizeof(mess));
	sprintf(buf2, mess, buf);
	gs_addmess(buf2);
	sprintf(buf2, "rc = %d\n", (int)rc);
	gs_addmess_update();
	return 1;
    }

    pVersion = UnzipVersion();
    sprintf(buf2, "Unzip version %d.%d%d\n", pVersion->unzip.major,
        pVersion->unzip.minor, pVersion->unzip.patchlevel);
    gs_addmess(buf2);

    unzip_init.structlen = 16;    /* length of the struct being passed */
    unzip_init.msgfn = MessagePrnt;
    unzip_init.inputfn = NULL;
    unzip_init.pausefn = NULL;
   
    return 0;	/* all is well */
}


int
unzip(char *zipname)
{
    int result;
    char *argv[4];
    int argc = 3;
    argv[0] = "unzip2.dll";
    argv[1] = "-o";
    argv[2] = zipname;
    argv[3] = NULL;
    dot_position = 0;	/* KLUDGE */
    result = UnzipAltMain(argc, argv, &unzip_init);
    gs_addmess("\n");	/* KLUDGE */
    return result;
}
