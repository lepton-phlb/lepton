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


/*============================================
| Includes
==============================================*/
#include "manage_order.h"

/*============================================
| Implementation
==============================================*/

int send_order_w(unsigned char s, u_int32 reg, u_int32 val) //OK
{
	ecos_virtual_order order;
	ecos_virtual_order  * ptr_order;

	int bufsz = sizeof(ecos_virtual_order);	
	int cb, w, r;	

	memset((void *)&order, 0, sizeof(ecos_virtual_order));
	//remplit ordre	
	order.sens = s;
	order.reg = reg;
	order.value = val;
	ptr_order = &order;
	//on s'assure d'envoyer tous les caracteres
	cb = 0;
	w = 0;
	while(bufsz - cb){
		w=cyg_hal_sys_write(1,(void *)(ptr_order+cb),bufsz-cb);
		cb += w; 
		if(w <= 0) { 
			cb = 0;
			w = 0;
		}           
	}
	//on s'assure d'avoir reçu tous les caractères
	cb = 0;
	r = 0;
	while(bufsz - cb){
		r=cyg_hal_sys_read(0,(void *)(ptr_order+cb),bufsz-cb);
		cb += r;
		if(r <= 0) { 
			cb = 0;
			r = 0;
		}         
	}	
}

int send_order_r(unsigned char s, u_int32 reg, u_int32 * val)
{
	ecos_virtual_order order;
	ecos_virtual_order *ptr_order;

	int bufsz = sizeof(ecos_virtual_order);	
	int cb, w, r;

	memset((void *)&order, 0, sizeof(ecos_virtual_order));
	//remplit ordre
	order.sens = s;
	order.reg = 0L;
	order.reg = reg;
	order.value = *val;
	ptr_order = &order;
	//on s'assure d'envoyer tous les caracteres
	cb = 0;
	w = 0;
	while(bufsz - cb){
		w=cyg_hal_sys_write(1,(void *)(ptr_order+cb),bufsz-cb);
		cb += w;  
		if(w <= 0) { 
			cb = 0;
			w = 0;
		}              
	}
	//on s'assure d'avoir reçu tous les caractères
	cb = 0;
	r = 0;
	while(bufsz - cb){
		r=cyg_hal_sys_read(0,(void *)(ptr_order+cb),bufsz-cb);
		cb += r;
		if(r <= 0) { 
			cb = 0;
			r = 0;
		}         
	}		
	*val = ptr_order->value;
}

/*============================================
| End of Source  : manage_order.c
|---------------------------------------------
| Historic:
|---------------------------------------------
| Authors     | Date     | Comments
| $Log: not supported by cvs2svn $
| Revision 1.1  2009/03/30 11:16:09  jjp
| First import of tauon
|
|---------------------------------------------
==============================================*/
