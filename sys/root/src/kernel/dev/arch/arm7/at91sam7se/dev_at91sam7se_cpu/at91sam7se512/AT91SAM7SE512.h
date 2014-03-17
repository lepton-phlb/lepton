/*
The contents of this file are subject to the Mozilla Public License Version 1.1
(the "License"); you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.mozilla.org/MPL/

Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License for the
specific language governing rights and limitations under the License.

The Original Code is Lepton.

The Initial Developer of the Original Code is Chauvin-Arnoux.
Portions created by Chauvin-Arnoux are Copyright (C) 2011. All Rights Reserved.

Alternatively, the contents of this file may be used under the terms of the eCos GPL license
(the  [eCos GPL] License), in which case the provisions of [eCos GPL] License are applicable
instead of those above. If you wish to allow use of your version of this file only under the
terms of the [eCos GPL] License and not to allow others to use your version of this file under
the MPL, indicate your decision by deleting  the provisions above and replace
them with the notice and other provisions required by the [eCos GPL] License.
If you do not delete the provisions above, a recipient may use your version of this file under
either the MPL or the [eCos GPL] License."
*/

#ifndef AT91SAM7SE512_H
#define AT91SAM7SE512_H

typedef volatile unsigned int AT91_REG; // Hardware register definition

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR System Peripherals
// *****************************************************************************
typedef struct _AT91S_SYS {
   AT91_REG AIC_SMR[32];                // Source Mode Register
   AT91_REG AIC_SVR[32];                // Source Vector Register
   AT91_REG AIC_IVR;                    // IRQ Vector Register
   AT91_REG AIC_FVR;                    // FIQ Vector Register
   AT91_REG AIC_ISR;                    // Interrupt Status Register
   AT91_REG AIC_IPR;                    // Interrupt Pending Register
   AT91_REG AIC_IMR;                    // Interrupt Mask Register
   AT91_REG AIC_CISR;                   // Core Interrupt Status Register
   AT91_REG Reserved0[2];               //
   AT91_REG AIC_IECR;                   // Interrupt Enable Command Register
   AT91_REG AIC_IDCR;                   // Interrupt Disable Command Register
   AT91_REG AIC_ICCR;                   // Interrupt Clear Command Register
   AT91_REG AIC_ISCR;                   // Interrupt Set Command Register
   AT91_REG AIC_EOICR;                  // End of Interrupt Command Register
   AT91_REG AIC_SPU;                    // Spurious Vector Register
   AT91_REG AIC_DCR;                    // Debug Control Register (Protect)
   AT91_REG Reserved1[1];               //
   AT91_REG AIC_FFER;                   // Fast Forcing Enable Register
   AT91_REG AIC_FFDR;                   // Fast Forcing Disable Register
   AT91_REG AIC_FFSR;                   // Fast Forcing Status Register
   AT91_REG Reserved2[45];                      //
   AT91_REG DBGU_CR;                    // Control Register
   AT91_REG DBGU_MR;                    // Mode Register
   AT91_REG DBGU_IER;                   // Interrupt Enable Register
   AT91_REG DBGU_IDR;                   // Interrupt Disable Register
   AT91_REG DBGU_IMR;                   // Interrupt Mask Register
   AT91_REG DBGU_CSR;                   // Channel Status Register
   AT91_REG DBGU_RHR;                   // Receiver Holding Register
   AT91_REG DBGU_THR;                   // Transmitter Holding Register
   AT91_REG DBGU_BRGR;                  // Baud Rate Generator Register
   AT91_REG Reserved3[7];               //
   AT91_REG DBGU_CIDR;                  // Chip ID Register
   AT91_REG DBGU_EXID;                  // Chip ID Extension Register
   AT91_REG DBGU_FNTR;                  // Force NTRST Register
   AT91_REG Reserved4[45];                      //
   AT91_REG DBGU_RPR;                   // Receive Pointer Register
   AT91_REG DBGU_RCR;                   // Receive Counter Register
   AT91_REG DBGU_TPR;                   // Transmit Pointer Register
   AT91_REG DBGU_TCR;                   // Transmit Counter Register
   AT91_REG DBGU_RNPR;                  // Receive Next Pointer Register
   AT91_REG DBGU_RNCR;                  // Receive Next Counter Register
   AT91_REG DBGU_TNPR;                  // Transmit Next Pointer Register
   AT91_REG DBGU_TNCR;                  // Transmit Next Counter Register
   AT91_REG DBGU_PTCR;                  // PDC Transfer Control Register
   AT91_REG DBGU_PTSR;                  // PDC Transfer Status Register
   AT91_REG Reserved5[54];                      //
   AT91_REG PIOA_PER;                   // PIO Enable Register
   AT91_REG PIOA_PDR;                   // PIO Disable Register
   AT91_REG PIOA_PSR;                   // PIO Status Register
   AT91_REG Reserved6[1];               //
   AT91_REG PIOA_OER;                   // Output Enable Register
   AT91_REG PIOA_ODR;                   // Output Disable Registerr
   AT91_REG PIOA_OSR;                   // Output Status Register
   AT91_REG Reserved7[1];               //
   AT91_REG PIOA_IFER;                  // Input Filter Enable Register
   AT91_REG PIOA_IFDR;                  // Input Filter Disable Register
   AT91_REG PIOA_IFSR;                  // Input Filter Status Register
   AT91_REG Reserved8[1];               //
   AT91_REG PIOA_SODR;                  // Set Output Data Register
   AT91_REG PIOA_CODR;                  // Clear Output Data Register
   AT91_REG PIOA_ODSR;                  // Output Data Status Register
   AT91_REG PIOA_PDSR;                  // Pin Data Status Register
   AT91_REG PIOA_IER;                   // Interrupt Enable Register
   AT91_REG PIOA_IDR;                   // Interrupt Disable Register
   AT91_REG PIOA_IMR;                   // Interrupt Mask Register
   AT91_REG PIOA_ISR;                   // Interrupt Status Register
   AT91_REG PIOA_MDER;                  // Multi-driver Enable Register
   AT91_REG PIOA_MDDR;                  // Multi-driver Disable Register
   AT91_REG PIOA_MDSR;                  // Multi-driver Status Register
   AT91_REG Reserved9[1];               //
   AT91_REG PIOA_PPUDR;                 // Pull-up Disable Register
   AT91_REG PIOA_PPUER;                 // Pull-up Enable Register
   AT91_REG PIOA_PPUSR;                 // Pull-up Status Register
   AT91_REG Reserved10[1];                      //
   AT91_REG PIOA_ASR;                   // Select A Register
   AT91_REG PIOA_BSR;                   // Select B Register
   AT91_REG PIOA_ABSR;                  // AB Select Status Register
   AT91_REG Reserved11[9];                      //
   AT91_REG PIOA_OWER;                  // Output Write Enable Register
   AT91_REG PIOA_OWDR;                  // Output Write Disable Register
   AT91_REG PIOA_OWSR;                  // Output Write Status Register
   AT91_REG Reserved12[85];                     //
   AT91_REG PIOB_PER;                   // PIO Enable Register
   AT91_REG PIOB_PDR;                   // PIO Disable Register
   AT91_REG PIOB_PSR;                   // PIO Status Register
   AT91_REG Reserved13[1];                      //
   AT91_REG PIOB_OER;                   // Output Enable Register
   AT91_REG PIOB_ODR;                   // Output Disable Registerr
   AT91_REG PIOB_OSR;                   // Output Status Register
   AT91_REG Reserved14[1];                      //
   AT91_REG PIOB_IFER;                  // Input Filter Enable Register
   AT91_REG PIOB_IFDR;                  // Input Filter Disable Register
   AT91_REG PIOB_IFSR;                  // Input Filter Status Register
   AT91_REG Reserved15[1];                      //
   AT91_REG PIOB_SODR;                  // Set Output Data Register
   AT91_REG PIOB_CODR;                  // Clear Output Data Register
   AT91_REG PIOB_ODSR;                  // Output Data Status Register
   AT91_REG PIOB_PDSR;                  // Pin Data Status Register
   AT91_REG PIOB_IER;                   // Interrupt Enable Register
   AT91_REG PIOB_IDR;                   // Interrupt Disable Register
   AT91_REG PIOB_IMR;                   // Interrupt Mask Register
   AT91_REG PIOB_ISR;                   // Interrupt Status Register
   AT91_REG PIOB_MDER;                  // Multi-driver Enable Register
   AT91_REG PIOB_MDDR;                  // Multi-driver Disable Register
   AT91_REG PIOB_MDSR;                  // Multi-driver Status Register
   AT91_REG Reserved16[1];                      //
   AT91_REG PIOB_PPUDR;                 // Pull-up Disable Register
   AT91_REG PIOB_PPUER;                 // Pull-up Enable Register
   AT91_REG PIOB_PPUSR;                 // Pull-up Status Register
   AT91_REG Reserved17[1];                      //
   AT91_REG PIOB_ASR;                   // Select A Register
   AT91_REG PIOB_BSR;                   // Select B Register
   AT91_REG PIOB_ABSR;                  // AB Select Status Register
   AT91_REG Reserved18[9];                      //
   AT91_REG PIOB_OWER;                  // Output Write Enable Register
   AT91_REG PIOB_OWDR;                  // Output Write Disable Register
   AT91_REG PIOB_OWSR;                  // Output Write Status Register
   AT91_REG Reserved19[341];                    //
   AT91_REG PMC_SCER;                   // System Clock Enable Register
   AT91_REG PMC_SCDR;                   // System Clock Disable Register
   AT91_REG PMC_SCSR;                   // System Clock Status Register
   AT91_REG Reserved20[1];                      //
   AT91_REG PMC_PCER;                   // Peripheral Clock Enable Register
   AT91_REG PMC_PCDR;                   // Peripheral Clock Disable Register
   AT91_REG PMC_PCSR;                   // Peripheral Clock Status Register
   AT91_REG Reserved21[1];                      //
   AT91_REG PMC_MOR;                    // Main Oscillator Register
   AT91_REG PMC_MCFR;                   // Main Clock  Frequency Register
   AT91_REG Reserved22[1];                      //
   AT91_REG PMC_PLLR;                   // PLL Register
   AT91_REG PMC_MCKR;                   // Master Clock Register
   AT91_REG Reserved23[3];                      //
   AT91_REG PMC_PCKR[4];                // Programmable Clock Register
   AT91_REG Reserved24[4];                      //
   AT91_REG PMC_IER;                    // Interrupt Enable Register
   AT91_REG PMC_IDR;                    // Interrupt Disable Register
   AT91_REG PMC_SR;                     // Status Register
   AT91_REG PMC_IMR;                    // Interrupt Mask Register
   AT91_REG Reserved25[36];                     //
   AT91_REG RSTC_RCR;                   // Reset Control Register
   AT91_REG RSTC_RSR;                   // Reset Status Register
   AT91_REG RSTC_RMR;                   // Reset Mode Register
   AT91_REG Reserved26[5];                      //
   AT91_REG RTTC_RTMR;                  // Real-time Mode Register
   AT91_REG RTTC_RTAR;                  // Real-time Alarm Register
   AT91_REG RTTC_RTVR;                  // Real-time Value Register
   AT91_REG RTTC_RTSR;                  // Real-time Status Register
   AT91_REG PITC_PIMR;                  // Period Interval Mode Register
   AT91_REG PITC_PISR;                  // Period Interval Status Register
   AT91_REG PITC_PIVR;                  // Period Interval Value Register
   AT91_REG PITC_PIIR;                  // Period Interval Image Register
   AT91_REG WDTC_WDCR;                  // Watchdog Control Register
   AT91_REG WDTC_WDMR;                  // Watchdog Mode Register
   AT91_REG WDTC_WDSR;                  // Watchdog Status Register
   AT91_REG Reserved27[5];                      //
   AT91_REG VREG_MR;                    // Voltage Regulator Mode Register
} AT91S_SYS, *AT91PS_SYS;


// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Advanced Interrupt Controller
// *****************************************************************************
typedef struct _AT91S_AIC {
   AT91_REG AIC_SMR[32];                // Source Mode Register
   AT91_REG AIC_SVR[32];                // Source Vector Register
   AT91_REG AIC_IVR;                    // IRQ Vector Register
   AT91_REG AIC_FVR;                    // FIQ Vector Register
   AT91_REG AIC_ISR;                    // Interrupt Status Register
   AT91_REG AIC_IPR;                    // Interrupt Pending Register
   AT91_REG AIC_IMR;                    // Interrupt Mask Register
   AT91_REG AIC_CISR;                   // Core Interrupt Status Register
   AT91_REG Reserved0[2];               //
   AT91_REG AIC_IECR;                   // Interrupt Enable Command Register
   AT91_REG AIC_IDCR;                   // Interrupt Disable Command Register
   AT91_REG AIC_ICCR;                   // Interrupt Clear Command Register
   AT91_REG AIC_ISCR;                   // Interrupt Set Command Register
   AT91_REG AIC_EOICR;                  // End of Interrupt Command Register
   AT91_REG AIC_SPU;                    // Spurious Vector Register
   AT91_REG AIC_DCR;                    // Debug Control Register (Protect)
   AT91_REG Reserved1[1];               //
   AT91_REG AIC_FFER;                   // Fast Forcing Enable Register
   AT91_REG AIC_FFDR;                   // Fast Forcing Disable Register
   AT91_REG AIC_FFSR;                   // Fast Forcing Status Register
} AT91S_AIC, *AT91PS_AIC;

// -------- AIC_SMR : (AIC Offset: 0x0) Control Register --------
#define AT91C_AIC_PRIOR       ((unsigned int) 0x7 <<  0) // (AIC) Priority Level
#define         AT91C_AIC_PRIOR_LOWEST               ((unsigned int) 0x0) // (AIC) Lowest priority level
#define         AT91C_AIC_PRIOR_HIGHEST              ((unsigned int) 0x7) // (AIC) Highest priority level
#define AT91C_AIC_SRCTYPE     ((unsigned int) 0x3 <<  5) // (AIC) Interrupt Source Type
#define         AT91C_AIC_SRCTYPE_EXT_LOW_LEVEL        ((unsigned int) 0x0 <<  5) // (AIC) External Sources Code Label Low-level Sensitive
#define         AT91C_AIC_SRCTYPE_INT_HIGH_LEVEL       ((unsigned int) 0x0 <<  5) // (AIC) Internal Sources Code Label High-level Sensitive
#define         AT91C_AIC_SRCTYPE_INT_POSITIVE_EDGE    ((unsigned int) 0x1 <<  5) // (AIC) Internal Sources Code Label Positive Edge triggered
#define         AT91C_AIC_SRCTYPE_EXT_NEGATIVE_EDGE    ((unsigned int) 0x1 <<  5) // (AIC) External Sources Code Label Negative Edge triggered
#define         AT91C_AIC_SRCTYPE_HIGH_LEVEL           ((unsigned int) 0x2 <<  5) // (AIC) Internal Or External Sources Code Label High-level Sensitive
#define         AT91C_AIC_SRCTYPE_POSITIVE_EDGE        ((unsigned int) 0x3 <<  5) // (AIC) Internal Or External Sources Code Label Positive Edge triggered
// -------- AIC_CISR : (AIC Offset: 0x114) AIC Core Interrupt Status Register --------
#define AT91C_AIC_NFIQ        ((unsigned int) 0x1 <<  0) // (AIC) NFIQ Status
#define AT91C_AIC_NIRQ        ((unsigned int) 0x1 <<  1) // (AIC) NIRQ Status
// -------- AIC_DCR : (AIC Offset: 0x138) AIC Debug Control Register (Protect) --------
#define AT91C_AIC_DCR_PROT    ((unsigned int) 0x1 <<  0) // (AIC) Protection Mode
#define AT91C_AIC_DCR_GMSK    ((unsigned int) 0x1 <<  1) // (AIC) General Mask

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Peripheral DMA Controller
// *****************************************************************************
typedef struct _AT91S_PDC {
   AT91_REG PDC_RPR;                    // Receive Pointer Register
   AT91_REG PDC_RCR;                    // Receive Counter Register
   AT91_REG PDC_TPR;                    // Transmit Pointer Register
   AT91_REG PDC_TCR;                    // Transmit Counter Register
   AT91_REG PDC_RNPR;                   // Receive Next Pointer Register
   AT91_REG PDC_RNCR;                   // Receive Next Counter Register
   AT91_REG PDC_TNPR;                   // Transmit Next Pointer Register
   AT91_REG PDC_TNCR;                   // Transmit Next Counter Register
   AT91_REG PDC_PTCR;                   // PDC Transfer Control Register
   AT91_REG PDC_PTSR;                   // PDC Transfer Status Register
} AT91S_PDC, *AT91PS_PDC;

// -------- PDC_PTCR : (PDC Offset: 0x20) PDC Transfer Control Register --------
#define AT91C_PDC_RXTEN       ((unsigned int) 0x1 <<  0) // (PDC) Receiver Transfer Enable
#define AT91C_PDC_RXTDIS      ((unsigned int) 0x1 <<  1) // (PDC) Receiver Transfer Disable
#define AT91C_PDC_TXTEN       ((unsigned int) 0x1 <<  8) // (PDC) Transmitter Transfer Enable
#define AT91C_PDC_TXTDIS      ((unsigned int) 0x1 <<  9) // (PDC) Transmitter Transfer Disable
// -------- PDC_PTSR : (PDC Offset: 0x24) PDC Transfer Status Register --------

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Debug Unit
// *****************************************************************************
typedef struct _AT91S_DBGU {
   AT91_REG DBGU_CR;                    // Control Register
   AT91_REG DBGU_MR;                    // Mode Register
   AT91_REG DBGU_IER;                   // Interrupt Enable Register
   AT91_REG DBGU_IDR;                   // Interrupt Disable Register
   AT91_REG DBGU_IMR;                   // Interrupt Mask Register
   AT91_REG DBGU_CSR;                   // Channel Status Register
   AT91_REG DBGU_RHR;                   // Receiver Holding Register
   AT91_REG DBGU_THR;                   // Transmitter Holding Register
   AT91_REG DBGU_BRGR;                  // Baud Rate Generator Register
   AT91_REG Reserved0[7];               //
   AT91_REG DBGU_CIDR;                  // Chip ID Register
   AT91_REG DBGU_EXID;                  // Chip ID Extension Register
   AT91_REG DBGU_FNTR;                  // Force NTRST Register
   AT91_REG Reserved1[45];                      //
   AT91_REG DBGU_RPR;                   // Receive Pointer Register
   AT91_REG DBGU_RCR;                   // Receive Counter Register
   AT91_REG DBGU_TPR;                   // Transmit Pointer Register
   AT91_REG DBGU_TCR;                   // Transmit Counter Register
   AT91_REG DBGU_RNPR;                  // Receive Next Pointer Register
   AT91_REG DBGU_RNCR;                  // Receive Next Counter Register
   AT91_REG DBGU_TNPR;                  // Transmit Next Pointer Register
   AT91_REG DBGU_TNCR;                  // Transmit Next Counter Register
   AT91_REG DBGU_PTCR;                  // PDC Transfer Control Register
   AT91_REG DBGU_PTSR;                  // PDC Transfer Status Register
} AT91S_DBGU, *AT91PS_DBGU;

// -------- DBGU_CR : (DBGU Offset: 0x0) Debug Unit Control Register --------
#define AT91C_US_RSTRX        ((unsigned int) 0x1 <<  2) // (DBGU) Reset Receiver
#define AT91C_US_RSTTX        ((unsigned int) 0x1 <<  3) // (DBGU) Reset Transmitter
#define AT91C_US_RXEN         ((unsigned int) 0x1 <<  4) // (DBGU) Receiver Enable
#define AT91C_US_RXDIS        ((unsigned int) 0x1 <<  5) // (DBGU) Receiver Disable
#define AT91C_US_TXEN         ((unsigned int) 0x1 <<  6) // (DBGU) Transmitter Enable
#define AT91C_US_TXDIS        ((unsigned int) 0x1 <<  7) // (DBGU) Transmitter Disable
#define AT91C_US_RSTSTA       ((unsigned int) 0x1 <<  8) // (DBGU) Reset Status Bits
// -------- DBGU_MR : (DBGU Offset: 0x4) Debug Unit Mode Register --------
#define AT91C_US_PAR          ((unsigned int) 0x7 <<  9) // (DBGU) Parity type
#define         AT91C_US_PAR_EVEN                 ((unsigned int) 0x0 <<  9) // (DBGU) Even Parity
#define         AT91C_US_PAR_ODD                  ((unsigned int) 0x1 <<  9) // (DBGU) Odd Parity
#define         AT91C_US_PAR_SPACE                ((unsigned int) 0x2 <<  9) // (DBGU) Parity forced to 0 (Space)
#define         AT91C_US_PAR_MARK                 ((unsigned int) 0x3 <<  9) // (DBGU) Parity forced to 1 (Mark)
#define         AT91C_US_PAR_NONE                 ((unsigned int) 0x4 <<  9) // (DBGU) No Parity
#define         AT91C_US_PAR_MULTI_DROP           ((unsigned int) 0x6 <<  9) // (DBGU) Multi-drop mode
#define AT91C_US_CHMODE       ((unsigned int) 0x3 << 14) // (DBGU) Channel Mode
#define         AT91C_US_CHMODE_NORMAL               ((unsigned int) 0x0 << 14) // (DBGU) Normal Mode: The USART channel operates as an RX/TX USART.
#define         AT91C_US_CHMODE_AUTO                 ((unsigned int) 0x1 << 14) // (DBGU) Automatic Echo: Receiver Data Input is connected to the TXD pin.
#define         AT91C_US_CHMODE_LOCAL                ((unsigned int) 0x2 << 14) // (DBGU) Local Loopback: Transmitter Output Signal is connected to Receiver Input Signal.
#define         AT91C_US_CHMODE_REMOTE               ((unsigned int) 0x3 << 14) // (DBGU) Remote Loopback: RXD pin is internally connected to TXD pin.
// -------- DBGU_IER : (DBGU Offset: 0x8) Debug Unit Interrupt Enable Register --------
#define AT91C_US_RXRDY        ((unsigned int) 0x1 <<  0) // (DBGU) RXRDY Interrupt
#define AT91C_US_TXRDY        ((unsigned int) 0x1 <<  1) // (DBGU) TXRDY Interrupt
#define AT91C_US_ENDRX        ((unsigned int) 0x1 <<  3) // (DBGU) End of Receive Transfer Interrupt
#define AT91C_US_ENDTX        ((unsigned int) 0x1 <<  4) // (DBGU) End of Transmit Interrupt
#define AT91C_US_OVRE         ((unsigned int) 0x1 <<  5) // (DBGU) Overrun Interrupt
#define AT91C_US_FRAME        ((unsigned int) 0x1 <<  6) // (DBGU) Framing Error Interrupt
#define AT91C_US_PARE         ((unsigned int) 0x1 <<  7) // (DBGU) Parity Error Interrupt
#define AT91C_US_TXEMPTY      ((unsigned int) 0x1 <<  9) // (DBGU) TXEMPTY Interrupt
#define AT91C_US_TXBUFE       ((unsigned int) 0x1 << 11) // (DBGU) TXBUFE Interrupt
#define AT91C_US_RXBUFF       ((unsigned int) 0x1 << 12) // (DBGU) RXBUFF Interrupt
#define AT91C_US_COMM_TX      ((unsigned int) 0x1 << 30) // (DBGU) COMM_TX Interrupt
#define AT91C_US_COMM_RX      ((unsigned int) 0x1 << 31) // (DBGU) COMM_RX Interrupt
// -------- DBGU_IDR : (DBGU Offset: 0xc) Debug Unit Interrupt Disable Register --------
// -------- DBGU_IMR : (DBGU Offset: 0x10) Debug Unit Interrupt Mask Register --------
// -------- DBGU_CSR : (DBGU Offset: 0x14) Debug Unit Channel Status Register --------
// -------- DBGU_FNTR : (DBGU Offset: 0x48) Debug Unit FORCE_NTRST Register --------
#define AT91C_US_FORCE_NTRST  ((unsigned int) 0x1 <<  0) // (DBGU) Force NTRST in JTAG

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Parallel Input Output Controler
// *****************************************************************************
typedef struct _AT91S_PIO {
   AT91_REG PIO_PER;                    // PIO Enable Register
   AT91_REG PIO_PDR;                    // PIO Disable Register
   AT91_REG PIO_PSR;                    // PIO Status Register
   AT91_REG Reserved0[1];               //
   AT91_REG PIO_OER;                    // Output Enable Register
   AT91_REG PIO_ODR;                    // Output Disable Registerr
   AT91_REG PIO_OSR;                    // Output Status Register
   AT91_REG Reserved1[1];               //
   AT91_REG PIO_IFER;                   // Input Filter Enable Register
   AT91_REG PIO_IFDR;                   // Input Filter Disable Register
   AT91_REG PIO_IFSR;                   // Input Filter Status Register
   AT91_REG Reserved2[1];               //
   AT91_REG PIO_SODR;                   // Set Output Data Register
   AT91_REG PIO_CODR;                   // Clear Output Data Register
   AT91_REG PIO_ODSR;                   // Output Data Status Register
   AT91_REG PIO_PDSR;                   // Pin Data Status Register
   AT91_REG PIO_IER;                    // Interrupt Enable Register
   AT91_REG PIO_IDR;                    // Interrupt Disable Register
   AT91_REG PIO_IMR;                    // Interrupt Mask Register
   AT91_REG PIO_ISR;                    // Interrupt Status Register
   AT91_REG PIO_MDER;                   // Multi-driver Enable Register
   AT91_REG PIO_MDDR;                   // Multi-driver Disable Register
   AT91_REG PIO_MDSR;                   // Multi-driver Status Register
   AT91_REG Reserved3[1];               //
   AT91_REG PIO_PPUDR;                  // Pull-up Disable Register
   AT91_REG PIO_PPUER;                  // Pull-up Enable Register
   AT91_REG PIO_PPUSR;                  // Pull-up Status Register
   AT91_REG Reserved4[1];               //
   AT91_REG PIO_ASR;                    // Select A Register
   AT91_REG PIO_BSR;                    // Select B Register
   AT91_REG PIO_ABSR;                   // AB Select Status Register
   AT91_REG Reserved5[9];               //
   AT91_REG PIO_OWER;                   // Output Write Enable Register
   AT91_REG PIO_OWDR;                   // Output Write Disable Register
   AT91_REG PIO_OWSR;                   // Output Write Status Register
} AT91S_PIO, *AT91PS_PIO;


// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Clock Generator Controler
// *****************************************************************************
typedef struct _AT91S_CKGR {
   AT91_REG CKGR_MOR;                   // Main Oscillator Register
   AT91_REG CKGR_MCFR;                  // Main Clock  Frequency Register
   AT91_REG Reserved0[1];               //
   AT91_REG CKGR_PLLR;                  // PLL Register
} AT91S_CKGR, *AT91PS_CKGR;

// -------- CKGR_MOR : (CKGR Offset: 0x0) Main Oscillator Register --------
#define AT91C_CKGR_MOSCEN     ((unsigned int) 0x1 <<  0) // (CKGR) Main Oscillator Enable
#define AT91C_CKGR_OSCBYPASS  ((unsigned int) 0x1 <<  1) // (CKGR) Main Oscillator Bypass
#define AT91C_CKGR_OSCOUNT    ((unsigned int) 0xFF <<  8) // (CKGR) Main Oscillator Start-up Time
// -------- CKGR_MCFR : (CKGR Offset: 0x4) Main Clock Frequency Register --------
#define AT91C_CKGR_MAINF      ((unsigned int) 0xFFFF <<  0) // (CKGR) Main Clock Frequency
#define AT91C_CKGR_MAINRDY    ((unsigned int) 0x1 << 16) // (CKGR) Main Clock Ready
// -------- CKGR_PLLR : (CKGR Offset: 0xc) PLL B Register --------
#define AT91C_CKGR_DIV        ((unsigned int) 0xFF <<  0) // (CKGR) Divider Selected
#define         AT91C_CKGR_DIV_0                    ((unsigned int) 0x0) // (CKGR) Divider output is 0
#define         AT91C_CKGR_DIV_BYPASS               ((unsigned int) 0x1) // (CKGR) Divider is bypassed
#define AT91C_CKGR_PLLCOUNT   ((unsigned int) 0x3F <<  8) // (CKGR) PLL Counter
#define AT91C_CKGR_OUT        ((unsigned int) 0x3 << 14) // (CKGR) PLL Output Frequency Range
#define         AT91C_CKGR_OUT_0                    ((unsigned int) 0x0 << 14) // (CKGR) Please refer to the PLL datasheet
#define         AT91C_CKGR_OUT_1                    ((unsigned int) 0x1 << 14) // (CKGR) Please refer to the PLL datasheet
#define         AT91C_CKGR_OUT_2                    ((unsigned int) 0x2 << 14) // (CKGR) Please refer to the PLL datasheet
#define         AT91C_CKGR_OUT_3                    ((unsigned int) 0x3 << 14) // (CKGR) Please refer to the PLL datasheet
#define AT91C_CKGR_MUL        ((unsigned int) 0x7FF << 16) // (CKGR) PLL Multiplier
#define AT91C_CKGR_USBDIV     ((unsigned int) 0x3 << 28) // (CKGR) Divider for USB Clocks
#define         AT91C_CKGR_USBDIV_0                    ((unsigned int) 0x0 << 28) // (CKGR) Divider output is PLL clock output
#define         AT91C_CKGR_USBDIV_1                    ((unsigned int) 0x1 << 28) // (CKGR) Divider output is PLL clock output divided by 2
#define         AT91C_CKGR_USBDIV_2                    ((unsigned int) 0x2 << 28) // (CKGR) Divider output is PLL clock output divided by 4

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Power Management Controler
// *****************************************************************************
typedef struct _AT91S_PMC {
   AT91_REG PMC_SCER;                   // System Clock Enable Register
   AT91_REG PMC_SCDR;                   // System Clock Disable Register
   AT91_REG PMC_SCSR;                   // System Clock Status Register
   AT91_REG Reserved0[1];               //
   AT91_REG PMC_PCER;                   // Peripheral Clock Enable Register
   AT91_REG PMC_PCDR;                   // Peripheral Clock Disable Register
   AT91_REG PMC_PCSR;                   // Peripheral Clock Status Register
   AT91_REG Reserved1[1];               //
   AT91_REG PMC_MOR;                    // Main Oscillator Register
   AT91_REG PMC_MCFR;                   // Main Clock  Frequency Register
   AT91_REG Reserved2[1];               //
   AT91_REG PMC_PLLR;                   // PLL Register
   AT91_REG PMC_MCKR;                   // Master Clock Register
   AT91_REG Reserved3[3];               //
   AT91_REG PMC_PCKR[4];                // Programmable Clock Register
   AT91_REG Reserved4[4];               //
   AT91_REG PMC_IER;                    // Interrupt Enable Register
   AT91_REG PMC_IDR;                    // Interrupt Disable Register
   AT91_REG PMC_SR;                     // Status Register
   AT91_REG PMC_IMR;                    // Interrupt Mask Register
} AT91S_PMC, *AT91PS_PMC;

