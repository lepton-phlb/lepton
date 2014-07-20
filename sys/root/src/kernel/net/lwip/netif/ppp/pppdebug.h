#ifndef PPPDEBUG_H
#define PPPDEBUG_H

/************************
*** PUBLIC DATA TYPES ***
************************/
/* Trace levels. */
typedef enum {
LOG_CRITICAL = 0,
LOG_ERR      = 1,
LOG_NOTICE   = 2,
LOG_WARNING  = 3,
LOG_INFO     = 5,
LOG_DETAIL   = 6,
LOG_DEBUG    = 7
} LogCodes;


/***********************
*** PUBLIC FUNCTIONS ***
***********************/
/*
 * ppp_trace - a form of printf to send tracing information to stderr
 */
void ppp_trace(int level, const char *format,...);

#define TRACELCP PPP_DEBUG

#if PPP_DEBUG

#define AUTHDEBUG(a) ppp_trace a
#define IPCPDEBUG(a) ppp_trace a
#define UPAPDEBUG(a) ppp_trace a
#define LCPDEBUG(a)  ppp_trace a
#define FSMDEBUG(a)  ppp_trace a
#define CHAPDEBUG(a) ppp_trace a
#define PPPDEBUG(a)  ppp_trace a

#else /* PPP_DEBUG */

#define AUTHDEBUG(a)
#define IPCPDEBUG(a)
#define UPAPDEBUG(a)
#define LCPDEBUG(a)
#define FSMDEBUG(a)
#define CHAPDEBUG(a)
#define PPPDEBUG(a)

#endif /* PPP_DEBUG */

#endif /* PPPDEBUG_H */
