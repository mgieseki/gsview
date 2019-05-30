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

/* gvcbeta.c */
/* Initialisation routines for PM and Windows GSview */

#include "gvc.h"

int beta_expired(void);

#ifndef BETA

int beta(void)
{
    return 0;
}

int beta_warn(void)
{
    return 0;
}

#else  /* BETA */

#include <time.h>

int
beta_expired(void)
{
  time_t today = time(NULL);
  struct tm *t;
  t = localtime(&today);
  if (t->tm_year+1900 < BETA_YEAR)
    return 0;
  if (t->tm_year+1900 > BETA_YEAR)
    return 1;    /* beta copy has expired */
  if (t->tm_mon+1 < BETA_MONTH)
    return 0;
  if (t->tm_mon+1 > BETA_MONTH)
    return 1;    /* beta copy has expired */
  if (t->tm_mday < BETA_DAY)
    return 0;
  return 1;    /* beta copy has expired */
}

int beta(void)
{
  if (beta_expired()) {
    TCHAR buf[MAXSTR];
    load_string(IDS_BETAEXPIRED, buf, sizeof(buf)/sizeof(TCHAR)-1);
    message_box(buf, MB_OK | MB_ICONHAND);
    return 1;
  }
  return 0;
}

int beta_warn(void)
{
  TCHAR buf[256];
  TCHAR mess[MAXSTR];
  load_string(IDS_BETAWARN, mess, sizeof(mess)/sizeof(TCHAR)-1);
  wsprintf(buf, mess, BETA_YEAR, BETA_MONTH, BETA_DAY);
  if (message_box(buf, MB_OKCANCEL | MB_ICONEXCLAMATION) != IDOK)
      return 1;
  return beta();
}

#endif /* BETA */
