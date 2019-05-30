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

/* gr\gvclang.h */
/* Common Greek language defines */

#define AAGREEK "Ελληνικά"

#define GSVIEW_COPYRIGHT1 "Πνευματικά δικαιώματα (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "Με επιφύλαξη κάθε νόμιμου δικαιώματος."
#define GSVIEW_COPYRIGHT3 "Για περισσότερες πληροφορίες ανατρέξτε στο αρχείο LICENCE."

#define GS_COPYRIGHT1 "Πνευματικά δικαιώματα (C) 2011 Artifex Softw3are, Inc."
#define GS_COPYRIGHT2 "California, U.S.A. Με επιφύλαξη κάθε νόμιμου δικαιώματος."
#define GS_COPYRIGHT3 "Για περισσότερες πληροφορίες ανατρέξτε στο αρχείο PUBLIC."

/* Buttons */
#ifdef UNIX
#define AAOK "Ok"
#define AACANCEL "’κυρο"
#define AAEDIT "Επεξεργασία"
#define AADEFAULTS "Προεπιλογή"
#define AAHELP "Βοήθεια"
#define AANEW "Δημιουργία"
#define AATEST "Δοκιμή"
#define AAYES "Ναι"
#define AANO "Όχι"
#define AANEXTGT "Επόμενο >"
#define AABACKLT "< Προηγούμενο"
#define AAFINISHGT "Τέλος >"
#define AAEXIT "Έξοδος"
#else
#ifdef _Windows
#define AAOK "&Ok"
#define AACANCEL "’&κυρο"
#define AAEDIT "&Επεξεργασία"
#define AADEFAULTS " Π&ροεπιλογή"
#define AAHELP "&Βοήθεια"
#define AANEW "&Δημιουργία"
#define AATEST "Δο&κιμή"
#define AAYES "&Ναι"
#define AANO "Ό&χι"
#define AANEXTGT "&Επόμενο >"
#define AABACKLT "< &Προηγούμενο"
#define AAFINISHGT "&Τέλος >"
#define AAEXIT "Έξο&δος"
#else
#define AAOK "~Ok"
#define AACANCEL "’~κυρο"
#define AADEFAULTS "Π~ροεπιλογή"
#define AAEDIT "~Επεξεργασία"
#define AAHELP "~Βοήθεια"
#define AANEW "~Δημιουργία"
#define AATEST "Δο~κιμή"
#define AAYES "~Ναι"
#define AANO "Ό~χι"
#define AANEXTGT "~Επόμενο >"
#define AABACKLT "< ~Προηγούμενο"
#define AAFINISHGT "~Τέλος >"
#define AAEXIT "Έξο~δος"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "Πληροφορίες για το GSview για Windows"
#define AAABOUTPM "Πληροφορίες για το GSview για PM"
#define AAABOUTX11 "Πληροφορίες για το GSview για X11"
#define AAABOUTGSV16SPL "Πληροφορίες για το GSV16SPL"
#define AAABOUTGSVIEWPRINT "Πληροφορίες για το GSview Print"
#define AACOPY1 "Εκδοχή:"
#define AACOPY2 "Περιβάλλον εργασίας γραφικών για GhostScript"
#define AACOPY4 "Το πρόγραμμα διανέμεται ΧΩΡΙΣ ΟΠΟΙΑΣΔΗΠΟΤΕ ΜΟΡΦΗΣ ΕΓΓΥΗΣΗ."
#define AACOPY5 "Συγγραφείς και διανομείς του προγράμματος δεν αποδέχονται καμία ευθύνη για"
#define AACOPY6 "τις οποιεσδήποτε συνέπειες από τη χρήση του, τις οποιεσδήποτε χρήσεις του"
#define AACOPY7 "ή για τη λειτουργικότητά του, εκτός εάν αυτό αναφέρεται ρητά και γραπτώς."
#define AACOPY8 "Για περισσότερες πληροφορίες ανατρέξτε στην άδεια χρήσης του προγράμματος."
#define AACOPY9 "Σε κάθε αντίγραφο του GSview πρέπει να συμπεριλαμβάνεται ένα πιστό αντίγραφο"
#define AACOPY10 "της άδειας, κανονικά, ένα αρχείο κειμένου ASCII με το όνομα LICENCE. Η άδεια"
#define AACOPY11 "αυτή σας δίνει το δικαίωμα να αντιγράψετε, να τροποποιήσετε ή να διανείμετε"
#define AACOPY12 "το GSview αλλά πάντοτε σύμφωνα με τις συνθήκες οι οποίες περιγράφονται στο"
#define AACOPY13 "κείμενο της άδειας. Η άδεια αυτή επιβάλλει να συμπεριλαμβάνονται, εκτός των"
#define AACOPY14 "άλλων, η δήλωση για τα πνευματικά δικαιώματα καθώς και η παραπάνω δήλωση."
#define AACOPY15 "pstotxt3.dll, Πνευματικά δικαιώματα (C) 1995-1998 Digital Equipment Corporation."
#define AACOPY16 "pstotxt2.dll, Πνευματικά δικαιώματα (C) 1995-1998 Digital Equipment Corporation."
#define AACOPY17 "pstotxt1.dll, Πνευματικά δικαιώματα (C) 1995-1998 Digital Equipment Corporation."
#define AACOPY18 "Διανέμεται σύμφωνα με διαφορετική άδεια. Λεπτομέρειες στο pstotext.txt."
#define AACOPY19 "Συγγραφέας: Russell Lang, Ghostgum Software Pty Ltd."
#define AACOPY20 "Ένα περιβάλλον εργασίας γραφικών της Ghostscript DLL."
#define AACOPY21 "Το πρόγραμμα αυτό αποτελεί τμήμα του GSview."
#define AACOPY22 "GSview, ουρά εκτυπώσεων Win32s/Win16"

