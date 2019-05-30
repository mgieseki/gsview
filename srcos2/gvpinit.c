/* Copyright (C) 1993-2007, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvpinit.c */
/* Initialisation routines for PM GSview */
#include "gvpm.h"

HELPINIT hi_help;
APIRET init1(void); 
APIRET init2(void); 
APIRET restore_window_position(SWP *pswp);
ULONG frame_flags = 
	    FCF_TITLEBAR |	/* have a title bar */
            FCF_SIZEBORDER |	/* have a sizeable window */
            FCF_MINMAX |	/* have a min and max button */
            FCF_SYSMENU | 	/* include a system menu */
            FCF_VERTSCROLL |	/* vertical scroll bar */
            FCF_HORZSCROLL |	/* horizontal scroll bar */
	    FCF_TASKLIST |	/* show it in window list */
	    FCF_ICON;		/* Load icon from resources */
#ifdef UNUSED
/* need to load these from a different module */
	    FCF_MENU |		/* Load menu from resources */
	    FCF_ACCELTABLE;	/* Load accelerator table from resources */
#endif

BOOL parse_args(GSVIEW_ARGS *pargs);


/* This code allows GSview to automatically find the language DLLs.
 * This allows languages to be added without having to alter the
 * source code of the GSview executable.
 */

/* At startup, we look for language DLLs in the GSview EXE directory
 * which have the following name.  The ?? is replaced by the two
 * letter Internet country code.
 */
const TCHAR lang_pattern[] = TEXT("gvpm??.dll");
const int lang_offset = 4;  /* offset to ?? */

/* We load each of these DLLs and if they contain a version
 * string that matches the GSview EXE, we consider them to
 * be a valid language DLL.  We store the details about each
 * in the following structure.  
 * The id is allocated automatically.
 * We refer to each language in the INI file using the twocc code, 
 * which matches the two letter Internet country code.
 * Each language DLL provides a string with the name of the language
 * in it's own language and also in English.  The latter is used 
 * on systems without that language installed.
 * The required ANSI codepage is in codepage.  If any code page is
 * acceptable (e.g. English) then this may be 0.
 */
typedef struct lang_s {
    int id;
    char twocc[3];
    char dllname[MAXSTR];
    char name[MAXSTR];	/* language name in it's language and codepage */
    char ename[MAXSTR];	/* language name in English */
    int codepage;
} lang_t;

/* The list of language DLLs available */
#define MAXLANG 19
lang_t lang[MAXLANG];
int nlang;

int
language_find(void)
{
ULONG cFilenames = 1;
    int i;
    FILEFINDBUF3 findbuf;
    HDIR hdir;
    CHAR dirname[MAXSTR];
    CHAR pattern[1024];
    CHAR buf[MAXSTR];
    int twocc_index;
    HMODULE hmodule;
    APIRET rc;

    for (i=0; i<MAXLANG; i++) {
	lang[i].id = 0;
	lang[i].twocc[0] = '\0';
	lang[i].dllname[0] = '\0';
    }

    /* First language is always English */
    nlang = 0;
    lang[nlang].id = IDM_LANGMENU + nlang + 1;
    lang[nlang].twocc[0] = 'e';
    lang[nlang].twocc[1] = 'n';
    lang[nlang].twocc[2] = '\0';
    lang[nlang].dllname[0] = '\0';
    strcpy(lang[nlang].name, "English");
    strcpy(lang[nlang].ename, "English");
    nlang++;

    /* Now search for language DLLs */
    strcpy(dirname, szExePath);
    strcpy(pattern, dirname);
    strcat(pattern, lang_pattern);
    twocc_index = strlen(pattern - 6);
    memset(&findbuf, 0, sizeof(findbuf));

    hdir = HDIR_CREATE;
    cFilenames = 1;
    rc = DosFindFirst((PBYTE)pattern, &hdir, FILE_NORMAL,
	    &findbuf, sizeof(findbuf), &cFilenames, FIL_STANDARD);
    while (!rc) {
        lang[nlang].id = IDM_LANGMENU + nlang + 1;
	lang[nlang].twocc[0] = findbuf.achName[lang_offset];
	lang[nlang].twocc[1] = findbuf.achName[lang_offset+1];
	lang[nlang].twocc[2] = '\0';
	if (strlen(dirname) + strlen(findbuf.achName) < 
	    sizeof(lang[nlang].dllname)-1) {
	    strcpy(lang[nlang].dllname, dirname); 
	    strcat(lang[nlang].dllname, findbuf.achName); 
 	}
	else
	    lang[nlang].id = 0;	/* can't do it */

	/* Try opening DLL */
	rc = DosLoadModule((PBYTE)buf, sizeof(buf), 
	    (PCSZ)lang[nlang].dllname, &hmodule);
	if (rc) {
	    gs_addmessf("Failed loading %s, rc=%d\n", lang[nlang].dllname, rc);
	    lang[nlang].id = 0;
	}
	else {
	    char vbuf[MAXSTR];
	    /* Make sure DLL version matches EXE */
	    memset(vbuf, 0, sizeof(vbuf));
	    WinLoadString(hab, hmodule, IDS_GSVIEWVERSION, sizeof(vbuf), 
		(unsigned char *)vbuf);
	    if (strcmp(TEXT(GSVIEW_DOT_VERSION), vbuf) != 0)
		lang[nlang].id = 0;
	    else  {
		/* Load string of language name */
		strncpy(lang[nlang].name, lang[nlang].twocc, sizeof(lang[nlang].name));
	        WinLoadString(hab, hmodule, IDS_LANGUAGE, 
		  sizeof(lang[nlang].name), (unsigned char *)lang[nlang].name);
	        WinLoadString(hab, hmodule, IDS_ELANGUAGE, 
		  sizeof(lang[nlang].ename), (unsigned char *)lang[nlang].ename);
		/* Get required code page */
	        WinLoadString(hab, hmodule, IDS_CODEPAGE, sizeof(vbuf), 
		    (unsigned char *)vbuf);
		lang[nlang].codepage = atoi(vbuf);
	    }
	    DosFreeModule(hmodule);
	}

	if (lang[nlang].id != 0) {
	    /* found a valid language */
	    nlang++;
	}

	cFilenames = 1;
	rc = DosFindNext(hdir, &findbuf, sizeof(findbuf), &cFilenames);
    }
    DosFindClose(hdir);

    return 0;
}

