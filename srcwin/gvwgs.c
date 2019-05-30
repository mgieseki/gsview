/* Copyright (C) 1996-2005, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwgs.c */
/* Ghostscript DLL interface for GSview */
#include "gvwgs.h"
#include "cdll.h"

#ifdef _MSC_VER
#define _export
#endif


HINSTANCE phInstance;
POINT char_size;
LPSTR szAppName = "GSview Print";
HWND hwnd_client, hwnd_text;
int status_height;


#define WINDOWWIDTH	(80*char_size.x)
#define WINDOWHEIGHT	(24*char_size.y)
HFONT hfont;


#define TWLENGTH 16384
#define TWSCROLL 1024
char twbuf[TWLENGTH];
int twend;

int multithread = FALSE;
BOOL quitnow = FALSE;

char gsarg[2048];
unsigned long gstid;
GSDLL gsdll;

/* command line options */
int debug = 0;	/* don't shut down or delete files if debugging */
char *gsdllname;
char *option;
char *filename;

FILE *infile;

unsigned long lsize, ldone;
int pcdone;
char title[64];

/* forward declarations */
void show_about(void);
int init_window(void);
void gs_addmess(const char *str);
int get_args(LPSTR lpszCmdLine, int *pargc, char **pargv[]);
int parse_args(int argc, char *argv[]);
void text_update(void);
LRESULT CALLBACK _export ClientWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int message_box(char *str, int icon);
void saveas(void);
void gs_thread(void *arg);

#define GetNotification(wParam,lParam) (HIWORD(wParam))
#define SendDlgNotification(hwnd, id, notice) \
    SendMessage((hwnd), WM_COMMAND, MAKELONG((id),(notice)), (LPARAM)GetDlgItem((hwnd),(id)))

int PASCAL 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int cmdShow)
{
    int i, argc;
    char **argv;
    MSG msg;
    phInstance = hInstance;

    if (init_window())
	return 1;
    ShowWindow(hwnd_client, cmdShow);

    get_args(lpszCmdLine, &argc, &argv);
    if (parse_args(argc, argv)) {
	if (multithread)
	    gstid = _beginthread(gs_thread, 131072, NULL);
	else 
        {
	    /* process messages for window creation */
	    while ((PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	    }
	    gs_thread(0);    /* this will call PeekMessage */
	}
    }

    while (!quitnow && GetMessage(&msg, (HWND)NULL, 0, 0)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
    }

    if (!debug) {
	if (option)
	    unlink(option);
	if (filename)
	    unlink(filename);
    }

    for (i=0; i<argc; i++)
	free(argv[i]);
    free(argv);

    DestroyWindow(hwnd_client);
    return 0;
}

int
init_window(void)
{
WNDCLASS wndclass;
HDC hdc;
TEXTMETRIC tm;
LOGFONT lf;
HMENU hmenu;
RECT rect;

	/* figure out which version of Windows */
DWORD version = GetVersion();
	/* Win32s: bit 15 HIWORD is 1 and bit 14 is 0 */
	/* Win95:  bit 15 HIWORD is 1 and bit 14 is 1 */
	/* WinNT:  bit 15 HIWORD is 0 and bit 14 is 0 */
	/* WinNT with Win95 shell recognised by WinNT + LOBYTE(LOWORD) >= 4 */
	/* check if Windows NT */
	if ((HIWORD(version) & 0x8000)==0)
	    multithread = TRUE;
	/* check if Windows 95 (Windows 4.0) */
	if ( ((HIWORD(version) & 0x8000)!=0) && ((HIWORD(version) & 0x4000)!=0) )
	    multithread = TRUE;
	/* Win32s */
	if ( ((HIWORD(version) & 0x8000)!=0) && ((HIWORD(version) & 0x4000)==0) )
	    multithread = FALSE;

	/* register the window class */
	wndclass.style = 0;
	wndclass.lpfnWndProc = ClientWndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = phInstance;
	wndclass.hIcon = LoadIcon(phInstance, MAKEINTRESOURCE(ID_GSVIEW));
	wndclass.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);
	wndclass.hbrBackground =  CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	RegisterClass(&wndclass);

	hdc = GetDC(NULL);  /* desktop */
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight = 8;
	strcpy(lf.lfFaceName, "Helv"); 
	hfont = CreateFontIndirect(&lf);
	SelectObject(hdc, hfont);
	GetTextMetrics(hdc, (LPTEXTMETRIC)&tm);
	ReleaseDC(NULL, hdc);
	char_size.x = tm.tmAveCharWidth;
	char_size.y = tm.tmHeight;
        status_height = 0 /* 6*char_size.y/4 */;

	hwnd_client = CreateWindow(szAppName, (LPSTR)szAppName,
		  WS_OVERLAPPEDWINDOW,
		  CW_USEDEFAULT, CW_USEDEFAULT, 
		  WINDOWWIDTH, WINDOWHEIGHT,
		  NULL, NULL, phInstance, (void FAR *)NULL);

	hmenu = LoadMenu(phInstance, MAKEINTRESOURCE(ID_GSVIEW));
	SetMenu(hwnd_client, hmenu);
	

	
        GetClientRect(hwnd_client, &rect);
	hwnd_text = CreateWindow("EDIT", "",
		WS_CHILD | /* WS_VISIBLE | */ ES_MULTILINE | ES_READONLY | WS_BORDER | WS_VSCROLL | WS_HSCROLL | DS_3DLOOK, 
		rect.left, rect.top,
		rect.right-rect.left, rect.bottom-rect.left-status_height,
		hwnd_client, (HMENU)TEXTWIN_MLE, phInstance, NULL);
	SendMessage(hwnd_text, WM_SETFONT, (WPARAM)hfont, MAKELPARAM(TRUE, 0));
	ShowWindow(hwnd_text, SW_SHOWNA);

	return 0;
}



