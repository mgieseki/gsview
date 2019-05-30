/* Copyright (C) 1996-1998, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvpgs.h */

#ifndef RC_INVOKED

/* sent to our window */
#define WM_TEXTUPDATE	WM_USER+1
#define WM_PCUPDATE	WM_USER+2

#define MAXSTR 256

/* main structure with info about the GS DLL */
typedef struct tagGSDLL {
	HMODULE	hmodule;	/* handle to module */
	int		state;
	long		revision_number;
	gs_main_instance *minst;

	/* pointers to DLL functions */
	PFN_gsapi_revision revision;
	PFN_gsapi_new_instance new_instance;
	PFN_gsapi_delete_instance delete_instance;
	PFN_gsapi_set_stdio set_stdio;
	PFN_gsapi_set_poll set_poll;
	PFN_gsapi_set_display_callback set_display_callback;
	PFN_gsapi_init_with_args init_with_args;
	PFN_gsapi_run_string run_string;
	PFN_gsapi_run_string_begin run_string_begin;
	PFN_gsapi_run_string_continue run_string_continue;
	PFN_gsapi_run_string_end run_string_end;
	PFN_gsapi_exit exit;
} GSDLL;


#endif
