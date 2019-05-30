/* unzip front end */

#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include <string.h>
#include "wizdll.h"
#include "gvcrc.h"


/* Porting definations between Win 3.1x and Win32 */
#ifdef WIN32
#  define far
#  define _far
#  define __far
#  define near
#  define _near
#  define __near
#define _fstrrchr strrchr
#endif

#ifdef _MSC_VER
#define _export
#endif

/* ************ EXTERNALS *********** */
extern zip_message(char *str, int count);
extern int load_string(int id, char *str, int len);

#define WIZUNZIP_MAX_PATH       128   /* max total file or directory name path   */
#define OPTIONS_BUFFER_LEN      256   /* buffer to hold .INI file options         */

/* Unzip Miscellaneous Buffers */
typedef struct
{
char szFileName[WIZUNZIP_MAX_PATH]; /* fully-qualified archive file name in OEM char set */
char szDirName[WIZUNZIP_MAX_PATH];  /* directory of archive file in ANSI char set */
char szUnzipToDirName[WIZUNZIP_MAX_PATH];    /* extraction ("unzip to") directory name in ANSI */
char szUnzipToDirNameTmp[WIZUNZIP_MAX_PATH]; /* temp extraction ("unzip to") directory name in ANSI */
char szUnzipFromDirName[WIZUNZIP_MAX_PATH];  /* extraction ("unzip from") directory name in ANSI */
char szTotalsLine[80];              /* text for totals of zip archive */
char szBuffer[OPTIONS_BUFFER_LEN];  /* option strings from .INI, & gen'l scratch buf */
char szSoundName[WIZUNZIP_MAX_PATH];/* wave file name or sound from WIN.INI [sounds] in ANSI */
char szPassword[81];
LPSTR lpPassword;
OPENFILENAME ofn;
OPENFILENAME wofn;                  /* wave open file name struct */
MSG msg;
OFSTRUCT of;                        /* archive open file struct */
OFSTRUCT wof;                       /* wave open file struct   */
} UMB, __far *LPUMB;

typedef int (far *DLLPRNT) (FILE *, unsigned int, char *);
typedef void (far *DLLSND) (void);

typedef struct {
DLLPRNT print;          /* a pointer to the application's print routine. */
DLLSND sound;           /* a pointer to the application's sound routine. */
FILE *Stdout;           /* stdout for the calling application */
LPUMB lpUMB;            /* stucture shown above */
HWND hWndList;          /* application's handle for a list box to display
								  the contents of an archive in */
HWND hWndMain;          /* application's main window */
HWND hInst;             /* application's instance */
int ExtractOnlyNewer;   /* true if you are to extract only newer */
int Overwrite;          /* true if always overwrite files */
int SpaceToUnderscore;  /* true if convert space to underscore */
int PromptToOverwrite;  /* true if prompt to overwrite is wanted */
int ncflag;             /* write to stdout if true */
int ntflag;             /* test zip file */
int nvflag;             /* verbose listing */
int nUflag;             /* "update" (extract only newer/new files */
int nzflag;             /* display zip file comment */
int ndflag;             /* all args are files/dir to be extracted */
int noflag;             /* */
int naflag;             /* do ASCII-EBCDIC and/or end of line translation */
int argc;               /* */
LPSTR lpszZipFN;        /* zip file name */
char **FNV;             /* file name vector to list of files to extract */
} DCL, _far *LPDCL;



/* global variables */
LPUMB lpumb;
HGLOBAL  hUMB;
LPDCL lpDCL;
HGLOBAL hDCL;
HINSTANCE hWinDll;
int (WINAPI far *DllProcessZipFiles)(DCL far *);
(WINAPI far *GetDllVersion)(DWORD far *);

typedef int (_export far *THUNKDLLPRNT) (FILE *, unsigned int, char *);
THUNKDLLPRNT lpfnDoPrint;

extern char szAppName[];

void
do_sound(void)
{
/* needs to be thunked like do_print if anything added here */
/*
    MessageBeep(-1);
*/
    return;
}

int
#ifndef __WIN32__
_far _export
#endif
do_print(FILE *file, unsigned int size, char *buffer)
{
    /* DS is current set to the DLL.
     * Call this via a thunk to temporarily set DS back to 
     * that of the caller.
     */
MSG msg;
    zip_message(buffer, size);
    while (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE)) {
	if ((lpDCL->hWndMain==0) || !IsDialogMessage(lpDCL->hWndMain, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	}
    }
    return size;
}


int
free_unzip(void)
{
    if (hWinDll)
	FreeLibrary(hWinDll);
    hWinDll = NULL;
    DllProcessZipFiles = NULL;
    GetDllVersion = NULL;
    if (hUMB) {
	GlobalUnlock(hUMB);
	GlobalFree(hUMB);
    }
    hUMB = NULL;
    lpumb = NULL;
#ifndef __WIN32__
    if (lpfnDoPrint)
	FreeProcInstance((FARPROC)lpfnDoPrint);
#endif
    if (hDCL) {
	GlobalUnlock(hDCL);
	GlobalFree(hDCL);
    }
    hDCL = NULL;
    lpDCL = NULL;
    return 0;
}



