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

/* gvcprf.c */
/* Profile (INI file) routines Common to PM and possibly Windows */

/* Windows provides good Profile functions */
/* PM provides binary PM functions which don't allow easy editing */
/* of the INI file.  Also emx08g crashed when calling PrfOpenProfile. */
/* To keep things consistent, these profile routines should write */
/* INI files compatible with Windows. */
/* Since the INI files are plain text, they are easy to edit */

#include "gvc.h"

void profile_free_section(struct prfsection *section);
PROFILE * profile_cleanup(PROFILE *prf);

/* free keys in the section, but not the section itself */
void
profile_free_section(struct prfsection *section)
{
struct prfentry *pe, *ne;
	pe = section->entry;
	while (pe) { /* free this entry */
	    if (pe->name)
		free(pe->name);
	    if (pe->value)
		free(pe->value);
	    ne = pe->next;
	    free(pe);
	    pe = ne;
	}
	if (section->name)
	    free(section->name);
}

PROFILE *
profile_cleanup(PROFILE *prf)
{
struct prfsection *ps, *ns;
	if (prf == (PROFILE *)NULL)
	    return NULL;
	if (prf->file)
	    fclose(prf->file);
	ps = prf->section;
	while (ps) {  /* free this section */
	    profile_free_section(ps);
	    ns = ps->next;
	    free(ps);
	    ps = ns;
	}
	if (prf->name)
	    free(prf->name);
	free(prf);
	return NULL;
}

PROFILE *
profile_open(const char *filename)
{
char line[256];
PROFILE *prf;
struct prfsection *ps, *ns;
struct prfentry *pe, *ne;
char *p;

	if ( (prf = (PROFILE *)malloc(sizeof(PROFILE))) == (PROFILE *)NULL )
	    return (PROFILE *)NULL;
	prf->changed = FALSE;
	prf->section = NULL;
	if ( (prf->name = (char *)malloc(strlen(filename)+1)) == (char *)NULL )
	    return profile_cleanup(prf);
	strcpy(prf->name, filename);
	if ( (prf->file = fopen(filename, "r")) == (FILE *)NULL )
	    return prf;	/* file doesn't exist - we may be creating it */
	ps = ns = NULL;
	pe = ne = NULL;
	while (fgets(line, sizeof(line), prf->file)) {
	    if (line[0] == '[') { /* new section */
	        if ( (ns = (struct prfsection *)malloc(sizeof(struct prfsection))) 
	            == (struct prfsection *)NULL )
	            return profile_cleanup(prf);
	        ns->name = NULL;
	        ns->entry = NULL;
	        ns->next = NULL;
	        if (ps)
	            ps->next = ns;
	        else
	            prf->section = ns;
		ps = ns;
		pe = NULL;
	        if ( (p = strrchr(line+1, ']')) != (char *)NULL )
	            *p = '\0';
	        if ( (ns->name = (char *)malloc(strlen(line))) == (char *)NULL )
	            return profile_cleanup(prf);
	        strcpy(ns->name, line+1);
	    }
	    else {	/* new entry */
	    	if (ns == (struct prfsection *)NULL)
	    	    continue;
	        if ( (p = strchr(line, '\n')) != (char *)NULL )
	            *p = '\0';
	        if (line[0] == '\0')
		    continue;
	        if ( (ne = (struct prfentry *)malloc(sizeof(struct prfentry))) 
	            == (struct prfentry *)NULL )
	            return profile_cleanup(prf);
	        ne->name = NULL;
	        ne->value = NULL;
	        ne->next = NULL;
	        if (pe)
	            pe->next = ne;
	        else
	            ns->entry = ne;
		pe = ne;
	        if (line[0] == ';') { /* comment line */
	            if ( (ne->value = (char *)malloc(strlen(line)+1)) == (char *)NULL )
	                return profile_cleanup(prf);
	            strcpy(ne->value, line);
	        }
	        else {	/* a real entry */
	            strtok(line, "=");
	            if ( (ne->name = (char *)malloc(strlen(line)+1)) == (char *)NULL )
	                return profile_cleanup(prf);
	            strcpy(ne->name, line);
		    p = line + strlen(line) + 1;
/*
	            if ( (p = strtok(NULL, "=")) == (char *)NULL )
	            	continue;
*/
	            if ( (ne->value = (char *)malloc(strlen(p)+1)) == (char *)NULL )
	                return profile_cleanup(prf);
	            strcpy(ne->value, p);
	        }
	    }
	}
	fclose(prf->file);
	prf->file = NULL;
	return prf;
}

