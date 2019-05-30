/*
 * doc2html.c  -- program to convert Gnuplot .DOC format to 
 *        World Wide Web (WWW) HyperText Markup Language (HTML) format
 *
 * Created by Russell Lang from doc2ipf by Roger Fearick from 
 *   doc2rtf by M Castro from doc2gih by Thomas Williams.
 *   1994-11-03
 * Modified by Russell Lang 1996-10-15
 *   obtain title from first line of doc file.
 *   Conform to HTML 3.2. 
 * Modified by Russell Lang 1997-11-28
 *   Convert {bml* file.bmp} to <IMG SRC="file.gif">
 * Modified by Russell Lang 1997-12-18
 *   Convert non-ascii characters to &#nnn;
 * Modified by Russell Lang 2000-06-16
 *   Change A HREF and NAME fields to use the topic title, not line number.
 * Modified by Russell Lang 2001-07-12
 *   Convert tags to lower case.
 *   Convert non-ascii characters in links to &#nnn;
 *   Replace isspace() which doesn't work with c > 127.
 *   Allow charset to be specified.  If charset provided, do not
 *   quote non-ascii characters.  This is to allow use with Greek.
 *
 * usage:  doc2html gnuplot.doc gnuplot.htm
 *
 */

/* note that tables must begin in at least the second column to */
/* be formatted correctly and tabs are forbidden */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

#define MAX_LINE_LEN	1024
#define TRUE 1
#define FALSE 0

struct LIST
{
	int level;
	int line;
	char *string;
	struct LIST *next;
	};

struct LIST *list = NULL;
struct LIST *head = NULL;

struct LIST *keylist = NULL;
struct LIST *keyhead = NULL;

int debug = FALSE;
int nolinks = FALSE;
char *charset = NULL;

void parse(FILE *a);
int lookup(char *s);
char *title_from_index(int id);
void refs(int l, FILE *f);
void convert(FILE *a,FILE *b);
void process_line(char *line, FILE *b);
void putquoted(char *s, FILE *f);

int
main(int argc,char *argv[])
{
FILE * infile;
FILE * outfile;

    if (argv[argc-1][0]=='-' && argv[argc-1][1]=='d') {
        debug = TRUE;
	argc--;
    }

    if (argv[argc-1][0]=='-' && argv[argc-1][1]=='n') {
        nolinks = TRUE;
	argc--;
    }

    if ( (argc > 4) || (argc == 1) ) {
        fprintf(stderr,"Usage: %s infile outfile [charset]\n", argv[0]);
        fprintf(stderr,"charset is ISO-8859-1 for ISO-Latin1, ISO-8859-7 for Greek, UTF-8 for Unicode\n");
        return(1);
    }
    if ( (infile = fopen(argv[1],"r")) == (FILE *)NULL) {
        fprintf(stderr,"%s: Can't open %s for reading\n",
            argv[0], argv[1]);
        return(1);
    }
    if (argc >= 3) {
      if ( (outfile = fopen(argv[2],"w")) == (FILE *)NULL) {
        fprintf(stderr,"%s: Can't open %s for writing\n",
            argv[0], argv[2]);
      }
    }
    else {
        outfile = stdout;
    }

    if (argc >= 4)
	charset = argv[3];

    parse(infile);
    convert(infile,outfile);
    return(0);
}

/* scan the file and build a list of line numbers where particular levels are */
void parse(FILE *a)
{
    static char line[MAX_LINE_LEN];
    char *c;
    int lineno=0;
    int lastline=0;

    /* skip title line */
    fgets(line,MAX_LINE_LEN,a);

    while (fgets(line,MAX_LINE_LEN,a)) 
    {
      lineno++;
      if (isdigit(line[0]))
      {
        if (list == NULL)    
            head = (list = (struct LIST *) malloc(sizeof(struct LIST)));
        else
            list = (list->next = (struct LIST *) malloc(sizeof(struct LIST)));
        list->line = lastline = lineno;
        list->level = line[0] - '0';
        list->string = (char *) malloc (strlen(line)+1);
        c = strtok(&(line[1]),"\n");
        strcpy(list->string, c);
        list->next = NULL;
      }
      if (line[0]=='?')
      {
        if (keylist == NULL)    
            keyhead = (keylist = (struct LIST *) malloc(sizeof(struct LIST)));
        else
            keylist = (keylist->next = (struct LIST *) malloc(sizeof(struct LIST)));
        keylist->line = lastline;
        keylist->level = line[0] - '0';
        c = strtok(&(line[1]),"\n");
        if( c == NULL || *c == '\0' ) c = list->string ;
        keylist->string = (char *) malloc (strlen(c)+1);
        strcpy(keylist->string, c);
        keylist->next = NULL;
      }
    }
    rewind(a);
    }

