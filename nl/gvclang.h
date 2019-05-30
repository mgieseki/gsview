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

/* nl\gvclang.h */
/* Common Dutch language defines */

#define AANEDERLANDS "Nederlands"

#define GSVIEW_COPYRIGHT1 "Copyright (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "Alle rechten voorbehouden"
#define GSVIEW_COPYRIGHT3 "Zie het LICENCE bestand voor meer details."

#define GS_COPYRIGHT1 "Copyright (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A.  Alle rechten voorbehouden."
#define GS_COPYRIGHT3 "Zie het bestand PUBLIC voor meer details"

/* Buttons */
#ifdef UNIX
#define AAOK "OK"
#define AACANCEL "Annuleren"
#define AAEDIT "Bewerken"
#define AADEFAULTS "Standaard"
#define AAHELP "Help"
#define AANEW "Nieuw"
#define AATEST "Test"
#define AAYES "Ja"
#define AANO "Nee"
#define AANEXTGT "Volgende >"
#define AABACKLT "< Terug"
#define AAFINISHGT "Afwerken >"
#define AAEXIT "Afsluiten"
#else
#ifdef _Windows
#define AAOK "&Ok"
#define AACANCEL "&Annuleren"
#define AAEDIT "&Bewerken"
#define AADEFAULTS "&Standaard"
#define AAHELP "&Help"
#define AANEW "&Nieuw"
#define AATEST "&Test"
#define AAYES "&Ja"
#define AANO "&Neen"
#define AANEXTGT "&Volgende >"
#define AABACKLT "< &Terug"
#define AAFINISHGT "Voltoo&ien >"
#define AAEXIT "Afsl&uiten"
#else
#define AAOK "~Ok"
#define AACANCEL "~Annuleren"
#define AAEDIT "~Bewerken"
#define AADEFAULTS "~Standaard"
#define AAHELP "~Help"
#define AANEW "~Nieuw"
#define AATEST "~Test"
#define AAYES "~Ja"
#define AANO "~Nee"
#define AANEXTGT "~Volgende >"
#define AABACKLT "< ~Terug"
#define AAFINISHGT "Voltoo~ien >"
#define AAEXIT "Afsl~uiten"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "Over GSview voor Windows"
#define AAABOUTPM "Over PM GSview voor OS/2 Warp"
#define AAABOUTX11 "Over X11 GSview"
#define AAABOUTGSV16SPL "Over GSV16SPL"
#define AAABOUTGSVIEWPRINT "Over GSview Print"
#define AACOPY1 "Versie:"
#define AACOPY2 "Ghostscript grafische interface"
#define AACOPY4 "Dit programma is uitgegeven ZONDER ENIGE GARANTIE van welke aard dan ook."
#define AACOPY5 "Auteurs en verdelers accepteren geen enkele verantwoordelijkheid"
#define AACOPY6 "voor de gevolgen van het gebruik, voor de praktische inzetbaarheid,"
#define AACOPY7 "of voor de werking, tenzij schriftelijk anders is overeengekomen."
#define AACOPY8 "Meer details vindt u in de GSview Free Public Licence ('de Licentie')."
#define AACOPY9 "Elke kopie van GSview moet een kopie van de licentie "
#define AACOPY10 "bevatten. Doorgaans in dit een ASCII tekstbestand met de naam"
#define AACOPY11 "LICENCE. De Licentie geeft u het recht GSView te kopiëren, te"
#define AACOPY12 "wijzigen en door te geven, echter uitsluitend onder de voorwaarden in de"
#define AACOPY13 "Licentie. Dit houdt onder meer in dat de copyright vermelding, alsmede"
#define AACOPY14 "deze tekst, bij elke kopie moeten worden bijgesloten."
#define AACOPY15 "pstotxt3.dll is Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY16 "pstotxt2.dll is Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY17 "pstotxt1.dll is Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY18 "hiervoor geldt een andere licentie.  Zie pstotext.txt voor de details"
#define AACOPY19 "Auteur: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Een Ghostscript DLL interface."
#define AACOPY21 "Dit programma is een onderdeel van GSview."
#define AACOPY22 "GSview Win32s/Win16 spooler"

