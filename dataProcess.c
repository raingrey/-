/*****************************************************************************
 * 2017-10-23
 * @raingrey
 * listeningNode is RBT
 * deviceNode is one-way link in listeningNode
 * modbusRegisterInfo is one-way link in deviceNode
 * if one listeningNode memory is inited
 * HeadDevice memory should be applyed and inited into a circle
 * every modbusRegisterInfoHead in every deviceNode should be inited into a circle also the memory
 * *****************************************************************************/

#include <stdio.h>
#include <math.h>
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
#include "dataProcess.h"
#include "dataSave.h"
#include "modbusDriver.h"

//LNode计数
uint32_t LNode_count=0;
//DNode计数
uint32_t DNode_count=0;
//MBRI计数
uint32_t MBRI_count=0;
//数据保存缓存链表计数
uint32_t data_save_primary_count=0;
uint32_t data_save_secondary_count=0;
//根据不同数据类型序号，选择不同的处理函数
uint32_t ushort_data_handler(uint32_t * i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t short_data_handler(uint32_t * i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t uint_data_handler(uint32_t * i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t int_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t ulong_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t long_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t float_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t double_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t char2_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t char4_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t char6_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t char8_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t char10_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t char12_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t char14_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t char16_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t char18_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t char20_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);
uint32_t timestamp_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p);

uint32_t (*add_data_to_link[MODBUSREGISTERDATATYPECOUNTE]) (uint32_t * i,struct mBDS mbds,uint32_t ord,void * p) ={
	ushort_data_handler,
	short_data_handler,
	uint_data_handler,
	int_data_handler,
	ulong_data_handler,
	long_data_handler,
	float_data_handler,
	double_data_handler,
	char2_data_handler,
	char4_data_handler,
	char6_data_handler,
	char8_data_handler,
	char10_data_handler,
	char12_data_handler,
	char14_data_handler,
	char16_data_handler,
	char18_data_handler,
	char20_data_handler,
	timestamp_data_handler
};
#define PRIMARYORDERCOUNT 5
uint32_t add_long_to_primary_link_instantflow(meterDataPrimary*p,long long data);
uint32_t add_double_to_primary_link_instantflow(meterDataPrimary*p,double data);
uint32_t add_long_to_primary_link_totalflow(meterDataPrimary*p,long long data);
uint32_t add_double_to_primary_link_totalflow(meterDataPrimary*p,double data);
uint32_t add_long_to_primary_link_t(meterDataPrimary*p,long long data);
uint32_t add_double_to_primary_link_t(meterDataPrimary*p,double data);
uint32_t add_long_to_primary_link_p(meterDataPrimary*p,long long data);
uint32_t add_double_to_primary_link_p(meterDataPrimary*p,double data);
uint32_t add_long_to_primary_link_dp(meterDataPrimary*p,long long data);
uint32_t add_double_to_primary_link_dp(meterDataPrimary*p,double data);
uint32_t (*add_long_to_primary_link[PRIMARYORDERCOUNT])
	(meterDataPrimary*p,long long modbusdataushort)={
	add_long_to_primary_link_instantflow,
	add_long_to_primary_link_totalflow,
	add_long_to_primary_link_t,
	add_long_to_primary_link_p,
	add_long_to_primary_link_dp
};
uint32_t (*add_double_to_primary_link[PRIMARYORDERCOUNT])
	(meterDataPrimary*p,double modbusdataushort)={
	add_double_to_primary_link_instantflow,
	add_double_to_primary_link_totalflow,
	add_double_to_primary_link_t,
	add_double_to_primary_link_p,
	add_double_to_primary_link_dp
};

/*add node to meter data save node */
/*add node to meter data save node */
/*add node to meter data save node */
char AddLongToMeterDataPrimaryLink(int ord,meterDataPrimary * p,unsigned long int k);
char AddCharToMeterDataSecondaryLink(int ord,meterDataSecondary * p,uint8_t * t,uint8_t i);
/*add node to meter data save node */
/*add node to meter data save node */
/*add node to meter data save node */

//红黑树处理
struct rb_root listeningNodeRoot = RB_ROOT;
struct listeningNode * RBTSearch(struct rb_root *root, uint64_t dtuid);
int RBTInsert(struct rb_root *root, struct listeningNode *data);
void RBTFree(struct listeningNode *node);
//红黑树处理
//
//deviceNode和ModBusRegisterInfo链表处理
//从mysql读东西
int GetMeterID(uint64_t dtuid,uint8_t addrstrtmp);
//generate or updata listeningNode's deviceNode link from mysql
int  CreateDeviceNodeCircleFromMysql(struct listeningNode *p);
//generate or updata deviceNode's modbusRegisterInfo link from mysql
int CreateModBusRegisterInfoCircleFromMysql(deviceNode *p);
//从mysql读东西


uint8_t FreeModBusRegisterInfoCircle(modbusRegisterInfo * p1);
uint8_t FreeDeviceNodeCircle(deviceNode * p1);
//deviceNode和ModBusRegisterInfo链表处理


/*handle data link*/
/*handle data link*/
/*handle data link*/
//if udp node can't be processed because of system,sent it back
//this have critical number for it's send back
//char SendBackUdpMsgNode(udpMsg * p);

//check if there is udp node waiting for handle
udpMsg * UdpMsgNodeWaitingForHandle();

//get deviceNode in listeningNode comfort to meterid
deviceNode * GetDeviceNodeComfortMeterid(listeningNode * p, uint32_t meterid);


/*handle data link*/
/*handle data link*/
/*handle data link*/

time_t LocalTime;

/*handle modbusData*/
/*handle modbusData*/
/*handle modbusData*/
//put modbus data into mBDS(modBusDataSplit) splitting
char SplitModBusData(mBDS *mbds,uint8_t mbst[],char n);
/*handle modbusData*/
/*handle modbusData*/
/*handle modbusData*/

int dataprocessconter = 0;
//检查RBT心跳和离线
char RBTListeningNodeCheck();
//该伪装主机了
char HostNodeUdpSend(listeningNode * tree);

char * get_str_time_now()
{
	time_t now;
	struct tm * timenow;
	time(&now);
	timenow=localtime(&now);
	return asctime(timenow);
}
void * ThreadDataProcess(void * arg){

    //temp for listeningNodeRoot
    //temp for udpMsg node
    udpMsg * p=NULL;
    //temp for listeningNode
	listeningNode * p1=NULL;
    //temp for device node in listeingNode
    deviceNode * dnp = NULL;
    deviceNode * dnpp = NULL;
    //temp for pointer of modbus register info in device node
    modbusRegisterInfo * pmbri=NULL;

    //temp for dtuid %s
    uint8_t dtuidstrtmp[DTUIDSIZE]= {0};
    //temp for dtuid %x
    uint64_t dtuid=0;
    //temp for heartbeat data %x
    uint8_t heartbeatstrtmp[sizeof(HEARTBEATSTR)]={0};
    //temp place for modbus data
    uint8_t modbusstrtmp[BUFF_SIZE]={0};
    //one frame modbus data size
    int imodbus=0;
    //place modbus data splitting
    struct mBDS mbds;
    //temp for meterid %d
    uint32_t meterid=0;
    //temp place when sent modbusdata to meterdata to save

    //signature for data belong
    char primarydatasign = 0;
    char secondarydatasign = 0;

    //temp data
	uint32_t i=0;
	int k=0;

    //meter data  node to save
	meterDataPrimary * meterdataprimary=NULL;
	meterDataSecondary * meterdatasecondary =NULL;

	time_t now;
#ifdef DEBUG
	printf("数据处理线程已经就绪——时间—%s",get_str_time_now());
#endif
	uint32_t counter=0;
	uint32_t sleep_counter=0;
	uint32_t effective_counter=0;
	while(1){
		if(RBTListeningNodeNumber){
			RBTListeningNodeNumber=0;
			RBTListeningNodeCheck();
		}
//1.0
/*data is incorrec thung up thread and turn back to while  */
/*data is incorrec thung up thread and turn back to while  */
/*data is incorrec thung up thread and turn back to while  */
		pthread_mutex_lock(&mtx);
		////imodbus is for length of modbus data
		while((p = udpMsgHead) == NULL){
#ifdef DEBUG
			printf("数据处理线程第————%d————次睡眠,时间-%s",++sleep_counter,get_str_time_now());
#endif
			pthread_cond_wait(&condDataProcess,&mtx);
#ifdef DEBUG
			printf("数据处理线程——第%d————次醒来处理数据,时间-%s",++counter,get_str_time_now());
#endif
			//if thread resume back to while(1)
			//continue;
		}
		udpMsgHead = udpMsgHead -> next;
		//if udp message is bigger than BUFF_SIZE?maybe this is no need to do
		//unlock thread
		pthread_mutex_unlock(&mtx);

//2.0
/*data is correct ,from now udpMsg is confirmed have DTUID,so i can add it to listeningNode RBT*/
/*data is correct ,from now udpMsg is confirmed have DTUID,so i can add it to listeningNode RBT*/
/*data is correct ,from now udpMsg is confirmed have DTUID,so i can add it to listeningNode RBT*/
		strncpy(dtuidstrtmp,p -> msg,DTUIDSIZE);
		//transmitt dtuid char to long
		dtuid=atol(dtuidstrtmp);

//Until here we have LN.if no LN we had created it.*/
//Until here we have LN.if no LN we had created it.*/
//Until here we have LN.if no LN we had created it.*/
//2.1 if udpmsg is not in RBT listeningNode,insert it
		if((p1=RBTSearch(&listeningNodeRoot,dtuid))==NULL){
			//create a new listeningNode
			//此处还要加入连接数超过限制，将会停止新建连接
			//怎呢弄好呢。。。如果一个连接，过一个小时发一个数据，然后超时时间是50分钟，那么这个连接肯定会产生不断新建的情况，如何是好？
			if(stable_buffer_size > stable_buffer_limit_size){
				goto data_process_continue;
			}
			if((p1 = (listeningNode*)malloc(sizeof(listeningNode))) == NULL){
				printf("listeningNode out of memory");
				//if thread resume back to while(1)
				goto data_process_continue;
			}
			LNode_count++;
#ifdef DEBUG_outofmemory
			//测试内存溢出的问题
			printf("+++++++++++++++LNode堆缓存计数%d--没有查找到LNode,创建的DTUID为：%d\n",++memory_node_counter_LNode,dtuid);
			//测试内存溢出的问题
#endif
			memset(p1,0,(sizeof(listeningNode)));
			//set new listeningNode's dtuid
			p1 -> DTUID = dtuid;
			 //reset listeningNode's dumptime
			p1 -> dumpTime=time(NULL);
			p1 -> clientAddr = p -> clientAddr;
			RBTInsert(&listeningNodeRoot,p1);
		}
        //save this node's MBStatus
//2.1 if udpmsg is not in RBT listeningNode,insert it
//Until here we have LN.if no LN we had created it.*/
//Until here we have LN.if no LN we had created it.*/
//Until here we have LN.if no LN we had created it.*/
/*
        printf("\n******************************************************\n"
               "root node's DTUID = %ld\n"
               "******************************************************\n",container_of(listeningNodeRoot.rb_node, struct listeningNode, node) -> DTUID);
*/



//2.2 udp data and listeningNode is ready,now we will handle it
		//handle heart beat data
		if(!memcmp(p -> msg+DTUIDSIZE,HEARTBEATSTR,sizeof(HEARTBEATSTR))){
			goto data_process_reset_connect;
		}

		//对于有效ModBus仪表数据，在DTU+2指针取得肯定是ModBus消息长度，+5是将ModBus地址、功能码、长度码、CRC校验码算入
		imodbus=*(p -> msg + DTUIDSIZE + 2)+ 5;
		memset(modbusstrtmp,0,sizeof(modbusstrtmp));
		memcpy(modbusstrtmp,p -> msg+DTUIDSIZE,imodbus); 
		memset(&mbds,0,sizeof(mbds));
		SplitModBusData(&mbds,&modbusstrtmp[0],imodbus);
		//remove situation CRC16 check failed
		if(!((mbds.fC == MODBUSFUNCCODE) && (mbds.CRC == ModBusCRC16(modbusstrtmp,imodbus-MODBUSCRCDATASIZE)))){
			goto data_process_reset_connect;
		}
//2.2 udp data and listeningNode is ready,now we will handle it

#ifdef DEBUG
		dataprocessconter++;
		printf("\ndataprocessconter: %d ,DTUID: %ld --- heartBeatNumber: %d,imodbus:%d\n\n",dataprocessconter,p1->DTUID,p1->heartBeat,imodbus);
#endif

//2.3 from now udpMsg is confirmed have device address,so i can identify it's meterID\startRegister*/
		meterid=GetMeterID(dtuid,mbds.addr);
		//check if meter id is correct
		if(meterid == 0){
			goto data_process_reset_connect;
		}

#ifdef DEBUG
		printf("\n******************************************************\n"
		       "current meterID = %d\n"
		       "******************************************************\n",meterid);
#endif

//2.3.2 get device node from listeningNode
		dnp=GetDeviceNodeComfortMeterid(p1,meterid);
        	if((dnp == NULL)||(dnp -> modbusRegisterInfoHead == NULL)){
#ifdef DEBUG
			if(dnp == NULL)
				printf("这次创建节点是由于没有DN符合");
			if(dnp&&dnp -> modbusRegisterInfoHead == NULL)
				printf("这次创建节点是由于DN没有MBRI符合");
#endif
			if(CreateDeviceNodeCircleFromMysql(p1)){
				printf("\nAlarm:mysql have no device node info about DTUID %ld",p1 -> DTUID);
				goto data_process_reset_connect;
				//zero it's dumptime
			}
			dnp=GetDeviceNodeComfortMeterid(p1,meterid);
			if((dnp == NULL)||(dnp -> modbusRegisterInfoHead == NULL)){
				printf("\nAlarm:mysql have no  info about meterID %d\n",meterid);
				goto data_process_reset_connect;
			}
		}
//2.3 from now udpMsg is confirmed have device address,so i can identify it's meterID\startRegister*/

//2.4 now we have all modbus data,manage data which is not from modbus client
		//check modbus function code
        	if(mbds.fC != MODBUSFUNCCODE){
			goto data_process_reset_connect;
		}

		//manage modbus host data(host on modbus wire)
		//if it is 0x03,check if it is host or client
		//if data fit with host data size,check if position of byte number counter is fit with register data number
        	if((imodbus == MODBUSHOSTDATASIZE)&&(mbds.bN!= 3)){
			p1 -> heartBeat=0;
			//3is the byte number when the length of modbus data is 8, this is host data,analysis it
			//get it's start register 
            		if((dnp -> startRegister!=mbds.sR)){
				if(CreateDeviceNodeCircleFromMysql(p1)){
					//we have no need to go to sleep,host data finally go to sleep
					printf("\nAlarm:mysql have no device node info about DTUID %ld",p1 -> DTUID);
				}
				if(dnp -> startRegister!=mbds.sR){
					printf("\nAlarm:modbus host commond start register is not fit mysql,but listeningNode is fit with mysql,i have synchronization listeningNode with current modbus host data");
				}else{
					printf("\nAlarm:modbus host commond start register is not fit mysql,listeningNode is not fit with mysql too,i have synchronization listeningNode with current modbus host data");
				}

			}
            		p1 -> dumpTime=time(NULL);
			goto data_process_continue;
		}

		//manage host/client confused data
		//(i have no solution for this situation,
		//maybe we can set modbus data should have timestamp
		//(timestamp is bigger than 3byte,then client data 
		//is bigger than 8))
        	if((imodbus == MODBUSHOSTDATASIZE)&&(mbds.bN== 3)){
			goto data_process_reset_connect;
		}

//2.4 now we have all modbus data,manage data which is not from modbus client


//2.5 handle client data,all bad situation had removed,i can sure this is client message,
//@2017/0803/23:16
//@raingrey
		i=0;
		//仪表应答数据是有可能与登记数据长度不符的，因此遍历modbusRegisterInfoHead从头算一下
		for(pmbri = dnp -> modbusRegisterInfoHead;pmbri;pmbri = pmbri -> next){
		//TODO:如存在时间戳字段应该在此检出,以便下游数据存储时可以插入其中，另外下游处理时不应再存储时间戳字段----并未实现
		//当前版本放弃了直接存储时间戳的处理，将时间戳的任务交给服务器,即数据存储时间为服务器时间
		//因为与其放一个错位的时间戳，还不如用服务器的时间戳
			i += pmbri -> bytenum;
			if(i == mbds.bN)
				break;
		}
		//if there is some data broken;
		if(i!=mbds.bN){
			printf("\nAlarm:modbus data has broken,not comfort to mysql's register info for meterid:%d 's data analsys",meterid);
			goto data_process_reset_connect;
		}
		pmbri = dnp -> modbusRegisterInfoHead;
		i=0;
		//create meterDataPrimary node
		//create meterDataSecondary node
		if(meterdataprimary)	memset(meterdataprimary,0,sizeof(meterDataPrimary));
		else {
			meterdataprimary= (meterDataPrimary*)malloc(sizeof(meterDataPrimary));
				//测试内存溢出的问题
#ifdef DEBUG_outofmemory
				printf("++datasave堆缓存计数%d--申请了常规有效数据\n",++memory_node_counter_datasave);
#endif
				//测试内存溢出的问题
		
		}
		if(meterdatasecondary)	memset(meterdatasecondary,0,sizeof(meterDataSecondary));
		else {
			meterdatasecondary = (meterDataSecondary*)malloc(sizeof(meterDataSecondary));
			//测试内存溢出的问题
	#ifdef DEBUG_outofmemory
			printf("++datasave堆缓存计数%d--申请了用户自定义数据\n",++memory_node_counter_datasave);
	#endif
			//测试内存溢出的问题
		}
		meterdataprimary -> meterID = meterid;
		meterdatasecondary -> meterID = meterid;
//
//2.5.2 handle it one by one
        do{
		if(pmbri -> ord <= ORDERPRIMARYNUMBER)
			primarydatasign = 1;
		else if(pmbri -> ord > ORDERPRIMARYNUMBER)
			secondarydatasign = 1;
		k=pmbri -> ord;
#ifdef DEBUG
		printf("k=%d,i=%d,pmbri->datatype=%d\n",k,i,pmbri->datatype);
#endif
		if((pmbri -> datatype < MODBUSREGISTERDATATYPECOUNTE))
			(*add_data_to_link[pmbri -> datatype - 1])(&i,mbds,k,(void *)meterdataprimary);
		//时间戳字段被跳过
		if(pmbri -> datatype == MODBUSREGISTERDATATYPETIMESTAMP)
                	i+= pmbri->bytenum;
		pmbri = pmbri -> next;
        }while((i<mbds.bN)&&(pmbri));
//2.5.2 handle it one by one
//
//2.5.3 send meterdata to it's link
        pthread_mutex_lock(&data_save_mtx);
#ifdef DEBUG
	printf("数据处理线程第————%d————次发现有效数据,时间-%s",++effective_counter,get_str_time_now());
#endif
        if(primarydatasign){
            meterdataprimary -> next= meterDataPrimaryHead;
            meterDataPrimaryHead = meterdataprimary;
            meterdataprimary= NULL;
		data_save_primary_count++;
        }
        if(secondarydatasign){
            meterdatasecondary -> next= meterDataSecondaryHead;
            meterDataSecondaryHead = meterdatasecondary;
            meterdatasecondary=NULL;
		data_save_secondary_count++;
        }
	if(data_save_secondary_count+data_save_primary_count>10){
		pthread_cond_signal(&condDataSave);
	}
        pthread_mutex_unlock(&data_save_mtx);
	goto data_process_no_heartbeat;
data_process_reset_connect:
		p1 -> heartBeat++;
data_process_no_heartbeat:
            	p1 -> dumpTime=time(NULL);
data_process_continue:
#ifdef DEBUG_outofmemory
		printf("--udpmsg缓存个数：%d--网络数据被复制完\n",--memory_node_counter_udpmsg);
#endif
		free(p);
		p=NULL;
		UdpMsgNumber--;
	}
//2.5.3 send meterdata to it's link
//
//2.5 handle client data,all bad situation had removed,i can sure this is client message,
//2.5 handle client data,all bad situation had removed,i can sure this is client message,
//2.5 handle client data,all bad situation had removed,i can sure this is client message,
	return 0;
}

uint32_t add_long_to_primary_link_instantflow(meterDataPrimary*p,long long data){
        p -> instantFlow = (float)data;
	return 0;
}
uint32_t add_double_to_primary_link_instantflow(meterDataPrimary*p,double data){
        p -> instantFlow = (float)data;
	return 0;
}
uint32_t add_long_to_primary_link_totalflow(meterDataPrimary*p,long long data){
        p -> totalFlow = (uint64_t)data;
	return 0;
}
uint32_t add_double_to_primary_link_totalflow(meterDataPrimary*p,double data){
        p -> totalFlow = (uint64_t)data;
	return 0;
}
uint32_t add_long_to_primary_link_t(meterDataPrimary*p,long long data){
        p -> T= (float)data;
	return 0;
}
uint32_t add_double_to_primary_link_t(meterDataPrimary*p,double data){
        p -> T= (float)data;
	return 0;
}
uint32_t add_long_to_primary_link_p(meterDataPrimary*p,long long data){
        p -> P= (float)data;
	return 0;
}
uint32_t add_double_to_primary_link_p(meterDataPrimary*p,double data){
        p -> P= (float)data;
	return 0;
}
uint32_t add_long_to_primary_link_dp(meterDataPrimary*p,long long data){
        p -> DP= (float)data;
	return 0;
}
uint32_t add_double_to_primary_link_dp(meterDataPrimary*p,double data){
        p -> DP= (float)data;
	return 0;
}

uint32_t ushort_data_handler(uint32_t * i,struct mBDS mbds,uint32_t ord,void * p){
	uint16_t modbusdataushort = mbds.mBD[*i] * 0x100 + mbds.mBD[(*i)+1];
	if(ord<PRIMARYORDERCOUNT)
		(*add_long_to_primary_link[ord-1])((meterDataPrimary*)p,(long long)modbusdataushort);
	(*i)+=MODBUSDATAUSHORTSIZE;
	return 0;
}
uint32_t short_data_handler(uint32_t * i,struct mBDS mbds,uint32_t ord,void * p){
	uint16_t modbusdataushort = mbds.mBD[*i] * 0x100 + mbds.mBD[(*i)+1];
	if(ord<PRIMARYORDERCOUNT)
		(*add_long_to_primary_link[ord-1])((meterDataPrimary*)p,*((short *)(&modbusdataushort)));
	(*i)+=MODBUSDATAUSHORTSIZE;
	return 0;
}
uint32_t uint_data_handler(uint32_t * i,struct mBDS mbds,uint32_t ord,void * p){
	uint32_t modbusdatauint = mbds.mBD[*i]*0x1000000+mbds.mBD[(*i)+1]*0x10000+mbds.mBD[(*i)+2]*0x100+mbds.mBD[(*i)+3];
	if(ord<PRIMARYORDERCOUNT)
		(*add_long_to_primary_link[ord-1])((meterDataPrimary*)p,modbusdatauint);
	(*i)+=MODBUSDATAUINTSIZE;
	return 0;
}
uint32_t int_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint32_t modbusdatauint=mbds.mBD[*i]*0x1000000+mbds.mBD[*i+1]*0x10000+mbds.mBD[*i+2]*0x100+mbds.mBD[*i+3];
	if(ord<PRIMARYORDERCOUNT)
		(*add_long_to_primary_link[ord-1])((meterDataPrimary*)p,*((int *)(&modbusdatauint)));
	(*i)+=MODBUSDATAINTSIZE;
	return 0;
}
uint32_t ulong_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint64_t modbusdataulong=
		mbds.mBD[*i] * 0x100000000000000+
		mbds.mBD[*i+1]*0x1000000000000+
		mbds.mBD[*i+2]*0x10000000000+
		mbds.mBD[*i+3]*0x100000000+
		mbds.mBD[*i+4]*0x1000000+
		mbds.mBD[*i+5]*0x10000+
		mbds.mBD[*i+6]*0x100+
		mbds.mBD[*i+7];
	if(ord<PRIMARYORDERCOUNT)
		(*add_long_to_primary_link[ord-1])((meterDataPrimary*)p,modbusdataulong);
	(*i)+=MODBUSDATAULONGSIZE;
	return 0;
}
uint32_t long_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint64_t modbusdataulong=
		mbds.mBD[*i] * 0x100000000000000+
		mbds.mBD[*i+1]*0x1000000000000+
		mbds.mBD[*i+2]*0x10000000000+
		mbds.mBD[*i+3]*0x100000000+
		mbds.mBD[*i+4]*0x1000000+
		mbds.mBD[*i+5]*0x10000+
		mbds.mBD[*i+6]*0x100+
		mbds.mBD[*i+7];
	if(ord<PRIMARYORDERCOUNT)
		(*add_long_to_primary_link[ord-1])((meterDataPrimary*)p,(long long)modbusdataulong);
	(*i)+=MODBUSDATALONGSIZE;
	return 0;
}
uint32_t float_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint32_t modbusdatauint=mbds.mBD[(*i)]*0x1000000+mbds.mBD[(*i)+1]*0x10000+mbds.mBD[(*i)+2]*0x100+mbds.mBD[(*i)+3];
#ifdef DEBUG
	printf("处理float数据，ord=%d,modbusdatauint=%lf",ord,(double)(*((float *)(&modbusdatauint))));
