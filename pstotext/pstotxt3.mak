# pstotxt3.mak
# Makefile for pstotxt3.dll, for use with Win32 GSview

# makefile created by
# Russell Lang, 1996-07-29
# updated 1996-10-11

# For debugging, use bcc -v

COMPBASE = c:\bc45
COMPDIR = $(COMPBASE)\bin
INCDIR = $(COMPBASE)\include
LIBDIR = $(COMPBASE)\lib
#DEBUGLINK = -v

CCAUX=$(COMPDIR)\bcc
CC=$(COMPDIR)\bcc32 $(DEBUGLINK) -DNEED_PROTO 

all:	pstotxt3.dll pstotxt3.exe

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

pstotxt3.obj: ptotdll.c ptotdll.h
	$(CC) -c -WD -opstotxt3.obj ptotdll.c

pstotxt3.rc:  ocr.h rot270.h rot90.h
	copy ocr.h+rot270.h+rot90.h pstotxt3.rc

pstotxt3.res: pstotxt3.rc
	$(COMPDIR)\brcc32 -i$(INCDIR) -r pstotxt3.rc


pstotxt3.dll: pstotxt3.obj pstotxt3.res
	$(CC) -WD -epstotxt3.dll pstotxt3.obj
	$(COMPDIR)\brc32 pstotxt3.res pstotxt3.dll

pstotxt3.exe: pstotxtd.c
	$(CC) -WC -epstotxtd.exe pstotxtd.c
	-del pstotxt3.exe
	rename pstotxtd.exe pstotxt3.exe

prezip: all
	copy pstotxt3.dll ..\pstotxt3.dll
	copy pstotxt3.exe ..\pstotxt3.exe
	copy pstotext.txt ..\pstotext.txt

clean:
	-del pstotxtd.exe
	-del pstotxt3.exe
	-del pstotxt3.dll
	-del pstotxt3.res
	-del pstotxt3.rc
	-del *.obj
	-del ocr.h
	-del rot270.h
	-del rot90.h
	-del mkrch.exe