#define AAINPUT "Input"

/* DSC error dialog box */
#define AADSC "Document Structuring Conventions"
#define AAIGNOREALLDSC "Alle DSC negeren"

/* Info dialog box */
#define AAINFO  "Info"
#define AAFILEC "Bestand:"
#define AATYPEC "Type:"
#define AATITLEC "Titel:"
#define AADATEC "Datum:"
#define AABOUNDINGBOXC "Inbinden:"
#define AAORIENTATIONC "Stand:"
#define AADEFAULTMEDIAC "Standaard Papierformaat:"
#define AAPAGEORDERC "Pagina Volgorde:"
#define AAPAGESC "Pagina's:"
#define AAPAGEC "Bladzijde:"
#define AABITMAPC "Bitmap:"

/* Sounds dialog box */
#define AASOUNDS "Geluiden"
#define AAEVENTC "Gebeurtenis:"
#define AASOUNDC "Geluid:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Seleceer de Printer Poort"
#define AASELECTPRINTER "Selecteer Printer"
#define AAPRINTING "Afdrukken bezig"
#define AASELECTPAGE "Selecteer Pagina"
#define AASELECTPAGES "Selecteer Pagina's"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Printer Setup"
#define AADEVICEC "Apparaat:"
#define AARESOLUTIONC "Resolutie:"
#define AAQUEUEC "Wachtrij:"
#define AAPRINTTOFILE "Naar bestand afdrukken"
#define AAPSPRINTER "PostScript Printer"
#define AAOPTIONSC "Opties:"
#define AAFIXEDMEDIA "Vast formaat"
#define AACONVERT "Omzetten"
#define AAWINPRINTERSETTINGS "Windows Printer instellingen"
#define AACOLOURS "Kleuren"
#define AABANDW "Zwart/wit"
#define AAGREY "Grijswaarden"
#define AAFULLCOLOUR "Full Color"
#define AASELECTGSDEVICE "Selecteer Ghostscript apparaat"

#define AAODDEVEN "Oneven/Even"
#define AAIGNOREDSC "DSC Negeren"
#define AAPAGES "Pagina's"
#define AAFROMC "Van:"
#define AATOC "Tot:"

#define AAADVANCED "Geavanceerd"
#define AAADVANCEDPSOPT "Geavanceerde PostScript Opties"
#define AASENDCTRLDBEFORE "Stuur vooraf CTRL+D"
#define AASENDCTRLDAFTER "Stuur CTRL+D achteraf"
#define AAPROLOGFILE "Proloog Bestand"
#define AAEPILOGFILE "Epiloog Bestand"
#define AABROWSE "Bladeren"

/* PS2EPS dialog box */
#define AAPSTOEPS "PS naar EPS"
#define AAPSTOEPSREAD "Help over `PS naar EPS` al gelezen?"
#define AAPSTOEPSAUTO "Het inbinden automatisch laten berekenen"