// -------- PMC_SCER : (PMC Offset: 0x0) System Clock Enable Register --------
#define AT91C_PMC_PCK         ((unsigned int) 0x1 <<  0) // (PMC) Processor Clock
#define AT91C_PMC_UDP         ((unsigned int) 0x1 <<  7) // (PMC) USB Device Port Clock
#define AT91C_PMC_PCK0        ((unsigned int) 0x1 <<  8) // (PMC) Programmable Clock Output
#define AT91C_PMC_PCK1        ((unsigned int) 0x1 <<  9) // (PMC) Programmable Clock Output
#define AT91C_PMC_PCK2        ((unsigned int) 0x1 << 10) // (PMC) Programmable Clock Output
#define AT91C_PMC_PCK3        ((unsigned int) 0x1 << 11) // (PMC) Programmable Clock Output
// -------- PMC_SCDR : (PMC Offset: 0x4) System Clock Disable Register --------
// -------- PMC_SCSR : (PMC Offset: 0x8) System Clock Status Register --------
// -------- CKGR_MOR : (PMC Offset: 0x20) Main Oscillator Register --------
// -------- CKGR_MCFR : (PMC Offset: 0x24) Main Clock Frequency Register --------
// -------- CKGR_PLLR : (PMC Offset: 0x2c) PLL B Register --------
// -------- PMC_MCKR : (PMC Offset: 0x30) Master Clock Register --------
#define AT91C_PMC_CSS         ((unsigned int) 0x3 <<  0) // (PMC) Programmable Clock Selection
#define         AT91C_PMC_CSS_SLOW_CLK             ((unsigned int) 0x0) // (PMC) Slow Clock is selected
#define         AT91C_PMC_CSS_MAIN_CLK             ((unsigned int) 0x1) // (PMC) Main Clock is selected
#define         AT91C_PMC_CSS_PLL_CLK              ((unsigned int) 0x3) // (PMC) Clock from PLL is selected
#define AT91C_PMC_PRES        ((unsigned int) 0x7 <<  2) // (PMC) Programmable Clock Prescaler
#define         AT91C_PMC_PRES_CLK                  ((unsigned int) 0x0 <<  2) // (PMC) Selected clock
#define         AT91C_PMC_PRES_CLK_2                ((unsigned int) 0x1 <<  2) // (PMC) Selected clock divided by 2
#define         AT91C_PMC_PRES_CLK_4                ((unsigned int) 0x2 <<  2) // (PMC) Selected clock divided by 4
#define         AT91C_PMC_PRES_CLK_8                ((unsigned int) 0x3 <<  2) // (PMC) Selected clock divided by 8
#define         AT91C_PMC_PRES_CLK_16               ((unsigned int) 0x4 <<  2) // (PMC) Selected clock divided by 16
#define         AT91C_PMC_PRES_CLK_32               ((unsigned int) 0x5 <<  2) // (PMC) Selected clock divided by 32
#define         AT91C_PMC_PRES_CLK_64               ((unsigned int) 0x6 <<  2) // (PMC) Selected clock divided by 64
// -------- PMC_PCKR : (PMC Offset: 0x40) Programmable Clock Register --------
// -------- PMC_IER : (PMC Offset: 0x60) PMC Interrupt Enable Register --------
#define AT91C_PMC_MOSCS       ((unsigned int) 0x1 <<  0) // (PMC) MOSC Status/Enable/Disable/Mask
#define AT91C_PMC_LOCK        ((unsigned int) 0x1 <<  2) // (PMC) PLL Status/Enable/Disable/Mask
#define AT91C_PMC_MCKRDY      ((unsigned int) 0x1 <<  3) // (PMC) MCK_RDY Status/Enable/Disable/Mask
#define AT91C_PMC_PCK0RDY     ((unsigned int) 0x1 <<  8) // (PMC) PCK0_RDY Status/Enable/Disable/Mask
#define AT91C_PMC_PCK1RDY     ((unsigned int) 0x1 <<  9) // (PMC) PCK1_RDY Status/Enable/Disable/Mask
#define AT91C_PMC_PCK2RDY     ((unsigned int) 0x1 << 10) // (PMC) PCK2_RDY Status/Enable/Disable/Mask
#define AT91C_PMC_PCK3RDY     ((unsigned int) 0x1 << 11) // (PMC) PCK3_RDY Status/Enable/Disable/Mask
// -------- PMC_IDR : (PMC Offset: 0x64) PMC Interrupt Disable Register --------
// -------- PMC_SR : (PMC Offset: 0x68) PMC Status Register --------
// -------- PMC_IMR : (PMC Offset: 0x6c) PMC Interrupt Mask Register --------

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Reset Controller Interface
// *****************************************************************************
typedef struct _AT91S_RSTC {
   AT91_REG RSTC_RCR;                   // Reset Control Register
   AT91_REG RSTC_RSR;                   // Reset Status Register
   AT91_REG RSTC_RMR;                   // Reset Mode Register
} AT91S_RSTC, *AT91PS_RSTC;

// -------- RSTC_RCR : (RSTC Offset: 0x0) Reset Control Register --------
#define AT91C_RSTC_PROCRST    ((unsigned int) 0x1 <<  0) // (RSTC) Processor Reset
#define AT91C_RSTC_PERRST     ((unsigned int) 0x1 <<  2) // (RSTC) Peripheral Reset
#define AT91C_RSTC_EXTRST     ((unsigned int) 0x1 <<  3) // (RSTC) External Reset
#define AT91C_RSTC_KEY        ((unsigned int) 0xFF << 24) // (RSTC) Password
// -------- RSTC_RSR : (RSTC Offset: 0x4) Reset Status Register --------
#define AT91C_RSTC_URSTS      ((unsigned int) 0x1 <<  0) // (RSTC) User Reset Status
#define AT91C_RSTC_BODSTS     ((unsigned int) 0x1 <<  1) // (RSTC) Brownout Detection Status
#define AT91C_RSTC_RSTTYP     ((unsigned int) 0x7 <<  8) // (RSTC) Reset Type
#define         AT91C_RSTC_RSTTYP_POWERUP              ((unsigned int) 0x0 <<  8) // (RSTC) Power-up Reset. VDDCORE rising.
#define         AT91C_RSTC_RSTTYP_WAKEUP               ((unsigned int) 0x1 <<  8) // (RSTC) WakeUp Reset. VDDCORE rising.
#define         AT91C_RSTC_RSTTYP_WATCHDOG             ((unsigned int) 0x2 <<  8) // (RSTC) Watchdog Reset. Watchdog overflow occured.
#define         AT91C_RSTC_RSTTYP_SOFTWARE             ((unsigned int) 0x3 <<  8) // (RSTC) Software Reset. Processor reset required by the software.
#define         AT91C_RSTC_RSTTYP_USER                 ((unsigned int) 0x4 <<  8) // (RSTC) User Reset. NRST pin detected low.
#define         AT91C_RSTC_RSTTYP_BROWNOUT             ((unsigned int) 0x5 <<  8) // (RSTC) Brownout Reset occured.
#define AT91C_RSTC_NRSTL      ((unsigned int) 0x1 << 16) // (RSTC) NRST pin level
#define AT91C_RSTC_SRCMP      ((unsigned int) 0x1 << 17) // (RSTC) Software Reset Command in Progress.
// -------- RSTC_RMR : (RSTC Offset: 0x8) Reset Mode Register --------
#define AT91C_RSTC_URSTEN     ((unsigned int) 0x1 <<  0) // (RSTC) User Reset Enable
#define AT91C_RSTC_URSTIEN    ((unsigned int) 0x1 <<  4) // (RSTC) User Reset Interrupt Enable
#define AT91C_RSTC_ERSTL      ((unsigned int) 0xF <<  8) // (RSTC) User Reset Length
#define AT91C_RSTC_BODIEN     ((unsigned int) 0x1 << 16) // (RSTC) Brownout Detection Interrupt Enable

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Real Time Timer Controller Interface
// *****************************************************************************
typedef struct _AT91S_RTTC {
   AT91_REG RTTC_RTMR;                  // Real-time Mode Register
   AT91_REG RTTC_RTAR;                  // Real-time Alarm Register
   AT91_REG RTTC_RTVR;                  // Real-time Value Register
   AT91_REG RTTC_RTSR;                  // Real-time Status Register
} AT91S_RTTC, *AT91PS_RTTC;

// -------- RTTC_RTMR : (RTTC Offset: 0x0) Real-time Mode Register --------
#define AT91C_RTTC_RTPRES     ((unsigned int) 0xFFFF <<  0) // (RTTC) Real-time Timer Prescaler Value
#define AT91C_RTTC_ALMIEN     ((unsigned int) 0x1 << 16) // (RTTC) Alarm Interrupt Enable
#define AT91C_RTTC_RTTINCIEN  ((unsigned int) 0x1 << 17) // (RTTC) Real Time Timer Increment Interrupt Enable
#define AT91C_RTTC_RTTRST     ((unsigned int) 0x1 << 18) // (RTTC) Real Time Timer Restart
// -------- RTTC_RTAR : (RTTC Offset: 0x4) Real-time Alarm Register --------
#define AT91C_RTTC_ALMV       ((unsigned int) 0x0 <<  0) // (RTTC) Alarm Value
// -------- RTTC_RTVR : (RTTC Offset: 0x8) Current Real-time Value Register --------
#define AT91C_RTTC_CRTV       ((unsigned int) 0x0 <<  0) // (RTTC) Current Real-time Value
// -------- RTTC_RTSR : (RTTC Offset: 0xc) Real-time Status Register --------
#define AT91C_RTTC_ALMS       ((unsigned int) 0x1 <<  0) // (RTTC) Real-time Alarm Status
#define AT91C_RTTC_RTTINC     ((unsigned int) 0x1 <<  1) // (RTTC) Real-time Timer Increment

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Periodic Interval Timer Controller Interface
// *****************************************************************************
typedef struct _AT91S_PITC {
   AT91_REG PITC_PIMR;                  // Period Interval Mode Register
   AT91_REG PITC_PISR;                  // Period Interval Status Register
   AT91_REG PITC_PIVR;                  // Period Interval Value Register
   AT91_REG PITC_PIIR;                  // Period Interval Image Register
} AT91S_PITC, *AT91PS_PITC;

// -------- PITC_PIMR : (PITC Offset: 0x0) Periodic Interval Mode Register --------
#define AT91C_PITC_PIV        ((unsigned int) 0xFFFFF <<  0) // (PITC) Periodic Interval Value
#define AT91C_PITC_PITEN      ((unsigned int) 0x1 << 24) // (PITC) Periodic Interval Timer Enabled
#define AT91C_PITC_PITIEN     ((unsigned int) 0x1 << 25) // (PITC) Periodic Interval Timer Interrupt Enable
// -------- PITC_PISR : (PITC Offset: 0x4) Periodic Interval Status Register --------
#define AT91C_PITC_PITS       ((unsigned int) 0x1 <<  0) // (PITC) Periodic Interval Timer Status
// -------- PITC_PIVR : (PITC Offset: 0x8) Periodic Interval Value Register --------
#define AT91C_PITC_CPIV       ((unsigned int) 0xFFFFF <<  0) // (PITC) Current Periodic Interval Value
#define AT91C_PITC_PICNT      ((unsigned int) 0xFFF << 20) // (PITC) Periodic Interval Counter
// -------- PITC_PIIR : (PITC Offset: 0xc) Periodic Interval Image Register --------

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Watchdog Timer Controller Interface
// *****************************************************************************
typedef struct _AT91S_WDTC {
   AT91_REG WDTC_WDCR;                  // Watchdog Control Register
   AT91_REG WDTC_WDMR;                  // Watchdog Mode Register
   AT91_REG WDTC_WDSR;                  // Watchdog Status Register
} AT91S_WDTC, *AT91PS_WDTC;

// -------- WDTC_WDCR : (WDTC Offset: 0x0) Periodic Interval Image Register --------
#define AT91C_WDTC_WDRSTT     ((unsigned int) 0x1 <<  0) // (WDTC) Watchdog Restart
#define AT91C_WDTC_KEY        ((unsigned int) 0xFF << 24) // (WDTC) Watchdog KEY Password
// -------- WDTC_WDMR : (WDTC Offset: 0x4) Watchdog Mode Register --------
#define AT91C_WDTC_WDV        ((unsigned int) 0xFFF <<  0) // (WDTC) Watchdog Timer Restart
#define AT91C_WDTC_WDFIEN     ((unsigned int) 0x1 << 12) // (WDTC) Watchdog Fault Interrupt Enable
#define AT91C_WDTC_WDRSTEN    ((unsigned int) 0x1 << 13) // (WDTC) Watchdog Reset Enable
#define AT91C_WDTC_WDRPROC    ((unsigned int) 0x1 << 14) // (WDTC) Watchdog Timer Restart
#define AT91C_WDTC_WDDIS      ((unsigned int) 0x1 << 15) // (WDTC) Watchdog Disable
#define AT91C_WDTC_WDD        ((unsigned int) 0xFFF << 16) // (WDTC) Watchdog Delta Value
#define AT91C_WDTC_WDDBGHLT   ((unsigned int) 0x1 << 28) // (WDTC) Watchdog Debug Halt
#define AT91C_WDTC_WDIDLEHLT  ((unsigned int) 0x1 << 29) // (WDTC) Watchdog Idle Halt
// -------- WDTC_WDSR : (WDTC Offset: 0x8) Watchdog Status Register --------
#define AT91C_WDTC_WDUNF      ((unsigned int) 0x1 <<  0) // (WDTC) Watchdog Underflow
#define AT91C_WDTC_WDERR      ((unsigned int) 0x1 <<  1) // (WDTC) Watchdog Error

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Voltage Regulator Mode Controller Interface
// *****************************************************************************
typedef struct _AT91S_VREG {
   AT91_REG VREG_MR;                    // Voltage Regulator Mode Register
} AT91S_VREG, *AT91PS_VREG;

// -------- VREG_MR : (VREG Offset: 0x0) Voltage Regulator Mode Register --------
#define AT91C_VREG_PSTDBY     ((unsigned int) 0x1 <<  0) // (VREG) Voltage Regulator Power Standby Mode

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Memory Controller Interface
// *****************************************************************************
typedef struct _AT91S_MC {
   AT91_REG MC_RCR;                     // MC Remap Control Register
   AT91_REG MC_ASR;                     // MC Abort Status Register
   AT91_REG MC_AASR;                    // MC Abort Address Status Register
   AT91_REG Reserved0[1];               //
   AT91_REG MC_PUIA[16];                // MC Protection Unit Area
   AT91_REG MC_PUP;                     // MC Protection Unit Peripherals
   AT91_REG MC_PUER;                    // MC Protection Unit Enable Register
   AT91_REG Reserved1[2];               //
   AT91_REG MC0_FMR;                    // MC Flash Mode Register
   AT91_REG MC0_FCR;                    // MC Flash Command Register
   AT91_REG MC0_FSR;                    // MC Flash Status Register
   AT91_REG MC0_VR;                     // MC Flash Version Register
   AT91_REG MC1_FMR;                    // MC Flash Mode Register
   AT91_REG MC1_FCR;                    // MC Flash Command Register
   AT91_REG MC1_FSR;                    // MC Flash Status Register
   AT91_REG MC1_VR;                     // MC Flash Version Register
} AT91S_MC, *AT91PS_MC;

// -------- MC_RCR : (MC Offset: 0x0) MC Remap Control Register --------
#define AT91C_MC_RCB          ((unsigned int) 0x1 <<  0) // (MC) Remap Command Bit
// -------- MC_ASR : (MC Offset: 0x4) MC Abort Status Register --------
#define AT91C_MC_UNDADD       ((unsigned int) 0x1 <<  0) // (MC) Undefined Addess Abort Status
#define AT91C_MC_MISADD       ((unsigned int) 0x1 <<  1) // (MC) Misaligned Addess Abort Status
#define AT91C_MC_MPU          ((unsigned int) 0x1 <<  2) // (MC) Memory protection Unit Abort Status
#define AT91C_MC_ABTSZ        ((unsigned int) 0x3 <<  8) // (MC) Abort Size Status
#define         AT91C_MC_ABTSZ_BYTE                 ((unsigned int) 0x0 <<  8) // (MC) Byte
#define         AT91C_MC_ABTSZ_HWORD                ((unsigned int) 0x1 <<  8) // (MC) Half-word
#define         AT91C_MC_ABTSZ_WORD                 ((unsigned int) 0x2 <<  8) // (MC) Word
#define AT91C_MC_ABTTYP       ((unsigned int) 0x3 << 10) // (MC) Abort Type Status
#define         AT91C_MC_ABTTYP_DATAR                ((unsigned int) 0x0 << 10) // (MC) Data Read
#define         AT91C_MC_ABTTYP_DATAW                ((unsigned int) 0x1 << 10) // (MC) Data Write
#define         AT91C_MC_ABTTYP_FETCH                ((unsigned int) 0x2 << 10) // (MC) Code Fetch
#define AT91C_MC_MST0         ((unsigned int) 0x1 << 16) // (MC) Master 0 Abort Source
#define AT91C_MC_MST1         ((unsigned int) 0x1 << 17) // (MC) Master 1 Abort Source
#define AT91C_MC_SVMST0       ((unsigned int) 0x1 << 24) // (MC) Saved Master 0 Abort Source
#define AT91C_MC_SVMST1       ((unsigned int) 0x1 << 25) // (MC) Saved Master 1 Abort Source
// -------- MC_PUIA : (MC Offset: 0x10) MC Protection Unit Area --------
#define AT91C_MC_PROT         ((unsigned int) 0x3 <<  0) // (MC) Protection
#define         AT91C_MC_PROT_PNAUNA               ((unsigned int) 0x0) // (MC) Privilege: No Access, User: No Access
#define         AT91C_MC_PROT_PRWUNA               ((unsigned int) 0x1) // (MC) Privilege: Read/Write, User: No Access
#define         AT91C_MC_PROT_PRWURO               ((unsigned int) 0x2) // (MC) Privilege: Read/Write, User: Read Only
#define         AT91C_MC_PROT_PRWURW               ((unsigned int) 0x3) // (MC) Privilege: Read/Write, User: Read/Write
#define AT91C_MC_SIZE         ((unsigned int) 0xF <<  4) // (MC) Internal Area Size
#define         AT91C_MC_SIZE_1KB                  ((unsigned int) 0x0 <<  4) // (MC) Area size 1KByte
#define         AT91C_MC_SIZE_2KB                  ((unsigned int) 0x1 <<  4) // (MC) Area size 2KByte
#define         AT91C_MC_SIZE_4KB                  ((unsigned int) 0x2 <<  4) // (MC) Area size 4KByte
#define         AT91C_MC_SIZE_8KB                  ((unsigned int) 0x3 <<  4) // (MC) Area size 8KByte
#define         AT91C_MC_SIZE_16KB                 ((unsigned int) 0x4 <<  4) // (MC) Area size 16KByte
#define         AT91C_MC_SIZE_32KB                 ((unsigned int) 0x5 <<  4) // (MC) Area size 32KByte
#define         AT91C_MC_SIZE_64KB                 ((unsigned int) 0x6 <<  4) // (MC) Area size 64KByte
#define         AT91C_MC_SIZE_128KB                ((unsigned int) 0x7 <<  4) // (MC) Area size 128KByte
#define         AT91C_MC_SIZE_256KB                ((unsigned int) 0x8 <<  4) // (MC) Area size 256KByte
#define         AT91C_MC_SIZE_512KB                ((unsigned int) 0x9 <<  4) // (MC) Area size 512KByte
#define         AT91C_MC_SIZE_1MB                  ((unsigned int) 0xA <<  4) // (MC) Area size 1MByte
#define         AT91C_MC_SIZE_2MB                  ((unsigned int) 0xB <<  4) // (MC) Area size 2MByte
#define         AT91C_MC_SIZE_4MB                  ((unsigned int) 0xC <<  4) // (MC) Area size 4MByte
#define         AT91C_MC_SIZE_8MB                  ((unsigned int) 0xD <<  4) // (MC) Area size 8MByte
#define         AT91C_MC_SIZE_16MB                 ((unsigned int) 0xE <<  4) // (MC) Area size 16MByte
#define         AT91C_MC_SIZE_64MB                 ((unsigned int) 0xF <<  4) // (MC) Area size 64MByte
#define AT91C_MC_BA           ((unsigned int) 0x3FFFF << 10) // (MC) Internal Area Base Address
// -------- MC_PUP : (MC Offset: 0x50) MC Protection Unit Peripheral --------
// -------- MC_PUER : (MC Offset: 0x54) MC Protection Unit Area --------
#define AT91C_MC_PUEB         ((unsigned int) 0x1 <<  0) // (MC) Protection Unit enable Bit

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Embedded Flash Controller Interface
// *****************************************************************************
typedef struct _AT91S_EFC {
   AT91_REG EFC_FMR;                    // MC Flash Mode Register
   AT91_REG EFC_FCR;                    // MC Flash Command Register
   AT91_REG EFC_FSR;                    // MC Flash Status Register
   AT91_REG EFC_VR;                     // MC Flash Version Register
} AT91S_EFC, *AT91PS_EFC;

// -------- MC_FMR : (EFC Offset: 0x0) MC Flash Mode Register --------
#define AT91C_MC_FRDY         ((unsigned int) 0x1 <<  0) // (EFC) Flash Ready
#define AT91C_MC_LOCKE        ((unsigned int) 0x1 <<  2) // (EFC) Lock Error
#define AT91C_MC_PROGE        ((unsigned int) 0x1 <<  3) // (EFC) Programming Error
#define AT91C_MC_NEBP         ((unsigned int) 0x1 <<  7) // (EFC) No Erase Before Programming
#define AT91C_MC_FWS          ((unsigned int) 0x3 <<  8) // (EFC) Flash Wait State
#define         AT91C_MC_FWS_0FWS                 ((unsigned int) 0x0 <<  8) // (EFC) 1 cycle for Read, 2 for Write operations
#define         AT91C_MC_FWS_1FWS                 ((unsigned int) 0x1 <<  8) // (EFC) 2 cycles for Read, 3 for Write operations
#define         AT91C_MC_FWS_2FWS                 ((unsigned int) 0x2 <<  8) // (EFC) 3 cycles for Read, 4 for Write operations
#define         AT91C_MC_FWS_3FWS                 ((unsigned int) 0x3 <<  8) // (EFC) 4 cycles for Read, 4 for Write operations
#define AT91C_MC_FMCN         ((unsigned int) 0xFF << 16) // (EFC) Flash Microsecond Cycle Number
// -------- MC_FCR : (EFC Offset: 0x4) MC Flash Command Register --------
#define AT91C_MC_FCMD         ((unsigned int) 0xF <<  0) // (EFC) Flash Command
#define         AT91C_MC_FCMD_START_PROG           ((unsigned int) 0x1) // (EFC) Starts the programming of th epage specified by PAGEN.
#define         AT91C_MC_FCMD_LOCK                 ((unsigned int) 0x2) // (EFC) Starts a lock sequence of the sector defined by the bits 4 to 7 of the field PAGEN.
#define         AT91C_MC_FCMD_PROG_AND_LOCK        ((unsigned int) 0x3) // (EFC) The lock sequence automatically happens after the programming sequence is completed.
#define         AT91C_MC_FCMD_UNLOCK               ((unsigned int) 0x4) // (EFC) Starts an unlock sequence of the sector defined by the bits 4 to 7 of the field PAGEN.
#define         AT91C_MC_FCMD_ERASE_ALL            ((unsigned int) 0x8) // (EFC) Starts the erase of the entire flash.If at least a page is locked, the command is cancelled.
#define         AT91C_MC_FCMD_SET_GP_NVM           ((unsigned int) 0xB) // (EFC) Set General Purpose NVM bits.
#define         AT91C_MC_FCMD_CLR_GP_NVM           ((unsigned int) 0xD) // (EFC) Clear General Purpose NVM bits.
#define         AT91C_MC_FCMD_SET_SECURITY         ((unsigned int) 0xF) // (EFC) Set Security Bit.
#define AT91C_MC_PAGEN        ((unsigned int) 0x3FF <<  8) // (EFC) Page Number
#define AT91C_MC_KEY          ((unsigned int) 0xFF << 24) // (EFC) Writing Protect Key
// -------- MC_FSR : (EFC Offset: 0x8) MC Flash Command Register --------
#define AT91C_MC_SECURITY     ((unsigned int) 0x1 <<  4) // (EFC) Security Bit Status
#define AT91C_MC_GPNVM0       ((unsigned int) 0x1 <<  8) // (EFC) Sector 0 Lock Status
#define AT91C_MC_GPNVM1       ((unsigned int) 0x1 <<  9) // (EFC) Sector 1 Lock Status
#define AT91C_MC_GPNVM2       ((unsigned int) 0x1 << 10) // (EFC) Sector 2 Lock Status
#define AT91C_MC_GPNVM3       ((unsigned int) 0x1 << 11) // (EFC) Sector 3 Lock Status
#define AT91C_MC_GPNVM4       ((unsigned int) 0x1 << 12) // (EFC) Sector 4 Lock Status
#define AT91C_MC_GPNVM5       ((unsigned int) 0x1 << 13) // (EFC) Sector 5 Lock Status
#define AT91C_MC_GPNVM6       ((unsigned int) 0x1 << 14) // (EFC) Sector 6 Lock Status
#define AT91C_MC_GPNVM7       ((unsigned int) 0x1 << 15) // (EFC) Sector 7 Lock Status
#define AT91C_MC_LOCKS0       ((unsigned int) 0x1 << 16) // (EFC) Sector 0 Lock Status
#define AT91C_MC_LOCKS1       ((unsigned int) 0x1 << 17) // (EFC) Sector 1 Lock Status
#define AT91C_MC_LOCKS2       ((unsigned int) 0x1 << 18) // (EFC) Sector 2 Lock Status
#define AT91C_MC_LOCKS3       ((unsigned int) 0x1 << 19) // (EFC) Sector 3 Lock Status
#define AT91C_MC_LOCKS4       ((unsigned int) 0x1 << 20) // (EFC) Sector 4 Lock Status
#define AT91C_MC_LOCKS5       ((unsigned int) 0x1 << 21) // (EFC) Sector 5 Lock Status
#define AT91C_MC_LOCKS6       ((unsigned int) 0x1 << 22) // (EFC) Sector 6 Lock Status
#define AT91C_MC_LOCKS7       ((unsigned int) 0x1 << 23) // (EFC) Sector 7 Lock Status
#define AT91C_MC_LOCKS8       ((unsigned int) 0x1 << 24) // (EFC) Sector 8 Lock Status
#define AT91C_MC_LOCKS9       ((unsigned int) 0x1 << 25) // (EFC) Sector 9 Lock Status
#define AT91C_MC_LOCKS10      ((unsigned int) 0x1 << 26) // (EFC) Sector 10 Lock Status
#define AT91C_MC_LOCKS11      ((unsigned int) 0x1 << 27) // (EFC) Sector 11 Lock Status
#define AT91C_MC_LOCKS12      ((unsigned int) 0x1 << 28) // (EFC) Sector 12 Lock Status
#define AT91C_MC_LOCKS13      ((unsigned int) 0x1 << 29) // (EFC) Sector 13 Lock Status
#define AT91C_MC_LOCKS14      ((unsigned int) 0x1 << 30) // (EFC) Sector 14 Lock Status
#define AT91C_MC_LOCKS15      ((unsigned int) 0x1 << 31) // (EFC) Sector 15 Lock Status
// -------- EFC_VR : (EFC Offset: 0xc) EFC version register --------
#define AT91C_EFC_VERSION     ((unsigned int) 0xFFF <<  0) // (EFC) EFC version number
#define AT91C_EFC_MFN         ((unsigned int) 0x7 << 16) // (EFC) EFC MFN

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Serial Parallel Interface
// *****************************************************************************
typedef struct _AT91S_SPI {
   AT91_REG SPI_CR;                     // Control Register
   AT91_REG SPI_MR;                     // Mode Register
   AT91_REG SPI_RDR;                    // Receive Data Register
   AT91_REG SPI_TDR;                    // Transmit Data Register
   AT91_REG SPI_SR;                     // Status Register
   AT91_REG SPI_IER;                    // Interrupt Enable Register
   AT91_REG SPI_IDR;                    // Interrupt Disable Register
   AT91_REG SPI_IMR;                    // Interrupt Mask Register
   AT91_REG Reserved0[4];               //
   AT91_REG SPI_CSR[4];                 // Chip Select Register
   AT91_REG Reserved1[48];                      //
   AT91_REG SPI_RPR;                    // Receive Pointer Register
   AT91_REG SPI_RCR;                    // Receive Counter Register
   AT91_REG SPI_TPR;                    // Transmit Pointer Register
   AT91_REG SPI_TCR;                    // Transmit Counter Register
   AT91_REG SPI_RNPR;                   // Receive Next Pointer Register
   AT91_REG SPI_RNCR;                   // Receive Next Counter Register
   AT91_REG SPI_TNPR;                   // Transmit Next Pointer Register
   AT91_REG SPI_TNCR;                   // Transmit Next Counter Register
   AT91_REG SPI_PTCR;                   // PDC Transfer Control Register
   AT91_REG SPI_PTSR;                   // PDC Transfer Status Register
} AT91S_SPI, *AT91PS_SPI;

