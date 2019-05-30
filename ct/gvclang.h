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

/* ct\gvclang.h */
/* Common Catalan language defines */

#define AACATALA "Catal�"

#define GSVIEW_COPYRIGHT1 "Copyright (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "Tots els drets reservats."
#define GSVIEW_COPYRIGHT3 "Vegeu el fitxer LICENCE per a m�s detalls."

#define GS_COPYRIGHT1 "Copyright (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A.  Tots els drets reservats."
#define GS_COPYRIGHT3 "Vegeu el fitxer PUBLIC per a m�s detalls."

/* Buttons */
#ifdef UNIX
#define AAOK "D'acord"
#define AACANCEL "Cancel�la"
#define AAEDIT "Edita"
#define AADEFAULTS "Per defecte"
#define AAHELP "Ajuda"
#define AANEW "Nou"
#define AATEST "Prova"
#define AAYES "S�"
#define AANO "No"
#define AANEXTGT "Endavant >"
#define AABACKLT "< Endarrere"
#define AAFINISHGT "Final >"
#define AAEXIT "Surt"
#else
#ifdef _Windows
#define AAOK "&D'acord"
#define AACANCEL "&Cancel�la"
#define AAEDIT "&Edita"
#define AADEFAULTS "&Per defecte"
#define AAHELP "&Ajuda"
#define AANEW "&Nou"
#define AATEST "&Prova"
#define AAYES "&S�"
#define AANO "&No"
#define AANEXTGT "&Endavant >"
#define AABACKLT "< En&darrere"
#define AAFINISHGT "Fi&nal >"
#define AAEXIT "Su&rt"
#else
#define AAOK "~D'acord"
#define AACANCEL "~Cancel�la"
#define AADEFAULTS "~Per defecte"
#define AAEDIT "~Edita"
#define AAHELP "~Ajuda"
#define AANEW "~Nou"
#define AATEST "~Prova"
#define AAYES "~S�"
#define AANO "~No"
#define AANEXTGT "~Endavant >"
#define AABACKLT "< En~darrera"
#define AAFINISHGT "Fi~nal >"
#define AAEXIT "Su~rt"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "Quant al GSview per al Windows"
#define AAABOUTPM "Quant al PM GSview"
#define AAABOUTX11 "Quant al X11 GSview"
#define AAABOUTGSV16SPL "Quant al GSV16SPL"
#define AAABOUTGSVIEWPRINT "Quant al GSview Print"
#define AACOPY1 "Versi�:"
#define AACOPY2 "Una interf�cie gr�fica del Ghostscript"
#define AACOPY4 "Aquest programa es distribueix SENSE GARANTIES DE CAP TIPUS."
#define AACOPY5 "Ni l'autor ni el distribu�dor accepten responsabilitats derivades"
#define AACOPY6 "de les conseq��ncies d'utilitzar-lo, encara que serveixi per a algun prop�sit"
#define AACOPY7 "en concret o treball en general, amb excepci� que ell o ella ho hagi dit per escrit.  Vegeu"
#define AACOPY8 "la GSview Free Public Licence (la 'Llic�ncia') per a tots els detalls."
#define AACOPY9 "Cada c�pia del GSView ha d'incloure una c�pia de la llic�ncia, normalment"
#define AACOPY10 "en un document de text pla ASCII que es diu LICENCE.  La llic�ncia us garanteix"
#define AACOPY11 "el dret a copiar, modificar o redistribuir el GSview, per� tan sols sota"
#define AACOPY12 "unes certes condicions, descrites a la Llic�ncia.  Entre d'altres coses,"
#define AACOPY13 "la llic�ncia necessita que s'hi conservi a totes les c�pies"
#define AACOPY14 "l'av�s del copyright i aquest mateix av�s."
#define AACOPY15 "pstotxt3.dll �s Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY16 "pstotxt2.dll �s Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY17 "pstotxt1.dll �s Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY18 "i t� una altra llic�ncia.  Vegeu el pstotext.txt per als detalls."
#define AACOPY19 "Autor: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Una interf�cie DLL del Ghostscript."
#define AACOPY21 "Aquest programa forma part del GSview."
#define AACOPY22 "Gestor de cues del GSview Win32s/Win16"

