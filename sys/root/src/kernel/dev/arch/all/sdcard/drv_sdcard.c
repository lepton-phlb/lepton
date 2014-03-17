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

#if defined(__IAR_SYSTEMS_ICC) || defined(__IAR_SYSTEMS_ICC__)
   #include <ioat91sam9261.h>
   #include <intrinsic.h>
#else
   #include "cyg/hal/at91sam9261.h"
#endif

#if defined(USE_ECOS)
   #include "pkgconf/hal_arm_at91sam9261.h"
#endif

#include "kernel/core/kernelconf.h"
#include "drv_sdcard.h"

/*----------------------------------------------------------------------------*/
// Pour optimiser le temps d'ecriture, on fait un envoi multiple des blocs de donnees.
// Pour se faire, le software a besoin de 2 buffers temporaire de 512 ocets chacun, un
// pour contenir le premier buffer puis les suivants et l'autre pour contenir le dernier.
// Afin de librer un des deux buffers de 512 oc

#define SDCARD_OPT_STACK_SIZE

//static kernel_pthread_mutex_t s_sd_mutex;

//#define __sd_lock()      kernel_pthread_mutex_lock  (&s_sd_mutex)
//#define __sd_unlock()    kernel_pthread_mutex_unlock(&s_sd_mutex)

/*----------------------------------------------------------------------------*/
// Prototypes des fonctions internes
int  sd_init(desc_t desc);
void sd_sendCMD(desc_t desc,unsigned char Command, unsigned long Arg);
long sd_waitRESPONSE_R1R2(desc_t desc,unsigned char ResponseType);
int  sd_getCSD(desc_t desc,unsigned char *CSD);
int  sd_getCID(desc_t desc,unsigned char *CID);
int  sd_waitTOKEN(desc_t desc,unsigned char token);
int  sd_waitDATARESP(desc_t desc);
int  sd_waitEndBUSY(desc_t desc);
int  sd_waitBUSY(desc_t desc);
void sd_DELAY(desc_t desc,int delay);
int sd_write_simple_block(desc_t desc,unsigned long AddBlock, unsigned char *Buffer);

/*----------------------------------------------------------------------------*/
// Variables globales externes
long status;            //Variable contenant le status d'erreur
volatile unsigned char BufferTemp[BLOCK_SIZE]  __attribute__ ((section (".no_cache")));

/*--------------------------------------------
| Name:        _lowlevel_drv_read
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _lowlevel_drv_read(desc_t desc,char* buffer, int size){
   desc_t desc_link;

   if((desc_link=ofile_lst[desc].desc_nxt[0])<0)
      return -1;

   return ofile_lst[desc_link].pfsop->fdev.fdev_read(desc,buffer,size);
}

/*--------------------------------------------
| Name:        _lowlevel_drv_write
| Description:
| Parameters:  none
| Return Type: none
| Comments:
| See:
----------------------------------------------*/
int _lowlevel_drv_write(desc_t desc,char* buffer, int size){
   desc_t desc_link;

   if((desc_link=ofile_lst[desc].desc_nxt[1])<0)
      return -1;

   return ofile_lst[desc_link].pfsop->fdev.fdev_write(desc,buffer,size);
}

