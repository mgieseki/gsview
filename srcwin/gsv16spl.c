/* Copyright (C) 1995-2000, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gsv16spl.c */
/* 16-bit access to print spooler from Win32s */
/* by Russell Lang */
/* 1995-11-10 */

/* 
 * GSview and Ghostscript produce printer specific output
 * which must be given to the print spooler.
 * Under Win16, the APIs OpenJob, WriteSpool etc. are used
 * Under Win32 and Windows 95/NT, the APIs OpenPrinter, WritePrinter etc.  
 * are used.
 * Under Win32s, the 16-bit spooler APIs are not available, and the
 * 32-bit spooler APIs are not implemented.
 * This purpose of this application is to provide a means for the Win32s
 * versions of GSview and Ghostscript to send output to the 16-bit spooler.
 */

/*
 * Usage:  gsv16spl hwnd
 *   hwnd is the window handle of the program starting gsv16spl
 *
 * hwnd will be sent a WM_USER+1=WM_GSV16SPL message with the window 
 * handle of gsv16spl in the lParam.
 * gsv16spl is then called using SendMessage with the handle of
 * a global shareable memory block in lParam.
 * The first SendMessage should have the null terminated name of the 
 * printer port in this memory block.
 * Subsequent memory blocks contain data to be sent to that port.
 * The memory block contains a length count in the first WORD,
 * followed by the actual data.
 * A length count of 0 indicates EOF.  This will cause gsv16spl 
 * to close the print job and exit.
 * A length count of -1 indicates an error.  gsv16spl will abort
 * the print job and exit.
 */


#define STRICT
#include <windows.h>
/* #include <string.h> */
#include <stdlib.h>

/* documented in Device Driver Adaptation Guide */
/* Prototypes taken from print.h */
DECLARE_HANDLE(HPJOB);

HPJOB   WINAPI OpenJob(LPSTR, LPSTR, HPJOB);
int     WINAPI StartSpoolPage(HPJOB);
int     WINAPI EndSpoolPage(HPJOB);
int     WINAPI WriteSpool(HPJOB, LPSTR, int);
int     WINAPI CloseJob(HPJOB);
int     WINAPI DeleteJob(HPJOB, int);
int     WINAPI WriteDialog(HPJOB, LPSTR, int);
int     WINAPI DeleteSpoolPage(HPJOB);

HPJOB hJob;
HDC hdc_printer;
unsigned long bytes_written;
char portname[64];
POINT char_size;
HWND hwndspl;
HWND hwndnotify;	/* window handle of caller */
HICON hiconspl;
HINSTANCE phInstance;

#define ID_GSV16SPL 42
#define IDC_ABOUT  200
#define WM_GSV16SPL WM_USER+1
#define WINDOWWIDTH	(36*char_size.x)
#define WINDOWHEIGHT	( 5*char_size.y)
char szAppName[] = "GS Win32s/Win16 spooler";
char debug_str[64];

void
draw_text(HWND hwnd, HDC hdc)
{
    char buf[256];
    RECT rect;
    int x, y;
    GetClientRect(hwnd, &rect);
    x = 3 * char_size.x + 32;
    y = (rect.bottom - rect.left - char_size.y) / 2;

    SetTextAlign(hdc, TA_LEFT | TA_TOP);
    if (hwndnotify) {
	wsprintf(buf, "%lu bytes written to %s", bytes_written, (LPSTR)portname);
	TextOut(hdc, x, y, buf, lstrlen(buf));
	if (debug_str)
	    TextOut(hdc, x, y + char_size.y, debug_str, lstrlen(debug_str));
    }
    else {
	lstrcpy(buf, "Playing Russian Roulette are we?");
	TextOut(hdc, x, y - char_size.y - 2 , buf, lstrlen(buf));
	lstrcpy(buf, "GSV16SPL is called automatically");
	TextOut(hdc, x, y, buf, lstrlen(buf));
	lstrcpy(buf, "from GSview or Ghostscript.");
	TextOut(hdc, x, y + char_size.y + 2, buf, lstrlen(buf));
    }
}

