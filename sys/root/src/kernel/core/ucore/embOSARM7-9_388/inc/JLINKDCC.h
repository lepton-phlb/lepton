/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2013  SEGGER Microcontroller GmbH & Co KG         *
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
*       OS version: 3.88a                                            *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : JLINKDCC.h
Purpose : Header file for J-Link ARM type DCC communication
---------------------------END-OF-HEADER------------------------------
*/

#ifndef JLINKDCC_H
#define JLINKDCC_H             // Avoid multiple inclusion

#define DCC_Process JLINKDCC_Process       // for compatibility

void JLINKDCC_Process(void);
void JLINKDCC_HandleDataAbort(void);
void JLINKDCC_SendString(const char * s);
void JLINKDCC_SendChar(char c);
void JLINKDCC_SendOnChannel      (unsigned Channel, unsigned char* pData, int NumItems);
void JLINKDCC_SetpfOnRx          (unsigned Channel, void          (* pf)(unsigned RxChannel, unsigned char c));
void JLINKDCC_SetpfOnTx          (unsigned Channel, void          (* pf)(unsigned TxChannel));
void JLINKDCC_SendCharOnChannelNB(unsigned Channel, unsigned char Data);

#endif                         // Avoid multiple inclusion

/*************************** end of file ****************************/

