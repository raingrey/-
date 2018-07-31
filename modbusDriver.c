#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include "rbtree.h"
#include "dataProcess.h"
#include "dataSave.h"
#include "modbusDriver.h"





/* CRC Check for ModBus
 * CRC-16 x^16+x^15+x^2+x^0
 * result code sequence	big-endian
 * initialize code 0xffff
 * xor code 0x0000
 * simple table and easy function
 *  * */
const unsigned char auchCRCHi[]=
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
};
const unsigned char auchCRCLo[] =
{
0x00,0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};
uint32_t  ModBusCRC16(unsigned char *updata,unsigned int len);
/* CRC Check for ModBus
 * CRC-16 x^16+x^15+x^2+x^0
 * result code sequence	big-endian
 * initialize code 0xffff
 * xor code 0x0000
 * simple table and easy function
 *  * */

/*thread things*/
/*thread things*/
/*thread things*/
//thread lock sign
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t data_save_mtx = PTHREAD_MUTEX_INITIALIZER;

//base thread number thread manager
pthread_t ThreadFixedManager=0;
//base thread number thread data save
pthread_t ThreadFixedDataSave=0;
//base thread number thread process
pthread_t ThreadFixedDataProcess=0;

//thread wait/wakeup signature-ThreadLNmanager
pthread_cond_t condLNManager = PTHREAD_COND_INITIALIZER;
//thread wait/wakeup signature-ThreadDataProcess
pthread_cond_t condDataProcess = PTHREAD_COND_INITIALIZER;
//thread wait/wakeup signature-ThreadDataSave
pthread_cond_t condDataSave = PTHREAD_COND_INITIALIZER;
/*thread things*/
/*thread things*/
/*thread things*/

/**data structure things*/
/**data structure things*/
/**data structure things*/
//触发处理线程检查RBT的心跳和离线
int RBTListeningNodeNumber=0;
//save the number of current UdpMsgNumber
int UdpMsgNumber = 0;
//save the number of current MeterDataToSave
int MeterDataNumber= 0;



//udpMsgHead is head node of this two-way link-list
udpMsg * udpMsgHead=NULL;
//meterDataPrimaryHead is head node of this two-way link-list
meterDataPrimary * meterDataPrimaryHead=NULL;
//meterDataSecondaryHead is head node of this two-way link-list
meterDataSecondary * meterDataSecondaryHead=NULL;
/**data structure things*/
/**data structure things*/
/**data structure things*/


/**mysql things*/
/**mysql things*/
/**mysql things*/
//mysql descripter for dataProcess
MYSQL *sql;
//mysql descripter for dataSave
MYSQL *sqlDataSave;
/**mysql things*/
/**mysql things*/
/**mysql things*/

/**socket things*/
/**socket things*/
/**socket things*/
//socket descripter of server
int serv_sock;
/**socket things*/
/**socket things*/
/**socket things*/


void InsertToUdpMsgLink(udpMsg * p);


/**business logic and data structrue things*/
/**business logic and data structrue things*/
/**business logic and data structrue things*/
//thread manager of thread dataprocess datasave
//and also triger the RBT check
//and also triger the datastruct check
void * ThreadLNManager();
//check and maintain RBT
//and also triger HostNodeUdpSend
char RBTListeningNodeCheck();
//maintain two way link list
char CheckLinkList();
//when dtu is used as host in modbus line,we should send host data
char HostNodeUdpSend();
/**business logic and data structrue things*/
/**business logic and data structrue things*/
/**business logic and data structrue things*/

uint8_t FreeModBusRegisterInfoCircle(modbusRegisterInfo * p1);
uint8_t FreeDeviceNodeCircle(deviceNode * p1);