#endif
	if(ord<PRIMARYORDERCOUNT)
		(*add_double_to_primary_link[ord-1])((meterDataPrimary*)p,(double)(*((float *)(&modbusdatauint))));
	(*i)+=MODBUSDATAFLOATSIZE;
	return 0;
}
	
uint32_t double_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint64_t modbusdataulong=
		mbds.mBD[*i] * 0x100000000000000+
		mbds.mBD[*i+1]*0x1000000000000+
		mbds.mBD[*i+2]*0x10000000000+
		mbds.mBD[*i+3]*0x100000000+
		mbds.mBD[*i+4]*0x1000000+
		mbds.mBD[*i+5]*0x10000+
		mbds.mBD[*i+6]*0x100+
		mbds.mBD[*i+7];
	if(ord<PRIMARYORDERCOUNT)
		(*add_double_to_primary_link[ord-1])((meterDataPrimary*)p,*((double *)(&modbusdataulong)));
	(*i)+=MODBUSDATADOUBLESIZE;
	return 0;
}

uint32_t char2_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint8_t modbusdatabuffer[MODBUSDATAUCHAR2];
	memcpy(modbusdatabuffer,&(mbds.mBD[*i]),MODBUSDATAUCHAR2);
	AddCharToMeterDataSecondaryLink(ord,(meterDataSecondary*)p,modbusdatabuffer,MODBUSDATAUCHAR2);
	(*i)+=MODBUSDATAUCHAR2;
	return 0;
}
uint32_t char4_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint8_t modbusdatabuffer[MODBUSDATAUCHAR4];
	memcpy(modbusdatabuffer,&(mbds.mBD[*i]),MODBUSDATAUCHAR4);
	AddCharToMeterDataSecondaryLink(ord,(meterDataSecondary*)p,modbusdatabuffer,MODBUSDATAUCHAR4);
	(*i)+=MODBUSDATAUCHAR4;
	return 0;
}
uint32_t char6_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint8_t modbusdatabuffer[MODBUSDATAUCHAR6];
	memcpy(modbusdatabuffer,&(mbds.mBD[*i]),MODBUSDATAUCHAR6);
	AddCharToMeterDataSecondaryLink(ord,(meterDataSecondary*)p,modbusdatabuffer,MODBUSDATAUCHAR6);
	(*i)+=MODBUSDATAUCHAR6;
	return 0;
}
uint32_t char8_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint8_t modbusdatabuffer[MODBUSDATAUCHAR8];
	memcpy(modbusdatabuffer,&(mbds.mBD[*i]),MODBUSDATAUCHAR8);
	AddCharToMeterDataSecondaryLink(ord,(meterDataSecondary*)p,modbusdatabuffer,MODBUSDATAUCHAR8);
	(*i)+=MODBUSDATAUCHAR8;
	return 0;
}
uint32_t char10_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint8_t modbusdatabuffer[MODBUSDATAUCHAR10];
	memcpy(modbusdatabuffer,&(mbds.mBD[*i]),MODBUSDATAUCHAR10);
	AddCharToMeterDataSecondaryLink(ord,(meterDataSecondary*)p,modbusdatabuffer,MODBUSDATAUCHAR10);
	(*i)+=MODBUSDATAUCHAR10;
	return 0;
}
uint32_t char12_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint8_t modbusdatabuffer[MODBUSDATAUCHAR12];
	memcpy(modbusdatabuffer,&(mbds.mBD[*i]),MODBUSDATAUCHAR12);
	AddCharToMeterDataSecondaryLink(ord,(meterDataSecondary*)p,modbusdatabuffer,MODBUSDATAUCHAR12);
	(*i)+=MODBUSDATAUCHAR12;
	return 0;
}
uint32_t char14_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint8_t modbusdatabuffer[MODBUSDATAUCHAR14];
	memcpy(modbusdatabuffer,&(mbds.mBD[*i]),MODBUSDATAUCHAR14);
	AddCharToMeterDataSecondaryLink(ord,(meterDataSecondary*)p,modbusdatabuffer,MODBUSDATAUCHAR14);
	(*i)+=MODBUSDATAUCHAR14;
	return 0;
}
uint32_t char16_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint8_t modbusdatabuffer[MODBUSDATAUCHAR16];
	memcpy(modbusdatabuffer,&(mbds.mBD[*i]),MODBUSDATAUCHAR16);
	AddCharToMeterDataSecondaryLink(ord,(meterDataSecondary*)p,modbusdatabuffer,MODBUSDATAUCHAR16);
	(*i)+=MODBUSDATAUCHAR16;
	return 0;
}
uint32_t char18_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint8_t modbusdatabuffer[MODBUSDATAUCHAR18];
	memcpy(modbusdatabuffer,&(mbds.mBD[*i]),MODBUSDATAUCHAR18);
	AddCharToMeterDataSecondaryLink(ord,(meterDataSecondary*)p,modbusdatabuffer,MODBUSDATAUCHAR18);
	(*i)+=MODBUSDATAUCHAR18;
	return 0;
}
uint32_t char20_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
	uint8_t modbusdatabuffer[MODBUSDATAUCHAR20];
	memcpy(modbusdatabuffer,&(mbds.mBD[*i]),MODBUSDATAUCHAR20);
	AddCharToMeterDataSecondaryLink(ord,(meterDataSecondary*)p,modbusdatabuffer,MODBUSDATAUCHAR20);
	(*i)+=MODBUSDATAUCHAR20;
	return 0;
}
//时间戳并不会被真实处理，此处仅为了防止出现bug
uint32_t timestamp_data_handler(uint32_t *i,struct mBDS mbds,uint32_t ord,void * p){
}
/*return:
 * 1	ok;
 * 0	mission failed
 * */
