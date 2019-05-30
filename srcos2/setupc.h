/* Copyright (C) 1996-2000, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* setupc.h */

#ifdef MAX_PATH
#define MAXSTR MAX_PATH
#else
#define MAXSTR 256
#endif


#ifdef _MSC_VER
#define _export
#endif

extern char szAppName[];

/* flags for items we need to do */
extern BOOL install_gs;
extern BOOL install_gsview;
extern BOOL install_autoexec;		/* update autoexec.bat or config.sys */
extern BOOL install_autoexecbak;	/* keep backup of old autoexec.bak */
extern BOOL install_group;		/* add program manager group */

extern int batch;		/* don't display windows */
extern BOOL failed;		/* TRUE if something went wrong */
extern BOOL cancelling;		/* TRUE if cancel request */
extern BOOL installing;		/* TRUE if we are currently installing */

/* variables that affect where we install things */
extern int gsver;
extern char gs_basedir[16];
extern char gs_zipprefix[16];
extern char gsviewbase[MAXSTR];

extern char workdir[MAXSTR];
extern char bootdrive[MAXSTR];
extern char sourcedir[MAXSTR];
extern char destdir[MAXSTR];
extern char autoexec_bak[MAXSTR];

extern char unzipname[MAXSTR];
extern char unzipprefix[MAXSTR];
extern FILE *unziplogfile;

extern HWND hMain;
extern HWND hwnd_current;
typedef struct tagWIZPAGE {
   int id;		/* resource ID */
   int prev;		/* resource id of previous page */
   int next;		/* resource id of next page */
   int (*func)(HWND);	/* function to run on exit from page */
   HWND hwnd;		/* window handle of dialog */
} WIZPAGE;

extern WIZPAGE pages[];

/* text window for unzipping */
#define TWLENGTH 16384
#define TWSCROLL 1024
extern char twbuf[TWLENGTH];
extern int twend;

#ifdef OS2
#define HINSTANCE HAB
extern int emx;
#define EMXZIP    "emxrt.zip"
#define GetDlgItemText(hwnd, id, str, len) \
	WinQueryWindowText(WinWindowFromID((hwnd), (id)), (len), (str))
#define SetDlgItemText(hwnd, id, str) \
	WinSetWindowText( WinWindowFromID((hwnd), (id)), (str))
#else
#define DID_OK IDOK
#define DID_CANCEL IDCANCEL
#define MBID_YES IDYES
#define MBID_NO IDNO
#define MB_MOVEABLE 0
#endif

extern HINSTANCE phInstance;
extern char get_string_answer[MAXSTR];
extern char error_message[MAXSTR];
extern char szUnzipDll[];
extern char szIniName[];

/* in XXsetup.c */
int done(HWND hwnd);
int finish(HWND hwnd);
void goto_page(HWND hwnd, int id);
void install_init(void);
int message_box(const char *str, int icon);
int gs_chdir(char *dirname);
int load_string(int id, char *str, int len);
int update_config(void);
int create_object(void);
int update_ini(char *ininame);
void gs_addmess_update(void);
#ifdef OS2
MRESULT EXPENTRY InputDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
int dialog(int resource, PFNWP dlgproc);
#else
BOOL CALLBACK _export InputDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
int dialog(int resource, DLGPROC dlgproc);
#endif

/* in XXXunzip.c :*/
int unzip(char *zipname);
int load_unzip(LPSTR lpszDllName, HINSTANCE hInstance, HWND hmain, HWND hlist);
int free_unzip(void);

/* in setupc.c */
void gs_addmess_count(LPSTR str, int count);
void gs_addmess(LPSTR str);
WIZPAGE * find_page_from_id(int id);
void next_page(HWND hwnd);
void prev_page(HWND hwnd);
int init_temp(void);
int add_gsver(HWND hwnd, int offset);
int check_gsver(HWND hwnd);
int check_dir(HWND hwnd);
int make_dir(HWND hwnd);
void zip_message(char *str, int count);
int unzip_to_dir(char *filename, char *destination);
int do_install(void);
int mkdirall(char *dirname);
int copyfile(char *dname, char *sname);
