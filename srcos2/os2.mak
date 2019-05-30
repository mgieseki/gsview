#  Copyright (C) 1993-2007, Ghostgum Software Pty Ltd.  All rights reserved.
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

# OS/2 makefile for GSview

#################################################################
# OS/2

# DEBUG=1 for debugging
DEBUG=0

# EMX
DRIVE=c:
CC=gcc
COMPBASE=$(DRIVE)\emx
EMXPATH=$(DRIVE)/emx
OBJ=.obj
!if $(DEBUG)
CDEBUG=-g
LDEBUG=/DEBUG
!endif
CFLAGS=-O -Wall -Zomf -Zmts $(CDEBUG) -DOS2 -DNONAG


COMPDIR=$(COMPBASE)\bin
INCDIR=$(EMXPATH)/include
LIBDIR=$(EMXPATH)/lib


BINDIR=.\bin
OBJDIR=.\obj
SRCDIR=.\src
SRCOS2DIR=.\srcos2


COMP=$(CC) $(CFLAGS) -I$(SRCDIR) -I$(SRCOS2DIR) -I$(OBJDIR)
CCAUX=$(CC) $(CFLAGS)

NUL=
SRC=$(SRCDIR)\$(NUL)
SRCOS2=$(SRCOS2DIR)\$(NUL)
OD=$(OBJDIR)\$(NUL)
BD=$(BINDIR)\$(NUL)
OBJ=.obj
EXE=.exe
CO=-c

FE=-o
FO=-o
FEO=-o$(OD)
FOO=-o$(OD)

HDRSPLAT=$(SRCOS2)gvpm.h


CP=copy
RM=del

# if you have a strict ANSI compiler, add -D__STDC__
EPSDEF=-I$(SRC) -DOS2 -DEPSTOOL -DSTDIO
EPSLIBS=
EPSOBJ2=

LANGOBJ=$(OD)gvplang$(OBJ)

target: all

#################################################################
# Common

!include "$(SRC)common.mak"
!include "$(SRC)gvcver.mak"
DISTDIR=gsview-$(GSVIEW_DOT_VERSION)


#################################################################
# OS/2 files


VIEWONLY=0

OBJPLAT1=$(OD)gvpm$(OBJ) $(OD)gvpdisp$(OBJ) $(OD)gvpdlg$(OBJ) \
 $(OD)gvpdll$(OBJ) $(OD)gvpgsver$(OBJ) $(OD)gvpinit$(OBJ) \
 $(OD)gvpmisc$(OBJ) $(OD)gvpreg$(OBJ) $(OD)gvcfile$(OBJ)

OBJPLAT2=$(OD)gvpedit$(OBJ) $(OD)gvpeps$(OBJ) $(OD)gvpprn$(OBJ) \
 $(OD)gvpmeas$(OBJ) $(OD)gvcmeas2$(OBJ)
!if $(VIEWONLY)
OBJS=viewonly$(OBJ) $(OBJCOM1) $(OBJPLAT1)
!else
OBJS=$(OBJCOM1) $(OBJCOM2) $(OBJPLAT1) $(OBJPLAT2)
!endif



# OS/2 target

all: $(BD)gvpm.exe $(BD)gvpmen.hlp \
  $(BD)gvpmct.dll $(BD)gvpmct.hlp $(BD)setup2ct.dll \
  $(BD)gvpmde.dll $(BD)gvpmde.hlp $(BD)setup2de.dll \
  $(BD)gvpmes.dll $(BD)gvpmes.hlp $(BD)setup2es.dll \
  $(BD)gvpmfr.dll $(BD)gvpmfr.hlp $(BD)setup2fr.dll \
  $(BD)gvpmgr.dll $(BD)gvpmgr.hlp $(BD)setup2gr.dll \
  $(BD)gvpmit.dll $(BD)gvpmit.hlp $(BD)setup2it.dll \
  $(BD)gvpmnl.dll $(BD)gvpmnl.hlp $(BD)setup2nl.dll \
  $(BD)gvpmru.dll $(BD)gvpmru.hlp $(BD)setup2ru.dll \
  $(BD)gvpmse.dll $(BD)gvpmse.hlp $(BD)setup2se.dll \
  $(BD)gvpmsk.dll $(BD)gvpmsk.hlp $(BD)setup2sk.dll \
  $(BD)gvpgs.exe $(BD)os2setup.exe


$(BD)gvpm.exe: $(OBJS) $(OD)gvpm.res $(SRCOS2)gvpm.def
	$(COMP) $(FE)$(BD)gvpm.exe  $(OBJS) $(SRCOS2)gvpm.def
	rc $(OD)gvpm.res $(BD)gvpm.exe

$(OD)gvpm$(OBJ): $(SRCOS2)gvpm.c $(HDRS)
	$(COMP) $(FOO)gvpm$(OBJ) $(CO) $(SRCOS2)gvpm.c

$(OD)gvpdisp$(OBJ): $(SRCOS2)gvpdisp.c $(HDRS)
	$(COMP) $(FOO)gvpdisp$(OBJ) $(CO) $(SRCOS2)gvpdisp.c

$(OD)gvpdlg$(OBJ): $(SRCOS2)gvpdlg.c $(HDRS)
	$(COMP) $(FOO)gvpdlg$(OBJ) $(CO) $(SRCOS2)gvpdlg.c

