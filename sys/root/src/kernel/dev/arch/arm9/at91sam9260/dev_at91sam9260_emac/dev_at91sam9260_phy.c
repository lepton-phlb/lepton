/*
The contents of this file are subject to the Mozilla Public License Version 1.1 
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis, 
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the 
specific language governing rights and limitations under the License.

The Original Code is ______________________________________.

The Initial Developer of the Original Code is ________________________.
Portions created by ______________________ are Copyright (C) ______ _______________________.
All Rights Reserved.

Contributor(s): ______________________________________.

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
| Includes    
==============================================*/

// Lepton
#include "kernel/core/types.h"
#include "kernel/core/interrupt.h"
#include "kernel/core/kernelconf.h"
#include "kernel/core/kernel.h"
#include "kernel/core/kernel_pthread.h"
#include "kernel/core/io.h"
#include "kernel/core/system.h"
#include "kernel/core/fcntl.h"
#include "lib/libc/termios/termios.h"
#include "kernel/fs/vfs/vfsdev.h"
#include "kernel/core/ioctl_eth.h"

// eCos
#include "cyg/hal/at91sam9260.h"
#include "cyg/hal/hal_platform_ints.h"
#include "pkgconf/hal_arm_at91sam9260.h"
#include <string.h>
#include <stdlib.h>

// drivers
#include "dev_at91sam9260_phy.h"
#include "dm9161_define.h"

/*============================================
| Global Declaration
==============================================*/


/// Default max retry count
#define DM9161_RETRY_MAX            100000
#define BOARD_EMAC_MODE_RMII        1

/*============================================
| Implementation
==============================================*/
static void dm9161_dump_registers(phy_stuff_t * pDm);


//-----------------------------------------------------------------------------
//          PHY management functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Wait PHY operation complete.
/// Return 1 if the operation completed successfully.
/// May be need to re-implemented to reduce CPU load.
/// \param retry: the retry times, 0 to wait forever until complete.
//-----------------------------------------------------------------------------
static unsigned char emac_wait_phy( unsigned int retry )
{
    unsigned int retry_count = 0;

    while((AT91C_BASE_EMACB->EMAC_NSR & AT91C_EMAC_IDLE) == 0) {
        // Dead LOOP!
        if (retry == 0) {
            continue;
        }
        // Timeout check
        retry_count++;
        if(retry_count >= retry) {
//            TRACE_ERROR("E: Wait PHY time out\n\r");
            return 0;
        }
    }
    return 1;
}

//-----------------------------------------------------------------------------
/// Set MDC clock according to current board clock. Per 802.3, MDC should be 
/// less then 2.5MHz.
/// Return 1 if successfully, 0 if MDC clock not found.
//-----------------------------------------------------------------------------
unsigned char emac_set_mdc_clock( unsigned int mck )
{
    int clock_dividor;

    if (mck <= 20000000) {
        clock_dividor = AT91C_EMAC_CLK_HCLK_8;          /// MDC clock = MCK/8
    } else if (mck <= 40000000) {
        clock_dividor = AT91C_EMAC_CLK_HCLK_16;         /// MDC clock = MCK/16
    } else if (mck <= 80000000) {
        clock_dividor = AT91C_EMAC_CLK_HCLK_32;         /// MDC clock = MCK/32
    } else if (mck <= 160000000) {
        clock_dividor = AT91C_EMAC_CLK_HCLK_64;         /// MDC clock = MCK/64
    } else {
      //  TRACE_ERROR("E: No valid MDC clock.\n\r");
        return 0;
    }
    AT91C_BASE_EMACB->EMAC_NCFGR = (AT91C_BASE_EMACB->EMAC_NCFGR & (~AT91C_EMAC_CLK))
                                 | clock_dividor;
    return 1;
}

//-----------------------------------------------------------------------------
/// Enable MDI with PHY
//-----------------------------------------------------------------------------
void emac_enable_mdio( void )
{
    AT91C_BASE_EMACB->EMAC_NCR |= AT91C_EMAC_MPE;
}

