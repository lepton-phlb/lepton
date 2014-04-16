
/**
 * \addtogroup lc
 * @{
 */

/**
 * \file
 * Implementation of local continuations based on switch() statment
 * \author Adam Dunkels <adam@sics.se>
 *
 * This implementation of local continuations uses the C switch()
 * statement to resume execution of a function somewhere inside the
 * function's body. The implementation is based on the fact that
 * switch() statements are able to jump directly into the bodies of
 * control structures such as if() or while() statmenets.
 *
 * This implementation borrows heavily from Simon Tatham's coroutines
 * implementation in C:
 * http://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
 */

#ifndef __LC_SWITCH_H__
#define __LC_SWTICH_H__

/* WARNING! lc implementation using switch() does not work if an
   LC_SET() is done within another switch() statement! */

/** \hideinitializer */
typedef unsigned short lc_t;

#define LC_INIT(s) s = 0;

#define LC_RESUME(s) switch(s) { case 0:

#define LC_SET(s) s = __LINE__; case __LINE__:

#define LC_END(s) }

#endif /* __LC_SWITCH_H__ */

/** @} */
