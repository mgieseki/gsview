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
  { "/_Файл",           NULL,   NULL,   IDM_FILEMENU, "<Branch>" },
  { "/Файл/_Открыть...",       "O",    GS_CMD, IDM_OPEN, NULL },
  { "/Файл/_Выбрать файл...",    "S",    GS_CMD, IDM_SELECT, NULL },
  { "/Файл/_Сохранить как...",        "A",    GS_CMD, IDM_SAVEAS, NULL },
  { "/Файл/_Закрыть\tC",         "C",    GS_CMD, IDM_CLOSE, NULL },
  { "/Файл/_Информация...",       "I",    GS_CMD, IDM_INFO, NULL },
  { "/Файл/sep1",           NULL,   NULL,       0, "<Separator>" },
  { "/Файл/_Конвертировать...",        NULL,   GS_CMD, IDM_CONVERTFILE, NULL },
  { "/Файл/И_звлечь...",        "E",    GS_CMD, IDM_EXTRACT, NULL },
  { "/Файл/_PS в EPS",      NULL,   GS_CMD, IDM_PSTOEPS, NULL },
  { "/Файл/sep2",           NULL,   NULL,       0, "<Separator>" },
  { "/Файл/_Печать...",      "P",    GS_CMD, IDM_PRINT, NULL },
  { "/Файл/Печатать _файл...",      NULL,   GS_CMD, IDM_SPOOL, NULL },
  { "/Файл/sep3",           NULL,   NULL,       0, "<Separator>" },
  { "/Файл/Соо_бщения Ghostscript...",      "M",    GS_CMD, IDM_GSMESS, NULL },
  { "/Файл/sep4",           NULL,   NULL,       0, "<Separator>" },
  { "/Файл/1LASTFILE",      NULL,   GS_CMD, IDM_LASTFILE1, NULL },
  { "/Файл/2LASTFILE",      NULL,   GS_CMD, IDM_LASTFILE2, NULL },
  { "/Файл/3LASTFILE",      NULL,   GS_CMD, IDM_LASTFILE3, NULL },
  { "/Файл/4LASTFILE",      NULL,   GS_CMD, IDM_LASTFILE4, NULL },
  { "/Файл/sep5",           NULL,   NULL,       0, "<Separator>" },
  { "/Файл/_Выход",          NULL,   GS_CMD, IDM_EXIT, NULL },

  { "/_Правка",           NULL,   NULL,   IDM_EDITMENU, "<Branch>" },
/*
  { "/Правка/_Копировать",          "<control>C", GS_CMD, IDM_COPYCLIP, NULL },
  { "/Правка/sep1",           NULL,   NULL,       0, "<Separator>" },
  { "/Правка/Конвертировать _бититовый образ",    NULL,   GS_CMD, IDM_CONVERT, NULL },
*/
  { "/Правка/Сохранить картинку как BMP",      NULL,   GS_CMD, IDM_PASTETO, NULL },
  { "/Правка/sep2",           NULL,   NULL,       0, "<Separator>" },
  { "/Правка/_Добавить EPS превью",   NULL,   NULL,       0, "<Branch>" },
  { "/Правка/Добавить EPS превью/_Interchange", NULL,     GS_CMD, IDM_MAKEEPSI, NULL },
  { "/Правка/Добавить EPS превью/TIFF _4", NULL,       GS_CMD, IDM_MAKEEPST4, NULL },
  { "/Правка/Добавить EPS превью/_TIFF 6 без сжатия", NULL, GS_CMD, IDM_MAKEEPST6U, NULL },
  { "/Правка/Добавить EPS превью/TIFF 6 &packbits", NULL, GS_CMD, IDM_MAKEEPST6P, NULL },
  { "/Правка/Добавить EPS превью/Метафайл _Windows", NULL,    GS_CMD, IDM_MAKEEPSW, NULL },
  { "/Правка/Добавить EPS превью/_Пользовательское превью",NULL,GS_CMD, IDM_MAKEEPSU, NULL },
  { "/Правка/_Извлечь из EPS",   NULL,   NULL,   IDM_EXTEPSMENU, "<Branch>" },
  { "/Правка/Извлечь из EPS/_PostScript", NULL,      GS_CMD, IDM_EXTRACTPS, NULL },
  { "/Правка/Извлечь из EPS/_Превью", NULL,         GS_CMD, IDM_EXTRACTPRE, NULL },
  { "/Правка/sep3",           NULL,   NULL,       0, "<Separator>" },
  { "/Правка/И_змерения", NULL,              GS_CMD, IDM_MEASURE, NULL },
  { "/Правка/К_онвертировать в векторный формат...", NULL,      GS_CMD, IDM_PSTOEDIT, NULL },
  { "/Правка/Извлечь _текст...", NULL,         GS_CMD, IDM_TEXTEXTRACT, NULL },
  { "/Правка/_Найти...", "<control>F",         GS_CMD, IDM_TEXTFIND, NULL },
