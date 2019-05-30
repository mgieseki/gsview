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

/* gvcmeas.c */
/* Measure lengths on display */

#include "gvc.h"
#include <math.h>

float fthreshold = (float)1e-12;

#define radians(x) (2. * 3.14159265358979 * ((x)/ 360.))

/* Rotate a matrix, possibly in place.  The angle is in degrees. */
/* from Ghostscript */
int
matrix_rotate(const MATRIX *pm, float ang, MATRIX *pmr)
{
  double mxx, mxy;
  double asin = sin(radians(ang)) ;
  double acos = cos(radians(ang)) ;
  mxx = pm->xx, mxy = pm->xy;
  pmr->xx = (float)(acos * mxx + asin * pm->yx);
  pmr->xy = (float)(acos * mxy + asin * pm->yy);
  pmr->yx = (float)(acos * pm->yx - asin * mxx);
  pmr->yy = (float)(acos * pm->yy - asin * mxy);
  if ( pmr != pm )
  {
    pmr->tx = pm->tx;
    pmr->ty = pm->ty;
  }
  return 0;
}


#define is_fzero(f) ((f) == 0.0)
#define is_fzero2(f1,f2) ((f1) == 0.0 && (f2) == 0.0)
#define is_xxyy(pmat) is_fzero2((pmat)->xy, (pmat)->yx)
#define is_xyyx(pmat) is_fzero2((pmat)->xx, (pmat)->yy)

/* Invert a matrix.  Return -1 if not invertible. */
/* from Ghostscript */
int
matrix_invert(const MATRIX *pm, MATRIX *pmr)
{     /* We have to be careful about fetch/store order, */
      /* because pm might be the same as pmr. */
      if ( is_xxyy(pm) )
      {       if ( is_fzero(pm->xx) || is_fzero(pm->yy) )
                return -1 ;
              pmr->tx = (float)(- (pmr->xx = (float)(1.0 / pm->xx)) * pm->tx);
              pmr->xy = 0.0;
              pmr->yx = 0.0;
              pmr->ty = (float)(- (pmr->yy = (float)(1.0 / pm->yy)) * pm->ty);
      }
      else
      {       double det = pm->xx * pm->yy - pm->xy * pm->yx;
              double mxx = pm->xx, mtx = pm->tx;
              if ( det == 0 )
                return -1 ;
              pmr->xx = (float)(pm->yy / det);
              pmr->xy = (float)(- pm->xy / det);
              pmr->yx = (float)(- pm->yx / det);
              pmr->yy = (float)(mxx / det);    /* xx is already changed */
              pmr->tx = (float)(- (mtx * pmr->xx + pm->ty * pmr->yx));
              pmr->ty = (float)(- (mtx * pmr->xy + pm->ty * pmr->yy)); /* tx ditto */
      }
      return 0;
}

void 
matrix_scale(const MATRIX *pm, float xscale, float yscale, MATRIX *pmr)
{     
    pmr->xx = pm->xx * xscale;
    pmr->xy = pm->xy * xscale;
    pmr->yx = pm->yx * yscale;
    pmr->yy = pm->yy * yscale;
    if (pmr != pm) {
	pmr->tx = pm->tx;
	pmr->ty = pm->ty;
    }
}

void
matrix_translate(const MATRIX *pm, float xoffset, float yoffset, MATRIX *pmr)
{     
    if (pmr != pm) {
	pmr->xx = pm->xx;
	pmr->xy = pm->xy;
	pmr->yx = pm->yx;
	pmr->yy = pm->yy;
    }
    pmr->tx = xoffset*pm->xx + yoffset*pm->yx + pm->tx;
    pmr->ty = xoffset*pm->xy + yoffset*pm->yy + pm->ty;
}


void matrix_set_unit(MATRIX *matrix, int unit)
{
  switch (unit)
  {
    case IDM_UNITPT:
	/*  identity matrix  */
	matrix->xx = 1 ;  matrix->xy = 0 ;
	matrix->yx = 0 ;  matrix->yy = 1 ;
	matrix->tx = 0 ;  matrix->ty = 0 ;
	break ;
    case IDM_UNITMM:
	matrix->xx = (float)(1./72*25.4) ;  matrix->xy = 0 ;
	matrix->yx = 0 ;  matrix->yy = (float)(1./72*25.4) ;
	matrix->tx = 0 ;  matrix->ty = 0 ;
	break ;
    case IDM_UNITINCH:
	matrix->xx = (float)(1./72.) ;  matrix->xy = 0 ;
	matrix->yx = 0 ;  matrix->yy = (float)(1./72.) ;
	matrix->tx = 0 ;  matrix->ty = 0 ;
	break ;
  }
}

