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
   #include "kernel/core/ioctl.h"
   #include "kernel/core/fcntl.h"

   #include "kernel/core/select.h"
   #include "kernel/core/net/socket.h"

   #include "lib/libc/stdio/stdio.h"
   #include "lib/libc/string/string.h"
   #include "lib/pthread/pthread.h"

   #include "kernel/core/libstd.h"

   #define SHUT_RDWR 3
//#define	snprintf		_snprintf //to remove ugly patch see _snprintf in mongoosed.c.
#else

   #define _GNU_SOURCE

   #include <pkgconf/system.h>

   #include <network.h>
   #include <stdio.h>
   #include <stdlib.h>
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <arpa/inet.h>
   #include <ctype.h>

   #include <time.h>
   #include <stdarg.h>
   #include <unistd.h>
   #include <signal.h>
   #include <sys/time.h>
   #include <errno.h>
   #include <string.h>
   #include <sys/types.h>

   #include <netinet/ip.h>
   #include <sys/stat.h>
   #include <fcntl.h>
   #include <sys/param.h>
   #include <sys/ioctl.h>
   #include <netdb.h>
#endif

#include "ftpd.h"
#include "ftpd_priv.h"

#define PATH_SEPARATOR_CHAR '/'
#define PATH_SEPARATOR_STRING "/"

#define FTP_CMDSIZE (PATH_MAX + 32)

int sfgets( ftp_session_t * );
int parser( ftp_session_t * );
void douser( ftp_session_t *, const char *name );
void dopass( ftp_session_t *, const char *password );
void docwd( ftp_session_t *, const char *dir );
void doretr( ftp_session_t *, const char *name );
void dorest (ftp_session_t *, const char *name );
void dodele( ftp_session_t *, const char *name );
void dostor( ftp_session_t *, const char *name );
void domkd( ftp_session_t *, const char *name );
void dormd( ftp_session_t *, const char *name );
void domdtm( ftp_session_t *, const char *name );
void dosize( ftp_session_t *, const char *name );
void doport( ftp_session_t *, unsigned int ip, unsigned int port );
void dopasv( ftp_session_t *, int );
void error( ftp_session_t *, int n, const char *msg );
void quit421( ftp_session_t *, const char * message, int lineno );

void domode( ftp_session_t *, const char *arg );
void dostru( ftp_session_t *, const char *arg );
void dotype( ftp_session_t *, const char *arg );
void dornfr( ftp_session_t *, const char *name );
void dornto( ftp_session_t *, const char *name );





struct reply {
   struct reply *prev;
   char line[1];
};



void addreply( ftp_session_t *session, int code, const char *line,... )
{
   struct reply *p;
   struct reply *q;
   char buf[PATH_MAX + 50];
   va_list ap;
   int offs;

   if ( code )
      session->replycode = code;
   q = NULL;

   va_start( ap, line );
   vsnprintf( buf, PATH_MAX + 50, line, ap );
   va_end( ap );

   offs = strlen( buf );
   while ( offs && isspace( buf[offs] ) )
      buf[offs--] = '\0';
   if ( !offs && buf[0] )
      offs++;
   while ( offs ) {
      while ( offs && buf[offs] != '\n' && isspace( buf[offs] ) )
         buf[offs--] = '\0';
      while ( offs && buf[offs] != '\n' )
         offs--;
      if ( offs )
         offs++;
      p = (struct reply *)malloc( sizeof(struct reply) + strlen(buf+offs) );
      if ( !p ) {
         quit421( session, "Out of memory", __LINE__ );
         return;
      }
      strcpy( p->line, buf + offs );
      if ( q ) {
         p->prev = q->prev;
         q->prev = p;
      } else {
         p->prev = session->lastreply;
         session->lastreply = p;
      }
      q = p;
      if ( offs )
         buf[--offs] = '\r';
   }
}



void replycont( ftp_session_t *session, struct reply *p )
{
   if ( p ) {
      char repbuf[128];
      replycont( session, p->prev );
      sprintf(repbuf, "%3d-%s\r\n", session->replycode, p->line );
      write(session->ctrlsock, repbuf, strlen(repbuf));
      free( (char * ) p );
   }
}


void doreply( ftp_session_t *session )
{
   if ( session->lastreply ) {
      char repbuf[128];
      replycont( session, session->lastreply->prev );
      sprintf(repbuf, "%3d %s\r\n", session->replycode, session->lastreply->line );
      write(session->ctrlsock, repbuf, strlen(repbuf));
      free( (char * ) session->lastreply );
      session->lastreply = NULL;
   }
}



/* this is dog-slow.  but now it's 100% in compliance with the protocol. */

int sfgets( ftp_session_t *session )
{
   int l;
   int nread;
   fd_set rs;
   struct timeval tv;

   FD_ZERO( &rs );
   tv.tv_sec = session->idletime;
   tv.tv_usec = 0;
   nread = 0;
   l = 0;
   do {
      FD_SET( session->ctrlsock, &rs );
      select(  session->ctrlsock+1, &rs, NULL, NULL, &tv );
      if ( ioctl( session->ctrlsock, FIONREAD, &nread ) < 0 )
         nread = 0;

      if ( FD_ISSET( session->ctrlsock, &rs ) ) {
         int i=0;

         i = read( session->ctrlsock, session->cmd+i, /*1*/ FTP_CMDSIZE - 2-l );
         if ( i == 0 ) {
            return 0;
         } else if ( i < 0 ) {
            return 0;
         }

         l=i;

         if ( session->cmd[l-1] == '\n' ) {
            /* if you need to session->debug ftpd, you can (e.g.) connect
               to it using an ordinary client, connect to the
               process using gdb or another ptrace()-aware
               session->debugger, and set a breakpoint on the next
               line. ftpd will hit the breakpoint immediately
               after the next command or whever the timeout hits. */
            session->cmd[l] = '\0';
            return 1;
         }
         /*
                        if ( l < FTP_CMDSIZE - 2 )
                                l++;
         */
      } else {
         return 0;
      }
   } while( 1 );
}