int language_free(void)
{
    int i;
    for (i=0; i<nlang; i++) {
	lang[i].id = 0;
	lang[i].twocc[0] = '\0';
	lang[i].dllname[0] = '\0';
    }
    return 0;
}

int language_id(const char *str)
{
    int i;
    for (i=0; i<nlang; i++) {
	if (strcmp(str, lang[i].twocc) == 0)
	    return lang[i].id;
    }
    return 0;
}

int language_lookup(int id)
{
    int i;
    for (i=0; i<nlang; i++) {
	if (id == lang[i].id)
	    return i;
    }
    return 0;
}

const char * language_twocc(int id)
{
    int i;
    for (i=0; i<nlang; i++) {
	if (id == lang[i].id)
	    return lang[i].twocc;
    }
    return "en";
}

const TCHAR * language_dllname(int id)
{
    int i;
    for (i=0; i<nlang; i++) {
	if (id == lang[i].id)
	    return lang[i].dllname;
    }
    return TEXT("");
}

/* load language dependent resources */
/* returns TRUE if language change successful */
BOOL
load_language(int language)
{
char langdll[MAXSTR];
char buf[MAXSTR];
HMODULE hmodule;
APIRET rc;

    if (language == IDM_LANGEN) {
	if (hlanguage)
	    DosFreeModule(hlanguage);
	/* Don't load a DLL - English is in main EXE resources */
	hlanguage = (HMODULE)NULL;
	/* English uses Western European code page */
	DosSetProcessCp(850);
	WinSetCp(hand_mq, 850);
	return TRUE;
    }

    strcpy(langdll, language_dllname(language));
    rc = DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)langdll, &hmodule);
    if (!rc) {
	if (hlanguage)
	    DosFreeModule(hlanguage);
	hlanguage = hmodule;

	/* Set codepage for this language */
	DosSetProcessCp(lang[language_lookup(language)].codepage);
	WinSetCp(hand_mq, lang[language_lookup(language)].codepage);

	load_string(IDS_GSVIEWVERSION, langdll, sizeof(langdll));
	if (strcmp(GSVIEW_DOT_VERSION, langdll) != 0)
	    message_box("Language resources version doesn't match GSview EXE", 0);

	return TRUE;
    }

    return FALSE;
}

void
change_language(void)
{
char *p;
char helptitle[MAXSTR];
int i;
#define MAXCP 32
    ULONG aulCodepage[MAXCP];
    int ncp;

HWND hwndMenu;
MENUITEM mi;
MENUITEM mii;

    /* Get list of code pages supported by PM */
    ncp = WinQueryCpList(hab, MAXCP, (PULONG)aulCodepage);

    WinEnableWindowUpdate(hwnd_frame, FALSE);
    if (haccel)
	WinDestroyAccelTable(haccel);
    if (hwnd_menu)
	WinDestroyWindow(hwnd_menu);
    hwnd_menu = WinLoadMenu(hwnd_frame, hlanguage, ID_GSVIEW);

    /* User media */
    hwndMenu = WinWindowFromID(hwnd_frame, FID_MENU);
    WinSendMsg(hwndMenu, MM_QUERYITEM, 
	    MPFROM2SHORT(IDM_MEDIAMENU, TRUE), MPFROMP(&mi));
    mii.iPosition = MIT_END;
    mii.afStyle = MIS_TEXT;
    mii.afAttribute = 0;
    mii.hwndSubMenu = (HWND)NULL;
    mii.hItem = 0;
    for (i=0; i<(int)(sizeof(usermedia)/sizeof(USERMEDIA)-1); i++) {
	if (usermedia[i].name[0]) {
	    mii.id = usermedia[i].id;
	    WinSendMsg(mi.hwndSubMenu, MM_INSERTITEM, 
		(MPARAM)&mii, (MPARAM)usermedia[i].name);
	}
    }

    /* Add extra languages to menu */
    {   ULONG arCP[3];
	ULONG pcCP = 0;
	int codepage = 0;
	int i;
	char *language_name;

	if ((DosQueryCp(sizeof(arCP), arCP, &pcCP)==0) && pcCP)
	    codepage = (int)(arCP[0]);
	hwndMenu = WinWindowFromID(hwnd_frame, FID_MENU);
	WinSendMsg(hwndMenu, MM_QUERYITEM, 
		MPFROM2SHORT(IDM_LANGMENU, TRUE), MPFROMP(&mi));
	mii.iPosition = MIT_END;
	mii.afStyle = MIS_TEXT;
	mii.afAttribute = 0;
	mii.hwndSubMenu = (HWND)NULL;
	mii.hItem = 0;
	for (i=1; i<nlang; i++) {
	    mii.id = lang[i].id;
	    if (codepage && lang[i].codepage && (codepage != lang[i].codepage))
		language_name = lang[i].ename;	/* English version of name */
	    else
		language_name = lang[i].name;	/* Native name */
	    WinSendMsg(mi.hwndSubMenu, MM_INSERTITEM, 
		(MPARAM)&mii, (MPARAM)language_name);
	    if (codepage && lang[i].codepage) {
		int j;
		for (j=0; j<ncp; j++) {
		    if (aulCodepage[j] == (ULONG)(lang[i].codepage))
			break;	/* PM supports this codepage */
		}
		if (j == ncp)
		    enable_menu_item(IDM_LANGMENU, lang[i].id, FALSE);
	    }
	}
    }

    haccel = WinLoadAccelTable(hab, hlanguage, ID_GSVIEW);
    WinSetAccelTable(hab, haccel, hwnd_frame);
    frame_flags |= (FCF_MENU | FCF_ACCELTABLE);
    WinSendMsg(hwnd_frame, WM_UPDATEFRAME, (MPARAM)(frame_flags), (MPARAM)0);

    /* get path to help file */
    strcpy(szHelpName, szExePath);
    p = szHelpName + strlen(szHelpName);
    load_string(IDS_HELPFILE, p, sizeof(szHelpName) - (int)(p-szHelpName));
    load_string(IDS_HELPTITLE, helptitle, sizeof(helptitle));

    if (hwnd_help)
	WinDestroyHelpInstance(hwnd_help);
    /* create help window */
    hi_help.cb = sizeof(HELPINIT);
    hi_help.ulReturnCode = 0;
    hi_help.pszTutorialName = NULL;
    hi_help.phtHelpTable = NULL;
    hi_help.hmodAccelActionBarModule = 0;
    hi_help.idAccelTable = 0;
    hi_help.idActionBar = 0;
    hi_help.pszHelpWindowTitle=(PSZ)helptitle;
    hi_help.hmodHelpTableModule = 0;
    hi_help.fShowPanelId = 0;
    hi_help.pszHelpLibraryName = (PSZ)szHelpName;
    hwnd_help = WinCreateHelpInstance(hab, &hi_help);
    if (!hwnd_help || hi_help.ulReturnCode) {
	char buf[512];
	sprintf(buf, "WinCreateHelpInstance helpfile=%s handle=%ld, rc=%ld",  
	    szHelpName, hwnd_help, hi_help.ulReturnCode);
	message_box(buf, 0);
    }
    if (hwnd_help)
	WinAssociateHelpInstance(hwnd_help, hwnd_frame);

    nHelpTopic = IDS_TOPICROOT;
    init_check_menu();

    WinEnableWindowUpdate(hwnd_frame, TRUE);
    WinShowWindow(hwnd_frame, TRUE);
}

