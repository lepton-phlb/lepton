//* ----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//* ----------------------------------------------------------------------------
//* DISCLAIMER:  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
//* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
//* DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
//* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
//* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//* ----------------------------------------------------------------------------
//* File Name           : lib_AT91SAM9261.h
//* Object              : AT91SAM9261 inlined functions
//* Generated           : AT91 SW Application Group  09/12/2005 (15:39:27)
//*
//* CVS Reference       : /lib_pdc.h/1.2/Tue Jul  2 12:29:40 2002//
//* CVS Reference       : /lib_dbgu.h/1.1/Fri Jan 31 12:18:40 2003//
//* CVS Reference       : /lib_shdwc_6122A.h/1.1/Wed Oct  6 13:06:00 2004//
//* CVS Reference       : /lib_spi2.h/1.2/Tue Aug 23 15:43:14 2005//
//* CVS Reference       : /lib_ssc.h/1.4/Fri Jan 31 12:19:20 2003//
//* CVS Reference       : /lib_LCDC_DIMITRI.h/1.2/Thu Jan 23 10:22:25 2003//
//* CVS Reference       : /lib_tc_1753b.h/1.1/Fri Jan 31 12:20:02 2003//
//* CVS Reference       : /lib_pitc_6079A.h/1.2/Thu Nov  4 14:01:11 2004//
//* CVS Reference       : /lib_hmatrix1_SAM9261.h/1.2/Mon Nov  8 16:38:17 2004//
//* CVS Reference       : /lib_pmc_SAM9261.h/1.1/Mon May 10 12:28:56 2004//
//* CVS Reference       : /lib_rstc_6098A.h/1.1/Wed Oct  6 09:39:19 2004//
//* CVS Reference       : /lib_pio.h/1.3/Fri Jan 31 12:18:56 2003//
//* CVS Reference       : /lib_rttc_6081A.h/1.1/Wed Oct  6 09:39:38 2004//
//* CVS Reference       : /lib_twi.h/1.3/Mon Jul 19 13:37:30 2004//
//* CVS Reference       : /lib_usart.h/1.5/Thu Nov 21 16:01:53 2002//
//* CVS Reference       : /lib_mci.h/1.7/Thu May 27 08:47:43 2004//
//* CVS Reference       : /lib_wdtc_6080A.h/1.1/Wed Oct  6 09:38:30 2004//
//* CVS Reference       : /lib_udp.h/1.5/Mon Aug 29 08:17:54 2005//
//* CVS Reference       : /lib_aic.h/1.3/Fri Jul 12 07:46:11 2002//
//* ----------------------------------------------------------------------------
#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_if.h>             // calling interface



/* *****************************************************************************
                SOFTWARE API FOR MATRIX
   ***************************************************************************** */


//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_Set_TCM_Size
//* \brief Enable one or more TCM block
//*----------------------------------------------------------------------------
void AT91F_MATRIX_Set_TCM_Size (
        AT91PS_MATRIX pMATRIX,
        unsigned int tcm_size)
{
    pMATRIX->MATRIX_TCMR = tcm_size;
}


//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_Remap_ARM
//* \brief Enable Remap of ARM
//*----------------------------------------------------------------------------
void AT91F_MATRIX_Remap_ARM (
        AT91PS_MATRIX pMATRIX)
{
    pMATRIX->MATRIX_MCFG = AT91C_MATRIX_RCA926I | AT91C_MATRIX_RCA926D;
}


