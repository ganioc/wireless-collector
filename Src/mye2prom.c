#include "mye2prom.h"
#include <string.h>
#include "thread_rs485.h"

extern I2C_HandleTypeDef hi2c1;

uint8_t   mbrInfo[MBRINFO_SIZE];
uint8_t   sysInfo[SYSINFO_SIZE];
uint8_t   rs485Info[RS485INFO_SIZE];
uint8_t   advanceInfo[ADVANCEINFO_SIZE];


void E2PROM_Read(uint8_t section, uint8_t page, uint8_t * data, uint8_t len)
{
    HAL_StatusTypeDef fb;
    uint16_t memaddr;

    memaddr = 32*(8*section + page);

    do
    {
        printf("E2PROM read\n");
        fb = HAL_I2C_Mem_Read(&hi2c1, E2PROM_ADDR, memaddr, I2C_MEMADD_SIZE_16BIT, data,   len, E2PROM_READ_WAIT);

    }
    while(fb != HAL_OK);

}
void E2PROM_Write(uint8_t section, uint8_t page, uint8_t * data, uint8_t len)
{
    HAL_StatusTypeDef fb;
    uint16_t memaddr;

    memaddr = 32*(8*section + page);

    do
    {
        printf("E2PROM write\n");
        fb = HAL_I2C_Mem_Write(&hi2c1, E2PROM_ADDR, memaddr, I2C_MEMADD_SIZE_16BIT, data, len, E2PROM_WRITE_WAIT);

    }
    while(fb != HAL_OK);

}

void Get_MBR(uint8_t * data, uint8_t len)
{
    E2PROM_Read(SECTION_MBRINFO, PAGE_MBRINFO,  data, len);
}
void Set_MBR(uint8_t * data, uint8_t len)
{
    E2PROM_Write(SECTION_MBRINFO, PAGE_MBRINFO,  data, len);
}
void Get_SysInfo(uint8_t * data, uint8_t len)
{
    E2PROM_Read(SECTION_SYSINFO, PAGE_SYSINFO,  data, len);
}
void Set_SysInfo(uint8_t * data, uint8_t len)
{
    E2PROM_Write(SECTION_SYSINFO, PAGE_SYSINFO,  data, len);
}
void Get_Rs485Info(uint8_t * data, uint8_t len)
{
    E2PROM_Read(SECTION_RS485INFO, PAGE_RS485INFO,  data, len);
}
void Set_Rs485Info(uint8_t * data, uint8_t len)
{
    E2PROM_Write(SECTION_RS485INFO, PAGE_RS485INFO,  data, len);
}
void Get_AdvanceInfo(uint8_t * data, uint8_t len){
    E2PROM_Read(SECTION_ADVANCEINFO, PAGE_ADVANCEINFO,  data, len);
}
void Set_AdvanceInfo(uint8_t * data, uint8_t len){
    E2PROM_Write(SECTION_ADVANCEINFO, PAGE_ADVANCEINFO,  data, len);
}