$(OD)gvpdll$(OBJ): $(SRCOS2)gvpdll.c $(HDRS)
	$(COMP) $(FOO)gvpdll$(OBJ) $(CO) $(SRCOS2)gvpdll.c

$(OD)gvpedit$(OBJ): $(SRCOS2)gvpedit.c $(HDRS)
	$(COMP) $(FOO)gvpedit$(OBJ) $(CO) $(SRCOS2)gvpedit.c

$(OD)gvpeps$(OBJ): $(SRCOS2)gvpeps.c $(SRC)gvceps.h $(HDRS)
	$(COMP) $(FOO)gvpeps$(OBJ) $(CO) $(SRCOS2)gvpeps.c

$(OD)gvpgsver$(OBJ): $(SRCOS2)gvpgsver.c $(HDRS)
	$(COMP) $(FOO)gvpgsver$(OBJ) $(CO) $(SRCOS2)gvpgsver.c

$(OD)gvpinit$(OBJ): $(SRCOS2)gvpinit.c $(HDRS)
	$(COMP) $(FOO)gvpinit$(OBJ) $(CO) $(SRCOS2)gvpinit.c

$(LANGOBJ): $(SRCOS2)gvplang.c
	$(COMP) -Zomf -Zsys $(FOO)gvplang$(OBJ) $(CO) $(SRCOS2)gvplang.c

$(OD)gvpmeas$(OBJ): $(SRCOS2)gvpmeas.c $(HDRS)
	$(COMP) $(FOO)gvpmeas$(OBJ) $(CO) $(SRCOS2)gvpmeas.c

$(OD)gvpmisc$(OBJ): $(SRCOS2)gvpmisc.c $(HDRS)
	$(COMP) $(FOO)gvpmisc$(OBJ) $(CO) $(SRCOS2)gvpmisc.c

$(OD)gvpprn$(OBJ): $(SRCOS2)gvpprn.c $(HDRS)
	$(COMP) $(FOO)gvpprn$(OBJ) $(CO) $(SRCOS2)gvpprn.c

$(OD)gvpreg$(OBJ): $(SRCOS2)gvpreg.c $(HDRS)
	$(COMP) $(FOO)gvpreg$(OBJ) $(CO) $(SRCOS2)gvpreg.c


# OS/2 resources also include common resources so use gvpm1.rc not gvpm3.rc
$(OD)gvpm.res: $(HDRS) $(SRCOS2)gvpm1.rc $(SRCOS2)gvpm3.rc binary\gvpm1.ico $(CODEPAGE) $(BD)gvpmen.hlp en\gvclang.h en\gvclang.rc en\gvplang.rc
        $(CODEPAGE) 1252 850 en\gvclang.h $(OD)gvclang.h
        $(CODEPAGE) 1252 850 en\gvclang.rc $(OD)gvclang.rc
        $(CODEPAGE) 1252 850 en\gvplang.rc $(OD)gvplang.rc
        copy $(SRCOS2)gvpm1.rc+$(OD)gvplang.rc+$(OD)gvphlpen.rc+$(OD)gvclang.rc+$(SRCOS2)gvpm3.rc $(OD)gvpmen.rc
        rc -i $(COMPBASE)\include -i $(SRCDIR) -i $(OBJDIR) -r $(OD)gvpmen.rc $(OD)gvpm.res

$(OD)gvpgs.res: $(SRCOS2)gvpgs.rc $(SRCOS2)gvpgs.h binary\gvpgs.ico $(LANGUAGE)\gvclang.h 
	rc -i $(COMPBASE)\include -i "$(LANGUAGE)" -i $(SRCDIR) -i $(OBJDIR) -r $(SRCOS2)gvpgs.rc $(OD)gvpgs.res 

$(BD)gvpgs.exe: $(SRCOS2)gvpgs.c $(SRCOS2)gvpgs.h $(OD)gvpgs.res
	$(COMP) $(FOO)gvpgs$(OBJ) $(CO) $(SRCOS2)gvpgs.c
	$(COMP) $(FE)$(BD)gvpgs.exe $(OD)gvpgs$(OBJ) $(SRCOS2)gvpgs.def
	rc $(OD)gvpgs.res $(BD)gvpgs.exe

$(BD)gvpmen.hlp: en\gvclang.txt $(CODEPAGE) $(GVDOC) $(DOC2IPF)
	$(CODEPAGE) 1252 850 en\gvclang.txt $(OD)gvc.txt
	$(GVDOC) P $(OD)gvc.txt $(OD)gvpm.txt
	$(DOC2IPF) $(OD)gvpm.txt $(OD)gvpmen.ipf $(OD)gvphlpen.rc
	ipfc $(OD)gvpmen.ipf
	copy $(OD)gvpmen.hlp $(BD)gvpmen.hlp


##########
# Catalan

$(BD)gvpmct.hlp: ct\gvclang.txt $(CODEPAGE) $(GVDOC) $(DOC2IPF)
	$(CODEPAGE) 1252 850 ct\gvclang.txt $(OD)gvc.txt
	$(GVDOC) P $(OD)gvc.txt $(OD)gvpm.txt
	$(DOC2IPF) $(OD)gvpm.txt $(OD)gvpmct.ipf $(OD)gvphlpct.rc
	ipfc $(OD)gvpmct.ipf
	copy $(OD)gvpmct.hlp $(BD)gvpmct.hlp

