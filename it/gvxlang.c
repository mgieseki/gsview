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
/* it/gvxlang.c */

#include "gvx.h"
#include "gvxres.h"
#include "gvxlang.h"
#include "gvclang.h"

/* string table ids must be in order since we use a binary search */
STRING_ENTRY string_it[] = {
#include "gvclang.rc"
#include "gvxlangh.rc"
};

int string_it_len = sizeof(string_it)/sizeof(STRING_ENTRY);

MENU_ENTRY menu_it[] = {
  { "/_File", 			NULL, 	NULL, 	IDM_FILEMENU, "<Branch>" },
  { "/File/_Apri...",     	"O", 	GS_CMD,	IDM_OPEN, NULL },
  { "/File/Sele_ziona File...",    "S", 	GS_CMD,	IDM_SELECT, NULL },
  { "/File/Salva con _Nome...",     	"A", 	GS_CMD,	IDM_SAVEAS, NULL },
  { "/File/_Chiudi",     	"C", 	GS_CMD,	IDM_CLOSE, NULL },
  { "/File/_Informazioni...",     	"I", 	GS_CMD,	IDM_INFO, NULL },
  { "/File/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/File/_Converti...",     	NULL, 	GS_CMD,	IDM_CONVERTFILE, NULL },
  { "/File/Es_trai...",     	"E", 	GS_CMD,	IDM_EXTRACT, NULL },
  { "/File/Converti PS in EPS",     	NULL, 	GS_CMD,	IDM_PSTOEPS, NULL },
  { "/File/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/File/Stam_pa...",     	"P", 	GS_CMD,	IDM_PRINT, NULL },
  { "/File/Stampa File...",     	NULL, 	GS_CMD,	IDM_SPOOL, NULL },
  { "/File/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/File/_Mostra Messaggi",     	"M", 	GS_CMD,	IDM_GSMESS, NULL },
  { "/File/sep4",     		NULL,   NULL, 		0, "<Separator>" },
  { "/File/1LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE1, NULL },
  { "/File/2LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE2, NULL },
  { "/File/3LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE3, NULL },
  { "/File/4LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE4, NULL },
  { "/File/sep5",     		NULL,   NULL, 		0, "<Separator>" },
  { "/File/_Esci",     		NULL, 	GS_CMD,	IDM_EXIT, NULL },

  { "/_Modifica",      		NULL,   NULL, 	IDM_EDITMENU, "<Branch>" },
/*
  { "/Modifica/_Copia",     		"<control>C", GS_CMD, IDM_COPYCLIP, NULL },
  { "/Modifica/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Modifica/Converti _Bitmap",    NULL,	GS_CMD,	IDM_CONVERT, NULL },
*/
  { "/Modifica/Save Pixmap as BMP",     	NULL,	GS_CMD,	IDM_PASTETO, NULL }, /* ENGLISH */
  { "/Modifica/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Modifica/_Aggiungi Anteprima EPS",   NULL,   NULL, 		0, "<Branch>" },
  { "/Modifica/Aggiungi Anteprima EPS/_Interchange", NULL,		GS_CMD, IDM_MAKEEPSI, NULL },
  { "/Modifica/Aggiungi Anteprima EPS/TIFF 4", NULL,		GS_CMD, IDM_MAKEEPST4, NULL },
  { "/Modifica/Aggiungi Anteprima EPS/_TIFF 6 non compresso", NULL,	GS_CMD, IDM_MAKEEPST6U, NULL },
  { "/Modifica/Aggiungi Anteprima EPS/TIFF 6 _packbits", NULL,	GS_CMD, IDM_MAKEEPST6P, NULL },
  { "/Modifica/Aggiungi Anteprima EPS/_Windows Metafile", NULL,	GS_CMD, IDM_MAKEEPSW, NULL },
  { "/Modifica/Aggiungi Anteprima EPS/Anteprima fornita dall'_Utente",NULL,GS_CMD, IDM_MAKEEPSU, NULL },
  { "/Modifica/_Estrai EPS",   NULL,   NULL, 	IDM_EXTEPSMENU, "<Branch>" },
  { "/Modifica/Estrai EPS/_PostScript", NULL,		GS_CMD, IDM_EXTRACTPS, NULL },
  { "/Modifica/Estrai EPS/_Anteprima", NULL,			GS_CMD, IDM_EXTRACTPRE, NULL },
  { "/Modifica/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Modifica/Misure", NULL,				GS_CMD, IDM_MEASURE, NULL },
  { "/Modifica/Converti in formato vettoriale...", NULL,		GS_CMD, IDM_PSTOEDIT, NULL },
  { "/Modifica/Es_trai testo...", NULL,			GS_CMD, IDM_TEXTEXTRACT, NULL },
  { "/Modifica/C_erca...", "<control>F",			GS_CMD, IDM_TEXTFIND, NULL },
  { "/Modifica/Cerca _Successivo", "<control>G",			GS_CMD, IDM_TEXTFINDNEXT, NULL },

  { "/_Opzioni",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opzioni/Configurazione Facile...", 	NULL,		GS_CMD, IDM_CFG, NULL },
  { "/Opzioni/Configurazione Avanzata...", 	NULL,		GS_CMD, IDM_GSCOMMAND, NULL },
  { "/Opzioni/Suoni...", 		NULL,		GS_CMD, IDM_SOUNDS, NULL },
  { "/Opzioni/_Unità di misura",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opzioni/Unità di misura/_pt", 		NULL,		GS_CMD, IDM_UNITPT, "<CheckItem>" },
  { "/Opzioni/Unità di misura/_mm", 		NULL,		GS_CMD, IDM_UNITMM, "<CheckItem>" },
  { "/Opzioni/Unità di misura/poll_ici", 		NULL,		GS_CMD, IDM_UNITINCH, "<CheckItem>" },
  { "/Opzioni/Unità di misura/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Opzioni/Unità di misura/Risoluzione _Fine", 	NULL,		GS_CMD, IDM_UNITFINE, "<CheckItem>" },
  { "/Opzioni/_Lingua",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opzioni/Lingua/_English", 	NULL,		GS_CMD, IDM_LANGEN, "<CheckItem>" },
  { "/Opzioni/Lingua/Catalan", 	NULL,		GS_CMD, IDM_LANGCT, "<CheckItem>" },
  { "/Opzioni/Lingua/_Deutsch", 	NULL,		GS_CMD, IDM_LANGDE, "<CheckItem>" },
  { "/Opzioni/Lingua/_Greek", 		NULL,		GS_CMD, IDM_LANGGR, "<CheckItem>" },
  { "/Opzioni/Lingua/E_spañol", 	NULL,		GS_CMD, IDM_LANGES, "<CheckItem>" },
  { "/Opzioni/Lingua/_Français", 	NULL,		GS_CMD, IDM_LANGFR, "<CheckItem>" },
  { "/Opzioni/Lingua/_Italiano", 	NULL,		GS_CMD, IDM_LANGIT, "<CheckItem>" },
  { "/Opzioni/Lingua/_Nederlands", 	NULL,		GS_CMD, IDM_LANGNL, "<CheckItem>" },
  { "/Opzioni/Lingua/Russian", 	NULL,		GS_CMD, IDM_LANGRU, "<CheckItem>" },
  { "/Opzioni/Lingua/Slovak", 	NULL,		GS_CMD, IDM_LANGSK, "<CheckItem>" },
  { "/Opzioni/Lingua/Svenska", 	NULL,		GS_CMD, IDM_LANGSE, "<CheckItem>" },
  { "/Opzioni/PStoText",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opzioni/PStoText/Disabilitato", 	NULL,		GS_CMD, IDM_PSTOTEXTDIS, "<CheckItem>" },
  { "/Opzioni/PStoText/Normale", 	NULL,		GS_CMD, IDM_PSTOTEXTNORM, "<CheckItem>" },
  { "/Opzioni/PStoText/Codifica Dvips Cork", 	NULL,	GS_CMD, IDM_PSTOTEXTCORK, "<CheckItem>" },
  { "/Opzioni/Avvisi DSC",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opzioni/Avvisi DSC/Disattivato", 	NULL,		GS_CMD, IDM_DSC_OFF, "<CheckItem>" },
  { "/Opzioni/Avvisi DSC/Errori", 	NULL,		GS_CMD, IDM_DSC_ERROR, "<CheckItem>" },
  { "/Opzioni/Avvisi DSC/Avvisi", 	NULL,		GS_CMD, IDM_DSC_WARN, "<CheckItem>" },
  { "/Opzioni/Avvisi DSC/Tutti", 	NULL,		GS_CMD, IDM_DSC_INFO, "<CheckItem>" },
  { "/Opzioni/Salva Impostazioni Adess_o", 	NULL,		GS_CMD, IDM_SETTINGS, NULL },
  { "/Opzioni/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Opzioni/_Salva Impostazioni all'uscita", 	NULL,		GS_CMD, IDM_SAVESETTINGS, "<CheckItem>" },
  { "/Opzioni/Si_curezza", 			NULL,		GS_CMD, IDM_SAFER, "<CheckItem>" },
  { "/Opzioni/Salva Ultima _Cartella", 	NULL,		GS_CMD, IDM_SAVEDIR, "<CheckItem>" },
  { "/Opzioni/_Barra Pulsanti", 		NULL,		GS_CMD, IDM_BUTTONSHOW, "<CheckItem>" },
/*  { "/Opzioni/_Adatta la Finestra alla Pagina", 	NULL,		GS_CMD, IDM_FITPAGE, "<CheckItem>" }, */
/*  { "/Opzioni/Apertura &Veloce", 		NULL,		GS_CMD, IDM_QUICK_OPEN, "<CheckItem>" }, */
  { "/Opzioni/_Rivisualizza Automaticamente", 	NULL,		GS_CMD, IDM_AUTOREDISPLAY, "<CheckItem>" },
  { "/Opzioni/Rita_glia EPS", 		NULL,		GS_CMD, IDM_EPSFCLIP, "<CheckItem>" },
  { "/Opzioni/_Avvisi EPS", 		NULL,		GS_CMD, IDM_EPSFWARN, "<CheckItem>" },
  { "/Opzioni/_Ignora DSC", 		NULL,		GS_CMD, IDM_IGNOREDSC, "<CheckItem>" },
  { "/Opzioni/Mostra Limiti del Bo_x", 	NULL,		GS_CMD, IDM_SHOWBBOX, "<CheckItem>" },

  { "/_Visualizza",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Visualizza/Pagina _Successiva", 	"+",	GS_CMD, 	IDM_NEXT, NULL },
  { "/Visualizza/Pagina _Precedente", 	"-",	GS_CMD, 	IDM_PREV, NULL },
  { "/Visualizza/_Vai a Pagina", 	"G",	GS_CMD, 	IDM_GOTO, NULL },
  { "/Visualizza/Vai In_dietro", 		"B",	GS_CMD, 	IDM_GOBACK, NULL },
  { "/Visualizza/Vai A_vanti",	NULL,	GS_CMD, 	IDM_GOFWD, NULL },
  { "/Visualizza/_Rivisualizza", 	"R",	GS_CMD, 	IDM_REDISPLAY, NULL },
  { "/Visualizza/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Visualizza/Pagina successiva in alto", " ",	GS_CMD, 	IDM_NEXTHOME, NULL },
  { "/Visualizza/Pagina precedente in alto", "<control>H",GS_CMD, 	IDM_PREVHOME, NULL },
  { "/Visualizza/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Visualizza/A tutto _Schermo", 	NULL,	GS_CMD, 	IDM_FULLSCREEN, NULL },
  { "/Visualizza/Adatta _Finestra", 	NULL,	GS_CMD, 	IDM_FITWIN, NULL },

  { "/_Oreintamento",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Oreintamento/_Automatico", 		NULL,	GS_CMD, 	IDM_AUTOORIENT, "<CheckItem>" },
  { "/Oreintamento/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Oreintamento/_Verticale", 		NULL,	GS_CMD, 	IDM_PORTRAIT, "<CheckItem>" },
  { "/Oreintamento/_Orizzontale", 		NULL,	GS_CMD, 	IDM_LANDSCAPE, "<CheckItem>" },
  { "/Oreintamento/Vai In_dietro", 	NULL,	GS_CMD, 	IDM_UPSIDEDOWN, "<CheckItem>" },
  { "/Oreintamento/O_rizzontale capovolto", 		NULL,	GS_CMD, 	IDM_SEASCAPE, "<CheckItem>" },
  { "/Oreintamento/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Oreintamento/_Inverti Orizzontale", 	NULL,	GS_CMD, 	IDM_SWAPLANDSCAPE, "<CheckItem>" },

  { "/_Carta", 			NULL,   NULL, 		0, "<Branch>" },
  { "/Carta/_Impostazioni visualizzazione...", NULL,GS_CMD, 	IDM_DISPLAYSETTINGS, NULL },
  { "/Carta/sep1", 		NULL,   NULL, 		0, "<Separator>" },
  { "/Carta/_Ruota Carta", 	NULL,	GS_CMD, 	IDM_MEDIAROTATE, "<CheckItem>"},
  { "/Carta/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Carta/11x17", 		NULL,	GS_CMD, 	IDM_11x17, "<CheckItem>" },
  { "/Carta/A3", 		NULL,	GS_CMD, 	IDM_A3, "<CheckItem>" },
  { "/Carta/A4", 		NULL,	GS_CMD, 	IDM_A4, "<CheckItem>" },
  { "/Carta/A5", 		NULL,	GS_CMD, 	IDM_A5, "<CheckItem>" },
  { "/Carta/B4", 		NULL,	GS_CMD, 	IDM_B4, "<CheckItem>" },
  { "/Carta/B5", 		NULL,	GS_CMD, 	IDM_B5, "<CheckItem>" },
  { "/Carta/Ledger", 		NULL,	GS_CMD, 	IDM_LEDGER, "<CheckItem>" },
  { "/Carta/Legal", 		NULL,	GS_CMD, 	IDM_LEGAL, "<CheckItem>" },
  { "/Carta/Letter", 		NULL,	GS_CMD, 	IDM_LETTER, "<CheckItem>" },
  { "/Carta/Note", 		NULL,	GS_CMD, 	IDM_NOTE, "<CheckItem>" },
  { "/Carta/Definite dall'utente...", 	NULL,	GS_CMD, 	IDM_USERSIZE, "<CheckItem>"},

  { "/_?",      		NULL,   NULL, 		0, "<Branch>" },
  { "/?/_Sommario", 		NULL,	GS_CMD, 	IDM_HELPCONTENT, NULL },
  { "/?/_Indice", NULL,	GS_CMD, 	IDM_HELPSEARCH, NULL },
  { "/?/_Tasti di scelta rapida", 	NULL,	GS_CMD, 	IDM_HELPKEYS, NULL },
  { "/?/sep1",		NULL,   NULL, 		0, "<Separator>" },
  { "/?/_Registrazione...", 	NULL,	GS_CMD, 	IDM_REGISTER, NULL },
  { "/?/_Informazioni su GSView...", 		NULL,	GS_CMD, 	IDM_ABOUT, NULL },
};

int menu_it_len = sizeof (menu_it) / sizeof (menu_it[0]);


