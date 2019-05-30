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
/* cview.cpp */

#include "gvc.h"


void
view_init(VIEW *view)
{
    view->img = &image;
    view->gsdll = &gsdll;
}


IMAGE *
view_get_image(VIEW *view)
{
    if (view == NULL)
	return NULL;
    return view->img;
}

int
view_poll(VIEW *view)
{
    if (!multithread)
	peek_message();
    if (pending.abort)
	return -100;
    return 0;
}

/* wait for semaphore */
void
view_wait_event(VIEW *view)
{
    wait_event();
}

/* close file while waiting for user input */
void
view_closefile(VIEW *view)
{
    if (!psfile.ispdf) {
	dfclose();
	if (view->input.index < view->input.count)	
	    /* must reseek on reopen */
	    view->input.section[view->input.index].seek = TRUE;
    }
}

/* reopen file to send to Ghostscript */
int
view_reopenfile(VIEW *view)
{
    if (psfile.ispdf)
	return 0;

    if (dfreopen() != 0)
	return -1;

    return 0;
}

int
view_post_message(VIEW *view, int message, int param)
{
   post_img_message(message, param);
   return 0;
}


/* Process page callback */
int
view_page_callback(VIEW *view)
{
    view->gsdll->state = GS_PAGE;

    /* if document not PDF, we close file while waiting for user input */
    view_closefile(view);

    view_post_message(view, WM_GSPAGE, 0);
    view_post_message(view, WM_GSWAIT, IDS_NOWAIT);

    if (multithread) {
	/* wait for semaphore */
	if (!pending.next && !pending.now && !pending.unload)
	    view_wait_event(view);
    }
    else {
	/* single threaded version */
	view_wait_message(view);
    }

    if (view_reopenfile(view)) {
	gs_addmess("Callback: PAGE Can't reopen input file.  File has changed or is missing.\n");
	/* document changed or missing, so force a rescan */
	request_mutex();
	pending.abort = TRUE;
	pending.now = FALSE;
	pending.redisplay = TRUE;
	release_mutex();
    }

    pending.next = FALSE;
    if (psfile.dsc == (CDSC *)NULL) {
	if (pending.abort)
	    psfile.pagenum = 1;
	else
	    psfile.pagenum++;
    }

    view->gsdll->state = GS_BUSY;
    view_post_message(view, WM_GSWAIT, IDS_WAITDRAW);
    return 0;
}
