/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/

/**
 * \addtogroup tst
 * @{
 *
 */


/**
 * \addtogroup tstpwr test et configuration du coulomb-mètre
 * @{
 *
 */



/*============================================
| Includes
==============================================*/

#include <stdlib.h>

#include "kernel/libstd.h"
#include "kernel/devio.h"
#include "kernel/fcntl.h"
#include "kernel/wait.h"
#include "kernel/time.h"
#include "kernel/select.h"

#include "stdio/stdio.h"


/*============================================
| Global Declaration
==============================================*/
#define __pwr_device "/dev/pwr0"

//#define DEFLT_PERIOD 3000000L
//#define DEFLT_PERIOD 5000000L
#define DEFLT_PERIOD 1000000L    // 1 Seconde

#define ZERO_K_C 2731 //0°K= -273,15 °C

//Alarm Bits
#define OVER_CHARGED_ALARM          0x8000
#define TERMINATE_CHARGE_ALARM      0x4000
//reserved                          0x2000
#define OVER_TEMP_ALARM             0x1000
#define TERMINATE_DISCHARGE_ALARM   0x0800
//reserved                          0x0400
#define REMAINING_CAPACITY_ALARM    0x0200
#define REMAINING_TIME_ALARM        0x0100

//Bits Status
#define INITIALIZED                 0x0080
#define DISCHARGING                 0x0040
#define FULLY_CHARGED               0x0020
#define FULLY_DISCHARGED            0x0010

//Error Codes
#define unknown_error               0x0007
#define badsize_error               0x0006
#define overflow_underflow_error    0x0005
#define access_denied_error         0x0004
#define unsupported_command_error   0x0003
#define reserved_command_error      0x0002
#define busy_error                  0x0001
#define OK                          0x0000

#define  __BUF_MAX 20
#define  OPT_MSK_X 0x01 //-x export

#define RSR                               0.1         // sense resistor value
#define GVFC                              22.2        // BQ2018 VFC gain in Hertz/volt

#define BAT_UNKNOWN                       0x01
#define BAT_IN_CHARGE                     0x02
#define BAT_CHARGED                       0x04
#define BAT_IN_USE                        0x08

// BQ2018 register offset
#define BQ2018_DISCHARGE_COUNT_REG_H      0x7F
#define BQ2018_DISCHARGE_COUNT_REG_L      0x7E
#define BQ2018_CHARGE_COUNT_REG_H         0x7D
#define BQ2018_CHARGE_COUNT_REG_L         0x7C
#define BQ2018_SELF_DISCHARGE_COUNT_REG_H 0x7B
#define BQ2018_SELF_DISCHARGE_COUNT_REG_L 0x7A
#define BQ2018_DISCHARGE_TIME_COUNT_REG_H 0x79
#define BQ2018_DISCHARGE_TIME_COUNT_REG_L 0x78
#define BQ2018_CHARGE_TIME_COUNT_REG_H    0x77
#define BQ2018_CHARGE_TIME_COUNT_REG_L    0x76
#define BQ2018_MODE_WAKEUP_ENABLE_REG     0x75
#define BQ2018_TEMP_CLEAR_REG             0x74
#define BQ2018_OFFSET_REG                 0x73
//

// ram registers in BQ2018
#define PWR_STATUS                        0x72
#define BQ2018_MAGIC_NUMDER1_ADR          0x71
#define BQ2018_MAGIC_NUMDER2_ADR          0x70
#define PRESENCE1                         0x6F
#define PRESENCE2                         0x6E
#define STATUS_BAT                        0x6D
#define RM_MEMOH                          0x6C
#define RM_MEMOL                          0x6B

#define INIT 1


typedef struct {
   uint8_t pwr_status;
   uint8_t status_bat;
   uint8_t mn1;
   uint8_t mn2;
   uint8_t pres1;
   uint8_t pres2;
   uint16_t delta_t;
   uint16_t iavg;
   uint16_t iacc;
   uint16_t rm1;
   uint16_t tte_at_rate;
   uint16_t capacite_rest;
   uint8_t autonomie;
   uint8_t machine_state;
   uint8_t memo_state;
   uint16_t memo_iavg;
   uint16_t memo_iacc;
} a0350_power_t;

