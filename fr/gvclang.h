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

/* fr\gvclang.h */
/* Common French language defines */

#define AAFRANCAIS "Fran�ais"	/* Francais in code page 1252 */

#define GSVIEW_COPYRIGHT1 "Copyright (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "Tous droits r�serv�s."
#define GSVIEW_COPYRIGHT3 "Voir le fichier LICENCE pour plus d'informations."

#define GS_COPYRIGHT1 "Copyright (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A. Tous droits r�serv�s."
#define GS_COPYRIGHT3 "Voir le fichier PUBLIC pour plus d'informations."

/* Buttons */
#ifdef UNIX
#define AAOK "Ok"
#define AACANCEL "Annuler"
#define AAEDIT "Edition"
#define AADEFAULTS "D�fauts"
#define AAHELP "Aide"
#define AANEW "Nouveau"
#define AATEST "Test"
#define AAYES "Oui"
#define AANO "Non"
#define AANEXTGT "Suivant >"
#define AABACKLT "< Pr�c�dent"
#define AAFINISHGT "D�marrer"
#define AAEXIT "Fin"
#else
#ifdef _Windows
#define AAOK "&Ok"
#define AACANCEL "&Annuler"
#define AAEDIT "&Edition"
#define AADEFAULTS "&D�fauts"
#define AAHELP "A&ide"
#define AANEW "&Nouveau"
#define AATEST "&Test"
#define AAYES "&Oui"
#define AANO "&Non"
#define AANEXTGT "&Suivant >"
#define AABACKLT "< &Pr�c�dent"
#define AAFINISHGT "&D�marrer"
#define AAEXIT "&Fin"
#else
#define AAOK "~Ok"
#define AACANCEL "~Annuler"
#define AADEFAULTS "~D�fauts"
#define AAEDIT "~Edition"
#define AAHELP "A~ide"
#define AANEW "~Nouveau"
#define AATEST "~Test"
#define AAYES "~Oui"
#define AANO "~Non"
#define AANEXTGT "~Suivant >"
#define AABACKLT "< ~Pr�c�dent"
#define AAFINISHGT "~D�marrer"
#define AAEXIT "~Fin"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "A propos de GSview pour Windows"
#define AAABOUTPM "A propos de PM GSview"
#define AAABOUTX11 "A propos de X11 GSview"
#define AAABOUTGSV16SPL "A propos de GSV16SPL"
#define AAABOUTGSVIEWPRINT "A propos de GSview Print"
#define AACOPY1 "Version :"
#define AACOPY2 "Une interface graphique pour Ghostscript"
#define AACOPY4 "Ce programme est distribu� SANS AUCUNE GARANTIE. Les auteurs et"
#define AACOPY5 "distributeurs n'assument aucune responsabilit� pour les cons�quences r�sultant"
#define AACOPY6 "de son utilisation et n'assurent pas qu'il remplit une fonction particuli�re ou"
#define AACOPY7 "simplement fonctionne, � moins qu'ils ou elles ne le pr�cisent par �crit. Voir"
#define AACOPY8 "la GSview Free Public Licence (la 'Licence') pour plus d'informations."
#define AACOPY9 "Chaque copie de GSview doit contenir une copie de la Licence, classique-"
#define AACOPY10 "ment sous la forme d'un fichier ASCII nomm� LICENCE. La Licence vous"
#define AACOPY11 "donne le droit de copier, modifier et redistribuer GSview, mais uniquement"
#define AACOPY12 "sous certaines conditions d�crites dans la Licence. Entre autres, la Licence"
#define AACOPY13 "exige que les informations de copyright et cette note soient pr�sentes sur"
#define AACOPY14 "toutes les copies."
#define AACOPY15 "pstotxt3.dll est Copyright (C) 1995-1998, Digital Equipment Corporation et a"
#define AACOPY16 "pstotxt2.dll est Copyright (C) 1995-1998, Digital Equipment Corporation et a"
#define AACOPY17 "pstotxt1.dll est Copyright (C) 1995-1998, Digital Equipment Corporation et a"
#define AACOPY18 "une licence particuli�re. Voir le fichier pstotext.txt pour plus d'informations."
#define AACOPY19 "Auteur : Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Une interface sous forme de DLL pour Ghostscript."
#define AACOPY21 "Ce programme fait partie de GSview."
#define AACOPY22 "Gestionnaire d'impression GSview Win32s/Win16"

