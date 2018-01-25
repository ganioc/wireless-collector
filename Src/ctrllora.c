#include "ctrllora.h"
#include "gpio.h"
#include "usart.h"
#include "mye2prom.h"
#include "frame.h"
#include "cmsis_os.h"
#include "encrypt.h"

uint8_t buf[512];
uint8_t headerBuf[128];

void SetLoraSettingMode()
{

    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M1, GPIO_PIN_SET);
}

void SetLoraWorkingMode()
{

    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LORA_M_PORT, LORA_M1, GPIO_PIN_RESET);
}

void WriteLora(uint8_t *buf, uint8_t len)
{
    uint8_t i;
    printf("writeLora len:%d\r\n", len);

    for(i=0; i< len; i++)
    {
        //printf("%02d: 0x%02x\r\n", i,  buf[i]);
    }

    UART2_Transmit(buf, len);
}
void WriteLoraConfig(uint8_t *buf, uint8_t len)
{



}

void WriteLoraData(uint8_t *buf, uint8_t len)
{



}

void SendOutLoraData(uint16_t addr,uint8_t * inBuf, uint8_t inLen)
{
    SysInfo_t  *pSysInfo =getSysInfoPointer();
    uint8_t channel = pSysInfo->chan;
    uint16_t index = 0;
    uint16_t i,j,nTemp, nTotal;

    printf("SendOut Lora data  to addr: %d at channel %d\r\n", addr, channel);

    buf[index++] = FRAME_HEAD;
    buf[index++] = pSysInfo->addrH;
    buf[index++] = pSysInfo->addrL;
    buf[index++] = 0xff&(addr>>8);
    buf[index++] = 0xff &(addr);
    buf[index++] = 0x00;
    buf[index++] = inLen;

    for(i=0; i <inLen; i++)
    {
        buf[index++] = inBuf[i];
    }
    buf[index++] = FRAME_TAIL_0;
    buf[index++] = FRAME_TAIL_1;

    nTotal = index;

    for(i=0; i< nTotal; i++)
    {
        // printf("%2d: %02x\r\n", i, buf[i]);
    }

    // encrypt buf
    if(isUseEncrypt() == 1)
    {
        nTotal = encrypt((char *)buf, nTotal);
    }


    headerBuf[0] = 0xff&(addr>>8);
    headerBuf[1] = 0xff &(addr);
    headerBuf[2] = channel;

    printf("total length:%d\r\n", nTotal);

    for(i = 0; i< nTotal; i+= MAX_DATA_LENGTH)
    {

        nTemp = (nTotal< i + MAX_DATA_LENGTH) ? nTotal: i+ MAX_DATA_LENGTH;
        printf("i:%d, nTemp:%d\r\n", i, nTemp);

        for(j = 0; j< nTemp - i; j++)
        {

            headerBuf[3+j] = buf[i+j];
        }
        WriteLora(headerBuf, nTemp - i +3);

        osDelay(10);
    }

}

void SendOutRs485Data(uint8_t * buf, uint16_t len)
{
    uint16_t i;

    printf("Sendout to RS485: len: %d\r\n", len);

    for(i=0; i< len; i++)
    {
        //printf("%02d: %02x\r\n", i, buf[i]);
    }

    // find out the data
    // decrypt it
    if(isUseEncrypt() == 1)
    {
        len = decrypt((char*)buf, len);
    }


    UART3_Transmit(buf,  len);

}