int parser( ftp_session_t *session )
{
   char *arg;
   int n;

   while ( 1 ) {
      doreply( session );

      if ( !sfgets( session ) ) {
         continue;
         //addreply( session, 421, "Timeout (%d seconds) or error.", session->idletime );
         //return -1;
      }
      if ( session->debug )
         addreply( session, 0, "%s", session->cmd );

      n = 0;
      while ( isalpha( session->cmd[n] ) && n < FTP_CMDSIZE ) {
         session->cmd[n] = tolower( session->cmd[n] );
         n++;
      }

      if ( !n ) {
         addreply( session, 221,
                   "Goodbye.  You uploaded %d and downloaded %d kbytes.",
                   (session->uploaded+1023)/1024, (session->downloaded+1023)/1024 );
         continue;
      }
      while ( isspace( session->cmd[n] ) && n < FTP_CMDSIZE )
         session->cmd[n++] = '\0';

      arg = session->cmd + n;

      while ( session->cmd[n] && n < FTP_CMDSIZE )
         n++;
      n--;

      while ( isspace( session->cmd[n] ) )
         session->cmd[n--] = '\0';

      if ( strlen( session->cmd ) > 10 ) {
         addreply( session, 500, "Unknown command." );
      } else if ( strlen( arg ) >= PATH_MAX ) {           /* ">=" on purpose. */
         addreply( session, 501, "Cannot handle %d-character file names" );
      } else if ( !strcasecmp( session->cmd, "user" ) ) {
         douser( session, arg );
      } else if ( !strcasecmp( session->cmd, "pass" ) ) {
         dopass( session, arg );
      } else if ( !strcasecmp( session->cmd, "quit" ) ) {
         addreply( session, 221,
                   "Goodbye.  You uploaded %d and downloaded %d kbytes.",
                   (session->uploaded+1023)/1024, (session->downloaded+1023)/1024 );
         return -1;
      } else if ( !strcasecmp( session->cmd, "noop" ) ) {
         addreply( session, 200, "NOOP command successful" );
      } else if ( !strcasecmp( session->cmd, "syst" ) ) {
         addreply( session, 215, "UNIX Type: L8" );
      } else if ( !strcasecmp( session->cmd, "port" ) ||
                  !strcasecmp( session->cmd, "eprt" ) ) {
         /* don't auto-login for PORT or PASV, but do auto-login
            for the command which _uses_ the data connection */
         unsigned int a1, a2, a3, a4, p1, p2;
         if ( session->epsvall ) {
            addreply( session, 501, "Cannot use PORT/EPRT after EPSV ALL" );
         } else if ( session->cmd[0] == 'e' && strncasecmp( arg, "|2|", 3 ) == 0 ) {
            addreply( session, 522, "IPv6 not supported, use IPv4 (1)" );
         } else if ( session->cmd[0] == 'e' &&
                     5 == sscanf( arg, "|1|%u.%u.%u.%u|%u|",
                                  &a1, &a2, &a3, &a4, &p1 ) &&
                     a1 < 256 && a2 < 256 && a3 < 256 && a4 < 256 &&
                     p1 < 65536 ) {
            doport( session, (a1 << 24) + (a2 << 16) + (a3 << 8) + a4, p1 );
         } else if ( session->cmd[0] == 'p' &&
                     6 == sscanf( arg, "%u,%u,%u,%u,%u,%u",
                                  &a1, &a2, &a3, &a4, &p1, &p2 ) &&
                     a1 < 256 && a2 < 256 && a3 < 256 && a4 < 256 &&
                     p1 < 256 && p2 < 256 ) {
            doport( session, (a1 << 24) + (a2 << 16) + (a3 << 8) + a4,
                    ( (p1 << 8 ) + p2 ) );
         } else {
            addreply( session, 501, "Syntax error." );
         }
      } else if ( !strcasecmp( session->cmd, "pasv" ) ) {
         dopasv( session, 0 );
      } else if ( !strcasecmp( session->cmd, "epsv" ) ) {
         if ( !strcasecmp( arg, "all" ) ) {
            addreply( session, 220, "OK; will reject non-EPSV data connections" );
            session->epsvall++;
         } else if ( !strcasecmp( arg, "2" ) ) {
            addreply( session, 522,
                      "IPv6 not supported, use IPv4 (1)" );
         } else if ( strlen( arg ) == 0 || !strcasecmp( arg, "1" ) ) {
            dopasv( session, 1 );
         } else {
         }
      } else if ( !strcasecmp( session->cmd, "pwd" ) ||
                  !strcasecmp( session->cmd, "xpwd" ) ) {
         if ( session->loggedin )
            addreply( session, 257, "\"%s\"", session->wd );
         else
            addreply( session, 550, "Not logged in" );
      } else if( !strcasecmp( session->cmd, "auth" ) ) {
         /* RFC 2228 Page 5 Authentication/Security mechanism (AUTH) */
         addreply( session, 502, "Security extensions not implemented" );
      } else {
         /* from this point, all commands trigger an automatic login */
         douser( session, NULL );

         if ( !strcasecmp( session->cmd, "cwd" ) ) {
            docwd( session, arg );
         } else if ( !strcasecmp( session->cmd, "cdup" ) ) {
            docwd( session, ".." );
         } else if ( !strcasecmp( session->cmd, "retr" ) ) {
            if ( arg && *arg )
               doretr( session, arg );
            else
               addreply( session, 501, "No file name" );
         } else if ( !strcasecmp( session->cmd, "rest" ) ) {
            if (arg && *arg)
               dorest(session, arg);
            else
               addreply (session, 501, "No restart point");
         } else if ( !strcasecmp( session->cmd, "dele" ) ) {
            if ( arg && *arg )
               dodele( session, arg );
            else
               addreply( session, 501, "No file name" );
         } else if ( !strcasecmp( session->cmd, "stor" ) ) {
            if ( arg && *arg )
               dostor( session, arg );
            else
               addreply( session, 501, "No file name." );
         } else if ( !strcasecmp( session->cmd, "mkd" ) ||
                     !strcasecmp( session->cmd, "xmkd" ) ) {
            if ( arg && *arg )
               domkd( session, arg );
            else
               addreply( session, 501, "No directory name." );
         } else if ( !strcasecmp( session->cmd, "rmd" ) ||
                     !strcasecmp( session->cmd, "xrmd" ) ) {
            if ( arg && *arg )
               dormd( session, arg );
            else
               addreply( session, 550, "No directory name." );
         } else if ( !strcasecmp( session->cmd, "list" ) ||
                     !strcasecmp( session->cmd, "nlst" ) ) {
            donlist( session, (arg && *arg ) ? arg :         "-l" );
         } else if ( !strcasecmp( session->cmd, "type" ) ) {
            dotype( session, arg );
         } else if ( !strcasecmp( session->cmd, "mode" ) ) {
            domode( session, arg );
         } else if ( !strcasecmp( session->cmd, "stru" ) ) {
            dostru( session, arg );
         } else if ( !strcasecmp( session->cmd, "abor" ) ) {
            addreply( session, 226, "ABOR succeeded." );
         } else if ( !strcasecmp( session->cmd, "site" ) ) {
            char *sitearg;

            sitearg = arg;
            while ( sitearg && *sitearg && !isspace( *sitearg ) )
               sitearg++;
            if ( sitearg )
               *sitearg++ = '\0';

            if ( !strcasecmp( arg, "idle" ) ) {
               if ( !*sitearg ) {
                  addreply( session, 501, "SITE IDLE: need argument" );
               } else {
                  unsigned long int i = 0;

                  i = strtoul( sitearg, &sitearg, 10 );
                  if ( sitearg && *sitearg ) {
                     addreply( session, 501, "Garbage (%s) after value (%u)",
                               sitearg, i );
                  } else {
                     if ( i > 7200 )
                        i = 7200;
                     if ( i < 10 )
                        i = 10;
                     session->idletime = i;
                     addreply( session, 200, "Idle time set to %u seconds", i );
                  }
               }
            } else if ( arg && *arg ) {
               addreply( session, 500, "SITE %s unknown", arg );
            } else {
               addreply( session, 500, "SITE: argument needed" );
            }
         } else if ( !strcasecmp( session->cmd, "xdbg" ) ) {
            session->debug++;
            addreply( session, 200,
                      "XDBG command succeeded, session->debug level is now %d.",
                      session->debug );
         } else if ( !strcasecmp( session->cmd, "mdtm" ) ) {
            domdtm( session, (arg && *arg ) ? arg : "" );
         } else if ( !strcasecmp( session->cmd, "size" ) ) {
            dosize( session, (arg && *arg ) ? arg : "" );
         } else if ( !strcasecmp( session->cmd, "rnfr" ) ) {
            if ( arg && *arg )
               dornfr( session, arg );
            else
               addreply( session, 550, "No file name given." );
         } else if ( !strcasecmp( session->cmd, "rnto" ) ) {
            if ( arg && *arg )
               dornto( session, arg );
            else
               addreply( session, 550, "No file name given." );
         } else {
            addreply( session, 500, "Unknown command." );
         }

         if ( strcasecmp( session->cmd, "rest" ) )
            session->restartat = 0;
      }
   }
}


