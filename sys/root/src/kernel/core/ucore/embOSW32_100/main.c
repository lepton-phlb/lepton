/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          main.c
| Path:        C:\tauon\src\kernel\core\ucore\embOSW32_100
| Authors:     
| Plateform:   
| Created:     
| Revision/Date: $Revision: 1.1 $  $Date: 2009/03/30 15:48:55 $ 
| Description: 
|---------------------------------------------
| Historic:    
|---------------------------------------------
| Authors     | Date     | Comments  
| $Log: main.c,v $
| Revision 1.1  2009/03/30 15:48:55  jjp
| first import of tauon
|
| Revision 1.1  2009/03/30 11:18:53  jjp
| First import of tauon
|
|---------------------------------------------*/




/*===========================================
Includes
=============================================*/
//#include "kernel/core/windows.h"
#include "kernel/core/ucore/embOSW32_100/win32/windows.h"
#include "kernel/core/ucore/embosW32_100/seggerwin32.h"
#include "kernel/core//kernel.h"





/*===========================================
Global Declaration
=============================================*/
#define MAX_ARG_LINE 255
static char arg[MAX_ARG_LINE]={0};



/*===========================================
Implementation
=============================================*/

/*-------------------------------------------
| Name:main
| Description:
| Parameters:
| Return Type:
| Comments:
| See:
---------------------------------------------*/
int main( int argc, char *argv[])
{
   
   int i;
   const int max=64;
   //test
   //ino_mod_t ino_mod;
   //int ino_size =  sizeof(ino_mod_t);
  
   //
   //ino_mod.rwxrwxrwx='\077';

   /*int r=0;
   int sz;

   for(sz=1;sz<=4096;sz++){
      r=(sz/4+((sz%4)?1:0))*4;

      if(r%4)
         return -1; 
   }*/
   



   //argument line
   for(i=1;i<argc;i++){
      strcat(arg,argv[i]);
      strcat(arg," ");
   }

   //segger specific
   OS_InitKern();

   //lepton
   //vfstest_mnt();
   _start_kernel(arg);

   //segger specific
   OS_Start();


   return 0;
}


/*============================================
| End of Source  : main.c
==============================================*/