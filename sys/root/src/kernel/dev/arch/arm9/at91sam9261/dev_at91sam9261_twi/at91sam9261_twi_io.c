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

#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernel.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"

#include "at91sam9261_twi_io.h"

#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
   #include <ioat91sam9261.h>
   #include <intrinsic.h>
#else
   #include "cyg/hal/at91sam9261.h"
   #include <string.h>
   #include <stdlib.h>
#endif


/*============================================
| Global Declaration
==============================================*/
kernel_pthread_mutex_t _i2c_core_mutex;

// PIO CONTROLLER A / PERIPHERAL A
//SDA (alias TWD) = PA7
//SCL (alias TWCK)= PA8

#define TWI_SCL_0             ((*AT91C_PIOA_CODR) = (1<<8))
#define TWI_SCL_1             ((*AT91C_PIOA_SODR) = (1<<8))

#define TWI_WSDA(__SDA_VALUE__) (__SDA_VALUE__ ? ((*AT91C_PIOA_SODR)= \
                                                     (1<<7)) : ((*AT91C_PIOA_CODR)=(1<<7)) )
#define TWI_RSDA              (((*AT91C_PIOA_PDSR)&(1<<7))>>7)

#define TWI_SDA_DIR_OUTPUT    ((*AT91C_PIOA_OER) =  1<<7)
#define TWI_SDA_DIR_INPUT     ((*AT91C_PIOA_ODR) =  1<<7)

#define TWI_SCL_DIR_OUTPUT    ((*AT91C_PIOA_OER) =  1<<8)


// Tempo de stabilisation des entrées-sorties (m16c62p 0:Fc=10MHz / 2:Fc=24MHz)
// Beware : arm7 48MHz TWI_TPSCL 1 -> 1us -> 1Mbits. TWI_TPSCL = 10  i2c environ 100 Kbits/s
// Beware : arm9 200MHz TWI_TPSCL 1 -> 1us -> 1Mbits. TWI_TPSCL = 10  i2c environ 100 Kbits/s
// ADU
// #define      TWI_TPSCL		10       //for arm7
// 100 MHz for arm9
#define         TWI_TPSCL               18      // for arm9

#define __in_twi()   _i2c_lock()

#define __out_twi(__twi_value__) \
   _i2c_unlock(); \
   return __twi_value__

#define __twi_io_start() \
   {int i; \
    TWI_WSDA(1); \
    TWI_SDA_DIR_OUTPUT; \
    TWI_SCL_1; \
    for(i=0; i<TWI_TPSCL; i++) ; \
    TWI_WSDA(0); \
   }

#define __twi_io_stop() \
   {int i; \
    TWI_WSDA(0); \
    TWI_SDA_DIR_OUTPUT; \
    TWI_SCL_1; \
    for(i=0; i<TWI_TPSCL; i++) ; \
    TWI_SDA_DIR_INPUT; \
   }

// Twi Bus freq 100 khz
#define  __bus_temporization() \
   {int t; \
    for(t=0; t<(TWI_TPSCL); t++) ; \
   }

// SDL/SDA toggle signals (20% of TWI_TPSCL)
#define  __sda_scl_temporization() \
   {int t; \
    for(t=0; t<(TWI_TPSCL/5); t++) ; \
   }

int _twi_io_snd(const unsigned char byte);
int _twi_io_rcv(unsigned char ack);
int _twi_io_start(void);
int _twi_io_stop(void);

/*============================================
| Implementation
==============================================*/

