/* Copyright (C) 1993-2012, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* ru\gvclang.h */
/* Common Russian language defines */

#define AARUSSIAN "Russian"

#define GSVIEW_COPYRIGHT1 "Copyright (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "Все права сохранены."
#define GSVIEW_COPYRIGHT3 "Подробности см. файл LICENCE."

#define GS_COPYRIGHT1 "Copyright (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A.  Все права сохранены."
#define GS_COPYRIGHT3 "Подробности см. файл PUBLIC."

/* Buttons */
#ifdef UNIX
#define AAOK "Ok"
#define AACANCEL "Отмена"
#define AAEDIT "Правка"
#define AADEFAULTS "По умолчанию"
#define AAHELP "Справка"
#define AANEW "Новый"
#define AATEST "Тест"
#define AAYES "Да"
#define AANO "Нет"
#define AANEXTGT "Далее >"
#define AABACKLT "< Назад"
#define AAFINISHGT "Конец >"
#define AAEXIT "Выход"
#else
#ifdef _Windows
#define AAOK "&Ok"
#define AACANCEL "Отмена"
#define AAEDIT "&Правка"
#define AADEFAULTS "По &умолчанию"
#define AAHELP "&Справка"
#define AANEW "&Новый"
#define AATEST "&Тест"
#define AAYES "&Да"
#define AANO "Н&ет"
#define AANEXTGT "Д&алее >"
#define AABACKLT "< Н&азад"
#define AAFINISHGT "&Конец >"
#define AAEXIT "В&ыход"
#else
#define AAOK "~Ok"
#define AACANCEL "Отмена"
#define AAEDIT "~Правка"
#define AADEFAULTS "По ~умолчанию"
#define AAHELP "~Справка"
#define AANEW "~Новый"
#define AATEST "~Тест"
#define AAYES "~Да"
#define AANO "Н~ет"
#define AANEXTGT "Д~алее >"
#define AABACKLT "< Н~азад"
#define AAFINISHGT "~Конец >"
#define AAEXIT "В~ыход"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "О GSview для Windows"
#define AAABOUTPM "О GSview для PM"
#define AAABOUTX11 "О GSview для X11"
#define AAABOUTGSV16SPL "О GSV16SPL"
#define AAABOUTGSVIEWPRINT "О программе печати GSview"
#define AACOPY1 "Версия:"
#define AACOPY2 "Графический интерфейс Ghostscript"
#define AACOPY4 "Эта программа распространяется БЕЗ ВСЯКИХ ГАРАНТИЙ."
#define AACOPY5 "Ни автор, ни распространитель не несут никакой ответственности"
#define AACOPY6 "за вред или пользу при её использовании в любом виде и случае,"
#define AACOPY7 "за исключением указанных письменно.  Подробности см. в"
#define AACOPY8 "GSview Free Public Licence (далее 'Лицензия')."
#define AACOPY9 "Каждая копия GSview должна включать копию Лицензии, обычно"
#define AACOPY10 "файл в текстовом формате с именем LICENCE. Лицензия"
#define AACOPY11 "предоставляет вам право копировать, изменять и передавать"
#define AACOPY12 "GSview, но только на условиях, указанных в Лицензии.  Кроме"
#define AACOPY13 "того, Лицензия требует, чтобы информация о правах и это"
#define AACOPY14 "сообщение были во всех копиях."
#define AACOPY15 "pstotxt3.dll is Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY16 "pstotxt2.dll is Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY17 "pstotxt1.dll is Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY18 "и имеет отдельную лицензию. Подробности см. pstotext.txt."
#define AACOPY19 "Автор: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Интерфейс DLL Ghostscript."
#define AACOPY21 "Эта программа -- часть GSview."
#define AACOPY22 "Спулер Win32s/Win16 для GSview"

#define AAINPUT "Ввод"

/* DSC error dialog box */
#define AADSC "Соглашения о структурировании документа (DSC)"
#define AAIGNOREALLDSC "Игнорировать все DSC"

