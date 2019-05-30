/* Copyright (C) 2000-2001, Ghostgum Software Pty Ltd.  All rights reserved.
  
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
/* de/gvxlang.c */

#include "gvx.h"
#include "gvxres.h"
#include "gvxlang.h"
#include "gvclang.h"

/* string table ids must be in order since we use a binary search */
STRING_ENTRY string_de[] = {
#include "gvclang.rc"
#include "gvxlangh.rc"
};

int string_de_len = sizeof(string_de)/sizeof(STRING_ENTRY);

MENU_ENTRY menu_de[] = {
  { "/_Datei", 			NULL, 	NULL, 	IDM_FILEMENU, "<Branch>" },
  { "/Datei/_Öffnen...",     	"O", 	GS_CMD,	IDM_OPEN, NULL },
  { "/Datei/_Auswahl Datei...",    "S", 	GS_CMD,	IDM_SELECT, NULL },
  { "/Datei/Speichern _Unter...",     	"A", 	GS_CMD,	IDM_SAVEAS, NULL },
  { "/Datei/_Schließen",     	"C", 	GS_CMD,	IDM_CLOSE, NULL },
  { "/Datei/_Info...",     	"I", 	GS_CMD,	IDM_INFO, NULL },
  { "/Datei/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Datei/_Konvertierung...",     	NULL, 	GS_CMD,	IDM_CONVERTFILE, NULL },
  { "/Datei/_Extrahiere...",     	"E", 	GS_CMD,	IDM_EXTRACT, NULL },
  { "/Datei/PS zu EPS",     	NULL, 	GS_CMD,	IDM_PSTOEPS, NULL },
  { "/Datei/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Datei/_Drucken...",     	"P", 	GS_CMD,	IDM_PRINT, NULL },
  { "/Datei/Drucke Datei...",     	NULL, 	GS_CMD,	IDM_SPOOL, NULL },
  { "/Datei/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Datei/Meldungen anzeigen",     	"M", 	GS_CMD,	IDM_GSMESS, NULL },
  { "/Datei/sep4",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Datei/1LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE1, NULL },
  { "/Datei/2LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE2, NULL },
  { "/Datei/3LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE3, NULL },
  { "/Datei/4LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE4, NULL },
  { "/Datei/sep5",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Datei/E_nde",     		NULL, 	GS_CMD,	IDM_EXIT, NULL },

  { "/_Bearbeiten",      		NULL,   NULL, 	IDM_EDITMENU, "<Branch>" },
/*
  { "/Edit/_Kopieren",     		"<control>C", GS_CMD, IDM_COPYCLIP, NULL },
  { "/Edit/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Edit/Konvertiere _Bitmap",    NULL,	GS_CMD,	IDM_CONVERT, NULL },
*/
  { "/Bearbeiten/Speichere Pixmap als BMP",    	NULL,	GS_CMD,	IDM_PASTETO, NULL },
  { "/Bearbeiten/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bearbeiten/EPS Vorschau ein_fügen",   NULL,   NULL, 		0, "<Branch>" },
  { "/Bearbeiten/EPS Vorschau einfügen/_Interchange", NULL,		GS_CMD, IDM_MAKEEPSI, NULL },
  { "/Bearbeiten/EPS Vorschau einfügen/TIFF 4", NULL,		GS_CMD, IDM_MAKEEPST4, NULL },
  { "/Bearbeiten/EPS Vorschau einfügen/_TIFF 6 unkomprimiert", NULL,	GS_CMD, IDM_MAKEEPST6U, NULL },
  { "/Bearbeiten/EPS Vorschau einfügen/TIFF 6 _gepackt", NULL,	GS_CMD, IDM_MAKEEPST6P, NULL },
  { "/Bearbeiten/EPS Vorschau einfügen/_Windows Metafile", NULL,	GS_CMD, IDM_MAKEEPSW, NULL },
  { "/Bearbeiten/EPS Vorschau einfügen/_Benutzer definiert",NULL,GS_CMD, IDM_MAKEEPSU, NULL },
  { "/Bearbeiten/_Extrahiere EPS",   NULL,   NULL, 	IDM_EXTEPSMENU, "<Branch>" },
  { "/Bearbeiten/Extrahiere EPS/_PostScript", NULL,		GS_CMD, IDM_EXTRACTPS, NULL },
  { "/Bearbeiten/Extrahiere EPS/_Vorschau", NULL,			GS_CMD, IDM_EXTRACTPRE, NULL },
  { "/Bearbeiten/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Bearbeiten/Messen", NULL,				GS_CMD, IDM_MEASURE, NULL },
  { "/Bearbeiten/Konvertierung in Vektor Format...", NULL,		GS_CMD, IDM_PSTOEDIT, NULL },
  { "/Bearbeiten/_Text extrahieren...", NULL,			GS_CMD, IDM_TEXTEXTRACT, NULL },
  { "/Bearbeiten/_Suche...", "<control>F",			GS_CMD, IDM_TEXTFIND, NULL },
  { "/Bearbeiten/Suche _Nächsten", "<control>G",			GS_CMD, IDM_TEXTFINDNEXT, NULL },

  { "/O_ptionen",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Optionen/Konfiguration...", 	NULL,		GS_CMD, IDM_CFG, NULL },
  { "/Optionen/Konfiguriere _Ghostscript...", 	NULL,		GS_CMD, IDM_GSCOMMAND, NULL },
  { "/Optionen/Klänge...", 		NULL,		GS_CMD, IDM_SOUNDS, NULL },
  { "/Optionen/_Einheiten",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Optionen/Einheiten/_pt", 		NULL,		GS_CMD, IDM_UNITPT, "<CheckItem>" },
  { "/Optionen/Einheiten/_mm", 		NULL,		GS_CMD, IDM_UNITMM, "<CheckItem>" },
  { "/Optionen/Einheiten/_inch", 		NULL,		GS_CMD, IDM_UNITINCH, "<CheckItem>" },
  { "/Optionen/Einheiten/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Optionen/Einheiten/_Fine Resolution", 	NULL,		GS_CMD, IDM_UNITFINE, "<CheckItem>" },
  { "/Optionen/_Sprache",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Optionen/Sprache/_English", 	NULL,		GS_CMD, IDM_LANGEN, "<CheckItem>" },
  { "/Optionen/Sprache/Catalan", 	NULL,		GS_CMD, IDM_LANGCT, "<CheckItem>" },
  { "/Optionen/Sprache/_Deutsch", 	NULL,		GS_CMD, IDM_LANGDE, "<CheckItem>" },
  { "/Optionen/Sprache/_Greek", 	NULL,		GS_CMD, IDM_LANGGR, "<CheckItem>" },
  { "/Optionen/Sprache/E_spañol", 	NULL,		GS_CMD, IDM_LANGES, "<CheckItem>" },
  { "/Optionen/Sprache/_Français", 	NULL,		GS_CMD, IDM_LANGFR, "<CheckItem>" },
  { "/Optionen/Sprache/_Italiano", 	NULL,		GS_CMD, IDM_LANGIT, "<CheckItem>" },
  { "/Optionen/Sprache/_Nederlands", 	NULL,		GS_CMD, IDM_LANGNL, "<CheckItem>" },
  { "/Optionen/Sprache/Russian", 	NULL,		GS_CMD, IDM_LANGRU, "<CheckItem>" },
  { "/Optionen/Sprache/Slovak", 	NULL,		GS_CMD, IDM_LANGSK, "<CheckItem>" },
  { "/Optionen/Sprache/Svenska", 	NULL,		GS_CMD, IDM_LANGSE, "<CheckItem>" },
  { "/Optionen/PSzuText",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Optionen/PSzuText/Deaktiviert", 	NULL,		GS_CMD, IDM_PSTOTEXTDIS, "<CheckItem>" },
  { "/Optionen/PSzuText/Normal", 	NULL,		GS_CMD, IDM_PSTOTEXTNORM, "<CheckItem>" },
  { "/Optionen/PSzuText/Dvips Cork Encoding", 	NULL,	GS_CMD, IDM_PSTOTEXTCORK, "<CheckItem>" },
  { "/Optionen/DSC Warnungen",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Optionen/DSC Warnungen/Aus", 	NULL,		GS_CMD, IDM_DSC_OFF, "<CheckItem>" },
  { "/Optionen/DSC Warnungen/Fehler", 	NULL,		GS_CMD, IDM_DSC_ERROR, "<CheckItem>" },
  { "/Optionen/DSC Warnungen/Warnungen", 	NULL,		GS_CMD, IDM_DSC_WARN, "<CheckItem>" },
  { "/Optionen/DSC Warnungen/Alle", 	NULL,		GS_CMD, IDM_DSC_INFO, "<CheckItem>" },
  { "/Optionen/Speichere _Optionen jetzt", 	NULL,		GS_CMD, IDM_SETTINGS, NULL },
  { "/Optionen/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Optionen/_Speichere Optionen am Ende", 	NULL,		GS_CMD, IDM_SAVESETTINGS, "<CheckItem>" },
  { "/Optionen/Sch_reibschutz für Dateien", 			NULL,		GS_CMD, IDM_SAFER, "<CheckItem>" },
  { "/Optionen/Merke Letztes _Verzeichnis", 	NULL,		GS_CMD, IDM_SAVEDIR, "<CheckItem>" },
  { "/Optionen/_Hilfsmittelpalette", 		NULL,		GS_CMD, IDM_BUTTONSHOW, "<CheckItem>" },
/*  { "/Optionen/_Fenster an Seite an_passen", 	NULL,		GS_CMD, IDM_FITPAGE, "<CheckItem>" }, */
/*  { "/Optionen/Schne_lles Öffnen", 		NULL,		GS_CMD, IDM_QUICK_OPEN, "<CheckItem>" }, */
  { "/Optionen/_Automatisches Neuzeichnen", 	NULL,		GS_CMD, IDM_AUTOREDISPLAY, "<CheckItem>" },
  { "/Optionen/EPS Ausschni_tt", 		NULL,		GS_CMD, IDM_EPSFCLIP, "<CheckItem>" },
  { "/Optionen/EPS _Warnung", 		NULL,		GS_CMD, IDM_EPSFWARN, "<CheckItem>" },
  { "/Optionen/_Ignoriere DSC", 		NULL,		GS_CMD, IDM_IGNOREDSC, "<CheckItem>" },
  { "/Optionen/Zeige Bounding Bo_x", 	NULL,		GS_CMD, IDM_SHOWBBOX, "<CheckItem>" },

  { "/_Anzeige",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Anzeige/_Nächste Seite", 	"+",	GS_CMD, 	IDM_NEXT, NULL },
  { "/Anzeige/_Vorherige Seite", 	"-",	GS_CMD, 	IDM_PREV, NULL },
  { "/Anzeige/_Gehe zu Seite", 	"G",	GS_CMD, 	IDM_GOTO, NULL },
  { "/Anzeige/_Zurück",		"B",	GS_CMD, 	IDM_GOBACK, NULL },
  { "/Anzeige/W_eiter",		NULL,	GS_CMD, 	IDM_GOFWD, NULL },
  { "/Anzeige/_Wiederaufbau", 	"R",	GS_CMD, 	IDM_REDISPLAY, NULL },
  { "/Anzeige/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Anzeige/Nächste Seite oben", " ",	GS_CMD, 	IDM_NEXTHOME, NULL },
  { "/Anzeige/Vorherige Seite oben", "<control>H",GS_CMD, 	IDM_PREVHOME, NULL },
  { "/Anzeige/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Anzeige/Ganzer _Bildschirm", 	NULL,	GS_CMD, 	IDM_FULLSCREEN, NULL },
  { "/Anzeige/Fenster an Seite anpassen", 	NULL,	GS_CMD, 	IDM_FITWIN, NULL },

  { "/_Seitenausrichtung",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Seitenausrichtung/_Auto Ausrichtung", 		NULL,	GS_CMD, 	IDM_AUTOORIENT, "<CheckItem>" },
  { "/Seitenausrichtung/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Seitenausrichtung/_Hochformat", 		NULL,	GS_CMD, 	IDM_PORTRAIT, "<CheckItem>" },
  { "/Seitenausrichtung/_Querformat", 		NULL,	GS_CMD, 	IDM_LANDSCAPE, "<CheckItem>" },
  { "/Seitenausrichtung/_Kopfüber", 	NULL,	GS_CMD, 	IDM_UPSIDEDOWN, "<CheckItem>" },
  { "/Seitenausrichtung/_Umgekehrt Quer", 		NULL,	GS_CMD, 	IDM_SEASCAPE, "<CheckItem>" },
  { "/Seitenausrichtung/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Seitenausrichtung/Gedreht Quer", 	NULL,	GS_CMD, 	IDM_SWAPLANDSCAPE, "<CheckItem>" },

  { "/_Formate", 			NULL,   NULL, 		0, "<Branch>" },
  { "/Formate/_Anzeige Einstellungen...", NULL,GS_CMD, 	IDM_DISPLAYSETTINGS, NULL },
  { "/Formate/sep1", 		NULL,   NULL, 		0, "<Separator>" },
  { "/Formate/_Rotiere Medium", 	NULL,	GS_CMD, 	IDM_MEDIAROTATE, "<CheckItem>"},
  { "/Formate/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Formate/11x17", 		NULL,	GS_CMD, 	IDM_11x17, "<CheckItem>" },
  { "/Formate/A3", 		NULL,	GS_CMD, 	IDM_A3, "<CheckItem>" },
  { "/Formate/A4", 		NULL,	GS_CMD, 	IDM_A4, "<CheckItem>" },
  { "/Formate/A5", 		NULL,	GS_CMD, 	IDM_A5, "<CheckItem>" },
  { "/Formate/B4", 		NULL,	GS_CMD, 	IDM_B4, "<CheckItem>" },
  { "/Formate/B5", 		NULL,	GS_CMD, 	IDM_B5, "<CheckItem>" },
  { "/Formate/Ledger", 		NULL,	GS_CMD, 	IDM_LEDGER, "<CheckItem>" },
  { "/Formate/Legal", 		NULL,	GS_CMD, 	IDM_LEGAL, "<CheckItem>" },
  { "/Formate/Letter", 		NULL,	GS_CMD, 	IDM_LETTER, "<CheckItem>" },
  { "/Formate/Note", 		NULL,	GS_CMD, 	IDM_NOTE, "<CheckItem>" },
  { "/Formate/Benutzerdefiniert...", 	NULL,	GS_CMD, 	IDM_USERSIZE, "<CheckItem>"},
  { "/Formate/USERSIZE1", 	NULL,	GS_CMD, 	IDM_USERSIZE1, "<CheckItem>"},
  { "/Formate/USERSIZE2", 	NULL,	GS_CMD, 	IDM_USERSIZE2, "<CheckItem>"},
  { "/Formate/USERSIZE3", 	NULL,	GS_CMD, 	IDM_USERSIZE3, "<CheckItem>"},
  { "/Formate/USERSIZE4", 	NULL,	GS_CMD, 	IDM_USERSIZE4, "<CheckItem>"},
  { "/Formate/USERSIZE5", 	NULL,	GS_CMD, 	IDM_USERSIZE5, "<CheckItem>"},
  { "/Formate/USERSIZE6", 	NULL,	GS_CMD, 	IDM_USERSIZE6, "<CheckItem>"},
  { "/Formate/USERSIZE7", 	NULL,	GS_CMD, 	IDM_USERSIZE7, "<CheckItem>"},
  { "/Formate/USERSIZE8", 	NULL,	GS_CMD, 	IDM_USERSIZE8, "<CheckItem>"},
  { "/Formate/USERSIZE9", 	NULL,	GS_CMD, 	IDM_USERSIZE9, "<CheckItem>"},
  { "/Formate/USERSIZE10", 	NULL,	GS_CMD, 	IDM_USERSIZE10, "<CheckItem>"},
  { "/Formate/USERSIZE11", 	NULL,	GS_CMD, 	IDM_USERSIZE11, "<CheckItem>"},
  { "/Formate/USERSIZE12", 	NULL,	GS_CMD, 	IDM_USERSIZE12, "<CheckItem>"},
  { "/Formate/USERSIZE13", 	NULL,	GS_CMD, 	IDM_USERSIZE13, "<CheckItem>"},

  { "/_Hilfe",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Hilfe/_Inhalt", 		NULL,	GS_CMD, 	IDM_HELPCONTENT, NULL },
  { "/Hilfe/_Suche nach Hilfe für...", NULL,	GS_CMD, 	IDM_HELPSEARCH, NULL },
  { "/Hilfe/_Tastenübersicht", 	NULL,	GS_CMD, 	IDM_HELPKEYS, NULL },
  { "/Hilfe/sep1",		NULL,   NULL, 		0, "<Separator>" },
  { "/Hilfe/_Registrierung...", 	NULL,	GS_CMD, 	IDM_REGISTER, NULL },
  { "/Hilfe/_Über GSview...", 		NULL,	GS_CMD, 	IDM_ABOUT, NULL },
};

int menu_de_len = sizeof (menu_de) / sizeof (menu_de[0]);

