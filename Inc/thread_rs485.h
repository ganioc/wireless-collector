#ifndef __THREAD_RS485_H
#define __THREAD_RS485_H


#define STATE_CONFIG     0x01
//#define STATE_WORKING  0x02
#define STATE_WAITING  0x03
#define STATE_WORKING_MASTER  0x04
#define STATE_WORKING_SLAVE  0x05

#define RX_STATE_HEAD  0x01
#define RX_STATE_BODY  0x02
#define RX_STATE_TAIL   0x03
#define RX_STATE_ZERO  0x04


#define CONFIG_STATE_ZERO     0x01
#define CONFIG_STATE_LAST_1 0x02
#define CONFIG_STATE_LAST_0 0x03
#define CONFIG_STATE_BODY     0x04

#define MASTER_STATE_ZERO  0x01
#define MASTER_STATE_BODY  0x02

#define SLAVE_STATE_ZERO  0x01
#define SLAVE_STATE_BODY  0x02

#define BUFFER_MAX_SIZE_RS485 255

#define RS485_STATE_RX_WAITING 0x01
#define RS485_STATE_RX_NONE       0x02


// about RS485 datarate
#define RS485_BAUDRATE_9600     0x0
#define RS485_BAUDRATE_19200   0x1

#define RS485_PARITY_NONE          0x0
#define RS485_PARITY_EVEN           0x1
#define RS485_PARITY_ODD            0x2

#define RS485_STOP_BITS_NONE         0x0
#define RS485_STOP_BITS_1               0x01
#define RS485_STOP_BITS_2               0x02


void Rs485ThreadInit();
void ResetRS485RxBuffer();


#endif