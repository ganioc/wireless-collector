#ifndef __MYLED_H
#define __MYLED_H

#include "gpio.h"

#define DELAY_NORMAL  500
#define DELAY_QUICK   60
#define DELAY_SLOW    1000
#define DELAY_FLASH   100

#define LED1                GPIO_PIN_6
#define LED2                GPIO_PIN_7
#define LED_PORT            GPIOA

#define  LED1_On() do{LED_On(LED1);}while(0)
#define  LED2_On() do{LED_On(LED2);}while(0)


#define  LED1_Off() do{LED_Off(LED1);}while(0)
#define  LED2_Off() do{LED_Off(LED2);}while(0)


#define  LED1_Toggle() do{LED_Toggle(LED1);}while(0)
#define  LED2_Toggle() do{LED_Toggle(LED2);}while(0)


#define  DEFAULT_KEY_PIN     GPIO_PIN_15
#define  DEFAULT_KEY_PORT   GPIOB

void LED_On(uint16_t pin);
void LED_Off(uint16_t pin);
void LED_Toggle(uint16_t pin);

void SetLED1Quick();
void SetLED1Slow();
void SetLED1Normal();
void  FlashLED2();

uint8_t bSetDefaultKey();

#endif