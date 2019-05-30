# Copyright (C) 1995, Digital Equipment Corporation.
# All rights reserved.
# See the file pstotext.txt for a full description.
# Last modified on Fri Jan 09 21:20:00 AEST 2004 by rjl (fixed dependencies)
# Last modified on Wed Oct 28 08:45:54 PST 1998 by mcjones

# pstotext now requires an ANSI-compatible C compiler, such as gcc.
# If you absolutely must use a pre-ANSI compiler, you can try
# commenting out the includes of ptotdll.h in ptotdll.c and main.c.
# PMcJ 6 Sep 96

CC=gcc
#CC=cc -std

BUNDLE = ocr.h rot270.h rot90.h

all:	pstotext

main.o: main.c ptotdll.h bundle.h ocr.h rot270.h rot90.h
	$(CC) -c $*.c

ptotdll.o: ptotdll.c ptotdll.h
	$(CC) -c $*.c

pstotext: bundle.o main.o ptotdll.o
	$(CC) -o pstotext main.o bundle.o ptotdll.o -lm

.SUFFIXES: .ps

.c.o:
	$(CC) -c $*.c

# "Bundle" an Ascii file.
.ps.h:
	echo "const char *$*[] = {" > $*.h
	sed -e 's/"/\\"/g' -e 's/\(.*\)/  "\1\\n",/' $*.ps >> $*.h
	echo "  0" >> $*.h
	echo "};" >> $*.h

tidy:
	rm -f ,* .,* .emacs_[0-9]* core *~ 


clean:
	rm -f pstotext *.o core $(BUNDLE)