/******************************************************************************/
// Fonction:	sdcard_init
/******************************************************************************/
// Arguments:	Pointeur sur une structure t_media permettant de retourner les
//              informations utiles sur la SD Card
/******************************************************************************/
// Retour:		Resultat de l'initialisation (0 si OK, -1 sinon)
/******************************************************************************/
// Description:	Initialise le driver SPI et la SD card en mode SPI
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
int sdcard_init(desc_t desc, t_media *sdcard)
{
   unsigned char Register[REGISTER_SIZE];

   // initialization
   // pthread_mutexattr_t  mutex_attr=0;
   // kernel_pthread_mutex_init(&s_sd_mutex, &mutex_attr);

#ifdef CHECKING_SD_CONNECTION
   if (__lowlevel_getstatus_CD==1)
      return(-1);                       // VERIFICATION DE LA PRESENCE DE LA CARTE
   if (__lowlevel_getstatus_WE==1)
      return(-1);                       // VERIFICATION QUE LA CARTE N'EST PAS LOCKEE
#endif
   //
   __lowlevel_drv_init();                                                               // INITIALISATION DU DRIVER SPI
   //
   if (sd_init(desc) == -1)
      return(-1);                                       // INITIALISATION DE LA SD CARD EN MODE SPI
   //
   if (sd_getCSD(desc,&Register[0]) == -1)
      return(-1);               // RECUPERATION DE LA TAILLE DU MEDIA

   //
   sdcard->size = 0x01 << ((((Register[9]&0x03)<<1) + ((Register[10]&0x80)>>7))+2);
   sdcard->size = sdcard->size *
                  (((Register[6]&0x03)<<10)+ (Register[7]<<2) + ((Register[8]&0xC0)>>6) +1);
   sdcard->size = sdcard->size * (1<<(Register[5]&0x0F));
   sd_DELAY(desc,1);

   if (sd_getCID(desc,&Register[0]) == -1) return(-1);                  // RECUPERATION DE L'ID DU MEDIA
   sdcard->volid[0] = Register[9];
   sdcard->volid[1] = Register[10];
   sdcard->volid[2] = Register[11];
   sdcard->volid[3] = Register[12];
   sd_DELAY(desc,1);

   return (0);
}

/******************************************************************************/
// Fonction:	sdcard_read
/******************************************************************************/
// Arguments:	Pointeur sur le buffer de reception des datas lues
//                              Adresse a partir de laquelle lire
//				Nombre d'octets a lire
/******************************************************************************/
// Retour:		0 si OK, -1 sinon
/******************************************************************************/
// Description:	Lire un nombre d'octet defini a une adresse definie
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
int sdcard_read (desc_t desc, unsigned char *BufferRead, unsigned long Add, unsigned long Size)
{
   // static unsigned char BufferTemp[BLOCK_SIZE]; voir d�claration en haut du fichier

   unsigned long AddBlock, NbBlock, NumBlock=0;
   int inc = 0, result = 0, FirstData,LastData,i, AddDiff;

   /* VALEURS UTILES POUR UN ENVOI EN BLOCS DE 512 OCTETS */
   AddDiff = Add % BLOCK_SIZE;                                                          // Offset entre l'adresse reelle et celle du 1er block a lire
   AddBlock = Add - AddDiff;                                                            // Addresse du premier block a lire
   NbBlock = Size + AddDiff;                                                            // Nombre de blocks a lire
   if (NbBlock % BLOCK_SIZE > 0)
      NbBlock = (NbBlock / BLOCK_SIZE) + 1;
   else
      NbBlock = NbBlock / BLOCK_SIZE;

   // Semaphore protect
   // __sd_lock();

   //
   __lowlevel_reset_CS;                                                                 // ACTIVATION DE LA COM

   /* ENVOI DE LA COMMANDE DE LECTURE */
   if (NbBlock == 1)
      sd_sendCMD(desc,CMD17_READ_SINGLE_BLOCK, AddBlock);
   else
      sd_sendCMD(desc,CMD18_READ_MULTIPLE_BLOCK, AddBlock);
   status = sd_waitRESPONSE_R1R2(desc,RESPONSE_R1);
   sd_DELAY(desc,1);
   if (status != STATUS_SD_SPI_OK)
      result = -1;

   while ((NumBlock<NbBlock) && (result == 0))                          // LECTURE DES BLOCS DE DATAS
   {
      result = sd_waitTOKEN(desc,TOKEN_MASK_START_SRW_MR);           // Attente du Start Token indiquant le debut d'un bloc
      if (result == 0)
      {
         __lowlevel_drv_read(desc,(char*)BufferTemp,BLOCK_SIZE);                // Lecture du bloc
         if (NumBlock == 0)
            FirstData = AddDiff;
         else
            FirstData = 0;
         if (NumBlock == (NbBlock-1))
         {
            LastData = (Size + AddDiff) % BLOCK_SIZE;
            if (LastData == 0) LastData = BLOCK_SIZE;
         }else {
            LastData = BLOCK_SIZE;
         }
         //
         for (i=FirstData; i<LastData; i++)
            BufferRead[inc++] = BufferTemp[i];

         NumBlock++;
         __lowlevel_drv_read(desc,(char*)BufferTemp,CHECKSUM_SIZE);               // Lecture du CRC
      }
   }
   sd_DELAY(desc,1);

   if ((NbBlock > 1) && (result == 0))                                          // ENVOI D'UN STOP SI LECTURE MULTIPLE
   {
      sd_sendCMD(desc,CMD12_STOP_TRANSMISSION, CMD_NO_ARG);          // Envoi de la commande
      status = sd_waitRESPONSE_R1R2(desc,RESPONSE_R1);                          // Reception de la reponse
      if (result !=0)
         result = -1;
      sd_DELAY(desc,1);
   }

   __lowlevel_set_CS;                                                                           // FIN DE LA COM

   // Semaphore protect
   // __sd_unlock();


   return (result);
}

