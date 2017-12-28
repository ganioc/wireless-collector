#ifndef __THREAD_LORA_H
#define __THREAD_LORA_H


#include "main.h"

#define LORA_STATE_RX_WAITING 0x01
#define LORA_STATE_RX_NONE       0x02

#define LORA_STATE_ROLE_MASTER  0x01
#define LORA_STATE_ROLE_SLAVE     0x02

void LoraThreadInit();

#endif