a0350_power_t *ptr_prw;
/*============================================
| Implementation
==============================================*/


/*--------------------------------------------
| Name:        read_magic_number
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int read_magic_number(void){
   int cb,fd;
   char mn=0x00;
   char buf[20];
   uint16_t command;

   //open power device
   if((fd=open(__pwr_device,O_RDWR,0))<0) {
      printf("error: cannot open %s\r\n",__pwr_device);
      return -1;
   }

   //read registers
   lseek(fd,0x03,SEEK_SET);
   if( (cb=read(fd,&buf,sizeof(buf))) == sizeof(buf) ) {
      ptr_prw->mn1 = buf[12];
      ptr_prw->mn2 = buf[13];

      printf("MN1:\t\t0x%02x  ",ptr_prw->mn1);
      printf("MN2: 0x%02x\r\n",ptr_prw->mn2);
      printf("ok\r\n");
   }

   close(fd);

   return 0;
}

/*--------------------------------------------
| Name:        read_power_register
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int read_power_register(void){
   int cb,fd;
   char buf[20];
   char tmpclr=0,ofr=0,mode=0;

   int error=0;

   //open power device
   if((fd=open(__pwr_device,O_RDWR,0))<0) {
      printf("error: cannot open %s\r\n",__pwr_device);
      return -1;
   }

   //read registers
   lseek(fd,0x03,SEEK_SET);
   if( (cb=read(fd,&buf,sizeof(buf))) == sizeof(buf) ) {
      ptr_prw->delta_t = (buf[1]<<8)+buf[0];
      ptr_prw->iavg = (buf[3]<<8)+buf[2];
      ptr_prw->iacc = (buf[5]<<8)+buf[4];
      ptr_prw->rm1 = (buf[7]<<8)+buf[6];
      ptr_prw->tte_at_rate = (buf[9]<<8)+buf[8];
      ptr_prw->status_bat = buf[10];
      ptr_prw->pwr_status = buf[11];     // read PWR_STATUS
      ptr_prw->mn1 = buf[12];
      ptr_prw->mn2 = buf[13];
      ptr_prw->pres1 = buf[14];
      ptr_prw->pres2 = buf[15];
      ptr_prw->capacite_rest = (buf[17]<<8)+buf[16];
      ptr_prw->autonomie = buf[18];

/*      printf("PRES1:\t\t0x%02x  ",ptr_prw->pres1);
      printf("PRES2: 0x%02x\r\n",ptr_prw->pres2);
      printf("MN1:\t\t0x%02x  ",ptr_prw->mn1);
      printf("MN2: 0x%02x\r\n",ptr_prw->mn2);
      printf("PWR_STATUS:\t0x%02x\r\n",ptr_prw->pwr_status);
      printf("DELTA_T:\t%dSec\r\n",ptr_prw->delta_t);
      printf("IAVG:\t\t%dmA  ",ptr_prw->iavg);
      printf("IACC:\t%dmA/S \r\n",ptr_prw->iacc);
      printf("RM:\t\t%dmA/h  ",ptr_prw->rm1);
      printf("TTE_at_RATE: %dSec  \r\n",ptr_prw->tte_at_rate);
      printf("REST_CAPACITY:\t%dmA/h\r\n",ptr_prw->capacite_rest);
      printf("STATUS_BAT:\t0x%02x ",ptr_prw->status_bat);
      printf("AUTONOMIE:\t%d/100 ",ptr_prw->autonomie);
*/
      printf("P1:0x%02x ",ptr_prw->pres1);
      printf("P2:0x%02x ",ptr_prw->pres2);
      printf("M1:0x%02x ",ptr_prw->mn1);
      printf("M2:0x%02x ",ptr_prw->mn2);
      printf("PW:0x%02x ",ptr_prw->pwr_status);
      printf("T:%dSec ",ptr_prw->delta_t);
      printf("IAVG:%dmA ",ptr_prw->iavg);
      printf("IACC:%dmA/S ",ptr_prw->iacc);
      printf("RM:%dmA/h ",ptr_prw->rm1);
      printf("T:%dSec ",ptr_prw->tte_at_rate);
      printf("CA:%dmA/h ",ptr_prw->capacite_rest);
      printf("ST:0x%02x ",ptr_prw->status_bat);
      printf("A:%d/100 ",ptr_prw->autonomie);

   } else if(cb<0) {
      error++;
   }

   close(fd);

   return error;
}

