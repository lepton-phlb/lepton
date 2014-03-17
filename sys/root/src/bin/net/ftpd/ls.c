/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/

//based on http://www.meduna.org/sw_ecosftpserver_en.html

#include "kernel/core/kernelconf.h"

#if defined(__tauon_posix__)

   #include <stdlib.h>
   #include <string.h>
   #include "lib/libc/ctype/ctype.h"
   #include "kernel/core/system.h"
   #include "kernel/core/signal.h"
   #include "kernel/core/wait.h"
   #include "kernel/core/devio.h"
   #include "kernel/core/stat.h"
   #include "kernel/core/statvfs.h"
   #include "kernel/core/dirent.h"
   #include "kernel/core/time.h"
   #include "kernel/core/fcntl.h"

   #include "kernel/core/select.h"
   #include "kernel/core/net/socket.h"

   #include "lib/libc/stdio/stdio.h"
   #include "lib/libc/string/string.h"
   #include "lib/pthread/pthread.h"

   #include "kernel/core/libstd.h"

//#define	snprintf		_snprintf //to remove ugly patch see _snprintf in mongoosed.c.

#else
   #include <pkgconf/system.h>

   #include <network.h>
   #include <stdio.h>
   #include <stdlib.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <arpa/inet.h>
   #include <ctype.h>

   #include <time.h>
   #include <unistd.h>
   #include <sys/time.h>
   #include <errno.h>
   #include <string.h>
   #include <sys/types.h>
   #include <sys/stat.h>
   #include <fcntl.h>
   #include <sys/param.h>
   #include <netdb.h>
   #include <dirent.h>
   #include <cyg/fileio/fileio.h>
#endif

#include "ftpd.h"
#include "ftpd_priv.h"

//
#if !defined(__tauon_posix__)
__externC cyg_mtab_entry cyg_mtab[];
__externC cyg_mtab_entry cyg_mtab_end;
#endif

static char **sreaddir( ftp_session_t *session, const char *dirname );

static void addfile( ftp_session_t *session, const char *, const char * );
static void outputfiles( ftp_session_t *session, int );

static int listfile( ftp_session_t *session, const char *name );
static void listdir( ftp_session_t *session, int f, const char *name );

static void wrstr( ftp_session_t *session, int f, const char * s )
{
   int l;

   if ( !s ) {
      if ( session->outptr )
         write( f, session->cpybuffer, session->outptr );
      session->outptr = 0;
      return;
   }

   l = strlen( s );
   if ( l + session->outptr > 1024 ) {
      if ( session->outptr )
         write( f, session->cpybuffer, session->outptr );
      session->outptr = 0;
   }

   if ( l > 1024 ) {
      write( f, s, l );
   } else {
      memcpy( session->cpybuffer+session->outptr, s, l );
      session->outptr += l;
   }
}

