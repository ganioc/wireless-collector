#ifndef __MYE2PROM_H_
#define __MYE2PROM_H_

#include "stm32f1xx_hal.h"
#include "main.h"

/**

8KB  = 64bit

there are 32 sections
1 section has 8 pages
1 page has 32 bytes

0 section - 0 page will store MBR info

0 section  - 1 page will store system information , <=32 bytes

**/

#define SECTION_MBRINFO     0
#define SECTION_SYSINFO      0
#define SECTION_RS485INFO  0

#define PAGE_MBRINFO          0
#define PAGE_SYSINFO          1
#define PAGE_RS485INFO       2

#define E2PROM_ADDR   0xA0
#define E2PROM_MAXPKT   32
#define E2PROM_WRITE_WAIT   10  //ms
#define E2PROM_READ_WAIT     10
#define E2PROM_TIMEOUT          5*E2PROM_WRITE_WAIT
#define E2PROM_SECTION_SIZE   32


typedef struct MBRInfo
{
    char mark1; // Whether it's a valid info, 'Ruff' is correct, 0xFF is not;
    char mark2;
    char mark3;
    char mark4;
} MBRInfo_t;

typedef struct SysInfo
{
    uint8_t addrH;
    uint8_t addrL;
    uint8_t sped;
    uint8_t chan;
    uint8_t option;
    
    uint8_t role;  // 1 master, 0 slave
    char model[20];
    char version[5];


} SysInfo_t;

typedef struct Rs485Info{
    uint8_t baudRate;
    uint8_t parity; 
    uint8_t stopBit;
    
}Rs485Info_t;

#define MBRINFO_SIZE                    sizeof(MBRInfo_t)
#define SYSINFO_SIZE                  sizeof(SysInfo_t)
#define RS485INFO_SIZE              sizeof(Rs485Info_t)

void E2PROM_Init(void);

SysInfo_t* getSysInfoPointer();
void saveSysInfoPointer();

Rs485Info_t* getRs485InfoPointer();
void saveRs485InfoPointer();

uint8_t getSysInfoChannel();
uint8_t getSysInfoRole();    

void ResetToDefaultE2Prom();
#endif