static int get_presence_charge(void)
{
   int fd,fd_r8;
   char buf[20];
   char buf_r8[60];
   float vbatt;
   int status=0;


   // open R8C dev
   if ((fd_r8 = open("/dev/r8c0",O_RDONLY,0)) < 0) {
      return -1;
   }
   read(fd_r8,&buf_r8,sizeof(buf_r8));
   vbatt = (buf_r8[11]+(buf_r8[12]<<8)) / (2.0f/0.015598763f);
   close(fd_r8);
   //

   status = read_power_register();
   if(status==-1) return status;

/*   if (!(ptr_prw->pwr_status & 0x01)) {   // Charge en cour
      dev_a0350_arm7_board_led(1);  // allumer led
      status = 1;
   } else {
      dev_a0350_arm7_board_led(0);  // eteindre led
      if (vbatt >= 12.0f)
         status = 2;
      else
         status = 0;                  // pas de charge
   }*/

   if ((ptr_prw->pwr_status && 0x01)) {   // pas en charge
      dev_a0350_arm7_board_led(0);        // eteindre led
      if (vbatt >= 12.0f)
         status = 2;                      // pas de charge mais presence chargeur
      else
         status = 0;                      // pas de charge
   } else {
      dev_a0350_arm7_board_led(1);        // allumer led
      status = 1;
   }

   return status;
}

