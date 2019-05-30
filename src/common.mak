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

# Common makefile for GSview


#################################################################
# Common

ECHOGSV=$(OD)echogsv$(EXE)
GVDOC=$(OD)gvdoc$(EXE)
DOC2IPF=$(OD)doc2ipf$(EXE)
DOC2HTML=$(OD)doc2html$(EXE)
DOC2RTF=$(OD)doc2rtf$(EXE)
DOC2HHP=$(OD)doc2hhp$(EXE)
CODEPAGE=$(OD)codepage$(EXE)

OBJCOM1=$(OD)gvcmisc$(OBJ) $(OD)gvcdisp$(OBJ) $(OD)gvccmd$(OBJ) \
 $(OD)gvcinit$(OBJ) $(OD)gvcprf$(OBJ) \
 $(OD)dscparse$(OBJ) $(OD)dscutil$(OBJ) \
 $(OD)gvcdll$(OBJ) $(OD)gvcpdf$(OBJ) \
 $(OD)gvcbeta$(OBJ) $(OD)gvcreg$(OBJ) \
 $(OD)cdll$(OBJ) $(OD)cimg$(OBJ) $(OD)cview$(OBJ)

OBJCOM2=$(OD)gvcedit$(OBJ) $(OD)gvcmeas$(OBJ) \
 $(OD)gvcprn$(OBJ) $(OD)gvceps$(OBJ) $(OD)gvctext$(OBJ)

OBJCOM=$(OBJCOM1) $(OBJCOM2)

HDRSGS=$(SRC)iapi.h $(SRC)errors.h $(SRC)gdevdsp.h
HDRSCOM=$(OD)gsvver.h $(SRC)dscparse.h $(SRC)gvcfn.h $(SRC)gvcver.h $(SRC)gvcfile.h $(SRC)gvctype.h

HDRS=$(HDRSGS) $(HDRSCOM) $(HDRSPLAT)

CLEAN1=$(OD)gsvver.h
CLEAN2=$(OD)*$(OBJ)
CLEAN3=$(OD)*$(EXE)


# Documentation

$(CODEPAGE): $(SRC)codepage.c $(SRC)gvccp.c $(SRC)gvccp.h
	$(CCAUX) $(FOO)codepage$(OBJ) $(CO) $(SRC)codepage.c
	$(CCAUX) $(FOO)gvccp$(OBJ) $(CO) $(SRC)gvccp.c
	$(CCAUX) $(CLFLAG) $(FE)$(CODEPAGE) $(OD)codepage$(OBJ) $(OD)gvccp$(OBJ)

$(GVDOC): $(SRC)gvdoc.c
	$(CCAUX) $(FOO)gvdoc$(OBJ) $(CO) $(SRC)gvdoc.c
	$(CCAUX) $(CLFLAG) $(FE)$(GVDOC) $(OD)gvdoc$(OBJ)

$(DOC2IPF): $(SRC)doc2ipf.c
	$(CCAUX) $(FOO)doc2ipf$(OBJ) $(CO) $(SRC)doc2ipf.c
	$(CCAUX) $(CLFLAG) $(FE)$(DOC2IPF) $(OD)doc2ipf$(OBJ)

$(DOC2HTML): $(SRC)doc2html.c
	$(CCAUX) $(FOO)doc2html$(OBJ) $(CO) $(SRC)doc2html.c
	$(CCAUX) $(CLFLAG) $(FE)$(DOC2HTML) $(OD)doc2html$(OBJ)

$(DOC2RTF): $(SRC)doc2rtf.c
	$(CCAUX) $(FOO)doc2rtf$(OBJ) $(CO) $(SRC)doc2rtf.c
	$(CCAUX) $(CLFLAG) $(FE)$(DOC2RTF) $(OD)doc2rtf$(OBJ)

$(DOC2HHP): $(SRC)doc2hhp.c
	$(CCAUX) $(FOO)doc2hhp$(OBJ) $(CO) $(SRC)doc2hhp.c
	$(CCAUX) $(CLFLAG) $(FE)$(DOC2HHP) $(OD)doc2hhp$(OBJ)


#################################################################
# Common files

$(ECHOGSV): $(SRC)echogsv.c
	-mkdir $(OBJDIR)
	-mkdir $(BINDIR)
	$(CCAUX) $(FOO)echogsv$(OBJ) $(CO) $(SRC)echogsv.c
	$(CCAUX) $(CLFLAG) $(FE)$(ECHOGSV) $(OD)echogsv$(OBJ)

$(OD)gvccmd$(OBJ): $(SRC)gvccmd.c $(HDRS)
	$(COMP) $(FOO)gvccmd$(OBJ) $(CO) $(SRC)gvccmd.c

$(OD)gvcdisp$(OBJ): $(SRC)gvcdisp.c $(HDRS)
	$(COMP) $(FOO)gvcdisp$(OBJ) $(CO) $(SRC)gvcdisp.c

