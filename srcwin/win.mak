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

# Edit VCVER and DEVBASE as required

!if defined(_NMAKE_VER) && !defined(VCVER)
!if "$(_NMAKE_VER)" == "162"
VCVER=5
!endif
!if "$(_NMAKE_VER)" == "6.00.8168.0"
VCVER=6
!endif
!if "$(_NMAKE_VER)" == "7.00.9466"
VCVER=7
!endif
!if "$(_NMAKE_VER)" == "7.10.3077"
VCVER=71
!endif
!if "$(_NMAKE_VER)" == "8.00.50727.42"
VCVER=8
!endif
!if "$(_NMAKE_VER)" == "8.00.50727.762"
VCVER=8
!endif
!if "$(_NMAKE_VER)" == "9.00.21022.08"
VCVER=9
!endif
!if "$(_NMAKE_VER)" == "9.00.30729.01"
VCVER=9
!endif
!endif

!ifndef VCVER
VCVER=71
!endif

# DEBUG=1 for Debugging options
!ifndef DEBUG
DEBUG=0
!endif

# Win64 requires Microsoft Visual Studio 8 (.NET 2005)
# or Microsoft Visual Studio .NET 2003 with Windows Server 2003 DDK.
!ifdef WIN64
WIN32=0
WIN64=1
!else
WIN32=1
WIN64=0
!endif

# XP defines this as an environment variable.
# 32-bit is C:\Program Files, 64-bit is C:\Program Files (x86)
!ifndef PROGRAMFILES
PROGRAMFILES=C:\Program Files
!endif

!ifndef DEVBASE
!if $(VCVER) <= 5
DEVBASE=$(PROGRAMFILES)\devstudio
!endif
!if $(VCVER) == 6
DEVBASE=$(PROGRAMFILES)\Microsoft Visual Studio
!endif
!if $(VCVER) == 7
DEVBASE=$(PROGRAMFILES)\Microsoft Visual Studio .NET
!endif
!if $(VCVER) == 71
DEVBASE=$(PROGRAMFILES)\Microsoft Visual Studio .NET 2003
DDKBASE=c:\winddk\3790
!endif
!if $(VCVER) == 8
DEVBASE=$(PROGRAMFILES)\Microsoft Visual Studio 8
!endif
!if $(VCVER) == 9
DEVBASE=$(PROGRAMFILES)\Microsoft Visual Studio 9.0
COMMONBASE=$(PROGRAMFILES)\Microsoft SDKs\Windows\v6.0A
!endif
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

BINDIR=.\bin
OBJDIR=.\obj
SRCDIR=.\src
SRCWINDIR=.\srcwin
SRCOS2DIR=.\srcos2
SRCUNXDIR=.\srcunx


!if $(VCVER) <= 5
COMPBASE = $(DEVBASE)\vc
!endif
!if $(VCVER) == 6
COMPBASE = $(DEVBASE)\vc98
!endif
!if (($(VCVER) == 7) || ($(VCVER) == 71))
COMPBASE = $(DEVBASE)\Vc7
PLATLIBDIR=$(COMPBASE)\PlatformSDK\lib
!endif
!if ($(VCVER) == 8)
COMPBASE = $(DEVBASE)\VC
PLATLIBDIR=$(COMPBASE)\PlatformSDK\lib
!endif
!if ($(VCVER) == 9)
COMPBASE = $(DEVBASE)\VC
PLATLIBDIR=$(COMMONBASE)\lib
INCPLAT=-I"$(COMMONBASE)\Include"
!endif

COMPDIR = $(COMPBASE)\bin
INCDIR = $(COMPBASE)\include
LIBDIR = $(COMPBASE)\lib


# MSVC 8 (2005) warns about deprecated common functions like fopen.
!if $(VCVER) >= 8
VC8WARN=/wd4996
!else
VC8WARN=
!endif

!if $(WIN32)
CDEFS=-D_Windows -D__WIN32__  $(INCPLAT) -I"$(INCDIR)"
WINEXT=32
CFLAGS=$(CDEFS) /MT /nologo $(VC8WARN)
LINKMACHINE=IX86
!if $(DEBUG)
DEBUGLINK=/DEBUG
CDEBUG=/Zi
!endif
MODEL=32
CCAUX = "$(COMPDIR)\cl" -I"$(INCDIR)" $(VC8WARN) /nologo
CC = "$(COMPDIR)\cl" $(CDEBUG)
CPP = "$(COMPDIR)\cl" $(CDEBUG)
LINK = "$(COMPDIR)\link"

