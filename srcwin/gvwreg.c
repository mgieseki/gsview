/* Copyright (C) 2005-2011, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwreg.c */

/* GSview Windows registration */

/* before changing this file, please see gvcreg.cpp */

#include "gvwin.h"

#define REG_KEY_NAME TEXT("Software\\Ghostgum\\GSview")
#define REGISTRATION_RECEIPT TEXT("Receipt")
#define REGISTRATION_NUMBER TEXT("Number")
#define REGISTRATION_NAME TEXT("Name")


/* display error message if reading or writing a registry key fails */
void registry_error(HKEY root, const TCHAR *name, const TCHAR *value, 
    BOOL bRead, LONG rc)
{
    char buf[1024];
    LPVOID lpMessageBuffer;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM,
	NULL, rc,
	MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* user default language */
	(LPSTR) &lpMessageBuffer, 0, NULL);
    if (lpMessageBuffer) {
	gs_addmess( bRead ? "Failed to read registry\n" :
	    "Failed to write registry\n");
	if (root == HKEY_CLASSES_ROOT)
	    gs_addmess("HKEY_CLASSES_ROOT\\");
	else if (root == HKEY_LOCAL_MACHINE)
	    gs_addmess("HKEY_LOCAL_MACHINE\\");
	else if (root == HKEY_CURRENT_USER)
	    gs_addmess("HKEY_CURRENT_USER\\");
	else
	    gs_addmess("???\\");
	convert_widechar(buf, name, sizeof(buf)-1);
	gs_addmess(buf);
	gs_addmess("\n");
	if (value) {
	    convert_widechar(buf, value, sizeof(buf)-1);
	    gs_addmessf("  %s\n", buf);
	}
	gs_addmessf("Error=%d\n", rc);
	convert_widechar(buf, (LPTSTR)lpMessageBuffer, sizeof(buf)-1);
	gs_addmess(buf);
	gs_addmess("\r\n");
	LocalFree(LocalHandle(lpMessageBuffer));
    }
    if (rc == 5)
	gs_addmess("You need administrator privileges to save registration details.\n");
    gs_showmess();
}

/* Run the external program gsvwrg32/64.exe, which will invoke
 * process elevation, then save the registration details.
 */
LONG
run_gsvwrg(unsigned int reg_receipt, unsigned int reg_number,
  TCHAR *reg_name)
{
    HINSTANCE hinst;
    TCHAR command[MAXSTR+MAXSTR];
    TCHAR params[MAXSTR+MAXSTR];
#ifdef _WIN64
    const TCHAR prog[] = "gsvwrg64.exe";
#else
    const TCHAR prog[] = "gsvwrg32.exe";
#endif
    wsprintf(command, "\042%s%s\042", szExePath, prog);
    wsprintf(params, "%d %d \042%s\042", reg_receipt, reg_number, reg_name);

    hinst = ShellExecute(NULL, "runas", command, params, NULL, SW_SHOWNORMAL);
    if ((int)hinst < 32)
	return GetLastError();

    return ERROR_SUCCESS;
}