/* Window Procedure */
LRESULT CALLBACK _export
ClientWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
/*	case WM_CLOSE:   */
	case WM_DESTROY:
		PostQuitMessage(0);
		quitnow = TRUE;
		break;
	case WM_SIZE:
	    if ( (wParam == SIZE_RESTORED) || (wParam == SIZE_MAXIMIZED) ) {
		SetWindowPos(hwnd_text, HWND_TOP, 0, 0, 
		    LOWORD(lParam), HIWORD(lParam)-status_height,
		    SWP_SHOWWINDOW);
	    }
	case WM_TEXTUPDATE:
	    text_update();
	    break;
	case WM_PCUPDATE:
	    sprintf(title, "%d%% - %s", 
		(int)(wParam) > 100 ? 100 : (int)(wParam) , szAppName);
	    SetWindowText(hwnd, title);
	    return 0;
	case WM_COMMAND:
	    switch(LOWORD(wParam)) {
		case IDM_EXIT:
		    PostQuitMessage(0);
		    return 0;
		case IDM_SAVEAS:
		    saveas();
		    return 0;
		case IDM_COPYCLIP:
		    {HGLOBAL hglobal;
		    LPSTR p;
		    DWORD start, end;
		    SendMessage(hwnd_text, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);
		    if (start == end) {
			start = 0;
			end = twend;
		    }
		    hglobal = GlobalAlloc(GHND | GMEM_SHARE, end-start+1);
		    if (hglobal == (HGLOBAL)NULL) {
			MessageBeep(-1);
			return(FALSE);
		    }
		    p = (LPSTR)GlobalLock(hglobal);
		    if (p == (LPSTR)NULL) {
			MessageBeep(-1);
			return(FALSE);
		    }
		    strncpy(p, twbuf+start, end-start);
		    GlobalUnlock(hglobal);
		    OpenClipboard(hwnd_client);
		    EmptyClipboard();
		    SetClipboardData(CF_TEXT, hglobal);
		    CloseClipboard();
		    }
		    return 0;
		case IDM_ABOUT:
		    show_about();
		    return 0;
	    }
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

/* Text Window for Ghostscript Messages */
/* uses MS-Windows multiline edit field */