MRESULT EXPENTRY 
LanguageDlgProc(HWND hwnd, ULONG mess, MPARAM mp1, MPARAM mp2)
{
    switch(mess) {
	case WM_INITDLG:
	  {
	    ULONG arCP[3];
	    ULONG pcCP = 0;
	    int codepage = 0;
	    int i;
	    HWND hwndButton;
	    SWP swp;
	    char *language_name;
	    swp.fl = SWP_ACTIVATE | SWP_SIZE | SWP_SHOW;
	    if ((DosQueryCp(sizeof(arCP), arCP, &pcCP)==0) && pcCP)
		codepage = (int)(arCP[0]);
	    for (i=0; i<nlang; i++) {
		if (codepage && lang[i].codepage && (codepage != lang[i].codepage))
		    language_name = lang[i].ename;	/* English version of name */
		else
		    language_name = lang[i].name;	/* Native name */
		hwndButton = WinCreateWindow(hwnd, WC_BUTTON,
		    (PCSZ)(language_name),
		    WS_VISIBLE | WS_TABSTOP | WS_GROUP,
		    35, 16 + (nlang - 1 - i) * 32, 110, 26,
		    hwnd,
		    HWND_TOP,
		    lang[i].id,
		    NULL,
		    NULL);
		if (codepage && lang[i].codepage && 
		    (codepage != lang[i].codepage))
		    WinEnableWindow(hwndButton, FALSE);
	    }
	    swp.cx = 35 + 110 + 35 + 
		2 * WinQuerySysValue (HWND_DESKTOP, SV_CXSIZEBORDER);
	    swp.cy = 16 + nlang * 32 + 
		2 * WinQuerySysValue (HWND_DESKTOP, SV_CYSIZEBORDER) +
		WinQuerySysValue (HWND_DESKTOP, SV_CYTITLEBAR) ;
	    WinSetWindowPos(hwnd, HWND_TOP,
		0, 0, swp.cx, swp.cy, swp.fl);
	  }
	  break;
        case WM_COMMAND:
	    if ((SHORT1FROMMP(mp1) >  IDM_LANGMENU)
		&& (SHORT1FROMMP(mp1) <= IDM_LANGLAST)) {
		WinDismissDlg(hwnd, SHORT1FROMMP(mp1));
		break;
	    }
            switch(SHORT1FROMMP(mp1)) {
		case DID_CANCEL:
                case DID_OK:
                    WinDismissDlg(hwnd, 0);
                    break;
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
char buf[MAXSTR];
char *p;
    pcc.country = 0;	/* ask about default country */
    pcc.codepage = 0;
    if (DosQueryCtryInfo(sizeof(pci), &pcc, &pci, &pcbActual) != 0)
	return;	/* give up */

   if (pcbActual == 0)
	return;

    WinLoadString(hab, hlanguage, IDS_SLANGUAGE, sizeof(buf), 
	(unsigned char *)buf);
    p = strtok(buf, ",");
    while (p) {
	if (pci.country == (unsigned int)atoi(p))
	    return;	/* country matches GSview language */
        p = strtok(NULL, ",");
    }

    /* GSview language doesn't match country code */
    language = WinDlgBox(HWND_DESKTOP, hwnd_frame, LanguageDlgProc, 
	hlanguage, IDD_LANG, NULL);
    if (language) 
	gsview_language(language);
}

APIRET
gsview_init(int argc, char *argv[])
{
    unsigned char szClass[] = "gvBmpClass";  /* class name */
    unsigned char status_class[] = "gvStatusClass";
    unsigned char button_class[] = "gvButtonClass";
    APIRET rc = 0;
    SWP swp;
    FONTMETRICS fm;
    POINTL char_size;
    RECTL rect;
    HPS hps;
    PVOID pResource;
    int i;
    short *pButtonID;
    ULONG version[3];
    int badarg;

    args.multithread = TRUE;
    badarg = parse_argv(&args, argc, argv);
    parse_args(&args);
    /* IMPORTANT: debug must be set before malloc is called. */

    if (DosQuerySysInfo(QSV_VERSION_MAJOR, QSV_VERSION_REVISION, 
	&version, sizeof(version))) os_version = 201000;  /* a guess */
    else
	os_version = version[0]*10000 + version[1]*100 + version[2];

    rc = init1();
    if (rc)
	return rc;

    /* save SHELL default size and location */
    rc = WinQueryTaskSizePos(hab, 0, &swp);
    if (rc)
	return rc;

    load_string(IDS_WAIT, szWait, sizeof(szWait));

    init_options();
    read_profile(szIniFile);
    use_args(&args);

    view_init(&view);

    if (!load_language(option.language)) {
	message_box("Couldn't load language specific resources.  Resetting to English.", 0);
	option.language = IDM_LANGEN;
	if (!load_language(option.language)) {
	    message_box("Couldn't load English resources.  Please reinstall GSview", 0);
	    return (APIRET)-1;
	}
    }

    if (!WinRegisterClass(	/* register this window class */
  	hab,			/* anchor block */
  	szClass,		/* class name */
  	(PFNWP) ClientWndProc,	/* window function */
  	CS_SIZEREDRAW |		/* window style */
	CS_MOVENOTIFY,		
  	0))			/* no storage */
	return (APIRET)-1;

    hwnd_frame = WinCreateStdWindow(
  	HWND_DESKTOP,		/* window type */
	0,			/* frame style is not WS_VISIBLE */
  	&frame_flags,		/* definitions */
  	szClass,		/* client class */
  	(PSZ)szAppName,		/* title */
  	WS_VISIBLE,		/* client style */
  	0,			/* resource module */
  	ID_GSVIEW,		/* resource identifier */
  	&hwnd_bmp);		/* pointer to client */
    hwnd_image = hwnd_bmp;

    change_language();

    if (!WinRegisterClass(	/* register this window class */
  	hab,			/* anchor block */
  	(PSZ)status_class,	/* class name */
  	(PFNWP) StatusWndProc,	/* window function */
  	CS_SIZEREDRAW,		/* window style */
  	0))			/* no storage */
	return (APIRET)-1;

    if (!WinRegisterClass(	/* register this window class */
  	hab,			/* anchor block */
  	(PSZ)button_class,	/* class name */
  	(PFNWP) ButtonWndProc,	/* window function */
  	CS_SIZEREDRAW | CS_MOVENOTIFY,		/* window style */
  	0))			/* no storage */
	return (APIRET)-1;

    hptr_crosshair = WinLoadPointer(HWND_DESKTOP, 0, IDP_CROSSHAIR);
    hptr_hand = WinLoadPointer(HWND_DESKTOP, 0, IDP_HAND);

    /* get initial size and position of status window */
    WinQueryWindowRect(hwnd_bmp, &rect);
    statusbar.x = rect.xRight - rect.xLeft;
    hps = WinGetPS(hwnd_bmp);
    GpiQueryFontMetrics(hps, sizeof(FONTMETRICS), &fm);
    char_size.y = fm.lMaxAscender + fm.lMaxDescender + fm.lExternalLeading + 2;
    char_size.x = fm.lAveCharWidth;
    statusbar.y = char_size.y;
    WinReleasePS(hwnd_bmp);

    hwnd_status = WinCreateWindow(
    	hwnd_frame,
    	status_class,
    	(PCSZ)"status bar",
	WS_VISIBLE | WS_SYNCPAINT,
	rect.xLeft, rect.yTop, rect.xRight, statusbar.y,
	hwnd_frame,
	HWND_TOP,
	ID_STATUSBAR,
	NULL,
	NULL);

    button_size.x = 24;
    button_size.y = 24;
    button_shift.x = 0;
    button_shift.y = button_size.y;
    buttonbar.x = button_size.x;
    hwnd_button = WinCreateWindow(
    	hwnd_frame,
    	button_class,
    	(PCSZ)"button bar",
	(option.button_show ? WS_VISIBLE : 0) | WS_SYNCPAINT,
	rect.xLeft, rect.yBottom, 
	rect.xLeft+buttonbar.x, rect.yTop - rect.yBottom,
	hwnd_frame,
	hwnd_bmp,
	ID_BUTTONBAR,
	NULL,
	NULL);
    
    rc = DosGetResource((HMODULE)0, RT_RCDATA, IDR_BUTTON ,&pResource);
    if ( rc || (pResource == NULL) )
	return rc;
    pButtonID = (short *)pResource;
    WinQueryWindowRect(hwnd_button, &rect);
    hps = WinGetPS(hwnd_button);
    for (i=0; pButtonID[i]; i++) {
        char title[16];
	HBITMAP hbm;
	title[0] = '\0';
	hbm = GpiLoadBitmap(hps, 0, pButtonID[i], 0, 0);
	if (!hbm) {
	    load_string(pButtonID[i], title, sizeof(title));
	}
        if (buttonhead == (struct button *)NULL)
                buttontail = buttonhead = (struct button *)malloc(sizeof(struct button));
        else {
                buttontail->next = (struct button *)malloc(sizeof(struct button));
                buttontail = buttontail->next;
        }
	buttontail->id = pButtonID[i];
        buttontail->hbitmap = hbm;
	buttontail->rect.xLeft = rect.xLeft + i * button_shift.x;
	buttontail->rect.yBottom = rect.yTop - button_size.y - i * button_shift.y;
	buttontail->rect.xRight = buttontail->rect.xLeft + button_size.x;
	buttontail->rect.yTop = buttontail->rect.yBottom + button_size.y;
	strcpy(buttontail->str, title);
        buttontail->next = NULL;
    }
    WinReleasePS(hps);
    DosFreeResource(pResource);

    OldFrameWndProc = WinSubclassWindow(hwnd_frame, (PFNWP)FrameWndProc);

    rc = restore_window_position(&swp);
    if (rc)
	return rc;

    init2();

#ifdef __EMX__
    if (_emx_vcmp < 0x302e3962) {
	char buf[MAXSTR];
	char mess[MAXSTR];
	load_string(IDS_WRONGEMX, mess, sizeof(mess)-1);
	sprintf(buf, mess, _emx_vprt, EMX_NEEDED);
	message_box(buf, MB_ICONEXCLAMATION);
    }
#endif

    if (argc == 1)
	return rc;

    if (!rc)
       rc = DosCreateMutexSem(NULL, &hmutex_ps, 0, FALSE);
    if (rc)
	return rc;

    gsview_initc(argc, argv);

    if (badarg) {
	gs_addmessf("Unknown argument %d: %s\n", badarg, argv[badarg]);
	gserror(IDS_PARSEERROR, NULL, 0, SOUND_ERROR);
    }

    return rc;
}

/* Platform specific preprocessing of arguments */
BOOL
parse_args(GSVIEW_ARGS *args)
{
    char workdir[MAXSTR];
    char *filename = args->filename;
    /* remember the working directory */
    gs_getcwd(workdir, sizeof(workdir));	

    debug = args->debug;
    multithread = args->multithread;
    if (args->print || args->convert) {
	print_silent = TRUE;
	print_exit = TRUE;
    }
    if (filename[0]) {
	/* make sure filename contains full path */
	char fullname[MAXSTR+MAXSTR];
	if (filename[0]=='\\' || filename[0]=='/') {
	    if (filename[1] && (filename[1]=='\\' ||  filename[1]=='/')) {
		/* UNC name */
		/* do nothing */
	    }
	    else {
		/* referenced from root directory, so add drive */
		fullname[0] = workdir[0];
		fullname[1] = workdir[1];
		strncpy(fullname+2, filename, sizeof(fullname)-2);
		strncpy(args->filename, fullname, sizeof(args->filename)-1);
	    }
	}
	else if ( ! (isalpha((int)(filename[0])) && filename[1]==':')) {
	    /* Doesn't include drive code, so add work dir */
	    int j;
	    strcpy(fullname, workdir);
	    j = strlen(workdir) - 1;
	    if ( (j >= 0) && !((fullname[j] == '\\') || (fullname[j] == '/')) )
		strcat(fullname, "\\");
	    strcat(fullname, filename);
	    strncpy(args->filename, fullname, sizeof(args->filename)-1);
	}
	else if (isalpha((int)(filename[0])) && (filename[1]==':') && 
	    !((filename[2]=='\\') || (filename[2]=='/'))) {
	    /* contains drive code, but not directory */
	    char filedir[MAXSTR];
	    strncpy(filedir, filename, 2); /* copy drive code */
	    filedir[2] = '\0';
	    gs_chdir(filedir);
	    /* get path on specified drive */
	    gs_getcwd(fullname, sizeof(fullname));	
	    if (fullname[0] && (fullname[strlen(fullname)-1]!='/')
		&& (fullname[strlen(fullname)-1]!='\\'))
		strcat(fullname, "\\");
	    strcat(fullname, filename+2);	/* append relative path */
	    strncpy(args->filename, fullname, sizeof(args->filename)-1);
	    gs_chdir(workdir);
	}
	else {
	    /* contains full path */
	    /* make this the work dir */
	    char *t;
	    char filedir[MAXSTR];
	    strcpy(filedir, filename);
	    if ( (t = strrchr(filedir, '\\')) != (char *)NULL ) {
		*(++t) = '\0';
		    gs_chdir(filedir);
	    }
	}
    }


    return TRUE;
}


void
post_args(void)
{
    WinPostMsg(hwnd_bmp, WM_COMMAND, (MPARAM)IDM_ARGS, (MPARAM)&args);
}


APIRET
restore_window_position(SWP *pswp)
{
    SWP swp;
    swp.fl = SWP_ACTIVATE | SWP_MOVE | SWP_SIZE | SWP_SHOW;

    if ((option.img_size.x   != CW_USEDEFAULT) &&
	(option.img_size.y   != CW_USEDEFAULT) &&
	(option.img_origin.y != CW_USEDEFAULT) &&
	(option.img_origin.y != CW_USEDEFAULT)) {
        LONG cxClientMax ;
        LONG cyClientMax ;
        LONG cyTitleBar ;
        LONG cxSizeBorder ;
        LONG cySizeBorder ;

	/* do not allow window to be entirely off-screen */
	if (option.img_size.x + option.img_origin.x < 16)
	    option.img_origin.x = 0;
	if (option.img_size.y + option.img_origin.y < 16)
	    option.img_origin.y = 0;

        /* get maximum client window size */
        cxClientMax = WinQuerySysValue (HWND_DESKTOP, SV_CXFULLSCREEN) ;
        cyClientMax = WinQuerySysValue (HWND_DESKTOP, SV_CYFULLSCREEN) ;
        cyTitleBar = WinQuerySysValue (HWND_DESKTOP, SV_CYTITLEBAR) ;
        cxSizeBorder = WinQuerySysValue (HWND_DESKTOP, SV_CXSIZEBORDER) ;
        cySizeBorder = WinQuerySysValue (HWND_DESKTOP, SV_CYSIZEBORDER) ;
        cyClientMax += cyTitleBar ;

        if (option.img_max)
            swp.fl |= SWP_MAXIMIZE;

        /* Make sure x origin is within display boundaries */
        swp.x = option.img_origin.x ;
        if (swp.x < -cxSizeBorder)
            swp.x = 0 ;

        /* Make sure window isn't too wide, or negative value */
        swp.cx = option.img_size.x ;
        if (swp.cx >= cxClientMax || swp.cx < 0) {
            swp.cx = cxClientMax ;
            swp.x = 0 ;
        }

        if ((swp.x + swp.cx) > (cxClientMax + cxSizeBorder))
            swp.x = cxClientMax + cxSizeBorder - swp.cx ;

        /* Make sure y origin is within display boundaries */
        swp.y = option.img_origin.y ;
        if (swp.y < -cySizeBorder)
            swp.y = 0 ;

        /* Make sure window isn't too high, or negative value */
        swp.cy = option.img_size.y ;
        if (swp.cy > cyClientMax || swp.cy < 0) {
            swp.cy = cyClientMax ;
            swp.y = 0 ;
        }

        if ((swp.y + swp.cy) > (cyClientMax + cySizeBorder))
            swp.y = cyClientMax + cySizeBorder - swp.cy ;
    }
    else { /* No saved position -- use supplied position */
	swp = *pswp;
        swp.fl = SWP_ACTIVATE | SWP_MOVE | SWP_SIZE | SWP_SHOW;
    }

    /* don't activate or show window yet */
    /* we might want to print silently */
    swp.fl &= ~(SWP_ACTIVATE | SWP_SHOW);
    swp.fl |= SWP_DEACTIVATE | SWP_HIDE;

    /* Position and size this frame window */
    if (!WinSetWindowPos(hwnd_frame, HWND_TOP,
	swp.x, swp.y, swp.cx, swp.cy, swp.fl))
	return 1;

    return 0;
}

APIRET init1() 
{
    char buf[MAXSTR];
    char *tail, *env;
    APIRET rc = 0;
    char *p;

    PTIB pptib;
    PPIB pppib;

    if ( (rc = DosGetInfoBlocks(&pptib, &pppib)) != 0 ) {
	sprintf(buf,"init1: Couldn't get pid, rc = %ld\n", rc);
	error_message(buf);
	return rc;
    }

    /* get path to EXE */
    if ( (rc = DosQueryModuleName(pppib->pib_hmte, sizeof(szExePath), szExePath)) != 0 ) {
	sprintf(buf,"init1: Couldn't get module name, rc = %ld\n", rc);
	error_message(buf);
	return FALSE;
    }
    if ((tail = strrchr(szExePath,'\\')) != (PCHAR)NULL) {
	tail++;
	*tail = '\0';
    }

    /* Find available language DLLs */
    language_find();

    strcpy(szHelpName, szExePath);
    p = szHelpName + strlen(szHelpName);
    load_string(IDS_HELPFILE, p, sizeof(szHelpName) - (int)(p-szHelpName));

    /* get path to INI directory */
    if ((env = getenv("SYSTEM_INI")) != (char *)NULL) {
	strcpy(szIniFile, env);
        if ((tail = strrchr(szIniFile,'\\')) != (PCHAR)NULL) {
		tail++;
		*tail = '\0';
	}
    }
    else
        strcpy(szIniFile, szExePath);
    strcat(szIniFile, INIFILE);

    if ((env = getenv("MMBASE")) != (char *)NULL) {
	char buf[MAXSTR];
	HMODULE hmodMCIAPI;
        strcpy((char *)szMMini, env);
	strtok((char *)szMMini, ";");
	strcat((char *)szMMini, "\\MMPM.INI");
	/* attempt to load MCIAPI.DLL ourselves so gvpm will work without MMOS2 */
	if (!DosLoadModule((PBYTE)buf, sizeof(buf), (PCSZ)"MCIAPI", &hmodMCIAPI)) {
	    DosQueryProcAddr(hmodMCIAPI, 0, (PCSZ)"mciPlayFile", (PFN *)(&pfnMciPlayFile));
	}
    }


    /* local stuff */
    if (!rc) {
	rc = DosCreateEventSem(NULL, &display.event, 0, FALSE);
  	if (rc)
		error_message("Failed to create display.event semaphore");
    }

    if (!rc) {
	HPS ps = WinGetPS(HWND_DESKTOP);
	HDC hdc = GpiQueryDevice(ps);
	DevQueryCaps(hdc, CAPS_COLOR_PLANES, 1, &display.planes);
	DevQueryCaps(hdc, CAPS_COLOR_BITCOUNT, 1, &display.bitcount);
	DevQueryCaps(hdc, CAPS_ADDITIONAL_GRAPHICS, 1, &display.hasPalMan);
	display.hasPalMan &= CAPS_PALETTE_MANAGER;
	WinReleasePS(ps);
/*
	{ char buf[MAXSTR];
	sprintf(buf,"main: display planes = %d, bitcount = %d",display.planes,display.bitcount);
	message_box(buf, 0);
	}
*/
    }
    return rc;
}

APIRET
init2(void)
{
	init_check_menu();
	update_scroll_bars();
	return 0;
}


void
show_buttons(void)
{
	WinSendMsg(hwnd_frame, WM_UPDATEFRAME, (MPARAM)frame_flags, (MPARAM)0);
}

int
gsview_create_objects(char *groupname)
{
char buf[MAXSTR];
char setup[MAXSTR];
APIRET rc;
    strcpy(buf, szExePath);
    strcat(buf, "\\gvpm.exe");
    sprintf(setup, "EXENAME=%s;ASSOCFILTER=*.ps,*.eps,*.pdf", buf);
    rc = !WinCreateObject((PCSZ)"WPProgram", (PCSZ)"GSview", (PCSZ)setup, (PCSZ)"<WP_DESKTOP>",
	CO_REPLACEIFEXISTS);
    if (rc)
        gserror(IDS_PROGRAMOBJECTFAILED, NULL, 0, SOUND_ERROR);
    return rc;
}

HINSTANCE zlib_hinstance;
PFN_gzopen gzopen;
PFN_gzread gzread;
PFN_gzclose gzclose;

void
unload_zlib(void)
{
    if (zlib_hinstance == (HINSTANCE)NULL)
	return;
    DosFreeModule(zlib_hinstance);
    zlib_hinstance = (HINSTANCE)NULL;
    gzopen = NULL;
    gzread = NULL;
    gzclose = NULL;
}

/* load zlib DLL for gunzip */
BOOL
load_zlib(void)
{   
char buf[MAXSTR];
char buf2[MAXSTR];
APIRET rc;
    if (zlib_hinstance != (HINSTANCE)NULL)
	return TRUE;	/* already loaded */

    strcpy(buf, szExePath);
    strcat(buf, "zlib2.dll");
    memset(buf2, 0, sizeof(buf2));
    rc = DosLoadModule((PBYTE)buf2, sizeof(buf2), (PCSZ)buf, &zlib_hinstance);
    if (rc==0) {
	if ((rc = DosQueryProcAddr(zlib_hinstance, 0, (PCSZ)"gzopen", (PFN *)(&gzopen)))!=0) {
	    unload_zlib();
	}
	else {
	    if ((rc = DosQueryProcAddr(zlib_hinstance, 0, (PCSZ)"gzread", (PFN *)(&gzread)))!=0) {
		unload_zlib();
	    }
	    else {
		if ((rc = DosQueryProcAddr(zlib_hinstance, 0, (PCSZ)"gzclose", (PFN *)(&gzclose)))!=0) {
		    unload_zlib();
		}
	    }
	}
    }
    else
	zlib_hinstance = (HINSTANCE)NULL;

    if (zlib_hinstance == (HINSTANCE)NULL) {
	load_string(IDS_ZLIB_FAIL, buf, sizeof(buf));
	if (message_box(buf, MB_OKCANCEL) == IDOK) {
	    nHelpTopic = IDS_TOPICZLIB;
	    get_help();
	}
	return FALSE;
    }
    
    return TRUE;
}


/***************************/
/* configure dialog wizard */

HWND hwnd_wizard = HWND_DESKTOP;
int gsver = GS_REVISION;

int wiz_exit(HWND hwnd);
int check_gsver(HWND hwnd);
int config_finish(HWND hwnd);
MRESULT EXPENTRY CfgChildDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);
MRESULT EXPENTRY CfgMainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2);