char AddCharToMeterDataSecondaryLink(int ord,meterDataSecondary * p,uint8_t * t,uint8_t i){

    switch(ord){
    case ORDER20:
        if(i>20) return 0;
        memcpy(p -> order20,t,i);
        break;
    case ORDER7:
        if(i>20) return 0;
        memcpy(p -> order7,t,i);
        break;
    case ORDER8:
        if(i>20) return 0;
        memcpy(p -> order8,t,i);
        break;
    case ORDER9:
        if(i>20) return 0;
        memcpy(p -> order9,t,i);
        break;
    case ORDER10:
        if(i>20) return 0;
        memcpy(p -> order10,t,i);
        break;
    case ORDER11:
        if(i>20) return 0;
        memcpy(p -> order11,t,i);
        break;
    case ORDER12:
        if(i>20) return 0;
        memcpy(p -> order12,t,i);
        break;
    case ORDER13:
        if(i>20) return 0;
        memcpy(p -> order13,t,i);
        break;
    case ORDER14:
        if(i>20) return 0;
        memcpy(p -> order14,t,i);
        break;
    case ORDER15:
        if(i>20) return 0;
        memcpy(p -> order15,t,i);
        break;
    case ORDER16:
        if(i>20) return 0;
        memcpy(p -> order16,t,i);
        break;
    case ORDER17:
        if(i>20) return 0;
        memcpy(p -> order17,t,i);
        break;
    case ORDER18:
        if(i>20) return 0;
        memcpy(p -> order18,t,i);
        break;
    case ORDER19:
        if(i>20) return 0;
        memcpy(p -> order19,t,i);
        break;
    default:
        return 0;
        break;
    }
    return 1;
}

