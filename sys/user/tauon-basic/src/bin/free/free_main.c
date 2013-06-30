
//Technical Note 28545
//IAR DLIB Library heap usage statistics
//EW targets: 	ARM
//EW component: 	General issues
//Keywords: 	debug, library, memory
//Last update: 	November 2, 2011
//
//Background
//The heap is described in "IAR C/C++ Development Guide for ARM", chapter "Dynamic memory on the heap". This technical note describes how to get heap usage statistics in an application. (I.e. the amount of memory used by malloc and similar functions).
//
//Example
//Download and open the example project from the link to the right on this page.
//
//Description
//With Embedded Workbench for ARM 6.10 and later, there is a possibility to include the file "arm\src\lib\dlmalloc.c" in your project, and set the define NO_MALLINFO to 0. (Note that you have to copy the dlmalloc.c file to the project directory and modify it).
//
//By doing the above you can get heap usage statistics using the function call:
//
//__iar_dlmallinfo()
//
//
//If you want a simple printout on stderr, the define NO_MALLOC_STATS needs to be set to 0. Then call the function:
//
//__iar_dlmalloc_stats()
//
//The output from __iar_dlmalloc_stats() looks like (for example):
//
//max system bytes =       2048
//system bytes     =       2048
//in use bytes     =         16
//
//
//Note: The example project can also be used in a C++ environment (for the "new" operator), but note that the "dlmalloc.c" file needs to be compiled as C language.
//
//
//Workaround for v6.30.1
//
//
//The example project issues a linker error with v6.30.1
//
//Error[Li009]: runtime model conflict
//
//The workaround is to set "Project > Options > Linker > Extra Options":
//
//--redirect malloc=__iar_dlmalloc
//--redirect free=__iar_dlfree
//
//source http://supp.iar.com/Support/?note=28545

//#include "stdio.h"
#include <stdlib.h>
#include <string.h>
#include "lib/libc/stdio/stdio.h"

#define MALLINFO_FIELD_TYPE size_t
//extern void __iar_dlmalloc_stats();
extern struct mallinfo __iar_dlmallinfo();


struct mallinfo {
  MALLINFO_FIELD_TYPE arena;    /* non-mmapped space allocated from system */
  MALLINFO_FIELD_TYPE ordblks;  /* number of free chunks */
  MALLINFO_FIELD_TYPE smblks;   /* always 0 */
  MALLINFO_FIELD_TYPE hblks;    /* always 0 */
  MALLINFO_FIELD_TYPE hblkhd;   /* space in mmapped regions */
  MALLINFO_FIELD_TYPE usmblks;  /* maximum total allocated space */
  MALLINFO_FIELD_TYPE fsmblks;  /* always 0 */
  MALLINFO_FIELD_TYPE uordblks; /* total allocated space */
  MALLINFO_FIELD_TYPE fordblks; /* total free space */
  MALLINFO_FIELD_TYPE keepcost; /* releasable (via malloc_trim) space */
};


int free_main(int argc,char* argv[])
{
#if 1
    //__iar_dlmalloc_stats();
    struct mallinfo info;
    info = __iar_dlmallinfo();
    //
    // Constant and meaning-less values (with IAR) deactivated.
    //
//    printf("non-mmapped space allocated from system\t= %i\n",   (int)info.arena);
//    printf("number of free chunks\t\t\t= %i\n",                 (int)info.ordblks);
//    printf("space in mmapped regions\t\t= %i\n",                (int)info.hblkhd);
    printf("maximum total allocated space\t\t= %i\n",           (int)info.usmblks);
    printf("total allocated space\t\t\t= %i\n",                 (int)info.uordblks);
    
    //equivalent to _gm_.topsize value
    printf("total free space\t\t\t= %i\n",                      (int)info.fordblks); 
    
    ///It is the amount of data the data pool can be reduced of (free space minus an overhead).
//    printf("releasable (via malloc_trim) space\t= %i\n",        (int)info.keepcost); 
#endif           
    return 0;
}