#define AAINPUT "Entr�e"

/* DSC error dialog box */
#define AADSC "Document Structuring Conventions" /* ENGLISH */
#define AAIGNOREALLDSC "Ignore all DSC" /* ENGLISH */

/* Info dialog box */
#define AAINFO  "Informations"
#define AAFILEC "Fichier :"
#define AATYPEC "Type :"
#define AATITLEC "Titre :"
#define AADATEC "Date :"
#define AABOUNDINGBOXC "Cadre limite :"
#define AAORIENTATIONC "Orientation :"
#define AADEFAULTMEDIAC "Taille du papier :"
#define AAPAGEORDERC "Ordre des pages :"
#define AAPAGESC "Pages :"
#define AAPAGEC "Page :"
#define AABITMAPC "Bitmap :"

/* Sounds dialog box */
#define AASOUNDS "Sons"
#define AAEVENTC "Ev�nement :"
#define AASOUNDC "Son :"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "S�lection du port de l'imprimante"
#define AASELECTPRINTER "S�lection de l'imprimante"
#define AAPRINTING "Impression"
#define AASELECTPAGE "S�lection de la page"
#define AASELECTPAGES "S�lection des pages"

/* Printer Setup dialog box */
#define AAPRINTERSETUP "Configuration de l'imprimante"
#define AADEVICEC "Type :"
#define AARESOLUTIONC "R�solution :"
#define AAQUEUEC "Imprimante :"
#define AAPRINTTOFILE "Impression dans un fichier"
#define AAPSPRINTER "Imprimante PostScript"
#define AAOPTIONSC "Options :"
#define AAFIXEDMEDIA "Format fixe"
#define AACONVERT "Convert" /* ENGLISH */
#define AAWINPRINTERSETTINGS "Windows Printer settings" /* ENGLISH */
#define AACOLOURS "Colours" /* ENGLISH */
#define AABANDW "Black and White" /* ENGLISH */
#define AAGREY "Grey" /* ENGLISH */
#define AAFULLCOLOUR "Full Colour" /* ENGLISH */
#define AASELECTGSDEVICE "Select Ghostscript device" /* ENGLISH */

#define AAODDEVEN "Impaires/Paires"
#define AAIGNOREDSC "Ignore DSC" /* ENGLISH */
#define AAPAGES "Pages"
#define AAFROMC "From:" /* ENGLISH */
#define AATOC "To:" /* ENGLISH */

#define AAADVANCED "Avanc�"
#define AAADVANCEDPSOPT "Options PostScript avanc�es"
#define AASENDCTRLDBEFORE "Envoyer CTRL+D avant le travail"
#define AASENDCTRLDAFTER "Envoyer CTRL+D apr�s le travail"
#define AAPROLOGFILE "Fichier prologue"
#define AAEPILOGFILE "Fichier �pilogue"
#define AABROWSE "Parcourir"

/* PS2EPS dialog box */
#define AAPSTOEPS "Conversion PS en EPS"
#define AAPSTOEPSREAD "Avez-vous lu la rubrique d'aide `Conversion PS en EPS` ?"
#define AAPSTOEPSAUTO "Calculer automatiquement le cadre limite"