/*--------------------------------------------
| Name:        clr_bq2018_register
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
static int clr_bq2018_register(void){

   int cb,fd;
   int tmpclr=0x00;
   uint16_t command;

   //open power device
   if((fd=open(__pwr_device,O_RDWR,0))<0) {
      printf("error: cannot open %s\r\n",__pwr_device);
      return -1;
   }

   printf("send reset...");
   command=0x1F;
   lseek(fd,0x74,SEEK_SET);
   if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
      return -1;
   printf(" ok\r\n");

   printf("Write MN1 to 0...");
   command=0x00;
   lseek(fd,BQ2018_MAGIC_NUMDER1_ADR,SEEK_SET);
   if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
      return -1;
   printf(" ok\r\n");

   printf("Write MN2 to 0...");
   command=0x00;
   lseek(fd,BQ2018_MAGIC_NUMDER2_ADR,SEEK_SET);
   if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
      return -1;
   printf(" ok\r\n");

   printf("BAT_UNKNOWN --> BAT_STATUS...");
   command=0x01;
   lseek(fd,0x6D,SEEK_SET);
   if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
      return -1;
   printf(" ok\r\n");

   close(fd);

   return 0;
}

/*--------------------------------------------
| Name:        tstpwr_main
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/

/*! \fn int tstpwr_main(int argc,char* argv[])
   \brief test du bq2018

   \note liste des options supportées:\n
      tstpwr -i | -r | -m  \n

      -m lit les magic numbers.\n
      \n

      -r lit les registre du driver du bq2018.\n
      \n

      -i re-initialise le bq2018 ( clr registers ).\n
      \n

*/
int tstpwr_main(int argc,char* argv[]){
   int fd;
   uint16_t command=0,reg;
   uint16_t remaining_time_alrm=0;
   uint16_t status=0;
   uint16_t volt=0;
   uint16_t cycle_count=0;
   int16_t current=0;
   int16_t remaining_capacity=0;
   int16_t runtime_to_empty=0;
   int16_t battery_mode=0;
   int error=0;
   unsigned char buf[__BUF_MAX];
   unsigned int opt=0;
   uint8_t cpt_charge=0;

   int32_t period=DEFLT_PERIOD; //en secondes
   int n=0;
   int cb;
   int index;
   int presence;

   int verbose=1;
   //
   int oflag;
   fd_set readfs;

   ptr_prw->machine_state=0;

   //get options
   for(index=1; index<argc; index++) {
      if (argv[index][0] == '-') {
         switch (argv[index][1]) {
         //i init
         case 'i':
            if(clr_bq2018_register()<0)
               printf(" error: cannot reset bq2018\r\n");
            break;

         //read mem
         case 'r':
            if(read_power_register()<0) {
               printf(" error: cannot read register\r\n");
            }
            printf("\r\nRegister read OK.\r\n");
            break;
         //magic number
         case 'm':
            if(read_magic_number()<0)
               printf(" error: cannot read magic number\r\n");
            break;

         default:
            printf("argument inconnu \r\n");
            break;

         }  //end switch
      }  //end of if(argv...
   }  //end of for


   oflag = fcntl(STDIN_FILENO,F_GETFL);
   fcntl(STDIN_FILENO,F_SETFL,oflag|O_NONBLOCK);

   //async
   FD_ZERO(&readfs);

   //
   for(;; ) {
      error = 0;
      FD_SET(STDIN_FILENO,&readfs); //stdin

      switch( select(STDIN_FILENO+1,&readfs,0,0,0) ) {

      case 0:
         //timeout
         presence = get_presence_charge();
         if(presence<0) {
            // panne hardware BQ2018
         } else {
            switch(ptr_prw->status_bat) {
            case BAT_UNKNOWN:     // battery unknown
               ptr_prw->machine_state = BAT_UNKNOWN;
               if((ptr_prw->status_bat&BAT_UNKNOWN)== BAT_UNKNOWN) {
                  printf(" UNKNOWN\r\n");
               }
               if(presence==1) {    // wait for external power supply
                  ptr_prw->memo_state = ptr_prw->machine_state;
                  if ((fd = open("/dev/pwr0", O_RDWR, 0)) < 0)
                     return -1;
                  command=0x1F;
                  reg = BQ2018_TEMP_CLEAR_REG;
                  lseek(fd,reg,SEEK_SET);
                  if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                     return -1;
                  command=BAT_IN_CHARGE;
                  reg = STATUS_BAT;
                  lseek(fd,reg,SEEK_SET);
                  if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                     return -1;
                  close(fd);
               }
               break;
            case BAT_IN_CHARGE:     // battery in charge
               ptr_prw->machine_state = BAT_IN_CHARGE;
               printf(" CHARGE.\r\n");
               if(presence==2) {    // wait end of charge
                  ptr_prw->memo_state = ptr_prw->machine_state;
                  if ((fd = open("/dev/pwr0", O_RDWR, 0)) < 0)
                     return -1;
                  command=BAT_CHARGED;
                  reg = STATUS_BAT;
                  lseek(fd,reg,SEEK_SET);
                  if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                     return -1;
                  close(fd);
               } else if((presence==0)&&(ptr_prw->memo_state==BAT_UNKNOWN)) {
                  ptr_prw->memo_state = ptr_prw->machine_state;
                  if ((fd = open("/dev/pwr0", O_RDWR, 0)) < 0)
                     return -1;
                  command=BAT_UNKNOWN;
                  reg = STATUS_BAT;
                  lseek(fd,reg,SEEK_SET);
                  if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                     return -1;
                  close(fd);
               }  else if((presence==0)&&(ptr_prw->memo_state==BAT_IN_USE)) {
                  ptr_prw->memo_state = BAT_IN_USE;
                  if ((fd = open("/dev/pwr0", O_RDWR, 0)) < 0)
                     return -1;
                  command=BAT_IN_USE;
                  reg = STATUS_BAT;
                  lseek(fd,reg,SEEK_SET);
                  if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                     return -1;
                  close(fd);
               }
               break;
            case BAT_CHARGED:
               ptr_prw->machine_state = BAT_CHARGED;
               printf(" CHARGED\r\n");
               if((presence==0) && (ptr_prw->memo_state==BAT_IN_CHARGE)) {
                  ptr_prw->memo_state = ptr_prw->machine_state;
                  if ((fd = open("/dev/pwr0", O_RDWR, 0)) < 0)
                     return -1;
                  command=BAT_IN_USE;
                  reg = STATUS_BAT;
                  lseek(fd,reg,SEEK_SET);
                  if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                     return -1;
                  // write charge max 4000 ( 0x0FA0 )
                  command=0xA0;
                  reg = RM_MEMOL;
                  lseek(fd,reg,SEEK_SET);
                  if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                     return -1;
                  command=0x0F;
                  reg = RM_MEMOH;
                  lseek(fd,reg,SEEK_SET);
                  if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                     return -1;
                  close(fd);
                  ptr_prw->capacite_rest = 0x0FA0;      // 4000 mA
                  //
               }  else if((presence==0)&&(ptr_prw->memo_state==BAT_IN_USE)) {
                  ptr_prw->memo_state = BAT_IN_USE;
                  if ((fd = open("/dev/pwr0", O_RDWR, 0)) < 0)
                     return -1;
                  command=BAT_IN_USE;
                  reg = STATUS_BAT;
                  lseek(fd,reg,SEEK_SET);
                  if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                     return -1;
                  close(fd);
               }
               break;
            case BAT_IN_USE:           // battery in use
               ptr_prw->machine_state = BAT_IN_USE;
               printf(" USE.\r\n");
               if(presence==0) {
                  if(ptr_prw->autonomie<1) {
                     printf("Autonomie faible --> charger la batterie!!!!!\r\n");
                  }
               }
               if(presence==1) {
                  if ((fd = open("/dev/pwr0", O_RDWR, 0)) < 0)
                     return -1;
                  command=0x1F;
                  reg = BQ2018_TEMP_CLEAR_REG;
                  lseek(fd,reg,SEEK_SET);
                  if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                     return -1;
                  command=BAT_IN_CHARGE;
                  reg = STATUS_BAT;
                  lseek(fd,reg,SEEK_SET);
                  if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                     return -1;
                  close(fd);
               }
               break;
            default:
               break;
            }

            //
            /*            if(verbose==1){
                           printf("Machine_state= %d\r\n",ptr_prw->machine_state);
                           printf("\r\n");
                           printf("n=%d\r\n",n);
                           if(error)
                              printf("report error =%d\r\n",error);
                        }
            */
            if(n>=0)
               n--;
            //
            if(n==0) {
               //restore old oflag
               fcntl(STDIN_FILENO,F_SETFL,oflag);
               exit(0);
            }

            //
            usleep(period);   //10 s
         }
         break;


      default:
         //
         if(FD_ISSET(STDIN_FILENO,&readfs)) {   //stdin
            char c;
            if(read(STDIN_FILENO,&c,1)<=0)
               break;

            if(c=='\x18') {   //ctrl-x:exit
               // Memorisation capacité restante dans le BQ2018
               if ((fd = open("/dev/pwr0", O_RDWR, 0)) < 0)
                  return -1;
               command=(uint8_t)(ptr_prw->capacite_rest&0x00FF);
               reg = RM_MEMOL;
               lseek(fd,reg,SEEK_SET);
               if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                  return -1;
               command=(uint8_t)((ptr_prw->capacite_rest&0xFF00)>>8);
               reg = RM_MEMOH;
               lseek(fd,reg,SEEK_SET);
               if( (cb=write(fd,&command,sizeof(command))) < sizeof(command) )
                  return -1;

               lseek(fd,0x03,SEEK_SET);
               if( (cb=read(fd,&buf,sizeof(buf))) == sizeof(buf) ) {
                  ptr_prw->capacite_rest = (buf[17]<<8)+buf[16];
                  printf("REST_CAPACITY_memo:\t%dmA/h\r\n",ptr_prw->capacite_rest);
               } else if(cb<0) {
                  error++;
               }
               close(fd);
               //
               //restore old oflag
               fcntl(STDIN_FILENO,F_SETFL,oflag);
               return 0;
            }
         }
         break;
      }
   }
//   close(fd);
}

/** @} */
/** @} */

/*============================================
| End of Source  : tstpwr.c
==============================================*/