// -------- SPI_CR : (SPI Offset: 0x0) SPI Control Register --------
#define AT91C_SPI_SPIEN       ((unsigned int) 0x1 <<  0) // (SPI) SPI Enable
#define AT91C_SPI_SPIDIS      ((unsigned int) 0x1 <<  1) // (SPI) SPI Disable
#define AT91C_SPI_SWRST       ((unsigned int) 0x1 <<  7) // (SPI) SPI Software reset
#define AT91C_SPI_LASTXFER    ((unsigned int) 0x1 << 24) // (SPI) SPI Last Transfer
// -------- SPI_MR : (SPI Offset: 0x4) SPI Mode Register --------
#define AT91C_SPI_MSTR        ((unsigned int) 0x1 <<  0) // (SPI) Master/Slave Mode
#define AT91C_SPI_PS          ((unsigned int) 0x1 <<  1) // (SPI) Peripheral Select
#define         AT91C_SPI_PS_FIXED                ((unsigned int) 0x0 <<  1) // (SPI) Fixed Peripheral Select
#define         AT91C_SPI_PS_VARIABLE             ((unsigned int) 0x1 <<  1) // (SPI) Variable Peripheral Select
#define AT91C_SPI_PCSDEC      ((unsigned int) 0x1 <<  2) // (SPI) Chip Select Decode
#define AT91C_SPI_FDIV        ((unsigned int) 0x1 <<  3) // (SPI) Clock Selection
#define AT91C_SPI_MODFDIS     ((unsigned int) 0x1 <<  4) // (SPI) Mode Fault Detection
#define AT91C_SPI_LLB         ((unsigned int) 0x1 <<  7) // (SPI) Clock Selection
#define AT91C_SPI_PCS         ((unsigned int) 0xF << 16) // (SPI) Peripheral Chip Select
#define AT91C_SPI_DLYBCS      ((unsigned int) 0xFF << 24) // (SPI) Delay Between Chip Selects
// -------- SPI_RDR : (SPI Offset: 0x8) Receive Data Register --------
#define AT91C_SPI_RD          ((unsigned int) 0xFFFF <<  0) // (SPI) Receive Data
#define AT91C_SPI_RPCS        ((unsigned int) 0xF << 16) // (SPI) Peripheral Chip Select Status
// -------- SPI_TDR : (SPI Offset: 0xc) Transmit Data Register --------
#define AT91C_SPI_TD          ((unsigned int) 0xFFFF <<  0) // (SPI) Transmit Data
#define AT91C_SPI_TPCS        ((unsigned int) 0xF << 16) // (SPI) Peripheral Chip Select Status
// -------- SPI_SR : (SPI Offset: 0x10) Status Register --------
#define AT91C_SPI_RDRF        ((unsigned int) 0x1 <<  0) // (SPI) Receive Data Register Full
#define AT91C_SPI_TDRE        ((unsigned int) 0x1 <<  1) // (SPI) Transmit Data Register Empty
#define AT91C_SPI_MODF        ((unsigned int) 0x1 <<  2) // (SPI) Mode Fault Error
#define AT91C_SPI_OVRES       ((unsigned int) 0x1 <<  3) // (SPI) Overrun Error Status
#define AT91C_SPI_ENDRX       ((unsigned int) 0x1 <<  4) // (SPI) End of Receiver Transfer
#define AT91C_SPI_ENDTX       ((unsigned int) 0x1 <<  5) // (SPI) End of Receiver Transfer
#define AT91C_SPI_RXBUFF      ((unsigned int) 0x1 <<  6) // (SPI) RXBUFF Interrupt
#define AT91C_SPI_TXBUFE      ((unsigned int) 0x1 <<  7) // (SPI) TXBUFE Interrupt
#define AT91C_SPI_NSSR        ((unsigned int) 0x1 <<  8) // (SPI) NSSR Interrupt
#define AT91C_SPI_TXEMPTY     ((unsigned int) 0x1 <<  9) // (SPI) TXEMPTY Interrupt
#define AT91C_SPI_SPIENS      ((unsigned int) 0x1 << 16) // (SPI) Enable Status
// -------- SPI_IER : (SPI Offset: 0x14) Interrupt Enable Register --------
// -------- SPI_IDR : (SPI Offset: 0x18) Interrupt Disable Register --------
// -------- SPI_IMR : (SPI Offset: 0x1c) Interrupt Mask Register --------
// -------- SPI_CSR : (SPI Offset: 0x30) Chip Select Register --------
#define AT91C_SPI_CPOL        ((unsigned int) 0x1 <<  0) // (SPI) Clock Polarity
#define AT91C_SPI_NCPHA       ((unsigned int) 0x1 <<  1) // (SPI) Clock Phase
#define AT91C_SPI_CSAAT       ((unsigned int) 0x1 <<  3) // (SPI) Chip Select Active After Transfer
#define AT91C_SPI_BITS        ((unsigned int) 0xF <<  4) // (SPI) Bits Per Transfer
#define         AT91C_SPI_BITS_8                    ((unsigned int) 0x0 <<  4) // (SPI) 8 Bits Per transfer
#define         AT91C_SPI_BITS_9                    ((unsigned int) 0x1 <<  4) // (SPI) 9 Bits Per transfer
#define         AT91C_SPI_BITS_10                   ((unsigned int) 0x2 <<  4) // (SPI) 10 Bits Per transfer
#define         AT91C_SPI_BITS_11                   ((unsigned int) 0x3 <<  4) // (SPI) 11 Bits Per transfer
#define         AT91C_SPI_BITS_12                   ((unsigned int) 0x4 <<  4) // (SPI) 12 Bits Per transfer
#define         AT91C_SPI_BITS_13                   ((unsigned int) 0x5 <<  4) // (SPI) 13 Bits Per transfer
#define         AT91C_SPI_BITS_14                   ((unsigned int) 0x6 <<  4) // (SPI) 14 Bits Per transfer
#define         AT91C_SPI_BITS_15                   ((unsigned int) 0x7 <<  4) // (SPI) 15 Bits Per transfer
#define         AT91C_SPI_BITS_16                   ((unsigned int) 0x8 <<  4) // (SPI) 16 Bits Per transfer
#define AT91C_SPI_SCBR        ((unsigned int) 0xFF <<  8) // (SPI) Serial Clock Baud Rate
#define AT91C_SPI_DLYBS       ((unsigned int) 0xFF << 16) // (SPI) Delay Before SPCK
#define AT91C_SPI_DLYBCT      ((unsigned int) 0xFF << 24) // (SPI) Delay Between Consecutive Transfers

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Usart
// *****************************************************************************
typedef struct _AT91S_USART {
   AT91_REG US_CR;                      // Control Register
   AT91_REG US_MR;                      // Mode Register
   AT91_REG US_IER;                     // Interrupt Enable Register
   AT91_REG US_IDR;                     // Interrupt Disable Register
   AT91_REG US_IMR;                     // Interrupt Mask Register
   AT91_REG US_CSR;                     // Channel Status Register
   AT91_REG US_RHR;                     // Receiver Holding Register
   AT91_REG US_THR;                     // Transmitter Holding Register
   AT91_REG US_BRGR;                    // Baud Rate Generator Register
   AT91_REG US_RTOR;                    // Receiver Time-out Register
   AT91_REG US_TTGR;                    // Transmitter Time-guard Register
   AT91_REG Reserved0[5];               //
   AT91_REG US_FIDI;                    // FI_DI_Ratio Register
   AT91_REG US_NER;                     // Nb Errors Register
   AT91_REG Reserved1[1];               //
   AT91_REG US_IF;                      // IRDA_FILTER Register
   AT91_REG Reserved2[44];                      //
   AT91_REG US_RPR;                     // Receive Pointer Register
   AT91_REG US_RCR;                     // Receive Counter Register
   AT91_REG US_TPR;                     // Transmit Pointer Register
   AT91_REG US_TCR;                     // Transmit Counter Register
   AT91_REG US_RNPR;                    // Receive Next Pointer Register
   AT91_REG US_RNCR;                    // Receive Next Counter Register
   AT91_REG US_TNPR;                    // Transmit Next Pointer Register
   AT91_REG US_TNCR;                    // Transmit Next Counter Register
   AT91_REG US_PTCR;                    // PDC Transfer Control Register
   AT91_REG US_PTSR;                    // PDC Transfer Status Register
} AT91S_USART, *AT91PS_USART;

// -------- US_CR : (USART Offset: 0x0) Debug Unit Control Register --------
#define AT91C_US_STTBRK       ((unsigned int) 0x1 <<  9) // (USART) Start Break
#define AT91C_US_STPBRK       ((unsigned int) 0x1 << 10) // (USART) Stop Break
#define AT91C_US_STTTO        ((unsigned int) 0x1 << 11) // (USART) Start Time-out
#define AT91C_US_SENDA        ((unsigned int) 0x1 << 12) // (USART) Send Address
#define AT91C_US_RSTIT        ((unsigned int) 0x1 << 13) // (USART) Reset Iterations
#define AT91C_US_RSTNACK      ((unsigned int) 0x1 << 14) // (USART) Reset Non Acknowledge
#define AT91C_US_RETTO        ((unsigned int) 0x1 << 15) // (USART) Rearm Time-out
#define AT91C_US_DTREN        ((unsigned int) 0x1 << 16) // (USART) Data Terminal ready Enable
#define AT91C_US_DTRDIS       ((unsigned int) 0x1 << 17) // (USART) Data Terminal ready Disable
#define AT91C_US_RTSEN        ((unsigned int) 0x1 << 18) // (USART) Request to Send enable
#define AT91C_US_RTSDIS       ((unsigned int) 0x1 << 19) // (USART) Request to Send Disable
// -------- US_MR : (USART Offset: 0x4) Debug Unit Mode Register --------
#define AT91C_US_USMODE       ((unsigned int) 0xF <<  0) // (USART) Usart mode
#define         AT91C_US_USMODE_NORMAL               ((unsigned int) 0x0) // (USART) Normal
#define         AT91C_US_USMODE_RS485                ((unsigned int) 0x1) // (USART) RS485
#define         AT91C_US_USMODE_HWHSH                ((unsigned int) 0x2) // (USART) Hardware Handshaking
#define         AT91C_US_USMODE_MODEM                ((unsigned int) 0x3) // (USART) Modem
#define         AT91C_US_USMODE_ISO7816_0            ((unsigned int) 0x4) // (USART) ISO7816 protocol: T = 0
#define         AT91C_US_USMODE_ISO7816_1            ((unsigned int) 0x6) // (USART) ISO7816 protocol: T = 1
#define         AT91C_US_USMODE_IRDA                 ((unsigned int) 0x8) // (USART) IrDA
#define         AT91C_US_USMODE_SWHSH                ((unsigned int) 0xC) // (USART) Software Handshaking
#define AT91C_US_CLKS         ((unsigned int) 0x3 <<  4) // (USART) Clock Selection (Baud Rate generator Input Clock
#define         AT91C_US_CLKS_CLOCK                ((unsigned int) 0x0 <<  4) // (USART) Clock
#define         AT91C_US_CLKS_FDIV1                ((unsigned int) 0x1 <<  4) // (USART) fdiv1
#define         AT91C_US_CLKS_SLOW                 ((unsigned int) 0x2 <<  4) // (USART) slow_clock (ARM)
#define         AT91C_US_CLKS_EXT                  ((unsigned int) 0x3 <<  4) // (USART) External (SCK)
#define AT91C_US_CHRL         ((unsigned int) 0x3 <<  6) // (USART) Clock Selection (Baud Rate generator Input Clock
#define         AT91C_US_CHRL_5_BITS               ((unsigned int) 0x0 <<  6) // (USART) Character Length: 5 bits
#define         AT91C_US_CHRL_6_BITS               ((unsigned int) 0x1 <<  6) // (USART) Character Length: 6 bits
#define         AT91C_US_CHRL_7_BITS               ((unsigned int) 0x2 <<  6) // (USART) Character Length: 7 bits
#define         AT91C_US_CHRL_8_BITS               ((unsigned int) 0x3 <<  6) // (USART) Character Length: 8 bits
#define AT91C_US_SYNC         ((unsigned int) 0x1 <<  8) // (USART) Synchronous Mode Select
#define AT91C_US_NBSTOP       ((unsigned int) 0x3 << 12) // (USART) Number of Stop bits
#define         AT91C_US_NBSTOP_1_BIT                ((unsigned int) 0x0 << 12) // (USART) 1 stop bit
#define         AT91C_US_NBSTOP_15_BIT               ((unsigned int) 0x1 << 12) // (USART) Asynchronous (SYNC=0) 2 stop bits Synchronous (SYNC=1) 2 stop bits
#define         AT91C_US_NBSTOP_2_BIT                ((unsigned int) 0x2 << 12) // (USART) 2 stop bits
#define AT91C_US_MSBF         ((unsigned int) 0x1 << 16) // (USART) Bit Order
#define AT91C_US_MODE9        ((unsigned int) 0x1 << 17) // (USART) 9-bit Character length
#define AT91C_US_CKLO         ((unsigned int) 0x1 << 18) // (USART) Clock Output Select
#define AT91C_US_OVER         ((unsigned int) 0x1 << 19) // (USART) Over Sampling Mode
#define AT91C_US_INACK        ((unsigned int) 0x1 << 20) // (USART) Inhibit Non Acknowledge
#define AT91C_US_DSNACK       ((unsigned int) 0x1 << 21) // (USART) Disable Successive NACK
#define AT91C_US_MAX_ITER     ((unsigned int) 0x1 << 24) // (USART) Number of Repetitions
#define AT91C_US_FILTER       ((unsigned int) 0x1 << 28) // (USART) Receive Line Filter
// -------- US_IER : (USART Offset: 0x8) Debug Unit Interrupt Enable Register --------
#define AT91C_US_RXBRK        ((unsigned int) 0x1 <<  2) // (USART) Break Received/End of Break
#define AT91C_US_TIMEOUT      ((unsigned int) 0x1 <<  8) // (USART) Receiver Time-out
#define AT91C_US_ITERATION    ((unsigned int) 0x1 << 10) // (USART) Max number of Repetitions Reached
#define AT91C_US_NACK         ((unsigned int) 0x1 << 13) // (USART) Non Acknowledge
#define AT91C_US_RIIC         ((unsigned int) 0x1 << 16) // (USART) Ring INdicator Input Change Flag
#define AT91C_US_DSRIC        ((unsigned int) 0x1 << 17) // (USART) Data Set Ready Input Change Flag
#define AT91C_US_DCDIC        ((unsigned int) 0x1 << 18) // (USART) Data Carrier Flag
#define AT91C_US_CTSIC        ((unsigned int) 0x1 << 19) // (USART) Clear To Send Input Change Flag
// -------- US_IDR : (USART Offset: 0xc) Debug Unit Interrupt Disable Register --------
// -------- US_IMR : (USART Offset: 0x10) Debug Unit Interrupt Mask Register --------
// -------- US_CSR : (USART Offset: 0x14) Debug Unit Channel Status Register --------
#define AT91C_US_RI           ((unsigned int) 0x1 << 20) // (USART) Image of RI Input
#define AT91C_US_DSR          ((unsigned int) 0x1 << 21) // (USART) Image of DSR Input
#define AT91C_US_DCD          ((unsigned int) 0x1 << 22) // (USART) Image of DCD Input
#define AT91C_US_CTS          ((unsigned int) 0x1 << 23) // (USART) Image of CTS Input

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Synchronous Serial Controller Interface
// *****************************************************************************
typedef struct _AT91S_SSC {
   AT91_REG SSC_CR;                     // Control Register
   AT91_REG SSC_CMR;                    // Clock Mode Register
   AT91_REG Reserved0[2];               //
   AT91_REG SSC_RCMR;                   // Receive Clock ModeRegister
   AT91_REG SSC_RFMR;                   // Receive Frame Mode Register
   AT91_REG SSC_TCMR;                   // Transmit Clock Mode Register
   AT91_REG SSC_TFMR;                   // Transmit Frame Mode Register
   AT91_REG SSC_RHR;                    // Receive Holding Register
   AT91_REG SSC_THR;                    // Transmit Holding Register
   AT91_REG Reserved1[2];               //
   AT91_REG SSC_RSHR;                   // Receive Sync Holding Register
   AT91_REG SSC_TSHR;                   // Transmit Sync Holding Register
   AT91_REG Reserved2[2];               //
   AT91_REG SSC_SR;                     // Status Register
   AT91_REG SSC_IER;                    // Interrupt Enable Register
   AT91_REG SSC_IDR;                    // Interrupt Disable Register
   AT91_REG SSC_IMR;                    // Interrupt Mask Register
   AT91_REG Reserved3[44];                      //
   AT91_REG SSC_RPR;                    // Receive Pointer Register
   AT91_REG SSC_RCR;                    // Receive Counter Register
   AT91_REG SSC_TPR;                    // Transmit Pointer Register
   AT91_REG SSC_TCR;                    // Transmit Counter Register
   AT91_REG SSC_RNPR;                   // Receive Next Pointer Register
   AT91_REG SSC_RNCR;                   // Receive Next Counter Register
   AT91_REG SSC_TNPR;                   // Transmit Next Pointer Register
   AT91_REG SSC_TNCR;                   // Transmit Next Counter Register
   AT91_REG SSC_PTCR;                   // PDC Transfer Control Register
   AT91_REG SSC_PTSR;                   // PDC Transfer Status Register
} AT91S_SSC, *AT91PS_SSC;

// -------- SSC_CR : (SSC Offset: 0x0) SSC Control Register --------
#define AT91C_SSC_RXEN        ((unsigned int) 0x1 <<  0) // (SSC) Receive Enable
#define AT91C_SSC_RXDIS       ((unsigned int) 0x1 <<  1) // (SSC) Receive Disable
#define AT91C_SSC_TXEN        ((unsigned int) 0x1 <<  8) // (SSC) Transmit Enable
#define AT91C_SSC_TXDIS       ((unsigned int) 0x1 <<  9) // (SSC) Transmit Disable
#define AT91C_SSC_SWRST       ((unsigned int) 0x1 << 15) // (SSC) Software Reset
// -------- SSC_RCMR : (SSC Offset: 0x10) SSC Receive Clock Mode Register --------
#define AT91C_SSC_CKS         ((unsigned int) 0x3 <<  0) // (SSC) Receive/Transmit Clock Selection
#define         AT91C_SSC_CKS_DIV                  ((unsigned int) 0x0) // (SSC) Divided Clock
#define         AT91C_SSC_CKS_TK                   ((unsigned int) 0x1) // (SSC) TK Clock signal
#define         AT91C_SSC_CKS_RK                   ((unsigned int) 0x2) // (SSC) RK pin
#define AT91C_SSC_CKO         ((unsigned int) 0x7 <<  2) // (SSC) Receive/Transmit Clock Output Mode Selection
#define         AT91C_SSC_CKO_NONE                 ((unsigned int) 0x0 <<  2) // (SSC) Receive/Transmit Clock Output Mode: None RK pin: Input-only
#define         AT91C_SSC_CKO_CONTINOUS            ((unsigned int) 0x1 <<  2) // (SSC) Continuous Receive/Transmit Clock RK pin: Output
#define         AT91C_SSC_CKO_DATA_TX              ((unsigned int) 0x2 <<  2) // (SSC) Receive/Transmit Clock only during data transfers RK pin: Output
#define AT91C_SSC_CKI         ((unsigned int) 0x1 <<  5) // (SSC) Receive/Transmit Clock Inversion
#define AT91C_SSC_CKG         ((unsigned int) 0x3 <<  6) // (SSC) Receive/Transmit Clock Gating Selection
#define         AT91C_SSC_CKG_NONE                 ((unsigned int) 0x0 <<  6) // (SSC) Receive/Transmit Clock Gating: None, continuous clock
#define         AT91C_SSC_CKG_LOW                  ((unsigned int) 0x1 <<  6) // (SSC) Receive/Transmit Clock enabled only if RF Low
#define         AT91C_SSC_CKG_HIGH                 ((unsigned int) 0x2 <<  6) // (SSC) Receive/Transmit Clock enabled only if RF High
#define AT91C_SSC_START       ((unsigned int) 0xF <<  8) // (SSC) Receive/Transmit Start Selection
#define         AT91C_SSC_START_CONTINOUS            ((unsigned int) 0x0 <<  8) // (SSC) Continuous, as soon as the receiver is enabled, and immediately after the end of transfer of the previous data.
#define         AT91C_SSC_START_TX                   ((unsigned int) 0x1 <<  8) // (SSC) Transmit/Receive start
#define         AT91C_SSC_START_LOW_RF               ((unsigned int) 0x2 <<  8) // (SSC) Detection of a low level on RF input
#define         AT91C_SSC_START_HIGH_RF              ((unsigned int) 0x3 <<  8) // (SSC) Detection of a high level on RF input
#define         AT91C_SSC_START_FALL_RF              ((unsigned int) 0x4 <<  8) // (SSC) Detection of a falling edge on RF input
#define         AT91C_SSC_START_RISE_RF              ((unsigned int) 0x5 <<  8) // (SSC) Detection of a rising edge on RF input
#define         AT91C_SSC_START_LEVEL_RF             ((unsigned int) 0x6 <<  8) // (SSC) Detection of any level change on RF input
#define         AT91C_SSC_START_EDGE_RF              ((unsigned int) 0x7 <<  8) // (SSC) Detection of any edge on RF input
#define         AT91C_SSC_START_0                    ((unsigned int) 0x8 <<  8) // (SSC) Compare 0
#define AT91C_SSC_STOP        ((unsigned int) 0x1 << 12) // (SSC) Receive Stop Selection
#define AT91C_SSC_STTDLY      ((unsigned int) 0xFF << 16) // (SSC) Receive/Transmit Start Delay
#define AT91C_SSC_PERIOD      ((unsigned int) 0xFF << 24) // (SSC) Receive/Transmit Period Divider Selection
// -------- SSC_RFMR : (SSC Offset: 0x14) SSC Receive Frame Mode Register --------
#define AT91C_SSC_DATLEN      ((unsigned int) 0x1F <<  0) // (SSC) Data Length
#define AT91C_SSC_LOOP        ((unsigned int) 0x1 <<  5) // (SSC) Loop Mode
#define AT91C_SSC_MSBF        ((unsigned int) 0x1 <<  7) // (SSC) Most Significant Bit First
#define AT91C_SSC_DATNB       ((unsigned int) 0xF <<  8) // (SSC) Data Number per Frame
#define AT91C_SSC_FSLEN       ((unsigned int) 0xF << 16) // (SSC) Receive/Transmit Frame Sync length
#define AT91C_SSC_FSOS        ((unsigned int) 0x7 << 20) // (SSC) Receive/Transmit Frame Sync Output Selection
#define         AT91C_SSC_FSOS_NONE                 ((unsigned int) 0x0 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: None RK pin Input-only
#define         AT91C_SSC_FSOS_NEGATIVE             ((unsigned int) 0x1 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: Negative Pulse
#define         AT91C_SSC_FSOS_POSITIVE             ((unsigned int) 0x2 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: Positive Pulse
#define         AT91C_SSC_FSOS_LOW                  ((unsigned int) 0x3 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: Driver Low during data transfer
#define         AT91C_SSC_FSOS_HIGH                 ((unsigned int) 0x4 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: Driver High during data transfer
#define         AT91C_SSC_FSOS_TOGGLE               ((unsigned int) 0x5 << 20) // (SSC) Selected Receive/Transmit Frame Sync Signal: Toggling at each start of data transfer
#define AT91C_SSC_FSEDGE      ((unsigned int) 0x1 << 24) // (SSC) Frame Sync Edge Detection
// -------- SSC_TCMR : (SSC Offset: 0x18) SSC Transmit Clock Mode Register --------
// -------- SSC_TFMR : (SSC Offset: 0x1c) SSC Transmit Frame Mode Register --------
#define AT91C_SSC_DATDEF      ((unsigned int) 0x1 <<  5) // (SSC) Data Default Value
#define AT91C_SSC_FSDEN       ((unsigned int) 0x1 << 23) // (SSC) Frame Sync Data Enable
// -------- SSC_SR : (SSC Offset: 0x40) SSC Status Register --------
#define AT91C_SSC_TXRDY       ((unsigned int) 0x1 <<  0) // (SSC) Transmit Ready
#define AT91C_SSC_TXEMPTY     ((unsigned int) 0x1 <<  1) // (SSC) Transmit Empty
#define AT91C_SSC_ENDTX       ((unsigned int) 0x1 <<  2) // (SSC) End Of Transmission
#define AT91C_SSC_TXBUFE      ((unsigned int) 0x1 <<  3) // (SSC) Transmit Buffer Empty
#define AT91C_SSC_RXRDY       ((unsigned int) 0x1 <<  4) // (SSC) Receive Ready
#define AT91C_SSC_OVRUN       ((unsigned int) 0x1 <<  5) // (SSC) Receive Overrun
#define AT91C_SSC_ENDRX       ((unsigned int) 0x1 <<  6) // (SSC) End of Reception
#define AT91C_SSC_RXBUFF      ((unsigned int) 0x1 <<  7) // (SSC) Receive Buffer Full
#define AT91C_SSC_CP0         ((unsigned int) 0x1 <<  8) // (SSC) Compare 0
#define AT91C_SSC_CP1         ((unsigned int) 0x1 <<  9) // (SSC) Compare 1
#define AT91C_SSC_TXSYN       ((unsigned int) 0x1 << 10) // (SSC) Transmit Sync
#define AT91C_SSC_RXSYN       ((unsigned int) 0x1 << 11) // (SSC) Receive Sync
#define AT91C_SSC_TXENA       ((unsigned int) 0x1 << 16) // (SSC) Transmit Enable
#define AT91C_SSC_RXENA       ((unsigned int) 0x1 << 17) // (SSC) Receive Enable
// -------- SSC_IER : (SSC Offset: 0x44) SSC Interrupt Enable Register --------
// -------- SSC_IDR : (SSC Offset: 0x48) SSC Interrupt Disable Register --------
// -------- SSC_IMR : (SSC Offset: 0x4c) SSC Interrupt Mask Register --------

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Two-wire Interface
// *****************************************************************************
typedef struct _AT91S_TWI {
   AT91_REG TWI_CR;                     // Control Register
   AT91_REG TWI_MMR;                    // Master Mode Register
   AT91_REG TWI_SMR;                    // Slave Mode Register
   AT91_REG TWI_IADR;                   // Internal Address Register
   AT91_REG TWI_CWGR;                   // Clock Waveform Generator Register
   AT91_REG Reserved0[3];               //
   AT91_REG TWI_SR;                     // Status Register
   AT91_REG TWI_IER;                    // Interrupt Enable Register
   AT91_REG TWI_IDR;                    // Interrupt Disable Register
   AT91_REG TWI_IMR;                    // Interrupt Mask Register
   AT91_REG TWI_RHR;                    // Receive Holding Register
   AT91_REG TWI_THR;                    // Transmit Holding Register
   AT91_REG Reserved1[50];                      //
   AT91_REG TWI_RPR;                    // Receive Pointer Register
   AT91_REG TWI_RCR;                    // Receive Counter Register
   AT91_REG TWI_TPR;                    // Transmit Pointer Register
   AT91_REG TWI_TCR;                    // Transmit Counter Register
   AT91_REG TWI_RNPR;                   // Receive Next Pointer Register
   AT91_REG TWI_RNCR;                   // Receive Next Counter Register
   AT91_REG TWI_TNPR;                   // Transmit Next Pointer Register
   AT91_REG TWI_TNCR;                   // Transmit Next Counter Register
   AT91_REG TWI_PTCR;                   // PDC Transfer Control Register
   AT91_REG TWI_PTSR;                   // PDC Transfer Status Register
} AT91S_TWI, *AT91PS_TWI;

