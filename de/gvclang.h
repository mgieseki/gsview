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

/* de\gvclang.h */
/* Common German language defines */

#define AADEUTSCH "Deutsch"

#define GSVIEW_COPYRIGHT1 "Copyright (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "Alle Rechte vorbehalten."
#define GSVIEW_COPYRIGHT3 "Siehe Datei LICENCE für mehr Details."

#define GS_COPYRIGHT1 "Copyright (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A.  Alle Rechte vorbehalten."
#define GS_COPYRIGHT3 "Siehe Datei PUBLIC für mehr Details."

/* Buttons */
#ifdef UNIX
#define AAOK "Ok"
#define AACANCEL "Abbruch"
#define AADEFAULTS "Standard"
#define AAEDIT "Bearbeiten"
#define AAHELP "Hilfe"
#define AANEW "Neu"
#define AATEST "Test"
#define AAYES "Ja"
#define AANO "Nein"
#define AANEXTGT "Weiter >"
#define AABACKLT "< Zurück"
#define AAFINISHGT "Fertig>"
#define AAEXIT "Ende"
#else
#ifdef _Windows
#define AAOK "&Ok"
#define AACANCEL "&Abbruch"
#define AADEFAULTS "&Standard"
#define AAEDIT "&Bearbeiten"
#define AAHELP "&Hilfe"
#define AANEW "&Neu"
#define AATEST "&Test"
#define AAYES "&Ja"
#define AANO "&Nein"
#define AANEXTGT "&Weiter >"
#define AABACKLT "< &Zurück"
#define AAFINISHGT "&Fertig>"
#define AAEXIT "&Ende"
#else
#define AAOK "~Ok"
#define AACANCEL "~Abbruch"
#define AADEFAULTS "~Standard"
#define AAEDIT "~Bearbeiten"
#define AAHELP "~Hilfe"
#define AANEW "~Neu"
#define AATEST "~Test"
#define AAYES "~Ja"
#define AANO "~Nein"
#define AANEXTGT "~Weiter >"
#define AABACKLT "< ~Zurück"
#define AAFINISHGT "~Fertig>"
#define AAEXIT "~Ende"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "Über GSview für Windows"
#define AAABOUTPM "Über PM GSview"
#define AAABOUTX11 "Über X11 GSview"
#define AAABOUTGSV16SPL "Über GSV16SPL"
#define AAABOUTGSVIEWPRINT "Über GSview Print"
#define AACOPY1 "Version:"
#define AACOPY2 "Grafische Bedienung von Ghostscript "
#define AACOPY4 "Dieses Programm wird ohne jede Gewährleistung vertrieben."
#define AACOPY5 "Keiner der Autoren oder Distributoren ist verantwortlich"
#define AACOPY6 "für die Folgen des Gebrauchs, weder für die Fehlerfreiheit einer"
#define AACOPY7 "einzelnen Funktion, noch für die des gesamten Programms.  Nähere"
#define AACOPY8 "Details finden sich in der GSview Free Public Licence (der 'Lizenz')."
#define AACOPY9 "Jede Kopie von GSview muß eine Kopie der Lizenz in Form einer ASCII"
#define AACOPY10 "Datei namens 'LICENCE' enthalten.  Diese Lizenz gibt Ihnen das Recht"
#define AACOPY11 "GSview zu kopieren, zu ändern und zu verbreiten, jedoch nur unter"
#define AACOPY12 "den Bedingungen, die in der Lizenz beschrieben sind.  Unter anderem"
#define AACOPY13 "fordert die Lizenz, daß das Copyright und dieser Hinweis in allen"
#define AACOPY14 "Kopien enthalten ist."
#define AACOPY15 "pstotxt3.dll, Copyright (C) 1995-1998 Digital Equipment Corporation,"
#define AACOPY16 "pstotxt2.dll, Copyright (C) 1995-1998 Digital Equipment Corporation,"
#define AACOPY17 "pstotxt1.dll, Copyright (C) 1995-1998 Digital Equipment Corporation,"
#define AACOPY18 "hat eine separate Lizenz.  Siehe Datei pstotext.txt."
#define AACOPY19 "Autor: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Eine Ghostscript DLL Schnittstelle."
#define AACOPY21 "Dies Programm ist Teil von GSview."
#define AACOPY22 "GSview Win32s/Win16 Druck-Manager"

