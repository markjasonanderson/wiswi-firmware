//
// WiSwi firmare, based on TI CC3200 blinky example
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "portmacro.h"
#include "osi.h"

#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_types.h"
#include "hw_ints.h"
#include "interrupt.h"
#include "rom.h"
#include "rom_map.h"
#include "uart.h"
#include "prcm.h"
#include "utils.h"
#include "gpio.h"
#include "gpio_if.h"

#include "pinmux.h"
#include "log.h"

#define TASK_STACK_SIZE 1024 
#define SPAWN_TASK_PRIORITY   9

#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif 
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif 


static void LEDTask(void *pvParameters);
static void BoardInit(void);

void LEDTask(void *pvParameters)
{
  log_info("LED Task has started");
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
#if defined(ccs) || defined(gcc)
  MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
  MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif

  // Enable Processor
  MAP_IntMasterEnable();
  MAP_IntEnable(FAULT_SYSTICK);

  PRCMCC3200MCUInit();
}

int main()
{
  long ret;
  BoardInit();
  PinMuxConfig();

  GPIO_IF_LedConfigure(LED1|LED2|LED3);
  GPIO_IF_LedOff(MCU_ALL_LED_IND);

  printf("*************************"); 
  printf(" WiSwi Running"); 
  printf("*************************"); 

  osi_TaskCreate(LEDTask, (signed portCHAR * ) "TASK1",
    TASK_STACK_SIZE, NULL, 1, NULL );

  osi_start();

  return 0;
}

