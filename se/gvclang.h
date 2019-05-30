/* Copyright (C) 1993-2012, Ghostgum Software Pty Ltd.  Alla r�ttigheter reserverade.

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

/* se\gvclang.h */
/* Common Swedish language defines */

#define AASVENSKA "Svenska"

#define GSVIEW_COPYRIGHT1 "Copyright (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "Alla r�ttigheter reserverade."
#define GSVIEW_COPYRIGHT3 "Se filen LICENCE f�r fler detaljer."

#define GS_COPYRIGHT1 "Copyright (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A.  Alla r�ttigheter reserverade."
#define GS_COPYRIGHT3 "Se filen PUBLIC f�r fler detaljer."

/* Buttons */
#ifdef UNIX
#define AAOK "Ok"
#define AACANCEL "�ngra"
#define AAEDIT "Redigera"
#define AADEFAULTS "Standard"
#define AAHELP "Hj�lp"
#define AANEW "Nytt"
#define AATEST "Test"
#define AAYES "Ja"
#define AANO "Nej"
#define AANEXTGT "N�sta >"
#define AABACKLT "< Bak�t"
#define AAFINISHGT "Klart >"
#define AAEXIT "Avsluta"
#else
#ifdef _Windows
#define AAOK "&Ok"
#define AACANCEL "&�ngra"
#define AAEDIT "&Redigera"
#define AADEFAULTS "&Standard"
#define AAHELP "&Hj�lp"
#define AANEW "N&ytt"
#define AATEST "&Test"
#define AAYES "&Ja"
#define AANO "&Nej"
#define AANEXTGT "N&�sta >"
#define AABACKLT "< &Bak�t"
#define AAFINISHGT "K&lart >"
#define AAEXIT "Av&sluta"
#else
#define AAOK "~Ok"
#define AACANCEL "~�ngra"
#define AADEFAULTS "~Standard"
#define AAEDIT "~Redigera"
#define AAHELP "~Hj�lp"
#define AANEW "N~ytt"
#define AATEST "~Test"
#define AAYES "~Ja"
#define AANO "~Nej"
#define AANEXTGT "N~�sta >"
#define AABACKLT "< ~Bak�t"
#define AAFINISHGT "K~lart >"
#define AAEXIT "Av~sluta"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "Om GSview f�r Windows"
#define AAABOUTPM "Om PM GSview"
#define AAABOUTX11 "Om X11 GSview"
#define AAABOUTGSV16SPL "Om GSV16SPL"
#define AAABOUTGSVIEWPRINT "Om GSview utskrift"
#define AACOPY1 "Version:"
#define AACOPY2 "Ett grafiskt Ghostscript interface"
#define AACOPY4 "Detta program distribueras UTAN GARANTI AV N�GOT SLAG."
#define AACOPY5 "Ingen f�rfattare eller distribut�r ansvarar f�r"
#define AACOPY6 "konsekvenserna av att anv�nda den, eller l�mpligheten f�r n�got"
#define AACOPY7 "som helst �ndam�l eller arbete, undantaget att han eller hon uttryckligen sagt s� i skrift."
#define AACOPY8 "Referera till GSview Free Public Licence ('Licensen') f�r alla detaljer."
#define AACOPY9 "Varje kopia av GSview m�ste inkludera en kopia av licensen, normalt"
#define AACOPY10 "i en ren ASCII textfil kallad LICENCE.  Licensen ger dig"
#define AACOPY11 "r�ttigheter att kopiera, modifiera och �terdistribuera GSview, men endast under"
#define AACOPY12 "vissa villkor som beskrivs i licensen.  Bland annat,"
#define AACOPY13 "kr�ver licensen att copyright notis och denna notis skall"
#define AACOPY14 "finnas med hos alla kopior."
#define AACOPY15 "pstotxt3.dll �r Copyright f�r (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY16 "pstotxt2.dll �r Copyright f�r (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY17 "pstotxt1.dll �r Copyright f�r (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY18 "och har en annorlunda licens.  Se pstotext.txt f�r detaljer."
#define AACOPY19 "F�rfattare: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Ett Ghostscript DLL interface."
#define AACOPY21 "Detta program �r en del av GSview."
#define AACOPY22 "GSview Win32s/Win16 spooler"

#define AAINPUT "Input"

/* DSC error dialog box */
#define AADSC "Document Structuring conventions"
#define AAIGNOREALLDSC "Ignorera alla DSC kommentarer"

