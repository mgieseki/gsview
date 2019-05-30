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

#define AAGREEK "��������"

#define GSVIEW_COPYRIGHT1 "���������� ���������� (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "�� ��������� ���� ������� �����������."
#define GSVIEW_COPYRIGHT3 "��� ������������ ����������� ��������� ��� ������ LICENCE."

#define GS_COPYRIGHT1 "���������� ���������� (C) 2011 Artifex Softw3are, Inc."
#define GS_COPYRIGHT2 "California, U.S.A. �� ��������� ���� ������� �����������."
#define GS_COPYRIGHT3 "��� ������������ ����������� ��������� ��� ������ PUBLIC."

/* Buttons */
#ifdef UNIX
#define AAOK "Ok"
#define AACANCEL "�����"
#define AAEDIT "�����������"
#define AADEFAULTS "����������"
#define AAHELP "�������"
#define AANEW "����������"
#define AATEST "������"
#define AAYES "���"
#define AANO "���"
#define AANEXTGT "������� >"
#define AABACKLT "< �����������"
#define AAFINISHGT "����� >"
#define AAEXIT "������"
#else
#ifdef _Windows
#define AAOK "&Ok"
#define AACANCEL "�&����"
#define AAEDIT "&�����������"
#define AADEFAULTS " �&���������"
#define AAHELP "&�������"
#define AANEW "&����������"
#define AATEST "��&����"
#define AAYES "&���"
#define AANO "�&��"
#define AANEXTGT "&������� >"
#define AABACKLT "< &�����������"
#define AAFINISHGT "&����� >"
#define AAEXIT "���&���"
#else
#define AAOK "~Ok"
#define AACANCEL "�~����"
#define AADEFAULTS "�~���������"
#define AAEDIT "~�����������"
#define AAHELP "~�������"
#define AANEW "~����������"
#define AATEST "��~����"
#define AAYES "~���"
#define AANO "�~��"
#define AANEXTGT "~������� >"
#define AABACKLT "< ~�����������"
#define AAFINISHGT "~����� >"
#define AAEXIT "���~���"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "����������� ��� �� GSview ��� Windows"
#define AAABOUTPM "����������� ��� �� GSview ��� PM"
#define AAABOUTX11 "����������� ��� �� GSview ��� X11"
#define AAABOUTGSV16SPL "����������� ��� �� GSV16SPL"
#define AAABOUTGSVIEWPRINT "����������� ��� �� GSview Print"
#define AACOPY1 "������:"
#define AACOPY2 "���������� �������� �������� ��� GhostScript"
#define AACOPY4 "�� ��������� ���������� ����� ������������ ������ �������."
#define AACOPY5 "���������� ��� ��������� ��� ������������ ��� ����������� ����� ������ ���"
#define AACOPY6 "��� ������������ ��������� ��� �� ����� ���, ��� ������������ ������� ���"
#define AACOPY7 "� ��� �� ��������������� ���, ����� ��� ���� ���������� ���� ��� �������."
#define AACOPY8 "��� ������������ ����������� ��������� ���� ����� ������ ��� ������������."
#define AACOPY9 "�� ���� ��������� ��� GSview ������ �� ����������������� ��� ����� ���������"
#define AACOPY10 "��� ������, ��������, ��� ������ �������� ASCII �� �� ����� LICENCE. � �����"
#define AACOPY11 "���� ��� ����� �� �������� �� �����������, �� ������������� � �� ����������"
#define AACOPY12 "�� GSview ���� ������� ������� �� ��� �������� �� ������ ������������� ���"
#define AACOPY13 "������� ��� ������. � ����� ���� ��������� �� ������������������, ����� ���"
#define AACOPY14 "�����, � ������ ��� �� ���������� ���������� ����� ��� � �������� ������."
#define AACOPY15 "pstotxt3.dll, ���������� ���������� (C) 1995-1998 Digital Equipment Corporation."
#define AACOPY16 "pstotxt2.dll, ���������� ���������� (C) 1995-1998 Digital Equipment Corporation."
#define AACOPY17 "pstotxt1.dll, ���������� ���������� (C) 1995-1998 Digital Equipment Corporation."
#define AACOPY18 "���������� ������� �� ����������� �����. ������������ ��� pstotext.txt."
#define AACOPY19 "����������: Russell Lang, Ghostgum Software Pty Ltd."
#define AACOPY20 "��� ���������� �������� �������� ��� Ghostscript DLL."
#define AACOPY21 "�� ��������� ���� �������� ����� ��� GSview."
#define AACOPY22 "GSview, ���� ���������� Win32s/Win16"

