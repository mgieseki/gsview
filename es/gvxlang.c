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
/* es/gvxlang.c */

#include "gvx.h"
#include "gvxres.h"
#include "gvxlang.h"
#include "gvclang.h"

/* string table ids must be in order since we use a binary search */
STRING_ENTRY string_es[] = {
#include "gvclang.rc"
#include "gvxlangh.rc"
};

int string_es_len = sizeof(string_es)/sizeof(STRING_ENTRY);

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

/* All the menu items from Windows GSview */
/* Not all of these are relevant */
MENU_ENTRY menu_es[] = {
  { "/_Archivo", 			NULL, 	NULL, 	IDM_FILEMENU, "<Branch>" },
  { "/Archivo/_Abrir...",     	"O", 	GS_CMD,	IDM_OPEN, NULL },
  { "/Archivo/Se_leccionar archivo...",    "S", 	GS_CMD,	IDM_SELECT, NULL },
  { "/Archivo/_Guardar como...",     	"A", 	GS_CMD,	IDM_SAVEAS, NULL },
  { "/Archivo/_Cerrar",     	"C", 	GS_CMD,	IDM_CLOSE, NULL },
  { "/Archivo/I_nformación...",     	"I", 	GS_CMD,	IDM_INFO, NULL },
  { "/Archivo/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Archivo/C_onvertir...",     	NULL, 	GS_CMD,	IDM_CONVERTFILE, NULL },
  { "/Archivo/_Extraer...",     	"E", 	GS_CMD,	IDM_EXTRACT, NULL },
  { "/Archivo/PS a EPS",     	NULL, 	GS_CMD,	IDM_PSTOEPS, NULL },
  { "/Archivo/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Archivo/_Imprimir...",     	"P", 	GS_CMD,	IDM_PRINT, NULL },
  { "/Archivo/Imprimir _un archivo...",     	NULL, 	GS_CMD,	IDM_SPOOL, NULL },
  { "/Archivo/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Archivo/Mostrar _mensajes",     	"M", 	GS_CMD,	IDM_GSMESS, NULL },
  { "/Archivo/sep4",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Archivo/1LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE1, NULL },
  { "/Archivo/2LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE2, NULL },
  { "/Archivo/3LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE3, NULL },
  { "/Archivo/4LASTFILE",     	NULL, 	GS_CMD,	IDM_LASTFILE4, NULL },
  { "/Archivo/sep5",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Archivo/_Salir",     		NULL, 	GS_CMD,	IDM_EXIT, NULL },

  { "/_Editar",      		NULL,   NULL, 	IDM_EDITMENU, "<Branch>" },
/*
  { "/Editar/_Copiar",     		"<control>C", GS_CMD, IDM_COPYCLIP, NULL },
  { "/Editar/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Editar/Convertir a _Bitmap",    NULL,	GS_CMD,	IDM_CONVERT, NULL },
*/
  { "/Editar/Save Pixmap as BMP",     	NULL,	GS_CMD,	IDM_PASTETO, NULL }, /* ENGLISH */
  { "/Editar/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Editar/_Añadir vista preliminar a EPS",   NULL,   NULL, 		0, "<Branch>" },
  { "/Editar/Añadir vista preliminar a EPS/_Interchange", NULL,		GS_CMD, IDM_MAKEEPSI, NULL },
  { "/Editar/Añadir vista preliminar a EPS/_TIFF 4", NULL,		GS_CMD, IDM_MAKEEPST4, NULL },
  { "/Editar/Añadir vista preliminar a EPS/TIFF 6 _no comprimido", NULL,	GS_CMD, IDM_MAKEEPST6U, NULL },
  { "/Editar/Añadir vista preliminar a EPS/TIFF 6 _comprimido", NULL,	GS_CMD, IDM_MAKEEPST6P, NULL },
  { "/Editar/Añadir vista preliminar a EPS/_Windows Metafile", NULL,	GS_CMD, IDM_MAKEEPSW, NULL },
  { "/Editar/Añadir vista preliminar a EPS/_Pegar desde un archivo",NULL,GS_CMD, IDM_MAKEEPSU, NULL },
  { "/Editar/_Extraer del archivo EPS",   NULL,   NULL, 	IDM_EXTEPSMENU, "<Branch>" },
  { "/Editar/Extraer del archivo EPS/La parte _PostScript", NULL,		GS_CMD, IDM_EXTRACTPS, NULL },
  { "/Editar/Extraer del archivo EPS/La vista preliminar", NULL,			GS_CMD, IDM_EXTRACTPRE, NULL },
  { "/Editar/sep3",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Editar/Medir", NULL,				GS_CMD, IDM_MEASURE, NULL },
  { "/Editar/Convertir a formato vectorial...", NULL,		GS_CMD, IDM_PSTOEDIT, NULL },
  { "/Editar/Extraer el _texto...", NULL,			GS_CMD, IDM_TEXTEXTRACT, NULL },
  { "/Editar/_Buscar...", "<control>F",			GS_CMD, IDM_TEXTFIND, NULL },
  { "/Editar/Buscar _siguiente", "<control>G",			GS_CMD, IDM_TEXTFINDNEXT, NULL },

  { "/O_pciones",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opciones/Configuración &sencilla...", 	NULL,		GS_CMD, IDM_CFG, NULL },
  { "/Opciones/Configuración &avanzada...", 	NULL,		GS_CMD, IDM_GSCOMMAND, NULL },
  { "/Opciones/Sonidos...", 		NULL,		GS_CMD, IDM_SOUNDS, NULL },
  { "/Opciones/_Unidades",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opciones/Unidades/_pt (punto)", 		NULL,		GS_CMD, IDM_UNITPT, "<CheckItem>" },
  { "/Opciones/Unidades/_mm (milímetro)", 		NULL,		GS_CMD, IDM_UNITMM, "<CheckItem>" },
  { "/Opciones/Unidades/_inch (pulgada)", 		NULL,		GS_CMD, IDM_UNITINCH, "<CheckItem>" },
  { "/Opciones/Unidades/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Opciones/Unidades/Resolución &fina", 	NULL,		GS_CMD, IDM_UNITFINE, "<CheckItem>" },
  { "/Opciones/_Idioma",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opciones/Idioma/_English", 	NULL,		GS_CMD, IDM_LANGEN, "<CheckItem>" },
  { "/Opciones/Idioma/Catalan", 	NULL,		GS_CMD, IDM_LANGCT, "<CheckItem>" },
  { "/Opciones/Idioma/_Deutsch", 	NULL,		GS_CMD, IDM_LANGDE, "<CheckItem>" },
  { "/Opciones/Idioma/_Greek", 		NULL,		GS_CMD, IDM_LANGGR, "<CheckItem>" },
  { "/Opciones/Idioma/E_spañol", 	NULL,		GS_CMD, IDM_LANGES, "<CheckItem>" },
  { "/Opciones/Idioma/_Français", 	NULL,		GS_CMD, IDM_LANGFR, "<CheckItem>" },
  { "/Opciones/Idioma/_Italiano", 	NULL,		GS_CMD, IDM_LANGIT, "<CheckItem>" },
  { "/Opciones/Idioma/_Nederlands", 	NULL,		GS_CMD, IDM_LANGNL, "<CheckItem>" },
  { "/Opciones/Idioma/Russian", 	NULL,		GS_CMD, IDM_LANGRU, "<CheckItem>" },
  { "/Opciones/Idioma/Slovak", 	NULL,		GS_CMD, IDM_LANGSK, "<CheckItem>" },
  { "/Opciones/Idioma/Svenska", 	NULL,		GS_CMD, IDM_LANGSE, "<CheckItem>" },
  { "/Opciones/Conversión a _texto",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opciones/Conversión a texto/Desactivado", 	NULL,		GS_CMD, IDM_PSTOTEXTDIS, "<CheckItem>" },
  { "/Opciones/Conversión a texto/Normal", 	NULL,		GS_CMD, IDM_PSTOTEXTNORM, "<CheckItem>" },
  { "/Opciones/Conversión a texto/Página de códigos Dvips Cork", 	NULL,	GS_CMD, IDM_PSTOTEXTCORK, "<CheckItem>" },
  { "/Opciones/Mensajes DSC",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Opciones/Mensajes DSC/Desactivar", 	NULL,		GS_CMD, IDM_DSC_OFF, "<CheckItem>" },
  { "/Opciones/Mensajes DSC/Mostrar errores", 	NULL,		GS_CMD, IDM_DSC_ERROR, "<CheckItem>" },
  { "/Opciones/Mensajes DSC/Mostrar precauciones", 	NULL,		GS_CMD, IDM_DSC_WARN, "<CheckItem>" },
  { "/Opciones/Mensajes DSC/Mostrar todos", 	NULL,		GS_CMD, IDM_DSC_INFO, "<CheckItem>" },
  { "/Opciones/_Guardar opciones ahora", 	NULL,		GS_CMD, IDM_SETTINGS, NULL },
  { "/Opciones/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Opciones/Guardar opciones al _salir", 	NULL,		GS_CMD, IDM_SAVESETTINGS, "<CheckItem>" },
  { "/Opciones/_Proteger los ficheros", 			NULL,		GS_CMD, IDM_SAFER, "<CheckItem>" },
  { "/Opciones/Guardar el último _directorio", 	NULL,		GS_CMD, IDM_SAVEDIR, "<CheckItem>" },
  { "/Opciones/_Mostrar barra de botones", 		NULL,		GS_CMD, IDM_BUTTONSHOW, "<CheckItem>" },
/*  { "/Opciones/A_justar la ventana a la página", 	NULL,		GS_CMD, IDM_FITPAGE, "<CheckItem>" }, */
/*  { "/Opciones/Apertura _rápida", 		NULL,		GS_CMD, IDM_QUICK_OPEN, "<CheckItem>" }, */
  { "/Opciones/Actualización automática", 	NULL,		GS_CMD, IDM_AUTOREDISPLAY, "<CheckItem>" },
  { "/Opciones/Re_cortar EPS", 		NULL,		GS_CMD, IDM_EPSFCLIP, "<CheckItem>" },
  { "/Opciones/A_visos EPS", 		NULL,		GS_CMD, IDM_EPSFWARN, "<CheckItem>" },
  { "/Opciones/_Ignorar los comentarios DSC", 		NULL,		GS_CMD, IDM_IGNOREDSC, "<CheckItem>" },
  { "/Opciones/Mostrar _BoundingBox", 	NULL,		GS_CMD, IDM_SHOWBBOX, "<CheckItem>" },

  { "/_Ver",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Ver/Página si_guiente", 	"+",	GS_CMD, 	IDM_NEXT, NULL },
  { "/Ver/Página a_nterior", 	"-",	GS_CMD, 	IDM_PREV, NULL },
  { "/Ver/Ir a la _página...", 	"G",	GS_CMD, 	IDM_GOTO, NULL },
  { "/Ver/Vista _anterior", 		"B",	GS_CMD, 	IDM_GOBACK, NULL },
  { "/Ver/Vista _siguiente",	NULL,	GS_CMD, 	IDM_GOFWD, NULL },
  { "/Ver/Ac_tualizar", 	"R",	GS_CMD, 	IDM_REDISPLAY, NULL },
  { "/Ver/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Ver/Cabecera de página siguiente", " ",	GS_CMD, 	IDM_NEXTHOME, NULL },
  { "/Ver/Cabecera de página anterior", "<control>H",GS_CMD, 	IDM_PREVHOME, NULL },
  { "/Ver/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Ver/Pantalla _completa", 	NULL,	GS_CMD, 	IDM_FULLSCREEN, NULL },
  { "/Ver/Ajustar a la _ventana", 	NULL,	GS_CMD, 	IDM_FITWIN, NULL },

  { "/_Orientación",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Orientación/_Automática", 		NULL,	GS_CMD, 	IDM_AUTOORIENT, "<CheckItem>" },
  { "/Orientación/sep1",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orientación/_Normal", 		NULL,	GS_CMD, 	IDM_PORTRAIT, "<CheckItem>" },
  { "/Orientación/Apaisada a _izquierda", 		NULL,	GS_CMD, 	IDM_LANDSCAPE, "<CheckItem>" },
  { "/Orientación/_Inversa", 	NULL,	GS_CMD, 	IDM_UPSIDEDOWN, "<CheckItem>" },
  { "/Orientación/Apaisada a _derecha", 		NULL,	GS_CMD, 	IDM_SEASCAPE, "<CheckItem>" },
  { "/Orientación/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Orientación/In_vierte apaisado", 	NULL,	GS_CMD, 	IDM_SWAPLANDSCAPE, "<CheckItem>" },

  { "/_Formatos", 			NULL,   NULL, 		0, "<Branch>" },
  { "/Formatos/_Parámetros de pantalla...", NULL,GS_CMD, 	IDM_DISPLAYSETTINGS, NULL },
  { "/Formatos/sep1", 		NULL,   NULL, 		0, "<Separator>" },
  { "/Formatos/Intercambiar dimensiones", 	NULL,	GS_CMD, 	IDM_MEDIAROTATE, "<CheckItem>"},
  { "/Formatos/sep2",     		NULL,   NULL, 		0, "<Separator>" },
  { "/Formatos/11x17", 		NULL,	GS_CMD, 	IDM_11x17, "<CheckItem>" },
  { "/Formatos/A3", 		NULL,	GS_CMD, 	IDM_A3, "<CheckItem>" },
  { "/Formatos/A4", 		NULL,	GS_CMD, 	IDM_A4, "<CheckItem>" },
  { "/Formatos/A5", 		NULL,	GS_CMD, 	IDM_A5, "<CheckItem>" },
  { "/Formatos/B4", 		NULL,	GS_CMD, 	IDM_B4, "<CheckItem>" },
  { "/Formatos/B5", 		NULL,	GS_CMD, 	IDM_B5, "<CheckItem>" },
  { "/Formatos/Ledger", 		NULL,	GS_CMD, 	IDM_LEDGER, "<CheckItem>" },
  { "/Formatos/Legal", 		NULL,	GS_CMD, 	IDM_LEGAL, "<CheckItem>" },
  { "/Formatos/Letter", 		NULL,	GS_CMD, 	IDM_LETTER, "<CheckItem>" },
  { "/Formatos/Note", 		NULL,	GS_CMD, 	IDM_NOTE, "<CheckItem>" },
  { "/Formatos/Personalizado...", 	NULL,	GS_CMD, 	IDM_USERSIZE, "<CheckItem>"},

  { "/_Ayuda",      		NULL,   NULL, 		0, "<Branch>" },
  { "/Ayuda/_Índice", 		NULL,	GS_CMD, 	IDM_HELPCONTENT, NULL },
  { "/Ayuda/_Buscar ayuda sobre...", NULL,	GS_CMD, 	IDM_HELPSEARCH, NULL },
  { "/Ayuda/Los atajos del _teclado", 	NULL,	GS_CMD, 	IDM_HELPKEYS, NULL },
  { "/Ayuda/sep1",		NULL,   NULL, 		0, "<Separator>" },
  { "/Ayuda/_Registro...", 	NULL,	GS_CMD, 	IDM_REGISTER, NULL },
  { "/Ayuda/_Acerca de...", 		NULL,	GS_CMD, 	IDM_ABOUT, NULL },
};

int menu_es_len = sizeof (menu_es) / sizeof (menu_es[0]);