/* Info dialog box */
#define AAINFO  "Информация"
#define AAFILEC "Файл:"
#define AATYPEC "Тип:"
#define AATITLEC "Заголовок:"
#define AADATEC "Дата:"
#define AABOUNDINGBOXC "Габариты:"
#define AAORIENTATIONC "Ориентация:"
#define AADEFAULTMEDIAC "Размер бумаги:"
#define AAPAGEORDERC "Порядок страниц:"
#define AAPAGESC "Всего страниц:"
#define AAPAGEC "Страница:"
#define AABITMAPC "Битовый образ:"

/* Sounds dialog box */
#define AASOUNDS "Звуки"
#define AAEVENTC "Событие:"
#define AASOUNDC "Звук:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Выбор порта принтера"
#define AASELECTPRINTER "Выбор принтера"
#define AAPRINTING "Идёт печать..."
#define AASELECTPAGE "Выбор страницы"
#define AASELECTPAGES "Выбор страниц"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Настройка принтера"
#define AADEVICEC "Устройство:"
#define AARESOLUTIONC "Разрешение:"
#define AAQUEUEC "Очередь:"
#define AAPRINTTOFILE "Печатать в файл"
#define AAPSPRINTER "Принтер PostScript"
#define AAOPTIONSC "Опции:"
#define AAFIXEDMEDIA "Фиксированный размер"
#define AACONVERT "Конвертировать"
#define AAWINPRINTERSETTINGS "Настройка принтера Windows"
#define AACOLOURS "Цвета"
#define AABANDW "Чёрно-белый"
#define AAGREY "Полутоновой"
#define AAFULLCOLOUR "Полноцветный"
#define AASELECTGSDEVICE "Выбор устройства Ghostscript"

#define AAODDEVEN "Нечётн./Чётн."
#define AAIGNOREDSC "Игнорировать DSC"
#define AAPAGES "Страницы"
#define AAFROMC "От:"
#define AATOC "До:"

#define AAADVANCED "Расширенные"
#define AAADVANCEDPSOPT "Расширенные настройки PostScript"
#define AASENDCTRLDBEFORE "Посылать CTRL+D перед заданием"
#define AASENDCTRLDAFTER "Посылать CTRL+D после задания"
#define AAPROLOGFILE "Файл пролога"
#define AAEPILOGFILE "Файл эпилога"
#define AABROWSE "Найти"

/* PS2EPS dialog box */
#define AAPSTOEPS "PS в EPS"
#define AAPSTOEPSREAD "Вы читали справку о `PS в EPS`?"
#define AAPSTOEPSAUTO "Автоматически вычислять габариты"

#ifdef UNIX
#define AAAPROPERTIES "Свойства"
#define AAALLPAGES "Все страницы"
#define AAODDPAGES "Нечётные страницы"
#define AAEVENPAGES "Чётные страницы"
#define AAALL "Все"
#define AAODD "Нечётные"
#define AAEVEN "Чётные"
#define AAREVERSE "В обратном"
#else
#ifdef _Windows
#define AAAPROPERTIES "&Свойства"
#define AAALLPAGES "&Все страницы"
#define AAODDPAGES "&Нечётные страницы"
#define AAEVENPAGES "&Чётные страницы"
#define AAALL "Вс&е"
#define AAODD "&Нечётные"
#define AAEVEN "&Чётные"
#define AAREVERSE "В &обратном"
#else
#define AAAPROPERTIES "~Свойства"
#define AAALLPAGES "~Все страницы"
#define AAODDPAGES "~Нечётные страницы"
#define AAEVENPAGES "~Чётные страницы"
#define AAALL "Вс~е"
#define AAODD "~Нечётные"
#define AAEVEN "~Чётные"
#define AAREVERSE "В ~обратном"
#endif
#endif


/* Properties dialog box */
#define AAPROPERTIES "Свойства"
#define AAPROPERTYC "Свойство:"
#define AAVALUEC "Значение:"
#define AAPAGEOFFSETPT "Смещение страницы (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Редактирование свойств"
#define AAPROPERTYTYPEC "Тип свойства:"
#define AANUMBER "Число"
#define AASTRING "Строка"
#define AANAMEC "Имя: напр. BitsPerPixel"
#define AAVALUESC "Значения: например 1,3,8,16,24"
#define AADELETE "Удалить"

