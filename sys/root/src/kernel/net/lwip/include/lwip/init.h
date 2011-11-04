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
#ifndef __LWIP_INIT_H__
#define __LWIP_INIT_H__

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif

/** X.x.x: Major version of the stack */
#define LWIP_VERSION_MAJOR      1U
/** x.X.x: Minor version of the stack */
#define LWIP_VERSION_MINOR      3U
/** x.x.X: Revision of the stack */
#define LWIP_VERSION_REVISION   2U
/** For release candidates, this is set to 1..254
  * For official releases, this is set to 255 (LWIP_RC_RELEASE)
  * For development versions (CVS), this is set to 0 (LWIP_RC_DEVELOPMENT) */
#define LWIP_VERSION_RC         255U

/** LWIP_VERSION_RC is set to LWIP_RC_RELEASE for official releases */
#define LWIP_RC_RELEASE         255U
/** LWIP_VERSION_RC is set to LWIP_RC_DEVELOPMENT for CVS versions */
#define LWIP_RC_DEVELOPMENT     0U

#define LWIP_VERSION_IS_RELEASE     (LWIP_VERSION_RC == LWIP_RC_RELEASE)
#define LWIP_VERSION_IS_DEVELOPMENT (LWIP_VERSION_RC == LWIP_RC_DEVELOPMENT)
#define LWIP_VERSION_IS_RC          ((LWIP_VERSION_RC != LWIP_RC_RELEASE) && (LWIP_VERSION_RC != LWIP_RC_DEVELOPMENT))

/** Provides the version of the stack */
#define LWIP_VERSION   (LWIP_VERSION_MAJOR << 24   | LWIP_VERSION_MINOR << 16 | \
                        LWIP_VERSION_REVISION << 8 | LWIP_VERSION_RC)

/* Modules initialization */
void lwip_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_INIT_H__ */
