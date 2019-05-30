/* Copyright (C) 1995-1998, Digital Equipment Corporation.    */
/* All rights reserved.                                       */
/* See the file pstotext.txt for a full description.          */
/* Last modified on Fri Jan 09 21:19:00 AEST 2004 by rjl   */
/*      modified on Fri Jan 09 08:21:00 AEST 2004 by rjl       */
/*      modified on Wed Oct 28 08:42:15 PST 1998 by mcjones   */
/*      modified on Sun Jul 28 00:00:00 UTC 1996 by rjl       */

/* Modifications by rjl
 *   Fixed compiler warnings
 */

/* This module is based on OCR_PS.m3, a module of the Virtual Paper
   project at the DEC Systems Research Center:
   http://www.research.digital.com/SRC/virtualpaper/ */

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "ptotdll.h"

#ifndef NULL
#define NULL 0
#endif

#define BOOLEAN int
#define FALSE 0
#define TRUE 1

#define MIN(a,b) ((a)<=(b)?(a):(b))
#define MAX(a,b) ((b)<=(a)?(a):(b))

/* Character encoding.  Each element of the QE directive produced by
   ocr.ps is either an index in the StandardGlyph array, or is
   "NonstandardGlyph" (indicating the corresponding entry in the font's
   encoding specifies some nonstandard glyph). */

typedef unsigned GlyphIndex;
#define NonstandardGlyph 9999

#define UnknownChar '#' /* substitute for nonstandard glyph */

/* The first 256 entries in StandardGlyphs correspond to ISOLatin1;
   the next 28 entries correspond to characters not in ISOLatin1, but
   defined in the standard /Times-Roman font. */

#define LastISOLatin1 255

#define FIRSTSpecialGlyphs (LastISOLatin1+1)
#define LASTSpecialGlyphs (LastISOLatin1+28)
static const char *SpecialGlyphs[] = {
    "''",    /* quotedblright */
    "S\237", /* Scaron */
    "+",     /* dagger */
    "<",     /* guilsinglleft */
    "Z\237", /* Zcaron */
    "#",     /* daggerdbl */
    "L/",    /* Lslash */
    "...",   /* ellipsis */
    ">",     /* guilsinglright */
    "oe",    /* oe */
    "fi",    /* fi */
    ".",     /* bullet */
    "o/oo",  /* perthousand */
    "''",    /* quotedblbase */
    "--",    /* endash */
    "---",   /* emdash */
    "^TM",   /* trademark */
    "f",     /* florin */
    "l/",    /* lslash */
    "s\237", /* scaron */
    "Y\250", /* Ydieresis */
    "fl",    /* fl */
    "/",     /* fraction */
    "``",    /* quotedblleft */
    "'",     /* quotesinglbase */
    "'",     /* quotesingle */
    "z\237", /* zcaron */
    "OE"     /* OE */
  };

/* The next 256 entries correspond to the self-named glyphs used in
   Type 3 fonts from dvips: "\000", ..., "\377":  */

#define FirstDvips (LASTSpecialGlyphs+1)
#define LastDvips  (FirstDvips+256-1)

/* The next 512 entries correspond to glyph names used in Microsoft
   TrueType fonts: "G00", ..., "Gff" and "G00", ..., "GFF", which
   in both cases correspond to ISOLatin1 with some extensions. */

#define FirstTT1 (LastDvips+1)
#define LastTT1 (FirstTT1+256-1)
#define FirstTT2 (LastTT1+1)
#define LastTT2 (FirstTT2+256-1)
#define FirstOldDvips (LastTT2+1)
#define LastOldDvips (FirstOldDvips+128-1) /* note only 128 */

#define FIRSTTTSpecialGlyphs (FirstTT1+130)
#define LASTTTSpecialGlyphs (FirstTT1+159)
static const char *TTSpecialGlyphs[] = {
    "'",     /* quotesinglbase */
    "f",     /* florin */
    "''",    /* quotdblbase */
    "...",   /* ellipsis */
    "+",     /* dagger */
    "#",     /* daggerdbl */
    "\223",  /* circumflex */
    "o/oo",  /* perthousand */
    "S\237", /* Scaron */
    "<",     /* guilsinglleft */
    "OE",    /* OE */
    "#",     /* <undefined> */
    "#",     /* <undefined> */
    "#",     /* <undefined> */
    "#",     /* <undefined> */
    "`",     /* ISOLatin1: quoteleft */
    "'",     /* ISOLatin1: quoteright */
    "``",    /* quotedblleft */
    "''",    /* quotedblright */
    ".",     /* bullet */
    "--",    /* endash */
    "---",   /* emdash */
    "~",     /* ISOLatin1: tilde */
    "^TM",   /* trademark */
    "s\237", /* scaron */
    ">",     /* guilsinglright */
    "oe",    /* oe */
    "#",     /* <undefined> */
    "#",     /* <undefined> */
    "Y\250"  /* Ydieresis" */
  };

