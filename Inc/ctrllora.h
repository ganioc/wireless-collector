#ifndef __CTRLLORA_H
#define __CTRLLORA_H

#include "main.h"

#define LORA_M0                GPIO_PIN_4
#define LORA_M1                GPIO_PIN_5
#define LORA_M_PORT            GPIOA

#define MAX_DATA_LENGTH   58

void SetLoraSettingMode();

void SetLoraWorkingMode();

void WriteLoraConfig(uint8_t *buf, uint8_t len);

void WriteLoraData(uint8_t *buf, uint8_t len);

void WriteLora(uint8_t *buf, uint8_t len);

void SendOutLoraData(uint16_t addr,  uint8_t * buf, uint8_t len);

void SendOutRs485Data(uint8_t * buf, uint16_t  len);

#endif