$(OD)gvpmct.res: $(HDRS) $(SRCOS2)gvpm2.rc $(SRCOS2)gvpm3.rc $(CODEPAGE) $(BD)gvpmct.hlp ct\gvclang.h ct\gvclang.rc ct\gvplang.rc
        $(CODEPAGE) 1252 850 ct\gvclang.h $(OD)gvclang.h
        $(CODEPAGE) 1252 850 ct\gvclang.rc $(OD)gvclang.rc
        $(CODEPAGE) 1252 850 ct\gvplang.rc $(OD)gvplang.rc
        copy $(SRCOS2)gvpm2.rc+$(OD)gvplang.rc+$(OD)gvphlpct.rc+$(OD)gvclang.rc+$(SRCOS2)gvpm3.rc $(OD)gvpmct.rc
        rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -r $(OD)gvpmct.rc $(OD)gvpmct.res

$(BD)gvpmct.dll: $(OD)gvpmct.res ct\gvplang.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)gvpmct.dll, $(OD)gvpmct.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, ct\gvplang.def
	rc $(OD)gvpmct.res $(BD)gvpmct.dll

$(OD)setup2ct.res: $(SRCOS2)os2setup.rc $(SRCOS2)os2setup.h $(SRC)gvcrc.h $(SRC)gvcver.h ct\gvclang.h $(CODEPAGE)
	$(CODEPAGE) 1252 850 ct\gvclang.h $(OD)gvclang.h
	rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -i $(SRCOS2) -r $(SRCOS2)os2setup.rc $(OD)setup2ct.res

$(BD)setup2ct.dll: $(OD)setup2ct.res ct\setup2.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)setup2ct.dll, $(OD)setup2ct.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, ct\setup2.def
	rc $(OD)setup2ct.res $(BD)setup2ct.dll

##########
# German

$(BD)gvpmde.hlp: de\gvclang.txt $(CODEPAGE) $(GVDOC) $(DOC2IPF)
	$(CODEPAGE) 1252 850 de\gvclang.txt $(OD)gvc.txt
	$(GVDOC) P $(OD)gvc.txt $(OD)gvpm.txt
	$(DOC2IPF) $(OD)gvpm.txt $(OD)gvpmde.ipf $(OD)gvphlpde.rc
	ipfc $(OD)gvpmde.ipf
	copy $(OD)gvpmde.hlp $(BD)gvpmde.hlp

$(OD)gvpmde.res: $(HDRS) $(SRCOS2)gvpm2.rc $(SRCOS2)gvpm3.rc $(CODEPAGE) $(BD)gvpmde.hlp de\gvclang.h de\gvclang.rc de\gvplang.rc
        $(CODEPAGE) 1252 850 de\gvclang.h $(OD)gvclang.h
        $(CODEPAGE) 1252 850 de\gvclang.rc $(OD)gvclang.rc
        $(CODEPAGE) 1252 850 de\gvplang.rc $(OD)gvplang.rc
        copy $(SRCOS2)gvpm2.rc+$(OD)gvplang.rc+$(OD)gvphlpde.rc+$(OD)gvclang.rc+$(SRCOS2)gvpm3.rc $(OD)gvpmde.rc
        rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -r $(OD)gvpmde.rc $(OD)gvpmde.res

$(BD)gvpmde.dll: $(OD)gvpmde.res de\gvplang.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)gvpmde.dll, $(OD)gvpmde.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, de\gvplang.def
	rc $(OD)gvpmde.res $(BD)gvpmde.dll

$(OD)setup2de.res: $(SRCOS2)os2setup.rc $(SRCOS2)os2setup.h $(SRC)gvcrc.h $(SRC)gvcver.h de\gvclang.h $(CODEPAGE)
	$(CODEPAGE) 1252 850 de\gvclang.h $(OD)gvclang.h
	rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -i $(SRCOS2) -r $(SRCOS2)os2setup.rc $(OD)setup2de.res

$(BD)setup2de.dll: $(OD)setup2de.res de\setup2.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)setup2de.dll, $(OD)setup2de.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, de\setup2.def
	rc $(OD)setup2de.res $(BD)setup2de.dll

##########
# Spanish

$(BD)gvpmes.hlp: es\gvclang.txt $(CODEPAGE) $(GVDOC) $(DOC2IPF)
	$(CODEPAGE) 1252 850 es\gvclang.txt $(OD)gvc.txt
	$(GVDOC) P $(OD)gvc.txt $(OD)gvpm.txt
	$(DOC2IPF) $(OD)gvpm.txt $(OD)gvpmes.ipf $(OD)gvphlpes.rc
	ipfc $(OD)gvpmes.ipf
	copy $(OD)gvpmes.hlp $(BD)gvpmes.hlp

$(OD)gvpmes.res: $(HDRS) $(SRCOS2)gvpm2.rc $(SRCOS2)gvpm3.rc $(CODEPAGE) $(BD)gvpmes.hlp es\gvclang.h es\gvclang.rc es\gvplang.rc
        $(CODEPAGE) 1252 850 es\gvclang.h $(OD)gvclang.h
        $(CODEPAGE) 1252 850 es\gvclang.rc $(OD)gvclang.rc
        $(CODEPAGE) 1252 850 es\gvplang.rc $(OD)gvplang.rc
        copy $(SRCOS2)gvpm2.rc+$(OD)gvplang.rc+$(OD)gvphlpes.rc+$(OD)gvclang.rc+$(SRCOS2)gvpm3.rc $(OD)gvpmes.rc
        rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -r $(OD)gvpmes.rc $(OD)gvpmes.res

