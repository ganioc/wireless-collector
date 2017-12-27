#include "myled.h"

uint16_t mDelayPeriod = DELAY_NORMAL;

void LED_On(uint16_t pin)
{
  HAL_GPIO_WritePin(LED_PORT, pin, GPIO_PIN_SET); 
}

void LED_Off(uint16_t pin)
{
  HAL_GPIO_WritePin(LED_PORT, pin, GPIO_PIN_RESET); 
}

void LED_Toggle(uint16_t pin)
{
  HAL_GPIO_TogglePin(LED_PORT, pin);
}

