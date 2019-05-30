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

/* gvcrc.h */
/* Common Resource header file */

#include "gvcver.h"

#define DEBUG_GENERAL	0x01
#define DEBUG_GDI	0x02	/* GDI printing */
#define DEBUG_MEM	0x04	/* memory allocation */
#define DEBUG_LOG	0x08	/* write gs_addmess() to file c:\gsview.txt */
#define DEBUG_GSINPUT	0x10	/* log all input written to GS */

#define ID_GSVIEW 1
#define ID_GSVIEW2 2
#define ID_GSVIEW3 3
#define ID_GSVIEW_DOC	4
#define ID_LANG	10

#define IDD_ABOUT 	50
#define IDD_INPUT 	51
#define ID_ANSWER	52
#define ID_PROMPT	53
#define ID_HELP		54
#define ID_DEFAULT	55
#define IDNEXT 56
#define IDPREV 57
#define IDFINISH 58

#define IDM_FILEMENU	100
#define IDM_OPEN	101
#define IDM_SELECT	102
#define IDM_SAVEAS	103
#define IDM_EXTRACT	104
#define IDM_PSTOEPS	105
#define IDM_CLOSE	106
#define IDM_CLOSE_DONE	107
#define IDM_INFO	108
#define IDM_PRINT	110
#define IDM_PRINTTOFILE 111
#define IDM_CONVERTFILE	112
#define IDM_SPOOL	113
#define IDM_GSMESS      114
#define IDM_LASTFILE1	115
#define IDM_LASTFILE2	116
#define IDM_LASTFILE3	117
#define IDM_LASTFILE4	118
#define IDM_EXIT	119
#define IDM_DROP	120
#define IDM_PSTOEDIT	121
#define IDM_ARGS	122

#define IDM_EDITMENU	150
#define IDM_COPYCLIP	151
#define IDM_PASTETO	152
#define IDM_CONVERT	153
#define IDM_ADDEPSMENU	154
#define IDM_MAKEEPSI	155
#define IDM_MAKEEPST4	156
#define IDM_MAKEEPST6U	157
#define IDM_MAKEEPST6P	158
#define IDM_MAKEEPSW	159
#define IDM_MAKEEPSU	160
#define IDM_EXTEPSMENU	161
#define IDM_EXTRACTPS	162
#define IDM_EXTRACTPRE	163
#define IDM_TEXTEXTRACT	     164
#define IDM_TEXTEXTRACT_SLOW 165
#define IDM_TEXTFIND	     166
#define IDM_TEXTFINDNEXT     167

#define IDM_OPTIONMENU	  174
#define IDM_GSCOMMAND	  175
#define IDM_CFG	  	  176
#define IDM_SOUNDS	  177
#define IDM_SETTINGS	  178
#define IDM_SAVESETTINGS  179
#define IDM_SAFER         180  
#define IDM_SAVEDIR	  181
#define IDM_BUTTONSHOW	  182
#define IDM_FITPAGE	  183
#define IDM_AUTOREDISPLAY 185
#define IDM_EPSFCLIP	  186
#define IDM_EPSFWARN	  187
#define IDM_IGNOREDSC	  188
#define IDM_SHOWBBOX      189

#define IDM_UNITMENU	190
#define IDM_UNITPT	191
#define IDM_UNITMM	192
#define IDM_UNITINCH	193
#define IDM_UNITCUSTOM	194
#define IDM_UNITFINE	195

#define IDM_VIEWMENU    200
#define IDM_NEXT	201
#define IDM_NEXTHOME	202
#define IDM_NEXTSKIP	203
#define IDM_PREV	204
#define IDM_PREVHOME	205
#define IDM_PREVSKIP	206
#define IDM_GOTO	207
#define IDM_GOBACK	208
#define IDM_GOFWD	209
#define IDM_REDISPLAY   210
#define IDM_SKIP	211
#define IDM_FULLSCREEN  212
#define IDM_FITWIN	213

