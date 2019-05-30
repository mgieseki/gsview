/* Copyright (C) 1993-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
  This file is part of GSview.
   
  This program is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  Refer to the GSview Licence (the "Licence") 
  for full details.
   
  Every copy of GSview must include a copy of the Licence, normally in a 
  plain ASCII text file named LICENCE.  The Licence grants you the right 
  to copy, modify and redistribute GSview, but only under certain conditions 
  described in the Licence.  Among other things, the Licence requires that 
  the copyright notice and this notice be preserved on all copies.
*/

/* gvcprf.h */
/* Profile (INI) includes */

struct prfentry {
	char *name;
	char *value;
	struct prfentry *next;
};

struct prfsection {
	char *name;
	struct prfentry *entry;
	struct prfsection *next;
};

struct prop_item_s {
	char	name[MAXSTR];
	char	value[MAXSTR];
};

typedef struct tagPROFILE {
	char *name;
	FILE *file;
	BOOL changed;
	struct prfsection *section;
} PROFILE;


PROFILE * profile_open(const char *filename);
int profile_read_string(PROFILE *prf, const char *section, const char *entry, const char *def, char *buffer, int len);
BOOL profile_write_string(PROFILE *prf, const char *section, const char *entry, const char *value);
BOOL profile_close(PROFILE *prf);
