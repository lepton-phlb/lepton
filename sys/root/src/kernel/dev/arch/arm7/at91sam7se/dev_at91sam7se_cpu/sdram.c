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

// Include Standard files
//#include "project.h"
#include "dev_at91sam7se_cpu.h"
#include "sdram.h"
//#include "AT91SAM7SE-EK.h"


//*----------------------------------------------------------------------------
//* \fn    AT91F_EBI_DM9000A_CfgPIO
//* \brief Configure the PIO for DM9000A check pinout connection
//*----------------------------------------------------------------------------
void AT91F_EBI_DM9000A_CfgPIO(void)
{
   // Configure the relevant PIO port A controller pins to periph mode
   AT91F_PIO_CfgPeriph(
      AT91C_BASE_PIOA, // PIO controller base address
      (unsigned int) AT91C_PA30_IRQ1,           // Peripheral A
                // BM le 5.03.08 ((unsigned int) AT91C_PA12_A12        ) |
                ((unsigned int) AT91C_PA20_NCS2_CFCS1 )   // Peripheral B
      );

        // Disable the internal pull up on the IRQ1 line
        *AT91C_PIOA_PPUDR = 0x40000000;

   // Configure the relevant PIO port C controller pins to periph mode
        // NB - AT91C_PC0_D0 --> _D15 already configured by sdram.c
   AT91F_PIO_CfgPeriph(
      AT91C_BASE_PIOC, // PIO controller base address
      0,                                           // Peripheral A
      ((unsigned int) AT91C_PC21_NWR0_NWE_CFWE ) |
                ((unsigned int) AT91C_PC22_NRD_CFOE      )   // Peripheral B
                );

}

