/**
  ******************************************************************************
  * File Name          : main.h
  * Description        : This file contains the common defines of the application
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
  /* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
/* USER CODE BEGIN Includes */

#define HADRWIRED_VERSION  "1.02"

#define MY_VERSION "1.01"
#define MY_MODEL    "WLScanner"

#define SPED_SERIAL_8N1                            (0x00<<6)

#define SPED_SERIAL_BAUDRATE_1200       (0x0 << 3)
#define SPED_SERIAL_BAUDRATE_2400       (0x1 << 3)
#define SPED_SERIAL_BAUDRATE_4800       (0x2 << 3)
#define SPED_SERIAL_BAUDRATE_9600       (0x3 << 3)
#define SPED_SERIAL_BAUDRATE_19200       (0x4 << 3)
#define SPED_SERIAL_BAUDRATE_38400       (0x5 << 3)
#define SPED_SERIAL_BAUDRATE_57600       (0x6 << 3)
#define SPED_SERIAL_BAUDRATE_115200       (0x7 << 3)

#define SPED_BPS_0K3                  (0x0)
#define SPED_BPS_1K2                  (0x1)
#define SPED_BPS_2K4                  (0x2)
#define SPED_BPS_4K8                  (0x3)
#define SPED_BPS_9K6                  (0x4)
#define SPED_BPS_192                  (0x5)

#define OPTION_MODE_TRANSPARENT             (0x0<< 7)
#define OPTION_MODE_NON_TRANSPARENT    (0x1<< 7)

#define OPTION_IO_MODE_ONE                                (0x1 << 6)
#define OPTION_IO_MODE_ZERO                              (0x0 << 6)

#define OPTION_WAKEUP_250                (0x0 <<3)
#define OPTION_WAKEUP_500                (0x1 <<3)
#define OPTION_WAKEUP_750                (0x2 <<3)
#define OPTION_WAKEUP_1000                (0x3 <<3)
#define OPTION_WAKEUP_1250                (0x4 <<3)
#define OPTION_WAKEUP_1500                (0x5 <<3)
#define OPTION_WAKEUP_1750                (0x6 <<3)
#define OPTION_WAKEUP_2000                (0x7 <<3)

#define OPTION_FEC_ON                 (0x1 <<2)
#define OPTION_FEC_OFF               (0x0<<2)

#define OPTION_PWR_20     (0x0)
#define OPTION_PWR_17     (0x01)
#define OPTION_PWR_14     (0x02)

#define ROLE_MASTER   0x1
#define ROLE_SLAVE      0x0

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
  * @}
  */ 

/**
  * @}
*/ 

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