#define FIRSTDvipsGlyphs FirstDvips
#define LASTDvipsGlyphs (FirstDvips+127)
static const char *DvipsGlyphs[] = {
  /* 00x */
    "\\Gamma", "\\Delta", "\\Theta", "\\Lambda",
    "\\Xi", "\\Pi", "\\Sigma", "\\Upsilon",
  /* 01x */
    "\\Phi", "\\Psi", "\\Omega", "ff", "fi", "fl", "ffi", "ffl",
  /* 02x */
    "i",     /* \imath */
    "j",     /* \jmath */
    "`",
    "'",
    "\237",  /* caron */
    "\226",  /* breve */
    "\257",  /* macron */
    "\232",  /* ring */
  /* 03x */
    "\270",  /* cedilla */
    "\337",  /* germandbls */
    "ae",
    "oe",
    "\370",  /* oslash */
    "AE",
    "OE",
    "\330",  /* Oslash */
  /* 04x */
    "/" /* bar for Polish suppressed-L ??? */, "!", "''", "#",
    "$", "%", "&", "'",
  /* 05x */
    "(", ")", "*", "+",
    ",", "\255" /* hyphen */, ".", "/",
  /* 06x */
    "0", "1", "2", "3", "4", "5", "6", "7",
  /* 07x */
    "8", "9", ":", ";",
    "!" /* exclamdown */, "=", "?" /* questiondown */, "?",
  /* 010x */
    "@", "A", "B", "C", "D", "E", "F", "G",
  /* 011x */
    "H", "I", "J", "K", "L", "M", "N", "O",
  /* 012x */
    "P", "Q", "R", "S", "T", "U", "V", "W",
  /* 013x */
    "X", "Y", "Z", "[",
    "``", "]", "\223" /* circumflex */, "\227" /* dotaccent */,
  /* 014x */
    "`", "a", "b", "c", "d", "e", "f", "g",
  /* 015x */
    "h", "i", "j", "k", "l", "m", "n", "o",
  /* 016x */
    "p", "q", "r", "s", "t", "u", "v", "w",
  /* 017x */
    "x", "y", "z",
    "--",    /* en dash */
    "---",   /* em dash */
    "\235",  /* hungarumlaut */
    "~",
    "\250"   /* dieresis */
  };