$(BD)gvpmes.dll: $(OD)gvpmes.res es\gvplang.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)gvpmes.dll, $(OD)gvpmes.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, es\gvplang.def
	rc $(OD)gvpmes.res $(BD)gvpmes.dll

$(OD)setup2es.res: $(SRCOS2)os2setup.rc $(SRCOS2)os2setup.h $(SRC)gvcrc.h $(SRC)gvcver.h es\gvclang.h $(CODEPAGE)
	$(CODEPAGE) 1252 850 es\gvclang.h $(OD)gvclang.h
	rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -i $(SRCOS2) -r $(SRCOS2)os2setup.rc $(OD)setup2es.res

$(BD)setup2es.dll: $(OD)setup2es.res es\setup2.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)setup2es.dll, $(OD)setup2es.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, es\setup2.def
	rc $(OD)setup2es.res $(BD)setup2es.dll

##########
# French

$(BD)gvpmfr.hlp: fr\gvclang.txt $(CODEPAGE) $(GVDOC) $(DOC2IPF)
	$(CODEPAGE) 1252 850 fr\gvclang.txt $(OD)gvc.txt
	$(GVDOC) P $(OD)gvc.txt $(OD)gvpm.txt
	$(DOC2IPF) $(OD)gvpm.txt $(OD)gvpmfr.ipf $(OD)gvphlpfr.rc
	ipfc $(OD)gvpmfr.ipf
	copy $(OD)gvpmfr.hlp $(BD)gvpmfr.hlp

$(OD)gvpmfr.res: $(HDRS) $(SRCOS2)gvpm2.rc $(SRCOS2)gvpm3.rc $(CODEPAGE) $(BD)gvpmfr.hlp fr\gvclang.h fr\gvclang.rc fr\gvplang.rc
        $(CODEPAGE) 1252 850 fr\gvclang.h $(OD)gvclang.h
        $(CODEPAGE) 1252 850 fr\gvclang.rc $(OD)gvclang.rc
        $(CODEPAGE) 1252 850 fr\gvplang.rc $(OD)gvplang.rc
        copy $(SRCOS2)gvpm2.rc+$(OD)gvplang.rc+$(OD)gvphlpfr.rc+$(OD)gvclang.rc+$(SRCOS2)gvpm3.rc $(OD)gvpmfr.rc
        rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -r $(OD)gvpmfr.rc $(OD)gvpmfr.res

$(BD)gvpmfr.dll: $(OD)gvpmfr.res fr\gvplang.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)gvpmfr.dll, $(OD)gvpmfr.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, fr\gvplang.def
	rc $(OD)gvpmfr.res $(BD)gvpmfr.dll

$(OD)setup2fr.res: $(SRCOS2)os2setup.rc $(SRCOS2)os2setup.h $(SRC)gvcrc.h $(SRC)gvcver.h fr\gvclang.h $(CODEPAGE)
	$(CODEPAGE) 1252 850 fr\gvclang.h $(OD)gvclang.h
	rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -i $(SRCOS2) -r $(SRCOS2)os2setup.rc $(OD)setup2fr.res

$(BD)setup2fr.dll: $(OD)setup2fr.res fr\setup2.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)setup2fr.dll, $(OD)setup2fr.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, fr\setup2.def
	rc $(OD)setup2fr.res $(BD)setup2fr.dll

##########
# Greek

$(BD)gvpmgr.hlp: gr\gvclang.txt $(CODEPAGE) $(GVDOC) $(DOC2IPF)
	$(CODEPAGE) 1253 869 gr\gvclang.txt $(OD)gvc.txt
	$(GVDOC) P $(OD)gvc.txt $(OD)gvpm.txt
	$(DOC2IPF) $(OD)gvpm.txt $(OD)gvpmgr.ipf $(OD)gvphlpgr.rc
	ipfc $(OD)gvpmgr.ipf
	copy $(OD)gvpmgr.hlp $(BD)gvpmgr.hlp

$(OD)gvpmgr.res: $(HDRS) $(SRCOS2)gvpm2.rc $(SRCOS2)gvpm3.rc $(CODEPAGE) $(BD)gvpmgr.hlp gr\gvclang.h gr\gvclang.rc gr\gvplang.rc
        $(CODEPAGE) 1253 869 gr\gvclang.h $(OD)gvclang.h
        $(CODEPAGE) 1253 869 gr\gvclang.rc $(OD)gvclang.rc
        $(CODEPAGE) 1253 869 gr\gvplang.rc $(OD)gvplang.rc
        copy $(SRCOS2)gvpm2.rc+$(OD)gvplang.rc+$(OD)gvphlpgr.rc+$(OD)gvclang.rc+$(SRCOS2)gvpm3.rc $(OD)gvpmgr.rc
        rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -r $(OD)gvpmgr.rc $(OD)gvpmgr.res

$(BD)gvpmgr.dll: $(OD)gvpmgr.res gr\gvplang.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)gvpmgr.dll, $(OD)gvpmgr.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, gr\gvplang.def
	rc $(OD)gvpmgr.res $(BD)gvpmgr.dll

$(OD)setup2gr.res: $(SRCOS2)os2setup.rc $(SRCOS2)os2setup.h $(SRC)gvcrc.h $(SRC)gvcver.h gr\gvclang.h $(CODEPAGE)
	$(CODEPAGE) 1253 869 gr\gvclang.h $(OD)gvclang.h
	rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -i $(SRCOS2) -r $(SRCOS2)os2setup.rc $(OD)setup2gr.res