char SplitModBusData(mBDS *mbds,uint8_t *mbst,char n){
    mbds -> addr = mbst[0];
    mbds -> fC = mbst[1];
    mbds -> bN = *(mbst+2);
    mbds -> CRC = *(mbst + n - 1)*0x100+*(mbst + n - 2) ;
    uint8_t i;
//    for(i=0;i<n;i++){
//        printf("%02x,",*(mbst+i));
//    }
    if(n==8){
        if(mbds -> fC == 3){
            mbds -> sR = *(mbst+2)*0x100+*(mbst+3) ;
            mbds -> rN = *(mbst+4)*0x100+*(mbst+5) ;
            memcpy(mbst + 3,mbds -> mBD,3);
            if(*(mbst+2) != 3){
                mbds -> dT = MODBUSDATAHOSTDATA ;
            }else if((*(mbst+2) == 3))
                mbds -> dT = MODBUSDATACLIENTORHOSTDATA;
        }else{
            mbds -> dT = MODBUSDATAWRONG;
        }
    }else if(n>5){
    //can't be host data
        mbds -> dT = MODBUSDATACLIENTDATA;
        for(i=0;i<n-5;i++){
           mbds->mBD[i]=mbst[i+3];
        }
//        memcpy(mbst + 3,mbds->mBD,n-5);
    }else if(n<5){
        mbds -> dT = MODBUSDATAWRONG;
        return 0;
    }
    return 1;
}