#define AAINPUT "Entrada"

/* DSC error dialog box */
#define AADSC "Convenis d'Estructuraci� del Document (DSC)"
#define AAIGNOREALLDSC "Ignora tots els DSC"

/* Info dialog box */
#define AAINFO  "Informaci�"
#define AAFILEC "Fitxer:"
#define AATYPEC "Tipus:"
#define AATITLEC "T�tol:"
#define AADATEC "Data:"
#define AABOUNDINGBOXC "Requadre del l�mit:"
#define AAORIENTATIONC "Orientaci�:"
#define AADEFAULTMEDIAC "Suport per defecte:"
#define AAPAGEORDERC "Ordre de p�gines:"
#define AAPAGESC "P�gines:"
#define AAPAGEC "P�gina:"
#define AABITMAPC "Mapa de bits:"

/* Sounds dialog box */
#define AASOUNDS "Sons"
#define AAEVENTC "Incid�ncia:"
#define AASOUNDC "S�:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Seleccioneu el port de la impressora"
#define AASELECTPRINTER "Seleccioneu la impressora"
#define AAPRINTING "Impressi�"
#define AASELECTPAGE "Seleccioneu la p�gina"
#define AASELECTPAGES "Seleccioneu les p�gines"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Configuraci� de la impressora"
#define AADEVICEC "Dispositiu:"
#define AARESOLUTIONC "Resoluci�:"
#define AAQUEUEC "Cua:"
#define AAPRINTTOFILE "Imprimeix a un fitxer"
#define AAPSPRINTER "Impressora PostScript"
#define AAOPTIONSC "Opcions:"
#define AAFIXEDMEDIA "Suport fixat"
#define AACONVERT "Converteix"
#define AAWINPRINTERSETTINGS "Par�metres de la impressora del Windows"
#define AACOLOURS "Colors"
#define AABANDW "Blanc i Negre"
#define AAGREY "Gris"
#define AAFULLCOLOUR "Ple de color"
#define AASELECTGSDEVICE "Seleccioneu el dispositiu Ghostscript"

#define AAODDEVEN "Senar/Parell"
#define AAIGNOREDSC "Ignora els DSC"
#define AAPAGES "P�gines"
#define AAFROMC "Des de:"
#define AATOC "Fins a:"

#define AAADVANCED "Avan�ats"
#define AAADVANCEDPSOPT "Opcions avan�ades del PostScript"
#define AASENDCTRLDBEFORE "Envia el CTRL+D abans de la feina"
#define AASENDCTRLDAFTER "Envia el CTRL+D despr�s de la feina"
#define AAPROLOGFILE "Fitxer de pr�leg"
#define AAEPILOGFILE "Fitxer d'ep�leg"
#define AABROWSE "Navega"

/* PS2EPS dialog box */
#define AAPSTOEPS "De PS a EPS"
#define AAPSTOEPSREAD "Us heu llegit l'ajuda del `De PS a EPS` ?"
#define AAPSTOEPSAUTO "Calcula autom�ticament el requadre del l�mit"