void douser( ftp_session_t *session, const char *username )
{
   if ( session->loggedin ) {
      if ( username ) {
         if ( !session->guest )
            addreply( session, 530, "You're already logged in." );
         else
            addreply( session, 230, "Anonymous user logged in." );
      }
      return;
   }
   if ( username &&
        strcasecmp( username, "ftp" ) &&
        strcasecmp( username, "anonymous" ) ) {

      strncpy( session->account, username, 8 );
      session->account[8] = '\0';
      addreply( session, 331, "User %s OK.  Password required.", session->account );
      session->loggedin = 0;
   } else {
      /* the 230 will be overwritten if this is an implicit login */
      addreply( session, 230, "Anonymous user logged in." );
      strcpy( session->account, "ftp" );
      session->loggedin = session->guest = 1;
   }
   strcpy( session->wd, PATH_SEPARATOR_STRING );
}



void dopass( ftp_session_t *session, const char *password )
{
   if ( session->loggedin ||
        (session->server->check_pwd == 0 ||
         (*session->server->check_pwd)(session->account, password) == 0)) {
      addreply( session, 230, "OK.  Current directory is %s", session->wd );
      session->loggedin = 1;
   } else {
      addreply( session, 530, "Sorry" );
   }
}


void docwd( ftp_session_t *session, const char *dir )
{
   struct stat st;

   if (combine_path(session->wd, dir, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) < 0 ||
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath, sizeof(session->wd)) < 0 ||
       ((session->u_session.u_path.fullpath[0] != PATH_SEPARATOR_CHAR ||
         session->u_session.u_path.fullpath[1] != '\0') &&
        (stat( session->u_session.u_path.fullpath, &st ) < 0 || !S_ISDIR( st.st_mode )))
       ) {

      addreply( session, 530, "Can't change directory to %s", session->u_session.u_path.combpath );
      return;
   }

   strcpy(session->wd, session->u_session.u_path.combpath);

   addreply( session, 250, "Changed to %s", session->wd );
}


void dopasv( ftp_session_t *session, int useepsv )
{
   unsigned int fodder;
   unsigned int a;
   unsigned int p;
   unsigned int on;
   struct sockaddr_in dataconn;         /* my data connection endpoint */
   unsigned int firstporttried;

   memset(&dataconn, 0, sizeof(dataconn));

   if ( session->datafd ) {      /* for buggy clients */
      close( session->datafd );
      session->datafd = 0;
   }

   session->datafd = socket( AF_INET, SOCK_STREAM, 0 );
   if ( session->datafd < 0 ) {
      error( session, 425, "Can't open passive connection" );
      session->datafd = 0;
      return;
   }
   on = 1;
#if !defined(__tauon_posix__)
   if ( setsockopt( session->datafd, SOL_SOCKET, SO_REUSEADDR,
                    &on, sizeof(on) ) < 0 ) {
      error( session, 421, "setsockopt" );
      return;
   }
#endif

   dataconn = session->ctrlconn;
   if ( session->server->firstport && session->server->lastport )
      firstporttried = session->server->firstport +
                       ( session->thread_hdl %
                         (session->server->lastport-session->server->firstport+1) );
   else
      firstporttried = 0;

   p = firstporttried;;
   dataconn.sin_port = ntohs( p );
   while ( bind( session->datafd,
                 (struct sockaddr *)&dataconn, sizeof(dataconn) ) < 0 ) {
      if ( firstporttried ) {
         p--;
         if ( p < session->server->firstport )
            p = session->server->lastport;
      }
      if ( firstporttried == 0 || firstporttried == p ) {
         if ( firstporttried )
            addreply( session, 0, "TCP ports %d to %d inclusive are all busy",
                      session->server->firstport, session->server->lastport );
         error( session, 425, "Can't bind to socket" );
         close( session->datafd );
         session->datafd = 0;
         return;
      }
      dataconn.sin_port = ntohs( p );
   }
   (void)listen( session->datafd, 1 );          /* maybe 0 on some unices? */

   fodder = sizeof( dataconn );
   if ( getsockname( session->datafd, (struct sockaddr *)&dataconn, &fodder ) < 0 ) {
      error( session, 425, "Can't getsockname( dataconn )" );
      close( session->datafd );
      session->datafd = 0;
      return;
   }
   a = ntohl( dataconn.sin_addr.s_addr );
   p = ntohs( (unsigned short int ) ( dataconn.sin_port ) );
   if ( useepsv )
      addreply( session, 229, "Extended Passive mode OK (|||%d|)", p );
   else
      addreply( session, 227, "Passive mode OK (%d,%d,%d,%d,%d,%d)",
                (a >> 24) & 255, (a >> 16) & 255, (a >> 8) & 255, a & 255,
                (p >> 8) & 255, p & 255 );
   session->passive = 1;
   return;
}