$(BD)setup2gr.dll: $(OD)setup2gr.res gr\setup2.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)setup2gr.dll, $(OD)setup2gr.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, gr\setup2.def
	rc $(OD)setup2gr.res $(BD)setup2gr.dll

##########
# Italian

$(BD)gvpmit.hlp: it\gvclang.txt $(CODEPAGE) $(GVDOC) $(DOC2IPF)
	$(CODEPAGE) 1252 850 it\gvclang.txt $(OD)gvc.txt
	$(GVDOC) P $(OD)gvc.txt $(OD)gvpm.txt
	$(DOC2IPF) $(OD)gvpm.txt $(OD)gvpmit.ipf $(OD)gvphlpit.rc
	ipfc $(OD)gvpmit.ipf
	copy $(OD)gvpmit.hlp $(BD)gvpmit.hlp

$(OD)gvpmit.res: $(HDRS) $(SRCOS2)gvpm2.rc $(SRCOS2)gvpm3.rc $(CODEPAGE) $(BD)gvpmit.hlp it\gvclang.h it\gvclang.rc it\gvplang.rc
        $(CODEPAGE) 1252 850 it\gvclang.h $(OD)gvclang.h
        $(CODEPAGE) 1252 850 it\gvclang.rc $(OD)gvclang.rc
        $(CODEPAGE) 1252 850 it\gvplang.rc $(OD)gvplang.rc
        copy $(SRCOS2)gvpm2.rc+$(OD)gvplang.rc+$(OD)gvphlpit.rc+$(OD)gvclang.rc+$(SRCOS2)gvpm3.rc $(OD)gvpmit.rc
        rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -r $(OD)gvpmit.rc $(OD)gvpmit.res

$(BD)gvpmit.dll: $(OD)gvpmit.res it\gvplang.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)gvpmit.dll, $(OD)gvpmit.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, it\gvplang.def
	rc $(OD)gvpmit.res $(BD)gvpmit.dll

$(OD)setup2it.res: $(SRCOS2)os2setup.rc $(SRCOS2)os2setup.h $(SRC)gvcrc.h $(SRC)gvcver.h it\gvclang.h $(CODEPAGE)
	$(CODEPAGE) 1252 850 it\gvclang.h $(OD)gvclang.h
	rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -i $(SRCOS2) -r $(SRCOS2)os2setup.rc $(OD)setup2it.res

$(BD)setup2it.dll: $(OD)setup2it.res it\setup2.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)setup2it.dll, $(OD)setup2it.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, it\setup2.def
	rc $(OD)setup2it.res $(BD)setup2it.dll

##########
# Dutch

$(BD)gvpmnl.hlp: nl\gvclang.txt $(CODEPAGE) $(GVDOC) $(DOC2IPF)
	$(CODEPAGE) 1252 850 nl\gvclang.txt $(OD)gvc.txt
	$(GVDOC) P $(OD)gvc.txt $(OD)gvpm.txt
	$(DOC2IPF) $(OD)gvpm.txt $(OD)gvpmnl.ipf $(OD)gvphlpnl.rc
	ipfc $(OD)gvpmnl.ipf
	copy $(OD)gvpmnl.hlp $(BD)gvpmnl.hlp

$(OD)gvpmnl.res: $(HDRS) $(SRCOS2)gvpm2.rc $(SRCOS2)gvpm3.rc $(CODEPAGE) $(BD)gvpmnl.hlp nl\gvclang.h nl\gvclang.rc nl\gvplang.rc
        $(CODEPAGE) 1252 850 nl\gvclang.h $(OD)gvclang.h
        $(CODEPAGE) 1252 850 nl\gvclang.rc $(OD)gvclang.rc
        $(CODEPAGE) 1252 850 nl\gvplang.rc $(OD)gvplang.rc
        copy $(SRCOS2)gvpm2.rc+$(OD)gvplang.rc+$(OD)gvphlpnl.rc+$(OD)gvclang.rc+$(SRCOS2)gvpm3.rc $(OD)gvpmnl.rc
        rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -r $(OD)gvpmnl.rc $(OD)gvpmnl.res

$(BD)gvpmnl.dll: $(OD)gvpmnl.res nl\gvplang.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)gvpmnl.dll, $(OD)gvpmnl.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, nl\gvplang.def
	rc $(OD)gvpmnl.res $(BD)gvpmnl.dll

$(OD)setup2nl.res: $(SRCOS2)os2setup.rc $(SRCOS2)os2setup.h $(SRC)gvcrc.h $(SRC)gvcver.h nl\gvclang.h $(CODEPAGE)
	$(CODEPAGE) 1252 850 nl\gvclang.h $(OD)gvclang.h
	rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -i $(SRCOS2) -r $(SRCOS2)os2setup.rc $(OD)setup2nl.res

$(BD)setup2nl.dll: $(OD)setup2nl.res nl\setup2.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)setup2nl.dll, $(OD)setup2nl.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, nl\setup2.def
	rc $(OD)setup2nl.res $(BD)setup2nl.dll

##########
# Russian

