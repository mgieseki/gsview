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
/* sk/gvxlang.c */

#include "gvx.h"
#include "gvxres.h"
#include "gvxlang.h"
#include "gvclang.h"

/* string table ids must be in order since we use a binary search */
STRING_ENTRY string_sk[] = {
#include "gvclang.rc"
#include "gvxlangh.rc"
};

int string_sk_len = sizeof(string_sk)/sizeof(STRING_ENTRY);

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

MENU_ENTRY menu_sk[] = {
  { "/_S�bor", 			NULL, 	NULL, 	IDM_FILEMENU, "<Branch>" },
  { "/S�bor/_Otvori�...",     	"O", 	GS_CMD,	IDM_OPEN, NULL },
  { "/S�bor/_Vybra� s�bor...",    "S", 	GS_CMD,	IDM_SELECT, NULL },
  { "/S�bor/Ulo�i� _ako...",     	"A", 	GS_CMD,	IDM_SAVEAS, NULL },
  { "/S�bor/_Zavrie�",     	"C", 	GS_CMD,	IDM_CLOSE, NULL },
  { "/S�bor/_Inform�cie...",     	"I", 	GS_CMD,	IDM_INFO, NULL },
  { "/S�bor/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/S�bor/_Konvertova�...",     	NULL, 	GS_CMD,	IDM_CONVERTFILE, NULL },
  { "/S�bor/_Extrahova�...",     	"E", 	GS_CMD,	IDM_EXTRACT, NULL },
  { "/S�bor/PS do EPS",     	NULL, 	GS_CMD,	IDM_PSTOEPS, NULL },
  { "/S�bor/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/S�bor/_Tla�i�...",     	"P", 	GS_CMD,	IDM_PRINT, NULL },
  { "/S�bor/Tla�i� s�bor...",     	NULL, 	GS_CMD,	IDM_SPOOL, NULL },
  { "/S�bor/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/S�bor/Zobrazi� spr�vy",     	"M", 	GS_CMD,	IDM_GSMESS, NULL },
  { "/S�bor/sep4",     		NULL,   NULL, 		0, "<Separator>" },
  { "/S�bor/1LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE1, NULL },
  { "/S�bor/2LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE2, NULL },
  { "/S�bor/3LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE3, NULL },
  { "/S�bor/4LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE4, NULL },
  { "/S�bor/sep5",     		NULL,   NULL, 		0, "<Separator>" },
  { "/S�bor/_Skon�i�",     		NULL, 	GS_CMD,	IDM_EXIT, NULL },

  { "/�pr_avy",      		NULL,   NULL, 	IDM_EDITMENU, "<Branch>" },
/*
  { "/�pravy/_Kop�rova�",     		"<control>C", GS_CMD, IDM_COPYCLIP, NULL },
  { "/�pravy/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/�pravy/Konvertova� _bitov� mapu",    NULL,	GS_CMD,	IDM_CONVERT, NULL },
*/
  { "/�pravy/Ulo�i� obr�zok ako bitov� mapu",     	NULL,	GS_CMD,	IDM_PASTETO, NULL },
  { "/�pravy/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/�pravy/_Prida� EPS n�h�ad",   NULL,   NULL, 		0, "<Branch>" },
  { "/�pravy/Prida� EPS n�h�ad/_Interchange", NULL,		GS_CMD, IDM_MAKEEPSI, NULL },
  { "/�pravy/Prida� EPS n�h�ad/TIFF 4", NULL,		GS_CMD, IDM_MAKEEPST4, NULL },
  { "/�pravy/Prida� EPS n�h�ad/_TIFF 6 bez kompresie", NULL,	GS_CMD, IDM_MAKEEPST6U, NULL },
  { "/�pravy/Prida� EPS n�h�ad/TIFF 6 _balen� bity", NULL,	GS_CMD, IDM_MAKEEPST6P, NULL },
  { "/�pravy/Prida� EPS n�h�ad/_Windows Meta s�bor", NULL,	GS_CMD, IDM_MAKEEPSW, NULL },
  { "/�pravy/Prida� EPS n�h�ad/_Pou��vate�sk� n�h�ad",NULL,GS_CMD, IDM_MAKEEPSU, NULL },
  { "/�pravy/_Extrahova� EPS",   NULL,   NULL, 	IDM_EXTEPSMENU, "<Branch>" },
  { "/�pravy/Extrahova� EPS/_PostScript", NULL,		GS_CMD, IDM_EXTRACTPS, NULL },
  { "/�pravy/Extrahova� EPS/_N�h�ad", NULL,			GS_CMD, IDM_EXTRACTPRE, NULL },
  { "/�pravy/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/�pravy/Measure", NULL,				GS_CMD, IDM_MEASURE, NULL },
  { "/�pravy/Konvertova� na vektorov� form�t...", NULL,		GS_CMD, IDM_PSTOEDIT, NULL },
  { "/�pravy/_Extrahova� text...", NULL,			GS_CMD, IDM_TEXTEXTRACT, NULL },
  { "/�pravy/_H�ada�...", "<control>F",			GS_CMD, IDM_TEXTFIND, NULL },
/*
  { "/�pravy/H�ada� �_alej", "<control>G",			GS_CMD, IDM_TEXTFINDNEXT, NULL },
*/
  { "/�pravy/H�ada� �_alej", "F3",			GS_CMD, IDM_TEXTFINDNEXT, NULL },

  { "/_Mo�nosti",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Mo�nosti/Jednoduch� nastavenia...", 	NULL,		GS_CMD, IDM_CFG, NULL },
  { "/Mo�nosti/Roz��ren� nastavenia...", 	NULL,		GS_CMD, IDM_GSCOMMAND, NULL },
  { "/Mo�nosti/Zvuky...", 		NULL,		GS_CMD, IDM_SOUNDS, NULL },
  { "/Mo�nosti/_Jendotky",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Mo�nosti/Jendotky/_pt", 		NULL,		GS_CMD, IDM_UNITPT, "<CheckItem>" },
  { "/Mo�nosti/Jendotky/_mm", 		NULL,		GS_CMD, IDM_UNITMM, "<CheckItem>" },
  { "/Mo�nosti/Jendotky/_inch", 		NULL,		GS_CMD, IDM_UNITINCH, "<CheckItem>" },
  { "/Mo�nosti/Jendotky/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Mo�nosti/Jendotky/_Fine Resolution", 	NULL,		GS_CMD, IDM_UNITFINE, "<CheckItem>" },
  { "/Mo�nosti/_Jazyk",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Mo�nosti/Jazyk/_English", 	NULL,		GS_CMD, IDM_LANGEN, "<CheckItem>" },
  { "/Mo�nosti/Jazyk/Catalan", 	NULL,		GS_CMD, IDM_LANGCT, "<CheckItem>" },
  { "/Mo�nosti/Jazyk/_Deutsch", 	NULL,		GS_CMD, IDM_LANGDE, "<CheckItem>" },
  { "/Mo�nosti/Jazyk/_Greek", 	NULL,		GS_CMD, IDM_LANGGR, "<CheckItem>" },
  { "/Mo�nosti/Jazyk/E_spa�ol", 	NULL,		GS_CMD, IDM_LANGES, "<CheckItem>" },
  { "/Mo�nosti/Jazyk/_Fran�ais", 	NULL,		GS_CMD, IDM_LANGFR, "<CheckItem>" },
  { "/Mo�nosti/Jazyk/_Italiano", 	NULL,		GS_CMD, IDM_LANGIT, "<CheckItem>" },
  { "/Mo�nosti/Jazyk/_Nederlands", 	NULL,		GS_CMD, IDM_LANGNL, "<CheckItem>" },
  { "/Mo�nosti/Jazyk/Russian", 	NULL,		GS_CMD, IDM_LANGRU, "<CheckItem>" },
  { "/Mo�nosti/Jazyk/Slovak", 	NULL,		GS_CMD, IDM_LANGSK, "<CheckItem>" },
  { "/Mo�nosti/Jazyk/Svenska", 	NULL,		GS_CMD, IDM_LANGSE, "<CheckItem>" },
  { "/Mo�nosti/PStoText",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Mo�nosti/PStoText/Vypnut�", 	NULL,		GS_CMD, IDM_PSTOTEXTDIS, "<CheckItem>" },
  { "/Mo�nosti/PStoText/Norm�lny", 	NULL,		GS_CMD, IDM_PSTOTEXTNORM, "<CheckItem>" },
  { "/Mo�nosti/PStoText/Dvips Cork enk�dovanie", 	NULL,	GS_CMD, IDM_PSTOTEXTCORK, "<CheckItem>" },
  { "/Mo�nosti/DSC upozornenia",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Mo�nosti/DSC upozornenia/Vypnut�", 	NULL,		GS_CMD, IDM_DSC_OFF, "<CheckItem>" },
  { "/Mo�nosti/DSC upozornenia/Chyby", 	NULL,		GS_CMD, IDM_DSC_ERROR, "<CheckItem>" },
  { "/Mo�nosti/DSC upozornenia/Upozornenia", 	NULL,		GS_CMD, IDM_DSC_WARN, "<CheckItem>" },
  { "/Mo�nosti/DSC upozornenia/V�etky", 	NULL,		GS_CMD, IDM_DSC_INFO, "<CheckItem>" },
  { "/Mo�nosti/Ulo�i� nastavenia _teraz", 	NULL,		GS_CMD, IDM_SETTINGS, NULL },
  { "/Mo�nosti/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Mo�nosti/_Ulo�i� nastavenia pri skon�en�", 	NULL,		GS_CMD, IDM_SAVESETTINGS, "<CheckItem>" },
  { "/Mo�nosti/Sa_fer", 			NULL,		GS_CMD, IDM_SAFER, "<CheckItem>" },
  { "/Mo�nosti/Save Last _Directory", 	NULL,		GS_CMD, IDM_SAVEDIR, "<CheckItem>" },
  { "/Mo�nosti/_Button Bar", 		NULL,		GS_CMD, IDM_BUTTONSHOW, "<CheckItem>" },
/*  { "/Mo�nosti/_Fit Window to Page", 	NULL,		GS_CMD, IDM_FITPAGE, "<CheckItem>" }, */
/*  { "/Mo�nosti/Quick Open", 		NULL,		GS_CMD, IDM_QUICK_OPEN, "<CheckItem>" }, */
  { "/Mo�nosti/Auto _Redisplay", 	NULL,		GS_CMD, IDM_AUTOREDISPLAY, "<CheckItem>" },
  { "/Mo�nosti/EPS _Clip", 		NULL,		GS_CMD, IDM_EPSFCLIP, "<CheckItem>" },
  { "/Mo�nosti/EPS _Warn", 		NULL,		GS_CMD, IDM_EPSFWARN, "<CheckItem>" },
  { "/Mo�nosti/_Ignorova� DSC", 		NULL,		GS_CMD, IDM_IGNOREDSC, "<CheckItem>" },
  { "/Mo�nosti/Show Bounding Bo_x", 	NULL,		GS_CMD, IDM_SHOWBBOX, "<CheckItem>" },

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

  { "/_Orient�cia",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Orient�cia/_Automatick�", 		NULL,	GS_CMD, 	IDM_AUTOORIENT, "<CheckItem>" },
  { "/Orient�cia/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orient�cia/_Portrait", 		NULL,	GS_CMD, 	IDM_PORTRAIT, "<CheckItem>" },
  { "/Orient�cia/_Landscape", 		NULL,	GS_CMD, 	IDM_LANDSCAPE, "<CheckItem>" },
  { "/Orient�cia/_Obr�tene", 	NULL,	GS_CMD, 	IDM_UPSIDEDOWN, "<CheckItem>" },
  { "/Orient�cia/_Seascape", 		NULL,	GS_CMD, 	IDM_SEASCAPE, "<CheckItem>" },
  { "/Orient�cia/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orient�cia/S_wap Landscape", 	NULL,	GS_CMD, 	IDM_SWAPLANDSCAPE, "<CheckItem>" },

  { "/_Form�t", 			NULL,   NULL, 		0, "<Branch>" },
  { "/Form�t/_Nastavenia zobrazenia...", NULL,GS_CMD, 	IDM_DISPLAYSETTINGS, NULL },
  { "/Form�t/sep1", 		NULL,   NULL, 		0, "<Separator>" },
  { "/Form�t/_Rotova� m�dium", 	NULL,	GS_CMD, 	IDM_MEDIAROTATE, "<CheckItem>"},
  { "/Form�t/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Form�t/11x17", 		NULL,	GS_CMD, 	IDM_11x17, "<CheckItem>" },
  { "/Form�t/A3", 		NULL,	GS_CMD, 	IDM_A3, "<CheckItem>" },
  { "/Form�t/A4", 		NULL,	GS_CMD, 	IDM_A4, "<CheckItem>" },
  { "/Form�t/A5", 		NULL,	GS_CMD, 	IDM_A5, "<CheckItem>" },
  { "/Form�t/B4", 		NULL,	GS_CMD, 	IDM_B4, "<CheckItem>" },
  { "/Form�t/B5", 		NULL,	GS_CMD, 	IDM_B5, "<CheckItem>" },
  { "/Form�t/Ledger", 		NULL,	GS_CMD, 	IDM_LEDGER, "<CheckItem>" },
  { "/Form�t/Legal", 		NULL,	GS_CMD, 	IDM_LEGAL, "<CheckItem>" },
  { "/Form�t/Letter", 		NULL,	GS_CMD, 	IDM_LETTER, "<CheckItem>" },
  { "/Form�t/Note", 		NULL,	GS_CMD, 	IDM_NOTE, "<CheckItem>" },
  { "/Form�t/Pou��vate�om definovan�...", 	NULL,	GS_CMD, 	IDM_USERSIZE, "<CheckItem>"},
  { "/Form�t/USERSIZE1", 	NULL,	GS_CMD, 	IDM_USERSIZE1, "<CheckItem>"},
  { "/Form�t/USERSIZE2", 	NULL,	GS_CMD, 	IDM_USERSIZE2, "<CheckItem>"},
  { "/Form�t/USERSIZE3", 	NULL,	GS_CMD, 	IDM_USERSIZE3, "<CheckItem>"},
  { "/Form�t/USERSIZE4", 	NULL,	GS_CMD, 	IDM_USERSIZE4, "<CheckItem>"},
  { "/Form�t/USERSIZE5", 	NULL,	GS_CMD, 	IDM_USERSIZE5, "<CheckItem>"},
  { "/Form�t/USERSIZE6", 	NULL,	GS_CMD, 	IDM_USERSIZE6, "<CheckItem>"},
  { "/Form�t/USERSIZE7", 	NULL,	GS_CMD, 	IDM_USERSIZE7, "<CheckItem>"},
  { "/Form�t/USERSIZE8", 	NULL,	GS_CMD, 	IDM_USERSIZE8, "<CheckItem>"},
  { "/Form�t/USERSIZE9", 	NULL,	GS_CMD, 	IDM_USERSIZE9, "<CheckItem>"},
  { "/Form�t/USERSIZE10", 	NULL,	GS_CMD, 	IDM_USERSIZE10, "<CheckItem>"},
  { "/Form�t/USERSIZE11", 	NULL,	GS_CMD, 	IDM_USERSIZE11, "<CheckItem>"},
  { "/Form�t/USERSIZE12", 	NULL,	GS_CMD, 	IDM_USERSIZE12, "<CheckItem>"},
  { "/Form�t/USERSIZE13", 	NULL,	GS_CMD, 	IDM_USERSIZE13, "<CheckItem>"},

  { "/_Pomocn�k",      		"F1",   NULL, 		0, "<Branch>" },
  { "/Pomocn�k/_Obsah", 		NULL,	GS_CMD, 	IDM_HELPCONTENT, NULL },
  { "/Pomocn�k/_Register", NULL,	GS_CMD, 	IDM_HELPSEARCH, NULL },
  { "/Pomocn�k/_Kl�vesov� skratky", 	NULL,	GS_CMD, 	IDM_HELPKEYS, NULL },
  { "/Pomocn�k/sep1",		NULL,   NULL, 		0, "<Separator>" },
  { "/Pomocn�k/R_egistr�cia...", 	NULL,	GS_CMD, 	IDM_REGISTER, NULL },
  { "/Pomocn�k/_�o je...", 		NULL,	GS_CMD, 	IDM_ABOUT, NULL },
};

int menu_sk_len = sizeof (menu_sk) / sizeof (menu_sk[0]);