#define AABOUNDINGBOX "BoundingBox"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Расширенная настройка"
#define AAGHOSTSCRIPTDLLC "DLL Ghostscript:"
#define AAGHOSTSCRIPTSOC "Разделяемые объекты Ghostscript:"
#define AAGHOSTSCRIPTEXEC "Программа Ghostscript:"
#define AAGHOSTSCRIPTINCC "Дополнительные каталоги Ghostscript:"
#define AAGHOSTSCRIPTOTHERC "Опции Ghostscript:"
#define AACOPYPRINTERDEF "Копировать умолчания принтера"
#define AAASSOCIATEPS "Связать .ps файлы с GSview"
#define AAASSOCIATEPDF "Связать .pdf файлы с GSview"
#define AACREATESTARTMENU "Создать ярлыки в Главном меню"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Простая настройка"
#define AAWHICHGS "Какую версию Ghostscript вы хотите использовать?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Загрузка Ghostscript"
#define AADOWNLOADNOW "Загрузить"
#define AAGSNOTFOUND "На Вашем компьютере не найден Ghostscript. Вам нужно загрузить и установить AFPL Ghostscript с сайта"

#define AAGSMESSWIN "GSview для Windows - сообщения Ghostscript"
#define AAGSMESSPM "GSview для PM - сообщения Ghostscript"
#define AAGSMESSX11 "GSview для X11 - сообщения Ghostscript"
#define AACOPY "Копировать"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Настройка экрана"
#define AARESOLUTION "Разрешение"
#define AAZOOMRESOLUTION "Увеличение"
#define AADEPTH "Глубина цвета"
#define AATEXTALPHA "Сглаживание текста"
#define AAGRAPHICSALPHA "Сглаживание графики"
#define AADRAWMETHOD "Метод рисования"
#define AADPI "dpi"
#define AABPP "бит на точку"
#define AABITS "бит"

/* Measure and custom units dialog box */
#define AAPT "поинты"
#define AAMM "мм"
#define AAINCH "дюймы"
#define AACUSTOM "произв."
#define AAMEASURE "Измерения"
#define AAUNITC "Единицы:"
#define AASTARTC "Начало:"
#define AAFINISHC "Конец:"
#define AADELTAC "Дельта:"
#define AALENGTHC "Вектор:"
#define AACHANGE "Правка"
#define AACALCXFORM "Вычисление преобразований"
#define AATRANSLATE "Перенести"
#define AAROTATE "Повернуть"
#define AASCALE "Масштаб"
#define AAINITMATRIX "initmatrix"
#define AAINVERTMATRIX "invertmatrix"

/* PS to Edit dialog box */
#define AAPSTOEDIT "PS to Edit"
#define AAFORMATC "Формат:"
#define AADRAWTASP "Текст многоугольниками"
#define AAMAPTOLATIN1 "Отображать в ISO-Latin1"
#define AAFLATNESSC "Спрямление:"
#define AADEFAULTFONTC "Шрифт по ум.:"
#define AADRIVEROPTIONSC "Опции драйвера:"