BOOL
write_registration(unsigned int reg_receipt, unsigned int reg_number,
  TCHAR *reg_name)
{
    LONG rc = -1;
    HKEY hkey = NULL;
    DWORD dwValue;
    HKEY root;
    TCHAR *name;
    TCHAR *value;
   
#ifndef UNICODE
    if (portable_app) {
	char profile[MAXSTR];
	char *section = INISECTION;
	PROFILE *prf;
	prf = profile_open(szIniFile);
	profile_write_string(prf, section, "RegistrationName", reg_name);
	sprintf(profile, "%u", reg_receipt);
	profile_write_string(prf, section, "RegistrationReceipt", profile);
	sprintf(profile, "%u", (reg_number ^ 0xffff));
	profile_write_string(prf, section, "RegistrationNumber", profile);
	profile_close(prf);
    }
    else 
#endif
    {
	root = HKEY_LOCAL_MACHINE;
	name = REG_KEY_NAME;
	value = NULL;
	if ((rc = RegOpenKeyEx(root, name, 0, 
		KEY_ALL_ACCESS, &hkey)) != ERROR_SUCCESS) {
	    /* failed to open key, so try to create it */
	    rc = RegCreateKey(root, name, &hkey);
	}

	if (rc == ERROR_SUCCESS) {
	    dwValue = (DWORD)reg_receipt;
	    value = REGISTRATION_RECEIPT;
	    rc = RegSetValueEx(hkey, value, 0, REG_DWORD,
			(CONST BYTE *)&dwValue, sizeof(DWORD));

	    dwValue = (DWORD)(reg_number ^ 0xffff);
	    if (rc == ERROR_SUCCESS) {
	        value = REGISTRATION_NUMBER;
		rc = RegSetValueEx(hkey, value, 0, REG_DWORD,
			(CONST BYTE *)&dwValue, sizeof(DWORD));
	    }

	    if (rc == ERROR_SUCCESS) {
	        value = REGISTRATION_NAME;
		rc = RegSetValueEx(hkey, value, 0, REG_SZ,
			(CONST BYTE *)reg_name, lstrlen(reg_name)+1);
	    }
	    RegCloseKey(hkey);
	}

	if (rc != ERROR_SUCCESS) {
	    /* Run gsvwrg32/64.exe to request process elevation
 	     * and then save the registration details.
	     */
	    rc = run_gsvwrg(reg_receipt, reg_number, reg_name);
	}
	
	if (rc != ERROR_SUCCESS) {
	    registry_error(root, name, value, FALSE, rc);
	    return FALSE;
	}
    }
    return TRUE;
}

#ifdef PREREGISTER
#include "../reg.h"
#endif