void doport( ftp_session_t *session, unsigned int a, unsigned int p )
{
   struct sockaddr_in dataconn;         /* his endpoint */
   int on;

   memset(&dataconn, 0, sizeof(dataconn));

   if ( session->datafd ) {      /* for buggy clients saying PORT over and over */
      close( session->datafd );
      session->datafd = 0;
   }

   session->datafd = socket( AF_INET, SOCK_STREAM, 0 );
   if ( session->datafd < 0 ) {
      error( session, 425, "Can't make data socket" );
      session->datafd = 0;
      return;
   }
   on = 1;
#if !defined(__tauon_posix__)
   if ( setsockopt( session->datafd, SOL_SOCKET, SO_REUSEADDR,
                    &on, sizeof(on) ) < 0 ) {
      error( session, 421, "setsockopt" );
      return;
   }
#endif

   dataconn = session->ctrlconn;
   dataconn.sin_port = htons( (short ) /*20*/ 0 );     /* FTP data connection port */
   if ( bind( session->datafd, (struct sockaddr *)&dataconn, sizeof(dataconn) ) < 0 ) {
      error( session, -220, "bind" );
      close( session->datafd );
      session->datafd = 0;
      return;
   }

   if ( session->debug )
      addreply( session, 0, "My data connection endpoint is %s:%d",
                inet_ntoa( *(struct in_addr *)&dataconn.sin_addr.s_addr ),
                ntohs( dataconn.sin_port ) );

   session->peerdataport = p;

   if ( htonl( a ) != session->peer.sin_addr.s_addr ) {
      addreply( session, 425, "Will not open connection to %d.%d.%d.%d (only to %s)",
                (a >> 24) & 255, (a >> 16) & 255, (a >> 8) & 255, a & 255,
                inet_ntoa( session->peer.sin_addr ) );
      close( session->datafd );
      session->datafd = 0;
      return;
   }

   session->passive = 0;

   addreply( session, 200, "PORT command successful" );
   return;
}



int opendata( ftp_session_t *session )
{
   struct sockaddr_in dataconn;         /* his data connection endpoint */
   int fd;
   int fodder;
   socklen_t sl;

   memset(&dataconn, 0, sizeof(dataconn));

   if ( !session->datafd ) {
      error( session, 425, "No data connection" );
      return 0;
   }

   if ( session->passive ) {
      fd_set rs;
      struct timeval tv;

      FD_ZERO( &rs );
      FD_SET( session->datafd, &rs );
      tv.tv_sec = session->idletime;
      tv.tv_usec = 0;
      /* I suppose it would be better to listen for ABRT too... */
      if ( !select( session->datafd + 1, &rs, NULL, NULL, &tv ) ) {
         addreply( session, 421,
                   "timeout (no connection for %d seconds)", session->idletime );
         doreply( session );
         return 0;
      }
      sl = sizeof( dataconn );
      fd = accept( session->datafd, (struct sockaddr *)&dataconn, &sl );
      if ( fd < 0 ) {
         error( session, 421, "accept failed" );
         close( session->datafd );
         session->datafd = 0;
         return 0;
      }
      if ( !session->guest && dataconn.sin_addr.s_addr != session->peer.sin_addr.s_addr ) {
         addreply( session, 425, "Connection must originate at %s",
                   inet_ntoa( dataconn.sin_addr ) );
         close( session->datafd );
         session->datafd = 0;
         return 0;
      }
      addreply( session, 150, "Accepted data connection from %s:%d",
                inet_ntoa( dataconn.sin_addr ),
                ntohs((unsigned short int)dataconn.sin_port) );
   } else {
      dataconn.sin_addr.s_addr = session->peer.sin_addr.s_addr;
      dataconn.sin_port = htons( session->peerdataport );
      dataconn.sin_family = AF_INET;

      if (connect(session->datafd, (struct sockaddr *)&dataconn, sizeof(dataconn))) {
         addreply( session, 425, "Could not open data connection to %s port %d: %s",
                   inet_ntoa( dataconn.sin_addr ), session->peerdataport,
                   strerror( errno ) );
         close( session->datafd );
         session->datafd = 0;
         return 0;
      }
      fd = session->datafd;
      session->datafd = 0;
      addreply( session, 150, "Connecting to %s:%d",
                inet_ntoa( dataconn.sin_addr ), session->peerdataport );
   }

   fodder = CYGNUM_NET_FTPSERVER_TCP_WINDOW;
   setsockopt( fd, SOL_SOCKET, SO_SNDBUF, (char *)&fodder, sizeof(int) );

   fodder = CYGNUM_NET_FTPSERVER_TCP_WINDOW;            /* not that important, but... */
   setsockopt( fd, SOL_SOCKET, SO_RCVBUF, (char *)&fodder, sizeof(int) );

   return fd;
}


void dodele( ftp_session_t *session, const char *name )
{
   if (combine_path(session->wd, name, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) < 0 ||
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath,
                     sizeof(session->u_session.u_path.fullpath)) < 0) {
      char buffer[PATH_MAX + 40];
      snprintf( buffer, PATH_MAX+39,"Can't get path of %s", name );
      error( session, 550, buffer );
      return;
   }

   if ( session->guest )
      addreply( session, 550, "Anonymous users can not delete files." );
   else if ( remove( session->u_session.u_path.fullpath ) )     //lepton modif before use unlink instead remove
      addreply( session, 550, "Could not delete %s: %s", name, strerror( errno ) );
   else
      addreply( session, 250, "Deleted %s", name );
}