#define FIRSTCorkSpecialGlyphs FirstDvips
#define LASTCorkSpecialGlyphs (FirstDvips+0277)
static const char *CorkSpecialGlyphs[] = {
  /* 000 - accents for lowercase letters */
    "`",
    "'",
    "^",
    "~",
    "\230",  /* umlaut/dieresis */
    "\235",  /* hungarumlaut */
    "\232",  /* ring */
    "\237",  /* hacek/caron */
    "\226",  /* breve */
    "\257",  /* macron */
    "\227",  /* dot above/dotaccent */
    "\270",  /* cedilla */
    "\236",  /* ogonek */
  /* 015 - miscellaneous */
    "'",     /* single base quote/quotesinglbase */
    "<",     /* single opening guillemet/guilsinglleft */
    ">",     /* single closing guillemet/guilsinglright */
    "``",    /* english opening quotes/quotedblleft */
    "''",    /* english closing quotes/quotedblright */
    ",,",    /* base quotes/quotedblbase */
    "<<",    /* opening guillemets/guillemotleft */
    ">>",    /* closing guillemets/guillemotright */
    "--",    /* en dash/endash */
    "---",   /* em dash/emdash */
    "",      /* compound work mark (invisible)/ */
    "o",     /* perthousandzero (used in conjunction with %) */
    "\220",  /* dotless i/dotlessi */
    "j",     /* dotless j */
    "ff",    /* ligature ff */
    "fi",    /* ligature fi */
    "fl",    /* ligature fl */
    "ffi",   /* ligature ffi */
    "ffl",   /* ligature ffl */
    "_",     /* visible space */
  /* 041 - ASCII */
         "!", "\"", "#", "$", "%", "&", "'",
    "(", ")", "*", "+", ",", "-", ".", "/",
    "0", "1", "2", "3", "4", "5", "6", "7",
    "8", "9", ":", ";", "<", "=", ">", "?",
    "@", "A", "B", "C", "D", "E", "F", "G",
    "H", "I", "J", "K", "L", "M", "N", "O",
    "P", "Q", "R", "S", "T", "U", "V", "W",
    "X", "Y", "Z", "[", "\\","]", "^", "_",
    "`", "a", "b", "c", "d", "e", "f", "g",
    "h", "i", "j", "k", "l", "m", "n", "o",
    "p", "q", "r", "s", "t", "u", "v", "w",
    "x", "y", "z", "{", "|", "}", "~", "\255", /* hyphenchar (hanging) */
  /* 200 - letters for eastern European languages from latin-2 */
    "A\226", /* Abreve */
    "A\236", /* Aogonek */
    "C\264", /* Cacute */
    "C\237", /* Chacek */
    "D\237", /* Dhacek */
    "E\237", /* Ehacek */
    "E\236", /* Eogonek */
    "G\226", /* Gbreve */
    "L\264", /* Lacute */
    "L\237", /* Lhacek */
    "L/",    /* Lslash/Lstroke */
    "N\264", /* Nacute */
    "N\237", /* Nhacek */
    "\\NG",  /* Eng */
    "O\235", /* Ohungarumlaut */
    "R\264", /* Racute */
    "R\237", /* Rhacek */
    "S\264", /* Sacute */
    "S\237", /* Shacek */
    "S\270", /* Scedilla */
    "T\237", /* Thacek */
    "T\270", /* Tcedilla */
    "U\235", /* Uhungarumlaut */
    "U\232", /* Uring */
    "Y\250", /* Ydieresis */
    "Z\264", /* Zacute */
    "Z\237", /* Zhacek */
    "Z\227", /* Zdot */
    "IJ",    /* IJ */
    "I\227", /* Idot */
    "\\dj",  /* dbar */
    "\247",  /* section */
    "a\226", /* abreve */
    "a\236", /* aogonek */
    "c\222", /* cacute */
    "c\237", /* chacek */
    "d\237", /* dhacek */
    "e\237", /* ehacek */
    "e\236", /* eogonek */
    "g\226", /* gbreve */
    "l\222", /* lacute */
    "l\237", /* lhacek */
    "l/",    /* lslash */
    "n\222", /* nacute */
    "n\237", /* nhacek */
    "\\ng",  /* eng */
    "o\235", /* ohungarumlaut */
    "r\222", /* racute */
    "r\237", /* rhacek */
    "s\222", /* sacute */
    "s\237", /* shacek */
    "s\270", /* scedilla */
    "t\237", /* thacek */
    "t\270", /* tcedilla */
    "u\235", /* uhungarumlaut */
    "u\232", /* uring */
    "y\230", /* ydieresis */
    "z\222", /* zacute */
    "z\237", /* zhacek */
    "z\227", /* zdot */
    "ij",    /* ij */
    "\241",  /* exclamdown */
    "\277",  /* questiondown */
    "\243"   /* sterling */
  /* 0300-0377 is same as ISO 8859/1 except:
       0337 is Ess-zed and 0377 is ess-zed/germandbls */
};

/* There are gaps in the set of printable ISOLatin1 characters: */
/*CONST ISOLatin1Gaps = SET OF [0..255] {
    8_0..8_37, 8_177..8_217, 8_231, 8_234};
*/

typedef struct {
  double blx, bly, toprx, topry; /* font matrix in character coordinates */
  struct {double x, y;} chr[256]; /* widths in character coordinates */
} MetricsRec;
typedef MetricsRec *Metrics;
typedef Metrics MetricsTable[];

typedef GlyphIndex EncodingVector[256];
typedef EncodingVector *Encoding;
typedef Encoding EncodingTable[];

typedef struct {
  double x, y; /* (1000,0) in font's character coordinate system */
  double xp, yp; /* (0,1000) in font's character coordinate system */
  int e; /* index in "encoding" */
  int m; /* index in "metrics" */
  double bx, by, tx, ty; /* height of font bbox in reporting coordinates */
} FontRec;
typedef FontRec *Font;
typedef Font FontTable[];


/* Instance "T". */
#define state_normal 0
#define state_metrics 1
#define state_encoding 2
typedef struct {
  double itransform[6]; /* transform from device to default coordinates */
  int metricsSize;
  MetricsTable *metrics;
  int encodingSize;
  EncodingTable *encoding;
  BOOLEAN dvipsIsCork; /* assume Cork rather than "OT1" for dvips output */
  int fontSize;
  FontTable *font;

  /* Data for current word prefix: */
  char buf[1000];
  int lbuf; /* elements 0 through "lbuf-1" of "buf" are in use */
  int f; /* font number */
  double x0, y0, x1, y1; /* initial and final currentpoint */

  BOOLEAN nonEmptyPage;
  long blx, bly, toprx, topry; /* bounding box of last word output */
  char word[1000]; /* last word output */
  int state;
  /* state-specific components: */
  /* state_encoding: */ int encoding_e, encoding_n, encoding_i;
  /* state_metrics: */ int metrics_m, metrics_i;
} T;