#define AAINPUT "Eingabe"

/* DSC error dialog box */
#define AADSC "Dokument-Strukturierungskonventionen" 
#define AAIGNOREALLDSC "Alle DSC-Kommentare ignorieren"

/* Info dialog box */
#define AAINFO  "Info"
#define AAFILEC "Datei:"
#define AATYPEC "Typ:"
#define AATITLEC "Titel:"
#define AADATEC "Datum:"
#define AABOUNDINGBOXC "BoundingBox:"
#define AAORIENTATIONC "Ausrichtung:"
#define AADEFAULTMEDIAC "Papierformat:"
#define AAPAGEORDERC "Seitenordnung:"
#define AAPAGESC "Seiten:"
#define AAPAGEC "Seite:"
#define AABITMAPC "Bitmap:"

/* Sounds dialog box */
#define AASOUNDS "Klänge"
#define AAEVENTC "Ereignis:"
#define AASOUNDC "Klang:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Auswahl Druckeranschluß"
#define AASELECTPRINTER "Auswahl Drucker"
#define AAPRINTING "Drucke"
#define AASELECTPAGE "Auswahl Seite"
#define AASELECTPAGES "Auswahl Seiten"

/* Printer Setup dialog box */
#define AAPRINTERSETUP "Druckereinstellung"
#define AADEVICEC "Gerät:"
#define AARESOLUTIONC "Auflösung:"
#define AAQUEUEC "Drucker:"
#define AAPRINTTOFILE "Druck in Datei"
#define AAPSPRINTER "PostScript-Drucker"
#define AAOPTIONSC "Optionen:"
#define AAFIXEDMEDIA "Feste Seitengröße"
#define AACONVERT "Umwandlung"
#define AAWINPRINTERSETTINGS "Windows-Druckereinstellungen" 
#define AACOLOURS "Farben"
#define AABANDW "Schwarz/Weiß"
#define AAGREY "Grau"
#define AAFULLCOLOUR "Echtfarben"
#define AASELECTGSDEVICE "Wähle Ghostscript-Gerät"

#define AAODDEVEN "Ungerade/Gerade"
#define AAIGNOREDSC "Ignoriere DSC"
#define AAPAGES "Seiten"
#define AAFROMC "Von:"
#define AATOC "Bis:"

#define AAADVANCED "Fortgeschritten"
#define AAADVANCEDPSOPT "Fortgeschrittene PostScript-Optionen"
#define AASENDCTRLDBEFORE "Sende STRG+D vor jedem Auftrag"
#define AASENDCTRLDAFTER "Sende STRG+D nach jedem Auftrag"
#define AAPROLOGFILE "Prolog-Datei"
#define AAEPILOGFILE "Epilog-Datei" 
#define AABROWSE "Durchsuchen"

/* PS2EPS dialog box */
#define AAPSTOEPS "PS zu EPS"
#define	AAPSTOEPSREAD  "Haben Sie die Hilfe `PS zu EPS` gelesen ?"
#define AAPSTOEPSAUTO "Automatische Berechnung der BoundingBox"

#ifdef _Windows
#define AAAPROPERTIES "&Einstellung"
#define AAALLPAGES "Alle &Seiten"
#define AAODDPAGES "&Ungerade"
#define AAEVENPAGES "&Gerade"
#define AAALL "Alle &Seiten"
#define AAODD "&Ungerade"
#define AAEVEN "&Gerade"
#define AAREVERSE "&Absteigend"
#else
#define AAAPROPERTIES "~Einstellung"
#define AAALLPAGES "Alle ~Seiten"
#define AAODDPAGES "~Ungerade"
#define AAEVENPAGES "~Gerade"
#define AAALL "Alle ~Seiten"
#define AAODD "~Ungerade"
#define AAEVEN "~Gerade"
#define AAREVERSE "~Absteigend"
#endif

