#include "ctrllora.h"
#include "gpio.h"
#include "usart.h"
#include "mye2prom.h"
#include "frame.h"

uint8_t buf[256];

void SetLoraSettingMode(){

    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M0, GPIO_PIN_SET); 
    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M1, GPIO_PIN_SET); 
}

void SetLoraWorkingMode(){

    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M0, GPIO_PIN_RESET); 
    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M1, GPIO_PIN_RESET); 
}

void WriteLora(uint8_t *buf, uint8_t len){
    uint8_t i;
    printf("writeLora len:%d\r\n", len);

    for(i=0; i< len; i++){
        printf("%02d: 0x%02x\r\n", i,  buf[i]);
    }
    
    UART2_Transmit(buf, len);
}
void WriteLoraConfig(uint8_t *buf, uint8_t len){

    

}

void WriteLoraData(uint8_t *buf, uint8_t len){



}

void SendOutLoraData(uint16_t addr,uint8_t * inBuf, uint8_t inLen){
        SysInfo_t  *pSysInfo =getSysInfoPointer();
        uint8_t channel = pSysInfo->chan;

        uint8_t index = 0;
        uint8_t i;

        printf("Send to %d at %d\r\n", addr, channel);
        buf[index++] = 0xff&(addr>>8);
        buf[index++] = 0xff &(addr);
        buf[index++] = channel;

        buf[index++] = FRAME_HEAD;
        buf[index++] = pSysInfo->addrH;
        buf[index++] = pSysInfo->addrL;
        buf[index++] = 0xff&(addr>>8);
        buf[index++] = 0xff &(addr);
        buf[index++] = 0x00;
        buf[index++] = inLen;

        for(i=0;i <inLen; i++){
            buf[index++] = inBuf[i];
        }
        buf[index++] = FRAME_TAIL_0;
        buf[index++] = FRAME_TAIL_1;

        WriteLora( buf, index);

}