static int ReadChar(char **instr);
static void UnreadChar(char **instr);
static int ReadInt(char **instr);
static long ReadLong(char **instr);
static int ParseInverseTransform(T *t, char *instr);
static int ParseEncoding(T *t, char *instr);
static int ParseEncodingMore(T *t, char *instr);
static void ReadPair(double *x, double *y, char **instr);
static int ParseFont(T *t, char *instr);
static int ParseMetrics(T *t, char *instr);
static int ParseMetricsMore(T *t, char *instr);
static void Itransform(T *t, double *x1, double *y1, double x0, double y0);
static void Output(T *t, const char **pre, const char **word, 
  int *llx, int *lly, int *urx, int *ury);
static BOOLEAN SameDirection(double x0, double y0, double x1, double y1);
static int ParseString(
  T *t, char *instr, const char **pre, const char **word, const char **post, 
  int *llx, int *lly, int *urx, int *ury);

int DLLEXPORT pstotextInit(void **instance) {
  T *t;
  int i;

  t = (T *)malloc(sizeof(T));
  if (t == NULL) return PSTOTEXT_INIT_MALLOC;

  t->state = state_normal;

  /* Initialize t->itransform to the identity transform. */
  t->itransform[0] = 1.0;
  t->itransform[1] = 0.0;
  t->itransform[2] = 0.0;
  t->itransform[3] = 1.0;
  t->itransform[4] = 0.0;
  t->itransform[5] = 0.0;

  t->metricsSize = t->encodingSize = t->fontSize = 100;

  t->metrics = (MetricsTable *)malloc(t->metricsSize * sizeof(Metrics));
  if (t->metrics == NULL) {
    free(t);
    return PSTOTEXT_INIT_MALLOC;
  }
  for(i=0; i<t->metricsSize; i++)(*t->metrics)[i] = NULL;

  t->encoding = (EncodingTable *)malloc(t->encodingSize * sizeof(Encoding));
  if (t->encoding == NULL) {
    free(t);
    return PSTOTEXT_INIT_MALLOC;
  }
  for(i=0;i<t->encodingSize;i++)(*t->encoding)[i] = NULL;

  t->dvipsIsCork = FALSE;

  t->font = (FontTable *)malloc(t->fontSize * sizeof(Font));
  if (t->font == NULL) {
    free(t);
    return PSTOTEXT_INIT_MALLOC;
  }
  for(i=0;i<t->fontSize;i++)(*t->font)[i] = NULL;

  t->lbuf = 0;
  t->nonEmptyPage = FALSE;
  t->blx = t->bly = t->toprx = t->topry = 0;

  *instance = t;

  return 0;
}

int DLLEXPORT pstotextSetCork(void *instance, int value) {
  T *t = (T *)instance;
  t->dvipsIsCork = value;
  return 0;  
}

int DLLEXPORT pstotextExit(void *instance) {
  T *t = (T *)instance;
  free(t->metrics);
  free(t->encoding);
  free(t->font);
  free(t);
  return 0;
}

static int ReadChar(char **instr) {
  int c = **(unsigned char**)instr;
  (*instr)++;
  return c;
}

static void UnreadChar(char **instr) {
  (*instr)--;
}

static int ReadInt(char **instr) {
  int i = 0;
  int sign = 1;
  int c;
  while ((c = ReadChar(instr))==' ') /* skip */ ;
  if (c=='-') {sign = -1; c = ReadChar(instr); }
  while ('0' <= c && c <= '9') {i = i*10+(c-'0'); c = ReadChar(instr);}
  UnreadChar(instr);
  return i*sign;
}

static long ReadLong(char **instr) {
  long i = 0;
  int sign = 1;
  int c;
  while ((c = ReadChar(instr))==' ') /* skip */ ;
  if (c=='-') {sign = -1; c = ReadChar(instr); }
  while ('0' <= c && c <= '9') {i = i*10+(c-'0'); c = ReadChar(instr);}
  UnreadChar(instr);
  return i*sign;
}

static int ParseInverseTransform(T *t, char *instr) {
  int i;
  for (i = 0; i<6; i++) t->itransform[i] = ReadLong(&instr) / 100.0;
  return 0;
}

