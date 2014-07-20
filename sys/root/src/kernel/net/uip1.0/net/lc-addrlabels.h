
/**
 * \addtogroup lc
 * @{
 */

/**
 * \file
 * Implementation of local continuations based on the "Labels as
 * values" feature of gcc
 * \author
 * Adam Dunkels <adam@sics.se>
 *
 * This implementation of local continuations is based on a special
 * feature of the GCC C compiler called "labels as values". This
 * feature allows assigning pointers with the address of the code
 * corresponding to a particular C label.
 *
 * For more information, see the GCC documentation:
 * http://gcc.gnu.org/onlinedocs/gcc/Labels-as-Values.html
 *
 * Thanks to dividuum for finding the nice local scope label
 * implementation.
 */

#ifndef __LC_ADDRLABELS_H__
#define __LC_ADDRLABELS_H__

/** \hideinitializer */
typedef void * lc_t;

#define LC_INIT(s) s = NULL


#define LC_RESUME(s)                            \
  do {                                          \
    if(s != NULL) {                             \
      goto *s;                                  \
    }                                           \
  } while(0)

#define LC_SET(s)                               \
  do { ({ __label__ resume; resume: (s) = &&resume; }); }while(0)

#define LC_END(s)

#endif /* __LC_ADDRLABELS_H__ */

/**  @} */
