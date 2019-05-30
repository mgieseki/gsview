/* Copyright (C) 1993-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvctype.h */
/* structures common to all platforms */

typedef struct tagUSERMEDIA {
    char name[32];	/* read from INI file */
    int width;		/* read from INI file */
    int height;		/* read from INI file */
    int id;		/* allocated automatically */
} USERMEDIA;

extern USERMEDIA usermedia[IDM_USERSIZE13 - IDM_USERSIZE1 + 1];

typedef struct tagPDFLINK {
    PSBBOX bbox;
    int page;
    char action[64];	/* named action, e.g. GoBack */
    float border_xr;
    float border_yr;
    float border_width;
    float colour_red;
    float colour_green;
    float colour_blue;
    BOOL  colour_valid;
    struct tagPDFLINK *next;
    /* need to add View */
} PDFLINK;

typedef struct tagPAGELIST {
	int current;	/* index of current selection */
	BOOL multiple;	/* true if multiple selection allowed */
	BOOL *select;	/* array of selection flags */
	BOOL reverse;	/* reverse pages when extracting or printing */
} PAGELIST;

typedef struct tagPSFILE {
	char 	name[MAXSTR];	/* name of selected document file */
	char	tname[MAXSTR];	/* name of temporary file (gunzipped) */
	GFile 	*file;		/* selected file */
	UINT	filetimel;	/* time/date of selected file */
	UINT	filetimeh;	/* time/date of selected file */
	LONG	length;		/* length of selected file */
	CDSC	*dsc;		/* DSC structure.  NULL if not DSC */
	PAGELIST page_list;	/* selected page list */
	int	print_from;
	int	print_to;
#define ALL_PAGES 0
#define ODD_PAGES 1
#define EVEN_PAGES 2
	int	print_oddeven;
	BOOL	print_ignoredsc;
	int	print_copies;
	BOOL	locked;		/* To prevent two threads using the file */
	BOOL	ignore_special;	/* true if %%PageOrder: Special to be ignored */
	int 	pagenum;	/* current page number */
	BOOL	ctrld;		/* TRUE if file starts with ^D */
	BOOL	pjl;		/* TRUE if file starts with HP LaserJet PJL prologue */
	BOOL	gzip;		/* TRUE if file compressed with gzip */
	BOOL	bzip2;		/* TRUE if file compressed with bzip2 */
	int 	preview;	/* preview type IDS_EPSF, IDS_EPSI, etc. */
	BOOL	ispdf;		/* true if PDF document */
	char 	text_name[MAXSTR];  /* name of file containing extracted text */
	unsigned long text_offset;  /* file offset after last text search match */
	int	text_page;	    /* page of last text search match */
	BOOL	text_extract;       /* TRUE=extracting, FALSE=searching */
	PSBBOX	text_bbox;	    /* bbox of found word */
} PSFILE;

/* In the single threaded version, there are three places that
 * process the message loop:
 *  1. Main get message loop in UNLOADED state
 *  2. Peek message loop in BUSY state
 *  3. Get message loop in PAGE state
 * We must avoid recursive calls from the last two message loop handlers.
 * In the multithreaded version, the DLL is handled by a separate thread.
 * User input that affects the GS DLL sets some pending variables, 
 * which are processed later by the appropriate message loop or thread.
 * Changes to the pending structure must be made inside a critical section.
 */
/* pending structure */
typedef struct tagPENDING {
	BOOL	unload;		/* We want to unload the DLL */
	BOOL	abort;		/* ignore errors and restart the interpreter */
	BOOL	restart;	/* restart interpreter */
	BOOL	redisplay;	/* redisplay after interpreter restarted */
	BOOL	next;		/* move to next page */
	BOOL	now;		/* We want to do something now */
	/* if now set, at least one of the following six will be set */
	int	pagenum;	/* page number to display */
	PSFILE *psfile;		/* new document to display */
	BOOL	resize;		/* size, resolution or orientation change */
	BOOL	text;		/* extract text, don't display */
	BOOL	pstoedit;	/* extract using pstoedit, don't display */
} PENDING;

extern PENDING pending;

typedef struct tagGSINPUT {
    unsigned long ptr;
    unsigned long end;
    BOOL seek;
} GSINPUT;

typedef struct tagGSDLL_INPUT {
    int	count;
    int	index;
    GSINPUT section[5];	/* header, defaults, prolog, setup, page */
} GSDLL_INPUT;

