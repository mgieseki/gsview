/* Copyright (C) 2001-2003, Ghostgum Software Pty Ltd.  All rights reserved.
 
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
/* gvccp.h */

typedef struct codepage_s {
    const char *name;	    /* Name of code page */
    const char *description;/* Description of code page */
    unsigned short *table;  /* Codepage translation table */
    int ms_cp;		    /* Microsoft code page number */
    int ucs2;	    	    /* non-zero if output is Unicode UCS-2 */
    int utf8;	    	    /* non-zero if output is Unicode UTF-8 */
} codepage_t;

extern codepage_t cptable[];

/* Convert UCS-2 to UTF-8 */
int ucs2_to_utf8(unsigned char *dest, int destlen,
    const unsigned short *src, int srclen);
/* Convert UTF-8 to UCS-2 */
int utf8_to_ucs2(unsigned short *dest, int destlen,
    const unsigned char *src, int srclen);
/* Convert a UCS-2 character to the given codepage */
unsigned char tocp(const unsigned short *cp, unsigned short uch);
/* Convert UTF-8 to the given codepage */
int utf8_to_cp(const unsigned short *cp, unsigned char *dest, int destlen,
    const unsigned char *src, int srclen);
/* Find a code page */
const unsigned short *find_cp(const char *name);

