#  Copyright (C) 1993-2002, Ghostgum Software Pty Ltd.  All rights reserved.
#  
# This file is part of GSview.
#  
# This program is distributed with NO WARRANTY OF ANY KIND.  No author
# or distributor accepts any responsibility for the consequences of using it,
# or for whether it serves any particular purpose or works at all, unless he
# or she says so in writing.  Refer to the GSview Licence (the "Licence") 
# for full details.
#  
# Every copy of GSview must include a copy of the Licence, normally in a 
# plain ASCII text file named LICENCE.  The Licence grants you the right 
# to copy, modify and redistribute GSview, but only under certain conditions 
# described in the Licence.  Among other things, the Licence requires that 
# the copyright notice and this notice be preserved on all copies.

# Common makefile for GSview

COMP=echo cc -flags

OBJDIR=.\obj
SRCWINDIR=.\srcwin
SRCOS2DIR=.\srcos2
SRCGTKDIR=.\srcgtk
SRCDIR=.\src

NUL=
SRCWIN=$(SRCWINDIR)$(DD)$(NUL)
SRCOS2=$(SRCOS2DIR)$(DD)$(NUL)
SRCGTK=$(SRCGTKDIR)$(DD)$(NUL)
OBJ=$(OBJDIR)$(DD)$(NUL)


FO=-Fo$(OBJ)


# Windows files

$(OBJ)gvwin.obj: $(SRCWIN)gvwin.c $(HDRS)
	$(COMP) $(FO)gvwin.obj $(SRCWIN)gvwin.c

gvwcp.obj: gvwclip.c $(HDRS)

gvwclip.obj: gvwclip.c $(HDRS)

gvwdisp.obj: gvwdisp.c $(HDRS)

gvwdlg.obj: gvwdlg.c $(HDRS)

gvwdll.obj: gvwdll.c $(HDRS)

gvwedit.obj: gvwedit.c $(HDRS)

gvweps.obj: gvweps.c gvceps.h $(HDRS)

gvwgsver.obj: gvwgsver.c $(HDRS)

gvwimg.obj: gvwimg.c $(HDRS)

gvwinit.obj: gvwinit.c $(HDRS)

gvwmeas.obj: gvwmeas.c $(HDRS)

gvwmisc.obj: gvwmisc.c $(HDRS)

gvwprn.obj: gvwprn.c $(HDRS)

gvccmd.obj: gvccmd.c $(HDRS)

gvcdisp.obj: gvcdisp.c $(HDRS)

gvcdll.obj: gvcdll.c $(HDRS)

cdll.obj: cdll.c $(HDRS)

cimg.obj: cimg.c $(HDRS)

cview.obj: cview.c $(HDRS)

dscparse.obj: dscparse.c dscparse.h

dscutil.obj: dscutil.c dscparse.h

gvcbeta.obj: gvcbeta.c gvcbeta.h $(HDRS)

gvcedit.obj: gvcedit.c $(HDRS)

gvceps.obj: gvceps.c gvceps.h $(HDRS)

gvcinit.obj: gvcinit.c $(HDRS)

gvcmeas.obj: gvcmeas.c $(HDRS)

gvcmeas2.obj: gvcmeas2.c $(HDRS)

gvcmfile.obj: gvcmfile.c gvcfile.h

gvcmisc.obj: gvcmisc.c $(HDRS)

gvcpdf.obj: gvcpdf.c $(HDRS)

gvcprn.obj: gvcprn.c $(HDRS)

gvctext.obj: gvctext.c $(HDRS)

gvcreg.obj: gvcreg.c $(HDRS)

gvwreg.obj: gvwreg.c $(HDRS)

gvwfile.obj: gvwfile.c gvcfile.h

gvwdib.obj: gvwdib.cpp $(HDRS)

gvwpdib.obj: gvwpdib.cpp $(HDRS)

gvwpgdi.obj: gvwpgdi.cpp $(HDRS)

viewonly.obj: viewonly.c $(HDRS)

winunzip.obj: winunzip.c wizdll.h
	$(CC) -c $(CFLAGS) $*.c

dwuninst.res: dwuninst.rc dwuninst.h
	$(RCOMP) -i"$(INCDIR)" -r $*.rc

winsetup.res: winsetup.rc setup.h $(LANGUAGE)\gvclang.h
	copy $(LANGUAGE)\gvclang.h  gvclang.h
	$(RCOMP) -i"$(INCDIR)" -r $*.rc

setupc.obj: setupc.c setup.h setupc.h gvcrc.h gvcbeta.h
	$(CC) -c $(CFLAGS) setupc.c

