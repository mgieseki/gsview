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

/* sk\gvclang.h */
/* Common Slovak language defines */

#define AASLOVAK "Slovak"

#define GSVIEW_COPYRIGHT1 "AutorskÈ pr·va (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "Vöetky pr·va vyhradenÈ."
#define GSVIEW_COPYRIGHT3 "Pozrite si s˙bor LICENCE pre viac inform·ciÌ."

#define GS_COPYRIGHT1 "AutorskÈ pr·va (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A.  Vöetky pr·va vyhradenÈ."
#define GS_COPYRIGHT3 "Pozrite si s˙bor PUBLIC pre viac inform·ciÌ."

/* Buttons */
#ifdef UNIX
#define AAOK "OK"
#define AACANCEL "Zruöiù"
#define AAEDIT "⁄pravy"
#define AADEFAULTS "ätandardnÈ"
#define AAHELP "PomocnÌk"
#define AANEW "Nov˝"
#define AATEST "Test"
#define AAYES "¡no"
#define AANO "Nie"
#define AANEXTGT "œalej >"
#define AABACKLT "< Nasp‰ù"
#define AAFINISHGT "DokonËiù >"
#define AAEXIT "SkonËiù"
#else
#ifdef _Windows
#define AAOK "&OK"
#define AACANCEL "&Zruöiù"
#define AAEDIT "⁄pr&avy"
#define AADEFAULTS "&ätandardnÈ"
#define AAHELP "&PomocnÌk"
#define AANEW "&Nov˝"
#define AATEST "&Test"
#define AAYES "¡n&o"
#define AANO "&Nie"
#define AANEXTGT "œ&alej >"
#define AABACKLT "< Na&sp‰ù"
#define AAFINISHGT "&DokonËiù >"
#define AAEXIT "&SkonËiù"
#else
#define AAOK "~OK"
#define AACANCEL "~Zruöiù"
#define AADEFAULTS "ä~tandardnÈ"
#define AAEDIT "⁄pr~avy"
#define AAHELP "~PomocnÌk"
#define AANEW "~Nov˝"
#define AATEST "~Test"
#define AAYES "¡n~o"
#define AANO "~Nie"
#define AANEXTGT "œa~lej >"
#define AABACKLT "< Na~sp‰ù"
#define AAFINISHGT "~DokonËiù >"
#define AAEXIT "~SkonËiù"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "»o je..."
#define AAABOUTPM "»o je PM GSview"
#define AAABOUTX11 "»o je X11 GSview"
#define AAABOUTGSV16SPL "»o je GSV16SPL"
#define AAABOUTGSVIEWPRINT "»o je GSview tlaË"
#define AACOPY1 "Verzia:"
#define AACOPY2 "GrafickÈ prostredie pre program Ghostscript"
#define AACOPY4 "Tento program je distribuovan˝ NO WARRANTY OF ANY KIND."
#define AACOPY5 "No author or distributor accepts any responsibility for the"
#define AACOPY6 "consequences of using it, or for whether it serves any particular"
#define AACOPY7 "purpose or works at all, unless he or she says so in writing.  Refer"
#define AACOPY8 "to the GSview Free Public Licence (the 'Licence') for full details."
#define AACOPY9 "Every copy of GSview must include a copy of the Licence, normally"
#define AACOPY10 "in a plain ASCII text file named LICENCE.  The Licence grants you"
#define AACOPY11 "the right to copy, modify and redistribute GSview, but only under"
#define AACOPY12 "certain conditions described in the Licence.  Among other things,"
#define AACOPY13 "the Licence requires that the copyright notice and this notice be"
#define AACOPY14 "preserved on all copies."
#define AACOPY15 "pstotxt3.dll autorskÈ pr·va (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY16 "pstotxt2.dll autorskÈ pr·va (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY17 "pstotxt1.dll autorskÈ pr·va (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY18 "and has a different licence.  Pozrite si s˙bor pstotext.txt pre viac inform·ciÌ."
#define AACOPY19 "Autor: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Ghostscript DLL prostredie."
#define AACOPY21 "Tento program je Ëasùou programu GSview."
#define AACOPY22 "GSview Win32s/Win16 spooler"