#ifdef UNIX
#define AAAPROPERTIES "Propietats"
#define AAALLPAGES "Totes les p�gines"
#define AAODDPAGES "P�gines senars"
#define AAEVENPAGES "P�gines parelles"
#define AAALL "Totes"
#define AAODD "Senars"
#define AAEVEN "Parelles"
#define AAREVERSE "Inverteix"
#else
#ifdef _Windows
#define AAAPROPERTIES "&Propietats"
#define AAALLPAGES "&Totes les p�gines"
#define AAODDPAGES "P�gines &senars"
#define AAEVENPAGES "P�gines p&arelles"
#define AAALL "&Totes"
#define AAODD "&Senars"
#define AAEVEN "P&arelles"
#define AAREVERSE "&Inverteix"
#else
#define AAAPROPERTIES "~Propietats"
#define AAALLPAGES "~Totes les p�gines"
#define AAODDPAGES "P�gines ~senars"
#define AAEVENPAGES "P�gines p~arelles"
#define AAALL "~Totes"
#define AAODD "~Senars"
#define AAEVEN "P~arelles"
#define AAREVERSE "~Inverteix"
#endif
#endif


/* Properties dialog box */
#define AAPROPERTIES "Propietats"
#define AAPROPERTYC "Propietat:"
#define AAVALUEC "Valor:"
#define AAPAGEOFFSETPT "�fset de la p�gina (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Edita les propietats"
#define AAPROPERTYTYPEC "Tipus de propietat:"
#define AANUMBER "N�mero"
#define AASTRING "Cadena"
#define AANAMEC "Nom: p.e.  BitsPerPixel"
#define AAVALUESC "Valors:  p.e.  1,3,8,16,24"
#define AADELETE "Suprimeix"

#define AABOUNDINGBOX "Requadre del l�mit"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Configuraci� avan�ada"
#define AAGHOSTSCRIPTDLLC "DLL del Ghostscript:"
#define AAGHOSTSCRIPTSOC "Objecte compartit del Ghostscript:"
#define AAGHOSTSCRIPTEXEC "Programa del Ghostscript:"
#define AAGHOSTSCRIPTINCC "Cam� d'inclusi� del Ghostscript:"
#define AAGHOSTSCRIPTOTHERC "Opcions del Ghostscript:"
#define AACOPYPRINTERDEF "Copia els par�metres predeterminats de la impressora"
#define AAASSOCIATEPS "Associa els fitxers .ps amb el GSview"
#define AAASSOCIATEPDF "Associa els fitxers .pdf amb el GSview"
#define AACREATESTARTMENU "Crea elements al men� d'inici"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Configuraci� f�cil"
#define AAWHICHGS "Quina versi� del Ghostscript voleu utilitzar?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Baixa el Ghostscript"
#define AADOWNLOADNOW "Baixa-ho ara"
#define AAGSNOTFOUND "No es pot trobar el Ghostscript a l'ordinador. Necessitareu baixar-vos i instal�lar l'AFPL Ghostscript des de"

#define AAGSMESSWIN "GSview per al Windows - Missatges del Ghostscript"
#define AAGSMESSPM "PM GSview - Missatges del Ghostscript"
#define AAGSMESSX11 "X11 GSview - Missatges del Ghostscript"
#define AACOPY "Copia"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Mostra els par�metres"
#define AARESOLUTION "Resoluci�"
#define AAZOOMRESOLUTION "Resoluci� del Zoom"
#define AADEPTH "Profuntidat"
#define AATEXTALPHA "Alfa del text"
#define AAGRAPHICSALPHA "Alfa dels gr�fics"
#define AADRAWMETHOD "M�tode de dibuix"
#define AADPI "ppp"
#define AABPP "bits/p�xel"
#define AABITS "bits"

/* Measure and custom units dialog box */
#define AAPT "pt"
#define AAMM "mm"
#define AAINCH "polzada"
#define AACUSTOM "Personalitzat"
#define AAMEASURE "Mida"
#define AAUNITC	"Unitat:"
#define AASTARTC "Inici:"
#define AAFINISHC "Final:"
#define AADELTAC "Delta:"
#define AALENGTHC "Longitud:"
#define AACHANGE "Canvia"
#define AACALCXFORM "Calcula la transformaci�"
#define AATRANSLATE "Trasllada"
#define AAROTATE "Gira"
#define AASCALE "Escala"
#define AAINITMATRIX "Inicialitza la matriu"
#define AAINVERTMATRIX "Inverteix la matriu"