!else if $(WIN64)
CDEFS=-D_Windows -D__WIN32__  $(INCPLAT) -I"$(INCDIR)"
WINEXT=64
CFLAGS=$(CDEFS) /MT /nologo $(VC8WARN)
!if $(VCVER) == 71
LINKMACHINE=X86
!else
LINKMACHINE=X64
!endif
!if $(DEBUG)
DEBUGLINK=/DEBUG
CDEBUG=/Zi
!endif
MODEL=64

!if $(VCVER) == 71
# Microsoft Visual Studio .NET 2003 + Windows Server 2003 DDK
CCAUX = "$(COMPDIR)\cl" -I"$(INCDIR)" $(VC8WARN) /nologo
CC = "$(DDKBASE)\bin\win64\x86\amd64\cl" $(CDEBUG)
CPP = "$(DDKBASE)\bin\win64\x86\amd64\cl" $(CDEBUG)
LINK = "$(DDKBASE)\bin\win64\x86\amd64\link"
LIBDIR=$(DDKBASE)\lib\wnet\amd64
PLATLIBDIR=$(DDKBASE)\lib\wnet\amd64
!else if $(VCVER) == 8
# Microsoft Visual Studio .NET 2005, AMD64 cross compiler
# Native AMD64 compiler is in $(COMPDIR)\amd64\cl
CCAUX = "$(COMPDIR)\cl" -I"$(INCDIR)" $(VC8WARN) /nologo
CC = "$(COMPDIR)\x86_amd64\cl" $(CDEBUG)
CPP = "$(COMPDIR)\x86_amd64\cl" $(CDEBUG)
LINK = "$(COMPDIR)\x86_amd64\link"
LIBDIR=$(COMPBASE)\lib\amd64
PLATLIBDIR=$(COMPBASE)\PlatformSDK\Lib\AMD64
!else
!message Win64 compilation needs a different compiler
!endif

!else
!message Only Win32 or Win64 is supported
!endif

!ifndef PLATLIBDIR
PLATLIBDIR=$(LIBDIR)
!endif

CLFLAG=
RIPATH=-i
ROFILE=-fo
RLANG=-c
RIFLAGS=$(RIPATH)"$(INCDIR)" $(RIPATH)"$(SRCDIR)" $(RIPATH)"$(SRCWINDIR)" $(RIPATH)"$(OBJDIR)"
!if $(VCVER) <= 5
HC="$(COMPDIR)\hcw" /C /E
RCOMP="$(DEVBASE)\sharedide\bin\rc" -D_MSC_VER $(CDEFS) $(RIFLAGS)
!endif
!if $(VCVER) == 6
HC="$(DEVBASE)\common\tools\hcw" /C /E
RCOMP="$(DEVBASE)\common\msdev98\bin\rc" -D_MSC_VER $(CDEFS) $(RIFLAGS)
!endif
!if (($(VCVER) == 7) || ($(VCVER) == 71))
HC="$(DEVBASE)\Common7\Tools\hcw" /C /E
RCOMP="$(DEVBASE)\Vc7\bin\rc" -D_MSC_VER $(CDEFS) $(RIFLAGS)
!endif
!if $(VCVER) == 8
# Help Compiler is no longer included in the SDK.
# Search on the Internet for hcw403_setup.zip
HC="$(PROGRAMFILES)\Help Workshop\hcw" /C /E
HTMLHELP="C:\Program Files (x86)\HTML Help Workshop\hhc.exe"
RCOMP="$(DEVBASE)\VC\bin\rc" -D_MSC_VER $(CDEFS) $(RIFLAGS)
!endif
!if $(VCVER) == 9
# Help Compiler is no longer included in the SDK.
# Search on the Internet for hcw403_setup.zip
HC="$(PROGRAMFILES)\Help Workshop\hcw" /C /E
HTMLHELP="C:\Program Files\HTML Help Workshop\hhc.exe"
RCOMP="$(COMMONBASE)\bin\rc" -D_MSC_VER $(CDEFS) $(RIFLAGS)
!endif


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
LDLL=/DLL /NODEFAULTLIB /NOENTRY /MACHINE:$(LINKMACHINE)
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

!include "$(SRCWIN)wincom.mak"

$(BD)gsview$(WINEXT).exe: $(OBJS) $(OD)gsvw$(WINEXT)en.res $(SRCWIN)gvwin$(WINEXT).def $(OD)lib.rsp
	$(LINK) $(DEBUGLINK) $(LGUI) $(LDEF)$(SRCWIN)gvwin$(WINEXT).def $(LOUT)$(BD)gsview$(WINEXT).exe $(OBJS) $(OD)gsvw$(WINEXT)en.res $(LIBRSP)


#################################################################