/* listfile returns non-zero if the file is a directory */
int listfile( ftp_session_t *session, const char *name )
{
   int rval = 0;
   char m[1024];
   struct stat st={0};
   char months[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                          "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
   struct tm *t;
   char suffix[2];
   char repbuf[128];

   if (combine_path(session->wd, name, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) == 0 &&
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath,
                     sizeof(session->u_session.u_path.fullpath)) == 0 &&
       stat(session->u_session.u_path.fullpath, &st ) == 0) {

      t = localtime( (time_t * ) &st.st_mtime );
      if ( !t ) {
         sprintf(repbuf, "421 Bailing out, localtime() is insane\r\n" );
         write(session->ctrlsock, repbuf, strlen(repbuf));
         return 0;
      }
      suffix[0] = suffix[1] = '\0';
      if ( session->opt_F ) {
         if ( S_ISLNK( st.st_mode ) )
            suffix[0] = '@';
         else if ( S_ISDIR( st.st_mode ) ) {
            suffix[0] = '/';
            rval = 1;
         } else if ( st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH) )
            suffix[0] = '*';
      }
      if ( session->opt_l ) {
         strcpy( m, " ---------" );
         switch ( st.st_mode & S_IFMT ) {
         case S_IFREG:
            m[0] = '-';
            break;
         case S_IFLNK:
            m[0] = 'l';
            break;                              /* readlink() here? */
         case S_IFDIR:
            m[0] = 'd';
            rval = 1;
            break;
         }
         if ( m[0] != ' ' ) {
            char nameline[PATH_MAX + PATH_MAX + 128];
            char timeline[6];

            if ( st.st_mode & S_IRUSR )
               m[1] = 'r';
            if ( st.st_mode & S_IWUSR )
               m[2] = 'w';
            if ( st.st_mode & S_IXUSR )
               m[3] = 'x';
            if ( st.st_mode & S_IRGRP )
               m[4] = 'r';
            if ( st.st_mode & S_IWGRP )
               m[5] = 'w';
            if ( st.st_mode & S_IXGRP )
               m[6] = 'x';
            if ( st.st_mode & S_IROTH )
               m[7] = 'r';
            if ( st.st_mode & S_IWOTH )
               m[8] = 'w';
            if ( st.st_mode & S_IXOTH )
               m[9] = 'x';

            if ( time( NULL ) - st.st_mtime > 180 * 24 * 60 * 60 )
               snprintf( timeline, 6, "%5d", t->tm_year + 1900 );
            else
               snprintf( timeline, 6, "%02d:%02d",
                         t->tm_hour, t->tm_min );

            snprintf( nameline, PATH_MAX + 128 - 1,
                      "%s %3d %d %d %7d %s %2d %s %s", m,
                      st.st_nlink, st.st_uid,
                      st.st_gid,
                      ( unsigned int ) st.st_size, months[t->tm_mon],
                      t->tm_mday, timeline, name );
            addfile( session, nameline, suffix );
         }                      /* hide non-downloadable files */
      } else {
         if ( S_ISREG( st.st_mode ) ||
              S_ISDIR( st.st_mode ) ||
              S_ISLNK( st.st_mode ) ) {
            addfile( session, name, suffix );
         }
      }
   }
   return rval;
}




struct filename {
   struct filename *down;
   struct filename *right;
   int top;
   char line[1];
};





void addfile( ftp_session_t *session, const char *name, const char *suffix )
{
   struct filename *p;
   int l;
   char repbuf[128];

   if ( !name || !suffix )
      return;

   session->matches++;

   l = strlen( name ) + strlen( suffix );
   if ( l > session->colwidth )
      session->colwidth = l;
   l = l + sizeof( struct filename );

   p = ( struct filename * ) malloc( l );

   if ( !p ) {
      sprintf(repbuf, "421 Out of memory\r\n" );
      write(session->ctrlsock, repbuf, strlen(repbuf));
      return;
   }
   snprintf( p->line, l, "%s%s", name, suffix );
   if ( session->tail )
      session->tail->down = p;
   else
      session->head = p;
   session->tail = p;
   session->filenames++;
}


void outputfiles( ftp_session_t *session, int f )
{
   int n;
   struct filename *p;
   struct filename *q;

   if ( !session->head )
      return;

   session->tail->down = NULL;
   session->tail = NULL;
   session->colwidth = ( session->colwidth | 7 ) + 1;
   if ( session->opt_l || !session->opt_C )
      session->colwidth = 75;

   /* set up first column */
   p = session->head;
   p->top = 1;
   n = (session->filenames + (75 / session->colwidth)-1) / (75 / session->colwidth);
   while ( n && p ) {
      p = p->down;
      if ( p )
         p->top = 0;
      n--;
   }

   /* while there's a neighbour to the right, point at it */
   q = session->head;
   while ( p ) {
      p->top = q->top;
      q->right = p;
      q = q->down;
      p = p->down;
   }

   /* some are at the right end */
   while ( q ) {
      q->right = NULL;
      q = q->down;
   }

   /* don't want wraparound, do we? */
   p = session->head;
   while ( p && p->down && !p->down->top )
      p = p->down;
   if ( p && p->down )
      p->down = NULL;

   /* print each line, which consists of each column */
   p = session->head;
   while ( p ) {
      q = p;
      p = p->down;
      while ( q ) {
         char pad[6];
         char *tmp = ( char * ) q;

         if ( q->right ) {
            strcpy( pad, "\t\t\t\t\t" );
            pad[( session->colwidth + 7 - strlen( q->line ) ) / 8] = '\0';
         } else {
            strcpy( pad, "\r\n" );
         }
         wrstr( session, f, q->line );
         wrstr( session, f, pad );
         q = q->right;
         free( tmp );
      }
   }

   /* reset variables for next time */
   session->head = session->tail = NULL;
   session->colwidth = 0;
   session->filenames = 0;
}