void ResetToDefaultE2Prom()
{

    MBRInfo_t     *pMBRInfo;
    SysInfo_t      *pSysInfo;
    Rs485Info_t  *pRs485Info;
    AdvanceInfo_t *pAdvanceInfo;
    char strTemp[32];
    uint8_t i;
    
    pMBRInfo = (MBRInfo_t *)mbrInfo;
    pSysInfo = (SysInfo_t *)sysInfo;
    pRs485Info = (Rs485Info_t  *)rs485Info;
    pAdvanceInfo = (AdvanceInfo_t*)advanceInfo;

    printf("E2PROM not initiated, set to default value\n");
    pMBRInfo->mark1 = 'r';
    pMBRInfo->mark2 = 'u';
    pMBRInfo->mark3 = 'f';
    pMBRInfo->mark4 = 'f';

    // Default setting,create the default data value
    pSysInfo->addrH = 0x00;
    pSysInfo->addrL = 0x01;
    pSysInfo->chan = 0x00; // channel 1  ,  0 ~ 1F
    pSysInfo->sped = SPED_SERIAL_8N1 \
                     | SPED_SERIAL_BAUDRATE_9600 \
                     | SPED_BPS_2K4;
    pSysInfo->option = OPTION_MODE_NON_TRANSPARENT\
                       |OPTION_IO_MODE_ZERO\
                       | OPTION_WAKEUP_250\
                       | OPTION_FEC_ON \
                       | OPTION_PWR_20;

    pSysInfo->role = ROLE_SLAVE;  // 1 is master


    sprintf(strTemp, "%s", MY_MODEL);

    for(i = 0; i< strlen(strTemp); i++){
        pSysInfo->model[i] = strTemp[i];

    }
    pSysInfo->model[i] = '\0';
    
    sprintf(strTemp, "%s", MY_VERSION);

    for(i = 0; i< strlen(strTemp); i++){
        pSysInfo->version[i] = strTemp[i];

    }
    pSysInfo->version[i] = '\0';

    pRs485Info->baudRate = RS485_BAUDRATE_9600;  // my own definition
    pRs485Info->parity = RS485_PARITY_NONE;
    pRs485Info->stopBit = RS485_STOP_BITS_1;

    pAdvanceInfo->packetDelayH = 0;
    pAdvanceInfo->packetDelayL = 15;

    Set_MBR(mbrInfo, MBRINFO_SIZE);
    Set_SysInfo(sysInfo, SYSINFO_SIZE);
    Set_Rs485Info(rs485Info, RS485INFO_SIZE);
    Set_AdvanceInfo(advanceInfo, ADVANCEINFO_SIZE);

}
void E2PROM_Init(void)
{

    MBRInfo_t       *pMBRInfo;
    SysInfo_t        *pSysInfo;
    Rs485Info_t     *pRs485Info;
    AdvanceInfo_t  *pAdvanceInfo;
    
    uint16_t i;

    printf("E2PROM init");

    // check mark
    Get_MBR(mbrInfo, MBRINFO_SIZE);
    Get_SysInfo(sysInfo, SYSINFO_SIZE);
    Get_Rs485Info(rs485Info, RS485INFO_SIZE);
    Get_AdvanceInfo(advanceInfo, ADVANCEINFO_SIZE);

    pMBRInfo = (MBRInfo_t *)mbrInfo;
    pSysInfo = (SysInfo_t *)sysInfo;
    pRs485Info = (Rs485Info_t *)rs485Info;
    pAdvanceInfo = (AdvanceInfo_t*)advanceInfo;

    // OK  , read out the data
    if(pMBRInfo->mark1 == 'r'
       &&pMBRInfo->mark2 == 'u'
       &&pMBRInfo->mark3 == 'f'
       &&pMBRInfo->mark4 == 'f')
    {
        // already configured
        printf("E2PROM already inited");
    }
    else
    {
        printf("E2PROM not initiated\r\n");

        ResetToDefaultE2Prom();

        Get_MBR(mbrInfo, MBRINFO_SIZE);
        Get_SysInfo(sysInfo, SYSINFO_SIZE);
        Get_Rs485Info(rs485Info, RS485INFO_SIZE);
        Get_AdvanceInfo(advanceInfo, ADVANCEINFO_SIZE);

        pMBRInfo = (MBRInfo_t *)mbrInfo;
        pSysInfo = (SysInfo_t *)sysInfo;
        pRs485Info = (Rs485Info_t *)rs485Info;
        pAdvanceInfo = (AdvanceInfo_t *)advanceInfo;
    }

    // Print out the parameters
    printf("MBR is:\n");
    for(i=0; i< MBRINFO_SIZE; i++)
    {
        printf("%c\n",mbrInfo[i]);
    }
    printf("Address:0x%x%x\n", pSysInfo->addrH, pSysInfo->addrL);
    printf("SPED:%x\n", pSysInfo->sped);
    printf("channel: %d\n", pSysInfo->chan);
    printf("option: %x\n", pSysInfo->option);
    printf("model:%s\n", pSysInfo->model);
    printf("version:%s\n", pSysInfo->version);
    printf("role:%d\n", pSysInfo->role);
    printf("RS485:\r\n");
    printf("baudrate:%d\r\n", pRs485Info->baudRate);
    printf("parity:%d\r\n", pRs485Info->parity);
    printf("stopbits:%d\r\n", pRs485Info->stopBit);

    printf("packetDelay H:%d\r\n", pAdvanceInfo->packetDelayH);
    printf("packetDelay L:%d\r\n", pAdvanceInfo->packetDelayL);

}

SysInfo_t* getSysInfoPointer()
{
    SysInfo_t  *pSysInfo;
    pSysInfo = (SysInfo_t *)sysInfo;

    return pSysInfo;
}

void saveSysInfoPointer()
{
    Set_SysInfo(sysInfo, SYSINFO_SIZE);
    
}

Rs485Info_t* getRs485InfoPointer()
{
    Rs485Info_t  *pRs485Info;
    pRs485Info = (Rs485Info_t *)rs485Info;

    return pRs485Info;
}

void saveRs485InfoPointer()
{
    Set_Rs485Info(rs485Info,RS485INFO_SIZE);
}
AdvanceInfo_t *getAdvanceInfoPointer(){
    AdvanceInfo_t *pAdvanceInfo;
    pAdvanceInfo = (AdvanceInfo_t *)advanceInfo;

    return pAdvanceInfo;
}
void saveAdvanceInfoPointer(){
    Set_AdvanceInfo(advanceInfo, ADVANCEINFO_SIZE);
}
uint8_t getSysInfoChannel()
{
    SysInfo_t  *pSysInfo= (SysInfo_t *)sysInfo;
    return pSysInfo->chan;
}
uint8_t getSysInfoRole()
{
    SysInfo_t  *pSysInfo= (SysInfo_t *)sysInfo;
    return pSysInfo->role;
}
uint16_t getPacketDelay(){
    uint16_t delay=0;
    AdvanceInfo_t *pAdvanceInfo = (AdvanceInfo_t *)advanceInfo;
    delay = (pAdvanceInfo->packetDelayH)<<8 |(pAdvanceInfo->packetDelayL);
    return delay;
}