/* Registration */
#define AAREGTOC "Владелец копии:"
#define AANUMBERC "Номер:"
#define AAGSVIEWREG "Регистрация GSview"
#define AAREGISTERNOW "Регистрировать"
#define AAONLINEREG "Регистрация по сети"
#define AAREG1 "Пожалуйста, введите Ваше имя и регистрационный номер"
#define AAREG2 ""
#define AANAG1 "GSview is Copyright 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Пожалуйста, поддержите регистрацией разработку GSview."
#define AANAG3 ""
#define AANAG4 "GSview можно зарегистрировать в Интернет по адресу"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "    http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "Установка GSview"
#define AAINSTALLINTRO1 "Этот программа поможет вам в установке"
#define AAINSTALLINTRO2 "Если вы не используете установку из самораспаковывающегося архива, убедитесь, что в текущем каталоге есть все файлы перечисленные в Readme.htm."
#define AAINSTALLINTRO3 "Для установки потребуется 20-30 Мбайт на диске."
#define AAINSTALLCOPYRIGHT1 "О Copyright"
#define AAINSTALLCOPYRIGHT2 "GSview и AFPL это - разные программы."
#define AAINSTALLCOPYRIGHT3 "У них разные владельцы."
#define AAINSTALLGSVER1 "Какие компоненты вы хотите установить?"
#define AAINSTALLGSVER2 "Установить GSview"
#define AAINSTALLGSVER3 "Установить Ghostscript"
#define AAINSTALLGSVER4 "Какую версию Ghostscript вы хотите использовать?"
#define AAINSTALLDIR1 "Укажите каталог, куда устанавливать GSview и Ghostscript."
#define AAINSTALLDIR2 "Внутри этого каталога GSview будет в подкаталоге:"
#define AAINSTALLDIR3 "Ghostscript будет в подкаталоге:"
#define AAINSTALLMKDIR1 "Указанный каталог не существует."
#define AAINSTALLMKDIR2 "Будет создан новый каталог." 
#define AAINSTALLMISSING1 "Установка GSview - отсутствует ZIP-файл"
#define AAINSTALLMISSING2 "Не могу найти ZIP-файл. Вставьте диск с указанным файлом."
#define AAINSTALLCONFIG1 "Для временных файлов GSview и Ghostscript используют каталог, указанный в переменной окружения TEMP."
#define AAINSTALLCONFIG2 "Переменная TEMP не определена."
#define AAINSTALLCONFIG3 "В autoexec.bat будет добавлена строка\r    SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Обновить autoexec.bat"
#define AAINSTALLCONFIG5 "Сохранить старый autoexec.bat"
#define AAINSTALLCONFIGPM1 "Для временных файлов GSview и Ghostscript используют каталог, указанный в переменной окружения TEMP."
#define AAINSTALLCONFIGPM2 "Переменная TEMP не определена или необходимо установить EMX."
#define AAINSTALLCONFIGPM3 "В config.sys будет добавлена строка 'SET TEMP=C:\\', а PATH и/или LIBPATH будут обновлены для указания на EMX."
#define AAINSTALLCONFIGPM4 "Обновить config.sys"
#define AAINSTALLCONFIGPM5 "Сохранить старый config.sys"
#define AAINSTALLFINISH "Для установки всё готово. \r\rДля продолжения нажмите кнопку Конец."
#define AAINSTALLUNZIP1 "Распаковка файлов..."
#define AADIRNOTEXIST "Каталог не существует. Создать?"
#define AAMKDIRFAIL "Не могу создать каталог"
#define AABETAWARN "Это BETA-версия GSview. Срок тестирования заканчивается %04d-%02d-%02d."
#define AAINSTALLOK "Установка успешно завершена."
#define AAINSTALLFAILED "Установка не удалась."
#define AAPROGMANGROUP1 "Установка GSview может создать группу в Диспетчере программ или пункт Главного меню."
#define AAPROGMANGROUP2 "Создать группу или пункт"
#define AAPROGMANGROUP3 "Имя"
#define AAPROGMANGROUP4 "Инструменты GS"
#define AAPROGMANGROUP5 "Создана группа Диспетчера программ с именем \042%s\042."
#define AAPROGMANGROUP6 "На рабочем столе создан программный объект \042GSview\042."
#define AAPMOBJECT1 "Программа установки GSview может создать объект для GSview на рабочем столе."
#define AAPMOBJECT2 "Создать программный объект GSview."
#define AADONEBAK "Старый autoexec.bat будет переименован в %s"
#define AACANTLOAD "Не могу загрузить %s"
#define AACANTCREATETEMPFILE "Не могу создать временный файл с именем"
#define AACANTOPENREAD "Не могу открыть для чтения %s"
#define AACANTOPENWRITE "Не могу открыть для записи %s"
#define AAERRORRENAME "Ошибка при переименовании %s в %s"
#define AANODDEPROGMAN "Не могу установить DDE-соединение с Диспетчером программ. Перезапустите Windows."
#define AAINSERTDISK "Вставьте диск с %s"
#define AAZIPNOTFOUND "Zip-файл не найден"
#define AAUNZIPCANCELLED "Распаковка прервана\n"
#define AACANTALLOCBUF "Недостаточно памяти для буфера копирования"
#define AAPROGRAMOBJECTFAILED "Не могу создать программный объект на рабочем столе"
#define AAUNINSTALLTITLE "GSview и AFPL Ghostscript"
#define AAUNINSTALLITEM "Удалить GSview и AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "Установка потребует на диске около 5 Мбайт"
#define AAGSVIEWREQUIRES "GSview также требует"
#define AAVIEWREADMEDETAILS "Подробности о получении Ghostscript см. файл Readme."
#define AACOPYRIGHTNOTICE "О Copyright"
#define AASELECTGSVIEWDIR "Выберите каталог для установки GSview"
#define AABROWSEE "Обзор..."
#define AAGSVIEWADDSTARTMENU "Установка GSview Setup добавит пункты Главного меню в"
#define AACREATEFOLDER "Создать папку"
#define AAALLUSERS "Все пользователи"
#define AASELECTFOLDER "Выбрать папку"
#define AAFOLDER "Папка"
#define AADESTFOLDER "Целевая папка"
#define AAVIEWREADME "См. Readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "Конфигурация GSview"
#define AACFG10 "Мастер поможет сконфигурировать GSview."
#define AACFG11 "Если вы не понимаете некоторые пункты на следующих страницах,\
 не изменяйте их. По умолчанию будут установлены разумные значения."