void doretr( ftp_session_t *session, const char *name )
{
   int c, f, o;
   struct stat st;
   int left;
   int flag;


   if (combine_path(session->wd, name, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) < 0 ||
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath,
                     sizeof(session->u_session.u_path.fullpath)) < 0) {
      char buffer[PATH_MAX + 40];
      snprintf( buffer, PATH_MAX+39, "Can't get path of %s", name );
      error( session, 550, buffer );
      return;
   }

   f = open( session->u_session.u_path.fullpath, O_RDONLY,0 );
   if ( f < 0 ) {
      char buffer[PATH_MAX + 40];
      snprintf( buffer, PATH_MAX+39, "Can't open %s", name );
      error( session, 550, buffer );
      return;
   }
   if ( fstat( f, &st ) ) {
      close( f );
      error( session, 451, "can't find file size" );
      return;
   }
   if ( session->restartat && ( session->restartat > st.st_size ) ) {
      addreply( session, 451, "Restart offset %d is too large for file size %d.\n"
                "Restart offset reset to 0.",
                session->restartat, st.st_size );
      return;
   }
   if ( !S_ISREG( st.st_mode ) ) {
      close( f );
      addreply( session, 450, "Not a regular file" );
      return;
   }

   c = opendata( session );
   if ( !c ) {
      close( f );
      return;
   }

   if ( session->restartat == st.st_size ) {
      /* some clients insist on doing this.  I can't imagine why. */
      addreply( session, 226,
                "Nothing left to download.  Restart offset reset to 0." );
      close( f );
      close( c );
      return;
   }

   flag = 1;
   ioctl( c, FIONBIO, &flag );

   if ( session->type == 1 ) {
      addreply( session, 0,
                "NOTE: ASCII mode requested, but binary mode used" );
   }
   if ( st.st_size - session->restartat > 4096 )
      addreply( session, 0, "%.1f kbytes to download",
                (st.st_size - session->restartat) / 1024.0 );

   doreply( session );

   o = session->restartat;
   left = st.st_size - o;
   while ( left > 0 ) {
      size_t w;
      int chunkleft;

      if (lseek(f, o, SEEK_SET) < 0) {
         error( session, 451, "lseek of file failed" );
         close( f );
         close( c );
         return;
      }

      chunkleft = st.st_size - o;
      if (chunkleft > sizeof(session->u_session.xferbuffer))
         chunkleft = sizeof(session->u_session.xferbuffer);

      w = read(f, session->u_session.xferbuffer, chunkleft);
      if (w < 0) {
         error( session, 451, "reading of file failed" );
         close( f );
         close( c );
         return;
      }

      if (w == 0)
         break;

      w = write( c, session->u_session.xferbuffer, (size_t) w );
      if ( (int ) w < 0 ) {
         if ( errno == EAGAIN ) {
            /* wait idletime seconds for progress */
            fd_set rs;
            fd_set ws;
            struct timeval tv;

            FD_ZERO( &rs );
            FD_ZERO( &ws );
            FD_SET( session->ctrlsock, &rs );
            FD_SET( c, &ws );
            tv.tv_sec = session->idletime;
            tv.tv_usec = 0;
            select( c + 1, &rs, &ws, NULL, &tv );
            if ( FD_ISSET( session->ctrlsock, &rs ) ) {
               /* we assume is is ABRT since nothing else is legal */
               addreply( session, 426, "Transfer aborted" );
               close( f );
               close( c );
               return;
            } else if ( !( FD_ISSET( c, &ws ) ) ) {
               /* client presumably gone away */
               return;
            }
            w = 0;
         } else {
            error( session, 450, "Error during write to data connection" );
            close( f );
            close( c );
            return;
         }
      }
      left -= w;
      o += w;
   }

   addreply( session, 226, "File written successfully" );
   close( f );
   close( c );

   session->downloaded = session->downloaded + st.st_size - session->restartat;

   if ( session->restartat ) {
      session->restartat = 0;
      addreply( session, 0, "Restart offset reset to 0." );
   }
}


void dorest (ftp_session_t *session, const char *name)
{
   char *endptr;
   session->restartat = strtoul( name, &endptr, 10 );
   if ( *endptr ) {
      session->restartat = 0;
      addreply( session, 501, "RESTART needs numeric parameter.\n"
                "Restart offset set to 0." );
   } else {
      addreply( session, 350,
                "Restarting at %ld. Send STOR or RETR to initiate transfer.",
                session->restartat );
   }
}



/* next two functions contributed by Patrick Michael Kane <modus@asimov.net> */
void domkd( ftp_session_t *session, const char *name )
{
   if ( session->guest ) {
      addreply( session, 550, "Sorry, anonymous users are not allowed to "
                "make directories." );
   }

   if (combine_path(session->wd, name, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) < 0 ||
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath,
                     sizeof(session->u_session.u_path.fullpath)) < 0) {
      char buffer[PATH_MAX + 40];
      snprintf( buffer, PATH_MAX+39, "Can't get path of %s", name );
      error( session, 550, buffer );
      return;
   }

   if ( (mkdir( session->u_session.u_path.fullpath, 0755 ) ) < 0 )
      error( session, 550, "Can't create directory" );
   else
      addreply( session, 257, "MKD command successful." );
}


void dormd( ftp_session_t *session, const char *name )
{
   if ( session->guest ) {
      addreply( session, 550, "Sorry, anonymous users are not allowed to "
                "remove directories." );
      return;
   }

   if (combine_path(session->wd, name, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) < 0 ||
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath,
                     sizeof(session->u_session.u_path.fullpath)) < 0) {
      char buffer[PATH_MAX + 40];
      snprintf( buffer, PATH_MAX+39, "Can't get path of %s", name );
      error( session, 550, buffer );
      return;
   }

   if ( (rmdir( session->u_session.u_path.fullpath ) ) < 0 )
      error( session, 550, "Can't remove directory" );
   else
      addreply( session, 250, "RMD command successful." );
}