void
measure_transform_point(float x, float y, float *px, float *py)
{
    *px = x * option.ctm.xx + y * option.ctm.yx + option.ctm.tx;
    *py = x * option.ctm.xy + y * option.ctm.yy + option.ctm.ty;
}


#define MEASURE_SECTION "Measure"
void
read_measure_profile(PROFILE *prf)
{
int i;
float fx;
char profile[MAXSTR];
const char *section = MEASURE_SECTION;
    /* Calculator dialog settings  */
    profile_read_string(prf, section, "XX",  "1", profile, sizeof(profile)) ;
    if (sscanf(profile,"%g", &fx) == 1)
	  option.ctm.xx = fx ;
    profile_read_string(prf, section, "XY",  "0", profile, sizeof(profile)) ;
    if (sscanf(profile,"%g", &fx) == 1)
	  option.ctm.xy = fx ;
    profile_read_string(prf, section, "YX",  "0", profile, sizeof(profile)) ;
    if (sscanf(profile,"%g", &fx) == 1)
	  option.ctm.yx = fx ;
    profile_read_string(prf, section, "YY",  "1", profile, sizeof(profile)) ;
    if (sscanf(profile,"%g", &fx) == 1)
	  option.ctm.yy = fx ;
    profile_read_string(prf, section, "TX",  "0", profile, sizeof(profile)) ;
    if (sscanf(profile,"%g", &fx) == 1)
	  option.ctm.tx = fx ;
    profile_read_string(prf, section, "TY",  "0", profile, sizeof(profile)) ;
    if (sscanf(profile,"%g", &fx) == 1)
	  option.ctm.ty = fx ;

    profile_read_string(prf, section, "Unit", "0", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.measure.unit = i + IDM_UNITPT;

    /* Remember these to make it easier to enter similar transforms */
    profile_read_string(prf, section, "TranslateX",  "0", profile, 
	sizeof(profile)) ;
    if (sscanf(profile,"%g", &fx) == 1)
	option.measure.tx = fx ;
    profile_read_string(prf, section, "TranslateY",  "0", profile, 
	sizeof(profile)) ;
    if (sscanf(profile,"%g", &fx) == 1)
	option.measure.ty = fx ;
    profile_read_string(prf, section, "Rotate",  "0", profile, 
	sizeof(profile)) ;
    if (sscanf(profile,"%g", &fx) == 1)
	option.measure.rotate = fx ;
    profile_read_string(prf, section, "ScaleX",  "1", profile, 
	sizeof(profile)) ;
    if (sscanf(profile,"%g", &fx) == 1)
	option.measure.sx = fx ;
    if (fabs(option.measure.sx) < fthreshold)
	option.measure.sx = 1.0;
    profile_read_string(prf, section, "ScaleY",  "1", profile, 
	sizeof(profile)) ;
    if (sscanf(profile,"%g", &fx) == 1)
	option.measure.sy = fx ;
    if (fabs(option.measure.sy) < fthreshold)
	option.measure.sy = 1.0;
}

void
write_measure_profile(PROFILE *prf)
{
const char *section = MEASURE_SECTION;
char profile[MAXSTR];
    sprintf(profile, "%g", option.ctm.xx);
    profile_write_string(prf, section, "XX", profile);
    sprintf(profile, "%g", option.ctm.xy);
    profile_write_string(prf, section, "XY", profile);
    sprintf(profile, "%g", option.ctm.yx);
    profile_write_string(prf, section, "YX", profile);
    sprintf(profile, "%g", option.ctm.yy);
    profile_write_string(prf, section, "YY", profile);
    sprintf(profile, "%g", option.ctm.tx);
    profile_write_string(prf, section, "TX", profile);
    sprintf(profile, "%g", option.ctm.ty);
    profile_write_string(prf, section, "TY", profile);

    sprintf(profile, "%d", option.measure.unit - IDM_UNITPT);
    profile_write_string(prf, section, "Unit", profile);

    sprintf(profile, "%g", option.measure.tx);
    profile_write_string(prf, section, "TranslateX", profile);
    sprintf(profile, "%g", option.measure.ty);
    profile_write_string(prf, section, "TranslateY", profile);
    sprintf(profile, "%g", option.measure.rotate);
    profile_write_string(prf, section, "Rotate", profile);
    sprintf(profile, "%g", option.measure.sx);
    profile_write_string(prf, section, "ScaleX", profile);
    sprintf(profile, "%g", option.measure.sx);
    profile_write_string(prf, section, "ScaleY", profile);
}