$(BD)gvpmru.hlp: ru\gvclang.txt $(CODEPAGE) $(GVDOC) $(DOC2IPF)
	$(CODEPAGE) 1251 866 ru\gvclang.txt $(OD)gvc.txt
	$(GVDOC) P $(OD)gvc.txt $(OD)gvpm.txt
	$(DOC2IPF) $(OD)gvpm.txt $(OD)gvpmru.ipf $(OD)gvphlpru.rc
	ipfc $(OD)gvpmru.ipf
	copy $(OD)gvpmru.hlp $(BD)gvpmru.hlp

$(OD)gvpmru.res: $(HDRS) $(SRCOS2)gvpm2.rc $(SRCOS2)gvpm3.rc $(CODEPAGE) $(BD)gvpmru.hlp ru\gvclang.h ru\gvclang.rc ru\gvplang.rc
        $(CODEPAGE) 1251 866 ru\gvclang.h $(OD)gvclang.h
        $(CODEPAGE) 1251 866 ru\gvclang.rc $(OD)gvclang.rc
        $(CODEPAGE) 1251 866 ru\gvplang.rc $(OD)gvplang.rc
        copy $(SRCOS2)gvpm2.rc+$(OD)gvplang.rc+$(OD)gvphlpru.rc+$(OD)gvclang.rc+$(SRCOS2)gvpm3.rc $(OD)gvpmru.rc
        rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -r $(OD)gvpmru.rc $(OD)gvpmru.res

$(BD)gvpmru.dll: $(OD)gvpmru.res ru\gvplang.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)gvpmru.dll, $(OD)gvpmru.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, ru\gvplang.def
	rc $(OD)gvpmru.res $(BD)gvpmru.dll

$(OD)setup2ru.res: $(SRCOS2)os2setup.rc $(SRCOS2)os2setup.h $(SRC)gvcrc.h $(SRC)gvcver.h ru\gvclang.h $(CODEPAGE)
	$(CODEPAGE) 1251 866 ru\gvclang.h $(OD)gvclang.h
	rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -i $(SRCOS2) -r $(SRCOS2)os2setup.rc $(OD)setup2ru.res

$(BD)setup2ru.dll: $(OD)setup2ru.res ru\setup2.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)setup2ru.dll, $(OD)setup2ru.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, ru\setup2.def
	rc $(OD)setup2ru.res $(BD)setup2ru.dll

##########
# Swedish


$(BD)gvpmse.hlp: se\gvclang.txt $(CODEPAGE) $(GVDOC) $(DOC2IPF)
	$(CODEPAGE) 1252 850 se\gvclang.txt $(OD)gvc.txt
	$(GVDOC) P $(OD)gvc.txt $(OD)gvpm.txt
	$(DOC2IPF) $(OD)gvpm.txt $(OD)gvpmse.ipf $(OD)gvphlpse.rc
	ipfc $(OD)gvpmse.ipf
	copy $(OD)gvpmse.hlp $(BD)gvpmse.hlp

$(OD)gvpmse.res: $(HDRS) $(SRCOS2)gvpm2.rc $(SRCOS2)gvpm3.rc $(CODEPAGE) $(BD)gvpmse.hlp se\gvclang.h se\gvclang.rc se\gvplang.rc
        $(CODEPAGE) 1252 850 se\gvclang.h $(OD)gvclang.h
        $(CODEPAGE) 1252 850 se\gvclang.rc $(OD)gvclang.rc
        $(CODEPAGE) 1252 850 se\gvplang.rc $(OD)gvplang.rc
        copy $(SRCOS2)gvpm2.rc+$(OD)gvplang.rc+$(OD)gvphlpse.rc+$(OD)gvclang.rc+$(SRCOS2)gvpm3.rc $(OD)gvpmse.rc
        rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -r $(OD)gvpmse.rc $(OD)gvpmse.res

$(BD)gvpmse.dll: $(OD)gvpmse.res se\gvplang.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)gvpmse.dll, $(OD)gvpmse.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, se\gvplang.def
	rc $(OD)gvpmse.res $(BD)gvpmse.dll

$(OD)setup2se.res: $(SRCOS2)os2setup.rc $(SRCOS2)os2setup.h $(SRC)gvcrc.h $(SRC)gvcver.h se\gvclang.h $(CODEPAGE)
	$(CODEPAGE) 1252 850 se\gvclang.h $(OD)gvclang.h
	rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -i $(SRCOS2) -r $(SRCOS2)os2setup.rc $(OD)setup2se.res

$(BD)setup2se.dll: $(OD)setup2se.res se\setup2.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)setup2se.dll, $(OD)setup2se.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, se\setup2.def
	rc $(OD)setup2se.res $(BD)setup2se.dll

##########
# Slovak


$(BD)gvpmsk.hlp: sk\gvclang.txt $(CODEPAGE) $(GVDOC) $(DOC2IPF)
	$(CODEPAGE) 1250 852 sk\gvclang.txt $(OD)gvc.txt
	$(GVDOC) P $(OD)gvc.txt $(OD)gvpm.txt
	$(DOC2IPF) $(OD)gvpm.txt $(OD)gvpmsk.ipf $(OD)gvphlpsk.rc
	ipfc $(OD)gvpmsk.ipf
	copy $(OD)gvpmsk.hlp $(BD)gvpmsk.hlp