#ifdef UNIX
#define AAAPROPERTIES "Eigenschappen"
#define AAALLPAGES "Alle Pagina's"
#define AAODDPAGES "Oneven pagina's"
#define AAEVENPAGES "Even Pagina's"
#define AAALL "Alle"
#define AAODD "Oneven"
#define AAEVEN "Even"
#define AAREVERSE "Achterstevoren"
#else
#ifdef _Windows
#define AAAPROPERTIES "Eigenscha&ppen"
#define AAALLPAGES "&Alle Pagina's"
#define AAODDPAGES "&Oneven Pagina's"
#define AAEVENPAGES "&Even Pagina's"
#define AAALL "&Alle"
#define AAODD "&Oneven"
#define AAEVEN "&Even"
#define AAREVERSE "Achte&rstevoren"
#else
#define AAAPROPERTIES "Eigenscha~ppen"
#define AAALLPAGES "~Alle Pagina's"
#define AAODDPAGES "~Oneven Pagina's"
#define AAEVENPAGES "~Even Pagina's"
#define AAALL "~Alle"
#define AAODD "~Oneven"
#define AAEVEN "~Even"
#define AAREVERSE "Achte~rstevoren"
#endif
#endif
/* Properties dialog box */
#define AAPROPERTIES "Eigenschappen"
#define AAPROPERTYC "Eigenschap:"
#define AAVALUEC "Waarde:"
#define AAPAGEOFFSETPT "Pagina Offset (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Eigenschappen bewerken"
#define AAPROPERTYTYPEC "Soort Eigenschap:"
#define AANUMBER "Getal"
#define AASTRING "Reeks"
#define AANAMEC "Naam: bijvoorbeeld 'BitsPerPixel'"
#define AAVALUESC "Waarden: bij voorbeeld 1,3,8,16,24"
#define AADELETE "Wissen"

#define AABOUNDINGBOX "Inbinden"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Geavanceerde Instellingen"
#define AAGHOSTSCRIPTDLLC "Ghostscript DLL:"
#define AAGHOSTSCRIPTSOC "Ghostscript Shared Object:" /* ENGLISH */
#define AAGHOSTSCRIPTEXEC "Ghostscript Program:" /* ENGLISH */
#define AAGHOSTSCRIPTINCC "Ghostscript Pad:"
#define AAGHOSTSCRIPTOTHERC "Ghostscript Opties:"
#define AACOPYPRINTERDEF "Kopieer Standaard Printerinstellingen"
#define AAASSOCIATEPS "*.ps extensie koppelen aan GSview"
#define AAASSOCIATEPDF "*.pdf extensie koppelen aan GSview"
#define AACREATESTARTMENU "Startmenu Items Creëren"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Eenvoudige Configuratie"
#define AAWHICHGS "Welke Ghostscript versie wilt u gebruiken?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Ghostscript Downloaden"
#define AADOWNLOADNOW "Nu Downloaden"
#define AAGSNOTFOUND "Ghostscript is niet op uw systeem gevonden.  U moet Ghostscript eerst downloaden en installeren. "

#define AAGSMESSWIN "GSview voor Windows - Ghostscript Berichten"
#define AAGSMESSPM "PM GSview voor OS/2 Warp - Ghostscript Berichten"
#define AAGSMESSX11 "X11 GSview - Ghostscript Berichten"
#define AACOPY "Kopiëren"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Schermresolutie"
#define AARESOLUTION "Resolutie"
#define AAZOOMRESOLUTION "Zoomfactor"
#define AADEPTH "Diepte"
#define AATEXTALPHA "Tekst Alpha"
#define AAGRAPHICSALPHA "Grafisch Alpha"
#define AADRAWMETHOD "Tekenmethode"
#define AADPI "dpi"
#define AABPP "bits/pixel"
#define AABITS "bits"

/* Measure and custom units dialog box */
#define AAPT "pt"
#define AAMM "mm"
#define AAINCH "inch"
#define AACUSTOM "Aangepast"
#define AAMEASURE "Meten"
#define AAUNITC "Eenheid:"
#define AASTARTC "Begin:"
#define AAFINISHC "Einde:"
#define AADELTAC "Delta:"
#define AALENGTHC "Lengte:"
#define AACHANGE "Veranderen"
#define AACALCXFORM "Transformatie Berekenen"
#define AATRANSLATE "Verschuiven"
#define AAROTATE "Roteren"
#define AASCALE "Verschalen"
#define AAINITMATRIX "initmatrix"
#define AAINVERTMATRIX "invertmatrix"

