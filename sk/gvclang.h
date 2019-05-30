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

#define GSVIEW_COPYRIGHT1 "Autorsk� pr�va (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "V�etky pr�va vyhraden�."
#define GSVIEW_COPYRIGHT3 "Pozrite si s�bor LICENCE pre viac inform�ci�."

#define GS_COPYRIGHT1 "Autorsk� pr�va (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A.  V�etky pr�va vyhraden�."
#define GS_COPYRIGHT3 "Pozrite si s�bor PUBLIC pre viac inform�ci�."

/* Buttons */
#ifdef UNIX
#define AAOK "OK"
#define AACANCEL "Zru�i�"
#define AAEDIT "�pravy"
#define AADEFAULTS "�tandardn�"
#define AAHELP "Pomocn�k"
#define AANEW "Nov�"
#define AATEST "Test"
#define AAYES "�no"
#define AANO "Nie"
#define AANEXTGT "�alej >"
#define AABACKLT "< Nasp�"
#define AAFINISHGT "Dokon�i� >"
#define AAEXIT "Skon�i�"
#else
#ifdef _Windows
#define AAOK "&OK"
#define AACANCEL "&Zru�i�"
#define AAEDIT "�pr&avy"
#define AADEFAULTS "&�tandardn�"
#define AAHELP "&Pomocn�k"
#define AANEW "&Nov�"
#define AATEST "&Test"
#define AAYES "�n&o"
#define AANO "&Nie"
#define AANEXTGT "�&alej >"
#define AABACKLT "< Na&sp�"
#define AAFINISHGT "&Dokon�i� >"
#define AAEXIT "&Skon�i�"
#else
#define AAOK "~OK"
#define AACANCEL "~Zru�i�"
#define AADEFAULTS "�~tandardn�"
#define AAEDIT "�pr~avy"
#define AAHELP "~Pomocn�k"
#define AANEW "~Nov�"
#define AATEST "~Test"
#define AAYES "�n~o"
#define AANO "~Nie"
#define AANEXTGT "�a~lej >"
#define AABACKLT "< Na~sp�"
#define AAFINISHGT "~Dokon�i� >"
#define AAEXIT "~Skon�i�"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "�o je..."
#define AAABOUTPM "�o je PM GSview"
#define AAABOUTX11 "�o je X11 GSview"
#define AAABOUTGSV16SPL "�o je GSV16SPL"
#define AAABOUTGSVIEWPRINT "�o je GSview tla�"
#define AACOPY1 "Verzia:"
#define AACOPY2 "Grafick� prostredie pre program Ghostscript"
#define AACOPY4 "Tento program je distribuovan� NO WARRANTY OF ANY KIND."
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
#define AACOPY15 "pstotxt3.dll autorsk� pr�va (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY16 "pstotxt2.dll autorsk� pr�va (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY17 "pstotxt1.dll autorsk� pr�va (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY18 "and has a different licence.  Pozrite si s�bor pstotext.txt pre viac inform�ci�."
#define AACOPY19 "Autor: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Ghostscript DLL prostredie."
#define AACOPY21 "Tento program je �as�ou programu GSview."
#define AACOPY22 "GSview Win32s/Win16 spooler"

#define AAINPUT "Vstup"

/* DSC error dialog box */
#define AADSC "Dokument Structuring Conventions"
#define AAIGNOREALLDSC "Ignorova� v�etky DSC"

/* Info dialog box */
#define AAINFO  "Inform�cie"
#define AAFILEC "S�bor:"
#define AATYPEC "Typ:"
#define AATITLEC "N�zov:"
#define AADATEC "D�tum:"
#define AABOUNDINGBOXC "BoundingBox:"
#define AAORIENTATIONC "Orient�cia:"
#define AADEFAULTMEDIAC "�tandardn� m�dium:"
#define AAPAGEORDERC "Page Order:"
#define AAPAGESC "Po�et str�n:"
#define AAPAGEC "Aktu�lna strana:"
#define AABITMAPC "Bitov� mapa:"

