/* Copyright (C) 2001-2005, Ghostgum Software Pty Ltd.  All rights reserved.

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
/* nl/gvxlang.c */

#include "gvx.h"
#include "gvxres.h"
#include "gvxlang.h"
#include "gvclang.h"

/* string table ids must be in order since we use a binary search */
STRING_ENTRY string_nl[] = {
#include "gvclang.rc"
#include "gvxlangh.rc"
};

int string_nl_len = sizeof(string_nl)/sizeof(STRING_ENTRY);

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

MENU_ENTRY menu_nl[] = {
  { "/_Bestand", 			NULL, 	NULL, 	IDM_FILEMENU, "<Branch>" },
  { "/Bestand/_Openen...",     	"O", 	GS_CMD,	IDM_OPEN, NULL },
  { "/Bestand/_Bestand selecteren...",    "B", 	GS_CMD,	IDM_SELECT, NULL },
  { "/Bestand/Opslaan _Als......",     	"A", 	GS_CMD,	IDM_SAVEAS, NULL },
  { "/Bestand/_Sluiten",     	"S", 	GS_CMD,	IDM_CLOSE, NULL },
  { "/Bestand/_Info...",     	"I", 	GS_CMD,	IDM_INFO, NULL },
  { "/Bestand/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bestand/Om_zetten...",     	NULL, 	GS_CMD,	IDM_CONVERTFILE, NULL },
  { "/Bestand/E_xporteren...",  	"x", 	GS_CMD,	IDM_EXTRACT, NULL },
  { "/Bestand/PS _naar EPS",     	NULL, 	GS_CMD,	IDM_PSTOEPS, NULL },
  { "/Bestand/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bestand/Af_drukken...",   "d", 	GS_CMD,	IDM_PRINT, NULL },
  { "/Bestand/Bestand afd_rukken...",     	NULL, 	GS_CMD,	IDM_SPOOL, NULL },
  { "/Bestand/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bestand/Berichten _tonen",     	"t", 	GS_CMD,	IDM_GSMESS, NULL },
  { "/Bestand/sep4",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bestand/1LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE1, NULL },
  { "/Bestand/2LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE2, NULL },
  { "/Bestand/3LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE3, NULL },
  { "/Bestand/4LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE4, NULL },
  { "/Bestand/sep5",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bestand/_Einde",     		NULL, 	GS_CMD,	IDM_EXIT, NULL },

  { "/B_ewerken",      		NULL,   NULL, 	IDM_EDITMENU, "<Branch>" },
/*
  { "/Bewerken/_Kopiëren",     		"<control>C", GS_CMD, IDM_COPYCLIP, NULL },
  { "/Bewerken/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bewerken/_Bitmap omzetten",    NULL,	GS_CMD,	IDM_CONVERT, NULL },
*/
  { "/Bewerken/_Plakken"/* WRONG */,     	NULL,	GS_CMD,	IDM_PASTETO, NULL },
  { "/Bewerken/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bewerken/EPS voorbeeld toevoegen",   NULL,   NULL, 		0, "<Branch>" },
  { "/Bewerken/EPS voorbeeld toevoegen/Interchange", NULL,		GS_CMD, IDM_MAKEEPSI, NULL },
  { "/Bewerken/EPS voorbeeld toevoegen/TIFF 4", NULL,		GS_CMD, IDM_MAKEEPST4, NULL },
  { "/Bewerken/EPS voorbeeld toevoegen/TIFF 6 niet gecomprimeerd", NULL,	GS_CMD, IDM_MAKEEPST6U, NULL },
  { "/Bewerken/EPS voorbeeld toevoegen/TIFF 6 packbits", NULL,	GS_CMD, IDM_MAKEEPST6P, NULL },
  { "/Bewerken/EPS voorbeeld toevoegen/Windows Meta Bestand", NULL,	GS_CMD, IDM_MAKEEPSW, NULL },
  { "/Bewerken/EPS voorbeeld toevoegen/Voorbeeld ingesteld door de gebruiker",NULL,GS_CMD, IDM_MAKEEPSU, NULL },
  { "/Bewerken/_EPS Exporteren",   NULL,   NULL, 	IDM_EXTEPSMENU, "<Branch>" },
  { "/Bewerken/EPS Exporteren/PostScript", NULL,		GS_CMD, IDM_EXTRACTPS, NULL },
  { "/Bewerken/EPS Exporteren/Voorbeeld", NULL,			GS_CMD, IDM_EXTRACTPRE, NULL },
  { "/Bewerken/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bewerken/Maat", NULL,				GS_CMD, IDM_MEASURE, NULL },
  { "/Bewerken/_Omzetten naar vektorindeling...", NULL,		GS_CMD, IDM_PSTOEDIT, NULL },
  { "/Bewerken/_Tekst Exporteren...", NULL,			GS_CMD, IDM_TEXTEXTRACT, NULL },
  { "/Bewerken/_Zoeken...", "<control>F",			GS_CMD, IDM_TEXTFIND, NULL },
  { "/Bewerken/Volgende Zoeken", "F3",			GS_CMD, IDM_TEXTFINDNEXT, NULL },

  { "/O_pties",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opties/Eenvoudige Configuratie...", 	NULL,		GS_CMD, IDM_CFG, NULL },
  { "/Opties/Aangepaste Configuratie...", 	NULL,		GS_CMD, IDM_GSCOMMAND, NULL },
  { "/Opties/Geluid...", 		NULL,		GS_CMD, IDM_SOUNDS, NULL },
  { "/Opties/Eenheden",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opties/Eenheden/_pt", 		NULL,		GS_CMD, IDM_UNITPT, "<CheckItem>" },
  { "/Opties/Eenheden/_mm", 		NULL,		GS_CMD, IDM_UNITMM, "<CheckItem>" },
  { "/Opties/Eenheden/_inch", 		NULL,		GS_CMD, IDM_UNITINCH, "<CheckItem>" },
  { "/Opties/Eenheden/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Opties/Eenheden/Beste _Resolutie", 	NULL,		GS_CMD, IDM_UNITFINE, "<CheckItem>" },
  { "/Opties/_Taal",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opties/Taal/_English", 	NULL,		GS_CMD, IDM_LANGEN, "<CheckItem>" },
  { "/Opties/Taal/Catalan", 	NULL,		GS_CMD, IDM_LANGCT, "<CheckItem>" },
  { "/Opties/Taal/_Deutsch", 	NULL,		GS_CMD, IDM_LANGDE, "<CheckItem>" },
  { "/Opties/Taal/_Greek", 	NULL,		GS_CMD, IDM_LANGGR, "<CheckItem>" },
  { "/Opties/Taal/E_spañol", 	NULL,		GS_CMD, IDM_LANGES, "<CheckItem>" },
  { "/Opties/Taal/_Français", 	NULL,		GS_CMD, IDM_LANGFR, "<CheckItem>" },
  { "/Opties/Taal/_Italiano", 	NULL,		GS_CMD, IDM_LANGIT, "<CheckItem>" },
  { "/Opties/Taal/_Nederlands", NULL,		GS_CMD, IDM_LANGNL, "<CheckItem>" },
  { "/Opties/Taal/Russian", 	NULL,		GS_CMD, IDM_LANGRU, "<CheckItem>" },
  { "/Opties/Taal/Slovak", 	NULL,		GS_CMD, IDM_LANGSK, "<CheckItem>" },
  { "/Opties/Taal/Svenska", 	NULL,		GS_CMD, IDM_LANGSE, "<CheckItem>" },
  { "/Opties/PStoText",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opties/PStoText/Uitgeschakeld", 	NULL,		GS_CMD, IDM_PSTOTEXTDIS, "<CheckItem>" },
  { "/Opties/PStoText/Normaal", 	NULL,		GS_CMD, IDM_PSTOTEXTNORM, "<CheckItem>" },
  { "/Opties/PStoText/Dvips Cork Codering", 	NULL,	GS_CMD, IDM_PSTOTEXTCORK, "<CheckItem>" },
  { "/Opties/DSC Warnings",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opties/DSC Waarschuwingen/Uit", 	NULL,		GS_CMD, IDM_DSC_OFF, "<CheckItem>" },
  { "/Opties/DSC Waarschuwingen/Fouten", 	NULL,		GS_CMD, IDM_DSC_ERROR, "<CheckItem>" },
  { "/Opties/DSC Waarschuwingen/Waarschuwingen", 	NULL,		GS_CMD, IDM_DSC_WARN, "<CheckItem>" },
  { "/Opties/DSC Waarschuwingen/Alle", 	NULL,		GS_CMD, IDM_DSC_INFO, "<CheckItem>" },
  { "/Opties/Instellingen _Nu bewaren", 	NULL,		GS_CMD, IDM_SETTINGS, NULL },
  { "/Opties/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Opties/Instellingen bewaren bij het afsluiten", 	NULL,		GS_CMD, IDM_SAVESETTINGS, "<CheckItem>" },
  { "/Opties/Bestanden Beveiligen", 			NULL,		GS_CMD, IDM_SAFER, "<CheckItem>" },
  { "/Opties/Laatste Directory bewaren", 	NULL,		GS_CMD, IDM_SAVEDIR, "<CheckItem>" },
  { "/Opties/Knoppenbalk", 		NULL,		GS_CMD, IDM_BUTTONSHOW, "<CheckItem>" },
/*  { "/Opties/Venster aan de bladzijde aanpassen", 	NULL,		GS_CMD, IDM_FITPAGE, "<CheckItem>" }, */
  { "/Opties/Automatisch Ve_rversen", 	NULL,		GS_CMD, IDM_AUTOREDISPLAY, "<CheckItem>" },
  { "/Opties/EPS _Knippen", 		NULL,		GS_CMD, IDM_EPSFCLIP, "<CheckItem>" },
  { "/Opties/EPS _Waarschuwing", 		NULL,		GS_CMD, IDM_EPSFWARN, "<CheckItem>" },
  { "/Opties/_DSC Overslaan", 		NULL,		GS_CMD, IDM_IGNOREDSC, "<CheckItem>" },
  { "/Opties/Inbinden _Tonen", 	NULL,		GS_CMD, IDM_SHOWBBOX, "<CheckItem>" },

  { "/_Bekijken",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Bekijken/Volgende bladzijde", 	"plus",	GS_CMD, 	IDM_NEXT, NULL },
  { "/Bekijken/Vorige bladzijde", 	"minus",	GS_CMD, 	IDM_PREV, NULL },
  { "/Bekijken/Ga naar bladzijde", 	"G",	GS_CMD, 	IDM_GOTO, NULL },
  { "/Bekijken/Terug", 		"T",	GS_CMD, 	IDM_GOBACK, NULL },
  { "/Bekijken/Verder",	NULL,	GS_CMD, 	IDM_GOFWD, NULL },
  { "/Bekijken/Opnieuw Tonen", 	"O",	GS_CMD, 	IDM_REDISPLAY, NULL },
  { "/Bekijken/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bekijken/Volgende Pagina en Bovenaan", "space",	GS_CMD, 	IDM_NEXTHOME, NULL },
  { "/Bekijken/Vorige Bladzijde en Bovenaan", "<control>H",GS_CMD, 	IDM_PREVHOME, NULL },
  { "/Bekijken/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bekijken/Volledig Scherm", 	"F4",	GS_CMD, 	IDM_FULLSCREEN, NULL },
  { "/Bekijken/In Venster Aanpassen", 	"F6",	GS_CMD, 	IDM_FITWIN, NULL },

  { "/_Orientatie",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Orientatie/_Automatisch", 		NULL,	GS_CMD, 	IDM_AUTOORIENT, "<CheckItem>" },
  { "/Orientatie/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orientatie/_Staand", 		NULL,	GS_CMD, 	IDM_PORTRAIT, "<CheckItem>" },
  { "/Orientatie/_Liggend", 		NULL,	GS_CMD, 	IDM_LANDSCAPE, "<CheckItem>" },
  { "/Orientatie/_Ondersteboven", 	NULL,	GS_CMD, 	IDM_UPSIDEDOWN, "<CheckItem>" },
  { "/Orientatie/_90 graden naar rechts draaien", 		NULL,	GS_CMD, 	IDM_SEASCAPE, "<CheckItem>" },
  { "/Orientatie/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orientatie/L_iggende positie omdraaien", 	NULL,	GS_CMD, 	IDM_SWAPLANDSCAPE, "<CheckItem>" },

  { "/_Papierformaat", 			NULL,   NULL, 		0, "<Branch>" },
  { "/Papierformaat/_Schermresolutie", NULL,GS_CMD, 	IDM_DISPLAYSETTINGS, NULL },
  { "/Papierformaat/sep1", 		NULL,   NULL, 		0, "<Separator>" },
  { "/Papierformaat/Omgekee_rd formaat", 	NULL,	GS_CMD, 	IDM_MEDIAROTATE, "<CheckItem>"},
  { "/Papierformaat/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Papierformaat/11x17", 		NULL,	GS_CMD, 	IDM_11x17, "<CheckItem>" },
  { "/Papierformaat/A3", 		NULL,	GS_CMD, 	IDM_A3, "<CheckItem>" },
  { "/Papierformaat/A4", 		NULL,	GS_CMD, 	IDM_A4, "<CheckItem>" },
  { "/Papierformaat/A5", 		NULL,	GS_CMD, 	IDM_A5, "<CheckItem>" },
  { "/Papierformaat/B4", 		NULL,	GS_CMD, 	IDM_B4, "<CheckItem>" },
  { "/Papierformaat/B5", 		NULL,	GS_CMD, 	IDM_B5, "<CheckItem>" },
  { "/Papierformaat/Ledger", 		NULL,	GS_CMD, 	IDM_LEDGER, "<CheckItem>" },
  { "/Papierformaat/Legal", 		NULL,	GS_CMD, 	IDM_LEGAL, "<CheckItem>" },
  { "/Papierformaat/Letter", 		NULL,	GS_CMD, 	IDM_LETTER, "<CheckItem>" },
  { "/Papierformaat/Note", 		NULL,	GS_CMD, 	IDM_NOTE, "<CheckItem>" },
  { "/Papierformaat/Door de gebruiker ingesteld...", 	NULL,	GS_CMD, 	IDM_USERSIZE, "<CheckItem>"},

  { "/_Help",      		"F1",   NULL, 		0, "<Branch>" },
  { "/Help/_Inhoud", 		NULL,	GS_CMD, 	IDM_HELPCONTENT, NULL },
  { "/Help/_Hulp zoeken over", NULL,	GS_CMD, 	IDM_HELPSEARCH, NULL },
  { "/Help/_Toetsen", 	NULL,	GS_CMD, 	IDM_HELPKEYS, NULL },
  { "/Help/sep1",		NULL,   NULL, 		0, "<Separator>" },
  { "/Help/_GSview _Registreren...", 	NULL,	GS_CMD, 	IDM_REGISTER, NULL },
  { "/Help/_Over GSview...", 		NULL,	GS_CMD, 	IDM_ABOUT, NULL },
};

int menu_nl_len = sizeof (menu_nl) / sizeof (menu_nl[0]);