#define AACFG20 "GSview нужно знать какую версию Ghostscript использовать\
 и где он находится."
#define AACFG21 "Какую версию Ghostscript вы хотите использовать?"
#define AACFG22 "В каком каталоге находится Ghostscript?"
#define AACFG23 "Искать шрифты Type 1 в"
#define AACFG30 "GSview хранит список принтеров Ghostscript и\
 их разрешения в своём инициализационном файле."
#define AACFG31 "По умолчанию список устройств и их разрешение вывода берутся из стандартной\
 версии Ghostscript 5.50 и может быть неполным."
#define AACFG32 "Обновить список принтеров GSview" 
#define AACFG33 "Обновление списка принтеров GSview перезапишет все существующие значения." 
#define AACFG40 "GSview может создать связи с файлами PostScript и\
 Portable Document Format (PDF)."
#define AACFG43 "При наличии этих связей двойной щелчок на имени PostScript\
 или PDF файла запустит GSview.\
 Эти связи также позволяют некоторым браузерам Интернет\
 использовать GSview для просмотра файлов PostScript и PDF."
#define AACFG41 "Связать файлы PostScript (.ps и .eps) с GSview."
#define AACFG42 "Связать файлы PDF (.pdf) с GSview."
#define AACFG50 "GSview может создать группу в Диспетчере программ или\
 пункты Главного меню для GSview и AFPL Ghostscript."
#define AACFG51 "Создать Группу или пункт"
#define AACFG52 "Имя"
#define AACFG53 "GSview может создать для себя программный объект на рабочем столе. \
 Это свяжет файлы PostScript (.ps и .eps) и PDF (.pdf) с GSview."
#define AACFG54 "Создать программный объект"
#define AACFG60 "GSview готов к конфигурированию. \r\rДля продолжения нажмите кнопку Конец."
#define AACFG70 "Конфигурация успешна."
#define AACFG72 "Если понадобится изменить конфигурацию,\
 запустите GSview, а затем выберите команду \042Настройка | Простая настройка...\042 или\
 \042Настройка | Расширенная настройка...\042."
#define AACFG73 "Конфигурирование не удалось."
#define AACFG74 "Конфигурация отменена."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "Печать GSview"
#define AAGSVIEWPRINTUSAGE "Используйте: %s [/d] dllpath optionfile inputfile\noptionfile и inputfile будут удалены при выходе\nПодразумевается, что gvpgs будет запущен с временными файлами\n"
#ifdef _Windows
#define AAMFILE "&Файл"
#define AAMSAVEAS "&Сохранить как..."
#define AAMEXIT "В&ыход"
#define AAMEDIT "&Правка"
#define AAMCOPY "&Копировать\tCtrl+C"
#define AAMHELP "&Справка"
#define AAMABOUT "&О программе..."
#else
#define AAMFILE "~Файл"
#define AAMSAVEAS "~Сохранить как..."
#define AAMEXIT "В~ыход"
#define AAMEDIT "~Правка"
#define AAMCOPY "~Копировать\tCtrl+C"
#define AAMHELP "~Справка"
#define AAMABOUT "~О программе..."
#endif

#define WINHELPFILE "gsviewru.chm"