int load_unzip(LPSTR lpszDllName, HINSTANCE hInstance, HWND hmain, HWND hlist)
{
DWORD dwVersion;  /* These variables are all used for version checking */

	 hUMB = GlobalAlloc( GPTR, (DWORD)sizeof(UMB));
	 if ( !hUMB )
	return -1;

	 lpumb = (LPUMB)GlobalLock( hUMB );
	 if ( !lpumb ) {
	free_unzip();
	return -1;
	 }

	 hDCL = GlobalAlloc( GPTR, (DWORD)sizeof(DCL));
	 if (!hDCL) {
	free_unzip();
	return -1;
	 }

	 lpDCL = (LPDCL)GlobalLock(hDCL);
	 if (!lpDCL) {
	free_unzip();
	return -1;
	 }

#ifdef __WIN32__
	 lpfnDoPrint = do_print;
#else
	 lpfnDoPrint = (DLLPRNT)MakeProcInstance((FARPROC)do_print, hInstance);
#endif
	 lpDCL->print = (DLLPRNT)lpfnDoPrint;
	 lpDCL->sound = (DLLSND)do_sound;
	 lpDCL->Stdout = stdout;
	 lpDCL->lpUMB = lpumb;
	 lpDCL->hInst = hInstance;
	 lpDCL->hWndMain = hmain;
	 lpDCL->hWndList = hlist;



	 hWinDll = LoadLibrary(lpszDllName);
	 if (hWinDll >= (HINSTANCE)HINSTANCE_ERROR) {
		 DllProcessZipFiles = GetProcAddress(hWinDll, "DllProcessZipFiles");
		 GetDllVersion = GetProcAddress(hWinDll, "GetDllVersion");
	 }
	 else {
	char str[256];
	char mess[256];
	load_string(IDS_CANTLOAD, mess, sizeof(mess));
	wsprintf (str, mess, lpszDllName, (int)hWinDll);
	MessageBox (lpDCL->hWndMain, str, szAppName, MB_ICONSTOP | MB_OK);
	free_unzip();
	return -1;
	}

	 /* If, for some reason we got a dll with the same name, we don't want to
	  * make a call out to never-never land. At some point we should put a message
	  * up saying we can't load the proper dll.
	  */
	 if (DllProcessZipFiles == NULL) {
	free_unzip();
	return -1;
	 }

	 /* We need a check here in case an old beta version of the dll is out there */
	 if (GetDllVersion != NULL) {
	(*GetDllVersion)(&dwVersion);
	if ((HIWORD(dwVersion) != DLL_MAJOR) || (LOWORD(dwVersion) != DLL_MINOR)) {
		 char str[256];
		 wsprintf(str, "Wrong version of UNZIP DLL.  Need %d.%d.  Found %d.%d.",
			 DLL_MAJOR, DLL_MINOR, HIWORD(dwVersion), LOWORD(dwVersion));
		 MessageBox(lpDCL->hWndMain, str, szAppName, MB_ICONSTOP | MB_OK);
		 free_unzip();
	    return -1;
	}
    }

	 return 0;	/* all is well */
}

/* zipname must contain full path name using backslashes */
/* unzip to current directory */
int
unzip(char *zipname)
{
LPSTR p;
	 lpDCL->Overwrite = TRUE;  /* overwrite existing files */
	 lpDCL->ncflag = FALSE;	/* don't write to stdout */
	 lpDCL->ntflag = FALSE;	/* don't test zip file */
	 lpDCL->nvflag = FALSE;	/* don't verbose list */
	 lpDCL->nUflag = FALSE;	/* replace all files */
	 lpDCL->nzflag = FALSE;	/* don't display zip file comment */
	 lpDCL->ndflag = TRUE;       /* all args are files/dir to be extracted */
	 lpDCL->noflag = FALSE;      /* */
	 lpDCL->naflag = FALSE;      /* do ASCII-EBCDIC and/or end of line translation */

	 lstrcpy(lpumb->szFileName, zipname);
	 lstrcpy(lpumb->szDirName, zipname);
	 p = _fstrrchr(lpumb->szDirName, '\\');
	 if (p)
	*p = '\0';

	 lpDCL->lpszZipFN = zipname;
	 lpDCL->argc = 0;
	 lpDCL->FNV = NULL;

	 return (*DllProcessZipFiles)(lpDCL);
}

#ifdef UNUSED
char szAppName[]  = "unzip test";

int main(int argc, char *argv)
{
int i;
	 i = load_unzip("wizunz32.dll", NULL, NULL, NULL);
	 if (i)
		  return 1;
	 i = unzip("c:\\temp\\ut.zip");
	 printf("\nunzip return value is %d\n", i);
	 free_unzip();
	 return i;
}
#endif