void
text_update(void)
{
    DWORD linecount;
    SendMessage(hwnd_text, WM_SETREDRAW, FALSE, 0);
    SetWindowText(hwnd_text, twbuf);
    /* EM_SETSEL, followed by EM_SCROLLCARET doesn't work */
    linecount = SendMessage(hwnd_text, EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
    SendMessage(hwnd_text, EM_LINESCROLL, (WPARAM)0, (LPARAM)linecount-17);
    SendMessage(hwnd_text, WM_SETREDRAW, TRUE, 0);
    InvalidateRect(hwnd_text, (LPRECT)NULL, TRUE);
    UpdateWindow(hwnd_text);
}



/* Add string for Ghostscript message window */
void
gs_addmess_count(const char *str, int count)
{
const char *s;
char *p;
int i, lfcount;
    /* we need to add \r after each \n, so count the \n's */
    lfcount = 0;
    s = str;
    for (i=0; i<count; i++) {
	if (*s == '\n')
	    lfcount++;
	s++;
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
    /* tell main thread to update the MLE */
    PostMessage(hwnd_client, WM_TEXTUPDATE, 0, 0);
}

void
gs_addmess(const char *str)
{
    gs_addmess_count(str, lstrlen(str));
}

void
gs_addmessf(const char *fmt, ...)
{
va_list args;
int count;
char buf[1024];
	va_start(args,fmt);
	count = vsprintf(buf,fmt,args);
        gs_addmess(buf);
	va_end(args);
}


#define MAX_ARGC 10
/* Scan command line and produce argc and argv. */
/* Quotes must be used around arguments which have embedded spaces. */
/* We use this because the inbuilt Borland scanner gives incorrect */
/* results for quoted arguments with embedded spaces. */
int
get_args(LPSTR lpszCmdLine, int *pargc, char **pargv[])
{
int argc = 0;
char **argv;
char *start, *end;
int inquote;
int length;
    argv = (char **)malloc( (MAX_ARGC+1) * sizeof(char *) );
    if (argv == (char **)NULL)
	return 0;
    memset(argv, 0, (MAX_ARGC+1) * sizeof(char *));
    argv[argc] = (char *)malloc(MAXSTR+1);
    if (argv[argc] == NULL) {
	free(argv);
	return 0;
    }
    GetModuleFileName(phInstance, argv[argc], MAXSTR);
    argc++;
    start = lpszCmdLine;
    /* skip leading spaces */
    while (*start && (*start == ' '))
	start++;
    end = start;
    while (*end) {
	/* found new argument */
	inquote = FALSE;
	if (*end == '\042') {
	    /* skip leading quote */
	    start++;
	    end++;
	    inquote = TRUE;
	}
	while (*end) {
	    if (!inquote && (*end==' ')) {
		break;
	    }
	    else if (inquote && (*end=='\042')) {
		break;
	    }
	    else
		end++;
	}
	length = (int)(end-start);
	argv[argc] = (char *)malloc(length+1);
	if (argv[argc] == NULL) {
	    free(argv);
	    return 0;
	}
	strncpy(argv[argc], start, length);
	argv[argc][length] = '\0';
	if (*end)
	   end++;	/* skip trailing quote or space */
	start = end;
        /* skip leading spaces */
	while (*start && (*start == ' '))
	    start++;
	end = start;
	argc++;
	if (argc == MAX_ARGC)
	    break;
    }
    *pargc = argc;
    *pargv = argv;
    return argc;
}
/* Parse command line arguments */
/* Return 0 if error */
int
parse_args(int argc, char *argv[])
{
char buf[MAXSTR];
char *usage="Usage: gvwgs [/d] dllpath optionfile inputfile\n\
optionfile and inputfile will be deleted on exit\n\
It is intended that gvwgs be called with temporary files\n";
    if (argc >= 2) {
	if ( ((argv[1][0] == '/') || (argv[1][0] == '-')) &&
	     ((argv[1][1] == 'd') || (argv[1][0] == 'D')) ) {
	    debug = 1;
	    sprintf(buf, "argv[0]=\042%s\042\n", argv[0]);
	    gs_addmess(buf);
	    sprintf(buf, "argv[1]=\042%s\042\n", argv[1]);
	    gs_addmess(buf);
	    sprintf(buf, "Shifting arguments\n");
	    gs_addmess(buf);
	    argv++;
	    argc--;
	}
    }

    if (debug || (argc != 4)) {
	int i;
	sprintf(buf, "argc=%d\n", argc);
        gs_addmess(buf);
	for (i=0; i<argc; i++) {
	    sprintf(buf, "argv[%d]=\042%s\042\n", i, argv[i]);
	    gs_addmess(buf);
	}
    }

    if (argc != 4) {
	gs_addmess(usage);
	return FALSE;
    }

    gsdllname = argv[1];
    option = argv[2];
    filename = argv[3]; 
    if (debug) {
	sprintf(buf, "multithread=%d\n", multithread);
	gs_addmess(buf);
	sprintf(buf, "gsdllname=\042%s\042\n", gsdllname);
	gs_addmess(buf);
	sprintf(buf, "option file=\042%s\042\n", option);
	gs_addmess(buf);
	sprintf(buf, "input file=\042%s\042\n", filename);
	gs_addmess(buf);
    }

    if (debug) {
        FILE *optfile = fopen(option, "r");
	if (optfile == NULL) {
	    gs_addmess(usage);
	    sprintf(gsarg, "Can't find file '%s'\n", option);
	    gs_addmess(gsarg);
	    return FALSE;
	}
	sprintf(buf, "Option file contents:\n");
	gs_addmess(buf);
	while (fgets(buf, sizeof(buf)-1, optfile)) {
	    gs_addmess("  ");
	    gs_addmess(buf);
	}
	gs_addmess("\n");
	fclose(optfile);
    }


    infile = fopen(filename, "rb");
    if (infile == (FILE *)NULL) {
	gs_addmess(usage);
	sprintf(gsarg, "Can't find file '%s'\n", filename);
	gs_addmess(gsarg);
	return FALSE;
    }

    /* find length of file */
    fseek(infile, 0L, SEEK_END);
    lsize = ftell(infile);
    lsize = lsize / 100;	/* to get percent values */
    if (lsize <= 0)
	lsize = 1;
    fseek(infile, 0L, SEEK_SET);
    ldone = 0;

    /* build options list */
    strcpy(gsarg, "@");
    strcat(gsarg, option);
    gsarg[strlen(gsarg) + 1] = '\0';

    return TRUE;
}


/* copyright dialog box */
#ifdef _WIN64
INT_PTR
#else
BOOL
#endif
CALLBACK _export
AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
            SetDlgItemText(hDlg, ABOUT_VERSION, GSVIEW_DOT_VERSION);
            return( TRUE);
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
	DialogBoxParam( phInstance, "AboutDlgBox", hwnd_client, AboutDlgProc, (LPARAM)NULL);
}


/* display message */
int 
message_box(char *str, int icon)
{
	return MessageBox(hwnd_client, str, szAppName, icon | MB_OK);
}

void
saveas(void)
{
FILE *f;
OPENFILENAME ofn;
char szOFilename[256];	/* filename for OFN */

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd_client;
	ofn.lpstrFile = szOFilename;
	ofn.nMaxFile = sizeof(szOFilename);
	ofn.Flags = OFN_PATHMUSTEXIST;

	if (GetSaveFileName(&ofn)) {
	    if ((f = fopen(szOFilename, "wb")) == (FILE *)NULL) {
		message_box("Can't create file", 0);
		return;
	    }
	    fwrite(twbuf, 1, twend, f);
	    fclose(f);
	}
	return;
}

/*********************************************************************/
/* stdio functions */
static int GSDLLCALL
gsdll_stdin(void *instance, char *buf, int len)
{
    char mess[MAXSTR];
    sprintf(mess,"stdin callback not supported: %p %d\n", buf, len);
    gs_addmess(mess);
    return 0; /* EOF */
}

static int GSDLLCALL
gsdll_stdout(void *instance, const char *str, int len)
{
    gs_addmess_count(str, len);
    return len;
}

static int GSDLLCALL
gsdll_stderr(void *instance, const char *str, int len)
{
    gs_addmess_count(str, len);
    return len;
}

/* Poll the caller for cooperative multitasking. */
/* If this function is NULL, polling is not needed */
int GSDLLCALL gsdll_poll(void *handle)
{
    if (!multithread) {
	MSG msg;
	if ((PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)) != 0) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}
    }
    return 0;
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

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM,
	NULL, reason,
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* user default language */
	(LPTSTR) &lpMessageBuffer, 0, NULL);
    if (lpMessageBuffer) {
	gs_addmess((LPTSTR)lpMessageBuffer);
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
    dll->hmodule = LoadLibrary(name);
    if (dll->hmodule < (HINSTANCE)HINSTANCE_ERROR) {
	/* failed */
	load_library_error(dll->hmodule, name);
	/* try again, with path of EXE */
	if ((shortname = strrchr((char *)name, '\\')) == (const char *)NULL)
	    shortname = name;
	else
	    shortname++;

	GetModuleFileName(phInstance, fullname, sizeof(fullname));
	if ((p = strrchr(fullname,'\\')) != (char *)NULL)
	    p++;
	else
	    p = fullname;
	*p = '\0';
	strcat(fullname, shortname);

	if (debug)
	    gs_addmessf("Trying to load %s\n", fullname);

	dll->hmodule = LoadLibrary(fullname);
	if (gsdll.hmodule < (HINSTANCE)HINSTANCE_ERROR) {
	    /* failed again */
	    load_library_error(gsdll.hmodule, fullname);
	    /* try once more, this time on system search path */
	    if (debug)
		gs_addmessf("Trying to load %s\n", shortname);
	    dll->hmodule = LoadLibrary(shortname);
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

/*********************************************************************/


/* load GS DLL if not already loaded */
/* return TRUE if OK */
BOOL
gs_load_dll(void)
{
char buf[MAXSTR];
gsapi_revision_t rv;
int code;
int gs_argc;
char *gs_argv[3];

    if (gsdll_load(&gsdll, gsdllname)) {
	gs_addmess("Can't load Ghostscript DLL\n");
	return FALSE;
    }


    gs_argv[0] = gsdllname;
    gs_argv[1] = gsarg;
    gs_argv[2] = NULL;
    gs_argc = 2;

    code = gsdll.new_instance(&gsdll.minst, NULL);
    if (code) {
	gs_addmessf("gsapi_new_instance returns %d\n", code);
	gsdll_free(&gsdll);
	return FALSE;
    }

    gsdll.set_stdio(gsdll.minst, gsdll_stdin, gsdll_stdout, gsdll_stderr);
    gsdll.set_poll(gsdll.minst, gsdll_poll);
    code = gsdll.init_with_args(gsdll.minst, gs_argc, gs_argv);

    if (debug) {
	sprintf(buf,"gsapi_init_with_args returns %d\n", code);
	gs_addmess(buf);
    }
    if (code) {
	gsdll_free(&gsdll);
	return !code;
    }
    return !code;
}


/* Thread which loads Ghostscript DLL and sends input file to DLL */
#ifdef __BORLANDC__
#pragma argsused
#endif
void 
gs_thread(void *arg)
{
char buf[MAXSTR];
int len;
int code;
int exit_code;
int error = 0;

    if (!gs_load_dll())
	return;
    gsdll.run_string_begin(gsdll.minst, 0, &exit_code);

    while ((len = fread(buf, 1, sizeof(buf), infile)) != 0) {
	code = gsdll.run_string_continue(gsdll.minst, buf, len,
		0, &exit_code);
	if (code == e_NeedInput)
	    code = 0;
	ldone += len;
	if (pcdone != (int)(ldone / lsize)) {
	    pcdone = (int)(ldone / lsize);
	    PostMessage(hwnd_client, WM_PCUPDATE, (WPARAM)pcdone, 0);
	}
	if (code) {
	    error = 1;
	    sprintf(buf, "gsapi_execute_cont returns %d\n", code);
	    gs_addmess(buf);
	    break;
	}
	if (!multithread) {
	    MSG msg;
	    while ((PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)) != 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	    }
	}
    }
    fclose(infile);

    gsdll.run_string_end(gsdll.minst, 0, &exit_code);
    code = gsdll.exit(gsdll.minst);
    gs_addmessf("gsapi_exit returns %d\n", code);
    gsdll_free(&gsdll);

    /* tell main thread to shut down */
    if ((code == 0) && (!error) && (!debug))
	PostMessage(hwnd_client, WM_QUIT, 0, 0);
    else 
	ShowWindow(hwnd_client, SW_SHOWNORMAL);
/*
*/
}

