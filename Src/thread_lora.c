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

TaskThread_t mLoraThread;
extern  uint8_t RX_BUF[];
extern  uint8_t indexRx;

void (*handlerByteLora)(uint8_t c);

uint8_t RX_BUF_LORA[BUFFER_MAX_SIZE_RS485];
uint8_t indexRxLora = 0;

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
static void handleByte(uint8_t c)
{
    RX_BUF_LORA[indexRxLora++] = c;
    osSignalSet(mLoraThread.idThread, 0x02);

}
static void TaskLoopMaster()
{
    osEvent ret;        
    uint16_t addr16 = 0, i;
    
    while(1)
    {
        // waiting for the comming characters
        ret = osSignalWait(0x3, 100);

        if(ret.status ==  osEventSignal && ret.value.v == 1&& mLoraThread.state
           == LORA_STATE_RX_NONE)
        {
            //printf("lorathread signal 1 rx\r\n");
            mLoraThread.state = LORA_STATE_RX_WAITING;

        }
        else if(ret.status ==  osEventSignal && ret.value.v == 2)
        {
            printf("lorathread signal 2 rx\r\n");
        }
        else if(ret.status == osEventTimeout && mLoraThread.state == LORA_STATE_RX_WAITING)
        {
            // frame ended
            // send it out

            printf("len:%d\r\n",indexRx);
            
            for(i=0; i< indexRx; i++)
            {
                printf("0x%02x\r\n", RX_BUF[i]);
            }
            addr16 = 0x00;
            addr16 |= RX_BUF[0]; // get the address 1byte from the modbus

            SendOutLoraData(addr16, RX_BUF, indexRx);

            ResetRS485RxBuffer();

            mLoraThread.state = LORA_STATE_RX_NONE;

            // toggle led2
            LED2_Toggle();
            osDelay( 50);
            LED2_Toggle();
            osDelay( 50);
            LED2_Toggle();
            osDelay( 50);
            LED2_Toggle();
            
        }
        else if(ret.status == osEventTimeout && mLoraThread.state == LORA_STATE_RX_NONE)
        {

        }
    }
}
static void TaskLoopSlave()
{
    osEvent ret;
    while(1)
    {
// waiting for the comming characters
        ret = osSignalWait(0x3, 50);

    }
}
static void TaskLoop(void const * argument)
{
    osEvent ret;
    
    printf("loraThread taskloop started\r\n");

    ret = osSignalWait(0x3, osWaitForever);


    printf("loraThread start task loop\r\n");

    // init Lora module
    // c2 + 5字节工作参数
    SetLoraSettingMode();
    osDelay(50);
    ConfigLora();

    osDelay(50);

    SetLoraWorkingMode();

    // open serial 2 rx
    handlerByteLora = handleByte;
    UART2_Receive();

    mLoraThread.state = LORA_STATE_RX_NONE;

    if(getSysInfoRole() == 1)
    {
        TaskLoopMaster();
    }
    else // if it's a slave
    {
        TaskLoopSlave();
    }

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

