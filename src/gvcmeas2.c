/* Copyright (C) 1998-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
  This file is part of GSview.
   
  This program is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  Refer to the GSview Licence (the "Licence") 
  for full details.
   
  Every copy of GSview must include a copy of the Licence, normally in a 
  plain ASCII text file named LICENCE.  The Licence grants you the right 
  to copy, modify and redistribute GSview, but only under certain conditions 
  described in the Licence.  Among other things, the Licence requires that 
  the copyright notice and this notice be preserved on all copies.
*/

/* gvcmeas2.cpp  - common measure code for Win and PM */

#include "gvc.h"
#include <math.h>

float measure_lastx;
float measure_lasty;

static const char measure_fmt[] = "%.5g";

static const char *ctmfmt = "%.5g" ;
extern float fthreshold;

void update_dialog_ctm(HWND hwnd, MATRIX *ctm)
{
  char buf[16] ;
  /* round small values to zero */
  if (fabs(ctm->xx) < fthreshold)
    ctm->xx = 0.0;
  if (fabs(ctm->xy) < fthreshold)
    ctm->xy = 0.0;
  if (fabs(ctm->yx) < fthreshold)
    ctm->yx = 0.0;
  if (fabs(ctm->yy) < fthreshold)
    ctm->yy = 0.0;
  if (fabs(ctm->tx) < fthreshold)
    ctm->tx = 0.0;
  if (fabs(ctm->ty) < fthreshold)
    ctm->ty = 0.0;

  sprintf(buf, ctmfmt, ctm->xx) ;
  SetDlgItemTextA(hwnd, IDC_CALCCTMA, buf) ;
  sprintf(buf, ctmfmt, ctm->xy) ;
  SetDlgItemTextA(hwnd, IDC_CALCCTMB, buf) ;
  sprintf(buf, ctmfmt, ctm->yx) ;
  SetDlgItemTextA(hwnd, IDC_CALCCTMC, buf) ;
  sprintf(buf, ctmfmt, ctm->yy) ;
  SetDlgItemTextA(hwnd, IDC_CALCCTMD, buf) ;
  sprintf(buf, ctmfmt, ctm->tx) ;
  SetDlgItemTextA(hwnd, IDC_CALCCTMTX, buf) ;
  sprintf(buf, ctmfmt, ctm->ty) ;
  SetDlgItemTextA(hwnd, IDC_CALCCTMTY, buf) ;
}


BOOL dialog_get_float(HWND hwnd, int field, float *fres)
{
  return dialog_get_float_error(hwnd, field, fres, TRUE);
}


void dialog_put_float(HWND hwnd, int field, float fx)
{
   char buf[64];
   sprintf(buf, ctmfmt, fx);
   SetDlgItemTextA(hwnd, field, buf);
}

BOOL dialog_get_ctm(HWND hwnd, MATRIX *ctm, BOOL error)
{
BOOL result = TRUE;
   if (result)
       result = dialog_get_float_error(hwnd, IDC_CALCCTMA, &ctm->xx, error);
   if (result)
       result = dialog_get_float_error(hwnd, IDC_CALCCTMB, &ctm->xy, error);
   if (result)
       result = dialog_get_float_error(hwnd, IDC_CALCCTMC, &ctm->yx, error);
   if (result)
       result = dialog_get_float_error(hwnd, IDC_CALCCTMD, &ctm->yy, error);
   if (result)
       result = dialog_get_float_error(hwnd, IDC_CALCCTMTX, &ctm->tx, error);
   if (result)
       result = dialog_get_float_error(hwnd, IDC_CALCCTMTY, &ctm->ty, error);
   return result;
}


