/******************** (C) COPYRIGHT 2013 IJINUS ********************************
* File Name          : target.h
* Author             : Yoann TREGUIER
* Version            : CMsys_1-18.5
* Date               : 2013/04/12
* Description        : Target dependant definitions
*******************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TARGET_H
#define __TARGET_H

/* Includes ------------------------------------------------------------------*/
//#include <RTL.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <stdarg.h>
//#include <string.h>
//#include <ctype.h>
//#include <time.h>

#include "sys_types.h"
#include "073a_target.h"

//#include "sys_cron.h"
//#include "sys_ctrl.h"
//#include "sys_fs.h"
//#include "sys_misc.h"
//#include "sys_os.h"
//#include "sys_stdio.h"
//#include "sys_time.h"
//#include "sys_tty.h"
//#include "sys_legacy.h"

/* Exported define -----------------------------------------------------------*/
#define FIRM_TYP    1
#define FIRM_MAJ    18
#define FIRM_MIN    6
#define FIRM_DATE   "2013/06/14"

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern const char Sys_Date[];

/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#endif /* __TARGET_H */

/******************* (C) COPYRIGHT 2013 IJINUS ****************END OF FILE*****/