/* Info dialog box */
#define AAINFO  "Info"
#define AAFILEC "Fil:"
#define AATYPEC "Typ:"
#define AATITLEC "Titel:"
#define AADATEC "Datum:"
#define AABOUNDINGBOXC "BoundingBox:"
#define AAORIENTATIONC "Orientering:"
#define AADEFAULTMEDIAC "Standard media:"
#define AAPAGEORDERC "Sidordning:"
#define AAPAGESC "Sidor:"
#define AAPAGEC "Sida:"
#define AABITMAPC "Bitmapp:"

/* Sounds dialog box */
#define AASOUNDS "Ljud"
#define AAEVENTC "H�ndelse:"
#define AASOUNDC "Ljud:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "V�lj skrivarport"
#define AASELECTPRINTER "V�lj skrivare"
#define AAPRINTING "Utskrift"
#define AASELECTPAGE "V�lj sida"
#define AASELECTPAGES "V�lj sidor"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Skrivare inst�llningar"
#define AADEVICEC "Device:"
#define AARESOLUTIONC "Uppl�sning:"
#define AAQUEUEC "K�:"
#define AAPRINTTOFILE "Skriv ut till fil"
#define AAPSPRINTER "PostScript skrivare"
#define AAOPTIONSC "Alternativ:"
#define AAFIXEDMEDIA "Fast media"
#define AACONVERT "Konvertera"
#define AAWINPRINTERSETTINGS "Windows skrivarinst�llningar"
#define AACOLOURS "F�rger"
#define AABANDW "Svart och vitt"
#define AAGREY "Gr�"
#define AAFULLCOLOUR "Alla f�rger"
#define AASELECTGSDEVICE "V�lj Ghostscript device"

#define AAODDEVEN "Udda/J�mna"
#define AAIGNOREDSC "Ignorera DSC"
#define AAPAGES "Sidor"
#define AAFROMC "Fr�n:"
#define AATOC "Till:"

#define AAADVANCED "Avancerad"
#define AAADVANCEDPSOPT "Avancerade PostScript alternativ"
#define AASENDCTRLDBEFORE "S�nd CTRL+D f�re jobb"
#define AASENDCTRLDAFTER "S�nd CTRL+D efter jobb"
#define AAPROLOGFILE "Prolog fil"
#define AAEPILOGFILE "Epilog fil"
#define AABROWSE "Genoms�k"

/* PS2EPS dialog box */
#define AAPSTOEPS "PS till EPS"
#define AAPSTOEPSREAD "Har du l�st hj�lpen `PS till EPS` ?"
#define AAPSTOEPSAUTO "Automatisk ber�kning av Bounding Box"

#ifdef UNIX
#define AAAPROPERTIES "Inst�llningar"
#define AAALLPAGES "Alla sidor"
#define AAODDPAGES "Udda sidor"
#define AAEVENPAGES "J�mna sidor"
#define AAALL "Alla"
#define AAODD "Udda"
#define AAEVEN "J�mna"
#define AAREVERSE "Omv�nt"
#else
#ifdef _Windows
#define AAAPROPERTIES "&Inst�llningar"
#define AAALLPAGES "Alla &sidor"
#define AAODDPAGES "U&dda sidor"
#define AAEVENPAGES "&J�mna sidor"
#define AAALL "&Alla"
#define AAODD "U&dda"
#define AAEVEN "&J�mna"
#define AAREVERSE "&Omv�nt"
#else
#define AAAPROPERTIES "~Inst�llningar"
#define AAALLPAGES "~Alla sidor"
#define AAODDPAGES "U~dda sidor"
#define AAEVENPAGES "~J�mna sidor"
#define AAALL "~Alla"
#define AAODD "U~dda"
#define AAEVEN "~J�mna"
#define AAREVERSE "~Omv�nt"
#endif
#endif


/* Properties dialog box */
#define AAPROPERTIES "Inst�llningar"
#define AAPROPERTYC "Inst�llning:"
#define AAVALUEC "V�rde:"
#define AAPAGEOFFSETPT "Sido offset (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Redigera inst�llningar"
#define AAPROPERTYTYPEC "Inst�llningstyp:"
#define AANUMBER "Nummer"
#define AASTRING "Str�ng"
#define AANAMEC "Namn: e.g.  BitsPerPixel"
#define AAVALUESC "V�rden:  e.g.  1,3,8,16,24"
#define AADELETE "Radera"