BOOL
calc_command(HWND hwnd, int message, MATRIX *ctm, int *unit)
{
float xx, yy ;
  switch(message) {
    case IDC_CALCPTS:
      matrix_set_unit(ctm, *unit = IDM_UNITPT) ;
      update_dialog_ctm(hwnd, ctm) ;
      calc_enable_custom(hwnd, FALSE) ;
      *unit = IDM_UNITPT;
      return TRUE;
			  
    case IDC_CALCIN:
      matrix_set_unit(ctm, *unit = IDM_UNITINCH) ;
      update_dialog_ctm(hwnd, ctm) ;
      calc_enable_custom(hwnd, FALSE) ;
      *unit = IDM_UNITINCH;
      return TRUE;
			  
    case IDC_CALCMM:
      matrix_set_unit(ctm, *unit = IDM_UNITMM) ;
      update_dialog_ctm(hwnd, ctm) ;
      calc_enable_custom(hwnd, FALSE) ;
      *unit = IDM_UNITMM;
      return TRUE;
			  
    case IDC_CALCCUST:
      *unit = IDM_UNITCUSTOM ;
      calc_enable_custom(hwnd, TRUE) ;
      return TRUE;

    case IDC_CALCRO:
      if (dialog_get_ctm(hwnd, ctm, TRUE) 
	&& dialog_get_float(hwnd, IDC_CALCROTTH, &xx))
      {
	matrix_rotate(ctm, xx, ctm) ;
	update_dialog_ctm(hwnd, ctm) ;
      }
      return TRUE;

    case IDC_CALCINI:
      matrix_set_unit(ctm, IDM_UNITPT) ;
      update_dialog_ctm(hwnd, ctm) ;
      return TRUE;
      
    case IDC_CALCINV:
      if (!dialog_get_ctm(hwnd, ctm, TRUE))
	return TRUE;
      if (-1 == matrix_invert(ctm, ctm))
	gserror(IDS_CANTINVERT, NULL, MB_ICONEXCLAMATION, SOUND_NONUMBER) ;
      else
	update_dialog_ctm(hwnd, ctm) ;
      return TRUE;
    case IDC_CALCTR:
      if (dialog_get_ctm(hwnd, ctm, TRUE) 
	  && dialog_get_float(hwnd, IDC_CALCTX, &xx)
	  && dialog_get_float(hwnd, IDC_CALCTY, &yy))
	{
	  matrix_translate(ctm, xx, yy, ctm);
	  update_dialog_ctm(hwnd, ctm) ;
	}
      return TRUE;
    case IDC_CALCSC:
      if (dialog_get_ctm(hwnd, ctm, TRUE) 
          && dialog_get_float(hwnd, IDC_CALCSCX, &xx)
	  && dialog_get_float(hwnd, IDC_CALCSCY, &yy))
	{
	  matrix_scale(ctm, xx, yy, ctm);
	  update_dialog_ctm(hwnd, ctm) ;
	}
      return TRUE;
  }
  return FALSE;
}

void
measure_update_last(void)
{
char buf[64];
float thisx, thisy;
    measure_transform_point(measure_lastx, measure_lasty, &thisx, &thisy);
    if (fabs(thisx) < fthreshold)
        thisx = 0.0;
    if (fabs(thisy) < fthreshold)
        thisy = 0.0;
    sprintf(buf, measure_fmt, thisx);
    SetDlgItemTextA(hwnd_measure, IDC_MEASURE_LASTX, buf);
    sprintf(buf, measure_fmt, thisy);
    SetDlgItemTextA(hwnd_measure, IDC_MEASURE_LASTY, buf);
}


/* This is called on mouse click */
void
measure_setpoint(float x, float y)
{
    if (hwnd_measure == (HWND)NULL)
	return;
    measure_lastx = x;
    measure_lasty = y;
    measure_update_last();
    measure_paint(x, y);
}

#define degrees(x) (x * 180.0 / 3.14159265358979)

/* This is called from cursorpos_paint */
void
measure_paint(float x, float y)
{
float lastx, lasty;
float thisx, thisy;
float deltax, deltay;
float radius, angle;
char buf[64];
    if (hwnd_measure == (HWND)NULL)
	return;
    measure_transform_point(x, y, &thisx, &thisy);
    if (fabs(thisx) < fthreshold)
        thisx = 0.0;
    if (fabs(thisy) < fthreshold)
        thisy = 0.0;
    measure_transform_point(measure_lastx, measure_lasty, &lastx, &lasty);
    if (fabs(lastx) < fthreshold)
        lastx = 0.0;
    if (fabs(lasty) < fthreshold)
        lasty = 0.0;
    deltax = thisx - lastx;
    deltay = thisy - lasty;
    if (fabs(deltax) < fthreshold)
        deltax = 0.0;
    if (fabs(deltay) < fthreshold)
        deltay = 0.0;
    radius = (float)sqrt(deltax*deltax + deltay*deltay);
    if ((deltax == 0.0) && (deltay == 0.0))
	angle = 0.0;
    else
	angle = (float)degrees( atan2(deltay, deltax) );

    sprintf(buf, measure_fmt, thisx);
    SetDlgItemTextA(hwnd_measure, IDC_MEASURE_X, buf);
    sprintf(buf, measure_fmt, thisy);
    SetDlgItemTextA(hwnd_measure, IDC_MEASURE_Y, buf);
    sprintf(buf, measure_fmt, deltax);
    SetDlgItemTextA(hwnd_measure, IDC_MEASURE_DELTAX, buf);
    sprintf(buf, measure_fmt, deltay);
    SetDlgItemTextA(hwnd_measure, IDC_MEASURE_DELTAY, buf);
    sprintf(buf, measure_fmt, radius);
    SetDlgItemTextA(hwnd_measure, IDC_MEASURE_RADIUS, buf);
    sprintf(buf, "%.2f", angle);
    SetDlgItemTextA(hwnd_measure, IDC_MEASURE_ANGLE, buf);
}


void
measure_dialog_unit(void)
{
char buf[MAXSTR];
    load_string_a(IDS_UNITNAME + option.measure.unit - IDM_UNITPT, 
	    buf, sizeof(buf));
    SetDlgItemTextA(hwnd_measure, IDC_MEASURE_UNIT, buf);
}