// -------- TWI_CR : (TWI Offset: 0x0) TWI Control Register --------
#define AT91C_TWI_START       ((unsigned int) 0x1 <<  0) // (TWI) Send a START Condition
#define AT91C_TWI_STOP        ((unsigned int) 0x1 <<  1) // (TWI) Send a STOP Condition
#define AT91C_TWI_MSEN        ((unsigned int) 0x1 <<  2) // (TWI) TWI Master Transfer Enabled
#define AT91C_TWI_MSDIS       ((unsigned int) 0x1 <<  3) // (TWI) TWI Master Transfer Disabled
#define AT91C_TWI_SVEN        ((unsigned int) 0x1 <<  4) // (TWI) TWI Slave mode Enabled
#define AT91C_TWI_SVDIS       ((unsigned int) 0x1 <<  5) // (TWI) TWI Slave mode Disabled
#define AT91C_TWI_SWRST       ((unsigned int) 0x1 <<  7) // (TWI) Software Reset
// -------- TWI_MMR : (TWI Offset: 0x4) TWI Master Mode Register --------
#define AT91C_TWI_IADRSZ      ((unsigned int) 0x3 <<  8) // (TWI) Internal Device Address Size
#define         AT91C_TWI_IADRSZ_NO                   ((unsigned int) 0x0 <<  8) // (TWI) No internal device address
#define         AT91C_TWI_IADRSZ_1_BYTE               ((unsigned int) 0x1 <<  8) // (TWI) One-byte internal device address
#define         AT91C_TWI_IADRSZ_2_BYTE               ((unsigned int) 0x2 <<  8) // (TWI) Two-byte internal device address
#define         AT91C_TWI_IADRSZ_3_BYTE               ((unsigned int) 0x3 <<  8) // (TWI) Three-byte internal device address
#define AT91C_TWI_MREAD       ((unsigned int) 0x1 << 12) // (TWI) Master Read Direction
#define AT91C_TWI_DADR        ((unsigned int) 0x7F << 16) // (TWI) Device Address
// -------- TWI_SMR : (TWI Offset: 0x8) TWI Slave Mode Register --------
#define AT91C_TWI_SADR        ((unsigned int) 0x7F << 16) // (TWI) Slave Address
// -------- TWI_CWGR : (TWI Offset: 0x10) TWI Clock Waveform Generator Register --------
#define AT91C_TWI_CLDIV       ((unsigned int) 0xFF <<  0) // (TWI) Clock Low Divider
#define AT91C_TWI_CHDIV       ((unsigned int) 0xFF <<  8) // (TWI) Clock High Divider
#define AT91C_TWI_CKDIV       ((unsigned int) 0x7 << 16) // (TWI) Clock Divider
// -------- TWI_SR : (TWI Offset: 0x20) TWI Status Register --------
#define AT91C_TWI_TXCOMP_SLAVE ((unsigned int) 0x1 <<  0) // (TWI) Transmission Completed
#define AT91C_TWI_TXCOMP_MASTER ((unsigned int) 0x1 <<  0) // (TWI) Transmission Completed
#define AT91C_TWI_RXRDY       ((unsigned int) 0x1 <<  1) // (TWI) Receive holding register ReaDY
#define AT91C_TWI_TXRDY_MASTER ((unsigned int) 0x1 <<  2) // (TWI) Transmit holding register ReaDY
#define AT91C_TWI_TXRDY_SLAVE ((unsigned int) 0x1 <<  2) // (TWI) Transmit holding register ReaDY
#define AT91C_TWI_SVREAD      ((unsigned int) 0x1 <<  3) // (TWI) Slave READ (used only in Slave mode)
#define AT91C_TWI_SVACC       ((unsigned int) 0x1 <<  4) // (TWI) Slave ACCess (used only in Slave mode)
#define AT91C_TWI_GACC        ((unsigned int) 0x1 <<  5) // (TWI) General Call ACcess (used only in Slave mode)
#define AT91C_TWI_OVRE        ((unsigned int) 0x1 <<  6) // (TWI) Overrun Error (used only in Master and Multi-master mode)
#define AT91C_TWI_NACK_MASTER ((unsigned int) 0x1 <<  8) // (TWI) Not Acknowledged
#define AT91C_TWI_NACK_SLAVE  ((unsigned int) 0x1 <<  8) // (TWI) Not Acknowledged
#define AT91C_TWI_ARBLST_MULTI_MASTER ((unsigned int) 0x1 <<  9) // (TWI) Arbitration Lost (used only in Multimaster mode)
#define AT91C_TWI_SCLWS       ((unsigned int) 0x1 << 10) // (TWI) Clock Wait State (used only in Slave mode)
#define AT91C_TWI_EOSACC      ((unsigned int) 0x1 << 11) // (TWI) End Of Slave ACCess (used only in Slave mode)
#define AT91C_TWI_ENDRX       ((unsigned int) 0x1 << 12) // (TWI) End of Receiver Transfer
#define AT91C_TWI_ENDTX       ((unsigned int) 0x1 << 13) // (TWI) End of Receiver Transfer
#define AT91C_TWI_RXBUFF      ((unsigned int) 0x1 << 14) // (TWI) RXBUFF Interrupt
#define AT91C_TWI_TXBUFE      ((unsigned int) 0x1 << 15) // (TWI) TXBUFE Interrupt
// -------- TWI_IER : (TWI Offset: 0x24) TWI Interrupt Enable Register --------
// -------- TWI_IDR : (TWI Offset: 0x28) TWI Interrupt Disable Register --------
// -------- TWI_IMR : (TWI Offset: 0x2c) TWI Interrupt Mask Register --------

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR PWMC Channel Interface
// *****************************************************************************
typedef struct _AT91S_PWMC_CH {
   AT91_REG PWMC_CMR;                   // Channel Mode Register
   AT91_REG PWMC_CDTYR;                 // Channel Duty Cycle Register
   AT91_REG PWMC_CPRDR;                 // Channel Period Register
   AT91_REG PWMC_CCNTR;                 // Channel Counter Register
   AT91_REG PWMC_CUPDR;                 // Channel Update Register
   AT91_REG PWMC_Reserved[3];                   // Reserved
} AT91S_PWMC_CH, *AT91PS_PWMC_CH;

// -------- PWMC_CMR : (PWMC_CH Offset: 0x0) PWMC Channel Mode Register --------
#define AT91C_PWMC_CPRE       ((unsigned int) 0xF <<  0) // (PWMC_CH) Channel Pre-scaler : PWMC_CLKx
#define         AT91C_PWMC_CPRE_MCK                  ((unsigned int) 0x0) // (PWMC_CH)
#define         AT91C_PWMC_CPRE_MCKA                 ((unsigned int) 0xB) // (PWMC_CH)
#define         AT91C_PWMC_CPRE_MCKB                 ((unsigned int) 0xC) // (PWMC_CH)
#define AT91C_PWMC_CALG       ((unsigned int) 0x1 <<  8) // (PWMC_CH) Channel Alignment
#define AT91C_PWMC_CPOL       ((unsigned int) 0x1 <<  9) // (PWMC_CH) Channel Polarity
#define AT91C_PWMC_CPD        ((unsigned int) 0x1 << 10) // (PWMC_CH) Channel Update Period
// -------- PWMC_CDTYR : (PWMC_CH Offset: 0x4) PWMC Channel Duty Cycle Register --------
#define AT91C_PWMC_CDTY       ((unsigned int) 0x0 <<  0) // (PWMC_CH) Channel Duty Cycle
// -------- PWMC_CPRDR : (PWMC_CH Offset: 0x8) PWMC Channel Period Register --------
#define AT91C_PWMC_CPRD       ((unsigned int) 0x0 <<  0) // (PWMC_CH) Channel Period
// -------- PWMC_CCNTR : (PWMC_CH Offset: 0xc) PWMC Channel Counter Register --------
#define AT91C_PWMC_CCNT       ((unsigned int) 0x0 <<  0) // (PWMC_CH) Channel Counter
// -------- PWMC_CUPDR : (PWMC_CH Offset: 0x10) PWMC Channel Update Register --------
#define AT91C_PWMC_CUPD       ((unsigned int) 0x0 <<  0) // (PWMC_CH) Channel Update

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Pulse Width Modulation Controller Interface
// *****************************************************************************
typedef struct _AT91S_PWMC {
   AT91_REG PWMC_MR;                    // PWMC Mode Register
   AT91_REG PWMC_ENA;                   // PWMC Enable Register
   AT91_REG PWMC_DIS;                   // PWMC Disable Register
   AT91_REG PWMC_SR;                    // PWMC Status Register
   AT91_REG PWMC_IER;                   // PWMC Interrupt Enable Register
   AT91_REG PWMC_IDR;                   // PWMC Interrupt Disable Register
   AT91_REG PWMC_IMR;                   // PWMC Interrupt Mask Register
   AT91_REG PWMC_ISR;                   // PWMC Interrupt Status Register
   AT91_REG Reserved0[55];                      //
   AT91_REG PWMC_VR;                    // PWMC Version Register
   AT91_REG Reserved1[64];                      //
   AT91S_PWMC_CH PWMC_CH[4];            // PWMC Channel
} AT91S_PWMC, *AT91PS_PWMC;

// -------- PWMC_MR : (PWMC Offset: 0x0) PWMC Mode Register --------
#define AT91C_PWMC_DIVA       ((unsigned int) 0xFF <<  0) // (PWMC) CLKA divide factor.
#define AT91C_PWMC_PREA       ((unsigned int) 0xF <<  8) // (PWMC) Divider Input Clock Prescaler A
#define         AT91C_PWMC_PREA_MCK                  ((unsigned int) 0x0 <<  8) // (PWMC)
#define AT91C_PWMC_DIVB       ((unsigned int) 0xFF << 16) // (PWMC) CLKB divide factor.
#define AT91C_PWMC_PREB       ((unsigned int) 0xF << 24) // (PWMC) Divider Input Clock Prescaler B
#define         AT91C_PWMC_PREB_MCK                  ((unsigned int) 0x0 << 24) // (PWMC)
// -------- PWMC_ENA : (PWMC Offset: 0x4) PWMC Enable Register --------
#define AT91C_PWMC_CHID0      ((unsigned int) 0x1 <<  0) // (PWMC) Channel ID 0
#define AT91C_PWMC_CHID1      ((unsigned int) 0x1 <<  1) // (PWMC) Channel ID 1
#define AT91C_PWMC_CHID2      ((unsigned int) 0x1 <<  2) // (PWMC) Channel ID 2
#define AT91C_PWMC_CHID3      ((unsigned int) 0x1 <<  3) // (PWMC) Channel ID 3
// -------- PWMC_DIS : (PWMC Offset: 0x8) PWMC Disable Register --------
// -------- PWMC_SR : (PWMC Offset: 0xc) PWMC Status Register --------
// -------- PWMC_IER : (PWMC Offset: 0x10) PWMC Interrupt Enable Register --------
// -------- PWMC_IDR : (PWMC Offset: 0x14) PWMC Interrupt Disable Register --------
// -------- PWMC_IMR : (PWMC Offset: 0x18) PWMC Interrupt Mask Register --------
// -------- PWMC_ISR : (PWMC Offset: 0x1c) PWMC Interrupt Status Register --------

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR USB Device Interface
// *****************************************************************************
typedef struct _AT91S_UDP {
   AT91_REG UDP_NUM;                    // Frame Number Register
   AT91_REG UDP_GLBSTATE;               // Global State Register
   AT91_REG UDP_FADDR;                  // Function Address Register
   AT91_REG Reserved0[1];               //
   AT91_REG UDP_IER;                    // Interrupt Enable Register
   AT91_REG UDP_IDR;                    // Interrupt Disable Register
   AT91_REG UDP_IMR;                    // Interrupt Mask Register
   AT91_REG UDP_ISR;                    // Interrupt Status Register
   AT91_REG UDP_ICR;                    // Interrupt Clear Register
   AT91_REG Reserved1[1];               //
   AT91_REG UDP_RSTEP;                  // Reset Endpoint Register
   AT91_REG Reserved2[1];               //
   AT91_REG UDP_CSR[6];                 // Endpoint Control and Status Register
   AT91_REG Reserved3[2];               //
   AT91_REG UDP_FDR[6];                 // Endpoint FIFO Data Register
   AT91_REG Reserved4[3];               //
   AT91_REG UDP_TXVC;                   // Transceiver Control Register
} AT91S_UDP, *AT91PS_UDP;

// -------- UDP_FRM_NUM : (UDP Offset: 0x0) USB Frame Number Register --------
#define AT91C_UDP_FRM_NUM     ((unsigned int) 0x7FF <<  0) // (UDP) Frame Number as Defined in the Packet Field Formats
#define AT91C_UDP_FRM_ERR     ((unsigned int) 0x1 << 16) // (UDP) Frame Error
#define AT91C_UDP_FRM_OK      ((unsigned int) 0x1 << 17) // (UDP) Frame OK
// -------- UDP_GLB_STATE : (UDP Offset: 0x4) USB Global State Register --------
#define AT91C_UDP_FADDEN      ((unsigned int) 0x1 <<  0) // (UDP) Function Address Enable
#define AT91C_UDP_CONFG       ((unsigned int) 0x1 <<  1) // (UDP) Configured
#define AT91C_UDP_ESR         ((unsigned int) 0x1 <<  2) // (UDP) Enable Send Resume
#define AT91C_UDP_RSMINPR     ((unsigned int) 0x1 <<  3) // (UDP) A Resume Has Been Sent to the Host
#define AT91C_UDP_RMWUPE      ((unsigned int) 0x1 <<  4) // (UDP) Remote Wake Up Enable
// -------- UDP_FADDR : (UDP Offset: 0x8) USB Function Address Register --------
#define AT91C_UDP_FADD        ((unsigned int) 0xFF <<  0) // (UDP) Function Address Value
#define AT91C_UDP_FEN         ((unsigned int) 0x1 <<  8) // (UDP) Function Enable
// -------- UDP_IER : (UDP Offset: 0x10) USB Interrupt Enable Register --------
#define AT91C_UDP_EPINT0      ((unsigned int) 0x1 <<  0) // (UDP) Endpoint 0 Interrupt
#define AT91C_UDP_EPINT1      ((unsigned int) 0x1 <<  1) // (UDP) Endpoint 0 Interrupt
#define AT91C_UDP_EPINT2      ((unsigned int) 0x1 <<  2) // (UDP) Endpoint 2 Interrupt
#define AT91C_UDP_EPINT3      ((unsigned int) 0x1 <<  3) // (UDP) Endpoint 3 Interrupt
#define AT91C_UDP_EPINT4      ((unsigned int) 0x1 <<  4) // (UDP) Endpoint 4 Interrupt
#define AT91C_UDP_EPINT5      ((unsigned int) 0x1 <<  5) // (UDP) Endpoint 5 Interrupt
#define AT91C_UDP_RXSUSP      ((unsigned int) 0x1 <<  8) // (UDP) USB Suspend Interrupt
#define AT91C_UDP_RXRSM       ((unsigned int) 0x1 <<  9) // (UDP) USB Resume Interrupt
#define AT91C_UDP_EXTRSM      ((unsigned int) 0x1 << 10) // (UDP) USB External Resume Interrupt
#define AT91C_UDP_SOFINT      ((unsigned int) 0x1 << 11) // (UDP) USB Start Of frame Interrupt
#define AT91C_UDP_WAKEUP      ((unsigned int) 0x1 << 13) // (UDP) USB Resume Interrupt
// -------- UDP_IDR : (UDP Offset: 0x14) USB Interrupt Disable Register --------
// -------- UDP_IMR : (UDP Offset: 0x18) USB Interrupt Mask Register --------
// -------- UDP_ISR : (UDP Offset: 0x1c) USB Interrupt Status Register --------
#define AT91C_UDP_ENDBUSRES   ((unsigned int) 0x1 << 12) // (UDP) USB End Of Bus Reset Interrupt
// -------- UDP_ICR : (UDP Offset: 0x20) USB Interrupt Clear Register --------
// -------- UDP_RST_EP : (UDP Offset: 0x28) USB Reset Endpoint Register --------
#define AT91C_UDP_EP0         ((unsigned int) 0x1 <<  0) // (UDP) Reset Endpoint 0
#define AT91C_UDP_EP1         ((unsigned int) 0x1 <<  1) // (UDP) Reset Endpoint 1
#define AT91C_UDP_EP2         ((unsigned int) 0x1 <<  2) // (UDP) Reset Endpoint 2
#define AT91C_UDP_EP3         ((unsigned int) 0x1 <<  3) // (UDP) Reset Endpoint 3
#define AT91C_UDP_EP4         ((unsigned int) 0x1 <<  4) // (UDP) Reset Endpoint 4
#define AT91C_UDP_EP5         ((unsigned int) 0x1 <<  5) // (UDP) Reset Endpoint 5
// -------- UDP_CSR : (UDP Offset: 0x30) USB Endpoint Control and Status Register --------
#define AT91C_UDP_TXCOMP      ((unsigned int) 0x1 <<  0) // (UDP) Generates an IN packet with data previously written in the DPR
#define AT91C_UDP_RX_DATA_BK0 ((unsigned int) 0x1 <<  1) // (UDP) Receive Data Bank 0
#define AT91C_UDP_RXSETUP     ((unsigned int) 0x1 <<  2) // (UDP) Sends STALL to the Host (Control endpoints)
#define AT91C_UDP_ISOERROR    ((unsigned int) 0x1 <<  3) // (UDP) Isochronous error (Isochronous endpoints)
#define AT91C_UDP_TXPKTRDY    ((unsigned int) 0x1 <<  4) // (UDP) Transmit Packet Ready
#define AT91C_UDP_FORCESTALL  ((unsigned int) 0x1 <<  5) // (UDP) Force Stall (used by Control, Bulk and Isochronous endpoints).
#define AT91C_UDP_RX_DATA_BK1 ((unsigned int) 0x1 <<  6) // (UDP) Receive Data Bank 1 (only used by endpoints with ping-pong attributes).
#define AT91C_UDP_DIR         ((unsigned int) 0x1 <<  7) // (UDP) Transfer Direction
#define AT91C_UDP_EPTYPE      ((unsigned int) 0x7 <<  8) // (UDP) Endpoint type
#define         AT91C_UDP_EPTYPE_CTRL                 ((unsigned int) 0x0 <<  8) // (UDP) Control
#define         AT91C_UDP_EPTYPE_ISO_OUT              ((unsigned int) 0x1 <<  8) // (UDP) Isochronous OUT
#define         AT91C_UDP_EPTYPE_BULK_OUT             ((unsigned int) 0x2 <<  8) // (UDP) Bulk OUT
#define         AT91C_UDP_EPTYPE_INT_OUT              ((unsigned int) 0x3 <<  8) // (UDP) Interrupt OUT
#define         AT91C_UDP_EPTYPE_ISO_IN               ((unsigned int) 0x5 <<  8) // (UDP) Isochronous IN
#define         AT91C_UDP_EPTYPE_BULK_IN              ((unsigned int) 0x6 <<  8) // (UDP) Bulk IN
#define         AT91C_UDP_EPTYPE_INT_IN               ((unsigned int) 0x7 <<  8) // (UDP) Interrupt IN
#define AT91C_UDP_DTGLE       ((unsigned int) 0x1 << 11) // (UDP) Data Toggle
#define AT91C_UDP_EPEDS       ((unsigned int) 0x1 << 15) // (UDP) Endpoint Enable Disable
#define AT91C_UDP_RXBYTECNT   ((unsigned int) 0x7FF << 16) // (UDP) Number Of Bytes Available in the FIFO
// -------- UDP_TXVC : (UDP Offset: 0x74) Transceiver Control Register --------
#define AT91C_UDP_TXVDIS      ((unsigned int) 0x1 <<  8) // (UDP)
#define AT91C_UDP_PUON        ((unsigned int) 0x1 <<  9) // (UDP) Pull-up ON

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Timer Counter Channel Interface
// *****************************************************************************
typedef struct _AT91S_TC {
   AT91_REG TC_CCR;                     // Channel Control Register
   AT91_REG TC_CMR;                     // Channel Mode Register (Capture Mode / Waveform Mode)
   AT91_REG Reserved0[2];               //
   AT91_REG TC_CV;                      // Counter Value
   AT91_REG TC_RA;                      // Register A
   AT91_REG TC_RB;                      // Register B
   AT91_REG TC_RC;                      // Register C
   AT91_REG TC_SR;                      // Status Register
   AT91_REG TC_IER;                     // Interrupt Enable Register
   AT91_REG TC_IDR;                     // Interrupt Disable Register
   AT91_REG TC_IMR;                     // Interrupt Mask Register
} AT91S_TC, *AT91PS_TC;

// -------- TC_CCR : (TC Offset: 0x0) TC Channel Control Register --------
#define AT91C_TC_CLKEN        ((unsigned int) 0x1 <<  0) // (TC) Counter Clock Enable Command
#define AT91C_TC_CLKDIS       ((unsigned int) 0x1 <<  1) // (TC) Counter Clock Disable Command
#define AT91C_TC_SWTRG        ((unsigned int) 0x1 <<  2) // (TC) Software Trigger Command
// -------- TC_CMR : (TC Offset: 0x4) TC Channel Mode Register: Capture Mode / Waveform Mode --------
#define AT91C_TC_CLKS         ((unsigned int) 0x7 <<  0) // (TC) Clock Selection
#define         AT91C_TC_CLKS_TIMER_DIV1_CLOCK     ((unsigned int) 0x0) // (TC) Clock selected: TIMER_DIV1_CLOCK
#define         AT91C_TC_CLKS_TIMER_DIV2_CLOCK     ((unsigned int) 0x1) // (TC) Clock selected: TIMER_DIV2_CLOCK
#define         AT91C_TC_CLKS_TIMER_DIV3_CLOCK     ((unsigned int) 0x2) // (TC) Clock selected: TIMER_DIV3_CLOCK
#define         AT91C_TC_CLKS_TIMER_DIV4_CLOCK     ((unsigned int) 0x3) // (TC) Clock selected: TIMER_DIV4_CLOCK
#define         AT91C_TC_CLKS_TIMER_DIV5_CLOCK     ((unsigned int) 0x4) // (TC) Clock selected: TIMER_DIV5_CLOCK
#define         AT91C_TC_CLKS_XC0                  ((unsigned int) 0x5) // (TC) Clock selected: XC0
#define         AT91C_TC_CLKS_XC1                  ((unsigned int) 0x6) // (TC) Clock selected: XC1
#define         AT91C_TC_CLKS_XC2                  ((unsigned int) 0x7) // (TC) Clock selected: XC2
#define AT91C_TC_CLKI         ((unsigned int) 0x1 <<  3) // (TC) Clock Invert
#define AT91C_TC_BURST        ((unsigned int) 0x3 <<  4) // (TC) Burst Signal Selection
#define         AT91C_TC_BURST_NONE                 ((unsigned int) 0x0 <<  4) // (TC) The clock is not gated by an external signal
#define         AT91C_TC_BURST_XC0                  ((unsigned int) 0x1 <<  4) // (TC) XC0 is ANDed with the selected clock
#define         AT91C_TC_BURST_XC1                  ((unsigned int) 0x2 <<  4) // (TC) XC1 is ANDed with the selected clock
#define         AT91C_TC_BURST_XC2                  ((unsigned int) 0x3 <<  4) // (TC) XC2 is ANDed with the selected clock
#define AT91C_TC_CPCSTOP      ((unsigned int) 0x1 <<  6) // (TC) Counter Clock Stopped with RC Compare
#define AT91C_TC_LDBSTOP      ((unsigned int) 0x1 <<  6) // (TC) Counter Clock Stopped with RB Loading
#define AT91C_TC_LDBDIS       ((unsigned int) 0x1 <<  7) // (TC) Counter Clock Disabled with RB Loading
#define AT91C_TC_CPCDIS       ((unsigned int) 0x1 <<  7) // (TC) Counter Clock Disable with RC Compare
#define AT91C_TC_ETRGEDG      ((unsigned int) 0x3 <<  8) // (TC) External Trigger Edge Selection
#define         AT91C_TC_ETRGEDG_NONE                 ((unsigned int) 0x0 <<  8) // (TC) Edge: None
#define         AT91C_TC_ETRGEDG_RISING               ((unsigned int) 0x1 <<  8) // (TC) Edge: rising edge
#define         AT91C_TC_ETRGEDG_FALLING              ((unsigned int) 0x2 <<  8) // (TC) Edge: falling edge
#define         AT91C_TC_ETRGEDG_BOTH                 ((unsigned int) 0x3 <<  8) // (TC) Edge: each edge
#define AT91C_TC_EEVTEDG      ((unsigned int) 0x3 <<  8) // (TC) External Event Edge Selection
#define         AT91C_TC_EEVTEDG_NONE                 ((unsigned int) 0x0 <<  8) // (TC) Edge: None
#define         AT91C_TC_EEVTEDG_RISING               ((unsigned int) 0x1 <<  8) // (TC) Edge: rising edge
#define         AT91C_TC_EEVTEDG_FALLING              ((unsigned int) 0x2 <<  8) // (TC) Edge: falling edge
#define         AT91C_TC_EEVTEDG_BOTH                 ((unsigned int) 0x3 <<  8) // (TC) Edge: each edge
#define AT91C_TC_ABETRG       ((unsigned int) 0x1 << 10) // (TC) TIOA or TIOB External Trigger Selection
#define AT91C_TC_EEVT         ((unsigned int) 0x3 << 10) // (TC) External Event  Selection
#define         AT91C_TC_EEVT_TIOB                 ((unsigned int) 0x0 << 10) // (TC) Signal selected as external event: TIOB TIOB direction: input
#define         AT91C_TC_EEVT_XC0                  ((unsigned int) 0x1 << 10) // (TC) Signal selected as external event: XC0 TIOB direction: output
#define         AT91C_TC_EEVT_XC1                  ((unsigned int) 0x2 << 10) // (TC) Signal selected as external event: XC1 TIOB direction: output
#define         AT91C_TC_EEVT_XC2                  ((unsigned int) 0x3 << 10) // (TC) Signal selected as external event: XC2 TIOB direction: output
#define AT91C_TC_ENETRG       ((unsigned int) 0x1 << 12) // (TC) External Event Trigger enable
#define AT91C_TC_WAVESEL      ((unsigned int) 0x3 << 13) // (TC) Waveform  Selection
#define         AT91C_TC_WAVESEL_UP                   ((unsigned int) 0x0 << 13) // (TC) UP mode without atomatic trigger on RC Compare
#define         AT91C_TC_WAVESEL_UPDOWN               ((unsigned int) 0x1 << 13) // (TC) UPDOWN mode without automatic trigger on RC Compare
#define         AT91C_TC_WAVESEL_UP_AUTO              ((unsigned int) 0x2 << 13) // (TC) UP mode with automatic trigger on RC Compare
#define         AT91C_TC_WAVESEL_UPDOWN_AUTO          ((unsigned int) 0x3 << 13) // (TC) UPDOWN mode with automatic trigger on RC Compare
#define AT91C_TC_CPCTRG       ((unsigned int) 0x1 << 14) // (TC) RC Compare Trigger Enable
#define AT91C_TC_WAVE         ((unsigned int) 0x1 << 15) // (TC)
#define AT91C_TC_LDRA         ((unsigned int) 0x3 << 16) // (TC) RA Loading Selection
#define         AT91C_TC_LDRA_NONE                 ((unsigned int) 0x0 << 16) // (TC) Edge: None
#define         AT91C_TC_LDRA_RISING               ((unsigned int) 0x1 << 16) // (TC) Edge: rising edge of TIOA
#define         AT91C_TC_LDRA_FALLING              ((unsigned int) 0x2 << 16) // (TC) Edge: falling edge of TIOA
#define         AT91C_TC_LDRA_BOTH                 ((unsigned int) 0x3 << 16) // (TC) Edge: each edge of TIOA
#define AT91C_TC_ACPA         ((unsigned int) 0x3 << 16) // (TC) RA Compare Effect on TIOA
#define         AT91C_TC_ACPA_NONE                 ((unsigned int) 0x0 << 16) // (TC) Effect: none
#define         AT91C_TC_ACPA_SET                  ((unsigned int) 0x1 << 16) // (TC) Effect: set
#define         AT91C_TC_ACPA_CLEAR                ((unsigned int) 0x2 << 16) // (TC) Effect: clear
#define         AT91C_TC_ACPA_TOGGLE               ((unsigned int) 0x3 << 16) // (TC) Effect: toggle
#define AT91C_TC_LDRB         ((unsigned int) 0x3 << 18) // (TC) RB Loading Selection
#define         AT91C_TC_LDRB_NONE                 ((unsigned int) 0x0 << 18) // (TC) Edge: None
#define         AT91C_TC_LDRB_RISING               ((unsigned int) 0x1 << 18) // (TC) Edge: rising edge of TIOA
#define         AT91C_TC_LDRB_FALLING              ((unsigned int) 0x2 << 18) // (TC) Edge: falling edge of TIOA
#define         AT91C_TC_LDRB_BOTH                 ((unsigned int) 0x3 << 18) // (TC) Edge: each edge of TIOA
#define AT91C_TC_ACPC         ((unsigned int) 0x3 << 18) // (TC) RC Compare Effect on TIOA
#define         AT91C_TC_ACPC_NONE                 ((unsigned int) 0x0 << 18) // (TC) Effect: none
#define         AT91C_TC_ACPC_SET                  ((unsigned int) 0x1 << 18) // (TC) Effect: set
#define         AT91C_TC_ACPC_CLEAR                ((unsigned int) 0x2 << 18) // (TC) Effect: clear
#define         AT91C_TC_ACPC_TOGGLE               ((unsigned int) 0x3 << 18) // (TC) Effect: toggle
#define AT91C_TC_AEEVT        ((unsigned int) 0x3 << 20) // (TC) External Event Effect on TIOA
#define         AT91C_TC_AEEVT_NONE                 ((unsigned int) 0x0 << 20) // (TC) Effect: none
#define         AT91C_TC_AEEVT_SET                  ((unsigned int) 0x1 << 20) // (TC) Effect: set
#define         AT91C_TC_AEEVT_CLEAR                ((unsigned int) 0x2 << 20) // (TC) Effect: clear
#define         AT91C_TC_AEEVT_TOGGLE               ((unsigned int) 0x3 << 20) // (TC) Effect: toggle
#define AT91C_TC_ASWTRG       ((unsigned int) 0x3 << 22) // (TC) Software Trigger Effect on TIOA
#define         AT91C_TC_ASWTRG_NONE                 ((unsigned int) 0x0 << 22) // (TC) Effect: none
#define         AT91C_TC_ASWTRG_SET                  ((unsigned int) 0x1 << 22) // (TC) Effect: set
#define         AT91C_TC_ASWTRG_CLEAR                ((unsigned int) 0x2 << 22) // (TC) Effect: clear
#define         AT91C_TC_ASWTRG_TOGGLE               ((unsigned int) 0x3 << 22) // (TC) Effect: toggle
#define AT91C_TC_BCPB         ((unsigned int) 0x3 << 24) // (TC) RB Compare Effect on TIOB
#define         AT91C_TC_BCPB_NONE                 ((unsigned int) 0x0 << 24) // (TC) Effect: none
#define         AT91C_TC_BCPB_SET                  ((unsigned int) 0x1 << 24) // (TC) Effect: set
#define         AT91C_TC_BCPB_CLEAR                ((unsigned int) 0x2 << 24) // (TC) Effect: clear
#define         AT91C_TC_BCPB_TOGGLE               ((unsigned int) 0x3 << 24) // (TC) Effect: toggle
#define AT91C_TC_BCPC         ((unsigned int) 0x3 << 26) // (TC) RC Compare Effect on TIOB
#define         AT91C_TC_BCPC_NONE                 ((unsigned int) 0x0 << 26) // (TC) Effect: none
#define         AT91C_TC_BCPC_SET                  ((unsigned int) 0x1 << 26) // (TC) Effect: set
#define         AT91C_TC_BCPC_CLEAR                ((unsigned int) 0x2 << 26) // (TC) Effect: clear
#define         AT91C_TC_BCPC_TOGGLE               ((unsigned int) 0x3 << 26) // (TC) Effect: toggle
#define AT91C_TC_BEEVT        ((unsigned int) 0x3 << 28) // (TC) External Event Effect on TIOB
#define         AT91C_TC_BEEVT_NONE                 ((unsigned int) 0x0 << 28) // (TC) Effect: none
#define         AT91C_TC_BEEVT_SET                  ((unsigned int) 0x1 << 28) // (TC) Effect: set
#define         AT91C_TC_BEEVT_CLEAR                ((unsigned int) 0x2 << 28) // (TC) Effect: clear
#define         AT91C_TC_BEEVT_TOGGLE               ((unsigned int) 0x3 << 28) // (TC) Effect: toggle
#define AT91C_TC_BSWTRG       ((unsigned int) 0x3 << 30) // (TC) Software Trigger Effect on TIOB
#define         AT91C_TC_BSWTRG_NONE                 ((unsigned int) 0x0 << 30) // (TC) Effect: none
#define         AT91C_TC_BSWTRG_SET                  ((unsigned int) 0x1 << 30) // (TC) Effect: set
#define         AT91C_TC_BSWTRG_CLEAR                ((unsigned int) 0x2 << 30) // (TC) Effect: clear
#define         AT91C_TC_BSWTRG_TOGGLE               ((unsigned int) 0x3 << 30) // (TC) Effect: toggle
// -------- TC_SR : (TC Offset: 0x20) TC Channel Status Register --------
#define AT91C_TC_COVFS        ((unsigned int) 0x1 <<  0) // (TC) Counter Overflow
#define AT91C_TC_LOVRS        ((unsigned int) 0x1 <<  1) // (TC) Load Overrun
#define AT91C_TC_CPAS         ((unsigned int) 0x1 <<  2) // (TC) RA Compare
#define AT91C_TC_CPBS         ((unsigned int) 0x1 <<  3) // (TC) RB Compare
#define AT91C_TC_CPCS         ((unsigned int) 0x1 <<  4) // (TC) RC Compare
#define AT91C_TC_LDRAS        ((unsigned int) 0x1 <<  5) // (TC) RA Loading
#define AT91C_TC_LDRBS        ((unsigned int) 0x1 <<  6) // (TC) RB Loading
#define AT91C_TC_ETRGS        ((unsigned int) 0x1 <<  7) // (TC) External Trigger
#define AT91C_TC_CLKSTA       ((unsigned int) 0x1 << 16) // (TC) Clock Enabling
#define AT91C_TC_MTIOA        ((unsigned int) 0x1 << 17) // (TC) TIOA Mirror
#define AT91C_TC_MTIOB        ((unsigned int) 0x1 << 18) // (TC) TIOA Mirror
// -------- TC_IER : (TC Offset: 0x24) TC Channel Interrupt Enable Register --------
// -------- TC_IDR : (TC Offset: 0x28) TC Channel Interrupt Disable Register --------
// -------- TC_IMR : (TC Offset: 0x2c) TC Channel Interrupt Mask Register --------

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Timer Counter Interface
// *****************************************************************************
typedef struct _AT91S_TCB {
   AT91S_TC TCB_TC0;                    // TC Channel 0
   AT91_REG Reserved0[4];               //
   AT91S_TC TCB_TC1;                    // TC Channel 1
   AT91_REG Reserved1[4];               //
   AT91S_TC TCB_TC2;                    // TC Channel 2
   AT91_REG Reserved2[4];               //
   AT91_REG TCB_BCR;                    // TC Block Control Register
   AT91_REG TCB_BMR;                    // TC Block Mode Register
} AT91S_TCB, *AT91PS_TCB;