void dostor( ftp_session_t *session, const char *name )
{
   int c, f;
   char *p;
   int r;
   int filesize;
   struct stat st;
   // Added for ascii upload
   unsigned int i,j;
   char *q;

   filesize = 0;

   if ( session->type < 1 ) {
      addreply( session, 503, "Only ASCII and binary modes are supported" );
      return;
   }
   if (combine_path(session->wd, name, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) < 0 ||
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath,
                     sizeof(session->u_session.u_path.fullpath)) < 0) {
      char buffer[PATH_MAX + 40];
      snprintf( buffer, PATH_MAX+39, "Can't get path of %s", name );
      error( session, 550, buffer );
      return;
   }

   if ( !stat( session->u_session.u_path.fullpath, &st ) )  {
      if ( session->guest ) {
         addreply( session, 553,
                   "Anonymous users may not overwrite existing files" );
         return;
      }
   } else if ( errno != ENOENT ) {
      error( session, 553, "Can't check for file presence" );
      return;
   }
   f = open( session->u_session.u_path.fullpath, O_CREAT | O_TRUNC | O_WRONLY, 0600 );
   if ( f < 0 ) {
      error( session, 553, "Can't open file" );
      return;
   }
   if ( session->restartat && lseek(f, session->restartat, SEEK_SET) < 0) {
      error (session, 451, "can't seek" );
      return;
   }

   c = opendata( session );
   if ( !c ) {
      close( f );
      return;
   }
   doreply( session );

   do {
      /* wait idletime seconds for data to be available */
      fd_set rs;
      struct timeval tv;

      FD_ZERO( &rs );
      FD_SET( session->ctrlsock, &rs );
      FD_SET( c, &rs );
      tv.tv_sec = session->idletime;
      tv.tv_usec = 0;
      select( c + 1, &rs, NULL, NULL, &tv );
      if ( FD_ISSET( session->ctrlsock, &rs ) ) {
         addreply( session, 0, "ABRT is the only legal command while uploading" );
         addreply( session, 426, "Transfer aborted" );
         close( f );
         close( c );
         addreply( session, 0, "%s %s", name,
                   unlink( session->u_session.u_path.fullpath ) ? "partially uploaded" : "removed" );
         return;
      } else if ( !( FD_ISSET( c, &rs ) ) ) {
         /* client presumably gone away */
         unlink( session->u_session.u_path.fullpath );
         return;
      }
      r = read( c, &session->u_session.xferbuffer, sizeof(session->u_session.xferbuffer) );
      if ( r > 0 ) {
         p = session->u_session.xferbuffer;

         filesize += r;
         while ( r ) {
            size_t w;

            if ( session->type == 1 ) {
               int k = 0;
               i = 0;
               j = 0;
               while ( i < (size_t) r ) {
                  if ( p[i] == '\r' ) {
                     i++;
                     k++;
                  }
                  session->cpybuffer[j++] = session->u_session.xferbuffer[i++];
               }
               q = session->cpybuffer;
               r -= k;
               w = write( f, q, (size_t) r );
            } else {
               w = write( f, p, (size_t) r );
            }

            if ( (signed int)w < 0 ) {
               error( session, -450, "Error during write to file" );
               close( f );
               close( c );
               addreply( session, 450, "%s %s", name,
                         unlink(
                            session->u_session.u_path.fullpath ) ? "partially uploaded" : "removed" );
               return;
            }
            r -= w;
            p += w;
         }
         r = 1;
      } else if ( r < 0 ) {
         error( session, -451, "Error during read from data connection" );
         close( f );
         close( c );
         addreply( session, 451, "%s %s", name,
                   unlink( session->u_session.u_path.fullpath ) ? "partially uploaded" : "removed" );
         return;
      }
   } while ( r > 0 );

   addreply( session, 226, "File written successfully" );

   close( f );
   close( c );
   session->uploaded += filesize;
   if ( session->restartat ) {
      session->restartat = 0;
      addreply( session, 0, "Restart offset reset to 0." );
   }
}



void domdtm( ftp_session_t *session, const char *name )
{
   struct stat st;
   struct tm *t;

   if ( !name || !*name ) {
      addreply( session, 500, "Command not understood" );
      return;
   }

   if (combine_path(session->wd, name, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) < 0 ||
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath,
                     sizeof(session->u_session.u_path.fullpath)) < 0) {
      char buffer[PATH_MAX + 40];
      snprintf( buffer, PATH_MAX+39, "Can't get path of %s", name );
      error( session, 550, buffer );
      return;
   }


   if ( stat( session->u_session.u_path.fullpath, &st ) ) {
      if ( session->debug )
         addreply( session, 0, "arg is %s, wd is %s", name, session->wd );
      addreply( session, 550, "Unable to stat()" );
   } else if ( !S_ISREG( st.st_mode ) ) {
      addreply( session, 550, "Not a regular file" );
   } else {
      t = gmtime( (time_t * ) &st.st_mtime );
      if ( !t ) {
         addreply( session, 550, "gmtime() returned NULL" );
      } else {
         addreply( session, 213, "%04d%02d%02d%02d%02d%02d",
                   t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                   t->tm_hour, t->tm_min, t->tm_sec );
      }
   }
   return;
}


void dosize( ftp_session_t *session, const char *name )
{
   struct stat st;

   if ( !name || !*name ) {
      addreply( session, 500, "Command not understood" );
      return;
   }

   if (combine_path(session->wd, name, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) < 0 ||
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath,
                     sizeof(session->u_session.u_path.fullpath)) < 0) {
      char buffer[PATH_MAX + 40];
      snprintf( buffer, PATH_MAX+39, "Can't get path of %s", name );
      error( session, 550, buffer );
      return;
   }

   if ( stat( session->u_session.u_path.fullpath, &st ) ) {
      if ( session->debug )
         addreply( session, 0, "arg is %s, wd is %s", name, session->wd );
      addreply( session, 550, "Unable to stat()" );
   } else if ( !S_ISREG( st.st_mode ) ) {
      addreply( session, 550, "Not a regular file" );
   } else {
      addreply( session, 213, "%ld", (long)st.st_size );
   }
   return;
}


void dotype( ftp_session_t *session, const char *arg )
{
   session->replycode = 200;            /* bloody awful hack */

   if ( !arg || !*arg ) {
      addreply( session, 501, "TYPE needs an argument\n"
                "Only A(scii), I(mage) and L(ocal) are supported" );
   } else if ( tolower(*arg) == 'a' )
      session->type = 1;
   else if ( tolower(*arg) == 'i' )
      session->type = 2;
   else if ( tolower(*arg) == 'l' ) {
      if ( arg[1] == '8' ) {
         session->type = 2;
      } else if ( isdigit( arg[1] ) ) {
         addreply( session, 504, "Only 8-bit bytes are supported" );
      } else {
         addreply( session, 0, "Byte size not specified" );
         session->type = 2;
      }
   } else {
      addreply( session, 504, "Unknown TYPE: %s", arg );
   }

   addreply( session, 0, "TYPE is now %s", ( session->type > 1 ) ? "8-bit binary" : "ASCII" );
}


void dostru( ftp_session_t *session, const char *arg )
{
   if ( !arg || !*arg )
      addreply( session, 500, "No arguments\n"
                "Not that it matters, only STRU F is supported" );
   else if ( strcasecmp( arg, "F" ) )
      addreply( session, 504, "STRU %s is not supported\nOnly F(ile) is supported",
                arg );
   else
      addreply( session, 200, "F OK" );
}


