/* Copyright (C) 1993-2012, Ghostgum Software Pty Ltd.  Alla rättigheter reserverade.

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
#define GSVIEW_COPYRIGHT2 "Alla rättigheter reserverade."
#define GSVIEW_COPYRIGHT3 "Se filen LICENCE för fler detaljer."

#define GS_COPYRIGHT1 "Copyright (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A.  Alla rättigheter reserverade."
#define GS_COPYRIGHT3 "Se filen PUBLIC för fler detaljer."

/* Buttons */
#ifdef UNIX
#define AAOK "Ok"
#define AACANCEL "Ångra"
#define AAEDIT "Redigera"
#define AADEFAULTS "Standard"
#define AAHELP "Hjälp"
#define AANEW "Nytt"
#define AATEST "Test"
#define AAYES "Ja"
#define AANO "Nej"
#define AANEXTGT "Nästa >"
#define AABACKLT "< Bakåt"
#define AAFINISHGT "Klart >"
#define AAEXIT "Avsluta"
#else
#ifdef _Windows
#define AAOK "&Ok"
#define AACANCEL "&Ångra"
#define AAEDIT "&Redigera"
#define AADEFAULTS "&Standard"
#define AAHELP "&Hjälp"
#define AANEW "N&ytt"
#define AATEST "&Test"
#define AAYES "&Ja"
#define AANO "&Nej"
#define AANEXTGT "N&ästa >"
#define AABACKLT "< &Bakåt"
#define AAFINISHGT "K&lart >"
#define AAEXIT "Av&sluta"
#else
#define AAOK "~Ok"
#define AACANCEL "~Ångra"
#define AADEFAULTS "~Standard"
#define AAEDIT "~Redigera"
#define AAHELP "~Hjälp"
#define AANEW "N~ytt"
#define AATEST "~Test"
#define AAYES "~Ja"
#define AANO "~Nej"
#define AANEXTGT "N~ästa >"
#define AABACKLT "< ~Bakåt"
#define AAFINISHGT "K~lart >"
#define AAEXIT "Av~sluta"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "Om GSview för Windows"
#define AAABOUTPM "Om PM GSview"
#define AAABOUTX11 "Om X11 GSview"
#define AAABOUTGSV16SPL "Om GSV16SPL"
#define AAABOUTGSVIEWPRINT "Om GSview utskrift"
#define AACOPY1 "Version:"
#define AACOPY2 "Ett grafiskt Ghostscript interface"
#define AACOPY4 "Detta program distribueras UTAN GARANTI AV NÅGOT SLAG."
#define AACOPY5 "Ingen författare eller distributör ansvarar för"
#define AACOPY6 "konsekvenserna av att använda den, eller lämpligheten för något"
#define AACOPY7 "som helst ändamål eller arbete, undantaget att han eller hon uttryckligen sagt så i skrift."
#define AACOPY8 "Referera till GSview Free Public Licence ('Licensen') för alla detaljer."
#define AACOPY9 "Varje kopia av GSview måste inkludera en kopia av licensen, normalt"
#define AACOPY10 "i en ren ASCII textfil kallad LICENCE.  Licensen ger dig"
#define AACOPY11 "rättigheter att kopiera, modifiera och återdistribuera GSview, men endast under"
#define AACOPY12 "vissa villkor som beskrivs i licensen.  Bland annat,"
#define AACOPY13 "kräver licensen att copyright notis och denna notis skall"
#define AACOPY14 "finnas med hos alla kopior."
#define AACOPY15 "pstotxt3.dll är Copyright för (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY16 "pstotxt2.dll är Copyright för (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY17 "pstotxt1.dll är Copyright för (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY18 "och har en annorlunda licens.  Se pstotext.txt för detaljer."
#define AACOPY19 "Författare: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Ett Ghostscript DLL interface."
#define AACOPY21 "Detta program är en del av GSview."
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
#define AAEVENTC "Händelse:"
#define AASOUNDC "Ljud:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Välj skrivarport"
#define AASELECTPRINTER "Välj skrivare"
#define AAPRINTING "Utskrift"
#define AASELECTPAGE "Välj sida"
#define AASELECTPAGES "Välj sidor"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Skrivare inställningar"
#define AADEVICEC "Device:"
#define AARESOLUTIONC "Upplösning:"
#define AAQUEUEC "Kö:"
#define AAPRINTTOFILE "Skriv ut till fil"
#define AAPSPRINTER "PostScript skrivare"
#define AAOPTIONSC "Alternativ:"
#define AAFIXEDMEDIA "Fast media"
#define AACONVERT "Konvertera"
#define AAWINPRINTERSETTINGS "Windows skrivarinställningar"
#define AACOLOURS "Färger"
#define AABANDW "Svart och vitt"
#define AAGREY "Grå"
#define AAFULLCOLOUR "Alla färger"
#define AASELECTGSDEVICE "Välj Ghostscript device"

