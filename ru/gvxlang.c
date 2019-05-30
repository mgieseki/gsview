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
/* ru/gvxlang.c */

#include "gvx.h"
#include "gvxres.h"
#include "gvxlang.h"
#include "gvclang.h"

/* string table ids must be in order since we use a binary search */
STRING_ENTRY string_ru[] = {
#include "gvclang.rc"
#include "gvxlangh.rc"
};

int string_ru_len = sizeof(string_ru)/sizeof(STRING_ENTRY);

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

MENU_ENTRY menu_ru[] = {
  { "/_����",           NULL,   NULL,   IDM_FILEMENU, "<Branch>" },
  { "/����/_�������...",       "O",    GS_CMD, IDM_OPEN, NULL },
  { "/����/_������� ����...",    "S",    GS_CMD, IDM_SELECT, NULL },
  { "/����/_��������� ���...",        "A",    GS_CMD, IDM_SAVEAS, NULL },
  { "/����/_�������\tC",         "C",    GS_CMD, IDM_CLOSE, NULL },
  { "/����/_����������...",       "I",    GS_CMD, IDM_INFO, NULL },
  { "/����/sep1",           NULL,   NULL,       0, "<Separator>" },
  { "/����/_��������������...",        NULL,   GS_CMD, IDM_CONVERTFILE, NULL },
  { "/����/�_������...",        "E",    GS_CMD, IDM_EXTRACT, NULL },
  { "/����/_PS � EPS",      NULL,   GS_CMD, IDM_PSTOEPS, NULL },
  { "/����/sep2",           NULL,   NULL,       0, "<Separator>" },
  { "/����/_������...",      "P",    GS_CMD, IDM_PRINT, NULL },
  { "/����/�������� _����...",      NULL,   GS_CMD, IDM_SPOOL, NULL },
  { "/����/sep3",           NULL,   NULL,       0, "<Separator>" },
  { "/����/���_������ Ghostscript...",      "M",    GS_CMD, IDM_GSMESS, NULL },
  { "/����/sep4",           NULL,   NULL,       0, "<Separator>" },
  { "/����/1LASTFILE",      NULL,   GS_CMD, IDM_LASTFILE1, NULL },
  { "/����/2LASTFILE",      NULL,   GS_CMD, IDM_LASTFILE2, NULL },
  { "/����/3LASTFILE",      NULL,   GS_CMD, IDM_LASTFILE3, NULL },
  { "/����/4LASTFILE",      NULL,   GS_CMD, IDM_LASTFILE4, NULL },
  { "/����/sep5",           NULL,   NULL,       0, "<Separator>" },
  { "/����/_�����",          NULL,   GS_CMD, IDM_EXIT, NULL },

  { "/_������",           NULL,   NULL,   IDM_EDITMENU, "<Branch>" },
/*
  { "/������/_����������",          "<control>C", GS_CMD, IDM_COPYCLIP, NULL },
  { "/������/sep1",           NULL,   NULL,       0, "<Separator>" },
  { "/������/�������������� _��������� �����",    NULL,   GS_CMD, IDM_CONVERT, NULL },
*/
  { "/������/��������� �������� ��� BMP",      NULL,   GS_CMD, IDM_PASTETO, NULL },
  { "/������/sep2",           NULL,   NULL,       0, "<Separator>" },
  { "/������/_�������� EPS ������",   NULL,   NULL,       0, "<Branch>" },
  { "/������/�������� EPS ������/_Interchange", NULL,     GS_CMD, IDM_MAKEEPSI, NULL },
  { "/������/�������� EPS ������/TIFF _4", NULL,       GS_CMD, IDM_MAKEEPST4, NULL },
  { "/������/�������� EPS ������/_TIFF 6 ��� ������", NULL, GS_CMD, IDM_MAKEEPST6U, NULL },
  { "/������/�������� EPS ������/TIFF 6 &packbits", NULL, GS_CMD, IDM_MAKEEPST6P, NULL },
  { "/������/�������� EPS ������/�������� _Windows", NULL,    GS_CMD, IDM_MAKEEPSW, NULL },
  { "/������/�������� EPS ������/_���������������� ������",NULL,GS_CMD, IDM_MAKEEPSU, NULL },
  { "/������/_������� �� EPS",   NULL,   NULL,   IDM_EXTEPSMENU, "<Branch>" },
  { "/������/������� �� EPS/_PostScript", NULL,      GS_CMD, IDM_EXTRACTPS, NULL },
  { "/������/������� �� EPS/_������", NULL,         GS_CMD, IDM_EXTRACTPRE, NULL },
  { "/������/sep3",           NULL,   NULL,       0, "<Separator>" },
  { "/������/�_��������", NULL,              GS_CMD, IDM_MEASURE, NULL },
  { "/������/�_������������� � ��������� ������...", NULL,      GS_CMD, IDM_PSTOEDIT, NULL },
  { "/������/������� _�����...", NULL,         GS_CMD, IDM_TEXTEXTRACT, NULL },
  { "/������/_�����...", "<control>F",         GS_CMD, IDM_TEXTFIND, NULL },
/*
  { "/������/Find _Next", "<control>G",           GS_CMD, IDM_TEXTFINDNEXT, NULL },
*/
  { "/������/����� _�����", "F3",           GS_CMD, IDM_TEXTFINDNEXT, NULL },

  { "/_���������",            NULL,   NULL,       0, "<Branch>" },
  { "/���������/������� ���������...",   NULL,       GS_CMD, IDM_CFG, NULL },
  { "/���������/����������� ���������...",   NULL,       GS_CMD, IDM_GSCOMMAND, NULL },
  { "/���������/�����...",       NULL,       GS_CMD, IDM_SOUNDS, NULL },
  { "/���������/_������� ���������",              NULL,   NULL,       0, "<Branch>" },
  { "/���������/������� ���������/_������",       NULL,       GS_CMD, IDM_UNITPT, "<CheckItem>" },
  { "/���������/������� ���������/_��",       NULL,       GS_CMD, IDM_UNITMM, "<CheckItem>" },
  { "/���������/������� ���������/_�����",         NULL,       GS_CMD, IDM_UNITINCH, "<CheckItem>" },
  { "/���������/������� ���������/sep1",          NULL,   NULL,       0, "<Separator>" },
  { "/���������/������� ���������/_� ������� ���������",  NULL,       GS_CMD, IDM_UNITFINE, "<CheckItem>" },
  { "/���������/_����",           NULL,   NULL,       0, "<Branch>" },
  { "/���������/����/_English",   NULL,       GS_CMD, IDM_LANGEN, "<CheckItem>" },
  { "/���������/����/Catalan",    NULL,       GS_CMD, IDM_LANGCT, "<CheckItem>" },
  { "/���������/����/_Deutsch",   NULL,       GS_CMD, IDM_LANGDE, "<CheckItem>" },
  { "/���������/����/_Greek",     NULL,       GS_CMD, IDM_LANGGR, "<CheckItem>" },
  { "/���������/����/E_spa�ol",   NULL,       GS_CMD, IDM_LANGES, "<CheckItem>" },
  { "/���������/����/_Fran�ais",  NULL,       GS_CMD, IDM_LANGFR, "<CheckItem>" },
  { "/���������/����/_Italiano",  NULL,       GS_CMD, IDM_LANGIT, "<CheckItem>" },
  { "/���������/����/_Nederlands",    NULL,       GS_CMD, IDM_LANGNL, "<CheckItem>" },
  { "/���������/����/Russian",    NULL,       GS_CMD, IDM_LANGRU, "<CheckItem>" },
  { "/���������/����/Slovak",     NULL,       GS_CMD, IDM_LANGSK, "<CheckItem>" },
  { "/���������/����/Svenska",    NULL,       GS_CMD, IDM_LANGSE, "<CheckItem>" },
  { "/���������/PStoText",            NULL,   NULL,       0, "<Branch>" },
  { "/���������/PStoText/��������",   NULL,       GS_CMD, IDM_PSTOTEXTDIS, "<CheckItem>" },
  { "/���������/PStoText/����������",     NULL,       GS_CMD, IDM_PSTOTEXTNORM, "<CheckItem>" },
  { "/���������/PStoText/Dvips Cork Encoding",    NULL,   GS_CMD, IDM_PSTOTEXTCORK, "<CheckItem>" },
  { "/���������/�������������� DSC",            NULL,   NULL,       0, "<Branch>" },
  { "/���������/�������������� DSC/���������",    NULL,       GS_CMD, IDM_DSC_OFF, "<CheckItem>" },
  { "/���������/�������������� DSC/������",     NULL,       GS_CMD, IDM_DSC_ERROR, "<CheckItem>" },
  { "/���������/�������������� DSC/��������������",   NULL,       GS_CMD, IDM_DSC_WARN, "<CheckItem>" },
  { "/���������/�������������� DSC/���",    NULL,       GS_CMD, IDM_DSC_INFO, "<CheckItem>" },
  { "/���������/��������� _���������",  NULL,       GS_CMD, IDM_SETTINGS, NULL },
  { "/���������/sep1",            NULL,   NULL,       0, "<Separator>" },
  { "/���������/��������� ��������� ��� _������",  NULL,       GS_CMD, IDM_SAVESETTINGS, "<CheckItem>" },
  { "/���������/_���������� �����",          NULL,       GS_CMD, IDM_SAFER, "<CheckItem>" },
  { "/���������/���������� ��������� _�������",    NULL,       GS_CMD, IDM_SAVEDIR, "<CheckItem>" },
  { "/���������/�_����� ������",         NULL,       GS_CMD, IDM_BUTTONSHOW, "<CheckItem>" },
/*  { "/���������/�_��� �� ������� ��������",   NULL,       GS_CMD, IDM_FITPAGE, "<CheckItem>" }, */
/*  { "/���������/������� ��������",        NULL,       GS_CMD, IDM_QUICK_OPEN, "<CheckItem>" }, */
  { "/���������/_�������������� ����������",     NULL,       GS_CMD, IDM_AUTOREDISPLAY, "<CheckItem>" },
  { "/���������/���_���� EPS",       NULL,       GS_CMD, IDM_EPSFCLIP, "<CheckItem>" },
  { "/���������/�������������� _EPS",       NULL,       GS_CMD, IDM_EPSFWARN, "<CheckItem>" },
  { "/���������/������������ ����������� _DSC",         NULL,       GS_CMD, IDM_IGNOREDSC, "<CheckItem>" },
  { "/���������/���������� ���������� ���_����������",  NULL,       GS_CMD, IDM_SHOWBBOX, "<CheckItem>" },

  { "/�_�������",           NULL,   NULL,       0, "<Branch>" },
  { "/��������/_��������� ��������",     "plus", GS_CMD,     IDM_NEXT, NULL },
  { "/��������/_���������� ��������",     "minus",    GS_CMD,     IDM_PREV, NULL },
  { "/��������/�_������ � ��������...",     "G",    GS_CMD,     IDM_GOTO, NULL },
  { "/��������/_�����",       "B",    GS_CMD,     IDM_GOBACK, NULL },
  { "/��������/_�����",    NULL,   GS_CMD,     IDM_GOFWD, NULL },
  { "/��������/_��������",     "R",    GS_CMD,     IDM_REDISPLAY, NULL },
  { "/��������/sep1",           NULL,   NULL,       0, "<Separator>" },
  { "/��������/��������� �������� � �����", "space",    GS_CMD,     IDM_NEXTHOME, NULL },
  { "/��������/���������� �������� � �����", "<control>H",GS_CMD,    IDM_PREVHOME, NULL },
  { "/��������/sep2",           NULL,   NULL,       0, "<Separator>" },
  { "/��������/������������� _�����",   "F4",   GS_CMD,     IDM_FULLSCREEN, NULL },
  { "/��������/�� �_������ ����",    "F6",   GS_CMD,     IDM_FITWIN, NULL },

  { "/_����������",            NULL,   NULL,       0, "<Branch>" },
  { "/����������/_����",       NULL,   GS_CMD,     IDM_AUTOORIENT, "<CheckItem>" },
  { "/����������/sep1",            NULL,   NULL,       0, "<Separator>" },
  { "/����������/_�������",       NULL,   GS_CMD,     IDM_PORTRAIT, "<CheckItem>" },
  { "/����������/�_��������",      NULL,   GS_CMD,     IDM_LANDSCAPE, "<CheckItem>" },
  { "/����������/�_����������� �������",    NULL,   GS_CMD,     IDM_UPSIDEDOWN, "<CheckItem>" },
  { "/����������/_�������� ���������",       NULL,   GS_CMD,     IDM_SEASCAPE, "<CheckItem>" },
  { "/����������/sep2",            NULL,   NULL,       0, "<Separator>" },
  { "/����������/_�������� ���������",     NULL,   GS_CMD,     IDM_SWAPLANDSCAPE, "<CheckItem>" },

  { "/_������",          NULL,   NULL,       0, "<Branch>" },
  { "/������/_��������� ������...", NULL,GS_CMD,     IDM_DISPLAYSETTINGS, NULL },
  { "/������/sep1",      NULL,   NULL,       0, "<Separator>" },
  { "/������/_��������� ����",     NULL,   GS_CMD,     IDM_MEDIAROTATE, "<CheckItem>"},
  { "/������/sep2",          NULL,   NULL,       0, "<Separator>" },
  { "/������/11x17",         NULL,   GS_CMD,     IDM_11x17, "<CheckItem>" },
  { "/������/A3",        NULL,   GS_CMD,     IDM_A3, "<CheckItem>" },
  { "/������/A4",        NULL,   GS_CMD,     IDM_A4, "<CheckItem>" },
  { "/������/A5",        NULL,   GS_CMD,     IDM_A5, "<CheckItem>" },
  { "/������/B4",        NULL,   GS_CMD,     IDM_B4, "<CheckItem>" },
  { "/������/B5",        NULL,   GS_CMD,     IDM_B5, "<CheckItem>" },
  { "/������/Ledger",        NULL,   GS_CMD,     IDM_LEDGER, "<CheckItem>" },
  { "/������/Legal",         NULL,   GS_CMD,     IDM_LEGAL, "<CheckItem>" },
  { "/������/Letter",        NULL,   GS_CMD,     IDM_LETTER, "<CheckItem>" },
  { "/������/Note",      NULL,   GS_CMD,     IDM_NOTE, "<CheckItem>" },
  { "/������/_������...",   NULL,   GS_CMD,     IDM_USERSIZE, "<CheckItem>"},
  { "/������/USERSIZE1",     NULL,   GS_CMD,     IDM_USERSIZE1, "<CheckItem>"},
  { "/������/USERSIZE2",     NULL,   GS_CMD,     IDM_USERSIZE2, "<CheckItem>"},
  { "/������/USERSIZE3",     NULL,   GS_CMD,     IDM_USERSIZE3, "<CheckItem>"},
  { "/������/USERSIZE4",     NULL,   GS_CMD,     IDM_USERSIZE4, "<CheckItem>"},
  { "/������/USERSIZE5",     NULL,   GS_CMD,     IDM_USERSIZE5, "<CheckItem>"},
  { "/������/USERSIZE6",     NULL,   GS_CMD,     IDM_USERSIZE6, "<CheckItem>"},
  { "/������/USERSIZE7",     NULL,   GS_CMD,     IDM_USERSIZE7, "<CheckItem>"},
  { "/������/USERSIZE8",     NULL,   GS_CMD,     IDM_USERSIZE8, "<CheckItem>"},
  { "/������/USERSIZE9",     NULL,   GS_CMD,     IDM_USERSIZE9, "<CheckItem>"},
  { "/������/USERSIZE10",    NULL,   GS_CMD,     IDM_USERSIZE10, "<CheckItem>"},
  { "/������/USERSIZE11",    NULL,   GS_CMD,     IDM_USERSIZE11, "<CheckItem>"},
  { "/������/USERSIZE12",    NULL,   GS_CMD,     IDM_USERSIZE12, "<CheckItem>"},
  { "/������/USERSIZE13",    NULL,   GS_CMD,     IDM_USERSIZE13, "<CheckItem>"},

  { "/_�������",           "F1",   NULL,       0, "<Branch>" },
  { "/?/_����������",      NULL,   GS_CMD,     IDM_HELPCONTENT, NULL },
  { "/?/_���������...", NULL,  GS_CMD,     IDM_HELPSEARCH, NULL },
  { "/?/_����������",     NULL,   GS_CMD,     IDM_HELPKEYS, NULL },
  { "/?/sep1",       NULL,   NULL,       0, "<Separator>" },
  { "/?/_�����������...",   NULL,   GS_CMD,     IDM_REGISTER, NULL },
  { "/?/_� ���������...",      NULL,   GS_CMD,     IDM_ABOUT, NULL },
};

int menu_ru_len = sizeof (menu_ru) / sizeof (menu_ru[0]);