#ifdef UNIX
#define AAAPROPERTIES "Propri�t�s"
#define AAALLPAGES "Toutes"
#define AAODDPAGES "Impaires"
#define AAEVENPAGES "Paires"
#define AAALL "Toutes"
#define AAODD "Impaires"
#define AAEVEN "Paires"
#define AAREVERSE "Ordre inverse"
#else
#ifdef _Windows
#define AAAPROPERTIES "&Propri�t�s"
#define AAALLPAGES "&Toutes"
#define AAODDPAGES "I&mpaires"
#define AAEVENPAGES "Pai&res"
#define AAALL "&Toutes"
#define AAODD "I&mpaires"
#define AAEVEN "Pai&res"
#define AAREVERSE "&Ordre inverse"
#else
#define AAAPROPERTIES "~Propri�t�s"
#define AAALLPAGES "~Toutes"
#define AAODDPAGES "I~mpaires"
#define AAEVENPAGES "Pai&res"
#define AAALL "~Toutes"
#define AAODD "I~mpaires"
#define AAEVEN "Pai~res"
#define AAREVERSE "~Ordre inverse"
#endif
#endif


/* Properties dialog box */
#define AAPROPERTIES "Propri�t�s"
#define AAPROPERTYC "Propri�t� :"
#define AAVALUEC "Valeur :"
#define AAPAGEOFFSETPT "Offset de page (pt)"
#define AAXC "X :"
#define AAYC "Y :"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Modification des propri�t�s"
#define AAPROPERTYTYPEC "Type de propri�t� :"
#define AANUMBER "Nombre"
#define AASTRING "Texte"
#define AANAMEC "Nom : par ex.  BitsPerPixel"
#define AAVALUESC "Valeurs :  par ex. 1,3,8,16,24"
#define AADELETE "Effacer"

#define AABOUNDINGBOX "Cadre limite"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Configuration avanc�e"
#define AAGHOSTSCRIPTDLLC "DLL de Ghostscript :"
#define AAGHOSTSCRIPTSOC "Ghostscript Shared Object:" /* ENGLISH */
#define AAGHOSTSCRIPTEXEC "Ghostscript Program:" /* ENGLISH */
#define AAGHOSTSCRIPTINCC "Chemin de recherche de Ghostscript :"
#define AAGHOSTSCRIPTOTHERC "Options de Ghostscript :"
#define AACOPYPRINTERDEF "Copier les param�tres par d�faut de l'imprimante"
#define AAASSOCIATEPS "Associer les fichiers .ps"
#define AAASSOCIATEPDF "Associer les fichiers .pdf"
#define AACREATESTARTMENU "Cr�er les entr�es dans le menu D�marrer"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Configuration standard"
#define AAWHICHGS "Quelle version de Ghostscript souhaitez-vous utiliser ?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "T�l�chargement de Ghostscript"
#define AADOWNLOADNOW "T�l�charger maintenant"
#define AAGSNOTFOUND "Ghostscript ne peut pas �tre trouv� sur votre ordinateur.  Vous devez t�l�charger et installer AFPL Ghostscript depuis"

#define AAGSMESSWIN "GSview pour Windows - Messages de Ghostscript"
#define AAGSMESSPM "PM GSview - Messages de Ghostscript"
#define AAGSMESSX11 "X11 GSview - Messages de Ghostscript"
#define AACOPY "Copier"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Param�tres de l'affichage"
#define AARESOLUTION "R�solution"
#define AAZOOMRESOLUTION "R�solution du zoom"
#define AADEPTH "Intensit�"
#define AATEXTALPHA "Texte Alpha"
#define AAGRAPHICSALPHA "Graphiques Alpha"
#define AADRAWMETHOD "M�thode de dessin"
#define AADPI "dpi"
#define AABPP "bits/pixel"
#define AABITS "bits"

/* Measure and custom units dialog box */
#define AAPT "pt"
#define AAMM "mm"
#define AAINCH "pouce"
#define AACUSTOM "Personnalis�e"
#define AAMEASURE "Mesure"
#define AAUNITC	"Unit� :"
#define AASTARTC "D�but :"
#define AAFINISHC "Fin :"
#define AADELTAC "Delta :"
#define AALENGTHC "Longueur :"
#define AACHANGE "Variation"
#define AACALCXFORM "Calcul de transformation"
#define AATRANSLATE "Translation"
#define AAROTATE "Rotation"
#define AASCALE "Etirement"
#define AAINITMATRIX "initmatrix"
#define AAINVERTMATRIX "invertmatrix"