gvcbetaa.obj: gvcbeta.c gvcbeta.h $(HDRS)
	copy gvcbeta.c gvcbetaa.c
	$(CC) -c $(CFLAGS) gvcbetaa.c
	-del gvcbetaa.c

winsetup.obj: winsetup.cpp setup.h gvcrc.h gvcbeta.h dwinst.h $(LANGUAGE)\gvclang.h 
	copy $(LANGUAGE)\gvclang.h gvclang.h
	$(CC) -c $(CFLAGS) winsetup.cpp

dwinst.obj: dwinst.cpp dwinst.h
	$(CC) -c $(CFLAGS) dwinst.cpp

dwuninst.obj: dwuninst.cpp dwuninst.h
	$(CC) -c $(CFLAGS) dwuninst.cpp


setp$(WINEXT)de.res: winsetup.rc de\gvclang.h gvcver.h gvcrc.h setup.h
	copy de\gvclang.h gvclang.h
	$(RCOMP) -i"$(INCDIR)" -r -fosetp$(WINEXT)de.res winsetup
	-del gvclang.h

setp$(WINEXT)es.res: winsetup.rc es\gvclang.h gvcver.h gvcrc.h setup.h
	copy es\gvclang.h gvclang.h
	$(RCOMP) -i"$(INCDIR)" -r -fosetp$(WINEXT)es.res winsetup
	-del gvclang.h

setp$(WINEXT)fr.res: winsetup.rc fr\gvclang.h gvcver.h gvcrc.h setup.h
	copy fr\gvclang.h gvclang.h
	$(RCOMP) -i"$(INCDIR)" -r -fosetp$(WINEXT)fr.res winsetup
	-del gvclang.h

setp$(WINEXT)gr.res: winsetup.rc gr\gvclang.h gvcver.h gvcrc.h setup.h
	copy gr\gvclang.h gvclang.h
	$(RCOMP) -i"$(INCDIR)" -r -fosetp$(WINEXT)gr.res -c1253 winsetup
	-del gvclang.h

setp$(WINEXT)it.res: winsetup.rc it\gvclang.h gvcver.h gvcrc.h setup.h
	copy it\gvclang.h gvclang.h
	$(RCOMP) -i"$(INCDIR)" -r -fosetp$(WINEXT)it.res winsetup
	-del gvclang.h

setp$(WINEXT)nl.res: winsetup.rc nl\gvclang.h gvcver.h gvcrc.h setup.h
	copy nl\gvclang.h gvclang.h
	$(RCOMP) -i"$(INCDIR)" -r -fosetp$(WINEXT)nl.res winsetup
	-del gvclang.h

setp$(WINEXT)se.res: winsetup.rc se\gvclang.h gvcver.h gvcrc.h setup.h
	copy se\gvclang.h gvclang.h
	$(RCOMP) -i"$(INCDIR)" -r -fosetp$(WINEXT)se.res winsetup
	-del gvclang.h

gvdoc.exe: gvdoc.c
	$(CCAUX) -I"$(INCDIR)" $(CLFLAG) gvdoc.c

gsview.txt: $(LANGUAGE)\gvclang.txt gvdoc.exe
	gvdoc W $(LANGUAGE)\gvclang.txt gsview.txt

doc2rtf.exe: doc2rtf.c
	$(CCAUX) -I"$(INCDIR)" $(CLFLAG) doc2rtf.c

doc2html.exe: doc2html.c
	$(CCAUX) -I"$(INCDIR)" $(CLFLAG) doc2html.c

doc2xml.exe: doc2xml.c
	$(CCAUX) -I"$(INCDIR)" $(CLFLAG) doc2xml.c

docutil.exe: docutil.c
	$(CCAUX) -I"$(INCDIR)" $(CLFLAG) docutil.c

codepage.exe: codepage.c
	$(CCAUX) -I"$(INCDIR)" $(CLFLAG) codepage.c

gsview.dvi: gsview.tex titlepag.tex

gsview.dvi: gsview.tex titlepag.tex
	-latex gsview
	-latex gsview

gsview.tex: gsview.txt doc2tex.exe
	doc2tex gsview.txt gsview.tex

doc2tex.exe: doc2tex.c
	$(CCAUX) -I"$(INCDIR)" $(CLFLAG) doc2tex.c

gsviewen.hlp: gvdoc.exe doc2rtf.exe en\gvclang.txt en\gsview.hpj
	copy en\gsview.hpj gsviewen.hpj
	gvdoc W en\gvclang.txt gsview.txt
	doc2rtf gsview.txt gsview.rtf
	$(HC) gsviewen.hpj
	rename gsviewen.hlp gsviewen.hlp
	-del gsviewen.hpj
	-del gsview.txt
	-del gsview.rtf