#define AAINPUT "Εισαγωγή δεδομένων"

/* DSC error dialog box */
#define AADSC "Συμβάσεις δομής εγγράφου DSC"
#define AAIGNOREALLDSC "Παράβλεψη των σχολίων DSC"

/* Info dialog box */
#define AAINFO  "Πληροφορίες"
#define AAFILEC "Αρχείο:"
#define AATYPEC "Τύπος:"
#define AATITLEC "Τίτλος:"
#define AADATEC "Ημερομηνία:"
#define AABOUNDINGBOXC "Όρια πλαισίου:"
#define AAORIENTATIONC "Προσανατολισμός:"
#define AADEFAULTMEDIAC "Είδος μέσου:"
#define AAPAGEORDERC "Σειρά σελίδων:"
#define AAPAGESC "Αριθμός σελίδων:"
#define AAPAGEC "Σελίδα:"
#define AABITMAPC "Bitmap:"

/* Sounds dialog box */
#define AASOUNDS "Ήχοι"
#define AAEVENTC "Συμβάν:"
#define AASOUNDC "Ήχος:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Επιλογή θύρας εκτυπωτή"
#define AASELECTPRINTER "Επιλογή εκτυπωτή"
#define AAPRINTING "Εκτύπωση"
#define AASELECTPAGE "Επιλογή σελίδας"
#define AASELECTPAGES "Επιλογή σελίδων"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Παράμετροι εκτύπωσης"
#define AADEVICEC "Συσκευή:"
#define AARESOLUTIONC "Ανάλυση:"
#define AAQUEUEC "Ουρά εκτύπωσης:"
#define AAPRINTTOFILE "Εκτύπωση σε αρχείο"
#define AAPSPRINTER "Εκτυπωτής PostScript"
#define AAOPTIONSC "Επιλογές:"
#define AAFIXEDMEDIA "Σταθερό μέγεθος σελίδας"
#define AACONVERT "Μετατροπή"
#define AAWINPRINTERSETTINGS "Ρυθμίσεις εκτυπωτή Windows"
#define AACOLOURS "Επιλογή χρώματος"
#define AABANDW "Ασπρόμαυρο"
#define AAGREY "Κλίμακα γκρι"
#define AAFULLCOLOUR "Έγχρωμο"
#define AASELECTGSDEVICE "Επιλογή συσκευής GhostScript"

#define AAODDEVEN "Μονές/Ζυγές"
#define AAIGNOREDSC "Παράβλεψη σχολίων DSC"
#define AAPAGES "Σελίδες"
#define AAFROMC "Από:"
#define AATOC "Έως:"

#define AAADVANCED "Πρόσθετοι"
#define AAADVANCEDPSOPT "Επιλογές PostScript για προχωρημένους"
#define AASENDCTRLDBEFORE "Αποστολή Ctrl+D πριν την εκτύπωση"
#define AASENDCTRLDAFTER "Αποστολή Ctrl+D μετά την εκτύπωση"
#define AAPROLOGFILE "Αρχείο προλόγου"
#define AAEPILOGFILE "Αρχείο επιλόγου"
#define AABROWSE "Αναζήτηση"