$(OD)gvpmsk.res: $(HDRS) $(SRCOS2)gvpm2.rc $(SRCOS2)gvpm3.rc $(CODEPAGE) $(BD)gvpmsk.hlp sk\gvclang.h sk\gvclang.rc sk\gvplang.rc
        $(CODEPAGE) 1250 852 sk\gvclang.h $(OD)gvclang.h
        $(CODEPAGE) 1250 852 sk\gvclang.rc $(OD)gvclang.rc
        $(CODEPAGE) 1250 852 sk\gvplang.rc $(OD)gvplang.rc
        copy $(SRCOS2)gvpm2.rc+$(OD)gvplang.rc+$(OD)gvphlpsk.rc+$(OD)gvclang.rc+$(SRCOS2)gvpm3.rc $(OD)gvpmsk.rc
        rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -r $(OD)gvpmsk.rc $(OD)gvpmsk.res

$(BD)gvpmsk.dll: $(OD)gvpmsk.res sk\gvplang.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)gvpmsk.dll, $(OD)gvpmsk.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, sk\gvplang.def
	rc $(OD)gvpmsk.res $(BD)gvpmsk.dll

$(OD)setup2sk.res: $(SRCOS2)os2setup.rc $(SRCOS2)os2setup.h $(SRC)gvcrc.h $(SRC)gvcver.h sk\gvclang.h $(CODEPAGE)
	$(CODEPAGE) 1250 852 sk\gvclang.h $(OD)gvclang.h
	rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -i $(SRCOS2) -r $(SRCOS2)os2setup.rc $(OD)setup2sk.res

$(BD)setup2sk.dll: $(OD)setup2sk.res sk\setup2.def $(LANGOBJ)
	LINK386 $(LDEBUG) $(COMPBASE)\lib\dll0.obj $(LANGOBJ), $(BD)setup2sk.dll, $(OD)setup2sk.map,$(COMPBASE)\lib\gcc.lib $(COMPBASE)\lib\st\c.lib $(COMPBASE)\lib\st\c_dllso.lib $(COMPBASE)\lib\st\sys.lib $(COMPBASE)\lib\c_alias.lib $(COMPBASE)\lib\end.lib $(COMPBASE)\lib\os2.lib, sk\setup2.def
	rc $(OD)setup2sk.res $(BD)setup2sk.dll

##########
# OS/2 setup

# -Zsys needed to avoid using emx.dll, which may not be installed yet.
SETUPFLAG=-Zsys
SETUPOBJS=$(OD)os2setup$(OBJ) $(OD)os2unzip$(OBJ) $(OD)setupc$(OBJ) $(OD)os2prf$(OBJ) $(OD)os2beta$(OBJ)

$(OD)os2beta$(OBJ): $(SRC)gvcbeta.c $(SRC)gvcbeta.h $(SRC)gvcrc.h
	$(COMP) $(SETUPFLAG) $(FOO)os2beta$(OBJ) $(CO) $(SRC)gvcbeta.c

$(OD)os2prf$(OBJ): $(SRC)gvcprf.c
	$(COMP) $(SETUPFLAG) -DNODEBUG_MALLOC $(FOO)os2prf$(OBJ) $(CO) $(SRC)gvcprf.c

$(OD)os2unzip$(OBJ): $(SRCOS2)unzip2.h $(SRCOS2)os2unzip.c
	$(COMP) $(SETUPFLAG) $(FOO)os2unzip$(OBJ) $(CO) $(SRCOS2)os2unzip.c

$(OD)setupc$(OBJ): $(SRCOS2)os2setup.h $(SRCOS2)setupc.h $(SRCOS2)setupc.c
	$(COMP) $(SETUPFLAG) $(FOO)setupc$(OBJ) $(CO) $(SRCOS2)setupc.c

$(OD)os2setup$(OBJ): $(SRCOS2)os2setup.c $(SRCOS2)os2setup.h $(SRC)gvcrc.h $(SRC)gvcbeta.h $(CODEPAGE)
	$(CODEPAGE) 1252 850 $(LANGUAGE)\gvclang.h $(OD)gvclang.h
	$(COMP) $(SETUPFLAG) $(FOO)os2setup$(OBJ) $(CO) $(SRCOS2)os2setup.c

$(OD)os2setup.res: $(SRCOS2)os2setup.rc $(SRCOS2)os2setup.h $(LANGUAGE)\gvclang.h $(CODEPAGE)
	$(CODEPAGE) 1252 850 $(LANGUAGE)\gvclang.h $(OD)gvclang.h
        rc -i $(COMPBASE)\include -i $(OBJDIR) -i $(SRCDIR) -i $(SRCOS2DIR) -r $(SRCOS2)os2setup.rc $(OD)os2setup.res

$(BD)os2setup.exe: $(OD)os2setup.res $(SRCOS2)os2setup.def $(SETUPOBJS)
	$(CCAUX) -Zomf -Zsys $(DEBUGFLAG) $(FE)$(BD)os2setup.exe $(SETUPOBJS) $(SRCOS2)os2setup.def
	rc $(OD)os2setup.res $(BD)os2setup.exe
	

##########
# Documentation


#################################################################
# Cleanup and distribution


clean: commonclean
	-$(RM) $(OD)*.res
	-$(RM) $(OD)*.rc
	-$(RM) $(OD)*.hlp
	-$(RM) $(OD)*.ipf
	-$(RM) $(OD)*.map
	-$(RM) $(OD)gvc.txt
	-$(RM) $(OD)gvclang.h


distclean:
	-del /n $(BINDIR)\*.lib
	-del /n $(BINDIR)\*.pdb
	-del /n $(BINDIR)\*.ilk
	-del /n $(BINDIR)\*.exp