gsviewde.hlp: gvdoc.exe doc2rtf.exe de\gvclang.txt de\gsview.hpj
	copy de\gsview.hpj gsviewde.hpj
	gvdoc W de\gvclang.txt gsview.txt
	doc2rtf gsview.txt gsview.rtf
	$(HC) gsviewde.hpj
	rename gsviewde.hlp gsviewde.hlp
	-del gsviewde.hpj
	-del gsview.txt
	-del gsview.rtf

gsviewes.hlp: gvdoc.exe doc2rtf.exe es\gvclang.txt es\gsview.hpj
	copy es\gsview.hpj gsviewes.hpj
	gvdoc W es\gvclang.txt gsview.txt
	doc2rtf gsview.txt gsview.rtf
	$(HC) gsviewes.hpj
	rename gsviewes.hlp gsviewes.hlp
	-del gsviewes.hpj
	-del gsview.txt
	-del gsview.rtf

gsviewfr.hlp: gvdoc.exe doc2rtf.exe fr\gvclang.txt fr\gsview.hpj
	copy fr\gsview.hpj gsviewfr.hpj
	gvdoc W fr\gvclang.txt gsview.txt
	doc2rtf gsview.txt gsview.rtf
	$(HC) gsviewfr.hpj
	rename gsviewfr.hlp gsviewfr.hlp
	-del gsviewfr.hpj
	-del gsview.txt
	-del gsview.rtf

gsviewgr.hlp: gvdoc.exe doc2rtf.exe gr\gvclang.txt gr\gsview.hpj
	copy gr\gsview.hpj gsviewgr.hpj
	gvdoc W gr\gvclang.txt gsview.txt
	doc2rtf gsview.txt gsview.rtf
	$(HC) gsviewgr.hpj
	rename gsviewgr.hlp gsviewgr.hlp
	-del gsviewgr.hpj
	-del gsview.txt
	-del gsview.rtf

gsviewit.hlp: gvdoc.exe doc2rtf.exe it\gvclang.txt it\gsview.hpj
	copy it\gsview.hpj gsviewit.hpj
	gvdoc W it\gvclang.txt gsview.txt
	doc2rtf gsview.txt gsview.rtf
	$(HC) gsviewit.hpj
	rename gsviewit.hlp gsviewit.hlp
	-del gsviewit.hpj
	-del gsview.txt
	-del gsview.rtf

gsviewnl.hlp: gvdoc.exe doc2rtf.exe nl\gvclang.txt nl\gsview.hpj
	copy nl\gsview.hpj gsviewnl.hpj
	gvdoc W nl\gvclang.txt gsview.txt
	doc2rtf gsview.txt gsview.rtf
	$(HC) gsviewnl.hpj
	rename gsviewnl.hlp gsviewnl.hlp
	-del gsviewnl.hpj
	-del gsview.txt
	-del gsview.rtf

gsviewse.hlp: gvdoc.exe doc2rtf.exe se\gvclang.txt se\gsview.hpj
	copy se\gsview.hpj gsviewse.hpj
	gvdoc W se\gvclang.txt gsview.txt
	doc2rtf gsview.txt gsview.rtf
	$(HC) gsviewse.hpj
	rename gsviewse.hlp gsviewse.hlp
	-del gsviewse.hpj
	-del gsview.txt
	-del gsview.rtf

gsview.htm: doc2html.exe gsview.txt
	doc2html gsview.txt gsview.htm

html: doc2html.exe codepage.exe en\gvclang.txt de\gvclang.txt es\gvclang.txt fr\gvclang.txt gr\gvclang.txt it\gvclang.txt nl\gvclang.txt se\gvclang.txt
	gvdoc W en\gvclang.txt gsview.txt
	doc2html gsview.txt gsviewen.htm
	-del gsview.txt
	gvdoc W de\gvclang.txt gsview.txt
	doc2html gsview.txt gsviewde.htm
	-del gsview.txt
	gvdoc W es\gvclang.txt gsview.txt
	doc2html gsview.txt gsviewes.htm
	-del gsview.txt
	gvdoc W fr\gvclang.txt gsview.txt
	doc2html gsview.txt gsviewfr.htm
	-del gsview.txt
	gvdoc W gr\gvclang.txt temp.txt
	codepage 1253_8859-7 temp.txt gsview.txt
	-del temp.txt
	doc2html gsview.txt gsviewgr.htm ISO-8859-7
	-del gsview.txt
	gvdoc W it\gvclang.txt gsview.txt
	doc2html gsview.txt gsviewit.htm
	-del gsview.txt
	gvdoc W nl\gvclang.txt gsview.txt
	doc2html gsview.txt gsviewnl.htm
	-del gsview.txt
	gvdoc W se\gvclang.txt gsview.txt
	doc2html gsview.txt gsviewse.htm
	-del gsview.txt
	gvdoc P en\gvclang.txt gsview.txt
	doc2html gsview.txt os2help.htm
	-del gsview.txt


