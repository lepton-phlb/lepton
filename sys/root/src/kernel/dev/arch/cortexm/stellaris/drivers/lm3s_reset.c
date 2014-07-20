
#include "inc\hw_types.h"
#include "inc\hw_sysctl.h"
#include "inc\hw_nvic.h"
#include "inc\hw_flash.h"
extern unsigned char IntMasterDisable(void);
void forceLM3Sreset(void);
void forceLM3Sreset(void)
{
    //
    // Disable processor interrupts.
    //
    IntMasterDisable();
    //
    // Disable the PLL and the system clock divider (this is a NOP if they are
    // already disabled).
    //
    HWREG(SYSCTL_RCC) = ((HWREG(SYSCTL_RCC) & ~(SYSCTL_RCC_USESYSDIV)) |
                         SYSCTL_RCC_BYPASS);
    HWREG(SYSCTL_RCC2) |= SYSCTL_RCC2_BYPASS2;
    //
    // Now, write RCC and RCC2 to their reset values.
    //
    HWREG(SYSCTL_RCC) = 0x078e3ad0 | (HWREG(SYSCTL_RCC) & SYSCTL_RCC_MOSCDIS);
    HWREG(SYSCTL_RCC2) = 0x07806810;
    HWREG(SYSCTL_RCC) = 0x078e3ad1;
    //
    // Reset the deep sleep clock configuration register.
    //
    HWREG(SYSCTL_DSLPCLKCFG) = 0x07800000;
    //
    // Reset the clock gating registers.
    //
    HWREG(SYSCTL_RCGC0) = 0x00000040;
    HWREG(SYSCTL_RCGC1) = 0;
    HWREG(SYSCTL_RCGC2) = 0;
    HWREG(SYSCTL_SCGC0) = 0x00000040;
    HWREG(SYSCTL_SCGC1) = 0;
    HWREG(SYSCTL_SCGC2) = 0;
    HWREG(SYSCTL_DCGC0) = 0x00000040;
    HWREG(SYSCTL_DCGC1) = 0;
    HWREG(SYSCTL_DCGC2) = 0;
    //
    // Reset the remaining SysCtl registers.
    //
    HWREG(SYSCTL_PBORCTL) = 0;
    HWREG(SYSCTL_IMC) = 0;
    HWREG(SYSCTL_GPIOHBCTL) = 0;
    HWREG(SYSCTL_MOSCCTL) = 0;
    HWREG(SYSCTL_PIOSCCAL) = 0;
    HWREG(SYSCTL_I2SMCLKCFG) = 0;
    //
    // Reset the peripherals.
    //
    HWREG(SYSCTL_SRCR0) = 0xffffffff;
    HWREG(SYSCTL_SRCR1) = 0xffffffff;
    HWREG(SYSCTL_SRCR2) = 0xffffffff;
    HWREG(SYSCTL_SRCR0) = 0;
    HWREG(SYSCTL_SRCR1) = 0;
    HWREG(SYSCTL_SRCR2) = 0;
    //
    // Clear any pending SysCtl interrupts.
    //
    HWREG(SYSCTL_MISC) = 0xffffffff;
    //
    // Wait for any pending flash operations to complete.
    //
    while((HWREG(FLASH_FMC) & 0xffff) != 0)
    {
    }
    while((HWREG(FLASH_FMC2) & 0xffff) != 0)
    {
    }
    //
    // Reset the flash controller registers.
    //
    HWREG(FLASH_FMA) = 0;
    HWREG(FLASH_FCIM) = 0;
    HWREG(FLASH_FCMISC) = 0xffffffff;
    HWREG(FLASH_FWBVAL) = 0;
    //
    // Issue the core reset.
    //
    HWREG(NVIC_APINT) = NVIC_APINT_VECTKEY | NVIC_APINT_VECT_RESET;
}