/* PS2EPS dialog box */
#define AAPSTOEPS "Από PS σε EPS"
#define AAPSTOEPSREAD "Διαβάσατε το κείμενο βοήθειας για τη μετατροπή 'PS σε EPS';"
#define AAPSTOEPSAUTO "Αυτόματος υπολογισμός ορίων πλαισίου"

#ifdef UNIX
#define AAAPROPERTIES "Ιδιότητες"
#define AAALLPAGES "Όλες οι σελίδες"
#define AAODDPAGES "Μονές σελίδες"
#define AAEVENPAGES "Ζυγές σελίδες"
#define AAALL "Όλες"
#define AAODD "Μονές"
#define AAEVEN "Ζυγές"
#define AAREVERSE "Ανάστροφη σειρά"
#else
#ifdef _Windows
#define AAAPROPERTIES "&Ιδιότητες"
#define AAALLPAGES "Ό&λες οι σελίδες"
#define AAODDPAGES "&Μονές σελίδες"
#define AAEVENPAGES "&Ζυγές σελίδες"
#define AAALL "Ό&λες"
#define AAODD "&Μονές"
#define AAEVEN "&Ζυγές"
#define AAREVERSE "&Ανάστροφη σειρά"
#else
#define AAAPROPERTIES "~Ιδιότητες"
#define AAALLPAGES "Ό~λες οι σελίδες"
#define AAODDPAGES "~Μονές σελίδες"
#define AAEVENPAGES "~Ζυγές σελίδες"
#define AAALL "Ό~λες"
#define AAODD "~Μονές"
#define AAEVEN "~Ζυγές"
#define AAREVERSE "~Ανάστροφη σειρά"
#endif
#endif


/* Properties dialog box */
#define AAPROPERTIES "Ιδιότητες"
#define AAPROPERTYC "Ιδιότητα:"
#define AAVALUEC "Τιμή:"
#define AAPAGEOFFSETPT "Μετακίνηση σελίδων (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Τροποποίηση ιδιοτήτων"
#define AAPROPERTYTYPEC "Τύπος ιδιότητας:"
#define AANUMBER "Αριθμός"
#define AASTRING "Συμβολοσειρά"
#define AANAMEC "Όνομα: π.χ. BitsPerPixel"
#define AAVALUESC "Τιμές: π.χ. 1,3,8,16,24"
#define AADELETE "Διαγραφή"

#define AABOUNDINGBOX "Όρια πλαισίου"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Διαμόρφωση για προχωρημένους"
#define AAGHOSTSCRIPTDLLC "Ghostscript DLL:"
#define AAGHOSTSCRIPTSOC "Κοινόχρηστο αντικείμενο Ghostscript:"
#define AAGHOSTSCRIPTEXEC "Πρόγραμμα Ghostscript:"
#define AAGHOSTSCRIPTINCC "Πρόσθετη διαδρομή αναζήτησης Ghostscript:"
#define AAGHOSTSCRIPTOTHERC "Επιλογές Ghostscript:"
#define AACOPYPRINTERDEF "Αντιγραφή προκαθορισμένων τιμών εκτυπωτών"
#define AAASSOCIATEPS "Συσχέτιση των αρχείων .ps με το πρόγραμμα GSview"
#define AAASSOCIATEPDF "Συσχέτιση των αρχείων .pdf με το πρόγραμμα GSview"
#define AACREATESTARTMENU "Δημιουργία καταχωρήσεων στο μενού Έναρξη"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Τυπική διαμόρφωση"
#define AAWHICHGS "Ποια εκδοχή του προγράμματος GhostScript θέλετε να χρησιμοποιηθεί;"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Λήψη GhostScript"
#define AADOWNLOADNOW "Λήψη τώρα"
#define AAGSNOTFOUND "Τα αρχεία GhostScript δεν εντοπίστηκαν στον υπολογιστή σας. Θα πρέπει να λάβετε και να εγκαταστήσετε τα αρχεία AFPL GhostScript από τη διεύθυνση:"