#define AABOUNDINGBOX "BoundingBox"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Avancerad konfiguration"
#define AAGHOSTSCRIPTDLLC "Ghostscript DLL:"
#define AAGHOSTSCRIPTSOC "Ghostscript delat objekt:"
#define AAGHOSTSCRIPTEXEC "Ghostscript program:"
#define AAGHOSTSCRIPTINCC "Ghostscript inkludera s�kv�g:"
#define AAGHOSTSCRIPTOTHERC "Ghostscript alternativ:"
#define AACOPYPRINTERDEF "Kopiera skrivarstandard"
#define AAASSOCIATEPS "Associera .ps filer med GSview"
#define AAASSOCIATEPDF "Associera .pdf filer med GSview"
#define AACREATESTARTMENU "Skapa startmeny objekt"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Enkel konfiguration"
#define AAWHICHGS "Vilken version av Ghostscript vill du anv�nda?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Ladda ner Ghostscript"
#define AADOWNLOADNOW "Ladda ner nu"
#define AAGSNOTFOUND "Ghostscript finns inte p� din dator.  Du m�ste ladda ner och installera AFPL Ghostscript fr�n"

#define AAGSMESSWIN "GSview f�r Windows - Ghostscript meddelanden"
#define AAGSMESSPM "PM GSview - Ghostscript meddelanden"
#define AAGSMESSX11 "X11 GSview - Ghostscript meddelanden"
#define AACOPY "Kopiera"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Display inst�llningar"
#define AARESOLUTION "Uppl�sning"
#define AAZOOMRESOLUTION "Zooma uppl�sning"
#define AADEPTH "Djup"
#define AATEXTALPHA "Text Alpha"
#define AAGRAPHICSALPHA "Grafik Alpha"
#define AADRAWMETHOD "Ritmetod"
#define AADPI "dpi"
#define AABPP "bits/pixel"
#define AABITS "bits"

/* Measure and custom units dialog box */
#define AAPT "pt"
#define AAMM "mm"
#define AAINCH "inch"
#define AACUSTOM "Anpassad"
#define AAMEASURE "M�ta"
#define AAUNITC	"enhet:"
#define AASTARTC "Start:"
#define AAFINISHC "Avslutning:"
#define AADELTAC "Delta:"
#define AALENGTHC "L�ngd:"
#define AACHANGE "�ndra"
#define AACALCXFORM "Ber�kna transformering"
#define AATRANSLATE "�vers�tt"
#define AAROTATE "Rotera"
#define AASCALE "Skala"
#define AAINITMATRIX "initmatrix"
#define AAINVERTMATRIX "invertmatrix"

/* PS to Edit dialog box */
#define AAPSTOEDIT "PS till Edit"
#define AAFORMATC "Format:"
#define AADRAWTASP "Rita text som polygoner"
#define AAMAPTOLATIN1 "Mappa till ISO-Latin1"
#define AAFLATNESSC "Flathet:"
#define AADEFAULTFONTC "Standardfont:"
#define AADRIVEROPTIONSC "Drivrutin alternativ:"

