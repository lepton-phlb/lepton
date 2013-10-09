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

#ifndef RANDM_H
#define RANDM_H

/***********************
*** PUBLIC FUNCTIONS ***
***********************/
/*
 * Initialize the random number generator.
 */
void avRandomInit(void);

/*
 * Churn the randomness pool on a random event.  Call this early and often
 * on random and semi-random system events to build randomness in time for
 * usage.  For randomly timed events, pass a null pointer and a zero length
 * and this will use the system timer and other sources to add randomness.
 * If new random data is available, pass a pointer to that and it will be
 * included.
 */
void avChurnRand(char *randData, u32_t randLen);

/*
 * Randomize our random seed value.  To be called for truely random events
 * such as user operations and network traffic.
 */
#if MD5_SUPPORT
#define avRandomize() avChurnRand(NULL, 0)
#else  /* MD5_SUPPORT */
void avRandomize(void);
#endif /* MD5_SUPPORT */

/*
 * Use the random pool to generate random data.  This degrades to pseudo
 * random when used faster than randomness is supplied using churnRand().
 * Thus it's important to make sure that the results of this are not
 * published directly because one could predict the next result to at
 * least some degree.  Also, it's important to get a good seed before
 * the first use.
 */
void avGenRand(char *buf, u32_t bufLen);

/*
 * Return a new random number.
 */
u32_t avRandom(void);


#endif /* RANDM_H */
