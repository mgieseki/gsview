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
/* cimg.c */

#include "gvc.h"
void image_16BGR555_to_24BGR(int width, unsigned char *dest, 
    unsigned char *source);
void image_16BGR565_to_24BGR(int width, unsigned char *dest, 
    unsigned char *source);
void image_16RGB555_to_24BGR(int width, unsigned char *dest, 
    unsigned char *source);
void image_16RGB565_to_24BGR(int width, unsigned char *dest, 
    unsigned char *source);
void image_32CMYK_to_24BGR(int width, unsigned char *dest, 
    unsigned char *source, int sep);
void image_to_24BGR(IMAGE *img, unsigned char *dest, unsigned char *source);
void image_draw_tile(IMAGE *img, int tx1, int ty1, int tx2, int ty2);
void image_tile_update(IMAGE *img, int ux, int uy, int uw, int uh);

/***********************************************************/

/* common image functions */

/* Return a palette entry for given format and index */
void
image_color(unsigned int format, int index, 
    unsigned char *r, unsigned char *g, unsigned char *b)
{
    switch (format & DISPLAY_COLORS_MASK) {
	case DISPLAY_COLORS_NATIVE:
	    switch (format & DISPLAY_DEPTH_MASK) {
		case DISPLAY_DEPTH_1:
		    *r = *g = *b = (index ? 0 : 255);
		    break;
		case DISPLAY_DEPTH_4:
		    {
		    int one = index & 8 ? 255 : 128;
		    *r = (index & 4 ? one : 0);
		    *g = (index & 2 ? one : 0);
		    *b = (index & 1 ? one : 0);
		    }
		    break;
		case DISPLAY_DEPTH_8:
		    /* palette of 96 colors */
		    /* 0->63 = 00RRGGBB, 64->95 = 010YYYYY */
		    if (index < 64) {
			int one = 255 / 3;
			*r = ((index & 0x30) >> 4) * one;
			*g = ((index & 0x0c) >> 2) * one;
			*b =  (index & 0x03) * one;
		    }
		    else {
			int val = index & 0x1f;
			*r = *g = *b = (val << 3) + (val >> 2);
		    }
		    break;
	    }
	    break;
	case DISPLAY_COLORS_GRAY:
	    switch (format & DISPLAY_DEPTH_MASK) {
		case DISPLAY_DEPTH_1:
		    *r = *g = *b = (index ? 255 : 0);
		    break;
		case DISPLAY_DEPTH_4:
		    *r = *g = *b = (unsigned char)((index<<4) + index);
		    break;
		case DISPLAY_DEPTH_8:
		    *r = *g = *b = (unsigned char)index;
		    break;
	    }
	    break;
    }
}

/* convert one line of 16BGR555 to 24BGR */
/* byte0=GGGBBBBB byte1=0RRRRRGG */
void
image_16BGR555_to_24BGR(int width, unsigned char *dest, unsigned char *source)
{
    int i;
    WORD w;
    unsigned char value;
    for (i=0; i<width; i++) {
	w = source[0] + (source[1] << 8);
	value = w & 0x1f;		/* blue */
	*dest++ = (value << 3) + (value >> 2);
	value = (w >> 5) & 0x1f;	/* green */
	*dest++ = (value << 3) + (value >> 2);
	value = (w >> 10) & 0x1f;	/* red */
	*dest++ = (value << 3) + (value >> 2);
	source += 2;
    }
}

/* convert one line of 16BGR565 to 24BGR */
/* byte0=GGGBBBBB byte1=RRRRRGGG */
void
image_16BGR565_to_24BGR(int width, unsigned char *dest, unsigned char *source)
{
    int i;
    WORD w;
    unsigned char value;
    for (i=0; i<width; i++) {
	w = source[0] + (source[1] << 8);
	value = w & 0x1f;		/* blue */
	*dest++ = (value << 3) + (value >> 2);
	value = (w >> 5) & 0x3f;	/* green */
	*dest++ = (value << 2) + (value >> 4);
	value = (w >> 11) & 0x1f;	/* red */
	*dest++ = (value << 3) + (value >> 2);
	source += 2;
    }
}

