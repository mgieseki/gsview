/* Copyright (C) 1996-2001, Ghostgum Software Pty Ltd.  All rights reserved.

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

/* gvwdll.c */
/* GS DLL associated routines for MS-Windows */
#include "gvwin.h"

/* process message queue until something is pending */
void
view_wait_message(VIEW *view)
{
    MSG msg;
    while (!pending.next && !pending.now && !pending.unload && 
	!quitnow && GetMessage(&msg, (HWND)NULL, 0, 0)) {
	if ((hDlgModeless == 0) || !IsDialogMessage(hDlgModeless, &msg)) {
	    if (!TranslateAccelerator(hwndimg, haccel, &msg)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	    }
	}
    }
}

/******************************************************************/

#ifndef ERROR_DLL_NOT_FOUND
#define ERROR_DLL_NOT_FOUND 1157L
#endif

/* display error message for LoadLibrary */
static void
load_library_error(HMODULE hmodule, const char *dllname)
{
char *text_reason;
char buf[MAXSTR+128];
int reason;
LPVOID lpMessageBuffer;
    reason = GetLastError() & 0xffff;
    switch (reason) {
	case ERROR_FILE_NOT_FOUND:	/* 2 */
	    text_reason = "File not found";
	    break;
	case ERROR_PATH_NOT_FOUND:	/* 3 */
	    text_reason = "Path not found";
	    break;
	case ERROR_NOT_ENOUGH_MEMORY:	/* 8 */
	    text_reason = "Not enough memory";
	    break;
	case ERROR_BAD_FORMAT:		/* 11 */
	    text_reason = "Bad EXE or DLL format";
	    break;
	case ERROR_OUTOFMEMORY:		/* 14 */
	    text_reason = "Out of memory";
	    break;
	case ERROR_DLL_NOT_FOUND:	/* 1157 */
	    text_reason = "DLL not found";
	    break;
	default:
	    text_reason = (char *)NULL;
    }
    if (text_reason)
        sprintf(buf, "Failed to load %s, error %d = %s\n", dllname, reason, text_reason);
    else
	sprintf(buf, "Failed to load %s, error %d\n", dllname, reason);
    gs_addmess(buf);

    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM,
	NULL, reason,
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* user default language */
	(LPSTR) &lpMessageBuffer, 0, NULL);
    if (lpMessageBuffer) {
	gs_addmess((LPSTR)lpMessageBuffer);
	gs_addmess("\r\n");
	LocalFree(LocalHandle(lpMessageBuffer));
    }
}

int
gsdll_open(GSDLL *dll, const char *name)
{
const char *shortname;
char fullname[MAX_PATH];
char *p;
    if (debug)
	gs_addmessf("Trying to load %s\n", name);

    /* Try to load DLL first with given path */
    dll->hmodule = LoadLibraryA(name);
    if (dll->hmodule < (HINSTANCE)HINSTANCE_ERROR) {
	/* failed */
	load_library_error(dll->hmodule, name);
	/* try again, with path of EXE */
	if ((shortname = strrchr((char *)name, '\\')) == (const char *)NULL)
	    shortname = name;
	else
	    shortname++;

	GetModuleFileNameA(phInstance, fullname, sizeof(fullname));
	if ((p = strrchr(fullname,'\\')) != (char *)NULL)
	    p++;
	else
	    p = fullname;
	*p = '\0';
	strcat(fullname, shortname);

	if (debug)
	    gs_addmessf("Trying to load %s\n", fullname);

	dll->hmodule = LoadLibraryA(fullname);
	if (gsdll.hmodule < (HINSTANCE)HINSTANCE_ERROR) {
	    /* failed again */
	    load_library_error(gsdll.hmodule, fullname);
	    /* try once more, this time on system search path */
	    if (debug)
		gs_addmessf("Trying to load %s\n", shortname);
	    dll->hmodule = LoadLibraryA(shortname);
	    if (dll->hmodule < (HINSTANCE)HINSTANCE_ERROR) {
		/* failed again */
		load_library_error(dll->hmodule, shortname);
	    }
	}
    }

    if (dll->hmodule < (HINSTANCE)HINSTANCE_ERROR)
	return_error(-1);

    return 0;
}


int
gsdll_close(GSDLL *dll)
{
    FreeLibrary(dll->hmodule);
    return 0;
}


void *
gsdll_sym(GSDLL *dll, const char *name)
{
    return GetProcAddress(dll->hmodule, name);
}

/******************************************************************/


static int poll(void)
{
    if (!multithread)
	peek_message();
    if (pending.abort)
	return -100;	/* signal an error if we want to abort */
    return 0;
}


