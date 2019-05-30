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

/* en\gvclang.h */
/* Common English language defines */

#define AAENGLISH "English"

#define GSVIEW_COPYRIGHT1 "Copyright (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "All rights reserved."
#define GSVIEW_COPYRIGHT3 "See the file LICENCE for more details."

#define GS_COPYRIGHT1 "Copyright (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "All rights reserved."
#define GS_COPYRIGHT3 "See the file PUBLIC for more details."

/* Buttons */
#ifdef UNIX
#define AAOK "Ok"
#define AACANCEL "Cancel"
#define AAEDIT "Edit"
#define AADEFAULTS "Defaults"
#define AAHELP "Help"
#define AANEW "New"
#define AATEST "Test"
#define AAYES "Yes"
#define AANO "No"
#define AANEXTGT "Next >"
#define AABACKLT "< Back"
#define AAFINISHGT "Finish >"
#define AAEXIT "Exit"
#else
#ifdef _Windows
#define AAOK "&Ok"
#define AACANCEL "&Cancel"
#define AAEDIT "&Edit"
#define AADEFAULTS "&Defaults"
#define AAHELP "&Help"
#define AANEW "&New"
#define AATEST "&Test"
#define AAYES "&Yes"
#define AANO "&No"
#define AANEXTGT "&Next >"
#define AABACKLT "< &Back"
#define AAFINISHGT "Fi&nish >"
#define AAEXIT "E&xit"
#else
#define AAOK "~Ok"
#define AACANCEL "~Cancel"
#define AADEFAULTS "~Defaults"
#define AAEDIT "~Edit"
#define AAHELP "~Help"
#define AANEW "~New"
#define AATEST "~Test"
#define AAYES "~Yes"
#define AANO "~No"
#define AANEXTGT "~Next >"
#define AABACKLT "< ~Back"
#define AAFINISHGT "Fi~nish >"
#define AAEXIT "E~xit"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "About GSview for Windows"
#define AAABOUTPM "About PM GSview"
#define AAABOUTX11 "About X11 GSview"
#define AAABOUTGSV16SPL "About GSV16SPL"
#define AAABOUTGSVIEWPRINT "About GSview Print"
#define AACOPY1 "Version:"
#define AACOPY2 "A Ghostscript graphical interface"
#define AACOPY4 "This program is distributed with NO WARRANTY OF ANY KIND."
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
#define AACOPY15 "pstotxt3.dll is Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY16 "pstotxt2.dll is Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY17 "pstotxt1.dll is Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY18 "and has a different licence.  See pstotext.txt for details."
#define AACOPY19 "Author: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "A Ghostscript DLL interface."
#define AACOPY21 "This program is part of GSview."
#define AACOPY22 "GSview Win32s/Win16 spooler"

#define AAINPUT "Input"

/* DSC error dialog box */
#define AADSC "Document Structuring Conventions"
#define AAIGNOREALLDSC "Ignore all DSC"

/* Info dialog box */
#define AAINFO  "Info"
#define AAFILEC "File:"
#define AATYPEC "Type:"
#define AATITLEC "Title:"
#define AADATEC "Date:"
#define AABOUNDINGBOXC "BoundingBox:"
#define AAORIENTATIONC "Orientation:"
#define AADEFAULTMEDIAC "Default Media:"
#define AAPAGEORDERC "Page Order:"
#define AAPAGESC "Pages:"
#define AAPAGEC "Page:"
#define AABITMAPC "Bitmap:"

/* Sounds dialog box */
#define AASOUNDS "Sounds"
#define AAEVENTC "Event:"
#define AASOUNDC "Sound:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Select Printer Port"
#define AASELECTPRINTER "Select Printer"
#define AAPRINTING "Printing"
#define AASELECTPAGE "Select Page"
#define AASELECTPAGES "Select Pages"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Printer Setup"
#define AADEVICEC "Device:"
#define AARESOLUTIONC "Resolution:"
#define AAQUEUEC "Queue:"
#define AAPRINTTOFILE "Print to File"
#define AAPSPRINTER "PostScript Printer"
#define AAOPTIONSC "Options:"
#define AAFIXEDMEDIA "Fixed media"
#define AACONVERT "Convert"
#define AAWINPRINTERSETTINGS "Windows Printer settings"
#define AACOLOURS "Colours"
#define AABANDW "Black and White"
#define AAGREY "Grey"
#define AAFULLCOLOUR "Full Colour"
#define AASELECTGSDEVICE "Select Ghostscript device"