#define AAGSMESSWIN "GSview για Windows - Μηνύματα Ghostscript"
#define AAGSMESSPM "GSview για PM - Μηνύματα Ghostscript"
#define AAGSMESSX11 "GSview για X11 - Μηνύματα Ghostscript"
#define AACOPY "Αντιγραφή"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Ρυθμίσεις οθόνης"
#define AARESOLUTION "Ανάλυση"
#define AAZOOMRESOLUTION "Ανάλυση ζουμ"
#define AADEPTH "Βάθος"
#define AATEXTALPHA "’λφα κειμένου"
#define AAGRAPHICSALPHA "’λφα γραφημάτων"
#define AADRAWMETHOD "Μέθοδος απεικόνισης"
#define AADPI "dpi"
#define AABPP "bits/pixel"
#define AABITS "bits"

/* Measure and custom units dialog box */
#define AAPT "στιγμές"
#define AAMM "χιλιοστά"
#define AAINCH "ίντσες"
#define AACUSTOM "Χρήστη"
#define AAMEASURE "Μέτρηση"
#define AAUNITC	"Μονάδα:"
#define AASTARTC "Αφετηρία:"
#define AAFINISHC "Τέρμα:"
#define AADELTAC "Δέλτα:"
#define AALENGTHC "Διαδρομή:"
#define AACHANGE "Αλλαγή"
#define AACALCXFORM "Υπολογισμός μετατροπής"
#define AATRANSLATE "Μεταφορά"
#define AAROTATE "Περιστροφή"
#define AASCALE "Μεγέθυνση"
#define AAINITMATRIX "Αρχικός πίνακας"
#define AAINVERTMATRIX "Πίνακας αναστροφής"

/* PS to Edit dialog box */
#define AAPSTOEDIT "Μετατροπή σε γράφημα διανυσματικής μορφής"
#define AAFORMATC "Μορφή:"
#define AADRAWTASP "Σχεδίαση κειμένου ως πολύγωνα"
#define AAMAPTOLATIN1 "Αντιστοίχιση σε ISO-Latin1"
#define AAFLATNESSC "Ομαλότητα:"
#define AADEFAULTFONTC "Γραμματοσειρά:"
#define AADRIVEROPTIONSC "Επιλογές οδηγού:"