/******************************************************************************/
// Fonction:	sdcard_write
/******************************************************************************/
// Arguments:	Pointeur sur le buffer a ecrire
//                              Adresse a partir de laquelle ecrire
//				Nombre d'octets a ecrire
/******************************************************************************/
// Retour:		retourne 0 si OK, -1 sinon
/******************************************************************************/
// Description:	Ecrire un nombre d'octets defini a une adresse definie
/******************************************************************************/
// Attention:	Le define SDCARD_OPT_STACK_SIZE permet d'optimiser la taille de
//				la pile en supprimant un buffer de 512 octets mais reduit la
//				vitesse car elle fait un envoi d'un bloc simple suppl�mentaire.
/******************************************************************************/
int sdcard_write (desc_t desc, unsigned char *BufferWrite, unsigned long Add, unsigned long Size)
{
#ifdef SDCARD_OPT_STACK_SIZE
   static unsigned char Buffer[BLOCK_SIZE];                                             // 1 seul buffer defini en cas d'optimisation de la pile
#else
   // static declaration - see above
   static unsigned char Buffer[BLOCK_SIZE],LastBuffer[BLOCK_SIZE];
#endif

   unsigned long AddBlock, NbBlock,NumBlock=0;
   unsigned int AddDiff;
   unsigned char Token;
   unsigned long i;
   int result = 0;

   // Semaphore protect
   // __sd_lock();

   AddDiff = Add % BLOCK_SIZE;                                                          // On calcule differentes valeurs pour faire des envois en bloc de 512
   AddBlock = Add - AddDiff;                                                            // correspondants aux secteurs memoires de la sdcard. (l'offset entre
   NbBlock = Size + AddDiff;                                                            // l'adresse physique et celle voulue,addresse du premier
   if (NbBlock % BLOCK_SIZE > 0)
      NbBlock = (NbBlock / BLOCK_SIZE) + 1;  // block a reecrire et le nombre de blocks a reecrire)
   else
      NbBlock = NbBlock / BLOCK_SIZE;

   if (NbBlock == 1)                                                                            // ********** SI UN SEUL BLOC, ENVOI EN MODE SIMPLE **********
   {
      if (!((Size == BLOCK_SIZE) && (AddDiff == 0)))            // Il faut memoriser les donnees du secteur pour ne pas reecrire dessus
      {                                                                                                         // Si le buffer a lire ne correspond pas a un bloc entier
         if(sdcard_read (desc,Buffer, AddBlock, BLOCK_SIZE) == -1) {
            // __sd_unlock();
            return (-1);
         }
      }
      for(i=0; i<Size; i++)
         Buffer[i+AddDiff] = BufferWrite[i];    // Creation du buffer a envoyer
      result = sd_write_simple_block(desc,AddBlock, Buffer);           // Envoi du buffer en mode simple
   }
   else                                                                                                 // ********** SI PLUSIEURS BLOCS, ENVOI EN MODE MULTIPLE **********
   {
      if (AddDiff != 0)                                                                         // L'adresse ne correspond pas a une adresse de secteur de la sdcard
      {                                                                                                         // On construit le premier bloc avec les donnees a ne pas modifier
         if(sdcard_read (desc,Buffer, AddBlock, BLOCK_SIZE) == -1) {
            // __sd_unlock();
            return (-1);
         }
         for(i=0; i<(BLOCK_SIZE-AddDiff); i++) Buffer[i+AddDiff] = BufferWrite[i];
      }
#ifndef SDCARD_OPT_STACK_SIZE
      if (((Size+AddDiff)%BLOCK_SIZE) != 0)                                     // Si le deuxieme buffer est disponible pour l'envoi multiple,
      {                                                                                                                 // On construit le dernier bloc avec les donnees a ne pas modifier
         if(sdcard_read (desc,LastBuffer, AddBlock + (BLOCK_SIZE * (NbBlock - 1)),
                         BLOCK_SIZE) == -1) {
            // __sd_unlock();
            return (-1);
         }
         for (i=0; i<((Size+AddDiff)%BLOCK_SIZE); i++)
            LastBuffer[i] = BufferWrite[i+((NbBlock-1)*BLOCK_SIZE)-AddDiff];
      }
#endif
      __lowlevel_reset_CS;                                                                      // ********** ACTIVATION DE LA COM **********
      sd_sendCMD(desc,CMD25_WRITE_MULTIPLE_BLOCK, AddBlock);           // Envoi de la commande d'ecriture
      status = sd_waitRESPONSE_R1R2(desc,RESPONSE_R1);                          // Reception de la reponse
      sd_DELAY(desc,1);
      if (status != STATUS_SD_SPI_OK)
         result = -1;   // ERREUR lors de la commande

#ifdef SDCARD_OPT_STACK_SIZE
      while((NumBlock<(NbBlock-1)) && (result == 0))                    // ENVOI DES BLOCKS DE DATAS sauf le dernier si la pile est optimisee
#else
      while((NumBlock<NbBlock) && (result == 0))                                // ENVOI DES BLOCKS DE DATAS
#endif
      {
         Token = TOKEN_MASK_START_MW;
         __lowlevel_drv_write(desc,&Token,TOKEN_SIZE);                  // Envoi du start token pour indiquer le debut des data
         if (NumBlock == 0)                                                     // Envoi du premier bloc
         {
            if (AddDiff !=0)
               __lowlevel_drv_write(desc,Buffer,BLOCK_SIZE);
            else
               __lowlevel_drv_write(desc,BufferWrite,BLOCK_SIZE);
            BufferWrite += BLOCK_SIZE - AddDiff;                        // Addresse du deuxieme block
         }
         else
         {
            if (NumBlock == (NbBlock -1))                                       // Envoi du dernier bloc
            {
#ifndef SDCARD_OPT_STACK_SIZE
               if (((Size+AddDiff)%BLOCK_SIZE) != 0)
                  __lowlevel_drv_write(desc,LastBuffer,BLOCK_SIZE);
               else
                  __lowlevel_drv_write(desc,BufferWrite,BLOCK_SIZE);
#endif
            }
            else                                                                                        // Envoi de blocs
            {
               __lowlevel_drv_write(desc,BufferWrite,BLOCK_SIZE);
               BufferWrite += BLOCK_SIZE;
            }
         }
         NumBlock++;
         __lowlevel_drv_write(desc,Buffer,CHECKSUM_SIZE);                       // Envoi d'un checksum bidon
         result = sd_waitDATARESP(desc);                                                // Attente de la data response
         if(result ==0)
            result = sd_waitEndBUSY(desc);      // Attente de la fin du busy
         sd_DELAY(desc,1);
      }

      if (result == 0)                                                                          // Envoi du stop token
      {
         Token = TOKEN_MASK_STOP_MW;
         __lowlevel_drv_write(desc,&Token,TOKEN_SIZE);
         result = sd_waitBUSY(desc);
      }
      __lowlevel_set_CS;                                                                                // ********** FIN DE LA COM **********

#ifdef SDCARD_OPT_STACK_SIZE
      if (((Size+AddDiff)%BLOCK_SIZE) != 0)                             // En cas d'optimisation de stack
      {                                                                                                         // On construit le dernier bloc avec les donnees a ne pas modifier
         if(sdcard_read (desc,Buffer, AddBlock + (BLOCK_SIZE * (NbBlock - 1)), BLOCK_SIZE) == -1) {
            // __sd_unlock();
            return (-1);
         }
         for (i=0; i<((Size+AddDiff)%BLOCK_SIZE); i++)
            Buffer[i] = BufferWrite[i];
      }
      if (((Size+AddDiff)%BLOCK_SIZE) != 0)
         result = sd_write_simple_block(desc,AddBlock + (BLOCK_SIZE * (NbBlock - 1)), Buffer);
      else
         result = sd_write_simple_block(desc,AddBlock + (BLOCK_SIZE * (NbBlock - 1)), BufferWrite);
#endif
   }

   // __sd_unlock();

   return (result);                                                                             // Retourne -1 en cas d'erraur, 0 sinon
}