//-----------------------------------------------------------------------------
/// Enable MDI with PHY
//-----------------------------------------------------------------------------
void emac_disable_mdio( void )
{
    AT91C_BASE_EMACB->EMAC_NCR &= ~AT91C_EMAC_MPE;
}

//-----------------------------------------------------------------------------
/// Enable MII mode for EMAC, called once after autonegotiate
//-----------------------------------------------------------------------------
void emac_enable_mii( void )
{
    AT91C_BASE_EMACB->EMAC_USRIO = AT91C_EMAC_CLKEN;
}

//-----------------------------------------------------------------------------
/// Enable RMII mode for EMAC, called once after autonegotiate
//-----------------------------------------------------------------------------
void emac_enable_rmii( void )
{
    AT91C_BASE_EMACB->EMAC_USRIO = AT91C_EMAC_CLKEN | AT91C_EMAC_RMII;
}

//-----------------------------------------------------------------------------
/// Read PHY register.
/// Return 1 if successfully, 0 if timeout.
/// \param PhyAddress PHY Address
/// \param Address Register Address
/// \param pValue Pointer to a 32 bit location to store read data
/// \param retry The retry times, 0 to wait forever until complete.
//-----------------------------------------------------------------------------
unsigned char emac_read_phy(unsigned char PhyAddress,
                           unsigned char Address,
                           unsigned int *pValue,
                           unsigned int retry)
{
    AT91C_BASE_EMACB->EMAC_MAN = (AT91C_EMAC_SOF & (0x01 << 30))
                              | (AT91C_EMAC_CODE & (2 << 16))
                              | (AT91C_EMAC_RW & (2 << 28))
                              | (AT91C_EMAC_PHYA & ((PhyAddress & 0x1f) << 23))
                              | (AT91C_EMAC_REGA & (Address << 18));

    if ( emac_wait_phy(retry) == 0 ) {
//        TRACE_ERROR("TimeOut emac_read_phy\n\r");
        return 0;
    }
    *pValue = ( AT91C_BASE_EMACB->EMAC_MAN & 0x0000ffff );
    return 1;
}

//-----------------------------------------------------------------------------
/// Write PHY register
/// Return 1 if successfully, 0 if timeout.
/// \param PhyAddress PHY Address
/// \param Address Register Address
/// \param Value Data to write ( Actually 16 bit data )
/// \param retry The retry times, 0 to wait forever until complete.
//-----------------------------------------------------------------------------
unsigned char emac_write_phy(unsigned char PhyAddress,
                            unsigned char Address,
                            unsigned int  Value,
                            unsigned int  retry)
{
    AT91C_BASE_EMACB->EMAC_MAN = (AT91C_EMAC_SOF & (0x01 << 30))
                              | (AT91C_EMAC_CODE & (2 << 16))
                              | (AT91C_EMAC_RW & (1 << 28))
                              | (AT91C_EMAC_PHYA & ((PhyAddress & 0x1f) << 23))
                              | (AT91C_EMAC_REGA & (Address << 18))
                              | (AT91C_EMAC_DATA & Value) ;
    if ( emac_wait_phy(retry) == 0 ) {
//        TRACE_ERROR("TimeOut emac_write_phy\n\r");
        return 0;
    }
    return 1;
}

//-----------------------------------------------------------------------------
/// Setup the EMAC for the link : speed 100M/10M and Full/Half duplex
/// \param speed        Link speed, 0 for 10M, 1 for 100M
/// \param fullduplex   1 for Full Duplex mode
//-----------------------------------------------------------------------------
void emac_set_link_speed(unsigned char speed, unsigned char fullduplex)
{
    unsigned int ncfgr;

    ncfgr = AT91C_BASE_EMACB->EMAC_NCFGR;
    ncfgr &= ~(AT91C_EMAC_SPD | AT91C_EMAC_FD);
    if (speed) {
        ncfgr |= AT91C_EMAC_SPD;
    }
    if (fullduplex) {
        ncfgr |= AT91C_EMAC_FD;
    }
    AT91C_BASE_EMACB->EMAC_NCFGR = ncfgr;
}