// -------- TCB_BCR : (TCB Offset: 0xc0) TC Block Control Register --------
#define AT91C_TCB_SYNC        ((unsigned int) 0x1 <<  0) // (TCB) Synchro Command
// -------- TCB_BMR : (TCB Offset: 0xc4) TC Block Mode Register --------
#define AT91C_TCB_TC0XC0S     ((unsigned int) 0x3 <<  0) // (TCB) External Clock Signal 0 Selection
#define         AT91C_TCB_TC0XC0S_TCLK0                ((unsigned int) 0x0) // (TCB) TCLK0 connected to XC0
#define         AT91C_TCB_TC0XC0S_NONE                 ((unsigned int) 0x1) // (TCB) None signal connected to XC0
#define         AT91C_TCB_TC0XC0S_TIOA1                ((unsigned int) 0x2) // (TCB) TIOA1 connected to XC0
#define         AT91C_TCB_TC0XC0S_TIOA2                ((unsigned int) 0x3) // (TCB) TIOA2 connected to XC0
#define AT91C_TCB_TC1XC1S     ((unsigned int) 0x3 <<  2) // (TCB) External Clock Signal 1 Selection
#define         AT91C_TCB_TC1XC1S_TCLK1                ((unsigned int) 0x0 <<  2) // (TCB) TCLK1 connected to XC1
#define         AT91C_TCB_TC1XC1S_NONE                 ((unsigned int) 0x1 <<  2) // (TCB) None signal connected to XC1
#define         AT91C_TCB_TC1XC1S_TIOA0                ((unsigned int) 0x2 <<  2) // (TCB) TIOA0 connected to XC1
#define         AT91C_TCB_TC1XC1S_TIOA2                ((unsigned int) 0x3 <<  2) // (TCB) TIOA2 connected to XC1
#define AT91C_TCB_TC2XC2S     ((unsigned int) 0x3 <<  4) // (TCB) External Clock Signal 2 Selection
#define         AT91C_TCB_TC2XC2S_TCLK2                ((unsigned int) 0x0 <<  4) // (TCB) TCLK2 connected to XC2
#define         AT91C_TCB_TC2XC2S_NONE                 ((unsigned int) 0x1 <<  4) // (TCB) None signal connected to XC2
#define         AT91C_TCB_TC2XC2S_TIOA0                ((unsigned int) 0x2 <<  4) // (TCB) TIOA0 connected to XC2
#define         AT91C_TCB_TC2XC2S_TIOA1                ((unsigned int) 0x3 <<  4) // (TCB) TIOA2 connected to XC2

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Analog to Digital Convertor
// *****************************************************************************
typedef struct _AT91S_ADC {
   AT91_REG ADC_CR;                     // ADC Control Register
   AT91_REG ADC_MR;                     // ADC Mode Register
   AT91_REG Reserved0[2];               //
   AT91_REG ADC_CHER;                   // ADC Channel Enable Register
   AT91_REG ADC_CHDR;                   // ADC Channel Disable Register
   AT91_REG ADC_CHSR;                   // ADC Channel Status Register
   AT91_REG ADC_SR;                     // ADC Status Register
   AT91_REG ADC_LCDR;                   // ADC Last Converted Data Register
   AT91_REG ADC_IER;                    // ADC Interrupt Enable Register
   AT91_REG ADC_IDR;                    // ADC Interrupt Disable Register
   AT91_REG ADC_IMR;                    // ADC Interrupt Mask Register
   AT91_REG ADC_CDR0;                   // ADC Channel Data Register 0
   AT91_REG ADC_CDR1;                   // ADC Channel Data Register 1
   AT91_REG ADC_CDR2;                   // ADC Channel Data Register 2
   AT91_REG ADC_CDR3;                   // ADC Channel Data Register 3
   AT91_REG ADC_CDR4;                   // ADC Channel Data Register 4
   AT91_REG ADC_CDR5;                   // ADC Channel Data Register 5
   AT91_REG ADC_CDR6;                   // ADC Channel Data Register 6
   AT91_REG ADC_CDR7;                   // ADC Channel Data Register 7
   AT91_REG Reserved1[44];                      //
   AT91_REG ADC_RPR;                    // Receive Pointer Register
   AT91_REG ADC_RCR;                    // Receive Counter Register
   AT91_REG ADC_TPR;                    // Transmit Pointer Register
   AT91_REG ADC_TCR;                    // Transmit Counter Register
   AT91_REG ADC_RNPR;                   // Receive Next Pointer Register
   AT91_REG ADC_RNCR;                   // Receive Next Counter Register
   AT91_REG ADC_TNPR;                   // Transmit Next Pointer Register
   AT91_REG ADC_TNCR;                   // Transmit Next Counter Register
   AT91_REG ADC_PTCR;                   // PDC Transfer Control Register
   AT91_REG ADC_PTSR;                   // PDC Transfer Status Register
} AT91S_ADC, *AT91PS_ADC;

// -------- ADC_CR : (ADC Offset: 0x0) ADC Control Register --------
#define AT91C_ADC_SWRST       ((unsigned int) 0x1 <<  0) // (ADC) Software Reset
#define AT91C_ADC_START       ((unsigned int) 0x1 <<  1) // (ADC) Start Conversion
// -------- ADC_MR : (ADC Offset: 0x4) ADC Mode Register --------
#define AT91C_ADC_TRGEN       ((unsigned int) 0x1 <<  0) // (ADC) Trigger Enable
#define         AT91C_ADC_TRGEN_DIS                  ((unsigned int) 0x0) // (ADC) Hradware triggers are disabled. Starting a conversion is only possible by software
#define         AT91C_ADC_TRGEN_EN                   ((unsigned int) 0x1) // (ADC) Hardware trigger selected by TRGSEL field is enabled.
#define AT91C_ADC_TRGSEL      ((unsigned int) 0x7 <<  1) // (ADC) Trigger Selection
#define         AT91C_ADC_TRGSEL_TIOA0                ((unsigned int) 0x0 <<  1) // (ADC) Selected TRGSEL = TIAO0
#define         AT91C_ADC_TRGSEL_TIOA1                ((unsigned int) 0x1 <<  1) // (ADC) Selected TRGSEL = TIAO1
#define         AT91C_ADC_TRGSEL_TIOA2                ((unsigned int) 0x2 <<  1) // (ADC) Selected TRGSEL = TIAO2
#define         AT91C_ADC_TRGSEL_TIOA3                ((unsigned int) 0x3 <<  1) // (ADC) Selected TRGSEL = TIAO3
#define         AT91C_ADC_TRGSEL_TIOA4                ((unsigned int) 0x4 <<  1) // (ADC) Selected TRGSEL = TIAO4
#define         AT91C_ADC_TRGSEL_TIOA5                ((unsigned int) 0x5 <<  1) // (ADC) Selected TRGSEL = TIAO5
#define         AT91C_ADC_TRGSEL_EXT                  ((unsigned int) 0x6 <<  1) // (ADC) Selected TRGSEL = External Trigger
#define AT91C_ADC_LOWRES      ((unsigned int) 0x1 <<  4) // (ADC) Resolution.
#define         AT91C_ADC_LOWRES_10_BIT               ((unsigned int) 0x0 <<  4) // (ADC) 10-bit resolution
#define         AT91C_ADC_LOWRES_8_BIT                ((unsigned int) 0x1 <<  4) // (ADC) 8-bit resolution
#define AT91C_ADC_SLEEP       ((unsigned int) 0x1 <<  5) // (ADC) Sleep Mode
#define         AT91C_ADC_SLEEP_NORMAL_MODE          ((unsigned int) 0x0 <<  5) // (ADC) Normal Mode
#define         AT91C_ADC_SLEEP_MODE                 ((unsigned int) 0x1 <<  5) // (ADC) Sleep Mode
#define AT91C_ADC_PRESCAL     ((unsigned int) 0x3F <<  8) // (ADC) Prescaler rate selection
#define AT91C_ADC_STARTUP     ((unsigned int) 0x1F << 16) // (ADC) Startup Time
#define AT91C_ADC_SHTIM       ((unsigned int) 0xF << 24) // (ADC) Sample & Hold Time
// --------     ADC_CHER : (ADC Offset: 0x10) ADC Channel Enable Register --------
#define AT91C_ADC_CH0         ((unsigned int) 0x1 <<  0) // (ADC) Channel 0
#define AT91C_ADC_CH1         ((unsigned int) 0x1 <<  1) // (ADC) Channel 1
#define AT91C_ADC_CH2         ((unsigned int) 0x1 <<  2) // (ADC) Channel 2
#define AT91C_ADC_CH3         ((unsigned int) 0x1 <<  3) // (ADC) Channel 3
#define AT91C_ADC_CH4         ((unsigned int) 0x1 <<  4) // (ADC) Channel 4
#define AT91C_ADC_CH5         ((unsigned int) 0x1 <<  5) // (ADC) Channel 5
#define AT91C_ADC_CH6         ((unsigned int) 0x1 <<  6) // (ADC) Channel 6
#define AT91C_ADC_CH7         ((unsigned int) 0x1 <<  7) // (ADC) Channel 7
// --------     ADC_CHDR : (ADC Offset: 0x14) ADC Channel Disable Register --------
// --------     ADC_CHSR : (ADC Offset: 0x18) ADC Channel Status Register --------
// -------- ADC_SR : (ADC Offset: 0x1c) ADC Status Register --------
#define AT91C_ADC_EOC0        ((unsigned int) 0x1 <<  0) // (ADC) End of Conversion
#define AT91C_ADC_EOC1        ((unsigned int) 0x1 <<  1) // (ADC) End of Conversion
#define AT91C_ADC_EOC2        ((unsigned int) 0x1 <<  2) // (ADC) End of Conversion
#define AT91C_ADC_EOC3        ((unsigned int) 0x1 <<  3) // (ADC) End of Conversion
#define AT91C_ADC_EOC4        ((unsigned int) 0x1 <<  4) // (ADC) End of Conversion
#define AT91C_ADC_EOC5        ((unsigned int) 0x1 <<  5) // (ADC) End of Conversion
#define AT91C_ADC_EOC6        ((unsigned int) 0x1 <<  6) // (ADC) End of Conversion
#define AT91C_ADC_EOC7        ((unsigned int) 0x1 <<  7) // (ADC) End of Conversion
#define AT91C_ADC_OVRE0       ((unsigned int) 0x1 <<  8) // (ADC) Overrun Error
#define AT91C_ADC_OVRE1       ((unsigned int) 0x1 <<  9) // (ADC) Overrun Error
#define AT91C_ADC_OVRE2       ((unsigned int) 0x1 << 10) // (ADC) Overrun Error
#define AT91C_ADC_OVRE3       ((unsigned int) 0x1 << 11) // (ADC) Overrun Error
#define AT91C_ADC_OVRE4       ((unsigned int) 0x1 << 12) // (ADC) Overrun Error
#define AT91C_ADC_OVRE5       ((unsigned int) 0x1 << 13) // (ADC) Overrun Error
#define AT91C_ADC_OVRE6       ((unsigned int) 0x1 << 14) // (ADC) Overrun Error
#define AT91C_ADC_OVRE7       ((unsigned int) 0x1 << 15) // (ADC) Overrun Error
#define AT91C_ADC_DRDY        ((unsigned int) 0x1 << 16) // (ADC) Data Ready
#define AT91C_ADC_GOVRE       ((unsigned int) 0x1 << 17) // (ADC) General Overrun
#define AT91C_ADC_ENDRX       ((unsigned int) 0x1 << 18) // (ADC) End of Receiver Transfer
#define AT91C_ADC_RXBUFF      ((unsigned int) 0x1 << 19) // (ADC) RXBUFF Interrupt
// -------- ADC_LCDR : (ADC Offset: 0x20) ADC Last Converted Data Register --------
#define AT91C_ADC_LDATA       ((unsigned int) 0x3FF <<  0) // (ADC) Last Data Converted
// -------- ADC_IER : (ADC Offset: 0x24) ADC Interrupt Enable Register --------
// -------- ADC_IDR : (ADC Offset: 0x28) ADC Interrupt Disable Register --------
// -------- ADC_IMR : (ADC Offset: 0x2c) ADC Interrupt Mask Register --------
// -------- ADC_CDR0 : (ADC Offset: 0x30) ADC Channel Data Register 0 --------
#define AT91C_ADC_DATA        ((unsigned int) 0x3FF <<  0) // (ADC) Converted Data
// -------- ADC_CDR1 : (ADC Offset: 0x34) ADC Channel Data Register 1 --------
// -------- ADC_CDR2 : (ADC Offset: 0x38) ADC Channel Data Register 2 --------
// -------- ADC_CDR3 : (ADC Offset: 0x3c) ADC Channel Data Register 3 --------
// -------- ADC_CDR4 : (ADC Offset: 0x40) ADC Channel Data Register 4 --------
// -------- ADC_CDR5 : (ADC Offset: 0x44) ADC Channel Data Register 5 --------
// -------- ADC_CDR6 : (ADC Offset: 0x48) ADC Channel Data Register 6 --------
// -------- ADC_CDR7 : (ADC Offset: 0x4c) ADC Channel Data Register 7 --------

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR External Bus Interface
// *****************************************************************************
typedef struct _AT91S_EBI {
   AT91_REG EBI_CSA;                    // EBI Chip Select Assignment Register
} AT91S_EBI, *AT91PS_EBI;

// -------- EBI_CSA : (EBI Offset: 0x0) Chip Select Assignment Register --------
#define AT91C_EBI_CS1A        ((unsigned int) 0x1 <<  1) // (EBI) Chip Select 1 Assignment
#define         AT91C_EBI_CS1A_SMC                  ((unsigned int) 0x0 <<  1) // (EBI) Chip Select 1 is assigned to the Static Memory Controller.
#define         AT91C_EBI_CS1A_SDRAMC               ((unsigned int) 0x1 <<  1) // (EBI) Chip Select 1 is assigned to the SDRAM Controller.
#define AT91C_EBI_CS2A        ((unsigned int) 0x1 <<  2) // (EBI) Chip Select 2 Assignment
#define         AT91C_EBI_CS2A_SMC                  ((unsigned int) 0x0 <<  2) // (EBI) Chip Select 2 is assigned to the Static Memory Controller and NCS2, NCS5 and NCS6 behave as defined by the SMC.
#define         AT91C_EBI_CS2A_SMC_CompactFlash     ((unsigned int) 0x1 <<  2) // (EBI) Chip Select 2 is assigned to the Static Memory Controller and the CompactFlash Logic (second slot) is actived. Accesing the address space reserved to NCS5 and NCS6 may lead to an unpedictable outcome
#define AT91C_EBI_CS3A        ((unsigned int) 0x1 <<  3) // (EBI) Chip Select 3 Assignment
#define         AT91C_EBI_CS3A_SMC                  ((unsigned int) 0x0 <<  3) // (EBI) Chip Select 3 is only assigned to the Static Memory Controller and NCS3 behaves as defined by the SMC2.
#define         AT91C_EBI_CS3A_SMC_NandFlash        ((unsigned int) 0x1 <<  3) // (EBI) Chip Select 3 is assigned to the Static Memory Controller and the NAND Flash Logic is activated.
#define AT91C_EBI_CS4A        ((unsigned int) 0x1 <<  4) // (EBI) Chip Select 4 Assignment
#define         AT91C_EBI_CS4A_SMC                  ((unsigned int) 0x0 <<  4) // (EBI) Chip Select 4 is assigned to the Static Memory Controller and NCS2, NCS5 and NCS6 behave as defined by the SMC.
#define         AT91C_EBI_CS4A_SMC_CompactFlash     ((unsigned int) 0x1 <<  4) // (EBI) Chip Select 4 is assigned to the Static Memory Controller and the CompactFlash Logic (first slot) is actived. Accesing the address space reserved to NCS5 and NCS6 may lead to an unpedictable outcome
#define AT91C_EBI_NWPC        ((unsigned int) 0x1 << 16) // (EBI) NWait Pin Configuration
#define         AT91C_EBI_NWPC_OFF                  ((unsigned int) 0x0 << 16) // (EBI) The NWAIT device pin is not connected to the External Wait Request input of the Static Memory Controller ,this multiplexe pin can be used as a PIO.
#define         AT91C_EBI_NWPC_ON                   ((unsigned int) 0x1 << 16) // (EBI) The NWAIT device pin is connected to the External Wait Request input of the Static Memory Controller.

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Static Memory Controller 2 Interface
// *****************************************************************************
typedef struct _AT91S_SMC2 {
   AT91_REG SMC2_CSR[8];                // SMC2 Chip Select Register
} AT91S_SMC2, *AT91PS_SMC2;

// -------- SMC2_CSR : (SMC2 Offset: 0x0) SMC2 Chip Select Register --------
#define AT91C_SMC2_NWS        ((unsigned int) 0x7F <<  0) // (SMC2) Number of Wait States
#define AT91C_SMC2_WSEN       ((unsigned int) 0x1 <<  7) // (SMC2) Wait State Enable
#define AT91C_SMC2_TDF        ((unsigned int) 0xF <<  8) // (SMC2) Data Float Time
#define AT91C_SMC2_BAT        ((unsigned int) 0x1 << 12) // (SMC2) Byte Access Type
#define AT91C_SMC2_DBW        ((unsigned int) 0x1 << 13) // (SMC2) Data Bus Width
#define         AT91C_SMC2_DBW_16                   ((unsigned int) 0x1 << 13) // (SMC2) 16-bit.
#define         AT91C_SMC2_DBW_8                    ((unsigned int) 0x2 << 13) // (SMC2) 8-bit.
#define AT91C_SMC2_DRP        ((unsigned int) 0x1 << 15) // (SMC2) Data Read Protocol
#define AT91C_SMC2_ACSS       ((unsigned int) 0x3 << 16) // (SMC2) Address to Chip Select Setup
#define         AT91C_SMC2_ACSS_STANDARD             ((unsigned int) 0x0 << 16) // (SMC2) Standard, asserted at the beginning of the access and deasserted at the end.
#define         AT91C_SMC2_ACSS_1_CYCLE              ((unsigned int) 0x1 << 16) // (SMC2) One cycle less at the beginning and the end of the access.
#define         AT91C_SMC2_ACSS_2_CYCLES             ((unsigned int) 0x2 << 16) // (SMC2) Two cycles less at the beginning and the end of the access.
#define         AT91C_SMC2_ACSS_3_CYCLES             ((unsigned int) 0x3 << 16) // (SMC2) Three cycles less at the beginning and the end of the access.
#define AT91C_SMC2_RWSETUP    ((unsigned int) 0x7 << 24) // (SMC2) Read and Write Signal Setup Time
#define AT91C_SMC2_RWHOLD     ((unsigned int) 0x7 << 28) // (SMC2) Read and Write Signal Hold Time

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR SDRAM Controller Interface
// *****************************************************************************
typedef struct _AT91S_SDRC {
   AT91_REG SDRC_MR;                    // SDRAM Controller Mode Register
   AT91_REG SDRC_TR;                    // SDRAM Controller Refresh Timer Register
   AT91_REG SDRC_CR;                    // SDRAM Controller Configuration Register
   AT91_REG SDRC_SRR;                   // SDRAM Controller Self Refresh Register
   AT91_REG SDRC_LPR;                   // SDRAM Controller Low Power Register
   AT91_REG SDRC_IER;                   // SDRAM Controller Interrupt Enable Register
   AT91_REG SDRC_IDR;                   // SDRAM Controller Interrupt Disable Register
   AT91_REG SDRC_IMR;                   // SDRAM Controller Interrupt Mask Register
   AT91_REG SDRC_ISR;                   // SDRAM Controller Interrupt Mask Register
   AT91_REG SDRC_VER;                   // SDRAM Controller Version Register
} AT91S_SDRC, *AT91PS_SDRC;

// -------- SDRC_MR : (SDRC Offset: 0x0) SDRAM Controller Mode Register --------
#define AT91C_SDRC_MODE       ((unsigned int) 0xF <<  0) // (SDRC) Mode
#define         AT91C_SDRC_MODE_NORMAL_CMD           ((unsigned int) 0x0) // (SDRC) Normal Mode
#define         AT91C_SDRC_MODE_NOP_CMD              ((unsigned int) 0x1) // (SDRC) NOP Command
#define         AT91C_SDRC_MODE_PRCGALL_CMD          ((unsigned int) 0x2) // (SDRC) All Banks Precharge Command
#define         AT91C_SDRC_MODE_LMR_CMD              ((unsigned int) 0x3) // (SDRC) Load Mode Register Command
#define         AT91C_SDRC_MODE_RFSH_CMD             ((unsigned int) 0x4) // (SDRC) Refresh Command
#define AT91C_SDRC_DBW        ((unsigned int) 0x1 <<  4) // (SDRC) Data Bus Width
#define         AT91C_SDRC_DBW_32_BITS              ((unsigned int) 0x0 <<  4) // (SDRC) 32 Bits datas bus
#define         AT91C_SDRC_DBW_16_BITS              ((unsigned int) 0x1 <<  4) // (SDRC) 16 Bits datas bus
// -------- SDRC_TR : (SDRC Offset: 0x4) SDRC Refresh Timer Register --------
#define AT91C_SDRC_COUNT      ((unsigned int) 0xFFF <<  0) // (SDRC) Refresh Counter
// -------- SDRC_CR : (SDRC Offset: 0x8) SDRAM Configuration Register --------
#define AT91C_SDRC_NC         ((unsigned int) 0x3 <<  0) // (SDRC) Number of Column Bits
#define         AT91C_SDRC_NC_8                    ((unsigned int) 0x0) // (SDRC) 8 Bits
#define         AT91C_SDRC_NC_9                    ((unsigned int) 0x1) // (SDRC) 9 Bits
#define         AT91C_SDRC_NC_10                   ((unsigned int) 0x2) // (SDRC) 10 Bits
#define         AT91C_SDRC_NC_11                   ((unsigned int) 0x3) // (SDRC) 11 Bits
#define AT91C_SDRC_NR         ((unsigned int) 0x3 <<  2) // (SDRC) Number of Row Bits
#define         AT91C_SDRC_NR_11                   ((unsigned int) 0x0 <<  2) // (SDRC) 11 Bits
#define         AT91C_SDRC_NR_12                   ((unsigned int) 0x1 <<  2) // (SDRC) 12 Bits
#define         AT91C_SDRC_NR_13                   ((unsigned int) 0x2 <<  2) // (SDRC) 13 Bits
#define AT91C_SDRC_NB         ((unsigned int) 0x1 <<  4) // (SDRC) Number of Banks
#define         AT91C_SDRC_NB_2_BANKS              ((unsigned int) 0x0 <<  4) // (SDRC) 2 banks
#define         AT91C_SDRC_NB_4_BANKS              ((unsigned int) 0x1 <<  4) // (SDRC) 4 banks
#define AT91C_SDRC_CAS        ((unsigned int) 0x3 <<  5) // (SDRC) CAS Latency
#define         AT91C_SDRC_CAS_2                    ((unsigned int) 0x2 <<  5) // (SDRC) 2 cycles
#define AT91C_SDRC_TWR        ((unsigned int) 0xF <<  7) // (SDRC) Number of Write Recovery Time Cycles
#define AT91C_SDRC_TRC        ((unsigned int) 0xF << 11) // (SDRC) Number of RAS Cycle Time Cycles
#define AT91C_SDRC_TRP        ((unsigned int) 0xF << 15) // (SDRC) Number of RAS Precharge Time Cycles
#define AT91C_SDRC_TRCD       ((unsigned int) 0xF << 19) // (SDRC) Number of RAS to CAS Delay Cycles
#define AT91C_SDRC_TRAS       ((unsigned int) 0xF << 23) // (SDRC) Number of RAS Active Time Cycles
#define AT91C_SDRC_TXSR       ((unsigned int) 0xF << 27) // (SDRC) Number of Command Recovery Time Cycles
// -------- SDRC_SRR : (SDRC Offset: 0xc) SDRAM Controller Self-refresh Register --------
#define AT91C_SDRC_SRCB       ((unsigned int) 0x1 <<  0) // (SDRC) Self-refresh Command Bit
// -------- SDRC_LPR : (SDRC Offset: 0x10) SDRAM Controller Low-power Register --------
#define AT91C_SDRC_LPCB       ((unsigned int) 0x1 <<  0) // (SDRC) Low-power Command Bit
// -------- SDRC_IER : (SDRC Offset: 0x14) SDRAM Controller Interrupt Enable Register --------
#define AT91C_SDRC_RES        ((unsigned int) 0x1 <<  0) // (SDRC) Refresh Error Status
// -------- SDRC_IDR : (SDRC Offset: 0x18) SDRAM Controller Interrupt Disable Register --------
// -------- SDRC_IMR : (SDRC Offset: 0x1c) SDRAM Controller Interrupt Mask Register --------
// -------- SDRC_ISR : (SDRC Offset: 0x20) SDRAM Controller Interrupt Status Register --------
// -------- IPB_VER : (SDRC Offset: 0x24) SDRAM Controller IP version Register --------
#define AT91C_SDRC_VERSION    ((unsigned int) 0xFFF <<  0) // (SDRC) IP version of the macrocell
#define AT91C_SDRC_MFN        ((unsigned int) 0x7 <<  1) // (SDRC)

// *****************************************************************************
//              SOFTWARE API DEFINITION  FOR Error Correction Code controller
// *****************************************************************************
typedef struct _AT91S_ECC {
   AT91_REG ECC_CR;                     //  ECC reset register
   AT91_REG ECC_MR;                     //  ECC Page size register
   AT91_REG ECC_SR;                     //  ECC Status register
   AT91_REG ECC_PR;                     //  ECC Parity register
   AT91_REG ECC_NPR;                    //  ECC Parity N register
   AT91_REG ECC_VR;                     //  ECC Version register
} AT91S_ECC, *AT91PS_ECC;

