#include "thread_rs485.h"
#include "message.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "mye2prom.h"
#include "parser.h"

TaskThread_t mRs485Thread;
extern TaskThread_t mLoraThread;

uint32_t timeMark;

void (*handlerByteRs485)(uint8_t c);

uint8_t RX_BUF[BUFFER_MAX_SIZE_RS485];
static uint8_t THREAD_RX_BUF[BUFFER_MAX_SIZE_RS485];
uint8_t indexRx = 0;
static uint8_t indexThreadRx = 0;

static uint8_t mRxState= RX_STATE_ZERO;

static uint8_t mCounterWaiting = 0;




static void handleByteWaiting(uint8_t c)
{
    // ruff\r\n will make it into the next stage
    switch(mRxState)
    {
        case RX_STATE_ZERO :
            if(c == 'r')
            {
                mRxState = RX_STATE_HEAD;
                RX_BUF[indexRx++] = c;
            }
            else
            {
                indexRx = 0;
            }
            break;
        case RX_STATE_HEAD:

            if(indexRx == BUFFER_MAX_SIZE_RS485)
            {
                indexRx = 0;
                mRxState = RX_STATE_ZERO;
            }
            else if(c == '\r')
            {
                mRxState = RX_STATE_TAIL;
            }
            else
            {
                RX_BUF[indexRx++] = c;
            }

            break;
        case RX_STATE_BODY:
            break;
        case RX_STATE_TAIL:
            if(c == '\n')
            {
                RX_BUF[indexRx] = '\0';

                osSignalSet(mRs485Thread.idThread, 0x01);
            }
            else
            {
                indexRx = 0;
                mRxState = RX_STATE_ZERO;
            }
            break;
        default:
            break;
    }

}
static void handleByteConfig(uint8_t c)
{
    uint8_t i;

    switch(mRxState)
    {
        case CONFIG_STATE_ZERO:
            if(c == '\r' || c == '\n')
            {
                indexRx = 0;
            }
            else
            {
                RX_BUF[indexRx++] = c;
                mRxState = CONFIG_STATE_BODY;
            }
            break;

        case CONFIG_STATE_LAST_0:
            if(c == '\n')
            {

                // copy buffer to thread buffer
                indexThreadRx = indexRx;
                for(i = 0; i< indexThreadRx; i++)
                {
                    THREAD_RX_BUF[i] = RX_BUF[i];
                }
                THREAD_RX_BUF[i] = '\0';
                // signal
                osSignalSet(mRs485Thread.idThread, 0x01);
            }
            mRxState = CONFIG_STATE_ZERO;
            indexRx = 0;

            break;
        case CONFIG_STATE_LAST_1:
            break;
        case CONFIG_STATE_BODY:
            if(c == '\r')
            {
                mRxState = CONFIG_STATE_LAST_0;
            }
            else if(c == '\n')
            {
                indexRx = 0;
                mRxState = CONFIG_STATE_ZERO;
            }
            else
            {
                RX_BUF[indexRx++] = c;
            }

            break;
        default:
            break;
    }

}
void ResetRS485RxBuffer()
{
    indexRx = 0;
}
void AppendZeroRxBuffer()
{
    RX_BUF[indexRx] = '\0';
}
/**
* 收到发来的数据，就把数据发给lorathread线程
**/
static void handleByteWorkingForMaster(uint8_t c)
{
    RX_BUF[indexRx++] = c;
    osSignalSet(mLoraThread.idThread, 0x01);
}

static void handleByteWorkingForSlave(uint8_t c)
{
//    RX_BUF[0] = c;
//    osSignalSet(mLoraThread.idThread, 0x01);
}