#define AAINPUT "�������� ���������"

/* DSC error dialog box */
#define AADSC "��������� ����� �������� DSC"
#define AAIGNOREALLDSC "��������� ��� ������� DSC"

/* Info dialog box */
#define AAINFO  "�����������"
#define AAFILEC "������:"
#define AATYPEC "�����:"
#define AATITLEC "������:"
#define AADATEC "����������:"
#define AABOUNDINGBOXC "���� ��������:"
#define AAORIENTATIONC "���������������:"
#define AADEFAULTMEDIAC "����� �����:"
#define AAPAGEORDERC "����� �������:"
#define AAPAGESC "������� �������:"
#define AAPAGEC "������:"
#define AABITMAPC "Bitmap:"

/* Sounds dialog box */
#define AASOUNDS "����"
#define AAEVENTC "������:"
#define AASOUNDC "����:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "������� ����� ��������"
#define AASELECTPRINTER "������� ��������"
#define AAPRINTING "��������"
#define AASELECTPAGE "������� �������"
#define AASELECTPAGES "������� �������"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "���������� ���������"
#define AADEVICEC "�������:"
#define AARESOLUTIONC "�������:"
#define AAQUEUEC "���� ���������:"
#define AAPRINTTOFILE "�������� �� ������"
#define AAPSPRINTER "��������� PostScript"
#define AAOPTIONSC "��������:"
#define AAFIXEDMEDIA "������� ������� �������"
#define AACONVERT "���������"
#define AAWINPRINTERSETTINGS "��������� �������� Windows"
#define AACOLOURS "������� ��������"
#define AABANDW "����������"
#define AAGREY "������� ����"
#define AAFULLCOLOUR "�������"
#define AASELECTGSDEVICE "������� �������� GhostScript"

#define AAODDEVEN "�����/�����"
#define AAIGNOREDSC "��������� ������� DSC"
#define AAPAGES "�������"
#define AAFROMC "���:"
#define AATOC "���:"

#define AAADVANCED "���������"
#define AAADVANCEDPSOPT "�������� PostScript ��� �������������"
#define AASENDCTRLDBEFORE "�������� Ctrl+D ���� ��� ��������"
#define AASENDCTRLDAFTER "�������� Ctrl+D ���� ��� ��������"
#define AAPROLOGFILE "������ ��������"
#define AAEPILOGFILE "������ ��������"
#define AABROWSE "���������"

/* PS2EPS dialog box */
#define AAPSTOEPS "��� PS �� EPS"
#define AAPSTOEPSREAD "��������� �� ������� �������� ��� �� ��������� 'PS �� EPS';"
#define AAPSTOEPSAUTO "��������� ����������� ����� ��������"

#ifdef UNIX
#define AAAPROPERTIES "���������"
#define AAALLPAGES "���� �� �������"
#define AAODDPAGES "����� �������"
#define AAEVENPAGES "����� �������"
#define AAALL "����"
#define AAODD "�����"
#define AAEVEN "�����"
#define AAREVERSE "��������� �����"
#else
#ifdef _Windows
#define AAAPROPERTIES "&���������"
#define AAALLPAGES "�&��� �� �������"
#define AAODDPAGES "&����� �������"
#define AAEVENPAGES "&����� �������"
#define AAALL "�&���"
#define AAODD "&�����"
#define AAEVEN "&�����"
#define AAREVERSE "&��������� �����"
#else
#define AAAPROPERTIES "~���������"
#define AAALLPAGES "�~��� �� �������"
#define AAODDPAGES "~����� �������"
#define AAEVENPAGES "~����� �������"
#define AAALL "�~���"
#define AAODD "~�����"
#define AAEVEN "~�����"
#define AAREVERSE "~��������� �����"
#endif
#endif