/* Registration */
#define AAREGTOC "Νόμιμος κάτοχος:"
#define AANUMBERC "Αριθμός:"
#define AAGSVIEWREG "Δήλωση GSview"
#define AAREGISTERNOW "Δήλωση τώρα"
#define AAONLINEREG "Ηλεκτρονική δήλωση"
#define AAREG1 "Παρακαλώ πληκτρολογήστε το όνομά σας και τον αριθμό"
#define AAREG2 "δήλωσης του προγράμματος."
#define AANAG1 "GSview - Πνευματικά δικαιώματα 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Υποστηρίξτε τη συνέχιση της ανάπτυξης του προγράμματος"
#define AANAG3 "GSview δηλώνοντας τη χρήση του."
#define AANAG4 "Η δήλωση του GSview μπορεί να γίνει ηλεκτρονικά στη διεύθυνση"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "      http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "Εγκατάσταση GSview"
#define AAINSTALLINTRO1 "Ο οδηγός αυτός θα σας βοηθήσει στην εγκατάσταση του"
#define AAINSTALLINTRO2 "Εάν για την εγκατάσταση δεν χρησιμοποιείτε το αυτο-εξαγόμενο EXE αρχείο, βεβαιωθείτε ότι στον τρέχοντα κατάλογο βρίσκονται όλα τα απαραίτητα αρχεία όπως αυτά αναφέρονται στο αρχείο Readme.htm."
#define AAINSTALLINTRO3 "Για την εγκατάσταση απαιτούνται 20 - 30 Mbytes χώρου στο δίσκο."
#define AAINSTALLCOPYRIGHT1 "Προειδοποίηση πνευματικών δικαιωμάτων"
#define AAINSTALLCOPYRIGHT2 "Τα προγράμματα GSview και AFPL Ghostscript είναι ανεξάρτητα προγράμματα."
#define AAINSTALLCOPYRIGHT3 "Ανήκουν σε δύο διαφορετικές οντότητες."
#define AAINSTALLGSVER1 "Ποια στοιχεία θέλετε να εγκατασταθούν;"
#define AAINSTALLGSVER2 "Εγκατάσταση GSview"
#define AAINSTALLGSVER3 "Εγκατάσταση Ghostscript"
#define AAINSTALLGSVER4 "Ποια εκδοχή του προγράμματος GhostScript θέλετε να χρησιμοποιηθεί;"
#define AAINSTALLDIR1 "Επιλέξτε τον κατάλογο στον οποίο πρόκειται να εγκατασταθούν GSview και Ghostscript."
#define AAINSTALLDIR2 "Σε αυτόν τον κατάλογο, το πρόγραμμα GSview θα εγκατασταθεί στον υποκατάλογο:"
#define AAINSTALLDIR3 "Το πρόγραμμα Ghostscript θα εγκατασταθεί στον υποκατάλογο:"
#define AAINSTALLMKDIR1 "Ο κατάλογος που καθορίσατε δεν υπάρχει."
#define AAINSTALLMKDIR2 "Ένας νέος κατάλογος θα δημιουργηθεί."
#define AAINSTALLMISSING1 "Εγκατάσταση GSview - Αδύνατος ο εντοπισμός του αρχείου ZIP"
#define AAINSTALLMISSING2 "Αδύνατος ο εντοπισμός του αρχείου ZIP. Τοποθετήστε στη μονάδα τη δισκέτα με το ακόλουθο αρχείο:"
#define AAINSTALLCONFIG1 "Τα προγράμματα GSview και Ghostscript χρησιμοποιούν τη μεταβλητή περιβάλλοντος TEMP για τον καθορισμό καταλόγου προσωρινών αρχείων."
#define AAINSTALLCONFIG2 "Η μεταβλητή περιβάλλοντος TEMP δεν έχει καθοριστεί."
#define AAINSTALLCONFIG3 "Η ακόλουθη γραμμή θα προστεθεί στο αρχείο Autoexec.bat:\012      SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Ενημέρωση του αρχείου Autoexec.bat"
#define AAINSTALLCONFIG5 "Δημιουργία εφεδρικού αντίγραφου του υπάρχοντος Autoexec.bat"
#define AAINSTALLCONFIGPM1 "Τα προγράμματα GSview και Ghostscript χρησιμοποιούν τη μεταβλητή περιβάλλοντος TEMP για τον καθορισμό καταλόγου προσωρινών αρχείων."
#define AAINSTALLCONFIGPM2 "Η μεταβλητή περιβάλλοντος TEMP δεν έχει καθοριστεί ή πρέπει να εγκαταστήσετε EMX."
#define AAINSTALLCONFIGPM3 "Η ακόλουθη γραμμή θα προστεθεί στο αρχείο Config.sys   'SET TEMP=C:\\'  ή/και οι μεταβλητές περιβάλλοντος PATH και LIBPATH θα ενημερωθούν κατάλληλα λόγω EMX."
#define AAINSTALLCONFIGPM4 "Ενημέρωση του αρχείου Config.sys"
#define AAINSTALLCONFIGPM5 "Δημιουργία εφεδρικού αντίγραφου του υπάρχοντος Config.sys"
#define AAINSTALLFINISH "Όλα είναι έτοιμα για να πραγματοποιηθεί η εγκατάσταση του GSview.\012Πατήστε το πλήκτρο Τέλος για να αρχίσει η εγκατάσταση."
#define AAINSTALLUNZIP1 "Αποσυμπίεση αρχείων..."
#define AADIRNOTEXIST "Ο κατάλογος '%s' δεν υπάρχει. Να δημιουργηθεί;"
#define AAMKDIRFAIL "Αδύνατη η δημιουργία του καταλόγου"
#define AABETAWARN "Η συγκεκριμένη έκδοση του προγράμματος GSview είναι δοκιμαστική. Η χρήση του θα καταστεί αδύνατη μετά την %04d-%02d-%02d."
#define AAINSTALLOK "Η εγκατάσταση ολοκληρώθηκε με επιτυχία."
#define AAINSTALLFAILED "Η εγκατάσταση απέτυχε."
#define AAPROGMANGROUP1 "Το πρόγραμμα εγκατάστασης του GSview μπορεί να δημιουργήσει καταχωρήσεις στο μενού Έναρξη/στις ομάδες Διαχείρισης προγραμμάτων."
#define AAPROGMANGROUP2 "Δημιουργία Ομάδας/Καταχώρησης"
#define AAPROGMANGROUP3 "Όνομα"
#define AAPROGMANGROUP4 "Εργαλεία GS"
#define AAPROGMANGROUP5 "Δημιουργήθηκε μια ομάδα του με το όνομα \042%s\042 για το πρόγραμμα Διαχείρισης προγραμμάτων."
#define AAPROGMANGROUP6 "Δημιουργήθηκε ένα νέο αντικείμενο στην επιφάνεια εργασίας με το όνομα \042GSview\042."
#define AAPMOBJECT1 "Το πρόγραμμα εγκατάστασης του GSview μπορεί να δημιουργήσει ένα αντικείμενο για το GSview στην επιφάνεια εργασίας."
#define AAPMOBJECT2 "Δημιουργία αντικειμένου για το GSview."
#define AADONEBAK "Το παλαιό αρχείο Autoexec.bat μετονομάστηκε σε %s"
#define AACANTLOAD "Αδύνατη η φόρτωση του %s"
#define AACANTCREATETEMPFILE "Αδύνατη η δημιουργία προσωρινού αρχείου"
#define AACANTOPENREAD "Αδύνατο το άνοιγμα του %s για ανάγνωση"
#define AACANTOPENWRITE "Αδύνατο το άνοιγμα του %s για εγγραφή"
#define AAERRORRENAME "Σφάλμα κατά τη μετονομασία του %s σε %s"
#define AANODDEPROGMAN "Αδύνατη η εγκατάσταση επικοινωνίας DDE με το πρόγραμμα διαχείρισης προγραμμάτων. Επιβάλλεται η επανεκκίνηση των Windows."
#define AAINSERTDISK "Τοποθετήστε τη δισκέτα στην οποία περιέχεται το %s"
#define AAZIPNOTFOUND "Το αρχείο zip δεν εντοπίστηκε"
#define AAUNZIPCANCELLED "Η αποσυμπίεση ακυρώθηκε\012"
#define AACANTALLOCBUF "Αδύνατη η εκχώρηση μνήμης για buffer αντιγραφής"
#define AAPROGRAMOBJECTFAILED "Αδύνατη η δημιουργία αντικειμένου στην επιφάνεια εργασίας"
#define AAUNINSTALLTITLE "GSview και AFPL Ghostscript"
#define AAUNINSTALLITEM "Κατάργηση εγκατάστασης GSview και AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "Για την εγκατάσταση αυτή απαιτείται χώρος 7 Mbytes στο μόνιμο δίσκο."
#define AAGSVIEWREQUIRES "Το GSview επίσης απαιτεί την ύπαρξη του μεταγλωττιστή "
#define AAVIEWREADMEDETAILS "Διαβάστε το αρχείο Readme για τον τρόπο προμήθειας του συνόλου των προγραμμάτων Ghostscript."
#define AACOPYRIGHTNOTICE "Πνευματικά δικαιώματα"
#define AASELECTGSVIEWDIR "Επιλέξτε κατάλογο στον οποίο πρόκειται να εγκατασταθεί το GSview."
#define AABROWSEE "Αναζήτηση..."
#define AAGSVIEWADDSTARTMENU "Το πρόγραμμα εγκατάστασης του GSview, εφόσον επιλεγεί, θα προσθέσει στοιχεία στον ακόλουθο φάκελο του μενού Έναρξη."
#define AACREATEFOLDER "Δημιουργία φακέλου"
#define AAALLUSERS "Για όλους τους χρήστες"
#define AASELECTFOLDER "Επιλογή φακέλου"
#define AAFOLDER "Φάκελος"
#define AADESTFOLDER "Φάκελος προορισμού"
#define AAVIEWREADME "Εξέταση του Readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "Διαμόρφωση GSview"
#define AACFG10 "Ο οδηγός αυτός θα σας βοηθήσει στη διαμόρφωση του GSview."
#define AACFG11 "Εάν δεν γνωρίζετε κάποιες από τις ακόλουθες παραμέτρους,\
 μην αλλάξετε τις τιμές. Οι προεπιλεγμένες τιμές είναι λογικές."