int 
profile_read_string(PROFILE *prf, const char *section, const char *entry, 
    const char *def, char *buffer, int len)
{
struct prfsection *ps;
struct prfentry *pe;
int count;
char *p;
	if (prf == (PROFILE *)NULL)
	    return 0;
	if (buffer == (char *)NULL)
	    return 0;
	ps = prf->section;
	if (section == NULL) {
	    /* return all section names */
	    count = 0;
	    p = buffer;
	    *p = '\0';
	    while (ps) {
		if ( ps->name && ((int)(strlen(ps->name) + 2 + count) < len) ) {
		    strcpy(p, ps->name);
		    count += strlen(ps->name) + 1;
		    p = buffer + count;
		}
	        ps = ps->next;
	    }
	    *p = '\0';
	    return count;
	}
	while (ps) {
	    if (strcmp(ps->name, section) == 0)
	        break;
	    ps = ps->next;
	}
	if (ps == (struct prfsection *)NULL) {
	    strncpy(buffer, def, len); 
	    return min(strlen(buffer), (unsigned int)len);
        }
	/* found section */
	pe = ps->entry;
	if (entry == NULL) {
	    /* return all entry names */
	    count = 0;
	    p = buffer;
	    *p = '\0';
	    while (pe) {
		if ( pe->name && ((int)(count + strlen(pe->name) + 2) < len) ) {
		    strcpy(p, pe->name);
		    count += strlen(pe->name) + 1;
		    p = buffer + count;
		}
	        pe = pe->next;
	    }
	    *p = '\0';
	    return count;
	}
	while (pe) {
	    if (pe->name && (strcmp(pe->name, entry) == 0))
	    	break;
	    pe = pe->next;
	}
	if ( (pe == (struct prfentry *)NULL) ||
	     (pe->value == (char *)NULL) ) {
	    strncpy(buffer, def, len); 
	    return min(strlen(buffer), (unsigned int)len);
	}
	/* return value */
        strncpy(buffer, pe->value, len);	/* got it! */
	return min(strlen(buffer), (unsigned int)len);
}

BOOL
profile_write_string(PROFILE *prf, const char *section, const char *entry, 
    const char *value)
{
struct prfsection *ps, *ns;
struct prfentry *pe, *ne;
	if (prf == (PROFILE *)NULL)
	    return FALSE;
	ns = prf->section;
	ps = NULL;
	while (ns) {
	    if (strcmp(ns->name, section) == 0)
	        break;
	    ps = ns;
	    ns = ns->next;
	}
	if (entry == (char *)NULL) {
	    /* delete section */
	    if (ns == (struct prfsection *)NULL)
		return TRUE;
	    profile_free_section(ns);
	    if (ps)
		ps->next = ns->next;
	    else
		prf->section = ns->next;
	    free(ns);
	    prf->changed = TRUE;
	    return TRUE;
	}
	if (ns == (struct prfsection *)NULL) {
	    /* add section */
	    if ( (ns = (struct prfsection *)malloc(sizeof(struct prfsection))) 
	        == (struct prfsection *)NULL )
	        return FALSE;
	    ns->name = NULL;
	    ns->entry = NULL;
	    ns->next = NULL;
	    if (ps)
	        ps->next = ns;
	    else
	        prf->section = ns;
	    ps = ns;
	    pe = NULL;
	    if ( (ns->name = (char *)malloc(strlen(section)+1)) == (char *)NULL )
	        return FALSE;
	    strcpy(ns->name, section);
        }
	ne = ns->entry;
	pe = NULL;
	while (ne) {
	    if (ne->name && (strcmp(ne->name, entry) == 0))
	    	break;
	    pe = ne;
	    ne = ne->next;
	}
	if (ne == (struct prfentry *)NULL) {
	    /* add new entry */
	    if ( (ne = (struct prfentry *)malloc(sizeof(struct prfentry))) 
	        == (struct prfentry *)NULL )
	        return FALSE;
	    ne->name = NULL;
	    ne->value = NULL;
	    ne->next = NULL;
	    if (pe)
	        pe->next = ne;
	    else
	        ns->entry = ne;
	    pe = ne;
	    if ( (ne->name = (char *)malloc(strlen(entry)+1)) == (char *)NULL )
	        return FALSE;
	    strcpy(ne->name, entry);
	}
	if (ne->value != (char *)NULL)
	    free(ne->value);	/* release old value */
	if (value) { /* change value */
	    if ( (ne->value = (char *)malloc(strlen(value)+1)) == (char *)NULL )
		return FALSE;
	    strcpy(ne->value, value);
	}
	else { /* delete entry */
	    free(ne->name);
	    if (pe)
	        pe->next = ne->next;
	    else
		ns->entry = ne->next;
	    free(ne);
	}
	prf->changed = TRUE;
	return TRUE;
}

BOOL
profile_close(PROFILE *prf)
{
struct prfsection *ps;
struct prfentry *pe;
	if (prf == (PROFILE *)NULL)
		return FALSE;
	if (prf->changed) {
	    if ( (prf->file = fopen(prf->name, "w")) == (FILE *)NULL ) {
		profile_cleanup(prf);
	        return FALSE;
	    }
	    ps = prf->section;
	    while (ps) {
	        if (ps->name)
		    fprintf(prf->file, "[%s]\n", ps->name);
	        pe = ps->entry;
	        while (pe) {
		    if (pe->name) {
		    	if (pe->value)
		    	    fprintf(prf->file, "%s=%s\n", pe->name, pe->value);
		    	else
		    	    fprintf(prf->file, "%s=\n", pe->name);
		    }
		    else {
		    	if (pe->value)
		    	    fprintf(prf->file, "%s\n", pe->value);
		    }
		    pe = pe->next;
	        }
		fprintf(prf->file, "\n");
	        ps = ps->next;
	    }
	}
	profile_cleanup(prf);
	return TRUE;
}