/* Properties dialog box */
#define AAPROPERTIES "Einstellung"
#define AAPROPERTYC "Einstellung:"
#define AAVALUEC "Wert:"
#define AAPAGEOFFSETPT "Seiten-Offset (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Einstellung bearbeiten"
#define AAPROPERTYTYPEC "Einstellungstyp:"
#define AANUMBER "Nummer"
#define AASTRING "Text"
#define AANAMEC "Name: z.B. BitsPerPixel"
#define AAVALUESC "Werte:  z.B. 1,3,8,16,24"
#define AADELETE "Löschen"

#define AABOUNDINGBOX "BoundingBox"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Fortgeschrittene Konfigurierung"
#define AAGHOSTSCRIPTDLLC "Ghostscript-DLL:"
#define AAGHOSTSCRIPTSOC "Ghostscript Shared Object:"
#define AAGHOSTSCRIPTEXEC "Ghostscript-Programm:"
#define AAGHOSTSCRIPTINCC "Ghostscript-Suchpfad:"
#define AAGHOSTSCRIPTOTHERC "Ghostscript-Optionen:"
#define AACOPYPRINTERDEF "Kopiere Druckervoreinstellungen"
#define AAASSOCIATEPS "Verknüpfe .ps-Dateien"
#define AAASSOCIATEPDF "Verknüpfe .pdf-Dateien"
#define AACREATESTARTMENU "Lege Startmenü-Einträge an"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Einfache Konfigurierung"
#define AAWHICHGS "Welche Version von Ghostscript wollen Sie einsetzen ?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Lade Ghostscript"
#define AADOWNLOADNOW "Lade jetzt"
#define AAGSNOTFOUND "Ghostscript kann nicht auf Ihrem Computer gefunden werden.  Sie müssen einen Download starten und AFPL Ghostscript installieren von"

#define AAGSMESSWIN "GSview für Windows - Ghostscript Meldungen"
#define AAGSMESSPM "PM GSview - Ghostscript Meldungen"
#define AAGSMESSX11 "X11 GSview - Ghostscript Meldungen"
#define AACOPY "Kopiere"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Anzeigeeinstellungen"
#define AARESOLUTION "Auflösung"
#define AAZOOMRESOLUTION "Zoom-Auflösung"
#define AADEPTH "Farbtiefe"
#define AATEXTALPHA "Text-Alpha"
#define AAGRAPHICSALPHA "Graphik-Alpha"
#define AADRAWMETHOD "Zeichenmethode"
#define AADPI "dpi"
#define AABPP "bits/pixel"
#define AABITS "bits"

/* Measure and custom units dialog box */
#define AAPT "pt"
#define AAMM "mm"
#define AAINCH "Zoll"
#define AACUSTOM "Nutzer"
#define AAMEASURE "Messen"
#define AAUNITC	"Einheit:"
#define AASTARTC "Start:"
#define AAFINISHC "Ende:"
#define AADELTAC "Delta:"
#define AALENGTHC "Länge:"
#define AACHANGE "Änderung"
#define AACALCXFORM "Berechne Transformation"
#define AATRANSLATE "Translation"
#define AAROTATE "Rotation"
#define AASCALE "Skalierung"
#define AAINITMATRIX "initmatrix"
#define AAINVERTMATRIX "invertmatrix"

/* PS to Edit dialog box */
#define AAPSTOEDIT "PS zu Edit"
#define AAFORMATC "Format:"
#define AADRAWTASP "Zeichne Text als Polygone"
#define AAMAPTOLATIN1 "Wandle in ISO-Latin1 um"
#define AAFLATNESSC "Flachheit:"
#define AADEFAULTFONTC "Standardzeichensatz:"
#define AADRIVEROPTIONSC "Treiberoptionen:"

