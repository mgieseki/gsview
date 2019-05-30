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

/* it\gvclang.h */
/* Common Italian language defines */

#define AAITALIANO "Italiano"

#define GSVIEW_COPYRIGHT1 "Copyright (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "Tutti i diritti riservati."
#define GSVIEW_COPYRIGHT3 "Vedere il file LICENCE per maggiori dettagli."

#define GS_COPYRIGHT1 "Copyright (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A.  Tutti i diritti riservati."
#define GS_COPYRIGHT3 "Vedere il file PUBLIC per maggiori dettagli."

/* Buttons */
#ifdef UNIX
#define AAOK "Ok"
#define AACANCEL "Annulla"
#define AAEDIT "Modifica"
#define AADEFAULTS "Predefinite"
#define AAHELP "Guida"
#define AANEW "Nuovo"
#define AATEST "Test"
#define AAYES "Si"
#define AANO "No"
#define AANEXTGT "Successiva >"
#define AABACKLT "< Precedente"
#define AAFINISHGT "Fine >"
#define AAEXIT "Esci"
#else
#ifdef _Windows
#define AAOK "&Ok"
#define AACANCEL "&Annulla"
#define AAEDIT "&Modifica"
#define AADEFAULTS "&Predefinite"
#define AAHELP "&Guida"
#define AANEW "&Nuovo"
#define AATEST "&Test"
#define AAYES "&Si"
#define AANO "&No"
#define AANEXTGT "S&uccessiva >"
#define AABACKLT "< P&recedente"
#define AAFINISHGT "&Fine >"
#define AAEXIT "&Esci"
#else
#define AAOK "~Ok"
#define AACANCEL "~Annulla"
#define AAEDIT "~Modifica"
#define AADEFAULTS "~Predefinite"
#define AAHELP "~Guida"
#define AANEW "~Nuovo"
#define AATEST "~Test"
#define AAYES "~Si"
#define AANO "~No"
#define AANEXTGT "S~uccessiva >"
#define AABACKLT "< P~recedente"
#define AAFINISHGT "~Fine >"
#define AAEXIT "~Esci"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "Informazioni su GSview per Windows"
#define AAABOUTPM "Informazioni su PM GSview"
#define AAABOUTX11 "Informazioni su X11 GSview"
#define AAABOUTGSV16SPL "Informazioni su GSV16SPL"
#define AAABOUTGSVIEWPRINT "Informazioni su GSview Print"
#define AACOPY1 "Versione:"
#define AACOPY2 "Una interfaccia grafica a GhostScript"
#define AACOPY4 "Questo programma è distribuito SENZA ALCUNA GARANZIA."
#define AACOPY5 "L'Autore e il Distributore non si assumono alcuna responsabilità per le"
#define AACOPY6 "conseguenze risultanti dal suo uso e non assicurano che che esso sia"
#define AACOPY7 "rispondente ad una particolare funzione o che semplicemente funzioni, a"
#define AACOPY8 "meno che esse non siano precisate per iscritto. Vedere la GSview Free"
#define AACOPY9 "Public Licence (la 'Licenza') per ulteriori dettagli. Ogni copia di GSview"
#define AACOPY10 "deve includere una copia della Licenza, normalmente sotto forma di file"
#define AACOPY11 "ASCII dal nome di LICENCE. La Licenza da diritto a copiare, modificare e"
#define AACOPY12 "redistribuire GSview, ma solo sotto certe condizioni descritte nella"
#define AACOPY13 "Licenza. Tra le altre cose, la Licenza richiede che l'avviso di copyright"
#define AACOPY14 "e questo avviso siano mantenuti in tutte le copie."
#define AACOPY15 "pstotxt3.dll è Copyright (C) 1995-1996 Digital Equipment Corporation"
#define AACOPY16 "pstotxt2.dll è Copyright (C) 1995-1996 Digital Equipment Corporation"
#define AACOPY17 "pstotxt1.dll è Copyright (C) 1995-1996 Digital Equipment Corporation"
#define AACOPY18 "e ha una licenza diversa.  Vedi pstotext.txt per maggiori dettagli."
#define AACOPY19 "Autore: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Una interfaccia alla DLL di Ghostscript."
#define AACOPY21 "Questo programma è parte integrante di GSview."
#define AACOPY22 "GSview Win32s/Win16 spooler"

#define AAINPUT "Input"

