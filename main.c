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

#include "uart.h"
#include "udma_if.h"
#include "common.h"

#include "pinmux.h"
#include "log.h"
#include "switch_task.h"
#include "smart_config_task.h"

#define TASK_STACK_SIZE 1024 
#define SPAWN_TASK_PRIORITY   9

#if defined(ccs) || defined(gcc)
extern void (* const g_pfnVectors[])(void);
#endif 
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif 

static void BoardInit(void);

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
  long lRetVal = -1;

  BoardInit();
  UDMAInit();
  PinMuxConfig();

  GPIO_IF_LedConfigure(LED1|LED2|LED3);
  GPIO_IF_LedOff(MCU_ALL_LED_IND);

  printf("*************************"); 
  printf("* WiSwi Running"); 
  printf("*************************"); 
  
  lRetVal = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
  if(lRetVal < 0)
  {
    ERR_PRINT(lRetVal);
    LOOP_FOREVER();
  }

  osi_TaskCreate(SwitchTask, (signed portCHAR * ) "Switch Task",
    TASK_STACK_SIZE, NULL, 1, NULL );

  osi_TaskCreate(SmartConfigTask, (signed portCHAR * ) "Smart Config Task",
    2048, NULL, 1, NULL );

  osi_start();

  return 0;
}

