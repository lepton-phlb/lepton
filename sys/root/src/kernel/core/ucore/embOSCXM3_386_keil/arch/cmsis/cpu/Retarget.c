/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co KG                  *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2012  SEGGER Microcontroller GmbH & Co KG         *
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
*       OS version: 3.86g                                            *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : Retarget.c
Purpose : Retargeting module for KEIL MDK-CM3/embOS.
          This modules uses SWO in order to output data to the 
          "Debug (printf) viewer" windows.
          The window can be displayed by starting a debug session 
          and selecting in the menu 
          View\Serial Windows\Debug (printf) viewer.
          In case the first character is missing in the debug viewer:
          Right-clicking in the console window and 
          de-selecting Settings\Add CR to LF solves this problem.
--------- END-OF-HEADER --------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rt_sys.h>
#include <rt_misc.h>

/*********************************************************************
*
*       #pragmas
*
**********************************************************************
*/
#pragma import(__use_no_semihosting)

#ifdef _MICROLIB
  #pragma import(__use_full_stdio)
#endif

/*********************************************************************
*
*       Defines non-configurable
*
**********************************************************************
*/

/* Standard IO device handles - arbitrary, but any real file system handles must be
   less than 0x8000. */
#define STDIN             0x8001    // Standard Input Stream
#define STDOUT            0x8002    // Standard Output Stream
#define STDERR            0x8003    // Standard Error Stream
// sfr
#define ITMStimulusPort8       (*((volatile unsigned char *)(0xE0000000)))
#define ITMStimulusPort32      (*((volatile unsigned long *)(0xE0000000)))
#define DEMCR                  (*((volatile unsigned long *)(0xE000EDFC)))

#define TRCENA                 0x01000000