int
open_printer(LPSTR port)
{
    char buf[256];
    LPSTR device = "";
    LPSTR driver = "";
    
#define XYZZY "xyzzy"
    if ((port == NULL) || (lstrlen(port) == 0))
	return FALSE;

    hdc_printer = NULL;

    /* if it is a port name, use it */
    GetProfileString("Ports", port, XYZZY, device, sizeof(device));
    if (lstrcmp(device, XYZZY) == 0) {
	/* It wasn't a port.  Check if it is a queue name */
	GetProfileString("Devices", port, "", buf, sizeof(buf));
	if (lstrlen(buf)) {
	    /* Found it, get driver and port name */
	    char *p;
	    device = port;
	    driver = buf;
	    while (*p && (*p != ','))
		p++;
	    if (*p == ',') {
		*p = '\0';
		port = p+1;
	    }
	    /* open device context, so OpenJob knows which printer 
	     * queue to use */
	    hdc_printer = CreateDC(driver, device, port, NULL);
	}
	else
	    return FALSE;
    }
	    
    /*
     * Under Win32s with multiple printers connected to the same port,
     * printing to a named printer always prints to the first listed
     * printer.  If the first listed printer is a bi-directional printer
     * but the desired printer is unidirectional, the wrong driver
     * complains.  Somehow we need to tell OpenJob which printer
     * owns the job.  Maybe this is done by passing an HDC for this
     * printer as the 3rd argument of OpenJob, but the Device Driver
     * Adaption Guide is no longer available, so we can't verify this.
     * This problem isn't fixed.
     */

    hJob = OpenJob(port, szAppName, (HDC)NULL);
    switch ((int)hJob) {
	case SP_APPABORT:
	case SP_ERROR:
	case SP_OUTOFDISK:
	case SP_OUTOFMEMORY:
	case SP_USERABORT:
	    hJob = NULL;
	    lstrcpy(debug_str, "OpenJob failed");
	    DeleteDC(hdc_printer);
	    return FALSE;
    }
    if (StartSpoolPage(hJob) < 0) {
	lstrcpy(debug_str, "StartSpoolPage failed");
        DeleteJob(hJob, 0);
	DeleteDC(hdc_printer);
        hJob = NULL;
	return FALSE;
    }
    return TRUE;	/* OK */
}

int
close_printer(void)
{	
    if (hJob == (HPJOB)NULL)
	return FALSE;
    if (hdc_printer != NULL) {
	DeleteDC(hdc_printer);
	hdc_printer = NULL;
    }
    EndSpoolPage(hJob);
    CloseJob(hJob);
    hJob = NULL;
    return TRUE;
}

int
abort_printer(void)
{
    if (hJob == (HPJOB)NULL)
	return FALSE;
    EndSpoolPage(hJob);
    DeleteJob(hJob, 0);
    hJob = NULL;
    return TRUE;
}

int
write_printer(LPSTR data, WORD length)
{
    if (hJob == (HPJOB)NULL)
	return FALSE;
    if (WriteSpool(hJob, data, length) < 0) {
	lstrcpy(debug_str, "WriteSpool failed");
	abort_printer();
        return FALSE;
    }
    return TRUE;
}

int
process_block(HGLOBAL hmem)
{
LPBYTE lpbyte;
WORD length;
int rc;
    if (hmem == (HGLOBAL)NULL) {
	lstrcpy(debug_str, "hglobal is null");
	return FALSE;
    }
    lpbyte = GlobalLock(hmem);
    if (lpbyte == (LPBYTE)NULL) {
	lstrcpy(debug_str, "Couldn't lock hglobal");
	return FALSE;
    }
    length = *(LPWORD)lpbyte;
    if (length == 0) {
	/* EOF */
	lstrcpy(debug_str, "Got EOF");
	GlobalUnlock(hmem);
	PostQuitMessage(0);
	return close_printer();
    }
    else if (length == 0xffff) {
	/* Abort */
	lstrcpy(debug_str, "Got Abort");
	GlobalUnlock(hmem);
	PostQuitMessage(0);
	return abort_printer();
    }

    if (hJob) {
	HDC hdc;
	rc = write_printer((LPSTR)(lpbyte+2), length); 
	bytes_written += length;
	hdc = GetDC(hwndspl);
	draw_text(hwndspl, hdc);
	ReleaseDC(hwndspl, hdc);
    }
    else {
	rc = open_printer((LPSTR)(lpbyte+2));
    }
    GlobalUnlock(hmem);
    return rc;
}

