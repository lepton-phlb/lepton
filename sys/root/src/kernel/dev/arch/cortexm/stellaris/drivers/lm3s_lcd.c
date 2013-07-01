//*****************************************************************************
//
// lm3s_lcd.c - Init DK-LM3SD96 LCD display
//
//*****************************************************************************

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
//#include "driverlib/rom.h"
#include "driverlib/udma.h"
#include "grlib/grlib.h"
//#include "grlib/widget.h"
//#include "grlib/canvas.h"
//#include "grlib/checkbox.h"
//#include "grlib/container.h"
//#include "grlib/pushbutton.h"
//#include "grlib/radiobutton.h"
//#include "grlib/slider.h"
//#include "utils/ustdlib.h"
#include "drivers/kitronix320x240x16_ssd2119_8bit.h"
//#include "drivers/sound.h"
//#include "drivers/touch.h"
#include "drivers/set_pinout.h"
//#include "images.h"

//tDaughterBoard g_eDaughterType;

void
lcd_init(void)
{
    tContext sContext;
    tRectangle sRect;
    
    //
    // Set the device pinout appropriately for this board.
    //
    PinoutSet(); //should take place before settong up sdram I think.. anyhow, switching to manual settings :) :
    g_eDaughterType = DAUGHTER_NONE;
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
//GD-TODO create a BOARD/peripherals init function/file
    //
    // Initialize the display driver.
    //
    Kitronix320x240x16_SSD2119Init();

    //
    // Initialize the graphics context.
    //
    GrContextInit(&sContext, &g_sKitronix320x240x16_SSD2119);

    //
    // Fill the top 24 rows of the screen with blue to create the banner.
    //
    sRect.sXMin = 0;
    sRect.sYMin = 0;
    sRect.sXMax = GrContextDpyWidthGet(&sContext) - 1;
    sRect.sYMax = 23;
    GrContextForegroundSet(&sContext, ClrDarkBlue);
    GrRectFill(&sContext, &sRect);
    
    //
    // Put a white box around the banner.
    //
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectDraw(&sContext, &sRect);

    //
    // Put the application name in the middle of the banner.
    //
    GrContextFontSet(&sContext, &g_sFontCmss20);
    GrStringDrawCentered(&sContext, "Lepton", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 10, 0);

    //
    // Configure and enable uDMA
    //
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
//    SysCtlDelay(10);
//    uDMAControlBaseSet(&sDMAControlTable[0]);
//    uDMAEnable();
}