#define IDM_ORIENTMENU	220
#define IDM_PORTRAIT	221
#define IDM_LANDSCAPE   222
#define IDM_UPSIDEDOWN  223
#define IDM_SEASCAPE	224
#define IDM_SWAPLANDSCAPE 225
#define IDM_AUTOORIENT  226

#define IDM_DISPLAYSETTINGS	250
#define IDM_ZOOM	251
#define IDM_MAGPLUS	252
#define IDM_MAGMINUS	253

/* OS/2 only */
#define IDM_DRAWMENU	260
#define IDM_DRAWDEF	261
#define IDM_DRAWGPI	262
#define IDM_DRAWWIN	263

/* X11 only */			/* must use same values as OS/2 */
#define IDM_DRAWPIXMAP	261	/* draw on pixmap */
#define IDM_DRAWBACKING	262	/* draw on backing pixmap */
#define IDM_DRAWSTORAGE	263	/* draw on window with backing storage */

#define IDM_MEDIAMENU	300
#define IDM_MEDIAFIRST	301
#define IDM_11x17	301
#define IDM_A3		302
#define IDM_A4		303
#define IDM_A5		304
#define IDM_B4		305
#define IDM_B5		306
#define IDM_LEDGER	307
#define IDM_LEGAL	308
#define IDM_LETTER	309
#define IDM_NOTE	310
#define IDM_MEDIALAST	311
#define IDM_USERSIZE	312
#define IDM_USERSIZE1   313
#define IDM_USERSIZE2   314
#define IDM_USERSIZE3   315
#define IDM_USERSIZE4   316
#define IDM_USERSIZE5   317
#define IDM_USERSIZE6   318
#define IDM_USERSIZE7   319
#define IDM_USERSIZE8   320
#define IDM_USERSIZE9   321
#define IDM_USERSIZE10  322
#define IDM_USERSIZE11  323
#define IDM_USERSIZE12  324
#define IDM_USERSIZE13  325
#define MEDIA_USERDEFINED "User Defined"

#define IDM_MEDIAROTATE	330

#define IDM_HELPMENU	340
#define IDM_HELPCONTENT 341
#define IDM_HELPSEARCH	342
#define IDM_HELPKEYS	343
#define IDM_ABOUT	344
#define IDM_MISC	345
#define IDM_REGISTER	346

#define IDD_LANG	350
#define IDM_LANGMENU	350
#define IDM_LANGEN	351
/* These language IDs may be reallocated in GSview */
/* but are needed by the setup program */
#define IDM_LANGDE	352
#define IDM_LANGFR	353
#define IDM_LANGIT	354
#define IDM_LANGES	355
#define IDM_LANGGR	356
#define IDM_LANGNL	357
#define IDM_LANGSE	358
#define IDM_LANGSK	359
#define IDM_LANGCT	360
#define IDM_LANGRU	361

#define IDM_LANGLAST    369

#define IDM_PSTOTEXTMENU	370
#define IDM_PSTOTEXTDIS 	371
#define IDM_PSTOTEXTNORM	372
#define IDM_PSTOTEXTCORK	373

/* configuration dialog resource IDs */
#define IDD_CFG0	380	/* parent */
#define IDD_CFG1	381	/* intro */
#define IDD_CFG2	382	/* gsver */
#define IDD_CFG3	383	/* printer.ini */
#define IDD_CFG4	384	/* associate */
#define IDD_CFG5	385	/* progman */
#define IDD_CFG6	386	/* finish */
#define IDD_CFG7	387	/* done */
/* control IDs */
#define IDC_CFG20	390	/* gsver edit field */
#define IDC_CFG21	391	/* gsver scroll bar */
#define IDC_CFG22	392	/* GS dir edit field */
#define IDC_CFG23	393	/* additional fonts edit field */
#define IDC_CFG32	394	/* printer.ini checkbox */
#define IDC_CFG41	395	/* PS associate checkbox */
#define IDC_CFG42	396	/* PDF associate checkbox */
#define IDC_CFG51	397	/* create group checkbox */
#define IDC_CFG52	398	/* group name */
#define IDC_CFG70	399	/* result */
#define IDC_CFG71	400	/* failed reason */