//-----------------------------------------------------------------------------
/// Find a valid PHY Address ( from 0 to 31 ).
/// Check BMSR register ( not 0 nor 0xFFFF )
/// Return 0xFF when no valid PHY Address found.
/// \param pDm          Pointer to the Dm9161 instance
//-----------------------------------------------------------------------------
static unsigned char dm9161_find_valid_phy(phy_stuff_t * pDm)
{
    unsigned int  retryMax;
    unsigned int  value=0;
    unsigned char rc;
    unsigned char phyAddress;
    unsigned char cnt;

 //   TRACE_DEBUG("dm9161_find_valid_phy\n\r");
 //   ASSERT(pDm, "F: dm9161_find_valid_phy\n\r");

    emac_enable_mdio();
    phyAddress = pDm->phyAddress;
    retryMax = pDm->retryMax;

    // Check current phyAddress
    rc = phyAddress;
    if( emac_read_phy(phyAddress, DM9161_PHYID1, &value, retryMax) == 0 ) {
     //   TRACE_ERROR("DM9161 PROBLEM\n\r");
      rc = 0xFF;
    }
   // TRACE_DEBUG("_PHYID1  : 0x%X, addr: %d\n\r", value, phyAddress);

    // Find another one
    if (value != DM9161_OUI_MSB) {
        rc = 0xFF;
        for(cnt = 0; cnt < 32; cnt ++) {
            phyAddress = (phyAddress + 1) & 0x1F;
            if( emac_read_phy(phyAddress, DM9161_PHYID1, &value, retryMax) == 0 ) {
             //   TRACE_ERROR("DM9161 PROBLEM\n\r");
               rc = 0xFF;
            }
          //  TRACE_DEBUG("_PHYID1  : 0x%X, addr: %d\n\r", value, phyAddress);
            if (value == DM9161_OUI_MSB) {
                rc = phyAddress;
                break;
            }
        }
    }
    
    emac_disable_mdio();
    if (rc != 0xFF) {
        //TRACE_INFO("** Valid PHY Found: %d\n\r", rc);
        emac_read_phy(phyAddress, DM9161_DSCSR, &value, retryMax);
        //TRACE_DEBUG("_DSCSR  : 0x%X, addr: %d\n\r", value, phyAddress);
    }
    return rc;
}


//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Setup the maximum timeout count of the driver.
/// \param pDm   Pointer to the Dm9161 instance
/// \param toMax Timeout maxmum count.
//-----------------------------------------------------------------------------
void dm9161_setup_timeout(phy_stuff_t *pDm, unsigned int toMax)
{
 //   ASSERT(pDm, "-F- DM9161_SetupTimeout\n\r");
    pDm->retryMax = toMax;
}

//-----------------------------------------------------------------------------
/// Initialize the Dm9161 instance
/// \param pDm          Pointer to the Dm9161 instance
/// \param pEmac        Pointer to the Emac instance for the Dm9161
/// \param phyAddress   The PHY address used to access the PHY
///                     ( pre-defined by pin status on PHY reset )
//-----------------------------------------------------------------------------
void dm9161_init(phy_stuff_t *pDm, unsigned char phyAddress)
{
   // ASSERT(pDm , "-F- DM9161_Init\n\r");
    pDm->phyAddress = phyAddress;
    // Initialize timeout by default
    pDm->retryMax = DM9161_RETRY_MAX;
}