#define AAODDEVEN "Odd/Even"
#define AAIGNOREDSC "Ignore DSC"
#define AAPAGES "Pages"
#define AAFROMC "From:"
#define AATOC "To:"

#define AAADVANCED "Advanced"
#define AAADVANCEDPSOPT "Advanced PostScript Options"
#define AASENDCTRLDBEFORE "Send CTRL+D before job"
#define AASENDCTRLDAFTER "Send CTRL+D after job"
#define AAPROLOGFILE "Prolog File"
#define AAEPILOGFILE "Epilog File"
#define AABROWSE "Browse"

/* PS2EPS dialog box */
#define AAPSTOEPS "PS to EPS"
#define AAPSTOEPSREAD "Have you read help `PS to EPS` ?"
#define AAPSTOEPSAUTO "Automatically calculate Bounding Box"

#ifdef UNIX
#define AAAPROPERTIES "Properties"
#define AAALLPAGES "All Pages"
#define AAODDPAGES "Odd Pages"
#define AAEVENPAGES "Even Pages"
#define AAALL "All"
#define AAODD "Odd"
#define AAEVEN "Even"
#define AAREVERSE "Reverse"
#else
#ifdef _Windows
#define AAAPROPERTIES "&Properties"
#define AAALLPAGES "&All Pages"
#define AAODDPAGES "O&dd Pages"
#define AAEVENPAGES "&Even Pages"
#define AAALL "&All"
#define AAODD "O&dd"
#define AAEVEN "&Even"
#define AAREVERSE "&Reverse"
#else
#define AAAPROPERTIES "~Properties"
#define AAALLPAGES "~All Pages"
#define AAODDPAGES "O~dd Pages"
#define AAEVENPAGES "~Even Pages"
#define AAALL "~All"
#define AAODD "O~dd"
#define AAEVEN "~Even"
#define AAREVERSE "~Reverse"
#endif
#endif


/* Properties dialog box */
#define AAPROPERTIES "Properties"
#define AAPROPERTYC "Property:"
#define AAVALUEC "Value:"
#define AAPAGEOFFSETPT "Page Offset (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Edit Properties"
#define AAPROPERTYTYPEC "Property Type:"
#define AANUMBER "Number"
#define AASTRING "String"
#define AANAMEC "Name: e.g.  BitsPerPixel"
#define AAVALUESC "Values:  e.g.  1,3,8,16,24"
#define AADELETE "Delete"

#define AABOUNDINGBOX "BoundingBox"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Advanced Configure"
#define AAGHOSTSCRIPTDLLC "Ghostscript DLL:"
#define AAGHOSTSCRIPTSOC "Ghostscript Shared Object:"
#define AAGHOSTSCRIPTEXEC "Ghostscript Program:"
#define AAGHOSTSCRIPTINCC "Ghostscript Include Path:"
#define AAGHOSTSCRIPTOTHERC "Ghostscript Options:"
#define AACOPYPRINTERDEF "Copy Printer Defaults"
#define AAASSOCIATEPS "Associate .ps files with GSview"
#define AAASSOCIATEPDF "Associate .pdf files with GSview"
#define AACREATESTARTMENU "Create Start Menu items"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Easy Configure"
#define AAWHICHGS "Which version of Ghostscript do you wish to use?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Download Ghostscript"
#define AADOWNLOADNOW "Download Now"
#define AAGSNOTFOUND "Ghostscript can not be found on your computer.  You will need to download and install GPL Ghostscript from"

#define AAGSMESSWIN "GSview for Windows - Ghostscript Messages"
#define AAGSMESSPM "PM GSview - Ghostscript Messages"
#define AAGSMESSX11 "X11 GSview - Ghostscript Messages"
#define AACOPY "Copy"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Display Settings"
#define AARESOLUTION "Resolution"
#define AAZOOMRESOLUTION "Zoom Resolution"
#define AADEPTH "Depth"
#define AATEXTALPHA "Text Alpha"
#define AAGRAPHICSALPHA "Graphics Alpha"
#define AADRAWMETHOD "Draw Method"
#define AADPI "dpi"
#define AABPP "bits/pixel"
#define AABITS "bits"

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
#define AACALCXFORM "Calculate Transformation"
#define AATRANSLATE "Translate"
#define AAROTATE "Rotate"
#define AASCALE "Scale"
#define AAINITMATRIX "initmatrix"
#define AAINVERTMATRIX "invertmatrix"