#define IDD_EASYCFG	401
#define IDC_GSVER	402

/* info dialog box */
#define IDD_INFO	410
#define INFO_FILE	411
#define INFO_TYPE	412
#define INFO_TITLE	413
#define INFO_DATE	414
#define INFO_BBOX	415
#define INFO_ORIENT	416
#define INFO_ORDER	417
#define INFO_DEFMEDIA	418
#define INFO_NUMPAGES	419
#define INFO_PAGE	420
#define INFO_BITMAP	421
#define INFO_ICON	422

#define ABOUT_ICON	451
#define ABOUT_VERSION	452

#define IDD_PSTOEPS	 470
#define PSTOEPS_AUTOBBOX  471

#define IDD_ADVPS		481
#define ADVPS_PRECTRLD		482
#define ADVPS_POSTCTRLD 	483
#define ADVPS_PROLOG		484
#define ADVPS_PROLOGBROWSE	485
#define ADVPS_EPILOG		486
#define ADVPS_EPILOGBROWSE	487
  
#define IDD_PSTOEDIT	490
#define EDIT_FORMAT	491
#define EDIT_DT		492
#define EDIT_LATIN1	493
#define EDIT_FLAT	494
#define EDIT_FONT	495
#define EDIT_OPTION	496

#define IDD_SOUND	500
#define SOUND_EVENT	501
#define SOUND_FILE	502
#define SOUND_PATH	503
#define SOUND_TEST	504

#define IDD_SPOOL	510
#define IDD_QUEUE	511
#define SPOOL_PORT	512
#define SPOOL_PORTTEXT	513
#define SPOOL_TOFILE	514

#define IDD_CANCEL	515
#define CANCEL_PCDONE	515

#define IDD_PAGE	520
#define IDD_MULTIPAGE	521
#define PAGE_EDIT	522
#define PAGE_LIST	523
#define PAGE_LISTTEXT	524
#define PAGE_ALL	525
#define PAGE_ODD	526
#define PAGE_EVEN	527
#define PAGE_REVERSE	528

#define IDD_CONVERT	530
#define IDD_DEVICE	531
#define IDD_PRINTGS	532
#define IDD_PRINTGDI	534
#define DEVICE_NAME	534
#define DEVICE_NAMETEXT	535
#define DEVICE_RES	536
#define DEVICE_RESTEXT	537
#define DEVICE_PROP	538
#define DEVICE_PSPRINT  539
#define DEVICE_OPTIONS     540
#define DEVICE_OPTIONSTEXT 541
#define DEVICE_UNIPRINT    542
#define DEVICE_ADVPS   	   543
#define DEVICE_FIXEDMEDIA  544

#define IDD_UNIPRINT	550
#define UPP_LIST	551
#define UPP_NAME	552

#define IDD_PROP	553
#define PROP_NAME	554
#define PROP_VALUE	555
#define PROP_EDIT	556
#define PROP_NEW	557
#define PROP_XOFFSET	558
#define PROP_YOFFSET	559

#define IDD_EDITPROP	560
#define EDITPROP_STRING	561
#define EDITPROP_NUMBER	562
#define EDITPROP_NAME	563
#define EDITPROP_VALUE	564
#define NEWPROP_DEL	565
#define EDITPROP_DEL	566

#define IDD_TEXTWIN	570
#define TEXTWIN_MLE	571
#define TEXTWIN_COPY	572

#define IDD_BBOX	575
#define BB_PROMPT	576
#define BB_CLICK	577

#define IDD_INSTALL	580
#define INSTALL_DLL	581
#define INSTALL_INCLUDE	582
#define INSTALL_OTHER	583
#define IDC_CONFIGADV_PRINTER	584
#define IDC_CONFIGADV_PS 	585
#define IDC_CONFIGADV_PDF	586
#define IDC_CONFIGADV_START	587