/* look up an in text reference */
int
lookup(char *s)
{
	char *c;
	char tokstr[MAX_LINE_LEN];

	strcpy(tokstr, s);

	/* first try the ? keyword entries */
	keylist = keyhead;
	while (keylist != NULL)
	{
		c = keylist->string;
		while (*c == ' ') c++;
		if (!strcmp(s, c)) return(keylist->line);
		keylist = keylist->next;
		}

	/* then try titles */
#ifdef GNUPLOT
	match = strtok(tokstr, " \n\t");
	l = 0; /* level */
	
	list = head;
	while (list != NULL)
	{
		c = list->string;
		while (*c == ' ') c++;
		if (!strcmp(match, c)) 
		{
			l = list->level;
			match = strtok(NULL, "\n\t ");
			if (match == NULL)
			{
				return(list->line);
				}
			}
		if (l > list->level)
			break;
		list = list->next;
		}
#else
    /* we list keys explicitly, rather than building them from multiple levels  */
    list = head;
    while (list != NULL)
    {
        c = list->string;
        while (*c == ' ') c++;
        if (!strcmp(s, c)) return(list->line);
        list = list->next;
        }
#endif
	return(-1);
}

char *title_from_index(int id)
{
    struct LIST *l = NULL;
    static char empty[] = "";
    if (id < 0)
	return empty;
    l = head;
    while (l != NULL) {
	if (id == l->line)
	    return l->string;
        l = l->next;
    }
    return NULL;
}

void putquoted(char *s, FILE *f)
{
    unsigned int value;
    unsigned int digit;
    for (; *s; s++) {
	if ((charset==NULL) && (*s & 0x80)) {
	    value = *s & 0xff;
	    fputc('&', f);
	    fputc('#', f);
	    digit = value / 100;
	    value -= digit * 100;
	    fputc('0' + digit, f);
	    digit = value / 10;
	    value -= digit * 10;
	    fputc('0' + digit, f);
	    fputc('0' + value, f);
	}
	else
	    fputc(*s, f);
    }
}

/* search through the list to find any references */
void
refs(int l, FILE *f)
{
    int curlevel;
    char *c;
    int inlist = FALSE;

    /* find current line */
    list = head;
    while (list->line != l)
        list = list->next;
    curlevel = list->level;
    list = list->next;        /* look at next element before going on */
    if (list != NULL)
    {
	inlist = TRUE;
	fprintf(f,"<p>\n");
	}

    while (list != NULL) {
        /* we are onto the next topic so stop */
        if (list->level == curlevel)
            break;
        /* these are the next topics down the list */
        if (list->level == curlevel+1) {
            c = list->string;
	    while (*c == ' ') c++;
	    if (nolinks) {
	        fprintf(f,"<b>\n");
		putquoted(c, f);
	        fprintf(f,"</b><br>\n");
	    }
	    else {
		char *p = c;
	        fprintf(f,"<a href=\042#");
		while (*p) {
		    if (*p == ' ')
			fputc('_', f);
		    else {
			if ((charset == NULL) && (*p & 0x80)) {
			    unsigned int value = *p & 0xff;
			    unsigned int digit;
			    fputc('&', f);
			    fputc('#', f);
			    digit = value / 100;
			    value -= digit * 100;
			    fputc('0' + digit, f);
			    digit = value / 10;
			    value -= digit * 10;
			    fputc('0' + digit, f);
			    fputc('0' + value, f);
			}
			else
			    fputc(*p, f);
		    }
		    p++;
		}
	        fprintf(f,"\042>");
		putquoted(c, f);
	        fprintf(f,"</a><br>\n");
	    }
	}
        list = list->next;
    }
    if (inlist)
	fprintf(f,"<p>\n");
}