struct rb_root listeningNodeRoot = RB_ROOT;

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
        /*while((node -> headDevice)){
            //free deviceNode -> modbusRegisterInfo circle

            FreeModBusRegisterInfoCircle((node -> headDevice -> modbusRegisterInfoHead));

            while((node -> headDevice -> modbusRegisterInfoHead));
            {
                p2 = node -> headDevice-> modbusRegisterInfoHead -> next -> next;
                if(node -> headDevice -> modbusRegisterInfoHead == p2){
                    free(p2);
                    p2 = NULL;
                    node -> headDevice -> modbusRegisterInfoHead = NULL;
                }else{
                    free(node -> headDevice -> modbusRegisterInfoHead -> next);
                    node -> headDevice -> modbusRegisterInfoHead -> next = p2;
                }
            }
            //free deviceNode circle
            p1 = node -> headDevice-> next -> next;
            if(node -> headDevice == p1){
                free(p1);
                p1 = NULL;
                node -> headDevice = NULL;
            }else{
                free(node -> headDevice -> next);
                node -> headDevice -> next = p1;
            }
        }*/
        free(node);
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
        p2 = p1-> next -> next;
        if(p2 == p1){
            free(p2);
            p1 = NULL;
            p2 = NULL;
        }else{
            free(p1 -> next);
            p1 -> next = p2;
        }
    }

}

uint8_t FreeModBusRegisterInfoCircle(modbusRegisterInfo * p1){

    modbusRegisterInfo * p2 = NULL;
    //free deviceNode -> modbusRegisterInfo circle
    while((p1)){
        p2 = p1 -> next -> next;
        if(p1 == p2){
            free(p2);
            p2 = NULL;
            p1 = NULL;
        }else{
            free(p1 -> next);
            p1 -> next = p2;
        }
    }
    return 1;
}
/**
 * argv[1] is server porb
 *STEP:
 * 1.initilize mysql
 * 2.connect mysql
 * 3.initialize server socket
 * 4.initialize server socket address
 * 5.bind server socket address to server socket
 * 6.initialize recvfrom() function, program blocking and wait for udp messages
 * 7.show message
 * 8.add messges to UdpMsgNode
 * back to step6
 * 9.close socket
 * 10.close mysql**/

