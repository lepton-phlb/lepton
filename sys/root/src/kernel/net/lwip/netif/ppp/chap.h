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

#ifndef CHAP_H
#define CHAP_H

/*************************
*** PUBLIC DEFINITIONS ***
*************************/

/* Code + ID + length */
#define CHAP_HEADERLEN 4

/*
 * CHAP codes.
 */

#define CHAP_DIGEST_MD5      5    /* use MD5 algorithm */
#define MD5_SIGNATURE_SIZE   16   /* 16 bytes in a MD5 message digest */
#define CHAP_MICROSOFT       0x80 /* use Microsoft-compatible alg. */
#define MS_CHAP_RESPONSE_LEN 49   /* Response length for MS-CHAP */

#define CHAP_CHALLENGE       1
#define CHAP_RESPONSE        2
#define CHAP_SUCCESS         3
#define CHAP_FAILURE         4

/*
 *  Challenge lengths (for challenges we send) and other limits.
 */
#define MIN_CHALLENGE_LENGTH 32
#define MAX_CHALLENGE_LENGTH 64
#define MAX_RESPONSE_LENGTH  64 /* sufficient for MD5 or MS-CHAP */

/*
 * Client (peer) states.
 */
#define CHAPCS_INITIAL       0 /* Lower layer down, not opened */
#define CHAPCS_CLOSED        1 /* Lower layer up, not opened */
#define CHAPCS_PENDING       2 /* Auth us to peer when lower up */
#define CHAPCS_LISTEN        3 /* Listening for a challenge */
#define CHAPCS_RESPONSE      4 /* Sent response, waiting for status */
#define CHAPCS_OPEN          5 /* We've received Success */

/*
 * Server (authenticator) states.
 */
#define CHAPSS_INITIAL       0 /* Lower layer down, not opened */
#define CHAPSS_CLOSED        1 /* Lower layer up, not opened */
#define CHAPSS_PENDING       2 /* Auth peer when lower up */
#define CHAPSS_INITIAL_CHAL  3 /* We've sent the first challenge */
#define CHAPSS_OPEN          4 /* We've sent a Success msg */
#define CHAPSS_RECHALLENGE   5 /* We've sent another challenge */
#define CHAPSS_BADAUTH       6 /* We've sent a Failure msg */

/************************
*** PUBLIC DATA TYPES ***
************************/

/*
 * Each interface is described by a chap structure.
 */

typedef struct chap_state {
  int unit;                               /* Interface unit number */
  int clientstate;                        /* Client state */
  int serverstate;                        /* Server state */
  u_char challenge[MAX_CHALLENGE_LENGTH]; /* last challenge string sent */
  u_char chal_len;                        /* challenge length */
  u_char chal_id;                         /* ID of last challenge */
  u_char chal_type;                       /* hash algorithm for challenges */
  u_char id;                              /* Current id */
  char *chal_name;                        /* Our name to use with challenge */
  int chal_interval;                      /* Time until we challenge peer again */
  int timeouttime;                        /* Timeout time in seconds */
  int max_transmits;                      /* Maximum # of challenge transmissions */
  int chal_transmits;                     /* Number of transmissions of challenge */
  int resp_transmits;                     /* Number of transmissions of response */
  u_char response[MAX_RESPONSE_LENGTH];   /* Response to send */
  u_char resp_length;                     /* length of response */
  u_char resp_id;                         /* ID for response messages */
  u_char resp_type;                       /* hash algorithm for responses */
  char *resp_name;                        /* Our name to send with response */
} chap_state;


/******************
*** PUBLIC DATA ***
******************/
extern chap_state chap[];

extern struct protent chap_protent;


/***********************
*** PUBLIC FUNCTIONS ***
***********************/

void ChapAuthWithPeer (int, char *, int);
void ChapAuthPeer (int, char *, int);

#endif /* CHAP_H */