#define IDD_DSET	590
#define DSET_RES	591
#define DSET_ZOOMRES	592
#define DSET_DEPTH	593
#define DSET_TALPHA	594
#define DSET_GALPHA	595
#define DSET_DRAWMETHOD	596


/* file filters */
#define FILTER_PSALL	0
#define FILTER_PS	1
#define FILTER_EPS	2
#define FILTER_EPI	3
#define FILTER_PDF	4
#define FILTER_ALL	5
#define FILTER_BMP	6
#define FILTER_TIFF	7
#define FILTER_WMF	8
#define FILTER_TXT	9

/* string constants */
#define IDS_SLANGUAGE	601
#define IDS_CODEPAGE	602
#define IDS_HELPFILE	603
#define IDS_HELPTITLE	604
#define IDS_SIGSEGV	605
#define IDS_FILENOTFOUND 606

#define IDS_LANGUAGE	608
#define IDS_ELANGUAGE	609
#define IDS_FILE	610
#define IDS_NOFILE	611
#define IDS_PAGE	612
#define IDS_NOPAGE	613
#define IDS_PAGESPECIAL	614
#define IDS_LANDSCAPE	615
#define IDS_PORTRAIT	616
#define IDS_ASCEND	617
#define IDS_DESCEND	618
#define IDS_SPECIAL	619
#define IDS_PAGEINFO	620
#define IDS_LINKPAGE	621
#define IDS_DEFAULT	622
#define IDS_NOTDEFTAG	623

#define IDS_EPSF	625
#define IDS_EPSI	626
#define IDS_EPST	627
#define IDS_EPSW	628
#define IDS_DSC		629
#define IDS_NOTDSC	630
#define IDS_PDF         631
#define IDS_IGNOREDSC   632
#define IDS_CTRLD	633
#define IDS_PJL		634
#define IDS_ZOOMED	635

#define IDS_OUTPUTFILE	640
#define IDS_PRINTINGALL	641
#define IDS_PRINTFILE	642
#define IDS_PRINTAGAIN	643
#define IDS_SELECTPAGE	645
#define IDS_SELECTPAGES	646
#define IDS_NOTOPEN	649
#define IDS_CANNOTRUN	650
#define IDS_TOOLONG	651
#define IDS_NOMORE	652
#define IDS_GSCOMMAND	653
#define IDS_NOZOOM	656
#define	IDS_USERWIDTH	657
#define	IDS_USERHEIGHT	658
#define IDS_BADEPS	659
#define IDS_NOPREVIEW	660
#define IDS_NOTDFNAME   661
#define IDS_NOTEMP	662
#define IDS_CANCELDONE	663
#define IDS_BADCLI      664
#define IDS_DUPOPT      665
#define IDS_TEXTFIND	666
#define IDS_TEXTNOTFIND	667
#define IDS_PRINTPDFPS	668
#define IDS_NOPDFQUICKTEXT 669
#define IDS_BAD_DOSEPS_HEADER 670
#define IDS_PROBABLY_PCL 671
#define IDS_BUSYPRINTING 672

#define IDS_SOUNDNAME	680
#define IDS_SNDPAGE	681
#define IDS_SNDNOPAGE	682
#define IDS_SNDNONUMBER 683
#define IDS_SNDNOTOPEN	684
#define IDS_SNDERROR	685
#define IDS_SNDTIMEOUT	686
#define IDS_SNDSTART	687
#define IDS_SNDEXIT	688
#define IDS_SNDBUSY	689
#define IDS_SOUNDNOMM	690
#define IDS_NONE	691
#define IDS_SPKR	692