/*--------------------------------------------
| Name:        _twi_io_init
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _twi_io_init(void)
{
   // Enable peripheral clock for selected PIOA
   *AT91C_PMC_PCER     = 1 << AT91C_ID_PIOA; //GPIO clock enable

   (*AT91C_PIOA_PER)   = 1<<7;   //SDA
   (*AT91C_PIOA_MDDR)  = 1<<7;   //SDA disable open drain
   (*AT91C_PIOA_PER)   = 1<<8;   //SCL
   TWI_SCL_DIR_OUTPUT;
}

/*--------------------------------------------
| Name:        _twi_io_write
| Description: Ecriture sur un peripherique twi
| Parameters:  dev_addr : type de device I2C
|              *data : pointe sur le buffer ou lire les données à écrire
|              data_len : nombre d'octets à écrire
|              addr_size : Dimensionnement de l'adresse sur 1 ou 2 octets
| Return Type:  0  si ecriture effectuée  /  -1  si ecriture non effectuée
| Comments:  Pour la programmation de l'EEPROM verifier que \WC = 0
----------------------------------------------*/
int _twi_io_write(unsigned char dev_addr,const unsigned char* data,unsigned int data_len,
                  unsigned char addr_size)
{
   char ack;

   __in_twi();                   // Protection accés concurrent

   __twi_io_start();             // Démarrage de l'échange I2C

   ack = _twi_io_snd((dev_addr << 1) | 0x00);   // Emission du type de device | 0x00 pour que  R/W (b0) = 0

   if (ack != 0)                 // Pb de dialogue ?
   {
      __twi_io_stop();         // Oui, arret de l'échange I2C
      __out_twi(-1);           // Sortie sur erreur, supprime protection accès concurrent
   }

   ack = _twi_io_snd(*data++);   // Emission du 1er octet d'adresse (8 bits ou poids fort)
   if (ack != 0)                 // Pb de dialogue ?
   {
      __twi_io_stop();         // Oui, arret de l'échange I2C
      __out_twi(-1);           // Sortie sur erreur, supprime protection accès concurrent
   }

   if (addr_size > 1)            // Si adresse sur 16 bit
   {
      ack = _twi_io_snd(*data++); // Emission du 2nd octet d'adresse (poids faible)
      if (ack != 0)            // Pb de dialogue ?
      {
         __twi_io_stop();    // Oui, arret de l'échange I2C
         __out_twi(-1);      // Sortie sur erreur, supprime protection accès concurrent
      }
   }

   //data write
   for(; data_len > 0; data_len--)
   {
      ack = _twi_io_snd(*data++); // Emission des datas
      if (ack != 0)            // Pb de dialogue ?
      {
         __twi_io_stop();    // Oui, arret de l'échange I2C
         __out_twi(-1);      // Sortie sur erreur, supprime protection accès concurrent
      }
   }

   __twi_io_stop();              // Arret de l'échange I2C
   __out_twi(0);                 // Ecriture effectuée, supprime protection accès concurrent
}

/*--------------------------------------------
| Name:        twi_io_read
| Description: Lecture sur un peripherique I2C
| Parameters:  dev_addr : type de device I2C
|              *data : pointe sur le buffer ou stocker les données lues
|              data_len : nombre d'octets à lire
|              addr_size : Dimensionnement de l'adresse sur 1 ou 2 octets
| Return Type:  nb de carac lus  si lecture effectuée  /  -1  si lecture non effectuée
| Comments:  Pour la programmation de l'EEPROM verifier que \WC = 0
----------------------------------------------*/
int _twi_io_read(unsigned char dev_addr,unsigned char* data,unsigned int data_len,
                 unsigned char addr_size)
{
   char ack;
   int i;

   __in_twi();                   // Protection accès concurrent

   __twi_io_start();             // Démarrage de l'échange I2C

   ack = _twi_io_snd((dev_addr << 1) | 0x00);    // Emission du type de device | 0x00 pour que  R/W (b0) = 0
   if (ack != 0)                 // Pb de dialogue ?
   {
      __twi_io_stop();         // Oui, arret de l'échange I2C
      __out_twi(-1);           // Sortie sur erreur, supprime protection accès concurrent
   }

   ack = _twi_io_snd(*data++);   // Emission du 1er octet d'adresse (8 bits ou poids fort)
   if (ack != 0)                 // Pb de dialogue ?
   {
      __twi_io_stop();         // Oui, arret de l'échange I2C
      __out_twi(-1);           // Sortie sur erreur, supprime protection accès concurrent
   }
   if (addr_size > 1)            // Si adresse sur 16 bit
   {
      ack = _twi_io_snd(*data++); // Emission du 2nd octet d'adresse (poids faible)
      if (ack != 0)            // Pb de dialogue ?
      {
         __twi_io_stop();    // Oui, arret de l'échange I2C
         __out_twi(-1);      // Sortie sur erreur, supprime protection accès concurrent
      }
   }

   __twi_io_start();             // Démarrage de l'échange I2C
   ack = _twi_io_snd((dev_addr << 1) | 0x01); // Emission du type de device | 0x01 pour que  R/W (b0) = 1
   if (ack != 0)                 // Pb de dialogue ?
   {
      __twi_io_stop();         // Oui, arret de l'échange I2C
      __out_twi(-1);           // Sortie sur erreur, supprime protection accès concurrent
   }

   for(i=data_len; i > 1; i--)
      *data++ = _twi_io_rcv(0);  // Reception des  data_len-1  octets de data avec ACK = 0

   *data = _twi_io_rcv(1);       // Reception du dernier octet de data avec ACK = 1 (fin de lecture)

   __twi_io_stop();              // Arret de l'échange I2C
   __out_twi(data_len);          // Lecture effectuée, supprime protection accès concurrent
}

