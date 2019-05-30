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
/* fr/gvxlang.c */

#include "gvx.h"
#include "gvxres.h"
#include "gvxlang.h"
#include "gvclang.h"

/* string table ids must be in order since we use a binary search */
STRING_ENTRY string_fr[] = {
#include "gvclang.rc"
#include "gvxlangh.rc"
};

int string_fr_len = sizeof(string_fr)/sizeof(STRING_ENTRY);

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


MENU_ENTRY menu_fr[] = {
  { "/_Fichier", 			NULL, 	NULL, 	IDM_FILEMENU, "<Branch>" },
  { "/Fichier/_Ouvrir...",     	"O", 	GS_CMD,	IDM_OPEN, NULL },
  { "/Fichier/_Sélectionner un fichier...",    "S", 	GS_CMD,	IDM_SELECT, NULL },
  { "/Fichier/_Enregistrer sous...",     	"A", 	GS_CMD,	IDM_SAVEAS, NULL },
  { "/Fichier/_Fermer",     	"C", 	GS_CMD,	IDM_CLOSE, NULL },
  { "/Fichier/_Infomations...",     	"I", 	GS_CMD,	IDM_INFO, NULL },
  { "/Fichier/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Fichier/_Convert...",     	NULL, 	GS_CMD,	IDM_CONVERTFILE, NULL },
  { "/Fichier/E_xtraire...",     	"E", 	GS_CMD,	IDM_EXTRACT, NULL },
  { "/Fichier/PS _vers EPS",     	NULL, 	GS_CMD,	IDM_PSTOEPS, NULL },
  { "/Fichier/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Fichier/Im_primer...",     	"P", 	GS_CMD,	IDM_PRINT, NULL },
  { "/Fichier/Imprimer _un fichier...",     	NULL, 	GS_CMD,	IDM_SPOOL, NULL },
  { "/Fichier/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Fichier/Afficher les _messages",     	"M", 	GS_CMD,	IDM_GSMESS, NULL },
  { "/Fichier/sep4",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Fichier/1LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE1, NULL },
  { "/Fichier/2LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE2, NULL },
  { "/Fichier/3LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE3, NULL },
  { "/Fichier/4LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE4, NULL },
  { "/Fichier/sep5",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Fichier/_Quitter",     		NULL, 	GS_CMD,	IDM_EXIT, NULL },

  { "/_Edition",      		NULL,   NULL, 	IDM_EDITMENU, "<Branch>" },
/*
  { "/Edition/_Copier",     		"<control>C", GS_CMD, IDM_COPYCLIP, NULL },
  { "/Edition/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Edition/Convertir _Bitmap",    NULL,	GS_CMD,	IDM_CONVERT, NULL },
*/
  { "/Edition/Save Pixmap as BMP",     	NULL,	GS_CMD,	IDM_PASTETO, NULL }, /* ENGLISH */
  { "/Edition/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Edition/_Ajouter un aperçu EPS",   NULL,   NULL, 		0, "<Branch>" },
  { "/Edition/Ajouter un aperçu EPS/_Interchange", NULL,		GS_CMD, IDM_MAKEEPSI, NULL },
  { "/Edition/Ajouter un aperçu EPS/TIFF 4", NULL,		GS_CMD, IDM_MAKEEPST4, NULL },
  { "/Edition/Ajouter un aperçu EPS/_TIFF 6 non compressé", NULL,	GS_CMD, IDM_MAKEEPST6U, NULL },
  { "/Edition/Ajouter un aperçu EPS/TIFF 6 compressé", NULL,	GS_CMD, IDM_MAKEEPST6P, NULL },
  { "/Edition/Ajouter un aperçu EPS/_Windows Metafile", NULL,	GS_CMD, IDM_MAKEEPSW, NULL },
  { "/Edition/Add EPS Preview/_Personnalisé",NULL,GS_CMD, IDM_MAKEEPSU, NULL },
  { "/Edition/_Extraire du fichier EPS",   NULL,   NULL, 	IDM_EXTEPSMENU, "<Branch>" },
  { "/Edition/Extraire du fichier EPS/la partie _PostScript", NULL,		GS_CMD, IDM_EXTRACTPS, NULL },
  { "/Edition/Extraire du fichier EPS/la partie _aperçu", NULL,			GS_CMD, IDM_EXTRACTPRE, NULL },
  { "/Edition/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Edition/Mesure", NULL,				GS_CMD, IDM_MEASURE, NULL },
  { "/Edition/Conversion vers un format vectoriel...", NULL,		GS_CMD, IDM_PSTOEDIT, NULL },
  { "/Edition/Extraire le _texte...", NULL,			GS_CMD, IDM_TEXTEXTRACT, NULL },
  { "/Edition/_Rechercher...", "<control>R",			GS_CMD, IDM_TEXTFIND, NULL },
  { "/Edition/Rechercher _suivant", "<control>G",			GS_CMD, IDM_TEXTFINDNEXT, NULL },

  { "/_Options",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Options/Configuration standard...", 	NULL,		GS_CMD, IDM_CFG, NULL },
  { "/Options/Configuration avancée...", 	NULL,		GS_CMD, IDM_GSCOMMAND, NULL },
  { "/Options/_Sons...", 		NULL,		GS_CMD, IDM_SOUNDS, NULL },
  { "/Options/_Unités",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Options/Unités/_pt", 		NULL,		GS_CMD, IDM_UNITPT, "<CheckItem>" },
  { "/Options/Unités/_mm", 		NULL,		GS_CMD, IDM_UNITMM, "<CheckItem>" },
  { "/Options/Unités/p_ouce", 		NULL,		GS_CMD, IDM_UNITINCH, "<CheckItem>" },
  { "/Options/Unités/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Options/Unités/_Résolution fine", 	NULL,		GS_CMD, IDM_UNITFINE, "<CheckItem>" },
  { "/Options/_Langue",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Options/Langue/_English", 	NULL,		GS_CMD, IDM_LANGEN, "<CheckItem>" },
  { "/Options/Langue/Catalan", 	NULL,		GS_CMD, IDM_LANGCT, "<CheckItem>" },
  { "/Options/Langue/_Deutsch", 	NULL,		GS_CMD, IDM_LANGDE, "<CheckItem>" },
  { "/Options/Langue/_Greek", 		NULL,		GS_CMD, IDM_LANGGR, "<CheckItem>" },
  { "/Options/Langue/E_spañol", 	NULL,		GS_CMD, IDM_LANGES, "<CheckItem>" },
  { "/Options/Langue/_Français", 	NULL,		GS_CMD, IDM_LANGFR, "<CheckItem>" },
  { "/Options/Langue/_Italiano", 	NULL,		GS_CMD, IDM_LANGIT, "<CheckItem>" },
  { "/Options/Langue/_Nederlands", 	NULL,		GS_CMD, IDM_LANGNL, "<CheckItem>" },
  { "/Options/Langue/Russian", 	NULL,		GS_CMD, IDM_LANGRU, "<CheckItem>" },
  { "/Options/Langue/Slovak", 	NULL,		GS_CMD, IDM_LANGSK, "<CheckItem>" },
  { "/Options/Langue/Svenska", 	NULL,		GS_CMD, IDM_LANGSE, "<CheckItem>" },
  { "/Options/PS vers _texte",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Options/PS vers _texte/Désactivé", 	NULL,		GS_CMD, IDM_PSTOTEXTDIS, "<CheckItem>" },
  { "/Options/PS vers _texte/Normal", 	NULL,		GS_CMD, IDM_PSTOTEXTNORM, "<CheckItem>" },
  { "/Options/PS vers _texte/Encodage Dvips Cork", 	NULL,	GS_CMD, IDM_PSTOTEXTCORK, "<CheckItem>" },
  { "/Options/DSC Warnings",      		NULL,   NULL, 		0, "<Branch>" }, /* ENGLISH */
  { "/Options/DSC Warnings/Off", 	NULL,		GS_CMD, IDM_DSC_OFF, "<CheckItem>" }, /* ENGLISH */
  { "/Options/DSC Warnings/Errors", 	NULL,		GS_CMD, IDM_DSC_ERROR, "<CheckItem>" }, /* ENGLISH */
  { "/Options/DSC Warnings/Warnings", 	NULL,		GS_CMD, IDM_DSC_WARN, "<CheckItem>" }, /* ENGLISH */
  { "/Options/DSC Warnings/All", 	NULL,		GS_CMD, IDM_DSC_INFO, "<CheckItem>" }, /* ENGLISH */
  { "/Options/Enregistrer les options _maintenant", 	NULL,		GS_CMD, IDM_SETTINGS, NULL },
  { "/Options/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Options/Enregistrer les options en _quittant", 	NULL,		GS_CMD, IDM_SAVESETTINGS, "<CheckItem>" },
  { "/Options/Protection des fichiers", 			NULL,		GS_CMD, IDM_SAFER, "<CheckItem>" },
  { "/Options/Enregistrer le _dernier répertoire", 	NULL,		GS_CMD, IDM_SAVEDIR, "<CheckItem>" },
  { "/Options/Afficher la _barre d'outils", 		NULL,		GS_CMD, IDM_BUTTONSHOW, "<CheckItem>" },
/*  { "/Options/Ajuster la fe_nêtre à la page", 	NULL,		GS_CMD, IDM_FITPAGE, "<CheckItem>" }, */
/*  { "/Options/Ouverture _rapide", 		NULL,		GS_CMD, IDM_QUICK_OPEN, "<CheckItem>" }, */
  { "/Options/Réaffichage _automatique", 	NULL,		GS_CMD, IDM_AUTOREDISPLAY, "<CheckItem>" },
  { "/Options/Ajuster l'affichage des _fichiers EPS", 		NULL,		GS_CMD, IDM_EPSFCLIP, "<CheckItem>" },
  { "/Options/A_vertissements EPS", 		NULL,		GS_CMD, IDM_EPSFWARN, "<CheckItem>" },
  { "/Options/_Ignorer les commentaires DSC", 		NULL,		GS_CMD, IDM_IGNOREDSC, "<CheckItem>" },
  { "/Options/Affic_her le cadre limite", 	NULL,		GS_CMD, IDM_SHOWBBOX, "<CheckItem>" },

  { "/_Affichage",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Affichage/Page _suivante", 	"+",	GS_CMD, 	IDM_NEXT, NULL },
  { "/Affichage/Page _précédente", 	"-",	GS_CMD, 	IDM_PREV, NULL },
  { "/Affichage/_Atteindre la page...", 	"G",	GS_CMD, 	IDM_GOTO, NULL },
  { "/Affichage/Re_tour", 		"B",	GS_CMD, 	IDM_GOBACK, NULL },
  { "/Affichage/A_vance",	NULL,	GS_CMD, 	IDM_GOFWD, NULL },
  { "/Affichage/_Réafficher", 	"R",	GS_CMD, 	IDM_REDISPLAY, NULL },
  { "/Affichage/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Affichage/Haut de la page suivante", " ",	GS_CMD, 	IDM_NEXTHOME, NULL },
  { "/Affichage/Haut de la page précédente", "<control>H",GS_CMD, 	IDM_PREVHOME, NULL },
  { "/Affichage/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Affichage/P_lein écran", 	NULL,	GS_CMD, 	IDM_FULLSCREEN, NULL },
  { "/Affichage/S'adapter à la fenêtre", 	NULL,	GS_CMD, 	IDM_FITWIN, NULL },

  { "/Orientatio_n",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Orientation/_Automatique", 		NULL,	GS_CMD, 	IDM_AUTOORIENT, "<CheckItem>" },
  { "/Orientation/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orientation/_Portrait", 		NULL,	GS_CMD, 	IDM_PORTRAIT, "<CheckItem>" },
  { "/Orientation/Pa_ysage", 		NULL,	GS_CMD, 	IDM_LANDSCAPE, "<CheckItem>" },
  { "/Orientation/_Renversé", 	NULL,	GS_CMD, 	IDM_UPSIDEDOWN, "<CheckItem>" },
  { "/Orientation/Pa_norama", 		NULL,	GS_CMD, 	IDM_SEASCAPE, "<CheckItem>" },
  { "/Orientation/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orientation/Pay_sage renversé", 	NULL,	GS_CMD, 	IDM_SWAPLANDSCAPE, "<CheckItem>" },

  { "/Forma_t", 			NULL,   NULL, 		0, "<Branch>" },
  { "/Format/Paramètres de l'_affichage...", NULL,GS_CMD, 	IDM_DISPLAYSETTINGS, NULL },
  { "/Format/sep1", 		NULL,   NULL, 		0, "<Separator>" },
  { "/Format/Format _inversé", 	NULL,	GS_CMD, 	IDM_MEDIAROTATE, "<CheckItem>"},
  { "/Format/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Format/11x17", 		NULL,	GS_CMD, 	IDM_11x17, "<CheckItem>" },
  { "/Format/A3", 		NULL,	GS_CMD, 	IDM_A3, "<CheckItem>" },
  { "/Format/A4", 		NULL,	GS_CMD, 	IDM_A4, "<CheckItem>" },
  { "/Format/A5", 		NULL,	GS_CMD, 	IDM_A5, "<CheckItem>" },
  { "/Format/B4", 		NULL,	GS_CMD, 	IDM_B4, "<CheckItem>" },
  { "/Format/B5", 		NULL,	GS_CMD, 	IDM_B5, "<CheckItem>" },
  { "/Format/Ledger", 		NULL,	GS_CMD, 	IDM_LEDGER, "<CheckItem>" },
  { "/Format/Legal", 		NULL,	GS_CMD, 	IDM_LEGAL, "<CheckItem>" },
  { "/Format/Letter", 		NULL,	GS_CMD, 	IDM_LETTER, "<CheckItem>" },
  { "/Format/Note", 		NULL,	GS_CMD, 	IDM_NOTE, "<CheckItem>" },
  { "/Format/_Personnalisé...", 	NULL,	GS_CMD, 	IDM_USERSIZE, "<CheckItem>"},

  { "/A_ide",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Aide/_Index", 		NULL,	GS_CMD, 	IDM_HELPCONTENT, NULL },
  { "/Aide/_Rechercher de l'aide sur...", NULL,	GS_CMD, 	IDM_HELPSEARCH, NULL },
  { "/Aide/Raccourcis _clavier", 	NULL,	GS_CMD, 	IDM_HELPKEYS, NULL },
  { "/Aide/sep1",		NULL,   NULL, 		0, "<Separator>" },
  { "/Aide/_Register...", 	NULL,	GS_CMD, 	IDM_REGISTER, NULL },
  { "/Aide/_A propos de...", 		NULL,	GS_CMD, 	IDM_ABOUT, NULL },
};

int menu_fr_len = sizeof (menu_fr) / sizeof (menu_fr[0]);