#define AAINPUT "Vstup"

/* DSC error dialog box */
#define AADSC "Dokument Structuring Conventions"
#define AAIGNOREALLDSC "Ignorovaù vöetky DSC"

/* Info dialog box */
#define AAINFO  "Inform·cie"
#define AAFILEC "S˙bor:"
#define AATYPEC "Typ:"
#define AATITLEC "N·zov:"
#define AADATEC "D·tum:"
#define AABOUNDINGBOXC "BoundingBox:"
#define AAORIENTATIONC "Orient·cia:"
#define AADEFAULTMEDIAC "ätandardnÈ mÈdium:"
#define AAPAGEORDERC "Page Order:"
#define AAPAGESC "PoËet str·n:"
#define AAPAGEC "Aktu·lna strana:"
#define AABITMAPC "Bitov· mapa:"

/* Sounds dialog box */
#define AASOUNDS "Zvuky"
#define AAEVENTC "Udalosù:"
#define AASOUNDC "Zvuk:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Vybraù port tlaËiarne"
#define AASELECTPRINTER "Vybraù tlaËiareÚ"
#define AAPRINTING "TlaËÌ sa"
#define AASELECTPAGE "Vybraù starnu"
#define AASELECTPAGES "Vybraù strany"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Nastavenie tlaËe"
#define AADEVICEC "Zariadenie:"
#define AARESOLUTIONC "RozlÌöenie:"
#define AAQUEUEC "Rad:"
#define AAPRINTTOFILE "TlaËiù do s˙boru"
#define AAPSPRINTER "PostScript tlaËiareÚ"
#define AAOPTIONSC "Moûnosti:"
#define AAFIXEDMEDIA "Fixed media"
#define AACONVERT "Konvertovaù"
#define AAWINPRINTERSETTINGS "Windows nastvenia tlaËiarne"
#define AACOLOURS "Farba"
#define AABANDW "»iernobielo"
#define AAGREY "Siv·"
#define AAFULLCOLOUR "Farebne"
#define AASELECTGSDEVICE "Vybraù Ghostscript zariadenie"

#define AAODDEVEN "Nep·rne/P·rne"
#define AAIGNOREDSC "Ignorovaù DSC"
#define AAPAGES "Strany"
#define AAFROMC "Od:"
#define AATOC "do:"

#define AAADVANCED "RozöÌrenÈ"
#define AAADVANCEDPSOPT "RozöÌrenÈ moûnosti PostScript"
#define AASENDCTRLDBEFORE "Send CTRL+D before job"
#define AASENDCTRLDAFTER "Send CTRL+D after job"
#define AAPROLOGFILE "Prolog s˙bor"
#define AAEPILOGFILE "Epilog s˙bor"
#define AABROWSE "Prehæad·vaù"

/* PS2EPS dialog box */
#define AAPSTOEPS "PS do EPS"
#define AAPSTOEPSREAD "Chcete zobraziù PomocnÌka o 'PS to EPS'?"
#define AAPSTOEPSAUTO "Automaticky r·taù Bounding Box"

#ifdef UNIX
#define AAAPROPERTIES "Vlastnosti"
#define AAALLPAGES "Vöetky strany"
#define AAODDPAGES "Nep·rne strany"
#define AAEVENPAGES "P·rne strany"
#define AAALL "Vöetky"
#define AAODD "Nep·rne"
#define AAEVEN "P·rne"
#define AAREVERSE "Reverse"
#else
#ifdef _Windows
#define AAAPROPERTIES "&Vlastnosti"
#define AAALLPAGES "&Vöetky strany"
#define AAODDPAGES "&Nep·rne strany"
#define AAEVENPAGES "&P·rne strany"
#define AAALL "&Vöetky"
#define AAODD "&Nep·rne"
#define AAEVEN "&P·rne"
#define AAREVERSE "&Reverse"
#else
#define AAAPROPERTIES "~Vlastnosti"
#define AAALLPAGES "~Vöetky strany"
#define AAODDPAGES "~Nep·rne strany"
#define AAEVENPAGES "~P·rne strany"
#define AAALL "~Vöetky"
#define AAODD "~Nep·rne"
#define AAEVEN "~P·rne"
#define AAREVERSE "~Reverse"
#endif
#endif


