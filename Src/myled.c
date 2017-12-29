#include "myled.h"
#include "cmsis_os.h"

uint16_t mDelayPeriod = DELAY_QUICK;

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

void SetLED1Quick(){
    mDelayPeriod = DELAY_QUICK;
}
void SetLED1Slow(){
    mDelayPeriod = DELAY_SLOW;
}
void SetLED1Normal(){
    mDelayPeriod = DELAY_NORMAL;
}

void  FlashLED2(){
            
            LED2_Toggle();
            osDelay( DELAY_FLASH);
            LED2_Toggle();
//            osDelay( DELAY_FLASH);
//            LED2_Toggle();
//            osDelay( DELAY_FLASH);
//            LED2_Toggle();
}

uint8_t bSetDefaultKey(){
    GPIO_PinState state = HAL_GPIO_ReadPin(DEFAULT_KEY_PORT, DEFAULT_KEY_PIN);

    if(state == GPIO_PIN_SET){
        return 1;
    }else{
        return 0;
    }
}