/* PS to Edit dialog box */
#define AAPSTOEDIT "De PS a Edit"
#define AAFORMATC "Format:"
#define AADRAWTASP "Dibuixa el text com pol�gons"
#define AAMAPTOLATIN1 "Aplica el ISO-Latin1"
#define AAFLATNESSC "Llisor:"
#define AADEFAULTFONTC "Lletra predeterminada:"
#define AADRIVEROPTIONSC "Opcions del controlador:"

/* Registration */
#define AAREGTOC "S'ha registrat a:"
#define AANUMBERC "Nom:"
#define AAGSVIEWREG "Registre del GSview"
#define AAREGISTERNOW "Registra ara"
#define AAONLINEREG "Registre en l�nia"
#define AAREG1 "Si us plau, introdu�u el vostre nom i el n�mero de registre"
#define AAREG2 ""
#define AANAG1 "El GSview �s Copyright 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Si us plau, registreu-vos per permetre el desenvolupament del GSview."
#define AANAG3 ""
#define AANAG4 "El GSview es pot registrar a"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "    http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "Instal�laci� del GSview"
#define AAINSTALLINTRO1 "Aquest auxiliar us ajudar� a la instal�laci� del"
#define AAINSTALLINTRO2 "Si no esteu utilitzant l'EXE autoextra�ble d'instal�laci�, assegureu-vos que teniu els fitxers que es llisten al Readme.htm del directori actual."
#define AAINSTALLINTRO3 "Aquesta instal�laci� necessita 20 - 30 Mbytes d'espai al disc dur."
#define AAINSTALLCOPYRIGHT1 "Avisos del Copyright"
#define AAINSTALLCOPYRIGHT2 "El GSview i l'AFPL Ghostscript s�n programes diferents."
#define AAINSTALLCOPYRIGHT3 "S�n propietat d'entitats diferents."
#define AAINSTALLGSVER1 "Quins components voleu instal�lar?"
#define AAINSTALLGSVER2 "Instal�la el GSview"
#define AAINSTALLGSVER3 "Instal�la el Ghostscript"
#define AAINSTALLGSVER4 "Quina versi� del Ghostscript voleu utilitzar?"
#define AAINSTALLDIR1 "Seleccioneu un directori on instal�lar el GSview i el Ghostscript."
#define AAINSTALLDIR2 "Dins aquest directori, el GSview ser� al subdirectori:"
#define AAINSTALLDIR3 "El Ghostscript ser� al subdirectori:"
#define AAINSTALLMKDIR1 "El directori que heu especificat no existeix."
#define AAINSTALLMKDIR2 "Es crear� un directori nou." 
#define AAINSTALLMISSING1 "Instal�laci� del GSview - Falta el fitxer ZIP"
#define AAINSTALLMISSING2 "No es pot trobar el fitxer ZIP. Inseriu un disc amb el seg�ent fitxer."
#define AAINSTALLCONFIG1 "El GSview i el Ghostscript utilitzen la variable d'entorn TEMP per especificar un directori per als fitxers temporals."
#define AAINSTALLCONFIG2 "Encara no teniu la variable TEMP definida."
#define AAINSTALLCONFIG3 "S'afegir� la seg�ent l�nia a l'autoexec.bat\r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Actualitza l'autoexec.bat"
#define AAINSTALLCONFIG5 "Conserva una c�pia de seguretat de l'autoexec.bat vell"
#define AAINSTALLCONFIGPM1 "El GSview i el Ghostscript utilitzen la variable d'entorn TEMP per especificar un directori per als fitxers temporals"
#define AAINSTALLCONFIGPM2 "Encara no teniu la variable TEMP definida, o necessiteu instal�lar l'EMX."
#define AAINSTALLCONFIGPM3 "S'afegir� la seg�ent l�nia al config.sys   'SET TEMP=C:\\' i/o s'actualitzaran el PATH i el LIBPATH per a l'EMX."
#define AAINSTALLCONFIGPM4 "Actualitza el config.sys"
#define AAINSTALLCONFIGPM5 "Conserva una c�pia de seguretat del config.sys vell"
#define AAINSTALLFINISH "El programa d'instal�laci� del GSview est� preparat per iniciar la instal�laci�.  \r\rPremeu Final per continuar."
#define AAINSTALLUNZIP1 "S'estan descomprimint els fitxers..."
#define AADIRNOTEXIST "El directori '%s' no existeix. Voleu crear-lo?"
#define AAMKDIRFAIL "No s'ha pogut crear el directori"
#define AABETAWARN "Aquesta �s una versi� BETA de prova del GSview. Es desactivar� el %04d-%02d-%02d."
#define AAINSTALLOK "S'ha instal�lat amb �xit."
#define AAINSTALLFAILED "Ha fallat la instal�laci�"
#define AAPROGMANGROUP1 "El programa d'instal�laci� del GSview pot crear un grup al Gestor de programes o un element del men� Inicia."
#define AAPROGMANGROUP2 "Crea un grup o element"
#define AAPROGMANGROUP3 "Nom"
#define AAPROGMANGROUP4 "Eines del GS"
#define AAPROGMANGROUP5 "S'ha creat un grup del Gestora de programes que es diu \042%s\042."
#define AAPROGMANGROUP6 "S'ha creat una drecera del programa a l'escriptori que es diu \042GSview\042."
#define AAPMOBJECT1 "El programa d'instal�laci� del GSview pot crear una drecera del GSview a l'escriptori."
#define AAPMOBJECT2 "Crea una drecera del GSview."
#define AADONEBAK "S'ha canviat el nom de l'autoexec.bat vell a %s"
#define AACANTLOAD "No es pot carregar el %s"
#define AACANTCREATETEMPFILE "No es pot crear el nom de fitxer temporal"
#define AACANTOPENREAD "No es pot obrir el %s per a lectura"
#define AACANTOPENWRITE "No es pot obrir el %s per a escriptura"
#define AAERRORRENAME "S'ha produ�t un error mentre es canvia de nom el %s a %s"
#define AANODDEPROGMAN "No s'ha pogut obrir la connexi� DDE al Gestor de programes. Reinicieu el Windows."
#define AAINSERTDISK "Inseriu un disc que contingui el %s"
#define AAZIPNOTFOUND "No s'ha trobat el fitxer Zip"
#define AAUNZIPCANCELLED "S'ha cancel�lat la descompressi�\n"
#define AACANTALLOCBUF "No s'ha pogut assignar mem�ria interm�dia per a la c�pia"
#define AAPROGRAMOBJECTFAILED "No s'ha pogut crear la drecera del programa a l'escriptori"
#define AAUNINSTALLTITLE "GSview i AFPL Ghostscript"
#define AAUNINSTALLITEM "Desinstal�la el GSview i l'AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "Aquesta instal�laci� necessita 5 Mbytes d'espai al disc dur"
#define AAGSVIEWREQUIRES "El GSview tamb� necessita"
#define AAVIEWREADMEDETAILS "Vegeu el fitxer Readme per als detalls sobre com obtenir el Ghostscript."
#define AACOPYRIGHTNOTICE "Av�s del Copyright"
#define AASELECTGSVIEWDIR "Seleccioneu un directori on instal�lar el GSview"
#define AABROWSEE "Navega..."
#define AAGSVIEWADDSTARTMENU "El programa d'instal�laci� del GSview afegir� els elements a la seg�ent carpeta del men� Inicia"
#define AACREATEFOLDER "Crea la carpeta"
#define AAALLUSERS "Tots els usuaris"
#define AASELECTFOLDER "Seleccioneu la carpeta"
#define AAFOLDER "Carpeta"
#define AADESTFOLDER "Carpeta de destinaci�"
#define AAVIEWREADME "Visualitza el Readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "Configuraci� del GSview"
#define AACFG10 "Aquest auxiliar us ajudar� a configurar el GSview."
#define AACFG11 "Si no enteneu algun element de les seg�ents p�gines,\
 no el canvieu. Les opcions predeterminades s�n assenyades."