/* Properties dialog box */
#define AAPROPERTIES "���������"
#define AAPROPERTYC "��������:"
#define AAVALUEC "����:"
#define AAPAGEOFFSETPT "���������� ������� (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "����������� ���������"
#define AAPROPERTYTYPEC "����� ���������:"
#define AANUMBER "�������"
#define AASTRING "������������"
#define AANAMEC "�����: �.�. BitsPerPixel"
#define AAVALUESC "�����: �.�. 1,3,8,16,24"
#define AADELETE "��������"

#define AABOUNDINGBOX "���� ��������"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "���������� ��� �������������"
#define AAGHOSTSCRIPTDLLC "Ghostscript DLL:"
#define AAGHOSTSCRIPTSOC "����������� ����������� Ghostscript:"
#define AAGHOSTSCRIPTEXEC "��������� Ghostscript:"
#define AAGHOSTSCRIPTINCC "�������� �������� ���������� Ghostscript:"
#define AAGHOSTSCRIPTOTHERC "�������� Ghostscript:"
#define AACOPYPRINTERDEF "��������� ��������������� ����� ���������"
#define AAASSOCIATEPS "��������� ��� ������� .ps �� �� ��������� GSview"
#define AAASSOCIATEPDF "��������� ��� ������� .pdf �� �� ��������� GSview"
#define AACREATESTARTMENU "���������� ������������ ��� ����� ������"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "������ ����������"
#define AAWHICHGS "���� ������ ��� ������������ GhostScript ������ �� ��������������;"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "���� GhostScript"
#define AADOWNLOADNOW "���� ����"
#define AAGSNOTFOUND "�� ������ GhostScript ��� ������������ ���� ���������� ���. �� ������ �� ������ ��� �� ������������� �� ������ AFPL GhostScript ��� �� ���������:"

#define AAGSMESSWIN "GSview ��� Windows - �������� Ghostscript"
#define AAGSMESSPM "GSview ��� PM - �������� Ghostscript"
#define AAGSMESSX11 "GSview ��� X11 - �������� Ghostscript"
#define AACOPY "���������"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "��������� ������"
#define AARESOLUTION "�������"
#define AAZOOMRESOLUTION "������� ����"
#define AADEPTH "�����"
#define AATEXTALPHA "���� ��������"
#define AAGRAPHICSALPHA "���� ����������"
#define AADRAWMETHOD "������� �����������"
#define AADPI "dpi"
#define AABPP "bits/pixel"
#define AABITS "bits"

/* Measure and custom units dialog box */
#define AAPT "�������"
#define AAMM "��������"
#define AAINCH "������"
#define AACUSTOM "������"
#define AAMEASURE "�������"
#define AAUNITC	"������:"
#define AASTARTC "��������:"
#define AAFINISHC "�����:"
#define AADELTAC "�����:"
#define AALENGTHC "��������:"
#define AACHANGE "������"
#define AACALCXFORM "����������� ����������"
#define AATRANSLATE "��������"
#define AAROTATE "����������"
#define AASCALE "���������"
#define AAINITMATRIX "������� �������"
#define AAINVERTMATRIX "������� ����������"

/* PS to Edit dialog box */
#define AAPSTOEDIT "��������� �� ������� ������������� ������"
#define AAFORMATC "�����:"
#define AADRAWTASP "�������� �������� �� ��������"
#define AAMAPTOLATIN1 "������������ �� ISO-Latin1"
#define AAFLATNESSC "���������:"
#define AADEFAULTFONTC "�������������:"
#define AADRIVEROPTIONSC "�������� ������:"

