#ifndef _MODBUSDRIVER_H_
#define _MODBUSDRIVER_H_

#define LNCHECKANDSENDHOSTTIMESTEP 10//second

#define BUFF_SIZE 100

#define MODBUSDATAUNITSIZEMAX 480

#define MAXDUMPTIME 3//seconds
#define MAXDUMPTIMEHOSTNODE 360//seconds
#define MAXHEARTBEATNUMBER 500//
#define MMAXHEARTBEATNUMBER 600//
#define HOSTHEARTBEATNUMBER 1

#define NEEDNOTHREAD 0
#define NEEDDATAPROCESSTHREAD 1
#define NEEDDATASAVETHREAD 2

/*Urgent Alarm*/
#define SERVERDUMPHOSTNODE 96
#define SERVERDUMPLISTENINGNODE 97
#define SERVERDUMPMETERDATAPRIMARYNODE 98
#define SERVERDUMPMETERDATASECONDARYNODE 99
#define SERVERDUMPUDPMSGNODE 100


#define MAXDATAPROCESSSAVETHREADNUMBER 14

#define MEDIUMNUMBERUDPMSGNODE 100
#define MAXNUMBERUDPMSGNODE 120

#define MEDIUMNUMBERMETERDATAPRIMARYNODE 100
#define MAXNUMBERMETERDATAPRIMARYNODE 120

#define MEDIUMNUMBERMETERDATASECONDARYNODE 100
#define MAXNUMBERMETERDATASECONDARYNODE 120

#define MEDIUMNUMBERRBTLISTENINGNODE 100
#define MAXNUMBERRBTLISTENINGNODE 120


//heart beat data less than 15 byte
#define HEARTBEATSTR "influid.com"
///IMEI code size and also the dtuID's size
#define DTUIDSIZE 15
#define MODBUSADDRSIZE 1
#define MODBUSFUNCCODE 0x03
#define MODBUSFUNCCODESIZE 1
#define MODBUSBYTENUMBERCODESIZE 1
#define MODBUSHOSTDATASIZE 8
#define MODBUSCRCDATASIZE 2
#define MODBUSSTARTREGISTERDATASIZE 2
#define MODBUSREGISTERNUMBERDATASIZE 2
#define MODBUSREGISTERNUMBERMAX 100

#define MYSQLQUERYSTRSIZE 250
#define MYSQLMANUFACTURESIZE 30
#define MYSQLSTARTREGISTERORD 1
#define MYSQLSELECTBACKINTSIZE 3


#define MODBUSDATAUSHORTSIZE 2
#define MODBUSDATASHORTSIZE 2
#define MODBUSDATAUINTSIZE 4
#define MODBUSDATAINTSIZE 4
#define MODBUSDATAULONGSIZE 8
#define MODBUSDATALONGSIZE 8
#define MODBUSDATAFLOATSIZE 4
#define MODBUSDATADOUBLESIZE 8
#define MODBUSDATAUCHAR1 1
#define MODBUSDATAUCHAR2 2
#define MODBUSDATAUCHAR3 3
#define MODBUSDATAUCHAR4 4
#define MODBUSDATAUCHAR5 5
#define MODBUSDATAUCHAR6 6
#define MODBUSDATAUCHAR7 7
#define MODBUSDATAUCHAR8 8
#define MODBUSDATAUCHAR9 9
#define MODBUSDATAUCHAR10 10
#define MODBUSDATAUCHAR11 11
#define MODBUSDATAUCHAR12 12
#define MODBUSDATAUCHAR13 13
#define MODBUSDATAUCHAR14 14
#define MODBUSDATAUCHAR15 15
#define MODBUSDATAUCHAR16 16
#define MODBUSDATAUCHAR17 17
#define MODBUSDATAUCHAR18 18
#define MODBUSDATAUCHAR19 19
#define MODBUSDATAUCHAR20 20

