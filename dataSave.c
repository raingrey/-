#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "rbtree.h"
#include "modbusDriver.h"
#include "dataProcess.h"
#include "dataSave.h"

//if meter data node can't not save sent it back
//this can sent back have critical counter(before the critical number,
//func will try to sent node back,after this number,func will free meterdata node)
//in case of try too many time
char SendBackMeterDataPrimaryNode(meterDataPrimary * p);
char SendBackMeterDataSecondaryNode(meterDataSecondary* p);

void * ThreadDataSave(void * arg){

	meterDataPrimary * p=NULL;
	meterDataSecondary * p1=NULL;
	uint8_t querystrtmp[MYSQLQUERYSTRSIZE] = {0};
#ifdef DEBUG
	printf("数据保存线程已经就绪——时间—%s",get_str_time_now());
#endif
	uint32_t counter=0;
	uint32_t sleep_counter=0;
	uint32_t effective_counter=0;
	while(1){
		p=NULL;
		p1=NULL;
		pthread_mutex_lock(&data_save_mtx);
//if there has no meter data to save hung up thread
		while((meterDataPrimaryHead == NULL)&&(meterDataSecondaryHead == NULL)){
#ifdef DEBUG
			printf("数据保存线程第————%d————次睡眠,时间-%s",++sleep_counter,get_str_time_now());
#endif
			pthread_cond_wait(&condDataSave,&data_save_mtx);
#ifdef DEBUG
			printf("数据保存线程第————%d————次唤醒,时间-%s",++counter,get_str_time_now());
#endif
		}
//1.1 cut node from one-way link-list meterdataprimary 
		if(meterDataPrimaryHead){
			p=meterDataPrimaryHead;
			if(p->next)
				meterDataPrimaryHead = p -> next;
			else
				meterDataPrimaryHead = NULL;
		}
//2.1 cut is from one-way link-list meterdatasecondary 
		if(meterDataSecondaryHead){
			p1=meterDataSecondaryHead;
			if(p1->next!=p1)
				meterDataSecondaryHead = p1 -> next;
			else
				meterDataSecondaryHead = NULL;
		}
		pthread_mutex_unlock(&data_save_mtx);
//1.2 if there is a node to handle
		if(p){
//1.3 insert meterdataprimary to mysql
			sprintf(querystrtmp,"insert into MeterDataPrimary "
                                "(meterID,instantflow,totalFlow,T,P,DP,timestamp)"
                                "values(%d,%f,%ld,%f,%f,%f,'%s')",
			p -> meterID,p -> instantFlow,p -> totalFlow,p -> T,p -> P,p -> DP,p -> timestamp);
			if(mysql_real_query(sqlDataSave,querystrtmp,strlen(querystrtmp))){
				printf("%s\n",mysql_error(sqlDataSave));
				pthread_mutex_lock(&data_save_mtx);
				SendBackMeterDataPrimaryNode(p);
				pthread_mutex_unlock(&data_save_mtx);
				p=NULL;
			}else{
printf("meterDataPrimary has been saved\n");
//1.4 free meterdataprimary node
				free(p);
#ifdef DEBUG_outofmemory
		printf("--dataSave缓存个数：%d\n",--memory_node_counter_datasave);
#endif
				p = NULL;
			}
		}
//2.0 handle meterdatasecondary
//2.2 if there is a meterdatasecondary data
		if(p1){
//2.3 insert meterdatasecondary to mysql
           		 sprintf(querystrtmp,"insert into MeterDataSecondary"
                                " (meterID,order7,order8,order9,"
                                "order10,order11,order12,order13,"
                                "order14,order15,order16,order17,order18,"
                                "order19,order20,timestamp)"
                                "values"
                                "(%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%s',"
                                "'%s','%s','%s')",
				p1 -> meterID,p1 -> order7,p1 -> order8,p1 -> order9,
				p1 -> order10,p1 -> order11,p1 -> order13,p1 -> order14,
				p1 -> order15,p1 -> order16,p1 -> order17,p1 -> order18,
				p1 -> order19,p1 -> order20,p1 -> timestamp);
		    //printf("%s\n",querystrtmp);
			if(mysql_real_query(sqlDataSave,querystrtmp,strlen(querystrtmp))){
				printf("%s\n",mysql_error(sqlDataSave));
				pthread_mutex_lock(&data_save_mtx);
					//无法存入时超过8次会被丢弃
				SendBackMeterDataSecondaryNode(p1);
				pthread_mutex_unlock(&data_save_mtx);
				p1=NULL;
			}else{
				printf("meterDataSecondary has been saved\n");
				free(p1);

#ifdef DEBUG_outofmemory
		printf("--dataSave缓存个数：%d\n",--memory_node_counter_datasave);
#endif
				p1= NULL;
			}
		}
	}
}


char SendBackMeterDataPrimaryNode(meterDataPrimary * p){
	//insert p back to two-way link-list's head
    if(p ->sC < METERDATAPRIMARYSAVECOUNTCRITICAL){
        p -> sC ++;
        p -> next = meterDataPrimaryHead;
        meterDataPrimaryHead = p;
    }else{
		printf("meterID-%d,常规数据无法存入次数超过上限\n",p->meterID);
		free(p);
#ifdef DEBUG_outofmemory
		printf("--dataSave缓存个数：%d\n",--memory_node_counter_datasave);
#endif
    }
    return 1;
	//insert p back to two-way link-list's head
}
char SendBackMeterDataSecondaryNode(meterDataSecondary* p){
	//insert p back to two-way link-list's head
    if(p ->sC < METERDATASECONDARYSAVECOUNTCRITICAL){
        p -> sC++;
        p -> next = meterDataSecondaryHead;
        meterDataSecondaryHead = p;
    }else{
		printf("meterID-%d,用户自定义数据无法存入次数超过上限\n",p->meterID);
		free(p);

#ifdef DEBUG_outofmemory
		printf("--dataSave缓存个数：%d\n",--memory_node_counter_datasave);
#endif
	    return 1;
    }
	//insert p back to two-way link-list's head
}
/*********************************************************
 * early version has thread adjust
 * if need more dataProcess/dataSave,create more
*********************************************************


        //check if current thread threadPoolInfo's cmd is THREDFREE
        //then return this thread
        if(checkThreadPool(tidself)==ThreadFree){
            return NULL;
        }





*********************************************************
 * early version has thread adjust
 * if need more dataProcess/dataSave,create more
*********************************************************/