/* help topics */
#define IDS_TOPICROOT	 701
#define IDS_TOPICOPEN	 702
#define IDS_TOPICPRINT	 703
#define IDS_TOPICPROP    704
#define IDS_TOPICEDITPROP 705
#define IDS_TOPICCONVERT 706
#define IDS_TOPICCLIP    707
#define IDS_TOPICPREVIEW 708
#define IDS_TOPICSOUND	 709
#define IDS_TOPICDSET    710
#define IDS_TOPICMEDIA   711
#define IDS_TOPICPSTOEPS 712
#define	IDS_TOPICGOTO    713
#define IDS_TOPICINSTALL 714
#define IDS_TOPICTEXT    715
#define IDS_TOPICKEYS    716
#define IDS_TOPICMESS    717
#define IDS_TOPICSPOOL   718
#define IDS_TOPICZLIB	 719
#define IDS_TOPICBZIP2	 720
#define IDS_TOPICPSTOEDIT 721
#define IDS_TOPICMEASURE 722
#define IDS_TOPICEASYCFG 723
#define IDS_TOPICADVANCEDCFG 724
#define IDS_TOPICDOWNLOAD 725
#define IDS_TOPICDSCWARN 726
#define IDS_TOPICREG     727
#define IDS_TOPICA2PS    728

/* ps_to_eps */
#define IDS_BBPROMPT	750
#define IDS_BBPROMPT1	751
#define IDS_BBPROMPT2	752
#define IDS_BBPROMPT3	753
#define IDS_EPSONEPAGE	754
#define IDS_EPSQPAGES	755
#define IDS_EPSNOBBOX	756
#define IDS_EPSREAD     757
#define IDS_EPSUSERINVALID 758
#define IDS_EPSUSERTITLE 759
#define IDS_EPS_OFF_PAGE 760
#define IDS_EPS_BAD_BBOX 761
#define IDS_MUSTUSEPORTRAIT 762

/* wait messages */
#define IDS_NOWAIT	0
#define IDS_WAIT	770
#define IDS_WAITREAD	771
#define IDS_WAITWRITE	772
#define IDS_WAITDRAW	773
#define IDS_WAITDRAW_PC	774
#define IDS_WAITGSOPEN	775
#define IDS_WAITGSCLOSE	776
#define IDS_WAITPRINT	777
#define IDS_WAITSEARCH	778
#define IDS_WAITTEXT  	779

/* debug messages */
#define IDS_DEBUG_DFISLOCKED   780
#define IDS_DEBUG_DFISOPEN     781
#define IDS_DEBUG_DFISMISSING  782
#define IDS_DEBUG_DFCHANGED    783
#define IDS_DEBUG_DFISCLOSED   784

/* process delayed messages */
#define IDS_PROCESS_GSLOAD_FAIL  785
#define IDS_PROCESS_INIT_FAIL    786
#define IDS_PROCESS_VIEWER_FAIL  787
#define IDS_PROCESS_EXECUTE_FAIL 788

/* PDF messages */
#define IDS_PDFNOPAGE		790
#define IDS_PDFEXTRACTALL	791
#define IDS_USEPDFWRITE		792

/* filter strings */
#define IDS_FILTER_BASE	800
#define IDS_FILTER_PSALL IDS_FILTER_BASE+FILTER_PSALL
#define IDS_FILTER_PS	IDS_FILTER_BASE+FILTER_PS
#define IDS_FILTER_EPS	IDS_FILTER_BASE+FILTER_EPS
#define IDS_FILTER_EPI	IDS_FILTER_BASE+FILTER_EPI
#define IDS_FILTER_PDF	IDS_FILTER_BASE+FILTER_PDF
#define IDS_FILTER_ALL	IDS_FILTER_BASE+FILTER_ALL
#define IDS_FILTER_BMP	IDS_FILTER_BASE+FILTER_BMP
#define IDS_FILTER_TIFF	IDS_FILTER_BASE+FILTER_TIFF
#define IDS_FILTER_WMF	IDS_FILTER_BASE+FILTER_WMF
#define IDS_FILTER_TXT	IDS_FILTER_BASE+FILTER_TXT

#define IDS_LARGEMEDIA		840
#define IDS_NOTIMPLEMENTED	841
#define IDS_NOLINKTARGET	842
#define IDS_PARSEERROR		844
#define IDS_ZLIB_FAIL		845
#define IDS_BZIP2_FAIL		846

