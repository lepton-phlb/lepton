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


/*===========================================
Includes
=============================================*/
#include <intrm16c.h>   // "Intrinsic Function IAR"
#include <string.h>     // memcpy

/*===========================================
Global Declaration
=============================================*/
extern void (* const _RomIntVect [64])(void);

static void (*vector_it_NearRAM [64])(void);


/*===========================================
Implementation
=============================================*/
short int _read_flag (void){
    asm (" STC      FLG,r0");
}
unsigned long _lReadIntAdd(void){
    asm ( "STC INTBH,R2");    //  INTB (interrupt base) to ad
    asm ( "STC INTBL,R0");
}

void _setRegisterIntb (void *p){
    asm ( "LDC   r2,INTBH");    // set register INTB (interrupt base) to ad
    asm ( "LDC   r0,INTBL");
}

void _setInterrupTable( void far * ad){
   //_setInterrupTable
   //set the register INTB with the parameter value short int i;
   // "void far * ad" the address of the vector table TO DO: Put your own code here && add JF_setInterrupTable to DebogCA.h
   //none //TRACEDEBUG(JF__setInterrupTable);
   //the interrupt is disabled before the change and restore after i = _readFlg();          // store Interrupt flag
   //option compil default parameter @-6-4
   //option compil speed 8 parametre @-4-2 disable_interrupt ();       // disable
   //use as library //asm ( "LDC   -4[FB],INTBH");    // set register INTB (interrupt base) to ad (default)

   //asm ( "LDC   -6[FB],INTBL");
   //asm ( "LDC   -2[FB],INTBH");    // set register INTB (interrupt base) to ad (speed 8)
   //asm ( "LDC   -4[FB],INTBL");
   _setRegisterIntb (ad);
   if (i & (1<<6))         // restore interrupt flg
      enable_interrupt ();
}

void _setDefaultIntTableToRam (void){
   memcpy (&vector_it_NearRAM,  _RomIntVect, sizeof(vector_it_NearRAM)); /// INTVECT to RAM 0x0fba00
   _setInterrupTable( (void far *) vector_it_NearRAM);
}

void _restoreDefaultIntTable (void)
{
   _setInterrupTable ( (void far *) _RomIntVect );      /// restore value in minim.xcl for INTVEC  0x0fba00
}


void _changeIntRamVect (char vector, void (* funct)(void)){
   short int i;
   if (vector >= sizeof(vector_it_NearRAM)/sizeof(*vector_it_NearRAM) ) return;
   i = _readFlg();               /// store Interrupt flag
   disable_interrupt ();         /// disable interrupt
   vector_it_NearRAM [vector] = funct ;   /// change vector in Ram Table

   if (i & (1<<6))               /// restore interrupt flag
      enable_interrupt ();
}


/*===========================================
End of Source m16c_irq.c
=============================================*/
