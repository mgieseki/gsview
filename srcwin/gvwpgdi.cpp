/* Copyright (C) 2000, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

// gvwpgdi.cpp - Print using Windows GDI

// Tells Ghostscript to output using a BMP device and write 
// to a pipe using -sOutputFile="%handle%XXXXXXXX".
// We read from the pipe and write GDI commands to the
// actual printer.

extern "C" {
#include "gvwin.h"
}
#include "gvwdib.h"
#include "gvwpdib.h"

// Nasty globals
int print_gdi_width;
int print_gdi_height;
int print_gdi_xdpi;
int print_gdi_ydpi;
HANDLE print_gdi_read_handle;
HANDLE print_gdi_write_handle;


#ifdef NOTUSED
typedef struct tagPGDI_CHECK {
    HANDLE hPipeWr;
    PROCESS_INFORMATION piProcInfo;
} PGDI_CHECK;
#endif


typedef struct tagPGDI_THREAD {
    int from;		// psfile.print_from
    int to;		// psfile.print_to
    int oddeven;	// psfile.print_oddeven
    HDC hdc;		// pd.hDC
    HANDLE hPipeRd; 	/* We read printer output from this one */
} PGDI_THREAD;



// Background part as separate thread
// Don't do any GUI things on this thread.
void print_gdi_thread(void *pdummy)
{
    PGDI_THREAD *pth= (PGDI_THREAD *)pdummy;
    CPrintDIB printdib;
    char buf[MAXSTR];
    int length;
    LPBYTE pLine;
    LPBYTE p;
    DWORD dwRead;
    int i;

    GFile *pFile = gfile_open_handle((int)pth->hPipeRd);

    int page = 0;
    BOOL print_it;
    printdib.debug = (debug & DEBUG_GDI);

    while (printdib.ReadHeader(pFile)) {
	page++;
	print_it = TRUE;
	if ((pth->oddeven == EVEN_PAGES) && ((page & 1) == 1))
		print_it = FALSE;
	else if ((pth->oddeven == ODD_PAGES) && ((page & 1) == 0))
		print_it = FALSE;
	if ((pth->from > 0) && (page < pth->from))
		print_it = FALSE;
	if ((pth->to > 0) && (page > pth->to))
		print_it = FALSE;
	
	sprintf(buf, "Page %d, %s\n", page, print_it ? "PRINT" : "ignore");
	gs_addmess(buf);
	if (print_it)
	    StartPage(pth->hdc);
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
		    printdib.AddPrintLine(pth->hdc, i, pLine);
	}
	if (print_it) {
		printdib.FlushPrintBitmap(pth->hdc);
		EndPage(pth->hdc);
	}
	delete pLine;
    }

    EndDoc(pth->hdc);
    DeleteDC(pth->hdc);
    pth->hdc = NULL;
    gfile_close(pFile);

    free(pth);

    print_count--;

    /* if printing from command line, close GSview */
    if (print_exit && (print_count==0) && !(debug & DEBUG_GDI))
	gsview_command(IDM_EXIT);

    gs_addmess("\nPrint GDI finished\n");
}



