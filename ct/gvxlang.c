/* Copyright (C) 2000-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
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
/* ct/gvxlang.c */

#include "gvx.h"
#include "gvxres.h"
#include "gvxlang.h"
#include "gvclang.h"

/* string table ids must be in order since we use a binary search */
STRING_ENTRY string_ct[] = {
#include "gvclang.rc"
#include "gvxlangh.rc"
};

int string_ct_len = sizeof(string_ct)/sizeof(STRING_ENTRY);

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

MENU_ENTRY menu_ct[] = {
  { "/_Fitxer", 			NULL, 	NULL, 	IDM_FILEMENU, "<Branch>" },
  { "/Fitxer/_Obre...",     	"O", 	GS_CMD,	IDM_OPEN, NULL },
  { "/Fitxer/_Selecciona un fitxer...",    "S", 	GS_CMD,	IDM_SELECT, NULL },
  { "/Fitxer/_Anomena i desa...",     	"A", 	GS_CMD,	IDM_SAVEAS, NULL },
  { "/Fitxer/_Tanca",     	"T", 	GS_CMD,	IDM_CLOSE, NULL },
  { "/Fitxer/_Informació...",     	"I", 	GS_CMD,	IDM_INFO, NULL },
  { "/Fitxer/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Fitxer/_Converteix...",     	NULL, 	GS_CMD,	IDM_CONVERT, NULL },
  { "/Fitxer/_Extreu...",     	"E", 	GS_CMD,	IDM_EXTRACT, NULL },
  { "/Fitxer/De PS a EPS",     	NULL, 	GS_CMD,	IDM_PSTOEPS, NULL },
  { "/Fitxer/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Fitxer/Im_primeix...",     	"P", 	GS_CMD,	IDM_PRINT, NULL },
  { "/Fitxer/Imprimeix un fitxer...",     	NULL, 	GS_CMD,	IDM_SPOOL, NULL },
  { "/Fitxer/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Fitxer/Mostra els missatges",     	"M", 	GS_CMD,	IDM_GSMESS, NULL },
  { "/Fitxer/sep4",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Fitxer/1LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE1, NULL },
  { "/Fitxer/2LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE2, NULL },
  { "/Fitxer/3LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE3, NULL },
  { "/Fitxer/4LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE4, NULL },
  { "/Fitxer/sep5",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Fitxer/S_urt",     		NULL, 	GS_CMD,	IDM_EXIT, NULL },

  { "/_Edita",      		NULL,   NULL, 	IDM_EDITMENU, "<Branch>" },
/*
  { "/Edita/_Copia",     		"<control>C", GS_CMD, IDM_COPYCLIP, NULL },
  { "/Edita/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Edita/Converteix el mapa _bits",    NULL,	GS_CMD,	IDM_CONVERT, NULL },
*/
  { "/Edita/Desa la imatge com un BMP",     	NULL,	GS_CMD,	IDM_PASTETO, NULL },
  { "/Edita/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Edita/_Afegeix la visualització prèvia EPS",   NULL,   NULL, 		0, "<Branch>" },
  { "/Edita/Afegeix la visualització prèvia EPS/_Intercanvia", NULL,		GS_CMD, IDM_MAKEEPSI, NULL },
  { "/Edita/Afegeix la visualització prèvia EPS/TIFF 4", NULL,		GS_CMD, IDM_MAKEEPST4, NULL },
  { "/Edita/Afegeix la visualització prèvia EPS/_TIFF 6 sense comprimir", NULL,	GS_CMD, IDM_MAKEEPST6U, NULL },
  { "/Edita/Afegeix la visualització prèvia EPS/TIFF 6 amb com_pressió de bits", NULL,	GS_CMD, IDM_MAKEEPST6P, NULL },
  { "/Edita/Afegeix la visualització prèvia EPS/Metafitxer del _Windows", NULL,	GS_CMD, IDM_MAKEEPSW, NULL },
  { "/Edita/Afegeix la visualització prèvia EPS/Visualització _prèvia proveïda per l'usuari",NULL,GS_CMD, IDM_MAKEEPSU, NULL },
  { "/Edita/_Extreu l'EPS",   NULL,   NULL, 	IDM_EXTEPSMENU, "<Branch>" },
  { "/Edita/Extreu l'EPS/_PostScript", NULL,		GS_CMD, IDM_EXTRACTPS, NULL },
  { "/Edita/Extreu l'EPS/_Visualització prèvia", NULL,			GS_CMD, IDM_EXTRACTPRE, NULL },
  { "/Edita/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Edita/Mida", NULL,				GS_CMD, IDM_MEASURE, NULL },
  { "/Edita/Converteix al format de vector...", NULL,		GS_CMD, IDM_PSTOEDIT, NULL },
  { "/Edita/Extreu el _text...", NULL,			GS_CMD, IDM_TEXTEXTRACT, NULL },
  { "/Edita/_Cerca...", "<control>C",			GS_CMD, IDM_TEXTFIND, NULL },
/*
  { "/Edita/Cerca el _següent", "<control>G",			GS_CMD, IDM_TEXTFINDNEXT, NULL },
*/
  { "/Edita/Cerca el _següent", "F3",			GS_CMD, IDM_TEXTFINDNEXT, NULL },

  { "/O_pcions",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opcions/Configuració fàcil...", 	NULL,		GS_CMD, IDM_CFG, NULL },
  { "/Opcions/Configuració avançada...", 	NULL,		GS_CMD, IDM_GSCOMMAND, NULL },
  { "/Opcions/Sons...", 		NULL,		GS_CMD, IDM_SOUNDS, NULL },
  { "/Opcions/_Unitats",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opcions/Unitats/_pt", 		NULL,		GS_CMD, IDM_UNITPT, "<CheckItem>" },
  { "/Opcions/Unitats/_mm", 		NULL,		GS_CMD, IDM_UNITMM, "<CheckItem>" },
  { "/Opcions/Unitats/p_olzada", 		NULL,		GS_CMD, IDM_UNITINCH, "<CheckItem>" },
  { "/Opcions/Unitats/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Opcions/Unitats/Resolució _fina", 	NULL,		GS_CMD, IDM_UNITFINE, "<CheckItem>" },
  { "/Opcions/_Idioma",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opcions/Idioma/_English", 	NULL,		GS_CMD, IDM_LANGEN, "<CheckItem>" },
  { "/Opcions/Idioma/Catalan", 	NULL,		GS_CMD, IDM_LANGCT, "<CheckItem>" },
  { "/Opcions/Idioma/_Deutsch", 	NULL,		GS_CMD, IDM_LANGDE, "<CheckItem>" },
  { "/Opcions/Idioma/_Greek", 	NULL,		GS_CMD, IDM_LANGGR, "<CheckItem>" },
  { "/Opcions/Idioma/E_spañol", 	NULL,		GS_CMD, IDM_LANGES, "<CheckItem>" },
  { "/Opcions/Idioma/_Français", 	NULL,		GS_CMD, IDM_LANGFR, "<CheckItem>" },
  { "/Opcions/Idioma/_Italiano", 	NULL,		GS_CMD, IDM_LANGIT, "<CheckItem>" },
  { "/Opcions/Idioma/_Nederlands", 	NULL,		GS_CMD, IDM_LANGNL, "<CheckItem>" },
  { "/Opcions/Idioma/Russian", 	NULL,		GS_CMD, IDM_LANGRU, "<CheckItem>" },
  { "/Opcions/Idioma/Slovak", 	NULL,		GS_CMD, IDM_LANGSK, "<CheckItem>" },
  { "/Opcions/Idioma/Svenska", 	NULL,		GS_CMD, IDM_LANGSE, "<CheckItem>" },
  { "/Opcions/De PS a text",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opcions/De PS a text/Inhabilitat", 	NULL,		GS_CMD, IDM_PSTOTEXTDIS, "<CheckItem>" },
  { "/Opcions/De PS a text/Normal", 	NULL,		GS_CMD, IDM_PSTOTEXTNORM, "<CheckItem>" },
  { "/Opcions/De PS a text/Codificiació Dvips Cork", 	NULL,	GS_CMD, IDM_PSTOTEXTCORK, "<CheckItem>" },
  { "/Opcions/Avisos DSC",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opcions/Avisos DSC/Inactius", 	NULL,		GS_CMD, IDM_DSC_OFF, "<CheckItem>" },
  { "/Opcions/Avisos DSC/Errors", 	NULL,		GS_CMD, IDM_DSC_ERROR, "<CheckItem>" },
  { "/Opcions/Avisos DSC/Avisos", 	NULL,		GS_CMD, IDM_DSC_WARN, "<CheckItem>" },
  { "/Opcions/Avisos DSC/Tots", 	NULL,		GS_CMD, IDM_DSC_INFO, "<CheckItem>" },
  { "/Opcions/_Desa les preferències ara", 	NULL,		GS_CMD, IDM_SETTINGS, NULL },
  { "/Opcions/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Opcions/De_sa les preferències en sortir", 	NULL,		GS_CMD, IDM_SAVESETTINGS, "<CheckItem>" },
  { "/Opcions/_Més seguretat", 			NULL,		GS_CMD, IDM_SAFER, "<CheckItem>" },
  { "/Opcions/Desa l'últim _directori", 	NULL,		GS_CMD, IDM_SAVEDIR, "<CheckItem>" },
  { "/Opcions/_Barra de botons", 		NULL,		GS_CMD, IDM_BUTTONSHOW, "<CheckItem>" },
/*  { "/Opcions/_Ajusta la finestra a la pàgina", 	NULL,		GS_CMD, IDM_FITPAGE, "<CheckItem>" }, */
/*  { "/Opcions/Obertura ràpida", 		NULL,		GS_CMD, IDM_QUICK_OPEN, "<CheckItem>" }, */
  { "/Opcions/_Torna a dibuixar automàticament", 	NULL,		GS_CMD, IDM_AUTOREDISPLAY, "<CheckItem>" },
  { "/Opcions/-Retalla l'EPS", 		NULL,		GS_CMD, IDM_EPSFCLIP, "<CheckItem>" },
  { "/Opcions/Avisos EPS", 		NULL,		GS_CMD, IDM_EPSFWARN, "<CheckItem>" },
  { "/Opcions/_Ignora els DSC", 		NULL,		GS_CMD, IDM_IGNOREDSC, "<CheckItem>" },
  { "/Opcions/Mostra el _requadre del límit", 	NULL,		GS_CMD, IDM_SHOWBBOX, "<CheckItem>" },

  { "/_Visualitza",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Visualitza/Pàgina _següent", 	"plus",	GS_CMD, 	IDM_NEXT, NULL },
  { "/Visualitza/Pàgina _anterior", 	"minus",	GS_CMD, 	IDM_PREV, NULL },
  { "/Visualitza/_Vés a la pàgina", 	"G",	GS_CMD, 	IDM_GOTO, NULL },
  { "/Visualitza/Vés _endarrere", 		"B",	GS_CMD, 	IDM_GOBACK, NULL },
  { "/Visualitza/Vés e_ndavant",	NULL,	GS_CMD, 	IDM_GOFWD, NULL },
  { "/Visualitza/Torna a dibuixar", 	"R",	GS_CMD, 	IDM_REDISPLAY, NULL },
  { "/Visualitza/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Visualitza/Pàgina següent i inicial", "space",	GS_CMD, 	IDM_NEXTHOME, NULL },
  { "/Visualitza/Pàgina anterior i inicial", "<control>H",GS_CMD, 	IDM_PREVHOME, NULL },
  { "/Visualitza/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Visualitza/_Pantalla completa", 	"F4",	GS_CMD, 	IDM_FULLSCREEN, NULL },
  { "/Visualitza/Ajusta a la _finestra", 	"F6",	GS_CMD, 	IDM_FITWIN, NULL },

  { "/_Orientació",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Orientació/_Automàtica", 		NULL,	GS_CMD, 	IDM_AUTOORIENT, "<CheckItem>" },
  { "/Orientació/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orientació/_Vertical", 		NULL,	GS_CMD, 	IDM_PORTRAIT, "<CheckItem>" },
  { "/Orientació/_Horitzontal", 		NULL,	GS_CMD, 	IDM_LANDSCAPE, "<CheckItem>" },
  { "/Orientació/V_ertical cap per avall", 	NULL,	GS_CMD, 	IDM_UPSIDEDOWN, "<CheckItem>" },
  { "/Orientació/H_oritzontal al revés", 		NULL,	GS_CMD, 	IDM_SEASCAPE, "<CheckItem>" },
  { "/Orientació/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orientació/_Intercanvia la posició horitzontal", 	NULL,	GS_CMD, 	IDM_SWAPLANDSCAPE, "<CheckItem>" },

  { "/_Suport", 			NULL,   NULL, 		0, "<Branch>" },
  { "/Suport/_Paràmetres de visualització...", NULL,GS_CMD, 	IDM_DISPLAYSETTINGS, NULL },
  { "/Suport/sep1", 		NULL,   NULL, 		0, "<Separator>" },
  { "/Suport/_Gira el suport", 	NULL,	GS_CMD, 	IDM_MEDIAROTATE, "<CheckItem>"},
  { "/Suport/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Suport/11x17", 		NULL,	GS_CMD, 	IDM_11x17, "<CheckItem>" },
  { "/Suport/A3", 		NULL,	GS_CMD, 	IDM_A3, "<CheckItem>" },
  { "/Suport/A4", 		NULL,	GS_CMD, 	IDM_A4, "<CheckItem>" },
  { "/Suport/A5", 		NULL,	GS_CMD, 	IDM_A5, "<CheckItem>" },
  { "/Suport/B4", 		NULL,	GS_CMD, 	IDM_B4, "<CheckItem>" },
  { "/Suport/B5", 		NULL,	GS_CMD, 	IDM_B5, "<CheckItem>" },
  { "/Suport/Llibre major (Ledger)", 		NULL,	GS_CMD, 	IDM_LEDGER, "<CheckItem>" },
  { "/Suport/Legal", 		NULL,	GS_CMD, 	IDM_LEGAL, "<CheckItem>" },
  { "/Suport/Carta (Letter)", 		NULL,	GS_CMD, 	IDM_LETTER, "<CheckItem>" },
  { "/Suport/Nota (Note)", 		NULL,	GS_CMD, 	IDM_NOTE, "<CheckItem>" },
  { "/Suport/Definit per l'usuari...", 	NULL,	GS_CMD, 	IDM_USERSIZE, "<CheckItem>"},
  { "/Suport/USERSIZE1", 	NULL,	GS_CMD, 	IDM_USERSIZE1, "<CheckItem>"},
  { "/Suport/USERSIZE2", 	NULL,	GS_CMD, 	IDM_USERSIZE2, "<CheckItem>"},
  { "/Suport/USERSIZE3", 	NULL,	GS_CMD, 	IDM_USERSIZE3, "<CheckItem>"},
  { "/Suport/USERSIZE4", 	NULL,	GS_CMD, 	IDM_USERSIZE4, "<CheckItem>"},
  { "/Suport/USERSIZE5", 	NULL,	GS_CMD, 	IDM_USERSIZE5, "<CheckItem>"},
  { "/Suport/USERSIZE6", 	NULL,	GS_CMD, 	IDM_USERSIZE6, "<CheckItem>"},
  { "/Suport/USERSIZE7", 	NULL,	GS_CMD, 	IDM_USERSIZE7, "<CheckItem>"},
  { "/Suport/USERSIZE8", 	NULL,	GS_CMD, 	IDM_USERSIZE8, "<CheckItem>"},
  { "/Suport/USERSIZE9", 	NULL,	GS_CMD, 	IDM_USERSIZE9, "<CheckItem>"},
  { "/Suport/USERSIZE10", 	NULL,	GS_CMD, 	IDM_USERSIZE10, "<CheckItem>"},
  { "/Suport/USERSIZE11", 	NULL,	GS_CMD, 	IDM_USERSIZE11, "<CheckItem>"},
  { "/Suport/USERSIZE12", 	NULL,	GS_CMD, 	IDM_USERSIZE12, "<CheckItem>"},
  { "/Suport/USERSIZE13", 	NULL,	GS_CMD, 	IDM_USERSIZE13, "<CheckItem>"},

  { "/_Ajuda",      		"F1",   NULL, 		0, "<Branch>" },
  { "/Ajuda/_Continguts", 		NULL,	GS_CMD, 	IDM_HELPCONTENT, NULL },
  { "/Ajuda/_Cerca ajuda sobre", NULL,	GS_CMD, 	IDM_HELPSEARCH, NULL },
  { "/Ajuda/_Tecles d'ajuda", 	NULL,	GS_CMD, 	IDM_HELPKEYS, NULL },
  { "/Ajuda/sep1",		NULL,   NULL, 		0, "<Separator>" },
  { "/Ajuda/_Registre...", 	NULL,	GS_CMD, 	IDM_REGISTER, NULL },
  { "/Ajuda/_Quant al...", 		NULL,	GS_CMD, 	IDM_ABOUT, NULL },
};

int menu_ct_len = sizeof (menu_ct) / sizeof (menu_ct[0]);