/* Registration */
#define AAREGTOC "Registrerad till:"
#define AANUMBERC "Nummer:"
#define AAGSVIEWREG "GSview registrering"
#define AAREGISTERNOW "Registrera nu"
#define AAONLINEREG "Online registrering"
#define AAREG1 "Var v�nlig uppge ditt namn och registreringsnummer"
#define AAREG2 ""
#define AANAG1 "GSview �r Copyright 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Var v�nlig st�d utvecklingen av GSview genom att registrera den."
#define AANAG3 ""
#define AANAG4 "GSview kan registeras online hos"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "    http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "GSview Installation"
#define AAINSTALLINTRO1 "Guiden kommer att hj�lpa dig att installera"
#define AAINSTALLINTRO2 "Om du inte anv�nder en sj�lvextraherande EXE, f�rs�kra dig om att du har tillg�ng till filerna listade i Readme.htm i den aktuella katalogen."
#define AAINSTALLINTRO3 "Installationen kommer att kr�va 20 - 30 M bytes h�rddiskutrymme."
#define AAINSTALLCOPYRIGHT1 "Copyright notiser"
#define AAINSTALLCOPYRIGHT2 "GSview och AFPL Ghostscript �r separata program."
#define AAINSTALLCOPYRIGHT3 "De �gs av olika personer."
#define AAINSTALLGSVER1 "Vilka komponententer �nskar du installera?"
#define AAINSTALLGSVER2 "Installera GSview"
#define AAINSTALLGSVER3 "Installera Ghostscript"
#define AAINSTALLGSVER4 "Vilken version av Ghostscript vill du anv�nda?"
#define AAINSTALLDIR1 "V�lj en katalog i vilken du vill installera GSview och Ghostscript."
#define AAINSTALLDIR2 "Inom den katalogen, kommer GSview att placeras i en underkatalog:"
#define AAINSTALLDIR3 "Ghostscript kommer att placeras i underkatalogen:"
#define AAINSTALLMKDIR1 "Den katalog som du specifierade existerar inte."
#define AAINSTALLMKDIR2 "En ny katalog kommer att skapas."
#define AAINSTALLMISSING1 "GSview Setup - ZIP filen saknas"
#define AAINSTALLMISSING2 "Kan inte hitta ZIP filen.  Stoppa in disketten med nedanst�ende fil."
#define AAINSTALLCONFIG1 "GSview och Ghostscript anv�nder milj�variabeln TEMP f�r att specificera en katalog f�r tempor�ra filer."
#define AAINSTALLCONFIG2 "Du har inte definerat TEMP."
#define AAINSTALLCONFIG3 "F�ljande rad kommer att l�ggas till din autoexec.bat\r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Uppdatera autoexec.bat"
#define AAINSTALLCONFIG5 "Beh�ll backup av gammal autoexec.bat"
#define AAINSTALLCONFIGPM1 "GSview och Ghostscript anv�nder milj�variabeln TEMP f�r att specificera en katalog f�r tempor�ra filer."
#define AAINSTALLCONFIGPM2 "Du har inte definererat TEMP, eller m�ste du installera EMX."
#define AAINSTALLCONFIGPM3 "F�ljande rad kommer att l�ggas till din config.sys   'SET TEMP=C:\\'  och/eller din PATH och LIBPATH kommer att uppdateras f�r EMX."
#define AAINSTALLCONFIGPM4 "Uppdatera config.sys"
#define AAINSTALLCONFIGPM5 "Beh�ll backup av gammal config.sys"
#define AAINSTALLFINISH "GSview Setup �r nu klar att starta installationen.  \r\rTryck klar f�r att forts�tta."
#define AAINSTALLUNZIP1 "Packar upp filer..."
#define AADIRNOTEXIST "Katalogen '%s' existerar inte.  Skapa den?"
#define AAMKDIRFAIL "Kunde inte skapa katalog"
#define AABETAWARN "Detta �r en BETA testversion av GSview.  Den kommer att upph�ra den %04d-%02d-%02d."
#define AAINSTALLOK "Installationen lyckades."
#define AAINSTALLFAILED "Installationen misslyckades"
#define AAPROGMANGROUP1 "GSview Setup kan skapa en Program Manager grupp eller Startmeny objekt."
#define AAPROGMANGROUP2 "Skapa Grupp / Objekt"
#define AAPROGMANGROUP3 "Namn"
#define AAPROGMANGROUP4 "GS verktyg"
#define AAPROGMANGROUP5 "En Program Manager grupp med namnet \042%s\042 har skapats."
#define AAPROGMANGROUP6 "Ett skrivbords programobjekt men namnet \042GSview\042 har skapats."
#define AAPMOBJECT1 "GSview Setup kan skapa en skrivbordsobjekt f�r GSview."
#define AAPMOBJECT2 "Skapa ett GSview programobjekt."
#define AADONEBAK "Den gamla autoexec.bat har blivit namn�ndrad till %s"
#define AACANTLOAD "Kan inte ladda %s"
#define AACANTCREATETEMPFILE "Kunde inte skapa tempor�ra filnamn"
#define AACANTOPENREAD "Kan inte �ppna %s f�r l�sning"
#define AACANTOPENWRITE "Kan inte �ppna %s f�r skrivning"
#define AAERRORRENAME "Problem med att byta namnet %s till %s"
#define AANODDEPROGMAN "Kunde inte �ppna DDE anslutning till Program Manager.  Starta om Windows."
#define AAINSERTDISK "Stopp i en diskett inneh�llande %s"
#define AAZIPNOTFOUND "Zipfil kunde inte hittas"
#define AAUNZIPCANCELLED "Unzip avbruten\n"
#define AACANTALLOCBUF "Kan inte allokera minne f�r kopieringsbuffert"
#define AAPROGRAMOBJECTFAILED "Kunde inte skapa n�got programobjekt p� skrivbordet"
#define AAUNINSTALLTITLE "GSview och AFPL Ghostscript"
#define AAUNINSTALLITEM "Avinstallera GSview och AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "Den h�r installationen kommer att kr�va 5 M bytes h�rddiskutrymme"
#define AAGSVIEWREQUIRES "GSview beh�ver ocks�"
#define AAVIEWREADMEDETAILS "L�s Readme filen f�r detaljer om hur Ghostscript h�mtas."
#define AACOPYRIGHTNOTICE "Copyright notis"
#define AASELECTGSVIEWDIR "V�lj katalog d�r GSview skall installeras"
#define AABROWSEE "S�k igenom..."
#define AAGSVIEWADDSTARTMENU "GSview Setup kommer att l�gga till f�ljande objekt i mappen f�r Startmenyn"
#define AACREATEFOLDER "Skapa mapp"
#define AAALLUSERS "Alla anv�ndare"
#define AASELECTFOLDER "V�lj mapp"
#define AAFOLDER "Mapp"
#define AADESTFOLDER "Destinationsmapp"
#define AAVIEWREADME "Visa readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "Konfigurera GSview"
#define AACFG10 "Den h�r guiden hj�lper dig att konfigurera GSview."
#define AACFG11 "Om du inte f�rst�r n�got �mne p� f�ljande sidor,\
 �ndra inte n�got.  Standardinst�llningarna �r normalt tillr�ckliga."
