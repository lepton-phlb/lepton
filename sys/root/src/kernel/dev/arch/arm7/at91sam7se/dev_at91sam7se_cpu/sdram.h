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

#ifndef SDRAM_H
#define SDRAM_H
#define AT91C_SDRAM_BASE 	((volatile unsigned int *)0x20000000)

//* Data bus definition MT48LC16M16A2   AT91SA7SE
#define DQ0             (unsigned int) AT91C_PC0_D0
#define DQ1             (unsigned int) AT91C_PC1_D1
#define DQ2             (unsigned int) AT91C_PC2_D2
#define DQ3             (unsigned int) AT91C_PC3_D3
#define DQ4             (unsigned int) AT91C_PC4_D4
#define DQ5             (unsigned int) AT91C_PC5_D5
#define DQ6             (unsigned int) AT91C_PC6_D6
#define DQ7             (unsigned int) AT91C_PC7_D7
#define DQ8             (unsigned int) AT91C_PC8_D8
#define DQ9             (unsigned int) AT91C_PC9_D9
#define DQ10            (unsigned int) AT91C_PC10_D10
#define DQ11            (unsigned int) AT91C_PC11_D11
#define DQ12            (unsigned int) AT91C_PC12_D12
#define DQ13            (unsigned int) AT91C_PC13_D13
#define DQ14            (unsigned int) AT91C_PC14_D14
#define DQ15            (unsigned int) AT91C_PC15_D15

//* Address bus definition MT48LC16M16A2   AT91SA7SE
//* 16 bits data interface unur 4 banks
#define DQML            (unsigned int) AT91C_PB0_A0_NBS0
#define DQMLH           (unsigned int) AT91C_PA23_NWR1_NBS1_CFIOR_NUB

#define DA0             (unsigned int) AT91C_PB2_A2
#define DA1             (unsigned int) AT91C_PB3_A3
#define DA2             (unsigned int) AT91C_PB4_A4
#define DA3             (unsigned int) AT91C_PB5_A5
#define DA4             (unsigned int) AT91C_PB6_A6
#define DA5             (unsigned int) AT91C_PB7_A7
#define DA6             (unsigned int) AT91C_PB8_A8
#define DA7             (unsigned int) AT91C_PB9_A9
#define DA8             (unsigned int) AT91C_PB10_A10
#define DA9             (unsigned int) AT91C_PB11_A11
#define DA10            (unsigned int) AT91C_PA24_SDA10  // PIO A
#define DA11            (unsigned int) AT91C_PB13_A13
#define DA12            (unsigned int) AT91C_PB14_A14

#define BA0             (unsigned int) AT91C_PB16_A16_BA0
#define BA1             (unsigned int) AT91C_PB17_A17_BA1


//* Control bus definition MT48LC16M16A2   AT91SA7SE
#define CKE             (unsigned int) AT91C_PA25_SDCKE
#define SDWE            (unsigned int) AT91C_PA27_SDWE
#define CAS             (unsigned int) AT91C_PA28_CAS
#define RAS             (unsigned int) AT91C_PA29_RAS

//* SDRAM Configuration MT48LC16M16A2

// Sdram Size	256 Mbits	32 Mbytes	8 MWords ( 4 bytes)
#define AT91C_SDRAM_SIZE_bits   ((256*1024*1024)/4)       // 256 Mbits
#define AT91C_SDRAM_SIZE        ((32*1024*1024)/4)     // 8 MWords (Unsigned int)


#define AT91C_SDRC_TWR_2    ((unsigned int) 0x2 <<  7) // (SDRC) Number of Write Recovery Time Cycles
#define AT91C_SDRC_TRC_4    ((unsigned int) 0x4 << 11) // (SDRC) Number of RAS Cycle Time Cycles 7
#define AT91C_SDRC_TRP_4    ((unsigned int) 0x4 << 15) // (SDRC) Number of RAS Precharge Time Cycles
#define AT91C_SDRC_TRCD_2   ((unsigned int) 0x2 << 19) // (SDRC) Number of RAS to CAS Delay Cycles 2
#define AT91C_SDRC_TRAS_3   ((unsigned int) 0x3 << 23) // (SDRC) Number of RAS Active Time Cycles 5
#define AT91C_SDRC_TXSR_4   ((unsigned int) 0x4 << 27) // (SDRC) Number of Command Recovery Time Cycles 8

// Refresh time 336 for 48MHz (TR= 15.6 * F ) 1/7 = 0.1428
// Refresh period (8,192 rows)
// Time 64ms	8192 row	Refresh period µs 7,8125	nb cycles 375,00
#define AT91B_MCK            __KERNEL_CPU_FREQ
#define AT91C_SDRC_TR_TIME  ((AT91B_MCK * 8) / 1000000)    // refresh time 384 ( 375+ 2%error)

//*----------------------------------------------------------------------------
//* External function prototype
//*----------------------------------------------------------------------------

extern void AT91F_EBI_SDRAM_CfgPIO(void);
extern void AT91F_InitSdram (void);
extern void AT91F_InitDM9000A (void);


#endif	 //  SDRAM_H