/* Registration */
#define AAREGTOC "������� �������:"
#define AANUMBERC "�������:"
#define AAGSVIEWREG "������ GSview"
#define AAREGISTERNOW "������ ����"
#define AAONLINEREG "����������� ������"
#define AAREG1 "�������� �������������� �� ����� ��� ��� ��� ������"
#define AAREG2 "������� ��� ������������."
#define AANAG1 "GSview - ���������� ���������� 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "����������� �� �������� ��� ��������� ��� ������������"
#define AANAG3 "GSview ���������� �� ����� ���."
#define AANAG4 "� ������ ��� GSview ������ �� ����� ����������� ��� ���������"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "      http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "����������� GSview"
#define AAINSTALLINTRO1 "� ������ ����� �� ��� �������� ���� ����������� ���"
#define AAINSTALLINTRO2 "��� ��� ��� ����������� ��� �������������� �� ����-��������� EXE ������, ����������� ��� ���� �������� �������� ���������� ��� �� ���������� ������ ���� ���� ����������� ��� ������ Readme.htm."
#define AAINSTALLINTRO3 "��� ��� ����������� ����������� 20 - 30 Mbytes ����� ��� �����."
#define AAINSTALLCOPYRIGHT1 "������������� ����������� �����������"
#define AAINSTALLCOPYRIGHT2 "�� ����������� GSview ��� AFPL Ghostscript ����� ���������� �����������."
#define AAINSTALLCOPYRIGHT3 "������� �� ��� ������������ ���������."
#define AAINSTALLGSVER1 "���� �������� ������ �� �������������;"
#define AAINSTALLGSVER2 "����������� GSview"
#define AAINSTALLGSVER3 "����������� Ghostscript"
#define AAINSTALLGSVER4 "���� ������ ��� ������������ GhostScript ������ �� ��������������;"
#define AAINSTALLDIR1 "�������� ��� �������� ���� ����� ��������� �� ������������� GSview ��� Ghostscript."
#define AAINSTALLDIR2 "�� ����� ��� ��������, �� ��������� GSview �� ������������ ���� �����������:"
#define AAINSTALLDIR3 "�� ��������� Ghostscript �� ������������ ���� �����������:"
#define AAINSTALLMKDIR1 "� ��������� ��� ���������� ��� �������."
#define AAINSTALLMKDIR2 "���� ���� ��������� �� ������������."
#define AAINSTALLMISSING1 "����������� GSview - �������� � ���������� ��� ������� ZIP"
#define AAINSTALLMISSING2 "�������� � ���������� ��� ������� ZIP. ����������� ��� ������ �� ������� �� �� �������� ������:"
#define AAINSTALLCONFIG1 "�� ����������� GSview ��� Ghostscript ������������� �� ��������� ������������� TEMP ��� ��� ��������� ��������� ���������� �������."
#define AAINSTALLCONFIG2 "� ��������� ������������� TEMP ��� ���� ����������."
#define AAINSTALLCONFIG3 "� �������� ������ �� ��������� ��� ������ Autoexec.bat:\012      SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "��������� ��� ������� Autoexec.bat"
#define AAINSTALLCONFIG5 "���������� ��������� ���������� ��� ���������� Autoexec.bat"
#define AAINSTALLCONFIGPM1 "�� ����������� GSview ��� Ghostscript ������������� �� ��������� ������������� TEMP ��� ��� ��������� ��������� ���������� �������."
#define AAINSTALLCONFIGPM2 "� ��������� ������������� TEMP ��� ���� ���������� � ������ �� ������������� EMX."
#define AAINSTALLCONFIGPM3 "� �������� ������ �� ��������� ��� ������ Config.sys   'SET TEMP=C:\\'  �/��� �� ���������� ������������� PATH ��� LIBPATH �� ����������� ��������� ���� EMX."
#define AAINSTALLCONFIGPM4 "��������� ��� ������� Config.sys"
#define AAINSTALLCONFIGPM5 "���������� ��������� ���������� ��� ���������� Config.sys"
#define AAINSTALLFINISH "��� ����� ������ ��� �� ��������������� � ����������� ��� GSview.\012������� �� ������� ����� ��� �� ������� � �����������."
#define AAINSTALLUNZIP1 "����������� �������..."
#define AADIRNOTEXIST "� ��������� '%s' ��� �������. �� ������������;"
#define AAMKDIRFAIL "������� � ���������� ��� ���������"
#define AABETAWARN "� ������������ ������ ��� ������������ GSview ����� �����������. � ����� ��� �� �������� ������� ���� ��� %04d-%02d-%02d."
#define AAINSTALLOK "� ����������� ������������ �� ��������."
#define AAINSTALLFAILED "� ����������� �������."
#define AAPROGMANGROUP1 "�� ��������� ������������ ��� GSview ������ �� ������������ ������������ ��� ����� ������/���� ������ ����������� ������������."
#define AAPROGMANGROUP2 "���������� ������/�����������"
#define AAPROGMANGROUP3 "�����"
#define AAPROGMANGROUP4 "�������� GS"
#define AAPROGMANGROUP5 "������������� ��� ����� ��� �� �� ����� \042%s\042 ��� �� ��������� ����������� ������������."
#define AAPROGMANGROUP6 "������������� ��� ��� ����������� ���� ��������� �������� �� �� ����� \042GSview\042."
#define AAPMOBJECT1 "�� ��������� ������������ ��� GSview ������ �� ������������ ��� ����������� ��� �� GSview ���� ��������� ��������."
#define AAPMOBJECT2 "���������� ������������ ��� �� GSview."
#define AADONEBAK "�� ������ ������ Autoexec.bat ������������� �� %s"
#define AACANTLOAD "������� � ������� ��� %s"
#define AACANTCREATETEMPFILE "������� � ���������� ���������� �������"
#define AACANTOPENREAD "������� �� ������� ��� %s ��� ��������"
#define AACANTOPENWRITE "������� �� ������� ��� %s ��� �������"
#define AAERRORRENAME "������ ���� �� ����������� ��� %s �� %s"
#define AANODDEPROGMAN "������� � ����������� ������������ DDE �� �� ��������� ����������� ������������. ����������� � ������������ ��� Windows."
#define AAINSERTDISK "����������� �� ������� ���� ����� ���������� �� %s"
#define AAZIPNOTFOUND "�� ������ zip ��� �����������"
#define AAUNZIPCANCELLED "� ����������� ���������\012"
#define AACANTALLOCBUF "������� � �������� ������ ��� buffer ����������"
#define AAPROGRAMOBJECTFAILED "������� � ���������� ������������ ���� ��������� ��������"
#define AAUNINSTALLTITLE "GSview ��� AFPL Ghostscript"
#define AAUNINSTALLITEM "��������� ������������ GSview ��� AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "��� ��� ����������� ���� ���������� ����� 7 Mbytes ��� ������ �����."
#define AAGSVIEWREQUIRES "�� GSview ������ ������� ��� ������ ��� ������������� "
#define AAVIEWREADMEDETAILS "�������� �� ������ Readme ��� ��� ����� ���������� ��� ������� ��� ������������ Ghostscript."
#define AACOPYRIGHTNOTICE "���������� ����������"
#define AASELECTGSVIEWDIR "�������� �������� ���� ����� ��������� �� ������������ �� GSview."
#define AABROWSEE "���������..."
#define AAGSVIEWADDSTARTMENU "�� ��������� ������������ ��� GSview, ������ ��������, �� ��������� �������� ���� �������� ������ ��� ����� ������."
#define AACREATEFOLDER "���������� �������"
#define AAALLUSERS "��� ����� ���� �������"
#define AASELECTFOLDER "������� �������"
#define AAFOLDER "�������"
#define AADESTFOLDER "������� ����������"
#define AAVIEWREADME "������� ��� Readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "���������� GSview"
#define AACFG10 "� ������ ����� �� ��� �������� ��� ���������� ��� GSview."
#define AACFG11 "��� ��� ��������� ������� ��� ��� ��������� �����������,\
 ��� �������� ��� �����. �� �������������� ����� ����� �������."
