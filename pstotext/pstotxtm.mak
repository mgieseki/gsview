# pstotxtm.mak
# Makefile for pstotxtm.exe, for 8086 MS-DOS

# makefile created by
# Russell Lang, 1996-10-11

# For debugging, use bcc -v

COMPBASE = c:\bc45
COMPDIR = $(COMPBASE)\bin
INCDIR = $(COMPBASE)\include
LIBDIR = $(COMPBASE)\lib
#DEBUGLINK = -v
MODEL=l

CCAUX=$(COMPDIR)\bcc
CC=$(COMPDIR)\bcc $(DEBUGLINK) -m$(MODEL) -DNEED_PROTO -DMSDOS

all:	pstotxtm.exe

.c.obj:
	$(CC) -c $*.c

ocr.h: ocr.ps mkbundle.exe
	mkbundle $*.ps $*.h

rot270.h: rot270.ps mkbundle.exe
	mkbundle $*.ps $*.h

rot90.h: rot90.ps mkbundle.exe
	mkbundle $*.ps $*.h

mkbundle.exe: mkbundle.c
	$(CCAUX) $*.c

ptotdll.obj: ptotdll.c ptotdll.h
	$(CC) -c $*.c

bundle.obj: bundle.c
	$(CC) -c $*.c

pstotxtm.obj: pstotxtd.c ocr.h rot270.h rot90.h
	$(CC) -c -opstotxtm.obj pstotxtd.c

pstotxtm.exe: pstotxtm.obj ptotdll.obj bundle.obj
	$(CC) -epstotxtm.exe pstotxtm.obj ptotdll.obj bundle.obj
	rename pstotxtm.exe pstotxtm.exe

prezip: pstotxtm.exe
	copy pstotxtm.exe ..\pstotxtm.exe
	copy pstotext.txt ..\pstotext.txt

clean:
	-del pstotxtm.exe
	-del *.obj
	-del ocr.h
	-del rot270.h
	-del rot90.h
	-del mkbundle.exe

