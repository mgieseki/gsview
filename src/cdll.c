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
/* cdll.c */

/* Load and unload Ghostscript DLL */
#include "gvc.h"


/******************************************************************/

static void
gsdll_reset(GSDLL *dll)
{
    dll->open = FALSE;
    dll->hmodule = (GGMODULE)NULL;
    dll->state = GS_UNINIT;
    dll->revision_number = 0;
    dll->minst = NULL;

    dll->revision = NULL;
    dll->new_instance = NULL;
    dll->delete_instance = NULL;
    dll->set_stdio = NULL;
    dll->set_display_callback = NULL;
    dll->set_poll = NULL;
    dll->run_string_begin = NULL;
    dll->run_string_continue = NULL;
    dll->run_string_end = NULL;
    dll->exit = NULL;
}


/* Free Ghostscript DLL */
/* This should only be called when gsapi_run_* and gsapi_exit have returned */
int
gsdll_free(GSDLL *dll)
{
    if (dll->state != GS_UNINIT)
	return_error(-1);

    if (dll->hmodule == (GGMODULE)NULL)
	return 0;	/* already unloaded */

    if (dll->minst && dll->delete_instance)
	dll->delete_instance(dll->minst);
    gsdll_close(dll);
    gsdll_reset(dll);

    return 0;
}

/* Cleanup from an error while loading the Ghostscript DLL */
static void
gsdll_cleanup(GSDLL *dll)
{
    gsdll_free(dll);
    gsdll_reset(dll);
}

/* Load GS DLL if not already loaded */
int
gsdll_load(GSDLL *dll, const char *name)
{
gsapi_revision_t rv;

    if (dll->hmodule != (GGMODULE)NULL)
	return 0;

    gsdll_reset(dll);

    if (gsdll_open(dll, name)) {
	gs_addmessf("Can't load Ghostscript DLL\n");
        gsdll_cleanup(dll);
	return_error(-1);
    }

    /* Get pointers to functions */
    dll->revision = (PFN_gsapi_revision) gsdll_sym(dll, "gsapi_revision");
    if (dll->revision == NULL) {
	gs_addmessf( 
	    "\nCan't find gsapi_revision.  Need Ghostscript %d or later\n",
	     GS_REVISION_MIN);
	gsdll_cleanup(dll);
	return_error(-1);
    }

    /* check DLL version */
    if (dll->revision(&rv, sizeof(rv)) != 0) {
	gs_addmessf("\nUnable to identify Ghostscript DLL revision - it must be newer than needed.\n");
	gsdll_cleanup(dll);
	return_error(-1);
    }
    dll->revision_number = rv.revision;

    if ( (dll->revision_number < GS_REVISION_MIN) || 
	(dll->revision_number > GS_REVISION_MAX) ) {
	gs_addmessf("\nWrong version of DLL found.\n\
  Found version %ld\n  Need version  %ld - %ld\n", 
	    dll->revision_number, 
	    (long)GS_REVISION_MIN, (long)GS_REVISION_MAX);
	if (dll->revision_number > 70000)
	  gs_addmessf("\nThis is a modified version of ghostscript that is broken.  It should be replaced by the official ghostscript release.\n");
	return_error(-1);
    }

    /* continue loading other functions */
    dll->new_instance = (PFN_gsapi_new_instance) 
	gsdll_sym(dll, "gsapi_new_instance");
    if (dll->new_instance == NULL) {
	gs_addmessf("Can't find gsapi_new_instance\n");
	gsdll_cleanup(dll);
	return_error(-1);
    }

    dll->delete_instance = (PFN_gsapi_delete_instance) 
	gsdll_sym(dll, "gsapi_delete_instance");
    if (dll->delete_instance == NULL) {
	gs_addmessf("Can't find gsapi_delete_instance\n");
	gsdll_cleanup(dll);
	return_error(-1);
    }

    dll->set_stdio = (PFN_gsapi_set_stdio) gsdll_sym(dll, "gsapi_set_stdio");
    if (dll->set_stdio == NULL) {
	gs_addmessf("Can't find gsapi_set_stdio\n");
	gsdll_cleanup(dll);
	return_error(-1);
    }

    dll->set_poll = (PFN_gsapi_set_poll) gsdll_sym(dll, "gsapi_set_poll");
    if (dll->set_poll == NULL) {
	gs_addmessf("Can't find gsapi_set_poll\n");
	gsdll_cleanup(dll);
	return_error(-1);
    }

    dll->set_display_callback = (PFN_gsapi_set_display_callback) 
	gsdll_sym(dll, "gsapi_set_display_callback");
    if (dll->set_display_callback == NULL) {
	gs_addmessf("Can't find gsapi_set_display_callback\n");
	gsdll_cleanup(dll);
	return_error(-1);
    }

    dll->init_with_args = (PFN_gsapi_init_with_args) 
	gsdll_sym(dll, "gsapi_init_with_args");
    if (dll->init_with_args == NULL) {
	gs_addmessf("Can't find gsapi_init_with_args\n");
	gsdll_cleanup(dll);
	return_error(-1);
    }

    dll->run_string_begin = (PFN_gsapi_run_string_begin) gsdll_sym(dll, 
	"gsapi_run_string_begin");
    if (dll->run_string_begin == NULL) {
	gs_addmessf("Can't find gsapi_run_string_begin\n");
	gsdll_cleanup(dll);
	return_error(-1);
    }

    dll->run_string_continue = (PFN_gsapi_run_string_continue) gsdll_sym(dll, 
	"gsapi_run_string_continue");
    if (dll->run_string_continue == NULL) {
	gs_addmessf("Can't find gsapi_run_string_continue\n");
	gsdll_cleanup(dll);
	return_error(-1);
    }

    dll->run_string_end = (PFN_gsapi_run_string_end) gsdll_sym(dll, 
	"gsapi_run_string_end");
    if (dll->run_string_end == NULL) {
	gs_addmessf("Can't find gsapi_run_string_end\n");
	gsdll_cleanup(dll);
	return_error(-1);
    }

    dll->exit = (PFN_gsapi_exit) gsdll_sym(dll, "gsapi_exit");
    if (dll->exit == NULL) {
	gs_addmessf("Can't find gsapi_exit\n");
	gsdll_cleanup(dll);
	return_error(-1);
    }


    dll->open = TRUE;
    dll->state = GS_UNINIT;

    return 0;
}

/******************************************************************/