/*********************************************************************
*
*       Public const
*
**********************************************************************
*/
const char __stdin_name[]  = "STDIN";
const char __stdout_name[] = "STDOUT";
const char __stderr_name[] = "STDERR";

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _OutputViaSWO
*
*  Function description:
*    Outputs the data via SWO.
*    Currently SWO only works with Cortex-M devices.
*    The data are only output when the SWO is enable on the device.
*    Please make also sure that the debugger is connected to the device
*    via SWD. Necessarily to say is that the frequency of the SWO output 
*    needs to set correctly otherwise the output is not displayed properly.
*    The stimulus port that is used Port[0].
*
*  Parameters:
*    pBuffer     - Pointer to the data to be output
*    NumBytes    - Number of bytes to output
*  
*  Return value:
*    Number of byte that have been output
*
*/
static int _OutputViaSWO(const unsigned char * pBuffer, size_t NumBytes) {
  size_t NumChars = 0;

  /* Write to stdout or stderr using swo */
  if (DEMCR & TRCENA) {
    while (NumBytes != 0) {
      while (ITMStimulusPort32 == 0);
      ITMStimulusPort8 = *pBuffer++;
      ++NumChars;
      --NumBytes;
    }
  }
  return NumChars;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       _ttywrch
*
*  Function description:
*    Outputs a character to the console
*
*  Parameters:
*    c    - character to output
*  
*/
void _ttywrch(int c) {
  fputc(c, stdout); // stdout
  fflush(stdout);
}

/*********************************************************************
*
*       _sys_open
*
*  Function description:
*    Opens the device/file in order to do read/write operations
*
*  Parameters:
*    sName        - sName of the device/file to open
*    OpenMode    - This parameter is currently ignored
*  
*  Return value:
*    != 0     - Handle to the object to open, otherwise 
*    == 0     -"device" is not handled by this module
*
*/
FILEHANDLE _sys_open(const char * sName, int OpenMode) {
  // Register standard Input Output devices.
  if (strcmp(sName, __stdout_name) == 0) {
    return (STDOUT);
  } else if (strcmp(sName, __stderr_name) == 0) {
    return (STDERR);
  } else
  return (0);
}

/*********************************************************************
*
*       _sys_close
*
*  Function description:
*    Closes the handle to the open device/file
*
*  Parameters:
*    hFile    - Handle to a file opened via _sys_open
*  
*  Return value:
*    0     - device/file closed
*
*/
int _sys_close(FILEHANDLE hFile) {
  return 0;
}

/*********************************************************************
*
*       _sys_write
*
*  Function description:
*    Writes the data to an open handle.
*    Currently this function only outputs data to the console
*
*  Parameters:
*    hFile    - Handle to a file opened via _sys_open
*    pBuffer  - Pointer to the data that shall be written
*    NumBytes      - Number of bytes to write
*    Mode     - The Mode that shall be used
*  
*  Return value:
*    Number of bytes written to the file/device
*
*/
int _sys_write(FILEHANDLE hFile, const unsigned char * pBuffer, unsigned NumBytes, int Mode) {
  int r = 0;

  if (hFile == STDOUT) {
    _OutputViaSWO(pBuffer, NumBytes);
  }
  return r;
}

/*********************************************************************
*
*       _sys_read
*
*  Function description:
*    Reads data from an open handle.
*    Currently this modules does nothing.
*
*  Parameters:
*    hFile    - Handle to a file opened via _sys_open
*    pBuffer  - Pointer to buffer to store the read data
*    NumBytes      - Number of bytes to read
*    Mode     - The Mode that shall be used
*  
*  Return value:
*    Number of bytes read from the file/device
*
*/
int _sys_read(FILEHANDLE hFile, unsigned char * pBuffer, unsigned NumBytes, int Mode) {
  // Not implemented
  return (0);
}

/*********************************************************************
*
*       _sys_istty
*
*  Function description:
*    This function shall return whether the opened file 
*    is a console device or not.
*
*  Parameters:
*    hFile    - Handle to a file opened via _sys_open
*  
*  Return value:
*    1       - Device is     a console
*    0       - Device is not a console
*
*/
int _sys_istty(FILEHANDLE hFile) {
  if (hFile > 0x8000) {
    return (1);
  }
  return (0);
}

/*********************************************************************
*
*       _sys_seek
*
*  Function description:
*    Seeks via the file to a specific position
*
*  Parameters:
*    hFile  - Handle to a file opened via _sys_open
*    Pos    - 
*  
*  Return value:
*    int       - 
*
*/
int _sys_seek(FILEHANDLE hFile, long Pos) {
  return (0);
}

/*********************************************************************
*
*       _sys_ensure
*
*  Function description:
*    
*
*  Parameters:
*    hFile    - Handle to a file opened via _sys_open
*  
*  Return value:
*    int       - 
*
*/
int _sys_ensure(FILEHANDLE hFile) {
  return (-1);
}

/*********************************************************************
*
*       _sys_flen
*
*  Function description:
*    Returns the length of the opened file handle
*
*  Parameters:
*    hFile    - Handle to a file opened via _sys_open
*  
*  Return value:
*    Length of the file
*
*/
long _sys_flen(FILEHANDLE hFile) {
  return (0);
}

/*********************************************************************
*
*       _sys_tmpnam
*
*  Function description:
*    This function converts the file number fileno for a temporary 
*    file to a unique filename, for example, tmp0001.
*
*  Parameters:
*    pBuffer    - Pointer to a buffer to store the name
*    FileNum    - file number to convert
*    MaxLen     - Size of the buffer
*  
*  Return value:
*     1 - Error
*     0 - Success  
*
*/
int _sys_tmpnam(char * pBuffer, int FileNum, unsigned MaxLen) {
  return (1);
}

/*********************************************************************
*
*       _sys_command_string
*
*  Function description:
*    This function shall execute a system command.
*
*  Parameters:
*    cmd    - Pointer to the command string
*    len    - Length of the string
*  
*  Return value:
*    == NULL - Command was not successfully executed
*    == sCmd - Command was passed successfully
*
*/
char * _sys_command_string(char * cmd, int len) {
  return cmd;
}

/*********************************************************************
*
*       _sys_exit
*
*  Function description:
*    This function is called when the application returns from main
*
*  Parameters:
*    ReturnCode    - Return code from the main function
*  
*
*/
// already defined in lepton
#if 0
void _sys_exit(int ReturnCode) {
  while (1);
}
#endif
