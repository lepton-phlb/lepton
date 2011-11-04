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
#ifndef _LIB_ARM_H_
#define _LIB_ARM_H_

//from ronetix
#ifdef __STDC__
#define LOADREGS(cond, base, reglist...)\
	ldm##cond	base,reglist
#else
#define LOADREGS(cond, base, reglist...)\
	ldm/**/cond	base,reglist
#endif

		.text

#define ENTER	\
		mov	ip,sp	;\
		stmfd	sp!,{r0,r4-r9,fp,ip,lr,pc}	;\
		sub	fp,ip,#4

#define EXIT	\
		LOADREGS(ea, fp, {r0, r4 - r9, fp, sp, pc})

#define EXITEQ	\
		LOADREGS(eqea, fp, {r0, r4 - r9, fp, sp, pc})

#define pull            lsr
#define push            lsl

#define PLD(code...)

		
//from freebsd http://www.freebsd.org/cgi/cvsweb.cgi/src/sys/arm/include/asm.h
#if defined (_ARM_ARCH_4T)
# define RET	bx	lr
# define RETeq	bxeq	lr
# define RETne	bxne	lr
# ifdef __STDC__
#  define RETc(c) bx##c	lr
# else
#  define RETc(c) bx/**/c	lr
# endif
#else
# define RET	mov	pc, lr
# define RETeq	moveq	pc, lr
# define RETne	movne	pc, lr
# ifdef __STDC__
#  define RETc(c) mov##c	pc, lr
# else
#  define RETc(c) mov/**/c	pc, lr
# endif
#endif

#endif