deviceNode * GetDeviceNodeComfortMeterid(listeningNode *p,uint32_t meterid){
	deviceNode * dnp = p -> headDevice;
	while(dnp){
#ifdef DEBUG
		printf("GetDeviceNodeComfortMeterid-----DN->meterID=%d----searching for meterid=%d",dnp -> meterID,meterid);
#endif
		if(dnp -> meterID == meterid){
			break;
		}
		dnp = dnp -> next;
	}
        return dnp;
}

udpMsg * UdpMsgNodeWaitingForHandle(){
	udpMsg * p=udpMsgHead->next;
    if((p!=udpMsgHead)){
        //cut is from one-way link-list udpMsg
        udpMsgHead -> next = p -> next;
	UdpMsgNumber--;
        return p;
	}
    //if there is no udp data
    return NULL;
}

int GetMeterID(uint64_t dtuid,uint8_t addrstrtmp){
	uint8_t querystrtmp[MYSQLQUERYSTRSIZE] = {0};
	int meterid=0;
//        printf("\n%d\n",dtuid);
	sprintf(querystrtmp,"select meterID from MeterIdentify where DTUID=%ld and deviceNumber=%d",dtuid,addrstrtmp);
	MYSQL_RES * mysqlres=NULL;
	MYSQL_ROW mysqlrow;
	if(mysql_real_query(sql,querystrtmp,strlen(querystrtmp))){
		printf("\n%s\n",querystrtmp);
		printf("\nfailed to query mysql when handle udpmsg,mysql,dtuid:%ld,addrstrtmp:%d, error msg%s\n",dtuid,addrstrtmp,mysql_error(sql));
		return MYSQLFAILED;
	}
	if(!(mysqlres=mysql_store_result(sql))){
        printf("\nfailed to get result of  mysql when handle udpmsg,mysql,dtuid:%ld,addrstrtmp:%d, error msg%s\n",dtuid,addrstrtmp,mysql_error(sql));
		return MYSQLFAILED;
	}
    if(mysqlrow=mysql_fetch_row(mysqlres))
        meterid=atoi(mysqlrow[0]);
    else    meterid = 0;
	//one result for per meterID per device
	if(mysqlres!=NULL){
		mysql_free_result(mysqlres);
		mysqlres = NULL;
    }
    return meterid;
}