/* PS to Edit dialog box */
#define AAPSTOEDIT "PS vers Edit"
#define AAFORMATC "Format :"
#define AADRAWTASP "Dessiner le texte sous forme de polygones"
#define AAMAPTOLATIN1 "Encodage ISO-Latin1"
#define AAFLATNESSC "Nettet� :"
#define AADEFAULTFONTC "Police par d�faut :"
#define AADRIVEROPTIONSC "Options du pilote :"

/* Registration */
/* ENGLISH */
#define AAREGTOC "Registered to:"
#define AANUMBERC "Number:"
#define AAGSVIEWREG "GSview Registration"
#define AAREGISTERNOW "Register Now"
#define AAONLINEREG "Online Registration"
#define AAREG1 "Please enter your name and registration number"
#define AAREG2 ""
#define AANAG1 "GSview is Copyright 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Please support the development of GSview"
#define AANAG3 "by registering it."
#define AANAG4 "GSview can be registered online at"
#define AANAG5 "    http:/\057www.ghostgum.com.au/"

/* Installation program */
#define AAINSTALL1 "Programme d'installation de GSview"
#define AAINSTALLINTRO1 "Cet assistant vous guide pendant toute la dur�e de l'installation de :"
#define AAINSTALLINTRO2 "Avant de poursuivre l'installation, veuillez-vous assurer que les fichiers list�s dans le fichier Readme.htm sont pr�sents dans le r�pertoire courant."
#define AAINSTALLINTRO3 "L'installation n�cessite 20 - 30 M octets d'espace disque."
#define AAINSTALLCOPYRIGHT1 "Informations de Copyright :"
#define AAINSTALLCOPYRIGHT2 "GSview et AFPL Ghostscript sont deux programmes distincts."
#define AAINSTALLCOPYRIGHT3 "Ils sont la propri�t� de diff�rentes personnes morales."
#define AAINSTALLGSVER1 "Quels composants voulez-vous installer ?"
#define AAINSTALLGSVER2 "GSview"
#define AAINSTALLGSVER3 "Ghostscript"
#define AAINSTALLGSVER4 "Quelle version de Ghostscript voulez-vous utiliser ?"
#define AAINSTALLDIR1 "S�lectionnez le r�pertoire d'installation de GSview et Ghostscript :"
#define AAINSTALLDIR2 "GSview va �tre install� dans le sous-r�pertoire :"
#define AAINSTALLDIR3 "Ghostscript va �tre install� dans le sous-r�pertoire :"
#define AAINSTALLMKDIR1 "Le r�pertoire sp�cifi� n'existe pas."
#define AAINSTALLMKDIR2 "Un nouveau r�pertoire va �tre cr��."
#define AAINSTALLMISSING1 "Programme d'installation de GSview - Fichier ZIP manquant"
#define AAINSTALLMISSING2 "Impossible de trouver le fichier ZIP. Veuillez ins�rer la disquette contenant le fichier ci-dessous :"
#define AAINSTALLCONFIG1 "GSview et Ghostscript utilisent la variable d'environnement TEMP pour sp�cifier un r�pertoire pour les fichiers temporaires."
#define AAINSTALLCONFIG2 "La variable d'environnement TEMP n'est pas encore d�finie."
#define AAINSTALLCONFIG3 "La ligne suivante va �tre ajout�e au fichier autoexec.bat\r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Mettre � jour le fichier autoexec.bat"
#define AAINSTALLCONFIG5 "Faire une copie de sauvegarde l'ancien fichier autoexec.bat"
#define AAINSTALLCONFIGPM1 "GSview et Ghostscript utilisent la variable d'environnement TEMP pour sp�cifier un r�pertoire pour les fichiers temporaires."
#define AAINSTALLCONFIGPM2 "La variable d'environnement TEMP n'est pas encore d�finie ou vous devez installer EMX."
#define AAINSTALLCONFIGPM3 "La ligne suivante va �tre ajout�e au fichier config.sys   'SET TEMP=C:\\'  et/ou le PATH et le LIBPATH vont �tre mis � jour pour EMX."
#define AAINSTALLCONFIGPM4 "Mettre � jour le fichier config.sys"
#define AAINSTALLCONFIGPM5 "Faire une copie de sauvegarde de l'ancien fichier config.sys"
#define AAINSTALLFINISH "L'installation peut maintenant commencer.  \r\rCliquez sur D�marrer pour poursuivre."
#define AAINSTALLUNZIP1 "D�compression des fichiers..."
#define AADIRNOTEXIST "Le r�pertoire '%s' n'existe pas.  Voulez-vous qu'il soit cr�� ?"
#define AAMKDIRFAIL "Impossible de cr�er le r�pertoire"
#define AABETAWARN "Ceci est une version BETA de GSview.  Elle cessera de fonctionner le %04d-%02d-%02d."
#define AAINSTALLOK "Installation r�ussie."
#define AAINSTALLFAILED "L'installation a �chou� :"
#define AAPROGMANGROUP1 "Voulez-vous cr�er un Groupe de programmes ?"
#define AAPROGMANGROUP2 "Cr�er un Groupe"
#define AAPROGMANGROUP3 "Nom :"
#define AAPROGMANGROUP4 "GS Tools"
#define AAPROGMANGROUP5 "Le Groupe de programmes \042%s\042 a �t� cr��."
#define AAPROGMANGROUP6 "L'objet de programme \042GSview\042 a �t� cr�� sur le bureau."
#define AAPMOBJECT1 "Voulez-vous cr�er un objet de programme pour GSview sur le bureau ?"
#define AAPMOBJECT2 "Cr�er un objet."
#define AADONEBAK "L'ancien fichier autoexec.bat a �t� renomm� %s."
#define AACANTLOAD "Chargement de %s impossible."
#define AACANTCREATETEMPFILE "Cr�ation du fichier temporaire impossible."
#define AACANTOPENREAD "Impossible de lire le fichier %s."
#define AACANTOPENWRITE "Impossible d'�crire le fichier %s."
#define AAERRORRENAME "Erreur en renommant le fichier %s en %s."
#define AANODDEPROGMAN "Ouverture de la connexion DDE avec le Gestionnaire de Programmes impossible. Veuillez relancer Windows."
#define AAINSERTDISK "Veuillez ins�rer la disquette contenant le fichier %s."
#define AAZIPNOTFOUND "Impossible de trouver le fichier ZIP."
#define AAUNZIPCANCELLED "D�compression interrompue.\n"
#define AACANTALLOCBUF "Allocation de m�moire pour copier le tampon impossible."
#define AAPROGRAMOBJECTFAILED "Cr�ation de l'objet de programme sur le bureau impossible"
#define AAUNINSTALLTITLE "GSview et AFPL Ghostscript"
#define AAUNINSTALLITEM "D�sinstaller GSview et AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "Cette installation n�cessite 3 M octets d'espace disque"
#define AAGSVIEWREQUIRES "GSview n�cessite �galement"
#define AAVIEWREADMEDETAILS "Voir le fichier Readme pour savoir comment obtenir Ghostscript."
#define AACOPYRIGHTNOTICE "Copyright"
#define AASELECTGSVIEWDIR "S�lectionnez un r�pertoire dans lequel vous souhaitez installer GSview"
#define AABROWSEE "Parcourir..."
#define AAGSVIEWADDSTARTMENU "Le programme d'installation de GSview va ajouter des entr�es dans le dossier suivant du menu D�marrer"
#define AACREATEFOLDER "Cr�er le dossier"
#define AAALLUSERS "Tous les utilisateurs"
#define AASELECTFOLDER "S�lection du dossier"
#define AAFOLDER "Dossier"
#define AADESTFOLDER "Dossier de destination"
#define AAVIEWREADME "Visualiser le fichier Readme"

