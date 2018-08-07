#ifndef _DATAPROCESS_H_
#define _DATAPROCESS_H_

//LNode计数
extern uint32_t LNode_count;
//DNode计数
extern uint32_t DNode_count;
//MBRI计数
extern uint32_t MBRI_count;
//数据保存缓存链表计数
extern uint32_t data_save_primary_count;
extern uint32_t data_save_secondary_count;


void * ThreadDataProcess(void * arg);

char checkThreadPool(pthread_t tidself);
char * get_str_time_now();


#define FREEUDPMSG 0xffffffff


#define ThreadFree 0
#define ThreadOK 1

#define MYSQLFAILED	0
#define MYSQLNORESULT	1
#define MYSQLOK		0xff

#endif