/*--------------------------------------------
| Name:        twi_io_snd
| Description: Ecriture du caractère "octet" sur le peripherique I2C ouvert
| Parameters:  byte : octet à envoyer
| Return Type: bit d'acquittement 0 : OK  /  1 : PB
----------------------------------------------*/
int _twi_io_snd(const unsigned char byte)
{
   char ack;
   int i,j;
   char temp;

   __bus_temporization();               // Attente
   TWI_SDA_DIR_OUTPUT;               // SDA en sortie
   __bus_temporization();               // Attente
   temp = (byte >> 7)&0x1;       // calcul du 1er bit a emettre
   for (i=6; i >= 0; i--)
   {
      TWI_SCL_0;
      __sda_scl_temporization();        // Attente basculement scl/sda
      TWI_WSDA(temp);           //  Emission du bit
      temp = (byte >> i)&0x1;   // Calcul du bit a emettre
      __bus_temporization();      // Attente
      TWI_SCL_1;
      __bus_temporization();      // Attente
   }
   TWI_SCL_0;
   __sda_scl_temporization();           // Attente basculement scl/sda
   TWI_WSDA(temp);                   // Emission du dernier bit pour
                                     // l'optimisation de la boucle
   __bus_temporization();               // Attente

   TWI_SCL_1;
   __bus_temporization();               // Attente

   TWI_SCL_0;
   TWI_SDA_DIR_INPUT;                // SDA en entree
   __sda_scl_temporization();           // Attente basculement scl/sda
   TWI_SCL_1;
   __sda_scl_temporization();           // Attente basculement scl/sda
   ack = TWI_RSDA;                    // lecture acquitement
   TWI_SCL_0;

   return (ack);
}

/*--------------------------------------------
| Name:        twi_io_rcv
| Description: Lecture d'un caractère sur le peripherique I2C ouvert
| Parameters:  byte : octet à envoyer
| Return Type:
|      - Renvoi  du caractère lu
|      - Positionnement de l'acquittement a la valeur "ack"
----------------------------------------------*/
int _twi_io_rcv(unsigned char ack)
{
   char octet = 0;
   int i,j;

   TWI_SCL_0;
   __sda_scl_temporization();           // Attente basculement scl/sda
   TWI_SDA_DIR_INPUT;

   for (i=0; i<=7; i++)
   {
      TWI_SCL_1;
      __sda_scl_temporization();        // Attente basculement scl/sda
      octet = (octet << 1) + TWI_RSDA;  // Lecture du bit reçu
      TWI_SCL_0;
      __bus_temporization();         // Attente
   }
   TWI_WSDA(ack);                        // Lecture acquitement
   TWI_SDA_DIR_OUTPUT;                   // SDA en sortie
   TWI_SCL_1;

   return(octet);
}

/*============================================
| End of Source  : at91sam7s_twi_io.c
==============================================*/
