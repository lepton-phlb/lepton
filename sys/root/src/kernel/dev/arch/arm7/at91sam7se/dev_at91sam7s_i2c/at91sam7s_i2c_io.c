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
| Includes
==============================================*/

#include "kernel/types.h"
#include "kernel/interrupt.h"
#include "kernel/kernel.h"
#include "kernel/system.h"
#include "kernel/fcntl.h"

#include "at91sam7s_i2c_io.h"

#include <ioat91sam7se512.h>
#include <intrinsic.h>



/*============================================
| Global Declaration
==============================================*/

//SDA = PA3
//SCL = PA4

#define SCL_0             ((*AT91C_PIOA_CODR) = (1<<4))
#define SCL_1             ((*AT91C_PIOA_SODR) = (1<<4))

#define WSDA(__SDA_VALUE__) (__SDA_VALUE__ ? ((*AT91C_PIOA_SODR)= \
                                                 (1<<3)) : ((*AT91C_PIOA_CODR)=(1<<3)) )
#define RSDA              (((*AT91C_PIOA_PDSR)&(1<<3))>>3)

#define SDA_DIR_OUTPUT    ((*AT91C_PIOA_OER) =  1<<3)
#define SDA_DIR_INPUT     ((*AT91C_PIOA_ODR) =  1<<3)

#define SCL_DIR_OUTPUT    ((*AT91C_PIOA_OER) =  1<<4)


// Tempo de stabilisation des entrées-sorties (m16c62p 0:Fc=10MHz / 2:Fc=24MHz)
// arm7 48MHz tpscl 1 -> 1us -> 1Mbits. tpscl = 10  i2c environ 100 Kbits/s
#define         tpscl           10

#define __in_i2c()   _i2c_lock()        // _i2c_lock()
#define __out_i2c(__i2c_value__) \
   _i2c_unlock(); \
   return __i2c_value__

#define __i2c_io_start() \
   {int i; \
    WSDA(1); \
    SDA_DIR_OUTPUT; \
    SCL_1; \
    WSDA(0); \
   }

#define __i2c_io_stop() \
   {int i; \
    WSDA(0); \
    SDA_DIR_OUTPUT; \
    SCL_1; \
    for(i=0; i<tpscl; i++) ; \
    SDA_DIR_INPUT; \
   }


int _i2c_io_snd(const unsigned char byte);
int _i2c_io_rcv(unsigned char ack);
int _i2c_io_start(void);
int _i2c_io_stop(void);

/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        _i2c_io_init
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _i2c_io_init(void){
   // unsigned char buf[16]={0};

   // Enable peripheral clock for selected PIOA
   *AT91C_PMC_PCER     = 1 << AT91C_ID_PIOA; //GPIO clock enable

   (*AT91C_PIOA_PER)   = 1<<3;   //SDA
   (*AT91C_PIOA_MDDR)  = 1<<3;   //SDA disable open drain
   (*AT91C_PIOA_PER)   = 1<<4;   //SCL
   SCL_DIR_OUTPUT;
}

/*--------------------------------------------
| Name:        _i2c_io_write
| Description: Ecriture sur un peripherique I2C
| Parameters:  dev_addr : type de device I2C
|              *data : pointe sur le buffer ou lire les données à écrire
|              data_len : nombre d'octets à écrire
|              addr_size : Dimensionnement de l'adresse sur 1 ou 2 octets
| Return Type:  0  si ecriture effectuée  /  -1  si ecriture non effectuée
| Comments:  Pour la programmation de l'EEPROM verifier que \WC = 0
----------------------------------------------*/
int _i2c_io_write(unsigned char dev_addr,const unsigned char* data,unsigned int data_len,
                  unsigned char addr_size)
{
   char ack;

   __in_i2c();                   // Protection accés concurrent

   __i2c_io_start();             // Démarrage de l'échange I2C

   ack = _i2c_io_snd((dev_addr << 1) | 0x00);   // Emission du type de device | 0x00 pour que  R/W (b0) = 0

   if (ack != 0)                 // Pb de dialogue ?
   {
      __i2c_io_stop();         // Oui, arret de l'échange I2C
      __out_i2c(-1);           // Sortie sur erreur, supprime protection accès concurrent
   }

   ack = _i2c_io_snd(*data++);   // Emission du 1er octet d'adresse (8 bits ou poids fort)
   if (ack != 0)                 // Pb de dialogue ?
   {
      __i2c_io_stop();         // Oui, arret de l'échange I2C
      __out_i2c(-1);           // Sortie sur erreur, supprime protection accès concurrent
   }

   if (addr_size > 1)            // Si adresse sur 16 bit
   {
      ack = _i2c_io_snd(*data++); // Emission du 2nd octet d'adresse (poids faible)
      if (ack != 0)            // Pb de dialogue ?
      {
         __i2c_io_stop();    // Oui, arret de l'échange I2C
         __out_i2c(-1);      // Sortie sur erreur, supprime protection accès concurrent
      }
   }

   //data write
   for(; data_len > 0; data_len--)
   {
      ack = _i2c_io_snd(*data++); // Emission des datas
      if (ack != 0)            // Pb de dialogue ?
      {
         __i2c_io_stop();    // Oui, arret de l'échange I2C
         __out_i2c(-1);      // Sortie sur erreur, supprime protection accès concurrent
      }
   }

   __i2c_io_stop();              // Arret de l'échange I2C
   __out_i2c(0);                 // Ecriture effectuée, supprime protection accès concurrent
}