//-----------------------------------------------------------------------------
/// Issue a SW reset to reset all registers of the PHY
/// Return 1 if successfully, 0 if timeout.
/// \param pDm   Pointer to the Dm9161 instance
//-----------------------------------------------------------------------------
static unsigned char dm9161_reset_phy(phy_stuff_t *pDm)
{
    unsigned int retryMax;
    unsigned int bmcr = DM9161_RESET;
    unsigned char phyAddress;
    unsigned int timeout = 10;
    unsigned char ret = 1;

   // ASSERT(pDm, "-F- DM9161_ResetPhy");
   // TRACE_INFO(" DM9161_ResetPhy\n\r");

    phyAddress = pDm->phyAddress;
    retryMax = pDm->retryMax;

    emac_enable_mdio();
    bmcr = DM9161_RESET;
    emac_write_phy(phyAddress, DM9161_BMCR, bmcr, retryMax);

    do {
        emac_read_phy(phyAddress, DM9161_BMCR, &bmcr, retryMax);
        timeout--;
    } while ((bmcr & DM9161_RESET) && timeout);

    emac_disable_mdio();

    if (!timeout) {
        ret = 0;
    }

    return( ret );
}

#define CONF_PIOA_PERIPH_A(mask) \
   AT91C_BASE_PIOA->PIO_ASR = mask; \
   AT91C_BASE_PIOA->PIO_PDR = mask

#define CONF_PIOA_PERIPH_B(mask) \
   AT91C_BASE_PIOA->PIO_BSR = mask; \
   AT91C_BASE_PIOA->PIO_PDR = mask

#define CONF_PIOA_OUTPUT1_PIN(mask) \
   AT91C_BASE_PIOA->PIO_SODR = mask; \
   AT91C_BASE_PIOA->PIO_IDR = mask; \
   AT91C_BASE_PIOA->PIO_OER = mask; \
   AT91C_BASE_PIOA->PIO_PER = mask

#define CONF_PIOA_OUTPUT0_PIN(mask) \
   AT91C_BASE_PIOA->PIO_CODR = mask; \
   AT91C_BASE_PIOA->PIO_IDR = mask; \
   AT91C_BASE_PIOA->PIO_OER = mask; \
   AT91C_BASE_PIOA->PIO_PER = mask

// PINS to run EMAC+PHY in RMII mode
void emac_run_pins(void)
{
   CONF_PIOA_PERIPH_B(AT91C_PA28_ECRS);
   CONF_PIOA_PERIPH_B(AT91C_PA29_ECOL);

   // Rx
   CONF_PIOA_PERIPH_A(AT91C_PA17_ERXDV);
   CONF_PIOA_PERIPH_A(AT91C_PA14_ERX0);
   CONF_PIOA_PERIPH_A(AT91C_PA15_ERX1);
   CONF_PIOA_PERIPH_B(AT91C_PA25_ERX2);
   CONF_PIOA_PERIPH_B(AT91C_PA26_ERX3);
   CONF_PIOA_PERIPH_A(AT91C_PA18_ERXER);
   CONF_PIOA_PERIPH_B(AT91C_PA27_ERXCK);

   // Tx
   CONF_PIOA_PERIPH_A(AT91C_PA16_ETXEN);
   CONF_PIOA_PERIPH_A(AT91C_PA12_ETX0);
   CONF_PIOA_PERIPH_A(AT91C_PA13_ETX1);
   CONF_PIOA_PERIPH_B(AT91C_PA10_ETX2_0);
   CONF_PIOA_PERIPH_B(AT91C_PA11_ETX3_0);
   CONF_PIOA_PERIPH_B(AT91C_PA22_ETXER);
   CONF_PIOA_PERIPH_A(AT91C_PA19_ETXCK);

   CONF_PIOA_PERIPH_A(AT91C_PA20_EMDC);
   CONF_PIOA_PERIPH_A(AT91C_PA21_EMDIO);
  // CONF_PIOA_PERIPH_A(AT91C_PA7_MCCDA);
}

