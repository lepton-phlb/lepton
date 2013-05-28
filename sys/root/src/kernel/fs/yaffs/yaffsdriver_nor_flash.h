/*--------------------------------------------
| Copyright(C) 2005 CHAUVIN-ARNOUX
|---------------------------------------------
| Project:         
| Project Manager: 
| Source:          yaffsdriver_nor_flash.h
| Path:        C:\tauon\sys\root\src\kernel\fs\yaffs
| Authors:     
| Plateform:   
| Created:     
| Revision/Date: $Revision: 1.1 $  $Date: 2010-05-31 13:44:38 $ 
| Description: 
|---------------------------------------------
| Historic:    
|---------------------------------------------
| Authors     | Date     | Comments  
| $Log: not supported by cvs2svn $
|---------------------------------------------*/


/*============================================
| Compiler Directive   
==============================================*/
#ifndef _YAFFSDRIVER_NOR_FLASH_H
#define _YAFFSDRIVER_NOR_FLASH_H


/*============================================
| Includes 
==============================================*/


/*============================================
| Declaration  
==============================================*/


void _yaffsdriver_nor_flash_read32(yaffs_Device* dev, uint32_t *addr, uint32_t *data, int nwords);
void _yaffsdriver_nor_flash_write32(yaffs_Device* dev, uint32_t *addr, uint32_t *data, int nwords);
void _yaffsdriver_nor_flash_eraseblock(yaffs_Device* dev, uint32_t *addr);
void _yaffsdriver_nor_flash_shutdown(yaffs_Device* dev);
void _yaffsdriver_nor_flash_initialise(yaffs_Device* dev);
uint32_t * _yaffsdriver_nor_flash_getbase(yaffs_Device* dev);


#endif