/* DSC error dialog box */
#define AADSC "Convenzioni sulla struttura del Documento (DSC)"
#define AAIGNOREALLDSC "Ignora tutti i DSC"

/* Info dialog box */
#define AAINFO  "Informazioni"
#define AAFILEC "File:"
#define AATYPEC "Tipo:"
#define AATITLEC "Titolo:"
#define AADATEC "Data:"
#define AABOUNDINGBOXC "Limiti:"
#define AAORIENTATIONC "Orientamento:"
#define AADEFAULTMEDIAC "Pagina predefinita:"
#define AAPAGEORDERC "Ordine di pagina:"
#define AAPAGESC "Pagine:"
#define AAPAGEC "Pagina:"
#define AABITMAPC "Bitmap:"

/* Sounds dialog box */
#define AASOUNDS "Suoni"
#define AAEVENTC "Evento:"
#define AASOUNDC "Suono:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Seleziona porta stampanete"
#define AASELECTPRINTER "Seleziona stampante"
#define AAPRINTING "In stampa"
#define AASELECTPAGE "Seleziona pagina"
#define AASELECTPAGES "Seleziona pagine"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Imposta Stampante"
#define AADEVICEC "Periferica:"
#define AARESOLUTIONC "Risoluzione:"
#define AAQUEUEC "Scelta:"
#define AAPRINTTOFILE "Stampa su file"
#define AAPSPRINTER "Stampante PostScript"
#define AAOPTIONSC "Opzioni:"
#define AAFIXEDMEDIA "Carta Fissa"
#define AACONVERT "Converti"
#define AAWINPRINTERSETTINGS "Impostazioni della Stampante di Windows"
#define AACOLOURS "Colori"
#define AABANDW "Bianco e Nero"
#define AAGREY "Grigio"
#define AAFULLCOLOUR "Gamma completa di colori"
#define AASELECTGSDEVICE "Selecziona device di Ghostscript"

#define AAODDEVEN "Dispari/Pari"
#define AAIGNOREDSC "Ignora DSC"
#define AAPAGES "Pagine"
#define AAFROMC "Da:"
#define AATOC "A:"

#define AAADVANCED "Avanzate"
#define AAADVANCEDPSOPT "Opzioni PostScript Avanzate"
#define AASENDCTRLDBEFORE "Invia CTRL+D prima del lavoro"
#define AASENDCTRLDAFTER "Invia CTRL+D dopo il lavoro"
#define AAPROLOGFILE "File di Prologo"
#define AAEPILOGFILE "File di Epilogo"
#define AABROWSE "Cerca"

/* PS2EPS dialog box */
#define AAPSTOEPS "Converti PS in EPS"
#define AAPSTOEPSREAD "Hai letto nella Guida l'argomento `Converti PS in EPS` ?"
#define AAPSTOEPSAUTO "Calcola automaticamente i limiti"

#ifdef _Windows
#define AAAPROPERTIES "&Proprietà"
#define AAALLPAGES "&Tutte"
#define AAODDPAGES "&Dispari"
#define AAEVENPAGES "P&ari"
#define AAALL "&Tutte"
#define AAODD "&Dispari"
#define AAEVEN "P&ari"
#define AAREVERSE "&Ordine inverso"
#else
#define AAAPROPERTIES "~Proprietà"
#define AAALLPAGES "~Tutte"
#define AAODDPAGES "~Dispari"
#define AAEVENPAGES "P~ari"
#define AAALL "~Tutte"
#define AAODD "~Dispari"
#define AAEVEN "P~ari"
#define AAREVERSE "~Ordine inverso"
#endif


/* Properties dialog box */
#define AAPROPERTIES "Proprietà"
#define AAPROPERTYC "Proprietà:"
#define AAVALUEC "Valore:"
#define AAPAGEOFFSETPT "Offset della Pagina (punti)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Modifita proprietà"
#define AAPROPERTYTYPEC "Tipo Proprietà:"
#define AANUMBER "Numero"
#define AASTRING "Stringa"
#define AANAMEC "Name: p.e.  BitsPerPixel"
#define AAVALUESC "Valori:  p.e.  1,3,8,16,24"
#define AADELETE "Cancella"