#define AACFG20 "��� �� ���������� ���, �� GSview ������ �� �������� ���\
 ������ ��� ��� ��������� ��� ������������ Ghostscript."
#define AACFG21 "���� ������ ��� ������������ GhostScript ������ �� ��������������;"
#define AACFG22 "�� ����� �������� ���������� ������������� �� ����������� Ghostscript;"
#define AACFG23 "��������� ������������ �������������� Type 1 ���"
#define AACFG30 "�� ��������� GSview �������� ���� �������� ��� �������� Ghostscript\
 ��� ��� ��������� ���� ��� ������ �������������."
#define AACFG31 "� �������������� ��������� ����� ��� ��������� ���� ������ ��� ���\
 ������ ����������� ������ Ghostscript 5.50 ��� ������ �� ����� �������."
#define AACFG32 "��������� ��������� ��������� GSview"
#define AACFG33 "��������� ��� ��������� ��������� GSview �� ��������������\
 ��� ���������� ������������."
#define AACFG40 "�� ��������� GSview ������ �� ����������� �� �� ������ PostScript\
 (PS) ��� Portable Document Format (PDF)."
#define AACFG43 "�� ��� ����� ����, ���� ������ ����� ���� �� ��� ������\
 PostScript � PDF �� ������� �������� � ������ ��� GSview.\012������,\
 �� �� ��������� ���� �� ����������� ��������� ����������� ��\
 ������������� �� GSview ��� ��� ������� ��� ������� PostScript �/��� PDF."