xml: doc2xml.exe docutil.exe
	doc2xml en\gvclang.txt temp.xml
	xt temp.xml xmlwin.xsl test.xml
	xt test.xml xml2htm.xsl test.htm
	xt test.xml xml2ipf.xsl test.ipf
	xt test.xml xml2rtf.xsl test.rtf
	xt test.xml xml2htmh.xsl testh.htm
	-mkdir html
	docutil --post-htmh testh.htm 
	xt test.xml xml2hhc.xsl temp.hhc
	docutil --post-hhc temp.hhc test.hhc
	xt test.xml xml2hhk.xsl temp.hhk
	docutil --post-hhk temp.hhk test.hhk

gvwgs$(WINEXT).res: gvwgs.rc gvwgs.h $(ICONS) $(LANGUAGE)\gvclang.h 
	copy $(LANGUAGE)\gvclang.h gvclang.h
	$(RCOMP) -i"$(INCDIR)" -r -fogvwgs$(WINEXT).res gvwgs


src: gsv$(GSVIEW_VERSION)src.zip

gsv$(GSVIEW_VERSION)src.zip:
	copy Readme.htm ..
	copy gsview.css ..
	copy LICENCE ..
	copy FILE_ID.DIZ ..
	cd ..
	-del pstotext.zip
	zip -9 -@ pstotext.zip     < src\gvclistp.txt
	-del src.zip
	copy src\gvclists.txt gvclists.txt
	zip -9 -@ src.zip     < gvclists.txt
	-del gvclists.txt
	-del gsv$(GSVIEW_VERSION)src.zip
	zip -9 gsv$(GSVIEW_VERSION)src.zip pstotext.zip src.zip Readme.htm gsview.css FILE_ID.DIZ LICENCE
	-del Readme.htm
	-del gsview.css
	-del LICENCE
	-del FILE_ID.DIZ
	cd src
	
prezip: gsv$(GSVIEW_VERSION)w$(WINEXT).exe

zip: prezip gsv$(GSVIEW_VERSION)src.zip
	copy Readme.htm ..\Readme.htm
	copy gsview.css ..\gsview.css
	copy LICENCE ..\LICENCE
	copy FILE_ID.DIZ ..\FILE_ID.DIZ
	cd ..
	-del gsview$(GSVIEW_VERSION).zip
#	rename gsv$(GSVIEW_VERSION)w16.zip gsv$(GSVIEW_VERSION)w16.zip 
	rename gsv$(GSVIEW_VERSION)os2.zip gsv$(GSVIEW_VERSION)os2.zip 
	zip -9 gsview$(GSVIEW_VERSION) gsv$(GSVIEW_VERSION)src.zip gsv$(GSVIEW_VERSION)os2.zip
# gsv$(GSVIEW_VERSION)w16.zip 
	zip -9 gsview$(GSVIEW_VERSION) gsv$(GSVIEW_VERSION)w32.exe
# gsv$(GSVIEW_VERSION)wda.zip
	zip -9 gsview$(GSVIEW_VERSION) Readme.htm gsview.css FILE_ID.DIZ LICENCE
	-del Readme.htm
	-del gsview.css
	-del LICENCE
	-del FILE_ID.DIZ
	cd src

# Create DEC Alpha zip file
# Since the author can't build this, it isn't in the main distribution.
gsv$(GSVIEW_VERSION)wda.zip:
#	Create list of files to archive
 	copy gvclista.txt filesda.txt
	echo Readme.htm >> filesda.txt
	echo FILE_ID.DIZ >> filesda.txt
	echo filelist.txt >> filesda.txt
	echo setup.exe >> filesda.txt
	echo setpdade.dll >> filesda.txt
	echo setpdaes.dll >> filesda.txt
	echo setpdafr.dll >> filesda.txt
	echo setpdagr.dll >> filesda.txt
	echo setpdait.dll >> filesda.txt
	echo setpdanl.dll >> filesda.txt
	echo setpdase.dll >> filesda.txt
