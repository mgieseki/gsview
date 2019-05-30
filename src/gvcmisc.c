/* Copyright (C) 1993-2005, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvcmisc.c */
/* Miscellaneous GSview routines common to Windows and PM */

#include "gvc.h"

int read_usermedia_profile(PROFILE *prf);

/* returns error code from GS */
void
gs_addmessf(const char *fmt, ...)
{
va_list args;
int count;
char buf[1024];
	va_start(args,fmt);
	count = vsprintf(buf,fmt,args);
        gs_addmess(buf);
	va_end(args);
}

void
make_cwd(const char *filename)
{
    char temp[MAXSTR];
    int i;
    strncpy(temp, filename, sizeof(temp)-1);
    for (i=strlen(temp)-1; i>=0; i--) {
	if ((temp[i]=='\\') || (temp[i]=='/'))
	    break;
    }
    
    if (i > 0) {
	temp[i] = '\0';
	gs_chdir(temp);
    }
    else {
	if (strlen(temp) > 2) {
	    if (isalpha((int)(temp[0])) && (temp[1]==':')) {
		temp[2] = '\0';
		gs_chdir(temp);
	    }
	}
    }
}


/* display error message and exit Ghostscript message */
void 
error_message(char *str)
{
    message_box_a(str, MB_ICONHAND);
    post_img_message(WM_CLOSE, 0);
}