int CreateModBusRegisterInfoCircleFromMysql(deviceNode * p){

    uint8_t querystrtmp[MYSQLQUERYSTRSIZE] = {0};
    /// get  of this device from mysql
    MYSQL_RES * mysqlres=NULL;
    MYSQL_ROW mysqlrow;
    int i=0,j=0;
    modbusRegisterInfo * mp1=NULL;
    modbusRegisterInfo * mp2=NULL;
    modbusRegisterInfo * lastmp1=NULL;
    time(&LocalTime);
    /// get modbusRegisterInfo from mysql
    sprintf(querystrtmp,"select ord,address,byteNumber,dataType from DataIdentify where meterID=%d order by address ASC,ord ASC",p -> meterID);
	if(mysql_real_query(sql,querystrtmp,strlen(querystrtmp))){
		printf("\n%sfailed to query mysql when handle deviceNode meterid:%d,deviceNumber:%d, error msg%s\n",ctime(&LocalTime),p -> meterID,p -> deviceNumber,mysql_error(sql));
		return 1;
	}
	if(!(mysqlres=mysql_store_result(sql))){
        printf("\n%s failed to get result of mysql when handle deviceNode meterid:%d,deviceNumber:%d, error msg%s\n",ctime(&LocalTime),p -> meterID,p -> deviceNumber,mysql_error(sql));
		mysql_free_result(mysqlres);
		mysqlres = NULL;
        return 1;
	}
    /// get modbusRegisterInfo from mysql


    /// create modbusRegisterInfo circle
    while(mysqlrow=mysql_fetch_row(mysqlres)){
	//首先，MBRI链中已有数据要剔除,然后，还要更新此阶位下已有数据
	i=atoi(mysqlrow[0]);
	for(mp1 = p -> modbusRegisterInfoHead;mp1;mp1 = mp1 -> next)
		if(i > mp1 -> ord)
			lastmp1=mp1;
		else if(i == mp1 -> ord){
			mp1 -> addr= atoi(mysqlrow[1]);
			mp1 -> bytenum= atoi(mysqlrow[2]);
			mp1 -> datatype= atoi(mysqlrow[3]);
			break;
		}
	if(mp1) continue;
        mp2 = (modbusRegisterInfo *)malloc(sizeof(modbusRegisterInfo));
	MBRI_count++;
#ifdef DEBUG_outofmemory
//	if(memory_node_counter_MBRI%100 == 0)
		printf("++ModBusRegisterInfo堆缓存计数%d--创建MBRI，meterID=%d,ord=%d,addr=%d,bytenum=%d,datatype=%d\n",++memory_node_counter_MBRI,p->meterID,atoi(mysqlrow[0]),atoi(mysqlrow[1]),atoi(mysqlrow[2]),atoi(mysqlrow[3]));
#endif
        memset(mp2,0,sizeof(modbusRegisterInfo));
        //get modbusRegisterInfo
        mp2 -> ord = atoi(mysqlrow[0]);
        mp2 -> addr= atoi(mysqlrow[1]);
        mp2 -> bytenum= atoi(mysqlrow[2]);
        mp2 -> datatype= atoi(mysqlrow[3]);
        if(MYSQLSTARTREGISTERORD == mp2 -> ord){
            //startregister could be 0;don't check it by 0;
            p -> startRegister=mp2 -> addr;
            //one result for per modbusbytenumber per device
        }
	//当modbusRegisterInfoHead没有元素
	if(! p -> modbusRegisterInfoHead){
		p -> modbusRegisterInfoHead = mp2;
	}else{
		//排序存放
		//lastmp1和next都有，找到插入位置
		//lastmp1有next无，插入在末尾
		//lastmp1无next无，head有，则mp2在head先，插头
		if(lastmp1&&lastmp1 -> next)
			mp2 -> next = lastmp1 -> next;
		else if(lastmp1)
			lastmp1 -> next = mp2;
		else{ 
			lastmp1 = p -> modbusRegisterInfoHead -> next;
			p -> modbusRegisterInfoHead = mp2;
			p -> modbusRegisterInfoHead -> next = lastmp1; 
		}
	}
	mp2 = NULL;
    }

        mysql_free_result(mysqlres);
        mysqlres = NULL;
	return 0;
}