$(OD)gvcdll$(OBJ): $(SRC)gvcdll.c $(HDRS)
	$(COMP) $(FOO)gvcdll$(OBJ) $(CO) $(SRC)gvcdll.c

$(OD)cdll$(OBJ): $(SRC)cdll.c $(HDRS)
	$(COMP) $(FOO)cdll$(OBJ) $(CO) $(SRC)cdll.c

$(OD)cimg$(OBJ): $(SRC)cimg.c $(HDRS)
	$(COMP) $(FOO)cimg$(OBJ) $(CO) $(SRC)cimg.c

$(OD)cview$(OBJ): $(SRC)cview.c $(HDRS)
	$(COMP) $(FOO)cview$(OBJ) $(CO) $(SRC)cview.c

$(OD)dscparse$(OBJ): $(SRC)dscparse.c $(SRC)dscparse.h
	$(COMP) $(FOO)dscparse$(OBJ) $(CO) $(SRC)dscparse.c

$(OD)dscutil$(OBJ): $(SRC)dscutil.c $(SRC)dscparse.h
	$(COMP) $(FOO)dscutil$(OBJ) $(CO) $(SRC)dscutil.c

$(OD)gvcbeta$(OBJ): $(SRC)gvcbeta.c $(SRC)gvcbeta.h $(HDRS)
	$(COMP) $(FOO)gvcbeta$(OBJ) $(CO) $(SRC)gvcbeta.c

$(OD)gvcedit$(OBJ): $(SRC)gvcedit.c $(HDRS)
	$(COMP) $(FOO)gvcedit$(OBJ) $(CO) $(SRC)gvcedit.c

$(OD)gvceps$(OBJ): $(SRC)gvceps.c $(SRC)gvceps.h $(HDRS)
	$(COMP) $(FOO)gvceps$(OBJ) $(CO) $(SRC)gvceps.c

$(OD)gvcfile$(OBJ): $(SRC)gvcfile.c $(HDRS)
	$(COMP) -DSTDIO $(FOO)gvcfile$(OBJ) $(CO) $(SRC)gvcfile.c

$(OD)gvcinit$(OBJ): $(SRC)gvcinit.c $(HDRS)
	$(COMP) $(FOO)gvcinit$(OBJ) $(CO) $(SRC)gvcinit.c

$(OD)gvcmeas$(OBJ): $(SRC)gvcmeas.c $(HDRS)
	$(COMP) $(FOO)gvcmeas$(OBJ) $(CO) $(SRC)gvcmeas.c

$(OD)gvcmeas2$(OBJ): $(SRC)gvcmeas2.c $(HDRS)
	$(COMP) $(FOO)gvcmeas2$(OBJ) $(CO) $(SRC)gvcmeas2.c

$(OD)gvcmfile$(OBJ): $(SRC)gvcmfile.c $(SRC)gvcfile.h
	$(COMP) $(FOO)gvcmfile$(OBJ) $(CO) $(SRC)gvcmfile.c

$(OD)gvcmisc$(OBJ): $(SRC)gvcmisc.c $(HDRS)
	$(COMP) $(FOO)gvcmisc$(OBJ) $(CO) $(SRC)gvcmisc.c

$(OD)gvcpdf$(OBJ): $(SRC)gvcpdf.c $(HDRS)
	$(COMP) $(FOO)gvcpdf$(OBJ) $(CO) $(SRC)gvcpdf.c

$(OD)gvcprn$(OBJ): $(SRC)gvcprn.c $(HDRS)
	$(COMP) $(FOO)gvcprn$(OBJ) $(CO) $(SRC)gvcprn.c

$(OD)gvcprf$(OBJ): $(SRC)gvcprf.c $(HDRS)
	$(COMP) $(FOO)gvcprf$(OBJ) $(CO) $(SRC)gvcprf.c

$(OD)gvctext$(OBJ): $(SRC)gvctext.c $(HDRS)
	$(COMP) $(FOO)gvctext$(OBJ) $(CO) $(SRC)gvctext.c

$(OD)gvcreg$(OBJ): $(SRC)gvcreg.c $(HDRS)
	$(COMP) $(FOO)gvcreg$(OBJ) $(CO) $(SRC)gvcreg.c

$(OD)gvcbetaa$(OBJ): $(SRC)gvcbeta.c $(SRC)gvcbeta.h $(HDRS)
	$(COMP) $(FOO)gvcbetaa$(OBJ) $(CO) $(SRC)gvcbeta.c


#################################################################
# epstool has been removed from this version of GSview
# Please use the later epstool 3.x



#################################################################
# Cleanup

commonclean:
	-$(RM) $(CLEAN1)
	-$(RM) $(CLEAN2)
	-$(RM) $(CLEAN3)