/* PS to Edit dialog box */
#define AAPSTOEDIT "PS naar Bewerken"
#define AAFORMATC "Format:"
#define AADRAWTASP "Tekst als veelhoek tekenen"
#define AAMAPTOLATIN1 "Overzetten naar ISO-Latin1"
#define AAFLATNESSC "Nauwkeurigheid:"
#define AADEFAULTFONTC "Standaardfont:"
#define AADRIVEROPTIONSC "Driveropties:"

/* Registration */
#define AAREGTOC "Geregistreerd aan:"
#define AANUMBERC "Nummer:"
#define AAGSVIEWREG "GSview Registratie"
#define AAREGISTERNOW "Nu Registreren"
#define AAONLINEREG "Online Registreren"
#define AAREG1 "Voer uw naam en registratienummmer in"
#define AAREG2 ""
#define AANAG1 "GSview is Copyright 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Ondersteun de ontwikkeling van GSview door het programma te registreren."
#define AANAG3 ""
#define AANAG4 "GSview kan online worden geregistreerd bij:"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "    http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "GSview Installatie"
#define AAINSTALLINTRO1 "U wordt geholpen door de installatie wizard"
#define AAINSTALLINTRO2 "Wanneer u het zelfuitpakkend -.exe bestand niet gebruikt, let er dan op dat alle bestanden die zijn vermeld in de Readme.htm in de huidige directory aanwezig zijn."
#define AAINSTALLINTRO3 "Het volledige programma neemt circa 20 - 30 MB op de harde schijf in beslag."
#define AAINSTALLCOPYRIGHT1 "Copyright Opmerkingen"
#define AAINSTALLCOPYRIGHT2 "GSview en AFPL Ghostscript zijn afzonderlijke programma's."
#define AAINSTALLCOPYRIGHT3 "En beide programma's hebben verschillende eigenaars."
#define AAINSTALLGSVER1 "Welke onderdelen wenst u te installeren?"
#define AAINSTALLGSVER2 "GSview installeren"
#define AAINSTALLGSVER3 "Ghostscript installeren"
#define AAINSTALLGSVER4 "Welke versie van Ghostscript wil u gebruiken?"
#define AAINSTALLDIR1 "Kies een directory waarin GSview en Ghostscript geïnstalleerd moeten worden."
#define AAINSTALLDIR2 "In deze directory zal GSview geïnstalleerd worden in de subdirectory:"
#define AAINSTALLDIR3 "Ghostscript zal zich bevinden in de subdirectory:"
#define AAINSTALLMKDIR1 "De door u aangegeven directory bestaat niet."
#define AAINSTALLMKDIR2 "Er wordt een nieuwe directory gecreëerd."
#define AAINSTALLMISSING1 "GSview Setup - Het ZIP-bestand ontbreekt."
#define AAINSTALLMISSING2 "Kan het ZIP-bestand niet vinden.  Plaats de schijf met onderstaand bestand."
#define AAINSTALLCONFIG1 "GSview en Ghostscript gebruiken de omgevingsvariabele TEMP bij de keuze van een directory voor tijdelijke bestanden."
#define AAINSTALLCONFIG2 "."
#define AAINSTALLCONFIG3 "De volgende regel wordt toegevoegd aan autoexec.bat\r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Autoexec.bat wordt bijgewerkt"
#define AAINSTALLCONFIG5 "Reservekopie van het vorige autoexec.bat bewaren"
#define AAINSTALLCONFIGPM1 "GSview en Ghostscript gebruiken de omgevingsvariabele TEMP bij de keuze van een directory voor tijdelijke bestanden."
#define AAINSTALLCONFIGPM2 "Op uw systeem is geen variabele TEMP gedefiniëerd, of u moet EMX installeren."
#define AAINSTALLCONFIGPM3 "Volgende regel wordt toegevoegd aan config.sys   'SET TEMP=C:\\'  en/of uw PAD en LIBPATH worden aangepast voor EMX."
#define AAINSTALLCONFIGPM4 "Config.sys wordt bijgewerkt"
#define AAINSTALLCONFIGPM5 "Reservekopie van vorige config.sys bewaren"
#define AAINSTALLFINISH "GSview is nu klaar om geïnstalleerd te worden.  \r\rKlik op Beëindigen om verder te gaan."
#define AAINSTALLUNZIP1 "Bestanden worden uitgepakt..."
#define AADIRNOTEXIST "Directory '%s' bestaat niet.  Deze directory aanmaken?"
#define AAMKDIRFAIL "Kon directory niet aanmaken."
#define AABETAWARN "Dit is een BETA testversie van GSview.  De werking vervalt op %04d-%02d-%02d."
#define AAINSTALLOK "Installatie met succes afgewerkt."
#define AAINSTALLFAILED "Installatie mislukt!"
#define AAPROGMANGROUP1 "GSview kan een programmaobject (eCS) of snelkoppeling (Windows) aanmaken."
#define AAPROGMANGROUP2 "Aanmaken van programmaobject / snelkoppeling"
#define AAPROGMANGROUP3 "Naam"
#define AAPROGMANGROUP4 "GS Tools"
#define AAPROGMANGROUP5 "Een programmaobject met de naam \042%s\042 werd aangemaakt."
#define AAPROGMANGROUP6 "Een snelkoppeling met de naam \042GSview\042 werd aangemaakt."
#define AAPMOBJECT1 "GSview Setup kan een snelkoppeling aanmaken voor GSview."
#define AAPMOBJECT2 "Programmaobject (eCS) of snelkoppeling (Windows)"
#define AADONEBAK "Het vorige autoexec.bat werd hernoemd tot %s"
#define AACANTLOAD "kan %s kan niet laden"
#define AACANTCREATETEMPFILE "Kan naam van tijdelijk bestand niet maken."
#define AACANTOPENREAD "Kan %s kan niet openen."
#define AACANTOPENWRITE "Kan %s niet opslaan."
#define AAERRORRENAME "Fout bij het herbenoemen van %s tot %s."
#define AANODDEPROGMAN "Kon DDE verbinding met de Program Manager niet openen.  Start Windows opnieuw op."
#define AAINSERTDISK "Plaats de schijf met %s."
#define AAZIPNOTFOUND "Zipbestand niet gevonden."
#define AAUNZIPCANCELLED "Uitpakken gestopt\n"
#define AACANTALLOCBUF "Kan geen geheugen toewijzen voor een kopieerbuffer."
#define AAPROGRAMOBJECTFAILED "De snelkoppeling of het programmaobject kon niet worden gemaakt."
#define AAUNINSTALLTITLE "GSview en AFPL Ghostscript"
#define AAUNINSTALLITEM "GSview en AFPL Ghostscript de-installeren."