// -------- ECC_CR : (ECC Offset: 0x0) ECC reset register --------
#define AT91C_ECC_RST         ((unsigned int) 0x1 <<  0) // (ECC) ECC reset parity
// -------- ECC_MR : (ECC Offset: 0x4) ECC page size register --------
#define AT91C_ECC_PAGE_SIZE   ((unsigned int) 0x3 <<  0) // (ECC) Nand Flash page size
// -------- ECC_SR : (ECC Offset: 0x8) ECC status register --------
#define AT91C_ECC_RECERR      ((unsigned int) 0x1 <<  0) // (ECC) ECC error
#define AT91C_ECC_ECCERR      ((unsigned int) 0x1 <<  1) // (ECC) ECC single error
#define AT91C_ECC_MULERR      ((unsigned int) 0x1 <<  2) // (ECC) ECC_MULERR
// -------- ECC_PR : (ECC Offset: 0xc) ECC parity register --------
#define AT91C_ECC_BITADDR     ((unsigned int) 0xF <<  0) // (ECC) Bit address error
#define AT91C_ECC_WORDADDR    ((unsigned int) 0xFFF <<  4) // (ECC) address of the failing bit
// -------- ECC_NPR : (ECC Offset: 0x10) ECC N parity register --------
#define AT91C_ECC_NPARITY     ((unsigned int) 0xFFFF <<  0) // (ECC) ECC parity N
// -------- ECC_VR : (ECC Offset: 0x14) ECC version register --------
#define AT91C_ECC_VERSION     ((unsigned int) 0xFFF <<  0) // (ECC) ECC version number
#define AT91C_ECC_MFN         ((unsigned int) 0x7 << 16) // (ECC) ECC MFN

// *****************************************************************************
//               REGISTER ADDRESS DEFINITION FOR AT91SAM7SE512
// *****************************************************************************
// ========== Register definition for SYS peripheral ==========
// ========== Register definition for AIC peripheral ==========
#define AT91C_AIC_ICCR  ((AT91_REG *)   0xFFFFF128) // (AIC) Interrupt Clear Command Register
#define AT91C_AIC_IECR  ((AT91_REG *)   0xFFFFF120) // (AIC) Interrupt Enable Command Register
#define AT91C_AIC_SMR   ((AT91_REG *)   0xFFFFF000) // (AIC) Source Mode Register
#define AT91C_AIC_ISCR  ((AT91_REG *)   0xFFFFF12C) // (AIC) Interrupt Set Command Register
#define AT91C_AIC_EOICR ((AT91_REG *)   0xFFFFF130) // (AIC) End of Interrupt Command Register
#define AT91C_AIC_DCR   ((AT91_REG *)   0xFFFFF138) // (AIC) Debug Control Register (Protect)
#define AT91C_AIC_FFER  ((AT91_REG *)   0xFFFFF140) // (AIC) Fast Forcing Enable Register
#define AT91C_AIC_SVR   ((AT91_REG *)   0xFFFFF080) // (AIC) Source Vector Register
#define AT91C_AIC_SPU   ((AT91_REG *)   0xFFFFF134) // (AIC) Spurious Vector Register
#define AT91C_AIC_FFDR  ((AT91_REG *)   0xFFFFF144) // (AIC) Fast Forcing Disable Register
#define AT91C_AIC_FVR   ((AT91_REG *)   0xFFFFF104) // (AIC) FIQ Vector Register
#define AT91C_AIC_FFSR  ((AT91_REG *)   0xFFFFF148) // (AIC) Fast Forcing Status Register
#define AT91C_AIC_IMR   ((AT91_REG *)   0xFFFFF110) // (AIC) Interrupt Mask Register
#define AT91C_AIC_ISR   ((AT91_REG *)   0xFFFFF108) // (AIC) Interrupt Status Register
#define AT91C_AIC_IVR   ((AT91_REG *)   0xFFFFF100) // (AIC) IRQ Vector Register
#define AT91C_AIC_IDCR  ((AT91_REG *)   0xFFFFF124) // (AIC) Interrupt Disable Command Register
#define AT91C_AIC_CISR  ((AT91_REG *)   0xFFFFF114) // (AIC) Core Interrupt Status Register
#define AT91C_AIC_IPR   ((AT91_REG *)   0xFFFFF10C) // (AIC) Interrupt Pending Register
// ========== Register definition for PDC_DBGU peripheral ==========
#define AT91C_DBGU_TNCR ((AT91_REG *)   0xFFFFF31C) // (PDC_DBGU) Transmit Next Counter Register
#define AT91C_DBGU_RNCR ((AT91_REG *)   0xFFFFF314) // (PDC_DBGU) Receive Next Counter Register
#define AT91C_DBGU_PTCR ((AT91_REG *)   0xFFFFF320) // (PDC_DBGU) PDC Transfer Control Register
#define AT91C_DBGU_PTSR ((AT91_REG *)   0xFFFFF324) // (PDC_DBGU) PDC Transfer Status Register
#define AT91C_DBGU_RCR  ((AT91_REG *)   0xFFFFF304) // (PDC_DBGU) Receive Counter Register
#define AT91C_DBGU_TCR  ((AT91_REG *)   0xFFFFF30C) // (PDC_DBGU) Transmit Counter Register
#define AT91C_DBGU_RPR  ((AT91_REG *)   0xFFFFF300) // (PDC_DBGU) Receive Pointer Register
#define AT91C_DBGU_TPR  ((AT91_REG *)   0xFFFFF308) // (PDC_DBGU) Transmit Pointer Register
#define AT91C_DBGU_RNPR ((AT91_REG *)   0xFFFFF310) // (PDC_DBGU) Receive Next Pointer Register
#define AT91C_DBGU_TNPR ((AT91_REG *)   0xFFFFF318) // (PDC_DBGU) Transmit Next Pointer Register
// ========== Register definition for DBGU peripheral ==========
#define AT91C_DBGU_EXID ((AT91_REG *)   0xFFFFF244) // (DBGU) Chip ID Extension Register
#define AT91C_DBGU_THR  ((AT91_REG *)   0xFFFFF21C) // (DBGU) Transmitter Holding Register
#define AT91C_DBGU_CSR  ((AT91_REG *)   0xFFFFF214) // (DBGU) Channel Status Register
#define AT91C_DBGU_IDR  ((AT91_REG *)   0xFFFFF20C) // (DBGU) Interrupt Disable Register
#define AT91C_DBGU_MR   ((AT91_REG *)   0xFFFFF204) // (DBGU) Mode Register
#define AT91C_DBGU_FNTR ((AT91_REG *)   0xFFFFF248) // (DBGU) Force NTRST Register
#define AT91C_DBGU_CIDR ((AT91_REG *)   0xFFFFF240) // (DBGU) Chip ID Register
#define AT91C_DBGU_BRGR ((AT91_REG *)   0xFFFFF220) // (DBGU) Baud Rate Generator Register
#define AT91C_DBGU_RHR  ((AT91_REG *)   0xFFFFF218) // (DBGU) Receiver Holding Register
#define AT91C_DBGU_IMR  ((AT91_REG *)   0xFFFFF210) // (DBGU) Interrupt Mask Register
#define AT91C_DBGU_IER  ((AT91_REG *)   0xFFFFF208) // (DBGU) Interrupt Enable Register
#define AT91C_DBGU_CR   ((AT91_REG *)   0xFFFFF200) // (DBGU) Control Register
// ========== Register definition for PIOA peripheral ==========
#define AT91C_PIOA_IMR  ((AT91_REG *)   0xFFFFF448) // (PIOA) Interrupt Mask Register
#define AT91C_PIOA_IER  ((AT91_REG *)   0xFFFFF440) // (PIOA) Interrupt Enable Register
#define AT91C_PIOA_OWDR ((AT91_REG *)   0xFFFFF4A4) // (PIOA) Output Write Disable Register
#define AT91C_PIOA_ISR  ((AT91_REG *)   0xFFFFF44C) // (PIOA) Interrupt Status Register
#define AT91C_PIOA_PPUDR ((AT91_REG *)  0xFFFFF460) // (PIOA) Pull-up Disable Register
#define AT91C_PIOA_MDSR ((AT91_REG *)   0xFFFFF458) // (PIOA) Multi-driver Status Register
#define AT91C_PIOA_MDER ((AT91_REG *)   0xFFFFF450) // (PIOA) Multi-driver Enable Register
#define AT91C_PIOA_PER  ((AT91_REG *)   0xFFFFF400) // (PIOA) PIO Enable Register
#define AT91C_PIOA_PSR  ((AT91_REG *)   0xFFFFF408) // (PIOA) PIO Status Register
#define AT91C_PIOA_OER  ((AT91_REG *)   0xFFFFF410) // (PIOA) Output Enable Register
#define AT91C_PIOA_BSR  ((AT91_REG *)   0xFFFFF474) // (PIOA) Select B Register
#define AT91C_PIOA_PPUER ((AT91_REG *)  0xFFFFF464) // (PIOA) Pull-up Enable Register
#define AT91C_PIOA_MDDR ((AT91_REG *)   0xFFFFF454) // (PIOA) Multi-driver Disable Register
#define AT91C_PIOA_PDR  ((AT91_REG *)   0xFFFFF404) // (PIOA) PIO Disable Register
#define AT91C_PIOA_ODR  ((AT91_REG *)   0xFFFFF414) // (PIOA) Output Disable Registerr
#define AT91C_PIOA_IFDR ((AT91_REG *)   0xFFFFF424) // (PIOA) Input Filter Disable Register
#define AT91C_PIOA_ABSR ((AT91_REG *)   0xFFFFF478) // (PIOA) AB Select Status Register
#define AT91C_PIOA_ASR  ((AT91_REG *)   0xFFFFF470) // (PIOA) Select A Register
#define AT91C_PIOA_PPUSR ((AT91_REG *)  0xFFFFF468) // (PIOA) Pull-up Status Register
#define AT91C_PIOA_ODSR ((AT91_REG *)   0xFFFFF438) // (PIOA) Output Data Status Register
#define AT91C_PIOA_SODR ((AT91_REG *)   0xFFFFF430) // (PIOA) Set Output Data Register
#define AT91C_PIOA_IFSR ((AT91_REG *)   0xFFFFF428) // (PIOA) Input Filter Status Register
#define AT91C_PIOA_IFER ((AT91_REG *)   0xFFFFF420) // (PIOA) Input Filter Enable Register
#define AT91C_PIOA_OSR  ((AT91_REG *)   0xFFFFF418) // (PIOA) Output Status Register
#define AT91C_PIOA_IDR  ((AT91_REG *)   0xFFFFF444) // (PIOA) Interrupt Disable Register
#define AT91C_PIOA_PDSR ((AT91_REG *)   0xFFFFF43C) // (PIOA) Pin Data Status Register
#define AT91C_PIOA_CODR ((AT91_REG *)   0xFFFFF434) // (PIOA) Clear Output Data Register
#define AT91C_PIOA_OWSR ((AT91_REG *)   0xFFFFF4A8) // (PIOA) Output Write Status Register
#define AT91C_PIOA_OWER ((AT91_REG *)   0xFFFFF4A0) // (PIOA) Output Write Enable Register
// ========== Register definition for PIOB peripheral ==========
#define AT91C_PIOB_OWSR ((AT91_REG *)   0xFFFFF6A8) // (PIOB) Output Write Status Register
#define AT91C_PIOB_PPUSR ((AT91_REG *)  0xFFFFF668) // (PIOB) Pull-up Status Register
#define AT91C_PIOB_PPUDR ((AT91_REG *)  0xFFFFF660) // (PIOB) Pull-up Disable Register
#define AT91C_PIOB_MDSR ((AT91_REG *)   0xFFFFF658) // (PIOB) Multi-driver Status Register
#define AT91C_PIOB_MDER ((AT91_REG *)   0xFFFFF650) // (PIOB) Multi-driver Enable Register
#define AT91C_PIOB_IMR  ((AT91_REG *)   0xFFFFF648) // (PIOB) Interrupt Mask Register
#define AT91C_PIOB_OSR  ((AT91_REG *)   0xFFFFF618) // (PIOB) Output Status Register
#define AT91C_PIOB_OER  ((AT91_REG *)   0xFFFFF610) // (PIOB) Output Enable Register
#define AT91C_PIOB_PSR  ((AT91_REG *)   0xFFFFF608) // (PIOB) PIO Status Register
#define AT91C_PIOB_PER  ((AT91_REG *)   0xFFFFF600) // (PIOB) PIO Enable Register
#define AT91C_PIOB_BSR  ((AT91_REG *)   0xFFFFF674) // (PIOB) Select B Register
#define AT91C_PIOB_PPUER ((AT91_REG *)  0xFFFFF664) // (PIOB) Pull-up Enable Register
#define AT91C_PIOB_IFDR ((AT91_REG *)   0xFFFFF624) // (PIOB) Input Filter Disable Register
#define AT91C_PIOB_ODR  ((AT91_REG *)   0xFFFFF614) // (PIOB) Output Disable Registerr
#define AT91C_PIOB_ABSR ((AT91_REG *)   0xFFFFF678) // (PIOB) AB Select Status Register
#define AT91C_PIOB_ASR  ((AT91_REG *)   0xFFFFF670) // (PIOB) Select A Register
#define AT91C_PIOB_IFER ((AT91_REG *)   0xFFFFF620) // (PIOB) Input Filter Enable Register
#define AT91C_PIOB_IFSR ((AT91_REG *)   0xFFFFF628) // (PIOB) Input Filter Status Register
#define AT91C_PIOB_SODR ((AT91_REG *)   0xFFFFF630) // (PIOB) Set Output Data Register
#define AT91C_PIOB_ODSR ((AT91_REG *)   0xFFFFF638) // (PIOB) Output Data Status Register
#define AT91C_PIOB_CODR ((AT91_REG *)   0xFFFFF634) // (PIOB) Clear Output Data Register
#define AT91C_PIOB_PDSR ((AT91_REG *)   0xFFFFF63C) // (PIOB) Pin Data Status Register
#define AT91C_PIOB_OWER ((AT91_REG *)   0xFFFFF6A0) // (PIOB) Output Write Enable Register
#define AT91C_PIOB_IER  ((AT91_REG *)   0xFFFFF640) // (PIOB) Interrupt Enable Register
#define AT91C_PIOB_OWDR ((AT91_REG *)   0xFFFFF6A4) // (PIOB) Output Write Disable Register
#define AT91C_PIOB_MDDR ((AT91_REG *)   0xFFFFF654) // (PIOB) Multi-driver Disable Register
#define AT91C_PIOB_ISR  ((AT91_REG *)   0xFFFFF64C) // (PIOB) Interrupt Status Register
#define AT91C_PIOB_IDR  ((AT91_REG *)   0xFFFFF644) // (PIOB) Interrupt Disable Register
#define AT91C_PIOB_PDR  ((AT91_REG *)   0xFFFFF604) // (PIOB) PIO Disable Register
// ========== Register definition for PIOC peripheral ==========
#define AT91C_PIOC_IFDR ((AT91_REG *)   0xFFFFF824) // (PIOC) Input Filter Disable Register
#define AT91C_PIOC_ODR  ((AT91_REG *)   0xFFFFF814) // (PIOC) Output Disable Registerr
#define AT91C_PIOC_ABSR ((AT91_REG *)   0xFFFFF878) // (PIOC) AB Select Status Register
#define AT91C_PIOC_SODR ((AT91_REG *)   0xFFFFF830) // (PIOC) Set Output Data Register
#define AT91C_PIOC_IFSR ((AT91_REG *)   0xFFFFF828) // (PIOC) Input Filter Status Register
#define AT91C_PIOC_CODR ((AT91_REG *)   0xFFFFF834) // (PIOC) Clear Output Data Register
#define AT91C_PIOC_ODSR ((AT91_REG *)   0xFFFFF838) // (PIOC) Output Data Status Register
#define AT91C_PIOC_IER  ((AT91_REG *)   0xFFFFF840) // (PIOC) Interrupt Enable Register
#define AT91C_PIOC_IMR  ((AT91_REG *)   0xFFFFF848) // (PIOC) Interrupt Mask Register
#define AT91C_PIOC_OWDR ((AT91_REG *)   0xFFFFF8A4) // (PIOC) Output Write Disable Register
#define AT91C_PIOC_MDDR ((AT91_REG *)   0xFFFFF854) // (PIOC) Multi-driver Disable Register
#define AT91C_PIOC_PDSR ((AT91_REG *)   0xFFFFF83C) // (PIOC) Pin Data Status Register
#define AT91C_PIOC_IDR  ((AT91_REG *)   0xFFFFF844) // (PIOC) Interrupt Disable Register
#define AT91C_PIOC_ISR  ((AT91_REG *)   0xFFFFF84C) // (PIOC) Interrupt Status Register
#define AT91C_PIOC_PDR  ((AT91_REG *)   0xFFFFF804) // (PIOC) PIO Disable Register
#define AT91C_PIOC_OWSR ((AT91_REG *)   0xFFFFF8A8) // (PIOC) Output Write Status Register
#define AT91C_PIOC_OWER ((AT91_REG *)   0xFFFFF8A0) // (PIOC) Output Write Enable Register
#define AT91C_PIOC_ASR  ((AT91_REG *)   0xFFFFF870) // (PIOC) Select A Register
#define AT91C_PIOC_PPUSR ((AT91_REG *)  0xFFFFF868) // (PIOC) Pull-up Status Register
#define AT91C_PIOC_PPUDR ((AT91_REG *)  0xFFFFF860) // (PIOC) Pull-up Disable Register
#define AT91C_PIOC_MDSR ((AT91_REG *)   0xFFFFF858) // (PIOC) Multi-driver Status Register
#define AT91C_PIOC_MDER ((AT91_REG *)   0xFFFFF850) // (PIOC) Multi-driver Enable Register
#define AT91C_PIOC_IFER ((AT91_REG *)   0xFFFFF820) // (PIOC) Input Filter Enable Register
#define AT91C_PIOC_OSR  ((AT91_REG *)   0xFFFFF818) // (PIOC) Output Status Register
#define AT91C_PIOC_OER  ((AT91_REG *)   0xFFFFF810) // (PIOC) Output Enable Register
#define AT91C_PIOC_PSR  ((AT91_REG *)   0xFFFFF808) // (PIOC) PIO Status Register
#define AT91C_PIOC_PER  ((AT91_REG *)   0xFFFFF800) // (PIOC) PIO Enable Register
#define AT91C_PIOC_BSR  ((AT91_REG *)   0xFFFFF874) // (PIOC) Select B Register
#define AT91C_PIOC_PPUER ((AT91_REG *)  0xFFFFF864) // (PIOC) Pull-up Enable Register
// ========== Register definition for CKGR peripheral ==========
#define AT91C_CKGR_PLLR ((AT91_REG *)   0xFFFFFC2C) // (CKGR) PLL Register
#define AT91C_CKGR_MCFR ((AT91_REG *)   0xFFFFFC24) // (CKGR) Main Clock  Frequency Register
#define AT91C_CKGR_MOR  ((AT91_REG *)   0xFFFFFC20) // (CKGR) Main Oscillator Register
// ========== Register definition for PMC peripheral ==========
#define AT91C_PMC_SCSR  ((AT91_REG *)   0xFFFFFC08) // (PMC) System Clock Status Register
#define AT91C_PMC_SCER  ((AT91_REG *)   0xFFFFFC00) // (PMC) System Clock Enable Register
#define AT91C_PMC_IMR   ((AT91_REG *)   0xFFFFFC6C) // (PMC) Interrupt Mask Register
#define AT91C_PMC_IDR   ((AT91_REG *)   0xFFFFFC64) // (PMC) Interrupt Disable Register
#define AT91C_PMC_PCDR  ((AT91_REG *)   0xFFFFFC14) // (PMC) Peripheral Clock Disable Register
#define AT91C_PMC_SCDR  ((AT91_REG *)   0xFFFFFC04) // (PMC) System Clock Disable Register
#define AT91C_PMC_SR    ((AT91_REG *)   0xFFFFFC68) // (PMC) Status Register
#define AT91C_PMC_IER   ((AT91_REG *)   0xFFFFFC60) // (PMC) Interrupt Enable Register
#define AT91C_PMC_MCKR  ((AT91_REG *)   0xFFFFFC30) // (PMC) Master Clock Register
#define AT91C_PMC_MOR   ((AT91_REG *)   0xFFFFFC20) // (PMC) Main Oscillator Register
#define AT91C_PMC_PCER  ((AT91_REG *)   0xFFFFFC10) // (PMC) Peripheral Clock Enable Register
#define AT91C_PMC_PCSR  ((AT91_REG *)   0xFFFFFC18) // (PMC) Peripheral Clock Status Register
#define AT91C_PMC_PLLR  ((AT91_REG *)   0xFFFFFC2C) // (PMC) PLL Register
#define AT91C_PMC_MCFR  ((AT91_REG *)   0xFFFFFC24) // (PMC) Main Clock  Frequency Register
#define AT91C_PMC_PCKR  ((AT91_REG *)   0xFFFFFC40) // (PMC) Programmable Clock Register
// ========== Register definition for RSTC peripheral ==========
#define AT91C_RSTC_RSR  ((AT91_REG *)   0xFFFFFD04) // (RSTC) Reset Status Register
#define AT91C_RSTC_RMR  ((AT91_REG *)   0xFFFFFD08) // (RSTC) Reset Mode Register
#define AT91C_RSTC_RCR  ((AT91_REG *)   0xFFFFFD00) // (RSTC) Reset Control Register
// ========== Register definition for RTTC peripheral ==========
#define AT91C_RTTC_RTSR ((AT91_REG *)   0xFFFFFD2C) // (RTTC) Real-time Status Register
#define AT91C_RTTC_RTAR ((AT91_REG *)   0xFFFFFD24) // (RTTC) Real-time Alarm Register
#define AT91C_RTTC_RTVR ((AT91_REG *)   0xFFFFFD28) // (RTTC) Real-time Value Register
#define AT91C_RTTC_RTMR ((AT91_REG *)   0xFFFFFD20) // (RTTC) Real-time Mode Register
// ========== Register definition for PITC peripheral ==========
#define AT91C_PITC_PIIR ((AT91_REG *)   0xFFFFFD3C) // (PITC) Period Interval Image Register
#define AT91C_PITC_PISR ((AT91_REG *)   0xFFFFFD34) // (PITC) Period Interval Status Register
#define AT91C_PITC_PIVR ((AT91_REG *)   0xFFFFFD38) // (PITC) Period Interval Value Register
#define AT91C_PITC_PIMR ((AT91_REG *)   0xFFFFFD30) // (PITC) Period Interval Mode Register
// ========== Register definition for WDTC peripheral ==========
#define AT91C_WDTC_WDMR ((AT91_REG *)   0xFFFFFD44) // (WDTC) Watchdog Mode Register
#define AT91C_WDTC_WDSR ((AT91_REG *)   0xFFFFFD48) // (WDTC) Watchdog Status Register
#define AT91C_WDTC_WDCR ((AT91_REG *)   0xFFFFFD40) // (WDTC) Watchdog Control Register
// ========== Register definition for VREG peripheral ==========
#define AT91C_VREG_MR   ((AT91_REG *)   0xFFFFFD60) // (VREG) Voltage Regulator Mode Register
// ========== Register definition for MC peripheral ==========
#define AT91C_MC_PUER   ((AT91_REG *)   0xFFFFFF54) // (MC) MC Protection Unit Enable Register
#define AT91C_MC_ASR    ((AT91_REG *)   0xFFFFFF04) // (MC) MC Abort Status Register
#define AT91C_MC_PUP    ((AT91_REG *)   0xFFFFFF50) // (MC) MC Protection Unit Peripherals
#define AT91C_MC_PUIA   ((AT91_REG *)   0xFFFFFF10) // (MC) MC Protection Unit Area
#define AT91C_MC_AASR   ((AT91_REG *)   0xFFFFFF08) // (MC) MC Abort Address Status Register
#define AT91C_MC_RCR    ((AT91_REG *)   0xFFFFFF00) // (MC) MC Remap Control Register
// ========== Register definition for EFC0 peripheral ==========
#define AT91C_EFC0_VR   ((AT91_REG *)   0xFFFFFF6C) // (EFC0) MC Flash Version Register
#define AT91C_EFC0_FCR  ((AT91_REG *)   0xFFFFFF64) // (EFC0) MC Flash Command Register
#define AT91C_EFC0_FSR  ((AT91_REG *)   0xFFFFFF68) // (EFC0) MC Flash Status Register
#define AT91C_EFC0_FMR  ((AT91_REG *)   0xFFFFFF60) // (EFC0) MC Flash Mode Register
// ========== Register definition for EFC1 peripheral ==========
#define AT91C_EFC1_VR   ((AT91_REG *)   0xFFFFFF7C) // (EFC1) MC Flash Version Register
#define AT91C_EFC1_FCR  ((AT91_REG *)   0xFFFFFF74) // (EFC1) MC Flash Command Register
#define AT91C_EFC1_FSR  ((AT91_REG *)   0xFFFFFF78) // (EFC1) MC Flash Status Register
#define AT91C_EFC1_FMR  ((AT91_REG *)   0xFFFFFF70) // (EFC1) MC Flash Mode Register
// ========== Register definition for PDC_SPI peripheral ==========
#define AT91C_SPI_PTCR  ((AT91_REG *)   0xFFFE0120) // (PDC_SPI) PDC Transfer Control Register
#define AT91C_SPI_TNPR  ((AT91_REG *)   0xFFFE0118) // (PDC_SPI) Transmit Next Pointer Register
#define AT91C_SPI_RNPR  ((AT91_REG *)   0xFFFE0110) // (PDC_SPI) Receive Next Pointer Register
#define AT91C_SPI_TPR   ((AT91_REG *)   0xFFFE0108) // (PDC_SPI) Transmit Pointer Register
#define AT91C_SPI_RPR   ((AT91_REG *)   0xFFFE0100) // (PDC_SPI) Receive Pointer Register
#define AT91C_SPI_PTSR  ((AT91_REG *)   0xFFFE0124) // (PDC_SPI) PDC Transfer Status Register
#define AT91C_SPI_TNCR  ((AT91_REG *)   0xFFFE011C) // (PDC_SPI) Transmit Next Counter Register
#define AT91C_SPI_RNCR  ((AT91_REG *)   0xFFFE0114) // (PDC_SPI) Receive Next Counter Register
#define AT91C_SPI_TCR   ((AT91_REG *)   0xFFFE010C) // (PDC_SPI) Transmit Counter Register
#define AT91C_SPI_RCR   ((AT91_REG *)   0xFFFE0104) // (PDC_SPI) Receive Counter Register
// ========== Register definition for SPI peripheral ==========
#define AT91C_SPI_CSR   ((AT91_REG *)   0xFFFE0030) // (SPI) Chip Select Register
#define AT91C_SPI_IDR   ((AT91_REG *)   0xFFFE0018) // (SPI) Interrupt Disable Register
#define AT91C_SPI_SR    ((AT91_REG *)   0xFFFE0010) // (SPI) Status Register
#define AT91C_SPI_RDR   ((AT91_REG *)   0xFFFE0008) // (SPI) Receive Data Register
#define AT91C_SPI_CR    ((AT91_REG *)   0xFFFE0000) // (SPI) Control Register
#define AT91C_SPI_IMR   ((AT91_REG *)   0xFFFE001C) // (SPI) Interrupt Mask Register
#define AT91C_SPI_IER   ((AT91_REG *)   0xFFFE0014) // (SPI) Interrupt Enable Register
#define AT91C_SPI_TDR   ((AT91_REG *)   0xFFFE000C) // (SPI) Transmit Data Register
#define AT91C_SPI_MR    ((AT91_REG *)   0xFFFE0004) // (SPI) Mode Register
// ========== Register definition for PDC_US1 peripheral ==========
#define AT91C_US1_PTSR  ((AT91_REG *)   0xFFFC4124) // (PDC_US1) PDC Transfer Status Register
#define AT91C_US1_TNCR  ((AT91_REG *)   0xFFFC411C) // (PDC_US1) Transmit Next Counter Register
#define AT91C_US1_RNCR  ((AT91_REG *)   0xFFFC4114) // (PDC_US1) Receive Next Counter Register
#define AT91C_US1_TCR   ((AT91_REG *)   0xFFFC410C) // (PDC_US1) Transmit Counter Register
#define AT91C_US1_RCR   ((AT91_REG *)   0xFFFC4104) // (PDC_US1) Receive Counter Register
#define AT91C_US1_PTCR  ((AT91_REG *)   0xFFFC4120) // (PDC_US1) PDC Transfer Control Register
#define AT91C_US1_TNPR  ((AT91_REG *)   0xFFFC4118) // (PDC_US1) Transmit Next Pointer Register
#define AT91C_US1_RNPR  ((AT91_REG *)   0xFFFC4110) // (PDC_US1) Receive Next Pointer Register
#define AT91C_US1_TPR   ((AT91_REG *)   0xFFFC4108) // (PDC_US1) Transmit Pointer Register
#define AT91C_US1_RPR   ((AT91_REG *)   0xFFFC4100) // (PDC_US1) Receive Pointer Register
// ========== Register definition for US1 peripheral ==========
#define AT91C_US1_RHR   ((AT91_REG *)   0xFFFC4018) // (US1) Receiver Holding Register
#define AT91C_US1_IMR   ((AT91_REG *)   0xFFFC4010) // (US1) Interrupt Mask Register
#define AT91C_US1_IER   ((AT91_REG *)   0xFFFC4008) // (US1) Interrupt Enable Register
#define AT91C_US1_CR    ((AT91_REG *)   0xFFFC4000) // (US1) Control Register
#define AT91C_US1_RTOR  ((AT91_REG *)   0xFFFC4024) // (US1) Receiver Time-out Register
#define AT91C_US1_THR   ((AT91_REG *)   0xFFFC401C) // (US1) Transmitter Holding Register
#define AT91C_US1_CSR   ((AT91_REG *)   0xFFFC4014) // (US1) Channel Status Register
#define AT91C_US1_IDR   ((AT91_REG *)   0xFFFC400C) // (US1) Interrupt Disable Register
#define AT91C_US1_FIDI  ((AT91_REG *)   0xFFFC4040) // (US1) FI_DI_Ratio Register
#define AT91C_US1_BRGR  ((AT91_REG *)   0xFFFC4020) // (US1) Baud Rate Generator Register
#define AT91C_US1_TTGR  ((AT91_REG *)   0xFFFC4028) // (US1) Transmitter Time-guard Register
#define AT91C_US1_IF    ((AT91_REG *)   0xFFFC404C) // (US1) IRDA_FILTER Register
#define AT91C_US1_NER   ((AT91_REG *)   0xFFFC4044) // (US1) Nb Errors Register
#define AT91C_US1_MR    ((AT91_REG *)   0xFFFC4004) // (US1) Mode Register
// ========== Register definition for PDC_US0 peripheral ==========
#define AT91C_US0_PTCR  ((AT91_REG *)   0xFFFC0120) // (PDC_US0) PDC Transfer Control Register
#define AT91C_US0_TNPR  ((AT91_REG *)   0xFFFC0118) // (PDC_US0) Transmit Next Pointer Register
#define AT91C_US0_RNPR  ((AT91_REG *)   0xFFFC0110) // (PDC_US0) Receive Next Pointer Register
#define AT91C_US0_TPR   ((AT91_REG *)   0xFFFC0108) // (PDC_US0) Transmit Pointer Register
#define AT91C_US0_RPR   ((AT91_REG *)   0xFFFC0100) // (PDC_US0) Receive Pointer Register
#define AT91C_US0_PTSR  ((AT91_REG *)   0xFFFC0124) // (PDC_US0) PDC Transfer Status Register
#define AT91C_US0_TNCR  ((AT91_REG *)   0xFFFC011C) // (PDC_US0) Transmit Next Counter Register
#define AT91C_US0_RNCR  ((AT91_REG *)   0xFFFC0114) // (PDC_US0) Receive Next Counter Register
#define AT91C_US0_TCR   ((AT91_REG *)   0xFFFC010C) // (PDC_US0) Transmit Counter Register
#define AT91C_US0_RCR   ((AT91_REG *)   0xFFFC0104) // (PDC_US0) Receive Counter Register
// ========== Register definition for US0 peripheral ==========
#define AT91C_US0_TTGR  ((AT91_REG *)   0xFFFC0028) // (US0) Transmitter Time-guard Register
#define AT91C_US0_BRGR  ((AT91_REG *)   0xFFFC0020) // (US0) Baud Rate Generator Register
#define AT91C_US0_RHR   ((AT91_REG *)   0xFFFC0018) // (US0) Receiver Holding Register
#define AT91C_US0_IMR   ((AT91_REG *)   0xFFFC0010) // (US0) Interrupt Mask Register
#define AT91C_US0_NER   ((AT91_REG *)   0xFFFC0044) // (US0) Nb Errors Register
#define AT91C_US0_RTOR  ((AT91_REG *)   0xFFFC0024) // (US0) Receiver Time-out Register
#define AT91C_US0_FIDI  ((AT91_REG *)   0xFFFC0040) // (US0) FI_DI_Ratio Register
#define AT91C_US0_CR    ((AT91_REG *)   0xFFFC0000) // (US0) Control Register
#define AT91C_US0_IER   ((AT91_REG *)   0xFFFC0008) // (US0) Interrupt Enable Register
#define AT91C_US0_IF    ((AT91_REG *)   0xFFFC004C) // (US0) IRDA_FILTER Register
#define AT91C_US0_MR    ((AT91_REG *)   0xFFFC0004) // (US0) Mode Register
#define AT91C_US0_IDR   ((AT91_REG *)   0xFFFC000C) // (US0) Interrupt Disable Register
#define AT91C_US0_CSR   ((AT91_REG *)   0xFFFC0014) // (US0) Channel Status Register
#define AT91C_US0_THR   ((AT91_REG *)   0xFFFC001C) // (US0) Transmitter Holding Register
// ========== Register definition for PDC_SSC peripheral ==========
#define AT91C_SSC_PTCR  ((AT91_REG *)   0xFFFD4120) // (PDC_SSC) PDC Transfer Control Register
#define AT91C_SSC_TNPR  ((AT91_REG *)   0xFFFD4118) // (PDC_SSC) Transmit Next Pointer Register
#define AT91C_SSC_RNPR  ((AT91_REG *)   0xFFFD4110) // (PDC_SSC) Receive Next Pointer Register
#define AT91C_SSC_TPR   ((AT91_REG *)   0xFFFD4108) // (PDC_SSC) Transmit Pointer Register
#define AT91C_SSC_RPR   ((AT91_REG *)   0xFFFD4100) // (PDC_SSC) Receive Pointer Register
#define AT91C_SSC_PTSR  ((AT91_REG *)   0xFFFD4124) // (PDC_SSC) PDC Transfer Status Register
#define AT91C_SSC_TNCR  ((AT91_REG *)   0xFFFD411C) // (PDC_SSC) Transmit Next Counter Register
#define AT91C_SSC_RNCR  ((AT91_REG *)   0xFFFD4114) // (PDC_SSC) Receive Next Counter Register
#define AT91C_SSC_TCR   ((AT91_REG *)   0xFFFD410C) // (PDC_SSC) Transmit Counter Register
#define AT91C_SSC_RCR   ((AT91_REG *)   0xFFFD4104) // (PDC_SSC) Receive Counter Register
// ========== Register definition for SSC peripheral ==========
#define AT91C_SSC_RFMR  ((AT91_REG *)   0xFFFD4014) // (SSC) Receive Frame Mode Register
#define AT91C_SSC_CMR   ((AT91_REG *)   0xFFFD4004) // (SSC) Clock Mode Register
#define AT91C_SSC_IDR   ((AT91_REG *)   0xFFFD4048) // (SSC) Interrupt Disable Register
#define AT91C_SSC_SR    ((AT91_REG *)   0xFFFD4040) // (SSC) Status Register
#define AT91C_SSC_RSHR  ((AT91_REG *)   0xFFFD4030) // (SSC) Receive Sync Holding Register
#define AT91C_SSC_RHR   ((AT91_REG *)   0xFFFD4020) // (SSC) Receive Holding Register
#define AT91C_SSC_TCMR  ((AT91_REG *)   0xFFFD4018) // (SSC) Transmit Clock Mode Register
#define AT91C_SSC_RCMR  ((AT91_REG *)   0xFFFD4010) // (SSC) Receive Clock ModeRegister
#define AT91C_SSC_CR    ((AT91_REG *)   0xFFFD4000) // (SSC) Control Register
#define AT91C_SSC_IMR   ((AT91_REG *)   0xFFFD404C) // (SSC) Interrupt Mask Register
#define AT91C_SSC_IER   ((AT91_REG *)   0xFFFD4044) // (SSC) Interrupt Enable Register
#define AT91C_SSC_TSHR  ((AT91_REG *)   0xFFFD4034) // (SSC) Transmit Sync Holding Register
#define AT91C_SSC_THR   ((AT91_REG *)   0xFFFD4024) // (SSC) Transmit Holding Register
#define AT91C_SSC_TFMR  ((AT91_REG *)   0xFFFD401C) // (SSC) Transmit Frame Mode Register
// ========== Register definition for TWI peripheral ==========
#define AT91C_TWI_RHR   ((AT91_REG *)   0xFFFB8030) // (TWI) Receive Holding Register
#define AT91C_TWI_IDR   ((AT91_REG *)   0xFFFB8028) // (TWI) Interrupt Disable Register
#define AT91C_TWI_SR    ((AT91_REG *)   0xFFFB8020) // (TWI) Status Register
#define AT91C_TWI_CWGR  ((AT91_REG *)   0xFFFB8010) // (TWI) Clock Waveform Generator Register
#define AT91C_TWI_SMR   ((AT91_REG *)   0xFFFB8008) // (TWI) Slave Mode Register
#define AT91C_TWI_CR    ((AT91_REG *)   0xFFFB8000) // (TWI) Control Register
#define AT91C_TWI_THR   ((AT91_REG *)   0xFFFB8034) // (TWI) Transmit Holding Register
#define AT91C_TWI_IMR   ((AT91_REG *)   0xFFFB802C) // (TWI) Interrupt Mask Register
#define AT91C_TWI_IER   ((AT91_REG *)   0xFFFB8024) // (TWI) Interrupt Enable Register
#define AT91C_TWI_IADR  ((AT91_REG *)   0xFFFB800C) // (TWI) Internal Address Register
#define AT91C_TWI_MMR   ((AT91_REG *)   0xFFFB8004) // (TWI) Master Mode Register
// ========== Register definition for PWMC_CH3 peripheral ==========
#define AT91C_CH3_CUPDR ((AT91_REG *)   0xFFFCC270) // (PWMC_CH3) Channel Update Register
#define AT91C_CH3_CPRDR ((AT91_REG *)   0xFFFCC268) // (PWMC_CH3) Channel Period Register
#define AT91C_CH3_CMR   ((AT91_REG *)   0xFFFCC260) // (PWMC_CH3) Channel Mode Register
#define AT91C_CH3_Reserved ((AT91_REG *)        0xFFFCC274) // (PWMC_CH3) Reserved
#define AT91C_CH3_CCNTR ((AT91_REG *)   0xFFFCC26C) // (PWMC_CH3) Channel Counter Register
#define AT91C_CH3_CDTYR ((AT91_REG *)   0xFFFCC264) // (PWMC_CH3) Channel Duty Cycle Register
// ========== Register definition for PWMC_CH2 peripheral ==========
#define AT91C_CH2_CUPDR ((AT91_REG *)   0xFFFCC250) // (PWMC_CH2) Channel Update Register
#define AT91C_CH2_CPRDR ((AT91_REG *)   0xFFFCC248) // (PWMC_CH2) Channel Period Register
#define AT91C_CH2_CMR   ((AT91_REG *)   0xFFFCC240) // (PWMC_CH2) Channel Mode Register
#define AT91C_CH2_Reserved ((AT91_REG *)        0xFFFCC254) // (PWMC_CH2) Reserved
#define AT91C_CH2_CCNTR ((AT91_REG *)   0xFFFCC24C) // (PWMC_CH2) Channel Counter Register
#define AT91C_CH2_CDTYR ((AT91_REG *)   0xFFFCC244) // (PWMC_CH2) Channel Duty Cycle Register
// ========== Register definition for PWMC_CH1 peripheral ==========
#define AT91C_CH1_CUPDR ((AT91_REG *)   0xFFFCC230) // (PWMC_CH1) Channel Update Register
#define AT91C_CH1_CPRDR ((AT91_REG *)   0xFFFCC228) // (PWMC_CH1) Channel Period Register
#define AT91C_CH1_CMR   ((AT91_REG *)   0xFFFCC220) // (PWMC_CH1) Channel Mode Register
#define AT91C_CH1_Reserved ((AT91_REG *)        0xFFFCC234) // (PWMC_CH1) Reserved
#define AT91C_CH1_CCNTR ((AT91_REG *)   0xFFFCC22C) // (PWMC_CH1) Channel Counter Register
#define AT91C_CH1_CDTYR ((AT91_REG *)   0xFFFCC224) // (PWMC_CH1) Channel Duty Cycle Register
// ========== Register definition for PWMC_CH0 peripheral ==========
#define AT91C_CH0_CUPDR ((AT91_REG *)   0xFFFCC210) // (PWMC_CH0) Channel Update Register
#define AT91C_CH0_CPRDR ((AT91_REG *)   0xFFFCC208) // (PWMC_CH0) Channel Period Register
#define AT91C_CH0_CMR   ((AT91_REG *)   0xFFFCC200) // (PWMC_CH0) Channel Mode Register
#define AT91C_CH0_Reserved ((AT91_REG *)        0xFFFCC214) // (PWMC_CH0) Reserved
#define AT91C_CH0_CCNTR ((AT91_REG *)   0xFFFCC20C) // (PWMC_CH0) Channel Counter Register
#define AT91C_CH0_CDTYR ((AT91_REG *)   0xFFFCC204) // (PWMC_CH0) Channel Duty Cycle Register
// ========== Register definition for PWMC peripheral ==========
#define AT91C_PWMC_VR   ((AT91_REG *)   0xFFFCC0FC) // (PWMC) PWMC Version Register
#define AT91C_PWMC_ISR  ((AT91_REG *)   0xFFFCC01C) // (PWMC) PWMC Interrupt Status Register
#define AT91C_PWMC_IDR  ((AT91_REG *)   0xFFFCC014) // (PWMC) PWMC Interrupt Disable Register
#define AT91C_PWMC_SR   ((AT91_REG *)   0xFFFCC00C) // (PWMC) PWMC Status Register
#define AT91C_PWMC_ENA  ((AT91_REG *)   0xFFFCC004) // (PWMC) PWMC Enable Register
#define AT91C_PWMC_IMR  ((AT91_REG *)   0xFFFCC018) // (PWMC) PWMC Interrupt Mask Register
#define AT91C_PWMC_MR   ((AT91_REG *)   0xFFFCC000) // (PWMC) PWMC Mode Register
#define AT91C_PWMC_DIS  ((AT91_REG *)   0xFFFCC008) // (PWMC) PWMC Disable Register
#define AT91C_PWMC_IER  ((AT91_REG *)   0xFFFCC010) // (PWMC) PWMC Interrupt Enable Register
// ========== Register definition for UDP peripheral ==========
#define AT91C_UDP_TXVC  ((AT91_REG *)   0xFFFB0074) // (UDP) Transceiver Control Register
#define AT91C_UDP_ISR   ((AT91_REG *)   0xFFFB001C) // (UDP) Interrupt Status Register
#define AT91C_UDP_IDR   ((AT91_REG *)   0xFFFB0014) // (UDP) Interrupt Disable Register
#define AT91C_UDP_CSR   ((AT91_REG *)   0xFFFB0030) // (UDP) Endpoint Control and Status Register
#define AT91C_UDP_RSTEP ((AT91_REG *)   0xFFFB0028) // (UDP) Reset Endpoint Register
#define AT91C_UDP_ICR   ((AT91_REG *)   0xFFFB0020) // (UDP) Interrupt Clear Register
#define AT91C_UDP_GLBSTATE ((AT91_REG *)        0xFFFB0004) // (UDP) Global State Register
#define AT91C_UDP_NUM   ((AT91_REG *)   0xFFFB0000) // (UDP) Frame Number Register
#define AT91C_UDP_FADDR ((AT91_REG *)   0xFFFB0008) // (UDP) Function Address Register
#define AT91C_UDP_IER   ((AT91_REG *)   0xFFFB0010) // (UDP) Interrupt Enable Register
#define AT91C_UDP_IMR   ((AT91_REG *)   0xFFFB0018) // (UDP) Interrupt Mask Register
#define AT91C_UDP_FDR   ((AT91_REG *)   0xFFFB0050) // (UDP) Endpoint FIFO Data Register
// ========== Register definition for TC0 peripheral ==========
#define AT91C_TC0_IMR   ((AT91_REG *)   0xFFFA002C) // (TC0) Interrupt Mask Register
#define AT91C_TC0_IER   ((AT91_REG *)   0xFFFA0024) // (TC0) Interrupt Enable Register
#define AT91C_TC0_RC    ((AT91_REG *)   0xFFFA001C) // (TC0) Register C
#define AT91C_TC0_RA    ((AT91_REG *)   0xFFFA0014) // (TC0) Register A
#define AT91C_TC0_CMR   ((AT91_REG *)   0xFFFA0004) // (TC0) Channel Mode Register (Capture Mode / Waveform Mode)
#define AT91C_TC0_IDR   ((AT91_REG *)   0xFFFA0028) // (TC0) Interrupt Disable Register
#define AT91C_TC0_SR    ((AT91_REG *)   0xFFFA0020) // (TC0) Status Register
#define AT91C_TC0_RB    ((AT91_REG *)   0xFFFA0018) // (TC0) Register B
#define AT91C_TC0_CV    ((AT91_REG *)   0xFFFA0010) // (TC0) Counter Value
#define AT91C_TC0_CCR   ((AT91_REG *)   0xFFFA0000) // (TC0) Channel Control Register
// ========== Register definition for TC1 peripheral ==========
#define AT91C_TC1_IMR   ((AT91_REG *)   0xFFFA006C) // (TC1) Interrupt Mask Register
#define AT91C_TC1_IER   ((AT91_REG *)   0xFFFA0064) // (TC1) Interrupt Enable Register
#define AT91C_TC1_RC    ((AT91_REG *)   0xFFFA005C) // (TC1) Register C
#define AT91C_TC1_RA    ((AT91_REG *)   0xFFFA0054) // (TC1) Register A
#define AT91C_TC1_CMR   ((AT91_REG *)   0xFFFA0044) // (TC1) Channel Mode Register (Capture Mode / Waveform Mode)
#define AT91C_TC1_IDR   ((AT91_REG *)   0xFFFA0068) // (TC1) Interrupt Disable Register
#define AT91C_TC1_SR    ((AT91_REG *)   0xFFFA0060) // (TC1) Status Register
#define AT91C_TC1_RB    ((AT91_REG *)   0xFFFA0058) // (TC1) Register B
#define AT91C_TC1_CV    ((AT91_REG *)   0xFFFA0050) // (TC1) Counter Value
#define AT91C_TC1_CCR   ((AT91_REG *)   0xFFFA0040) // (TC1) Channel Control Register
// ========== Register definition for TC2 peripheral ==========
#define AT91C_TC2_IMR   ((AT91_REG *)   0xFFFA00AC) // (TC2) Interrupt Mask Register
#define AT91C_TC2_IER   ((AT91_REG *)   0xFFFA00A4) // (TC2) Interrupt Enable Register
#define AT91C_TC2_RC    ((AT91_REG *)   0xFFFA009C) // (TC2) Register C
#define AT91C_TC2_RA    ((AT91_REG *)   0xFFFA0094) // (TC2) Register A
#define AT91C_TC2_CMR   ((AT91_REG *)   0xFFFA0084) // (TC2) Channel Mode Register (Capture Mode / Waveform Mode)
#define AT91C_TC2_IDR   ((AT91_REG *)   0xFFFA00A8) // (TC2) Interrupt Disable Register
#define AT91C_TC2_SR    ((AT91_REG *)   0xFFFA00A0) // (TC2) Status Register
#define AT91C_TC2_RB    ((AT91_REG *)   0xFFFA0098) // (TC2) Register B
#define AT91C_TC2_CV    ((AT91_REG *)   0xFFFA0090) // (TC2) Counter Value
#define AT91C_TC2_CCR   ((AT91_REG *)   0xFFFA0080) // (TC2) Channel Control Register
// ========== Register definition for TCB peripheral ==========
#define AT91C_TCB_BMR   ((AT91_REG *)   0xFFFA00C4) // (TCB) TC Block Mode Register
#define AT91C_TCB_BCR   ((AT91_REG *)   0xFFFA00C0) // (TCB) TC Block Control Register
// ========== Register definition for PDC_ADC peripheral ==========
#define AT91C_ADC_PTCR  ((AT91_REG *)   0xFFFD8120) // (PDC_ADC) PDC Transfer Control Register
#define AT91C_ADC_TNPR  ((AT91_REG *)   0xFFFD8118) // (PDC_ADC) Transmit Next Pointer Register
#define AT91C_ADC_RNPR  ((AT91_REG *)   0xFFFD8110) // (PDC_ADC) Receive Next Pointer Register
#define AT91C_ADC_TPR   ((AT91_REG *)   0xFFFD8108) // (PDC_ADC) Transmit Pointer Register
#define AT91C_ADC_RPR   ((AT91_REG *)   0xFFFD8100) // (PDC_ADC) Receive Pointer Register
#define AT91C_ADC_PTSR  ((AT91_REG *)   0xFFFD8124) // (PDC_ADC) PDC Transfer Status Register
#define AT91C_ADC_TNCR  ((AT91_REG *)   0xFFFD811C) // (PDC_ADC) Transmit Next Counter Register
#define AT91C_ADC_RNCR  ((AT91_REG *)   0xFFFD8114) // (PDC_ADC) Receive Next Counter Register
#define AT91C_ADC_TCR   ((AT91_REG *)   0xFFFD810C) // (PDC_ADC) Transmit Counter Register
#define AT91C_ADC_RCR   ((AT91_REG *)   0xFFFD8104) // (PDC_ADC) Receive Counter Register
// ========== Register definition for ADC peripheral ==========
#define AT91C_ADC_IMR   ((AT91_REG *)   0xFFFD802C) // (ADC) ADC Interrupt Mask Register
#define AT91C_ADC_CDR4  ((AT91_REG *)   0xFFFD8040) // (ADC) ADC Channel Data Register 4
#define AT91C_ADC_CDR2  ((AT91_REG *)   0xFFFD8038) // (ADC) ADC Channel Data Register 2
#define AT91C_ADC_CDR0  ((AT91_REG *)   0xFFFD8030) // (ADC) ADC Channel Data Register 0
#define AT91C_ADC_CDR7  ((AT91_REG *)   0xFFFD804C) // (ADC) ADC Channel Data Register 7
#define AT91C_ADC_CDR1  ((AT91_REG *)   0xFFFD8034) // (ADC) ADC Channel Data Register 1
#define AT91C_ADC_CDR3  ((AT91_REG *)   0xFFFD803C) // (ADC) ADC Channel Data Register 3
#define AT91C_ADC_CDR5  ((AT91_REG *)   0xFFFD8044) // (ADC) ADC Channel Data Register 5
#define AT91C_ADC_MR    ((AT91_REG *)   0xFFFD8004) // (ADC) ADC Mode Register
#define AT91C_ADC_CDR6  ((AT91_REG *)   0xFFFD8048) // (ADC) ADC Channel Data Register 6
#define AT91C_ADC_CR    ((AT91_REG *)   0xFFFD8000) // (ADC) ADC Control Register
#define AT91C_ADC_CHER  ((AT91_REG *)   0xFFFD8010) // (ADC) ADC Channel Enable Register
#define AT91C_ADC_CHSR  ((AT91_REG *)   0xFFFD8018) // (ADC) ADC Channel Status Register
#define AT91C_ADC_IER   ((AT91_REG *)   0xFFFD8024) // (ADC) ADC Interrupt Enable Register
#define AT91C_ADC_SR    ((AT91_REG *)   0xFFFD801C) // (ADC) ADC Status Register
#define AT91C_ADC_CHDR  ((AT91_REG *)   0xFFFD8014) // (ADC) ADC Channel Disable Register
#define AT91C_ADC_IDR   ((AT91_REG *)   0xFFFD8028) // (ADC) ADC Interrupt Disable Register
#define AT91C_ADC_LCDR  ((AT91_REG *)   0xFFFD8020) // (ADC) ADC Last Converted Data Register
// ========== Register definition for EBI peripheral ==========
#define AT91C_EBI_CSA   ((AT91_REG *)   0xFFFFFF80) // (EBI) EBI Chip Select Assignment Register
// ========== Register definition for SMC peripheral ==========
#define AT91C_SMC_CSR   ((AT91_REG *)   0xFFFFFF90) // (SMC) SMC2 Chip Select Register
// ========== Register definition for SDRC peripheral ==========
#define AT91C_SDRC_VER  ((AT91_REG *)   0xFFFFFFD4) // (SDRC) SDRAM Controller Version Register
#define AT91C_SDRC_IMR  ((AT91_REG *)   0xFFFFFFCC) // (SDRC) SDRAM Controller Interrupt Mask Register
#define AT91C_SDRC_IER  ((AT91_REG *)   0xFFFFFFC4) // (SDRC) SDRAM Controller Interrupt Enable Register
#define AT91C_SDRC_SRR  ((AT91_REG *)   0xFFFFFFBC) // (SDRC) SDRAM Controller Self Refresh Register
#define AT91C_SDRC_TR   ((AT91_REG *)   0xFFFFFFB4) // (SDRC) SDRAM Controller Refresh Timer Register
#define AT91C_SDRC_ISR  ((AT91_REG *)   0xFFFFFFD0) // (SDRC) SDRAM Controller Interrupt Mask Register
#define AT91C_SDRC_IDR  ((AT91_REG *)   0xFFFFFFC8) // (SDRC) SDRAM Controller Interrupt Disable Register
#define AT91C_SDRC_LPR  ((AT91_REG *)   0xFFFFFFC0) // (SDRC) SDRAM Controller Low Power Register
#define AT91C_SDRC_CR   ((AT91_REG *)   0xFFFFFFB8) // (SDRC) SDRAM Controller Configuration Register
#define AT91C_SDRC_MR   ((AT91_REG *)   0xFFFFFFB0) // (SDRC) SDRAM Controller Mode Register
// ========== Register definition for HECC peripheral ==========
#define AT91C_HECC_NPR  ((AT91_REG *)   0xFFFFFFEC) // (HECC)  ECC Parity N register
#define AT91C_HECC_SR   ((AT91_REG *)   0xFFFFFFE4) // (HECC)  ECC Status register
#define AT91C_HECC_CR   ((AT91_REG *)   0xFFFFFFDC) // (HECC)  ECC reset register
#define AT91C_HECC_VR   ((AT91_REG *)   0xFFFFFFF0) // (HECC)  ECC Version register
#define AT91C_HECC_PR   ((AT91_REG *)   0xFFFFFFE8) // (HECC)  ECC Parity register
#define AT91C_HECC_MR   ((AT91_REG *)   0xFFFFFFE0) // (HECC)  ECC Page size register

