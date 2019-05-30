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

/* gvcfn.h */
/* Common function prototypes for GSview */


/* in gvpm.c or gvwin.c */
#ifndef VIEWONLY
void copy_clipboard(void);
#endif
BOOL get_cursorpos(float *x, float *y);
void scroll_to_find(void);
void gsview_fullscreen_end(void);
void gsview_fullscreen(void);
void gsview_fitwin(void);

/* in gvpinit.c  or gvwinit.c */
void calc_info_button_areas(int width, int height);
void show_buttons(void);
void delete_buttons(void);
int gsview_create_objects(char *name);
int update_registry(BOOL ps, BOOL pdf);
BOOL load_language(int language);
void change_language(void);
void check_language(void);
void post_args(void);
void system_colours(void);
#ifndef VIEWONLY
void unload_zlib(void);
BOOL load_zlib(void);
void unload_bzip2(void);
BOOL load_bzip2(void);
#endif
int config_wizard(BOOL bVerbose);
void drop_filename(HWND hwnd, char *str);

int language_id(const char *str);
const char * language_twocc(int id);
int language_free(void);

/* in gvwdde.c */
int gsview_progman(char *name, char *gsviewpath, 
	int gsver, char *gspath, char *gsargs);

/* in gvwdde2.c */
BOOL dde_initialise(void);
void dde_uninitialise(void);
void dde_enable_server(BOOL enable);
BOOL dde_execute(char *str);
BOOL dde_execute_line(char *str);

/* in gvcinit.c */
int parse_argv(GSVIEW_ARGS *args, int argc, char *argv[]);
void use_args(GSVIEW_ARGS *args);
int gsview_printer_profiles(void);
void init_options(void);
void gsview_initc(int argc, char *argv[]);
void init_check_menu(void);
void default_gsexe(char *buf);
void default_gsdll(char *buf);
void default_gsinclude(char *buf);
void default_gsinclude_from_path(char *buf, char *gspath);
void install_default(HWND hwnd);
int gsview_changed(void);

/* in gvcmisc.c */
void gs_addmessf(const char *fmt, ...);
void make_cwd(const char *filename);
void error_message(char *str);
void info_init(HWND hwnd);
void read_profile(const char *str);
void write_profile(void);
void write_profile_last_files(void);

/* in gvwmisc or gvpmisc.c */
void post_img_message(int message, int param);
void get_help(void);
int message_box(LPCTSTR str, int icon);
void delayed_message_box(int id, int icon);
void check_menu_item(int menuid, int itemid, BOOL checked);
int get_menu_string(int menuid, int itemid, char *str, int len);
int load_string(int id, LPTSTR str, int len);
int load_resource(int resource, char *str, int len);
void play_system_sound(TCHAR *id);
void play_sound(int i);
void info_wait(int id);
int send_prolog(int resource);
void profile_create_section(PROFILE *prf, const char *section, int id);
int gs_chdir(char *dirname);
char * gs_getcwd(char *dirname, int size);

/* in gvcdisp.c */
void transform_cursorpos(float *x, float *y);
void transform_point(float *x, float *y);
void itransform_point(float *x, float *y);
int real_depth(int depth);
int get_paper_width(void);
int get_paper_height(void);
void gs_size(PENDING *pend);
void gs_resize(void);
void gs_magnify(float scale);
void gsview_orientation(int new_orientation);
void gsview_media(int new_media);
void gsview_unit(int new_unit);
void gsview_endfile(void);
void e_free_psfile(PSFILE *ppsfile);
PSFILE *gsview_openfile(char *filename);
void gsview_select(void);
void gsview_selectfile(char *filename);
void gsview_display(void);
void gsview_displayfile(char *filename);
void send_orientation_prolog(FILE *f);
void send_epswarn_prolog(FILE *f);
void fix_orientation(FILE *f);
#ifndef VIEWONLY
FILE * gp_open_scratch_file(const char *prefix, char *fname, const char *mode);
#endif
int dfchanged(void);
int dfreopen(void);
void dfclose(void);
void dsc_addmess(void *caller_data, const char *str);
BOOL dsc_scan(PSFILE *psf);
void dsc_getpages(FILE *f, int first, int last);
void dsc_header(FILE *f);
void dsc_dopage(void);
void dsc_skip(int skip);
int map_page(int page);
void page_ordlabel(char *buf, int page_number);
void psfile_free(PSFILE *);
char * psfile_name(PSFILE *psf);
void history_add(int pagenum);
void history_reset(void);
void history_back(void);
void history_forward(void);

