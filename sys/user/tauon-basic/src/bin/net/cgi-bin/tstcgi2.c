/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          tstedf.c
| Path:        X:\sources\bin\tst
| Authors:     
| Plateform:   
| Created:     
| Revision/Date: $Revision: 1.1 $  $Date: 2009/07/15 14:43:21 $ 
| Description: 
|---------------------------------------------
| Historic:    
|---------------------------------------------
| Authors     | Date     | Comments  
| $Log: tstcgi2.c,v $
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
int tstcgi2_main(int argc,char* argv[]){
   static unsigned long edf_counter=0;
   
   printf("<table border=\"1\">");
   printf("<tr>");
   printf("<td>U1:</td><td>%4.1f V</td><td>I1:</td><td>%4.1f A</td>",(float)0,(float)0);
   printf("</tr>");
   printf("<tr>");
   printf("<td>U2:</td><td>%4.1f V</td><td>I2:</td><td>%4.1f A</td>",(float)0,(float)0);
   printf("</tr>");
   printf("<tr>");
   printf("<td>U3:</td><td>%4.1f V</td><td>I3:</td><td>%4.1f A</td>",(float)0,(float)0);
   printf("</tr>");

   printf("<tr>");
   printf("<td>compteur</td><td>%d</td>",edf_counter++);
   printf("</tr>");

   printf("</table>");

   printf("<table border=\"0\">");
   printf("<tr>");
   printf("<td>température</td><td>%4.1f C</td><td>Humidité</td><td>%4.1f %</td>",(float)0,(float)0);
   printf("<td>pression</td><td>%4.1f hPa</td>",(float)0);
   printf("</tr>");
   

   printf("</table>");

   //Pression    = ihm_zone_file.ptu_pa;
   //Humidite    = ihm_zone_file.ptu_hair;
   //Temperature = ihm_zone_file.ptu_tair;
  
   //printf("T Air = %5d V\r\n",edf_data_zone.ptu_tair);

   //printf("%s","{\"temperature\":\"43\", \"pression\":\"1033\"}");
   //printf("toto");

   //write(1,"\n\n",2);
   //write(1,"\n\r\n",3);

   return 0;
}

/*============================================
| End of Source  : tstedf.c
==============================================*/