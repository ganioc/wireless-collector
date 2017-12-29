#include "thread_lora.h"
#include "thread_rs485.h"
#include "message.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "mye2prom.h"
#include "parser.h"
#include "ctrllora.h"
#include "thread_rs485.h"
#include "myled.h"
#include "cmsis_os.h"

TaskThread_t mLoraThread;
extern  uint8_t RX_BUF[];
extern  uint8_t indexRx;

void (*handlerByteLora)(uint8_t c);

uint8_t RX_BUF_LORA[BUFFER_MAX_SIZE_RS485];
uint8_t indexRxLora = 0;

// packet storage for lora data
uint8_t RX_BUF_FOR_RS485[BUFFER_MAX_SIZE_RS485];
uint8_t indexRxForRs485 = 0;

uint8_t mLoraState;

uint16_t  addr16LastTime;

void ConfigLora()
{
    uint8_t params[6];
    SysInfo_t *pSysInfo ;

    pSysInfo = getSysInfoPointer();

    params[0] = 0xC0;
    params[1] = pSysInfo->addrH;
    params[2] = pSysInfo->addrL;
    params[3] = pSysInfo->sped;
    params[4] = pSysInfo->chan;
    params[5] = pSysInfo->option;


    WriteLora(params, 6);

    printf("config data:0x%02x %02x %02x %02x %02x %02x\r\n",  \
           params[0], params[1], params[2], params[3], params[4], params[5]);

}
static void loraHandleByte(uint8_t c)
{
    RX_BUF_LORA[indexRxLora++] = c;
    osSignalSet(mLoraThread.idThread, 0x01);

}
uint8_t bExactLen(uint8_t lenH, uint8_t lenL, uint8_t lenPacket)
{
    if((lenH<<8 | lenL) == lenPacket)
    {
        return 1;
    }

    return 0;
}
static void HandleLoraBytes(uint8_t * inBuf, uint8_t inLen)
{
    uint8_t i;
    uint16_t len16;
    
    if(inLen <= MAX_DATA_LENGTH
       && inLen > 9
       && inBuf[0] == 0x3a
       && inBuf[inLen-1] == 0x0a
       && inBuf[inLen-2]== 0x0d
       && bExactLen(inBuf[5], inBuf[6], inLen - 9))  // Whole packet
    {

        if(mLoraThread.state == LORA_STATE_ROLE_SLAVE)
        {
            addr16LastTime = inBuf[1]<< 8| inBuf[2];
        }

        SendOutRs485Data(inBuf + 7,  inBuf[5]<< 8| inBuf[6]);

    }
    else if(inLen < MAX_DATA_LENGTH)   // Last packet
    {
        for(i=0; i< inLen; i++){
            RX_BUF_FOR_RS485[indexRxForRs485++]=inBuf[i];
        }
        len16 = RX_BUF_FOR_RS485[5]<< 8| RX_BUF_FOR_RS485[6];
        
        SendOutRs485Data(RX_BUF_FOR_RS485+7, len16);

        indexRxForRs485 = 0;
    }
    else if(inLen == MAX_DATA_LENGTH
        && inBuf[0] == 0x3a
        && inBuf[1] == 0x00)   // First packet 
    {
        if(mLoraThread.state == LORA_STATE_ROLE_SLAVE)
        {
            addr16LastTime = inBuf[1]<< 8| inBuf[2];
        }
        indexRxForRs485 =0;
        
        for(i=0; i< MAX_DATA_LENGTH; i++){
            RX_BUF_FOR_RS485[indexRxForRs485++]=inBuf[i];
        }

    }else if(inLen == MAX_DATA_LENGTH) // Middle packet
    {
        for(i=0; i< MAX_DATA_LENGTH; i++){
            RX_BUF_FOR_RS485[indexRxForRs485++]=inBuf[i];
        }
    }

}
static void TaskLoopRx()
{

    osEvent ret;
    uint16_t i;

    while(1)
    {
        // waiting for the comming characters
        ret = osSignalWait(0x3, 10);

        if(ret.status ==  osEventSignal && ret.value.v == 1&& mLoraState  == LORA_STATE_RX_NONE)
        {
            //printf("lorathread signal 1 rx\r\n");
            mLoraState = LORA_STATE_RX_WAITING;

        }
        else if(ret.status ==  osEventSignal && ret.value.v == 2)
        {
            printf("lorathread signal 2 rx\r\n");
        }
        else if(ret.status == osEventTimeout && mLoraState == LORA_STATE_RX_WAITING)
        {
            // frame ended
            // send it out

            printf("Master get data from Lora, len:%d\r\n",indexRxLora);

            for(i=0; i< indexRxLora; i++)
            {
                printf("0x%02x\r\n", RX_BUF_LORA[i]);
            }

            // send it to RS485 port
            if(indexRxLora < 4)
            {
                printf("too few bytes\r\n");
            }
            else
            {
                HandleLoraBytes(RX_BUF_LORA, indexRxLora);
            }

            // Housekeeping
            indexRxLora = 0;

            mLoraState = LORA_STATE_RX_NONE;

            // toggle led2
            FlashLED2();

        }
        else if(ret.status == osEventTimeout && mLoraState == LORA_STATE_RX_NONE)
        {

        }
    }
}

static void TaskLoop(void const * argument)
{
    osEvent ret;
    printf("loraThread taskloop started\r\n");

    ret = osSignalWait(0x3, osWaitForever);


    if(ret.status == osEventSignal)
    {
        printf("lora thread received signal\r\n");
    }


    printf("loraThread start task loop\r\n");

    // init Lora module
    // c2 + 5字节工作参数
    SetLoraSettingMode();
    osDelay(50);
    ConfigLora();

    osDelay(50);

    SetLoraWorkingMode();

    // open serial 2 rx
    handlerByteLora = loraHandleByte;
    UART2_Receive();

    mLoraState = LORA_STATE_RX_NONE;

    if(getSysInfoRole() == 1)
    {
        mLoraThread.state = LORA_STATE_ROLE_MASTER;
    }
    else // if it's a slave
    {
        mLoraThread.state= LORA_STATE_ROLE_SLAVE;
    }

    TaskLoopRx();

}

void LoraThreadInit()
{

    osThreadDef(lorathread, TaskLoop, osPriorityHigh, 0, 128);
    mLoraThread.idThread = osThreadCreate(osThread(lorathread), NULL);

    if(mLoraThread.idThread == NULL)
    {

        printf("lorathread create fail\r\n");
    }
    else
    {
        printf("lorathread create OK\r\n");
    }
}