int main(int argc,char *argv[])
{
//default server port for udp correspond
	unsigned short int server_port = 3389;
//server Address
	struct sockaddr_in serv_addr;
//client Address
	struct sockaddr_in client_addr;
//length of client address 
	socklen_t client_addr_size;
//buffer for udp message
//	char messages[BUFF_SIZE]={};
	char user[20]={"root"};
	char passwd[30]={"ytfy1032744819"};
	char database_name[30]={"tcp"};

//argc-------count the unit of argv[]
	if((argc==2)||(argc==5)){
		server_port=atoi(argv[1]);
		printf("端口号：%d\n",server_port);
	}else{
		printf("默认端口3389\n");
		server_port=3389;
	}
	if(argc==3){
		strcpy(user,argv[2]);
		strcpy(passwd,argv[3]);
		printf("数据库用户名、密码已收到\n");
	}else if((argc==4)){
		strcpy(user,argv[2]);
		strcpy(passwd,argv[3]);
		strcpy(database_name,argv[4]);
		printf("数据库用户名、密码、数据库名已收到1\n");
//alarm that lack of server port number
	}else if(argc==5){
		strcpy(user,argv[3]);
		strcpy(passwd,argv[4]);
		strcpy(database_name,argv[5]);
		printf("数据库用户名、密码、数据库名已收到2\n");
	}else{
		printf("使用默认数据库");
	}
//initlize mysql
	sql=mysql_init(sql);
//connect to mysqlSendBackUdpMsgNode
	while(!mysql_real_connect(sql,"localhost",user,passwd,database_name,0,NULL,0)){
		puts("Error connecting");
		printf("%s\n",mysql_error(sql));
		printf("请输入数据库用户名");
		scanf("%s",user);
		printf("请输入数据库密码");
		scanf("%s",passwd);
		printf("请输入所用数据库名");
		scanf("%s",database_name);
	}
	if(mysql_real_query(sql,"set names utf8",14)){
		printf("%s\n",mysql_error(sql));
		exit(0);
	}

//initlize mysqlDataSave
	sqlDataSave=mysql_init(sqlDataSave);
//connect to mysqlDataSave
	if(!mysql_real_connect(sqlDataSave,"localhost",user,passwd,database_name,0,NULL,0)){
		puts("Error connecting");
		printf("%s\n",mysql_error(sqlDataSave));
		exit(0);
	}else{
		printf("Mysql connection build!\n");
	}
	if(mysql_real_query(sqlDataSave,"set names utf8",14)){
		printf("%s\n",mysql_error(sqlDataSave));
		exit(0);
	}



//set protocol and connect type of server
	serv_sock=socket(PF_INET,SOCK_DGRAM,0);
//check if it works
	if(serv_sock==-1){
		printf("socket() error");
		exit(0);
	}
//initialize memory space of server_addr
	memset(&serv_addr,0,sizeof(serv_addr));

//set protocol IPAddress and port 
	serv_addr.sin_family=AF_INET;
//any ip address is allowed
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
//set server port 
	serv_addr.sin_port=htons(server_port);
	printf("监听端口%d\n",ntohs(serv_addr.sin_port));
//bind serv_addr info to serv_sock
	if(bind(serv_sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr))==-1){
		printf("bind() error");
	}


    /*******************************************************************
	//initialize listeningNode tree;
    if((listeningNodeRoot -> root =(listeningNode *)malloc(sizeof(listeningNode)))==NULL){
        printf("\nlisteningNode -> root Node out of memory");
        return 0;
    }
    memset(listeningNodeRoot -> root,0,(sizeof(listeningNode)));
    listeningNodeRoot -> root -> color = BLACK;
    listeningNodeRoot -> root -> dumpTime= time(NULL);
    ********************************************************/
	//initialize udpMsg two-way link-list
	//check and malloc memory for head node
	if((udpMsgHead=(udpMsg *)malloc(sizeof(udpMsg)))==NULL){
		printf("udpMsg's Head Node out of memory");
		return 0;
	}
	memset(udpMsgHead,0,(sizeof(udpMsg)));
	//link the head node to a circle
	udpMsgHead -> next = udpMsgHead;
	//initialize udpMsg two-way link-list
	
	//create a temp udpMsg Node point 
	udpMsg * msg=NULL;


    //crea--------------------+-------te thread pool
	//2.1create a thread for udp data process;
	pthread_create(&ThreadFixedDataProcess,NULL,ThreadDataProcess,NULL);
	//3.create a thread for save data to mysql;
	pthread_create(&ThreadFixedDataSave,NULL,ThreadDataSave,NULL);
	//1.create a thread manage listeningNode;
	pthread_create(&ThreadFixedManager,NULL,ThreadLNManager,NULL);
	//get sizeof client IPAddress for function recvfrom()
	client_addr_size=sizeof(client_addr);

	//ip address for print out
	char* IPaddress=NULL;
	long udpmsgcounter=0;
	uint32_t i;
	while(1){
		if(!(msg||msg=(udpMsg *)malloc(sizeof(udpMsg))){
			printf("udp listeningNode out of memory");
			continue;
		}
		memset(msg,0,(sizeof(udpMsg)));

//listen and accept a request,program will blocking here
		if(recvfrom(serv_sock,(char*)(msg->msg),BUFF_SIZE,0,
			(struct sockaddr *)&client_addr,&client_addr_size)!=-1){
			IPaddress=inet_ntoa(client_addr.sin_addr);
			 udpmsgcounter++;
			printf("Message from %s ,udpmsgcounter:%ld,message: ",IPaddress,udpmsgcounter);
			// for(i=0;i<sizeof(messages);i++)
			//    printf("%x,",messages[i]);
			printf("\n\n,");
			msg -> clientAddr=client_addr;
			
			for(i=0;i<DTUIDSIZE;i++)
				if(msg->msg[i]<'0'||msg->msg[i]>'9')
						continue;
			//insert to two-way link-list's head
			pthread_mutex_lock(&mtx);
			msg -> next = udpMsgHead -> next;
			udpMsgHead -> next = msg;
			UdpMsgNumber++;
			pthread_mutex_unlock(&mtx);
			msg=NULL;
			if(UdpMsgNumber>10){
				pthread_cond_signal(&condDataProcess);
			}
        }
    }
//	pthread_detach(t_id);
	close(serv_sock);
	mysql_close(sql);
	return 0;
}
void InsertToUdpMsgLink(udpMsg * p){
	p -> next = udpMsgHead -> next;
    udpMsgHead -> next = p;
	UdpMsgNumber++;
    return;
}
void * ThreadLNManager(){

	while(1){
//////////////thread monopoly
		RBTListeningNodeNumber=1;
        //RBTListeningNodeCheck();
		pthread_mutex_lock(&mtx);
		udpMsg * p=udpMsgHead -> next;
	//1.1 handle udpmsg two way link
		UdpMsgNumber=0;
		while((p!=udpMsgHead)){
			p = p -> next;
			UdpMsgNumber++;
		}
		if(UdpMsgNumber>0){
			pthread_cond_signal(&condDataProcess);
		}
		pthread_mutex_unlock(&mtx);
	//1.1 handle udpmsg two way link
	//
		
		printf("ThreadLNManager将要-----------获取------------锁data_save_mtx\n");
		pthread_mutex_lock(&data_save_mtx);
		printf("ThreadLNManager-----------获取锁成功------------data_save_mtx\n");
		meterDataPrimary * p1 = meterDataPrimaryHead;
		meterDataSecondary * p2 = meterDataSecondaryHead;
	//1.2 handle meterdataprimary and meterdatasecondary two way link
		MeterDataNumber=0;
		while((p1)){
			MeterDataNumber++;
			if(p1->next)
				p1 = p1 -> next;
			else
				break;
		}
	//1.2.1 handle meterdatasecondary two way link
		while((p2)){
			MeterDataNumber++;
			if(p2 -> next)
				p2 = p2 -> next;
			else
				break;
		}
		printf("ThreadLNManager将要-----------释放-----------锁data_save_mtx\n");
		pthread_mutex_unlock(&data_save_mtx);
		if(MeterDataNumber>0){
			pthread_cond_signal(&condDataSave);
		}
		
//		ThreadAdjust();
//////////////thread monopoly
///
/// sleep control the frequency of HostNodeUdpSend
        sleep(5);
	}
	return NULL;
}