/* Properties dialog box */
#define AAPROPERTIES "Vlastnosti"
#define AAPROPERTYC "Vlastnosù:"
#define AAVALUEC "Hodnota:"
#define AAPAGEOFFSETPT "Page Offset (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Upraviù vlastnosti"
#define AAPROPERTYTYPEC "Typ vlastnosti:"
#define AANUMBER "»Ìslo"
#define AASTRING "Reùazec"
#define AANAMEC "N·zov: e.g.  BitsPerPixel"
#define AAVALUESC "Hodnoty:  e.g.  1,3,8,16,24"
#define AADELETE "Odstr·niù"

#define AABOUNDINGBOX "BoundingBox"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "RozöÌrenÈ nastavenia"
#define AAGHOSTSCRIPTDLLC "Ghostscript DLL:"
#define AAGHOSTSCRIPTSOC "Ghostscript zdielan˝ objekt:"
#define AAGHOSTSCRIPTEXEC "Program Ghostscript:"
#define AAGHOSTSCRIPTINCC "Ghostscript Include Path:"
#define AAGHOSTSCRIPTOTHERC "Moûnosti Ghostscript:"
#define AACOPYPRINTERDEF "Copy Printer Defaults"
#define AAASSOCIATEPS "Asociovaù .ps s˙bory s programom GSview"
#define AAASSOCIATEPDF "Associovaù .pdf s˙bory s programom GSview"
#define AACREATESTARTMENU "Vytvoriù programov˙ skupinu v ponuke ätart"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "JednoduchÈ nastavenia"
#define AAWHICHGS "Ktor˙ verziu programu Ghostscript chcete pouûÌvaù?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Preberanie Ghostscript"
#define AADOWNLOADNOW "Prevziaù"
#define AAGSNOTFOUND "Ghostscript sa nenaöiel na Vaöom poËÌtaËi.  MusÌte prevziaù program AFPL Ghostscript a potom ho naiötalovaù"

#define AAGSMESSWIN "GSview pre Windows - Ghostscript spr·vy"
#define AAGSMESSPM "PM GSview - Ghostscript spr·vy"
#define AAGSMESSX11 "X11 GSview - Ghostscript spr·vy"
#define AACOPY "KopÌrovaù"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Nastavenia zobrazenia"
#define AARESOLUTION "RozlÌûenie"
#define AAZOOMRESOLUTION "Zoom Resolution"
#define AADEPTH "HÂbka"
#define AATEXTALPHA "Textov˝ znak"
#define AAGRAPHICSALPHA "Grafick˝ znak"
#define AADRAWMETHOD "MetÛda kreslenia"
#define AADPI "dpi"
#define AABPP "bitov/pixel"
#define AABITS "bitov"

/* Measure and custom units dialog box */
#define AAPT "pt"
#define AAMM "mm"
#define AAINCH "inch"
#define AACUSTOM "Custom"
#define AAMEASURE "Measure"
#define AAUNITC	"Unit:"
#define AASTARTC "Start:"
#define AAFINISHC "Finish:"
#define AADELTAC "Delta:"
#define AALENGTHC "Length:"
#define AACHANGE "Change"
#define AACALCXFORM "Prer·tavaù zmenu"
#define AATRANSLATE "Prer·tvaù"
#define AAROTATE "Rotovaù"
#define AASCALE "Scale"
#define AAINITMATRIX "Initmatrix"
#define AAINVERTMATRIX "Invertmatrix"