#	Copy the files into place
	-mkdir ..\gsview
	copy refresh.htm ..\Readme.htm
	copy FILE_ID.DIZ ..\FILE_ID.DIZ
	copy LICENCE ..\gsview\LICENCE
	copy Readme.htm ..\gsview\Readme.htm
	copy regorder.txt ..\gsview\regorder.txt
	copy gsviewda.exe ..\gsview\gsviewda.exe
	copy NUL ..\gsview\gsview32.ini
	copy binary\gvwin1.ico ..\gsview32.ico
	copy gsviewen.hlp ..\gsview\gsviewen.hlp
	copy gsviewde.hlp ..\gsview\gsviewde.hlp
	copy gsviewes.hlp ..\gsview\gsviewes.hlp
	copy gsviewfr.hlp ..\gsview\gsviewfr.hlp
	copy gsviewgr.hlp ..\gsview\gsviewgr.hlp
	copy gsviewit.hlp ..\gsview\gsviewit.hlp
	copy gsviewnl.hlp ..\gsview\gsviewnl.hlp
	copy gsviewse.hlp ..\gsview\gsviewse.hlp
	copy gsvwdade.dll ..\gsview\gsvwdade.dll
	copy gsvwdaes.dll ..\gsview\gsvwdaes.dll
	copy gsvwdafr.dll ..\gsview\gsvwdafr.dll
	copy gsvwdagr.dll ..\gsview\gsvwdagr.dll
	copy gsvwdait.dll ..\gsview\gsvwdait.dll
	copy gsvwdanl.dll ..\gsview\gsvwdanl.dll
	copy gsvwdase.dll ..\gsview\gsvwdase.dll
	copy gvwgsda.exe ..\gsview\gvwgsda.exe
	copy printer.ini ..\gsview\printer.ini
	copy uninstgs.exe ..\gsview\uninstgs.exe
	copy winsetup.exe ..\setup.exe
	copy setpdade.dll ..\setpdade.dll
	copy setpdaes.dll ..\setpdaes.dll
	copy setpdafr.dll ..\setpdafr.dll
	copy setpdagr.dll ..\setpdagr.dll
	copy setpdait.dll ..\setpdait.dll
	copy setpdanl.dll ..\setpdanl.dll
	copy setpdase.dll ..\setpdase.dll
# assume zlib32.dll is in ..\gsview\zlib32.dll
# assume libbz2.dll is in ..\gsview\libbz2.dll
	echo GSview $(GSVIEW_DOT_VERSION)> filel_da.tmp
	echo gsview>> filel_da.tmp
	copy filel_da.tmp+gvclista.txt ..\filelist.txt
	del filel_da.tmp
#	Create the DEC Alpha zip file
	cd ..
	-del gsv$(GSVIEW_VERSION)wda.zip 
	zip -9 -@ gsv$(GSVIEW_VERSION)wda.zip < src\filesda.txt
	cd src
	-del filesda.txt

# create self extracting archive
test:	all distcopy

distcopy:
	-mkdir ..\gsview
	copy refresh.htm ..\Readme.htm
	copy FILE_ID.DIZ ..\FILE_ID.DIZ
	copy LICENCE ..\gsview\LICENCE
	copy Readme.htm ..\gsview\Readme.htm
	copy gsview.css ..\gsview\gsview.css
	copy regorder.txt ..\gsview\regorder.txt
	copy gsview32.exe ..\gsview\gsview32.exe
	copy binary\gvwin1.ico ..\gsview32.ico
	copy gsviewen.hlp ..\gsview\gsviewen.hlp
	copy gsviewde.hlp ..\gsview\gsviewde.hlp
	copy gsviewes.hlp ..\gsview\gsviewes.hlp
	copy gsviewfr.hlp ..\gsview\gsviewfr.hlp
	copy gsviewgr.hlp ..\gsview\gsviewgr.hlp
	copy gsviewit.hlp ..\gsview\gsviewit.hlp
	copy gsviewnl.hlp ..\gsview\gsviewnl.hlp
	copy gsviewse.hlp ..\gsview\gsviewse.hlp
	copy gsvw32de.dll ..\gsview\gsvw32de.dll
	copy gsvw32es.dll ..\gsview\gsvw32es.dll
	copy gsvw32fr.dll ..\gsview\gsvw32fr.dll
	copy gsvw32gr.dll ..\gsview\gsvw32gr.dll
	copy gsvw32it.dll ..\gsview\gsvw32it.dll
	copy gsvw32nl.dll ..\gsview\gsvw32nl.dll
	copy gsvw32se.dll ..\gsview\gsvw32se.dll
	copy gvwgs32.exe ..\gsview\gvwgs32.exe
	copy printer.ini ..\gsview\printer.ini
	copy NUL > ..\gsview\gsview32.ini
	copy uninstgs.exe ..\gsview\uninstgs.exe
	copy winsetup.exe ..\setup.exe
	copy setp32de.dll ..\setp32de.dll
	copy setp32es.dll ..\setp32es.dll
	copy setp32fr.dll ..\setp32fr.dll
	copy setp32gr.dll ..\setp32gr.dll
	copy setp32it.dll ..\setp32it.dll
	copy setp32nl.dll ..\setp32nl.dll
	copy setp32se.dll ..\setp32se.dll
