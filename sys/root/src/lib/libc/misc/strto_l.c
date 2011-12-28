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


/*****************************************************************************/
/*                            OPTIONS                                        */
/*****************************************************************************/

/* Set if we want errno set appropriately. */
/* NOTE: Implies _STRTO_ENDPTR below */
#define _STRTO_ERRNO            0

/* Set if we want support for the endptr arg. */
/* Implied by _STRTO_ERRNO. */
#define _STRTO_ENDPTR           1

/*****************************************************************************/
/* Don't change anything that follows.                                       */
/*****************************************************************************/

#if _STRTO_ERRNO
#undef _STRTO_ENDPTR
#define _STRTO_ENDPTR           1
#endif

/*****************************************************************************/

/* Are there actually any machines where this might fail? */
#if 'A' > 'a'
#error ordering assumption violated : 'A' > 'a'
#endif

#include <ctype.h>
#include "lib/libc/ctype/ctype.h"
#include "lib/libc/stdint.h"

#if _STRTO_ERRNO
#include <errno.h>
#endif

unsigned long _strto_l(const char *str, char **endptr, int base, int uflag);


/*
 * This is the main work fuction which handles both strtol (uflag = 0) and
 * strtoul (uflag = 1).
 */
/*--------------------------------------------
| Name:        _strto_l
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
unsigned long _strto_l(const char *str, char **endptr, int base, int uflag)
{
    unsigned long number = 0;
    unsigned long cutoff;
    char *pos = (char *) str;
#if _STRTO_ENDPTR
    char *fail_char = (char *) str;
#endif
    int digit, cutoff_digit;
    int negative;

    while (isspace(*pos)) {	/* skip leading whitespace */
	++pos;
    }

    /* handle optional sign */
    negative = 0;
    switch(*pos) {
    case '-': negative = 1;	/* fall through to increment pos */
    case '+': ++pos;
    }

    if ((base == 16) && (*pos == '0')) { /* handle option prefix */
	++pos;
#if _STRTO_ENDPTR
	fail_char = pos;
#endif
	if ((*pos == 'x') || (*pos == 'X')) {
	    ++pos;
	}
    }
    
    if (base == 0) {		/* dynamic base */
	base = 10;		/* default is 10 */
	if (*pos == '0') {
	    ++pos;
	    base -= 2;		/* now base is 8 (or 16) */
#if _STRTO_ENDPTR
	    fail_char = pos;
#endif
	    if ((*pos == 'x') || (*pos == 'X')) {
		base += 8;	/* base is 16 */
		++pos;
	    }
	}
    }

    if ((base < 2) || (base > 36)) { /* illegal base */
	goto DONE;
    }

    cutoff_digit = ULONG_MAX % base;
    cutoff = ULONG_MAX / base;

    while (1) {
	digit = 40;
	if ((*pos >= '0') && (*pos <= '9')) {
	    digit = (*pos - '0');
	} else if (*pos >= 'a') {
	    digit = (*pos - 'a' + 10);
	} else if (*pos >= 'A') {
	    digit = (*pos - 'A' + 10);
	} else break;

	if (digit >= base) {
	    break;
	}

	++pos;
#if _STRTO_ENDPTR
	fail_char = pos;
#endif

	/* adjust number, with overflow check */
	if ((number > cutoff)
	    || ((number == cutoff) && (digit > cutoff_digit))) {
	    number = ULONG_MAX;
	    if (uflag) {
		negative = 0; /* since unsigned returns ULONG_MAX */
	    }
#if _STRTO_ERRNO
	    __set_errno(ERANGE);
#endif
	} else {
	    number = number * base + digit;
	}

    }

 DONE:
#if _STRTO_ENDPTR
    if (endptr) {
	*endptr = fail_char;
    }
#endif

    if (negative) {
	if (!uflag && (number > ((unsigned long)(-(1+LONG_MIN)))+1)) {
#if _STRTO_ERRNO
	    __set_errno(ERANGE);
#endif
	    return (unsigned long) LONG_MIN;
	}
	return (unsigned long)(-((long)number));
    } else {
	if (!uflag && (number > (unsigned long) LONG_MAX)) {
#if _STRTO_ERRNO
	    __set_errno(ERANGE);
#endif
	    return LONG_MAX;
	}
	return number;
    }
}

/*--------------------------------------------
| Name:        strtoul
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
unsigned long __strtoul(const char *str, char **endptr, int base){
    return _strto_l(str, endptr, base, 1);
}

/*--------------------------------------------
| Name:        strtol
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
long __strtol(const char *str, char **endptr, int base){
    return _strto_l(str, endptr, base, 0);
}

/*--------------------------------------------
| Name:        atoi
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int __atoi(const char *str){
    return((int)_strto_l((str),(char**)0,10, 0));
}

/*--------------------------------------------
| Name:        atol
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
long __atol(const char *str){
    return(_strto_l((str),(char**)0,10, 0));
}

