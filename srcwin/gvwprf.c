/* Copyright (C) 1993-1998, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwprf.c */
/* Profile (INI file) routines for Windows */

#include "gvwin.h"

PROFILE *
profile_open(const char *filename)
{
PROFILE *prf;
	if ( (prf = (PROFILE *)malloc(sizeof(PROFILE))) == (PROFILE *)NULL )
	    return (PROFILE *)NULL;
	if ( (prf->name = (char *)malloc(strlen(filename)+1)) == (char *)NULL ) {
	    free(prf);
	    return (PROFILE *)NULL;
	}
	strcpy(prf->name, filename);
	return prf;
}

int 
profile_read_string(PROFILE *prf, const char *section, const char *entry, const char *def, char *buffer, int len)
{
	return GetPrivateProfileStringA(section, entry, def, buffer, len, prf->name);
}

BOOL
profile_write_string(PROFILE *prf, const char *section, const char *entry, const char *value)
{
	if (debug) {
	    BOOL flag = WritePrivateProfileStringA(section, entry, 
		value, prf->name);
	    if (!flag) {
		char buf[256];
#ifdef __WIN32__
		sprintf(buf, "WritePrivateProfileString failed, code %ld\n",
			GetLastError());
#else
		sprintf(buf, "WritePrivateProfileString failed\n");
#endif
		gs_addmess(buf);
		sprintf(buf, "  while writing [%s] %s=%s to %s\n",
		    section, entry, value, prf->name);
		gs_addmess(buf);
	    }
	    return flag;
	}
	return WritePrivateProfileStringA(section, entry, value, prf->name);
}

BOOL
profile_close(PROFILE *prf)
{
	if (prf == (PROFILE *)NULL)
		return FALSE;
	if (prf->name)
	    free(prf->name);
	free(prf);
	return TRUE;
}