void domode( ftp_session_t *session, const char *arg )
{
   if ( !arg || !*arg )
      addreply( session, 500, "No arguments\n"
                "Not that it matters, only MODE S is supported" );
   else if ( strcasecmp( arg, "S" ) )
      addreply( session, 504, "MODE %s is not supported\n"
                "Only S(tream) is supported", arg );
   else
      addreply( session, 200, "S OK" );
}


void dornfr( ftp_session_t *session, const char *name )
{
   struct stat st;
   if ( session->guest ) {
      addreply( session, 550,
                "Sorry, anonymous users are not allowed to rename files." );
      return;
   }

   if (combine_path(session->wd, name, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) < 0 ||
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath,
                     sizeof(session->u_session.u_path.fullpath)) < 0) {
      char buffer[PATH_MAX + 40];
      snprintf( buffer, PATH_MAX+39, "Can't get path of %s", name );
      error( session, 550, buffer );
      return;
   }

   if ( ( stat( session->u_session.u_path.fullpath, &st ) ) == 0 ) {
      if ( session->renamefrom ) {
         addreply( session, 0, "Aborting previous rename operation." );
         (void) free( session->renamefrom );
      }
      session->renamefrom = strdup( session->u_session.u_path.fullpath );
      addreply( session, 350, "RNFR accepted - file exists, ready for destination." );
   } else {
      addreply( session, 550, "File does not exist!" );
   }
   return;
}


/* rnto can return 550, which is not strictly allowed */

void dornto( ftp_session_t *session, const char *name )
{
   struct stat st;
   if ( session->guest ) {
      addreply( session, 550,
                "Sorry, anonymous users are not allowed to rename files." );
   }

   if (combine_path(session->wd, name, session->u_session.u_path.combpath,
                    sizeof(session->u_session.u_path.combpath)) < 0 ||
       get_full_path(session, session->u_session.u_path.combpath,
                     session->u_session.u_path.fullpath,
                     sizeof(session->u_session.u_path.fullpath)) < 0) {
      char buffer[PATH_MAX + 40];
      snprintf( buffer, PATH_MAX+39, "Can't get path of %s", name );
      error( session, 550, buffer );
      return;
   }

   if ( ( stat( session->u_session.u_path.fullpath, &st ) ) == 0 )
      addreply( session, 550, "RENAME Failed - destination file already exists." );
   else if ( !session->renamefrom )
      addreply( session, 503, "Need RNFR before RNTO" );
   else if ( rename( session->renamefrom, session->u_session.u_path.fullpath ) < 0 )
      addreply( session, 550, "Rename failed: %s", strerror( errno ) );
   else
      addreply( session, 250, "File renamed." );

   if ( session->renamefrom )
      (void) free( session->renamefrom );
   session->renamefrom = 0;
   return;
}


void error( ftp_session_t *session, int n, const char *msg )
{
   int e = errno;
   addreply( session, n, "%s: %s", msg, strerror( e ) );
}


void quit421( ftp_session_t *session, const char * message, int lineno )
{
   char repbuf[128];
   sprintf( repbuf, "421 %s\r\n", message );
   write(session->ctrlsock, repbuf, strlen(repbuf));
}


int ftpd_session( ftp_session_t *session )
{
   int fodder;
   socklen_t sl;
   char repbuf[128];

   sl = sizeof(struct sockaddr_in);

   if ( getsockname( session->ctrlsock, (struct sockaddr *)&session->ctrlconn, &sl ) ) {
      sprintf(repbuf, "421 Cannot getsockname( STDIN ), errno=%d\r\n", errno );
      write(session->ctrlsock, repbuf, strlen(repbuf));
      return -1;
   }

   session->loggedin = 0;
   memset(&session->peer, 0, sizeof(session->peer));

   if ( getpeername( session->ctrlsock, (struct sockaddr *)&session->peer, &sl ) ) {
      sprintf(repbuf, "421 Cannot getpeername( STDIN ), errno=%d\r\n", errno );
      write(session->ctrlsock, repbuf, strlen(repbuf));
      return -1;
   }

   strcpy( session->wd, PATH_SEPARATOR_STRING );

   fodder = 1;
   setsockopt( session->ctrlsock, SOL_SOCKET, SO_OOBINLINE, (char *)&fodder, sizeof(int) );

   addreply( session, 220, "You will be disconnected after %d seconds of inactivity.",
             session->idletime );

   parser( session );

   doreply( session );

   return 0;
}

int combine_path(const char *old_dir, const char *path, char *result_path, int result_path_size) {
   /* old_dir is a valid directory path */
   /* path is a new path - either absolute, or relative to the old_dir */
   /* result_path returns a combined path with .. and . resolved */

   /* Returns -1 for path too long etc */

   int len, path_len;
   const char *src, *sep;
   char *dst;

   /* If relative, copy old_dir, remove trailing slash with the exception of root dir
      If absolute, just copy and return */
   src = (path[0] == PATH_SEPARATOR_CHAR) ? path : old_dir;

   *result_path = '\0';

   len = strlen(src);
   if (len >= result_path_size)
      return -1;

   strcpy(result_path, src);
   if (len == 0)
   {
      result_path[0] = PATH_SEPARATOR_CHAR;
      result_path[1] = '\0';
      ++len;
   }
   else
   {
      if (len > 1 && result_path[len-1] == PATH_SEPARATOR_CHAR)
      {
         result_path[--len] = '\0';
      }
   }

   if (path[0] == PATH_SEPARATOR_CHAR)
      return 0;

   /* Parse the path step by step */
   src = path;
   dst = result_path + len;
   path_len = strlen(path);

   while(*src)
   {
      int elem_len = 0;

      sep = strchr(src, PATH_SEPARATOR_CHAR);
      elem_len = (sep != NULL) ? sep - src : path + path_len - src;

      /* Ignore empty part or the dot */
      if (elem_len == 0 || (elem_len == 1 && *src == '.'))
      {
         src += elem_len + (sep != NULL ? 1 : 0);
         continue;
      }

      /* For two dots remove the last path element from the destination, if possible */
      if (elem_len == 2 && src[0] == '.' && src[1] == '.')
      {
         if (result_path[0] == PATH_SEPARATOR_CHAR && result_path[1] == '\0') {
            src += elem_len + (sep != NULL ? 1 : 0);
            continue;
         }

         --dst;
         while (dst > result_path && *dst != PATH_SEPARATOR_CHAR)
            --dst;

         if (dst == result_path)
            ++dst;

         *dst = '\0';

         src += elem_len + (sep != NULL ? 1 : 0);
         continue;
      }

      /* For everything other add a separator and the element */
      if (dst - result_path + elem_len + 1 >= result_path_size)
         return -1;

      if (result_path[0] != PATH_SEPARATOR_CHAR || result_path[1] != '\0')
         *dst++ = PATH_SEPARATOR_CHAR;
      strncpy(dst, src, elem_len);
      dst += elem_len;
      *dst = '\0';
      src += elem_len + (sep != NULL ? 1 : 0);
   }

   return 0;
}

