//
// WiSwi firmare, based on TI CC3200 blinky example

#include <stdio.h>
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "interrupt.h"
#include "pin.h"
#include "hw_apps_rcm.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "prcm.h"
#include "gpio.h"
#include "utils.h"
#include "pinmux.h"
#include "gpio_if.h"

#include "log.h"

#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

void LEDTask();
static void BoardInit(void);

void LEDTask()
{
    GPIO_IF_LedOff(MCU_ALL_LED_IND);
    while(1)
    {
        MAP_UtilsDelay(8000000);
        GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
        if (MAP_GPIOPinRead(GPIOA0_BASE,GPIO_PIN_6))
        {
          GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        }
        else 
        {
          GPIO_IF_LedOff(MCU_RED_LED_GPIO);
        }
        log_info("Switch A is %s", MAP_GPIOPinRead(GPIOA0_BASE,GPIO_PIN_6) ? "on" : "off");
        
        if (MAP_GPIOPinRead(GPIOA0_BASE,GPIO_PIN_7))
        {
          GPIO_IF_LedOn(MCU_ORANGE_LED_GPIO);
        }
        else 
        {
          GPIO_IF_LedOff(MCU_ORANGE_LED_GPIO);
        }
        log_info("Switch B is %s", MAP_GPIOPinRead(GPIOA0_BASE,GPIO_PIN_7) ? "on" : "off");

        MAP_UtilsDelay(8000000);
        GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);
    }
}

static void BoardInit(void)
{
    // Enable Processor
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

int main()
{
    // Initialize Board configurations
    BoardInit();
    
    PinMuxConfig();
    GPIO_IF_LedConfigure(LED1|LED2|LED3);
	  GPIO_IF_LedOff(MCU_ALL_LED_IND);

    printf("*************************"); 
    printf("WiSwi Running"); 
    printf("*************************"); 
    // Start the LEDTask
    LEDTask();
    return 0;
}

