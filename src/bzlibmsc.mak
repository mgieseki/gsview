# Makefile for Microsoft Visual C++ 6.0
# usage: nmake -f makefile.msc
# K.M. Syring (syring@gsf.de)
# Fixed up by JRS for bzip2-0.9.5d release.
# DLL target added by Russell Lang, Ghostgum Software Pty Ltd.

# To build libbz2.dll, set MAKEDLL=1
# or use 'nmake -f makefile.msc MAKEDLL=1'
MAKEDLL=0

!if $(MAKEDLL)
# Use static multithread RTL, export APIs
RTL=/MT
DLLFLAG=/DMAKEDLL
!else
# Use dynamic (DLL) multithread RTL
RTL=/MD
!endif

# To enable debugging, set DEBUG=1
DEBUG=0
!if $(DEBUG)
OPT=/Zi /Od
!else
OPT=/Ox
!endif

CC=cl
CFLAGS= -DWIN32 $(OPT) $(RTL) $(DLLFLAG)

OBJS= blocksort.obj  \
      huffman.obj    \
      crctable.obj   \
      randtable.obj  \
      compress.obj   \
      decompress.obj \
      bzlib.obj

!if $(MAKEDLL)
all: dlltest.exe
!else
all: lib bzip2 test
!endif

bzip2: lib
	$(CC) $(CFLAGS) -o bzip2 bzip2.c libbz2.lib setargv.obj
	$(CC) $(CFLAGS) -o bzip2recover bzip2recover.c

lib: $(OBJS)
	del libbz2.lib
	lib /out:libbz2.lib $(OBJS)

test: bzip2
	type words1
	.\\bzip2 -1  < sample1.ref > sample1.rb2
	.\\bzip2 -2  < sample2.ref > sample2.rb2
	.\\bzip2 -3  < sample3.ref > sample3.rb2
	.\\bzip2 -d  < sample1.bz2 > sample1.tst
	.\\bzip2 -d  < sample2.bz2 > sample2.tst
	.\\bzip2 -ds < sample3.bz2 > sample3.tst
	fc sample1.bz2 sample1.rb2 
	fc sample2.bz2 sample2.rb2
	fc sample3.bz2 sample3.rb2
	fc sample1.tst sample1.ref
	fc sample2.tst sample2.ref
	fc sample3.tst sample3.ref
	@echo All six of the fc's should find no differences.
	@echo If fc finds an error on sample3.tst, this could be
	@echo because WinZips 'TAR file smart CR/LF conversion'
	@echo is too clever for its own good.  Disable this option.
	@echo The correct size for sample3.ref is 120,244.  If it
	@echo is around 150k, WinZip has stuffed it up.
	@echo Also remember to set BZ_UNIX to 0 and BZ_LCCWIN32
	@echo to 1 in bzip2.c.

!if $(MAKEDLL)
libbz2.dll: $(OBJS) libbz2.def
	$(CC) $(CFLAGS) /LD /Felibbz2.dll $(OBJS) libbz2.def

dlltest.exe: libbz2.dll dlltest.obj
	$(CC) $(CFLAGS) -o dlltest.exe dlltest.obj
!endif
	

clean: 
	del *.obj
	del dlltest.exe
	del libbz2.lib 
	del libbz2.dll
	del libbz2.exp
	del bzip2.exe
	del bzip2recover.exe
	del sample1.rb2 
	del sample2.rb2 
	del sample3.rb2
	del sample1.tst 
	del sample2.tst
	del sample3.tst

.c.obj: 
	$(CC) $(CFLAGS) -c $*.c -o $*.obj