int get_full_path(ftp_session_t *session, const char *path, char *result_path,
                  int result_path_size) {
   int cr_len = strlen(session->server->chrootdir);
   int p_len = strlen(path);

   if (cr_len + p_len + 1 >= result_path_size)
      return -1;

   if (cr_len <= 1) {
      result_path[0] = PATH_SEPARATOR_CHAR;
      result_path[1] = '\0';
      cr_len = 1;
   }
   else {
      strcpy(result_path, session->server->chrootdir);
      if (result_path[cr_len-1] != PATH_SEPARATOR_CHAR) {
         result_path[cr_len++] = PATH_SEPARATOR_CHAR;
         result_path[cr_len] = '\0';
      }
   }

   if (*path == PATH_SEPARATOR_CHAR)
      ++path;

   strcpy(result_path + cr_len, path);

   p_len = strlen(result_path);
   if (p_len > 1 && result_path[p_len - 1] == PATH_SEPARATOR_CHAR)
      result_path[p_len - 1] = '\0';

   return 0;
}

void* ftpd_session_thrfunc(void* data) {
   ftp_session_t *session = (ftp_session_t *) data;

   //
   session->thread_hdl = pthread_self();
   //
   ftpd_session(session);

   shutdown(session->ctrlsock, SHUT_RDWR);
   close(session->ctrlsock);

   session->can_delete = 1;

   //cyg_thread_exit(); // Caller will delete
   pthread_exit((void*)0);

   return (void*)0;
}

static int ftpd_server( ftp_server_t *server ) {
   struct sockaddr_in sock_addr;
   int sock_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

   struct sockaddr_in addr_client;      /* his data connection endpoint */
   int addr_len= sizeof( addr_client );

   if(-1 == sock_fd)
   {
      return -1;
   }

   if (server->port == 0)
      server->port = CYGNUM_NET_FTPSERVER_PORT;

   if (server->max_nr_of_clients == 0)
      server->max_nr_of_clients = CYGNUM_NET_FTPSERVER_MAX_CLIENTS;

   memset(&sock_addr, 0, sizeof(sock_addr));

   sock_addr.sin_family = AF_INET;
   sock_addr.sin_port = htons(server->port);
   sock_addr.sin_addr.s_addr = INADDR_ANY;

   if(-1 == bind(sock_fd,(/*const*/ void *)&sock_addr, sizeof(sock_addr)))
   {
      close(sock_fd);
      return -1;
   }

   if(-1 == listen(sock_fd, 10))
   {
      close(sock_fd);
      return -1;
   }

   //cyg_mutex_init(&server->mutex);
   pthread_mutex_init(&server->mutex, NULL);
   server->nr_of_clients = 0;
   server->active_sessions = NULL;

   for(;; )
   {
      int conn_fd = accept(sock_fd, (struct sockaddr *)&addr_client, &addr_len);
      ftp_session_t *session = NULL;
      ftp_session_t *search, *last = 0;

      if(0 > conn_fd)
      {
         close(sock_fd);
         continue;
      }

      //cyg_mutex_lock(&server->mutex);
      pthread_mutex_lock(&server->mutex);

      /* Cleanup */
      search = server->active_sessions;
      while (search) {
         if (search->can_delete) {
            session = search;
            search = search->next;
            if (last == 0)
               server->active_sessions = session->next;
            else {
               last->next = session->next;
            }

            --server->nr_of_clients;

            //cyg_thread_delete(session->thread_hdl);
            pthread_cancel(session->thread_hdl);

            free(session);
         } else {
            last = search;
            search = search->next;
         }
      }

      session = 0;
      if (server->nr_of_clients >= server->max_nr_of_clients) {
         static const char err[] = "421 Max number of clients exceeded\r\n";
         write(conn_fd, err, sizeof(err));

         shutdown(conn_fd, SHUT_RDWR);
         close(conn_fd);
      } else {
         session = malloc(sizeof(ftp_session_t));
         if (!session) {
            static const char err[] = "421 Out of memory\r\n";
            write(conn_fd, err, sizeof(err));

            shutdown(conn_fd, SHUT_RDWR);
            close(conn_fd);
         } else {
            memset(session, 0, sizeof(ftp_session_t));

            session->server = server;
            session->ctrlsock = conn_fd;
            session->type = 1;
            session->idletime = CYGNUM_NET_FTPSERVER_IDLE_TIME;
            session->next = server->active_sessions;
            server->active_sessions = session;
            ++server->nr_of_clients;
         }
      }

      //cyg_mutex_unlock(&server->mutex);
      pthread_mutex_unlock(&server->mutex);

      if (session) {
         pthread_t thread_id;
         pthread_attr_t attr;
         /*
                        cyg_thread_create(
                                        CYGNUM_NET_FTPSERVER_PRIO,
                                        ftpd_session_thrfunc,
                                        (cyg_addrword_t) session,
                                        "Ftp Session",
                                        (void *)session->stack,
                                        CYGNUM_NET_FTPSERVER_STACK_SIZE,
                                        &session->thread_hdl,
                                        &session->thread_obj );
                        cyg_thread_resume( session->thread_hdl );
         */

         attr.stacksize = (CYGNUM_NET_FTPSERVER_STACK_SIZE);
         attr.stackaddr = NULL;
#if !defined(__GNUC__)
         attr.priority  = 100;
#else
         attr.priority  = 10;
#endif
         attr.timeslice = 1;
         //
         pthread_create(&thread_id, &attr, ftpd_session_thrfunc, session);
      }
   }
   return 0;
}

int chpwd(const char *user, const char *password)
{
   if (!strcmp(user, "tauon"))
      return strcmp(password, "tauon");

   return -1;
}

/*--------------------------------------------
| Name:        ftpd_main
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int ftpd_main(int argc, char *argv[]){
   ftp_server_t server;

   memset(&server, 0, sizeof(ftp_server_t));
   server.check_pwd = chpwd;
   server.firstport = 30000;
   server.lastport = 30500;
   //strcpy(server.chrootdir, "/fs");

   ftpd_server(&server);

   return 0;
}