static int ParseEncoding(T *t, char *instr) {
  /* Parse first line of QE directive. */
  int e = ReadInt(&instr);
  int n = ReadInt(&instr);
  int i;
  if (e<0) return PSTOTEXT_FILTER_BADENCODINGNUMBER;
  if (n>/*256*/1024) return PSTOTEXT_FILTER_TOOMANYGLYPHINDEXES;

  /* Grow "t->encoding" if necessary. */
  if (t->encodingSize<=e) {
    int oldSize = t->encodingSize;
    t->encodingSize = 2*e;
    t->encoding = (EncodingTable *)realloc(
      (char *)t->encoding,
      t->encodingSize * sizeof(Encoding)
    );
    for(i=oldSize;i<t->encodingSize;i++)(*t->encoding)[i] = NULL;
  }

  /* If this is the first encoding numbered "e", allocate array. */
  if ((*t->encoding)[e] == NULL)
    (*t->encoding)[e] = (EncodingVector *)malloc(sizeof(EncodingVector));

  t-> state = state_encoding;
  t->encoding_e = e; t->encoding_n = n; t->encoding_i = 0;

  return 0;
}

static int ParseEncodingMore(T *t, char *instr) {
  /* Parse subsequent line of QE directive. */
  Encoding enc = (*t->encoding)[t->encoding_e];
  int i, tooSparse;

  for (i = t->encoding_i; i<t->encoding_i+16 ; i++)
    (*enc)[i] = (i<t->encoding_n) ? ReadInt(&instr) : NonstandardGlyph;

  t->encoding_i += 16;
  if (t->encoding_i < 256) /* skip */ ;
  else {
    /* End of directive. */
    t->state = state_normal;

    /* Some applications build the encoding vector incrementally.  If
       this one doesn't have at least the lower-case letters, we augment
       it with ISOLatin1. */
    tooSparse = 0;
    for (i = 'a'; i<='z'; i++)
      tooSparse = (*enc)[i] == NonstandardGlyph;
    if (tooSparse)
      for (i = 0; i<256; i++)
        if ((*enc)[i] == NonstandardGlyph) (*enc)[i] = i;
  }

  return 0;

}

#define GuessAscend 0.9
#define GuessDescend -0.3

static void ReadPair(double /*out*/ *x, /*out*/ double *y, char **instr) {
  *x = ReadLong(instr) / 100.0;
  *y = ReadLong(instr) / 100.0;
}

static int ParseFont(T *t, char *instr) {
  /* Parse QF directive. */
  int n = ReadInt(&instr), i;
  Metrics mt;
  Font f;
  double xmax, bly, topry;
  if (n<0) return PSTOTEXT_FILTER_BADFONTNUMBER;

  /* Grow "t->font" if necessary. */
  if (t->fontSize<=n) {
    int oldSize = t->fontSize;
    t->fontSize = 2*n;
    t->font = (FontTable *)realloc(
      (char *)t->font,
      t->fontSize * sizeof(Font)
    );
    for(i=oldSize;i<t->fontSize;i++)(*t->font)[i] = NULL;
  }

  /* If this is the first font numbered "n", allocate "FontRec". */
  if ((*t->font)[n] == NULL)
    (*t->font)[n] = (Font)malloc(sizeof(FontRec));

  f = (*t->font)[n];
  ReadPair(&f->x, &f->y, &instr);
  ReadPair(&f->xp, &f->yp, &instr);
  f->e = ReadInt(&instr);
  if ((*t->encoding)[f->e] == NULL) return PSTOTEXT_FILTER_BADENCODINGNUMBER;
  f->m = ReadInt(&instr);
  mt = (*t->metrics)[f->m];
  if (mt == NULL) return PSTOTEXT_FILTER_BADMETRICNUMBER;

  /* Transform height of font bounding box to reporting coordinates: */
  f->bx = f->xp * mt->bly / 1000.0;
  f->by = f->yp * mt->bly / 1000.0;
  f->tx = f->xp * mt->topry / 1000.0;
  f->ty = f->yp * mt->topry / 1000.0;

  /* In some fonts produced by dvips, the FontBBox is incorrectly
     defined as [0 0 1 1].  We check for this, and apply the same
     heuristic used for an undefined FontBBox in "ParseMetrics".  */
  if (f->by-f->ty < 1.1) {
    xmax = 0.0;
    for (i = 0; i<256; i++)
      if (mt->chr[i].x > xmax) xmax = mt->chr[i].x;
      bly = GuessDescend * xmax; topry = GuessAscend * xmax;
      f->bx = f->xp * bly / 1000.0;
      f->by = f->yp * bly / 1000.0;
      f->tx = f->xp * topry / 1000.0;
      f->ty = f->yp * topry / 1000.0;
  }

  return 0;
}