/******************************************************************************/
// Fonction:	sd_write_simple_block
/******************************************************************************/
// Arguments:
/******************************************************************************/
// Retour:		retourne 0 si OK, -1 sinon
/******************************************************************************/
// Description:	Ecriture d'un block de 512 octets en mode simple dans la sdcard
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
int sd_write_simple_block(desc_t desc, unsigned long AddBlock, unsigned char *Buffer)
{
   unsigned char Token;

   __lowlevel_reset_CS;                                                                 // ********** ACTIVATION DE LA COM **********

   sd_sendCMD(desc,CMD24_WRITE_BLOCK, AddBlock);                        // Envoi de la commande
   status = sd_waitRESPONSE_R1R2(desc,RESPONSE_R1);                             // Reception de la reponse
   sd_DELAY(desc,1);
   if (status != STATUS_SD_SPI_OK)
      return (-1);              // ERREUR lors de la commande

   Token = TOKEN_MASK_START_SRW_MR;
   __lowlevel_drv_write(desc,&Token,TOKEN_SIZE);                        // Envoi du start token pour indiquer le debut des data
   __lowlevel_drv_write(desc,Buffer,BLOCK_SIZE);                        // Envoi des datas
   __lowlevel_drv_write(desc,Buffer,CHECKSUM_SIZE);                     // Envoi d'un checksum bidon
   if (sd_waitDATARESP(desc) == -1) return(-1);                         // Attente de la data response
   if (sd_waitEndBUSY(desc) == -1) return(-1);                                  // Attente de la fin du busy
   sd_DELAY(desc,1);

   __lowlevel_set_CS;                                                                           // ********** FIN DE LA COM **********

   return(0);
}