BOOL
read_registration(unsigned int *preg_receipt, unsigned int *preg_number,
  TCHAR *reg_name, int reg_len)
{
    LONG rc;
    HKEY hkey;
    DWORD dwValue;
    DWORD cbData;
    DWORD keytype;
    HKEY root;
    TCHAR *name;
    TCHAR *value;

#ifdef PREREGISTER
    *preg_receipt = PREREG_RECEIPT;
    *preg_number = make_reg(*preg_receipt);
    lstrcpyn(reg_name, PREREG_NAME, reg_len-1);
#else
#ifndef UNICODE
    if (portable_app) {
	unsigned int i;
	char profile[MAXSTR];
	char *section = INISECTION;
	PROFILE *prf;
	prf = profile_open(szIniFile);
	profile_read_string(prf, section, "RegistrationReceipt", "", 
		profile, sizeof(profile));
	if (sscanf(profile,"%u", &i) == 1)
	    *preg_receipt = i;
	profile_read_string(prf, section, "RegistrationNumber", "", 
		profile, sizeof(profile));
	if (sscanf(profile,"%u", &i) == 1)
	    *preg_number = i ^ 0xffff;
	profile_read_string(prf, section, "RegistrationName", "", 
		reg_name, reg_len);
	profile_close(prf);
    }
    else 
#endif
    {
	root = HKEY_LOCAL_MACHINE;
	name = REG_KEY_NAME;
	value = NULL;
	rc = RegOpenKeyEx(root, name, 0, KEY_READ, &hkey);

	if (rc == ERROR_SUCCESS) {
	    cbData = sizeof(dwValue);
	    keytype =  REG_DWORD;
	    if (rc == ERROR_SUCCESS) {
		value = REGISTRATION_RECEIPT;
		if ((rc = RegQueryValueEx(hkey, value, 0, &keytype, 
		    (LPBYTE)&dwValue, &cbData)) == ERROR_SUCCESS)
		    *preg_receipt = dwValue;
	    }

	    cbData = sizeof(dwValue);
	    keytype =  REG_DWORD;
	    if (rc == ERROR_SUCCESS) {
		value = REGISTRATION_NUMBER;
		if ((rc = RegQueryValueEx(hkey, value, 0, &keytype, 
		    (LPBYTE)&dwValue, &cbData)) == ERROR_SUCCESS)
		    *preg_number = dwValue ^ 0xffff;
	    }

	    cbData = reg_len;
	    keytype =  REG_SZ;
	    if (rc == ERROR_SUCCESS) {
		value = REGISTRATION_NAME;
		rc = RegQueryValueEx(hkey, value, 0, &keytype, 
		    (LPBYTE)reg_name, &cbData);
	    }

	    RegCloseKey(hkey);
	    if ((rc != ERROR_SUCCESS) && (rc != ERROR_FILE_NOT_FOUND))
		registry_error(root, name, value, TRUE, rc);
	}
	else {
	    if (rc != ERROR_FILE_NOT_FOUND)
		registry_error(root, name, value, TRUE, rc);

	}
	
	if (rc != ERROR_SUCCESS)
	    return FALSE;
    }
#endif
    return TRUE;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
RegDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
	    centre_dialog(hDlg);
            return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case IDOK:
		    {int reg_num;
		    int reg_receipt;
		    TCHAR buf[MAXSTR];
		    reg_receipt = GetDlgItemInt(hDlg, REGDLG_RECEIPT,
			NULL, FALSE);
		    reg_num = GetDlgItemInt(hDlg, REGDLG_NUMBER, NULL, FALSE);
		    GetDlgItemText(hDlg, REGDLG_NAME, buf, sizeof(buf));
		    if ((reg_receipt != 0) && 
			((unsigned int)reg_num == make_reg(reg_receipt)) &&
			(lstrlen(buf) > 0)) {
			lstrcpyn(registration_name, buf, 
			    sizeof(registration_name)/sizeof(TCHAR)-1);
			registration_receipt = reg_receipt;
			write_registration(registration_receipt,
			    reg_num, registration_name);
			EndDialog(hDlg, IDOK);
		    }
		    else {
			TCHAR buf[MAXSTR];
			load_string(IDS_INVALIDREG, buf, sizeof(buf)-1);
			MessageBox(hDlg, buf, szAppName, 
				MB_ICONEXCLAMATION | MB_OK);
		    }
		    }
                    return(TRUE);
                case IDCANCEL:
                    EndDialog(hDlg, IDCANCEL);
                    return(TRUE);
                case REGDLG_ONLINE:
		    ShellExecute(hDlg, NULL, 
			TEXT("http://www.ghostgum.com.au/index.htm"), 
			NULL, NULL, SW_SHOWNORMAL);
                    return(TRUE);
		case ID_HELP:
		    get_help();
		    return(FALSE);
                default:
                    return(FALSE);
            }
        default:
            return(FALSE);
    }
}
#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
NagDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message) {
        case WM_INITDIALOG:
            SetDlgItemText(hDlg, NAG_NAME, registration_name);
	    if (registration_receipt != 0) {
		TCHAR buf[16];
		wsprintf(buf, TEXT("%u"), registration_receipt);
		SetDlgItemText(hDlg, NAG_RECEIPT, buf);
	    }
	    centre_dialog(hDlg);
            return TRUE;
        case WM_COMMAND:
            switch(LOWORD(wParam)) {
		case IDOK:
                    EndDialog(hDlg, IDOK);
                    return(TRUE);
                case NAG_REGISTER:
                    EndDialog(hDlg, NAG_REGISTER);
                    return(TRUE);
		case ID_HELP:
		    get_help();
		    return(FALSE);
                default:
                    return(FALSE);
            }
        default:
            return(FALSE);
    }
}


/* show registation nag screen */
BOOL registration_nag(void)
{
    int code;
    nHelpTopic = IDS_TOPICREG;
    code = DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_NAG), hwndimg, 
	NagDlgProc, (LPARAM)NULL);
    if (code == NAG_REGISTER) {
	if (DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_REG), hwndimg, 
	    RegDlgProc, (LPARAM)NULL) == IDOK)
	    return TRUE;
    }
    else if (code == -1) {
	/* Somebody removed the nag dialog from the resources */
	MessageBeep(-1);
	Sleep(15000);
	MessageBeep(-1);
    }
    return FALSE;
}


#ifdef NOTUSED
int main(int argc, char *argv[])
{
    int receipt_number;
    if (argc != 2) {
	printf("Calculate GSview registration number\n");
	printf("Usage: reg number\n");
	return 1;
    }
    receipt_number = atoi(argv[1]);
    printf("%d-%d\n", receipt_number, make_reg(receipt_number));
    return 0;
}
#endif