#define AACFG20 "GSview beh�ver veta vilken version av Ghostscript som skall anv�ndas\
 och var den finns."
#define AACFG21 "Vilken version av Ghostscript vill du anv�nda?"
#define AACFG22 "I vilken katalog finns Ghostscript?"
#define AACFG23 "S�k efter ytterligare Type 1 fonter i"
#define AACFG30 "GSview h�ller en lista p� Ghostscript utskriftsdevicer och\
 uppl�sningar i sin initieringsfil."
#define AACFG31 "Standardlista �ver devicer och uppl�sningar �r tagen fr�n standard\
 distributionsversionen av Ghostscript 5.50 och �r kanske inte komplett."
#define AACFG32 "Uppdatera GSview's skrivarlista"
#define AACFG33 "Uppdatering av GSview's skrivarlista kommer att skriva �ver alla existerande objekt."
#define AACFG40 "GSview kan skapa filassociationer f�r PostScript och\
 Portabelt Dokument Format (PDF) filer."
#define AACFG43 "Med dessa associationer, kommer dubbelklickning p� en PostScript\
 eller PDF fil att starta GSview.\
 Dessa associationer kommer ocks� att g�ra s� att vissa WWW browsers\
 anv�nder GSview som visare f�r PostScript och PDF filer."
#define AACFG41 "Associera PostScript (.ps och.eps) filer med GSview."
#define AACFG42 "Associera PDF (.pdf) filer med GSview."
#define AACFG50 "GSview kan skapa en Program Manager grupp eller\
 Startmeny Objekt f�r GSview och AFPL Ghostscript."
#define AACFG51 "Skapa Grupp / Objekt"
#define AACFG52 "Namn"
#define AACFG53 "GSview kan skapa ett srivbords programobjekt f�r GSview. \
 Detta kommer att associera PostScript (.ps och .eps) och PDF (.pdf) filer med GSview."
#define AACFG54 "Skapa Programobjekt"
#define AACFG60 "GSview �r nu klar att starta konfigurationen. \r\rTryck avsluta f�r att forts�tta."
#define AACFG70 "Konfigurationen lyckades."
#define AACFG72 "Om du beh�ver �ndra konfigurationen senare,\
 k�r GSview och v�lj sedan \042Alternativ | Enkel konfiguration ..\042 eller\
 \042Alternativ | Avancerad konfiguration..\042."
#define AACFG73 "Konfigurationen misslyckades."
#define AACFG74 "Konfigurationen avbr�ts."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "GSview skriv ut"
#define AAGSVIEWPRINTUSAGE "Anv�ndning: %s [/d] dllpath optionfil inputfil\noptionfil och inputfil kommer att raderas vid avslutning\nDet �r meningen att gvpgs anv�nds med tempor�ra filer\n"
#ifdef _Windows
#define AAMFILE "&Arkiv"
#define AAMSAVEAS "Spara &som..."
#define AAMEXIT "S&t�ng"
#define AAMEDIT "&Redigera"
#define AAMCOPY "Kopiera\tCtrl+C"
#define AAMHELP "&Hj�lp"
#define AAMABOUT "&Om..."
#else
#define AAMFILE "~Arkiv"
#define AAMSAVEAS "Spara ~som..."
#define AAMEXIT "S~t�ng"
#define AAMEDIT "~Redigera"
#define AAMCOPY "Kopiera\tCtrl+C"
#define AAMHELP "~Hj�lp"
#define AAMABOUT "~Om..."
#endif

#define WINHELPFILE "gsviewse.chm"