#define AABOUNDINGBOX "BoundingBox"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Configurazione Avanzata"
#define AAGHOSTSCRIPTDLLC "DLL di Ghostscript:"
#define AAGHOSTSCRIPTSOC "Ghostscript Shared Object:" /* ENGLISH */
#define AAGHOSTSCRIPTEXEC "Ghostscript Program:" /* ENGLISH */
#define AAGHOSTSCRIPTINCC "Includi Percorso Di Ricerca di Ghostscript:"
#define AAGHOSTSCRIPTOTHERC "Opzioni di Ghostscript:"
#define AACOPYPRINTERDEF "Copia Stampanti predefinite"
#define AAASSOCIATEPS "Associa files .ps"
#define AAASSOCIATEPDF "Associa files .pdf"
#define AACREATESTARTMENU "Crea collegamenti nello Start Menu"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Configurazione Veloce"
#define AAWHICHGS "Quale versione di Ghostscript vuoi usare?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Download Ghostscript"
#define AADOWNLOADNOW "Download"
#define AAGSNOTFOUND "Ghostscript non può essere trovato sul computer.  E' necessario effettuare il download ed installare AFPL Ghostscript da"

#define AAGSMESSWIN "GSview per Windows - Messaggi di Ghostscript"
#define AAGSMESSPM "PM GSview - Messaggi di Ghostscript"
#define AAGSMESSX11 "X11 GSview - Messaggi di Ghostscript"
#define AACOPY "Copia"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Impostazioni di visualizzazione"
#define AARESOLUTION "Risoluzione"
#define AAZOOMRESOLUTION "Risoluzione dello zoom"
#define AADEPTH "Intensità"
#define AATEXTALPHA "Testo Alpha"
#define AAGRAPHICSALPHA "Grafica Alpha"
#define AADRAWMETHOD "Metodo di Disegno"
#define AADPI "dpi"
#define AABPP "bits/pixel"
#define AABITS "bits"

/* Measure and custom units dialog box */
#define AAPT "pt"
#define AAMM "mm"
#define AAINCH "pollici"
#define AACUSTOM "Personalizzate"
#define AAMEASURE "Misure"
#define AAUNITC	"Unità:"
#define AASTARTC "Inizio:"
#define AAFINISHC "Fine:"
#define AADELTAC "Delta:"
#define AALENGTHC "Lunghezza:"
#define AACHANGE "Cambia"
#define AACALCXFORM "Calcola Trasformazione"
#define AATRANSLATE "Traduci"
#define AAROTATE "Ruota"
#define AASCALE "Scala"
#define AAINITMATRIX "Inizializza Matrix"
#define AAINVERTMATRIX "Inverti Matrix"

/* PS to Edit dialog box */
#define AAPSTOEDIT "Converti in formato vettoriale"
#define AAFORMATC "Formato:"
#define AADRAWTASP "Disegna testo come poligoni"
#define AAMAPTOLATIN1 "Mappa in ISO-Latin1"
#define AAFLATNESSC "Uniformità:"
#define AADEFAULTFONTC "Carattere Predefinito:"
#define AADRIVEROPTIONSC "Opzioni del Driver:"

