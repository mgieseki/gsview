/* Written on 27-MAY-1998 13:08 by Hunter Goatley. */

#ifdef VMS

#define MAXPATHLEN 255
#include <unixio.h>
#define unlink(x) delete(x)

#ifndef WIFEXITED	/* VAX C doesn't know these */
#define WIFEXITED(s)   (((s)&0x7F)==0)
#define WIFSIGNALED(s) (((unsigned)(((s)&0x7F)-1))<0x7E)
#define WEXITSTATUS(s) (((s)>>8)&0xFF)
#endif

#if !defined(__DECC) || (__VMS_VER < 70000000)
#include <stdlib.h>
#include <string.h>
#include <unixlib.h>

char *tempnam(char *dir, char *prefix)
{
   int pid, len;
   char *buf;
   pid = getpid();
   len = strlen(dir) + strlen(prefix) + 8;
   buf = malloc(len);
   sprintf(buf, "%s%s%X", dir, prefix, pid);
   return(buf);
}

#include <ctype.h>

int strcasecmp(register char *s1, register char *s2)
{
  register int c1, c2;
  
  while (*s1 && *s2){
      c1 = tolower(*s1);
      c2 = tolower(*s2);
      if (c1 != c2)
          return (c1-c2);
      s1++;
      s2++;
     }
  return (int) (*s1 - *s2);
} 
#endif

#endif	/* VMS */
