#ifndef MESSAGE_H
#define MESSAGE_H

#include "cmsis_os.h"

#define NET_MESSAGE_LEN 128

#define MSG_TYPE_DEFAULT   0


typedef struct Mail{
    uint8_t type;
    uint8_t length;
    uint8_t  data[NET_MESSAGE_LEN];

} Mail_t;





typedef struct TaskThread{
    osMailQId idMail;
    uint8_t lenMail;
    uint8_t state;
    osThreadId idThread;
    

}TaskThread_t;

#endif