/* Poll the caller for cooperative multitasking. */
/* If this function is NULL, polling is not needed */
int GSDLLCALL gsdll_poll(void *handle)
{
    return poll();
}



int
get_message(void)
{
MSG msg;
int status;
    if ((status = GetMessage(&msg, (HWND)NULL, 0, 0)) != 0) {
	if ((hDlgModeless == 0) || !IsDialogMessage(hDlgModeless, &msg)) {
	    if (!TranslateAccelerator(hwndimg, haccel, &msg)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	    }
	}
    }
    return status;
}


int
peek_message()
{
MSG msg;
int status;
    while ((status = PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)) != 0) {
	if ((hDlgModeless == 0) || !IsDialogMessage(hDlgModeless, &msg)) {
	    if (!TranslateAccelerator(hwndimg, haccel, &msg)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	    }
	}
    }
    return status;
}


void
wait_event(void)
{
    if (multithread) {
	ResetEvent(display.event);
	WaitForSingleObject(display.event, INFINITE);
    }
}

void 
request_mutex(void)
{
    if (multithread)
	WaitForSingleObject(hmutex_ps, 120000);
}

void 
release_mutex(void)
{
    if (multithread)
	ReleaseMutex(hmutex_ps);
}

/* for pstotext */

int
load_pstotext(void)
{
char dllname[MAXSTR];
char *p;
    /* load pstotext DLL */
    convert_widechar(dllname, szExePath, sizeof(dllname)-1);
    p = strrchr(dllname, '\\');	/* remove trailing backslash */
    if (p)
	*p = '\0';
    p = strrchr(dllname, '\\');	/* remove trailing gsview */
    if (p)
	*(++p) = '\0';
    strcat(dllname, "pstotext\\");
    strcat(dllname, "pstotxt3.dll");
    pstotextModule = LoadLibraryA(dllname);
    if (pstotextModule < (HINSTANCE)HINSTANCE_ERROR) {
	gs_addmess("Can't load ");
        gs_addmess(dllname);
        gs_addmess("\n");
	gs_addmess("Please select Options | PStoText | Disable\n");
	return 1;
    }
    pstotextInit = (PFN_pstotextInit) GetProcAddress(pstotextModule, "pstotextInit");
    if (pstotextInit == (PFN_pstotextInit)NULL) {
	gs_addmess("Can't find pstotextInit() in ");
	gs_addmess(dllname);
        gs_addmess("\n");
	FreeLibrary(pstotextModule);
	return 1;
    }
    pstotextFilter = (PFN_pstotextFilter) GetProcAddress(pstotextModule, "pstotextFilter");
    if (pstotextFilter == (PFN_pstotextFilter)NULL) {
	gs_addmess("Can't find pstotextFilter() in ");
	gs_addmess(dllname);
        gs_addmess("\n");
	FreeLibrary(pstotextModule);
	return 1;
    }
    pstotextExit = (PFN_pstotextExit) GetProcAddress(pstotextModule, "pstotextExit");
    if (pstotextExit == (PFN_pstotextExit)NULL) {
	gs_addmess("Can't find pstotextExit() in ");
	gs_addmess(dllname);
        gs_addmess("\n");
	FreeLibrary(pstotextModule);
	return 1;
    }
    pstotextSetCork = (PFN_pstotextSetCork) GetProcAddress(pstotextModule, "pstotextSetCork");
    if (pstotextSetCork == (PFN_pstotextSetCork)NULL) {
	gs_addmess("Can't find pstotextSetCork() in ");
	gs_addmess(dllname);
        gs_addmess("\n");
	FreeLibrary(pstotextModule);
	return 1;
    }

    pstotextInit(&pstotextInstance);
    pstotextCount = 0;

    return 0;
}

int
unload_pstotext(void)
{
    if (pstotextOutfile)
	fclose(pstotextOutfile);
    pstotextOutfile = (FILE *)NULL;
    if (pstotextInstance)
        pstotextExit(pstotextInstance);
    pstotextInstance = NULL;
    FreeLibrary(pstotextModule);
    pstotextModule = NULL;
    pstotextCount = 0;
    return 0;
}

int
send_pstotext_prolog(HINSTANCE hmodule, int resource)
{  
HGLOBAL hglobal;
LPSTR prolog;
int code = -1;
	hglobal = LoadResource(hmodule, 
	    FindResource(hmodule, (LPTSTR)resource, RT_RCDATA));
	if ( (prolog = (LPSTR)LockResource(hglobal)) != (LPSTR)NULL) {
	    code = gs_execute(prolog, strlen(prolog));
	    FreeResource(hglobal);
	}
	return code;
}