/* Sounds dialog box */
#define AASOUNDS "Zvuky"
#define AAEVENTC "Udalos�:"
#define AASOUNDC "Zvuk:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Vybra� port tla�iarne"
#define AASELECTPRINTER "Vybra� tla�iare�"
#define AAPRINTING "Tla�� sa"
#define AASELECTPAGE "Vybra� starnu"
#define AASELECTPAGES "Vybra� strany"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Nastavenie tla�e"
#define AADEVICEC "Zariadenie:"
#define AARESOLUTIONC "Rozl�enie:"
#define AAQUEUEC "Rad:"
#define AAPRINTTOFILE "Tla�i� do s�boru"
#define AAPSPRINTER "PostScript tla�iare�"
#define AAOPTIONSC "Mo�nosti:"
#define AAFIXEDMEDIA "Fixed media"
#define AACONVERT "Konvertova�"
#define AAWINPRINTERSETTINGS "Windows nastvenia tla�iarne"
#define AACOLOURS "Farba"
#define AABANDW "�iernobielo"
#define AAGREY "Siv�"
#define AAFULLCOLOUR "Farebne"
#define AASELECTGSDEVICE "Vybra� Ghostscript zariadenie"

#define AAODDEVEN "Nep�rne/P�rne"
#define AAIGNOREDSC "Ignorova� DSC"
#define AAPAGES "Strany"
#define AAFROMC "Od:"
#define AATOC "do:"

#define AAADVANCED "Roz��ren�"
#define AAADVANCEDPSOPT "Roz��ren� mo�nosti PostScript"
#define AASENDCTRLDBEFORE "Send CTRL+D before job"
#define AASENDCTRLDAFTER "Send CTRL+D after job"
#define AAPROLOGFILE "Prolog s�bor"
#define AAEPILOGFILE "Epilog s�bor"
#define AABROWSE "Preh�ad�va�"

/* PS2EPS dialog box */
#define AAPSTOEPS "PS do EPS"
#define AAPSTOEPSREAD "Chcete zobrazi� Pomocn�ka o 'PS to EPS'?"
#define AAPSTOEPSAUTO "Automaticky r�ta� Bounding Box"

#ifdef UNIX
#define AAAPROPERTIES "Vlastnosti"
#define AAALLPAGES "V�etky strany"
#define AAODDPAGES "Nep�rne strany"
#define AAEVENPAGES "P�rne strany"
#define AAALL "V�etky"
#define AAODD "Nep�rne"
#define AAEVEN "P�rne"
#define AAREVERSE "Reverse"
#else
#ifdef _Windows
#define AAAPROPERTIES "&Vlastnosti"
#define AAALLPAGES "&V�etky strany"
#define AAODDPAGES "&Nep�rne strany"
#define AAEVENPAGES "&P�rne strany"
#define AAALL "&V�etky"
#define AAODD "&Nep�rne"
#define AAEVEN "&P�rne"
#define AAREVERSE "&Reverse"
#else
#define AAAPROPERTIES "~Vlastnosti"
#define AAALLPAGES "~V�etky strany"
#define AAODDPAGES "~Nep�rne strany"
#define AAEVENPAGES "~P�rne strany"
#define AAALL "~V�etky"
#define AAODD "~Nep�rne"
#define AAEVEN "~P�rne"
#define AAREVERSE "~Reverse"
#endif
#endif


/* Properties dialog box */
#define AAPROPERTIES "Vlastnosti"
#define AAPROPERTYC "Vlastnos�:"
#define AAVALUEC "Hodnota:"
#define AAPAGEOFFSETPT "Page Offset (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Upravi� vlastnosti"
#define AAPROPERTYTYPEC "Typ vlastnosti:"
#define AANUMBER "��slo"
#define AASTRING "Re�azec"
#define AANAMEC "N�zov: e.g.  BitsPerPixel"
#define AAVALUESC "Hodnoty:  e.g.  1,3,8,16,24"
#define AADELETE "Odstr�ni�"

