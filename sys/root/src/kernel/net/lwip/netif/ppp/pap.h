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

#ifndef PAP_H
#define PAP_H

#if PAP_SUPPORT /* don't build if not configured for use in lwipopts.h */

/*************************
*** PUBLIC DEFINITIONS ***
*************************/
/*
 * Packet header = Code, id, length.
 */
#define UPAP_HEADERLEN (sizeof (u_char) + sizeof (u_char) + sizeof (u_short))


/*
 * UPAP codes.
 */
#define UPAP_AUTHREQ 1 /* Authenticate-Request */
#define UPAP_AUTHACK 2 /* Authenticate-Ack */
#define UPAP_AUTHNAK 3 /* Authenticate-Nak */

/*
 * Client states.
 */
#define UPAPCS_INITIAL 0 /* Connection down */
#define UPAPCS_CLOSED  1 /* Connection up, haven't requested auth */
#define UPAPCS_PENDING 2 /* Connection down, have requested auth */
#define UPAPCS_AUTHREQ 3 /* We've sent an Authenticate-Request */
#define UPAPCS_OPEN    4 /* We've received an Ack */
#define UPAPCS_BADAUTH 5 /* We've received a Nak */

/*
 * Server states.
 */
#define UPAPSS_INITIAL 0 /* Connection down */
#define UPAPSS_CLOSED  1 /* Connection up, haven't requested auth */
#define UPAPSS_PENDING 2 /* Connection down, have requested auth */
#define UPAPSS_LISTEN  3 /* Listening for an Authenticate */
#define UPAPSS_OPEN    4 /* We've sent an Ack */
#define UPAPSS_BADAUTH 5 /* We've sent a Nak */


/************************
*** PUBLIC DATA TYPES ***
************************/

/*
 * Each interface is described by upap structure.
 */
typedef struct upap_state {
  int us_unit;           /* Interface unit number */
  const char *us_user;   /* User */
  int us_userlen;        /* User length */
  const char *us_passwd; /* Password */
  int us_passwdlen;      /* Password length */
  int us_clientstate;    /* Client state */
  int us_serverstate;    /* Server state */
  u_char us_id;          /* Current id */
  int us_timeouttime;    /* Timeout (seconds) for auth-req retrans. */
  int us_transmits;      /* Number of auth-reqs sent */
  int us_maxtransmits;   /* Maximum number of auth-reqs to send */
  int us_reqtimeout;     /* Time to wait for auth-req from peer */
} upap_state;


/***********************
*** PUBLIC FUNCTIONS ***
***********************/

extern upap_state upap[];

void upap_setloginpasswd(int unit, const char *luser, const char *lpassword);
void upap_authwithpeer  (int, char *, char *);
void upap_authpeer      (int);

extern struct protent pap_protent;

#endif /* PAP_SUPPORT */

#endif /* PAP_H */
