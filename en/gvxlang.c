/* Copyright (C) 2000, Ghostgum Software Pty Ltd.  All rights reserved.
  
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
/* en/gvxlang.c */

#include "gvx.h"
#include "gvxres.h"
#include "gvxlang.h"
#include "gvclang.h"

/* string table ids must be in order since we use a binary search */
STRING_ENTRY string_en[] = {
#include "gvclang.rc"
#include "gvxlangh.rc"
};

int string_en_len = sizeof(string_en)/sizeof(STRING_ENTRY);

/* This is the GtkItemFactoryEntry structure used to generate new menus.
   Item 1: The menu path. The letter after the underscore indicates an
           accelerator key once the menu is open.
   Item 2: The accelerator key for the entry
   Item 3: The callback function.
   Item 4: The callback action.  This changes the parameters with
           which the function is called.  The default is 0.
   Item 5: The item type, used to define what kind of an item it is.
           Here are the possible values:

           NULL               -> "<Item>"
           ""                 -> "<Item>"
           "<Title>"          -> create a title item
           "<Item>"           -> create a simple item
           "<CheckItem>"      -> create a check item
           "<ToggleItem>"     -> create a toggle item
           "<RadioItem>"      -> create a radio item
           <path>             -> path of a radio item to link against
           "<Separator>"      -> create a separator
           "<Branch>"         -> create an item to hold sub items (optional)
           "<LastBranch>"     -> create a right justified branch 
*/