//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_Disable_Remap_ARM
//* \brief Disable Remap of ARM
//*----------------------------------------------------------------------------
void AT91F_MATRIX_Disable_Remap_ARM (
        AT91PS_MATRIX pMATRIX)
{
    pMATRIX->MATRIX_MCFG = AT91C_MATRIX_RCA926I | AT91C_MATRIX_RCA926D;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_Remap_ARMI
//* \brief Enable Remap of ARM-I
//*----------------------------------------------------------------------------
void AT91F_MATRIX_Remap_ARMI (
        AT91PS_MATRIX pMATRIX)
{
    pMATRIX->MATRIX_MCFG = AT91C_MATRIX_RCA926I;
}


//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_Disable_Remap_ARMI
//* \brief Disable Remap of ARM-I
//*----------------------------------------------------------------------------
void AT91F_MATRIX_Disable_Remap_ARMI (
        AT91PS_MATRIX pMATRIX)
{
    pMATRIX->MATRIX_MCFG = AT91C_MATRIX_RCA926I;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_Remap_ARMD
//* \brief Enable Remap of ARM-D
//*----------------------------------------------------------------------------
void AT91F_MATRIX_Remap_ARMD (
        AT91PS_MATRIX pMATRIX)
{
    pMATRIX->MATRIX_MCFG = AT91C_MATRIX_RCA926D;
}


//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_Disable_Remap_ARM-D
//* \brief Disable Remap of ARM-D
//*----------------------------------------------------------------------------
void AT91F_MATRIX_Disable_Remap_ARMD (
        AT91PS_MATRIX pMATRIX)
{
    pMATRIX->MATRIX_MCFG = AT91C_MATRIX_RCA926D;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_EnableSDRAMC
//* \brief Enable the Chip Select of the SDRAM Controller
//*----------------------------------------------------------------------------
void AT91F_MATRIX_EnableSDRAMC (
        AT91PS_MATRIX pMATRIX)                        // \arg pointer to the MATRIX registers
         
{
    pMATRIX->MATRIX_EBICSA |= AT91C_MATRIX_CS1A_SDRAMC;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_DisableSDRAMC
//* \brief Disable the Chip Select of the SDRAM Controller
//*----------------------------------------------------------------------------
void AT91F_MATRIX_DisableSDRAMC (
        AT91PS_MATRIX pMATRIX)                        // \arg pointer to the MATRIX registers
         
{
    pMATRIX->MATRIX_EBICSA &= ~AT91C_MATRIX_CS1A_SDRAMC;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_EnableSM_On_CS3
//* \brief Enable the Chip Select 3 for CF Controller
//*----------------------------------------------------------------------------
void AT91F_MATRIX_EnableSM_On_CS3 (
        AT91PS_MATRIX pMATRIX)                        // \arg pointer to the MATRIX registers
         
{
    pMATRIX->MATRIX_EBICSA |= AT91C_MATRIX_CS3A_SM;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_DisableSM_On_CS3
//* \brief Disable the Chip Select 3 for CF Controller
//*----------------------------------------------------------------------------
void AT91F_MATRIX_DisableSM_On_CS3 (
        AT91PS_MATRIX pMATRIX)                        // \arg pointer to the MATRIX registers
         
{
    pMATRIX->MATRIX_EBICSA &= ~AT91C_MATRIX_CS3A_SM;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_EnableCF_On_CS4
//* \brief Enable the Chip Select 4 for CF Controller
//*----------------------------------------------------------------------------
void AT91F_MATRIX_EnableCF_On_CS4 (
        AT91PS_MATRIX pMATRIX)                        // \arg pointer to the MATRIX registers
         
{
    pMATRIX->MATRIX_EBICSA |= AT91C_MATRIX_CS4A_CF;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_DisableCF_On_CS4
//* \brief Disable the Chip Select 4 for CF Controller
//*----------------------------------------------------------------------------
void AT91F_MATRIX_DisableCF_On_CS4 (
        AT91PS_MATRIX pMATRIX)                        // \arg pointer to the MATRIX registers
         
{
    pMATRIX->MATRIX_EBICSA &= ~AT91C_MATRIX_CS4A_CF;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_EnableCF_On_CS5
//* \brief Enable the Chip Select 5 for CF Controller
//*----------------------------------------------------------------------------
void AT91F_MATRIX_EnableCF_On_CS5 (
        AT91PS_MATRIX pMATRIX)                        // \arg pointer to the MATRIX registers
         
{
    pMATRIX->MATRIX_EBICSA |= AT91C_MATRIX_CS5A_CF;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_DisableCF_On_CS5
//* \brief Disable the Chip Select 5 for CF Controller
//*----------------------------------------------------------------------------
void AT91F_MATRIX_DisableCF_On_CS5 (
        AT91PS_MATRIX pMATRIX)                        // \arg pointer to the MATRIX registers
         
{
    pMATRIX->MATRIX_EBICSA &= ~AT91C_MATRIX_CS5A_CF;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_EnableEBIPULLUP
//* \brief Enable the EBI PULLUP
//*----------------------------------------------------------------------------
void AT91F_MATRIX_EnableEBIPULLUP (
        AT91PS_MATRIX pMATRIX)                        // \arg pointer to the MATRIX registers
         
{
    pMATRIX->MATRIX_EBICSA |= AT91C_MATRIX_DBPUC;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MATRIX_DisableEBIPULLUP
//* \brief Disable the EBI PULLUP
//*----------------------------------------------------------------------------
void AT91F_MATRIX_DisableEBIPULLUP (
        AT91PS_MATRIX pMATRIX)                        // \arg pointer to the MATRIX registers
         
{
    pMATRIX->MATRIX_EBICSA &= ~AT91C_MATRIX_DBPUC;
}



/* *****************************************************************************
                SOFTWARE API FOR AIC
   ***************************************************************************** */
#define AT91C_AIC_BRANCH_OPCODE ((void (*) ()) 0xE51FFF20) // ldr, pc, [pc, #-&F20]

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_ConfigureIt
//* \brief Interrupt Handler Initialization
//*----------------------------------------------------------------------------
unsigned int AT91F_AIC_ConfigureIt (
	AT91PS_AIC pAic,  // \arg pointer to the AIC registers
	unsigned int irq_id,     // \arg interrupt number to initialize
	unsigned int priority,   // \arg priority to give to the interrupt
	unsigned int src_type,   // \arg activation and sense of activation
	void (*newHandler) (void) ) // \arg address of the interrupt handler
{
	unsigned int oldHandler;
    unsigned int mask ;

    oldHandler = pAic->AIC_SVR[irq_id];

    mask = 0x1 << irq_id ;
    //* Disable the interrupt on the interrupt controller
    pAic->AIC_IDCR = mask ;
    //* Save the interrupt handler routine pointer and the interrupt priority
    pAic->AIC_SVR[irq_id] = (unsigned int) newHandler ;
    //* Store the Source Mode Register
    pAic->AIC_SMR[irq_id] = src_type | priority  ;
    //* Clear the interrupt on the interrupt controller
    pAic->AIC_ICCR = mask ;

	return oldHandler;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_EnableIt
//* \brief Enable corresponding IT number
//*----------------------------------------------------------------------------
void AT91F_AIC_EnableIt (
	AT91PS_AIC pAic,      // \arg pointer to the AIC registers
	unsigned int irq_id ) // \arg interrupt number to initialize
{
    //* Enable the interrupt on the interrupt controller
    pAic->AIC_IECR = 0x1 << irq_id ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_DisableIt
//* \brief Disable corresponding IT number
//*----------------------------------------------------------------------------
void AT91F_AIC_DisableIt (
	AT91PS_AIC pAic,      // \arg pointer to the AIC registers
	unsigned int irq_id ) // \arg interrupt number to initialize
{
    unsigned int mask = 0x1 << irq_id;
    //* Disable the interrupt on the interrupt controller
    pAic->AIC_IDCR = mask ;
    //* Clear the interrupt on the Interrupt Controller ( if one is pending )
    pAic->AIC_ICCR = mask ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_ClearIt
//* \brief Clear corresponding IT number
//*----------------------------------------------------------------------------
void AT91F_AIC_ClearIt (
	AT91PS_AIC pAic,     // \arg pointer to the AIC registers
	unsigned int irq_id) // \arg interrupt number to initialize
{
    //* Clear the interrupt on the Interrupt Controller ( if one is pending )
    pAic->AIC_ICCR = (0x1 << irq_id);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_AcknowledgeIt
//* \brief Acknowledge corresponding IT number
//*----------------------------------------------------------------------------
void AT91F_AIC_AcknowledgeIt (
	AT91PS_AIC pAic)     // \arg pointer to the AIC registers
{
    pAic->AIC_EOICR = pAic->AIC_EOICR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_SetExceptionVector
//* \brief Configure vector handler
//*----------------------------------------------------------------------------
unsigned int  AT91F_AIC_SetExceptionVector (
	unsigned int *pVector, // \arg pointer to the AIC registers
	void (*Handler) () )   // \arg Interrupt Handler
{
	unsigned int oldVector = *pVector;

	if ((unsigned int) Handler == (unsigned int) AT91C_AIC_BRANCH_OPCODE)
		*pVector = (unsigned int) AT91C_AIC_BRANCH_OPCODE;
	else
		*pVector = (((((unsigned int) Handler) - ((unsigned int) pVector) - 0x8) >> 2) & 0x00FFFFFF) | 0xEA000000;

	return oldVector;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_Trig
//* \brief Trig an IT
//*----------------------------------------------------------------------------
void  AT91F_AIC_Trig (
	AT91PS_AIC pAic,     // \arg pointer to the AIC registers
	unsigned int irq_id) // \arg interrupt number
{
	pAic->AIC_ISCR = (0x1 << irq_id) ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_IsActive
//* \brief Test if an IT is active
//*----------------------------------------------------------------------------
unsigned int  AT91F_AIC_IsActive (
	AT91PS_AIC pAic,     // \arg pointer to the AIC registers
	unsigned int irq_id) // \arg Interrupt Number
{
	return (pAic->AIC_ISR & (0x1 << irq_id));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_IsPending
//* \brief Test if an IT is pending
//*----------------------------------------------------------------------------
unsigned int  AT91F_AIC_IsPending (
	AT91PS_AIC pAic,     // \arg pointer to the AIC registers
	unsigned int irq_id) // \arg Interrupt Number
{
	return (pAic->AIC_IPR & (0x1 << irq_id));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_Open
//* \brief Set exception vectors and AIC registers to default values
//*----------------------------------------------------------------------------
void AT91F_AIC_Open(
	AT91PS_AIC pAic,        // \arg pointer to the AIC registers
	void (*IrqHandler) (),  // \arg Default IRQ vector exception
	void (*FiqHandler) (),  // \arg Default FIQ vector exception
	void (*DefaultHandler)  (), // \arg Default Handler set in ISR
	void (*SpuriousHandler) (), // \arg Default Spurious Handler
	unsigned int protectMode)   // \arg Debug Control Register
{
	int i;

	// Disable all interrupts and set IVR to the default handler
	for (i = 0; i < 32; ++i) {
		AT91F_AIC_DisableIt(pAic, i);
		AT91F_AIC_ConfigureIt(pAic, i, AT91C_AIC_PRIOR_LOWEST, AT91C_AIC_SRCTYPE_INT_LEVEL_SENSITIVE, DefaultHandler);
	}

	// Set the IRQ exception vector
	AT91F_AIC_SetExceptionVector((unsigned int *) 0x18, IrqHandler);
	// Set the Fast Interrupt exception vector
	AT91F_AIC_SetExceptionVector((unsigned int *) 0x1C, FiqHandler);

	pAic->AIC_SPU = (unsigned int) SpuriousHandler;
	pAic->AIC_DCR = protectMode;
}
/* *****************************************************************************
                SOFTWARE API FOR PDC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetNextRx
//* \brief Set the next receive transfer descriptor
//*----------------------------------------------------------------------------
void AT91F_PDC_SetNextRx (
	AT91PS_PDC pPDC,     // \arg pointer to a PDC controller
	char *address,       // \arg address to the next bloc to be received
	unsigned int bytes)  // \arg number of bytes to be received
{
	pPDC->PDC_RNPR = (unsigned int) address;
	pPDC->PDC_RNCR = bytes;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetNextTx
//* \brief Set the next transmit transfer descriptor
//*----------------------------------------------------------------------------
void AT91F_PDC_SetNextTx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be transmitted
	unsigned int bytes)    // \arg number of bytes to be transmitted
{
	pPDC->PDC_TNPR = (unsigned int) address;
	pPDC->PDC_TNCR = bytes;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetRx
//* \brief Set the receive transfer descriptor
//*----------------------------------------------------------------------------
void AT91F_PDC_SetRx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be received
	unsigned int bytes)    // \arg number of bytes to be received
{
	pPDC->PDC_RPR = (unsigned int) address;
	pPDC->PDC_RCR = bytes;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SetTx
//* \brief Set the transmit transfer descriptor
//*----------------------------------------------------------------------------
void AT91F_PDC_SetTx (
	AT91PS_PDC pPDC,       // \arg pointer to a PDC controller
	char *address,         // \arg address to the next bloc to be transmitted
	unsigned int bytes)    // \arg number of bytes to be transmitted
{
	pPDC->PDC_TPR = (unsigned int) address;
	pPDC->PDC_TCR = bytes;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_EnableTx
//* \brief Enable transmit
//*----------------------------------------------------------------------------
void AT91F_PDC_EnableTx (
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	pPDC->PDC_PTCR = AT91C_PDC_TXTEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_EnableRx
//* \brief Enable receive
//*----------------------------------------------------------------------------
void AT91F_PDC_EnableRx (
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	pPDC->PDC_PTCR = AT91C_PDC_RXTEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_DisableTx
//* \brief Disable transmit
//*----------------------------------------------------------------------------
void AT91F_PDC_DisableTx (
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	pPDC->PDC_PTCR = AT91C_PDC_TXTDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_DisableRx
//* \brief Disable receive
//*----------------------------------------------------------------------------
void AT91F_PDC_DisableRx (
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	pPDC->PDC_PTCR = AT91C_PDC_RXTDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_IsTxEmpty
//* \brief Test if the current transfer descriptor has been sent
//*----------------------------------------------------------------------------
int AT91F_PDC_IsTxEmpty ( // \return return 1 if transfer is complete
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	return !(pPDC->PDC_TCR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_IsNextTxEmpty
//* \brief Test if the next transfer descriptor has been moved to the current td
//*----------------------------------------------------------------------------
int AT91F_PDC_IsNextTxEmpty ( // \return return 1 if transfer is complete
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	return !(pPDC->PDC_TNCR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_IsRxEmpty
//* \brief Test if the current transfer descriptor has been filled
//*----------------------------------------------------------------------------
int AT91F_PDC_IsRxEmpty ( // \return return 1 if transfer is complete
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	return !(pPDC->PDC_RCR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_IsNextRxEmpty
//* \brief Test if the next transfer descriptor has been moved to the current td
//*----------------------------------------------------------------------------
int AT91F_PDC_IsNextRxEmpty ( // \return return 1 if transfer is complete
	AT91PS_PDC pPDC )       // \arg pointer to a PDC controller
{
	return !(pPDC->PDC_RNCR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_Open
//* \brief Open PDC: disable TX and RX reset transfer descriptors, re-enable RX and TX
//*----------------------------------------------------------------------------
void AT91F_PDC_Open (
	AT91PS_PDC pPDC)       // \arg pointer to a PDC controller
{
    //* Disable the RX and TX PDC transfer requests
	AT91F_PDC_DisableRx(pPDC);
	AT91F_PDC_DisableTx(pPDC);

	//* Reset all Counter register Next buffer first
	AT91F_PDC_SetNextTx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetNextRx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetTx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetRx(pPDC, (char *) 0, 0);

    //* Enable the RX and TX PDC transfer requests
	AT91F_PDC_EnableRx(pPDC);
	AT91F_PDC_EnableTx(pPDC);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_Close
//* \brief Close PDC: disable TX and RX reset transfer descriptors
//*----------------------------------------------------------------------------
void AT91F_PDC_Close (
	AT91PS_PDC pPDC)       // \arg pointer to a PDC controller
{
    //* Disable the RX and TX PDC transfer requests
	AT91F_PDC_DisableRx(pPDC);
	AT91F_PDC_DisableTx(pPDC);

	//* Reset all Counter register Next buffer first
	AT91F_PDC_SetNextTx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetNextRx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetTx(pPDC, (char *) 0, 0);
	AT91F_PDC_SetRx(pPDC, (char *) 0, 0);

}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_SendFrame
//* \brief Close PDC: disable TX and RX reset transfer descriptors
//*----------------------------------------------------------------------------
unsigned int AT91F_PDC_SendFrame(
	AT91PS_PDC pPDC,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	if (AT91F_PDC_IsTxEmpty(pPDC)) {
		//* Buffer and next buffer can be initialized
		AT91F_PDC_SetTx(pPDC, pBuffer, szBuffer);
		AT91F_PDC_SetNextTx(pPDC, pNextBuffer, szNextBuffer);
		return 2;
	}
	else if (AT91F_PDC_IsNextTxEmpty(pPDC)) {
		//* Only one buffer can be initialized
		AT91F_PDC_SetNextTx(pPDC, pBuffer, szBuffer);
		return 1;
	}
	else {
		//* All buffer are in use...
		return 0;
	}
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PDC_ReceiveFrame
//* \brief Close PDC: disable TX and RX reset transfer descriptors
//*----------------------------------------------------------------------------
unsigned int AT91F_PDC_ReceiveFrame (
	AT91PS_PDC pPDC,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	if (AT91F_PDC_IsRxEmpty(pPDC)) {
		//* Buffer and next buffer can be initialized
		AT91F_PDC_SetRx(pPDC, pBuffer, szBuffer);
		AT91F_PDC_SetNextRx(pPDC, pNextBuffer, szNextBuffer);
		return 2;
	}
	else if (AT91F_PDC_IsNextRxEmpty(pPDC)) {
		//* Only one buffer can be initialized
		AT91F_PDC_SetNextRx(pPDC, pBuffer, szBuffer);
		return 1;
	}
	else {
		//* All buffer are in use...
		return 0;
	}
}
/* *****************************************************************************
                SOFTWARE API FOR DBGU
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_InterruptEnable
//* \brief Enable DBGU Interrupt
//*----------------------------------------------------------------------------
void AT91F_DBGU_InterruptEnable(
        AT91PS_DBGU pDbgu,   // \arg  pointer to a DBGU controller
        unsigned int flag) // \arg  dbgu interrupt to be enabled
{
        pDbgu->DBGU_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_InterruptDisable
//* \brief Disable DBGU Interrupt
//*----------------------------------------------------------------------------
void AT91F_DBGU_InterruptDisable(
        AT91PS_DBGU pDbgu,   // \arg  pointer to a DBGU controller
        unsigned int flag) // \arg  dbgu interrupt to be disabled
{
        pDbgu->DBGU_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_GetInterruptMaskStatus
//* \brief Return DBGU Interrupt Mask Status
//*----------------------------------------------------------------------------
unsigned int AT91F_DBGU_GetInterruptMaskStatus( // \return DBGU Interrupt Mask Status
        AT91PS_DBGU pDbgu) // \arg  pointer to a DBGU controller
{
        return pDbgu->DBGU_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_IsInterruptMasked
//* \brief Test if DBGU Interrupt is Masked 
//*----------------------------------------------------------------------------
int AT91F_DBGU_IsInterruptMasked(
        AT91PS_DBGU pDbgu,   // \arg  pointer to a DBGU controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_DBGU_GetInterruptMaskStatus(pDbgu) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR PIO
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgPeriph
//* \brief Enable pins to be drived by peripheral
//*----------------------------------------------------------------------------
void AT91F_PIO_CfgPeriph(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int periphAEnable,  // \arg PERIPH A to enable
	unsigned int periphBEnable)  // \arg PERIPH B to enable

{
	pPio->PIO_ASR = periphAEnable;
	pPio->PIO_BSR = periphBEnable;
	pPio->PIO_PDR = (periphAEnable | periphBEnable); // Set in Periph mode
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgOutput
//* \brief Enable PIO in output mode
//*----------------------------------------------------------------------------
void AT91F_PIO_CfgOutput(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int pioEnable)      // \arg PIO to be enabled
{
	pPio->PIO_PER = pioEnable; // Set in PIO mode
	pPio->PIO_OER = pioEnable; // Configure in Output
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgInput
//* \brief Enable PIO in input mode
//*----------------------------------------------------------------------------
void AT91F_PIO_CfgInput(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int inputEnable)      // \arg PIO to be enabled
{
	// Disable output
	pPio->PIO_ODR  = inputEnable;
	pPio->PIO_PER  = inputEnable;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgOpendrain
//* \brief Configure PIO in open drain
//*----------------------------------------------------------------------------
void AT91F_PIO_CfgOpendrain(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int multiDrvEnable) // \arg pio to be configured in open drain
{
	// Configure the multi-drive option
	pPio->PIO_MDDR = ~multiDrvEnable;
	pPio->PIO_MDER = multiDrvEnable;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgPullup
//* \brief Enable pullup on PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_CfgPullup(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int pullupEnable)   // \arg enable pullup on PIO
{
		// Connect or not Pullup
	pPio->PIO_PPUDR = ~pullupEnable;
	pPio->PIO_PPUER = pullupEnable;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgDirectDrive
//* \brief Enable direct drive on PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_CfgDirectDrive(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int directDrive)    // \arg PIO to be configured with direct drive

{
	// Configure the Direct Drive
	pPio->PIO_OWDR  = ~directDrive;
	pPio->PIO_OWER  = directDrive;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_CfgInputFilter
//* \brief Enable input filter on input PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_CfgInputFilter(
	AT91PS_PIO pPio,             // \arg pointer to a PIO controller
	unsigned int inputFilter)    // \arg PIO to be configured with input filter

{
	// Configure the Direct Drive
	pPio->PIO_IFDR  = ~inputFilter;
	pPio->PIO_IFER  = inputFilter;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetInput
//* \brief Return PIO input value
//*----------------------------------------------------------------------------
unsigned int AT91F_PIO_GetInput( // \return PIO input
	AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
	return pPio->PIO_PDSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsInputSet
//* \brief Test if PIO is input flag is active
//*----------------------------------------------------------------------------
int AT91F_PIO_IsInputSet(
	AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
	unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_PIO_GetInput(pPio) & flag);
}


//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_SetOutput
//* \brief Set to 1 output PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_SetOutput(
	AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
	unsigned int flag) // \arg  output to be set
{
	pPio->PIO_SODR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_ClearOutput
//* \brief Set to 0 output PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_ClearOutput(
	AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
	unsigned int flag) // \arg  output to be cleared
{
	pPio->PIO_CODR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_ForceOutput
//* \brief Force output when Direct drive option is enabled
//*----------------------------------------------------------------------------
void AT91F_PIO_ForceOutput(
	AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
	unsigned int flag) // \arg  output to be forced
{
	pPio->PIO_ODSR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_Enable
//* \brief Enable PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_Enable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio to be enabled 
{
        pPio->PIO_PER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_Disable
//* \brief Disable PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_Disable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio to be disabled 
{
        pPio->PIO_PDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetStatus
//* \brief Return PIO Status
//*----------------------------------------------------------------------------
unsigned int AT91F_PIO_GetStatus( // \return PIO Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_PSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsSet
//* \brief Test if PIO is Set
//*----------------------------------------------------------------------------
int AT91F_PIO_IsSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_OutputEnable
//* \brief Output Enable PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_OutputEnable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio output to be enabled
{
        pPio->PIO_OER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_OutputDisable
//* \brief Output Enable PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_OutputDisable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio output to be disabled
{
        pPio->PIO_ODR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetOutputStatus
//* \brief Return PIO Output Status
//*----------------------------------------------------------------------------
unsigned int AT91F_PIO_GetOutputStatus( // \return PIO Output Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_OSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsOuputSet
//* \brief Test if PIO Output is Set
//*----------------------------------------------------------------------------
int AT91F_PIO_IsOutputSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetOutputStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_InputFilterEnable
//* \brief Input Filter Enable PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_InputFilterEnable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio input filter to be enabled
{
        pPio->PIO_IFER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_InputFilterDisable
//* \brief Input Filter Disable PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_InputFilterDisable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio input filter to be disabled
{
        pPio->PIO_IFDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetInputFilterStatus
//* \brief Return PIO Input Filter Status
//*----------------------------------------------------------------------------
unsigned int AT91F_PIO_GetInputFilterStatus( // \return PIO Input Filter Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_IFSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsInputFilterSet
//* \brief Test if PIO Input filter is Set
//*----------------------------------------------------------------------------
int AT91F_PIO_IsInputFilterSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetInputFilterStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetOutputDataStatus
//* \brief Return PIO Output Data Status 
//*----------------------------------------------------------------------------
unsigned int AT91F_PIO_GetOutputDataStatus( // \return PIO Output Data Status 
	AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_ODSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_InterruptEnable
//* \brief Enable PIO Interrupt
//*----------------------------------------------------------------------------
void AT91F_PIO_InterruptEnable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio interrupt to be enabled
{
        pPio->PIO_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_InterruptDisable
//* \brief Disable PIO Interrupt
//*----------------------------------------------------------------------------
void AT91F_PIO_InterruptDisable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio interrupt to be disabled
{
        pPio->PIO_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetInterruptMaskStatus
//* \brief Return PIO Interrupt Mask Status
//*----------------------------------------------------------------------------
unsigned int AT91F_PIO_GetInterruptMaskStatus( // \return PIO Interrupt Mask Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetInterruptStatus
//* \brief Return PIO Interrupt Status
//*----------------------------------------------------------------------------
unsigned int AT91F_PIO_GetInterruptStatus( // \return PIO Interrupt Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_ISR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsInterruptMasked
//* \brief Test if PIO Interrupt is Masked 
//*----------------------------------------------------------------------------
int AT91F_PIO_IsInterruptMasked(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetInterruptMaskStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsInterruptSet
//* \brief Test if PIO Interrupt is Set
//*----------------------------------------------------------------------------
int AT91F_PIO_IsInterruptSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetInterruptStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_MultiDriverEnable
//* \brief Multi Driver Enable PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_MultiDriverEnable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio to be enabled
{
        pPio->PIO_MDER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_MultiDriverDisable
//* \brief Multi Driver Disable PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_MultiDriverDisable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio to be disabled
{
        pPio->PIO_MDDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetMultiDriverStatus
//* \brief Return PIO Multi Driver Status
//*----------------------------------------------------------------------------
unsigned int AT91F_PIO_GetMultiDriverStatus( // \return PIO Multi Driver Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_MDSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsMultiDriverSet
//* \brief Test if PIO MultiDriver is Set
//*----------------------------------------------------------------------------
int AT91F_PIO_IsMultiDriverSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetMultiDriverStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_A_RegisterSelection
//* \brief PIO A Register Selection 
//*----------------------------------------------------------------------------
void AT91F_PIO_A_RegisterSelection(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio A register selection
{
        pPio->PIO_ASR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_B_RegisterSelection
//* \brief PIO B Register Selection 
//*----------------------------------------------------------------------------
void AT91F_PIO_B_RegisterSelection(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio B register selection 
{
        pPio->PIO_BSR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_Get_AB_RegisterStatus
//* \brief Return PIO Interrupt Status
//*----------------------------------------------------------------------------
unsigned int AT91F_PIO_Get_AB_RegisterStatus( // \return PIO AB Register Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_ABSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsAB_RegisterSet
//* \brief Test if PIO AB Register is Set
//*----------------------------------------------------------------------------
int AT91F_PIO_IsAB_RegisterSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_Get_AB_RegisterStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_OutputWriteEnable
//* \brief Output Write Enable PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_OutputWriteEnable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio output write to be enabled
{
        pPio->PIO_OWER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_OutputWriteDisable
//* \brief Output Write Disable PIO
//*----------------------------------------------------------------------------
void AT91F_PIO_OutputWriteDisable(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  pio output write to be disabled
{
        pPio->PIO_OWDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetOutputWriteStatus
//* \brief Return PIO Output Write Status
//*----------------------------------------------------------------------------
unsigned int AT91F_PIO_GetOutputWriteStatus( // \return PIO Output Write Status
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_OWSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsOutputWriteSet
//* \brief Test if PIO OutputWrite is Set
//*----------------------------------------------------------------------------
int AT91F_PIO_IsOutputWriteSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetOutputWriteStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_GetCfgPullup
//* \brief Return PIO Configuration Pullup
//*----------------------------------------------------------------------------
unsigned int AT91F_PIO_GetCfgPullup( // \return PIO Configuration Pullup 
        AT91PS_PIO pPio) // \arg  pointer to a PIO controller
{
        return pPio->PIO_PPUSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsOutputDataStatusSet
//* \brief Test if PIO Output Data Status is Set 
//*----------------------------------------------------------------------------
int AT91F_PIO_IsOutputDataStatusSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_PIO_GetOutputDataStatus(pPio) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIO_IsCfgPullupStatusSet
//* \brief Test if PIO Configuration Pullup Status is Set
//*----------------------------------------------------------------------------
int AT91F_PIO_IsCfgPullupStatusSet(
        AT91PS_PIO pPio,   // \arg  pointer to a PIO controller
        unsigned int flag) // \arg  flag to be tested
{
        return (~AT91F_PIO_GetCfgPullup(pPio) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR PMC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_CfgSysClkEnableReg
//* \brief Configure the System Clock Enable Register of the PMC controller
//*----------------------------------------------------------------------------
void AT91F_PMC_CfgSysClkEnableReg (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int mode)
{
	//* Write to the SCER register
	pPMC->PMC_SCER = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_CfgSysClkDisableReg
//* \brief Configure the System Clock Disable Register of the PMC controller
//*----------------------------------------------------------------------------
void AT91F_PMC_CfgSysClkDisableReg (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int mode) 
{
	//* Write to the SCDR register
	pPMC->PMC_SCDR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetSysClkStatusReg
//* \brief Return the System Clock Status Register of the PMC controller
//*----------------------------------------------------------------------------
unsigned int AT91F_PMC_GetSysClkStatusReg (
	AT91PS_PMC pPMC // pointer to a CAN controller
	)
{
	return pPMC->PMC_SCSR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_EnableHCK
//* \brief Enable AHB peripheral clock
//*----------------------------------------------------------------------------
void AT91F_PMC_EnableHCK (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int hck)
{
	pPMC->PMC_SCER = (1 << hck) << 16;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_DisableHCK
//* \brief Disable AHB peripheral clock
//*----------------------------------------------------------------------------
void AT91F_PMC_DisableHCK (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int hck)  // \arg Peripheral clock identifier 0 .. 7
{
	pPMC->PMC_SCDR = (1 << hck) << 16;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_EnablePeriphClock
//* \brief Enable peripheral clock
//*----------------------------------------------------------------------------
void AT91F_PMC_EnablePeriphClock (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int periphIds)  // \arg IDs of peripherals to enable
{
	pPMC->PMC_PCER = periphIds;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_DisablePeriphClock
//* \brief Disable peripheral clock
//*----------------------------------------------------------------------------
void AT91F_PMC_DisablePeriphClock (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int periphIds)  // \arg IDs of peripherals to enable
{
	pPMC->PMC_PCDR = periphIds;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetPeriphClock
//* \brief Get peripheral clock status
//*----------------------------------------------------------------------------
unsigned int AT91F_PMC_GetPeriphClock (
	AT91PS_PMC pPMC) // \arg pointer to PMC controller
{
	return pPMC->PMC_PCSR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_CfgMainOscillatorReg
//* \brief Cfg the main oscillator
//*----------------------------------------------------------------------------
void AT91F_CKGR_CfgMainOscillatorReg (
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int mode)
{
	pCKGR->CKGR_MOR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_GetMainOscillatorReg
//* \brief Cfg the main oscillator
//*----------------------------------------------------------------------------
unsigned int AT91F_CKGR_GetMainOscillatorReg (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	return pCKGR->CKGR_MOR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_EnableMainOscillator
//* \brief Enable the main oscillator
//*----------------------------------------------------------------------------
void AT91F_CKGR_EnableMainOscillator(
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	pCKGR->CKGR_MOR |= AT91C_CKGR_MOSCEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_DisableMainOscillator
//* \brief Disable the main oscillator
//*----------------------------------------------------------------------------
void AT91F_CKGR_DisableMainOscillator (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	pCKGR->CKGR_MOR &= ~AT91C_CKGR_MOSCEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_CfgMainOscStartUpTime
//* \brief Cfg MOR Register according to the main osc startup time
//*----------------------------------------------------------------------------
void AT91F_CKGR_CfgMainOscStartUpTime (
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int startup_time,  // \arg main osc startup time in microsecond (us)
	unsigned int slowClock)  // \arg slowClock in Hz
{
	pCKGR->CKGR_MOR &= ~AT91C_CKGR_OSCOUNT;
	pCKGR->CKGR_MOR |= ((slowClock * startup_time)/(8*1000000)) << 8;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_GetMainClockFreqReg
//* \brief Cfg the main oscillator
//*----------------------------------------------------------------------------
unsigned int AT91F_CKGR_GetMainClockFreqReg (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	return pCKGR->CKGR_MCFR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_GetMainClock
//* \brief Return Main clock in Hz
//*----------------------------------------------------------------------------
unsigned int AT91F_CKGR_GetMainClock (
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int slowClock)  // \arg slowClock in Hz
{
	return ((pCKGR->CKGR_MCFR  & AT91C_CKGR_MAINF) * slowClock) >> 4;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_CfgPLLAReg
//* \brief Cfg the PLLA Register
//*----------------------------------------------------------------------------
void AT91F_CKGR_CfgPLLAReg (
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int mode)
{
	pCKGR->CKGR_PLLAR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_GetPLLAReg
//* \brief Get the PLLA Register
//*----------------------------------------------------------------------------
unsigned int AT91F_CKGR_GetPLLAReg (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	return pCKGR->CKGR_PLLAR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_CfgPLLBReg
//* \brief Cfg the PLLB Register
//*----------------------------------------------------------------------------
void AT91F_CKGR_CfgPLLBReg (
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int mode)
{
	pCKGR->CKGR_PLLBR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_GetPLLBReg
//* \brief Get the PLLB Register
//*----------------------------------------------------------------------------
unsigned int AT91F_CKGR_GetPLLBReg (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	return pCKGR->CKGR_PLLBR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_CfgPLLA_TransientTime
//* \brief Cfg PLL oscount field according to the PLL transient time
//*----------------------------------------------------------------------------
void AT91F_CKGR_CfgPLLA_TransientTime (
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int transient_time,  // \arg PLL transient time in microsecond (us)
	unsigned int slowClock)  // \arg slowClock in Hz
{
	pCKGR->CKGR_PLLAR &= ~AT91C_CKGR_PLLACOUNT;
	pCKGR->CKGR_PLLAR |= ((slowClock * transient_time)/1000000) << 8;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_CfgPLLB_TransientTime
//* \brief Cfg PLL oscount field according to the PLL transient time
//*----------------------------------------------------------------------------
void AT91F_CKGR_CfgPLLB_TransientTime (
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int transient_time,  // \arg PLL transient time in microsecond (us)
	unsigned int slowClock)  // \arg slowClock in Hz
{
	pCKGR->CKGR_PLLBR &= ~AT91C_CKGR_PLLBCOUNT;
	pCKGR->CKGR_PLLBR |= ((slowClock * transient_time)/1000000) << 8;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_DisablePLLAOutput
//* \brief Disable the PLLA output
//*----------------------------------------------------------------------------
void AT91F_CKGR_DisablePLLAOutput (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	pCKGR->CKGR_PLLAR &= ~AT91C_CKGR_MULA;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_DisablePLLBOutput
//* \brief Disable the PLLB output
//*----------------------------------------------------------------------------
void AT91F_CKGR_DisablePLLBOutput (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	pCKGR->CKGR_PLLBR &= ~AT91C_CKGR_MULB;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_DisablePLLAInput
//* \brief Disable the PLLA Input
//*----------------------------------------------------------------------------
void AT91F_CKGR_DisablePLLAInput (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	pCKGR->CKGR_PLLAR &= ~AT91C_CKGR_DIVA;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_DisablePLLBInput
//* \brief Disable the PLLB Input
//*----------------------------------------------------------------------------
void AT91F_CKGR_DisablePLLBInput (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	pCKGR->CKGR_PLLBR &= ~AT91C_CKGR_DIVB;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_DisablePLLA
//* \brief Disable the PLLA input and output for conso reduction
//*----------------------------------------------------------------------------
void AT91F_CKGR_DisablePLLA (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	pCKGR->CKGR_PLLAR &= ~(AT91C_CKGR_MULA|AT91C_CKGR_DIVA);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_CKGR_DisablePLLB
//* \brief Disable the PLLB input and output for conso reduction
//*----------------------------------------------------------------------------
void AT91F_CKGR_DisablePLLB (
	AT91PS_CKGR pCKGR) // \arg pointer to CKGR controller
{
	pCKGR->CKGR_PLLBR &= ~(AT91C_CKGR_MULB|AT91C_CKGR_DIVB);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_CfgMCKReg
//* \brief Cfg Master Clock Register
//*----------------------------------------------------------------------------
void AT91F_PMC_CfgMCKReg (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int mode)
{
	pPMC->PMC_MCKR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetMCKReg
//* \brief Return Master Clock Register
//*----------------------------------------------------------------------------
unsigned int AT91F_PMC_GetMCKReg(
	AT91PS_PMC pPMC) // \arg pointer to PMC controller
{
	return pPMC->PMC_MCKR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetProcessorClock
//* \brief Return processor clock in Hz
//*----------------------------------------------------------------------------
unsigned int AT91F_PMC_GetProcessorClock (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int slowClock)  // \arg slowClock in Hz
{
	unsigned int reg = pPMC->PMC_MCKR;
	unsigned int prescaler = (1 << ((reg & AT91C_PMC_PRES) >> 2));
	unsigned int pllDivider, pllMultiplier;

	switch (reg & AT91C_PMC_CSS) {
		case AT91C_PMC_CSS_SLOW_CLK: // Slow clock selected
			return slowClock / prescaler;
		case AT91C_PMC_CSS_MAIN_CLK: // Main clock is selected
			return AT91F_CKGR_GetMainClock(pCKGR, slowClock) / prescaler;
		case AT91C_PMC_CSS_PLLA_CLK: // PLLA clock is selected
			reg = pCKGR->CKGR_PLLAR;
			pllDivider    = (reg  & AT91C_CKGR_DIVA);
			pllMultiplier = ((reg  & AT91C_CKGR_MULA) >> 16) + 1;
			if (reg & AT91C_CKGR_SRCA) // Source is Main clock
				return AT91F_CKGR_GetMainClock(pCKGR, slowClock) / pllDivider * pllMultiplier / prescaler;
			else                       // Source is Slow clock
				return slowClock / pllDivider * pllMultiplier / prescaler;
		case AT91C_PMC_CSS_PLLB_CLK: // PLLB clock is selected
			reg = pCKGR->CKGR_PLLBR;
			pllDivider    = (reg  & AT91C_CKGR_DIVB);
			pllMultiplier = ((reg  & AT91C_CKGR_MULB) >> 16) + 1;
			return AT91F_CKGR_GetMainClock(pCKGR, slowClock) / pllDivider * pllMultiplier / prescaler;
	}
	return 0;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetMasterClock
//* \brief Return master clock in Hz
//*----------------------------------------------------------------------------
unsigned int AT91F_PMC_GetMasterClock (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	AT91PS_CKGR pCKGR, // \arg pointer to CKGR controller
	unsigned int slowClock)  // \arg slowClock in Hz
{
	return AT91F_PMC_GetProcessorClock(pPMC, pCKGR, slowClock) /
		(((pPMC->PMC_MCKR & AT91C_PMC_MDIV) >> 8)+1);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_EnablePCK
//* \brief Enable peripheral clock
//*----------------------------------------------------------------------------
void AT91F_PMC_EnablePCK (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int pck,  // \arg Peripheral clock identifier 0 .. 7
	unsigned int mode)
{
	pPMC->PMC_PCKR[pck] = mode;
	pPMC->PMC_SCER = (1 << pck) << 8;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_DisablePCK
//* \brief Enable peripheral clock
//*----------------------------------------------------------------------------
void AT91F_PMC_DisablePCK (
	AT91PS_PMC pPMC, // \arg pointer to PMC controller
	unsigned int pck)  // \arg Peripheral clock identifier 0 .. 7
{
	pPMC->PMC_SCDR = (1 << pck) << 8;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_EnableIt
//* \brief Enable PMC interrupt
//*----------------------------------------------------------------------------
void AT91F_PMC_EnableIt (
	AT91PS_PMC pPMC,     // pointer to a PMC controller
	unsigned int flag)   // IT to be enabled
{
	//* Write to the IER register
	pPMC->PMC_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_DisableIt
//* \brief Disable PMC interrupt
//*----------------------------------------------------------------------------
void AT91F_PMC_DisableIt (
	AT91PS_PMC pPMC, // pointer to a PMC controller
	unsigned int flag) // IT to be disabled
{
	//* Write to the IDR register
	pPMC->PMC_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetStatus
//* \brief Return PMC Interrupt Status
//*----------------------------------------------------------------------------
unsigned int AT91F_PMC_GetStatus( // \return PMC Interrupt Status
	AT91PS_PMC pPMC) // pointer to a PMC controller
{
	return pPMC->PMC_SR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_GetInterruptMaskStatus
//* \brief Return PMC Interrupt Mask Status
//*----------------------------------------------------------------------------
unsigned int AT91F_PMC_GetInterruptMaskStatus( // \return PMC Interrupt Mask Status
	AT91PS_PMC pPMC) // pointer to a PMC controller
{
	return pPMC->PMC_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_IsInterruptMasked
//* \brief Test if PMC Interrupt is Masked 
//*----------------------------------------------------------------------------
unsigned int AT91F_PMC_IsInterruptMasked(
        AT91PS_PMC pPMC,   // \arg  pointer to a PMC controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_PMC_GetInterruptMaskStatus(pPMC) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_IsStatusSet
//* \brief Test if PMC Status is Set
//*----------------------------------------------------------------------------
unsigned int AT91F_PMC_IsStatusSet(
        AT91PS_PMC pPMC,   // \arg  pointer to a PMC controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_PMC_GetStatus(pPMC) & flag);
}/* *****************************************************************************
                SOFTWARE API FOR RSTC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTSoftReset
//* \brief Start Software Reset
//*----------------------------------------------------------------------------
void AT91F_RSTSoftReset(
        AT91PS_RSTC pRSTC,
        unsigned int reset)
{
	pRSTC->RSTC_RCR = (0xA5000000 | reset);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTSetMode
//* \brief Set Reset Mode
//*----------------------------------------------------------------------------
void AT91F_RSTSetMode(
        AT91PS_RSTC pRSTC,
        unsigned int mode)
{
	pRSTC->RSTC_RMR = (0xA5000000 | mode);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTGetMode
//* \brief Get Reset Mode
//*----------------------------------------------------------------------------
unsigned int AT91F_RSTGetMode(
        AT91PS_RSTC pRSTC)
{
	return (pRSTC->RSTC_RMR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTGetStatus
//* \brief Get Reset Status
//*----------------------------------------------------------------------------
unsigned int AT91F_RSTGetStatus(
        AT91PS_RSTC pRSTC)
{
	return (pRSTC->RSTC_RSR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTIsSoftRstActive
//* \brief Return !=0 if software reset is still not completed
//*----------------------------------------------------------------------------
unsigned int AT91F_RSTIsSoftRstActive(
        AT91PS_RSTC pRSTC)
{
	return ((pRSTC->RSTC_RSR) & AT91C_RSTC_SRCMP);
}
/* *****************************************************************************
                SOFTWARE API FOR SHDWC
   ***************************************************************************** */
//*--------------------------------------------------------------------------------------
//* Function Name       : AT91F_SHDWNSetMode()
//* Object              : Set the Shutdown mode register
//*--------------------------------------------------------------------------------------
unsigned int AT91F_SHDWNSetMode(
        AT91PS_SHDWC pSHDWC,
        unsigned int mode)
{
	pSHDWC->SHDWC_SHMR = mode;
	return(pSHDWC->SHDWC_SHMR);
}

//*--------------------------------------------------------------------------------------
//* Function Name       : AT91F_SHDWNGetMode()
//* Object              : Get the Shutdown mode register
//*--------------------------------------------------------------------------------------
unsigned int AT91F_SHDWNGetMode(
        AT91PS_SHDWC pSHDWC)
{
	return(pSHDWC->SHDWC_SHMR);
}

//*--------------------------------------------------------------------------------------
//* Function Name       : AT91F_SHDWNEnable()
//* Object              : Enable Shutdown
//*--------------------------------------------------------------------------------------
void AT91F_SHDWNEnable(
        AT91PS_SHDWC pSHDWC)
{
	pSHDWC->SHDWC_SHCR = 0xA5000001;
}

//*--------------------------------------------------------------------------------------
//* Function Name       : AT91F_SHDWNGetStatus()
//* Object              : Get Shutdown Status
//*--------------------------------------------------------------------------------------
unsigned int AT91F_SHDWNGetStatus(
        AT91PS_SHDWC pSHDWC)
{
	return(pSHDWC->SHDWC_SHSR);
}

/* *****************************************************************************
                SOFTWARE API FOR RTTC
   ***************************************************************************** */
//*--------------------------------------------------------------------------------------
//* \fn     AT91F_SetRTT_TimeBase()
//* \brief  Set the RTT prescaler according to the TimeBase in ms
//*--------------------------------------------------------------------------------------
unsigned int AT91F_RTTSetTimeBase(
        AT91PS_RTTC pRTTC, 
        unsigned int ms)
{
	if (ms > 2000)
		return 1;   // AT91C_TIME_OUT_OF_RANGE
	pRTTC->RTTC_RTMR &= ~0xFFFF;	
	pRTTC->RTTC_RTMR |= (((ms << 15) /1000) & 0xFFFF);	
	return 0;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTTSetPrescaler()
//* \brief  Set the new prescaler value
//*--------------------------------------------------------------------------------------
unsigned int AT91F_RTTSetPrescaler(
        AT91PS_RTTC pRTTC, 
        unsigned int rtpres)
{
	pRTTC->RTTC_RTMR &= ~0xFFFF;	
	pRTTC->RTTC_RTMR |= (rtpres & 0xFFFF);	
	return (pRTTC->RTTC_RTMR);
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTTRestart()
//* \brief  Restart the RTT prescaler
//*--------------------------------------------------------------------------------------
void AT91F_RTTRestart(
        AT91PS_RTTC pRTTC)
{
	pRTTC->RTTC_RTMR |= AT91C_RTTC_RTTRST;	
}


//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_SetAlarmINT()
//* \brief  Enable RTT Alarm Interrupt
//*--------------------------------------------------------------------------------------
void AT91F_RTTSetAlarmINT(
        AT91PS_RTTC pRTTC)
{
	pRTTC->RTTC_RTMR |= AT91C_RTTC_ALMIEN;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_ClearAlarmINT()
//* \brief  Disable RTT Alarm Interrupt
//*--------------------------------------------------------------------------------------
void AT91F_RTTClearAlarmINT(
        AT91PS_RTTC pRTTC)
{
	pRTTC->RTTC_RTMR &= ~AT91C_RTTC_ALMIEN;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_SetRttIncINT()
//* \brief  Enable RTT INC Interrupt
//*--------------------------------------------------------------------------------------
void AT91F_RTTSetRttIncINT(
        AT91PS_RTTC pRTTC)
{
	pRTTC->RTTC_RTMR |= AT91C_RTTC_RTTINCIEN;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_ClearRttIncINT()
//* \brief  Disable RTT INC Interrupt
//*--------------------------------------------------------------------------------------
void AT91F_RTTClearRttIncINT(
        AT91PS_RTTC pRTTC)
{
	pRTTC->RTTC_RTMR &= ~AT91C_RTTC_RTTINCIEN;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_SetAlarmValue()
//* \brief  Set RTT Alarm Value
//*--------------------------------------------------------------------------------------
void AT91F_RTTSetAlarmValue(
        AT91PS_RTTC pRTTC, unsigned int alarm)
{
	pRTTC->RTTC_RTAR = alarm;
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_GetAlarmValue()
//* \brief  Get RTT Alarm Value
//*--------------------------------------------------------------------------------------
unsigned int AT91F_RTTGetAlarmValue(
        AT91PS_RTTC pRTTC)
{
	return(pRTTC->RTTC_RTAR);
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTTGetStatus()
//* \brief  Read the RTT status
//*--------------------------------------------------------------------------------------
unsigned int AT91F_RTTGetStatus(
        AT91PS_RTTC pRTTC)
{
	return(pRTTC->RTTC_RTSR);
}

//*--------------------------------------------------------------------------------------
//* \fn     AT91F_RTT_ReadValue()
//* \brief  Read the RTT value
//*--------------------------------------------------------------------------------------
unsigned int AT91F_RTTReadValue(
        AT91PS_RTTC pRTTC)
{
        register volatile unsigned int val1,val2;
	do
	{
		val1 = pRTTC->RTTC_RTVR;
		val2 = pRTTC->RTTC_RTVR;
	}	
	while(val1 != val2);
	return(val1);
}
/* *****************************************************************************
                SOFTWARE API FOR PITC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_PITInit
//* \brief System timer init : period in µsecond, system clock freq in MHz
//*----------------------------------------------------------------------------
void AT91F_PITInit(
        AT91PS_PITC pPITC,
        unsigned int period,
        unsigned int pit_frequency)
{
	pPITC->PITC_PIMR = period? (period * pit_frequency + 8) >> 4 : 0; // +8 to avoid %10 and /10
	pPITC->PITC_PIMR |= AT91C_PITC_PITEN;	 
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITSetPIV
//* \brief Set the PIT Periodic Interval Value 
//*----------------------------------------------------------------------------
void AT91F_PITSetPIV(
        AT91PS_PITC pPITC,
        unsigned int piv)
{
	pPITC->PITC_PIMR = piv | (pPITC->PITC_PIMR & (AT91C_PITC_PITEN | AT91C_PITC_PITIEN));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITEnableInt
//* \brief Enable PIT periodic interrupt
//*----------------------------------------------------------------------------
void AT91F_PITEnableInt(
        AT91PS_PITC pPITC)
{
	pPITC->PITC_PIMR |= AT91C_PITC_PITIEN;	 
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITDisableInt
//* \brief Disable PIT periodic interrupt
//*----------------------------------------------------------------------------
void AT91F_PITDisableInt(
        AT91PS_PITC pPITC)
{
	pPITC->PITC_PIMR &= ~AT91C_PITC_PITIEN;	 
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITGetMode
//* \brief Read PIT mode register
//*----------------------------------------------------------------------------
unsigned int AT91F_PITGetMode(
        AT91PS_PITC pPITC)
{
	return(pPITC->PITC_PIMR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITGetStatus
//* \brief Read PIT status register
//*----------------------------------------------------------------------------
unsigned int AT91F_PITGetStatus(
        AT91PS_PITC pPITC)
{
	return(pPITC->PITC_PISR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITGetPIIR
//* \brief Read PIT CPIV and PICNT without ressetting the counters
//*----------------------------------------------------------------------------
unsigned int AT91F_PITGetPIIR(
        AT91PS_PITC pPITC)
{
	return(pPITC->PITC_PIIR);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITGetPIVR
//* \brief Read System timer CPIV and PICNT without ressetting the counters
//*----------------------------------------------------------------------------
unsigned int AT91F_PITGetPIVR(
        AT91PS_PITC pPITC)
{
	return(pPITC->PITC_PIVR);
}
/* *****************************************************************************
                SOFTWARE API FOR WDTC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_WDTSetMode
//* \brief Set Watchdog Mode Register
//*----------------------------------------------------------------------------
void AT91F_WDTSetMode(
        AT91PS_WDTC pWDTC,
        unsigned int Mode)
{
	pWDTC->WDTC_WDMR = Mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_WDTRestart
//* \brief Restart Watchdog
//*----------------------------------------------------------------------------
void AT91F_WDTRestart(
        AT91PS_WDTC pWDTC)
{
	pWDTC->WDTC_WDCR = 0xA5000001;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_WDTSGettatus
//* \brief Get Watchdog Status
//*----------------------------------------------------------------------------
unsigned int AT91F_WDTSGettatus(
        AT91PS_WDTC pWDTC)
{
	return(pWDTC->WDTC_WDSR & 0x3);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_WDTGetPeriod
//* \brief Translate ms into Watchdog Compatible value
//*----------------------------------------------------------------------------
unsigned int AT91F_WDTGetPeriod(unsigned int ms)
{
	if ((ms < 4) || (ms > 16000))
		return 0;
	return((ms << 8) / 1000);
}
/* *****************************************************************************
                SOFTWARE API FOR TC
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_TC_InterruptEnable
//* \brief Enable TC Interrupt
//*----------------------------------------------------------------------------
void AT91F_TC_InterruptEnable(
        AT91PS_TC pTc,   // \arg  pointer to a TC controller
        unsigned int flag) // \arg  TC interrupt to be enabled
{
        pTc->TC_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC_InterruptDisable
//* \brief Disable TC Interrupt
//*----------------------------------------------------------------------------
void AT91F_TC_InterruptDisable(
        AT91PS_TC pTc,   // \arg  pointer to a TC controller
        unsigned int flag) // \arg  TC interrupt to be disabled
{
        pTc->TC_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC_GetInterruptMaskStatus
//* \brief Return TC Interrupt Mask Status
//*----------------------------------------------------------------------------
unsigned int AT91F_TC_GetInterruptMaskStatus( // \return TC Interrupt Mask Status
        AT91PS_TC pTc) // \arg  pointer to a TC controller
{
        return pTc->TC_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC_IsInterruptMasked
//* \brief Test if TC Interrupt is Masked 
//*----------------------------------------------------------------------------
int AT91F_TC_IsInterruptMasked(
        AT91PS_TC pTc,   // \arg  pointer to a TC controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_TC_GetInterruptMaskStatus(pTc) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR UDP
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EnableIt
//* \brief Enable UDP IT
//*----------------------------------------------------------------------------
void AT91F_UDP_EnableIt (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned int flag)   // \arg IT to be enabled
{
	//* Write to the IER register
	pUDP->UDP_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_DisableIt
//* \brief Disable UDP IT
//*----------------------------------------------------------------------------
void AT91F_UDP_DisableIt (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned int flag)   // \arg IT to be disabled
{
	//* Write to the IDR register
	pUDP->UDP_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_SetAddress
//* \brief Set UDP functional address
//*----------------------------------------------------------------------------
void AT91F_UDP_SetAddress (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned char address)   // \arg new UDP address
{
	pUDP->UDP_FADDR = (AT91C_UDP_FEN | address);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EnableEp
//* \brief Enable Endpoint
//*----------------------------------------------------------------------------
void AT91F_UDP_EnableEp (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned char endpoint)   // \arg endpoint number
{
	pUDP->UDP_CSR[endpoint] |= AT91C_UDP_EPEDS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_DisableEp
//* \brief Enable Endpoint
//*----------------------------------------------------------------------------
void AT91F_UDP_DisableEp (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned char endpoint)   // \arg endpoint number
{
	pUDP->UDP_CSR[endpoint] &= ~AT91C_UDP_EPEDS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_SetState
//* \brief Set UDP Device state
//*----------------------------------------------------------------------------
void AT91F_UDP_SetState (
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned int flag)   // \arg new UDP address
{
	pUDP->UDP_GLBSTATE  &= ~(AT91C_UDP_FADDEN | AT91C_UDP_CONFG);
	pUDP->UDP_GLBSTATE  |= flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_GetState
//* \brief return UDP Device state
//*----------------------------------------------------------------------------
unsigned int AT91F_UDP_GetState ( // \return the UDP device state
	AT91PS_UDP pUDP)     // \arg pointer to a UDP controller
{
	return (pUDP->UDP_GLBSTATE  & (AT91C_UDP_FADDEN | AT91C_UDP_CONFG));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_ResetEp
//* \brief Reset UDP endpoint
//*----------------------------------------------------------------------------
void AT91F_UDP_ResetEp ( // \return the UDP device state
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned int flag)   // \arg Endpoints to be reset
{
	pUDP->UDP_RSTEP = flag;
	pUDP->UDP_RSTEP = 0;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpStall
//* \brief Endpoint will STALL requests
//*----------------------------------------------------------------------------
void AT91F_UDP_EpStall(
	AT91PS_UDP pUDP,     // \arg pointer to a UDP controller
	unsigned char endpoint)   // \arg endpoint number
{
	pUDP->UDP_CSR[endpoint] |= AT91C_UDP_FORCESTALL;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpWrite
//* \brief Write value in the DPR
//*----------------------------------------------------------------------------
void AT91F_UDP_EpWrite(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint,  // \arg endpoint number
	unsigned char value)     // \arg value to be written in the DPR
{
	pUDP->UDP_FDR[endpoint] = value;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpRead
//* \brief Return value from the DPR
//*----------------------------------------------------------------------------
unsigned int AT91F_UDP_EpRead(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint)  // \arg endpoint number
{
	return pUDP->UDP_FDR[endpoint];
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpEndOfWr
//* \brief Notify the UDP that values in DPR are ready to be sent
//*----------------------------------------------------------------------------
void AT91F_UDP_EpEndOfWr(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint)  // \arg endpoint number
{
	pUDP->UDP_CSR[endpoint] |= AT91C_UDP_TXPKTRDY;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpClear
//* \brief Clear flag in the endpoint CSR register
//*----------------------------------------------------------------------------
void AT91F_UDP_EpClear(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint,  // \arg endpoint number
	unsigned int flag)       // \arg flag to be cleared
{
	pUDP->UDP_CSR[endpoint] &= ~(flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpSet
//* \brief Set flag in the endpoint CSR register
//*----------------------------------------------------------------------------
void AT91F_UDP_EpSet(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint,  // \arg endpoint number
	unsigned int flag)       // \arg flag to be cleared
{
	pUDP->UDP_CSR[endpoint] |= flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_EpStatus
//* \brief Return the endpoint CSR register
//*----------------------------------------------------------------------------
unsigned int AT91F_UDP_EpStatus(
	AT91PS_UDP pUDP,         // \arg pointer to a UDP controller
	unsigned char endpoint)  // \arg endpoint number
{
	return pUDP->UDP_CSR[endpoint];
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_GetInterruptMaskStatus
//* \brief Return UDP Interrupt Mask Status
//*----------------------------------------------------------------------------
unsigned int AT91F_UDP_GetInterruptMaskStatus(
  AT91PS_UDP pUdp)        // \arg  pointer to a UDP controller
{
  return pUdp->UDP_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_IsInterruptMasked
//* \brief Test if UDP Interrupt is Masked 
//*----------------------------------------------------------------------------
int AT91F_UDP_IsInterruptMasked(
  AT91PS_UDP pUdp,       // \arg  pointer to a UDP controller
  unsigned int flag)     // \arg  flag to be tested
{
  return (AT91F_UDP_GetInterruptMaskStatus(pUdp) & flag);
}

// ----------------------------------------------------------------------------
//  \fn    AT91F_UDP_InterruptStatusRegister
//  \brief Return the Interrupt Status Register
// ----------------------------------------------------------------------------
unsigned int AT91F_UDP_InterruptStatusRegister( 
  AT91PS_UDP pUDP )      // \arg  pointer to a UDP controller
{
  return pUDP->UDP_ISR;
}

// ----------------------------------------------------------------------------
//  \fn    AT91F_UDP_InterruptClearRegister
//  \brief Clear Interrupt Register
// ----------------------------------------------------------------------------
void AT91F_UDP_InterruptClearRegister (
  AT91PS_UDP pUDP,       // \arg pointer to UDP controller
  unsigned int flag)     // \arg IT to be cleat
{
  pUDP->UDP_ICR = flag; 
}

// ----------------------------------------------------------------------------
//  \fn    AT91F_UDP_EnableTransceiver
//  \brief Enable transceiver
// ----------------------------------------------------------------------------
void AT91F_UDP_EnableTransceiver( 
  AT91PS_UDP pUDP )      // \arg  pointer to a UDP controller
{
    pUDP->UDP_TXVC &= ~AT91C_UDP_TXVDIS; 
}

// ----------------------------------------------------------------------------
//  \fn    AT91F_UDP_DisableTransceiver
//  \brief Disable transceiver
// ----------------------------------------------------------------------------
void AT91F_UDP_DisableTransceiver( 
  AT91PS_UDP pUDP )      // \arg  pointer to a UDP controller
{
    pUDP->UDP_TXVC = AT91C_UDP_TXVDIS; 
}

/* *****************************************************************************
                SOFTWARE API FOR MCI
   ***************************************************************************** */
// Classic MCI Data Timeout Register Configuration with 1048576 MCK cycles between 2 data transfer
/*#define AT91C_MCI_DTOR_1MEGA_CYCLES	(AT91C_MCI_DTOCYC | AT91C_MCI_DTOMUL)

// Classic MCI SDCard Register Configuration with 1-bit data bus on slot A
#define AT91C_MCI_MMC_SLOTA	(AT91C_MCI_SCDSEL & 0x0)

// Classic MCI SDCard Register Configuration with 1-bit data bus on slot B
#define AT91C_MCI_MMC_SLOTB	(AT91C_MCI_SCDSEL & 0x1)

// Classic MCI SDCard Register Configuration with 4-bit data bus on slot A
#define AT91C_MCI_SDCARD_4BITS_SLOTA	( (AT91C_MCI_SCDSEL & 0x0) | AT91C_MCI_SCDBUS )

// Classic MCI SDCard Register Configuration with 4-bit data bus on slot B
#define AT91C_MCI_SDCARD_4BITS_SLOTB	( (AT91C_MCI_SCDSEL & 0x1) | AT91C_MCI_SCDBUS )
*/

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_Configure
//* \brief Configure the MCI
//*----------------------------------------------------------------------------
void AT91F_MCI_Configure (
        AT91PS_MCI pMCI,  			 // \arg pointer to a MCI controller
        unsigned int DTOR_register,  // \arg Data Timeout Register to be programmed
        unsigned int MR_register,  	 // \arg Mode Register to be programmed
        unsigned int SDCR_register)  // \arg SDCard Register to be programmed
{
    //* Disable the MCI
    pMCI->MCI_CR = AT91C_MCI_MCIDIS | AT91C_MCI_PWSDIS;

    //* Disable all the interrupts
    pMCI->MCI_IDR = 0xFFFFFFFF;

    //* Set the Data Timeout Register
    pMCI->MCI_DTOR = DTOR_register;

    //* Set the Mode Register
    pMCI->MCI_MR = MR_register;

    //* Set the SDCard Register
    pMCI->MCI_SDCR = SDCR_register;

   //* Enable the MCI
    pMCI->MCI_CR = AT91C_MCI_MCIEN | AT91C_MCI_PWSEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_EnableIt
//* \brief Enable MCI IT
//*----------------------------------------------------------------------------
void AT91F_MCI_EnableIt (
        AT91PS_MCI pMCI, // \arg pointer to a MCI controller
        unsigned int flag)   // \arg IT to be enabled
{
    //* Write to the IER register
    pMCI->MCI_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_DisableIt
//* \brief Disable MCI IT
//*----------------------------------------------------------------------------
void AT91F_MCI_DisableIt (
        AT91PS_MCI pMCI, // \arg pointer to a MCI controller
        unsigned int flag)   // \arg IT to be disabled
{
    //* Write to the IDR register
    pMCI->MCI_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_GetStatus
//* \brief Return MCI Interrupt Status
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_GetStatus( // \return MCI Interrupt Status
	AT91PS_MCI pMCI) // pointer to a MCI controller
{
	return pMCI->MCI_SR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_GetInterruptMaskStatus
//* \brief Return MCI Interrupt Mask Status
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_GetInterruptMaskStatus( // \return MCI Interrupt Mask Status
	AT91PS_MCI pMCI) // pointer to a MCI controller
{
	return pMCI->MCI_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_IsInterruptMasked
//* \brief Test if MCI Interrupt is Masked 
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_IsInterruptMasked(
        AT91PS_MCI pMCI,   // \arg  pointer to a MCI controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_MCI_GetInterruptMaskStatus(pMCI) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_IsStatusSet
//* \brief Test if MCI Status is Set
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_IsStatusSet(
        AT91PS_MCI pMCI,   // \arg  pointer to a MCI controller
        unsigned int flag) // \arg  flag to be tested
{
	return (AT91F_MCI_GetStatus(pMCI) & flag);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_Enable
//* \brief Enable the MCI Interface
//*----------------------------------------------------------------------------
void AT91F_MCI_Enable (
        AT91PS_MCI pMCI)     // \arg pointer to a MCI controller
{
    //* Enable the MCI
    pMCI->MCI_CR = AT91C_MCI_MCIEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_Disable
//* \brief Disable the MCI Interface
//*----------------------------------------------------------------------------
void AT91F_MCI_Disable (
        AT91PS_MCI pMCI)     // \arg pointer to a MCI controller
{
    //* Disable the MCI
    pMCI->MCI_CR = AT91C_MCI_MCIDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_SW_Reset
//* \brief Reset the MCI Interface
//*----------------------------------------------------------------------------
void AT91F_MCI_SW_Reset (
        AT91PS_MCI pMCI)     // \arg pointer to a MCI controller
{
    //* Disable the MCI
    pMCI->MCI_CR = AT91C_MCI_SWRST;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_CfgModeReg
//* \brief Configure the Mode Register of the MCI controller
//*----------------------------------------------------------------------------
void AT91F_MCI_CfgModeReg (
	AT91PS_MCI pMCI, // pointer to a MCI controller
	unsigned int mode)        // mode register 
{
	//* Write to the MR register
	pMCI->MCI_MR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_GetModeReg
//* \brief Return the Mode Register of the MCI controller value
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_GetModeReg (
	AT91PS_MCI pMCI // pointer to a MCI controller
	)
{
	return pMCI->MCI_MR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_CfgFrequency
//* \brief Configure the frequency MCCK of the MCI controller
//*----------------------------------------------------------------------------
void AT91F_MCI_CfgFrequency (
	AT91PS_MCI pMCI, // pointer to a MCI controller
	unsigned int mck_clock, // in MHz 
	unsigned int mci_clock)	// in MHz  
{
	unsigned int prescal;
	
	prescal = mck_clock/(2*mci_clock) - 1;
	
	//* Write to the MR register
	pMCI->MCI_MR &= ~AT91C_MCI_CLKDIV;
	pMCI->MCI_MR |= (prescal & AT91C_MCI_CLKDIV);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_CfgDataTimeOutReg
//* \brief Configure the Data TimeOut Register of the MCI controller
//*----------------------------------------------------------------------------
void AT91F_MCI_CfgDataTimeOutReg (
	AT91PS_MCI pMCI, // pointer to a MCI controller
	unsigned int cfg)
{
	//* Write to the DTOR register
	pMCI->MCI_DTOR = cfg;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_GetDataTimeOutReg
//* \brief Return the Data TimeOut Register of the MCI controller value
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_GetDataTimeOutReg (
	AT91PS_MCI pMCI // pointer to a MCI controller
	)
{
	return pMCI->MCI_DTOR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_CfgSDCardReg
//* \brief Configure the SDCard Register of the MCI controller
//*----------------------------------------------------------------------------
void AT91F_MCI_CfgSDCardReg (
	AT91PS_MCI pMCI, // pointer to a MCI controller
	unsigned int cfg)
{
	//* Write to the SDCR register
	pMCI->MCI_SDCR = cfg;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_GetSDCardReg
//* \brief Return the SDCard Register of the MCI controller value
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_GetSDCardReg (
	AT91PS_MCI pMCI // pointer to a MCI controller
	)
{
	return pMCI->MCI_SDCR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_CfgARGReg
//* \brief Configure the Command ARGUMENT Register of the MCI controller
//*----------------------------------------------------------------------------
void AT91F_MCI_CfgARGReg (
	AT91PS_MCI pMCI, // pointer to a MCI controller
	unsigned int arg)
{
	//* Write to the ARG register
	pMCI->MCI_ARGR = arg;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_GetARGReg
//* \brief Return the Command ARGUMENT Register of the MCI controller value
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_GetARGReg (
	AT91PS_MCI pMCI // pointer to a MCI controller
	)
{
	return pMCI->MCI_ARGR;	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_CfgCMDReg
//* \brief Configure the COMMAND Register of the MCI controller
//*----------------------------------------------------------------------------
void AT91F_MCI_CfgCMDReg (
	AT91PS_MCI pMCI, // pointer to a MCI controller
	unsigned int cmd)
{
	//* Write to the ARG register
	pMCI->MCI_CMDR = cmd;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_GetRSP0Reg
//* \brief Return the RESPONSE Register 0 of the MCI controller value
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_GetRSP0Reg (
	AT91PS_MCI pMCI // pointer to a MCI controller
	)
{
	return pMCI->MCI_RSPR[0];	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_GetRSP1Reg
//* \brief Return the RESPONSE Register 1 of the MCI controller value
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_GetRSP1Reg (
	AT91PS_MCI pMCI // pointer to a MCI controller
	)
{
	return pMCI->MCI_RSPR[0];	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_GetRSP2Reg
//* \brief Return the RESPONSE Register 2 of the MCI controller value
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_GetRSP2Reg (
	AT91PS_MCI pMCI // pointer to a MCI controller
	)
{
	return pMCI->MCI_RSPR[0];	
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_GetRSP3Reg
//* \brief Return the RESPONSE Register 3 of the MCI controller value
//*----------------------------------------------------------------------------
unsigned int AT91F_MCI_GetRSP3Reg (
	AT91PS_MCI pMCI // pointer to a MCI controller
	)
{
	return pMCI->MCI_RSPR[0];	
}

/* *****************************************************************************
                SOFTWARE API FOR TWI
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_EnableIt
//* \brief Enable TWI IT
//*----------------------------------------------------------------------------
void AT91F_TWI_EnableIt (
	AT91PS_TWI pTWI, // \arg pointer to a TWI controller
	unsigned int flag)   // \arg IT to be enabled
{
	//* Write to the IER register
	pTWI->TWI_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_DisableIt
//* \brief Disable TWI IT
//*----------------------------------------------------------------------------
void AT91F_TWI_DisableIt (
	AT91PS_TWI pTWI, // \arg pointer to a TWI controller
	unsigned int flag)   // \arg IT to be disabled
{
	//* Write to the IDR register
	pTWI->TWI_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_Configure
//* \brief Configure TWI in master mode
//*----------------------------------------------------------------------------
void AT91F_TWI_Configure ( AT91PS_TWI pTWI )          // \arg pointer to a TWI controller
{
    //* Disable interrupts
	pTWI->TWI_IDR = (unsigned int) -1;

    //* Reset peripheral
	pTWI->TWI_CR = AT91C_TWI_SWRST;

	//* Set Master mode
	pTWI->TWI_CR = AT91C_TWI_MSEN;

}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_GetInterruptMaskStatus
//* \brief Return TWI Interrupt Mask Status
//*----------------------------------------------------------------------------
unsigned int AT91F_TWI_GetInterruptMaskStatus( // \return TWI Interrupt Mask Status
        AT91PS_TWI pTwi) // \arg  pointer to a TWI controller
{
        return pTwi->TWI_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_IsInterruptMasked
//* \brief Test if TWI Interrupt is Masked 
//*----------------------------------------------------------------------------
int AT91F_TWI_IsInterruptMasked(
        AT91PS_TWI pTwi,   // \arg  pointer to a TWI controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_TWI_GetInterruptMaskStatus(pTwi) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR USART
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_US_Baudrate
//* \brief Calculate the baudrate
//* Standard Asynchronous Mode : 8 bits , 1 stop , no parity
#define AT91C_US_ASYNC_MODE ( AT91C_US_USMODE_NORMAL + \
                        AT91C_US_NBSTOP_1_BIT + \
                        AT91C_US_PAR_NONE + \
                        AT91C_US_CHRL_8_BITS + \
                        AT91C_US_CLKS_CLOCK )

//* Standard External Asynchronous Mode : 8 bits , 1 stop , no parity
#define AT91C_US_ASYNC_SCK_MODE ( AT91C_US_USMODE_NORMAL + \
                            AT91C_US_NBSTOP_1_BIT + \
                            AT91C_US_PAR_NONE + \
                            AT91C_US_CHRL_8_BITS + \
                            AT91C_US_CLKS_EXT )

//* Standard Synchronous Mode : 8 bits , 1 stop , no parity
#define AT91C_US_SYNC_MODE ( AT91C_US_SYNC + \
                       AT91C_US_USMODE_NORMAL + \
                       AT91C_US_NBSTOP_1_BIT + \
                       AT91C_US_PAR_NONE + \
                       AT91C_US_CHRL_8_BITS + \
                       AT91C_US_CLKS_CLOCK )

//* SCK used Label
#define AT91C_US_SCK_USED (AT91C_US_CKLO | AT91C_US_CLKS_EXT)

//* Standard ISO T=0 Mode : 8 bits , 1 stop , parity
#define AT91C_US_ISO_READER_MODE ( AT91C_US_USMODE_ISO7816_0 + \
					   		 AT91C_US_CLKS_CLOCK +\
                       		 AT91C_US_NBSTOP_1_BIT + \
                       		 AT91C_US_PAR_EVEN + \
                       		 AT91C_US_CHRL_8_BITS + \
                       		 AT91C_US_CKLO +\
                       		 AT91C_US_OVER)

//* Standard IRDA mode
#define AT91C_US_ASYNC_IRDA_MODE (  AT91C_US_USMODE_IRDA + \
                            AT91C_US_NBSTOP_1_BIT + \
                            AT91C_US_PAR_NONE + \
                            AT91C_US_CHRL_8_BITS + \
                            AT91C_US_CLKS_CLOCK )

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_Baudrate
//* \brief Caluculate baud_value according to the main clock and the baud rate
//*----------------------------------------------------------------------------
unsigned int AT91F_US_Baudrate (
	const unsigned int main_clock, // \arg peripheral clock
	const unsigned int baud_rate)  // \arg UART baudrate
{
	unsigned int baud_value = ((main_clock*10)/(baud_rate * 16));
	if ((baud_value % 10) >= 5)
		baud_value = (baud_value / 10) + 1;
	else
		baud_value /= 10;
	return baud_value;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_SetBaudrate
//* \brief Set the baudrate according to the CPU clock
//*----------------------------------------------------------------------------
void AT91F_US_SetBaudrate (
	AT91PS_USART pUSART,    // \arg pointer to a USART controller
	unsigned int mainClock, // \arg peripheral clock
	unsigned int speed)     // \arg UART baudrate
{
	//* Define the baud rate divisor register
	pUSART->US_BRGR = AT91F_US_Baudrate(mainClock, speed);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_SetTimeguard
//* \brief Set USART timeguard
//*----------------------------------------------------------------------------
void AT91F_US_SetTimeguard (
	AT91PS_USART pUSART,    // \arg pointer to a USART controller
	unsigned int timeguard) // \arg timeguard value
{
	//* Write the Timeguard Register
	pUSART->US_TTGR = timeguard ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_EnableIt
//* \brief Enable USART IT
//*----------------------------------------------------------------------------
void AT91F_US_EnableIt (
	AT91PS_USART pUSART, // \arg pointer to a USART controller
	unsigned int flag)   // \arg IT to be enabled
{
	//* Write to the IER register
	pUSART->US_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_DisableIt
//* \brief Disable USART IT
//*----------------------------------------------------------------------------
void AT91F_US_DisableIt (
	AT91PS_USART pUSART, // \arg pointer to a USART controller
	unsigned int flag)   // \arg IT to be disabled
{
	//* Write to the IER register
	pUSART->US_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_Configure
//* \brief Configure USART
//*----------------------------------------------------------------------------
void AT91F_US_Configure (
	AT91PS_USART pUSART,     // \arg pointer to a USART controller
	unsigned int mainClock,  // \arg peripheral clock
	unsigned int mode ,      // \arg mode Register to be programmed
	unsigned int baudRate ,  // \arg baudrate to be programmed
	unsigned int timeguard ) // \arg timeguard to be programmed
{
    //* Disable interrupts
    pUSART->US_IDR = (unsigned int) -1;

    //* Reset receiver and transmitter
    pUSART->US_CR = AT91C_US_RSTRX | AT91C_US_RSTTX | AT91C_US_RXDIS | AT91C_US_TXDIS ;

	//* Define the baud rate divisor register
	AT91F_US_SetBaudrate(pUSART, mainClock, baudRate);

	//* Write the Timeguard Register
	AT91F_US_SetTimeguard(pUSART, timeguard);

    //* Clear Transmit and Receive Counters
    AT91F_PDC_Open((AT91PS_PDC) &(pUSART->US_RPR));

    //* Define the USART mode
    pUSART->US_MR = mode  ;

}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_EnableRx
//* \brief Enable receiving characters
//*----------------------------------------------------------------------------
void AT91F_US_EnableRx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
    //* Enable receiver
    pUSART->US_CR = AT91C_US_RXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_EnableTx
//* \brief Enable sending characters
//*----------------------------------------------------------------------------
void AT91F_US_EnableTx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
    //* Enable  transmitter
    pUSART->US_CR = AT91C_US_TXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_ResetRx
//* \brief Reset Receiver and re-enable it
//*----------------------------------------------------------------------------
void AT91F_US_ResetRx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
	//* Reset receiver
	pUSART->US_CR = AT91C_US_RSTRX;
    //* Re-Enable receiver
    pUSART->US_CR = AT91C_US_RXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_ResetTx
//* \brief Reset Transmitter and re-enable it
//*----------------------------------------------------------------------------
void AT91F_US_ResetTx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
	//* Reset transmitter
	pUSART->US_CR = AT91C_US_RSTTX;
    //* Enable transmitter
    pUSART->US_CR = AT91C_US_TXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_DisableRx
//* \brief Disable Receiver
//*----------------------------------------------------------------------------
void AT91F_US_DisableRx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
    //* Disable receiver
    pUSART->US_CR = AT91C_US_RXDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_DisableTx
//* \brief Disable Transmitter
//*----------------------------------------------------------------------------
void AT91F_US_DisableTx (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
    //* Disable transmitter
    pUSART->US_CR = AT91C_US_TXDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_Close
//* \brief Close USART: disable IT disable receiver and transmitter, close PDC
//*----------------------------------------------------------------------------
void AT91F_US_Close (
	AT91PS_USART pUSART)     // \arg pointer to a USART controller
{
    //* Reset the baud rate divisor register
    pUSART->US_BRGR = 0 ;

    //* Reset the USART mode
    pUSART->US_MR = 0  ;

    //* Reset the Timeguard Register
    pUSART->US_TTGR = 0;

    //* Disable all interrupts
    pUSART->US_IDR = 0xFFFFFFFF ;

    //* Abort the Peripheral Data Transfers
    AT91F_PDC_Close((AT91PS_PDC) &(pUSART->US_RPR));

    //* Disable receiver and transmitter and stop any activity immediately
    pUSART->US_CR = AT91C_US_TXDIS | AT91C_US_RXDIS | AT91C_US_RSTTX | AT91C_US_RSTRX ;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_TxReady
//* \brief Return 1 if a character can be written in US_THR
//*----------------------------------------------------------------------------
unsigned int AT91F_US_TxReady (
	AT91PS_USART pUSART )     // \arg pointer to a USART controller
{
    return (pUSART->US_CSR & AT91C_US_TXRDY);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_RxReady
//* \brief Return 1 if a character can be read in US_RHR
//*----------------------------------------------------------------------------
unsigned int AT91F_US_RxReady (
	AT91PS_USART pUSART )     // \arg pointer to a USART controller
{
    return (pUSART->US_CSR & AT91C_US_RXRDY);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_Error
//* \brief Return the error flag
//*----------------------------------------------------------------------------
unsigned int AT91F_US_Error (
	AT91PS_USART pUSART )     // \arg pointer to a USART controller
{
    return (pUSART->US_CSR &
    	(AT91C_US_OVRE |  // Overrun error
    	 AT91C_US_FRAME | // Framing error
    	 AT91C_US_PARE));  // Parity error
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_PutChar
//* \brief Send a character,does not check if ready to send
//*----------------------------------------------------------------------------
void AT91F_US_PutChar (
	AT91PS_USART pUSART,
	int character )
{
    pUSART->US_THR = (character & 0x1FF);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_GetChar
//* \brief Receive a character,does not check if a character is available
//*----------------------------------------------------------------------------
int AT91F_US_GetChar (
	const AT91PS_USART pUSART)
{
    return((pUSART->US_RHR) & 0x1FF);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_SendFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initializaed with Next Buffer, 0 if PDC is busy
//*----------------------------------------------------------------------------
unsigned int AT91F_US_SendFrame(
	AT91PS_USART pUSART,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_SendFrame(
		(AT91PS_PDC) &(pUSART->US_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_ReceiveFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initializaed with Next Buffer, 0 if PDC is busy
//*----------------------------------------------------------------------------
unsigned int AT91F_US_ReceiveFrame (
	AT91PS_USART pUSART,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_ReceiveFrame(
		(AT91PS_PDC) &(pUSART->US_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US_SetIrdaFilter
//* \brief Set the value of IrDa filter tregister
//*----------------------------------------------------------------------------
void AT91F_US_SetIrdaFilter (
	AT91PS_USART pUSART,
	unsigned char value
)
{
	pUSART->US_IF = value;
}

/* *****************************************************************************
                SOFTWARE API FOR SSC
   ***************************************************************************** */
//* Define the standard I2S mode configuration

//* Configuration to set in the SSC Transmit Clock Mode Register
//* Parameters :  nb_bit_by_slot : 8, 16 or 32 bits
//* 			  nb_slot_by_frame : number of channels
#define 	AT91C_SSC_STTOUT      ((unsigned int) 0x1 << 15) // (SSC) Receive/Transmit Start Output Selection
#define 	AT91C_SSC_CKG_NONE    ((unsigned int) 0x0 <<  6) // (SSC) Receive/Transmit Clock Gating: None, continuous clock

#define AT91C_I2S_ASY_MASTER_TX_SETTING(nb_bit_by_slot, nb_slot_by_frame)( +\
									   AT91C_SSC_CKS_DIV   +\
                            		   AT91C_SSC_CKO_CONTINOUS      +\
                            		   AT91C_SSC_CKG_NONE    +\
                                       AT91C_SSC_START_FALL_RF +\
                           			   AT91C_SSC_STTOUT  +\
                            		   ((1<<16) & AT91C_SSC_STTDLY) +\
                            		   ((((nb_bit_by_slot*nb_slot_by_frame)/2)-1) <<24))


//* Configuration to set in the SSC Transmit Frame Mode Register
//* Parameters : nb_bit_by_slot : 8, 16 or 32 bits
//* 			 nb_slot_by_frame : number of channels
#define AT91C_I2S_ASY_TX_FRAME_SETTING(nb_bit_by_slot, nb_slot_by_frame)( +\
									(nb_bit_by_slot-1)  +\
                            		AT91C_SSC_MSBF   +\
                            		(((nb_slot_by_frame-1)<<8) & AT91C_SSC_DATNB)  +\
                            		(((nb_bit_by_slot-1)<<16) & AT91C_SSC_FSLEN) +\
                            		AT91C_SSC_FSOS_NEGATIVE)


//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_SetBaudrate
//* \brief Set the baudrate according to the CPU clock
//*----------------------------------------------------------------------------
void AT91F_SSC_SetBaudrate (
        AT91PS_SSC pSSC,        // \arg pointer to a SSC controller
        unsigned int mainClock, // \arg peripheral clock
        unsigned int speed)     // \arg SSC baudrate
{
        unsigned int baud_value;
        //* Define the baud rate divisor register
        if (speed == 0)
           baud_value = 0;
        else
        {
           baud_value = (unsigned int) (mainClock * 10)/(2*speed);
           if ((baud_value % 10) >= 5)
                  baud_value = (baud_value / 10) + 1;
           else
                  baud_value /= 10;
        }

        pSSC->SSC_CMR = baud_value;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_Configure
//* \brief Configure SSC
//*----------------------------------------------------------------------------
void AT91F_SSC_Configure (
             AT91PS_SSC pSSC,          // \arg pointer to a SSC controller
             unsigned int syst_clock,  // \arg System Clock Frequency
             unsigned int baud_rate,   // \arg Expected Baud Rate Frequency
             unsigned int clock_rx,    // \arg Receiver Clock Parameters
             unsigned int mode_rx,     // \arg mode Register to be programmed
             unsigned int clock_tx,    // \arg Transmitter Clock Parameters
             unsigned int mode_tx)     // \arg mode Register to be programmed
{
    //* Disable interrupts
	pSSC->SSC_IDR = (unsigned int) -1;

    //* Reset receiver and transmitter
	pSSC->SSC_CR = AT91C_SSC_SWRST | AT91C_SSC_RXDIS | AT91C_SSC_TXDIS ;

    //* Define the Clock Mode Register
	AT91F_SSC_SetBaudrate(pSSC, syst_clock, baud_rate);

     //* Write the Receive Clock Mode Register
	pSSC->SSC_RCMR =  clock_rx;

     //* Write the Transmit Clock Mode Register
	pSSC->SSC_TCMR =  clock_tx;

     //* Write the Receive Frame Mode Register
	pSSC->SSC_RFMR =  mode_rx;

     //* Write the Transmit Frame Mode Register
	pSSC->SSC_TFMR =  mode_tx;

    //* Clear Transmit and Receive Counters
	AT91F_PDC_Open((AT91PS_PDC) &(pSSC->SSC_RPR));


}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_EnableRx
//* \brief Enable receiving datas
//*----------------------------------------------------------------------------
void AT91F_SSC_EnableRx (
	AT91PS_SSC pSSC)     // \arg pointer to a SSC controller
{
    //* Enable receiver
    pSSC->SSC_CR = AT91C_SSC_RXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_DisableRx
//* \brief Disable receiving datas
//*----------------------------------------------------------------------------
void AT91F_SSC_DisableRx (
	AT91PS_SSC pSSC)     // \arg pointer to a SSC controller
{
    //* Disable receiver
    pSSC->SSC_CR = AT91C_SSC_RXDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_EnableTx
//* \brief Enable sending datas
//*----------------------------------------------------------------------------
void AT91F_SSC_EnableTx (
	AT91PS_SSC pSSC)     // \arg pointer to a SSC controller
{
    //* Enable  transmitter
    pSSC->SSC_CR = AT91C_SSC_TXEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_DisableTx
//* \brief Disable sending datas
//*----------------------------------------------------------------------------
void AT91F_SSC_DisableTx (
	AT91PS_SSC pSSC)     // \arg pointer to a SSC controller
{
    //* Disable  transmitter
    pSSC->SSC_CR = AT91C_SSC_TXDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_EnableIt
//* \brief Enable SSC IT
//*----------------------------------------------------------------------------
void AT91F_SSC_EnableIt (
	AT91PS_SSC pSSC, // \arg pointer to a SSC controller
	unsigned int flag)   // \arg IT to be enabled
{
	//* Write to the IER register
	pSSC->SSC_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_DisableIt
//* \brief Disable SSC IT
//*----------------------------------------------------------------------------
void AT91F_SSC_DisableIt (
	AT91PS_SSC pSSC, // \arg pointer to a SSC controller
	unsigned int flag)   // \arg IT to be disabled
{
	//* Write to the IDR register
	pSSC->SSC_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_ReceiveFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initialized with Next Buffer, 0 if PDC is busy
//*----------------------------------------------------------------------------
unsigned int AT91F_SSC_ReceiveFrame (
	AT91PS_SSC pSSC,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_ReceiveFrame(
		(AT91PS_PDC) &(pSSC->SSC_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_SendFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initialized with Next Buffer, 0 if PDC is busy
//*----------------------------------------------------------------------------
unsigned int AT91F_SSC_SendFrame(
	AT91PS_SSC pSSC,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_SendFrame(
		(AT91PS_PDC) &(pSSC->SSC_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_GetInterruptMaskStatus
//* \brief Return SSC Interrupt Mask Status
//*----------------------------------------------------------------------------
unsigned int AT91F_SSC_GetInterruptMaskStatus( // \return SSC Interrupt Mask Status
        AT91PS_SSC pSsc) // \arg  pointer to a SSC controller
{
        return pSsc->SSC_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC_IsInterruptMasked
//* \brief Test if SSC Interrupt is Masked 
//*----------------------------------------------------------------------------
int AT91F_SSC_IsInterruptMasked(
        AT91PS_SSC pSsc,   // \arg  pointer to a SSC controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_SSC_GetInterruptMaskStatus(pSsc) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR SPI
   ***************************************************************************** */
//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_CfgCs
//* \brief Configure SPI chip select register
//*----------------------------------------------------------------------------
void AT91F_SPI_CfgCs (
	AT91PS_SPI pSPI,     // pointer to a SPI controller
	int cs,     // SPI cs number (0 to 3)
 	int val)   //  chip select register
{
	//* Write to the CSR register
	*(pSPI->SPI_CSR + cs) = val;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_EnableIt
//* \brief Enable SPI interrupt
//*----------------------------------------------------------------------------
void AT91F_SPI_EnableIt (
	AT91PS_SPI pSPI,     // pointer to a SPI controller
	unsigned int flag)   // IT to be enabled
{
	//* Write to the IER register
	pSPI->SPI_IER = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_DisableIt
//* \brief Disable SPI interrupt
//*----------------------------------------------------------------------------
void AT91F_SPI_DisableIt (
	AT91PS_SPI pSPI, // pointer to a SPI controller
	unsigned int flag) // IT to be disabled
{
	//* Write to the IDR register
	pSPI->SPI_IDR = flag;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_Reset
//* \brief Reset the SPI controller
//*----------------------------------------------------------------------------
void AT91F_SPI_Reset (
	AT91PS_SPI pSPI // pointer to a SPI controller
	)
{
	//* Write to the CR register
	pSPI->SPI_CR = AT91C_SPI_SWRST;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_Enable
//* \brief Enable the SPI controller
//*----------------------------------------------------------------------------
void AT91F_SPI_Enable (
	AT91PS_SPI pSPI // pointer to a SPI controller
	)
{
	//* Write to the CR register
	pSPI->SPI_CR = AT91C_SPI_SPIEN;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_Disable
//* \brief Disable the SPI controller
//*----------------------------------------------------------------------------
void AT91F_SPI_Disable (
	AT91PS_SPI pSPI // pointer to a SPI controller
	)
{
	//* Write to the CR register
	pSPI->SPI_CR = AT91C_SPI_SPIDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_CfgMode
//* \brief Enable the SPI controller
//*----------------------------------------------------------------------------
void AT91F_SPI_CfgMode (
	AT91PS_SPI pSPI, // pointer to a SPI controller
	int mode)        // mode register 
{
	//* Write to the MR register
	pSPI->SPI_MR = mode;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_CfgPCS
//* \brief Switch to the correct PCS of SPI Mode Register : Fixed Peripheral Selected
//*----------------------------------------------------------------------------
void AT91F_SPI_CfgPCS (
	AT91PS_SPI pSPI, // pointer to a SPI controller
	char PCS_Device) // PCS of the Device
{	
 	//* Write to the MR register
	pSPI->SPI_MR &= 0xFFF0FFFF;
	pSPI->SPI_MR |= ( (PCS_Device<<16) & AT91C_SPI_PCS );
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_ReceiveFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initializaed with Next Buffer, 0 if PDC is busy
//*----------------------------------------------------------------------------
unsigned int AT91F_SPI_ReceiveFrame (
	AT91PS_SPI pSPI,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_ReceiveFrame(
		(AT91PS_PDC) &(pSPI->SPI_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_SendFrame
//* \brief Return 2 if PDC has been initialized with Buffer and Next Buffer, 1 if PDC has been initializaed with Next Buffer, 0 if PDC is bSPIy
//*----------------------------------------------------------------------------
unsigned int AT91F_SPI_SendFrame(
	AT91PS_SPI pSPI,
	char *pBuffer,
	unsigned int szBuffer,
	char *pNextBuffer,
	unsigned int szNextBuffer )
{
	return AT91F_PDC_SendFrame(
		(AT91PS_PDC) &(pSPI->SPI_RPR),
		pBuffer,
		szBuffer,
		pNextBuffer,
		szNextBuffer);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_Close
//* \brief Close SPI: disable IT disable transfert, close PDC
//*----------------------------------------------------------------------------
void AT91F_SPI_Close (
	AT91PS_SPI pSPI)     // \arg pointer to a SPI controller
{
    //* Reset all the Chip Select register
    pSPI->SPI_CSR[0] = 0 ;
    pSPI->SPI_CSR[1] = 0 ;
    pSPI->SPI_CSR[2] = 0 ;
    pSPI->SPI_CSR[3] = 0 ;

    //* Reset the SPI mode
    pSPI->SPI_MR = 0  ;

    //* Disable all interrupts
    pSPI->SPI_IDR = 0xFFFFFFFF ;

    //* Abort the Peripheral Data Transfers
    AT91F_PDC_Close((AT91PS_PDC) &(pSPI->SPI_RPR));

    //* Disable receiver and transmitter and stop any activity immediately
    pSPI->SPI_CR = AT91C_SPI_SPIDIS;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_PutChar
//* \brief Send a character,does not check if ready to send
//*----------------------------------------------------------------------------
void AT91F_SPI_PutChar (
	AT91PS_SPI pSPI,
	unsigned int character,
             unsigned int cs_number )
{
    unsigned int value_for_cs;
    value_for_cs = (~(1 << cs_number)) & 0xF;  //Place a zero among a 4 ONEs number
    pSPI->SPI_TDR = (character & 0xFFFF) | (value_for_cs << 16);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_GetChar
//* \brief Receive a character,does not check if a character is available
//*----------------------------------------------------------------------------
int AT91F_SPI_GetChar (
	const AT91PS_SPI pSPI)
{
    return((pSPI->SPI_RDR) & 0xFFFF);
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_GetInterruptMaskStatus
//* \brief Return SPI Interrupt Mask Status
//*----------------------------------------------------------------------------
unsigned int AT91F_SPI_GetInterruptMaskStatus( // \return SPI Interrupt Mask Status
        AT91PS_SPI pSpi) // \arg  pointer to a SPI controller
{
        return pSpi->SPI_IMR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI_IsInterruptMasked
//* \brief Test if SPI Interrupt is Masked 
//*----------------------------------------------------------------------------
int AT91F_SPI_IsInterruptMasked(
        AT91PS_SPI pSpi,   // \arg  pointer to a SPI controller
        unsigned int flag) // \arg  flag to be tested
{
        return (AT91F_SPI_GetInterruptMaskStatus(pSpi) & flag);
}

/* *****************************************************************************
                SOFTWARE API FOR LCDC
   ***************************************************************************** */
#define LCDC_HCLK_ENABLE    (1 << 17)
#define LCDC_CORECLK_ENABLE (1 << 12)
#define LCDC_PCKR_ID        4



//*----------------------------------------------------------------------------
//* \fn    AT91F_LCDC_DMA_CfgPMC
//* \brief Enable DMA Clock
//*----------------------------------------------------------------------------
void AT91F_LCDC_DMA_CfgPMC ( void )
{
        AT91PS_PMC    pAPMC        = (AT91PS_PMC) AT91C_BASE_PMC;
        pAPMC->PMC_SCER = LCDC_HCLK_ENABLE;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_LCDC_CORE_CfgPMC
//* \brief Enable LCD Controller Clock
//*----------------------------------------------------------------------------
void AT91F_LCDC_CORE_CfgPMC ( unsigned int div_ratio )
{
        AT91PS_PMC    pAPMC        = (AT91PS_PMC) AT91C_BASE_PMC;

        pAPMC->PMC_PCKR[LCDC_PCKR_ID] = (AT91C_PMC_CSS_MAIN_CLK | div_ratio);
        pAPMC->PMC_SCER = LCDC_CORECLK_ENABLE;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_CfgPMC
//* \brief Enable Peripheral clock in PMC for  DBGU
//*----------------------------------------------------------------------------
void AT91F_DBGU_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_DBGU_CfgPIO
//* \brief Configure PIO controllers to drive DBGU signals
//*----------------------------------------------------------------------------
void AT91F_DBGU_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA10_DTXD    ) |
		((unsigned int) AT91C_PA9_DRXD    ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UHP_CfgPMC
//* \brief Enable Peripheral clock in PMC for  UHP
//*----------------------------------------------------------------------------
void AT91F_UHP_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_UHP));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SHDWC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  SHDWC
//*----------------------------------------------------------------------------
void AT91F_SHDWC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI0_CfgPMC
//* \brief Enable Peripheral clock in PMC for  SPI0
//*----------------------------------------------------------------------------
void AT91F_SPI0_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SPI0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI0_CfgPIO
//* \brief Configure PIO controllers to drive SPI0 signals
//*----------------------------------------------------------------------------
void AT91F_SPI0_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA1_SPI0_MOSI) |
		((unsigned int) AT91C_PA3_SPI0_NPCS0) |
		((unsigned int) AT91C_PA4_SPI0_NPCS1) |
		((unsigned int) AT91C_PA0_SPI0_MISO) |
		((unsigned int) AT91C_PA5_SPI0_NPCS2) |
		((unsigned int) AT91C_PA2_SPI0_SPCK) |
		((unsigned int) AT91C_PA6_SPI0_NPCS3), // Peripheral A
		
		((unsigned int) AT91C_PA27_SPI0_NPCS1) |
		((unsigned int) AT91C_PA28_SPI0_NPCS2) |
		((unsigned int) AT91C_PA29_SPI0_NPCS3)); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI1_CfgPMC
//* \brief Enable Peripheral clock in PMC for  SPI1
//*----------------------------------------------------------------------------
void AT91F_SPI1_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SPI1));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SPI1_CfgPIO
//* \brief Configure PIO controllers to drive SPI1 signals
//*----------------------------------------------------------------------------
void AT91F_SPI1_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PA24_SPI1_NPCS1) |
		((unsigned int) AT91C_PA25_SPI1_NPCS2) |
		((unsigned int) AT91C_PA26_SPI1_NPCS3)); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB31_SPI1_MOSI) |
		((unsigned int) AT91C_PB28_SPI1_NPCS0) |
		((unsigned int) AT91C_PB30_SPI1_MISO) |
		((unsigned int) AT91C_PB27_SPI1_NPCS1) |
		((unsigned int) AT91C_PB29_SPI1_SPCK), // Peripheral A
		0); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PC14_SPI1_NPCS2) |
		((unsigned int) AT91C_PC15_SPI1_NPCS3)); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC0_CfgPMC
//* \brief Enable Peripheral clock in PMC for  SSC0
//*----------------------------------------------------------------------------
void AT91F_SSC0_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SSC0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC0_CfgPIO
//* \brief Configure PIO controllers to drive SSC0 signals
//*----------------------------------------------------------------------------
void AT91F_SSC0_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB26_RF0     ) |
		((unsigned int) AT91C_PB22_TK0     ) |
		((unsigned int) AT91C_PB25_RK0     ) |
		((unsigned int) AT91C_PB23_TD0     ) |
		((unsigned int) AT91C_PB24_RD0     ) |
		((unsigned int) AT91C_PB21_TF0     ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC1_CfgPMC
//* \brief Enable Peripheral clock in PMC for  SSC1
//*----------------------------------------------------------------------------
void AT91F_SSC1_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SSC1));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC1_CfgPIO
//* \brief Configure PIO controllers to drive SSC1 signals
//*----------------------------------------------------------------------------
void AT91F_SSC1_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PA22_RF1     ) |
		((unsigned int) AT91C_PA18_TK1     ) |
		((unsigned int) AT91C_PA21_RK1     ) |
		((unsigned int) AT91C_PA19_TD1     ) |
		((unsigned int) AT91C_PA20_RD1     ) |
		((unsigned int) AT91C_PA17_TF1     )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC2_CfgPMC
//* \brief Enable Peripheral clock in PMC for  SSC2
//*----------------------------------------------------------------------------
void AT91F_SSC2_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SSC2));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SSC2_CfgPIO
//* \brief Configure PIO controllers to drive SSC2 signals
//*----------------------------------------------------------------------------
void AT91F_SSC2_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PC25_TF2     ) |
		((unsigned int) AT91C_PC30_RF2     ) |
		((unsigned int) AT91C_PC26_TK2     ) |
		((unsigned int) AT91C_PC29_RK2     ) |
		((unsigned int) AT91C_PC27_TD2     ) |
		((unsigned int) AT91C_PC28_RD2     )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_LCDC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  LCDC
//*----------------------------------------------------------------------------
void AT91F_LCDC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_LCDC));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_LCDC_CfgPIO
//* \brief Configure PIO controllers to drive LCDC signals
//*----------------------------------------------------------------------------
void AT91F_LCDC_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		((unsigned int) AT91C_PB0_LCDVSYNC) |
		((unsigned int) AT91C_PB4_LCDCC   ) |
		((unsigned int) AT91C_PB15_LCDD10  ) |
		((unsigned int) AT91C_PB16_LCDD11  ) |
		((unsigned int) AT91C_PB17_LCDD12  ) |
		((unsigned int) AT91C_PB18_LCDD13  ) |
		((unsigned int) AT91C_PB19_LCDD14  ) |
		((unsigned int) AT91C_PB20_LCDD15  ) |
		((unsigned int) AT91C_PB5_LCDD0   ) |
		((unsigned int) AT91C_PB6_LCDD1   ) |
		((unsigned int) AT91C_PB7_LCDD2   ) |
		((unsigned int) AT91C_PB3_LCDDEN  ) |
		((unsigned int) AT91C_PB8_LCDD3   ) |
		((unsigned int) AT91C_PB9_LCDD4   ) |
		((unsigned int) AT91C_PB10_LCDD5   ) |
		((unsigned int) AT91C_PB11_LCDD6   ) |
		((unsigned int) AT91C_PB2_LCDDOTCK) |
		((unsigned int) AT91C_PB12_LCDD7   ) |
		((unsigned int) AT91C_PB1_LCDHSYNC) |
		((unsigned int) AT91C_PB13_LCDD8   ) |
		((unsigned int) AT91C_PB14_LCDD9   ), // Peripheral A
		((unsigned int) AT91C_PB25_LCDD20  ) |
		((unsigned int) AT91C_PB26_LCDD21  ) |
		((unsigned int) AT91C_PB27_LCDD22  ) |
		((unsigned int) AT91C_PB28_LCDD23  ) |
		((unsigned int) AT91C_PB21_LCDD16  ) |
		((unsigned int) AT91C_PB22_LCDD17  ) |
		((unsigned int) AT91C_PB4_LCDD2   ) |
		((unsigned int) AT91C_PB23_LCDD18  ) |
		((unsigned int) AT91C_PB24_LCDD19  )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_LCDC_16B_TFT_CfgPIO
//* \brief Configure PIO controllers to drive LCDC_16B_TFT signals
//*----------------------------------------------------------------------------
void AT91F_LCDC_16B_TFT_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PB10_LCDD10  ) |
		((unsigned int) AT91C_PB11_LCDD11  ) |
		((unsigned int) AT91C_PB17_LCDD20  ) |
		((unsigned int) AT91C_PB12_LCDD12  ) |
		((unsigned int) AT91C_PB18_LCDD21  ) |
		((unsigned int) AT91C_PB13_LCDD13  ) |
		((unsigned int) AT91C_PB19_LCDD22  ) |
		((unsigned int) AT91C_PB14_LCDD14  ) |
		((unsigned int) AT91C_PB20_LCDD23  ) |
		((unsigned int) AT91C_PB15_LCDD15  ) |
		((unsigned int) AT91C_PB5_LCDD3   ) |
		((unsigned int) AT91C_PB16_LCDD19  ) |
		((unsigned int) AT91C_PB6_LCDD4   ) |
		((unsigned int) AT91C_PB7_LCDD5   ) |
		((unsigned int) AT91C_PB8_LCDD6   ) |
		((unsigned int) AT91C_PB9_LCDD7   )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC0_CfgPMC
//* \brief Enable Peripheral clock in PMC for  TC0
//*----------------------------------------------------------------------------
void AT91F_TC0_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_TC0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC0_CfgPIO
//* \brief Configure PIO controllers to drive TC0 signals
//*----------------------------------------------------------------------------
void AT91F_TC0_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PC19_TIOA0   ) |
		((unsigned int) AT91C_PC20_TIOB0   ) |
		((unsigned int) AT91C_PC16_TCLK0   )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC1_CfgPMC
//* \brief Enable Peripheral clock in PMC for  TC1
//*----------------------------------------------------------------------------
void AT91F_TC1_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_TC1));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC1_CfgPIO
//* \brief Configure PIO controllers to drive TC1 signals
//*----------------------------------------------------------------------------
void AT91F_TC1_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PC21_TIOA1   ) |
		((unsigned int) AT91C_PC22_TIOB1   ) |
		((unsigned int) AT91C_PC17_TCLK1   )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC2_CfgPMC
//* \brief Enable Peripheral clock in PMC for  TC2
//*----------------------------------------------------------------------------
void AT91F_TC2_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_TC2));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TC2_CfgPIO
//* \brief Configure PIO controllers to drive TC2 signals
//*----------------------------------------------------------------------------
void AT91F_TC2_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PC23_TIOA2   ) |
		((unsigned int) AT91C_PC24_TIOB2   ) |
		((unsigned int) AT91C_PC18_TCLK2   )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PITC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  PITC
//*----------------------------------------------------------------------------
void AT91F_PITC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  PMC
//*----------------------------------------------------------------------------
void AT91F_PMC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PMC_CfgPIO
//* \brief Configure PIO controllers to drive PMC signals
//*----------------------------------------------------------------------------
void AT91F_PMC_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PA8_PCK1    ) |
		((unsigned int) AT91C_PA9_PCK2    ) |
		((unsigned int) AT91C_PA10_PCK3    ) |
		((unsigned int) AT91C_PA7_PCK0    )); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PB31_PCK2    ) |
		((unsigned int) AT91C_PB2_PCK0    )); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PC31_PCK1    ) |
		((unsigned int) AT91C_PC8_PCK2    ) |
		((unsigned int) AT91C_PC9_PCK3    )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RSTC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  RSTC
//*----------------------------------------------------------------------------
void AT91F_RSTC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIOA_CfgPMC
//* \brief Enable Peripheral clock in PMC for  PIOA
//*----------------------------------------------------------------------------
void AT91F_PIOA_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_PIOA));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIOB_CfgPMC
//* \brief Enable Peripheral clock in PMC for  PIOB
//*----------------------------------------------------------------------------
void AT91F_PIOB_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_PIOB));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_PIOC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  PIOC
//*----------------------------------------------------------------------------
void AT91F_PIOC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_PIOC));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_RTTC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  RTTC
//*----------------------------------------------------------------------------
void AT91F_RTTC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_CfgPMC
//* \brief Enable Peripheral clock in PMC for  TWI
//*----------------------------------------------------------------------------
void AT91F_TWI_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_TWI));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_TWI_CfgPIO
//* \brief Configure PIO controllers to drive TWI signals
//*----------------------------------------------------------------------------
void AT91F_TWI_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		((unsigned int) AT91C_PA7_TWD     ) |
		((unsigned int) AT91C_PA8_TWCK    ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SDRAMC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  SDRAMC
//*----------------------------------------------------------------------------
void AT91F_SDRAMC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SDRAMC_CfgPIO
//* \brief Configure PIO controllers to drive SDRAMC signals
//*----------------------------------------------------------------------------
void AT91F_SDRAMC_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		((unsigned int) AT91C_PC20_D20     ) |
		((unsigned int) AT91C_PC21_D21     ) |
		((unsigned int) AT91C_PC30_D30     ) |
		((unsigned int) AT91C_PC22_D22     ) |
		((unsigned int) AT91C_PC31_D31     ) |
		((unsigned int) AT91C_PC23_D23     ) |
		((unsigned int) AT91C_PC16_D16     ) |
		((unsigned int) AT91C_PC24_D24     ) |
		((unsigned int) AT91C_PC17_D17     ) |
		((unsigned int) AT91C_PC25_D25     ) |
		((unsigned int) AT91C_PC18_D18     ) |
		((unsigned int) AT91C_PC26_D26     ) |
		((unsigned int) AT91C_PC19_D19     ) |
		((unsigned int) AT91C_PC27_D27     ) |
		((unsigned int) AT91C_PC28_D28     ) |
		((unsigned int) AT91C_PC29_D29     ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US0_CfgPMC
//* \brief Enable Peripheral clock in PMC for  US0
//*----------------------------------------------------------------------------
void AT91F_US0_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_US0));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US0_CfgPIO
//* \brief Configure PIO controllers to drive US0 signals
//*----------------------------------------------------------------------------
void AT91F_US0_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PA23_RTS0    )); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		((unsigned int) AT91C_PC9_RXD0    ) |
		((unsigned int) AT91C_PC8_TXD0    ) |
		((unsigned int) AT91C_PC10_RTS0    ) |
		((unsigned int) AT91C_PC11_CTS0    ), // Peripheral A
		((unsigned int) AT91C_PC10_SCK0    )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US1_CfgPMC
//* \brief Enable Peripheral clock in PMC for  US1
//*----------------------------------------------------------------------------
void AT91F_US1_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_US1));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US1_CfgPIO
//* \brief Configure PIO controllers to drive US1 signals
//*----------------------------------------------------------------------------
void AT91F_US1_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PA12_RTS1    ) |
		((unsigned int) AT91C_PA11_SCK1    ) |
		((unsigned int) AT91C_PA13_CTS1    )); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		((unsigned int) AT91C_PC13_RXD1    ) |
		((unsigned int) AT91C_PC12_TXD1    ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US2_CfgPMC
//* \brief Enable Peripheral clock in PMC for  US2
//*----------------------------------------------------------------------------
void AT91F_US2_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_US2));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_US2_CfgPIO
//* \brief Configure PIO controllers to drive US2 signals
//*----------------------------------------------------------------------------
void AT91F_US2_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PA16_CTS2    ) |
		((unsigned int) AT91C_PA15_RTS2    ) |
		((unsigned int) AT91C_PA14_SCK2    )); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		((unsigned int) AT91C_PC15_RXD2    ) |
		((unsigned int) AT91C_PC14_TXD2    ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_CfgPMC
//* \brief Enable Peripheral clock in PMC for  MCI
//*----------------------------------------------------------------------------
void AT91F_MCI_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_MCI));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_MCI_CfgPIO
//* \brief Configure PIO controllers to drive MCI signals
//*----------------------------------------------------------------------------
void AT91F_MCI_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOA, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PA6_MCDA3   ) |
		((unsigned int) AT91C_PA1_MCCDA   ) |
		((unsigned int) AT91C_PA0_MCDA0   ) |
		((unsigned int) AT91C_PA4_MCDA1   ) |
		((unsigned int) AT91C_PA2_MCCK    ) |
		((unsigned int) AT91C_PA5_MCDA2   )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_WDTC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  WDTC
//*----------------------------------------------------------------------------
void AT91F_WDTC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_SYS));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_SMC_CfgPIO
//* \brief Configure PIO controllers to drive SMC signals
//*----------------------------------------------------------------------------
void AT91F_SMC_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		((unsigned int) AT91C_PC5_NCS5_CFCS1) |
		((unsigned int) AT91C_PC4_NCS4_CFCS0) |
		((unsigned int) AT91C_PC3_A25_CFRNW) |
		((unsigned int) AT91C_PC6_CFCE1   ) |
		((unsigned int) AT91C_PC7_CFCE2   ) |
		((unsigned int) AT91C_PC1_SMWE    ) |
		((unsigned int) AT91C_PC0_SMOE    ), // Peripheral A
		0); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_UDP_CfgPMC
//* \brief Enable Peripheral clock in PMC for  UDP
//*----------------------------------------------------------------------------
void AT91F_UDP_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_UDP));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_CfgPMC
//* \brief Enable Peripheral clock in PMC for  AIC
//*----------------------------------------------------------------------------
void AT91F_AIC_CfgPMC (void)
{
	AT91F_PMC_EnablePeriphClock(
		AT91C_BASE_PMC, // PIO controller base address
		((unsigned int) 1 << AT91C_ID_IRQ0) |
		((unsigned int) 1 << AT91C_ID_IRQ1) |
		((unsigned int) 1 << AT91C_ID_FIQ) |
		((unsigned int) 1 << AT91C_ID_IRQ2));
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_AIC_CfgPIO
//* \brief Configure PIO controllers to drive AIC signals
//*----------------------------------------------------------------------------
void AT91F_AIC_CfgPIO (void)
{
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOB, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PB30_IRQ1    ) |
		((unsigned int) AT91C_PB29_IRQ2    )); // Peripheral B
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, // PIO controller base address
		0, // Peripheral A
		((unsigned int) AT91C_PC2_IRQ0    )); // Peripheral B
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_NAND_CfgPIO
//* \brief Configure PIO controllers to drive NAND Flash signals
//*----------------------------------------------------------------------------
void AT91F_NAND_CfgPIO(int bus_width)
{
cyg_uint32 width;
	
	// enable peripheral clock for PIOC
	//=================================	
	AT91F_PIOC_CfgPMC();
	
	// initialite CS3, 16 bit access
	//=============================
	AT91C_BASE_SMC->SMC_SETUP3 = 3*AT91C_SMC_NWESETUP +				// NWE setup length
								 3*AT91C_SMC_NCSSETUPWR +			// NCS setup length in write access
								 3*AT91C_SMC_NRDSETUP + 			// NRD setup length
								 3*AT91C_SMC_NCSSETUPRD;			// NCS setup length in read access
								 
	AT91C_BASE_SMC->SMC_PULSE3 = 4*AT91C_SMC_NWEPULSE +				// NWE pulse length
								 4*AT91C_SMC_NCSPULSEWR +			// NCS pulse length in write access
								 4*AT91C_SMC_NRDPULSE +				// NRD pulse length
								 4*AT91C_SMC_NCSPULSERD;			// NCS pulse length in read access
								 
	AT91C_BASE_SMC->SMC_CYCLE3 = 8*AT91C_SMC_NWECYCLE +				// Total Write cycle length
								 8*AT91C_SMC_NRDCYCLE;				// Total Read cycle length
	
	width = (bus_width == 8) ? AT91C_SMC_DBW_WIDTH_EIGTH_BITS : AT91C_SMC_DBW_WIDTH_SIXTEEN_BITS;
	
	AT91C_BASE_SMC->SMC_CTRL3 = AT91C_SMC_READMODE +
								AT91C_SMC_WRITEMODE +				
								AT91C_SMC_NWAITM_NWAIT_DISABLE +	// NWAIT disabled	
								width +								// 16-bit bus
								6*AT91C_SMC_TDF +					// data float time
								AT91C_SMC_TDFEN;					// TDF optimization enabled
	

	
	// Chip Select 3 is used for NAND operation
	//=========================================
	AT91C_BASE_MATRIX->MATRIX_EBICSA |= AT91C_MATRIX_CS3A_SM;
	
	// Configure PIO controllers to periph mode
	AT91F_PIO_CfgPeriph(
		AT91C_BASE_PIOC, 							// PIO controller base address
		((unsigned int) AT91C_PC0_SMOE ) |
		((unsigned int) AT91C_PC1_SMWE ), 			// Peripheral A
		0); 										// Peripheral B

	// Use PC14 for Chip Select
	//========================
	AT91F_PIO_CfgOutput( AT91C_BASE_PIOC, AT91C_PIO_PC14 );

	// set Chip Select to '1'
	//=======================
	AT91F_PIO_SetOutput( AT91C_BASE_PIOC, AT91C_PIO_PC14);
		
	// Use PA16 for Ready/Busy
	//========================
	AT91F_PIO_CfgInput( AT91C_BASE_PIOA, AT91C_PIO_PA16 );
}


void AT91F_NAND_assert_CS( void )
{
	AT91F_PIO_ClearOutput(AT91C_BASE_PIOC, AT91C_PIO_PC14);
}

void AT91F_NAND_release_CS( void )
{
	AT91F_PIO_SetOutput( AT91C_BASE_PIOC, AT91C_PIO_PC14);
}

int AT91F_NAND_is_BUSY_asserted( void )
{
	return AT91F_PIO_GetInput(AT91C_BASE_PIOA) & (1<<16) ? 0 : 1;
}

