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

/*===========================================
Compiler Directive
=============================================*/
#ifndef _OS_EMUINTERRUPT_H
#define _OS_EMUINTERRUPT_H


/*===========================================
Includes
=============================================*/


/*===========================================
Declaration
=============================================*/


#define WM_EMUINTERRUPT    WM_APP+100


typedef void (*FN_INTERRUPT)(void);
typedef FN_INTERRUPT PFN_INTERRUPT; 

//
extern PFN_INTERRUPT interrupt0;
extern PFN_INTERRUPT interrupt1;
extern PFN_INTERRUPT interrupt2;
extern PFN_INTERRUPT interrupt3;
extern PFN_INTERRUPT interrupt4;
extern PFN_INTERRUPT interrupt5;
extern PFN_INTERRUPT interrupt6;
extern PFN_INTERRUPT interrupt7;
extern PFN_INTERRUPT interrupt8;
extern PFN_INTERRUPT interrupt9;
extern PFN_INTERRUPT interrupt10;
extern PFN_INTERRUPT interrupt11;
extern PFN_INTERRUPT interrupt12;
extern PFN_INTERRUPT interrupt13;
extern PFN_INTERRUPT interrupt14;
extern PFN_INTERRUPT interrupt15;
extern PFN_INTERRUPT interrupt16;
extern PFN_INTERRUPT interrupt17;
extern PFN_INTERRUPT interrupt18;
extern PFN_INTERRUPT interrupt19;
extern PFN_INTERRUPT interrupt20;
extern PFN_INTERRUPT interrupt21;
extern PFN_INTERRUPT interrupt22;
extern PFN_INTERRUPT interrupt23;
extern PFN_INTERRUPT interrupt24;
extern PFN_INTERRUPT interrupt25;
extern PFN_INTERRUPT interrupt26;
extern PFN_INTERRUPT interrupt27;
extern PFN_INTERRUPT interrupt28;
extern PFN_INTERRUPT interrupt29;
extern PFN_INTERRUPT interrupt30;
extern PFN_INTERRUPT interrupt31;
extern PFN_INTERRUPT interrupt32;
extern PFN_INTERRUPT interrupt33;
extern PFN_INTERRUPT interrupt34;
extern PFN_INTERRUPT interrupt35;
extern PFN_INTERRUPT interrupt36;
extern PFN_INTERRUPT interrupt37;
extern PFN_INTERRUPT interrupt38;
extern PFN_INTERRUPT interrupt39;
extern PFN_INTERRUPT interrupt40;
extern PFN_INTERRUPT interrupt41;
extern PFN_INTERRUPT interrupt42;
extern PFN_INTERRUPT interrupt43;
extern PFN_INTERRUPT interrupt44;
extern PFN_INTERRUPT interrupt45;
extern PFN_INTERRUPT interrupt46;
extern PFN_INTERRUPT interrupt47;
extern PFN_INTERRUPT interrupt48;
extern PFN_INTERRUPT interrupt49;
extern PFN_INTERRUPT interrupt50;
extern PFN_INTERRUPT interrupt51;
extern PFN_INTERRUPT interrupt52;
extern PFN_INTERRUPT interrupt53;
extern PFN_INTERRUPT interrupt54;
extern PFN_INTERRUPT interrupt55;
extern PFN_INTERRUPT interrupt56;
extern PFN_INTERRUPT interrupt57;
extern PFN_INTERRUPT interrupt58;
extern PFN_INTERRUPT interrupt59;
extern PFN_INTERRUPT interrupt60;
extern PFN_INTERRUPT interrupt61;
extern PFN_INTERRUPT interrupt62;
extern PFN_INTERRUPT interrupt63;
extern PFN_INTERRUPT interrupt64;
extern PFN_INTERRUPT interrupt65;
extern PFN_INTERRUPT interrupt66;
extern PFN_INTERRUPT interrupt67;
extern PFN_INTERRUPT interrupt68;
extern PFN_INTERRUPT interrupt69;
extern PFN_INTERRUPT interrupt70;
extern PFN_INTERRUPT interrupt71;
extern PFN_INTERRUPT interrupt72;
extern PFN_INTERRUPT interrupt73;
extern PFN_INTERRUPT interrupt74;
extern PFN_INTERRUPT interrupt75;
extern PFN_INTERRUPT interrupt76;
extern PFN_INTERRUPT interrupt77;
extern PFN_INTERRUPT interrupt78;
extern PFN_INTERRUPT interrupt79;
extern PFN_INTERRUPT interrupt80;
extern PFN_INTERRUPT interrupt81;
extern PFN_INTERRUPT interrupt82;
extern PFN_INTERRUPT interrupt83;
extern PFN_INTERRUPT interrupt84;
extern PFN_INTERRUPT interrupt85;
extern PFN_INTERRUPT interrupt86;
extern PFN_INTERRUPT interrupt87;
extern PFN_INTERRUPT interrupt88;
extern PFN_INTERRUPT interrupt89;
extern PFN_INTERRUPT interrupt90;
extern PFN_INTERRUPT interrupt91;
extern PFN_INTERRUPT interrupt92;
extern PFN_INTERRUPT interrupt93;
extern PFN_INTERRUPT interrupt94;
extern PFN_INTERRUPT interrupt95;
extern PFN_INTERRUPT interrupt96;
extern PFN_INTERRUPT interrupt97;
extern PFN_INTERRUPT interrupt98;
extern PFN_INTERRUPT interrupt99;
extern PFN_INTERRUPT interrupt100;
extern PFN_INTERRUPT interrupt101;
extern PFN_INTERRUPT interrupt102;
extern PFN_INTERRUPT interrupt103;
extern PFN_INTERRUPT interrupt104;
extern PFN_INTERRUPT interrupt105;
extern PFN_INTERRUPT interrupt106;
extern PFN_INTERRUPT interrupt107;
extern PFN_INTERRUPT interrupt108;
extern PFN_INTERRUPT interrupt109;
extern PFN_INTERRUPT interrupt110;
extern PFN_INTERRUPT interrupt111;
extern PFN_INTERRUPT interrupt112;
extern PFN_INTERRUPT interrupt113;
extern PFN_INTERRUPT interrupt114;
extern PFN_INTERRUPT interrupt115;
extern PFN_INTERRUPT interrupt116;
extern PFN_INTERRUPT interrupt117;
extern PFN_INTERRUPT interrupt118;
extern PFN_INTERRUPT interrupt119;
extern PFN_INTERRUPT interrupt120;
extern PFN_INTERRUPT interrupt121;
extern PFN_INTERRUPT interrupt122;
extern PFN_INTERRUPT interrupt123;
extern PFN_INTERRUPT interrupt124;
extern PFN_INTERRUPT interrupt125;
extern PFN_INTERRUPT interrupt126;
extern PFN_INTERRUPT interrupt127;
extern PFN_INTERRUPT interrupt128;

#ifdef __cplusplus
   #define LIBC_API_FUNCT extern "C" 
#else
   #define LIBC_API_FUNCT
#endif
      
LIBC_API_FUNCT void emuSetInterruptThreadId(LONG threadId);
LIBC_API_FUNCT LONG emuGetInterruptThreadId(void);
LIBC_API_FUNCT void emuFireInterrupt(LONG lInterruptNumber);
LIBC_API_FUNCT void initEmuInterrupt(void);

#endif