//*----------------------------------------------------------------------------
//* \fn    AT91F_InitDM9000A
//* \brief Init EBI and SMC controller for DM9000A
//*----------------------------------------------------------------------------
void AT91F_InitDM9000A (void)
{
    volatile unsigned int i;
    AT91PS_SMC2 psmc = AT91C_BASE_SMC;

    // Init the EBI CS2 for SMC/NCS2 (should be the default anyway)
    AT91C_BASE_EBI->EBI_CSA |= AT91C_EBI_CS2A_SMC
                            |  AT91C_EBI_NWPC_ON;

    AT91F_EBI_DM9000A_CfgPIO();

    // Configure SMC chip select register 2
    psmc->SMC2_CSR[2] = (AT91C_SMC2_NWS & 0x01)             |
                        AT91C_SMC2_WSEN                     |
                        (AT91C_SMC2_TDF & (0x02 << 8))      |
                        AT91C_SMC2_BAT                      |
                        AT91C_SMC2_DBW_16                   |
                        (AT91C_SMC2_RWSETUP & (0x01 << 24)) |
                        (AT91C_SMC2_RWHOLD  & (0x01 << 28)) ;

    // Wait time
    for (i =0; i< 1000;i++);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_EBI_SDRAM_CfgPIO
//* \brief Configure the PIO for SDRAM check pinout connection
//*----------------------------------------------------------------------------
void AT91F_EBI_SDRAM_CfgPIO(void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
      ((unsigned int) AT91C_PA20_NCS2_CFCS1) |/*phlb patch for dm9000a*/
		((unsigned int) AT91C_PA23_NWR1_NBS1_CFIOR_NUB) |
		((unsigned int) AT91C_PA24_SDA10   )            |
		((unsigned int) AT91C_PA25_SDCKE   )            |
		((unsigned int) AT91C_PA26_NCS1_SDCS)           |
		((unsigned int) AT91C_PA27_SDWE    )            |
		((unsigned int) AT91C_PA28_CAS     )            |
		((unsigned int) AT91C_PA29_RAS     )
		); // Peripheral B

	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PB1_A1_NBS2  ) |
		((unsigned int) AT91C_PB16_A16_BA0 ) |
		((unsigned int) AT91C_PB0_A0_NBS0  ) |
		((unsigned int) AT91C_PB2_A2       ) |
		((unsigned int) AT91C_PB3_A3       ) |
		((unsigned int) AT91C_PB4_A4       ) |
		((unsigned int) AT91C_PB10_A10     ) |
		((unsigned int) AT91C_PB5_A5       ) |
		((unsigned int) AT91C_PB11_A11     ) |
		((unsigned int) AT91C_PB6_A6       ) |
		((unsigned int) AT91C_PB12_A12     ) |
		((unsigned int) AT91C_PB7_A7       ) |
		((unsigned int) AT91C_PB13_A13     ) |
		((unsigned int) AT91C_PB8_A8       ) |
		((unsigned int) AT91C_PB14_A14     ) |
		((unsigned int) AT91C_PB9_A9       ) |
		((unsigned int) AT91C_PB15_A15     ) |
		((unsigned int) AT91C_PB17_A17_BA1 )); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address

		((unsigned int) AT91C_PC10_D10 ) |
		((unsigned int) AT91C_PC11_D11 ) |
		((unsigned int) AT91C_PC12_D12 ) |
		((unsigned int) AT91C_PC13_D13 ) |
		((unsigned int) AT91C_PC14_D14 ) |
		((unsigned int) AT91C_PC15_D15 ) |
		((unsigned int) AT91C_PC0_D0   ) |
		((unsigned int) AT91C_PC1_D1   ) |
		((unsigned int) AT91C_PC2_D2   ) |
		((unsigned int) AT91C_PC3_D3   ) |
		((unsigned int) AT91C_PC4_D4   ) |
		((unsigned int) AT91C_PC5_D5   ) |
		((unsigned int) AT91C_PC6_D6   ) |
		((unsigned int) AT91C_PC7_D7   ) |
		((unsigned int) AT91C_PC8_D8   ) |
		((unsigned int) AT91C_PC9_D9   ) , // Peripheral A

      ((unsigned int) AT91C_PC21_NWR0_NWE_CFWE) | /*phlb patch for dm9000a*/
		((unsigned int) AT91C_PC22_NRD_CFOE) /*phlb patch for dm9000a*/
      ); // Peripheral B

}
//*----------------------------------------------------------------------------
//* \fn    AT91F_InitSdram
//* \brief Init EBI and SDRAM controler for MT48LC16M16A2
//*----------------------------------------------------------------------------
void AT91F_InitSdram (void)
{
	 volatile unsigned int i;
    AT91PS_SDRC	psdrc = AT91C_BASE_SDRC;

    // Init the EBI for SDRAM
    AT91C_BASE_EBI -> EBI_CSA =  AT91C_EBI_CS1A_SDRAMC;



	AT91F_EBI_SDRAM_CfgPIO();
    // Set Control Register
    psdrc->SDRC_CR =  AT91C_SDRC_NC_9         |  // 9  bits Column Addressing: 512 (A0-A8) AT91C_SDRC_NC_9
                      AT91C_SDRC_NR_13        |  // 13 bits Row Addressing     8K (A0-12)  AT91C_SDRC_NR_13
                      AT91C_SDRC_CAS_2        |  //  Check Table 8 for 7E(133) and 75(100) need CAS 2
                      AT91C_SDRC_NB_4_BANKS   |  // 4 banks
                      AT91C_SDRC_TWR_2        |
                      AT91C_SDRC_TRC_4        |
                      AT91C_SDRC_TRP_4        |
                      AT91C_SDRC_TRCD_2       |
                      AT91C_SDRC_TRAS_3       |
                      AT91C_SDRC_TXSR_4       ;
    // Wait time
	for (i =0; i< 1000;i++);

    // SDRAM initiazlization step
	psdrc->SDRC_MR = AT91C_SDRC_DBW_16_BITS | AT91C_SDRC_MODE_NOP_CMD;	// Set NOP
	*AT91C_SDRAM_BASE = 0x00000000;		                                // Perform NOP

	psdrc->SDRC_MR = AT91C_SDRC_DBW_16_BITS | 0x00000002;		        // Set PRCHG AL
	*AT91C_SDRAM_BASE	= 0x00000000;	                                // Perform PRCHG

    // Wait time
	for (i =0; i< 10000;i++);

	psdrc->SDRC_MR = AT91C_SDRC_DBW_16_BITS |AT91C_SDRC_MODE_RFSH_CMD;	// Set 1st CBR
    *AT91C_SDRAM_BASE = 0x00000000;	                                    // Perform CBR

	psdrc->SDRC_MR = AT91C_SDRC_DBW_16_BITS |AT91C_SDRC_MODE_RFSH_CMD;	// Set 2 CBR
    *AT91C_SDRAM_BASE = 0x00000000;	                                    // Perform CBR

	psdrc->SDRC_MR	= AT91C_SDRC_DBW_16_BITS |AT91C_SDRC_MODE_RFSH_CMD;	// Set 3 CBR
    *AT91C_SDRAM_BASE = 0x00000000;	                                    // Perform CBR

	psdrc->SDRC_MR	= AT91C_SDRC_DBW_16_BITS |AT91C_SDRC_MODE_RFSH_CMD;	// Set 4 CBR
    *AT91C_SDRAM_BASE = 0x00000000;	                                    // Perform CBR

	psdrc->SDRC_MR	= AT91C_SDRC_DBW_16_BITS |AT91C_SDRC_MODE_RFSH_CMD;	// Set 5 CBR
    *AT91C_SDRAM_BASE = 0x00000000;	                                    // Perform CBR

	psdrc->SDRC_MR	= AT91C_SDRC_DBW_16_BITS | AT91C_SDRC_MODE_RFSH_CMD;// Set 6 CBR
    *AT91C_SDRAM_BASE = 0x00000000;	                                    // Perform CBR

	psdrc->SDRC_MR	= AT91C_SDRC_DBW_16_BITS | AT91C_SDRC_MODE_RFSH_CMD;// Set 7 CBR
    *AT91C_SDRAM_BASE = 0x00000000;	                                    // Perform CBR

	psdrc->SDRC_MR	= AT91C_SDRC_DBW_16_BITS | AT91C_SDRC_MODE_RFSH_CMD;// Set 8 CBR
    *AT91C_SDRAM_BASE = 0x00000000;	                                    // Perform CBR

	psdrc->SDRC_MR	= AT91C_SDRC_DBW_16_BITS | AT91C_SDRC_MODE_LMR_CMD;	// Set LMR operation
	*(AT91C_SDRAM_BASE  + 20)= 0xcafedede;	                            // Perform LMR burst=1, lat=2

    // Set Refresh Timer
	psdrc->SDRC_TR	= AT91C_SDRC_TR_TIME;
	psdrc->SDRC_MR	= AT91C_SDRC_DBW_16_BITS ;    // Set Normal mode // 16 bits
	*AT91C_SDRAM_BASE= 0x00000000;	              // Perform Normal mode

   //to remove test just for init sdram
   for(i = 0; i < 0x100000; i++){
		*(AT91C_SDRAM_BASE + i) = 0x00000000;
	}
   
}

