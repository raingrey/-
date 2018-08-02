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

/*add node to meter data save node */
/*add node to meter data save node */
/*add node to meter data save node */
char AddTimestampToMeterDataPrimarySecondaryNode(int ord,
                                                 meterDataSecondary * p1,
                                                 meterDataPrimary * p,
                                                 uint8_t * t,
                                                 uint8_t i);
char AddLongToMeterDataPrimaryLink(int ord,meterDataPrimary * p,unsigned long int k);
char AddDoubleToMeterDataPrimaryLink(int ord,meterDataPrimary * p,double k);
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
int GetMeterID(udpMsg * p,uint8_t * dtuidstrtmp,uint8_t addrstrtmp);
//generate or updata listeningNode's deviceNode link from mysql
deviceNode * CreateDeviceNodeCircleFromMysql(uint64_t dtuid);
//generate or updata deviceNode's modbusRegisterInfo link from mysql
modbusRegisterInfo * CreateModBusRegisterInfoCircleFromMysql(deviceNode *p1);
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
    uint8_t modbusdatabuffer[MODBUSDATAUNITSIZEMAX]= {0};

    //used to make modbus data back to it's original
    unsigned short modbusdataushort=0;
    unsigned int modbusdatauint=0;
    unsigned long long modbusdataulong=0;
    float modbusdatafloat = 0;
    double modbusdatadouble= 0;
    //used to make modbus data back to it's original

    //signature for data belong
    char primarydatasign = 0;
    char secondarydatasign = 0;

    //temp data
	int i=0;
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
		while(!(p = UdpMsgNodeWaitingForHandle())){
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
			if((p1 = (listeningNode*)malloc(sizeof(listeningNode))) == NULL){
				printf("listeningNode out of memory");
				//if thread resume back to while(1)
				continue;
			}
#ifdef DEBUG_outofmemory
			//测试内存溢出的问题
			printf("++LNode堆缓存计数%d\n",++memory_node_counter_LNode);
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
//2.2 udp data and listeningNode is ready,now we will handle it
//2.2 udp data and listeningNode is ready,now we will handle it

//2.2.1 handle heart beat data
		if(!memcmp(p -> msg+DTUIDSIZE,HEARTBEATSTR,sizeof(HEARTBEATSTR))){
			//if there is a listeningNode,count it's heart beat
			p1 -> heartBeat++;
			free(p);
#ifdef DEBUG_outofmemory
			printf("--udpmsg缓存个数：%d\n",--memory_node_counter_udpmsg);
#endif
			p=NULL;
            p1 -> dumpTime=time(NULL);
			//if thread resume back to while(1)
			continue;
		}
//2.2.1 manage heart beat data

        //对于有效ModBus仪表数据，在DTU+2指针取得肯定是ModBus消息长度，+5是将ModBus地址、功能码、长度码、CRC校验码算入
        imodbus=*(p -> msg + DTUIDSIZE + 2)+ 5;

        //place modbus data
		memset(modbusstrtmp,0,sizeof(modbusstrtmp));
		memcpy(modbusstrtmp,p -> msg+DTUIDSIZE,imodbus); 

		memset(&mbds,0,sizeof(mbds));
        SplitModBusData(&mbds,&modbusstrtmp[0],imodbus);

//2.2.2 remove situation CRC16 check failed
        if(mbds.CRC!=ModBusCRC16(modbusstrtmp,imodbus-MODBUSCRCDATASIZE)){
			p1 -> heartBeat++;
			if(p!=NULL){
				free(p);
#ifdef DEBUG_outofmemory
			printf("--udpmsg缓存个数：%d\n",--memory_node_counter_udpmsg);
#endif
				p = NULL;
			}
            p1 -> dumpTime=time(NULL);
			//if thread resume back to while(1)
			continue;
        }
//2.2.2 remove situation CRC16 check failed
//2.2 udp data and listeningNode is ready,now we will handle it
//2.2 udp data and listeningNode is ready,now we will handle it
//2.2 udp data and listeningNode is ready,now we will handle it

#ifdef DEBUG
dataprocessconter++;
printf("\ndataprocessconter: %d ,DTUID: %ld --- heartBeatNumber: %d,imodbus:%d\n\n",dataprocessconter,p1->DTUID,p1->heartBeat,imodbus);
#endif

//2.3 from now udpMsg is confirmed have device address,so i can identify it's meterID\startRegister*/
//2.3 from now udpMsg is confirmed have device address,so i can identify it's meterID\startRegister*/
//2.3 from now udpMsg is confirmed have device address,so i can identify it's meterID\startRegister*/

//2.3.1 get meter id
        meterid=GetMeterID(p,dtuidstrtmp,mbds.addr);
		//check if meter id is correct
        if(meterid == 0){
			//save it's IP address
			//zero it's dumptime
			p1 -> heartBeat++;
			if(p != NULL){
				free(p);
#ifdef DEBUG_outofmemory
				printf("--udpmsg缓存个数：%d\n",--memory_node_counter_udpmsg);
#endif
				p=NULL;
			}
            p1 -> dumpTime=time(NULL);
			//if thread resume back to while(1)
			continue;
        }

#ifdef DEBUG
        printf("\n******************************************************\n"
               "current meterID = %d\n"
               "******************************************************\n",meterid);
#endif
//2.3.1 get meter id

//2.3.2 get device node from listeningNode
        //if there is a correct meterid
        dnp=GetDeviceNodeComfortMeterid(p1,meterid);
        //if RBT p1 have no device node comfort to udpMsg's meterid
        if((dnp == NULL)||!(dnp -> modbusRegisterInfoHead)){
            if(dnpp =CreateDeviceNodeCircleFromMysql(p1 -> DTUID)){
                FreeDeviceNodeCircle(p1 -> headDevice);
                p1 -> headDevice = dnpp;
                dnpp = NULL;
            }else{
                printf("\nAlarm:mysql have no device node info about DTUID %ld",p1 -> DTUID);
				//zero it's dumptime
				p1 -> heartBeat++;
				if(p != NULL){
					free(p);
#ifdef DEBUG_outofmemory
					printf("--udpmsg缓存个数：%d\n",--memory_node_counter_udpmsg);
#endif
					p=NULL;
                }
                p1 -> dumpTime=time(NULL);
				continue;
            }
        }
        dnp=GetDeviceNodeComfortMeterid(p1,meterid);
        if((dnp == NULL)||!(dnp -> modbusRegisterInfoHead)){
            printf("\nAlarm:mysql have no  info about meterID %d",meterid);
            //zero it's dumptime
            p1 -> heartBeat++;
            if(p != NULL){
                free(p);
#ifdef DEBUG_outofmemory
		printf("--udpmsg缓存个数：%d\n",--memory_node_counter_udpmsg);
#endif

                p=NULL;
            }
            p1 -> dumpTime=time(NULL);
            continue;
        }

//2.3.2 get device node from listeningNode
//2.3 from now udpMsg is confirmed have device address,so i can identify it's meterID\startRegister*/
//2.3 from now udpMsg is confirmed have device address,so i can identify it's meterID\startRegister*/
//2.3 from now udpMsg is confirmed have device address,so i can identify it's meterID\startRegister*/

//2.4 now we have all modbus data,manage data which is not from modbus client
//2.4 now we have all modbus data,manage data which is not from modbus client
//2.4 now we have all modbus data,manage data which is not from modbus client
//2.4.1 manage unknown data(unknown function code on modbus wire)
        //check modbus function code
        if(mbds.fC != MODBUSFUNCCODE){
		//if it is not 0x03,this driver can not deal with it.we can extend this function in next relase
			p1 -> heartBeat++;
			if(p != NULL){
				free(p);
#ifdef DEBUG_outofmemory
		printf("--udpmsg缓存个数：%d\n",--memory_node_counter_udpmsg);
#endif
				p=NULL;
			}
//			if(p1 -> MBStatus != ListeningNodeHost)
//				p1 -> MBStatus = ListeningNodeUnknownFunctionCode;
            p1 -> dumpTime=time(NULL);
            //if thread resume back to while(1)
			continue;
		}
//2.4.1manage unknown data(unknown function code on modbus wire)

//2.4.2manage modbus host data(host on modbus wire)
		//if it is 0x03,check if it is host or client
		//if data fit with host data size,check if position of byte number counter is fit with register data number
        if((imodbus == MODBUSHOSTDATASIZE)&&(mbds.bN!= 3)){
			//if p1 is host status ,change it to listening status;	
//			p1 -> MBStatus = ListeningNodeListening;
			p1 -> heartBeat=0;
			//3is the byte number when the length of modbus data is 8, this is host data,analysis it
			//get it's start register 

            if((dnp -> startRegister!=mbds.sR)){
                if(dnpp =CreateDeviceNodeCircleFromMysql(p1 -> DTUID)){
                    FreeDeviceNodeCircle(p1 -> headDevice);
                    p1 -> headDevice = dnpp;
                    dnpp = NULL;
                }else{
					//we have no need to go to sleep,host data finally go to sleep
                    printf("\nAlarm:mysql have no device node info about DTUID %ld",p1 -> DTUID);
				}
                if(dnp -> startRegister!=mbds.sR){
                    printf("\nAlarm:modbus host commond start register is not fit mysql,but listeningNode is fit with mysql,i have synchronization listeningNode with current modbus host data");
				}else{
                    printf("\nAlarm:modbus host commond start register is not fit mysql,listeningNode is not fit with mysql too,i have synchronization listeningNode with current modbus host data");
				}

			}
			if(p != NULL){
				free(p);
#ifdef DEBUG_outofmemory
		printf("--udpmsg缓存个数：%d\n",--memory_node_counter_udpmsg);
#endif
				p = NULL;
			}
            p1 -> dumpTime=time(NULL);
            //if thread resume back to while(1)
			continue;
		}
//2.4.2manage modbus host data(host on modbus wire)

//2.4.3 manage host/client confused data
		//(i have no solution for this situation,
		//maybe we can set modbus data should have timestamp
		//(timestamp is bigger than 3byte,then client data 
		//is bigger than 8))
        if((imodbus == MODBUSHOSTDATASIZE)&&(mbds.bN== 3)){
			p1 -> heartBeat++;
			if(p != NULL){
				free(p);
#ifdef DEBUG_outofmemory
		printf("--udpmsg缓存个数：%d\n",--memory_node_counter_udpmsg);
#endif
				p = NULL;
			}
            p1 -> dumpTime=time(NULL);
			continue;
		}
//2.4.3 manage host/client confused data

//2.4 now we have all modbus data,manage data which is not from modbus client
//2.4 now we have all modbus data,manage data which is not from modbus client
//2.4 now we have all modbus data,manage data which is not from modbus client


//2.5 handle client data,all bad situation had removed,i can sure this is client message,
//2.5 handle client data,all bad situation had removed,i can sure this is client message,
//2.5 handle client data,all bad situation had removed,i can sure this is client message,
//@2017/0803/23:16
//@raingrey
//2.5.1 prepare
		i=0;
		//check if received modbus byte num is comfort to mysql's register info
        pmbri = dnp -> modbusRegisterInfoHead;
        do{
            i += pmbri -> bytenum;
            pmbri = pmbri -> next;
            if(i == mbds.bN)
                break;
        }while(dnp -> modbusRegisterInfoHead != pmbri);

		//if there is some data broken;
        if(i!=mbds.bN){
            printf("\nAlarm:modbus data has broken,not comfort to mysql's register info for meterid:%d 's data analsys",meterid);
			p1 -> heartBeat = 0;
//			if(p1 -> MBStatus != ListeningNodeHost)
//				p1 -> MBStatus = ListeningNodeListening;
			if(p != NULL){
				free(p);
#ifdef DEBUG_outofmemory
		printf("--udpmsg缓存个数：%d\n",--memory_node_counter_udpmsg);
#endif
				p=NULL;
			}
            p1 -> dumpTime=time(NULL);
			//if thread resume back to while(1)
			continue;
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
			printf("++datasave堆缓存计数%d\n",++memory_node_counter_datasave);
#endif
			//测试内存溢出的问题
	
	}
        if(meterdatasecondary)	memset(meterdatasecondary,0,sizeof(meterDataSecondary));
        else {
		meterdatasecondary = (meterDataSecondary*)malloc(sizeof(meterDataSecondary));
		//测试内存溢出的问题
#ifdef DEBUG_outofmemory
		printf("++datasave堆缓存计数%d\n",++memory_node_counter_datasave);
#endif
		//测试内存溢出的问题
	}
        meterdataprimary -> meterID = meterid;
        meterdatasecondary -> meterID = meterid;
//2.5.1 prepare
//
//2.5.2 handle it one by one
        do{
            modbusdatauint=0;
            modbusdataulong=0;
            modbusdataushort=0;
            modbusdatafloat=0;
            modbusdatadouble=0;
			memset(modbusdatabuffer,0,sizeof(modbusdatabuffer));
			if(pmbri -> ord <= ORDERPRIMARYNUMBER)
				primarydatasign = 1;
			else if(pmbri -> ord > ORDERPRIMARYNUMBER)
				secondarydatasign = 1;
            k=pmbri -> ord;
            switch(pmbri -> datatype){
			//ushort data handler
			case MODBUSREGISTERDATATYPEUSHORT :
                modbusdataushort=modbusdataushort+mbds.mBD[i]*0x100+mbds.mBD[i+1];
                AddLongToMeterDataPrimaryLink(k,meterdataprimary,modbusdataushort);
				i+=MODBUSDATAUSHORTSIZE;
			break;

			//short data handler
			case MODBUSREGISTERDATATYPESHORT :
                modbusdataushort=mbds.mBD[i]*0x100+mbds.mBD[i+1];
                AddLongToMeterDataPrimaryLink(k,meterdataprimary,*((short *)(&modbusdataushort)));
                i+=MODBUSDATAUSHORTSIZE;
			break;
			case MODBUSREGISTERDATATYPEUINT:
                modbusdatauint=mbds.mBD[i]*0x1000000+mbds.mBD[i+1]*0x10000+mbds.mBD[i+2]*0x100+mbds.mBD[i+3];
                AddLongToMeterDataPrimaryLink(k,meterdataprimary,modbusdatauint);
                i+=MODBUSDATAUINTSIZE;
			break;
			case MODBUSREGISTERDATATYPEINT:
                modbusdatauint=mbds.mBD[i]*0x1000000+mbds.mBD[i+1]*0x10000+mbds.mBD[i+2]*0x100+mbds.mBD[i+3];
                AddLongToMeterDataPrimaryLink(k,meterdataprimary,*((int *)(&modbusdatauint)));
                i+=MODBUSDATAINTSIZE;
			break;
			case MODBUSREGISTERDATATYPEFLOAT:
                modbusdatauint=mbds.mBD[i]*0x1000000+mbds.mBD[i+1]*0x10000+mbds.mBD[i+2]*0x100+mbds.mBD[i+3];
                modbusdatafloat=*((float *)(&modbusdatauint));
                AddDoubleToMeterDataPrimaryLink(k,meterdataprimary,(double)modbusdatafloat);
				i+=MODBUSDATAFLOATSIZE;
			break;
			case MODBUSREGISTERDATATYPEDOUBLE:
                modbusdataulong=
                        mbds.mBD[i] * 0x100000000000000+
                        mbds.mBD[i+1]*0x1000000000000+
                        mbds.mBD[i+2]*0x10000000000+
                        mbds.mBD[i+3]*0x100000000+
                        mbds.mBD[i+4]*0x1000000+
                        mbds.mBD[i+5]*0x10000+
                        mbds.mBD[i+6]*0x100+
                        mbds.mBD[i+7];
                modbusdatadouble=*((double *)(&modbusdataulong));
                AddDoubleToMeterDataPrimaryLink(k,meterdataprimary,modbusdatadouble);
				i+=MODBUSDATADOUBLESIZE;
			break;
            case MODBUSREGISTERDATATYPETIMESTAMP:
                AddTimestampToMeterDataPrimarySecondaryNode(k,
                                                            meterdatasecondary,
                                                            meterdataprimary,
                                                            &(mbds.mBD[i]),
                                                            pmbri->bytenum);
                i+= pmbri->bytenum;
            break;


			case MODBUSREGISTERDATATYPEUCHAR2:
                memcpy(modbusdatabuffer,&(mbds.mBD[i]),MODBUSDATAUCHAR2);
                AddCharToMeterDataSecondaryLink(k,meterdatasecondary,modbusdatabuffer,MODBUSDATAUCHAR2);
				i+=MODBUSDATAUCHAR2;
			break;

			//
			//
			//char4-char10 could be timestamp so we should check it
			case MODBUSREGISTERDATATYPEUCHAR4:
                memcpy(modbusdatabuffer,&(mbds.mBD[i]),MODBUSDATAUCHAR4);
                AddCharToMeterDataSecondaryLink(k,meterdatasecondary,modbusdatabuffer,MODBUSDATAUCHAR4);
                AddTimestampToMeterDataPrimarySecondaryNode(pmbri -> ord,
                                                            meterdatasecondary,
                                                            meterdataprimary,
                                                            modbusdatabuffer,
                                                            MODBUSDATAUCHAR4);
				i+=MODBUSDATAUCHAR4;
			break;

			case MODBUSREGISTERDATATYPEUCHAR6:
                memcpy(modbusdatabuffer,&(mbds.mBD[i]),MODBUSDATAUCHAR6);
                AddCharToMeterDataSecondaryLink(k,meterdatasecondary,modbusdatabuffer,MODBUSDATAUCHAR6);
                AddTimestampToMeterDataPrimarySecondaryNode(pmbri -> ord,
                                                            meterdatasecondary,
                                                            meterdataprimary,
                                                            modbusdatabuffer,
                                                            MODBUSDATAUCHAR6);
				i+=MODBUSDATAUCHAR6;
			break;

            case MODBUSREGISTERDATATYPEUCHAR8:
                memcpy(modbusdatabuffer,&(mbds.mBD[i]),MODBUSDATAUCHAR8);
                AddCharToMeterDataSecondaryLink(k,meterdatasecondary,modbusdatabuffer,MODBUSDATAUCHAR8);
                AddTimestampToMeterDataPrimarySecondaryNode(pmbri -> ord,
                                                            meterdatasecondary,
                                                            meterdataprimary,
                                                            modbusdatabuffer,
                                                            MODBUSDATAUCHAR8);
                i+=MODBUSDATAUCHAR8;
            break;

            case MODBUSREGISTERDATATYPEUCHAR10:
                memcpy(modbusdatabuffer,&(mbds.mBD[i]),MODBUSDATAUCHAR10);
                AddCharToMeterDataSecondaryLink(k,meterdatasecondary,modbusdatabuffer,MODBUSDATAUCHAR10);
                AddTimestampToMeterDataPrimarySecondaryNode(pmbri -> ord,
                                                            meterdatasecondary,
                                                            meterdataprimary,
                                                            modbusdatabuffer,
                                                            MODBUSDATAUCHAR10);
                i+=MODBUSDATAUCHAR10;
            break;
			//char4-char10 could be timestamp so we should check it
			//
            //
            case MODBUSREGISTERDATATYPEUCHAR12:
                memcpy(modbusdatabuffer,&(mbds.mBD[i]),MODBUSDATAUCHAR12);
                AddCharToMeterDataSecondaryLink(k,meterdatasecondary,modbusdatabuffer,MODBUSDATAUCHAR12);
                i+=MODBUSDATAUCHAR12;
            break;

            case MODBUSREGISTERDATATYPEUCHAR14:
                memcpy(modbusdatabuffer,&(mbds.mBD[i]),MODBUSDATAUCHAR14);
                AddCharToMeterDataSecondaryLink(k,meterdatasecondary,modbusdatabuffer,MODBUSDATAUCHAR14);
                i+=MODBUSDATAUCHAR14;
            break;

            case MODBUSREGISTERDATATYPEUCHAR16:
                memcpy(modbusdatabuffer,&(mbds.mBD[i]),MODBUSDATAUCHAR16);
                AddCharToMeterDataSecondaryLink(k,meterdatasecondary,modbusdatabuffer,MODBUSDATAUCHAR16);
                i+=MODBUSDATAUCHAR16;
            break;

            case MODBUSREGISTERDATATYPEUCHAR18:
                memcpy(modbusdatabuffer,&(mbds.mBD[i]),MODBUSDATAUCHAR18);
                AddCharToMeterDataSecondaryLink(k,meterdatasecondary,modbusdatabuffer,MODBUSDATAUCHAR18);
                i+=MODBUSDATAUCHAR18;
            break;

            case MODBUSREGISTERDATATYPEUCHAR20:
                memcpy(modbusdatabuffer,&(mbds.mBD[i]),MODBUSDATAUCHAR20);
                AddCharToMeterDataSecondaryLink(k,meterdatasecondary,modbusdatabuffer,MODBUSDATAUCHAR20);
                i+=MODBUSDATAUCHAR20;
            break;

			default:
///Alarm3:this need to control mysql-> alarm table
                printf("\nunknown data type");
			break;
			}
			pmbri = pmbri -> next;
        }while((i<mbds.bN)&&(dnp -> modbusRegisterInfoHead != pmbri));
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
		MeterDataNumber++;
        }
        if(secondarydatasign){
            meterdatasecondary -> next= meterDataSecondaryHead;
            meterDataSecondaryHead = meterdatasecondary;
            meterdatasecondary=NULL;
		MeterDataNumber++;
        }
	if(MeterDataNumber>10){
		pthread_cond_signal(&condDataSave);
	}
        pthread_mutex_unlock(&data_save_mtx);
	}