void emac_reset_pins(void)
{
   CONF_PIOA_OUTPUT0_PIN(AT91C_PA17_ERXDV); // TEST
   CONF_PIOA_OUTPUT1_PIN(AT91C_PA29_ECOL); // RMII

   // PHYAD
   CONF_PIOA_OUTPUT1_PIN(AT91C_PA14_ERX0);
   CONF_PIOA_OUTPUT1_PIN(AT91C_PA15_ERX1);
   CONF_PIOA_OUTPUT0_PIN(AT91C_PA25_ERX2);
   CONF_PIOA_OUTPUT0_PIN(AT91C_PA26_ERX3);
   CONF_PIOA_OUTPUT0_PIN(AT91C_PA28_ECRS);

   CONF_PIOA_OUTPUT0_PIN(AT91C_PA27_ERXCK); // 10BT
   CONF_PIOA_OUTPUT0_PIN(AT91C_PA18_ERXER); // RPTR
}

#define RSTC_KEY_PASSWORD       (0xA5 << 24)

//-----------------------------------------------------------------------------
/// Setup the external reset length. The length is asserted during a time of
/// pow(2, powl+1) Slow Clock(32KHz). The duration is between 60us and 2s.
/// \param powl   Power length defined.
//-----------------------------------------------------------------------------
void RSTC_SetExtResetLength(unsigned char powl)
{
    unsigned int rmr = AT91C_BASE_RSTC->RSTC_RMR;
    rmr &= ~(AT91C_RSTC_KEY | AT91C_RSTC_ERSTL);
    rmr |=  (powl << 8) & AT91C_RSTC_ERSTL;
    AT91C_BASE_RSTC->RSTC_RMR = rmr | RSTC_KEY_PASSWORD;
}

//-----------------------------------------------------------------------------
/// Asserts the NRST pin for external resets.
//-----------------------------------------------------------------------------
void RSTC_ExtReset(void)
{
    AT91C_BASE_RSTC->RSTC_RCR = AT91C_RSTC_EXTRST | RSTC_KEY_PASSWORD;
}

//-----------------------------------------------------------------------------
/// Return NRST pin level ( 1 or 0 ).
//-----------------------------------------------------------------------------
unsigned char RSTC_GetNrstLevel(void)
{
    if (AT91C_BASE_RSTC->RSTC_RSR & AT91C_RSTC_NRSTL) {

        return 1;
    }
    return 0;
}

//-----------------------------------------------------------------------------
/// Do a HW initialize to the PHY ( via RSTC ) and setup clocks & PIOs
/// This should be called only once to initialize the PHY pre-settings.
/// The PHY address is reset status of CRS,RXD[3:0] (the emacPins' pullups).
/// The COL pin is used to select MII mode on reset (pulled up for Reduced MII)
/// The RXDV pin is used to select test mode on reset (pulled up for test mode)
/// The above pins should be predefined for corresponding settings in resetPins
/// The EMAC peripheral pins are configured after the reset done.
/// Return 1 if RESET OK, 0 if timeout.
/// \param pDm         Pointer to the Dm9161 instance
/// \param mck         Main clock setting to initialize clock
/// \param resetPins   Pointer to list of PIOs to configure before HW RESET
///                       (for PHY power on reset configuration latch)
/// \param nbResetPins Number of PIO items that should be configured
/// \param emacPins    Pointer to list of PIOs for the EMAC interface
/// \param nbEmacPins  Number of PIO items that should be configured
//-----------------------------------------------------------------------------
unsigned char dm9161_init_phy(phy_stuff_t *pDm)
{
   unsigned char rc = 1;
   unsigned char phy;
   volatile int k;

   // ASSERT(pDm, "-F- DM9161_InitPhy\n\r");

    // Perform RESET
  //  TRACE_DEBUG("RESET PHY\n\r");

//    if (pResetPins) {
        // Configure PINS
  //      PIO_Configure(pResetPins, nbResetPins);
   emac_reset_pins();

   // Execute reset
   RSTC_SetExtResetLength(DM9161_RESET_LENGTH);
   RSTC_ExtReset();
   // Wait for end hardware reset
   while (!RSTC_GetNrstLevel());
//    }

   

  // for(k=0;k<1000;k++);

   // Configure EMAC runtime pins
   if (rc) {
   //     PIO_Configure(pEmacPins, nbEmacPins);
      emac_run_pins();
      rc = emac_set_mdc_clock( CYGNUM_HAL_ARM_AT91SAM9260_CLOCK_SPEED );
      if (!rc) {
         //TRACE_ERROR("No Valid MDC clock\n\r");
         return 0;
      }
      // Check PHY Address
      phy = dm9161_find_valid_phy(pDm);
      if (phy == 0xFF) {
         // TRACE_ERROR("PHY Access fail\n\r");
         return 0;
      }
      if(phy != pDm->phyAddress) {
         pDm->phyAddress = phy;
         dm9161_reset_phy(pDm);
      }
   } else {
   //   TRACE_ERROR("PHY Reset Timeout\n\r");
   }

   return rc;
}