#define IDS_BETAEXPIRED		850
#define IDS_BETAWARN		851
#define IDS_NOPRINTERINI	852
#define IDS_NOINI		853
#define IDS_NEEDTEMP		854
#define IDS_VERSIONCHANGED	855
#define IDS_CONFIGURECANCELLED	856
#define IDS_GSNOTINSTALLED	857
#define IDS_GSLIBNOTINSTALLED	858
#define IDS_NOPROGMAN		859
#define IDS_PROGRAMOBJECTFAILED 860
#define IDS_APPNAME		861
#define IDS_TARGET_GROUP	862
#define IDS_TARGET_DIR		863
#define IDS_INVALIDREG		864
#define IDS_UNREGISTERED	865

#define IDS_MKDIRFAIL		870
#define IDS_WRONGEMX		871
#define IDS_DIRNOTEXIST		872
#define IDS_GSVIEWBASE		873
#define IDS_GSVIEWVERSION	874
#define IDS_PROGMANGROUP4	875
#define IDS_PROGMANGROUP5	876
#define IDS_PROGMANGROUP6	877
#define IDS_DONEBAK		878
#define IDS_CANTLOAD		879
#define IDS_CANTCREATETEMPFILE	880
#define IDS_CANTOPENREAD	881
#define IDS_CANTOPENWRITE	882
#define IDS_ERRORRENAME		883
#define IDS_NODDEPROGMAN	884
#define IDS_INSERTDISK		885
#define IDS_ZIPNOTFOUND		886
#define IDS_UNZIPCANCELLED	887
#define IDS_CANTALLOCBUF	888
#define IDS_CFG73		889
#define IDS_CFG74		890
#define IDS_UNINSTALLTITLE	891
#define IDS_UNINSTALLITEM	892

#define IDS_PRN_PRINTMETHOD	900
#define IDS_PRN_WINDOWSGDI	901
#define IDS_PRN_GSDEVICE	902
#define IDS_PRN_PSPRINTER	903
#define IDS_PRN_SETTINGS	904
#define IDS_PRN_HELP		905
#define IDS_PRN_SELECTPAGES	906
#define IDS_ALL			907

/* PageSize policies */
#define IDS_PAGESIZE_VARIABLE	911
#define IDS_PAGESIZE_FIXED	912
#define IDS_PAGESIZE_SHRINK	913


/* cursors */
#define IDP_CROSSHAIR		940
#define IDP_HAND		941

/* RCDATA resources */
#define IDR_VIEWER	951
#define IDR_EPSFWARN	952
#define IDR_DEVICES	953
#define IDR_PORTS	954
#define IDR_BUTTON	955
#define IDR_HELP	956


/* window messages */

#define WM_GSV16SPL    WM_USER+1	/* 16 bit spooler interface */
#define WM_GSWAIT      WM_USER+102	/* info_wait from dll thread */
#define WM_GSMESSBOX   WM_USER+103	/* delayed message box */
#define WM_GSSHOWMESS  WM_USER+104	/* delayed show messages */
#define WM_GSREDISPLAY WM_USER+105	/* document changed during display */
#define WM_GSTITLE     WM_USER+106	/* change window title */
#define WM_GSPERCENT   WM_USER+107	/* percent processed for status line */
#define WM_GSTEXTINDEX WM_USER+108	/* load text index */

/* messages for Ghostscript events */
#define WM_GSDEVICE    WM_USER+111
#define WM_GSSYNC      WM_USER+112
#define WM_GSPAGE      WM_USER+113
#define WM_GSSIZE      WM_USER+114
#define WM_GSTILE      WM_USER+115

/* non standard scroll bar code */
#define SB_FIND WM_USER+120