#define MODBUSREGISTERDATATYPEUSHORT 1
#define MODBUSREGISTERDATATYPESHORT 2
#define MODBUSREGISTERDATATYPEUINT 3
#define MODBUSREGISTERDATATYPEINT 4
#define MODBUSREGISTERDATATYPEULONG 5
#define MODBUSREGISTERDATATYPELONG 6
#define MODBUSREGISTERDATATYPEFLOAT 7
#define MODBUSREGISTERDATATYPEDOUBLE 8
#define MODBUSREGISTERDATATYPEUCHAR1 9
#define MODBUSREGISTERDATATYPEUCHAR2 10
#define MODBUSREGISTERDATATYPEUCHAR3 11
#define MODBUSREGISTERDATATYPEUCHAR4 12
#define MODBUSREGISTERDATATYPEUCHAR5 13
#define MODBUSREGISTERDATATYPEUCHAR6 14
#define MODBUSREGISTERDATATYPEUCHAR7 15
#define MODBUSREGISTERDATATYPEUCHAR8 16
#define MODBUSREGISTERDATATYPEUCHAR9 17
#define MODBUSREGISTERDATATYPEUCHAR10 18
#define MODBUSREGISTERDATATYPEUCHAR11 19
#define MODBUSREGISTERDATATYPEUCHAR12 20
#define MODBUSREGISTERDATATYPEUCHAR13 21
#define MODBUSREGISTERDATATYPEUCHAR14 22
#define MODBUSREGISTERDATATYPEUCHAR15 23
#define MODBUSREGISTERDATATYPEUCHAR16 24
#define MODBUSREGISTERDATATYPEUCHAR17 25
#define MODBUSREGISTERDATATYPEUCHAR18 26
#define MODBUSREGISTERDATATYPEUCHAR19 27
#define MODBUSREGISTERDATATYPEUCHAR20 28
#define MODBUSREGISTERDATATYPETIMESTAMP 29

#define ORDERINSTANTFLOW 1
#define ORDERTOTALFLOW 2
#define ORDERT 3
#define ORDERP 4
#define ORDERDP 5
#define ORDERTIMESTAMP 6
#define ORDERPRIMARYNUMBER 6

#define ORDER7 7
#define ORDER8 8
#define ORDER9 9
#define ORDER10 10
#define ORDER11 11
#define ORDER12 12
#define ORDER13 13
#define ORDER14 14
#define ORDER15 15
#define ORDER16 16
#define ORDER17 17
#define ORDER18 18
#define ORDER19 19
#define ORDER20 20




/**
 *  * udp data struct
 *   * two-way linked-list
 *    * used for Thread:
 *     * 1.Recvform of main
 *      * 2.and LNManager
 *       * 3.DataProcess
 *        * 
 *         * status:
 *          * 0    can not use
 *           * 1    can be processed by ThreadProcess
 *            * 2    processing by ThreadProcess
 *             * socket:
 *              * socket descripter
 *               * clientAddr:
 *                * from IP
 *                 * msg:
 *                  * message data from buffer
 *                   *
 *                    * */
#define UDPNODESENDBACKTIMECRITCAL 8
typedef struct udpMsg{
//        char status;
        struct sockaddr_in clientAddr;
        uint8_t msg[BUFF_SIZE];
        struct udpMsg * next;
}udpMsg;

/*
 * threadascription:
 * 0	NULL
 * 1	data process thread
 * 2	data save thread
 * threadstatus:
 * 0	no thread
 * 1	thread hung up
 * 2	thread running
 */

//return:
#define FREEDATAPROCESSTHREAD 1
#define FREEDATASAVETHREAD 2

#define MAXTHREADNUMBER 16

#define THREADASCRIPTIONNULL 0
#define THREADASCRIPTIONDATAPROCESS 1
#define THREADASCRIPTIONDATASAVE 2
#define THREADASCRIPTIONLNMANAGER 3

#define THREADSTATUSNULL 0
#define THREADSTATUSHUNGUP 1
#define THREADSTATUSRUNNING 2

#define THREADPOOLFILL 0

#define THREADFREE 0
#define THREADPERSIST 1
typedef struct threadPoolInfo{
	pthread_t tid;
	uint8_t threadascription;
	char threadstatus;
	char cmd;
}threadPoolInfo;