MENU_ENTRY menu_en[] = {
  { "/_File", 			NULL, 	NULL, 	IDM_FILEMENU, "<Branch>" },
  { "/File/_Open...",     	"O", 	GS_CMD,	IDM_OPEN, NULL },
  { "/File/_Select File...",    "S", 	GS_CMD,	IDM_SELECT, NULL },
  { "/File/Save _As...",     	"A", 	GS_CMD,	IDM_SAVEAS, NULL },
  { "/File/_Close",     	"C", 	GS_CMD,	IDM_CLOSE, NULL },
  { "/File/_Info...",     	"I", 	GS_CMD,	IDM_INFO, NULL },
  { "/File/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/File/_Convert...",     	NULL, 	GS_CMD,	IDM_CONVERTFILE, NULL },
  { "/File/_Extract...",     	"E", 	GS_CMD,	IDM_EXTRACT, NULL },
  { "/File/PS to EPS",     	NULL, 	GS_CMD,	IDM_PSTOEPS, NULL },
  { "/File/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/File/_Print...",     	"P", 	GS_CMD,	IDM_PRINT, NULL },
  { "/File/Print File...",     	NULL, 	GS_CMD,	IDM_SPOOL, NULL },
  { "/File/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/File/Show Messages",     	"M", 	GS_CMD,	IDM_GSMESS, NULL },
  { "/File/sep4",     		NULL,   NULL, 		0, "<Separator>" },
  { "/File/1LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE1, NULL },
  { "/File/2LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE2, NULL },
  { "/File/3LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE3, NULL },
  { "/File/4LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE4, NULL },
  { "/File/sep5",     		NULL,   NULL, 		0, "<Separator>" },
  { "/File/E_xit",     		"<control>Q", 	GS_CMD,	IDM_EXIT, NULL },

  { "/_Edit",      		NULL,   NULL, 	IDM_EDITMENU, "<Branch>" },
/*
  { "/Edit/_Copy",     		"<control>C", GS_CMD, IDM_COPYCLIP, NULL },
  { "/Edit/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Edit/Convert _Bitmap",    NULL,	GS_CMD,	IDM_CONVERT, NULL },
*/
  { "/Edit/Save Image as BMP",     	NULL,	GS_CMD,	IDM_PASTETO, NULL },
  { "/Edit/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Edit/_Add EPS Preview",   NULL,   NULL, 		0, "<Branch>" },
  { "/Edit/Add EPS Preview/_Interchange", NULL,		GS_CMD, IDM_MAKEEPSI, NULL },
  { "/Edit/Add EPS Preview/TIFF 4", NULL,		GS_CMD, IDM_MAKEEPST4, NULL },
  { "/Edit/Add EPS Preview/_TIFF 6 uncompressed", NULL,	GS_CMD, IDM_MAKEEPST6U, NULL },
  { "/Edit/Add EPS Preview/TIFF 6 _packbits", NULL,	GS_CMD, IDM_MAKEEPST6P, NULL },
  { "/Edit/Add EPS Preview/_Windows Metafile", NULL,	GS_CMD, IDM_MAKEEPSW, NULL },
  { "/Edit/Add EPS Preview/_User Supplied Preview",NULL,GS_CMD, IDM_MAKEEPSU, NULL },
  { "/Edit/_Extract EPS",   NULL,   NULL, 	IDM_EXTEPSMENU, "<Branch>" },
  { "/Edit/Extract EPS/_PostScript", NULL,		GS_CMD, IDM_EXTRACTPS, NULL },
  { "/Edit/Extract EPS/Pre_view", NULL,			GS_CMD, IDM_EXTRACTPRE, NULL },
  { "/Edit/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Edit/Measure", NULL,				GS_CMD, IDM_MEASURE, NULL },
  { "/Edit/Convert to vector format...", NULL,		GS_CMD, IDM_PSTOEDIT, NULL },
  { "/Edit/_Text Extract...", NULL,			GS_CMD, IDM_TEXTEXTRACT, NULL },
  { "/Edit/_Find...", "<control>F",			GS_CMD, IDM_TEXTFIND, NULL },
/*
  { "/Edit/Find _Next", "<control>G",			GS_CMD, IDM_TEXTFINDNEXT, NULL },
*/
  { "/Edit/Find _Next", "F3",			GS_CMD, IDM_TEXTFINDNEXT, NULL },

  { "/O_ptions",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Options/Easy Configure...", 	NULL,		GS_CMD, IDM_CFG, NULL },
  { "/Options/Advanced Configure...", 	NULL,		GS_CMD, IDM_GSCOMMAND, NULL },
  { "/Options/Sounds...", 		NULL,		GS_CMD, IDM_SOUNDS, NULL },
  { "/Options/_Units",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Options/Units/_pt", 		NULL,		GS_CMD, IDM_UNITPT, "<CheckItem>" },
  { "/Options/Units/_mm", 		NULL,		GS_CMD, IDM_UNITMM, "<CheckItem>" },
  { "/Options/Units/_inch", 		NULL,		GS_CMD, IDM_UNITINCH, "<CheckItem>" },
  { "/Options/Units/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Options/Units/_Fine Resolution", 	NULL,		GS_CMD, IDM_UNITFINE, "<CheckItem>" },
  { "/Options/_Language",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Options/Language/_English", 	NULL,		GS_CMD, IDM_LANGEN, "<CheckItem>" },
  { "/Options/Language/Catalan", 	NULL,		GS_CMD, IDM_LANGCT, "<CheckItem>" },
  { "/Options/Language/_Deutsch", 	NULL,		GS_CMD, IDM_LANGDE, "<CheckItem>" },
  { "/Options/Language/_Greek", 	NULL,		GS_CMD, IDM_LANGGR, "<CheckItem>" },
  { "/Options/Language/E_spañol", 	NULL,		GS_CMD, IDM_LANGES, "<CheckItem>" },
  { "/Options/Language/_Français", 	NULL,		GS_CMD, IDM_LANGFR, "<CheckItem>" },
  { "/Options/Language/_Italiano", 	NULL,		GS_CMD, IDM_LANGIT, "<CheckItem>" },
  { "/Options/Language/_Nederlands", 	NULL,		GS_CMD, IDM_LANGNL, "<CheckItem>" },
  { "/Options/Language/Russian", 	NULL,		GS_CMD, IDM_LANGRU, "<CheckItem>" },
  { "/Options/Language/Slovak", 	NULL,		GS_CMD, IDM_LANGSK, "<CheckItem>" },
  { "/Options/Language/Svenska", 	NULL,		GS_CMD, IDM_LANGSE, "<CheckItem>" },
  { "/Options/PStoText",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Options/PStoText/Disabled", 	NULL,		GS_CMD, IDM_PSTOTEXTDIS, "<CheckItem>" },
  { "/Options/PStoText/Normal", 	NULL,		GS_CMD, IDM_PSTOTEXTNORM, "<CheckItem>" },
  { "/Options/PStoText/Dvips Cork Encoding", 	NULL,	GS_CMD, IDM_PSTOTEXTCORK, "<CheckItem>" },
  { "/Options/DSC Warnings",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Options/DSC Warnings/Off", 	NULL,		GS_CMD, IDM_DSC_OFF, "<CheckItem>" },
  { "/Options/DSC Warnings/Errors", 	NULL,		GS_CMD, IDM_DSC_ERROR, "<CheckItem>" },
  { "/Options/DSC Warnings/Warnings", 	NULL,		GS_CMD, IDM_DSC_WARN, "<CheckItem>" },
  { "/Options/DSC Warnings/All", 	NULL,		GS_CMD, IDM_DSC_INFO, "<CheckItem>" },
  { "/Options/Save Settings _Now", 	NULL,		GS_CMD, IDM_SETTINGS, NULL },
  { "/Options/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Options/_Save Settings on Exit", 	NULL,		GS_CMD, IDM_SAVESETTINGS, "<CheckItem>" },
  { "/Options/Sa_fer", 			NULL,		GS_CMD, IDM_SAFER, "<CheckItem>" },
  { "/Options/Save Last _Directory", 	NULL,		GS_CMD, IDM_SAVEDIR, "<CheckItem>" },
  { "/Options/_Button Bar", 		NULL,		GS_CMD, IDM_BUTTONSHOW, "<CheckItem>" },
/*  { "/Options/_Fit Window to Page", 	NULL,		GS_CMD, IDM_FITPAGE, "<CheckItem>" }, */
/*  { "/Options/Quick Open", 		NULL,		GS_CMD, IDM_QUICK_OPEN, "<CheckItem>" }, */
  { "/Options/Auto _Redisplay", 	NULL,		GS_CMD, IDM_AUTOREDISPLAY, "<CheckItem>" },
  { "/Options/EPS _Clip", 		NULL,		GS_CMD, IDM_EPSFCLIP, "<CheckItem>" },
  { "/Options/EPS _Warn", 		NULL,		GS_CMD, IDM_EPSFWARN, "<CheckItem>" },
  { "/Options/_Ignore DSC", 		NULL,		GS_CMD, IDM_IGNOREDSC, "<CheckItem>" },
  { "/Options/Show Bounding Bo_x", 	NULL,		GS_CMD, IDM_SHOWBBOX, "<CheckItem>" },

  { "/_View",      		NULL,   NULL, 		0, "<Branch>" },
  { "/View/_Next Page", 	"plus",	GS_CMD, 	IDM_NEXT, NULL },
  { "/View/Pre_vious Page", 	"minus",	GS_CMD, 	IDM_PREV, NULL },
  { "/View/_Goto Page", 	"G",	GS_CMD, 	IDM_GOTO, NULL },
  { "/View/Go _Back", 		"B",	GS_CMD, 	IDM_GOBACK, NULL },
  { "/View/Go _Forward",	NULL,	GS_CMD, 	IDM_GOFWD, NULL },
  { "/View/_Redisplay", 	"R",	GS_CMD, 	IDM_REDISPLAY, NULL },
  { "/View/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/View/Next Page and Home", "space",	GS_CMD, 	IDM_NEXTHOME, NULL },
  { "/View/Previous Page and Home", "<control>H",GS_CMD, 	IDM_PREVHOME, NULL },
  { "/View/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/View/Full _Screen", 	"F4",	GS_CMD, 	IDM_FULLSCREEN, NULL },
  { "/View/Fit _Window", 	"F6",	GS_CMD, 	IDM_FITWIN, NULL },

  { "/_Orientation",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Orientation/_Auto", 		NULL,	GS_CMD, 	IDM_AUTOORIENT, "<CheckItem>" },
  { "/Orientation/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orientation/_Portrait", 		NULL,	GS_CMD, 	IDM_PORTRAIT, "<CheckItem>" },
  { "/Orientation/_Landscape", 		NULL,	GS_CMD, 	IDM_LANDSCAPE, "<CheckItem>" },
  { "/Orientation/_Upside-down", 	NULL,	GS_CMD, 	IDM_UPSIDEDOWN, "<CheckItem>" },
  { "/Orientation/_Seascape", 		NULL,	GS_CMD, 	IDM_SEASCAPE, "<CheckItem>" },
  { "/Orientation/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orientation/S_wap Landscape", 	NULL,	GS_CMD, 	IDM_SWAPLANDSCAPE, "<CheckItem>" },

  { "/_Media", 			NULL,   NULL, 		0, "<Branch>" },
  { "/Media/_Display Settings...", NULL,GS_CMD, 	IDM_DISPLAYSETTINGS, NULL },
  { "/Media/sep1", 		NULL,   NULL, 		0, "<Separator>" },
  { "/Media/_Rotate Media", 	NULL,	GS_CMD, 	IDM_MEDIAROTATE, "<CheckItem>"},
  { "/Media/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Media/11x17", 		NULL,	GS_CMD, 	IDM_11x17, "<CheckItem>" },
  { "/Media/A3", 		NULL,	GS_CMD, 	IDM_A3, "<CheckItem>" },
  { "/Media/A4", 		NULL,	GS_CMD, 	IDM_A4, "<CheckItem>" },
  { "/Media/A5", 		NULL,	GS_CMD, 	IDM_A5, "<CheckItem>" },
  { "/Media/B4", 		NULL,	GS_CMD, 	IDM_B4, "<CheckItem>" },
  { "/Media/B5", 		NULL,	GS_CMD, 	IDM_B5, "<CheckItem>" },
  { "/Media/Ledger", 		NULL,	GS_CMD, 	IDM_LEDGER, "<CheckItem>" },
  { "/Media/Legal", 		NULL,	GS_CMD, 	IDM_LEGAL, "<CheckItem>" },
  { "/Media/Letter", 		NULL,	GS_CMD, 	IDM_LETTER, "<CheckItem>" },
  { "/Media/Note", 		NULL,	GS_CMD, 	IDM_NOTE, "<CheckItem>" },
  { "/Media/User Defined...", 	NULL,	GS_CMD, 	IDM_USERSIZE, "<CheckItem>"},
  { "/Media/USERSIZE1", 	NULL,	GS_CMD, 	IDM_USERSIZE1, "<CheckItem>"},
  { "/Media/USERSIZE2", 	NULL,	GS_CMD, 	IDM_USERSIZE2, "<CheckItem>"},
  { "/Media/USERSIZE3", 	NULL,	GS_CMD, 	IDM_USERSIZE3, "<CheckItem>"},
  { "/Media/USERSIZE4", 	NULL,	GS_CMD, 	IDM_USERSIZE4, "<CheckItem>"},
  { "/Media/USERSIZE5", 	NULL,	GS_CMD, 	IDM_USERSIZE5, "<CheckItem>"},
  { "/Media/USERSIZE6", 	NULL,	GS_CMD, 	IDM_USERSIZE6, "<CheckItem>"},
  { "/Media/USERSIZE7", 	NULL,	GS_CMD, 	IDM_USERSIZE7, "<CheckItem>"},
  { "/Media/USERSIZE8", 	NULL,	GS_CMD, 	IDM_USERSIZE8, "<CheckItem>"},
  { "/Media/USERSIZE9", 	NULL,	GS_CMD, 	IDM_USERSIZE9, "<CheckItem>"},
  { "/Media/USERSIZE10", 	NULL,	GS_CMD, 	IDM_USERSIZE10, "<CheckItem>"},
  { "/Media/USERSIZE11", 	NULL,	GS_CMD, 	IDM_USERSIZE11, "<CheckItem>"},
  { "/Media/USERSIZE12", 	NULL,	GS_CMD, 	IDM_USERSIZE12, "<CheckItem>"},
  { "/Media/USERSIZE13", 	NULL,	GS_CMD, 	IDM_USERSIZE13, "<CheckItem>"},

  { "/_Help",      		"F1",   NULL, 		0, "<Branch>" },
  { "/Help/_Contents", 		NULL,	GS_CMD, 	IDM_HELPCONTENT, NULL },
  { "/Help/_Search for Help on", NULL,	GS_CMD, 	IDM_HELPSEARCH, NULL },
  { "/Help/_Keys Help", 	NULL,	GS_CMD, 	IDM_HELPKEYS, NULL },
  { "/Help/sep1",		NULL,   NULL, 		0, "<Separator>" },
  { "/Help/_Register...", 	NULL,	GS_CMD, 	IDM_REGISTER, NULL },
  { "/Help/_About...", 		NULL,	GS_CMD, 	IDM_ABOUT, NULL },
};

int menu_en_len = sizeof (menu_en) / sizeof (menu_en[0]);