static int ParseMetrics(T *t, char *instr) {
  /* Parse first line of QM directive. */
  int m = ReadInt(&instr), i;
  Metrics mt;

  if (m<0) return PSTOTEXT_FILTER_BADMETRICNUMBER;

  /* Grow "t->metrics" if necessary. */
  if (t->metricsSize<=m) {
    int oldSize = t->metricsSize;
    t->metricsSize = 2*m;
    t->metrics = (MetricsTable *)realloc(
      (char *)t->metrics,
      t->metricsSize * sizeof(Metrics)
    );
    for (i=oldSize;i<t->metricsSize;i++)(*t->metrics)[i] = NULL;
  }

  /* If this is the first metrics numbered "m", allocate "MetricsRec". */
  if ((*t->metrics)[m] == NULL)
    (*t->metrics)[m] = (Metrics)malloc(sizeof(MetricsRec));

  mt = (*t->metrics)[m];

  ReadPair(&mt->blx, &mt->bly, &instr);
  ReadPair(&mt->toprx, &mt->topry, &instr);

  t->state = state_metrics; t->metrics_m = m; t->metrics_i = 0;

  return 0;
}

static int ParseMetricsMore(T *t, char *instr) {
  /* Parse subsequent line of QM directive. */
  int i;
  Metrics mt = (*t->metrics)[t->metrics_m];

  for (i = t->metrics_i; i<t->metrics_i+8; i++)
    ReadPair(&mt->chr[i].x, &mt->chr[i].y, &instr);

  t->metrics_i += 8;
  if (t->metrics_i < 256) /* skip */ ;
  else {
    /* End of directive. */
    t->state = state_normal;

    /* If "FontBBox" was not specified, take a guess. */
    if (mt->blx == 0.0 && mt->bly == 0.0 && mt->toprx == 0.0 && mt->topry == 0.0) {
      for (i = 0; i<256; i++)
        if (mt->chr[i].x > mt->toprx) mt->toprx = mt->chr[i].x;
      mt->bly = GuessDescend * mt->toprx;
      mt->topry = GuessAscend * mt->toprx;
    }
  }

  return 0;
}

static void Itransform(T *t, double *x1, double *y1, double x0, double y0) {
/* Set (*x1, *y1) to (t->itransform) * (x0, y0). */
  *x1 = t->itransform[0]*x0 + t->itransform[2]*y0 + t->itransform[4];
  *y1 = t->itransform[1]*x0 + t->itransform[3]*y0 + t->itransform[5];
}

static void Output(T *t, const char **pre, const char **word, 
    int *llx, int *lly, int *urx, int *ury) {
  /* Output the next word. */
  double x0, y0, x1, y1, x2, y2, x3, y3;
  long blx, bly, toprx, topry, mid;
  Font f;

  f = (*t->font)[t->f];

  /* Compute the corners of the parallelogram with width "(t->x0,t->y0)"
     to "(t->x1,t->y1)" and height "(f.bx,f.by)" to "(f.tx,f.ty)". Then
     compute the bottom left corner and the top right corner of the
     bounding box (rectangle with sides parallel to the coordinate
     system) of this rectangle. */
  x0 = t->x0 + f->bx; y0 = t->y0 + f->by;
  x1 = t->x1 + f->bx; y1 = t->y1 + f->by;
  x2 = t->x0 + f->tx; y2 = t->y0 + f->ty;
  x3 = t->x1 + f->tx; y3 = t->y1 + f->ty;

  blx = (long)ceil(MIN(MIN(MIN(x0, x1), x2), x3));
  bly = (long)ceil(MAX(MAX(MAX(y0, y1), y2), y3)); /* *** should this be floor? PMcJ 981002 */
  toprx = (long)floor(MAX(MAX(MAX(x0, x1), x2), x3));
  topry = (long)floor(MIN(MIN(MIN(y0, y1), y2), y3)); /* *** should this be ceil? PMcJ 981002 */

  if (blx!=toprx && bly!=topry) {

    /* Output word separator if this isn't first word on page. */
    if (t->nonEmptyPage) {
      mid = (topry+bly) / 2;
      if (blx<toprx && topry<bly
	  && t->blx <= blx
	  && t->topry <= mid
	  && mid <= t->bly) *pre = " "; /* same line */
      else *pre = "\n"; /* different line */
    }
    else *pre = "";

    /* Output elements "0" through "t->lbuf-1" of "t->buf". */
    t->buf[t->lbuf] = '\0';
    strncpy(t->word, t->buf, t->lbuf+1);
    *word = t->word;

    t->nonEmptyPage = TRUE;
    t->blx = blx; t->bly = bly; t->toprx = toprx; t->topry = topry;

    /* transform device units to default PostScript units */
    Itransform( t, &x1, &y1, (double)blx, (double)bly);
    blx = (long)floor(x1); bly = (long)floor(y1);
    Itransform( t, &x1, &y1, (double)toprx, (double)topry);
    toprx = (long)ceil(x1); topry = (long)ceil(y1);

    if (blx < toprx) {
	*llx = blx; 
	*urx = toprx; 
    }
    else {
	*llx = toprx; 
	*urx = blx; 
    }
    if (bly < topry) {
	*lly = bly; 
	*ury = topry; 
    }
    else {
	*lly = topry; 
	*ury = bly; 
    }

  } /*if (blx!=toprx && bly!=topry) { */

  t->lbuf = 0;
}