/* functions to to sort for qsort() */
static int cmp( const void *a, const void *b ) {
   return strcmp( *( const char ** ) a, *( const char ** ) b );
}
static int cmp_r ( const void *a, const void *b ) {
   return strcmp( *( const char ** ) b, *( const char ** ) a );
}
static int cmp_t ( const void *a, const void *b ) {
   return *(*(const int **)a -2) - *(*(const int **)b -2);
}
static int cmp_rt ( const void *a, const void *b ) {
   return *(*(const int **)b -2) - *(*(const int **)a -2);
}
static int cmp_S ( const void *a, const void *b ) {
   return *(*(const int **)b -1) - *(*(const int **)a -1);
}
static int cmp_rS ( const void *a, const void *b ) {
   return *(*(const int **)a -1) - *(*(const int **)b -1);
}


char **sreaddir( ftp_session_t *session, const char *dirname )
{
   DIR *d;
   struct dirent *de;
   struct stat st;
   int i;
   char **p;
   unsigned int s;
   int dsize;
   int isroot=0;

   if (combine_path(session->wd, dirname, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) < 0 ||
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath,
                     sizeof(session->u_session.u_path.fullpath)) < 0)
      return NULL;

#if !defined(__tauon_posix__)

   /* Root in ecos is special and this is quite a hack. We present to the user
    * all of the single-level entries in the mount table with the exception of devfs.
    * We hope that the length of the mountpoint is never > 64 */
   isroot = session->u_session.u_path.fullpath[0] == '/' &&
            session->u_session.u_path.fullpath[1] == '\0';
   if (isroot) {
      dsize = CYGNUM_FILEIO_MTAB_MAX * (64 + sizeof(void *) + 2 * sizeof(int)) + sizeof(void *);
      p = (char **)malloc( dsize );
      if (!p) {
         errno = ENOMEM;
         return NULL;
      }

      i = 0;
      s = dsize;
      cyg_mtab_entry *mte;
      for (mte = cyg_mtab; mte != &cyg_mtab_end; ++mte) {
         int nlen = strlen(mte->name);
         if (!*mte->name ||
             nlen < 2 || nlen > 63 ||
             mte->name[0] != '/' ||
             strchr(mte->name+1, '/') != NULL ||
             !strcmp(mte->fsname, "devfs")
             )
            continue;

         nlen--;
         s -= nlen + 1;
         strcpy( ((char *)p)+s, mte->name + 1 );
         p[i++] = ((char *)p)+s;
         s -= sizeof(int);
         *((int *)(((char *)p) + s))=0;
         s -= sizeof(int);
         *((int *)(((char *)p) + s))=0;
      }
   }
   else
#endif    // end __tauon_posix__
   {

      if ( stat( session->u_session.u_path.fullpath, &st ) < 0 )
         return NULL;

      if ( !S_ISDIR( st.st_mode ) ) {
         errno = ENOTDIR;
         return NULL;
      }
      if ( (d = opendir( session->u_session.u_path.fullpath ) ) == NULL )
         return NULL;

      /* st_size is enough for any sane fs, but procfs is insane */
      dsize = st.st_size + 100;                 /* okay okay, a little margin is cool */

berkeley:
      dsize = dsize * 2;
      p = (char **)malloc( dsize );

      if ( !p ) {
         closedir( d );
         errno = ENOMEM;
         return NULL;
      }
      s = dsize;
      i = 0;

      while ( (de = readdir( d ) ) != NULL ) {
         struct stat st;
         if ( s < i*sizeof(int) + 3*sizeof(int) + strlen( de->d_name ) + 1 ) {
            /* should leak some memory too, make it perfect : ) */
            free( p );
            rewinddir( d );
            goto berkeley;
         }
         s -= strlen( de->d_name ) + 1;
         strcpy( ((char *)p)+s, de->d_name );
         p[i++] = ((char *)p)+s;

         if (combine_path(session->wd, de->d_name, session->u_session.u_path.combpath,
                          sizeof(session->u_session.u_path.combpath)) == 0 &&
             get_full_path(session, session->u_session.u_path.combpath,
                           session->u_session.u_path.fullpath,
                           sizeof(session->u_session.u_path.fullpath)) == 0 &&
             stat(session->u_session.u_path.fullpath,&st) == 0) {
            s -= sizeof(int);
            *((int *)(((char *)p) + s))=st.st_size;
            s -= sizeof(int);
            *((int *)(((char *)p) + s))=st.st_size;
         } else {
            s -= sizeof(int);
            *((int *)(((char *)p) + s))=0;
            s -= sizeof(int);
            *((int *)(((char *)p) + s))=0;
         }
      }
      closedir( d );
   }
   p[i] = NULL;

   if (session->opt_t)
      if (session->opt_r)
         qsort( p, i, sizeof( char * ), cmp_rt );
      else
         qsort( p, i, sizeof( char * ), cmp_t );
   else if (session->opt_S)
      if (session->opt_r)
         qsort( p, i, sizeof( char * ), cmp_rS );
      else
         qsort( p, i, sizeof( char * ), cmp_S );
   else
   if (session->opt_r)
      qsort( p, i, sizeof( char * ), cmp_r );
   else
      qsort( p, i, sizeof( char * ), cmp );

   return p;
}