/* copyright dialog box */
BOOL CALLBACK _export
AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
                case IDOK:
                    EndDialog(hDlg, TRUE);
                    return(TRUE);
                default:
                    return(FALSE);
            }
        default:
            return(FALSE);
    }
}

void
show_about(void)
{
    DLGPROC lpProcAbout;
    lpProcAbout = (DLGPROC)MakeProcInstance((FARPROC)AboutDlgProc, phInstance);
    DialogBoxParam( phInstance, "AboutDlgBox", hwndspl, lpProcAbout, (LPARAM)NULL);
    FreeProcInstance((FARPROC)lpProcAbout);
}


/* Window Procedure */
LRESULT CALLBACK _export
WndSplProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
HMENU hmenu;
PAINTSTRUCT ps;
HDC hdc;
RECT rect;
    switch(message) {
	case WM_GSV16SPL:
		/* we have been called using SendMessage so we */
		/* must not do anything that requires the message queue */
		return process_block((HGLOBAL)lParam);
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		draw_text(hwnd, hdc);
		GetClientRect(hwnd, &rect);
		DrawIcon(hdc, char_size.x, (rect.bottom-rect.top-32)/2, hiconspl);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_CREATE:
		hmenu = GetSystemMenu(hwnd,FALSE);
		EnableMenuItem(hmenu, SC_SIZE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		EnableMenuItem(hmenu, SC_MAXIMIZE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		AppendMenu(hmenu, MF_SEPARATOR, 0, NULL);
		AppendMenu(hmenu, MF_ENABLED, IDC_ABOUT, "&About");
		break;
/*	case WM_CLOSE:   */
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SYSCOMMAND:
        	switch(LOWORD(wParam)) {
		    case IDC_ABOUT:
			show_about();
			return 0;
		}
		break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}


void
init_window(void)
{
int i;
WNDCLASS wndclass;
HDC hdc;
TEXTMETRIC tm;
HFONT hfont;

	/* register the window class */
	wndclass.style = 0;
	wndclass.lpfnWndProc = WndSplProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = phInstance;
	wndclass.hIcon = hiconspl = LoadIcon(phInstance, MAKEINTRESOURCE(ID_GSV16SPL));
	wndclass.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);
	wndclass.hbrBackground =  GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	RegisterClass(&wndclass);

	hdc = GetDC(NULL);  /* desktop */
	hfont = GetStockObject(SYSTEM_FIXED_FONT);
	SelectObject(hdc, hfont);
	GetTextMetrics(hdc, (LPTEXTMETRIC)&tm);
	ReleaseDC(NULL, hdc);
	char_size.x = tm.tmAveCharWidth;
	char_size.y = tm.tmHeight;

	hwndspl = CreateWindow(szAppName, (LPSTR)szAppName,
		  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
/*
		  CW_USEDEFAULT, CW_USEDEFAULT, 
*/
0,0,
		  WINDOWWIDTH, WINDOWHEIGHT,
		  NULL, NULL, phInstance, (void FAR *)NULL);

	return;
}

int PASCAL 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int cmdShow)
{
	MSG msg;
	phInstance = hInstance;

	/* find out window handle of caller */
	if (lpszCmdLine[0] != '\0') {
	    char buf[64];
	    lstrcpyn(buf, lpszCmdLine, sizeof(buf)-1);
	    hwndnotify = (HWND)atoi(buf);
	}
	else {
	    hwndnotify = (HWND)NULL;
	}

	init_window();
	ShowWindow(hwndspl, cmdShow);

	/* notify caller about our window handle */
	if (hwndnotify)
	    SendMessage(hwndnotify, WM_GSV16SPL, 0, (LPARAM)hwndspl);

	while (GetMessage(&msg, (HWND)NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DestroyWindow(hwndspl);
	return 0;
}