/* convert one line of 16RGB555 to 24BGR */
/* byte0=0RRRRRGG byte1=GGGBBBBB */
void
image_16RGB555_to_24BGR(int width, unsigned char *dest, unsigned char *source)
{
    int i;
    WORD w;
    unsigned char value;
    for (i=0; i<width; i++) {
	w = (source[0] << 8) + source[1];
	value = w & 0x1f;		/* blue */
	*dest++ = (value << 3) + (value >> 2);
	value = (w >> 5) & 0x1f;	/* green */
	*dest++ = (value << 3) + (value >> 2);
	value = (w >> 10) & 0x1f;	/* red */
	*dest++ = (value << 3) + (value >> 2);
	source += 2;
    }
}

/* convert one line of 16RGB565 to 24BGR */
/* byte0=RRRRRGGG byte1=GGGBBBBB */
void
image_16RGB565_to_24BGR(int width, unsigned char *dest, unsigned char *source)
{
    int i;
    WORD w;
    unsigned char value;
    for (i=0; i<width; i++) {
	w = (source[0] << 8) + source[1];
	value = w & 0x1f;		/* blue */
	*dest++ = (value << 3) + (value >> 2);
	value = (w >> 5) & 0x3f;	/* green */
	*dest++ = (value << 2) + (value >> 4);
	value = (w >> 11) & 0x1f;	/* red */
	*dest++ = (value << 3) + (value >> 2);
	source += 2;
    }
}


/* convert one line of 32CMYK to 24BGR */
void
image_32CMYK_to_24BGR(int width, unsigned char *dest, unsigned char *source,
    int sep)
{
    int i;
    int cyan, magenta, yellow, black;
    for (i=0; i<width; i++) {
	cyan = source[0];
	magenta = source[1];
	yellow = source[2];
	black = source[3];
	if (!(sep & SEP_CYAN))
	    cyan = 0;
	if (!(sep & SEP_MAGENTA))
	    magenta = 0;
	if (!(sep & SEP_YELLOW))
	    yellow = 0;
	if (!(sep & SEP_BLACK))
	    black = 0;
	*dest++ = (255 - yellow)  * (255 - black)/255; /* blue */
	*dest++ = (255 - magenta) * (255 - black)/255; /* green */
	*dest++ = (255 - cyan)    * (255 - black)/255; /* red */
	source += 4;
    }
}

void
image_to_24BGR(IMAGE *img, unsigned char *dest, unsigned char *source)
{
    unsigned char *d = dest;
    unsigned char *s = source;
    int width = img->width;
    unsigned int alpha = img->format & DISPLAY_ALPHA_MASK;
    BOOL bigendian = (img->format & DISPLAY_ENDIAN_MASK) == DISPLAY_BIGENDIAN;
    int i;

    switch (img->format & DISPLAY_COLORS_MASK) {
	case DISPLAY_COLORS_NATIVE:
	    if ((img->format & DISPLAY_DEPTH_MASK) == DISPLAY_DEPTH_16) {
		if (bigendian) {
		    if ((img->format & DISPLAY_555_MASK)
			== DISPLAY_NATIVE_555)
			image_16RGB555_to_24BGR(img->width, dest, source);
		    else
			image_16RGB565_to_24BGR(img->width, dest, source);
		}
		else {
		    if ((img->format & DISPLAY_555_MASK)
			== DISPLAY_NATIVE_555) {
			image_16BGR555_to_24BGR(img->width, dest, source);
		    }
		    else
			image_16BGR565_to_24BGR(img->width, dest, source);
		}
	    }
	    break;
	case DISPLAY_COLORS_RGB:
	    if ((img->format & DISPLAY_DEPTH_MASK) != DISPLAY_DEPTH_8)
		return;
	    for (i=0; i<width; i++) {
		if ((alpha == DISPLAY_ALPHA_FIRST) || 
		    (alpha == DISPLAY_UNUSED_FIRST))
		    s++;
		if (bigendian) {
		    *d++ = s[2];
		    *d++ = s[1];
		    *d++ = s[0];
		    s+=3;
		}
		else {
		    *d++ = *s++;
		    *d++ = *s++;
		    *d++ = *s++;
		}
		if ((alpha == DISPLAY_ALPHA_LAST) || 
		    (alpha == DISPLAY_UNUSED_LAST))
		    s++;
	    }
	    break;
	case DISPLAY_COLORS_CMYK:
	    if ((img->format & DISPLAY_DEPTH_MASK) != DISPLAY_DEPTH_8)
		return;
	    image_32CMYK_to_24BGR(width, dest, source, img->separation);
	    break;
    }
}