//-----------------------------------------------------------------------------
/// Issue a Auto Negotiation of the PHY
/// Return 1 if successfully, 0 if timeout.
/// \param pDm   Pointer to the Dm9161 instance
//-----------------------------------------------------------------------------
unsigned char dm9161_auto_negotiate(phy_stuff_t *pDm)
{
    unsigned int retryMax;
    unsigned int value;
    unsigned int phyAnar;
    unsigned int phyAnalpar;
    unsigned int retryCount= 0;
    unsigned char phyAddress;
    unsigned char rc = 1;

//    ASSERT(pDm, "-F- DM9161_AutoNegotiate\n\r");
    phyAddress = pDm->phyAddress;
    retryMax = pDm->retryMax;

   dm9161_dump_registers(pDm);

    emac_enable_mdio();

    if (!emac_read_phy(phyAddress, DM9161_PHYID1, &value, retryMax)) {
  //      TRACE_ERROR("Pb emac_read_phy Id1\n\r");
        rc = 0;
        goto AutoNegotiateExit;
    }
   // TRACE_DEBUG("ReadPhy Id1 0x%X, addresse: %d\n\r", value, phyAddress);
    if (!emac_read_phy(phyAddress, DM9161_PHYID2, &phyAnar, retryMax)) {
     //   TRACE_ERROR("Pb emac_read_phy Id2\n\r");
        rc = 0;
        goto AutoNegotiateExit;
    }
    //TRACE_DEBUG("ReadPhy Id2 0x%X\n\r", phyAnar);

    if( ( value == DM9161_OUI_MSB )
     && ( ((phyAnar>>10)&DM9161_LSB_MASK) == DM9161_OUI_LSB ) ) {

      //  TRACE_DEBUG("Vendor Number Model = 0x%X\n\r", ((phyAnar>>4)&0x3F));
      //  TRACE_DEBUG("Model Revision Number = 0x%X\n\r", (phyAnar&0x7));
    } else {
      //  TRACE_ERROR("Problem OUI value\n\r");
    }        

    // Setup control register
    rc  = emac_read_phy(phyAddress, DM9161_BMCR, &value, retryMax);
    if (rc == 0) {
        goto AutoNegotiateExit;
    }

    value &= ~DM9161_AUTONEG;   // Remove autonegotiation enable
    value &= ~(DM9161_LOOPBACK|DM9161_POWER_DOWN);
    value |=  DM9161_ISOLATE;   // Electrically isolate PHY
    rc = emac_write_phy(phyAddress, DM9161_BMCR, value, retryMax);
    if (rc == 0) {
        goto AutoNegotiateExit;
    }

    // Set the Auto_negotiation Advertisement Register
    // MII advertising for Next page
    // 100BaseTxFD and HD, 10BaseTFD and HD, IEEE 802.3
    phyAnar = DM9161_NP | DM9161_TX_FDX | DM9161_TX_HDX |
              DM9161_10_FDX | DM9161_10_HDX | DM9161_AN_IEEE_802_3;
    rc = emac_write_phy(phyAddress, DM9161_ANAR, phyAnar, retryMax);
    if (rc == 0) {
        goto AutoNegotiateExit;
    }

    // Read & modify control register
    rc  = emac_read_phy(phyAddress, DM9161_BMCR, &value, retryMax);
    if (rc == 0) {
        goto AutoNegotiateExit;
    }

    value |= DM9161_SPEED_SELECT | DM9161_AUTONEG | DM9161_DUPLEX_MODE;
    rc = emac_write_phy(phyAddress, DM9161_BMCR, value, retryMax);
    if (rc == 0) {
        goto AutoNegotiateExit;
    }

    // Restart Auto_negotiation
    value |=  DM9161_RESTART_AUTONEG;
    value &= ~DM9161_ISOLATE;
    rc = emac_write_phy(phyAddress, DM9161_BMCR, value, retryMax);
    if (rc == 0) {
        goto AutoNegotiateExit;
    }
    //TRACE_DEBUG(" _BMCR: 0x%X\n\r", value);

    // Check AutoNegotiate complete
    while (1) {
        rc  = emac_read_phy(phyAddress, DM9161_BMSR, &value, retryMax);
        if (rc == 0) {
//            TRACE_ERROR("rc==0\n\r");
            goto AutoNegotiateExit;
        }
        // Done successfully
        if (value & DM9161_AUTONEG_COMP) {
//            TRACE_INFO("AutoNegotiate complete\n\r");
            break;
        }
        // Timeout check
        if (retryMax) {
            if (++ retryCount >= retryMax) {
             //   DM9161_DumpRegisters(pDm);
  //              TRACE_FATAL("TimeOut\n\r");
                goto AutoNegotiateExit;
            }
        }
    }

    // Get the AutoNeg Link partner base page
    rc  = emac_read_phy(phyAddress, DM9161_ANLPAR, &phyAnalpar, retryMax);
    if (rc == 0) {
        goto AutoNegotiateExit;
    }

    // Setup the EMAC link speed
    if ((phyAnar & phyAnalpar) & DM9161_TX_FDX) {
        // set MII for 100BaseTX and Full Duplex
        emac_set_link_speed(1, 1);
    } else if ((phyAnar & phyAnalpar) & DM9161_10_FDX) {
        // set MII for 10BaseT and Full Duplex
        emac_set_link_speed(0, 1);
    } else if ((phyAnar & phyAnalpar) & DM9161_TX_HDX) {
        // set MII for 100BaseTX and half Duplex
        emac_set_link_speed(1, 0);
    } else if ((phyAnar & phyAnalpar) & DM9161_10_HDX) {
        // set MII for 10BaseT and half Duplex
        emac_set_link_speed(0, 0);
    }

    // Setup EMAC mode
#if BOARD_EMAC_MODE_RMII != 1
    emac_enable_mii();
#else
    emac_enable_rmii();
#endif

AutoNegotiateExit:
    emac_disable_mdio();
    return rc;
}