char CheckLinkList(){

//1.2 handle meterdataprimary and meterdatasecondary two way link
	return 1;
}

uint32_t  ModBusCRC16(unsigned char *updata,unsigned int len)
{
        unsigned char uchCRCHi=0xff;
        unsigned char uchCRCLo=0xff;
        unsigned int  uindex;		//check if current thread threadPoolInfo's cmd is THREDFREE
        while(len--)
        {
                uindex=uchCRCLo^*updata++;
                uchCRCLo=uchCRCHi^auchCRCHi[uindex];
                uchCRCHi=auchCRCLo[uindex];
        }
        return (uchCRCHi<<8|uchCRCLo);
}


/**********************************************
 * RBTListeningNodeCheck
 * early version handle MBStatus
		switch(tree -> MBStatus){
		case ListeningNodeError:

///alarm6:save message to mysql and free the node
		break;
		case ListeningNodeListening:
		RBTListeningNodeNumber++;
		break;
		case ListeningNodeHeartBeat:
		break;
		case ListeningNodeProcessing:
		break;
		case ListeningNodeLastDataIsHost:
			tree -> MBStatus = ListeningNodeListening;
		break;
		case ListeningNodeHostClientConfuse:
		break;
		case ListeningNodeUnknownFunctionCode:
		break;
		case ListeningNodeHost:
		break;
		default:
		break;
		}
*********************************************/

/*
 * RedBlack Tree test code
 * @raingrey
 * @2017/07/26
	int a[]={10,40,30,60,90,70,20,50,80};
	int i ,ilen=sizeof(a)/sizeof(a[0]);
	RBRoot * root = NULL;
	listeningNode * nod=NULL,*nod1=NULL;
	root = RBTCreateRBTree();
	printf("==原始数据:");
	for(i=0;i<ilen;i++)	printf("%d",a[i]);
	printf("\n");
	long dtua=0;
	for(i=0;i<ilen;i++){
		nod = (listeningNode *)malloc(sizeof(listeningNode));
		nod -> DTUID = a[i];
		RBTSearchAndInsert(root,nod);
		printf("==a[%d]:%d\n",i,a[i]);
		if(nod1=RBTLoopSearch(root->root,a[i]))
		printf("==insert node:%d\n",nod1-> DTUID);
	}
	for(i=0;i<ilen;i++){
		nod = (listeningNode *)malloc(sizeof(listeningNode));
		nod -> DTUID = a[i];
		RBTDelete(root,nod);
		printf("==a[%d]:%d\n",i,a[i]);
		if(nod1=RBTLoopSearch(root->root,a[i]))
			printf("==node:%d still be there\n",nod1-> DTUID);
		else
			printf("%d has deleted\n",a[i]);
	}
*/