/*
  { "/Правка/Find _Next", "<control>G",           GS_CMD, IDM_TEXTFINDNEXT, NULL },
*/
  { "/Правка/Найти _далее", "F3",           GS_CMD, IDM_TEXTFINDNEXT, NULL },

  { "/_Настройка",            NULL,   NULL,       0, "<Branch>" },
  { "/Настройка/Простая настройка...",   NULL,       GS_CMD, IDM_CFG, NULL },
  { "/Настройка/Расширенная настройка...",   NULL,       GS_CMD, IDM_GSCOMMAND, NULL },
  { "/Настройка/Звуки...",       NULL,       GS_CMD, IDM_SOUNDS, NULL },
  { "/Настройка/_Единицы измерения",              NULL,   NULL,       0, "<Branch>" },
  { "/Настройка/Единицы измерения/_поинты",       NULL,       GS_CMD, IDM_UNITPT, "<CheckItem>" },
  { "/Настройка/Единицы измерения/_мм",       NULL,       GS_CMD, IDM_UNITMM, "<CheckItem>" },
  { "/Настройка/Единицы измерения/_дюймы",         NULL,       GS_CMD, IDM_UNITINCH, "<CheckItem>" },
  { "/Настройка/Единицы измерения/sep1",          NULL,   NULL,       0, "<Separator>" },
  { "/Настройка/Единицы измерения/_С высокой точностью",  NULL,       GS_CMD, IDM_UNITFINE, "<CheckItem>" },
  { "/Настройка/_Язык",           NULL,   NULL,       0, "<Branch>" },
  { "/Настройка/Язык/_English",   NULL,       GS_CMD, IDM_LANGEN, "<CheckItem>" },
  { "/Настройка/Язык/Catalan",    NULL,       GS_CMD, IDM_LANGCT, "<CheckItem>" },
  { "/Настройка/Язык/_Deutsch",   NULL,       GS_CMD, IDM_LANGDE, "<CheckItem>" },
  { "/Настройка/Язык/_Greek",     NULL,       GS_CMD, IDM_LANGGR, "<CheckItem>" },
  { "/Настройка/Язык/E_spaсol",   NULL,       GS_CMD, IDM_LANGES, "<CheckItem>" },
  { "/Настройка/Язык/_Franзais",  NULL,       GS_CMD, IDM_LANGFR, "<CheckItem>" },
  { "/Настройка/Язык/_Italiano",  NULL,       GS_CMD, IDM_LANGIT, "<CheckItem>" },
  { "/Настройка/Язык/_Nederlands",    NULL,       GS_CMD, IDM_LANGNL, "<CheckItem>" },
  { "/Настройка/Язык/Russian",    NULL,       GS_CMD, IDM_LANGRU, "<CheckItem>" },
  { "/Настройка/Язык/Slovak",     NULL,       GS_CMD, IDM_LANGSK, "<CheckItem>" },
  { "/Настройка/Язык/Svenska",    NULL,       GS_CMD, IDM_LANGSE, "<CheckItem>" },
  { "/Настройка/PStoText",            NULL,   NULL,       0, "<Branch>" },
  { "/Настройка/PStoText/Отключен",   NULL,       GS_CMD, IDM_PSTOTEXTDIS, "<CheckItem>" },
  { "/Настройка/PStoText/Нормальный",     NULL,       GS_CMD, IDM_PSTOTEXTNORM, "<CheckItem>" },
  { "/Настройка/PStoText/Dvips Cork Encoding",    NULL,   GS_CMD, IDM_PSTOTEXTCORK, "<CheckItem>" },
  { "/Настройка/Предупреждения DSC",            NULL,   NULL,       0, "<Branch>" },
  { "/Настройка/Предупреждения DSC/Отключить",    NULL,       GS_CMD, IDM_DSC_OFF, "<CheckItem>" },
  { "/Настройка/Предупреждения DSC/Ошибки",     NULL,       GS_CMD, IDM_DSC_ERROR, "<CheckItem>" },
  { "/Настройка/Предупреждения DSC/Предупреждения",   NULL,       GS_CMD, IDM_DSC_WARN, "<CheckItem>" },
  { "/Настройка/Предупреждения DSC/Все",    NULL,       GS_CMD, IDM_DSC_INFO, "<CheckItem>" },
  { "/Настройка/Сохранить _настройки",  NULL,       GS_CMD, IDM_SETTINGS, NULL },
  { "/Настройка/sep1",            NULL,   NULL,       0, "<Separator>" },
  { "/Настройка/Сохранять настройки при _выходе",  NULL,       GS_CMD, IDM_SAVESETTINGS, "<CheckItem>" },
  { "/Настройка/_Безопасный режим",          NULL,       GS_CMD, IDM_SAFER, "<CheckItem>" },
  { "/Настройка/Запоминать последний _каталог",    NULL,       GS_CMD, IDM_SAVEDIR, "<CheckItem>" },
  { "/Настройка/П_анель кнопок",         NULL,       GS_CMD, IDM_BUTTONSHOW, "<CheckItem>" },