typedef struct gsview_args_s {
    int debug;			/* /d */
    int portable;		/* /a */
    int multithread;		/* /t */
    int help;			/* -help */
    int version;		/* -version */
    char filename[MAXSTR];	/* filename */
    char queue[MAXSTR];		/* /pQUEUE  or /sQUEUE */
    char device[64];		/* /fDEVICE */
    int print;			/* /p */
    int convert;		/* /f */
    int spool;			/* /s */
    int existing;		/* /e */
    int exit_existing; 		/* /x */
    char media[64];		/* /ma4 */
    int orient;	/* IDM_AUTOORIENT etc */	/* /oportrait */
    float xdpi;			/* -rXDPIxYDPI */
    float ydpi;
    int geometry;		/* -geometry WIDTHxHEIGHT+XOFFSET+YOFFSET */
    int geometry_width;
    int geometry_height;
    int geometry_xoffset;
    int geometry_yoffset;
} GSVIEW_ARGS;

typedef struct tagMATRIX {
   float xx, xy, yx, yy, tx, ty;
} MATRIX;

typedef struct tagMEASURE {
   float tx, ty;	/* translation */
   float rotate;	/* rotation */
   float sx, sy;	/* scaling */
   int unit;		/* IDM_UNITPT .. IDM_UNITCUSTOM */
} MEASURE;

/* options that are saved in INI file */
typedef struct tagOPTIONS {
	int	language;
	int	gsversion;
	char	gsdll[MAXSTR];
	char	gsexe[MAXSTR];
	char	gsinclude[MAXSTR];
	char	gsother[MAXSTR];
	BOOL	configured;
	int	drawmethod;	/* OS/2 only */
	int	update;		/* ProgressiveUpdate, 0=default, */
			        /*  1=local only, 2=never */
	char	helpcmd[MAXSTR];
	POINT	img_origin;
	POINT	img_size;
	BOOL	img_max;
	int	unit;
	BOOL	unitfine;
	int	pstotext;
	BOOL	settings;
	BOOL	button_show;
	BOOL	fit_page;
	BOOL	safer;
	int	media;
	char	medianame[32];
	BOOL	media_rotate;
	int	user_width;
	int	user_height;
	int	user_width_warn;
	int	user_height_warn;
	BOOL	epsf_clip;
	BOOL	epsf_warn;
	BOOL	redisplay;
	BOOL    ignore_dsc;
	int	dsc_warn;	/* level of DSC error warnings */
	BOOL	show_bbox;
	BOOL	auto_orientation;
	int	orientation;
	BOOL	swap_landscape;
	float	xdpi;
	float	ydpi;
	float	zoom_xdpi;
	float	zoom_ydpi;
	int	depth;
	int	alpha_text;
	int	alpha_graphics;
	BOOL	save_dir;
        /* for printing to GS device */
	char	printer_device[64];	/* Ghostscript device for printing */
	char	printer_resolution[64];
	int	print_fixed_media;
	/* for converting with GS device */
	char	convert_device[64];
	char	convert_resolution[64];	/* Ghostscript device for converting */
	int	convert_fixed_media;
	/* for printing to GDI device */
	int	print_gdi_depth;	/* IDC_MONO, IDC_GREY, IDC_COLOUR */
	int	print_gdi_fixed_media;
        /* general printing */
#define PRINT_GDI 0
#define PRINT_GS 1
#define PRINT_PS 2
#define PRINT_CONVERT 3
	int	print_method;		/* GDI, GS, PS */
	BOOL	print_reverse;		/* pages to be in reverse order */
	BOOL	print_to_file;
	char	printer_port[MAXSTR];	/* for Win32s */
	char	printer_queue[MAXSTR];	/* for Win32 */
	BOOL	auto_bbox;
	MATRIX	ctm;
	MEASURE measure;
} OPTIONS;

extern char last_files[4][MAXSTR];	/* last 4 files used */
extern int last_files_count;		/* number of files known */


#define HISTORY_MAX 32
typedef struct tagHISTORY {
    int index;  /* index of next page to store */
    int count;	/* number of valid pages in history */
    int pages[HISTORY_MAX];
} HISTORY;
extern HISTORY history;		/* history of pages displayed */


typedef struct tagTEXTINDEX {
    int word;	/* offset to word */
    int line;	/* line number on page */
    PSBBOX bbox;
} TEXTINDEX;
extern TEXTINDEX *text_index;
extern unsigned int text_index_count;	/* number of words in index */
extern char *text_words;	/* storage for words */