#define AABOUNDINGBOX "BoundingBox"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Roz��ren� nastavenia"
#define AAGHOSTSCRIPTDLLC "Ghostscript DLL:"
#define AAGHOSTSCRIPTSOC "Ghostscript zdielan� objekt:"
#define AAGHOSTSCRIPTEXEC "Program Ghostscript:"
#define AAGHOSTSCRIPTINCC "Ghostscript Include Path:"
#define AAGHOSTSCRIPTOTHERC "Mo�nosti Ghostscript:"
#define AACOPYPRINTERDEF "Copy Printer Defaults"
#define AAASSOCIATEPS "Asociova� .ps s�bory s programom GSview"
#define AAASSOCIATEPDF "Associova� .pdf s�bory s programom GSview"
#define AACREATESTARTMENU "Vytvori� programov� skupinu v ponuke �tart"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Jednoduch� nastavenia"
#define AAWHICHGS "Ktor� verziu programu Ghostscript chcete pou��va�?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Preberanie Ghostscript"
#define AADOWNLOADNOW "Prevzia�"
#define AAGSNOTFOUND "Ghostscript sa nena�iel na Va�om po��ta�i.  Mus�te prevzia� program AFPL Ghostscript a potom ho nai�talova�"

#define AAGSMESSWIN "GSview pre Windows - Ghostscript spr�vy"
#define AAGSMESSPM "PM GSview - Ghostscript spr�vy"
#define AAGSMESSX11 "X11 GSview - Ghostscript spr�vy"
#define AACOPY "Kop�rova�"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Nastavenia zobrazenia"
#define AARESOLUTION "Rozl�enie"
#define AAZOOMRESOLUTION "Zoom Resolution"
#define AADEPTH "H�bka"
#define AATEXTALPHA "Textov� znak"
#define AAGRAPHICSALPHA "Grafick� znak"
#define AADRAWMETHOD "Met�da kreslenia"
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
#define AACALCXFORM "Prer�tava� zmenu"
#define AATRANSLATE "Prer�tva�"
#define AAROTATE "Rotova�"
#define AASCALE "Scale"
#define AAINITMATRIX "Initmatrix"
#define AAINVERTMATRIX "Invertmatrix"

/* PS to Edit dialog box */
#define AAPSTOEDIT "PS do Edit"
#define AAFORMATC "Form�t:"
#define AADRAWTASP "Kresly� text ak mnuhouholn�k"
#define AAMAPTOLATIN1 "Mapova� do ISO-Latin1"
#define AAFLATNESSC "Plochos�:"
#define AADEFAULTFONTC "�tandardn� p�smo:"
#define AADRIVEROPTIONSC "Mo�nosti ovl�da�a:"