// *****************************************************************************
//               PIO DEFINITIONS FOR AT91SAM7SE512
// *****************************************************************************
#define AT91C_PIO_PA0        ((unsigned int) 1 <<  0) // Pin Controlled by PA0
#define AT91C_PA0_PWM0     ((unsigned int) AT91C_PIO_PA0) //
#define AT91C_PA0_A0_NBS0  ((unsigned int) AT91C_PIO_PA0) //
#define AT91C_PIO_PA1        ((unsigned int) 1 <<  1) // Pin Controlled by PA1
#define AT91C_PA1_PWM1     ((unsigned int) AT91C_PIO_PA1) //
#define AT91C_PA1_A1_NBS2  ((unsigned int) AT91C_PIO_PA1) //
#define AT91C_PIO_PA10       ((unsigned int) 1 << 10) // Pin Controlled by PA10
#define AT91C_PA10_DTXD     ((unsigned int) AT91C_PIO_PA10) //
#define AT91C_PA10_A10      ((unsigned int) AT91C_PIO_PA10) //
#define AT91C_PIO_PA11       ((unsigned int) 1 << 11) // Pin Controlled by PA11
#define AT91C_PA11_NPCS0    ((unsigned int) AT91C_PIO_PA11) //
#define AT91C_PA11_A11      ((unsigned int) AT91C_PIO_PA11) //
#define AT91C_PIO_PA12       ((unsigned int) 1 << 12) // Pin Controlled by PA12
#define AT91C_PA12_MISO     ((unsigned int) AT91C_PIO_PA12) //
#define AT91C_PA12_A12      ((unsigned int) AT91C_PIO_PA12) //
#define AT91C_PIO_PA13       ((unsigned int) 1 << 13) // Pin Controlled by PA13
#define AT91C_PA13_MOSI     ((unsigned int) AT91C_PIO_PA13) //
#define AT91C_PA13_A13      ((unsigned int) AT91C_PIO_PA13) //
#define AT91C_PIO_PA14       ((unsigned int) 1 << 14) // Pin Controlled by PA14
#define AT91C_PA14_SPCK     ((unsigned int) AT91C_PIO_PA14) //
#define AT91C_PA14_A14      ((unsigned int) AT91C_PIO_PA14) //
#define AT91C_PIO_PA15       ((unsigned int) 1 << 15) // Pin Controlled by PA15
#define AT91C_PA15_TF       ((unsigned int) AT91C_PIO_PA15) //
#define AT91C_PA15_A15      ((unsigned int) AT91C_PIO_PA15) //
#define AT91C_PIO_PA16       ((unsigned int) 1 << 16) // Pin Controlled by PA16
#define AT91C_PA16_TK       ((unsigned int) AT91C_PIO_PA16) //
#define AT91C_PA16_A16_BA0  ((unsigned int) AT91C_PIO_PA16) //
#define AT91C_PIO_PA17       ((unsigned int) 1 << 17) // Pin Controlled by PA17
#define AT91C_PA17_TD       ((unsigned int) AT91C_PIO_PA17) //
#define AT91C_PA17_A17_BA1  ((unsigned int) AT91C_PIO_PA17) //
#define AT91C_PIO_PA18       ((unsigned int) 1 << 18) // Pin Controlled by PA18
#define AT91C_PA18_RD       ((unsigned int) AT91C_PIO_PA18) //
#define AT91C_PA18_NBS3_CFIOW ((unsigned int) AT91C_PIO_PA18) //
#define AT91C_PIO_PA19       ((unsigned int) 1 << 19) // Pin Controlled by PA19
#define AT91C_PA19_RK       ((unsigned int) AT91C_PIO_PA19) //
#define AT91C_PA19_NCS4_CFCS0 ((unsigned int) AT91C_PIO_PA19) //
#define AT91C_PIO_PA2        ((unsigned int) 1 <<  2) // Pin Controlled by PA2
#define AT91C_PA2_PWM2     ((unsigned int) AT91C_PIO_PA2) //
#define AT91C_PA2_A2       ((unsigned int) AT91C_PIO_PA2) //
#define AT91C_PIO_PA20       ((unsigned int) 1 << 20) // Pin Controlled by PA20
#define AT91C_PA20_RF       ((unsigned int) AT91C_PIO_PA20) //
#define AT91C_PA20_NCS2_CFCS1 ((unsigned int) AT91C_PIO_PA20) //
#define AT91C_PIO_PA21       ((unsigned int) 1 << 21) // Pin Controlled by PA21
#define AT91C_PA21_RXD1     ((unsigned int) AT91C_PIO_PA21) //
#define AT91C_PA21_NCS6_CFCE2 ((unsigned int) AT91C_PIO_PA21) //
#define AT91C_PIO_PA22       ((unsigned int) 1 << 22) // Pin Controlled by PA22
#define AT91C_PA22_TXD1     ((unsigned int) AT91C_PIO_PA22) //
#define AT91C_PA22_NCS5_CFCE1 ((unsigned int) AT91C_PIO_PA22) //
#define AT91C_PIO_PA23       ((unsigned int) 1 << 23) // Pin Controlled by PA23
#define AT91C_PA23_SCK1     ((unsigned int) AT91C_PIO_PA23) //
#define AT91C_PA23_NWR1_NBS1_CFIOR_NUB ((unsigned int) AT91C_PIO_PA23) //
#define AT91C_PIO_PA24       ((unsigned int) 1 << 24) // Pin Controlled by PA24
#define AT91C_PA24_RTS1     ((unsigned int) AT91C_PIO_PA24) //
#define AT91C_PA24_SDA10    ((unsigned int) AT91C_PIO_PA24) //
#define AT91C_PIO_PA25       ((unsigned int) 1 << 25) // Pin Controlled by PA25
#define AT91C_PA25_CTS1     ((unsigned int) AT91C_PIO_PA25) //
#define AT91C_PA25_SDCKE    ((unsigned int) AT91C_PIO_PA25) //
#define AT91C_PIO_PA26       ((unsigned int) 1 << 26) // Pin Controlled by PA26
#define AT91C_PA26_DCD1     ((unsigned int) AT91C_PIO_PA26) //
#define AT91C_PA26_NCS1_SDCS ((unsigned int) AT91C_PIO_PA26) //
#define AT91C_PIO_PA27       ((unsigned int) 1 << 27) // Pin Controlled by PA27
#define AT91C_PA27_DTR1     ((unsigned int) AT91C_PIO_PA27) //
#define AT91C_PA27_SDWE     ((unsigned int) AT91C_PIO_PA27) //
#define AT91C_PIO_PA28       ((unsigned int) 1 << 28) // Pin Controlled by PA28
#define AT91C_PA28_DSR1     ((unsigned int) AT91C_PIO_PA28) //
#define AT91C_PA28_CAS      ((unsigned int) AT91C_PIO_PA28) //
#define AT91C_PIO_PA29       ((unsigned int) 1 << 29) // Pin Controlled by PA29
#define AT91C_PA29_RI1      ((unsigned int) AT91C_PIO_PA29) //
#define AT91C_PA29_RAS      ((unsigned int) AT91C_PIO_PA29) //
#define AT91C_PIO_PA3        ((unsigned int) 1 <<  3) // Pin Controlled by PA3
#define AT91C_PA3_TWD      ((unsigned int) AT91C_PIO_PA3) //
#define AT91C_PA3_A3       ((unsigned int) AT91C_PIO_PA3) //
#define AT91C_PIO_PA30       ((unsigned int) 1 << 30) // Pin Controlled by PA30
#define AT91C_PA30_IRQ1     ((unsigned int) AT91C_PIO_PA30) //
#define AT91C_PA30_D30      ((unsigned int) AT91C_PIO_PA30) //
#define AT91C_PIO_PA31       ((unsigned int) 1 << 31) // Pin Controlled by PA31
#define AT91C_PA31_NPCS1    ((unsigned int) AT91C_PIO_PA31) //
#define AT91C_PA31_D31      ((unsigned int) AT91C_PIO_PA31) //
#define AT91C_PIO_PA4        ((unsigned int) 1 <<  4) // Pin Controlled by PA4
#define AT91C_PA4_TWCK     ((unsigned int) AT91C_PIO_PA4) //
#define AT91C_PA4_A4       ((unsigned int) AT91C_PIO_PA4) //
#define AT91C_PIO_PA5        ((unsigned int) 1 <<  5) // Pin Controlled by PA5
#define AT91C_PA5_RXD0     ((unsigned int) AT91C_PIO_PA5) //
#define AT91C_PA5_A5       ((unsigned int) AT91C_PIO_PA5) //
#define AT91C_PIO_PA6        ((unsigned int) 1 <<  6) // Pin Controlled by PA6
#define AT91C_PA6_TXD0     ((unsigned int) AT91C_PIO_PA6) //
#define AT91C_PA6_A6       ((unsigned int) AT91C_PIO_PA6) //
#define AT91C_PIO_PA7        ((unsigned int) 1 <<  7) // Pin Controlled by PA7
#define AT91C_PA7_RTS0     ((unsigned int) AT91C_PIO_PA7) //
#define AT91C_PA7_A7       ((unsigned int) AT91C_PIO_PA7) //
#define AT91C_PIO_PA8        ((unsigned int) 1 <<  8) // Pin Controlled by PA8
#define AT91C_PA8_CTS0     ((unsigned int) AT91C_PIO_PA8) //
#define AT91C_PA8_A8       ((unsigned int) AT91C_PIO_PA8) //
#define AT91C_PIO_PA9        ((unsigned int) 1 <<  9) // Pin Controlled by PA9
#define AT91C_PA9_DRXD     ((unsigned int) AT91C_PIO_PA9) //
#define AT91C_PA9_A9       ((unsigned int) AT91C_PIO_PA9) //
#define AT91C_PIO_PB0        ((unsigned int) 1 <<  0) // Pin Controlled by PB0
#define AT91C_PB0_TIOA0    ((unsigned int) AT91C_PIO_PB0) //
#define AT91C_PB0_A0_NBS0  ((unsigned int) AT91C_PIO_PB0) //
#define AT91C_PIO_PB1        ((unsigned int) 1 <<  1) // Pin Controlled by PB1
#define AT91C_PB1_TIOB0    ((unsigned int) AT91C_PIO_PB1) //
#define AT91C_PB1_A1_NBS2  ((unsigned int) AT91C_PIO_PB1) //
#define AT91C_PIO_PB10       ((unsigned int) 1 << 10) // Pin Controlled by PB10
#define AT91C_PB10_NPCS2    ((unsigned int) AT91C_PIO_PB10) //
#define AT91C_PB10_A10      ((unsigned int) AT91C_PIO_PB10) //
#define AT91C_PIO_PB11       ((unsigned int) 1 << 11) // Pin Controlled by PB11
#define AT91C_PB11_PWM0     ((unsigned int) AT91C_PIO_PB11) //
#define AT91C_PB11_A11      ((unsigned int) AT91C_PIO_PB11) //
#define AT91C_PIO_PB12       ((unsigned int) 1 << 12) // Pin Controlled by PB12
#define AT91C_PB12_PWM1     ((unsigned int) AT91C_PIO_PB12) //
#define AT91C_PB12_A12      ((unsigned int) AT91C_PIO_PB12) //
#define AT91C_PIO_PB13       ((unsigned int) 1 << 13) // Pin Controlled by PB13
#define AT91C_PB13_PWM2     ((unsigned int) AT91C_PIO_PB13) //
#define AT91C_PB13_A13      ((unsigned int) AT91C_PIO_PB13) //
#define AT91C_PIO_PB14       ((unsigned int) 1 << 14) // Pin Controlled by PB14
#define AT91C_PB14_PWM3     ((unsigned int) AT91C_PIO_PB14) //
#define AT91C_PB14_A14      ((unsigned int) AT91C_PIO_PB14) //
#define AT91C_PIO_PB15       ((unsigned int) 1 << 15) // Pin Controlled by PB15
#define AT91C_PB15_TIOA1    ((unsigned int) AT91C_PIO_PB15) //
#define AT91C_PB15_A15      ((unsigned int) AT91C_PIO_PB15) //
#define AT91C_PIO_PB16       ((unsigned int) 1 << 16) // Pin Controlled by PB16
#define AT91C_PB16_TIOA2    ((unsigned int) AT91C_PIO_PB16) //
#define AT91C_PB16_A16_BA0  ((unsigned int) AT91C_PIO_PB16) //
#define AT91C_PIO_PB17       ((unsigned int) 1 << 17) // Pin Controlled by PB17
#define AT91C_PB17_PCK1     ((unsigned int) AT91C_PIO_PB17) //
#define AT91C_PB17_A17_BA1  ((unsigned int) AT91C_PIO_PB17) //
#define AT91C_PIO_PB18       ((unsigned int) 1 << 18) // Pin Controlled by PB18
#define AT91C_PB18_PCK2     ((unsigned int) AT91C_PIO_PB18) //
#define AT91C_PB18_D16      ((unsigned int) AT91C_PIO_PB18) //
#define AT91C_PIO_PB19       ((unsigned int) 1 << 19) // Pin Controlled by PB19
#define AT91C_PB19_FIQ      ((unsigned int) AT91C_PIO_PB19) //
#define AT91C_PB19_D17      ((unsigned int) AT91C_PIO_PB19) //
#define AT91C_PIO_PB2        ((unsigned int) 1 <<  2) // Pin Controlled by PB2
#define AT91C_PB2_SCK0     ((unsigned int) AT91C_PIO_PB2) //
#define AT91C_PB2_A2       ((unsigned int) AT91C_PIO_PB2) //
#define AT91C_PIO_PB20       ((unsigned int) 1 << 20) // Pin Controlled by PB20
#define AT91C_PB20_IRQ0     ((unsigned int) AT91C_PIO_PB20) //
#define AT91C_PB20_D18      ((unsigned int) AT91C_PIO_PB20) //
#define AT91C_PIO_PB21       ((unsigned int) 1 << 21) // Pin Controlled by PB21
#define AT91C_PB21_PCK1     ((unsigned int) AT91C_PIO_PB21) //
#define AT91C_PB21_D19      ((unsigned int) AT91C_PIO_PB21) //
#define AT91C_PIO_PB22       ((unsigned int) 1 << 22) // Pin Controlled by PB22
#define AT91C_PB22_NPCS3    ((unsigned int) AT91C_PIO_PB22) //
#define AT91C_PB22_D20      ((unsigned int) AT91C_PIO_PB22) //
#define AT91C_PIO_PB23       ((unsigned int) 1 << 23) // Pin Controlled by PB23
#define AT91C_PB23_PWM0     ((unsigned int) AT91C_PIO_PB23) //
#define AT91C_PB23_D21      ((unsigned int) AT91C_PIO_PB23) //
#define AT91C_PIO_PB24       ((unsigned int) 1 << 24) // Pin Controlled by PB24
#define AT91C_PB24_PWM1     ((unsigned int) AT91C_PIO_PB24) //
#define AT91C_PB24_D22      ((unsigned int) AT91C_PIO_PB24) //
#define AT91C_PIO_PB25       ((unsigned int) 1 << 25) // Pin Controlled by PB25
#define AT91C_PB25_PWM2     ((unsigned int) AT91C_PIO_PB25) //
#define AT91C_PB25_D23      ((unsigned int) AT91C_PIO_PB25) //
#define AT91C_PIO_PB26       ((unsigned int) 1 << 26) // Pin Controlled by PB26
#define AT91C_PB26_TIOA2    ((unsigned int) AT91C_PIO_PB26) //
#define AT91C_PB26_D24      ((unsigned int) AT91C_PIO_PB26) //
#define AT91C_PIO_PB27       ((unsigned int) 1 << 27) // Pin Controlled by PB27
#define AT91C_PB27_TIOB2    ((unsigned int) AT91C_PIO_PB27) //
#define AT91C_PB27_D25      ((unsigned int) AT91C_PIO_PB27) //
#define AT91C_PIO_PB28       ((unsigned int) 1 << 28) // Pin Controlled by PB28
#define AT91C_PB28_TCLK1    ((unsigned int) AT91C_PIO_PB28) //
#define AT91C_PB28_D26      ((unsigned int) AT91C_PIO_PB28) //
#define AT91C_PIO_PB29       ((unsigned int) 1 << 29) // Pin Controlled by PB29
#define AT91C_PB29_TCLK2    ((unsigned int) AT91C_PIO_PB29) //
#define AT91C_PB29_D27      ((unsigned int) AT91C_PIO_PB29) //
#define AT91C_PIO_PB3        ((unsigned int) 1 <<  3) // Pin Controlled by PB3
#define AT91C_PB3_NPCS3    ((unsigned int) AT91C_PIO_PB3) //
#define AT91C_PB3_A3       ((unsigned int) AT91C_PIO_PB3) //
#define AT91C_PIO_PB30       ((unsigned int) 1 << 30) // Pin Controlled by PB30
#define AT91C_PB30_NPCS2    ((unsigned int) AT91C_PIO_PB30) //
#define AT91C_PB30_D28      ((unsigned int) AT91C_PIO_PB30) //
#define AT91C_PIO_PB31       ((unsigned int) 1 << 31) // Pin Controlled by PB31
#define AT91C_PB31_PCK2     ((unsigned int) AT91C_PIO_PB31) //
#define AT91C_PB31_D29      ((unsigned int) AT91C_PIO_PB31) //
#define AT91C_PIO_PB4        ((unsigned int) 1 <<  4) // Pin Controlled by PB4
#define AT91C_PB4_TCLK0    ((unsigned int) AT91C_PIO_PB4) //
#define AT91C_PB4_A4       ((unsigned int) AT91C_PIO_PB4) //
#define AT91C_PIO_PB5        ((unsigned int) 1 <<  5) // Pin Controlled by PB5
#define AT91C_PB5_NPCS3    ((unsigned int) AT91C_PIO_PB5) //
#define AT91C_PB5_A5       ((unsigned int) AT91C_PIO_PB5) //
#define AT91C_PIO_PB6        ((unsigned int) 1 <<  6) // Pin Controlled by PB6
#define AT91C_PB6_PCK0     ((unsigned int) AT91C_PIO_PB6) //
#define AT91C_PB6_A6       ((unsigned int) AT91C_PIO_PB6) //
#define AT91C_PIO_PB7        ((unsigned int) 1 <<  7) // Pin Controlled by PB7
#define AT91C_PB7_PWM3     ((unsigned int) AT91C_PIO_PB7) //
#define AT91C_PB7_A7       ((unsigned int) AT91C_PIO_PB7) //
#define AT91C_PIO_PB8        ((unsigned int) 1 <<  8) // Pin Controlled by PB8
#define AT91C_PB8_ADTRG    ((unsigned int) AT91C_PIO_PB8) //
#define AT91C_PB8_A8       ((unsigned int) AT91C_PIO_PB8) //
#define AT91C_PIO_PB9        ((unsigned int) 1 <<  9) // Pin Controlled by PB9
#define AT91C_PB9_NPCS1    ((unsigned int) AT91C_PIO_PB9) //
#define AT91C_PB9_A9       ((unsigned int) AT91C_PIO_PB9) //
#define AT91C_PIO_PC0        ((unsigned int) 1 <<  0) // Pin Controlled by PC0
#define AT91C_PC0_D0       ((unsigned int) AT91C_PIO_PC0) //
#define AT91C_PIO_PC1        ((unsigned int) 1 <<  1) // Pin Controlled by PC1
#define AT91C_PC1_D1       ((unsigned int) AT91C_PIO_PC1) //
#define AT91C_PIO_PC10       ((unsigned int) 1 << 10) // Pin Controlled by PC10
#define AT91C_PC10_D10      ((unsigned int) AT91C_PIO_PC10) //
#define AT91C_PC10_PCK0     ((unsigned int) AT91C_PIO_PC10) //
#define AT91C_PIO_PC11       ((unsigned int) 1 << 11) // Pin Controlled by PC11
#define AT91C_PC11_D11      ((unsigned int) AT91C_PIO_PC11) //
#define AT91C_PC11_PCK1     ((unsigned int) AT91C_PIO_PC11) //
#define AT91C_PIO_PC12       ((unsigned int) 1 << 12) // Pin Controlled by PC12
#define AT91C_PC12_D12      ((unsigned int) AT91C_PIO_PC12) //
#define AT91C_PC12_PCK2     ((unsigned int) AT91C_PIO_PC12) //
#define AT91C_PIO_PC13       ((unsigned int) 1 << 13) // Pin Controlled by PC13
#define AT91C_PC13_D13      ((unsigned int) AT91C_PIO_PC13) //
#define AT91C_PIO_PC14       ((unsigned int) 1 << 14) // Pin Controlled by PC14
#define AT91C_PC14_D14      ((unsigned int) AT91C_PIO_PC14) //
#define AT91C_PC14_NPCS1    ((unsigned int) AT91C_PIO_PC14) //
#define AT91C_PIO_PC15       ((unsigned int) 1 << 15) // Pin Controlled by PC15
#define AT91C_PC15_D15      ((unsigned int) AT91C_PIO_PC15) //
#define AT91C_PC15_NCS3_NANDCS ((unsigned int) AT91C_PIO_PC15) //
#define AT91C_PIO_PC16       ((unsigned int) 1 << 16) // Pin Controlled by PC16
#define AT91C_PC16_A18      ((unsigned int) AT91C_PIO_PC16) //
#define AT91C_PC16_NWAIT    ((unsigned int) AT91C_PIO_PC16) //
#define AT91C_PIO_PC17       ((unsigned int) 1 << 17) // Pin Controlled by PC17
#define AT91C_PC17_A19      ((unsigned int) AT91C_PIO_PC17) //
#define AT91C_PC17_NANDOE   ((unsigned int) AT91C_PIO_PC17) //
#define AT91C_PIO_PC18       ((unsigned int) 1 << 18) // Pin Controlled by PC18
#define AT91C_PC18_A20      ((unsigned int) AT91C_PIO_PC18) //
#define AT91C_PC18_NANDWE   ((unsigned int) AT91C_PIO_PC18) //
#define AT91C_PIO_PC19       ((unsigned int) 1 << 19) // Pin Controlled by PC19
#define AT91C_PC19_A21      ((unsigned int) AT91C_PIO_PC19) //
#define AT91C_PIO_PC2        ((unsigned int) 1 <<  2) // Pin Controlled by PC2
#define AT91C_PC2_D2       ((unsigned int) AT91C_PIO_PC2) //
#define AT91C_PIO_PC20       ((unsigned int) 1 << 20) // Pin Controlled by PC20
#define AT91C_PC20_A22      ((unsigned int) AT91C_PIO_PC20) //
#define AT91C_PC20_NCS7     ((unsigned int) AT91C_PIO_PC20) //
#define AT91C_PIO_PC21       ((unsigned int) 1 << 21) // Pin Controlled by PC21
#define AT91C_PC21_A23      ((unsigned int) AT91C_PIO_PC21) //
#define AT91C_PC21_NWR0_NWE_CFWE ((unsigned int) AT91C_PIO_PC21) //
#define AT91C_PIO_PC22       ((unsigned int) 1 << 22) // Pin Controlled by PC22
#define AT91C_PC22_A24      ((unsigned int) AT91C_PIO_PC22) //
#define AT91C_PC22_NRD_CFOE ((unsigned int) AT91C_PIO_PC22) //
#define AT91C_PIO_PC23       ((unsigned int) 1 << 23) // Pin Controlled by PC23
#define AT91C_PC23_A25_CFRNW ((unsigned int) AT91C_PIO_PC23) //
#define AT91C_PC23_NCS0     ((unsigned int) AT91C_PIO_PC23) //
#define AT91C_PIO_PC3        ((unsigned int) 1 <<  3) // Pin Controlled by PC3
#define AT91C_PC3_D3       ((unsigned int) AT91C_PIO_PC3) //
#define AT91C_PIO_PC4        ((unsigned int) 1 <<  4) // Pin Controlled by PC4
#define AT91C_PC4_D4       ((unsigned int) AT91C_PIO_PC4) //
#define AT91C_PIO_PC5        ((unsigned int) 1 <<  5) // Pin Controlled by PC5
#define AT91C_PC5_D5       ((unsigned int) AT91C_PIO_PC5) //
#define AT91C_PIO_PC6        ((unsigned int) 1 <<  6) // Pin Controlled by PC6
#define AT91C_PC6_D6       ((unsigned int) AT91C_PIO_PC6) //
#define AT91C_PIO_PC7        ((unsigned int) 1 <<  7) // Pin Controlled by PC7
#define AT91C_PC7_D7       ((unsigned int) AT91C_PIO_PC7) //
#define AT91C_PIO_PC8        ((unsigned int) 1 <<  8) // Pin Controlled by PC8
#define AT91C_PC8_D8       ((unsigned int) AT91C_PIO_PC8) //
#define AT91C_PC8_RTS1     ((unsigned int) AT91C_PIO_PC8) //
#define AT91C_PIO_PC9        ((unsigned int) 1 <<  9) // Pin Controlled by PC9
#define AT91C_PC9_D9       ((unsigned int) AT91C_PIO_PC9) //
#define AT91C_PC9_DTR1     ((unsigned int) AT91C_PIO_PC9) //