void
convert(FILE *a,FILE *b)
{
    static char line[MAX_LINE_LEN];
    
    /* generate html header */
    fprintf(b,"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">\n");
    fprintf(b,"<html>\n");
    fprintf(b,"<head>\n");
    fgets(line,sizeof(line)-1,a);
    strtok(line, "\n");
    if (charset)
        fprintf(b,"<meta http-equiv=\042Content-Type\042 content=\042text/html; charset=%s\042>\n", charset);
    else
        fprintf(b,"<meta http-equiv=\042Content-Type\042 content=\042text/html; charset=ISO-8859-1\042>\n");
    fprintf(b,"<title>%s</title>\n", line+1);
    fprintf(b,"</head>\n");
    fprintf(b,"<body>\n");
    fprintf(b,"<h1>%s</h1>\n", line+1);

    /* process each line of the file */
        while (fgets(line, sizeof(line)-1, a)) {
       process_line(line, b);
       }

    /* close final page and generate trailer */
    fprintf(b,"\n<p><hr>Created automatically by doc2html\n");
    fprintf(b,"</body>\n");
    fprintf(b,"</html>\n");
}

void
process_line(char *line, FILE *b)
{
    static int line_count = 0;
    /* quoting can increase the line length five fold */
    static char line2[MAX_LINE_LEN * 5];
    static int last_line;
    char hyplink1[MAX_LINE_LEN] ;
    int i;
    int j;
    static int startpage = 1;
    char topic[MAX_LINE_LEN];
    int k, l;
    static int tabl=0;
    static int para=0;
    static int inquote = FALSE;
    static int inref = FALSE;

    line_count++;

    i = 0;
    j = 0;
    while (line[i] != '\0')
    {
        switch(line[i])
        {
            case '<':
                strcpy( &line2[j], "&lt;" ) ;
                j += strlen( "&lt." ) - 1 ;
                break ;

            case '>':
                strcpy( &line2[j], "&gt;" ) ;
                j += strlen( "&gt." ) - 1 ;
                break ;

            case '&':
                strcpy( &line2[j], "&amp;" ) ;
                j += strlen( "&amp;" ) - 1 ;
                break ;
                                
            case '\r':
            case '\n':
                break;
            case '`':    /* backquotes mean boldface or link */
                if ((!inref) && (!inquote))
                {
                    k=i+1;    /* index into current string */
                    l=0;    /* index into topic string */
                    while ((line[k] != '`') && (line[k] != '\0'))
                    {
                        topic[l] = line[k];
                        k++;
                        l++;
                        }
                    topic[l] = '\0';
                    k = lookup(topic);
		    if ((k > 0) && (k != last_line))
                    {
			if (nolinks)
                            sprintf( hyplink1, "<b>") ;
			else {
			    char *p = title_from_index(k);
			    char *t;
                            sprintf( hyplink1, "<a href=\042#");
			    t = hyplink1 + strlen(hyplink1);
			
			    while (p && *p) {
				if (*p == ' ')
				    *t++ = '_';
				else 
				    *t++ = *p;
				p++;
			    }
			    *t = '\0';
			    strcat(hyplink1, "\042>") ;
			}
                        strcpy( line2+j, hyplink1 ) ;
                        j += strlen( hyplink1 )-1 ;
                        
                        inref = k;
                        }
                    else
                    {
                        if (debug && k != last_line)
                            fprintf(stderr,"Can't make link for \042%s\042 on line %d\n",topic,line_count);
                        line2[j++] = '<';
                        line2[j++] = 'b';
                        line2[j] = '>';
                        inquote = TRUE;
                        }
                    }
                else
                {
                    if (inquote && inref)
                        fprintf(stderr, "Warning: Reference Quote conflict line %d\n", line_count);
                    if (inquote)
                    {
                        line2[j++] = '<';
                        line2[j++] = '/';
                        line2[j++] = 'b';
                        line2[j] = '>';
                        inquote = FALSE;
                        }
                    if (inref)
                    {
                        /* must be inref */
                        line2[j++] = '<';
                        line2[j++] = '/';
			if (nolinks)
                            line2[j++] = 'b';
			else
                            line2[j++] = 'a';
                        line2[j] = '>';
                        inref = 0;
                        }
                }
                break;
            default:
		if ((charset == NULL) && ((unsigned int)(line[i]) >= 127)) {
		    /* quote non-ASCII characters */
		    unsigned int value = line[i] & 0xff;
		    unsigned int digit;
		    line2[j++] = '&';
		    line2[j++] = '#';
		    digit = value / 100;
		    value -= digit * 100;
		    line2[j++] = digit + '0';
		    digit = value / 10;
		    value -= digit * 10;
		    line2[j++] = digit + '0';
		    line2[j++] = value + '0';
		    line2[j] = ';';
		}
		else
                    line2[j] = line[i];
            }
        i++;
        j++;
        line2[j] = '\0';
        }

    i = 1;

    switch(line[0]) {        /* control character */
       case '?': {            /* interactive help entry */
#ifdef FIXLATER
               if( intable ) intablebut = TRUE ;
               fprintf(b,"\n:i1. %s", &(line[1])); /* index entry */
#endif
               break;
       }
       case '-': {            /* filename */
          break;            /* ignore */
       }
       case '@': {            /* start/end table */
          break;            /* ignore */
       }
       case '#': {            /* latex table entry */
          break;            /* ignore */
       }
       case '%': {            /* troff table entry */
          break;            /* ignore */
       }
       case '\n':            /* empty text line */
	    if (tabl)
	        fprintf(b,"</pre>\n"); /* rjl */
            para = 0;
            tabl = 0;
            fprintf(b,"<p>");
            break;
       case ' ': {            /* normal text line */
          if ((line2[1] == '\0') || (line2[1] == '\n'))
          {
                fprintf(b,"<p>"); 
                para = 0;
		tabl = 0;
                }
          if (line2[1] == ' ') 
          {
                if (!tabl)
                {
		    fprintf(b,"<pre>\n");
                    }
                fprintf(b,"%s\n",&line2[1]); 
                tabl = 1;
                para = 0;
                }
          else if (strncmp(line2+1, "{bml", 4)==0)
	  {
		/* assume bitmap is available as GIF */
		char *p;
		fprintf(b, "\n<img src=\042");
		for (p=line2+1; *p && *p!=' '; p++)
		   ; /* skip over bml text */
		for (; *p && *p==' '; p++)
		   ; /* skip over spaces */
		for (; *p && *p!='.' && *p!=' '; p++)
		    fprintf(b, "%c", *p);
		fprintf(b, ".gif\042>\n");
	      }
	  else 
          {
		if (tabl) {
		    fprintf(b,"</pre>\n"); /* rjl */
                    fprintf(b,"<p>"); 	/* rjl */
		}
                tabl = 0;
                if (!para)
                    para = 1;        /* not in para so start one */
                  fprintf(b,"%s \n",&line2[1]); 
                }
          break;
       }
       default: {
          if (isdigit(line[0])) { /* start of section */
	    if (tabl)
	          fprintf(b,"</pre>\n"); /* rjl */
            if (!startpage)
            {
                refs(last_line,b);
                }
            para = 0;                    /* not in a paragraph */
            tabl = 0;
            last_line = line_count;
            startpage = 0;
	    if (debug)
		fprintf( stderr, "%d: %s\n", line_count, &line2[1] ) ;
            k=lookup(&line2[1]) ;
	    /* output unique ID and section title */
	    if (nolinks)
                fprintf(b,"<hr>\n<h%c>", line[0]=='1'?line[0]:line[0]-1);
	    else {
		char *p = &(line2[1]);
                fprintf(b,"<hr>\n<h%c><a name=\042", 
			line[0]=='1'?line[0]:line[0]-1);
		while (*p) {
		    if (*p == ' ')
			fputc('_', b);
		    else 
			fputc(*p, b);
		    p++;
		}
                fprintf(b,"\042>");
	    }
            fprintf(b,&(line2[1])); /* title */
            fprintf(b,"</a></h%c>\n", line[0]=='1'?line[0]:line[0]-1) ;
          } else
            fprintf(stderr, "unknown control code '%c' in column 1, line %d\n",
                line[0], line_count);
          break;
       }
    }
}

