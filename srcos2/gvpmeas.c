/* Copyright (C) 1998, Ghostgum Software Pty Ltd.  All rights reserved.
  
  This file is part of GSview.
  
  This program is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  Refer to the GSview Free Public Licence 
  (the "Licence") for full details.
  
  Every copy of GSview must include a copy of the Licence, normally in a 
  plain ASCII text file named LICENCE.  The Licence grants you the right 
  to copy, modify and redistribute GSview, but only under certain conditions 
  described in the Licence.  Among other things, the Licence requires that 
  the copyright notice and this notice be preserved on all copies.
*/

/* gvpmeas.c */
/* Measure lengths on display */

#include "gvpm.h"
#include <math.h>

#define GetDlgItemText(hwnd, id, str, len) \
	WinQueryWindowText(WinWindowFromID((hwnd), (id)), (len), (str))
#define SetDlgItemText(hwnd, id, str) \
	WinSetWindowText( WinWindowFromID((hwnd), (id)), (str))

/* if error is TRUE, an error message can be shown */
BOOL dialog_get_float_error(HWND hwnd, int field, float *fres, 
   BOOL error)
{
  float ftemp ;
  char buf[64] ;
  GetDlgItemText(hwnd, field, (PBYTE)buf, sizeof(buf)-1) ;
  if ((1 > sscanf(buf,"%g",&ftemp)) && error)
  {
    HWND errfld = WinWindowFromID(hwnd, field);
    gserror(IDS_INVALIDNUMBER, NULL, MB_ICONEXCLAMATION, SOUND_NONUMBER) ;
    WinSetFocus(HWND_DESKTOP, errfld) ;
/* Replace 100 with the end */
    WinSendMsg( errfld, EM_SETSEL, 
        MPFROM2SHORT(0, 100), MPFROMLONG(0) );
  }
  else
  {
    *fres = ftemp ;
    return TRUE ;
  }
  return FALSE;
}

/*  transformation calculator dialog procedure  */

/*  dialog controls to gray/ungray based on "custom" radio button */
static int calccustom[] =
{
  IDC_CALCCTMA, IDC_CALCCTMB, IDC_CALCCTMC, IDC_CALCCTMD,
  IDC_CALCCTMTX, IDC_CALCCTMTY,
  /* IDC_CALCNAME, */ IDC_CALCTX, IDC_CALCTY, IDC_CALCTR,
  IDC_CALCROTTH, IDC_CALCRO, IDC_CALCSCX, IDC_CALCSCY, IDC_CALCSC,
  IDC_CALCINI, IDC_CALCINV } ;

void calc_enable_custom(HWND hwnd, BOOL enab)
{
  unsigned int ix ;
  for (ix = 0 ; ix < (sizeof(calccustom)/sizeof(int)); ix++)
    WinEnableWindow(WinWindowFromID(hwnd, calccustom[ix]), enab);
}