// *****************************************************************************
//               PERIPHERAL ID DEFINITIONS FOR AT91SAM7SE512
// *****************************************************************************
#define AT91C_ID_FIQ    ((unsigned int)  0) // Advanced Interrupt Controller (FIQ)
#define AT91C_ID_SYS    ((unsigned int)  1) // System Peripheral
#define AT91C_ID_PIOA   ((unsigned int)  2) // Parallel IO Controller A
#define AT91C_ID_PIOB   ((unsigned int)  3) // Parallel IO Controller B
#define AT91C_ID_PIOC   ((unsigned int)  4) // Parallel IO Controller C
#define AT91C_ID_SPI    ((unsigned int)  5) // Serial Peripheral Interface 0
#define AT91C_ID_US0    ((unsigned int)  6) // USART 0
#define AT91C_ID_US1    ((unsigned int)  7) // USART 1
#define AT91C_ID_SSC    ((unsigned int)  8) // Serial Synchronous Controller
#define AT91C_ID_TWI    ((unsigned int)  9) // Two-Wire Interface
#define AT91C_ID_PWMC   ((unsigned int) 10) // PWM Controller
#define AT91C_ID_UDP    ((unsigned int) 11) // USB Device Port
#define AT91C_ID_TC0    ((unsigned int) 12) // Timer Counter 0
#define AT91C_ID_TC1    ((unsigned int) 13) // Timer Counter 1
#define AT91C_ID_TC2    ((unsigned int) 14) // Timer Counter 2
#define AT91C_ID_ADC    ((unsigned int) 15) // Analog-to-Digital Converter
#define AT91C_ID_16_Reserved ((unsigned int) 16) // Reserved
#define AT91C_ID_17_Reserved ((unsigned int) 17) // Reserved
#define AT91C_ID_18_Reserved ((unsigned int) 18) // Reserved
#define AT91C_ID_19_Reserved ((unsigned int) 19) // Reserved
#define AT91C_ID_20_Reserved ((unsigned int) 20) // Reserved
#define AT91C_ID_21_Reserved ((unsigned int) 21) // Reserved
#define AT91C_ID_22_Reserved ((unsigned int) 22) // Reserved
#define AT91C_ID_23_Reserved ((unsigned int) 23) // Reserved
#define AT91C_ID_24_Reserved ((unsigned int) 24) // Reserved
#define AT91C_ID_25_Reserved ((unsigned int) 25) // Reserved
#define AT91C_ID_26_Reserved ((unsigned int) 26) // Reserved
#define AT91C_ID_27_Reserved ((unsigned int) 27) // Reserved
#define AT91C_ID_28_Reserved ((unsigned int) 28) // Reserved
#define AT91C_ID_IRQ0   ((unsigned int) 29) // Advanced Interrupt Controller (IRQ0)
#define AT91C_ID_IRQ1   ((unsigned int) 30) // Advanced Interrupt Controller (IRQ1)
#define AT91C_ID_31_Reserved ((unsigned int) 31) // Reserved
#define AT91C_ALL_INT   ((unsigned int) 0x6000FFFF) // ALL VALID INTERRUPTS

// *****************************************************************************
//               BASE ADDRESS DEFINITIONS FOR AT91SAM7SE512
// *****************************************************************************
#define AT91C_BASE_SYS       ((AT91PS_SYS)      0xFFFFF000) // (SYS) Base Address
#define AT91C_BASE_AIC       ((AT91PS_AIC)      0xFFFFF000) // (AIC) Base Address
#define AT91C_BASE_PDC_DBGU  ((AT91PS_PDC)      0xFFFFF300) // (PDC_DBGU) Base Address
#define AT91C_BASE_DBGU      ((AT91PS_DBGU)     0xFFFFF200) // (DBGU) Base Address
#define AT91C_BASE_PIOA      ((AT91PS_PIO)      0xFFFFF400) // (PIOA) Base Address
#define AT91C_BASE_PIOB      ((AT91PS_PIO)      0xFFFFF600) // (PIOB) Base Address
#define AT91C_BASE_PIOC      ((AT91PS_PIO)      0xFFFFF800) // (PIOC) Base Address
#define AT91C_BASE_CKGR      ((AT91PS_CKGR)     0xFFFFFC20) // (CKGR) Base Address
#define AT91C_BASE_PMC       ((AT91PS_PMC)      0xFFFFFC00) // (PMC) Base Address
#define AT91C_BASE_RSTC      ((AT91PS_RSTC)     0xFFFFFD00) // (RSTC) Base Address
#define AT91C_BASE_RTTC      ((AT91PS_RTTC)     0xFFFFFD20) // (RTTC) Base Address
#define AT91C_BASE_PITC      ((AT91PS_PITC)     0xFFFFFD30) // (PITC) Base Address
#define AT91C_BASE_WDTC      ((AT91PS_WDTC)     0xFFFFFD40) // (WDTC) Base Address
#define AT91C_BASE_VREG      ((AT91PS_VREG)     0xFFFFFD60) // (VREG) Base Address
#define AT91C_BASE_MC        ((AT91PS_MC)       0xFFFFFF00) // (MC) Base Address
#define AT91C_BASE_EFC0      ((AT91PS_EFC)      0xFFFFFF60) // (EFC0) Base Address
#define AT91C_BASE_EFC1      ((AT91PS_EFC)      0xFFFFFF70) // (EFC1) Base Address
#define AT91C_BASE_PDC_SPI   ((AT91PS_PDC)      0xFFFE0100) // (PDC_SPI) Base Address
#define AT91C_BASE_SPI       ((AT91PS_SPI)      0xFFFE0000) // (SPI) Base Address
#define AT91C_BASE_PDC_US1   ((AT91PS_PDC)      0xFFFC4100) // (PDC_US1) Base Address
#define AT91C_BASE_US1       ((AT91PS_USART)    0xFFFC4000) // (US1) Base Address
#define AT91C_BASE_PDC_US0   ((AT91PS_PDC)      0xFFFC0100) // (PDC_US0) Base Address
#define AT91C_BASE_US0       ((AT91PS_USART)    0xFFFC0000) // (US0) Base Address
#define AT91C_BASE_PDC_SSC   ((AT91PS_PDC)      0xFFFD4100) // (PDC_SSC) Base Address
#define AT91C_BASE_SSC       ((AT91PS_SSC)      0xFFFD4000) // (SSC) Base Address
#define AT91C_BASE_TWI       ((AT91PS_TWI)      0xFFFB8000) // (TWI) Base Address
#define AT91C_BASE_PWMC_CH3  ((AT91PS_PWMC_CH)  0xFFFCC260) // (PWMC_CH3) Base Address
#define AT91C_BASE_PWMC_CH2  ((AT91PS_PWMC_CH)  0xFFFCC240) // (PWMC_CH2) Base Address
#define AT91C_BASE_PWMC_CH1  ((AT91PS_PWMC_CH)  0xFFFCC220) // (PWMC_CH1) Base Address
#define AT91C_BASE_PWMC_CH0  ((AT91PS_PWMC_CH)  0xFFFCC200) // (PWMC_CH0) Base Address
#define AT91C_BASE_PWMC      ((AT91PS_PWMC)     0xFFFCC000) // (PWMC) Base Address
#define AT91C_BASE_UDP       ((AT91PS_UDP)      0xFFFB0000) // (UDP) Base Address
#define AT91C_BASE_TC0       ((AT91PS_TC)       0xFFFA0000) // (TC0) Base Address
#define AT91C_BASE_TC1       ((AT91PS_TC)       0xFFFA0040) // (TC1) Base Address
#define AT91C_BASE_TC2       ((AT91PS_TC)       0xFFFA0080) // (TC2) Base Address
#define AT91C_BASE_TCB       ((AT91PS_TCB)      0xFFFA0000) // (TCB) Base Address
#define AT91C_BASE_PDC_ADC   ((AT91PS_PDC)      0xFFFD8100) // (PDC_ADC) Base Address
#define AT91C_BASE_ADC       ((AT91PS_ADC)      0xFFFD8000) // (ADC) Base Address
#define AT91C_BASE_EBI       ((AT91PS_EBI)      0xFFFFFF80) // (EBI) Base Address
#define AT91C_BASE_SMC       ((AT91PS_SMC2)     0xFFFFFF90) // (SMC) Base Address
#define AT91C_BASE_SDRC      ((AT91PS_SDRC)     0xFFFFFFB0) // (SDRC) Base Address
#define AT91C_BASE_HECC      ((AT91PS_ECC)      0xFFFFFFDC) // (HECC) Base Address

// *****************************************************************************
//               MEMORY MAPPING DEFINITIONS FOR AT91SAM7SE512
// *****************************************************************************
// ISRAM
#define AT91C_ISRAM      ((char *)      0x00200000) // Internal SRAM base address
#define AT91C_ISRAM_SIZE         ((unsigned int) 0x00010000) // Internal SRAM size in byte (64 Kbytes)
// IFLASH
#define AT91C_IFLASH     ((char *)      0x00100000) // Internal FLASH base address
#define AT91C_IFLASH_SIZE        ((unsigned int) 0x00080000) // Internal FLASH size in byte (512 Kbytes)
#define AT91C_IFLASH_PAGE_SIZE   ((unsigned int) 256) // Internal FLASH Page Size: 256 bytes
#define AT91C_IFLASH_LOCK_REGION_SIZE    ((unsigned int) 16384) // Internal FLASH Lock Region Size: 16 Kbytes
#define AT91C_IFLASH_NB_OF_PAGES         ((unsigned int) 2048) // Internal FLASH Number of Pages: 2048 bytes
#define AT91C_IFLASH_NB_OF_LOCK_BITS     ((unsigned int) 32) // Internal FLASH Number of Lock Bits: 32 bytes

#endif
