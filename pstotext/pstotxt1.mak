# pstotxt1.mak
# Makefile for pstotxt1.dll, for use with Win16 GSview

# makefile created by
# Russell Lang, 1996-07-29
# updated 1996-10-11

# For debugging, use bcc -v

COMPBASE = c:\bc45
COMPDIR = $(COMPBASE)\bin
INCDIR = $(COMPBASE)\include
LIBDIR = $(COMPBASE)\lib
#DEBUGLINK = -v
MODEL=l

CCAUX=$(COMPDIR)\bcc
CC=$(COMPDIR)\bcc $(DEBUGLINK) -m$(MODEL) -DNEED_PROTO

all:	pstotxt1.dll

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

pstotxt1.obj: ptotdll.c ptotdll.h
	$(CC) -c -WD -opstotxt1.obj ptotdll.c

pstotxt1.rc:  ocr.h rot270.h rot90.h
	copy ocr.h+rot270.h+rot90.h pstotxt1.rc

pstotxt1.res: pstotxt1.rc
	$(COMPDIR)\brcc -i$(INCDIR) -r pstotxt1.rc


pstotxt1.dll: pstotxt1.obj pstotxt1.res
	$(CC) -WD -epstotxt1.dll pstotxt1.obj
	$(COMPDIR)\rlink pstotxt1.res pstotxt1.dll

prezip: pstotxt1.dll
	copy pstotxt1.dll ..\pstotxt1.dll
	copy pstotext.txt ..\pstotext.txt

clean:
	-del pstotxt1.dll
	-del pstotxt1.res
	-del pstotxt1.rc
	-del *.obj
	-del ocr.h
	-del rot270.h
	-del rot90.h
	-del mkrch.exe