/* Registration */
/* ENGLISH */
#define AAREGTOC "Registrato come:"
#define AANUMBERC "Numero:"
#define AAGSVIEWREG "Registrazione di GSview"
#define AAREGISTERNOW "Registra adesso"
#define AAONLINEREG "Registrazione Online"
#define AAREG1 "Inserire il nome e il numero di registrazione"
#define AAREG2 ""
#define AANAG1 "GSview è Copyright 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Si prega di supportare lo sviluppo di GSview"
#define AANAG3 "tramite la registrazione."
#define AANAG4 "GSview può essere registrato online presso"
#define AANAG5 "    http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "Installazione di GSview"
#define AAINSTALLINTRO1 "Questo wizard vi aiuterà nell'installazione"
#define AAINSTALLINTRO2 "Prima di continuare l'installazione, assicuratevi di avere i files elencati in Readme.htm nella cartella corrente."
#define AAINSTALLINTRO3 "L'installazione richiede 20 - 30 MB di spazio libero sul disco."
#define AAINSTALLCOPYRIGHT1 "Informazioni sui Diritti d'Autore"
#define AAINSTALLCOPYRIGHT2 "GSview e AFPL Ghostscript sono dei programmi separati."
#define AAINSTALLCOPYRIGHT3 "Essi sono di proprità di diverse entità."
#define AAINSTALLGSVER1 "Quali componenti vuoi installare?"
#define AAINSTALLGSVER2 "Installa GSview"
#define AAINSTALLGSVER3 "Installa Ghostscript"
#define AAINSTALLGSVER4 "Quale versione di GhostScript vuoi usare?"
#define AAINSTALLDIR1 "Seleziona una cartella nella quale installare GSview e Ghostscript."
#define AAINSTALLDIR2 "All'interno di questa cartella, GSview sarà nella sottocartella:"
#define AAINSTALLDIR3 "Ghostscript sarà nella sottocartella:"
#define AAINSTALLMKDIR1 "La cartella che hai definito non esiste."
#define AAINSTALLMKDIR2 "Sarà creata una nuova cartella." 
#define AAINSTALLMISSING1 "Installazione di GSview - Manca il file ZIP"
#define AAINSTALLMISSING2 "Non riesco a trovare il file ZIP.  Inserisci il disco con il file seguente."
#define AAINSTALLCONFIG1 "GSview e Ghostscript usano la variabile di ambiente TEMP per specificare la cartella per i file temporanei."
#define AAINSTALLCONFIG2 "La variabile TEMP non è definita."
#define AAINSTALLCONFIG3 "La seguente linea sarà aggiunta al file autoexec.bat\r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Aggiorna autoexec.bat"
#define AAINSTALLCONFIG5 "Mantieni una copia di backup del vecchio file autoexec.bat"
#define AAINSTALLCONFIGPM1 "GSview e Ghostscript usano la variabile di ambiente TEMP per specificare la cartella per i file temporanei."
#define AAINSTALLCONFIGPM2 "La variabile TEMP non è definita oppure è necessario installare EMX."
#define AAINSTALLCONFIGPM3 "La seguente linea sarà aggiunta al file config.sys   'SET TEMP=C:\\'  e/o la variabile PATH e LIBPATH sarà aggiornata per EMX."
#define AAINSTALLCONFIGPM4 "Aggiorna config.sys"
#define AAINSTALLCONFIGPM5 "Mantieni una copia di backup del vecchio file config.sys"
#define AAINSTALLFINISH "L'installazione di GSview è adesso pronta per la partenza.  \r\rPremi Fine per continuare."
#define AAINSTALLUNZIP1 "Decomprimi i files..."
#define AADIRNOTEXIST "La Cartella '%s' non esiste.  Vuoi crearla?"
#define AAMKDIRFAIL "Couldn't make directory"
#define AABETAWARN "Questa è una versione BETA Test di GSview.  Il programma sarà disabilitato il %04d-%02d-%02d."
#define AAINSTALLOK "Installazione terminata con successo."
#define AAINSTALLFAILED "Installazione fallita"
#define AAPROGMANGROUP1 "GSview può creare un Gruppo in Program Manager oppure una cartella nello Start Menu per GSview e AFPL Ghostscript."
#define AAPROGMANGROUP2 "Creare Gruppo / Cartella"
#define AAPROGMANGROUP3 "Nome"
#define AAPROGMANGROUP4 "GS Tools"
#define AAPROGMANGROUP5 "Il gruppo \042%s\042 è stato creato in Program Manager."
#define AAPROGMANGROUP6 "Il Collegamento desktop \042GSview\042 è stato creato."
#define AAPMOBJECT1 "L'installazione di GSview può creare un collegamento a GSview sul Desktop."
#define AAPMOBJECT2 "Creato un collegamento a GSview sul Desktop."
#define AADONEBAK "Il vecchio autoexec.bat è stato rinominato in %s"
#define AACANTLOAD "Non è possibile caricare %s"
#define AACANTCREATETEMPFILE "Non è possibile creare file temporanei"
#define AACANTOPENREAD "Non è possibile aprire %s per la lettura"
#define AACANTOPENWRITE "Non è possibile aprire %s per la scrittura"
#define AAERRORRENAME "Errore nel rinominare %s in %s"
#define AANODDEPROGMAN "Non è possibile aprire la connessione DDE verso il Program Manager.  Riavviare Windows."
#define AAINSERTDISK "Inserire il disco che contiene %s"
#define AAZIPNOTFOUND "File ZIP non trovato"
#define AAUNZIPCANCELLED "Unzip cancellato\n"
#define AACANTALLOCBUF "Non è possibile allocare memoria per il buffer di copia"
#define AAPROGRAMOBJECTFAILED "Non è possibile creare il Collegamento sul Desktop"
#define AAUNINSTALLTITLE "GSview e AFPL Ghostscript"
#define AAUNINSTALLITEM "Disinstalla GSview d AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "Questa installazione richiederà circa 3 Mb di spazio nell'hard disk"
#define AAGSVIEWREQUIRES "GSview richiede anche"
#define AAVIEWREADMEDETAILS "Vedere il file Readme per informazioni su come ottenere Ghostscript."
#define AACOPYRIGHTNOTICE "Avviso di Copyright"
#define AASELECTGSVIEWDIR "Selezionare una cartella nella quale installare GSview"
#define AABROWSEE "Sfoglia..."
#define AAGSVIEWADDSTARTMENU "L'installazione di GSview Setup aggiungerà le seguenti cartelle nel Menu Start"
#define AACREATEFOLDER "Crea Cartella"
#define AAALLUSERS "Tutti gli Utenti"
#define AASELECTFOLDER "Seleziona Cartella"
#define AAFOLDER "Cartella"
#define AADESTFOLDER "Cartella di destinazione"
#define AAVIEWREADME "Vedere Readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "Configura GSview"
#define AACFG10 "Il wizard ti aiuterà nel configurare GSview."
#define AACFG11 "Se non capisci un argomento delle pagine seguenti,\
 non cambiarlo.  Il predefinito è già sensato."