/***********************************************************/
/* display device callback functions */

/* handle is a pointer to a view */

/* 
#define DISPLAY_DEBUG
 */


/* New device has been opened */
/* This is the first event from this device. */
static int display_open(void *handle, void *device)
{
    VIEW *view = (VIEW *)handle;
    IMAGE *img = view_get_image(view);
#ifdef DISPLAY_DEBUG
    gs_addmessf("display_open(0x%x, 0x%x)\n", handle, device);
#endif
    
    if (img == NULL)
	return_error(DISPLAY_ERROR);

    if (img->open || img->opening)
	return_error(DISPLAY_ERROR);

    image_lock(img);
    img->opening = TRUE;
    img->handle = handle;
    img->device = device;

    img->width = img->height = img->raster = 0;
    img->format = 0;
    img->image = NULL;
    img->tile = NULL;

    return 0;
}

/* Device is about to be closed. */
/* Device will not be closed until this function returns. */
static int display_preclose(void *handle, void *device)
{
    VIEW *view = (VIEW *)handle;
    IMAGE *img = view_get_image(view);
#ifdef DISPLAY_DEBUG
    gs_addmessf("display_preclose(0x%x, 0x%x)\n", handle, device);
#endif
    image_lock(img);
    img->open = FALSE;
    img->opening = FALSE;
    image_preclose(img);
    return 0;
}

/* Device has been closed. */
/* This is the last event from this device. */
static int display_close(void *handle, void *device)
{
    VIEW *view = (VIEW *)handle;
    IMAGE *img = view_get_image(view);
#ifdef DISPLAY_DEBUG
    gs_addmessf("display_close(0x%x, 0x%x)\n", handle, device);
#endif
    img->open = FALSE;
    img->opening = FALSE;

    img->handle = handle;
    img->device = device;

    img->width = img->height = img->raster = 0;
    img->format = 0;
    img->image = NULL;

    if (img->tile)
	free(img->tile);
    img->tile = NULL;
    img->tile_width = 0;
    img->tile_height = 0;
    img->tile_dirty = 0;
    img->tile_verydirty_threshold = 0;
    img->tile_threshold = 0;
    img->tile_columns = 0;
    img->tile_rows = 0;
    img->tile_time = 0;
    img->tile_interval = 0;

    image_unlock(img);

    view_post_message(view, WM_GSDEVICE, 0);
    return 0;
}

/* Device is about to be resized. */
/* Resize will only occur if this function returns 0. */
static int display_presize(void *handle, void *device, int width, int height, 
	int raster, unsigned int format)
{
    VIEW *view = (VIEW *)handle;
    IMAGE *img = view_get_image(view);
#ifdef DISPLAY_DEBUG
    gs_addmessf("display_presize(0x%x, 0x%x, width=%d height=%d raster=%d\
 format=%d)\n", 
       handle, device, width, height, raster, format);
#endif
    if (!img->opening)
	image_lock(img);

    /* check if we understand format */
    if (image_presize(img, width, height, raster, format))
	return_error(DISPLAY_ERROR);

    if (img->tile)
	free(img->tile);
    img->tile = NULL;

    return 0;
}
   