#define GetDlgItemText(hwnd, id, str, len) \
	WinQueryWindowText(WinWindowFromID((hwnd), (id)), (len), (str))
#define SetDlgItemText(hwnd, id, str) \
	WinSetWindowText( WinWindowFromID((hwnd), (id)), (str))

/* hwnd_modeless */
typedef struct tagWIZPAGE {
   int id;		/* resource ID */
   int prev;		/* resource id of previous page */
   int next;		/* resource id of next page */
   int (*func)(HWND);	/* function to run on exit from page */
   HWND hwnd;		/* window handle of dialog */
} WIZPAGE;

WIZPAGE pages[]={
	{IDD_CFG1, IDD_CFG1, IDD_CFG2, NULL, 0},
	{IDD_CFG2, IDD_CFG1, IDD_CFG3, check_gsver, 0},
	{IDD_CFG3, IDD_CFG2, IDD_CFG5, NULL, 0},
	{IDD_CFG4, IDD_CFG3, IDD_CFG5, NULL, 0},  /* orphan for OS/2 */
	{IDD_CFG5, IDD_CFG3, IDD_CFG6, NULL, 0},
	{IDD_CFG6, IDD_CFG5, IDD_CFG7, config_finish, 0},
	{IDD_CFG7, IDD_CFG7, IDD_CFG7, wiz_exit, 0},
	{0, 0, 0, NULL, 0}
};

