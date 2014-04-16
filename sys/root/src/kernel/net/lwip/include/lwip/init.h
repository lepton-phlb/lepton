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
