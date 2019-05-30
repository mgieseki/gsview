/* Copyright (C) 2000-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvcreg.cpp */

/* This is the source module which controls registration of GSview
 *
 * GSview is made available with the Aladdin Free Public Licence.
 * This allows free use, but restricts commercial distribution.
 * There is no requirement for you to register GSview.
 *
 * To disable the registration nag screen you have two options:
 * 1. Register at http://www.ghostgum.com.au/ for a cost of AUD$40.
 * 2. Disable the registration nag screen by uncommenting
 *    the "#define NONAG" line below.
 *
 * We would prefer it if you do not distribute modified versions 
 * with registration disabled.
 */


/* #define NONAG */

#include "gvc.h"

TCHAR registration_name[MAXSTR];
unsigned int registration_receipt;

unsigned int noreg[] = {10011, 10144, 10206, 10654,
    20587, 20899, 22291, 
    23762, 23874, 23970, 
    24063, 24068, 24160, 24352, 24353, 
    24447, 24493, 24522, 24979, 24980, 
    25061, 25182, 25361, 25417, 25422, 25464, 25466,
    25499, 25756, 25830, 25831, 25832, 25894, 
    26011, 26013, 26014, 26015, 26113, 26318, 26370, 26465, 
    26599, 26600, 
    26677, 26679, 26680, 26681};

BOOL registration_check(void)
{
    unsigned int registration_number;
    int i;
    int n = sizeof(noreg) / sizeof(int);
    if (read_registration(&registration_receipt, &registration_number,
	registration_name, sizeof(registration_name)-1)) {
	if ((registration_receipt !=0) &&
	    (registration_number == make_reg(registration_receipt))) {
	    for (i=0; i < n; i++) {
		if (registration_receipt == noreg[i])
		    break;
	    }
	    if (i >= n)
	        return TRUE;
	}
    }

    /* If GSview is unregistered, this won't be updated when the */
    /* language changes. */
    load_string(IDS_UNREGISTERED, registration_name, sizeof(registration_name));
    registration_receipt = 0;
#ifdef NONAG
    return TRUE;
#else
    return registration_nag();
#endif
}


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