/******************************************************************************/
// Fonction:	sd_init
/******************************************************************************/
// Arguments:
/******************************************************************************/
// Retour:		Resultat de l'initialisation (0 si OK, -1 sinon)
/******************************************************************************/
// Description:	Initialisation de la SD Card en mode SPI
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
int sd_init(desc_t desc)
{
   unsigned char Buffer;
   unsigned long count = (unsigned long)IDLE_TIMEOUT;

   sd_DELAY(desc,10);                                                                                   // REVEIL DE LA CARTE : 10 clocks

   __lowlevel_drv_read(desc,&Buffer,0);                                                 // On fait une lecture afin de vider les buffers d'emission et reception

   __lowlevel_reset_CS;                                                                 // ACTIVATION DE LA COM

   sd_sendCMD(desc,CMD0_GO_IDLE_STATE, CMD_NO_ARG);                             // RESET DU MEDIA
   status = sd_waitRESPONSE_R1R2(desc,RESPONSE_R1);                             // Reception de la reponse, le media doit etre en mode Idle
   sd_DELAY(desc,1);
   if (status != STATUS_SD_IDLE_STATE)
   {
      __lowlevel_set_CS;
      return (-1);
   }

   while ((status!=STATUS_SD_SPI_OK) && count)                          // INITIALISATION DU MEDIA EN MODE SPI
   {
      sd_sendCMD(desc,CMD55_APP_CMD, CMD_NO_ARG);                               // Envoi des commandes CMD55 et ACMD41 jusqu'a ce que le
      status = sd_waitRESPONSE_R1R2(desc,RESPONSE_R1);                          // media quitte le mode Idle
      sd_DELAY(desc,1);
      if ((status != STATUS_SD_IDLE_STATE)&&(status != STATUS_SD_SPI_OK))
      {
         __lowlevel_set_CS;
         return (-1);
      }
      sd_sendCMD(desc,ACMD41_SD_SEND_OP_COND, CMD_NO_ARG);
      status = sd_waitRESPONSE_R1R2(desc,RESPONSE_R1);
      sd_DELAY(desc,1);
      if ((status != STATUS_SD_IDLE_STATE)&&(status != STATUS_SD_SPI_OK))
      {
         __lowlevel_set_CS;
         return (-1);
      }
      count--;
   }
   if(!count)                                                                                   // Compteur au cas ou le media ne soit pas sorti du mode Idle
   {
      status = STATUS_SD_TIMEOUT_IDLE;
      __lowlevel_set_CS;
      return (-1);
   }

   sd_sendCMD(desc,CMD16_SET_BLOCKLEN, BLOCK_SIZE);                             // INITIALISATION DE LA LONGUEUR DES BLOCS
   status = sd_waitRESPONSE_R1R2(desc,RESPONSE_R1);
   sd_DELAY(desc,1);
   if (status != STATUS_SD_SPI_OK) return(-1);

   __lowlevel_set_CS;                                                                           // FIN DE LA COM
   return (0);
}

