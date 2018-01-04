#include "thread_rs485.h"
#include "message.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "mye2prom.h"
#include "parser.h"
#include "myled.h"
#include "ctrllora.h"

TaskThread_t mRs485Thread;
extern TaskThread_t mLoraThread;

extern uint16_t  addr16LastTime;

uint32_t timeMark;

void (*handlerByteRs485)(uint8_t c);

uint8_t RX_BUF[BUFFER_MAX_SIZE_RS485];
static uint8_t THREAD_RX_BUF[BUFFER_MAX_SIZE_RS485];
uint16_t indexRx = 0;
static uint8_t indexThreadRx = 0;

static uint8_t mRxState= RX_STATE_ZERO;

static uint8_t mCounterWaiting = 0;

uint8_t mRs485State;


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

/**
* 收到发来的数据，就把数据发给lorathread线程
**/
static void handleByteWorking(uint8_t c)
{
    RX_BUF[indexRx++] = c;
    osSignalSet(mRs485Thread.idThread, 0x01);
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
    else if(mRs485Thread.state == STATE_WORKING_MASTER
            || mRs485Thread.state == STATE_WORKING_SLAVE)
    {
        handleByteWorking(c);
    }

}
static void switchToConfig()
{

    indexRx=0;
    mRs485Thread.state = STATE_CONFIG;
    mRxState = CONFIG_STATE_ZERO;

    SetLED1Slow();

}


static void switchToWorking()
{
    SysInfo_t *pSysInfo ;
    Rs485Info_t  *pRs485Info;

    SetLED1Normal();

    indexRx=0;

    printf("Go to working state ==>\r\n");

    pSysInfo = getSysInfoPointer();
    pRs485Info = getRs485InfoPointer();
    //printf("role:%d\n", pSysInfo->role);


    // set RS485 new baudrate

    if(pSysInfo->role == ROLE_MASTER)
    {
        mRs485Thread.state = STATE_WORKING_MASTER;
        mRxState = RS485_STATE_RX_NONE;
        printf("Role master\r\n");

        // trigure loraThread
        osSignalSet(mLoraThread.idThread, 0x01);
    }
    else if(pSysInfo->role == ROLE_SLAVE)
    {

        //printf("Set RS485 baudrate\r\n");

        //SetRs485WorkingBaudrate(pRs485Info);


        mRs485Thread.state = STATE_WORKING_SLAVE;
        mRxState = RS485_STATE_RX_NONE;
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
static void TaskHandlerWorking(osEvent ret)
{

    uint16_t i;
    uint16_t addr16;

    if(ret.status ==  osEventSignal&& ret.value.v == 1&& mRs485State== RS485_STATE_RX_NONE)
    {
        mRs485State = RS485_STATE_RX_WAITING;

    }
    else if(ret.status == osEventTimeout && mRs485State == RS485_STATE_RX_WAITING)
    {
        // frame ended
        // send it out

        printf("master get data from RS485, len:%d\r\n",indexRx);

        for(i=0; i< indexRx; i++)
        {
            printf("0x%02x\r\n", RX_BUF[i]);
        }

        if(mRs485Thread.state == STATE_WORKING_MASTER)  // it is a master
        {
            // addr16 = 0x00;
            printf("Master: %d\r\n", RX_BUF[0]);
            addr16 = RX_BUF[0]; // get the address 1byte from the modbus
        }
        else   // it is a slave
        {
            printf("Slave:%d \r\n", addr16LastTime);
            addr16 = addr16LastTime; // get the address from the loraThread
        }

        if(indexRx <= 2)
        {
            printf("too few bytes\r\n");

        }
        else
        {
            printf("Print out addr:%x\r\n", addr16);
            SendOutLoraData(addr16, RX_BUF, indexRx);
        }

        indexRx = 0;

        mRs485State = RS485_STATE_RX_NONE;

        // toggle led2
        FlashLED2();

    }
    else if(ret.status == osEventTimeout && mRs485State == RS485_STATE_RX_NONE)
    {

    }

}

static  void TaskHandlerWaiting(osEvent ret)
{

    if(ret.status ==  osEventSignal)
    {
        printf("Should go to Config state\r\n");
        printf("%s is a match, now in Config state\r\n", RX_BUF);

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

    mRs485State = RS485_STATE_RX_NONE;


    while(1)
    {
        if(mRs485Thread.state == STATE_CONFIG)
        {
            ret = osSignalWait(0x3, 1000);
            TaskHandlerConfig(ret);
        }
        else if(mRs485Thread.state == STATE_WORKING_MASTER)
        {
            ret = osSignalWait(0x3, 15);
            TaskHandlerWorking(ret);
        }
        else if(mRs485Thread.state == STATE_WORKING_SLAVE)
        {
            ret = osSignalWait(0x3, 15);
            TaskHandlerWorking(ret);

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