//2.5.3 send meterdata to it's link
//
//2.5 handle client data,all bad situation had removed,i can sure this is client message,
//2.5 handle client data,all bad situation had removed,i can sure this is client message,
//2.5 handle client data,all bad situation had removed,i can sure this is client message,
	return 0;
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
	if(!dnp)    return dnp;
	while((dnp)){
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

int GetMeterID(udpMsg * p,uint8_t * dtuidstrtmp,uint8_t addrstrtmp){
	uint8_t querystrtmp[MYSQLQUERYSTRSIZE] = {0};
	int meterid=0;
	sprintf(querystrtmp,"select meterID from MeterIdentify where DTUID=%s and deviceNumber=%d",dtuidstrtmp,addrstrtmp);
	MYSQL_RES * mysqlres=NULL;
	MYSQL_ROW mysqlrow;
	if(mysql_real_query(sql,querystrtmp,strlen(querystrtmp))){
        printf("\n%s\n",querystrtmp);
        printf("\nfailed to query mysql when handle %s,mysql,dtuid:%s,addrstrtmp:%d, error msg%s\n",p -> msg,dtuidstrtmp,addrstrtmp,mysql_error(sql));
		return MYSQLFAILED;
	}
	if(!(mysqlres=mysql_store_result(sql))){
        printf("\nfailed to get result of  mysql when handle %s,mysql,dtuid:%s,addrstrtmp:%d, error msg%s\n",p -> msg,dtuidstrtmp,addrstrtmp,mysql_error(sql));
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

modbusRegisterInfo * CreateModBusRegisterInfoCircleFromMysql(deviceNode * p1){

    uint8_t querystrtmp[MYSQLQUERYSTRSIZE] = {0};
    /// get  of this device from mysql
    MYSQL_RES * mysqlres=NULL;
    MYSQL_ROW mysqlrow;
    char i=0;
    modbusRegisterInfo * mp1=NULL;
    modbusRegisterInfo * mp2=NULL;
    time(&LocalTime);
    /// get modbusRegisterInfo from mysql
    /// get modbusRegisterInfo from mysql
    /// get modbusRegisterInfo from mysql
    sprintf(querystrtmp,"select ord,address,byteNumber,dataType from DataIdentify where meterID=%d order by address ASC,ord ASC",p1 -> meterID);
	if(mysql_real_query(sql,querystrtmp,strlen(querystrtmp))){
        printf("\n%sfailed to query mysql when handle deviceNode meterid:%d,deviceNumber:%d, error msg%s\n",ctime(&LocalTime),p1 -> meterID,p1 -> deviceNumber,mysql_error(sql));
        return NULL;
	}
	if(!(mysqlres=mysql_store_result(sql))){
        printf("\n%s failed to get result of mysql when handle deviceNode meterid:%d,deviceNumber:%d, error msg%s\n",ctime(&LocalTime),p1 -> meterID,p1 -> deviceNumber,mysql_error(sql));
		mysql_free_result(mysqlres);
		mysqlres = NULL;
        return NULL;
	}
    /// get modbusRegisterInfo from mysql
    /// get modbusRegisterInfo from mysql
    /// get modbusRegisterInfo from mysql


    /// create modbusRegisterInfo circle
    /// create modbusRegisterInfo circle
    /// create modbusRegisterInfo circle
    while(mysqlrow=mysql_fetch_row(mysqlres)){
        mp2 = (modbusRegisterInfo *)malloc(sizeof(modbusRegisterInfo));
#ifdef DEBUG_outofmemory
	printf("++ModBusRegisterInfo堆缓存计数%d\n",++memory_node_counter_MBRI);
#endif
        memset(mp2,0,sizeof(modbusRegisterInfo));
        //get modbusRegisterInfo
        mp2 -> ord = atoi(mysqlrow[0]);
        mp2 -> addr= atoi(mysqlrow[1]);
        mp2 -> bytenum= atoi(mysqlrow[2]);
        mp2 -> datatype= atoi(mysqlrow[3]);
        if(MYSQLSTARTREGISTERORD == mp2 -> ord){
            //startregister could be 0;don't check it by 0;
            p1 -> startRegister=mp2 -> addr;
            //one result for per modbusbytenumber per device
        }
        if(mp1){
            mp1 -> next = mp2;
        }else{
            mp1 = mp2;
        }
    }
    /// create modbusRegisterInfo circle
    /// create modbusRegisterInfo circle
    /// create modbusRegisterInfo circle

    /// send circle back or alarm
    /// send circle back or alarm
    /// send circle back or alarm
    if(mysqlres!=NULL){
        mysql_free_result(mysqlres);
        mysqlres = NULL;
    }
    if(!mp1){
        printf("\n %s Alarm:mysql have no device node Regisger info about meterID %d\n",ctime(&LocalTime),p1 -> meterID);
    }
    /// send circle back or alarm
    /// send circle back or alarm
    /// send circle back or alarm
    return mp1;
}

deviceNode * CreateDeviceNodeCircleFromMysql(uint64_t dtuid){
    uint8_t querystrtmp[MYSQLQUERYSTRSIZE] = {0};
    uint8_t i=0;
    deviceNode * p1 = NULL;
    deviceNode * p2 = NULL;
    modbusRegisterInfo * mp1 = NULL;
    /// get  of this device from mysql
	MYSQL_RES * mysqlres=NULL;
	MYSQL_ROW mysqlrow;
    time(&LocalTime);

    /// get device node data from mysql
    /// get device node data from mysql
    /// get device node data from mysql
    sprintf(querystrtmp,
            "select deviceNumber,meterID from MeterIdentify where DTUID=%ld",
            dtuid);
	if(mysql_real_query(sql,querystrtmp,strlen(querystrtmp))){
        printf("\n%s failed to query mysql when handle listeningNode DTUID:%ld,error msg%s\n",
               ctime(&LocalTime),dtuid,mysql_error(sql));
        return NULL;
	}
	if(!(mysqlres=mysql_store_result(sql))){
        printf("\nfailed to get result of mysql when handle listeningNode DTUID:%d,error msg%s\n",dtuid,mysql_error(sql));
		if(mysqlres!=NULL){
            mysql_free_result(mysqlres);
			mysqlres = NULL;
		}
        return NULL;
	}
    /// get device node data from mysql
    /// get device node data from mysql
    /// get device node data from mysql


    /// create device node circle
    /// create device node circle
    /// create device node circle
    while(mysqlrow=mysql_fetch_row(mysqlres)){
        //deviceNode: apply memory and init to circle
        //这里不能轻率的申请一段内存，而是应该检查meterID是否重复
        //重复的话可直接跳过，
        //如果不重复的话可加入
        //如果走完以后，！！存在！！没加入的内存，还要释放内存
        p2 = (deviceNode *)malloc(sizeof(deviceNode));
#ifdef DEBUG_outofmemory
	printf("++deviceNode堆缓存计数%d\n",++memory_node_counter_DN);
#endif
        memset(p2,0,sizeof(deviceNode));
        p2 -> deviceNumber = atoi(mysqlrow[0]);
        p2 -> meterID = atoi(mysqlrow[1]);
        if(mp1 = CreateModBusRegisterInfoCircleFromMysql(p2)){
            p2 -> modbusRegisterInfoHead = mp1;
        }else{
            p2 -> modbusRegisterInfoHead = NULL;
            printf("\n%s Alarm: No ModBusRegisterInfo founded when create modbusRegisterInfo circle for deviceNode %d",ctime(&LocalTime),p2 -> meterID);
        }

        if(!p1){
            p1 = p2;
	    p2=NULL;
        }else{
            p1 -> next = p2;
            p1 = p2;
	    p2=NULL;
        }

    }
    /// create device node circle
    /// create device node circle
    /// create device node circle
    ///
    /// send circle back or alarm
    /// send circle back or alarm
    /// send circle back or alarm
    mysql_free_result(mysqlres);
    mysqlres = NULL;
    if(!p1){
        printf("\n %s Alarm:mysql have no device node about DTUID %ld",ctime(&LocalTime),dtuid);
    }
    /// send circle back or alarm
    /// send circle back or alarm
    /// send circle back or alarm
    return p1;
}


char AddTimestampToMeterDataPrimarySecondaryNode(int ord, meterDataSecondary * p1,
                                                 meterDataPrimary * p, uint8_t *t, uint8_t i){
    if(ord == ORDERTIMESTAMP){
        if(i>10) return 0;
        memcpy(p -> timestamp,t,i);
        memcpy(p1 -> timestamp,t,i);
    }
}
/*return:
 * 1	ok;
 * 0	mission failed
 * */
char AddLongToMeterDataPrimaryLink(int ord,meterDataPrimary * p,unsigned long int k){
    switch(ord){
    case ORDERINSTANTFLOW :
        p -> instantFlow = (float)k;
        break;
    case ORDERTOTALFLOW :
        p -> totalFlow = (unsigned long int)k;
        break;
    case ORDERT:
        p -> T= (float)k;
        break;
    case ORDERP:
        p -> P= (float)k;
        break;
    case ORDERDP:
        p -> DP= (float)k;
        break;
    default:
        return 0;
        break;
    }
    return 1;
}
/*return:
 * 1	ok;
 * 0	mission failed
 * */
char AddDoubleToMeterDataPrimaryLink(int ord,meterDataPrimary * p,double k){
    switch(ord){
    case ORDERINSTANTFLOW :
        p -> instantFlow = (float)k;
        break;
    case ORDERTOTALFLOW :
        p -> totalFlow = (long int)k;
        break;
    case ORDERT:
        p -> T= (float)k;
        break;
    case ORDERP:
        p -> P= (float)k;
        break;
    case ORDERDP:
        p -> DP= (float)k;
        break;
    default:
        return 0;
        break;
    }
    return 1;
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
        while(p ->modbusRegisterInfoHead!= p1 -> next){
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
		if(p == tree -> headDevice)
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
#ifdef DEBUG_outofmemory
	printf("--deviceNode堆缓存计数%d\n",--memory_node_counter_DN);
#endif
	p1=p2
        p2 = NULL;
    }

}

uint8_t FreeModBusRegisterInfoCircle(modbusRegisterInfo * p1){

    modbusRegisterInfo * p2 = NULL;
    //free deviceNode -> modbusRegisterInfo circle
    while((p1)){
        p2 = p1 -> next;
        free(p1);
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