int CreateDeviceNodeCircleFromMysql(listeningNode * p){
	uint64_t dtuid = p -> DTUID;
	uint8_t querystrtmp[MYSQLQUERYSTRSIZE] = {0};
	uint8_t i=0;
	deviceNode * p1 = NULL;
	deviceNode * p2 = NULL;
	deviceNode * lastp1 = NULL;
	modbusRegisterInfo * mp1 = NULL;
    /// get  of this device from mysql
	MYSQL_RES * mysqlres=NULL;
	MYSQL_ROW mysqlrow;
	time(&LocalTime);

	/// get device node data from mysql
	sprintf(querystrtmp,
            "select deviceNumber,meterID from MeterIdentify where DTUID=%ld",
            dtuid);
	if(mysql_real_query(sql,querystrtmp,strlen(querystrtmp))){
		printf("\n%s failed to query mysql when handle listeningNode DTUID:%ld,error msg%s\n",
               		ctime(&LocalTime),dtuid,mysql_error(sql));
		return 1;
	}
	if(!(mysqlres=mysql_store_result(sql))){
		printf("\nfailed to get result of mysql when handle listeningNode DTUID:%d,error msg%s\n",dtuid,mysql_error(sql));
		if(mysqlres!=NULL){
			mysql_free_result(mysqlres);
			mysqlres = NULL;
		}
        	return 1;
	}
    	/// get device node data from mysql


    /// create device node circle
    while(mysqlrow=mysql_fetch_row(mysqlres)){
	//确定只有没被添加过得DN才会申请内存并添加
	i=atoi(mysqlrow[1]);
	for(p1 = p -> headDevice;p1;lastp1=p1,p1 = p1 -> next){//lastp1用于存放链表最后一个元素(用于被插)
#ifdef DEBUG
//		printf("i和p1->meterID为%d-%d\n",i,p1->meterID);
#endif
		if(i == p1 -> meterID){
        		p1 -> deviceNumber = atoi(mysqlrow[0]);
			if(CreateModBusRegisterInfoCircleFromMysql(p1)){
				printf("\n%s Alarm: No ModBusRegisterInfo founded when create modbusRegisterInfo circle for deviceNode %d",ctime(&LocalTime),p1 -> meterID);
			}
#ifdef DEBUG
			printf("跳过了重复的meterID\n");
#endif
			break;
		}
	}
	if(p1)	continue;
	//确定只有没被添加过得DN才会申请内存并添加

        p2 = (deviceNode *)malloc(sizeof(deviceNode));
	DNode_count++;
#ifdef DEBUG_outofmemory
//	if(memory_node_counter_DN%100 == 0)
		printf("++deviceNode堆缓存计数%d--创建新DN-meterID=%d,deviceNumber=%d\n",++memory_node_counter_DN,atoi(mysqlrow[1]),atoi(mysqlrow[0]));
#endif
        memset(p2,0,sizeof(deviceNode));
        p2 -> deviceNumber = atoi(mysqlrow[0]);
        p2 -> meterID = atoi(mysqlrow[1]);
        if(CreateModBusRegisterInfoCircleFromMysql(p2)){
            printf("\n%s Alarm: No ModBusRegisterInfo founded when create modbusRegisterInfo circle for deviceNode %d",ctime(&LocalTime),p2 -> meterID);
        }
	//当headDevice没有元素
	if(p -> headDevice == NULL){
		p -> headDevice = p2;
	}else{
		lastp1 -> next = p2;
	}
	p2=NULL;
    }
    /// create device node circle
    /// create device node circle
    /// create device node circle
    ///
    /// send circle back or alarm
    mysql_free_result(mysqlres);
    mysqlres = NULL;
	return 0;
    /// send circle back or alarm
}