/* in gvpdisp.c or gvwdisp.c */

/* in gvccmd.c */
int gsview_command(int command);
BOOL not_open(void);
BOOL not_dsc(void);
BOOL order_is_special(void);
void gserror(UINT id, const char *str, UINT icon, int sound);
int not_implemented(void);
void gsview_check_usersize(void);
void gsview_unzoom(void);
void gsview_language(int new_language);
void gsview_goto_page(int pagenum);

/* in gvpdlg.c or gvwdlg.c */
void centre_dialog(HWND hwnd);
BOOL get_filename(char *filename, BOOL save, int filter, int title, int help);
BOOL query_string(LPCTSTR prompt, char *answer);
BOOL get_page(int *ppage, BOOL multiple, BOOL allpages);
void show_info(void);
void show_about(void);
BOOL get_bbox(void);
BOOL pstoeps_warn(void);
void change_sounds(void);
BOOL install_gsdll(void);
void display_settings(void);
void gs_showmess(void);
HWND gs_showmess_modeless(void);
void gs_addmess_count(const char *str, int count);
void gs_addmess(const char *str);
int get_dsc_response(char *message);

/* in gvceps.c */
#ifndef VIEWONLY
void ps_to_eps(void);
#endif
/* see gvceps.h */

/* in gvwedit.c or gvpedit.c */
#ifndef VIEWONLY
int unload_pstoedit(void);
int gsview_pstoedit(void);
void process_pstoedit(void *arg);
#endif

/* in gvpeps.c or gvwclip.c */
#ifndef VIEWONLY
void clip_convert(void);
void paste_to_file(void);
#endif

/* in gvcprn.c */
#ifndef VIEWONLY
void add_copies(FILE *f, int copies);
struct prop_item_s * get_properties(const char *device);
void gsview_spool(char *, char *);
BOOL psfile_extract(FILE *f, int copies);
char *get_devices(BOOL convert);
void print_cleanup(void);
void gsview_saveas(void);
void gsview_extract(void);
int enum_upp_path(char *path, char *buffer, int len);
char *uppmodel_to_name(char *buffer, char *model);
BOOL copy_for_printer(FILE *pcfile, BOOL convert);
BOOL gsview_cprint(char *psname, char *optname, BOOL convert);
#endif

/* in gvctext.c */
#ifndef VIEWONLY
void gsview_text_extract(void);
void gsview_text_extract_slow(void);
void gsview_text_find(void);
void gsview_text_findnext(void);
void free_text_index(void);
BOOL make_text_index(void);
int word_find(int x, int y);
BOOL wildmatch(const char *w, const char *s);
#endif

/* in gvcdll.c */
int d_orientation(int pagenum);
int gs_execute(const char *str, int len);
int gs_printf(const char *fmt, ...);
void gs_process(void);
int gs_page_skip(int skip);
char * gs_argnext(char *argline, char *arg, BOOL copyquotes);
#ifndef VIEWONLY
#if defined(_Windows) || defined(OS2)
int callback_pstotext(const char *str, unsigned long count);
#endif
#endif

int gs_dll_init(char *devname);

/* gvwgsver.cpp or gvxdll.cpp or gvpdll.cpp */
BOOL get_gs_string(int gs_revision, const char *name, char *ptr, int len);

