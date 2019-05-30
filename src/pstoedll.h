/*
   pstoedll.h : This file describes the interface to query information about
   the drivers available via pstoedit and to call pstoedit via the dll interface
  
   Copyright (C) 1998 - 1999 Wolfgang Glunz, wglunz@geocities.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

/* 
   the struct version of DriverDescription 
   this is needed in order to allow plain old C programs to use the .dll
*/

struct DriverDescription_S {
	char *	symbolicname;
	char *	explanation;
	char *	suffix;
	char *	additionalInfo;
	int		backendSupportsSubPathes;
	int		backendSupportsCurveto;
	int 	backendSupportsMerging; 
	int 	backendSupportsText;
	int 	backendSupportsImages;
	int		backendSupportsMultiplePages;
};

static unsigned int pstoeditdllversion = 301;
// 301: added the clearPstoeditDriverInfo function to avoid problems with using different mallac/free in server and client.

typedef int  (pstoedit_checkversion_func) (unsigned int callersversion );

typedef int  (pstoedit_plainC_func) (int argc,const char * const argv[],const char * const psinterpreter );
/* psinterpreter can be set to 0, in which case pstoedit tries to locate one on his own. */
typedef struct DriverDescription_S * (getPstoeditDriverInfo_plainC_func)(void);
/* returned result must be freed via a call to clearPstoeditDriverInfo_plainC !! */
/* the end of the array is indicated by p->symbolicname == 0 */
typedef void  (clearPstoeditDriverInfo_plainC_func)(struct DriverDescription_S * ptr);

typedef int  (write_callback_func) (void * cb_data, const char* text, unsigned long length);
typedef void (setPstoeditOutputFunction_func)(void * cbData,write_callback_func* cbFunction);
 