#define AACFG20 "El GSview necessita saber quina versi� del Ghostscript ha d'utilitzar\
 i on s'ubica."
#define AACFG21 "Quina versi� del Ghostscript voleu utilitzar?"
#define AACFG22 "A quin directori s'ubica el Ghostscript?"
#define AACFG23 "Cerca tipus de lletra Type 1 addicionals a"
#define AACFG30 "El GSview mant� una llista dels dispositius d'impressi� Ghostscript\
 i les resolucions en el seu fitxer d'inicialitzaci�."
#define AACFG31 "La llista predeterminada de dispositius i resolucions s'ha tret de la distribuci�\
 est�ndard del Ghostscript 5.50 i pot no ser completa."
#define AACFG32 "Actualitza la llista d'impressores del GSview" 
#define AACFG33 "L'actualitzaci� de la llista d'impressores del GSview sobreescriur� les entrades existents." 
#define AACFG40 "El GSview pot crear associacions per als fitxers PostScript i\
 Portable Document Format (PDF)."
#define AACFG43 "Amb aquestes associacions, fent doble clic a un fitxer PostScript\
 o PDF s'iniciar� el GSview.\
  Tamb� permetran a alguns navegadors del WWW\
 utilitzar el GSview com un visualitzador per als fitxers PostScript i PDF."
