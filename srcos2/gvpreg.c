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

/* gvpreg.cpp */

/* GSview OS/2 registration */

/* before changing this file, please see gvcreg.cpp */

#include "gvpm.h"

BOOL
write_registration(unsigned int reg_receipt, unsigned int reg_number,
  char *reg_name)
{
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
    return TRUE;
}


BOOL
read_registration(unsigned int *preg_receipt, unsigned int *preg_number,
  char *reg_name, int reg_len)
{
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
    return TRUE;
}

/* Reg Dialog Box */
MRESULT EXPENTRY RegDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  switch(msg) {
    case WM_INITDLG:
	WinSendMsg( WinWindowFromID(hwnd, REGDLG_NAME),
	    EM_SETTEXTLIMIT, MPFROM2SHORT(MAXSTR, 0), MPFROMLONG(0) );
	break;
    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1)) {
        case DID_OK:
	    {unsigned int reg_num;
	    unsigned int reg_receipt;
	    char buf[MAXSTR];
	    WinQueryWindowText(WinWindowFromID(hwnd, REGDLG_RECEIPT),
                sizeof(buf), (PBYTE)buf);
	    reg_receipt = (unsigned int)atoi(buf);
	    WinQueryWindowText(WinWindowFromID(hwnd, REGDLG_NUMBER),
                sizeof(buf), (PBYTE)buf);
	    reg_num = (unsigned int)atoi(buf);
	    WinQueryWindowText(WinWindowFromID(hwnd, REGDLG_NAME),
		    sizeof(buf), (PBYTE)buf);
	    if ((reg_receipt != 0) && 
		(reg_num == make_reg(reg_receipt)) &&
		(strlen(buf) > 0)) {
		strncpy(registration_name, buf, sizeof(registration_name));
		registration_receipt = reg_receipt;
		write_registration(registration_receipt,
		    reg_num, registration_name);
	        WinDismissDlg(hwnd, DID_OK);
	    }
	    else {
		char buf[MAXSTR];
		load_string(IDS_INVALIDREG, buf, sizeof(buf)-1);
		WinMessageBox(HWND_DESKTOP, hwnd, (PCSZ)buf, (PCSZ)szAppName, 
			0, MB_ICONEXCLAMATION | MB_MOVEABLE | MB_OK);
	    }
	    }
	    return (MRESULT)TRUE;
	    return (MRESULT)TRUE;
	case DID_CANCEL:
	    WinDismissDlg(hwnd, DID_CANCEL);
	    return (MRESULT)TRUE;
	case ID_HELP:
	    get_help();
	    return (MRESULT)TRUE;
      }
      break;
  }
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
}

/* Reg Dialog Box */
MRESULT EXPENTRY NagDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  switch(msg) {
    case WM_INITDLG:
	WinSetWindowText( WinWindowFromID(hwnd, NAG_NAME),
	    (PBYTE)registration_name );
	if (registration_receipt != 0) {
	    char buf[16];
	    sprintf(buf, "%u", registration_receipt);
	    WinSetWindowText( WinWindowFromID(hwnd, NAG_RECEIPT),
		(PBYTE)buf );
	}
	break;
    case WM_COMMAND:
      switch(SHORT1FROMMP(mp1)) {
        case DID_OK:
	    WinDismissDlg(hwnd, DID_OK);
	    return (MRESULT)TRUE;
	case NAG_REGISTER:
	    WinDismissDlg(hwnd, NAG_REGISTER);
	    return (MRESULT)TRUE;
	case ID_HELP:
	    get_help();
	    return (MRESULT)TRUE;
      }
      break;
  }
  return WinDefDlgProc(hwnd, msg, mp1, mp2);
}



/* show registation nag screen */
BOOL registration_nag(void)
{
    nHelpTopic = IDS_TOPICREG;
    if (WinDlgBox(HWND_DESKTOP, hwnd_frame, NagDlgProc, hlanguage, 
	IDD_NAG, 0) == NAG_REGISTER) {
	if (WinDlgBox(HWND_DESKTOP, hwnd_frame, RegDlgProc, hlanguage, 
	    IDD_REG, 0) == DID_OK) {
	    return TRUE;
	}
    }
    return FALSE;
}