#define AACFG20 "Για τη λειτουργία του, το GSview πρέπει να γνωρίζει την\
 εκδοχή και την τοποθεσία των προγραμμάτων Ghostscript."
#define AACFG21 "Ποια εκδοχή του προγράμματος GhostScript θέλετε να χρησιμοποιηθεί;"
#define AACFG22 "Σε ποιον κατάλογο βρίσκονται εγκατεστημένα τα προγράμματα Ghostscript;"
#define AACFG23 "Αναζήτηση επιπρόσθετων γραμματοσειρών Type 1 στο"
#define AACFG30 "Το πρόγραμμα GSview διατηρεί έναν κατάλογο των συσκευών Ghostscript\
 και των ρυθμίσεών τους στο αρχείο προετοιμασίας."
#define AACFG31 "Ο προεπιλεγμένος κατάλογος τιμών και ρυθμίσεων έχει ληφθεί από την\
 τυπικά διανεμόμενη εκδοχή Ghostscript 5.50 και μπορεί να είναι ελλιπής."
#define AACFG32 "Ενημέρωση καταλόγου εκτυπωτών GSview"
#define AACFG33 "Ενημέρωση του καταλόγου εκτυπωτών GSview θα αντικαταστήσει\
 τις υπάρχουσες καταχωρήσεις."