#define IDD_PRINT		1538	/* must be this number */
#define IDC_DEVICE_GROUP	1601	/* Group box */
#define IDC_DEVICE_GDI		1602	/* GDI printing */
#define IDC_DEVICE_GS		1603	/* GS printing */
#define IDC_DEVICE_PS		1604	/* PS printing */
#define IDC_GS_WINDOWS		1605	/* mono, grey, colour */
#define IDC_GS_DEVNAME		1606	/* djet500, pdfwrite etc. */
#define IDC_GS_ADVPS		1607	/* PS prolog, epilog */
#define IDC_SELECT_PAGES	1608	/* show dialog for page selection */

/*  Custom units dialog box  */
#define IDD_CALC              2000
#define IDC_CALCCTMA          2001
#define IDC_CALCCTMB          2002
#define IDC_CALCCTMC          2003
#define IDC_CALCCTMD          2004
#define IDC_CALCCTMTX         2005
#define IDC_CALCCTMTY         2006
#define IDC_CALCPTS           2007
#define IDC_CALCMM            2008
#define IDC_CALCIN            2009
#define IDC_CALCCUST          2010
/* #define IDC_CALCNAME          2011 */
#define IDC_CALCTX            2012
#define IDC_CALCTY            2013
#define IDC_CALCTR            2014
#define IDC_CALCROTTH         2015
#define IDC_CALCRO            2016
#define IDC_CALCSCX           2017
#define IDC_CALCSCY           2018
#define IDC_CALCSC            2019
#define IDC_CALCINI           2020
#define IDC_CALCINV           2021

#define IDS_INVALIDNUMBER       2022
#define IDS_CANTINVERT          2023

#define IDS_UNITNAME		2030
#define IDS_UNITPT		2030
#define IDS_UNITMM		2031
#define IDS_UNITINCH		2032
#define IDS_UNITCUSTOM		2033
/*
#define IDS_UNITPT		IDS_UNITNAME + IDM_UNITPT - IDM_UNITPT
#define IDS_UNITMM		IDS_UNITNAME + IDM_UNITMM - IDM_UNITPT
#define IDS_UNITINCH		IDS_UNITNAME + IDM_UNITINCH - IDM_UNITPT
#define IDS_UNITCUSTOM		IDS_UNITNAME + IDM_UNITCUSTOM - IDM_UNITPT
*/

/* Measure dialog box */
#define IDD_MEASURE         2100
#define IDM_MEASURE         2100
#define IDM_MEASURE_CALC    2101

#define IDC_MEASURE_UNIT    2102
#define IDC_MEASURE_LASTX   2103
#define IDC_MEASURE_LASTY   2104
#define IDC_MEASURE_X       2105
#define IDC_MEASURE_Y       2106
#define IDC_MEASURE_DELTAX  2107
#define IDC_MEASURE_DELTAY  2108
#define IDC_MEASURE_RADIUS  2109
#define IDC_MEASURE_ANGLE   2110
#define IDC_MEASURE_SETPOINT   2111


#define IDD_DOWNLOAD 		2220

#define IDD_DSCERROR 		2230
#define DSC_IGNORE_ALL		2231
#define DSC_STATIC_TEXT		2232
#define DSC_STATIC_TEXT_LAST	2249

#define IDS_DSC_INFO 		2250
#define IDS_DSC_WARN		2251
#define IDS_DSC_ERROR		2252
#define IDS_DSC_LINEFMT		2253

#define IDM_DSCMENU		2260
#define IDM_DSC_OFF		2261
#define IDM_DSC_ERROR		2262
#define IDM_DSC_WARN		2263
#define IDM_DSC_INFO		2264

#define IDD_NAG			2270
#define NAG_REGISTER		2271
#define NAG_NAME		2272
#define NAG_RECEIPT		2273
#define NAG_ICON		2274

#define IDD_REG			2280
#define REGDLG_ONLINE		2281
#define REGDLG_NAME		2282
#define REGDLG_RECEIPT		2283
#define REGDLG_NUMBER		2284
#define REGDLG_ICON		2285

#define IDD_PAGERANGE		2290
#define IDC_ALL			2291
#define IDC_ODD			2292
#define IDC_EVEN		2293
#define IDC_IGNOREDSC		2294
#define IDC_FROM		2295
#define IDC_TO			2296

