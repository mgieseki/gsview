#  Copyright (C) 1993-2011, Ghostgum Software Pty Ltd.  All rights reserved.
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

#################################################################
# Windows

!if "$(VSCMD_ARG_TGT_ARCH)" == "x64"
WIN32=0
WIN64=1
!else
WIN32=1
WIN64=0
!endif

# Edit VCVER and DEVBASE as required
VCVER=2019
#
# DEBUG=1 for Debugging options
!ifndef DEBUG
DEBUG=0
!endif

NUL=
D=\$(NUL)

# To disable print/convert, set VIEWONLY=1
!ifndef VIEWONLY
VIEWONLY=0
!endif

# Define the location of the WinZip self-extracting-archive-maker.
!ifndef WINZIPSE_XE
WINZIPSE_XE="$(PROGRAMFILES)\WinZip Self-Extractor\WZIPSE32.EXE"
!endif

BINDIR=bin
OBJDIR=obj
SRCDIR=src
SRCWINDIR=srcwin
SRCOS2DIR=srcos2
SRCUNXDIR=srcunx


# MSVC 8 (2005) warns about deprecated common functions like fopen.
VC8WARN=/wd4996

CDEFS=-D_Windows -D__WIN32__
CFLAGS=$(CDEFS) /MT /nologo $(VC8WARN)

!if $(WIN32)
WINEXT=32
MODEL=32
!else if $(WIN64)
WINEXT=64
MODEL=64
!endif

!if $(DEBUG)
DEBUGLINK=/DEBUG
CDEBUG=/Zi
!endif

CCAUX = "cl" $(VC8WARN) /nologo
CC = "cl" $(CDEBUG)
CPP = "cl" $(CDEBUG)
LINK = "link" $(DEBUGLINK)

CLFLAG=
RIPATH=-i
ROFILE=-fo
RLANG=-c
RIFLAGS=$(RIPATH)"$(SRCDIR)" $(RIPATH)"$(SRCWINDIR)" $(RIPATH)"$(OBJDIR)"
# Help Compiler is no longer included in the SDK.
# It can be downloaded here:
# https://www.microsoft.com/en-us/download/details.aspx?id=21138
HC="hcw" /C /E
HTMLHELP="c:\program files (x86)\HTML Help Workshop\hhc.exe"
RCOMP="rc" -D_MSC_VER $(CDEFS) $(RIFLAGS)


!if $(VIEWONLY)
VIEWFLAGS=-DVIEWONLY -DPREREGISTER
!else
VIEWFLAGS=
!endif

COMP=$(CC) -I$(SRCDIR) -I$(SRCWINDIR) -I$(OBJDIR) $(CFLAGS) $(VIEWFLAGS)
CPPCOMP=$(CC) -I$(SRCDIR) -I$(SRCWINDIR) -I$(OBJDIR) $(CFLAGS) $(VIEWFLAGS)


SRC=$(SRCDIR)\$(NUL)
SRCWIN=$(SRCWINDIR)\$(NUL)
SRCOS2=$(SRCOS2DIR)\$(NUL)
SRCUNX=$(SRCUNX)\$(NUL)
OD=$(OBJDIR)\$(NUL)
BD=$(BINDIR)\$(NUL)
OBJ=.obj
EXE=.exe
CO=-c

FE=-Fe
FO=-Fo
FEO=-Fe$(OD)
FOO=-Fo$(OD)

LDEF=/DEF:
LOUT=/OUT:
LDLL=/DLL /NODEFAULTLIB /NOENTRY
LCONSOLE=/SUBSYSTEM:CONSOLE 
LGUI=/SUBSYSTEM:WINDOWS
LIBRSP=@$(OD)lib.rsp

HDRSPLAT=$(SRCWIN)gvwin.h $(SRCWIN)gvwdib.h $(SRCWIN)gvwpdib.h $(SRCWIN)gvwgsver.h 

CP=copy
RM=del
target: all

#################################################################
# Common

!include "$(SRC)common.mak"
!include "$(SRC)gvcver.mak"
DISTDIR=gsview-$(GSVIEW_DOT_VERSION)


#################################################################
# Windows files

!include "srcwin\wincom.mak"

$(BD)gsview$(WINEXT).exe: $(OBJS) $(OD)gsvw$(WINEXT)en.res $(SRCWIN)gvwin$(WINEXT).def $(OD)lib.rsp
	$(LINK) $(DEBUGLINK) $(LGUI) $(LDEF)$(SRCWIN)gvwin$(WINEXT).def $(LOUT)$(BD)gsview$(WINEXT).exe $(OBJS) $(OD)gsvw$(WINEXT)en.res $(LIBRSP)


#################################################################