//dt = 0 	wrong
//dt = 1 	client data
//dt = 2 	host data
//dt = 3 	client or host data
#define MODBUSDATAWRONG 0
#define MODBUSDATACLIENTDATA 1
#define MODBUSDATAHOSTDATA 2
#define MODBUSDATACLIENTORHOSTDATA 3
typedef struct mBDS{
	uint8_t dT;
        uint8_t addr;
        uint8_t fC;
        uint8_t bN;
	uint16_t sR;
	uint16_t rN;
	uint16_t CRC;
	uint8_t mBD[BUFF_SIZE-20];
}mBDS;
#define ListeningNodeHost 7
#define ListeningNodeHeartBeat 6
#define ListeningNodeUnknownFunctionCode 5
#define ListeningNodeListening 4
typedef struct listeningNode {
    uint32_t MBStatus;
    uint32_t heartBeat;
    struct rb_node node;
    uint64_t DTUID;
    time_t dumpTime;
//这里建立一个哈希表的数据，256个仪表单元0xff，
//用0xff%0xf来确定所属哈希表，然后进行最多16次比较即可确定元素
    //struct deviceNode deviceNode_hash[16];
    struct deviceNode * headDevice;
    struct sockaddr_in clientAddr;

}listeningNode;


typedef struct modbusRegisterInfo{
    int ord;
    int addr;
    int bytenum;
    int datatype;
	struct modbusRegisterInfo * next;
}modbusRegisterInfo;

/* client of ModBus that connect with current DTU
 *  * few nodes
 *   * */
typedef struct deviceNode{
    uint32_t deviceNumber;
    uint32_t meterID;
    uint32_t startRegister;
	struct modbusRegisterInfo * modbusRegisterInfoHead;
    struct deviceNode *next;
}deviceNode;



/* *
 * status:
 * 0	bad node
 * 1	waiting for handle
 * 2	processing
 * */
#define METERDATAPRIMARYSAVECOUNTCRITICAL 8
typedef struct meterDataPrimary{
        int sC;
        int meterID;
        float instantFlow;
        unsigned long int totalFlow;
        float T;
        float P;
        float DP;
        uint8_t timestamp[10];
        struct meterDataPrimary * next;
}meterDataPrimary;

#define METERDATASECONDARYSAVECOUNTCRITICAL 8
typedef struct meterDataSecondary{
    int sC;
    int meterID;
    uint8_t timestamp[10];
	uint8_t order7[20];
	uint8_t order8[20];
	uint8_t order9[20];
	uint8_t order10[20];
	uint8_t order11[20];
	uint8_t order12[20];
	uint8_t order13[20];
	uint8_t order14[20];
	uint8_t order15[20];
	uint8_t order16[20];
	uint8_t order17[20];
	uint8_t order18[20];
	uint8_t order19[20];
	uint8_t order20[20];
	struct meterDataSecondary* next;
}meterDataSecondary;

//thread information table
extern threadPoolInfo ThreadPool[MAXDATAPROCESSSAVETHREADNUMBER]; 


extern struct rb_root listeningNodeRoot;
extern pthread_mutex_t mtx;
extern pthread_mutex_t data_save_mtx;

extern pthread_cond_t condLNManager;
extern pthread_cond_t condDataProcess;
extern pthread_cond_t condDataProcessTimer;
extern pthread_cond_t condDataSave;

//udpMsgHead is head node of this two-way link-list
extern udpMsg * udpMsgHead;
////meterDataPrimaryHead is head node of this two-way link-list
extern meterDataPrimary * meterDataPrimaryHead;
////meterDataSecondaryHead is head node of this two-way link-list
extern meterDataSecondary * meterDataSecondaryHead;
//触发处理线程检查RBT的心跳和离线
extern int RBTListeningNodeNumber;

//save the number of current MeterDataToSave
extern int MeterDataNumber;
//网络数据包缓存链表计数
extern int UdpMsgNumber;

//
//mysql descripter
extern MYSQL *sql;
extern MYSQL *sqlDataSave;
//
//
///全程序共用的socket
extern serv_sock;

uint32_t  ModBusCRC16(unsigned char *updata,unsigned int len);

struct listeningNode * RBTSearch(struct rb_root *root, uint64_t dtuid);
int RBTInsert(struct rb_root *root, struct listeningNode *data);
///urgent: this is important
/// for every time every code line
/// if a pointer is freed(by free() or by defined function())
/// this pointer should be set NULL
/// in line after freefunction
void RBTFree(struct listeningNode *node);
///urgent: this is important
/// for every time every code line
/// if a pointer is freed(by free() or by defined function())
/// this pointer should be set NULL
/// in line after freefunction
uint8_t FreeModBusRegisterInfoCircle(modbusRegisterInfo * p1);
///urgent: this is important
/// for every time every code line
/// if a pointer is freed(by free() or by defined function())
/// this pointer should be set NULL
/// in line after freefunction
uint8_t FreeDeviceNodeCircle(deviceNode * p1);



#endif
