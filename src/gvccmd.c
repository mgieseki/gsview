/* Copyright (C) 1993-2003, Ghostgum Software Pty Ltd.  All rights reserved.
  
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

/* gvccmd.c */
/* Menu command module of PM and Windows GSview */

#include "gvc.h"

void gsview_drawmethod(int new_drawmethod);
BOOL gsview_usersize(void);
void gsview_unzoom(void);

/* gsview menu commands */
int
gsview_command(int command)
{
    switch (command) {
	case IDM_NEXTHOME:
	case IDM_NEXT:
	case IDM_NEXTSKIP:
	case IDM_REDISPLAY:
	case IDM_PREVHOME:
	case IDM_PREVSKIP:
	case IDM_PREV:
	case IDM_GOBACK:
	case IDM_GOFWD:
	case IDM_MAGPLUS:
	case IDM_MAGMINUS:
	case IDM_FITWIN:
	case IDM_ZOOM:
	case IDM_FULLSCREEN:
	    /* These don't close the full screen window */
	    break;
	default:
	    gsview_fullscreen_end();
    }
    switch (command) {
	case IDM_FULLSCREEN:
		gsview_fullscreen();
		return 0;
	case IDM_OPEN:
		if (pending.psfile) {
		    play_sound(SOUND_BUSY);
		    return 0;
		}
		gsview_display();
		return 0;
	case IDM_LASTFILE1:
	case IDM_LASTFILE2:
	case IDM_LASTFILE3:
	case IDM_LASTFILE4:
		if (pending.psfile) {
		    play_sound(SOUND_BUSY);
		    return 0;
		}
		make_cwd(last_files[command-IDM_LASTFILE1]);
		gsview_displayfile(last_files[command-IDM_LASTFILE1]);
		return 0;
	case IDM_CLOSE:
		/* doesn't unload DLL */
		/* close file */
	  	if (gsdll.open && (gsdll.state != GS_UNINIT)) {
		    PSFILE *tpsfile;
		    if (pending.psfile) {
			play_sound(SOUND_BUSY);
			return 0;
		    }
		    tpsfile = (PSFILE *)malloc(sizeof(PSFILE));
		    if (tpsfile == NULL)
			return 0;
		    memset((char *)tpsfile, 0, sizeof(PSFILE));
		    pending.psfile = tpsfile;
		    pending.now = TRUE;
		    if (psfile.name[0] && psfile.dsc==(CDSC *)NULL)
			pending.abort = TRUE;
		}
		else {
		    /* DLL isn't loaded */
		    if (psfile.file)
			dfclose();	/* just to make sure */
		    psfile_free(&psfile);
		    post_img_message(WM_GSTITLE, 0);
		    info_wait(IDS_NOWAIT);
		}
		return 0;
	case IDM_CLOSE_DONE:
		if (selectname[0] != '\0') {
		    /* pending IDM_SELECT */
		    PSFILE *tpsfile;
		    tpsfile = gsview_openfile(selectname);
		    if (tpsfile) {
		        psfile = *tpsfile;
		        free(tpsfile);
		    }
		    selectname[0] = '\0';
		    post_img_message(WM_GSTITLE, 0);
		    info_wait(IDS_NOWAIT);
		}
		return 0;
	case IDM_NEXTHOME:
#ifdef UNIX
		set_scroll(-1, 0);
#else
#ifdef _Windows
		PostMessage(hwnd_image ,WM_VSCROLL,SB_TOP,0L);
#else
		WinPostMsg(hwnd_frame, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_TOP));
#endif
#endif
		/* fall thru */
	case IDM_NEXT:
		if (not_open())
		    return 0;
		gs_page_skip(1);
		return 0;
	case IDM_NEXTSKIP:
		if (not_dsc())
		    return 0;
		if (order_is_special())
		    return 0;
		gs_page_skip(page_skip);
		return 0;
	case IDM_REDISPLAY:
		if (dfchanged()) {
		    PSFILE *tpsfile;
		    if (dfchanged() < 0) {
			gs_addmess("File has been deleted\n"); 
			gsview_command(IDM_CLOSE);
		        pending.unload = TRUE;
		        pending.now = FALSE;
			return 0;
		    }
		    if (pending.psfile)
		      tpsfile = pending.psfile;	/* new file, old file deleted */
		    else
		      tpsfile = gsview_openfile(psfile.name);
		    if (tpsfile) {
		      tpsfile->pagenum = psfile.pagenum;
		      request_mutex();
		      pending.psfile = tpsfile;
		      if ( gsdll.hmodule &&  (psfile.dsc==(CDSC *)NULL) && 
			  (gsdll.state != GS_IDLE) )
			  /* don't know where we are so close and reopen */
			  pending.abort = TRUE;
		      pending.now = TRUE;
		      release_mutex();
		    }
		}

		if (not_open())
		    return 0;
		if (psfile.dsc==(CDSC *)NULL) {
		    /* don't know where we are so close and reopen */
		    if (gsdll.state != GS_IDLE) {
			if (!pending.psfile) {
			    pending.psfile = (PSFILE *)malloc(sizeof(PSFILE));
			    if (pending.psfile)
			        *pending.psfile = psfile;
			}
			pending.psfile->pagenum = pending.pagenum = 1;
			pending.abort = TRUE;
			pending.now = TRUE;
		    }
		}
		else {
		    pending.pagenum = -1;  /* default page number is current page */
		    if (psfile.dsc->page_order == CDSC_SPECIAL)
		        pending.pagenum = 1;	/* restart */
		}
		gsview_unzoom();
	        pending.now = TRUE;
		return 0;
	case IDM_PREVHOME:
#ifdef UNIX
		set_scroll(-1, 0);
#else
#ifdef _Windows
		PostMessage(hwnd_image ,WM_VSCROLL,SB_TOP,0L);
#else
		WinPostMsg(hwnd_frame, WM_VSCROLL, MPFROMLONG(0), MPFROM2SHORT(0, SB_TOP));
#endif
#endif
		/* fall thru */
	case IDM_PREV:
		if (not_dsc())
			return 0;
		if (order_is_special())
		    return 0;
		gs_page_skip(-1);
		return 0;
	case IDM_PREVSKIP:
		if (not_dsc())
			return 0;
		if (order_is_special())
		    return 0;
		gs_page_skip(-page_skip);
		return 0;
	case IDM_GOTO:
		if (not_dsc())
			return 0;
		if (order_is_special())
		    return 0;
		if (psfile.dsc->page_count == 0) {
		    gserror(IDS_NOPAGE, NULL, MB_ICONEXCLAMATION, SOUND_NONUMBER);
		    return 0;
		}
		nHelpTopic = IDS_TOPICGOTO;
		{ int pagenum;
		    pagenum = psfile.pagenum;
		    if (get_page(&pagenum, FALSE, FALSE)) {
			gsview_goto_page(pagenum);
		    }
		}
		return 0;
	case IDM_GOBACK:
		if (not_dsc())
		    return 0;
		if (order_is_special())
		    return 0;
		history_back();
		return 0;
	case IDM_GOFWD:
		if (not_open())
		    return 0;
	        if (psfile.dsc == (CDSC *)NULL)
		    gsview_command(IDM_NEXT);
		else
		    history_forward();
		return 0;
	case IDM_INFO:
		show_info();
		return 0;
	case IDM_SELECT:
		if (pending.psfile) {
		    play_sound(SOUND_BUSY);
		    return 0;
		}
		gsview_select();
		return 0;
#ifndef VIEWONLY
	case IDM_PRINT:
	case IDM_PRINTTOFILE:
	case IDM_CONVERTFILE:
		if (psfile.name[0] == '\0')
		    gsview_select();
		if (gsdll.state == GS_BUSY) {
		    play_sound(SOUND_BUSY);
		    return 0;
		}
		if (dfreopen() != 0)
		    return 0;
		if (command == IDM_PRINTTOFILE)
		    option.print_to_file = TRUE;
		if (psfile.name[0] != '\0')
		    gsview_print(command == IDM_CONVERTFILE);
		dfclose();
		return 0;
	case IDM_SPOOL:
		gsview_spool((char *)NULL, (char *)NULL);
		return 0;
	case IDM_SAVEAS:
		if (gsdll.state == GS_BUSY) {
		    play_sound(SOUND_BUSY);
		    return 0;
		}
		if (psfile.name[0] == '\0')
		    gsview_select();
		if (psfile.name[0] != '\0')
		    gsview_saveas();
		return 0;
	case IDM_EXTRACT:
		if (gsdll.state == GS_BUSY) {
		    play_sound(SOUND_BUSY);
		    return 0;
		}
		if (psfile.name[0] == '\0')
		    gsview_select();
		if (order_is_special())
		    return 0;
		if (psfile.name[0] != '\0')
		    gsview_extract();
		return 0;
	case IDM_PSTOEDIT:
		if (gsdll.state == GS_BUSY) {
		    play_sound(SOUND_BUSY);
		    return 0;
		}
		if (psfile.name[0] == '\0')
		    gsview_select();
		(void)order_is_special();    /* warn, but allow it anyway */
		if (dfreopen() != 0)
		    return 0;
		if (psfile.name[0] != '\0')
		    gsview_pstoedit();
		dfclose();
		return 0;
	case IDM_TEXTEXTRACT:
		if (psfile.name[0] == '\0')
		    gsview_select();
		if (psfile.name[0] != '\0')
		    gsview_text_extract();
		return 0;
	case IDM_TEXTEXTRACT_SLOW:
	        gsview_text_extract_slow();
		return 0;
	case IDM_TEXTFIND:
		gsview_text_find();
		return 0;
	case IDM_TEXTFINDNEXT:
		gsview_text_findnext();
		return 0;
	case IDM_COPYCLIP:
		copy_clipboard();
		return 0;
	case IDM_PASTETO:
		paste_to_file();
		return 0;
	case IDM_CONVERT:
		clip_convert();
		return 0;
	case IDM_MEASURE:
		if (gsdll.state == GS_BUSY) {
		    play_sound(SOUND_BUSY);
		    return 0;
		}
	        measure_show();
		return 0;
#endif /* !VIEWONLY */
	case IDM_GSMESS:
		gs_showmess();	/* show messages from Ghostscript */
		return 0;
	case IDM_EXIT:
		if (print_count) {
		    /* Still busy printing.  Warn user. */
		    TCHAR buf[MAXSTR];
		    load_string(IDS_BUSYPRINTING, buf, 
			sizeof(buf)/sizeof(TCHAR)-1);
		    if (message_box(buf, MB_OKCANCEL) != IDOK)
			return 0;
		}
		post_img_message(WM_CLOSE, 0);
		return 0;
	case IDM_CFG:	/* Easy configure */
		config_wizard(TRUE);
		return 0;
	case IDM_GSCOMMAND:	/* Advanced configure */
	        if (install_gsdll() && gsdll.open)
		    pending.unload = TRUE;
		return 0;
	case IDM_UNITPT:
	case IDM_UNITMM:
	case IDM_UNITINCH:
		gsview_unit(command);
		return 0;
	case IDM_UNITFINE:
		option.unitfine = !option.unitfine;
		check_menu_item(IDM_UNITMENU, IDM_UNITFINE, option.unitfine);
		return 0;
	case IDM_LANGMENU+1:
	case IDM_LANGMENU+2:
	case IDM_LANGMENU+3:
	case IDM_LANGMENU+4:
	case IDM_LANGMENU+5:
	case IDM_LANGMENU+6:
	case IDM_LANGMENU+7:
	case IDM_LANGMENU+8:
	case IDM_LANGMENU+9:
	case IDM_LANGMENU+10:
	case IDM_LANGMENU+11:
	case IDM_LANGMENU+12:
	case IDM_LANGMENU+13:
	case IDM_LANGMENU+14:
	case IDM_LANGMENU+15:
	case IDM_LANGMENU+16:
	case IDM_LANGMENU+17:
	case IDM_LANGMENU+18:
	case IDM_LANGMENU+19:
		gsview_language(command);
		return 0;
	case IDM_SAFER:
		option.safer = !option.safer;
		check_menu_item(IDM_OPTIONMENU, IDM_SAFER, option.safer);
		return 0;
	case IDM_SAVEDIR:
		option.save_dir = !option.save_dir;
		check_menu_item(IDM_OPTIONMENU, IDM_SAVEDIR, option.save_dir);
		return 0;
	case IDM_BUTTONSHOW:
		option.button_show = !option.button_show;
		check_menu_item(IDM_OPTIONMENU, IDM_BUTTONSHOW, option.button_show);
		show_buttons();
		return 0;
	case IDM_FITPAGE:
		option.fit_page = !option.fit_page;
		check_menu_item(IDM_OPTIONMENU, IDM_FITPAGE, option.fit_page);
		/* should cause WM_SIZE message to be sent */
		return 0;
	case IDM_PSTOTEXTDIS:
	case IDM_PSTOTEXTNORM:
	case IDM_PSTOTEXTCORK:
		check_menu_item(IDM_PSTOTEXTMENU, option.pstotext + IDM_PSTOTEXTMENU + 1, FALSE);
		option.pstotext = command - IDM_PSTOTEXTMENU - 1;
		check_menu_item(IDM_PSTOTEXTMENU, option.pstotext + IDM_PSTOTEXTMENU + 1, TRUE);
		return 0;
	case IDM_AUTOREDISPLAY:
		option.redisplay = !option.redisplay;
		check_menu_item(IDM_OPTIONMENU, IDM_AUTOREDISPLAY, option.redisplay);
		return 0;
	case IDM_EPSFCLIP:
		option.epsf_clip = !option.epsf_clip;
		check_menu_item(IDM_OPTIONMENU, IDM_EPSFCLIP, option.epsf_clip);
		gs_resize();
		return 0;
	case IDM_EPSFWARN:
		option.epsf_warn = !option.epsf_warn;
		check_menu_item(IDM_OPTIONMENU, IDM_EPSFWARN, option.epsf_warn);
		return 0;
	case IDM_IGNOREDSC:
		option.ignore_dsc = !option.ignore_dsc;
		check_menu_item(IDM_OPTIONMENU, IDM_IGNOREDSC, option.ignore_dsc);
		if (psfile.name[0]) {
		    if (option.redisplay)
		        gsview_displayfile(psfile.name);
		    else
		        gsview_selectfile(psfile.name);
		}
		return 0;
	case IDM_SHOWBBOX:
		option.show_bbox = !option.show_bbox;
		check_menu_item(IDM_OPTIONMENU, IDM_SHOWBBOX, option.show_bbox);
#ifdef UNIX
		gtk_widget_draw(img, NULL);
#else
#ifdef _Windows
		PostMessage(hwndimg, WM_GSSYNC, 0, 0L);
#else
		if (!WinInvalidateRect(hwnd_bmp, (PRECTL)NULL, TRUE))
			error_message("error invalidating rect");
  		if (!WinUpdateWindow(hwnd_bmp))
			error_message("error updating window");
#endif
#endif
		return 0;
#ifndef VIEWONLY
	case IDM_PSTOEPS:
		if (not_open())
		    return 0;
		if (psfile.name[0] != '\0') {
		    if (dfreopen() != 0)
			return 0;
		    ps_to_eps();
		    dfclose();
		}
		return 0;
	case IDM_MAKEEPSI:
		if ( (option.orientation == IDM_PORTRAIT) ||
		     (option.auto_orientation == TRUE) ) {
		    char epsname[MAXSTR];
		    epsname[0] = '\0';
		    if (dfreopen() != 0)
			return 0;
		    if (!get_filename(epsname, TRUE, FILTER_EPS, 0, 
			    IDS_TOPICPREVIEW)) {
			dfclose();
			return 0;
		    }
		    image_lock(view.img);
		    make_eps_interchange(FALSE, epsname);
		    image_unlock(view.img);
		    dfclose();
	  	}
		else
		    gserror(IDS_MUSTUSEPORTRAIT, 0, MB_ICONEXCLAMATION, 0); 
		return 0;
	case IDM_MAKEEPST4:
	case IDM_MAKEEPST6U:
	case IDM_MAKEEPST6P:
		if ( (option.orientation == IDM_PORTRAIT) ||
		     (option.auto_orientation == TRUE) ) {
		    char epsname[MAXSTR];
		    epsname[0] = '\0';
		    if (dfreopen() != 0)
			return 0;
		    if (!get_filename(epsname, TRUE, FILTER_EPS, 0, 
			    IDS_TOPICPREVIEW)) {
		        dfclose();
			return 0;
		    }
		    image_lock(view.img);
		    make_eps_tiff(command, FALSE, epsname);
		    image_unlock(view.img);
		    dfclose();
		}
		else
		    gserror(IDS_MUSTUSEPORTRAIT, 0, MB_ICONEXCLAMATION, 0); 
		return 0;
	case IDM_MAKEEPSW:
		if ( (option.orientation == IDM_PORTRAIT) ||
		     (option.auto_orientation == TRUE) ) {
		    char epsname[MAXSTR];
		    epsname[0] = '\0';
		    if (dfreopen() != 0)
			return 0;
		    if (!get_filename(epsname, TRUE, FILTER_EPS, 0, 
			    IDS_TOPICPREVIEW)) {
			dfclose();
			return 0;
		    }
		    image_lock(view.img);
		    make_eps_metafile(FALSE, epsname);
		    image_unlock(view.img);
		    dfclose();
		}
		else
		    gserror(IDS_MUSTUSEPORTRAIT, 0, MB_ICONEXCLAMATION, 0); 
		return 0;
	case IDM_MAKEEPSU:
		if (dfreopen() != 0)
		    return 0;
		make_eps_user();
		dfclose();
		return 0;
	case IDM_EXTRACTPS:
	case IDM_EXTRACTPRE:
		if (dfreopen() != 0)
		    return 0;
		extract_doseps(command);
		dfclose();
		return 0;
#endif
	case IDM_SETTINGS:
		write_profile();
		return 0;
	case IDM_SAVESETTINGS:
		option.settings = !option.settings;
		check_menu_item(IDM_OPTIONMENU, IDM_SAVESETTINGS, option.settings);
		{ char buf[MAXSTR];
		  PROFILE *prf = profile_open(szIniFile);
		  sprintf(buf, "%d", (int)option.settings);
		  profile_write_string(prf, INISECTION, "SaveSettings", buf);
		  profile_close(prf);
		}
		return 0;
	case IDM_SOUNDS:
		change_sounds();
		return 0;
	case IDM_AUTOORIENT:
	case IDM_PORTRAIT:
	case IDM_LANDSCAPE:
	case IDM_UPSIDEDOWN:
	case IDM_SEASCAPE:
	case IDM_SWAPLANDSCAPE:
		gsview_orientation(command);
		return 0;
	case IDM_DSC_OFF:
	case IDM_DSC_ERROR:
	case IDM_DSC_WARN:
	case IDM_DSC_INFO:
		check_menu_item(IDM_DSCMENU, option.dsc_warn, FALSE);
		option.dsc_warn = command;
		check_menu_item(IDM_DSCMENU, option.dsc_warn, TRUE);
		return 0;
	case IDM_ZOOM:		/* called indirectly from Right Mouse Button */
		if (not_dsc()) {
		    zoom = FALSE;
		    return 0;
		}
		if (order_is_special()) {
		    zoom = FALSE;
		    return 0;
		}
		if (! ((gsdll.state == GS_PAGE) || (gsdll.state == GS_IDLE)) ) {
		    zoom = FALSE;
	    	    gserror(IDS_NOZOOM, NULL, MB_ICONEXCLAMATION, SOUND_ERROR);
	    	    return 0;
		}
		gs_resize();
	        pending.pagenum = -1;  /* default page number is current page */
		pending.now = TRUE;
		return 0;
	case IDM_MAGPLUS:
		gs_magnify((float)1.2);
		return 0;
	case IDM_MAGMINUS:
		gs_magnify((float)0.8333);
		return 0;
	case IDM_FITWIN:
		/* fit media to size of current window */
		gsview_fitwin();
		return 0;
	case IDM_DISPLAYSETTINGS:
		display_settings();
		return 0;
	case IDM_MEDIAROTATE:
		option.media_rotate = !option.media_rotate;
		check_menu_item(IDM_MEDIAMENU, IDM_MEDIAROTATE, 
			option.media_rotate);
		zoom = FALSE;
		gs_resize();
		return 0;
	case IDM_11x17:
	case IDM_A3:
	case IDM_A4:
	case IDM_A5:
	case IDM_B4:
	case IDM_B5:
	case IDM_LEDGER:
	case IDM_LEGAL:
	case IDM_LETTER:
	case IDM_NOTE:
	case IDM_USERSIZE:
	case IDM_USERSIZE1:
	case IDM_USERSIZE2:
	case IDM_USERSIZE3:
	case IDM_USERSIZE4:
	case IDM_USERSIZE5:
	case IDM_USERSIZE6:
	case IDM_USERSIZE7:
	case IDM_USERSIZE8:
	case IDM_USERSIZE9:
	case IDM_USERSIZE10:
	case IDM_USERSIZE11:
	case IDM_USERSIZE12:
	case IDM_USERSIZE13:
		if (command == IDM_USERSIZE)
		    if (!gsview_usersize())
			return 0;
		gsview_media(command);
		return 0;
	case IDM_HELPCONTENT:
#ifdef UNIX
		nHelpTopic = IDS_TOPICROOT;
		get_help();
#else
#ifdef _Windows
#ifdef USE_HTMLHELP
		nHelpTopic = IDS_TOPICROOT;
		get_help();
#else
		WinHelp(hwndimg,szHelpName,HELP_CONTENTS,(DWORD)NULL);
#endif
#else
		WinSendMsg(hwnd_help, HM_HELP_CONTENTS, 0L, 0L);
#endif
#endif
		return 0;
	case IDM_HELPSEARCH:
#ifdef UNIX
		gs_addmess("IDM_HELPSEARCH: not implemented\n");
#else
#ifdef _Windows
#ifdef USE_HTMLHELP
		HtmlHelp(hwndimg,szHelpName,HH_DISPLAY_INDEX, (DWORD)TEXT(""));
		gs_addmessf("HtmlHelp: %s HH_DISPLAY_INDEX\n", szHelpName);
#else
		WinHelp(hwndimg,szHelpName,HELP_PARTIALKEY,(DWORD)"");
#endif
#else
		WinSendMsg(hwnd_help, HM_HELP_INDEX, 0L, 0L);
#endif
#endif
		return 0;
	case IDM_HELPKEYS:
		nHelpTopic = IDS_TOPICKEYS;
		get_help();
		return 0;
	case IDM_ABOUT:
		show_about();
		return 0;
	case IDM_REGISTER:
		registration_nag();
		return 0;
	}
	return 0;
}

