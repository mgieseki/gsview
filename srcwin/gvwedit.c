/* Copyright (C) 1998-2005, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwedit.c */
/* Convert PostScript to editable form using pstoedit by Wolfgang Glunz */
#include "gvc.h"
#include "gvcedit.h"

HMODULE pstoeditModule;

PSTOEDIT p2e;

setPstoeditOutputFunction_func * setPstoeditOutputFunction;
getPstoeditDriverInfo_plainC_func * getPstoeditDriverInfo_plainC;
pstoedit_plainC_func * pstoedit_plainC;
pstoedit_checkversion_func * PstoeditCheckVersionFunction;
clearPstoeditDriverInfo_plainC_func * clearPstoeditDriverInfo;
struct DriverDescription_S * pstoedit_driver_info;

int
unload_pstoedit(void)
{
    if (pstoeditModule) {
	gs_addmess("Unloading pstoedit\n");
	/* If we are using the old pstoedit dll 300,
	 * we can't free the driver info because we didn't allocate it.
	 * A small amount of memory will be leaked.
	 * If using pstoedit dll 301, we use clearPstoeditDriverInfo.
	 */
	if ( clearPstoeditDriverInfo != 
	    (clearPstoeditDriverInfo_plainC_func *)NULL ) {
	    clearPstoeditDriverInfo(pstoedit_driver_info);
	}
	pstoedit_driver_info = NULL;
	clearPstoeditDriverInfo = NULL;

	setPstoeditOutputFunction = NULL;
	getPstoeditDriverInfo_plainC = NULL;
	pstoedit_plainC = NULL;
	FreeLibrary(pstoeditModule);
	pstoeditModule = NULL;
	memset(&p2e, 0, sizeof(p2e));
    }
    return 0;
}


BOOL
load_pstoedit(void)
{
char *p;
char dllname[MAXSTR];

    if (pstoeditModule)
	return TRUE;

    gs_addmess("Loading pstoedit\n");
    /* assume that pstoedit is in the adjacent directory */
    convert_widechar(dllname, szExePath, sizeof(dllname)-1);
    p = dllname + strlen(dllname) - 1;	/* remove trailing slash */
    if (*p == '\\')
	*p = '\0';
    p = strrchr(dllname, '\\');		/* remove trailing gsview */
    if (p)
	*(++p) = '\0';
    strcat(dllname, "pstoedit\\pstoedit.dll");

    /* load pstoedit DLL */
    pstoeditModule = LoadLibraryA(dllname);
    if (pstoeditModule < (HINSTANCE)HINSTANCE_ERROR) {
	/* If that failed, try using the registry to locate pstoedit */
	LONG rc;
	HKEY hkey;
	DWORD cbData;
	DWORD keytype;
	TCHAR pstoedit_path[MAXSTR];
	rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\wglunz\\pstoedit",
		0, KEY_READ, &hkey);
	if (rc == ERROR_SUCCESS) {
	    cbData = sizeof(pstoedit_path)-1;
	    keytype =  REG_SZ;
	    rc = RegQueryValueEx(hkey, "InstallPath", 0, &keytype, 
		    (LPBYTE)pstoedit_path, &cbData);
	    RegCloseKey(hkey);
	    if (rc == ERROR_SUCCESS) {
		memset(dllname, 0, sizeof(dllname));
		convert_widechar(dllname, pstoedit_path, sizeof(dllname)-14);
		strncat(dllname, "\\pstoedit.dll", 14);
		pstoeditModule = LoadLibraryA(dllname);
	    }
	}

        if (pstoeditModule < (HINSTANCE)HINSTANCE_ERROR) {
	    gs_addmess("Can't load ");
	    gs_addmess(dllname);
	    gs_addmess("\n");
	    gs_addmess("pstoedit is not available\n");
	    gs_addmess("See help topic 'PStoEdit'\n");
	    pstoeditModule = NULL;
	    return FALSE;
 	}
    }

    /* check version match */
    if ( (PstoeditCheckVersionFunction = (pstoedit_checkversion_func *) 
	GetProcAddress(pstoeditModule, "pstoedit_checkversion"))
	== NULL ) {
	gs_addmess("Can't find pstoedit_checkversion()\n");
	unload_pstoedit();
	return FALSE;
    }
    if (PstoeditCheckVersionFunction(pstoeditdllversion)) {
	/* load the clearPstoeditDriverInfo function, which is only
	 * available in version 301 and later.
	 */
	if ( (clearPstoeditDriverInfo= (clearPstoeditDriverInfo_plainC_func *) 
	    GetProcAddress(pstoeditModule, "clearPstoeditDriverInfo_plainC"))
	    == NULL ) {
	    gs_addmess("Couldn't find clearPstoeditDriverInfo\n");
	    unload_pstoedit();
	    return FALSE;
	}
    }
    else {
	/* We didn't have the latest version.
	 * Check if the older version 300 is available, 
	 * which has memory leak problem but is otherwise OK.
	 */
	char buf[MAXSTR];
	clearPstoeditDriverInfo = (clearPstoeditDriverInfo_plainC_func *)NULL;
	sprintf(buf, "Warning: pstoedit dll version is NOT %d\n", 
		pstoeditdllversion);
	gs_addmess(buf);
        if (!PstoeditCheckVersionFunction(300)) {
	    gs_addmess("wrong version of pstoedit.dll\n");
	    unload_pstoedit();
	    return FALSE;
	}
	else {
	    gs_addmess("Loaded pstoedit dll version 300\n");
	}
    }

    /* install callback function */
    if ( (setPstoeditOutputFunction = (setPstoeditOutputFunction_func *) 
	GetProcAddress(pstoeditModule, "setPstoeditOutputFunction"))
	 == NULL ) {
	gs_addmess("Can't find setPstoeditOutputFunction()\n");
	unload_pstoedit();
	return FALSE;
    }
    setPstoeditOutputFunction(0, /* no Call Back Data ptr */
				pstoedit_callback);

    /* find out which formats (backends) are supported */
    if ( (getPstoeditDriverInfo_plainC = (getPstoeditDriverInfo_plainC_func *)
        GetProcAddress(pstoeditModule, "getPstoeditDriverInfo_plainC"))
	== NULL ) {
	gs_addmess("Can't find getPstoeditDriverInfo_plainC");
	unload_pstoedit();
	return FALSE;
    }
    pstoedit_driver_info = getPstoeditDriverInfo_plainC();

    GetProcAddress(pstoeditModule, "pstoedit_plainC");
    if ( (pstoedit_plainC = 
	 (pstoedit_plainC_func *) 
        GetProcAddress(pstoeditModule, "pstoedit_plainC"))
	== NULL ) {
	gs_addmess("Can't find pstoedit_plainC" );
    }

    memset(&p2e, 0, sizeof(p2e));
    p2e.driver_info = pstoedit_driver_info;

    return TRUE;
}


