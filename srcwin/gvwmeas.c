/* Copyright (C) 1998-2005, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvwmeas.c */
/* Measure lengths on display */

#include "gvwin.h"
#include <math.h>


/* if error is TRUE, an error message can be shown */
BOOL dialog_get_float_error(HWND hwnd, int field, float *fres, 
   BOOL error)
{
  float ftemp ;
  char buf[64] ;
  GetDlgItemTextA(hwnd, field, buf, sizeof(buf)-1) ;
  if ((1 > sscanf(buf,"%g",&ftemp)) && error)
  {
    HWND errfld = GetDlgItem(hwnd, field) ;
    gserror(IDS_INVALIDNUMBER, NULL, MB_ICONEXCLAMATION, SOUND_NONUMBER) ;
    SetFocus(errfld) ;
    SendMessage(errfld, EM_SETSEL, (WPARAM)0, (LPARAM) -1) ;
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
  int ix ;
  for (ix = 0 ; ix < (sizeof(calccustom)/sizeof(int)); ix++)
    EnableWindow(GetDlgItem(hwnd, calccustom[ix]), enab) ;
}

#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
CalcDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  static MATRIX tempmtx ;
  static int tempunit ;

  switch(message) {
  case WM_INITDIALOG:
    tempmtx =  option.ctm;
    tempunit = option.measure.unit;
    update_dialog_ctm(hDlg, &tempmtx);
    dialog_put_float(hDlg, IDC_CALCTX, option.measure.tx);
    dialog_put_float(hDlg, IDC_CALCTY, option.measure.ty);
    dialog_put_float(hDlg, IDC_CALCROTTH, option.measure.rotate);
    dialog_put_float(hDlg, IDC_CALCSCX, option.measure.sx);
    dialog_put_float(hDlg, IDC_CALCSCY, option.measure.sy);
   switch(tempunit)
    {
    case IDM_UNITCUSTOM:
      calc_enable_custom(hDlg, TRUE) ;
      CheckRadioButton(hDlg, IDC_CALCPTS, IDC_CALCCUST, IDC_CALCCUST) ;
      break ;
    case IDM_UNITPT:
      calc_enable_custom(hDlg, FALSE) ;
      CheckRadioButton(hDlg, IDC_CALCPTS, IDC_CALCCUST, IDC_CALCPTS) ;
      break ;
    case IDM_UNITMM:
      calc_enable_custom(hDlg, FALSE) ;
      CheckRadioButton(hDlg, IDC_CALCPTS, IDC_CALCCUST, IDC_CALCMM) ;
      break ;
    case IDM_UNITINCH:
      calc_enable_custom(hDlg, FALSE) ;
      CheckRadioButton(hDlg, IDC_CALCPTS, IDC_CALCCUST, IDC_CALCIN) ;
      break ;
    }
    return(TRUE) ;
    
  case WM_COMMAND:
    switch(LOWORD(wParam)) {
    case IDOK:
      if (!dialog_get_ctm(hDlg, &tempmtx, TRUE))
	return(TRUE);
      option.ctm = tempmtx;
      option.measure.unit = tempunit;
      dialog_get_float_error(hDlg, IDC_CALCTX, 
	&option.measure.tx, FALSE);
      dialog_get_float_error(hDlg, IDC_CALCTY, 
	&option.measure.ty, FALSE);
      dialog_get_float_error(hDlg, IDC_CALCROTTH, 
	&option.measure.rotate, FALSE);
      dialog_get_float_error(hDlg, IDC_CALCSCX, 
	&option.measure.sx, FALSE);
      dialog_get_float_error(hDlg, IDC_CALCSCY, 
	&option.measure.sy, FALSE);
      EndDialog(hDlg, TRUE);
      return(TRUE);
    case IDCANCEL:
      EndDialog(hDlg, TRUE);
      return(TRUE);
    case ID_HELP:
      nHelpTopic = IDS_TOPICMEASURE;
      get_help();
      return(FALSE);

    default:
      return calc_command(hDlg, LOWORD(wParam), &tempmtx, &tempunit);
      
    }
  default:
    return(FALSE);
  }
}

void
show_calc_dialog(void)
{
    DialogBoxParamL(hlanguage, MAKEINTRESOURCE(IDD_CALC), hwndimg, 
	CalcDlgProc, (LPARAM)NULL);
}


#ifdef __BORLANDC__
#pragma argsused
#endif
DLGRETURN CALLBACK _export
MeasureDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
float x, y;
    switch(message) {
	case WM_INITDIALOG:
	    hwnd_measure = hDlg;
	    measure_dialog_unit();
	    measure_setpoint(0.0, 0.0);
	    return TRUE;
	case WM_COMMAND:
	    switch(LOWORD(wParam)) {
		case IDM_MEASURE_CALC:
		    show_calc_dialog();
		    measure_dialog_unit();
		    measure_update_last();
		    return FALSE;
		case IDC_MEASURE_SETPOINT:
		    if (!get_cursorpos(&x, &y)) {
			DestroyWindow(hDlg);
			hwnd_measure = 0;
		    }
		    measure_setpoint(x, y);
		    return FALSE;
		case IDCANCEL:
		    DestroyWindow(hDlg);
		    hwnd_measure = 0;
		    return TRUE;
	    }
	case WM_CLOSE:
	    DestroyWindow(hDlg);
	    hwnd_measure = 0;
	    return TRUE;
    }
    return FALSE;
}


void 
measure_show(void)
{
    if (hwnd_measure) {
	BringWindowToTop(hwnd_measure);
	return;
    }

    /* display dialog box */
    hwnd_measure = CreateDialogParamL(hlanguage, MAKEINTRESOURCE(IDD_MEASURE), 
	hwndimg, MeasureDlgProc, (LPARAM)NULL);

    /* Don't bother to clean up 16-bit thunk */
}

