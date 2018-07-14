#ifndef _DATAPROCESS_H_
#define _DATAPROCESS_H_


void * ThreadDataProcess(void * arg);

char checkThreadPool(pthread_t tidself);


#define FREEUDPMSG 0xffffffff


#define ThreadFree 0
#define ThreadOK 1

#define MYSQLFAILED	0
#define MYSQLNORESULT	1
#define MYSQLOK		0xff

#endif