/* Registration */
#define AAREGTOC "Registrovan� na:"
#define AANUMBERC "��slo:"
#define AAGSVIEWREG "GSview Registr�cia"
#define AAREGISTERNOW "Registrova�"
#define AAONLINEREG "On-line registr�cia"
#define AAREG1 "Ur�te pros�m Va�e meno a ��slo pre registr�ciu"
#define AAREG2 ""
#define AANAG1 "GSview autorsk� pr�va 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Popdporte pros�m v�voj programu GSview jeho registr�ciou."
#define AANAG3 ""
#define AANAG4 "GSview m��ete rigistrova� aj on-line"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "    http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "GSview in�tal�cia"
#define AAINSTALLINTRO1 "Tento Sprievodca V�m pom��e nai�talova�"
#define AAINSTALLINTRO2 "If you are not using the self extracting EXE install, make sure you have the files listed in Readme.htm in the current directory."
#define AAINSTALLINTRO3 "T�to in�tal�cia potrebuje 20 - 30 MB vo�n�ho miesta na disku."
#define AAINSTALLCOPYRIGHT1 "Pozn�mka k autorsk�m pr�vam"
#define AAINSTALLCOPYRIGHT2 "GSview a AFPL Ghostscript s� oddelen� programy."
#define AAINSTALLCOPYRIGHT3 "S� to vlastne oddelen� celky."
#define AAINSTALLGSVER1 "Ktor� s��asti chcete in�talova�?"
#define AAINSTALLGSVER2 "In�taluje sa GSview"
#define AAINSTALLGSVER3 "In�taluje sa Ghostscript"
#define AAINSTALLGSVER4 "Ktor� verziu programu Ghostscript chcete pou��va�?"
#define AAINSTALLDIR1 "Vyberte prie�inok do ktor�ho chcete in�talova� GSview a Ghostscript."
#define AAINSTALLDIR2 "Within this directory, GSview will be in the subdirectory:"
#define AAINSTALLDIR3 "Ghostscript will be in the subdirectory:"
#define AAINSTALLMKDIR1 "Ur�en� prie�inok neexistuje."
#define AAINSTALLMKDIR2 "Nov� prie�inok bude vytvoren�." 
#define AAINSTALLMISSING1 "GSview in�tal�cia - Ch�ba s�bor ZIP"
#define AAINSTALLMISSING2 "Ned� sa n�js� s�bor ZIP.  Vlo�te disk so s�borom."
#define AAINSTALLCONFIG1 "GSview a Ghostscript pu��vaj� prie�inok pre do�asn� s�bory."
#define AAINSTALLCONFIG2 "Nem�te ur�en� prie�inok pre do�asn� s�bory."
#define AAINSTALLCONFIG3 "Nasleduj�ci riadok bude pridan� do sboru autoexec.bat\r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Aktualizova� autoexec.bat"
#define AAINSTALLCONFIG5 "Vytvori� z�lohu star�ho s�boru autoexec.bat"
#define AAINSTALLCONFIGPM1 "GSview A Ghostscript pu��vaj� prie�inok pre do�asn� s�bory."
#define AAINSTALLCONFIGPM2 "Nem�te ur�en� prie�inok pre do�asn� s�bory, alebo mus�te nainn�talova� EMX."
#define AAINSTALLCONFIGPM3 "Nasleduj�ci riadok bude pridan� do sboru config.sys   'SET TEMP=C:\\'  and/or your PATH and LIBPATH will be updated for EMX."
#define AAINSTALLCONFIGPM4 "Aktualizova� config.sys"
#define AAINSTALLCONFIGPM5 "Vytvori� z�lohu star�ho s�boru config.sys"
#define AAINSTALLFINISH "GSview in�tala�n� program m��e za�a� in�tal�ciu.  \r\rKliknite na tla�idlo Dokon�i� pre pokra�ovanie."
#define AAINSTALLUNZIP1 "Extrahuj� sa s�bory..."
#define AADIRNOTEXIST "Prie�inok '%s' neexistuje.  m� sa vytvori�?"
#define AAMKDIRFAIL "Ned� sa vytvori� prie�inok"
#define AABETAWARN "Toto je BETA verzia programu GSview.  Bude vypnut� %04d-%02d-%02d."
#define AAINSTALLOK "In�tal�cia �spe�ne dokon�en�."
#define AAINSTALLFAILED "Nastala chyba po�as in�tal�cie"
#define AAPROGMANGROUP1 "Vytv�raj� sa polo�ky v ponuke �tart."
#define AAPROGMANGROUP2 "Create Group / Item"
#define AAPROGMANGROUP3 "N�zov"
#define AAPROGMANGROUP4 "GS n�stroje"
#define AAPROGMANGROUP5 "Programov� skupina s n�zvom \042%s\042 bola vytvoren�."
#define AAPROGMANGROUP6 "Odkaz na pracovnej ploche s n�zvom \042GSview\042 bol vytvoren�."
#define AAPMOBJECT1 "Ned� sa vytvori� idkaz na pracovnej ploche pre program GSview."
#define AAPMOBJECT2 "Vytv�ra sa programov�n skupina programu GSview."
#define AADONEBAK "Star� s�bor autoexec.bat bol premenovan� na %s"
#define AACANTLOAD "Ned� sa nahra� %s"
#define AACANTCREATETEMPFILE "Ned� sa vytvori� do�asn� n�zov s�boru"
#define AACANTOPENREAD "Ned� a otvori� %s pre ��tanie"
#define AACANTOPENWRITE "Ned� sa otvori� %s pre z�pis"
#define AAERRORRENAME "Chyba pri premenov�van� %s na %s"
#define AANODDEPROGMAN "Ned� sa otvori� DDE spoje nie na spr�vacu s�borov.  Spustitesyst�m Windows znovu."
#define AAINSERTDISK "Vlo�te disk obsahuj�ci %s"
#define AAZIPNOTFOUND "S�bor Zip sa ned� n�js�"
#define AAUNZIPCANCELLED "Extrahovanie zru�en�\n"
#define AACANTALLOCBUF "Ned� sa vyhradi� pam� pre kop�rovanie vyrovn�vacej pam�te"
#define AAPROGRAMOBJECTFAILED "Ned� sa vytvori� odkaz na pracovnej ploche pre program GSview"
#define AAUNINSTALLTITLE "GSview a AFPL Ghostscript"
#define AAUNINSTALLITEM "Odin�talova� GSview a AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "T�to in�tal�cia potrebuje 5MB na disku"
#define AAGSVIEWREQUIRES "Program GSview tie� potrebuje"
#define AAVIEWREADMEDETAILS "Pozrite si s�bor Readme ako z�ska� program Ghostscript."
#define AACOPYRIGHTNOTICE "Autorsk� pr�va"
#define AASELECTGSVIEWDIR "Vyberte prie�inok do ktor�ho sa nain�taluje GSview"
#define AABROWSEE "Preh�ad�va�..."
#define AAGSVIEWADDSTARTMENU "In�tal�tor prid� nasleduj�ce polo�ky do programovej skupiny"
#define AACREATEFOLDER "Vytv�ra sa prie�inok"
#define AAALLUSERS "V�etci pou��vatelia"
#define AASELECTFOLDER "Vybra� prie�inok"
#define AAFOLDER "Prie�inok"
#define AADESTFOLDER "Cie�ov� prie�inok"
#define AAVIEWREADME "Zobrazi� Readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "GSview nastavenia"
#define AACFG10 "Tento sprievodca V�m pom��e s nastaveniami preprogram GSview."
#define AACFG11 "Ak neporozumiete polo�k�m na nasleduj�cek strane,\
 tak ich nemente.  �tandardn� s� tie� rozumn�."