MRESULT EXPENTRY 
CalcDlgProc(HWND hwnd, ULONG mess, MPARAM mp1, MPARAM mp2)
{
  static MATRIX tempmtx ;
  static int tempunit ;
  switch(mess) {
  case WM_INITDLG:
    tempmtx =  option.ctm;
    tempunit = option.measure.unit;
    update_dialog_ctm(hwnd, &tempmtx);
    dialog_put_float(hwnd, IDC_CALCTX, option.measure.tx);
    dialog_put_float(hwnd, IDC_CALCTY, option.measure.ty);
    dialog_put_float(hwnd, IDC_CALCROTTH, option.measure.rotate);
    dialog_put_float(hwnd, IDC_CALCSCX, option.measure.sx);
    dialog_put_float(hwnd, IDC_CALCSCY, option.measure.sy);
   switch(tempunit)
    {
    case IDM_UNITPT:
      calc_enable_custom(hwnd, FALSE) ;
      WinSendMsg( WinWindowFromID(hwnd, IDC_CALCPTS),
		BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
      break ;
    case IDM_UNITMM:
      calc_enable_custom(hwnd, FALSE) ;
      WinSendMsg( WinWindowFromID(hwnd, IDC_CALCMM),
		BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
      break ;
    case IDM_UNITINCH:
      calc_enable_custom(hwnd, FALSE) ;
      WinSendMsg( WinWindowFromID(hwnd, IDC_CALCIN),
		BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
      break ;
    case IDM_UNITCUSTOM:
      calc_enable_custom(hwnd, TRUE) ;
      WinSendMsg( WinWindowFromID(hwnd, IDC_CALCPTS),
		BM_SETCHECK, MPFROMLONG(0), MPFROMLONG(0));
      WinSendMsg( WinWindowFromID(hwnd, IDC_CALCMM),
		BM_SETCHECK, MPFROMLONG(0), MPFROMLONG(0));
      WinSendMsg( WinWindowFromID(hwnd, IDC_CALCIN),
		BM_SETCHECK, MPFROMLONG(0), MPFROMLONG(0));
      WinSendMsg( WinWindowFromID(hwnd, IDC_CALCCUST),
		BM_SETCHECK, MPFROMLONG(1), MPFROMLONG(0));
      break ;
    }
    break;
    
  case WM_CONTROL:
    switch (SHORT2FROMMP(mp1)) {
	case BN_CLICKED:
	  return (MRESULT)calc_command(hwnd, SHORT1FROMMP(mp1), &tempmtx, &tempunit);
    }
    break;
  case WM_COMMAND:
    switch(SHORT1FROMMP(mp1)) {
    case DID_OK:
      if (!dialog_get_ctm(hwnd, &tempmtx, TRUE))
	return (MRESULT)TRUE;
      option.ctm = tempmtx;
      option.measure.unit = tempunit;
      dialog_get_float_error(hwnd, IDC_CALCTX, 
	&option.measure.tx, FALSE);
      dialog_get_float_error(hwnd, IDC_CALCTY, 
	&option.measure.ty, FALSE);
      dialog_get_float_error(hwnd, IDC_CALCROTTH, 
	&option.measure.rotate, FALSE);
      dialog_get_float_error(hwnd, IDC_CALCSCX, 
	&option.measure.sx, FALSE);
      dialog_get_float_error(hwnd, IDC_CALCSCY, 
	&option.measure.sy, FALSE);
      WinDismissDlg(hwnd, TRUE);
      return (MRESULT)TRUE;
    case DID_CANCEL:
      WinDismissDlg(hwnd, FALSE);
      return (MRESULT)TRUE;
    case ID_HELP:
      nHelpTopic = IDS_TOPICMEASURE;
      get_help();
      return (MRESULT)TRUE;
    default:
      return (MRESULT)calc_command(hwnd, SHORT1FROMMP(mp1), &tempmtx, &tempunit);
    }
    break;
  }
  return WinDefDlgProc(hwnd, mess, mp1, mp2);
}

void
show_calc_dialog(void)
{
   WinDlgBox(HWND_DESKTOP, hwnd_measure, CalcDlgProc, hlanguage, IDD_CALC, NULL);
}

/* What should we return? Does TRUE mean we processed it? */

MRESULT EXPENTRY 
MeasureDlgProc(HWND hwnd, ULONG mess, MPARAM mp1, MPARAM mp2)
{
    switch(mess) {
        case WM_INITDLG:
	    hwnd_measure = hwnd;
	    measure_dialog_unit();
	    measure_setpoint(0.0, 0.0);
	    break;
	case WM_COMMAND:
	    switch(SHORT1FROMMP(mp1)) {
		case IDM_MEASURE_CALC:
		    show_calc_dialog();
		    measure_dialog_unit();
		    measure_update_last();
		    return (MRESULT)FALSE;
		case IDCANCEL:
		    WinDestroyWindow(hwnd);
		    hwnd_measure = 0;
		    return (MRESULT)TRUE;
	    }
	    break;
	case WM_CLOSE:
	    WinDestroyWindow(hwnd);
	    return (MRESULT)TRUE;
	case WM_DESTROY:
	    hwnd_measure = 0;
	    break;
    }
    return WinDefDlgProc(hwnd, mess, mp1, mp2);
}



void 
measure_show(void)
{
    if (hwnd_measure) {
	WinSetWindowPos(hwnd_measure, HWND_TOP, 0, 0, 0, 0, 
	    SWP_ZORDER | SWP_ACTIVATE);
	return;
    }

    hwnd_measure = WinLoadDlg(HWND_DESKTOP, hwnd_frame, MeasureDlgProc, 
	hlanguage, IDD_MEASURE, NULL);
    WinSetWindowPos(hwnd_measure, HWND_TOP, 0, 0, 0, 0, 
	SWP_ZORDER | SWP_ACTIVATE);

}