/* Start child program with inherited pipe */
BOOL start_gvwgs_with_pipe(HDC hdc)
{
    STARTUPINFO siStartInfo;
    LPVOID env;
    TCHAR wcommand[MAXSTR+MAXSTR];
    TCHAR wgsdll[MAXSTR];
    TCHAR woptname[MAXSTR];
    TCHAR wpsname[MAXSTR];

    convert_multibyte(wgsdll, option.gsdll, 
	sizeof(wgsdll)/sizeof(TCHAR)-1);
    convert_multibyte(woptname, printer.optname, 
	sizeof(woptname)/sizeof(TCHAR)-1);
    convert_multibyte(wpsname, printer.psname, 
	sizeof(wpsname)/sizeof(TCHAR)-1);

    PGDI_THREAD *pth= (PGDI_THREAD *)malloc(sizeof(PGDI_THREAD));
    if (pth == NULL) {
	gs_addmess("Failed to allocate PGDI_THREAD\n");
	return FALSE;
    }

    if (psfile.dsc != (CDSC *)NULL && !psfile.print_ignoredsc) {
	// PostScript file contains only needed pages
	// Print all
	pth->from = 1;
	pth->to = 9999;
	pth->oddeven = ALL_PAGES;
    }
    else {
	// we render all pages, and print only some
	pth->from = psfile.print_from;
	pth->to = psfile.print_to;
	pth->oddeven = psfile.print_oddeven;
    }
    pth->hdc = hdc;
    pth->hPipeRd = print_gdi_read_handle;

    wsprintf(wcommand, TEXT("\042%s%s\042 %s \042%s\042 \042%s\042 \042%s\042"),
	szExePath,
#ifdef DECALPHA
	TEXT("gvwgsda.exe"),
#else
# ifdef _WIN64
	TEXT("gvwgs64.exe"),
# else
	TEXT("gvwgs32.exe"),
# endif
#endif
	debug ? TEXT("/d") : TEXT(""),
	wgsdll, woptname, wpsname);
	

    info_wait(IDS_WAIT);


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
    siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
//    siStartInfo.wShowWindow = SW_SHOWNORMAL;	
    siStartInfo.wShowWindow = SW_SHOWMINNOACTIVE;	
    siStartInfo.cbReserved2 = 0;
    siStartInfo.lpReserved2 = NULL;
    siStartInfo.hStdInput = NULL;
    siStartInfo.hStdOutput = NULL;
    siStartInfo.hStdError = NULL;

    env = NULL;

    /* Create the child process. */
    PROCESS_INFORMATION piProcInfo;

    if (!CreateProcess(NULL,
        wcommand,      /* command line                       */
        NULL,          /* process security attributes        */
        NULL,          /* primary thread security attributes */
        TRUE,          /* handles are inherited              */
        0,             /* creation flags                     */
        env,           /* environment                        */
        NULL,          /* use parent's current directory     */
        &siStartInfo,  /* STARTUPINFO pointer                */
	&piProcInfo)	/* receives PROCESS_INFORMATION  */
	) {
        char command[MAXSTR+MAXSTR];
	// cleanup items created by gsviev_cprint()
	if (!debug)
	    unlink(printer.psname);
	printer.psname[0] = '\0';
	if (!debug)
	    unlink(printer.optname);
	printer.optname[0] = '\0';
	// also items PRINT_GDI items
	CloseHandle(print_gdi_read_handle);
	print_gdi_read_handle = NULL;
	CloseHandle(print_gdi_write_handle);
	print_gdi_write_handle = NULL;
	EndDoc(pth->hdc);
	free(pth);
	info_wait(IDS_NOWAIT);
	convert_widechar(command, wcommand, sizeof(command)-1);
	gserror(IDS_CANNOTRUN, command, MB_ICONHAND, SOUND_ERROR);
	return FALSE;
    }

    WaitForInputIdle(piProcInfo.hProcess, 30000);
    info_wait(IDS_NOWAIT);

    /* we shouldn't exit GSview until print_gdi_thread finishes
     * and print_count becomes 0.
     */
    print_count++;	

    _beginthread(print_gdi_thread, 65536, (void *)pth);
    // we don't need to know anything more */
    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    // Now that Ghostscript has started, close our copy of the write
    // handle so that pipe will break when Ghostscript closes its 
    // write handle.
    CloseHandle(print_gdi_write_handle);

    return TRUE;
}

BOOL
init_print_gdi(HDC hdc)
{
    // create the pipe for capturing printer output
    /* Set the bInheritHandle flag so pipe handles are inherited. */
    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    HANDLE hPipeTemp;

    if (!CreatePipe(&hPipeTemp, &print_gdi_write_handle, &saAttr, 0)) {
	gs_addmess("failed to open printer pipe\n");
	return FALSE;
    }
    /* make the read handle non-inherited */
    if (!DuplicateHandle(GetCurrentProcess(), hPipeTemp,
	    GetCurrentProcess(), &print_gdi_read_handle, 0,
	    FALSE,       /* not inherited */
	    DUPLICATE_SAME_ACCESS)) {
	gs_addmess("failed to duplicate pipe handle\n");
	return FALSE;
    }
    CloseHandle(hPipeTemp);

    // open printer, get size and resolution
    DOCINFO di;
    TCHAR wpsname[MAXSTR];
    convert_multibyte(wpsname, psfile_name(&psfile), 
	sizeof(wpsname)/sizeof(TCHAR)-1);
    memset(&di, 0, sizeof(DOCINFO));
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = wpsname;
    di.lpszOutput = NULL;
    if (StartDoc(hdc, &di) == SP_ERROR) {
	DWORD err = GetLastError();
	LPSTR lpMessageBuffer;
	char buf[MAXSTR];
	sprintf(buf, "StartDoc failed, error %d\n", err);
	gs_addmess(buf);
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM,
	    NULL, err,
	    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* user default language */
	    (LPSTR)&lpMessageBuffer, 0, NULL);
	if (lpMessageBuffer) {
	    gs_addmess(lpMessageBuffer);
	    gs_addmess("\r\n");
	    LocalFree(LocalHandle(lpMessageBuffer));
	}
	return FALSE;
    }

    // Need to pass these back to gvcprn.cpp.
    // Use global variables for now.
    print_gdi_width = GetDeviceCaps(hdc, PHYSICALWIDTH);
    print_gdi_height = GetDeviceCaps(hdc, PHYSICALHEIGHT);
    print_gdi_xdpi = GetDeviceCaps(hdc, LOGPIXELSX);
    print_gdi_ydpi = GetDeviceCaps(hdc, LOGPIXELSY);
    if (debug & DEBUG_GDI) {
	char buf[MAXSTR];
	sprintf(buf, "GDI width=%d height=%d xdpi=%d ydpy=%d\n",
	    print_gdi_width, print_gdi_height, print_gdi_xdpi, print_gdi_ydpi);
	gs_addmess(buf);
    }

    if ( (print_gdi_width == 0) || (print_gdi_height == 0) ) {
	gs_addmess("Printer width or height is zero\n");
	return FALSE;
    }
 
    return TRUE;
}



