/* Copyright (C) 1998-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvcedit.h */
/* Convert PostScript to editable form using pstoedit by Wolfgang Glunz */

#ifdef __cplusplus
extern "C" {
#endif
#include "pstoedll.h"
#ifdef __cplusplus
}
#endif

#define PSTOEDIT_SECTION "PStoEdit"	/* INI file */

extern setPstoeditOutputFunction_func * setPstoeditOutputFunction;
extern getPstoeditDriverInfo_plainC_func * getPstoeditDriverInfo_plainC;
extern pstoedit_plainC_func * pstoedit_plainC;
extern pstoedit_checkversion_func * PstoeditCheckVersionFunction;
extern clearPstoeditDriverInfo_plainC_func * clearPstoeditDriverInfo;
extern struct DriverDescription_S * pstoedit_driver_info;

typedef struct tagPSTOEDIT {
    struct DriverDescription_S *driver_info;
    struct DriverDescription_S *format;
    char default_font[MAXSTR];
    BOOL draw_text_as_polygon;
    BOOL map_to_latin1;
    float flatness;
    char driver_option[MAXSTR];
    char output_filename[MAXSTR];	/* output from pstoedit */
    char temp_filename[MAXSTR];		/* input to pstoedit */
} PSTOEDIT;


extern PSTOEDIT p2e;

int pstoedit_callback (void * cb_data, const char* text, unsigned long length);
void load_pstoedit_options(void);
void save_pstoedit_options(void);
BOOL extract_temp(char *filename);
void process_pstoedit(void *arg);
int unload_pstoedit(void);

