/*********************************************************************
*               SEGGER MICROCONTROLLER SYSTEME GmbH                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (C) 2006   SEGGER Microcontroller Systeme GmbH               *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS * Real time operating system for microcontrollers      *
*                                                                    *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product or a real-time            *
*       operating system for in-house use.                           *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS version: 3.28q                                         *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : OS_Config.h
Purpose : Configuration settings for embOS

          Settings in this file are used for RTOSInit
          Feel free to modify this file acc. to your
          target system.
--------  END-OF-HEADER  ---------------------------------------------
*/

#ifndef OS_CONFIG_H                     /* Avoid multiple inclusion */
#define OS_CONFIG_H

/*********************************************************************
*
*       Configuration for RTOSInit
*
*********************************************************************/

/*********************************************************************
*
*       Clock frequency settings
*/
//lepton patch 20/06/2007
#ifndef OS_FSYS
#define OS_FSYS 47923200L
#endif

/********************************************************************/

#endif                                  /* Avoid multiple inclusion */

/*****  EOF  ********************************************************/
