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

/* gvpdisp.c */
/* Display GSview routines for PM */
#include "gvpm.h"


/* execute program */
BOOL
exec_pgm(char *name, char *arg, PROG* prog)
{
	STARTDATA sdata;
	APIRET rc;
	char buf[MAXSTR];
	CHAR progname[MAXSTR];
	PROG pg;
	PTIB pptib;
	PPIB pppib;

	memset(&pg, 0, sizeof(PROG));

	if (DosGetInfoBlocks(&pptib, &pppib)) {
		error_message("\nexec_pgm: Couldn't get environment\n");
		return FALSE;
	}
	/* Look for program in same directory as this EXE */
	progname[0] = '\0';
	if (!strchr(name, '\\'))
	    strcpy(progname, szExePath);
	strcat(progname, name);

	memset(&sdata, 0, sizeof(sdata));
	/* because new program is a different EXE type, 
	 * we must use start session not DosExecPgm() */
	sdata.Length = sizeof(sdata);
	sdata.Related = SSF_RELATED_INDEPENDENT;
	sdata.FgBg = SSF_FGBG_BACK;		/* start in background */
	sdata.TraceOpt = 0;
	sdata.PgmTitle = (PSZ)name;
	sdata.PgmName = (PSZ)progname;
	sdata.PgmInputs = (PSZ)arg;
	sdata.TermQ = 0;
	sdata.Environment = (PBYTE)pppib->pib_pchenv;	/* use Parent's environment */
	sdata.InheritOpt =  0;
	sdata.InheritOpt = SSF_INHERTOPT_PARENT;
	sdata.SessionType = SSF_TYPE_DEFAULT;		/* default is text */
	sdata.IconFile = NULL;
	sdata.PgmHandle = 0;
	sdata.PgmControl = 0; /* normal, not SSF_CONTROL_MINIMIZE */
	sdata.InitXPos = 0;
	sdata.InitYPos = 0;
	sdata.InitXSize = 0;
	sdata.InitYSize = 0;
	sdata.ObjectBuffer = NULL;
	sdata.ObjectBuffLen = 0;

/*
sprintf(buf,"exec_pgm: %s %s\n",sdata.PgmName, sdata.PgmInputs);
message_box(buf, 0);
*/
	rc = DosStartSession(&sdata, &pg.session_id, &pg.process_id);
	if (rc == ERROR_FILE_NOT_FOUND) {
	    /* didn't find it in same directory as this EXE so try PATH */
	    sdata.PgmName = (PSZ)name;
	    rc = DosStartSession(&sdata, &pg.session_id, &pg.process_id);
	}
	if (rc) {
	    sprintf(buf,"\"%s %s\", rc = %ld\n", sdata.PgmName, sdata.PgmInputs, rc);
	    gserror(IDS_CANNOTRUN, buf, MB_ICONHAND, SOUND_ERROR);
	    nHelpTopic = IDS_TOPICINSTALL;
	    get_help();
	    return FALSE;
	}
	pg.valid = TRUE;
	*prog = pg;	/* give details back to caller */
	return TRUE;
}


/* stop specified program */
void
stop_pgm(PROG* prog)
{
QMSG q_mess;		/* queue message */
int i = 0;
	if (!prog->valid) {
	    cleanup_pgm(prog);
	    return;
	}
	DosStopSession(STOP_SESSION_SPECIFIED, prog->session_id);
	while (prog->valid && (i < 100)) {
	    /* wait for termination queue message to cause cleanup_pgm() to be called */
  	    if (WinGetMsg(hab, &q_mess, 0L, 0, 0))
	        WinDispatchMsg(hab, &q_mess);
	    i++;
	}
	if (i >= 100)
	    gserror(0, "can't stop program", MB_ICONHAND, SOUND_ERROR);
/* cleanup should already have occurred */
	cleanup_pgm(prog);
}


/* cleanup after program has stopped */
void
cleanup_pgm(PROG* prog)
{
	prog->valid = FALSE;
	prog->session_id = 0;
	prog->process_id = (PID)0;
}

