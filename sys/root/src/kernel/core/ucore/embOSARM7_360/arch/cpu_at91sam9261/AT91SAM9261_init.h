/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : AT91SAM9261_init.h
 *    Description : Define init module
 *
 *    History :
 *    1. Data        : March, 31 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 1.1 $
 **************************************************************************/
#include <ioat91sam9261.h>
#include "lib_AT91SAM9261.h"
#include "at91sam9261_board.h"

#ifndef __AT92SAM9261_INIT_H
#define __AT92SAM9261_INIT_H

/*************************************************************************
 * Function Name: AT91F_DBGU_Printk
 * Parameters: char *buffer
 *
 * Return: none
 *
 * Description: This function is used to send a string through the
 *             DBGU channel (Very low level debugging)
 * Note: Arg pointer to a string ending by \0
 *************************************************************************/
void AT91F_DBGU_Printk(char *buffer);

/*************************************************************************
 * Function Name: AT91F_Putc
 * Parameters: int ch
 *
 * Return: int
 *
 * Description: This function sends a char through the DBGU
 *
 *************************************************************************/
int AT91F_Putc(int ch);

/*************************************************************************
 * Function Name: AT91F_Getc
 * Parameters: none
 *
 * Return: int
 *
 * Description: This function receives a char through the DBGU
 *
 *************************************************************************/
int AT91F_Getc(void);

/*************************************************************************
 * Function Name: AT91F_UndefHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Undef handler subroutine
 *
 *************************************************************************/
__arm void AT91F_UndefHandler(void);

/*************************************************************************
 * Function Name: AT91F_SpuriousHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Spurious handler subroutine
 *
 *************************************************************************/
__arm void AT91F_SpuriousHandler(void);

#endif // __AT92SAM9261_INIT_H
