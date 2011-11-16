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

/*
 * Modifications by Lauri Pesonen / lpesonen@clinet.fi, april 1997
 *
 *   Implemented LANManager type password response to MS-CHAP challenges.
 *   Now pppd provides both NT style and LANMan style blocks, and the
 *   prefered is set by option "ms-lanman". Default is to use NT.
 *   The hash text (StdText) was taken from Win95 RASAPI32.DLL.
 *
 *   You should also use DOMAIN\\USERNAME as described in README.MSCHAP80
 */

#define USE_CRYPT

#include "lwip/opt.h"

#if PPP_SUPPORT /* don't build if not configured for use in lwipopts.h */

#if MSCHAP_SUPPORT /* don't build if not configured for use in lwipopts.h */

#include "ppp.h"
#include "pppdebug.h"

#include "md4.h"
#ifndef USE_CRYPT
#include "des.h"
#endif
#include "chap.h"
#include "chpms.h"


/*************************/
/*** LOCAL DEFINITIONS ***/
/*************************/


/************************/
/*** LOCAL DATA TYPES ***/
/************************/
typedef struct {
    u_char LANManResp[24];
    u_char NTResp[24];
    u_char UseNT; /* If 1, ignore the LANMan response field */
} MS_ChapResponse;
/* We use MS_CHAP_RESPONSE_LEN, rather than sizeof(MS_ChapResponse),
   in case this struct gets padded. */



/***********************************/
/*** LOCAL FUNCTION DECLARATIONS ***/
/***********************************/

/* XXX Don't know what to do with these. */
extern void setkey(const char *);
extern void encrypt(char *, int);

static void DesEncrypt (u_char *, u_char *, u_char *);
static void MakeKey (u_char *, u_char *);

#ifdef USE_CRYPT
static void Expand (u_char *, u_char *);
static void Collapse (u_char *, u_char *);
#endif

static void ChallengeResponse(
  u_char *challenge, /* IN   8 octets */
  u_char *pwHash,    /* IN  16 octets */
  u_char *response   /* OUT 24 octets */
);
static void ChapMS_NT(
  char *rchallenge,
  int rchallenge_len,
  char *secret,
  int secret_len,
  MS_ChapResponse *response
);
static u_char Get7Bits(
  u_char *input,
  int startBit
);


/***********************************/
/*** PUBLIC FUNCTION DEFINITIONS ***/
/***********************************/
void
ChapMS( chap_state *cstate, char *rchallenge, int rchallenge_len, char *secret, int secret_len)
{
  MS_ChapResponse response;
#ifdef MSLANMAN
  extern int ms_lanman;
#endif

#if 0
  CHAPDEBUG((LOG_INFO, "ChapMS: secret is '%.*s'\n", secret_len, secret));
#endif
  BZERO(&response, sizeof(response));

  /* Calculate both always */
  ChapMS_NT(rchallenge, rchallenge_len, secret, secret_len, &response);

#ifdef MSLANMAN
  ChapMS_LANMan(rchallenge, rchallenge_len, secret, secret_len, &response);

  /* prefered method is set by option  */
  response.UseNT = !ms_lanman;
#else
  response.UseNT = 1;
#endif

  BCOPY(&response, cstate->response, MS_CHAP_RESPONSE_LEN);
  cstate->resp_length = MS_CHAP_RESPONSE_LEN;
}


/**********************************/
/*** LOCAL FUNCTION DEFINITIONS ***/
/**********************************/
static void
ChallengeResponse( u_char *challenge, /* IN   8 octets */
                   u_char *pwHash,    /* IN  16 octets */
                   u_char *response   /* OUT 24 octets */)
{
  char    ZPasswordHash[21];

  BZERO(ZPasswordHash, sizeof(ZPasswordHash));
  BCOPY(pwHash, ZPasswordHash, 16);

#if 0
  log_packet(ZPasswordHash, sizeof(ZPasswordHash), "ChallengeResponse - ZPasswordHash", LOG_DEBUG);
#endif

  DesEncrypt(challenge, ZPasswordHash +  0, response + 0);
  DesEncrypt(challenge, ZPasswordHash +  7, response + 8);
  DesEncrypt(challenge, ZPasswordHash + 14, response + 16);

#if 0
  log_packet(response, 24, "ChallengeResponse - response", LOG_DEBUG);
#endif
}


#ifdef USE_CRYPT
static void
DesEncrypt( u_char *clear, /* IN  8 octets */
            u_char *key,   /* IN  7 octets */
            u_char *cipher /* OUT 8 octets */)
{
  u_char des_key[8];
  u_char crypt_key[66];
  u_char des_input[66];

  MakeKey(key, des_key);

  Expand(des_key, crypt_key);
  setkey(crypt_key);

#if 0
  CHAPDEBUG((LOG_INFO, "DesEncrypt: 8 octet input : %02X%02X%02X%02X%02X%02X%02X%02X\n",
             clear[0], clear[1], clear[2], clear[3], clear[4], clear[5], clear[6], clear[7]));
#endif

  Expand(clear, des_input);
  encrypt(des_input, 0);
  Collapse(des_input, cipher);

#if 0
  CHAPDEBUG((LOG_INFO, "DesEncrypt: 8 octet output: %02X%02X%02X%02X%02X%02X%02X%02X\n",
             cipher[0], cipher[1], cipher[2], cipher[3], cipher[4], cipher[5], cipher[6], cipher[7]));
#endif
}

#else /* USE_CRYPT */