static BOOLEAN SameDirection(double x0, double y0, double x1, double y1) {
  return (y0 == 0.0 && y1 == 0.0 && x0*x1 > 0.0)
      || (x0 == 0.0 && x1 == 0.0 && y0*y1 > 0.0)
      || (x0 * y1 == x1 * y0);
}

static int ParseString(T *t, char *instr, 
  const char **pre, const char **word, const char **post,
  int *llx, int *lly, int *urx, int *ury) {
  /* Parse QS directive. */
#define spaceTol 0.3 /* fraction of average character width to signal word break */
  char buf[1000];
  int n, ch, i, j, in, l;
  Font f;
  Encoding enc;
  GlyphIndex glyph;
  double x0, y0, x1, y1, xsp, ysp, dx, dy, maxx, maxy;

#define SetBuf() \
  { \
  strncpy(t->buf, buf, l); \
  t->lbuf = l; \
  t->f = n; \
  t->x0 = x0; t->y0 = y0; t->x1 = x1; t->y1 = y1; \
  }

  n = ReadInt(&instr); /* index in "t->font" */
  f = (*t->font)[n];
  if (f == NULL) return PSTOTEXT_FILTER_BADFONTNUMBER;
  enc = (*t->encoding)[f->e];
  if (enc==NULL) return PSTOTEXT_FILTER_BADENCODINGNUMBER;
  ReadPair(&x0, &y0, &instr); /* initial currentpoint */
  j = ReadInt(&instr); /* length of string */
  ch = ReadChar(&instr);
  if (ch != ' ')
    return PSTOTEXT_FILTER_BADQS;

  l = 0;
  for (i = 0; i<=j-1; i++) {
    in = ReadChar(&instr);
    /* if (in=='\0') return PSTOTEXT_FILTER_BADQS; */ /* TeX uses '\0' */
    glyph = (*enc)[in];

    /* If "glyph==0", then "in" mapped to the glyph ".notdef".  This
       is usually a mistake, but we check for several known cases: */
    if (glyph == 0) {

      /* If any element of the current encoding is in the range used
         by Microsoft TrueType, assume this character is, too. */
      int k; BOOLEAN tt = FALSE;
      for(k = 0; !tt && k < sizeof(*enc)/sizeof((*enc)[0]); k++) {
        if (FirstTT1 <= (*enc)[k] && (*enc)[k] <= LastTT2) tt = TRUE;
      }
      if (tt) glyph = FirstTT1 + (int)in;
      /* There are too many other exceptions to actually trap this:
        else if (in == '\r') ; // Adobe Illustrator does this...
        else if (in == '\t') ; // MacDraw Pro does this...
        else if (in == '\032') ; // MS Word on Mac does this...
        else return PSTOTEXT_FILTER_BADGLYPHINDEX;
      */
    }
    if (glyph == 0) 
      /* skip */;
    else if (glyph <= LastISOLatin1) {
      buf[l] = (char)glyph;
      /* *** if (glyph IN ISOLatin1Gaps) buf[l] = UnknownChar; */
      l++;
    }
    else if (glyph <= LASTSpecialGlyphs) {
      const char *str = SpecialGlyphs[glyph-FIRSTSpecialGlyphs];
      int lstr = strlen(str);
      strncpy(&buf[l], str, lstr);
      l += lstr;
    }
    else if (glyph <= LastDvips) {
      const char *str; int lstr; char tempstr[2];
      if (t->dvipsIsCork) {
        if (glyph <= LASTCorkSpecialGlyphs)
          str = CorkSpecialGlyphs[glyph-FIRSTCorkSpecialGlyphs];
        else if (glyph == FIRSTCorkSpecialGlyphs+0337)
          str = "SS";
        else if (glyph == FIRSTCorkSpecialGlyphs+0377)
          str = "\337";
        else {
          tempstr[0] = (char)(glyph-FIRSTCorkSpecialGlyphs); tempstr[1] = '\0';
          str = &tempstr[0];
        }
      }
      else if (glyph <= LASTDvipsGlyphs)
        /* Assume old text layout (OT1?). */
        str = DvipsGlyphs[glyph-FIRSTDvipsGlyphs];
      else {
        tempstr[0] = UnknownChar; tempstr[1] = '\0';
        str = &tempstr[0];
      }
      lstr = strlen(str);
      strncpy(&buf[l], str, lstr);
      l += lstr;
    }
    else if (glyph <= LastTT2) {
      if (FirstTT2 <= glyph) glyph -= FirstTT2-FirstTT1;
      if (glyph < FirstTT1+32) {
        buf[l] = UnknownChar; l++;
      }
      else if (glyph < FIRSTTTSpecialGlyphs ||
            LASTTTSpecialGlyphs < glyph) {
        buf[l] = (char)(glyph - FirstTT1); l++;
      }
      else {
        const char *str = TTSpecialGlyphs[glyph-FIRSTTTSpecialGlyphs];
        int lstr = strlen(str);
        strncpy(&buf[l], str, lstr);
        l += lstr;
      }
    }
    else if (glyph <= LastOldDvips) {
      const char *str = DvipsGlyphs[glyph-FirstOldDvips];
      int lstr = strlen(str);
      strncpy(&buf[l], str, lstr);
      l += lstr;
    }
    else if (glyph == NonstandardGlyph) { /* not in StandardGlyphs */
      buf[l] = UnknownChar;
      l++;
    }
    else return PSTOTEXT_FILTER_BADGLYPHINDEX;

    /* We no longer substitute minus for hyphen. */
    /* if (buf[l-1] == '\255') buf[l-1] = '-'; */
  }

  ReadPair(&x1, &y1, &instr); /* final currentpoint */
  if (l != 0) { /* "l==0" e.g., when Adobe Illustrator outputs "\r" */
    if (t->lbuf == 0) {SetBuf();}
    else {
      /* If the distance between this string and the previous one is
         less than "spaceTol" times the minimum of the average
         character widths in the two strings, and the two strings
         are in the same direction, then append this string to the
         previous one.  Otherwise, output the previous string and
         then save the current one.

         Sometimes this string overlaps the previous string, e.g.,
         when TeX is overprinting an accent over another character.
         So we make a special case for this (but only handle the
         left-to-right orientation). */

      /* Set "(xsp,ysp)" to the reporting space coordinates of the
         minimum of the average width of the characters in this
         string and the previous one. */

      xsp = MIN((t->x1-t->x0) / t->lbuf, (x1-x0) / l);
      ysp = MIN((t->y1-t->y0) / t->lbuf, (y1-y0) / l);

      dx = x0 - t->x1;
      dy = y0 - t->y1;
      maxx = spaceTol * xsp;
      maxy = spaceTol * ysp;
      if ((dx*dx + dy*dy < maxx*maxx + maxy*maxy)
          || ((t->y1 == y0 && t->x0 <= t->x1 && t->x0 <= x0 && x0 <= t->x1)
         && SameDirection(t->x1-t->x0, t->y1-t->y0, x1-x0, y1-y0))) {
        if (t->lbuf+l >= sizeof(t->buf)) {
          Output(t, pre, word, llx, lly, urx, ury);
          *post = "";
          SetBuf();
        }
        else {
          strncpy(&t->buf[t->lbuf], buf, l);
          t->lbuf += l;
          t->x1 = x1; t->y1 = y1;
          /* *** Merge font bounding boxes? */
        }
      }
      else {
        Output(t, pre, word, llx, lly, urx, ury);
        *post = "";
        SetBuf();
      }
    }
  }

  return 0;
}