#define AAODDEVEN "Udda/Jämna"
#define AAIGNOREDSC "Ignorera DSC"
#define AAPAGES "Sidor"
#define AAFROMC "Från:"
#define AATOC "Till:"

#define AAADVANCED "Avancerad"
#define AAADVANCEDPSOPT "Avancerade PostScript alternativ"
#define AASENDCTRLDBEFORE "Sänd CTRL+D före jobb"
#define AASENDCTRLDAFTER "Sänd CTRL+D efter jobb"
#define AAPROLOGFILE "Prolog fil"
#define AAEPILOGFILE "Epilog fil"
#define AABROWSE "Genomsök"

/* PS2EPS dialog box */
#define AAPSTOEPS "PS till EPS"
#define AAPSTOEPSREAD "Har du läst hjälpen `PS till EPS` ?"
#define AAPSTOEPSAUTO "Automatisk beräkning av Bounding Box"

#ifdef UNIX
#define AAAPROPERTIES "Inställningar"
#define AAALLPAGES "Alla sidor"
#define AAODDPAGES "Udda sidor"
#define AAEVENPAGES "Jämna sidor"
#define AAALL "Alla"
#define AAODD "Udda"
#define AAEVEN "Jämna"
#define AAREVERSE "Omvänt"
#else
#ifdef _Windows
#define AAAPROPERTIES "&Inställningar"
#define AAALLPAGES "Alla &sidor"
#define AAODDPAGES "U&dda sidor"
#define AAEVENPAGES "&Jämna sidor"
#define AAALL "&Alla"
#define AAODD "U&dda"
#define AAEVEN "&Jämna"
#define AAREVERSE "&Omvänt"
#else
#define AAAPROPERTIES "~Inställningar"
#define AAALLPAGES "~Alla sidor"
#define AAODDPAGES "U~dda sidor"
#define AAEVENPAGES "~Jämna sidor"
#define AAALL "~Alla"
#define AAODD "U~dda"
#define AAEVEN "~Jämna"
#define AAREVERSE "~Omvänt"
#endif
#endif


/* Properties dialog box */
#define AAPROPERTIES "Inställningar"
#define AAPROPERTYC "Inställning:"
#define AAVALUEC "Värde:"
#define AAPAGEOFFSETPT "Sido offset (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Redigera inställningar"
#define AAPROPERTYTYPEC "Inställningstyp:"
#define AANUMBER "Nummer"
#define AASTRING "Sträng"
#define AANAMEC "Namn: e.g.  BitsPerPixel"
#define AAVALUESC "Värden:  e.g.  1,3,8,16,24"
#define AADELETE "Radera"