/* Registration */
#define AAREGTOC "Registriert an:"
#define AANUMBERC "Nummer:"
#define AAGSVIEWREG "GSview Registrierung"
#define AAREGISTERNOW "Registriere jetzt"
#define AAONLINEREG "Online Registrierung"
#define AAREG1 "Bitte geben Sie ihren Namen und ihre Registrierungsnummer ein"
#define AAREG2 ""
#define AANAG1 "GSview ist Copyright 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Bitte unterstützen Sie durch Ihre Registrierung"
#define AANAG3 "die Entwicklung von GSview."
#define AANAG4 "GSview kann online registriert werden bei"
#define AANAG5 "    http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "GSview Installation"
#define AAINSTALLINTRO1 "Dies Programm soll Ihnen bei der Installation helfen "
#define AAINSTALLINTRO2 "Bevor Sie mit der Installation fortfahren, stellen Sie bitte sicher, daß sich die Dateien aus Readme.htm im aktuellen Verzeichnis."
#define AAINSTALLINTRO3 "Die Installation beansprucht ca. 20 - 30 MB Platz auf der Festplatte."
#define AAINSTALLCOPYRIGHT1 "Copyright Anmerkungen"
#define AAINSTALLCOPYRIGHT2 "GSview und AFPL Ghostscript sind separate Programme."
#define AAINSTALLCOPYRIGHT3 "Sie gehören unterschiedlichen Personen."
#define AAINSTALLGSVER1 "Welche Komponenten möchten Sie installieren?"
#define AAINSTALLGSVER2 "Installiere GSview"
#define AAINSTALLGSVER3 "Installiere Ghostscript"
#define AAINSTALLGSVER4 "Welche Version von Ghostscript möchten Sie benutzen?"
#define AAINSTALLDIR1 "Wählen Sie bitte ein Verzeichnis für Gsview und Ghostscript aus."
#define AAINSTALLDIR2 "Innerhalb dieses Verzeichnisses wird sich Gsview in folgendem Unterverzeichnis befinden:"
#define AAINSTALLDIR3 "Ghostscript wird in dem Unterverzeichnis installiert:"
#define AAINSTALLMKDIR1 "Das angegebene Verzeichnis existiert nicht."
#define AAINSTALLMKDIR2 "Ein neues Verzeichnis wird angelegt."
#define AAINSTALLMISSING1 "GSview Installation - Fehlende ZIP Datei"
#define AAINSTALLMISSING2 "Kann ZIP Datei nicht finden. Bitte die Diskette mit folgender Datei einlegen."
#define AAINSTALLCONFIG1 "GSview und Ghostscript benutzen die Umgebungsvariable TEMP um ein Verzeichnis für temporäre Dateien zu bestimmen."
#define AAINSTALLCONFIG2 "Bisher haben Sie kein TEMP definiert."
#define AAINSTALLCONFIG3 "Folgende Zeile wird in Ihre autoexec.bat eingefügt \r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Update autoexec.bat"
#define AAINSTALLCONFIG5 "Backup der alten autoexec.bat behalten "
#define AAINSTALLCONFIGPM1 "GSview und Ghostscript benutzen die Umgebungsvariable TEMP um ein Verzeichnis für temporäre Dateien zu bestimmen. "
#define AAINSTALLCONFIGPM2 "Bisher haben Sie kein TEMP definiert, oder Sie benötigen EMX."
#define AAINSTALLCONFIGPM3 "Folgende Zeile wird Ihrer config.sys zugefügt 'SET TEMP=C:\\'  und/oder Ihr PATH und LIBPATH wird um EMX erweitert. "
#define AAINSTALLCONFIGPM4 "Aktualisieren config.sys"
#define AAINSTALLCONFIGPM5 "Backup der alten config.sys behalten"
#define AAINSTALLFINISH "GSview Setup ist jetzt fertig zum Start der Installation.  \r\rDrücken Sie Fertig um fortzufahren."
#define AAINSTALLUNZIP1 "Entpacke Dateien..."
#define AADIRNOTEXIST "Verzeichnis '%s' existiert nicht. Soll es angelegt werden?"
#define AAMKDIRFAIL "Verzeichnis kann nicht angelegt werden"
#define AABETAWARN "Dies ist eine BETA Test Version von GSview. Sie wird deaktiviert am %04d-%02d-%02d."
#define AAINSTALLOK "Installation erfolgreich."
#define AAINSTALLFAILED "Installation fehlgeschlagen"
#define AAPROGMANGROUP1 "GSview kann eine Programm Manager Gruppe oder ein Start Menü anlegen."
#define AAPROGMANGROUP2 "Lege Gruppe / Symbol an"
#define AAPROGMANGROUP3 "Name"
#define AAPROGMANGROUP4 "GS Tools"
#define AAPROGMANGROUP5 "Eine Programm Manager Gruppe namens \042%s\042 wurde angelegt."
#define AAPROGMANGROUP6 "Ein Desktop Objekt namens \042GSview\042 wurde angelegt."
#define AAPMOBJECT1 "GSview Setup kann ein Desktop Objekt für Gsview anlegen."
#define AAPMOBJECT2 "Lege GSview Programm Object an."
#define AADONEBAK "Die alte autoexec.bat wurde umbenannt in %s"
#define AACANTLOAD "Nicht ladbar %s"
#define AACANTCREATETEMPFILE "Kann temporäre Datei nicht anlegen "
#define AACANTOPENREAD "Kann Datei %s nicht zum lesen öffnen "
#define AACANTOPENWRITE "Kann Datei %s nicht zum schreiben öffnen "
#define AAERRORRENAME "Fehler beim umbenennen von %s zu %s"
#define AANODDEPROGMAN "Kann DDE Verbindung zum Programm Manager nicht öffnen.  Starten Sie bitte Windows erneut."
#define AAINSERTDISK "Disk einlegen, die enthält %s"
#define AAZIPNOTFOUND "Zip Datei nicht gefunden"
#define AAUNZIPCANCELLED "Entpacken abgebrochen \n"
#define AACANTALLOCBUF "Kann keinen Speicher für den Kopierpuffer bekommen "
#define AAPROGRAMOBJECTFAILED  "Kann Desktop Objekt nicht anlegen"
#define AAUNINSTALLTITLE "GSview und AFPL Ghostscript "
#define AAUNINSTALLITEM "GSview und AFPL Ghostscript löschen"