#define AACFG41 "Associa els fitxers PostScript (.ps i .eps) amb el GSview."
#define AACFG42 "Associa els fitxers PDF (.pdf) amb el GSview."
#define AACFG50 "El GSview pot crear un grup al Gestor de programes o\
 un element al men� Inicia per al GSview i l'AFPL Ghostscript."
#define AACFG51 "Crea un grup o un element"
#define AACFG52 "Nom"
#define AACFG53 "El GSview pot crear una drecera del programa a l'escriptori. \
 Aix� associar� els fitxers PostScript (.ps and .eps) i PDF (.pdf) amb el GSview."
#define AACFG54 "Crea una drecera"
#define AACFG60 "El GSview est� preparat per iniciar la configuraci�. \r\rPremeu Final per continuar."
#define AACFG70 "S'ha configurat amb �xit."
#define AACFG72 "Si necessiteu canviar la configuraci� m�s tard,\
 executeu el GSview i seleccioneu \042Opcions | Configuraci� f�cil...\042 o\
 \042Opcions | Configuraci� avan�ada...\042."
#define AACFG73 "Ha fallat la configuraci�."
#define AACFG74 "S'ha cancel�lat la configuraci�."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "Impressi� del GSview"
#define AAGSVIEWPRINTUSAGE "Sintaxi: %s [/d] cami_del_dll opcions_del_fitxer fitxer_entrada\nopcions_del_fitxer i fitxer_entrada se suprimiran en sortir\nEst� pensat per a qu� es cridi el gvpgs amb fitxers temporals\n"
#ifdef _Windows
#define AAMFILE "&Fitxer"
#define AAMSAVEAS "&Anomena i desa..."
#define AAMEXIT "S&urt"
#define AAMEDIT "&Edita"
#define AAMCOPY "Copia\tCtrl+C"
#define AAMHELP "&Ajuda"
#define AAMABOUT "&Quant al..."
#else
#define AAMFILE "~Fitxer"
#define AAMSAVEAS "~Anomena i desa..."
#define AAMEXIT "S~urt"
#define AAMEDIT "~Edita"
#define AAMCOPY "Copia\tCtrl+C"
#define AAMHELP "~Ajuda"
#define AAMABOUT "~Quant al..."
#endif

#define WINHELPFILE "gsviewct.chm"
