//Hooks for FreeRTOS
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "osi.h"

void vApplicationTickHook( void )
{
}

void vAssertCalled( const char *pcFile, unsigned long ulLine )
{
  while(1)
  {
  }
}

void vApplicationIdleHook( void )
{
}

void vApplicationStackOverflowHook( OsiTaskHandle *pxTask,
    signed char *pcTaskName)
{
  while(1)
  {
  }
}

void vApplicationMallocFailedHook()
{
  while(1)
  {
  }
}