/*********************************************************
 * early version has thread adjust
 * if need more dataProcess/dataSave,create more
*********************************************************


//thread information table
threadPoolInfo ThreadPool[MAXDATAPROCESSSAVETHREADNUMBER]={0};

//sign of data process thread is needed
char NeedDataProcessThread=NEEDNOTHREAD;
//sign of data save thread is needed
char NeedDataSaveThread=NEEDNOTHREAD;

//current thread number
char TotalThreadNumber=0;


//balace thread distribution
char ThreadAdjust();
//manage thread every xx second
void ThreadMangerControlledByTimer();

//code init time signal
//code init time signal
//code init time signal
    struct itimerval tick;
    memset(&tick,0,sizeof(tick));
    signal(SIGALRM,ThreadMangerControlledByTimer);
    //timeout to run first time
    tick.it_value.tv_sec=1;
    tick.it_value.tv_usec=0;
    //after first,the interval time for clock
    tick.it_interval.tv_sec=LNCHECKANDSENDHOSTTIMESTEP;
    tick.it_interval.tv_usec=0;
    if(setitimer(ITIMER_REAL,&tick,NULL)<0)
        printf("set timer failed!\n");
//code init time signal
//code init time signal
//code init time signal

char ThreadAdjust(){
    char i=0;

    if(TotalThreadNumber >= MAXTHREADNUMBER){
        //need urgent alarm
        printf("thread pool fill");
        for(i=0;i<MAXTHREADNUMBER;i++){
            if((NeedDataSaveThread == NEEDDATASAVETHREAD)){
                if((ThreadPool[i].threadascription == THREADASCRIPTIONDATAPROCESS)&&(ThreadPool[i].threadstatus != THREADSTATUSRUNNING)){
                    ThreadPool[i].cmd = THREADFREE;
                    pthread_cond_broadcast(&condDataProcess);
                    return FREEDATAPROCESSTHREAD;
                }
            }
            if((NeedDataProcessThread == NEEDDATAPROCESSTHREAD)){
                if((ThreadPool[i].threadascription == THREADASCRIPTIONDATASAVE)&&(ThreadPool[i].threadstatus != THREADSTATUSRUNNING)){
                    ThreadPool[i].cmd = THREADFREE;
                    pthread_cond_broadcast(&condDataSave);
                    return FREEDATASAVETHREAD;
                }
            }
        }
        if(i == MAXTHREADNUMBER) return THREADPOOLFILL;
    }
    if(NeedDataSaveThread == NEEDDATASAVETHREAD){
        pthread_create(&(ThreadPool[TotalThreadNumber-1].tid),NULL,ThreadDataSave,NULL);
        ThreadPool[TotalThreadNumber-1].threadascription = THREADASCRIPTIONDATASAVE;
        ThreadPool[TotalThreadNumber-1].threadstatus = THREADSTATUSNULL;
        ThreadPool[TotalThreadNumber-1].cmd = THREADPERSIST;
        TotalThreadNumber++;
        if(TotalThreadNumber >= MAXTHREADNUMBER){
            //need urgent alarm
            printf("thread pool fill");
            return THREADPOOLFILL;
        }
        NeedDataSaveThread == NEEDNOTHREAD;
    }

    if(NeedDataProcessThread == NEEDDATAPROCESSTHREAD){
        pthread_create(&(ThreadPool[TotalThreadNumber-1].tid),NULL,ThreadDataProcess,NULL);
        ThreadPool[TotalThreadNumber-1].threadascription = THREADASCRIPTIONDATAPROCESS;
        ThreadPool[TotalThreadNumber-1].threadstatus = THREADSTATUSNULL;
        ThreadPool[TotalThreadNumber-1].cmd = THREADPERSIST;
        TotalThreadNumber++;
        if(TotalThreadNumber >= MAXTHREADNUMBER){
            //need urgent alarm
            printf("thread pool fill");
            return THREADPOOLFILL;
        }
        NeedDataProcessThread == NEEDNOTHREAD;
    }
}

void ThreadMangerControlledByTimer(){
    if((RBTListeningNodeNumber>MEDIUMNUMBERRBTLISTENINGNODE)&&(RBTListeningNodeNumber<MAXNUMBERRBTLISTENINGNODE)){
///Alarm4:so many listeningNode,server almost dump
    printf("MEDIUMNUMBERRBTLISTENINGNODE listeningNode waiting for handle ");
//may be we can create more thread to handle udpMsg
    if(NeedDataSaveThread != NEEDDATASAVETHREAD)
        NeedDataProcessThread=NEEDDATAPROCESSTHREAD;
    }
    if((RBTListeningNodeNumber>MAXNUMBERRBTLISTENINGNODE)){
///UrgentAlarm1:too much udpMsgNode,server will dump
        printf("MAXNUMBERRBTLISTENINGNODE listeningNode waiting for handle ");
//may be we can create more thread to handle udpMsg
//but we should recycle some thread
//uncontrolled add thread will cause trouble
        NeedDataProcessThread=NEEDDATAPROCESSTHREAD;
    }
    if((UdpMsgNumber >MEDIUMNUMBERUDPMSGNODE)&&(UdpMsgNumber<MAXNUMBERUDPMSGNODE)){
///Alarm4:too much udpMsgNode,server almost dump
        printf("MEDIUMNUMBERUDPNODE udpMsg Node waiting for handle ");
//may be we can create more thread to handle udpMsg
        if(NeedDataSaveThread != NEEDDATASAVETHREAD)
            NeedDataProcessThread=NEEDDATAPROCESSTHREAD;
    }
    if((UdpMsgNumber>MAXNUMBERUDPMSGNODE)){
///UrgentAlarm1:too much udpMsgNode,server will dump
        printf("MAXNUMBERUDPNODE udpMsg Node waiting for handle ");
//may be we can create more thread to handle udpMsg
//but we should recycle some thread
//uncontrolled add thread will cause trouble
        NeedDataProcessThread=NEEDDATAPROCESSTHREAD;
    }
    if((MeterDataNumber>MEDIUMNUMBERMETERDATAPRIMARYNODE)&&(MeterDataNumber<MAXNUMBERMETERDATAPRIMARYNODE)){
///Alarm4:too much meterDataPrimaryNode,server almost dump
        printf("MEDIUMNUMBERMETERDATAPRIMARYNODE meterdataprimary Node waiting for handle ");
//may be we can create more thread to handle
        if(NeedDataProcessThread != NEEDDATAPROCESSTHREAD)
            NeedDataSaveThread=NEEDDATASAVETHREAD;
    }
    if((MeterDataNumber>MAXNUMBERMETERDATAPRIMARYNODE)){
///UrgentAlarm2:too much udpMsgNode,server will dump
        printf("MAXNUMBERUDPNODE udpMsg Node waiting for handle ");
//may be we can create more thread to handle udpMsg
//but we should recycle some thread
//uncontrolled add thread will cause trouble
        NeedDataSaveThread=NEEDDATASAVETHREAD;
    }
}


*********************************************************
 * early version has thread adjust
 * if need more dataProcess/dataSave,create more
*********************************************************/

/*
 * udp message test
 * @author raingrey
 * @2017/07/18
    udpMsg * msg = (udpMsg *) arg;
    char * IPaddress=inet_ntoa(msg -> clientAddr.sin_addr);
    printf("Message from %s : %s \n",IPaddress,msg->msg);
//sent messages back to client
    sendto(msg->socket,msg->msg,strlen(msg->msg),0,(struct sockaddr*)&(msg->clientAddr),sizeof(msg->clientAddr));
 * */