#define AABOUNDINGBOX "BoundingBox"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Avancerad konfiguration"
#define AAGHOSTSCRIPTDLLC "Ghostscript DLL:"
#define AAGHOSTSCRIPTSOC "Ghostscript delat objekt:"
#define AAGHOSTSCRIPTEXEC "Ghostscript program:"
#define AAGHOSTSCRIPTINCC "Ghostscript inkludera sökväg:"
#define AAGHOSTSCRIPTOTHERC "Ghostscript alternativ:"
#define AACOPYPRINTERDEF "Kopiera skrivarstandard"
#define AAASSOCIATEPS "Associera .ps filer med GSview"
#define AAASSOCIATEPDF "Associera .pdf filer med GSview"
#define AACREATESTARTMENU "Skapa startmeny objekt"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Enkel konfiguration"
#define AAWHICHGS "Vilken version av Ghostscript vill du använda?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Ladda ner Ghostscript"
#define AADOWNLOADNOW "Ladda ner nu"
#define AAGSNOTFOUND "Ghostscript finns inte på din dator.  Du måste ladda ner och installera AFPL Ghostscript från"

#define AAGSMESSWIN "GSview för Windows - Ghostscript meddelanden"
#define AAGSMESSPM "PM GSview - Ghostscript meddelanden"
#define AAGSMESSX11 "X11 GSview - Ghostscript meddelanden"
#define AACOPY "Kopiera"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Display inställningar"
#define AARESOLUTION "Upplösning"
#define AAZOOMRESOLUTION "Zooma upplösning"
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
#define AAMEASURE "Mäta"
#define AAUNITC	"enhet:"
#define AASTARTC "Start:"
#define AAFINISHC "Avslutning:"
#define AADELTAC "Delta:"
#define AALENGTHC "Längd:"
#define AACHANGE "Ändra"
#define AACALCXFORM "Beräkna transformering"
#define AATRANSLATE "Översätt"
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
#define AAREG1 "Var vänlig uppge ditt namn och registreringsnummer"
#define AAREG2 ""
#define AANAG1 "GSview är Copyright 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Var vänlig stöd utvecklingen av GSview genom att registrera den."
#define AANAG3 ""
#define AANAG4 "GSview kan registeras online hos"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "    http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "GSview Installation"
#define AAINSTALLINTRO1 "Guiden kommer att hjälpa dig att installera"
#define AAINSTALLINTRO2 "Om du inte använder en självextraherande EXE, försäkra dig om att du har tillgång till filerna listade i Readme.htm i den aktuella katalogen."
#define AAINSTALLINTRO3 "Installationen kommer att kräva 20 - 30 M bytes hårddiskutrymme."
#define AAINSTALLCOPYRIGHT1 "Copyright notiser"
#define AAINSTALLCOPYRIGHT2 "GSview och AFPL Ghostscript är separata program."
#define AAINSTALLCOPYRIGHT3 "De ägs av olika personer."
#define AAINSTALLGSVER1 "Vilka komponententer önskar du installera?"
#define AAINSTALLGSVER2 "Installera GSview"
#define AAINSTALLGSVER3 "Installera Ghostscript"
#define AAINSTALLGSVER4 "Vilken version av Ghostscript vill du använda?"
#define AAINSTALLDIR1 "Välj en katalog i vilken du vill installera GSview och Ghostscript."
#define AAINSTALLDIR2 "Inom den katalogen, kommer GSview att placeras i en underkatalog:"
#define AAINSTALLDIR3 "Ghostscript kommer att placeras i underkatalogen:"
#define AAINSTALLMKDIR1 "Den katalog som du specifierade existerar inte."
#define AAINSTALLMKDIR2 "En ny katalog kommer att skapas."
#define AAINSTALLMISSING1 "GSview Setup - ZIP filen saknas"
#define AAINSTALLMISSING2 "Kan inte hitta ZIP filen.  Stoppa in disketten med nedanstående fil."
#define AAINSTALLCONFIG1 "GSview och Ghostscript använder miljövariabeln TEMP för att specificera en katalog för temporära filer."
#define AAINSTALLCONFIG2 "Du har inte definerat TEMP."
#define AAINSTALLCONFIG3 "Följande rad kommer att läggas till din autoexec.bat\r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Uppdatera autoexec.bat"
#define AAINSTALLCONFIG5 "Behåll backup av gammal autoexec.bat"
#define AAINSTALLCONFIGPM1 "GSview och Ghostscript använder miljövariabeln TEMP för att specificera en katalog för temporära filer."
#define AAINSTALLCONFIGPM2 "Du har inte definererat TEMP, eller måste du installera EMX."
#define AAINSTALLCONFIGPM3 "Följande rad kommer att läggas till din config.sys   'SET TEMP=C:\\'  och/eller din PATH och LIBPATH kommer att uppdateras för EMX."
#define AAINSTALLCONFIGPM4 "Uppdatera config.sys"
#define AAINSTALLCONFIGPM5 "Behåll backup av gammal config.sys"
#define AAINSTALLFINISH "GSview Setup är nu klar att starta installationen.  \r\rTryck klar för att fortsätta."
#define AAINSTALLUNZIP1 "Packar upp filer..."
#define AADIRNOTEXIST "Katalogen '%s' existerar inte.  Skapa den?"
#define AAMKDIRFAIL "Kunde inte skapa katalog"
#define AABETAWARN "Detta är en BETA testversion av GSview.  Den kommer att upphöra den %04d-%02d-%02d."
#define AAINSTALLOK "Installationen lyckades."
#define AAINSTALLFAILED "Installationen misslyckades"
#define AAPROGMANGROUP1 "GSview Setup kan skapa en Program Manager grupp eller Startmeny objekt."
#define AAPROGMANGROUP2 "Skapa Grupp / Objekt"
#define AAPROGMANGROUP3 "Namn"
#define AAPROGMANGROUP4 "GS verktyg"
#define AAPROGMANGROUP5 "En Program Manager grupp med namnet \042%s\042 har skapats."
#define AAPROGMANGROUP6 "Ett skrivbords programobjekt men namnet \042GSview\042 har skapats."
#define AAPMOBJECT1 "GSview Setup kan skapa en skrivbordsobjekt för GSview."
#define AAPMOBJECT2 "Skapa ett GSview programobjekt."
#define AADONEBAK "Den gamla autoexec.bat har blivit namnändrad till %s"
#define AACANTLOAD "Kan inte ladda %s"
#define AACANTCREATETEMPFILE "Kunde inte skapa temporära filnamn"
#define AACANTOPENREAD "Kan inte öppna %s för läsning"
#define AACANTOPENWRITE "Kan inte öppna %s för skrivning"
#define AAERRORRENAME "Problem med att byta namnet %s till %s"
#define AANODDEPROGMAN "Kunde inte öppna DDE anslutning till Program Manager.  Starta om Windows."
#define AAINSERTDISK "Stopp i en diskett innehållande %s"
#define AAZIPNOTFOUND "Zipfil kunde inte hittas"
#define AAUNZIPCANCELLED "Unzip avbruten\n"
#define AACANTALLOCBUF "Kan inte allokera minne för kopieringsbuffert"
#define AAPROGRAMOBJECTFAILED "Kunde inte skapa något programobjekt på skrivbordet"
#define AAUNINSTALLTITLE "GSview och AFPL Ghostscript"
#define AAUNINSTALLITEM "Avinstallera GSview och AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "Den här installationen kommer att kräva 5 M bytes hårddiskutrymme"
#define AAGSVIEWREQUIRES "GSview behöver också"
#define AAVIEWREADMEDETAILS "Läs Readme filen för detaljer om hur Ghostscript hämtas."
#define AACOPYRIGHTNOTICE "Copyright notis"
#define AASELECTGSVIEWDIR "Välj katalog där GSview skall installeras"
#define AABROWSEE "Sök igenom..."
#define AAGSVIEWADDSTARTMENU "GSview Setup kommer att lägga till följande objekt i mappen för Startmenyn"
#define AACREATEFOLDER "Skapa mapp"
#define AAALLUSERS "Alla användare"
#define AASELECTFOLDER "Välj mapp"
#define AAFOLDER "Mapp"
#define AADESTFOLDER "Destinationsmapp"
#define AAVIEWREADME "Visa readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "Konfigurera GSview"
#define AACFG10 "Den här guiden hjälper dig att konfigurera GSview."
#define AACFG11 "Om du inte förstår något ämne på följande sidor,\
 ändra inte något.  Standardinställningarna är normalt tillräckliga."