/* setup program */
#define AAINSTALLSPACE "Diese Installation benötigt 5 MB Plattenplatz"
#define AAGSVIEWREQUIRES "GSview benötigt ferner"
#define AAVIEWREADMEDETAILS "Lesen Sie bitte in Readme, wie Sie Ghostscript bekommen."
#define AACOPYRIGHTNOTICE "Copyright-Anmerkung"
#define AASELECTGSVIEWDIR "Wählen Sie ein Verzeichnis aus, in das Sie GSview installieren wollen"
#define AABROWSEE "Durchsuchen..."
#define AAGSVIEWADDSTARTMENU "GSview-Setup wird Einträge in den folgenden Startmenüordner machen"
#define AACREATEFOLDER "Mappe anlegen"
#define AAALLUSERS "Alle Benutzer"
#define AASELECTFOLDER "Ordner auswählen"
#define AAFOLDER "Ordner"
#define AADESTFOLDER "Zielordner"
#define AAVIEWREADME "Lies Readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "GSview Konfigurieren"
#define AACFG10 "Dieser Wizard hilft Ihnen Gsview zu konfigurieren."
#define AACFG11 "Wenn Sie einen Eintrag der folgenden Seiten nicht verstehen,\
 dann ändern Sie ihn nicht.  Die Defaults sollten sinnvoll sein."
