/* Copyright (C) 1998-2009, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvcedit.c */
/* Convert PostScript to editable form using pstoedit by Wolfgang Glunz */
#include "gvc.h"
#include "gvcedit.h"

int 
pstoedit_callback (void * cb_data, const char* text, unsigned long length)
/* Returns number of characters successfully written  */
{
/*
	fwrite(text, 1, length, stdout);
*/
	gs_addmess_count(text, length);
	return length;
}

void
load_pstoedit_options(void)
{
PROFILE *prf;
char profile[MAXSTR];
const char *section = PSTOEDIT_SECTION;
struct DriverDescription_S * dd;
int i;
float u;

    /* should load pstoedit options from INI file */
    prf = profile_open(szIniFile);
    profile_read_string(prf, section, "Format", "", profile, sizeof(profile));
    dd = p2e.driver_info;
    p2e.format = dd;
    while(dd && (dd->symbolicname) ) {
	if (strcmp(dd->symbolicname, profile) == 0) {
	    p2e.format = dd;
	    break;
	}
	dd++;
    }

    profile_read_string(prf, section, "DefaultFont", "", 
	p2e.default_font, sizeof(p2e.default_font));
    profile_read_string(prf, section, "DriverOption", "", 
	p2e.driver_option, sizeof(p2e.driver_option));
    profile_read_string(prf, section, "OutputFile", "", 
	p2e.output_filename, sizeof(p2e.output_filename));

    profile_read_string(prf, section, "Flatness", "", 
	profile, sizeof(profile));
    if (sscanf(profile,"%f", &u) == 1)
	p2e.flatness = u;
    else
	p2e.flatness = 0.0;

    profile_read_string(prf, section, "DrawTextAsPolygon", "", 
	profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	p2e.draw_text_as_polygon = i;
    else
	p2e.draw_text_as_polygon = 0;

    profile_read_string(prf, section, "DontMapToISOLatin1", "", 
	profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	p2e.map_to_latin1 = i;
    else
	p2e.map_to_latin1 = 1;

    profile_close(prf);
}

void
save_pstoedit_options(void)
{
char profile[MAXSTR];
const char *section = PSTOEDIT_SECTION;
PROFILE *prf;
    prf = profile_open(szIniFile);
    if (p2e.format && p2e.format->symbolicname)
        profile_write_string(prf, section, "Format", p2e.format->symbolicname);
    profile_write_string(prf, section, "DefaultFont", p2e.default_font);
    profile_write_string(prf, section, "DriverOption", p2e.driver_option);
    profile_write_string(prf, section, "OutputFile", p2e.output_filename);
    sprintf(profile, "%g", p2e.flatness);
    profile_write_string(prf, section, "Flatness", profile);
    sprintf(profile, "%d", (int)p2e.draw_text_as_polygon);
    profile_write_string(prf, section, "DrawTextAsPolygon", profile);
    sprintf(profile, "%d", (int)p2e.map_to_latin1);
    /* OS/2 had the following line commented out for some reason */
    profile_write_string(prf, section, "DontMapToISOLatin1", profile);
    profile_close(prf);
}

/* extract a temporary file for pstoedit */
BOOL
extract_temp(char *filename)
{
FILE *pcfile;

    filename[0] = '\0';
    if ( (pcfile = gp_open_scratch_file(szScratch, filename, "wb")) == 
        (FILE *)NULL) {
	gserror(IDS_NOTEMP, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	play_sound(SOUND_ERROR);
	return FALSE;
    }

    if (!copy_for_printer(pcfile, TRUE)) {
	unlink(filename);
	return FALSE;
    }

    fclose(pcfile);
    return TRUE;
}


#define PSTOEDIT_ARGC_MAX 20
void
process_pstoedit(void *arg)
{
int argc = 0;
const char *argv[22];
char flatarg[64];
char format[MAXSTR];
char libpath[MAXSTR];
const char* psinterpreter = NULL;

    pending.pstoedit = FALSE;

    /* build command line */
    argv[argc++] = "pstoedit"; 
    if (debug)
        argv[argc++] = "-v"; 

    if (p2e.draw_text_as_polygon)
        argv[argc++] = "-dt"; 
    if (!p2e.map_to_latin1)
        argv[argc++] = "-nomaptoisolatin1"; 
    if (strlen(p2e.default_font) != 0) {
        argv[argc++] = "-df";
        argv[argc++] = p2e.default_font;
    }
    if (p2e.flatness > 0.0) {
        argv[argc++] = "-flat";
	sprintf(flatarg, "%g", p2e.flatness);
        argv[argc++] = flatarg;
    }

    if (portable_app) {
	psinterpreter = option.gsdll; 
	strcpy(libpath, "-I\042");
	strcat(libpath, option.gsinclude);
	strcat(libpath, "\042");
	argv[argc++] = "-psarg";
	argv[argc++] = libpath;
    }
    
    argv[argc++] = "-f";
    strcpy(format, p2e.format->symbolicname);
    if (strlen(p2e.driver_option) != 0) {
	strcat(format, ":");
	strcat(format, p2e.driver_option);
    }
    argv[argc++] = format;

    argv[argc++] = p2e.temp_filename;
    argv[argc++] = p2e.output_filename;

    if (debug) {
	int i;
	gs_addmess("pstoedit arguments:\r\n");
	for (i=0; i<argc; i++) {
	    gs_addmess("  ");
	    gs_addmess(argv[i]);
	    gs_addmess("\r\n");
	}
    }

    /* invoke pstoedit */
    if (pstoedit_plainC(argc, (const char **)argv, psinterpreter ) != 0)
	post_img_message(WM_GSSHOWMESS, 0);

    if (!debug)
        unlink(p2e.temp_filename);
    p2e.temp_filename[0] = '\0';

    return;
}