/* PS to Edit dialog box */
#define AAPSTOEDIT "PS do Edit"
#define AAFORMATC "Form·t:"
#define AADRAWTASP "Kreslyù text ak mnuhouholnÌk"
#define AAMAPTOLATIN1 "Mapovaù do ISO-Latin1"
#define AAFLATNESSC "Plochosù:"
#define AADEFAULTFONTC "ätandardnÈ pÌsmo:"
#define AADRIVEROPTIONSC "Moûnosti ovl·daËa:"

/* Registration */
#define AAREGTOC "RegistrovanÈ na:"
#define AANUMBERC "»Ìslo:"
#define AAGSVIEWREG "GSview Registr·cia"
#define AAREGISTERNOW "Registrovaù"
#define AAONLINEREG "On-line registr·cia"
#define AAREG1 "UrËte prosÌm Vaöe meno a ËÌslo pre registr·ciu"
#define AAREG2 ""
#define AANAG1 "GSview autorskÈ pr·va 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Popdporte prosÌm v˝voj programu GSview jeho registr·ciou."
#define AANAG3 ""
#define AANAG4 "GSview mÙûete rigistrovaù aj on-line"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "    http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "GSview inötal·cia"
#define AAINSTALLINTRO1 "Tento Sprievodca V·m pomÙûe naiötalovaù"
#define AAINSTALLINTRO2 "If you are not using the self extracting EXE install, make sure you have the files listed in Readme.htm in the current directory."
#define AAINSTALLINTRO3 "T·to inötal·cia potrebuje 20 - 30 MB voænÈho miesta na disku."
#define AAINSTALLCOPYRIGHT1 "Pozn·mka k autorsk˝m pr·vam"
#define AAINSTALLCOPYRIGHT2 "GSview a AFPL Ghostscript s˙ oddelenÈ programy."
#define AAINSTALLCOPYRIGHT3 "S˙ to vlastne oddelenÈ celky."
#define AAINSTALLGSVER1 "KtorÈ s˙Ëasti chcete inötalovaù?"
#define AAINSTALLGSVER2 "Inötaluje sa GSview"
#define AAINSTALLGSVER3 "Inötaluje sa Ghostscript"
#define AAINSTALLGSVER4 "Ktor˙ verziu programu Ghostscript chcete pouûÌvaù?"
#define AAINSTALLDIR1 "Vyberte prieËinok do ktorÈho chcete inötalovaù GSview a Ghostscript."
#define AAINSTALLDIR2 "Within this directory, GSview will be in the subdirectory:"
#define AAINSTALLDIR3 "Ghostscript will be in the subdirectory:"
#define AAINSTALLMKDIR1 "UrËen˝ prieËinok neexistuje."
#define AAINSTALLMKDIR2 "Nov˝ prieËinok bude vytvoren˝." 
#define AAINSTALLMISSING1 "GSview inötal·cia - Ch˝ba s˙bor ZIP"
#define AAINSTALLMISSING2 "Ned· sa n·jsù s˙bor ZIP.  Vloûte disk so s˙borom."
#define AAINSTALLCONFIG1 "GSview a Ghostscript puûÌvaj˙ prieËinok pre doËasnÈ s˙bory."
#define AAINSTALLCONFIG2 "Nem·te urËen˝ prieËinok pre doËasnÈ s˙bory."
#define AAINSTALLCONFIG3 "Nasleduj˙ci riadok bude pridan˝ do sboru autoexec.bat\r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Aktualizovaù autoexec.bat"
#define AAINSTALLCONFIG5 "Vytvoriù z·lohu starÈho s˙boru autoexec.bat"
#define AAINSTALLCONFIGPM1 "GSview A Ghostscript puûÌvaj˙ prieËinok pre doËasnÈ s˙bory."
#define AAINSTALLCONFIGPM2 "Nem·te urËen˝ prieËinok pre doËasnÈ s˙bory, alebo musÌte nainnötalovaù EMX."
#define AAINSTALLCONFIGPM3 "Nasleduj˙ci riadok bude pridan˝ do sboru config.sys   'SET TEMP=C:\\'  and/or your PATH and LIBPATH will be updated for EMX."
#define AAINSTALLCONFIGPM4 "Aktualizovaù config.sys"
#define AAINSTALLCONFIGPM5 "Vytvoriù z·lohu starÈho s˙boru config.sys"
#define AAINSTALLFINISH "GSview inötalaËn˝ program mÙûe zaËaù inötal·ciu.  \r\rKliknite na tlaËidlo DokonËiù pre pokraËovanie."
#define AAINSTALLUNZIP1 "Extrahuj˙ sa s˙bory..."
#define AADIRNOTEXIST "PrieËinok '%s' neexistuje.  m· sa vytvoriù?"
#define AAMKDIRFAIL "Ned· sa vytvoriù prieËinok"
#define AABETAWARN "Toto je BETA verzia programu GSview.  Bude vypnutÈ %04d-%02d-%02d."
#define AAINSTALLOK "Inötal·cia ˙speöne dokonËen·."
#define AAINSTALLFAILED "Nastala chyba poËas inötal·cie"
#define AAPROGMANGROUP1 "Vytv·raj˙ sa poloûky v ponuke ätart."
#define AAPROGMANGROUP2 "Create Group / Item"
#define AAPROGMANGROUP3 "N·zov"
#define AAPROGMANGROUP4 "GS n·stroje"
#define AAPROGMANGROUP5 "Programov· skupina s n·zvom \042%s\042 bola vytvoren·."
#define AAPROGMANGROUP6 "Odkaz na pracovnej ploche s n·zvom \042GSview\042 bol vytvoren˝."
#define AAPMOBJECT1 "Ned· sa vytvoriù idkaz na pracovnej ploche pre program GSview."
#define AAPMOBJECT2 "Vytv·ra sa programov·n skupina programu GSview."
#define AADONEBAK "Star˝ s˙bor autoexec.bat bol premenovan˝ na %s"
#define AACANTLOAD "Ned· sa nahraù %s"
#define AACANTCREATETEMPFILE "Ned· sa vytvoriù doËasn˝ n·zov s˙boru"
#define AACANTOPENREAD "Ned· a otvoriù %s pre ËÌtanie"
#define AACANTOPENWRITE "Ned· sa otvoriù %s pre z·pis"
#define AAERRORRENAME "Chyba pri premenov·vanÌ %s na %s"
#define AANODDEPROGMAN "Ned· sa otvoriù DDE spoje nie na spr·vacu s˙borov.  SpustitesystÈm Windows znovu."
#define AAINSERTDISK "Vloûte disk obsahuj˙ci %s"
#define AAZIPNOTFOUND "S˙bor Zip sa ned· n·jsù"
#define AAUNZIPCANCELLED "Extrahovanie zruöenÈ\n"
#define AACANTALLOCBUF "Ned· sa vyhradiù pam‰ù pre kopÌrovanie vyrovn·vacej pam‰te"
#define AAPROGRAMOBJECTFAILED "Ned· sa vytvoriù odkaz na pracovnej ploche pre program GSview"
#define AAUNINSTALLTITLE "GSview a AFPL Ghostscript"
#define AAUNINSTALLITEM "Odinötalovaù GSview a AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "T·to inötal·cia potrebuje 5MB na disku"
#define AAGSVIEWREQUIRES "Program GSview tieû potrebuje"
#define AAVIEWREADMEDETAILS "Pozrite si s˙bor Readme ako zÌskaù program Ghostscript."
#define AACOPYRIGHTNOTICE "AutorskÈ pr·va"
#define AASELECTGSVIEWDIR "Vyberte prieËinok do ktorÈho sa nainötaluje GSview"
#define AABROWSEE "Prehæad·vaù..."
#define AAGSVIEWADDSTARTMENU "Inötal·tor prid· nasleduj˙ce poloûky do programovej skupiny"
#define AACREATEFOLDER "Vytv·ra sa prieËinok"
#define AAALLUSERS "Vöetci pouûÌvatelia"
#define AASELECTFOLDER "Vybraù prieËinok"
#define AAFOLDER "PrieËinok"
#define AADESTFOLDER "Cieæov˝ prieËinok"
#define AAVIEWREADME "Zobraziù Readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "GSview nastavenia"
#define AACFG10 "Tento sprievodca V·m pomÙûe s nastaveniami preprogram GSview."
#define AACFG11 "Ak neporozumiete poloûk·m na nasleduj˙cek strane,\
 tak ich nemente.  ätandardnÈ s˙ tieû rozumnÈ."