//-----------------------------------------------------------------------------
/// Get the Link & speed settings, and automatically setup the EMAC with the
/// settings.
/// Return 1 if link found, 0 if no ethernet link.
/// \param pDm          Pointer to the Dm9161 instance
/// \param applySetting Apply the settings to EMAC interface
//-----------------------------------------------------------------------------
unsigned char dm9161_get_link_speed(phy_stuff_t *pDm, unsigned char applySetting)
{
    unsigned int retryMax;
    unsigned int stat1;
    unsigned int stat2;
    unsigned char phyAddress;
    unsigned char rc = 1;

 //   TRACE_DEBUG("DM9161_GetLinkSpeed\n\r");
 //   ASSERT(pDm, "-F- DM9161_GetLinkSpeed\n\r");

    emac_enable_mdio();
    phyAddress = pDm->phyAddress;
    retryMax = pDm->retryMax;

    rc  = emac_read_phy(phyAddress, DM9161_BMSR, &stat1, retryMax);
    if (rc == 0) {
        goto GetLinkSpeedExit;
    }
    if ((stat1 & DM9161_LINK_STATUS) == 0) {
//        TRACE_ERROR("Pb: LinkStat: 0x%x\n\r", stat1);
        rc = 0;
        goto GetLinkSpeedExit;
    }
    if (applySetting == 0) {
//        TRACE_ERROR("Pb: applySetting: 0x%x\n\r", applySetting);
        goto GetLinkSpeedExit;
    }
    // Re-configure Link speed
    rc  = emac_read_phy(phyAddress, DM9161_DSCSR, &stat2, retryMax);
    if (rc == 0) {
//        TRACE_ERROR("Pb: rc: 0x%x\n\r", rc);
        goto GetLinkSpeedExit;
    }
    if ((stat1 & DM9161_100BASE_TX_FD) && (stat2 & DM9161_100FDX)) {
        // set Emac for 100BaseTX and Full Duplex
        emac_set_link_speed(1, 1);
    }
    if ((stat1 & DM9161_10BASE_T_FD) && (stat2 & DM9161_10FDX)) {
        // set MII for 10BaseT and Full Duplex
        emac_set_link_speed(0, 1);
    }
    if ((stat1 & DM9161_100BASE_T4_HD) && (stat2 & DM9161_100HDX)) {
        // set MII for 100BaseTX and Half Duplex
        emac_set_link_speed(1, 0);
    }
    if ((stat1 & DM9161_10BASE_T_HD) && (stat2 & DM9161_10HDX)) {
        // set MII for 10BaseT and Half Duplex
        emac_set_link_speed(0, 0);
    }

    // Start the EMAC transfers
  //  TRACE_DEBUG("DM9161_GetLinkSpeed passed\n\r");

GetLinkSpeedExit:
    emac_disable_mdio();
    return rc;
}