/******************************************************************************/
// Fonction:	sd_sendCMD
/******************************************************************************/
// Arguments:	Le type de commande et les arguments
/******************************************************************************/
// Retour:
/******************************************************************************/
// Description:	Construction et Envoi d'une commande a la SD Card
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
void sd_sendCMD(desc_t desc,unsigned char Command, unsigned long Arg)
{
   unsigned char BufferCmd[CMD_SIZE];

   BufferCmd[0]=CMD_START_TRAME|Command;                                // CREATION DE LA TRAME COMMANDE
   BufferCmd[1]=(unsigned char)(Arg>>24);
   BufferCmd[2]=(unsigned char)(Arg>>16);
   BufferCmd[3]=(unsigned char)(Arg>>8);
   BufferCmd[4]=(unsigned char)(Arg);                                           // En mode SPI, seule la commande CMD0 doit avoir un checksum valide
   if (Command == CMD0_GO_IDLE_STATE)
      BufferCmd[5] = CMD_END_TRAME_CMD0_CS;
   else
      BufferCmd[5] = CMD_END_TRAME_NO_CS;

   __lowlevel_drv_write(desc,BufferCmd, CMD_SIZE);                      // ENVOI DE LA CMD AU SPI
   return;
}

/******************************************************************************/
// Fonction:	sd_waitRESPONSE_R1R2
/******************************************************************************/
// Arguments:	Le type de reponse
/******************************************************************************/
// Retour:		Reponse recue
/******************************************************************************/
// Description:	Reception d'une reponse de type R1 ou R2
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
long sd_waitRESPONSE_R1R2(desc_t desc,unsigned char ResponseType)
{
   unsigned char Response = RESPONSE_R1_MASK;
   unsigned char count    = RESPONSE_TIMEOUT;
   unsigned long result   = 0;

   sd_DELAY(desc,1);                                                                                    // DELAI DE 1 AVANT LA RECEPTION

   while ((Response & RESPONSE_R1_MASK) && count)               // RECEPTION DE R1
   {                                                                                                            // Attente d'un octet avec un MSB a 0
      __lowlevel_drv_read(desc,&Response,RESPONSE_R1_SIZE);           // Lecture de la reponse R1
      count--;
   }

   if(!count)
      return (STATUS_SD_RESPONSE_TIMEOUT);              // Reponse non arrivee
   result = result | (Response<<8);                                     // Memorisation de R1 dans result (avec un decalage de 8)

   if (ResponseType == RESPONSE_R2)                                             // RECEPTION DE R2 SI REQUIS
   {
      __lowlevel_drv_read(desc,&Response,RESPONSE_R2_SIZE);           // Lecture de la reponse R2
      result = result | Response;                                                       // Memorisation de R2 dans Response
   }

   return (result);                                                                             // ON RETOURNE R1 ou R1 ET R2
}