#define AACFG20 "GSview proterbuje vedieù aku verziu Ghostscript pouûÌvate\
 ak kde sa nach·dza."
#define AACFG21 "Ktor˙ verziu programu Ghostscript chcete pouûÌvaù?"
#define AACFG22 "V ktorom prieËinku je umiesten˝ program Ghostscript?"
#define AACFG23 "Hæadaù Ôaöie typy 1 pÌsma v"
#define AACFG30 "Program GSview si ponech· zoznam Ghostscript tlaËiarnÌ\
 a rozlÌûenÌ v jeho inicializaËnom s˙bore."
#define AACFG31 "The default list of devices and resolutions is taken from the standard\
 distribution version of Ghostscript 5.50 and may not be complete."
#define AACFG32 "Aktualizovaù zoznam talËiarnÌ programu GSview" 
#define AACFG33 "Aktualizuje sa zoznam talËiarnÌ programu GSview, zoznam tlaËiarnÌ bude nahraden˝ nov˝m." 
#define AACFG40 "Ned· sa vytvoriù asocia·cia medzi programom GSview a s˙bormi PS a\
 PDF."
#define AACFG43 "With these associations, double clicking on a PostScript\
 or PDF file will start GSview.\
  These associations also allow some WWW browsers\
 to use GSview as a viewer for PostScript and PDF files."