char HostNodeUdpSend(listeningNode * tree){

	uint32_t j = 0;
	uint16_t i=0;
	deviceNode * p =NULL;
	modbusRegisterInfo * p1 = NULL;
	uint8_t hostcode[8]={0};
	//	char * IPaddress=NULL;
	//	char message[]={"what's the fuck,where is my data???"};
	if(tree -> headDevice)
		p = tree -> headDevice;
	else    return 0;
	while(p != NULL){
		hostcode[0] = p -> deviceNumber;
		hostcode[1] = 0x03;
		hostcode[2] = p -> startRegister % 0x10000 / 0x100;
		hostcode[3] = p -> startRegister % 0x100;
		i=0;
		if(p -> modbusRegisterInfoHead)
		    p1=p -> modbusRegisterInfoHead;
		else    return 0;
		while(p1){
			i+=p1->bytenum;
			p1 = p1 ->next;
		}
		i/=2;
		hostcode[4] = i % 0x10000 / 0x100;
		hostcode[5] = i % 0x100;
		j=ModBusCRC16(hostcode,6);
		hostcode[6] = j % 0x100;
		hostcode[7] = j % 0x10000 / 0x100;
//		printf("hostcode:%s\n",hostcode);
		sendto(serv_sock,hostcode,sizeof(hostcode),0,(struct sockaddr*)&(tree->clientAddr),sizeof(tree -> clientAddr));
		p = p -> next;
		if(!p)
			break;
	}
	return 1;
}

///urgent: this is important
/// for every time every code line
/// if a pointer is freed(by free() or by defined function())
/// this pointer should be set NULL
/// in line after freefunction
void RBTFree(struct listeningNode *node)
{
    deviceNode * p1 = NULL;
    modbusRegisterInfo * p2 = NULL;
    if (node != NULL) {
        //free deviceNode and it's modbusRegisterInfo circle
        FreeDeviceNodeCircle(node -> headDevice);
        node -> headDevice = NULL;
        free(node);
	LNode_count--;
#ifdef DEBUG_outofmemory
	printf("--listeningNode堆缓存计数%d\n",--memory_node_counter_LNode);
#endif
        node = NULL;
    }
}


///urgent: this is important
/// for every time every code line
/// if a pointer is freed(by free() or by defined function())
/// this pointer should be set NULL
/// in line after freefunction
uint8_t FreeDeviceNodeCircle(deviceNode * p1){
    deviceNode * p2=NULL;
    while((p1)){
        //free deviceNode -> modbusRegisterInfo circle
        FreeModBusRegisterInfoCircle((p1 -> modbusRegisterInfoHead));
        p1 -> modbusRegisterInfoHead = NULL;
        //free deviceNode circle
        p2 = p1-> next;
        free(p1);
	DNode_count--;
#ifdef DEBUG_outofmemory
	printf("--deviceNode堆缓存计数%d\n",--memory_node_counter_DN);
#endif
	p1=p2;
        p2 = NULL;
    }

}

uint8_t FreeModBusRegisterInfoCircle(modbusRegisterInfo * p1){

    modbusRegisterInfo * p2 = NULL;
    //free deviceNode -> modbusRegisterInfo circle
    while((p1)){
        p2 = p1 -> next;
        free(p1);
	MBRI_count--;
#ifdef DEBUG_outofmemory
	printf("--ModBusRegisterInfo堆缓存计数%d\n",--memory_node_counter_MBRI);
#endif

        p1 = p2;
        p2 = NULL;
    }
    return 1;
}

char RBTListeningNodeCheck(){
	int i=0;
	struct rb_node * node= rb_first(&listeningNodeRoot);
    for (; node; node = rb_next(node)){
        listeningNode * p = rb_entry(node, struct listeningNode, node);
//        printf("key = %ld\n", p->DTUID);
        //1.2 handle heartBeat logic
        if(p -> heartBeat > MMAXHEARTBEATNUMBER){
            p -> heartBeat = HOSTHEARTBEATNUMBER;
        }
        if((p -> heartBeat <= MAXHEARTBEATNUMBER)&&(p -> heartBeat > HOSTHEARTBEATNUMBER)){
            HostNodeUdpSend(p);
        }
        //1.2 handle heartBeat logic
        //1.1 handle dumpTime out
        i=(int)(time(NULL)-(p -> dumpTime));
        if(i > MAXDUMPTIME){
            struct listeningNode *data = RBTSearch(&listeningNodeRoot,p -> DTUID);
            if (data) {
                rb_erase(&data->node, &listeningNodeRoot);
                RBTFree(data);
                data = NULL;
            }
         }
        //1.1 handle dumpTime out
    }
	return 1;
}

struct listeningNode * RBTSearch(struct rb_root *root, uint64_t dtuid)
{
    struct rb_node *node = root->rb_node;

    while (node) {
        struct listeningNode *data = container_of(node, struct listeningNode, node);

        if (dtuid < data->DTUID)
            node = node->rb_left;
        else if (dtuid > data->DTUID)
            node = node->rb_right;
        else
            return data;
    }
    return NULL;
}

int RBTInsert(struct rb_root *root, struct listeningNode *data)
{
    struct rb_node **new = &(root->rb_node), *parent = NULL;

    /* Figure out where to put new node */
    while (*new) {
        struct listeningNode *this = container_of(*new, struct listeningNode, node);

        parent = *new;
        if (this -> DTUID > data->DTUID)
            new = &((*new)->rb_left);
        else if (this -> DTUID < data->DTUID)
            new = &((*new)->rb_right);
        else
            return 0;
    }

    /* Add new node and rebalance tree. */
    rb_link_node(&data->node, parent, new);
    rb_insert_color(&data->node, root);

    return 1;
}