/* if no document open, display error message and return true */
BOOL
not_open()
{
    if (psfile.name[0] != '\0')
	return FALSE;
    gserror(IDS_NOTOPEN, NULL, MB_ICONEXCLAMATION, SOUND_NOTOPEN);
    return TRUE;
}

/* if order is SPECIAL, display error message and return TRUE */
BOOL
order_is_special()
{
TCHAR buf[MAXSTR];
    if (psfile.dsc==(CDSC *)NULL)
	return TRUE;
    if (psfile.dsc->page_order != CDSC_SPECIAL)
	return FALSE;
    if (psfile.dsc->page_count == 1)
	return FALSE;	/* can't reorder anyway */
    if (psfile.ignore_special)
	return FALSE;
    gsview_fullscreen_end();
    load_string(IDS_PAGESPECIAL, buf, sizeof(buf)-1);
    if (message_box(buf, MB_OKCANCEL) == IDOK) {
	/* don't show this warning again for this file */
	psfile.ignore_special = TRUE;
	return FALSE;	/* User override */
    }
    return TRUE;		/* order is special */
}


/* if not DSC document or not open, display error message and return true */
BOOL
not_dsc()
{
    if (not_open())
	return TRUE;
    if (psfile.dsc!=(CDSC *)NULL)
	return FALSE;
    gserror(IDS_NOPAGE, NULL, MB_ICONEXCLAMATION, SOUND_NONUMBER);
    return TRUE;
}