/******************************************************************************/
// Fonction:	sd_getCSD
/******************************************************************************/
// Arguments:	Pointeur sur le Registre CSD
/******************************************************************************/
// Retour:		Resultat de la lecture (0 si OK, -1 sinon)
/******************************************************************************/
// Description:	Reception du registre CSD
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
int sd_getCSD(desc_t desc,unsigned char *CSD)
{
   unsigned long count = RESPONSE_TIMEOUT;
   unsigned char Response[2];

   __lowlevel_reset_CS;                                                                 // ACTIVATION DE LA COM

   sd_sendCMD(desc,CMD9_SEND_CSD, CMD_NO_ARG);                                  // DEMANDE DE LECTURE DU CSD
   status = sd_waitRESPONSE_R1R2(desc,RESPONSE_R1);
   if (status != STATUS_SD_SPI_OK)
   {
      __lowlevel_set_CS;
      return(-1);
   }

   Response[0] = 0xFF;                                                                          // RECEPTION DU REGISTRE CSD
   while ((Response[0] != TOKEN_MASK_START_SRW_MR) && count)      // On attend le token indiquant que le registre va
   {                                                                                                                    // etre envoye
      __lowlevel_drv_read(desc,&Response[0],RESPONSE_R1_SIZE);
      count--;
   }
   if(!count)                                                                                   // Token non recu
   {
      __lowlevel_set_CS;
      status = STATUS_SD_RESPONSE_TIMEOUT;
      return (-1);
   }
   __lowlevel_drv_read(desc,CSD,REGISTER_SIZE);                                 // Lecture du registre CSD
   __lowlevel_drv_read(desc,&Response[0],CHECKSUM_SIZE);        // Lecture du checksum

   return (0);
}

/******************************************************************************/
// Fonction:	sd_getCID
/******************************************************************************/
// Arguments:	Pointeur sur le Registre CID
/******************************************************************************/
// Retour:		Resultat de la lecture (0 si OK, -1 sinon)
/******************************************************************************/
// Description:	Reception du registre CID
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
int sd_getCID(desc_t desc,unsigned char *CID)
{
   unsigned long count = RESPONSE_TIMEOUT;
   unsigned char Response[2];

   __lowlevel_reset_CS;                                                                 // ACTIVATION DE LA COM

   sd_sendCMD(desc,CMD10_SEND_CID, CMD_NO_ARG);                                 // DEMANDE DE LECTURE DU CID
   status = sd_waitRESPONSE_R1R2(desc,RESPONSE_R1);
   if (status != STATUS_SD_SPI_OK)
   {
      __lowlevel_set_CS;
      return(-1);
   }

   Response[0] = 0xFF;                                                                          // RECEPTION DU REGISTRE CID
   while ((Response[0] != TOKEN_MASK_START_SRW_MR) && count)      // On attend le token indiquant que le
   {                                                                                                            // registre va etre envoye
      __lowlevel_drv_read(desc,&Response[0],RESPONSE_R1_SIZE);
      count--;
   }
   if(!count)                                                                                   // Token non recu
   {
      __lowlevel_set_CS;
      status = STATUS_SD_RESPONSE_TIMEOUT;
      return (-1);
   }
   __lowlevel_drv_read(desc,CID,REGISTER_SIZE);                                 // Lecture du registre
   __lowlevel_drv_read(desc,&Response[0],CHECKSUM_SIZE);        // Lecture du checksum

   return (0);
}

/******************************************************************************/
// Fonction:	sd_waitTOKEN
/******************************************************************************/
// Arguments:	Le type de token attendu
/******************************************************************************/
// Retour:		0 si OK, -1 sinon
/******************************************************************************/
// Description:	Reception d'une reponse de type token
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
int sd_waitTOKEN(desc_t desc,unsigned char token)
{
   unsigned char Response = 0xFF;
   unsigned long count = READ_TIMEOUT;

   sd_DELAY(desc,1);                                                                                    // DELAI DE 1 MINIMUM AVANT DE LIRE LE TOKEN

   while ((Response != token) && count)                                 // ATTENTE D'UN OCTET DE TYPE TOKEN
   {
      __lowlevel_drv_read(desc,&Response, TOKEN_SIZE);                          // Lecture de l'octet
      status = ((unsigned long)Response)<<8;
      if (status == STATUS_SD_READ_ERROR) return(-1);           // On retourne une erreur si c'est un token error
      if (status == STATUS_SD_READ_CC_ERROR) return(-1);
      if (status == STATUS_SD_READ_ECC_ERROR) return(-1);
      if (status == STATUS_SD_READ_OUT_OF_RANGE) return(-1);
      count--;
   }

   if(!count)                                                                                   // Token non recu
   {
      status = STATUS_SD_TOKEN_TIMEOUT;
      return(-1);
   }

   return (0);
}