int
config_wizard(BOOL bVerbose)
{
    /* main dialog box */
    /* we must use modeless dialog box to get the correct dialog control */
    /* handling in the child windows */
    hwnd_wizard = WinLoadDlg(HWND_DESKTOP, hwnd_frame, CfgMainDlgProc, hlanguage, IDD_CFG0, 0);
    WinSetFocus(HWND_DESKTOP, WinWindowFromID(pages[0].hwnd, IDNEXT));
    if (hwnd_wizard == (HWND)NULL)
        message_box("Failed to create config_wizard", 0);
    else
        WinEnableWindow(hwnd_frame, FALSE);
    return 0; /* success */
}


int wiz_exit(HWND hwnd)
{
    WinPostMsg(hwnd_wizard, WM_COMMAND, (MPARAM)IDOK, (MPARAM)0);
    return 0;
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
goto_page(HWND hwnd, int id)
{
WIZPAGE *page;
HWND hbutton;
    page = find_page_from_id(id);
    if (page) {
	WinShowWindow(hwnd, FALSE);		/* hide window */
	WinShowWindow(page->hwnd, TRUE);	/* show window */
	hwnd_modeless = page->hwnd;
	if (WinIsWindowEnabled(WinWindowFromID(page->hwnd, IDNEXT)))
	    hbutton = WinWindowFromID(page->hwnd, IDNEXT);
	else
	    hbutton = WinWindowFromID(page->hwnd, IDCANCEL);
	WinSendMsg(hbutton, BM_SETDEFAULT, (MPARAM)TRUE , (MPARAM)0);
	WinSetFocus(HWND_DESKTOP, hbutton);
	return;
    }
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
add_gsver(HWND hwnd, int offset)
{
char buf[MAXSTR];
int ver;
    GetDlgItemText(hwnd, IDC_CFG20, (PBYTE)buf, sizeof(buf));
    if (strlen(buf) != 4)
	return GS_REVISION;
    ver = (buf[0]-'0')*100 + (buf[2]-'0')*10 + (buf[3]-'0');
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
    GetDlgItemText(hwnd, IDC_CFG20, (PBYTE)buf, sizeof(buf));
    if (strlen(buf) == 4) {
	ver = (buf[0]-'0')*100 + (buf[2]-'0')*10 + (buf[3]-'0');
	if ( (ver > GS_REVISION_MAX) || (ver < GS_REVISION_MIN) )
	    fixit = TRUE;
    }
    else
	fixit = TRUE;
    if (fixit) {
	ver = GS_REVISION;
	sprintf(buf, "%d.%02d", ver / 100, ver % 100);
	SetDlgItemText(hwnd, IDC_CFG20, (PCSZ)buf);
	/* don't move until it is valid */
	return IDD_CFG2;
    }
    
    gsver = ver;
    return 0;
}

/* update GS directory edit field when version number changes */
void
gsdir_fix(HWND hwnd, char *verstr)
{
char buf[MAXSTR];
char *p;
    GetDlgItemText(hwnd, IDC_CFG22, (PBYTE)buf, sizeof(buf));
    if (strlen(buf) < 6)
	return;
    p = buf + strlen(buf) - 4;
    if (isdigit((int)(p[0])) && (p[1]=='.') && 
	isdigit((int)(p[2])) && isdigit((int)(p[3]))) {
	strcpy(p, verstr);
        SetDlgItemText(hwnd, IDC_CFG22, (PCSZ)buf);
    }
    else {
	p = buf + strlen(buf) - 3;
	if (isdigit((int)(p[0])) && (p[1]=='.') && isdigit((int)(p[2]))) {
	    strcpy(p, verstr);
	    SetDlgItemText(hwnd, IDC_CFG22, (PCSZ)buf);
	}
    }
}

int
config_now(void)
{
char buf[MAXSTR];
WIZPAGE *page;
FILE *f;
char *p;

    /* get info from wizard */
    page = find_page_from_id(IDD_CFG2);
    option.gsversion = add_gsver(page->hwnd, 0);
    GetDlgItemText(page->hwnd, IDC_CFG22, (PBYTE)buf, sizeof(buf));
    if (option.gsversion >= 593)
        sprintf(option.gsdll, "%s\\bin\\%s", buf, GS_DLLNAME);
    else
        sprintf(option.gsdll, "%s\\%s", buf, GS_DLLNAME);
    default_gsinclude_from_path(option.gsinclude, buf);
    strcpy(option.gsother, "-dNOPLATFONTS");
    GetDlgItemText(page->hwnd, IDC_CFG23, (PBYTE)buf, sizeof(buf));
    if (strlen(buf)) {
	strcat(option.gsother, " -sFONTPATH=\042");
	strcat(option.gsother, buf);
	strcat(option.gsother, "\042");
    }

    /* check if Ghostscript really has been installed */
    /* first look for the DLL */
    if ( (f = fopen(option.gsdll, "rb")) == (FILE *)NULL ) {
	load_string(IDS_GSNOTINSTALLED, buf, sizeof(buf));
	SetDlgItemText(find_page_from_id(IDD_CFG7)->hwnd, IDC_CFG71,
	    (PCSZ)buf);
	return 1;
    }
    fclose(f);

    /* next look for gs_init.ps */
    strcpy(buf, option.gsinclude);
    p = strchr(buf, ';');	/* remove trailing paths */
    if (p)
	*p = '\0';
    strcat(buf, "\\gs_init.ps");
    strcpy(buf, option.gsdll);
    if ( (f = fopen(buf, "rb")) == (FILE *)NULL ) {
	load_string(IDS_GSLIBNOTINSTALLED, buf, sizeof(buf));
	SetDlgItemText(find_page_from_id(IDD_CFG7)->hwnd, IDC_CFG71,
	    (PCSZ)buf);
	return 1;
    }
    fclose(f);
    /* at this stage we don't look for fonts, but maybe we should */
    

    /* The windows code associates PS and PDF in the registry */
    /* It isn't done that way in OS/2 */


    GetDlgItemText(find_page_from_id(IDD_CFG5)->hwnd, 
	IDC_CFG52, (PBYTE)buf, sizeof(buf));
    if ((BOOL)WinSendMsg( WinWindowFromID(find_page_from_id(IDD_CFG5)->hwnd, IDC_CFG51),
	BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0))
	&& gsview_create_objects(buf))
	return 1;
    
    if ((BOOL)WinSendMsg( WinWindowFromID(find_page_from_id(IDD_CFG3)->hwnd, IDC_CFG32),
	BM_QUERYCHECK, MPFROMLONG(0), MPFROMLONG(0)))
	gsview_printer_profiles();


    option.configured = TRUE;

    write_profile();

    return 0;
}


int
config_finish(HWND hwnd)
{
    WinEnableWindow(WinWindowFromID(hwnd, IDNEXT), FALSE);
    WinEnableWindow(WinWindowFromID(hwnd, IDPREV), FALSE);
    WinEnableWindow(WinWindowFromID(hwnd, IDCANCEL), FALSE);
    if (config_now())
    {	char buf[MAXSTR];
	load_string(IDS_CFG73, buf, sizeof(buf));
	SetDlgItemText(find_page_from_id(IDD_CFG7)->hwnd, IDC_CFG70, (PCSZ)buf);
    }
    return 0;
}



MRESULT EXPENTRY CfgMainDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  switch(msg) {
    case WM_INITDLG:
	/* create child dialog windows */
	{
	    WIZPAGE *page;
	    char buf[MAXSTR];
	    char gsdir[MAXSTR];
	    char *p;
	    for (page=pages; page->id; page++) {
		page->hwnd = WinLoadDlg(hwnd, hwnd, CfgChildDlgProc,
		    hlanguage, page->id, 0);
		WinShowWindow(page->hwnd, FALSE);
	    }
	    WinShowWindow(pages[0].hwnd, TRUE);
	    WinSetFocus(HWND_DESKTOP, WinWindowFromID(pages[0].hwnd, IDNEXT));
	    WinSendMsg(WinWindowFromID(pages[0].hwnd, IDNEXT),
		BM_SETDEFAULT, (MPARAM)TRUE, (MPARAM)0);
	    hwnd_modeless = pages[0].hwnd;

	    /* initialize GS version */
	    page = find_page_from_id(IDD_CFG2);
	    if (page) {
		HWND hwndScroll = WinWindowFromID(page->hwnd, IDC_CFG21);
		WinSendMsg(hwndScroll, SBM_SETSCROLLBAR, MPFROMLONG(1), 
			    MPFROM2SHORT(0, 2));
		sprintf(buf, "%d.%02d", option.gsversion / 100, 
		    option.gsversion % 100);
		SetDlgItemText(page->hwnd, IDC_CFG20, (PCSZ)buf);
		SetDlgItemText(page->hwnd, IDC_CFG22, (PCSZ)szExePath);
		SetDlgItemText(page->hwnd, IDC_CFG23, (PCSZ)"c:\\psfonts");
		WinSendMsg( WinWindowFromID(page->hwnd, IDC_CFG22),
		    EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
		WinSendMsg( WinWindowFromID(page->hwnd, IDC_CFG23),
		    EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	    }

	    /* assume that GS is in the adjacent directory */
	    strcpy(gsdir, szExePath);
	    p = strrchr(gsdir, '\\');	/* remove trailing \ */
	    if (p)
		*p = '\0';
	    p = strrchr(gsdir, '\\');	/* remove trailing gsview */
	    if (p)
		*(++p) = '\0';
	    sprintf(buf, "%d.%02d", option.gsversion / 100, 
		    option.gsversion % 100);
	    strcat(gsdir, "gs");
	    strcat(gsdir, buf);
	    SetDlgItemText(page->hwnd, IDC_CFG22, (PCSZ)gsdir);
	    SetDlgItemText(page->hwnd, IDC_CFG23, (PCSZ)"c:\\psfonts");

	    WinSendMsg(WinWindowFromID(find_page_from_id(IDD_CFG3)->hwnd, IDC_CFG32), 
		    BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
	    WinSendMsg(WinWindowFromID(find_page_from_id(IDD_CFG4)->hwnd, IDC_CFG41), 
		    BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
	    WinSendMsg(WinWindowFromID(find_page_from_id(IDD_CFG4)->hwnd, IDC_CFG42), 
		    BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));

	    /* program group */
	    load_string(IDS_PROGMANGROUP4, buf, sizeof(buf));
	    page = find_page_from_id(IDD_CFG5);
	    if (page) {
		WinSendMsg( WinWindowFromID(page->hwnd, IDC_CFG52),
		    EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
		WinSendMsg(WinWindowFromID(page->hwnd, IDC_CFG51), 
		    BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
		SetDlgItemText(page->hwnd, IDC_CFG52, (PCSZ)buf);
	    }

	}
    	break;
    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1)) {
        case DID_OK:
	case DID_CANCEL:
	  WinEnableWindow(hwnd_frame, TRUE);
	  WinDestroyWindow(hwnd);
	  hwnd_modeless = (HWND)NULL;
	  WinSetFocus(HWND_DESKTOP, hwnd_frame);
	  post_args();
          return (MRESULT)TRUE;
      }
      break;
    case WM_CLOSE:
      WinEnableWindow(hwnd_frame, TRUE);
      WinDestroyWindow(hwnd);
      hwnd_modeless = (HWND)NULL;
      WinSetFocus(HWND_DESKTOP, hwnd_frame);
      post_args();
      return (MRESULT)TRUE;
  }
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
}


MRESULT EXPENTRY CfgChildDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
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
	case IDCANCEL:
		{   char buf[MAXSTR];
		    load_string(IDS_CFG74, buf, sizeof(buf));
		    SetDlgItemText(find_page_from_id(IDD_CFG7)->hwnd, IDC_CFG70, (PCSZ)buf);
		    goto_page(hwnd, IDD_CFG7);
		}
	    return(MRESULT)TRUE;
	default:
	    return(MRESULT)FALSE;
      }
      break;
    case WM_CONTROL:
	if (SHORT1FROMMP(mp1) == IDC_CFG20) {
	    if (SHORT2FROMMP(mp1) == EN_CHANGE) {
		int ver;
		char buf[16];
		ver = add_gsver(hwnd, 0);
		sprintf(buf, "%d.%02d", ver / 100, ver % 100);
		/* don't use or touch IDC_CFG20 - this would be recursive */
		gsdir_fix(hwnd, buf);
	        return (MRESULT)TRUE;
	    }
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
	  WinSetWindowText( WinWindowFromID(hwnd, IDC_CFG20), (PCSZ)buf); 
      }
      return(MRESULT)TRUE;
    case WM_CLOSE:
	WinPostMsg( WinQueryWindow(hwnd, QW_PARENT), WM_CLOSE, (MPARAM)0, (MPARAM)0);
	break;
  }
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
}	

