
#include "switch_task.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "rom_map.h"
#include "utils.h"

#include "pinmux.h"
#include "gpio.h"
#include "gpio_if.h"
#include "log.h"

void SwitchTask(void *pvParameters)
{
  log_info("LED Task is running");
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