#define AACFG20 "GSview proterbuje vedie� aku verziu Ghostscript pou��vate\
 ak kde sa nach�dza."
#define AACFG21 "Ktor� verziu programu Ghostscript chcete pou��va�?"
#define AACFG22 "V ktorom prie�inku je umiesten� program Ghostscript?"
#define AACFG23 "H�ada� �a�ie typy 1 p�sma v"
#define AACFG30 "Program GSview si ponech� zoznam Ghostscript tla�iarn�\
 a rozl�en� v jeho inicializa�nom s�bore."
#define AACFG31 "The default list of devices and resolutions is taken from the standard\
 distribution version of Ghostscript 5.50 and may not be complete."
#define AACFG32 "Aktualizova� zoznam tal�iarn� programu GSview" 
#define AACFG33 "Aktualizuje sa zoznam tal�iarn� programu GSview, zoznam tla�iarn� bude nahraden� nov�m." 
#define AACFG40 "Ned� sa vytvori� asocia�cia medzi programom GSview a s�bormi PS a\
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
#define AACFG52 "N�zov"
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
#define AAGSVIEWPRINT "GSview tla�iare�"
#define AAGSVIEWPRINTUSAGE "Usage: %s [/d] dllpath optionfile inputfile\noptionfile and inputfile will be deleted on exit\nIt is intended that gvpgs be called with temporary files\n"
#ifdef _Windows
#define AAMFILE "&S�bor"
#define AAMSAVEAS "Ulo�i� &ako..."
#define AAMEXIT "&Skon�i�"
#define AAMEDIT "�pr&avy"
#define AAMCOPY "Kop�rova�\tCtrl+C"
#define AAMHELP "&Pomocn�k"
#define AAMABOUT "�&o je"
#else
#define AAMFILE "~S�bor"
#define AAMSAVEAS "Ulo�i� ~ako..."
#define AAMEXIT "~Skon�i�"
#define AAMEDIT "�pr~avy"
#define AAMCOPY "Kop�rova�\tCtrl+C"
#define AAMHELP "~Pomocn�k"
#define AAMABOUT "�~o je"
#endif

#define WINHELPFILE "gsviewsk.chm"
