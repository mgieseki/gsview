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
/* cview.h */

typedef struct tagVIEW {
    IMAGE *img;
    GSDLL *gsdll;
    GSDLL_INPUT input;
/*
    PENDING pending;
    DOC *doc;
*/
} VIEW;


void view_init(VIEW *view);
IMAGE * view_get_image(VIEW *view);
void view_wait_message(VIEW *view);
void view_wait_event(VIEW *view);
int view_poll(VIEW *view);
void view_closefile(VIEW *view);
int view_reopenfile(VIEW *view);
int view_post_message(VIEW *view, int message, int param);
int view_page_callback(VIEW *view);