/* Device has been resized. */
/* New pointer to raster returned in pimage */
static int display_size(void *handle, void *device, int width, int height, 
	int raster, unsigned int format, unsigned char *pimage)
{
    VIEW *view = (VIEW *)handle;
    IMAGE *img = view_get_image(view);
    int len;
#ifdef DISPLAY_DEBUG
    gs_addmessf("display_size(0x%x, 0x%x, width=%d height=%d raster=%d\
 format=%d image=0x%x)\n", 
       handle, device, width, height, raster, format, pimage);
#endif
    img->width = width;
    img->height = height;
    img->raster = raster;
    img->format = format;
    img->image = pimage;
    if (image_size(img))
	img->open = FALSE;
    else {
	if (img->opening)
	    img->open = TRUE;
	img->opening = FALSE;
    }

    img->tile_width = 64;
    img->tile_height = 64;
    img->tile_dirty = 0;
    img->tile_verydirty_threshold = 256;
    if (img->tile) {
	free(img->tile);
	img->tile = NULL;
    }
    img->tile_columns = (img->width + img->tile_width - 1) / img->tile_width;
    img->tile_rows = (img->height + img->tile_height - 1) / img->tile_height;
    len = img->tile_rows * img->tile_columns * sizeof(img->tile[0]);
    img->tile = (int *)malloc(len);
    if (img->tile)
	memset(img->tile, 0, len);
    img->tile_threshold = max(4, img->tile_columns/2);
    img->tile_time = 0;
    img->tile_interval = 100;	/* start at 100ms */

    image_unlock(img);
    view_post_message(view, WM_GSSIZE, 0);
    return 0;
}
   
/* flushpage */
static int display_sync(void *handle, void *device)
{
    VIEW *view = (VIEW *)handle;
    IMAGE *img = view_get_image(view);
#ifdef DISPLAY_DEBUG
    gs_addmessf("display_sync(0x%x, 0x%x)\n", handle, device);
#endif
    if (img->ignore_sync) {
	/* ignore this sync, but not the next */
	img->ignore_sync = FALSE;
	return 0;
    }
    image_sync(img);
    view_post_message(view, WM_GSSYNC, 0);
    return 0;
}

/* showpage */
/* If you want to pause on showpage, then don't return immediately */
static int display_page(void *handle, void *device, int copies, int flush)
{
    VIEW *view = (VIEW *)handle;
    IMAGE *img = view_get_image(view);

#ifdef DISPLAY_DEBUG
    gs_addmessf("display_page(0x%x, 0x%x, copies=%d flush=%d)\n", 
	handle, device, copies, flush);
#endif
    img->ignore_sync = FALSE;
    image_sync(img);
    view_page_callback(view);

    return 0;
}

/* 
 * This keeps track of how much of the page is currently
 * dirty by segmenting the page into tiles and keeping
 * track of which tiles are dirty.
 * When more than img->tile_threshold tiles are dirty,
 * or a single tile has been dirtied many times,
 * a refresh of those tiles is requested.
 * The first part of this code must be fast because it is
 * called frequently.
 *
 * The current code causes the redraw to occur in chunks.
 * Perhaps a LRU method would be better so that it progressively
 * draws those tiles which are no longer being dirtied.
 */

void
image_draw_tile(IMAGE *img, int tx1, int ty1, int tx2, int ty2)
{
    int param;
    tx1 = min(max(0, tx1), 255);
    ty1 = min(max(0, ty1), 255);
    tx2 = min(max(0, tx2), 255);
    ty2 = min(max(0, ty2), 255);
    param = (tx1<<24) + (ty1<<16) + (tx2<<8) + ty2;
    view_post_message((VIEW *)(img->handle), WM_GSTILE, param);
}