static void
DesEncrypt( u_char *clear, /* IN  8 octets */
            u_char *key,   /* IN  7 octets */
            u_char *cipher /* OUT 8 octets */)
{
  des_cblock    des_key;
  des_key_schedule  key_schedule;

  MakeKey(key, des_key);

  des_set_key(&des_key, key_schedule);

#if 0
  CHAPDEBUG((LOG_INFO, "DesEncrypt: 8 octet input : %02X%02X%02X%02X%02X%02X%02X%02X\n",
             clear[0], clear[1], clear[2], clear[3], clear[4], clear[5], clear[6], clear[7]));
#endif

  des_ecb_encrypt((des_cblock *)clear, (des_cblock *)cipher, key_schedule, 1);

#if 0
  CHAPDEBUG((LOG_INFO, "DesEncrypt: 8 octet output: %02X%02X%02X%02X%02X%02X%02X%02X\n",
             cipher[0], cipher[1], cipher[2], cipher[3], cipher[4], cipher[5], cipher[6], cipher[7]));
#endif
}

#endif /* USE_CRYPT */


static u_char
Get7Bits( u_char *input, int startBit)
{
  register unsigned int  word;

  word  = (unsigned)input[startBit / 8] << 8;
  word |= (unsigned)input[startBit / 8 + 1];

  word >>= 15 - (startBit % 8 + 7);

  return word & 0xFE;
}

#ifdef USE_CRYPT

/* in == 8-byte string (expanded version of the 56-bit key)
 * out == 64-byte string where each byte is either 1 or 0
 * Note that the low-order "bit" is always ignored by by setkey()
 */
static void
Expand(u_char *in, u_char *out)
{
  int j, c;
  int i;

  for(i = 0; i < 64; in++){
    c = *in;
    for(j = 7; j >= 0; j--) {
      *out++ = (c >> j) & 01;
    }
    i += 8;
  }
}

/* The inverse of Expand
 */
static void
Collapse(u_char *in, u_char *out)
{
  int j;
  int i;
  unsigned int c;

  for (i = 0; i < 64; i += 8, out++) {
    c = 0;
    for (j = 7; j >= 0; j--, in++) {
      c |= *in << j;
    }
    *out = c & 0xff;
  }
}
#endif

static void
MakeKey( u_char *key,    /* IN  56 bit DES key missing parity bits */
         u_char *des_key /* OUT 64 bit DES key with parity bits added */)
{
  des_key[0] = Get7Bits(key,  0);
  des_key[1] = Get7Bits(key,  7);
  des_key[2] = Get7Bits(key, 14);
  des_key[3] = Get7Bits(key, 21);
  des_key[4] = Get7Bits(key, 28);
  des_key[5] = Get7Bits(key, 35);
  des_key[6] = Get7Bits(key, 42);
  des_key[7] = Get7Bits(key, 49);
  
#ifndef USE_CRYPT
  des_set_odd_parity((des_cblock *)des_key);
#endif
  
#if 0
  CHAPDEBUG((LOG_INFO, "MakeKey: 56-bit input : %02X%02X%02X%02X%02X%02X%02X\n",
             key[0], key[1], key[2], key[3], key[4], key[5], key[6]));
  CHAPDEBUG((LOG_INFO, "MakeKey: 64-bit output: %02X%02X%02X%02X%02X%02X%02X%02X\n",
             des_key[0], des_key[1], des_key[2], des_key[3], des_key[4], des_key[5], des_key[6], des_key[7]));
#endif
}

static void
ChapMS_NT( char *rchallenge,
           int rchallenge_len,
           char *secret,
           int secret_len,
           MS_ChapResponse *response)
{
  int      i;
  MDstruct  md4Context;
  u_char    unicodePassword[MAX_NT_PASSWORD * 2];
  static int  low_byte_first = -1;

  /* Initialize the Unicode version of the secret (== password). */
  /* This implicitly supports 8-bit ISO8859/1 characters. */
  BZERO(unicodePassword, sizeof(unicodePassword));
  for (i = 0; i < secret_len; i++) {
    unicodePassword[i * 2] = (u_char)secret[i];
  }
  MDbegin(&md4Context);
  MDupdate(&md4Context, unicodePassword, secret_len * 2 * 8);  /* Unicode is 2 bytes/char, *8 for bit count */

  if (low_byte_first == -1) {
    low_byte_first = (htons((unsigned short int)1) != 1);
  }
  if (low_byte_first == 0) {
    MDreverse((u_long *)&md4Context);  /*  sfb 961105 */
  }

  MDupdate(&md4Context, NULL, 0);  /* Tell MD4 we're done */

  ChallengeResponse(rchallenge, (char *)md4Context.buffer, response->NTResp);
}

#ifdef MSLANMAN
static u_char *StdText = (u_char *)"KGS!@#$%"; /* key from rasapi32.dll */

static void
ChapMS_LANMan( char *rchallenge,
               int rchallenge_len,
               char *secret,
               int secret_len,
               MS_ChapResponse  *response)
{
  int      i;
  u_char    UcasePassword[MAX_NT_PASSWORD]; /* max is actually 14 */
  u_char    PasswordHash[16];
  
  /* LANMan password is case insensitive */
  BZERO(UcasePassword, sizeof(UcasePassword));
  for (i = 0; i < secret_len; i++) {
    UcasePassword[i] = (u_char)toupper(secret[i]);
  }
  DesEncrypt( StdText, UcasePassword + 0, PasswordHash + 0 );
  DesEncrypt( StdText, UcasePassword + 7, PasswordHash + 8 );
  ChallengeResponse(rchallenge, PasswordHash, response->LANManResp);
}
#endif

#endif /* MSCHAP_SUPPORT */

#endif /* PPP_SUPPORT */