#define AACFG41 "Associate PostScript (.ps and .eps) files with GSview."
#define AACFG42 "Associate PDF (.pdf) files with GSview."
#define AACFG50 "GSview can create a Program Manager Group or\
 Start Menu Item for GSview and AFPL Ghostscript."
#define AACFG51 "Create Group / Item"
#define AACFG52 "N·zov"
#define AACFG53 "GSview can create a Desktop Program Object for GSview. \
 This will associate PostScript (.ps and .eps) and PDF (.pdf) files with GSview."
#define AACFG54 "Create Program Object"
#define AACFG60 "GSview is now ready to start configuration. \r\rPress Finish to continue."
#define AACFG70 "Configuration successful."
#define AACFG72 "If you need to change the configuration later,\
 run GSview then select \042Options | Easy Configure...\042 or\
 \042Options | Advanced Configure...\042."
#define AACFG73 "Configuration failed."
#define AACFG74 "Configuration cancelled."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "GSview tlaËiareÚ"
#define AAGSVIEWPRINTUSAGE "Usage: %s [/d] dllpath optionfile inputfile\noptionfile and inputfile will be deleted on exit\nIt is intended that gvpgs be called with temporary files\n"
#ifdef _Windows
#define AAMFILE "&S˙bor"
#define AAMSAVEAS "Uloûiù &ako..."
#define AAMEXIT "&SkonËiù"
#define AAMEDIT "⁄pr&avy"
#define AAMCOPY "KopÌrovaù\tCtrl+C"
#define AAMHELP "&PomocnÌk"
#define AAMABOUT "»&o je"
#else
#define AAMFILE "~S˙bor"
#define AAMSAVEAS "Uloûiù ~ako..."
#define AAMEXIT "~SkonËiù"
#define AAMEDIT "⁄pr~avy"
#define AAMCOPY "KopÌrovaù\tCtrl+C"
#define AAMHELP "~PomocnÌk"
#define AAMABOUT "»~o je"
#endif

#define WINHELPFILE "gsviewsk.chm"