static void handleByte(uint8_t c)
{
    if(mRs485Thread.state == STATE_WAITING)
    {
        handleByteWaiting(c);
    }
    else if(mRs485Thread.state == STATE_CONFIG)
    {
        handleByteConfig(c);
    }
    else if(mRs485Thread.state == STATE_WORKING_MASTER)
    {
        handleByteWorkingForMaster(c);
    }
    else if(mRs485Thread.state == STATE_WORKING_SLAVE)
    {
        handleByteWorkingForSlave(c);
    }

}
static void switchToConfig()
{

    indexRx=0;
    mRs485Thread.state = STATE_CONFIG;
    mRxState = CONFIG_STATE_ZERO;

}
static void switchToWorking()
{
    SysInfo_t *pSysInfo ;

    indexRx=0;

    printf("Go to working state ==>\r\n");

    pSysInfo = getSysInfoPointer();
    //printf("role:%d\n", pSysInfo->role);

    if(pSysInfo->role == ROLE_MASTER)
    {
        mRs485Thread.state = STATE_WORKING_MASTER;
        mRxState = MASTER_STATE_ZERO;
        printf("Role master\r\n");

        // trigure loraThread
        osSignalSet(mLoraThread.idThread, 0x01);
    }
    else if(pSysInfo->role == ROLE_SLAVE)
    {
        mRs485Thread.state = STATE_WORKING_SLAVE;
        mRxState = SLAVE_STATE_ZERO;
        printf("Role slave\r\n");

        // trigure lorathread
        osSignalSet(mLoraThread.idThread, 0x01);
    }
    else
    {
        printf("unrecognized role\r\n");
    }
}
static  void TaskHandlerConfig(osEvent ret)
{
    if(ret.status ==  osEventSignal && ret.value.v == 1)
    {
        printf("%d:%s\r\n", strlen((char*)THREAD_RX_BUF),THREAD_RX_BUF);

        parseConfig((char*)THREAD_RX_BUF, strlen((char*)THREAD_RX_BUF));

    }
    else if(ret.status ==  osEventSignal && ret.value.v == 2)
    {
        switchToWorking();
    }
}
static  void TaskHandlerWorkingMaster(osEvent ret)
{
    if(ret.status ==  osEventSignal)
    {
        ;

    }
}
static  void TaskHandlerWorkingSlave(osEvent ret)
{

    if(ret.status ==  osEventSignal)
    {
        ;

    }
    else if(ret.status == osEventTimeout)
    {
        ;
    }
}
static  void TaskHandlerWaiting(osEvent ret)
{

    if(ret.status ==  osEventSignal)
    {
        printf("Should go to Config state\r\n");
        printf("%s is a match\r\n", RX_BUF);

        UART3_Transmit("OK\r\n", 4);

        // otherwise it will switch to the config state
        switchToConfig();

    }
    else if(ret.status == osEventTimeout)
    {
        printf("timeout %d\r\n", mCounterWaiting++);
    }

    // after 10 seconds, it will switch to the Working state
    if(mCounterWaiting == 10)
    {
        switchToWorking();
    }
}
static void TaskLoop(void const * argument)
{

    osEvent ret;

    printf("mRs485Thread taskloop started\r\n");

    mRs485Thread.state = STATE_WAITING;
    handlerByteRs485 = handleByte;

    printf("mRS485Thread in wating state\r\n");

    mCounterWaiting = 0;

    // Triger uart3 receive
    UART3_Receive();


    while(1)
    {
        if(mRs485Thread.state == STATE_CONFIG)
        {
            ret = osSignalWait(0x3, 1000);
            TaskHandlerConfig(ret);
        }
        else if(mRs485Thread.state == STATE_WORKING_MASTER)
        {
            ret = osSignalWait(0x3, 100);
            TaskHandlerWorkingMaster(ret);
        }
        else if(mRs485Thread.state == STATE_WORKING_SLAVE)
        {
            ret = osSignalWait(0x3, 100);
            TaskHandlerWorkingSlave(ret);

        }
        else if(mRs485Thread.state == STATE_WAITING)
        {
            ret = osSignalWait(0x3, 1000);
            TaskHandlerWaiting(ret);
        }
    }
}

void Rs485ThreadInit()
{

    osThreadDef(rs485thread, TaskLoop, osPriorityHigh, 0, 256);
    mRs485Thread.idThread = osThreadCreate(osThread(rs485thread), NULL);

    if(mRs485Thread.idThread == NULL)
    {

        printf("rs485thread create fail\r\n");
    }
    else
    {
        printf("rs485thread create OK\r\n");
    }


}

