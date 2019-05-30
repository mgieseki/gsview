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

# Windows makefile for GSview
# for Cygwin compiler
# You will also need the Microsoft Help Compiler from
#  ftp://ftp.microsoft.com/Softlib/MSLFILES/HCWSETUP.EXE

#################################################################
# Windows Cygwin

# DEBUG=1 for Debugging options
DEBUG=1
# WIN32 is the default - don't change this
WIN32=1

INCDIR=/usr/include/w32api

NUL=
D=/$(NUL)

RIPATH=--include-dir=
ROFILE=-o $(NUL)
RLANG=--language=
RIFLAGS=$(RIPATH)$(SRCDIR) $(RIPATH)$(SRCWINDIR) $(RIPATH)$(OBJDIR)
#RIFLAGS=-ipath `cygpath -w $(INCDIR)` -ipath `cygpath -w $(SRCDIR)` -ipath `cygpath -w $(SRCWINDIR)` -ipath `cygpath -w $(OBJDIR)`

# To disable print/convert, set VIEWONLY=1
VIEWONLY=0

# Define the location of the WinZip self-extracting-archive-maker.
ifeq ($(WINZIPSE_XE),)
WINZIPSE_XE="C:\\Program Files\\WinZip Self-Extractor\\WZIPSE32.EXE"
endif

BINDIR=./bin
OBJDIR=./obj
SRCDIR=./src
SRCWINDIR=./srcwin
SRCOS2DIR=./srcos2
SRCUNXDIR=./srcunx

ifeq ($(WIN32),1)
	CDEFS=-D_Windows -D__WIN32__
	WINEXT=32
	CFLAGS=$(CDEFS) -mno-cygwin -O2
	ifeq ($(DEBUG),1)
		DEBUGLINK=-g
		CDEBUG=-g
	endif
	CCAUX = $(CC) $(CFLAGS) -I$(SRCDIR)
	MODEL=32
	CC=gcc $(CDEBUG)
	CPP=g++ $(CDEBUG)
	LINK=g++ -mno-cygwin
        HC=hcw /C /E
else
	echo Only Win32 is supported
endif

RCOMP=windres $(RIFLAGS) $(CDEFS) -O coff

ifeq ($(VIEWONLY),1)
	VIEWFLAGS=-DVIEWONLY -DPREREGISTER
else
	VIEWFLAGS=
endif

COMP=$(CC) -I$(SRCDIR) -I$(SRCWINDIR) -I$(OBJDIR) $(CFLAGS) $(VIEWFLAGS)
CPPCOMP=$(CPP) -I$(SRCDIR) -I$(SRCWINDIR) -I$(OBJDIR) $(CFLAGS) $(VIEWFLAGS)

SRC=$(SRCDIR)/$(NUL)
SRCWIN=$(SRCWINDIR)/$(NUL)
SRCOS2=$(SRCOS2DIR)/$(NUL)
SRCUNX=$(SRCUNX)/$(NUL)
OD=$(OBJDIR)/$(NUL)
BD=$(BINDIR)/$(NUL)
OBJ=.o
EXE=.exe
CO=-c

FE=-o
FO=-o
FEO=-o$(OD)
FOO=-o$(OD)

LDEF=--output-def $(NUL)
LOUT=-o $(NUL)
LDLL=-shared
LCONSOLE=
LGUI=-mwindows
LIBRSP=-lgdi32 -lole32 -lshell32 -lcomdlg32 -lwinspool -ladvapi32 -lole32 -luuid

HDRSPLAT=$(SRCWIN)gvwin.h $(SRCWIN)gvwdib.h $(SRCWIN)gvwpdib.h $(SRCWIN)gvwgsver.h 

CP=cp
RM=rm -f

# if you have a strict ANSI compiler, add -D__STDC__
EPSDEF=-I$(SRC) -D__WIN32__ -DEPSTOOL -DSTDIO
EPSLIBS=-ladvapi32
EPSOBJ2=$(OD)gvwgsver$(OBJ)


target: all

#################################################################
# Common

include $(SRC)common.mak
include $(SRC)gvcver.mak
DISTDIR=gsview-$(GSVIEW_DOT_VERSION)


#################################################################
# Windows files

include $(SRCWIN)wincom.mak

$(BD)gsview$(WINEXT).exe: $(OBJS) $(OD)gsvw$(WINEXT)en.res $(SRCWIN)gvwin$(WINEXT).def $(OD)lib.rsp
# cygwin produces incorrect output if --output-def used.
#	$(LINK) $(DEBUGLINK) $(LGUI) $(LDEF)$(SRCWIN)gvwin$(WINEXT).def $(LOUT)$(BD)gsview$(WINEXT).exe $(OBJS) $(OD)gsvw$(WINEXT)en.res $(LIBRSP)
	$(LINK) $(DEBUGLINK) $(LGUI) $(LOUT)$(BD)gsview$(WINEXT).exe $(OBJS) $(OD)gsvw$(WINEXT)en.res $(LIBRSP)



#################################################################