#define IDC_MONO		2297
#define IDC_GREY		2298
#define IDC_COLOUR		2299

#define PRINT_GDI	0
#define PRINT_GS	1
#define PRINT_PS	2
#define PRINT_CONVERT	3


/* We must define these directly, without using arithmetic */
/* because OS/2 Resource Compiler won't accept arithmetic */
#define CDSC_RESOURCE_BASE 		2400
#define CDSC_RESOURCE_BBOX 		2400
#define CDSC_RESOURCE_BBOX2 		2401
#define CDSC_RESOURCE_BBOX3 		2402
#define CDSC_RESOURCE_EARLY_TRAILER 	2404
#define CDSC_RESOURCE_EARLY_TRAILER2 	2405
#define CDSC_RESOURCE_EARLY_TRAILER3 	2406
#define CDSC_RESOURCE_EARLY_EOF 	2408
#define CDSC_RESOURCE_EARLY_EOF2 	2409
#define CDSC_RESOURCE_EARLY_EOF3 	2410
#define CDSC_RESOURCE_PAGE_IN_TRAILER 	2412
#define CDSC_RESOURCE_PAGE_IN_TRAILER2 	2413
#define CDSC_RESOURCE_PAGE_IN_TRAILER3 	2414
#define CDSC_RESOURCE_PAGE_ORDINAL 	2416
#define CDSC_RESOURCE_PAGE_ORDINAL2 	2417
#define CDSC_RESOURCE_PAGE_ORDINAL3 	2418
#define CDSC_RESOURCE_PAGES_WRONG 	2420
#define CDSC_RESOURCE_PAGES_WRONG2 	2421
#define CDSC_RESOURCE_PAGES_WRONG3 	2422
#define CDSC_RESOURCE_EPS_NO_BBOX 	2424
#define CDSC_RESOURCE_EPS_NO_BBOX2 	2425
#define CDSC_RESOURCE_EPS_NO_BBOX3 	2426
#define CDSC_RESOURCE_EPS_PAGES 	2428
#define CDSC_RESOURCE_EPS_PAGES2 	2429
#define CDSC_RESOURCE_EPS_PAGES3 	2430
#define CDSC_RESOURCE_NO_MEDIA 		2432
#define CDSC_RESOURCE_NO_MEDIA2 	2433
#define CDSC_RESOURCE_NO_MEDIA3 	2434
#define CDSC_RESOURCE_ATEND 		2436
#define CDSC_RESOURCE_ATEND2 		2437
#define CDSC_RESOURCE_ATEND3 		2438
#define CDSC_RESOURCE_DUP_COMMENT 	2440
#define CDSC_RESOURCE_DUP_COMMENT2 	2441
#define CDSC_RESOURCE_DUP_COMMENT3 	2442
#define CDSC_RESOURCE_DUP_TRAILER 	2444
#define CDSC_RESOURCE_DUP_TRAILER2 	2445
#define CDSC_RESOURCE_DUP_TRAILER3 	2446
#define CDSC_RESOURCE_BEGIN_END 	2448
#define CDSC_RESOURCE_BEGIN_END2 	2449
#define CDSC_RESOURCE_BEGIN_END3 	2450
#define CDSC_RESOURCE_BAD_SECTION 	2452
#define CDSC_RESOURCE_BAD_SECTION2 	2453
#define CDSC_RESOURCE_BAD_SECTION3 	2454
#define CDSC_RESOURCE_LONG_LINE 	2456
#define CDSC_RESOURCE_LONG_LINE2 	2457
#define CDSC_RESOURCE_LONG_LINE3 	2458
#define CDSC_RESOURCE_INCORRECT_USAGE 	2460
#define CDSC_RESOURCE_INCORRECT_USAGE2 	2461
#define CDSC_RESOURCE_INCORRECT_USAGE3 	2462


/* Language changing */
#define AASELECTLANGUAGE "Select Language"   /* Don't translate this */