/* dialog box for pstoedit options */
DLGRETURN CALLBACK _export
PStoEditDlgProc(HWND hDlg, UINT wmsg, WPARAM wParam, LPARAM lParam)
{
struct DriverDescription_S * dd;
char buf[MAXSTR];
float u;
    switch (wmsg) {
	case WM_INITDIALOG:
	    {
	      char defformat[MAXSTR];
	      defformat[0] = '\0';
	      SendDlgItemMessage(hDlg, EDIT_FORMAT, LB_RESETCONTENT, 
		(WPARAM)0, (LPARAM)0);
	      dd = p2e.driver_info;
	      while(dd && (dd->symbolicname) ) {
		sprintf(buf, "%s:  %s %s", dd->symbolicname, dd->explanation,
			dd->additionalInfo);
		if (strcmp(dd->symbolicname, p2e.format->symbolicname) == 0)
		    strcpy(defformat, buf);
		SendDlgItemMessageA(hDlg, EDIT_FORMAT, LB_ADDSTRING, 0, 
		    (LPARAM)((LPSTR)buf));
		dd++;
	      }
	      SendDlgItemMessageA(hDlg, EDIT_FORMAT, LB_SELECTSTRING, -1, 
		    (LPARAM)((LPSTR)defformat));

	      SendDlgItemMessage(hDlg, EDIT_DT, BM_SETCHECK, 
			    (WPARAM)p2e.draw_text_as_polygon, 0L);
	      SendDlgItemMessage(hDlg, EDIT_LATIN1, BM_SETCHECK, 
			    (WPARAM)p2e.map_to_latin1, 0L);
	      if (p2e.flatness > 0) {
	          sprintf(buf, "%g", p2e.flatness);
	          SetDlgItemTextA(hDlg, EDIT_FLAT, buf);
	      }
	      SetDlgItemTextA(hDlg, EDIT_FONT, p2e.default_font);
	      SetDlgItemTextA(hDlg, EDIT_OPTION, p2e.driver_option);

	      
	    }
	    return TRUE;
	case WM_COMMAND:
	    switch (LOWORD(wParam)) {
		case ID_HELP:
		    get_help();
		    return FALSE;
		case IDOK:
		    /* get format */
		    SendDlgItemMessageA(hDlg, EDIT_FORMAT, LB_GETTEXT, 
			(int)SendDlgItemMessage(hDlg, EDIT_FORMAT, 
			    LB_GETCURSEL, 0, 0L), (LPARAM)(LPSTR)buf);
		    strtok(buf, ":");
		    dd = p2e.driver_info;
		    p2e.format = dd;
		    while(dd && (dd->symbolicname) ) {
			if (strcmp(dd->symbolicname, buf) == 0) {
			    p2e.format = dd;
			    break;
			}
			dd++;
		    }

		    GetDlgItemTextA(hDlg, EDIT_OPTION, p2e.driver_option,
			sizeof(p2e.driver_option));
		    GetDlgItemTextA(hDlg, EDIT_FONT, p2e.default_font,
			sizeof(p2e.default_font));

		    p2e.draw_text_as_polygon = (BOOL)SendDlgItemMessage(hDlg, 
			EDIT_DT, BM_GETCHECK, 0, 0);
		    p2e.map_to_latin1 = (BOOL)SendDlgItemMessage(hDlg, 
			EDIT_LATIN1, BM_GETCHECK, 0, 0);

		    p2e.flatness = (float)GetDlgItemTextA(hDlg, 
			EDIT_FLAT, buf, sizeof(buf));
		    if ( (sscanf(buf, "%f", &u) == 1) && (u > 0.0))
			p2e.flatness = u;
		    else
			p2e.flatness = 0.0;

		    EndDialog(hDlg, TRUE);
		    return TRUE;
		case IDCANCEL:
		    EndDialog(hDlg, FALSE);
		    return TRUE;
	    }
	    break;
    }
	return FALSE;
}


