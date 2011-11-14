/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Philippe Le Boulanger.
Portions created by Philippe Le Boulanger are Copyright (C) 2011 <lepton.phlb@gmail.com>.
All Rights Reserved.

Contributor(s): Jean-Jacques Pitrolle <lepton.jjp@gmail.com>.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license 
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable 
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under 
the MPL, indicate your decision by deleting  the provisions above and replace 
them with the notice and other provisions required by the [eCos GPL] License. 
If you do not delete the provisions above, a recipient may use your version of this file under 
either the MPL or the [eCos GPL] License."
*/


/*============================================
| Compiler Directive   
==============================================*/
#ifndef _DEV_AT91M55800A_UART_S0_H
#define _DEV_AT91M55800A_UART_S0_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/

#if defined(USE_ECOS)
/*UART 0*/
#define __US_CR  	*((unsigned int *) 	0xFFFC0000)
#define __US_MR  	*((unsigned int *) 	0xFFFC0004)
#define __US_IER  *((unsigned int *) 	0xFFFC0008)
#define __US_IDR  *((unsigned int *) 	0xFFFC000C)
#define __US_IMR  *((unsigned int *) 	0xFFFC0010)
#define __US_CSR 	*((unsigned int *)	0xFFFC0014)
#define __US_RHR 	*((unsigned int *) 	0xFFFC0018)
#define __US_THR 	*((unsigned int *) 	0xFFFC001C)
#define __US_BRGR *((unsigned int *) 	0xFFFC0020)
#define __US_RTOR *((unsigned int *) 	0xFFFC0024)
#define __US_TTGR *((unsigned int *) 	0xFFFC0028)
#define __US_RPR 	*((unsigned int *) 	0xFFFC0030)
#define __US_RCR 	*((unsigned int *) 	0xFFFC0034)
#define __US_TPR 	*((unsigned int *) 	0xFFFC0038)
#define __US_TCR 	*((unsigned int *)	0xFFFC003C)

/*APMC*/
#define __APMC_SCER *((unsigned int *) 	0xFFFF4000)
#define __APMC_SCDR *((unsigned int *) 	0xFFFF4004)
#define __APMC_SCSR *((unsigned int *) 	0xFFFF4008)
#define __APMC_PCER *((unsigned int *) 	0xFFFF4010)
#define __APMC_PCDR *((unsigned int *) 	0xFFFF4014)
#define __APMC_PCSR *((unsigned int *) 	0xFFFF4018)

/*PIO User Interface*/
#define __PIO_PER 	*((unsigned int *) 	0xFFFEC000)
#define __PIO_PDR 	*((unsigned int *) 	0xFFFEC004)
#define __PIO_PSR 	*((unsigned int *) 	0xFFFEC008)

#endif

//TO DO: Put your declaration here


#endif
