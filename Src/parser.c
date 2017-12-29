#include "parser.h"
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "thread_rs485.h"
#include "message.h"
#include "mye2prom.h"
#include "usart.h"
#include <stdlib.h>

extern TaskThread_t mRs485Thread;


void parseConfigSet(char* str, uint8_t len)
{
    char strObj[5];
    char strContent[16];
    uint8_t i, j=0;
    SysInfo_t *pSysInfo ;
    uint16_t addr16, channel;

    for(i=0; i< 4; i++)
    {
        strObj[i] = str[i];
    }
    strObj[i] = '\0';

    for(; i< len; i++)
    {
        strContent[j++] = str[i];
    }
    strContent[j] = '\0';

    if(strcmp(strObj, "QUIT") == 0)
    {
        printf("Quit config\r\n");
        osSignalSet(mRs485Thread.idThread, 0x02);

    }
    else if(strcmp(strObj, "ADDR") == 0)
    {
        printf("address config\r\n");
        
        if(strlen(strContent) > 3 ||strlen(strContent) == 0 ){
            printf("wrong format of addr: %d\r\n", strlen(strContent));
        }else{
            // save addr
            // strContent to number
            addr16 = atoi((char*)strContent);
            
            pSysInfo = getSysInfoPointer();
            pSysInfo->addrH= 0xff&(addr16>>8);
            pSysInfo->addrL = 0xff&addr16;
            
            printf("Set addr to:0x%x%x\r\n", 
                        pSysInfo->addrH,
                        pSysInfo->addrL);
        }
        
    }
    else if(strcmp(strObj, "CHAN") == 0)
    {
        printf("channel config\r\n");
        if(strlen(strContent) > 3 ||strlen(strContent) == 0 ){
            printf("wrong format of chan: %d\r\n", strlen(strContent));
        }else{
            // save addr
            pSysInfo = getSysInfoPointer();
            channel = atoi((char*)strContent);
            pSysInfo->chan= channel & 0xff;

            printf("Set channel to:0x%x\r\n", 
                        pSysInfo->chan);
        }

    }
    else if(strcmp(strObj, "SAVE") == 0)
    {
        printf("save to e2prom config\r\n");
        saveSysInfoPointer();

    }
    else if(strcmp(strObj, "MAST") == 0)
    {
        printf("set to master config\r\n");
        pSysInfo = getSysInfoPointer();
        pSysInfo->role = ROLE_MASTER;

    }
    else if(strcmp(strObj, "SLAV") == 0)
    {
        printf("set to slave config\r\n");
        pSysInfo = getSysInfoPointer();
        pSysInfo->role = ROLE_SLAVE;
    }
    else{

        printf("Unrecognized configset cmd\r\n%s\r\n", strObj);
    }
}
void parseConfigRead(char* str, uint8_t len)
{
    char strObj[5];
    char strBuf[64];
    uint8_t i;
    SysInfo_t *pSysInfo ;

    for(i=0; i< 4; i++)
    {
        strObj[i] = str[i];
    }
    strObj[i] = '\0';

    if(strcmp(strObj, "VER*") == 0)
    {


    }
    else if(strcmp(strObj, "MODL") == 0)
    {


    }
    else if(strcmp(strObj, "SYS*") == 0)
    {

        printf("read sys info\r\n");

    
        pSysInfo = getSysInfoPointer();
        sprintf(strBuf,"addr:0x%x%x\r\n", pSysInfo->addrH, pSysInfo->addrL);
        UART3_Transmit((uint8_t*)strBuf, strlen(strBuf));
        sprintf(strBuf,"chan:0x%x\r\n", pSysInfo->chan);
        UART3_Transmit((uint8_t*)strBuf, strlen(strBuf));
        sprintf(strBuf,"role:0x%x\r\n", pSysInfo->role);
        UART3_Transmit((uint8_t*)strBuf, strlen(strBuf));        
    }
    else{

        printf("Unrecognized configread cmd \n%s\r\n", strObj);
    }
}
void parseConfig(char* str, uint8_t len)
{

    char strType[5];
    char strObj[32];
    uint8_t i, j=0;

    for(i=0; i<4; i++)
    {
        strType[i] = str[i];
    }
    strType[i] = '\0';

    for(i; i<len; i++)
    {
        strObj[j++] = str[i];
    }
    strObj[j] = '\0';

    if(strcmp(strType, "SET*")==0)
    {
        parseConfigSet(strObj, strlen(strObj));
        
    }
    else if(strcmp(strType, "READ")==0)
    {
        parseConfigRead(strObj, strlen(strObj));
    }
    else
    {
        printf("Unrecognized config cmd:%s\r\n", str);
    }
}