/* setup program */
#define AAINSTALLSPACE "Voor deze installatie is minimum 5 MB op de harde schijf vereist."
#define AAGSVIEWREQUIRES "GSview heeft ook nodig"
#define AAVIEWREADMEDETAILS "Lees het Readme bestand om te vernemen hoe u zich Ghostscript kunt aanschaffen."
#define AACOPYRIGHTNOTICE "Copyright Bericht"
#define AASELECTGSVIEWDIR "Kies de directory waarin GSview moet worden geïnstalleerd."
#define AABROWSEE "Bladeren..."
#define AAGSVIEWADDSTARTMENU "GSview Setup zal snelkoppelingen toevoegen aan het Start Menu."
#define AACREATEFOLDER "Map wordt aangemaakt."
#define AAALLUSERS "Alle Gebruikers."
#define AASELECTFOLDER "Selecteer de map"
#define AAFOLDER "Map"
#define AADESTFOLDER "Doelmap"
#define AAVIEWREADME "Readme lezen"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "GSview Configuratie"
#define AACFG10 "Deze wizard helpt u met het configureren van GSview."
#define AACFG11 "Indien u iets niet begrijpt op de volgende pagina's,\
 verander er dan ook niets aan.  De standaardwaarden voldoen meestal."
#define AACFG20 "GSview moet weten welke versie van Ghostscript het moet gebruiken,\
 en waar deze versie zich bevindt."