#if defined(_Windows) || defined(OS2)
void
info_init(HWND hwnd)
{
    CDSC *dsc = psfile.dsc;
    char buf[MAXSTR];
    TCHAR wbuf[MAXSTR];
    TCHAR *p;
    int n;
    if (psfile.name[0] != '\0') {
	SetDlgItemTextA(hwnd, INFO_FILE, psfile.name);
	if (dsc) {
	    p = wbuf;
	    *p = '\0';
	    if (psfile.gzip) {
		lstrcpy(p, TEXT("gzip "));
	        p += lstrlen(p);
	    }
	    if (psfile.bzip2) {
		lstrcpy(p, TEXT("bzip2 "));
	        p += lstrlen(p);
	    }
	    if (psfile.ctrld)
		load_string(IDS_CTRLD, p, sizeof(wbuf)/sizeof(TCHAR)-1);
	    if (psfile.pjl)
		load_string(IDS_PJL, p, sizeof(wbuf)/sizeof(TCHAR)-1);
	    p += lstrlen(p);
	    if (psfile.ispdf) {
		load_string(IDS_PDF, p, 
			sizeof(wbuf)/sizeof(TCHAR)-1-lstrlen(wbuf));
	    }
	    else if (dsc->epsf) {
		switch (psfile.preview) {
		    case IDS_EPSI:
		      load_string(IDS_EPSI, p, 
			sizeof(wbuf)/sizeof(TCHAR)-1-lstrlen(wbuf));
		      break;
		    case IDS_EPST:
		      load_string(IDS_EPST, p, 
			sizeof(wbuf)/sizeof(TCHAR)-1-lstrlen(wbuf));
		      break;
		    case IDS_EPSW:
		      load_string(IDS_EPSW, p, 
			sizeof(wbuf)/sizeof(TCHAR)-1-lstrlen(wbuf));
		      break;
		    default:
		      load_string(IDS_EPSF, p, 
			sizeof(wbuf)/sizeof(TCHAR)-1-lstrlen(wbuf));
		}
	    }
	    else
		load_string(IDS_DSC, p, 
		    sizeof(wbuf)/sizeof(TCHAR)-1-lstrlen(wbuf));
	    SetDlgItemText(hwnd, INFO_TYPE, wbuf);
	    SetDlgItemTextA(hwnd, INFO_TITLE, 
		dsc->dsc_title ? dsc->dsc_title : "");
	    SetDlgItemTextA(hwnd, INFO_DATE, 
		dsc->dsc_date ? dsc->dsc_date : "");
	    if (dsc->bbox)
		sprintf(buf, "%d %d %d %d", dsc->bbox->llx, dsc->bbox->lly, 
		    dsc->bbox->urx, dsc->bbox->ury);
	    else
		buf[0]='\0';
	    SetDlgItemTextA(hwnd, INFO_BBOX, buf);
	    switch(dsc->page_orientation) {
		case CDSC_LANDSCAPE:
			load_string(IDS_LANDSCAPE, wbuf, 
			    sizeof(wbuf)/sizeof(TCHAR)-1);
			break;
		case CDSC_PORTRAIT:
			load_string(IDS_PORTRAIT, wbuf, 
			    sizeof(wbuf)/sizeof(TCHAR)-1);
			break;
		default:
			wbuf[0] = '\0';
	    }
	    SetDlgItemText(hwnd, INFO_ORIENT, wbuf);
	    switch(dsc->page_order) {
		case CDSC_ASCEND: 
			load_string(IDS_ASCEND, wbuf, 
			    sizeof(wbuf)/sizeof(TCHAR)-1);
			break;
		case CDSC_DESCEND: 
			load_string(IDS_DESCEND, wbuf, 
			    sizeof(wbuf)/sizeof(TCHAR)-1);
			break;
		case CDSC_SPECIAL:
			load_string(IDS_SPECIAL, wbuf, 
			    sizeof(wbuf)/sizeof(TCHAR)-1);
			break;
		default:
			wbuf[0] = '\0';
	    }
	    SetDlgItemText(hwnd, INFO_ORDER, wbuf);
	    if (dsc->page_media && dsc->page_media->name) {
		sprintf(buf,"%.200s %g %g",dsc->page_media->name,
		    dsc->page_media->width, dsc->page_media->height);
	    }
	    else {
		buf[0] = '\0';
	    }
	    SetDlgItemTextA(hwnd, INFO_DEFMEDIA, buf);
	    sprintf(buf, "%d", dsc->page_count);
	    SetDlgItemTextA(hwnd, INFO_NUMPAGES, buf);
	    n = map_page(psfile.pagenum - 1);
	    if (dsc->page_count)
		    sprintf(buf, "\"%.200s\"   %d", dsc->page[n].label ? dsc->page[n].label : "", psfile.pagenum);
	    else
		    buf[0] = '\0';
	    SetDlgItemTextA(hwnd, INFO_PAGE, buf);
	
	}
	else {
	    if (option.ignore_dsc)
	        load_string(IDS_IGNOREDSC, wbuf, sizeof(wbuf)/sizeof(TCHAR)-1);
	    else
	        load_string(IDS_NOTDSC, wbuf, sizeof(wbuf)/sizeof(TCHAR)-1);
	    SetDlgItemText(hwnd, INFO_TYPE, wbuf);
	}
	sprintf(buf, "%d x %d", view.img->width, view.img->height);
	SetDlgItemTextA(hwnd, INFO_BITMAP, buf);
    }
    else {
	load_string(IDS_NOFILE, wbuf, sizeof(wbuf)/sizeof(TCHAR)-1);
	SetDlgItemText(hwnd, INFO_FILE, wbuf);
    }
}
#endif

int
read_usermedia_profile(PROFILE *prf)
{
    const char *section = "UserMedia";
    char *entries, *p;
    int i, numentry;
    int width, height;
    char buf[MAXSTR];
    entries = (char *)malloc(PROFILE_SIZE);
    if (entries == (char *)NULL)
       return 0;
    profile_read_string(prf, section, NULL, "", entries, PROFILE_SIZE);
    p = entries;
    for (numentry=0; p!=(char *)NULL && strlen(p)!=0; numentry++)
	p += strlen(p) + 1;
    numentry = min(numentry, (int)(sizeof(usermedia)/sizeof(USERMEDIA)));
    p = entries;
    for (i=0; i<numentry; i++) {
	profile_read_string(prf, section, p, "", buf, sizeof(buf));
	if (sscanf(buf, "%d,%d", &width, &height) == 2) {
	    strncpy(usermedia[i].name, p, sizeof(usermedia[i].name)-1);
	    usermedia[i].id = i + IDM_USERSIZE1;
	    usermedia[i].width = width;
	    usermedia[i].height = height;
	}
	p += strlen(p) + 1;
    }
    free(entries);
    return numentry;
}