# assume zlib32.dll is in ..\gsview\zlib32.dll
# assume libbz2.dll is in ..\gsview\libbz2.dll
	copy ..\pstoedit\pstoedit.htm ..\gsview\pstoedit.htm
	copy gsprint.htm ..\gsview\gsprint.htm
	copy gsprint.exe ..\gsview\gsprint.exe
	copy ..\epstool\epstool.htm ..\gsview\epstool.htm
	copy gsv16spl.exe ..\gsview\gsv16spl.exe
	echo GSview $(GSVIEW_DOT_VERSION)> filelist.tmp
	echo gsview>> filelist.tmp
	copy filelist.tmp+gvclist3.txt ..\filelist.txt
	del filelist.tmp

files32.txt:
 	copy gvclist3.txt files32.txt
	echo Readme.htm >> files32.txt
	echo FILE_ID.DIZ >> files32.txt
	echo filelist.txt >> files32.txt
	echo setup.exe >> files32.txt
	echo setp32de.dll >> files32.txt
	echo setp32es.dll >> files32.txt
	echo setp32fr.dll >> files32.txt
	echo setp32gr.dll >> files32.txt
	echo setp32it.dll >> files32.txt
	echo setp32nl.dll >> files32.txt
	echo setp32se.dll >> files32.txt

gsv$(GSVIEW_VERSION)w32.zip: strip distcopy files32.txt
	cd ..
	-del gsv$(GSVIEW_VERSION)w32.zip 
	zip -9 -@ gsv$(GSVIEW_VERSION)w32.zip < src\files32.txt
	cd src



# Now convert to a self extracting archive.
# This involves making a few temporary files.
gsv$(GSVIEW_VERSION)w32.exe: strip distcopy gsv$(GSVIEW_VERSION)w32.zip
	cd ..
	echo -win32 -setup > setup.rsp
	echo -st "GSview $(GSVIEW_DOT_VERSION) for Win32" >> setup.rsp
	echo -i gsview32.ico >> setup.rsp
	echo -a about.txt >> setup.rsp
	echo -t dialog.txt >> setup.rsp
	echo -c .\setup.exe >> setup.rsp
	echo GSview is Copyright (C) 2007 Ghostgum Software Pty Ltd. > about.txt
	echo See licence in gsview\LICENCE >> about.txt
	echo This installs GSview $(GSVIEW_DOT_VERSION) for Win32. > dialog.txt
	echo GSview uses GPL Ghostscript to display, print and convert PostScript and PDF files. >> dialog.txt
	$(WINZIPSE_XE) gsv$(GSVIEW_VERSION)w32 @setup.rsp
# Don't delete temporary files, because make continues
# before these files are used.
#	-del setup.rsp 
#	-del about.txt
#	-del dialog.txt
#	-del gsview32.ico
	cd src

language:
	-del *.res
	-del gvclang.h
	-del gvclang.rc
	-del gvwlang.rc