#define AACFG21 "Welke versie van Ghostscript wenst u te gebruiken?"
#define AACFG22 "In welke directory bevindt Ghostscript zich?"
#define AACFG23 "Zoek naar extra Type 1 fonts in"
#define AACFG30 "GSview houdt een lijst van Ghostscript printers en\
 printerresoluties bij in het initialisatiebestand."
#define AACFG31 "De standaardlijst van apparaten en resoluties komt uit\
 de standaard distributieversie van Ghostscript 5.50 versie en kan incompleet zijn."
#define AACFG32 "Lijst met afdrukapparaten  bijwerken."
#define AACFG33 "Tijdens het bijwerken van de GSview lijst met afdrukapparaten worden alle bestaande gegevens overschreven."
#define AACFG40 "GSview kan koppelingen aanmaken voor PostScript en\
 Portable Document Format (PDF) bestanden."
#define AACFG43 "Door deze koppelingen zal GSview worden opgestart\
 wanneer op u dubbelklikt op het pictogram van een PDF- of Postscriptbestand.\
  Met deze koppelingen kunnen sommige Webbrowsers \
  GSview ook als viewer gebruiken voor PostScript en PDF bestanden."
#define AACFG41 "Koppel PostScript (*.ps and *.eps) bestanden met GSview."
#define AACFG42 "Koppel PDF (*.pdf) bestanden met GSview."
#define AACFG50 "GSview kan een programmaobject of\
 snelkoppeling in het Start Menu aanmaken voor GSview en AFPL Ghostscript."
#define AACFG51 "Programmaobject/Snelkoppeling aanmaken"
#define AACFG52 "Naam"
#define AACFG53 "GSview kan een programmaobject of snelkoppeling aanmaken. \
 Hierbij worden extensies van PostScript (*.ps, *.eps) en PDF (*.pdf) gekoppeld aan GSview."
#define AACFG54 "Programmaobject of snelkoppeling aanmaken."
#define AACFG60 "GSview is nu klaar om met de configuratie te beginnen. \r\rKlik op Afsluiten om verder te gaan."
#define AACFG70 "Configuratie met succes uitgevoerd."
#define AACFG72 "Indien het later nodig zou zijn de configuratie wijzigen,\
 start dan GSview en activeer dan \042Opties | Eenvoudige configuratie...\042 of\
 \042Opties | geavanceerde configuratie...\042."
#define AACFG73 "Het configureren is mislukt."
#define AACFG74 "Het configureren werd gestopt."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "GSview Afdrukken"
#define AAGSVIEWPRINTUSAGE "Syntaxis: %s [/d] dllpath optiesbestand inputbestand\noptiesbestand en het inputbestand zal gewist worden bij het afsluiten\nHet is de bedoeling dat gvpgs gestart wordt met tijdelijke bestanden\n"
#ifdef _Windows
#define AAMFILE "&Bestand"
#define AAMSAVEAS "Opslaan &Als..."
#define AAMEXIT "Af&sluiten"
#define AAMEDIT "&Bewerken"
#define AAMCOPY "&Kopiëren\tCtrl+C"
#define AAMHELP "&Help"
#define AAMABOUT "&Over GSview..."
#else
#define AAMFILE "~Bestand"
#define AAMSAVEAS "Opslaan ~Als..."
#define AAMEXIT "Af~sluiten"
#define AAMEDIT "~Bewerken"
#define AAMCOPY "~Kopiëren\tCtrl+C"
#define AAMHELP "~Help"
#define AAMABOUT "~Over GSview..."
#endif

#define WINHELPFILE "gsviewnl.chm"