#define AACFG20 "GSview muß die Version von Ghostscript kennen\
 und wissen, wo es installiert ist."
#define AACFG21 "Welche Version von Ghostscript wollen Sie einsetzen?"
#define AACFG22 "In welchem Verzeichnis ist Ghostscript installiert?"
#define AACFG23 "Zusätzliche Typ 1 Zeichensätze sind in"
#define AACFG30 "GSview hält eine Liste von Ghostscript Druckern und\
 Auflösungen in seiner INI Datei vor."
#define AACFG31 "Die Standardliste der Geräte und Auflösungen wurde der Standarddistribution \
 von Ghostscript 5.01 entnommen und könnte unvollständig sein."
#define AACFG32 "Aktualisiere GSview Druckerliste"
#define AACFG33 "Aktualisierung der GSview-Druckerliste überschreibt vorhandene Einträge"
#define AACFG40 "GSview kann Dateiverknüfungen für PostScript und\
 Portable Document Format (PDF) Dateien anlegen."
#define AACFG43 "Durch diese Verknüpfungen führt ein Doppelklick auf jene Dateien \
 zum Start von GSview.  Außerdem bewirken sie bei einigen WWW-Browsern,\
 daß GSview als ein Viewer für PostScript- und PDF-Dateien benutzt wird."
#define AACFG41 "Verknüpfung von PostScript (.ps and .eps) Dateien mit GSview."
#define AACFG42 "Verknüpfung von PDF (.pdf) Dateien mit GSview."
#define AACFG50 "GSview kann eine Programmmanagergruppe oder\
 Startmenüeinträge für GSview und AFPL Ghostscript anlegen."
#define AACFG51 "Lege Gruppe / Eintrag an"
#define AACFG52 "Name"
#define AACFG60 "GSview ist jetzt bereit die Konfiguration zu starten.\r\r\
Drücken Sie Fertig um weiter zu machen."
#define AACFG53 "GSview kann ein Desktop-Programmobjekt für Gsview anlegen. \
 Dieses verknüpft PostScript- (.ps und .eps) und PDF- (.pdf) Dateien mit GSview."
#define AACFG54 "Lege Programm Objekt an"
#define AACFG70 "Konfiguration erfolgreich."
#define AACFG72 "Um die Konfiguration später zu verändern,\
 starten Sie GSview und wählen \042Optionen | Konfiguriere...\042 oder\
 \042Optionen | Konfiguriere Ghostscript...\042."
#define AACFG73 "Konfiguration fehlgeschlagen."
#define AACFG74 "Konfiguration abgebrochen."

/* Menu items for gvXgs.rc */
#define AAGSVIEWPRINT "GSview Druck"
#define AAGSVIEWPRINTUSAGE "Aufruf: %s [/d] dllpfad optionsdatei eingabedatei\nOptionsdatei und Eingabedatei werden am Ende gelöscht\nEs ist beabsichtigt gvpgs mit temporären Dateien aufzurufen\n"
#ifdef _Windows
#define AAMFILE "&Datei"
#define AAMSAVEAS "Speichen &Unter..."
#define AAMEXIT "E&nde"
#define AAMEDIT "&Bearbeiten"
#define AAMCOPY "Kopieren\tCtrl+K"
#define AAMHELP "&Hilfe"
#define AAMABOUT "&Über..."
#else
#define AAMFILE "~Datei"
#define AAMSAVEAS "Speichern ~Unter..."
#define AAMEXIT "E~nde"
#define AAMEDIT "~Bearbeiten"
#define AAMCOPY "Kopieren\tCtrl+K"
#define AAMHELP "~Hilfe"
#define AAMABOUT "~Über..."
#endif


#define WINHELPFILE "gsviewde.chm"