#define AACFG20 "GSview behöver veta vilken version av Ghostscript som skall användas\
 och var den finns."
#define AACFG21 "Vilken version av Ghostscript vill du använda?"
#define AACFG22 "I vilken katalog finns Ghostscript?"
#define AACFG23 "Sök efter ytterligare Type 1 fonter i"
#define AACFG30 "GSview håller en lista på Ghostscript utskriftsdevicer och\
 upplösningar i sin initieringsfil."
#define AACFG31 "Standardlista över devicer och upplösningar är tagen från standard\
 distributionsversionen av Ghostscript 5.50 och är kanske inte komplett."
#define AACFG32 "Uppdatera GSview's skrivarlista"
#define AACFG33 "Uppdatering av GSview's skrivarlista kommer att skriva över alla existerande objekt."
#define AACFG40 "GSview kan skapa filassociationer för PostScript och\
 Portabelt Dokument Format (PDF) filer."
#define AACFG43 "Med dessa associationer, kommer dubbelklickning på en PostScript\
 eller PDF fil att starta GSview.\
 Dessa associationer kommer också att göra så att vissa WWW browsers\
 använder GSview som visare för PostScript och PDF filer."
#define AACFG41 "Associera PostScript (.ps och.eps) filer med GSview."
#define AACFG42 "Associera PDF (.pdf) filer med GSview."
#define AACFG50 "GSview kan skapa en Program Manager grupp eller\
 Startmeny Objekt för GSview och AFPL Ghostscript."