#define AACFG41 "��������� ��� ������� PostScript (.ps ��� .eps) �� �� GSview"
#define AACFG42 "��������� ��� ������� PDF (.pdf) �� �� GSview"
#define AACFG50 "�� ��������� GSview ������ �� ������������ ������������ ���\
 ����� ������/���� ������ ����������� ������������ ��� �� GSview ��� AFPL Ghostscript."
#define AACFG51 "���������� ������/�����������"
#define AACFG52 "�����"
#define AACFG53 "�� GSview ������ �� ������������ ��� ����������� ���� ��������� ��������.\
 ���� �� ����������� �� �� ������ PostScript (.ps ��� .eps) ��� Portable Document Format\
 (PDF) �� �� GSview."
#define AACFG54 "���������� ������������ ������������"
#define AACFG60 "��� ����� ������ ��� �� ������� � ���������� ��� GSview. \r\r������� ����� ��� �� ������� � ����������."
#define AACFG70 "� ���������� ������������ �� ��������."
#define AACFG72 "��� �������� �������� �� ������������� �� ����������,\
 ��������� �� GSview ��� �������� \042�������� | ������ ����������...\042 �\
 \042�������� | ���������� ��� �������������...\042."
#define AACFG73 "� ���������� �������."
#define AACFG74 "� ���������� ���������."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "�������� GSview"
#define AAGSVIEWPRINTUSAGE "�����: %s [/d] ��������_dll ������_�������� ������_�������\012�� ������_�������� ��� ������_������� �� ���������� ���� ��� ����� ��� ������������\012� ������ ����� �� gvpgs �� �������� �� ��������� ������\012"
#ifdef _Windows
#define AAMFILE "&������"
#define AAMSAVEAS "���������� &��..."
#define AAMEXIT "���&���"
#define AAMEDIT "&�����������"
#define AAMCOPY "���������\tCtrl+C"
#define AAMHELP "&�������"
#define AAMABOUT "&�����������..."
#else
#define AAMFILE "~������"
#define AAMSAVEAS "���������� ~��..."
#define AAMEXIT "���~���"
#define AAMEDIT "~�����������"
#define AAMCOPY "���������\tCtrl+C"
#define AAMHELP "~�������"
#define AAMABOUT "~�����������..."
#endif

#define WINHELPFILE "gsviewgr.chm"
