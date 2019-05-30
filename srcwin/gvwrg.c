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

/* gvwrg.c */

/* GSview Windows registration
 * Executable to save registration with process elevation.
 *
 * If we don't use process elevation, then writing the registration
 * details either fails, or writes to
 *  HKEY_CURRENT_USER\Software\Classes\VirtualStore\MACHINE\SOFTWARE\GHOSTGUM\GSview
 */

#define STRICT
#include <windows.h>
#include <stdio.h>

#define REG_KEY_NAME TEXT("Software\\Ghostgum\\GSview")
#define REGISTRATION_RECEIPT TEXT("Receipt")
#define REGISTRATION_NUMBER TEXT("Number")
#define REGISTRATION_NAME TEXT("Name")

#define gs_addmess(x) fputs(x, stderr)
#define gs_showmess()


unsigned int make_reg(unsigned int receipt_number)
{
    /* MSB of polynomial is in LSB of value */
    /* x^16 + x^12 + x^5 + 1 */
    /* 1 0001 0000 0010 0001 */
    unsigned int crc_gen = 0x8408; /* backwards omitting bit 16 */
    unsigned int bit16;
    unsigned int i;
    unsigned int bits = receipt_number;
    unsigned int crc16 = 0;
    for (i=0; i < 32; i++) {
	bit16 = crc16 & 1;
	crc16 = (crc16 >> 1) + ((bits & 1) << 15);
	if (bit16 == 1) {
	    crc16 ^= crc_gen; 
	}
	bits >>= 1;
    }
    return crc16;
}

/* Convert a wide or multibyte string to multibyte string */
int
convert_widechar(LPSTR mbstr, LPCTSTR str, int len)
{
#ifdef UNICODE
    return WideCharToMultiByte(nCodePageLanguage, 0,
	str, lstrlen(str)+1, mbstr, len, NULL, NULL);
#else
    strncpy(mbstr, str, len);
    return strlen(str);
#endif
}


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
	    gs_addmess("  ");
	    gs_addmess(buf);
	    gs_addmess("\n");
	}
	sprintf(buf, "Error=%d\n", rc);
	gs_addmess(buf);
	convert_widechar(buf, (LPTSTR)lpMessageBuffer, sizeof(buf)-1);
	gs_addmess(buf);
	gs_addmess("\r\n");
	LocalFree(LocalHandle(lpMessageBuffer));
    }
    if (rc == 5)
	gs_addmess("You need administrator privileges to save registration details.\n");
    gs_showmess();
}

BOOL
write_registration(unsigned int reg_receipt, unsigned int reg_number,
  TCHAR *reg_name)
{
    LONG rc;
    HKEY hkey;
    DWORD dwValue;
    HKEY root;
    TCHAR *name;
    TCHAR *value;
   
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
	registry_error(root, name, value, FALSE, rc);
	return FALSE;
    }
    return TRUE;
}


int main(int argc, char *argv[])
{
    int reg_receipt;
    int reg_number;
    
    if (argc != 4) {
	fprintf(stderr, "Usage: gsvwrg number1 number2 \042name\042\n");
	return 1;
    }
    reg_receipt = atoi(argv[1]);
    reg_number = atoi(argv[2]);
    if ((reg_receipt != 0) && (reg_number == make_reg(reg_receipt))) {
	if (write_registration(reg_receipt, reg_number, argv[3])) {
	    return 0;
	}
	else
	    return 1;
    }
    Sleep(5000);
    return 1;
}