#define AACFG51 "Skapa Grupp / Objekt"
#define AACFG52 "Namn"
#define AACFG53 "GSview kan skapa ett srivbords programobjekt för GSview. \
 Detta kommer att associera PostScript (.ps och .eps) och PDF (.pdf) filer med GSview."
#define AACFG54 "Skapa Programobjekt"
#define AACFG60 "GSview är nu klar att starta konfigurationen. \r\rTryck avsluta för att fortsätta."
#define AACFG70 "Konfigurationen lyckades."
#define AACFG72 "Om du behöver ändra konfigurationen senare,\
 kör GSview och välj sedan \042Alternativ | Enkel konfiguration ..\042 eller\
 \042Alternativ | Avancerad konfiguration..\042."
#define AACFG73 "Konfigurationen misslyckades."
#define AACFG74 "Konfigurationen avbröts."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "GSview skriv ut"
#define AAGSVIEWPRINTUSAGE "Användning: %s [/d] dllpath optionfil inputfil\noptionfil och inputfil kommer att raderas vid avslutning\nDet är meningen att gvpgs används med temporära filer\n"
#ifdef _Windows
#define AAMFILE "&Arkiv"
#define AAMSAVEAS "Spara &som..."
#define AAMEXIT "S&täng"
#define AAMEDIT "&Redigera"
#define AAMCOPY "Kopiera\tCtrl+C"
#define AAMHELP "&Hjälp"
#define AAMABOUT "&Om..."
#else
#define AAMFILE "~Arkiv"
#define AAMSAVEAS "Spara ~som..."
#define AAMEXIT "S~täng"
#define AAMEDIT "~Redigera"
#define AAMCOPY "Kopiera\tCtrl+C"
#define AAMHELP "~Hjälp"
#define AAMABOUT "~Om..."
#endif

#define WINHELPFILE "gsviewse.chm"
