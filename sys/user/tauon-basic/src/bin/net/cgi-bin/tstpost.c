/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          tstpost.c
| Path:        C:\tauon\sys\user\tauon-basic\src\bin\net\cgi-bin
| Authors:     
| Plateform:   
| Created:     
| Revision/Date: $Revision: 1.1 $  $Date: 2009/07/15 14:43:21 $ 
| Description: 
|---------------------------------------------
| Historic:    
|---------------------------------------------
| Authors     | Date     | Comments  
| $Log: tstpost.c,v $
| Revision 1.1  2009/07/15 14:43:21  phlb
| test cgi and html features.
|
|---------------------------------------------*/


/*============================================
| Includes    
==============================================*/
#include <stdlib.h>
#include "kernel/core/errno.h"
#include "kernel/core/libstd.h"

#include "lib/libc/stdio/stdio.h"

/*============================================
| Global Declaration 
==============================================*/


/*============================================
| Implementation 
==============================================*/
/*--------------------------------------------
| Name:        tstpost_main
| Description: 
| Parameters:  none
| Return Type: none
| Comments:    
| See:         
----------------------------------------------*/
int tstpost_main(int argc,char* argv[]){


   int c;
 
   printf("<H1>Résultat du traitement du formulaire</H1>\n");
   printf("<H2>Chaine de données reçue par le CGI</H2>");

   
   printf("STDIN (Methode POST) <B>");

   while((c=getchar()) != EOF) {
      printf("%c" ,c);
   }
   printf("</B>");

   printf("<H2>Liste des informations décodées</H2>");
   printf("Non traitée dans cet exemple...");

   return 0;
}


/*============================================
| End of Source  : tstpost.c
==============================================*/