/* configure */
#define AAGSVIEWC "GSview :"
#define AAALADDINGSC "AFPL Ghostscript :"
#define AACFG1 "Programme de configuration de GSview"
#define AACFG10 "Cet assistant vous aide � configurer GSview."
#define AACFG11 "Si vous rencontrez une rubrique que vous ne comprenez pas dans les pages suivantes,\
 ne modifiez pas ce qui vous est propos� par d�faut."
#define AACFG20 "GSview doit savoir quelle version de Ghostscript il doit utiliser\
 et o� elle se trouve."
#define AACFG21 "Quelle version de Ghostscript souhaitez-vous utiliser ?"
#define AACFG22 "Dans quel r�pertoire se trouve Ghostscript ?"
#define AACFG23 "Rechercher des polices de Type 1 suppl�mentaires dans :"
#define AACFG30 "GSview conserve une liste des imprimantes Ghostscript et\
 de leurs r�solutions dans son fichier d'initialisation."
#define AACFG31 "Par d�faut, la liste des imprimantes et de leurs r�solutions est extraite de la\
 distribution de Ghostscript version 5.50 et il se peut qu'elle soit incompl�te."
#define AACFG32 "Mettre � jour la liste des imprimantes"
#define AACFG33 "La mise � jour de la liste des imprimantes va remplacer toutes les entr�es d�j� existantes."
#define AACFG40 "GSview peut cr�er des associations de fichiers pour les types de fichiers PostScript et\
 Portable Document Format (PDF)."