clean: language
 	-del cdll.obj
 	-del cimg.obj
 	-del cview.obj
	-del gsvver.h
	-del gvwin.obj
	-del gvwclip.obj
	-del gvwcp.obj
	-del gvwdib.obj
	-del gvwdisp.obj
	-del gvwdlg.obj
	-del gvwdll.obj
	-del gvwedit.obj
	-del gvweps.obj
	-del gvwfile.obj
	-del gvwimg.obj
	-del gvwinit.obj
	-del gvwdde.obj
	-del gvwdde2.obj
	-del gvwgsver.obj
	-del gvwmeas.obj
	-del gvwmisc.obj
	-del gvwpdib.obj
	-del gvwpgdi.obj
	-del gvwprf.obj
	-del gvwprn.obj
	-del gvwreg.obj
	-del gvcbeta.obj
	-del gvccmd.obj
	-del dscparse.obj
	-del dscutil.obj
	-del gvcdisp.obj
	-del gvcedit.obj
	-del gvceps.obj
	-del gvcinit.obj
	-del gvcmeas.obj
	-del gvcmeas2.obj
	-del gvcmisc.obj
	-del gvcdll.obj
	-del gvcpdf.obj
	-del gvcprn.obj
	-del gvcprf.obj
	-del gvcreg.obj
	-del gvctext.obj
	-del gsview16.map
	-del gsview16.sym
	-del gsview32.map
	-del gsview32.sym
	-del gsview32.ilc
	-del gsview32.ild
	-del gsview32.ilf
	-del gsview32.ils
	-del gsview32.ilk
	-del gsview32.lib
	-del gsview32.exp
	-del gsview32.pdb
	-del gsview32.tds
	-del gsvw32de.map
	-del gsvw32de.ilc
	-del gsvw32de.ild
	-del gsvw32de.ilf
	-del gsvw32de.ils
	-del gsvw32de.ilk
	-del gsvw32de.lib
	-del gsvw32de.exp
	-del gsvw32de.pdb
	-del gsvw32de.tds
	-del gsvw32en.map
	-del gsvw32en.ilc
	-del gsvw32en.ild
	-del gsvw32en.ilf
	-del gsvw32en.ils
	-del gsvw32en.ilk
	-del gsvw32en.lib
	-del gsvw32en.exp
	-del gsvw32en.pdb
	-del gsvw32en.tds
	-del gsvw32es.map
	-del gsvw32es.ilc
	-del gsvw32es.ild
	-del gsvw32es.ilf
	-del gsvw32es.ils
	-del gsvw32es.ilk
	-del gsvw32es.lib
	-del gsvw32es.exp
	-del gsvw32es.pdb
	-del gsvw32es.tds
	-del gsvw32fr.map
	-del gsvw32fr.ilc
	-del gsvw32fr.ild
	-del gsvw32fr.ilf
	-del gsvw32fr.ils
	-del gsvw32fr.ilk
	-del gsvw32fr.lib
	-del gsvw32fr.exp
	-del gsvw32fr.pdb
	-del gsvw32fr.tds
	-del gsvw32gr.map
	-del gsvw32gr.ilc
	-del gsvw32gr.ild
	-del gsvw32gr.ilf
	-del gsvw32gr.ils
	-del gsvw32gr.ilk
	-del gsvw32gr.lib
	-del gsvw32gr.exp
	-del gsvw32gr.pdb
	-del gsvw32gr.tds
	-del gsvw32it.map
	-del gsvw32it.ilc
	-del gsvw32it.ild
	-del gsvw32it.ilf
	-del gsvw32it.ils
	-del gsvw32it.ilk
	-del gsvw32it.lib
	-del gsvw32it.exp
	-del gsvw32it.pdb
	-del gsvw32it.tds
	-del gsvw32nl.map
	-del gsvw32nl.ilc
	-del gsvw32nl.ild
	-del gsvw32nl.ilf
	-del gsvw32nl.ils
	-del gsvw32nl.ilk
	-del gsvw32nl.lib
	-del gsvw32nl.exp
	-del gsvw32nl.pdb
	-del gsvw32nl.tds
	-del gsvw32se.map
	-del gsvw32se.ilc
	-del gsvw32se.ild
	-del gsvw32se.ilf
	-del gsvw32se.ils
	-del gsvw32se.ilk
	-del gsvw32se.lib
	-del gsvw32se.exp
	-del gsvw32se.pdb
	-del gsvw32se.tds
	-del gsvw16de.map
	-del gsvw16en.map
	-del gsvw16es.map
	-del gsvw16fr.map
	-del gsvw16gr.map
	-del gsvw16it.map
	-del gsvw16nl.map
	-del gsvw16se.map
	-del gvwlang.obj
	-del gvwin16.res
	-del gvwin32.res
	-del gsviewen.txt
	-del gsviewde.txt
	-del gsviewes.txt
	-del gsviewfr.txt
	-del gsviewgr.txt
	-del gsviewit.txt
	-del gsviewnl.txt
	-del gsviewse.txt
	-del gsview.rtf
	-del doc2html.obj
	-del doc2html.exe
	-del doc2rtf.obj
	-del doc2rtf.exe
	-del doc2rtf.tds
	-del doc2tex.obj
	-del doc2tex.exe
	-del doc2xml.obj
	-del doc2xml.exe
	-del docutil.obj
	-del docutil.exe
	-del codepage.obj
	-del codepage.exe
	-del gvdoc.exe
	-del gvdoc.obj
	-del gvdoc.tds
	-del gsview.txt
	-del gsview.aux
	-del gsview.dvi
	-del gsview.log
	-del gsview.toc
	-del gsview.tex
	-del gsv16spl.obj
	-del gsv16spl.res
	-del gsv16spl.map
	-del dwinst.obj
	-del dwuninst.obj
	-del winsetup.obj
	-del winsetup.res
	-del winsetup.map
	-del winsetup.ilc
	-del winsetup.ild
	-del winsetup.ilf
	-del winsetup.ils
	-del winsetup.ilk
	-del winsetup.lib
	-del winsetup.exp
	-del winsetup.pdb
	-del winsetup.tds
	-del uninstgs.obj
	-del uninstgs.res
	-del uninstgs.map
	-del uninstgs.ilc
	-del uninstgs.ild
	-del uninstgs.ilf
	-del uninstgs.ils
	-del uninstgs.ilk
	-del uninstgs.lib
	-del uninstgs.exp
	-del uninstgs.pdb
	-del uninstgs.tds
	-del setp32de.res
	-del setp32de.map
	-del setp32de.ilc
	-del setp32de.ild
	-del setp32de.ilf
	-del setp32de.ils
	-del setp32de.ilk
	-del setp32de.lib
	-del setp32de.exp
	-del setp32de.pdb
	-del setp32de.tds
	-del setp32es.res
	-del setp32es.map
	-del setp32es.ilc
	-del setp32es.ild
	-del setp32es.ilf
	-del setp32es.ils
	-del setp32es.ilk
	-del setp32es.lib
	-del setp32es.exp
	-del setp32es.pdb
	-del setp32es.tds
	-del setp32fr.res
	-del setp32fr.map
	-del setp32fr.ilc
	-del setp32fr.ild
	-del setp32fr.ilf
	-del setp32fr.ils
	-del setp32fr.ilk
	-del setp32fr.lib
	-del setp32fr.exp
	-del setp32fr.pdb
	-del setp32fr.tds
	-del setp32gr.res
	-del setp32gr.map
	-del setp32gr.ilc
	-del setp32gr.ild
	-del setp32gr.ilf
	-del setp32gr.ils
	-del setp32gr.ilk
	-del setp32gr.lib
	-del setp32gr.exp
	-del setp32gr.pdb
	-del setp32gr.tds
	-del setp32it.res
	-del setp32it.map
	-del setp32it.ilc
	-del setp32it.ild
	-del setp32it.ilf
	-del setp32it.ils
	-del setp32it.ilk
	-del setp32it.lib
	-del setp32it.exp
	-del setp32it.pdb
	-del setp32it.tds
	-del setp32nl.res
	-del setp32nl.map
	-del setp32nl.ilc
	-del setp32nl.ild
	-del setp32nl.ilf
	-del setp32nl.ils
	-del setp32nl.ilk
	-del setp32nl.lib
	-del setp32nl.exp
	-del setp32nl.pdb
	-del setp32nl.tds
	-del setp32se.res
	-del setp32se.map
	-del setp32se.ilc
	-del setp32se.ild
	-del setp32se.ilf
	-del setp32se.ils
	-del setp32se.ilk
	-del setp32se.lib
	-del setp32se.exp
	-del setp32se.pdb
	-del setp32se.tds
	-del setupc.obj
	-del winunzip.obj
	-del gvcbetaa.obj
	-del gvwgs.obj
	-del gvwgs16.res
	-del gvwgs16.map
	-del gvwgs32.res
	-del gvwgs32.map
	-del gvwgs32.ilc
	-del gvwgs32.ild
	-del gvwgs32.ilf
	-del gvwgs32.ils
	-del gvwgs32.ilk
	-del gvwgs32.lib
	-del gvwgs32.exp
	-del gvwgs32.pdb
	-del gvwgs32.tds
	-del lib.rsp
	-del link.rsp
	-del vc60.pdb
	-del vc50.pdb
	-del gsprint.ilk
	-del gsprint.obj
	-del gsprint.pdb
	-del echogsv.exe
	-del echogsv.ilk
	-del echogsv.obj
	-del echogsv.pdb