/* PS to Edit dialog box */
#define AAPSTOEDIT "PS to Edit"
#define AAFORMATC "Format:"
#define AADRAWTASP "Draw text as polygons"
#define AAMAPTOLATIN1 "Map to ISO-Latin1"
#define AAFLATNESSC "Flatness:"
#define AADEFAULTFONTC "Default font:"
#define AADRIVEROPTIONSC "Driver options:"

/* Registration */
#define AAREGTOC "Registered to:"
#define AANUMBERC "Number:"
#define AAGSVIEWREG "GSview Registration"
#define AAREGISTERNOW "Register Now"
#define AAONLINEREG "Online Registration"
#define AAREG1 "Please enter your name and registration number"
#define AAREG2 ""
#define AANAG1 "GSview is Copyright 2009 Ghostgum Software Pty Ltd."
#define AANAG2 "Please support the development of GSview by registering it."
#define AANAG3 ""
#define AANAG4 "GSview can be registered online at"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "    http:/\057www.ghostgum.com.au/"


/* Installation program */
#define AAINSTALL1 "GSview Install"
#define AAINSTALLINTRO1 "This wizard will help you install"
#define AAINSTALLINTRO2 "If you are not using the self extracting EXE install, make sure you have the files listed in Readme.htm in the current directory."
#define AAINSTALLINTRO3 "The installation will require 45 M bytes of hard disk space."
#define AAINSTALLCOPYRIGHT1 "Copyright Notices"
#define AAINSTALLCOPYRIGHT2 "GSview and GPL Ghostscript are separate programs."
#define AAINSTALLCOPYRIGHT3 "They are owned by separate entities."
#define AAINSTALLGSVER1 "Which components do you wish to install?"
#define AAINSTALLGSVER2 "Install GSview"
#define AAINSTALLGSVER3 "Install Ghostscript"
#define AAINSTALLGSVER4 "Which version of Ghostscript do you wish to use?"
#define AAINSTALLDIR1 "Select a directory in which to install GSview and Ghostscript."
#define AAINSTALLDIR2 "Within this directory, GSview will be in the subdirectory:"
#define AAINSTALLDIR3 "Ghostscript will be in the subdirectory:"
#define AAINSTALLMKDIR1 "The directory you specified does not exist."
#define AAINSTALLMKDIR2 "A new directory will be created." 
#define AAINSTALLMISSING1 "GSview Setup - Missing ZIP file"
#define AAINSTALLMISSING2 "Can't find ZIP file.  Insert disk with the file below."
#define AAINSTALLCONFIG1 "GSview and Ghostscript use the environment variable TEMP to specify a directory for temporary files."
#define AAINSTALLCONFIG2 "You do not have TEMP already defined."
#define AAINSTALLCONFIG3 "The following line will be added to your autoexec.bat\r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Update autoexec.bat"
#define AAINSTALLCONFIG5 "Keep backup of old autoexec.bat"
#define AAINSTALLCONFIGPM1 "GSview and Ghostscript use the environment variable TEMP to specify a directory for temporary files."
#define AAINSTALLCONFIGPM2 "You do not have TEMP already defined, or you need to install EMX."
#define AAINSTALLCONFIGPM3 "The following line will be added to your config.sys   'SET TEMP=C:\\'  and/or your PATH and LIBPATH will be updated for EMX."
#define AAINSTALLCONFIGPM4 "Update config.sys"
#define AAINSTALLCONFIGPM5 "Keep backup of old config.sys"
#define AAINSTALLFINISH "GSview Setup is now ready to start installation.  \r\rPress Finish to continue."
#define AAINSTALLUNZIP1 "Unzipping files..."
#define AADIRNOTEXIST "Directory '%s' does not exist.  Create it?"
#define AAMKDIRFAIL "Couldn't make directory"
#define AABETAWARN "This is a BETA test version of GSview.  It will disable on %04d-%02d-%02d."
#define AAINSTALLOK "Installation successful."
#define AAINSTALLFAILED "Installation failed"
#define AAPROGMANGROUP1 "GSview Setup can create a Program Manager Group or Start Menu Item."
#define AAPROGMANGROUP2 "Create Group / Item"
#define AAPROGMANGROUP3 "Name"
#define AAPROGMANGROUP4 "GS Tools"
#define AAPROGMANGROUP5 "A Program Manager group named \042%s\042 has been created."
#define AAPROGMANGROUP6 "A desktop program object named \042GSview\042 has been created."
#define AAPMOBJECT1 "GSview Setup can create a desktop object for GSview."
#define AAPMOBJECT2 "Create GSview program object."
#define AADONEBAK "The old autoexec.bat has been renamed to %s"
#define AACANTLOAD "Can't load %s"
#define AACANTCREATETEMPFILE "Can't create temporary filename"
#define AACANTOPENREAD "Can't open %s for reading"
#define AACANTOPENWRITE "Can't open %s for writing"
#define AAERRORRENAME "Error renaming %s to %s"
#define AANODDEPROGMAN "Couldn't open DDE connection to Program Manager.  Restart Windows."
#define AAINSERTDISK "Insert disk containing %s"
#define AAZIPNOTFOUND "Zip file not found"
#define AAUNZIPCANCELLED "Unzip cancelled\n"
#define AACANTALLOCBUF "Can't allocate memory for copy buffer"
#define AAPROGRAMOBJECTFAILED "Couldn't create desktop program object"
#define AAUNINSTALLTITLE "GSview and GPL Ghostscript"
#define AAUNINSTALLITEM "Uninstall GSview and GPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "This installation will require 7 M bytes of hard disk space"
#define AAGSVIEWREQUIRES "GSview also requires"
#define AAVIEWREADMEDETAILS "View the Readme file for details of how to obtain Ghostscript."
#define AACOPYRIGHTNOTICE "Copyright Notice"
#define AASELECTGSVIEWDIR "Select a directory in which to install GSview"
#define AABROWSEE "Browse..."
#define AAGSVIEWADDSTARTMENU "GSview Setup will add items to the following Start Menu folder"
#define AACREATEFOLDER "Create Folder"
#define AAALLUSERS "All Users"
#define AASELECTFOLDER "Select Folder"
#define AAFOLDER "Folder"
#define AADESTFOLDER "Destination Folder"
#define AAVIEWREADME "View Readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "GPL Ghostscript:"
#define AACFG1 "GSview Configure"
#define AACFG10 "This wizard will help you configure GSview."
#define AACFG11 "If you don't understand an item on the following pages,\
 don't change it.  The defaults are all sensible."
