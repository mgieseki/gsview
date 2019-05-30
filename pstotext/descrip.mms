#
#  VMS MMK build file for PSTOTEXT
#
#  Hunter Goatley, 27-MAY-1998 13:22
#
.IFDEF __ALPHA__
CFLAGS = $(CFLAGS)/PREFIX=ALL/L_DOUBLE=64
.ELSE
.IFDEF __DECC__
CFLAGS = $(CFLAGS)/PREFIX=ALL
.ELSE
OPTFILE = ,VAXCRTL.OPT
OPTIONS = $(OPTFILE)/OPTIONS
.ENDIF
.ENDIF

.IFDEF __DEBUG__
CDBG = /DEBUG/NOOPTIMIZE
LDBG = /DEBUG
.ELSE
LDBG = /NOTRACEBACK
.ENDIF

.SUFFIXES .PS

.PS.H:
	@ mkbundle := $sys$disk:[]mkbundle.exe
	- mkbundle $(MMS$SOURCE) $(MMS$TARGET)

.C.OBJ:
	$(CC)$(CFLAGS)$(CDBG) $(MMS$SOURCE)

PSTOTEXT :	PSTOTEXT.EXE
	!PStoTEXT built

MKBUNDLE.EXE : MKBUNDLE.OBJ $(OPTFILE)
	$(LINK)$(LINKFLAGS)$(LDBG) $(MMS$SOURCE)$(OPTIONS)

MKBUNDLE.OBJ :	MKBUNDLE.C
	@ open/write tmp strip-cr.edt
	@ cr[0,7] = 13
	@ eof[0,7] = 26
	@ write tmp "s/''cr'//:e"
	@ write tmp "s/''eof'//:e"
	@ write tmp "exit"
	@ close tmp
	@ define/user sys$output _NLA0:
	@ edit/edt/command=strip-cr.edt mkbundle.c
	@ deletee/nolog strip-cr.edt;
	$(CC)$(CFLAGS)$(CDBG) $(MMS$SOURCE)

PSTOTEXT_OBJS = MAIN.OBJ, BUNDLE.OBJ, PTOTDLL.OBJ

PSTOTEXT.EXE :	$(PSTOTEXT_OBJS) $(OPTFILE)
	$(LINK)$(LINKFLAGS)$(LDBG) $(PSTOTEXT_OBJS) $(OPTIONS)

MAIN.OBJ :	MAIN.C, BUNDLE.H, OCR.H, ROT270.H, ROT90.H, PTOTDLL.H, VMS.H

PTOTDLL.OBJ :	PTOTDLL.C, PTOTDLL.H
BUNDLE.OBJ :	BUNDLE.C, BUNDLE.H

OCR.H :		OCR.PS, MKBUNDLE.EXE
ROT90.H :	ROT90.PS, MKBUNDLE.EXE
ROT270.H :	ROT270.PS, MKBUNDLE.EXE

VAXCRTL.OPT :
	@ open/write tmp $(MMS$TARGET)
	@ write tmp "sys$share:vaxcrtl.exe/share"
	@ close tmp