void
gserror(UINT id, const char *str, UINT icon, int sound)
{
int i;
TCHAR mess[MAXSTR+MAXSTR];
    if (sound >= 0)
	play_sound(sound);
    i = 0;
    if (id)
	i = load_string(id, mess, sizeof(mess)/sizeof(TCHAR)-1);
    mess[i] = '\0';
    if (str)
	convert_multibyte(mess+i, str, sizeof(mess)/sizeof(TCHAR)-i-1);
    message_box(mess, icon);
}


int
not_implemented()
{
    gserror(IDS_NOTIMPLEMENTED, NULL, 0, 0);
    return 0;
}

static float get_points(const char *str)
{
    float val;
    char ptbuf[32];
    char inchbuf[32];
    char mmbuf[32];
    TCHAR tbuf[32];
    const char *p = str;
    int i;
    ptbuf[0] = inchbuf[0] = mmbuf[0] = '\0';
    load_string(IDS_UNITNAME + IDM_UNITPT - IDM_UNITPT, tbuf, sizeof(tbuf));
    convert_widechar(ptbuf, tbuf, sizeof(ptbuf));
    load_string(IDS_UNITNAME + IDM_UNITINCH - IDM_UNITPT, 
	tbuf, sizeof(tbuf));
    convert_widechar(inchbuf, tbuf, sizeof(inchbuf));
    load_string(IDS_UNITNAME + IDM_UNITMM - IDM_UNITPT, tbuf, sizeof(tbuf));
    convert_widechar(mmbuf, tbuf, sizeof(mmbuf));
    while (*p && (*p == ' '))
	p++;
    val = (float)atof(p);
    while (*p && (isdigit((int)(*p)) || (*p == '.')))
	p++;
    while (*p && (*p == ' '))
	p++;
    i = 0;;
    while (p[i] && (isalpha((int)(p[i]))))
	i++;
    if (p[i])
	i++;
    if ((strnicmp(p, ptbuf, max(i, (int)strlen(ptbuf))) == 0) ||
	     (strnicmp(p, "pt", 2) == 0)) {
	/* do nothing */
    }
    else if ((strnicmp(p, inchbuf, max(i, (int)strlen(inchbuf))) == 0) || 
	     (strnicmp(p, "in", 2) == 0))
	val *= 72.0;
    else if ((strnicmp(p, mmbuf, max(i, (int)strlen(mmbuf))) == 0) || 
	     (strnicmp(p, "mm", 2) == 0))
	val *= (float)(72.0 / 25.4);
    else if (strnicmp(p, "cm", 2) == 0)
	val *= (float)(72.0 / 2.54);
    else if (strnicmp(p, "m", 1) == 0)
	val *= (float)(72.0 / 0.0254);
    else if (strnicmp(p, "ft", 2) == 0)
	val *= (float)(72.0 * 12.0);
    return val;
}

