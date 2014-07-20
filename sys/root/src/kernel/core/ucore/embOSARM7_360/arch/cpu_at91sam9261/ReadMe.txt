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
**********************************************************************

----------------------------------------------------------------------
File    : ReadMe.txt               for ATMEL AT91SAM9261 start project
----------------------------------------------------------------------

**********************************************************************

The embOS start project Start_AT91SAM9261.ewp may be used as a start
for your own application.
The project is setup to run in CSpy simulator or on the ATMEL
AT91SAM9261-EK Evaluation board.

The start project comes with three configurations:

* Debug_SIM
This configuration may be used to develop an embOS application using
CSpy simulator debugger.
The embOS timer interrupt is simulated by the macro file
AT91SAM9261_SIM.mac


* Debug_iRAM
The Debug_iRAM configuration is setup to run an embos application from
CSpy in target RAM using J-Link.

Using different target hardware may require modifications.
A target independent sample application can be found under
Start\Src\main.c

**********************************************************************