int
gsview_pstoedit(void)
{
BOOL flag;
int pagenum;
    flag = load_pstoedit();
    load_pstoedit_options();

    if (flag) {
        nHelpTopic = IDS_TOPICPSTOEDIT;
	flag = DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_PSTOEDIT), 
	    hwndimg, PStoEditDlgProc, (LPARAM)NULL);
    }
    if (flag)
        save_pstoedit_options();

    if (flag) {
	pagenum = psfile.pagenum;
	if ( (psfile.dsc != (CDSC *)NULL) && (psfile.dsc->page_count != 0) )
	  flag = get_page(&pagenum, 
	    p2e.format->backendSupportsMultiplePages, FALSE);
    }

    if (flag) {
	/* get filename */
	/* Specify extension would be nice, but it is hard to do
	 * with the current get_filename interface.
	 */
	flag = get_filename(p2e.output_filename, TRUE, FILTER_ALL, 
		0, IDS_TOPICPSTOEDIT);
    }

    if (debug && flag) {
      char buf[MAXSTR];
      sprintf(buf, "format=%s dt=%d latin1=%d flat=%g\nfont=%s option=%s\n",
        p2e.format->symbolicname, p2e.draw_text_as_polygon,
	p2e.map_to_latin1, p2e.flatness,
	p2e.default_font, p2e.driver_option);
      gs_addmess(buf);
      sprintf(buf, "filename=%s\n", p2e.output_filename);
      gs_addmess(buf);
    }

    if (flag)
        save_pstoedit_options();

    /* extract pages to temporary file */
    if (flag)
	flag = extract_temp(p2e.temp_filename);

    if (!flag) {
	post_img_message(WM_GSSHOWMESS, 0);
	return FALSE;
    }

    
    /* will need to do the remaining items on the GS thread */

#ifdef NOTUSED
    /* If pstoedit.dll calls gswin32c.exe, then it uses GS in
     * a separate process.  In that case we can start another
     * thread without out disturbing our instance of the GS DLL.
     */
    _beginthread(process_pstoedit, 131072, NULL);
#else
    /* shut down Ghostscript */
    pending.unload = TRUE;	
    pending.now = TRUE;	

    pending.pstoedit = TRUE;
#endif

    return flag;
}