static void dm9161_dump_registers(phy_stuff_t * pDm)
{
    unsigned char phyAddress;
    unsigned int retryMax;
    unsigned int value;

 //   TRACE_INFO("DM9161_DumpRegisters\n\r");
 //   ASSERT(pDm, "F: DM9161_DumpRegisters\n\r");

    emac_enable_mdio();
    phyAddress = pDm->phyAddress;
    retryMax = pDm->retryMax;

//    TRACE_INFO("DM9161 (%d) Registers:\n\r", phyAddress);

    emac_read_phy(phyAddress, DM9161_BMCR, &value, retryMax);
//    TRACE_INFO(" _BMCR   : 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_BMSR, &value, retryMax);
//    TRACE_INFO(" _BMSR   : 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_ANAR, &value, retryMax);
//    TRACE_INFO(" _ANAR   : 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_ANLPAR, &value, retryMax);
//    TRACE_INFO(" _ANLPAR : 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_ANER, &value, retryMax);
//    TRACE_INFO(" _ANER   : 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_DSCR, &value, retryMax);
//    TRACE_INFO(" _DSCR   : 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_DSCSR, &value, retryMax);
 //   TRACE_INFO(" _DSCSR  : 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_10BTCSR, &value, retryMax);
 //   TRACE_INFO(" _10BTCSR: 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_PWDOR, &value, retryMax);
 //   TRACE_INFO(" _PWDOR  : 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_CONFIGR, &value, retryMax);
 //   TRACE_INFO(" _CONFIGR: 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_MDINTR, &value, retryMax);
 //   TRACE_INFO(" _MDINTR : 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_RECR, &value, retryMax);
 //   TRACE_INFO(" _RECR   : 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_DISCR, &value, retryMax);
 //   TRACE_INFO(" _DISCR  : 0x%X\n\r", value);
    emac_read_phy(phyAddress, DM9161_RLSR, &value, retryMax);
 //   TRACE_INFO(" _RLSR   : 0x%X\n\r", value);

    emac_disable_mdio();
}

/*============================================
| End of Source  : dev_at91sam9260_phy.c
==============================================*/