/* have to change to the directory first ( speed hack for -R ) */
void listdir( ftp_session_t *session, int f, const char *name )
{
   char **dir;

   dir = sreaddir( session, "." );
   if ( dir ) {
      char **s;
      int d;

      wrstr( session, f, "total 1\r\n" );               /* so what is total anyway */
      s = dir;
      while ( *s ) {
         if ( **s != '.' ) {
            d = listfile( session, *s );
         } else if ( session->opt_a ) {
            d = listfile( session, *s );
            if ( ( (*s)[1] == '\0' ) ||
                 ( ( (*s)[1] == '.' ) &&
                   ( (*s)[2] == '\0' ) ) )
               d = 0;
         } else {
            d = 0;
         }
         if ( !d )
            *s = NULL;
         s++;
      }
      outputfiles( session, f );
      free( dir );
   } else {
      addreply( session, 226, "Error during reading of %s", name );
   }
}


void donlist( ftp_session_t *session, char *arg )
{
   int c;

   session->matches = 0;

   session->opt_l = session->opt_a = session->opt_C = session->opt_d = session->opt_F =
                                                                          session->opt_R =
                                                                             session->opt_r =
                                                                                session->opt_t =
                                                                                   session->opt_S =
                                                                                      0;

   while ( isspace( *arg ) )
      arg++;

   while ( arg && *arg == '-' ) {
      while ( arg++ && isalnum( *arg ) ) {
         switch ( *arg ) {
         case 'a':
            session->opt_a = 1;
            break;
         case 'l':
            session->opt_l = 1;
            session->opt_C = 0;
            break;
         case '1':
            session->opt_l = session->opt_C = 0;
            break;
         case 'C':
            session->opt_l = 0;
            session->opt_C = 1;
            break;
         case 'F':
            session->opt_F = 1;
            break;
         case 'R':
            session->opt_R = 1;
            break;
         case 'd':
            session->opt_d = 1;
            break;
         case 'r':
            session->opt_r = 1;
            break;
         case 't':
            session->opt_t = 1;
            session->opt_S = 0;
            break;
         case 'S':
            session->opt_S = 1;
            session->opt_t = 0;
            break;
         default:
            break;

         }
      }
      while ( isspace( *arg ) )
         arg++;
   }

   c = opendata( session );
   if ( !c )
      return;

   doreply( session );

   if ( session->type == 2 )
      addreply( session, 0, "Binary mode requested, but A (ASCII) used." );

   if (session->opt_R)
      addreply( session, 0, "Recursive listing unimplemented" );
   if ( arg && *arg ) {
      addreply( session, 0, "LIST with arguments unimplemented" );
   } else {
      if ( session->opt_d )
         listfile( session, "." );
      else
         listdir( session, c, "." );
      outputfiles( session, c );
   }
   wrstr( session, c, NULL );
   close( c );
   if ( session->opt_a || session->opt_C || session->opt_d || session->opt_F || session->opt_l ||
        session->opt_r || session->opt_R ||
        session->opt_t || session->opt_S )
      addreply( session, 0, "Options: %s%s%s%s%s%s%s%s%s",
                session->opt_a ? "-a " : "",
                session->opt_C ? "-C " : "",
                session->opt_d ? "-d " : "",
                session->opt_F ? "-F " : "",
                session->opt_l ? "-l " : "",
                session->opt_r ? "-r " : "",
                session->opt_R ? "-R " : "",
                session->opt_S ? "-S " : "",
                session->opt_t ? "-t" : "" );
   addreply( session, 226, "%d matches total", session->matches );
}