#define AACFG40 "Το πρόγραμμα GSview μπορεί να συσχετιστεί με τα αρχεία PostScript\
 (PS) και Portable Document Format (PDF)."
#define AACFG43 "Με τον τρόπο αυτό, όταν κάνετε διπλό κλικ σε ένα αρχείο\
 PostScript ή PDF θα γίνεται αυτόματα η έναρξη του GSview.\012Επίσης,\
 με τη συσχέτιση αυτή τα προγράμματα ανάγνωσης ιστοσελίδων θα\
 χρησιμοποιούν το GSview για την εξέταση των αρχείων PostScript ή/και PDF."
#define AACFG41 "Συσχέτιση των αρχείων PostScript (.ps και .eps) με το GSview"
#define AACFG42 "Συσχέτιση των αρχείων PDF (.pdf) με το GSview"
#define AACFG50 "Το πρόγραμμα GSview μπορεί να δημιουργήσει καταχωρήσεις στο\
 μενού Έναρξη/στις ομάδες Διαχείρισης προγραμμάτων για τα GSview και AFPL Ghostscript."
#define AACFG51 "Δημιουργία Ομάδας/Καταχώρησης"
#define AACFG52 "Όνομα"
#define AACFG53 "Το GSview μπορεί να δημιουργήσει ένα αντικείμενο στην επιφάνεια εργασίας.\
 Αυτό θα συσχετιστεί με τα αρχεία PostScript (.ps και .eps) και Portable Document Format\
 (PDF) με το GSview."
#define AACFG54 "Δημιουργία αντικειμένου προγράμματος"
#define AACFG60 "Όλα είναι έτοιμα για να αρχίσει η διαμόρφωση του GSview. \r\rΠατήστε Τέλος για να αρχίσει η διαμόρφωση."
#define AACFG70 "Η διαμόρφωση ολοκληρώθηκε με επιτυχία."
#define AACFG72 "Εάν αργότερα θελήσετε να τροποποιήσετε τη διαμόρφωση,\
 εκτελέστε το GSview και επιλέξτε \042Επιλογές | Τυπική διαμόρφωση...\042 ή\
 \042Επιλογές | Διαμόρφωση για προχωρημένους...\042."
#define AACFG73 "Η διαμόρφωση απέτυχε."
#define AACFG74 "Η διαμόρφωση ακυρώθηκε."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "Εκτύπωση GSview"
#define AAGSVIEWPRINTUSAGE "Χρήση: %s [/d] διαδρομή_dll αρχείο_επιλογών αρχείο_εισόδου\012Τα αρχείο_επιλογών και αρχείο_εισόδου θα διαγραφούν κατά την έξοδο του προγράμματος\012Ο σκοπός είναι το gvpgs να καλείται με προσωρινά αρχεία\012"
#ifdef _Windows
#define AAMFILE "&Αρχείο"
#define AAMSAVEAS "Αποθήκευση &ως..."
#define AAMEXIT "Έξο&δος"
#define AAMEDIT "&Επεξεργασία"
#define AAMCOPY "Αντιγραφή\tCtrl+C"
#define AAMHELP "&Βοήθεια"
#define AAMABOUT "&Πληροφορίες..."
#else
#define AAMFILE "~Αρχείο"
#define AAMSAVEAS "Αποθήκευση ~ως..."
#define AAMEXIT "Έξο~δος"
#define AAMEDIT "~Επεξεργασία"
#define AAMCOPY "Αντιγραφή\tCtrl+C"
#define AAMHELP "~Βοήθεια"
#define AAMABOUT "~Πληροφορίες..."
#endif

#define WINHELPFILE "gsviewgr.chm"