distcopy:
	-mkdir dist
	-mkdir dist\gsview2
	copy gsview2\unzip2.dll dist\gsview2\unzip2.dll
	copy $(BD)os2setup.exe dist\gsview2\os2setup.exe
	copy $(BD)setup2ct.dll dist\gsview2\setup2ct.dll
	copy $(BD)setup2de.dll dist\gsview2\setup2de.dll
	copy $(BD)setup2es.dll dist\gsview2\setup2es.dll
	copy $(BD)setup2fr.dll dist\gsview2\setup2fr.dll
	copy $(BD)setup2gr.dll dist\gsview2\setup2gr.dll
	copy $(BD)setup2it.dll dist\gsview2\setup2it.dll
	copy $(BD)setup2nl.dll dist\gsview2\setup2nl.dll
	copy $(BD)setup2ru.dll dist\gsview2\setup2ru.dll
	copy $(BD)setup2se.dll dist\gsview2\setup2se.dll
	copy $(BD)setup2sk.dll dist\gsview2\setup2sk.dll
	copy FILE_ID.DIZ dist\gsview2\FILE_ID.DIZ
	copy FILE_ID.DIZ dist\FILE_ID.DIZ
	copy LICENCE dist\gsview2\LICENCE
	copy LICENCE dist\LICENCE
	copy Readme.htm dist\gsview2\Readme.htm
	copy Readme.htm dist\Readme.htm
	copy gsview.css dist\gsview2\gsview.css
	copy gsview.css dist\gsview.css
	copy cdorder.txt dist\gsview2\cdorder.txt
	copy cdorder.txt dist\cdorder.txt
	copy regorder.txt dist\gsview2\regorder.txt
	copy regorder.txt dist\regorder.txt
#	copy gsview2\zlib2.dll dist\zlib2.dll
	copy $(BD)gvpm.exe dist\gvpm.exe
	copy binary\gvpm1.ico dist\gvpm.ico
	copy $(BD)gvpmen.hlp dist\gvpmen.hlp
	copy $(BD)gvpmct.hlp dist\gvpmct.hlp
	copy $(BD)gvpmde.hlp dist\gvpmde.hlp
	copy $(BD)gvpmes.hlp dist\gvpmes.hlp
	copy $(BD)gvpmfr.hlp dist\gvpmfr.hlp
	copy $(BD)gvpmgr.hlp dist\gvpmgr.hlp
	copy $(BD)gvpmit.hlp dist\gvpmit.hlp
	copy $(BD)gvpmnl.hlp dist\gvpmnl.hlp
	copy $(BD)gvpmru.hlp dist\gvpmru.hlp
	copy $(BD)gvpmse.hlp dist\gvpmse.hlp
	copy $(BD)gvpmsk.hlp dist\gvpmsk.hlp
	copy $(BD)gvpmct.dll dist\gvpmct.dll
	copy $(BD)gvpmde.dll dist\gvpmde.dll
	copy $(BD)gvpmes.dll dist\gvpmes.dll
	copy $(BD)gvpmfr.dll dist\gvpmfr.dll
	copy $(BD)gvpmgr.dll dist\gvpmgr.dll
	copy $(BD)gvpmit.dll dist\gvpmit.dll
	copy $(BD)gvpmnl.dll dist\gvpmnl.dll
	copy $(BD)gvpmru.dll dist\gvpmru.dll
	copy $(BD)gvpmse.dll dist\gvpmse.dll
	copy $(BD)gvpmsk.dll dist\gvpmsk.dll
	copy $(BD)gvpgs.exe dist\gvpgs.exe
	copy $(SRC)printer.ini dist\printer.ini
	copy pstotext\pstotext.1 dist\pstotext.1
	copy pstotext\pstotext.txt dist\pstotext.txt
	copy pstotext\pstotxt2.dll dist\pstotxt2.dll
	copy pstotext\pstotxt2.exe dist\pstotxt2.exe

gsv$(GSVIEW_VERSION)os2.zip: distcopy
	cd dist
	-del gsview2\os2.zip
	zip -9 -@ gsview2\os2.zip < ..\$(SRCOS2)distlist.txt
	echo Redistribution of this OS/2 GSview MUST be accompanied by the> gsview2\README2.TXT
	echo sources in gsv$(GSVIEW_VERSION)src.zip to meet the licence requirements. >> gsview2\README2.TXT
	-del ..\gsv$(GSVIEW_VERSION)os2.zip
	zip -9 ..\gsv$(GSVIEW_VERSION)os2.zip gsview2\os2.zip gsview2\os2setup.exe gsview2\unzip2.dll 
	zip -9 ..\gsv$(GSVIEW_VERSION)os2.zip gsview2\setup2ct.dll gsview2\setup2de.dll gsview2\setup2es.dll gsview2\setup2fr.dll gsview2\setup2gr.dll gsview2\setup2it.dll gsview2\setup2nl.dll gsview2\setup2ru.dll gsview2\setup2se.dll gsview2\setup2sk.dll
	zip -9 ..\gsv$(GSVIEW_VERSION)os2.zip gsview2\README2.TXT gsview2\Readme.htm gsview2\gsview.css gsview2\cdorder.txt gsview2\regorder.txt gsview2\FILE_ID.DIZ gsview2\LICENCE
	cd ..

zip: gsv$(GSVIEW_VERSION)os2.zip