#define AACFG20 "GSview needs to know which version of Ghostscript to use\
 and where it is located."
#define AACFG21 "Which version of Ghostscript do you wish to use?"
#define AACFG22 "In which directory is Ghostscript located?"
#define AACFG23 "Search for additional Type 1 fonts in"
#define AACFG30 "GSview keeps a list of Ghostscript printer devices and\
 resolutions in its initialisation file."
#define AACFG31 "The default list of devices and resolutions is taken from the standard\
 distribution version of Ghostscript 5.50 and may not be complete."
#define AACFG32 "Update GSview printer list" 
#define AACFG33 "Updating the GSview printer list will overwrite any existing entries." 
#define AACFG40 "GSview can create file associations for PostScript and\
 Portable Document Format (PDF) files."
#define AACFG43 "With these associations, double clicking on a PostScript\
 or PDF file will start GSview.\
  These associations also allow some WWW browsers\
 to use GSview as a viewer for PostScript and PDF files."
#define AACFG41 "Associate PostScript (.ps and .eps) files with GSview."
#define AACFG42 "Associate PDF (.pdf) files with GSview."
#define AACFG50 "GSview can create a Program Manager Group or\
 Start Menu Item for GSview and GPL Ghostscript."
#define AACFG51 "Create Group / Item"
#define AACFG52 "Name"
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
#define AAGSVIEWPRINT "GSview Print"
#define AAGSVIEWPRINTUSAGE "Usage: %s [/d] dllpath optionfile inputfile\noptionfile and inputfile will be deleted on exit\nIt is intended that gvpgs be called with temporary files\n"
#ifdef _Windows
#define AAMFILE "&File"
#define AAMSAVEAS "Save &As..."
#define AAMEXIT "E&xit"
#define AAMEDIT "&Edit"
#define AAMCOPY "Copy\tCtrl+C"
#define AAMHELP "&Help"
#define AAMABOUT "&About..."
#else
#define AAMFILE "~File"
#define AAMSAVEAS "Save ~As..."
#define AAMEXIT "E~xit"
#define AAMEDIT "~Edit"
#define AAMCOPY "Copy\tCtrl+C"
#define AAMHELP "~Help"
#define AAMABOUT "~About..."
#endif

#define WINHELPFILE "gsviewen.chm"