/******************************************************************************/
// Fonction:	sd_waitDATARESP
/******************************************************************************/
// Arguments:
/******************************************************************************/
// Retour:		0 si OK, -1 sinon
/******************************************************************************/
// Description:	Attendre l'octet de debut de datas
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
int sd_waitDATARESP(desc_t desc)
{
   unsigned char Response = 0xFF;
   unsigned long count = DATA_RESP_TIMEOUT;

   while (count)                                                                                // ATTENTE DE L'ARRIVEE DU TOKEN
   {
      __lowlevel_drv_read(desc,&Response,TOKEN_SIZE);                   // Lecture de l'octet
      Response = Response & 0x0F;                                                       // On retourne 0 si le token est arrive
      if ((Response & TOKEN_MASK_WRITE_OK) == TOKEN_MASK_WRITE_OK) return (0);
      if ((Response & TOKEN_MASK_WRITE_CRC_ERROR) == TOKEN_MASK_WRITE_CRC_ERROR)
      {                                                                                                         // En cas de token error, on retourne une erreur
         status = STATUS_SD_WRITE_CRC_ERROR;
         return(-1);
      }
      if ((Response & TOKEN_MASK_WRITE_ERROR) == TOKEN_MASK_WRITE_ERROR)
      {
         status = STATUS_SD_WRITE_ERROR;
         return (-1);
      }
      count--;
   }
   status = STATUS_SD_DATA_RESP_TIMEOUT;                                // Token non recu
   return(-1);
}

/******************************************************************************/
// Fonction:	sd_waitEndBUSY
/******************************************************************************/
// Arguments:
/******************************************************************************/
// Retour:		0 si OK, -1 sinon
/******************************************************************************/
// Description:	Attendre la fin d'un busy
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
int sd_waitEndBUSY(desc_t desc)
{
   unsigned char Response = 0x00;
   unsigned long count = WRITE_TIMEOUT;

   while ((Response == TOKEN_MASK_BUSY) && (count))      // ATTENTE DE LA FIN D'UN BUSY
   {
      __lowlevel_drv_read(desc,&Response,TOKEN_SIZE);
      count--;
   }
   if (!count)                  // Erreur : Carte toujours Busy
   {
      status = STATUS_SD_BUSY_TIMEOUT;
      return(-1);
   }

   return (0);
}

/******************************************************************************/
// Fonction:	sd_waitBUSY
/******************************************************************************/
// Arguments:
/******************************************************************************/
// Retour:		0 si OK, -1 sinon
/******************************************************************************/
// Description:	Attendre le debut puis la fin d'un busy
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
int sd_waitBUSY(desc_t desc)
{
   unsigned char Response = 0xFF;
   unsigned long count = WRITE_TIMEOUT;

   while ((Response != TOKEN_MASK_BUSY) && (count))      // ATTENTE DU DEBUT DU BUSY
   {
      __lowlevel_drv_read(desc,&Response,TOKEN_SIZE);
      count--;
   }
   if (!count)                                                  // Busy non recu
   {
      status = STATUS_SD_BUSY_TIMEOUT;
      return(-1);
   }
   return (sd_waitEndBUSY(desc));               // ATTENTE DE LA FIN DU BUSY
}

/******************************************************************************/
// Fonction:	sd_DELAY
/******************************************************************************/
// Arguments:	Temps a attendre (unite : 8 clocks)
/******************************************************************************/
// Retour:
/******************************************************************************/
// Description:	Attendre un certain nombre de cycles de clock
/******************************************************************************/
// Attention:	XXXX
/******************************************************************************/
void sd_DELAY(desc_t desc,int delay)
{
   unsigned char Buffer = 0xFF;

   while(delay-- != 0) __lowlevel_drv_write(desc,&Buffer,1);
   return;
}

/******************************************************************************/


