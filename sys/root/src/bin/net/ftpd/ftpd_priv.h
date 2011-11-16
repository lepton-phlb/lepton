/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

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

#ifndef FTPD_PRIVATE_H
#define FTPD_PRIVATE_H

//#include <pkgconf/net_ftpserver.h>
//#include <limits.h>
//#include <sys/socket.h>

#define CYGNUM_NET_FTPSERVER_XFER_BUFFER_SIZE   1024
#define CYGNUM_NET_FTPSERVER_STACK_SIZE         16000//8192
#define CYGNUM_NET_FTPSERVER_TCP_WINDOW         51200 
#define CYGNUM_NET_FTPSERVER_PORT               21
#define CYGNUM_NET_FTPSERVER_MAX_CLIENTS        5
#define CYGNUM_NET_FTPSERVER_IDLE_TIME          1800

struct filename;
struct ftp_session;
struct reply;

typedef struct ftp_session {
	int can_delete;

	ftp_server_t *server;
	struct ftp_session *next;

	int stack[CYGNUM_NET_FTPSERVER_STACK_SIZE / sizeof(int)];

	//void* thread_obj;
	pthread_t thread_hdl;

	unsigned int downloaded;	/* bytes downloaded */
	unsigned int uploaded;	/* bytes uploaded */

	int passive;
	int datafd;		/* data connection file descriptor */
	struct sockaddr_in ctrlconn;	/* stdin/stdout, for using the same ip number */


	char cmd[PATH_MAX + 32];	/* command line - about 30 chars for command */
	char wd[PATH_MAX + 1];	/* current working directory */
	int loggedin;
	char account[9];
	char * renamefrom;
	int epsvall; /* nonzero means we've seen an EPSV ALL */

	int debug;	/* don't give debug output */
	int guest;	/* if non-zero, it's a guest user */

	int ctrlsock; /* Control socket */

	int type;	/* type - 0 = error, 1 = ascii, 2 = binary */

	int restartat;
	unsigned int idletime;

	struct reply *lastreply;
	int replycode;

	struct sockaddr_in peer;
	unsigned short int peerdataport;

	union
	{
		char xferbuffer[CYGNUM_NET_FTPSERVER_XFER_BUFFER_SIZE]; /* Also used for path mangling */
		struct
		{
			char combpath[CYGNUM_NET_FTPSERVER_XFER_BUFFER_SIZE / 2];
			char fullpath[CYGNUM_NET_FTPSERVER_XFER_BUFFER_SIZE / 2];
		}u_path;
	}u_session;

	char cpybuffer[CYGNUM_NET_FTPSERVER_XFER_BUFFER_SIZE]; /* Also used for list output */

	int matches;
	int outptr;

	int opt_a, opt_C, opt_d, opt_F, opt_l, opt_R, opt_r, opt_t, opt_S; /* ls options */

	int colwidth;
	int filenames;

	struct filename *head;
	struct filename *tail;
} ftp_session_t;

void donlist(ftp_session_t *, char * arg);

int opendata(ftp_session_t *);

void addreply( ftp_session_t *, int, const char *, ... );
void doreply( ftp_session_t * );

int combine_path(const char *old_dir, const char *path, char *result_path, int result_path_size);
int get_full_path(ftp_session_t *, const char *path, char *result_path, int result_path_size);
void* ftpd_session_thrfunc( void* );
int ftpd_session( ftp_session_t * );

#endif