/*--------------------------------------------
| Name:        i2c_io_read
| Description: Lecture sur un peripherique I2C
| Parameters:  dev_addr : type de device I2C
|              *data : pointe sur le buffer ou stocker les données lues
|              data_len : nombre d'octets à lire
|              addr_size : Dimensionnement de l'adresse sur 1 ou 2 octets
| Return Type:  nb de carac lus  si lecture effectuée  /  -1  si lecture non effectuée
| Comments:  Pour la programmation de l'EEPROM verifier que \WC = 0
----------------------------------------------*/
int _i2c_io_read(unsigned char dev_addr,unsigned char* data,unsigned int data_len,
                 unsigned char addr_size)
{
   char ack;
   int i;

   __in_i2c();                   // Protection accès concurrent

   __i2c_io_start();             // Démarrage de l'échange I2C

   ack = _i2c_io_snd((dev_addr << 1) | 0x00);    // Emission du type de device | 0x00 pour que  R/W (b0) = 0
   if (ack != 0)                 // Pb de dialogue ?
   {
      __i2c_io_stop();         // Oui, arret de l'échange I2C
      __out_i2c(-1);           // Sortie sur erreur, supprime protection accès concurrent
   }

   ack = _i2c_io_snd(*data++);   // Emission du 1er octet d'adresse (8 bits ou poids fort)
   if (ack != 0)                 // Pb de dialogue ?
   {
      __i2c_io_stop();         // Oui, arret de l'échange I2C
      __out_i2c(-1);           // Sortie sur erreur, supprime protection accès concurrent
   }
   if (addr_size > 1)            // Si adresse sur 16 bit
   {
      ack = _i2c_io_snd(*data++); // Emission du 2nd octet d'adresse (poids faible)
      if (ack != 0)            // Pb de dialogue ?
      {
         __i2c_io_stop();    // Oui, arret de l'échange I2C
         __out_i2c(-1);      // Sortie sur erreur, supprime protection accès concurrent
      }
   }

   __i2c_io_start();             // Démarrage de l'échange I2C
   ack = _i2c_io_snd((dev_addr << 1) | 0x01); // Emission du type de device | 0x01 pour que  R/W (b0) = 1
   if (ack != 0)                 // Pb de dialogue ?
   {
      __i2c_io_stop();         // Oui, arret de l'échange I2C
      __out_i2c(-1);           // Sortie sur erreur, supprime protection accès concurrent
   }

   for(i=data_len; i > 1; i--)
      *data++ = _i2c_io_rcv(0);  // Reception des  data_len-1  octets de data avec ACK = 0

   *data = _i2c_io_rcv(1);       // Reception du dernier octet de data avec ACK = 1 (fin de lecture)

   __i2c_io_stop();              // Arret de l'échange I2C
   __out_i2c(data_len);          // Lecture effectuée, supprime protection accès concurrent
}

/*--------------------------------------------
| Name:        i2c_io_snd
| Description: Ecriture du caractère "octet" sur le peripherique I2C ouvert
| Parameters:  byte : octet à envoyer
| Return Type: bit d'acquittement 0 : OK  /  1 : PB
----------------------------------------------*/
int _i2c_io_snd(const unsigned char byte)
{
   char ack;
   int i,j;
   char temp;

   for(i=0; i<(tpscl); i++) ;           // Attente
   SDA_DIR_OUTPUT;               // SDA en sortie
   for(i=0; i<(tpscl); i++) ;           // Attente
   temp = (byte >> 7)&0x1;       // calcul du 1er bit a emettre
   for (i=6; i >= 0; i--)
   {
      SCL_0;
      WSDA(temp);       //  Emission du bit
      temp = (byte >> i)&0x1;   // Calcul du bit a emettre
      for(j=0; j<tpscl; j++) ;  // Attente
      SCL_1;
      for(j=0; j<tpscl; j++) ;  // Attente
   }
   SCL_0;
   WSDA(temp);                   // Emission du dernier bit pour
   // l'optimisation de la boucle
   for(i=0; i<tpscl; i++) ;     // Attente

   SCL_1;
   for(i=0; i<tpscl; i++) ;     // Attente

   SCL_0;
   SDA_DIR_INPUT;                // SDA en entree

   SCL_1;
   ack = RSDA;                    // lecture acquitement
   SCL_0;

   return (ack);
}

/*--------------------------------------------
| Name:        i2c_io_rcv
| Description: Lecture d'un caractère sur le peripherique I2C ouvert
| Parameters:  byte : octet à envoyer
| Return Type:
|      - Renvoi  du caractère lu
|      - Positionnement de l'acquittement a la valeur "ack"
----------------------------------------------*/
int _i2c_io_rcv(unsigned char ack)
{
   char octet = 0;
   int i,j;

   SCL_0;
   SDA_DIR_INPUT;
   for (i=0; i<=7; i++)
   {
      SCL_1;
      octet = (octet << 1) + RSDA;  // Lecture du bit reçu
      SCL_0;
      for(j=0; j<tpscl; j++) ;     // Attente
   }
   WSDA(ack);                        // Lecture acquitement
   SDA_DIR_OUTPUT;                   // SDA en sortie
   SCL_1;

   return(octet);
}

/*============================================
| End of Source  : at91sam7s_i2c_io.c
==============================================*/