/* in gvpdll.c or gvwdll.c */
extern GSDLL gsdll;
extern GSDLL_INPUT gsdll_input;
extern IMAGE image;
extern VIEW view;
extern display_callback gsdisplay;

int GSDLLCALL gsdll_poll(void *handle);

#ifdef NOTUSED
void gs_load_dll_cleanup(void);
BOOL gs_load_dll(void);
BOOL gs_free_dll(void);
BOOL gsdll_close(void);
#endif
int get_message(void);
int peek_message(void);
void wait_event(void);

#ifndef VIEWONLY
#if defined(_Windows) || defined(OS2)
int load_pstotext(void);
int unload_pstotext(void);
int send_pstotext_prolog(HINSTANCE hmodule, int resource);
#endif
#endif

/* mutex for psfile */
void request_mutex(void);
void release_mutex(void);

/* gvcpdf.c */
int pdf_add_pdfmark(void);
int pdf_head(void);
int pdf_makedoc(int first, int last);
int pdf_trailer(void);
int pdf_page_init(int pagenum);
int pdf_page(void);
int pdf_checktag(const char *str, int len);
int pdf_extract(FILE *f, int copies);
int pdf_orientation(int page);
BOOL pdf_get_link(int index, PDFLINK *link);
void pdf_free_link(void);
BOOL is_link(float x, float y, PDFLINK *link);
#ifdef UNIX
int pdf_scan(void);
#endif


/* gvwprn.c or gvpprn.c */
#ifndef VIEWONLY
void gsview_print(BOOL convert);
BOOL get_page_range(HWND hwnd, int method);
void printer_cleanup(void);
BOOL query_printer(void);
#endif

#ifndef VIEWONLY
/* gvwmeas.c or gvpmeas.c */
void measure_show(void);
#ifndef UNIX
BOOL dialog_get_float_error(HWND hwnd, int field, float *fres, BOOL error);
void calc_enable_custom(HWND hwnd, BOOL enab);
#endif

/* gvcmeas.c */
int matrix_rotate(const MATRIX *pm, float ang, MATRIX *pmr);
int matrix_invert(const MATRIX *pm, MATRIX *pmr);
void matrix_scale(const MATRIX *pm, float xscale, float yscale, MATRIX *pmr);
void matrix_translate(const MATRIX *pm, float xoffset, float yoffset, MATRIX *pmr);
void matrix_set_unit(MATRIX *matrix, int unit);
void measure_transform_point(float x, float y, float *px, float *py);
void read_measure_profile(PROFILE *prf);
void write_measure_profile(PROFILE *prf);

/* gvcmeas2.c */
void measure_setpoint(float x, float y);
void measure_paint(float x, float y);
void measure_update_last(void);
#ifndef UNIX
void update_dialog_ctm(HWND hwnd, MATRIX *ctm);
void dialog_put_float(HWND hwnd, int field, float fx);
BOOL dialog_get_ctm(HWND hwnd, MATRIX *ctm, BOOL error);
BOOL calc_command(HWND hwnd, int message, MATRIX *ctm, int *unit);
void measure_dialog_unit(void);
#endif
#endif /* !VIEWONLY */

/* gvwreg.cpp or gvpreg.cpp */
BOOL write_registration(unsigned int reg_receipt, unsigned int reg_number, 
  TCHAR *reg_name);
BOOL read_registration(unsigned int *preg_receipt, unsigned int *preg_number,
  TCHAR *reg_name, int reg_len);
BOOL registration_nag(void);

/* gvcreg.cpp */

BOOL registration_check(void);
unsigned int make_reg(unsigned int receipt_number);

/* gvwpgdi.cpp */
#ifndef VIEWONLY
#ifdef _Windows
BOOL start_gvwgs_with_pipe(HDC hdc);
BOOL init_print_gdi(HDC hdc);
#endif
#endif

/* dscutil.cpp */
void dsc_display(CDSC *dsc, void (*dfn)(P2(void *ptr, const char *str)));
