# pstotxtv.mak
# Makefile for pstotxt[3a].dll, for use with GSview, 
# Microsoft Visual C++ and Win32 Intel x86 or DEC Alpha. 

# Has not been tested with DEC Alpha

# makefile created by
# Russell Lang, 1998-10-09
# Updated for MSVC++ 7, 2003-03-09
# Updated for MSVC++ 7.1 2004-01-09


# Edit VCVER and DEVBASE as required
!ifndef VCVER
VCVER=71
!endif

!ifndef DEVBASE
!if $(VCVER) <= 5
DEVBASE=C:\Program Files\devstudio
!endif
!if $(VCVER) == 6
DEVBASE=C:\Program Files\Microsoft Visual Studio
!endif
!if $(VCVER) == 7
DEVBASE=C:\Program Files\Microsoft Visual Studio .NET
!endif
!if $(VCVER) == 71
DEVBASE=C:\Program Files\Microsoft Visual Studio .NET 2003
!endif
!endif

# Debugging
DEBUG=1

!if $(DEBUG)
DEBUGLINK=/DEBUG
CDEBUG=/Zi
!endif


# For Intel x386 use pstotxt3
DEST=pstotxt3
DEFS=/D__WIN32__
CFLAGS=$(DEFS) /W4

# For Alpha, uncomment the following two lines
#DEST=pstotxta
#DEFS=/D__WIN32__ /DDECALPHA
#CFLAGS=$(DEFS) /W4

NULL=
D=\$(NUL)

!if $(VCVER) == 0
# No paths specified
COMPBASE=
RCOMP=rc -D_MSC_VER $(DEFS)
COMPDIR=
INCDIR=.
!else
!if $(VCVER) <= 5
COMPBASE = $(DEVBASE)\vc
RCOMP="$(DEVBASE)\sharedide\bin\rc" -D_MSC_VER $(DEFS)
!endif
!if $(VCVER) == 6
COMPBASE = $(DEVBASE)\vc98
RCOMP="$(DEVBASE)\common\msdev98\bin\rc" -D_MSC_VER $(DEFS)
!endif
!if (($(VCVER) == 7) || ($(VCVER) == 71))
COMPBASE = $(DEVBASE)\vc7
RCOMP="$(DEVBASE)\Vc7\bin\rc" -D_MSC_VER $(DEFS)
!endif
COMPDIR = $(COMPBASE)\bin$(D)
INCDIR = $(COMPBASE)\include
LIBDIR = $(COMPBASE)\lib
!endif

CC="$(COMPDIR)cl" -DNEED_PROTO $(CFLAGS) $(CDEBUG) "-I$(INCDIR)"
CCAUX=$(CC)

all:	$(DEST).dll $(DEST).exe

.c.obj:
	$(CC) -c $*.c

ocr.h: ocr.ps mkrch.exe
	mkrch $*.ps $*.h 1

rot270.h: rot270.ps mkrch.exe
	mkrch $*.ps $*.h 2

rot90.h: rot90.ps mkrch.exe
	mkrch $*.ps $*.h 3

mkrch.exe: mkrch.c
	$(CCAUX) $*.c

$(DEST).obj: ptotdll.c ptotdll.h
	$(CC) /c /D_Windows /D__DLL__ /Fo$(DEST).obj ptotdll.c

$(DEST).rc:  ocr.h rot270.h rot90.h
	copy ocr.h+rot270.h+rot90.h $(DEST).rc

$(DEST).res: pstotxt3.rc
	$(RCOMP) "-i$(INCDIR)" -r $(DEST).rc

$(DEST).dll: $(DEST).obj $(DEST).res
	"$(COMPDIR)link" $(DEBUGLINK) /DLL /DEF:pstotxt3.def /OUT:$(DEST).dll $(DEST).obj $(DEST).res

$(DEST).exe: pstotxtd.c
	$(CC) /D_Windows /Fe$(DEST).exe pstotxtd.c /link $(DEBUGLINK)

prezip: all
	copy $(DEST).dll ..\$(DEST).dll
	copy $(DEST).exe ..\$(DEST).exe
	copy pstotext.txt ..\pstotext.txt

clean:
	-del pstotxtd.exe
	-del $(DEST).exe
	-del $(DEST).dll
	-del $(DEST).res
	-del $(DEST).rc
	-del $(DEST).exp
	-del $(DEST).ilk
	-del $(DEST).lib
	-del $(DEST).pdb
	-del *.obj
	-del ocr.h
	-del rot270.h
	-del rot90.h
	-del mkrch.exe
	-del mkrch.ilk
	-del mkrch.pdb
	-del vc*.pdb