/* read settings fron INI file */
void
read_profile(const char *ininame)
{
int i, j;
char profile[MAXSTR];
const char *section = INISECTION;
char secver[MAXSTR];
PROFILE *prf;
    if (debug & DEBUG_GENERAL)
	gs_addmessf("Reading profile \042%s\042\n", ininame);
    prf = profile_open(ininame);

    /* GSview-N.N */
    sprintf(secver, "GSview-%s", GSVIEW_DOT_VERSION);
    profile_read_string(prf, secver, "Configured", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	option.configured = i;
    else
	option.configured = FALSE;
    profile_read_string(prf, secver, "GSversion", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	option.gsversion = i;
    else
	option.gsversion = GS_REVISION;
    if ( (option.gsversion < GS_REVISION_MIN) ||
	 (option.gsversion > GS_REVISION_MAX) )
	option.gsversion = GS_REVISION;
    profile_read_string(prf, secver, "Version", "", profile, sizeof(profile));
    if (strcmp(profile, GSVIEW_DOT_VERSION)!=0)
	option.configured = FALSE;
    profile_read_string(prf, secver, "GhostscriptDLL", "", profile, sizeof(profile));
    if (profile[0] != '\0')	/* don't copy a default - assume already set */
	    strncpy(option.gsdll, profile, MAXSTR-1);
    profile_read_string(prf, secver, "GhostscriptEXE", "", profile, sizeof(profile));
    if (profile[0] != '\0')	/* don't copy a default - assume already set */
	    strncpy(option.gsexe, profile, MAXSTR-1);
    profile_read_string(prf, secver, "GhostscriptInclude", "", profile, sizeof(profile));
    if (profile[0] != '\0')	/* don't copy a default - assume already set */
	    strncpy(option.gsinclude, profile, MAXSTR-1);
    profile_read_string(prf, secver, "GhostscriptOther", "", profile, sizeof(profile));
    if (profile[0] != '\0')	/* don't copy a default - assume already set */
	    strncpy(option.gsother, profile, MAXSTR-1);

    /* Options */
    profile_read_string(prf, section, "Language", "", profile, sizeof(profile));
    option.language = language_id(profile);
    if (option.language == 0)
	option.language = IDM_LANGEN;
    profile_read_string(prf, section, "HelpCmd", "", profile, sizeof(profile));
    if (profile[0] != '\0')	/* don't copy a default - assume already set */
	    strncpy(option.helpcmd, profile, MAXSTR-1);
    profile_read_string(prf, section, "Origin", "", profile, sizeof(profile));
    option.img_origin.x = option.img_origin.y = CW_USEDEFAULT;
    if (sscanf(profile,"%d %d", &i, &j) == 2) {
	    option.img_origin.x = i;
	    option.img_origin.y = j;
    }
    profile_read_string(prf, section, "Size", "", profile, sizeof(profile));
    option.img_size.x = option.img_size.y = CW_USEDEFAULT;
    if (sscanf(profile,"%d %d", &i, &j) == 2) {
	    option.img_size.x = i;
	    option.img_size.y = j;
    }
    if ((option.img_size.x<32) || (option.img_size.y<32))
	    option.img_size.x = option.img_size.y = CW_USEDEFAULT;
    profile_read_string(prf, section, "Maximized", "", profile, sizeof(profile));
    option.img_max = 0;
    if (sscanf(profile,"%d", &i) == 1)
	    option.img_max = i;
    profile_read_string(prf, section, "SaveSettings", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.settings = i;
    profile_read_string(prf, section, "ButtonBar", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.button_show = i;
    profile_read_string(prf, section, "FitWindowToPage", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.fit_page = i;
    profile_read_string(prf, section, "Resolution", "", profile, sizeof(profile));
    if (sscanf(profile,"%f %f", &option.xdpi, &option.ydpi) != 2) {
	    option.xdpi = option.ydpi = DEFAULT_RESOLUTION;
    }
    profile_read_string(prf, section, "ZoomResolution", "", profile, sizeof(profile));
    if (sscanf(profile,"%f %f", &option.zoom_xdpi, &option.zoom_ydpi) != 2) {
	    option.zoom_xdpi = option.zoom_ydpi = DEFAULT_ZOOMRES;
    }
    profile_read_string(prf, section, "Depth", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.depth = i;
    profile_read_string(prf, section, "TextAlphaBits", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.alpha_text = i;
    if (option.alpha_text <= 0)
	    option.alpha_text = 1;
    profile_read_string(prf, section, "GraphicsAlphaBits", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.alpha_graphics = i;
    if (option.alpha_graphics <= 0)
	    option.alpha_graphics = 1;
    profile_read_string(prf, section, "Media", "", profile, sizeof(profile));
    if (strlen(profile)!=0)
	strncpy(option.medianame, profile, sizeof(option.medianame));
    profile_read_string(prf, section, "MediaRotate", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.media_rotate = i;
    profile_read_string(prf, section, "UserSizeWarn", "", profile, sizeof(profile));
    if (sscanf(profile,"%d %d", &option.user_width_warn, 
	&option.user_height_warn) != 2) {
	    option.user_width_warn = 5669;	/* 2 metres */
	    option.user_height_warn = 5669;
    }
    profile_read_string(prf, section, "UserSize", "", profile, sizeof(profile));
    if (sscanf(profile,"%d %d", &option.user_width, &option.user_height) != 2) {
	    /* this gives 640x480 pixels at 96dpi */
	    option.user_width = 480;
	    option.user_height = 360;
    }
    gsview_check_usersize();
    profile_read_string(prf, section, "EpsfClip", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.epsf_clip = i;
    profile_read_string(prf, section, "EpsfWarn", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.epsf_warn = i;
    profile_read_string(prf, section, "IgnoreDSC", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.ignore_dsc = i;
    profile_read_string(prf, section, "DSCWarn", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.dsc_warn = i+IDM_DSC_OFF;
    profile_read_string(prf, section, "ShowBBox", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.show_bbox = i;
    profile_read_string(prf, section, "AutoOrientation", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.auto_orientation = i;
    profile_read_string(prf, section, "Orientation", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.orientation = i+IDM_PORTRAIT;
    profile_read_string(prf, section, "SwapLandscape", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.swap_landscape = i;
    profile_read_string(prf, section, "Unit", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.unit = i+IDM_UNITPT;
    if ((option.unit < IDM_UNITPT) || (option.unit > IDM_UNITINCH))
	    option.unit = IDM_UNITPT;
    profile_read_string(prf, section, "UnitFine", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.unitfine = i;
    profile_read_string(prf, section, "PStoText", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.pstotext = i;
    profile_read_string(prf, section, "Safer", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.safer = i;
    profile_read_string(prf, section, "AutoRedisplay", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.redisplay = i;
    profile_read_string(prf, section, "AutoBoundingBox", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.auto_bbox = i;
    profile_read_string(prf, section, "SaveLastDir", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.save_dir = i;
    if (option.save_dir) {
	char workdir[MAXSTR];
	gs_getcwd(workdir, sizeof(workdir));	/* save current in case chdir fails */
	profile_read_string(prf, section, "LastDir", "", profile, sizeof(profile));
	if (gs_chdir(profile))
	    gs_chdir(workdir);
    }
    profile_read_string(prf, section, "DrawMethod", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1) {
	switch (i) {
	    case (IDM_DRAWGPI-IDM_DRAWMENU):
		option.drawmethod = IDM_DRAWGPI;
		break;
	    case (IDM_DRAWWIN-IDM_DRAWMENU):
		option.drawmethod = IDM_DRAWWIN;
		break;
	    default:
	    case (IDM_DRAWDEF-IDM_DRAWMENU):
		option.drawmethod = IDM_DRAWDEF;
		break;
	}
    }

    profile_read_string(prf, section, "ProgressiveUpdate", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1) {
	option.update = i;
    }

    profile_read_string(prf, section, "PrinterDevice", "", profile, sizeof(profile));
    if (strlen(profile)!=0)
	strncpy(option.printer_device, profile, sizeof(option.printer_device)-1);
    profile_read_string(prf, section, "PrinterResolution", "", profile, sizeof(profile));
    if (strlen(profile)!=0)
	strncpy(option.printer_resolution, profile, sizeof(option.printer_resolution)-1);

    profile_read_string(prf, section, "PrintFixedMedia", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	option.print_fixed_media = i;

    profile_read_string(prf, section, "ConvertDevice", "", profile, sizeof(profile));
    if (strlen(profile)!=0)
	strncpy(option.convert_device, profile, sizeof(option.convert_device)-1);
    profile_read_string(prf, section, "ConvertResolution", "", profile, sizeof(profile));
    if (strlen(profile)!=0)
	strncpy(option.convert_resolution, profile, sizeof(option.convert_resolution)-1);

    profile_read_string(prf, section, "ConvertFixedMedia", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	option.convert_fixed_media = i;

    profile_read_string(prf, section, "PrintGDIDepth", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	option.print_gdi_depth = i+IDC_MONO;

    profile_read_string(prf, section, "PrintGDIFixedMedia", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	option.print_gdi_fixed_media = i;


    profile_read_string(prf, section, "PrinterPort", "", profile, sizeof(option.printer_port)-1);
    if (profile[0] != '\0')
	strncpy(option.printer_port, profile, sizeof(option.printer_port)-1);
    profile_read_string(prf, section, "PrinterQueue", "", profile, sizeof(option.printer_queue)-1);
    if (profile[0] != '\0')
	strncpy(option.printer_queue, profile, sizeof(option.printer_queue)-1);
    profile_read_string(prf, section, "PrintToFile", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.print_to_file = i;
    profile_read_string(prf, section, "PrintMethod", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.print_method = i;
    profile_read_string(prf, section, "PrintReverse", "", profile, sizeof(profile));
    if (sscanf(profile,"%d", &i) == 1)
	    option.print_reverse = i;
    for (i=0; i<NUMSOUND; i++) {
	char buf[MAXSTR];
	convert_widechar(buf, sound[i].file, sizeof(buf)-1);
	profile_read_string(prf, section, sound[i].entry, buf, 
	    profile, sizeof(profile));
	convert_multibyte(sound[i].file, profile, 
	    sizeof(sound[i].file)/sizeof(TCHAR)-1);
    }
    profile_read_string(prf, section, "LastFile1", "", profile,sizeof(profile));
    if (profile[0])
	strncpy(last_files[0], profile, sizeof(last_files[0])-1);
    profile_read_string(prf, section, "LastFile2", "", profile,sizeof(profile));
    if (profile[0])
	strncpy(last_files[1], profile, sizeof(last_files[1])-1);
    profile_read_string(prf, section, "LastFile3", "", profile,sizeof(profile));
    if (profile[0])
	strncpy(last_files[2], profile, sizeof(last_files[2])-1);
    profile_read_string(prf, section, "LastFile4", "", profile,sizeof(profile));
    if (profile[0])
	strncpy(last_files[3], profile, sizeof(last_files[3])-1);
    for (last_files_count=0; last_files_count<4; last_files_count++)
	if (strlen(last_files[last_files_count])==0)
	    break;

#ifndef VIEWONLY
    read_measure_profile(prf);
#endif

    read_usermedia_profile(prf);

    profile_close(prf);
}

/* write settings to INI file */
void
write_profile(void)
{
char profile[MAXSTR];
const char *section = INISECTION;
int i;
PROFILE *prf;
char secver[MAXSTR];
	if (debug & DEBUG_GENERAL)
	    gs_addmessf("Writing profile \042%s\042\n", szIniFile);
	prf = profile_open(szIniFile);
	if (prf == (PROFILE *)NULL) {
	    message_box_a("profile_open() failed, no memory\n", 0);
	    return;
	}

        /* GSview-N.N */
	sprintf(secver, "GSview-%s", GSVIEW_DOT_VERSION);
	profile_write_string(prf, secver, "Version", GSVIEW_DOT_VERSION);
	sprintf(profile, "%d", (int)option.gsversion);
	profile_write_string(prf, secver, "GSversion", profile);
	sprintf(profile, "%d", (int)option.configured);
	profile_write_string(prf, secver, "Configured", profile);
	profile_write_string(prf, secver, "GhostscriptDLL", option.gsdll);
	profile_write_string(prf, secver, "GhostscriptEXE", option.gsexe);
	profile_write_string(prf, secver, "GhostscriptInclude", option.gsinclude);
	profile_write_string(prf, secver, "GhostscriptOther", option.gsother);
	sprintf(profile, "%d %d", (int)option.img_origin.x, (int)option.img_origin.y);
	/* Options */
	profile_write_string(prf, section, "Language", 
	    language_twocc(option.language));
	profile_write_string(prf, section, "HelpCmd", option.helpcmd);
	profile_write_string(prf, section, "Origin", profile);
	sprintf(profile, "%d %d", (int)option.img_size.x, (int)option.img_size.y);
	profile_write_string(prf, section, "Size", profile);
	sprintf(profile, "%d", (int)option.img_max);
	profile_write_string(prf, section, "Maximized", profile);
	sprintf(profile, "%d", (int)option.settings);
	profile_write_string(prf, section, "SaveSettings", profile);
	sprintf(profile, "%d", (int)option.button_show);
	profile_write_string(prf, section, "ButtonBar", profile);
	sprintf(profile, "%d", (int)option.fit_page);
	profile_write_string(prf, section, "FitWindowToPage", profile);
	sprintf(profile, "%g %g", option.xdpi, option.ydpi);
	profile_write_string(prf, section, "Resolution", profile);
	sprintf(profile, "%g %g", option.zoom_xdpi, option.zoom_ydpi);
	profile_write_string(prf, section, "ZoomResolution", profile);
	sprintf(profile, "%d", option.depth);
	profile_write_string(prf, section, "Depth", profile);
	sprintf(profile, "%d", option.alpha_text);
	profile_write_string(prf, section, "TextAlphaBits", profile);
	sprintf(profile, "%d", option.alpha_graphics);
	profile_write_string(prf, section, "GraphicsAlphaBits", profile);
	if (option.media == IDM_USERSIZE)
	    strcpy(profile, MEDIA_USERDEFINED);
	else
	    strcpy(profile, option.medianame);
	profile_write_string(prf, section, "Media", profile);
	sprintf(profile, "%d", (int)option.media_rotate);
	profile_write_string(prf, section, "MediaRotate", profile);
	sprintf(profile, "%u %u", option.user_width, option.user_height);
	profile_write_string(prf, section, "UserSize", profile);
	sprintf(profile, "%u %u", option.user_width_warn, 
		option.user_height_warn);
	profile_write_string(prf, section, "UserSizeWarn", profile);
	sprintf(profile, "%d", (int)option.epsf_clip);
	profile_write_string(prf, section, "EpsfClip", profile);
	sprintf(profile, "%d", (int)option.epsf_warn);
	profile_write_string(prf, section, "EpsfWarn", profile);
	sprintf(profile, "%d", (int)option.ignore_dsc);
	profile_write_string(prf, section, "IgnoreDSC", profile);
	sprintf(profile, "%d", (int)option.dsc_warn - IDM_DSC_OFF);
	profile_write_string(prf, section, "DSCWarn", profile);
	sprintf(profile, "%d", (int)option.show_bbox);
	profile_write_string(prf, section, "ShowBBox", profile);
	sprintf(profile, "%d", (int)option.auto_orientation);
	profile_write_string(prf, section, "AutoOrientation", profile);
	sprintf(profile, "%d", option.orientation - IDM_PORTRAIT);
	profile_write_string(prf, section, "Orientation", profile);
	sprintf(profile, "%d", (int)option.swap_landscape);
	profile_write_string(prf, section, "SwapLandscape", profile);
	sprintf(profile, "%d", option.unit - IDM_UNITPT);
	profile_write_string(prf, section, "Unit", profile);
	sprintf(profile, "%d", (int)option.unitfine);
	profile_write_string(prf, section, "UnitFine", profile);
	sprintf(profile, "%d", (int)option.pstotext);
	profile_write_string(prf, section, "PStoText", profile);
	sprintf(profile, "%d", (int)option.safer);
	profile_write_string(prf, section, "Safer", profile);
	sprintf(profile, "%d", (int)option.redisplay);
	profile_write_string(prf, section, "AutoRedisplay", profile);
	sprintf(profile, "%d", (int)option.auto_bbox);
	profile_write_string(prf, section, "AutoBoundingBox", profile);
	sprintf(profile, "%d", (int)option.save_dir);
	profile_write_string(prf, section, "SaveLastDir", profile);
	if (option.save_dir) {
	    gs_getcwd(profile, sizeof(profile));
	    profile_write_string(prf, section, "LastDir", profile);
	}
	sprintf(profile, "%d", (option.drawmethod - IDM_DRAWMENU));
	profile_write_string(prf, section, "DrawMethod", profile);
	sprintf(profile, "%d", option.update);
	profile_write_string(prf, section, "ProgressiveUpdate", profile);

	profile_write_string(prf, section, "PrinterPort", option.printer_port);
	profile_write_string(prf, section, "PrinterQueue", 
		option.printer_queue);
	if (option.printer_device[0] != '\0')
            profile_write_string(prf, section, "PrinterDevice", 
		option.printer_device);
	if (option.printer_resolution[0] != '\0')
            profile_write_string(prf, section, "PrinterResolution", 
		option.printer_resolution);
	sprintf(profile, "%d", (int)option.print_fixed_media);
	profile_write_string(prf, section, "PrintFixedMedia", profile);

	if (option.convert_device[0] != '\0')
            profile_write_string(prf, section, "ConvertDevice", 
		option.convert_device);
	if (option.convert_resolution[0] != '\0')
            profile_write_string(prf, section, "ConvertResolution", 
		option.convert_resolution);
	sprintf(profile, "%d", (int)option.convert_fixed_media);
	profile_write_string(prf, section, "ConvertFixedMedia", profile);

	sprintf(profile, "%d", (int)(option.print_gdi_depth-IDC_MONO));
	profile_write_string(prf, section, "PrintGDIDepth", profile);
	sprintf(profile, "%d", (int)option.print_gdi_fixed_media);
	profile_write_string(prf, section, "PrintGDIFixedMedia", profile);

	sprintf(profile, "%d", (int)option.print_to_file);
	profile_write_string(prf, section, "PrintToFile", profile);
	sprintf(profile, "%d", (int)option.print_method);
	profile_write_string(prf, section, "PrintMethod", profile);
	sprintf(profile, "%d", (int)option.print_reverse);
	profile_write_string(prf, section, "PrintReverse", profile);
	for (i=0; i<NUMSOUND; i++) {
	    char buf[MAXSTR];
	    convert_widechar(buf, sound[i].file, sizeof(buf)-1);
	    profile_write_string(prf, section, sound[i].entry, buf);
	}

	profile_write_string(prf, section, "LastFile1", last_files[0]);
	profile_write_string(prf, section, "LastFile2", last_files[1]);
	profile_write_string(prf, section, "LastFile3", last_files[2]);
	profile_write_string(prf, section, "LastFile4", last_files[3]);

#ifndef VIEWONLY
	write_measure_profile(prf);
#endif

	profile_close(prf);
}

void
write_profile_last_files(void)
{
const char *section = INISECTION;
PROFILE *prf;
	prf = profile_open(szIniFile);
	profile_write_string(prf, section, "LastFile1", last_files[0]);
	profile_write_string(prf, section, "LastFile2", last_files[1]);
	profile_write_string(prf, section, "LastFile3", last_files[2]);
	profile_write_string(prf, section, "LastFile4", last_files[3]);
	profile_close(prf);
}

#ifdef DEBUG_MALLOC
FILE *malloc_file;
#define MALLOC_FILE "c:\\gsview.txt"
#endif

#undef malloc
#undef calloc
#undef realloc
#undef free
long allocated_memory = 0;

void * debug_malloc(size_t size)
{
    if (debug & DEBUG_MEM) {
	char buf[MAXSTR];
	void *p = malloc(size+sizeof(long));
	long *pl = (long *)p; 
	if (pl) {
	    *pl = size;
	    allocated_memory += size;
	    pl++;
	}
#ifdef NOTUSED
if (size == 4096)
gs_addmess("\r\nstop here\r\n");
#endif
	sprintf(buf, "malloc(%ld) %p, allocated = %ld\r\n", 
	    (long)size, pl, allocated_memory);
	gs_addmess(buf);
#ifdef DEBUG_MALLOC
	if (malloc_file == (FILE *)NULL)
	    malloc_file = fopen(MALLOC_FILE, "wb");
	if (malloc_file != (FILE *)NULL) {
	    fputs(buf, malloc_file);
	    fflush(malloc_file);
	}
#endif
	return (void *)pl;
    }

    return malloc(size);
}


void * debug_realloc(void *block, size_t size)
{
    if (debug & DEBUG_MEM) {
	char buf[MAXSTR];
	long *pl = (long *)block; 
	long oldsize = 0;
	if (pl) {
	    pl--;
	    oldsize = *pl;
	    allocated_memory -= oldsize;
	    pl = (long *)realloc((void *)pl, size+sizeof(long));
	    if (pl) {
		*pl = size;
		allocated_memory += size;
		pl++;
	    }
	}
	sprintf(buf, "realloc old %ld %p, new %ld %p, allocated = %ld\r\n",
	    oldsize, block, (long)size, pl, allocated_memory);
	gs_addmess(buf);
#ifdef DEBUG_MALLOC
	if (malloc_file == (FILE *)NULL)
	    malloc_file = fopen(MALLOC_FILE, "wb");
	if (malloc_file != (FILE *)NULL) {
	    fputs(buf, malloc_file);
	    fflush(malloc_file);
	}
#endif
	return (void *)pl;
    }

    return realloc(block, size);
}

void debug_free(void *block)
{
    if (debug & DEBUG_MEM) {
	char buf[MAXSTR];
	long *pl = (long *)block; 
	long oldsize = 0;
	if (pl) {
	    pl--;
	    oldsize = *pl;
	    allocated_memory -= oldsize;
	    free((void *)pl);
	}
	sprintf(buf, "free %ld %p, allocated = %ld\r\n",
	    oldsize, block, allocated_memory);
	gs_addmess(buf);
#ifdef DEBUG_MALLOC
	if (malloc_file == (FILE *)NULL)
	    malloc_file = fopen(MALLOC_FILE, "wb");
	if (malloc_file != (FILE *)NULL) {
	    fputs(buf, malloc_file);
	    fflush(malloc_file);
	}
#endif
	return;
    }

    free(block);
    return;
}

void debug_memory_report(void)
{
    if (debug & DEBUG_MEM) {
	char buf[256];
	sprintf(buf, "Allocated memory = %ld bytes", allocated_memory);
	message_box_a(buf, 0);
#ifdef DEBUG_MALLOC
	fclose(malloc_file);
#endif
    }
}