veryclean: clean
	-del gsview$(WINEXT).exe
	-del gsviewen.hlp
	-del gsviewde.hlp
	-del gsviewes.hlp
	-del gsviewfr.hlp
	-del gsviewgr.hlp
	-del gsviewit.hlp
	-del gsviewnl.hlp
	-del gsviewse.hlp
	-del gsvw$(WINEXT)de.dll
	-del gsvw$(WINEXT)es.dll
	-del gsvw$(WINEXT)fr.dll
	-del gsvw$(WINEXT)gr.dll
	-del gsvw$(WINEXT)it.dll
	-del gsvw$(WINEXT)nl.dll
	-del gsvw$(WINEXT)se.dll
	-del gsview.htm
# This doesn't get made by MSVC++
#	-del gsv16spl.exe
	-del gvwgs$(WINEXT).exe
	-del winsetup.exe
	-del setp$(WINEXT)de.dll
	-del setp$(WINEXT)es.dll
	-del setp$(WINEXT)fr.dll
	-del setp$(WINEXT)gr.dll
	-del setp$(WINEXT)it.dll
	-del setp$(WINEXT)nl.dll
	-del setp$(WINEXT)se.dll
	-del uninstgs.exe
	-del files32.txt
	-del gsviewen.htm
	-del gsviewde.htm
	-del gsviewes.htm
	-del gsviewfr.htm
	-del gsviewgr.htm
	-del gsviewit.htm
	-del gsviewnl.htm
	-del gsviewse.htm
	-del os2help.htm
	-del gsprint.exe