static void put_points(char *buf, int len, float n)
{
    TCHAR tbuf[32];
    char ubuf[32];
    float factor = 1.0;
    if (len < 1)
	return;
    buf[0] = '\0';
    ubuf[0] = '\0';
    load_string(IDS_UNITNAME + option.unit - IDM_UNITPT, tbuf, sizeof(tbuf));
    convert_widechar(ubuf, tbuf, sizeof(ubuf));
    if (len < 32 + (int)strlen(ubuf))
	return;
    switch (option.unit) {
	case IDM_UNITMM:
	    factor = (float)(25.4 / 72.0);
	    break;
	case IDM_UNITINCH:
	    factor = (float)(1.0 / 72.0);
	    break;
	case IDM_UNITPT:
	default:
	   factor = 1.0;
    }
    sprintf(buf, "%g %s", n*factor, ubuf);
}



/* get user defined size */
BOOL
gsview_usersize()
{
    TCHAR prompt[MAXSTR];
    char answer[MAXSTR];
    nHelpTopic = IDS_TOPICMEDIA;
    load_string(IDS_USERWIDTH, prompt, sizeof(prompt)/sizeof(TCHAR)-1);
    put_points(answer, sizeof(answer), (float)option.user_width);
    if (!query_string(prompt,answer))
	return FALSE;
    option.user_width = (int)(get_points(answer) + 0.5);
    load_string(IDS_USERHEIGHT, prompt, sizeof(prompt)/sizeof(TCHAR)-1);
    put_points(answer, sizeof(answer), (float)option.user_height);
    if (!query_string(prompt,answer))
	return FALSE;
    option.user_height = (int)(get_points(answer) + 0.5);
    if ((option.user_width==0) || (option.user_height == 0)) {
	option.user_width = 595;
	option.user_width = 842;
    }
    gsview_check_usersize();
    return TRUE;
}

void
gsview_check_usersize()
{
    if ( (option.user_width > option.user_width_warn) || 
	(option.user_height > option.user_width_warn) ) {
	gserror(IDS_LARGEMEDIA, NULL, 0, SOUND_ERROR);
    }
}

/* unzoom when redisplaying or changing page */ 
void
gsview_unzoom(void)
{
    if (zoom) {
	    zoom = FALSE;
	    gs_resize();
    }
}

void
gsview_language(int new_language)
{
    if (load_language(new_language)) {
	check_menu_item(IDM_LANGMENU, option.language, FALSE);
	option.language = new_language;
	check_menu_item(IDM_LANGMENU, option.language, TRUE);
	change_language();
    }
}

void
gsview_goto_page(int pagenum)
{
    if (not_dsc())
	return;
    if (psfile.dsc->page_count == 0)
	return;
    if (pagenum > (int)psfile.dsc->page_count) {
	pagenum = psfile.dsc->page_count;
	play_sound(SOUND_NOPAGE);
    }
    else if (pagenum < 1) {
	pagenum = 1;
	play_sound(SOUND_NOPAGE);
    }
    gsview_unzoom();
    pending.pagenum = pagenum;
    pending.now = TRUE;
    history_add(pagenum);
}