void
image_tile_update(IMAGE *img, int ux, int uy, int uw, int uh)
{
    int tx, ty;
    int tx1, ty1, tx2, ty2;
    int columns = img->tile_columns;
    int rows = img->tile_rows;
    int dirty = 0;
    int very_dirty = 0;
    if (!img->tile)
	return;
    if ((img->width==0) || (img->height ==0) || 
	(img->tile_width == 0) || (img->tile_height ==0))
	return;

    /* Mark tiles as dirty */
    tx1 = ux / img->tile_width;		/* lower left */
    ty1 = uy / img->tile_height;
    tx2 = (ux+uw-1) / img->tile_width;	/* upper right */
    ty2 = (uy+uh-1) / img->tile_height;

    /* Sanity check on tile index */
    if ((tx2 >= img->tile_columns) || (tx1 > tx2) || (tx1 < 0) ||
        (ty2 >= img->tile_rows) || (ty1 > ty2) || (ty1 < 0))
	return;
    /* Mark dirty tiles */
    for (ty=ty1; ty<=ty2; ty++) {
	for (tx=tx1; tx<=tx2; tx++) {
	    /* is tile newly dirtied? */
	    if (!img->tile[ty*columns + tx])
		img->tile_dirty++;		/* newly dirtied tile */
	    /* mark tile as dirty, and note if it is very dirty */
	    dirty = ++(img->tile[ty*columns + tx]);
	    if (dirty > img->tile_verydirty_threshold)
		very_dirty = 1;
	}
    }
    /* code above here must be fast */

    if (very_dirty || (img->tile_dirty > img->tile_threshold)) {
	/* Make sure we do this no more than 10 times per second */
	/* to avoid swamping the window queue */
	if (image_update_time(img) != 0) {
	    /* too soon */
	    return;
	}

	/* work out rectangle enclosing dirty tiles */
	tx1 = columns;
	tx2 = 0;
	ty1 = rows;
	ty2 = 0;
	dirty = 0;
	for (ty=0; ty<rows; ty++) {
	    for (tx=0; tx<columns; tx++) {
		if (img->tile[ty * columns + tx]) {
		    dirty++;
		    if (tx < tx1)
			tx1 = tx;
		    if (tx > tx2)
			tx2 = tx;
		    if (ty < ty1)
			ty1 = ty;
		    if (ty > ty2)
			ty2 = ty;
		}
		else {
		}
	    }
	}
	if (img->ignore_sync) {
	    img->ignore_sync = FALSE;
	    image_sync(img);
	}
	else if ((tx2 >= tx1) && (ty2 >= ty1)) {
	    /* redraw it */
	    int tx3;
	    if (dirty * 10 < (tx2 - tx1 + 1) * (ty2 - ty1 + 1)) {
		/* draw individual tiles */
		/* but merge horizontally adjacent tiles together */
		for (ty=ty1; ty<=ty2; ty++) {
		    for (tx=tx1; tx<=tx2; tx++) {
			if (img->tile[ty*columns + tx]) {
			    /* look for adjacent tiles */
			    for (tx3 = tx+1; tx3 <= tx2; tx3++) {
				if (!img->tile[ty*columns + tx3])
				    break;
			    }
			    tx3--;
			    image_draw_tile(img, tx, ty, tx3, ty);
			}
		    }
		}
	    }
	    else {
		image_draw_tile(img, tx1, ty1, tx2, ty2);
	    }
		
	}
	memset(img->tile, 0, sizeof(img->tile[0]) * rows * columns);
	img->tile_dirty = 0;	/* all tiles will be cleaned */
    }
}

/* Poll the caller for cooperative multitasking. */
/* If this function is NULL, polling is not needed */
static int display_update(void *handle, void *device, 
    int x, int y, int w, int h)
{
    VIEW *view = (VIEW *)handle;
    IMAGE *img = view_get_image(view);
    image_tile_update(img, x, y, w, h);

    if (img->lock_count != 0)
	return 0; /* prevent update during image resize on single thread */
    return view_poll((VIEW *)handle);
}


display_callback gsdisplay = { 
    sizeof(display_callback),
    DISPLAY_VERSION_MAJOR,
    DISPLAY_VERSION_MINOR,
    display_open,
    display_preclose,
    display_close,
    display_presize,
    display_size,
    display_sync,
    display_page,
    display_update,
#ifdef OS2
    display_memalloc,
    display_memfree
#else
    NULL,	/* memalloc */
    NULL	/* memfree */
#endif
};