#define AACFG43 "Gr�ce � ces associations, double-cliquer sur un fichier PostScript\
 ou PDF lancera GSview qui ouvrira ce fichier.\
  Ces associations permettent �galement � certains navigateurs WWW\
 d'utiliser GSview comme visualisateur pour les fichiers PostScript et PDF."
#define AACFG41 "Associer les fichiers PostScript (.ps et .eps) avec GSview."
#define AACFG42 "Associer les fichiers PDF (.pdf) avec GSview."
#define AACFG50 "GSview peut cr�er pour GSview et AFPL Ghostscript un Groupe de programmes dans le Gestionnaire de programmes ou\
 une entr�e dans le menu D�marrer."
#define AACFG51 "Cr�er Groupe / entr�e"
#define AACFG52 "Nom :"
#define AACFG53 "GSview peut cr�er objet de programme pour GSview sur le bureau.\
 Ceci va associer les fichiers PostScript (.ps and .eps) et PDF (.pdf) avec GSview."
#define AACFG54 "Cr�er un objet."
#define AACFG60 "GSview peut maintenant commencer la configuration. \r\rCliquez sur D�marrer pour poursuivre."
#define AACFG70 "Configuration r�ussie."
#define AACFG72 "Si vous devez modifier la configuration ult�rieurement,\
 lancez GSview et ex�cutez la commande \042Options | Configuration Standard...\042 ou\
 \042Options | Configuration avanc�e...\042."
#define AACFG73 "La configuration a �chou�."
#define AACFG74 "Configuration interrompue."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "Impression avec GSview"
#define AAGSVIEWPRINTUSAGE "Syntaxe : %s [/d] chemin_d'acc�s_dll fichier_option fichier_entr�e\nfichier_option et fichier_entr�e seront effac�s en sortie\ngvpgs est destin� � �tre appel� avec des fichiers temporaires\n"
#ifdef _Windows
#define AAMFILE "&Fichier"
#define AAMSAVEAS "Enregistrer &sous..."
#define AAMEXIT "&Quitter"
#define AAMEDIT "&Edition"
#define AAMCOPY "Copier\tCtrl+C"
#define AAMHELP "&Aide"
#define AAMABOUT "&A propos de..."
#else
#define AAMFILE "~Fichier"
#define AAMSAVEAS "Enregistrer ~sous..."
#define AAMEXIT "~Quitter"
#define AAMEDIT "~Edition"
#define AAMCOPY "Copier\tCtrl+C"
#define AAMHELP "~Aide"
#define AAMABOUT "~A propos de..."
#endif

#define WINHELPFILE "gsviewfr.chm"
