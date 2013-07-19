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
| Compiler Directive
==============================================*/
#ifndef _FLASH_H
#define _FLASH_H


/*============================================
| Includes
==============================================*/


/*============================================
| Declaration
==============================================*/

//NOR FLASH
#define FLASH_AM29DL640D   0
#define FLASH_AM29LV320DT  1
#define FLASH_S29Gl256P    2

typedef struct {
   unsigned long addr;
   unsigned long size;
}flash_sector_t;

typedef struct {
   const flash_sector_t* p_sector_map;
   const unsigned long size;
   const char width;
}flash_type_t;

typedef struct {
   unsigned short *flash_base_pt;
   flash_type_t* p_flash_type;
}dev_flash_t;

extern const flash_type_t flash_type_lst[];

//NAND SPI FLASH
//#if __tauon_cpu_device__!=__tauon_cpu_device_win32_simulation__
   typedef __compiler_directive__packed union
   {
     char tab[3];
     __compiler_directive__packed struct
     {
       uchar8_t manufacturer_id;
       uchar8_t device_Id[2];
     };
   } flash_jedec_did_u;
   
   //
   typedef __compiler_directive__packed struct
   {
     flash_jedec_did_u did;
     uint32_t flash_size;
     uint16_t page_size;
     uchar8_t sector_count;
     uchar8_t firmware_from_sector;
     uchar8_t firmware_sector_count;
     uint32_t firmware_address;
     const char *device_name;
   } dev_flash_nand_spi_t;
//#endif

#endif