#define AACFG20 "GSview ha bisogno di conoscere la versione di GhostScript da usare\
 e dove è situata."
#define AACFG21 "Quale versione di GhostScript vuoi usare ?"
#define AACFG22 "In quale cartella è situato Ghostscript ?"
#define AACFG23 "Cerca per caratteri Type 1 addizionali"
#define AACFG30 "GSview mantiene una lista una lista delle periferiche di stampa di Ghostscript e\
 delle risoluzioni nei loro file di inizializzazione."
#define AACFG31 "La lista predefinita delle periferiche e delle risoluzioni è presa dalla distribuzione\
 standard della versione 5.01 di Ghostscript e può non essere completa."
#define AACFG32 "Aggiorna la lista delle stampanti di GSview" 
#define AACFG33 "Aggiornando la lista delle stampanti di GSview sarà sovrascritta ogni dato esistente." 
#define AACFG40 "GSview può creare l'associazione ai file PostScript e\
 ai file Portable Document Format (PDF)."
#define AACFG43 "Con queste associazioni, il doppio click su un file PostScript\
 o PDF farà avviare GSview.\
  Queste associazioni consentono anche ad alcuni browsers WWW\
 di usare GSview come visualizzatore dei files PostScript e PDF."
#define AACFG41 "Associare i files PostScript (.ps and .eps) con GSview."
#define AACFG42 "Associare i files PDF (.pdf) con GSview."
#define AACFG50 "GSview può creare un Gruppo in Program Manager oppure\
 una cartella nello Start Menu per GSview e AFPL Ghostscript."
#define AACFG51 "Create Gruppo / Cartella"
#define AACFG52 "Nome"
#define AACFG53 "GSview può creare un collegamento a GSview sul Desktop. \
 Questo associerà i files PostScript (.ps and .eps) e PDF (.pdf) con GSview."
#define AACFG54 "Creare collegamento sul Desktop"
#define AACFG60 "GSview è adesso pronto ad iniziare la configurazione. \r\rPremi Fine per continuare."
#define AACFG70 "Configurazione terminata con successo."
#define AACFG72 "Se sarà necessario cambiare la configurazione successivamente,\
 avvia GSview e seleziona \042Opzioni | Configura...\042 oppure\
 \042Opzioni | Configura Ghostscript...\042."
#define AACFG73 "Configurazione fallita."
#define AACFG74 "Configurazione cancellata."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "Stampa GSview"
#define AAGSVIEWPRINTUSAGE "Usage: %s [/d] dllpath optionfile inputfile\noptionfile and inputfile will be deleted on exit\nIt is intended that gvpgs be called with temporary files\n"
#ifdef _Windows
#define AAMFILE "&File"
#define AAMSAVEAS "Salva Con &Nome..."
#define AAMEXIT "&Esci"
#define AAMEDIT "&Modifica"
#define AAMCOPY "Copia\tCtrl+C"
#define AAMHELP "&Guida"
#define AAMABOUT "&Informazioni su GSview..."
#else
#define AAMFILE "~File"
#define AAMSAVEAS "Salva Con ~Nome..."
#define AAMEXIT "~Esci"
#define AAMEDIT "~Modifica"
#define AAMCOPY "Copia\tCtrl+C"
#define AAMHELP "~Guida"
#define AAMABOUT "~Informazioni su GSview..."
#endif


#define WINHELPFILE "gsviewit.chm"