int DLLEXPORT pstotextFilter(void *instance, char *instr, 
  const char **pre, const char **word, const char **post, 
  int *llx, int *lly, int *urx, int *ury) {
  T *t = (T *)instance;
  int c;
  *word = NULL;
  switch (t->state) {
    case state_normal:
      do {c = ReadChar(&instr); if (c=='\0') return 0;} while (c!='Q');
      c = ReadChar(&instr);
      switch (c) {
        case 'I': return ParseInverseTransform(t, instr);
        case 'M': return ParseMetrics(t, instr);
        case 'E': return ParseEncoding(t, instr);
        case 'F': return ParseFont(t, instr);
        case 'S': return ParseString(
                           t, instr, pre, word, post, llx, lly, urx, ury);
        case 'C':
        case 'P': /* copypage, showpage */
                  /* If any QS directives have been encountered on this page,
                     t->buf will be nonempty now. */
                  if (t->lbuf > 0) {
                    Output(t, pre, word, llx, lly, urx, ury);
                    *post = "\n\f\n";
                  }
                  else {
                    *pre = "";
                    *word = "";
                    *llx = 0; *lly = 0; *urx = 0; *ury = 0;
                    *post = "\f\n";
                  }
                  t->nonEmptyPage = FALSE;
                  t->blx = t->bly = t->toprx = t->topry = 0;
                  break;
        case 'Z': /* erasepage */ /* skip */ break;
        case '\0': return 0;
        /* default: skip */
      }
      break;
    case state_metrics: return ParseMetricsMore(t, instr);
    case state_encoding: return ParseEncodingMore(t, instr);
  }
  return 0;
}

