/* Copyright (C) 2000-2002, Ghostgum Software Pty Ltd.  All rights reserved.
  
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
/* display device image */

/* cimg.h */

#ifdef UNIX
#ifdef MULTITHREAD
#define GGMUTEX pthread_mutex_t
#else
#define GGMUTEX int
#endif
#endif

#ifdef _Windows
#define GGMUTEX HANDLE
#endif

#ifdef OS2
#define GGMUTEX HMTX
#endif

/* return this to Ghostscript on error */
#define DISPLAY_ERROR e_rangecheck;

enum SEPARATIONS {
    SEP_CYAN = 8,
    SEP_MAGENTA = 4,
    SEP_YELLOW = 2,
    SEP_BLACK = 1
};

typedef struct IMAGE_S IMAGE;
struct IMAGE_S {
    BOOL opening;	/* between display_open() and first display_size() */
    BOOL open;		/* image is valid */
    void *handle;
    void *device;
    int width;
    int height;
    int raster;
    unsigned int format;
    unsigned char *image;

    /* During presize, we check if we understand the format */
    BOOL format_known;		/* true if we understand format */

    /* Windows and OS/2 require a raster line which is a multiple 
     * of 4 bytes.  If raster is not what we expect, then we need 
     * to lie about about how many pixels are really on the line 
     * when doing bitmap transfers.  
     * align_width contains our calculation of how many pixels are 
     * needed to match raster.
     * If align_width != width, then we bitmap transfer uses:
     *  source width      = img->align_width
     *  destination width = img->width
     */
    int align_width;

    /* When we open Ghostscript, we get one SYNC as the display
     * device is opened, then later we get either SYNC or PAGE
     * when we need to show something.
     * If ignore_sync is TRUE, we are ignoring the first SYNC. */
    BOOL ignore_sync;

    int separation;	/* for displaying C or M or Y or K */

    GGMUTEX hmutex;	/* thread synchronisation */
    int lock_count;	/* used for debugging mutex locking */

#ifdef UNIX
    GdkRgbCmap *cmap;	/* colour map = palette */
    guchar *rgbbuf;	/* used when we need to convert raster format */
#endif

#ifdef _Windows
    BITMAPINFOHEADER bmih;
    HPALETTE palette;
#endif

#ifdef OS2
    unsigned char *bitmap;	/* allocated memory */
#endif

    /* To do progressive update while drawing, maintain a list 
     * dirty tiles and update them when a tile is too dirty,
     * or when enough tiles to fill a row are dirty */
    int tile_width;	/* normally 64 */
    int tile_height;	/* normally 64 */
    int tile_columns;
    int tile_rows;
    int tile_dirty; 	/* number of dirty tiles */
    int tile_threshold; /* number of dirty tiles before update */
    int tile_verydirty_threshold;  /* update when one tile dirtied this much */
    int tile_time;	/* time in milliseconds since start of day */
    int tile_interval;	/* minimum time in milliseconds between updates */
    int *tile;		/* number of times each tile is touched */
};

void image_lock(IMAGE *img);
void image_unlock(IMAGE *img);
void image_to_24BGR(IMAGE *img, unsigned char *dest, unsigned char *source);
void image_color(unsigned int format, int index, 
    unsigned char *r, unsigned char *g, unsigned char *b);
int image_sync(IMAGE *img);
int image_size(IMAGE *img);
int image_presize(IMAGE *img, int width, int height, int raster,
    unsigned int format);
int image_preclose(IMAGE *img);
int image_update_time(IMAGE *img);	/* returns zero if enough time */
					/* has passed to do an update */


void *display_memalloc(void *handle, void *device, unsigned long size);
int display_memfree(void *handle, void *device, void *mem);