/*  { "/Настройка/О_кно по размеру страницы",   NULL,       GS_CMD, IDM_FITPAGE, "<CheckItem>" }, */
/*  { "/Настройка/Быстрое открытие",        NULL,       GS_CMD, IDM_QUICK_OPEN, "<CheckItem>" }, */
  { "/Настройка/_Автоматическое обновление",     NULL,       GS_CMD, IDM_AUTOREDISPLAY, "<CheckItem>" },
  { "/Настройка/Обр_езка EPS",       NULL,       GS_CMD, IDM_EPSFCLIP, "<CheckItem>" },
  { "/Настройка/Предупреждения _EPS",       NULL,       GS_CMD, IDM_EPSFWARN, "<CheckItem>" },
  { "/Настройка/Игнорировать комментарии _DSC",         NULL,       GS_CMD, IDM_IGNOREDSC, "<CheckItem>" },
  { "/Настройка/Показывать габаритный пря_моугольник",  NULL,       GS_CMD, IDM_SHOWBBOX, "<CheckItem>" },

  { "/П_росмотр",           NULL,   NULL,       0, "<Branch>" },
  { "/Просмотр/_Следующая страница",     "plus", GS_CMD,     IDM_NEXT, NULL },
  { "/Просмотр/_Предыдущая страница",     "minus",    GS_CMD,     IDM_PREV, NULL },
  { "/Просмотр/П_ерейти к странице...",     "G",    GS_CMD,     IDM_GOTO, NULL },
  { "/Просмотр/_Назад",       "B",    GS_CMD,     IDM_GOBACK, NULL },
  { "/Просмотр/_Вперёд",    NULL,   GS_CMD,     IDM_GOFWD, NULL },
  { "/Просмотр/_Обновить",     "R",    GS_CMD,     IDM_REDISPLAY, NULL },
  { "/Просмотр/sep1",           NULL,   NULL,       0, "<Separator>" },
  { "/Просмотр/Следующая страница и вверх", "space",    GS_CMD,     IDM_NEXTHOME, NULL },
  { "/Просмотр/Предыдущая страница и вверх", "<control>H",GS_CMD,    IDM_PREVHOME, NULL },
  { "/Просмотр/sep2",           NULL,   NULL,       0, "<Separator>" },
  { "/Просмотр/Полноэкранный _режим",   "F4",   GS_CMD,     IDM_FULLSCREEN, NULL },
  { "/Просмотр/По р_азмеру окна",    "F6",   GS_CMD,     IDM_FITWIN, NULL },

  { "/_Ориентация",            NULL,   NULL,       0, "<Branch>" },
  { "/Ориентация/_Авто",       NULL,   GS_CMD,     IDM_AUTOORIENT, "<CheckItem>" },
  { "/Ориентация/sep1",            NULL,   NULL,       0, "<Separator>" },
  { "/Ориентация/_Книжная",       NULL,   GS_CMD,     IDM_PORTRAIT, "<CheckItem>" },
  { "/Ориентация/А_льбомная",      NULL,   GS_CMD,     IDM_LANDSCAPE, "<CheckItem>" },
  { "/Ориентация/П_еревернутая книжная",    NULL,   GS_CMD,     IDM_UPSIDEDOWN, "<CheckItem>" },
  { "/Ориентация/_Обратная альбомная",       NULL,   GS_CMD,     IDM_SEASCAPE, "<CheckItem>" },
  { "/Ориентация/sep2",            NULL,   NULL,       0, "<Separator>" },
  { "/Ориентация/_Поменять альбомные",     NULL,   GS_CMD,     IDM_SWAPLANDSCAPE, "<CheckItem>" },

  { "/_Бумага",          NULL,   NULL,       0, "<Branch>" },
  { "/Бумага/_Настройка экрана...", NULL,GS_CMD,     IDM_DISPLAYSETTINGS, NULL },
  { "/Бумага/sep1",      NULL,   NULL,       0, "<Separator>" },
  { "/Бумага/_Повернуть лист",     NULL,   GS_CMD,     IDM_MEDIAROTATE, "<CheckItem>"},
  { "/Бумага/sep2",          NULL,   NULL,       0, "<Separator>" },
  { "/Бумага/11x17",         NULL,   GS_CMD,     IDM_11x17, "<CheckItem>" },
  { "/Бумага/A3",        NULL,   GS_CMD,     IDM_A3, "<CheckItem>" },
  { "/Бумага/A4",        NULL,   GS_CMD,     IDM_A4, "<CheckItem>" },
  { "/Бумага/A5",        NULL,   GS_CMD,     IDM_A5, "<CheckItem>" },
  { "/Бумага/B4",        NULL,   GS_CMD,     IDM_B4, "<CheckItem>" },
  { "/Бумага/B5",        NULL,   GS_CMD,     IDM_B5, "<CheckItem>" },
  { "/Бумага/Ledger",        NULL,   GS_CMD,     IDM_LEDGER, "<CheckItem>" },
  { "/Бумага/Legal",         NULL,   GS_CMD,     IDM_LEGAL, "<CheckItem>" },
  { "/Бумага/Letter",        NULL,   GS_CMD,     IDM_LETTER, "<CheckItem>" },
  { "/Бумага/Note",      NULL,   GS_CMD,     IDM_NOTE, "<CheckItem>" },
  { "/Бумага/_Другая...",   NULL,   GS_CMD,     IDM_USERSIZE, "<CheckItem>"},
  { "/Бумага/USERSIZE1",     NULL,   GS_CMD,     IDM_USERSIZE1, "<CheckItem>"},
  { "/Бумага/USERSIZE2",     NULL,   GS_CMD,     IDM_USERSIZE2, "<CheckItem>"},
  { "/Бумага/USERSIZE3",     NULL,   GS_CMD,     IDM_USERSIZE3, "<CheckItem>"},
  { "/Бумага/USERSIZE4",     NULL,   GS_CMD,     IDM_USERSIZE4, "<CheckItem>"},
  { "/Бумага/USERSIZE5",     NULL,   GS_CMD,     IDM_USERSIZE5, "<CheckItem>"},
  { "/Бумага/USERSIZE6",     NULL,   GS_CMD,     IDM_USERSIZE6, "<CheckItem>"},
  { "/Бумага/USERSIZE7",     NULL,   GS_CMD,     IDM_USERSIZE7, "<CheckItem>"},
  { "/Бумага/USERSIZE8",     NULL,   GS_CMD,     IDM_USERSIZE8, "<CheckItem>"},
  { "/Бумага/USERSIZE9",     NULL,   GS_CMD,     IDM_USERSIZE9, "<CheckItem>"},
  { "/Бумага/USERSIZE10",    NULL,   GS_CMD,     IDM_USERSIZE10, "<CheckItem>"},
  { "/Бумага/USERSIZE11",    NULL,   GS_CMD,     IDM_USERSIZE11, "<CheckItem>"},
  { "/Бумага/USERSIZE12",    NULL,   GS_CMD,     IDM_USERSIZE12, "<CheckItem>"},
  { "/Бумага/USERSIZE13",    NULL,   GS_CMD,     IDM_USERSIZE13, "<CheckItem>"},

  { "/_Справка",           "F1",   NULL,       0, "<Branch>" },
  { "/?/_Содержание",      NULL,   GS_CMD,     IDM_HELPCONTENT, NULL },
  { "/?/_Указатель...", NULL,  GS_CMD,     IDM_HELPSEARCH, NULL },
  { "/?/_Клавиатура",     NULL,   GS_CMD,     IDM_HELPKEYS, NULL },
  { "/?/sep1",       NULL,   NULL,       0, "<Separator>" },
  { "/?/_Регистрация...",   NULL,   GS_CMD,     IDM_REGISTER, NULL },
  { "/?/_О программе...",      NULL,   GS_CMD,     IDM_ABOUT, NULL },
};

int menu_ru_len = sizeof (menu_ru) / sizeof (menu_ru[0]);

