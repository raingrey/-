#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFF_SIZE 578
void error_handling(char *message);
typedef struct{
	unsigned int DTU_number;
	unsigned int FM_number;
	char start_DTU[15]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','0'};
	
}user_input;

int main (int argc,char* argv[]){
//local socket descripter
	int sock;
//server socket address
	struct sockaddr_in server_addr;
	struct sockaddr_in from_addr;
	socklen_t addr_size;
//table to save accepted string
	char message[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','0',0x08,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbe,0xa6};
//length of accepted string
	int str_len=0;
	int IPlen;
	char* IPaddress;
	int i=0;
	user_input setdata; 
	setdata.DTU_number=1000;
	setdata.FM_number=30;
	int tmp;
//check argc if the parameter is legal
	if(argc < 3){
		printf("<IP> Address and <PORT> number to connect is needed\n",argv[0]);
		exit(1);
	}
	if(argc > 3){
		setdata.DTU_number=atoi(argv[3]);

	}
	if(argc > 4){
		if(setdata.FM_number=atoi(argv[4])>256){
			setdata.FM_number=255;
			printf("单ModBus总线中仪表数量超出255会被修改为255\n");
		}
	}
	printf("DTU数量\n",setdata.DTU_number);
	printf("ModBus总线中仪表数量\n",setdata.FM_number);
//2018.7.17
//@dark_jadeite
//写完用户带参数输入的解析，小媳妇快饿死了，先去看看小媳妇。
//明天继续写根据用户输入来调整DTUID，和仪表地址
//还要把CRC校验的代码弄过来，在仪表伪装的部分有实现
//create client socket, set protocol and connect type
	sock=socket(PF_INET,SOCK_DGRAM,0);
//initialize memory applyed by server_addr
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr(argv[1]);
	server_addr.sin_port=htons(atoi(argv[2]));
	while(1){
		for(i=0;i<1000;i++){
			sendto(sock,message0,strlen(message0),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
			addr_size=sizeof(server_addr);
	//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
			printf("Message sent up0\n");
			usleep(2000000);
		}

	}
//print accepted message
	close(sock);

	return 0;

}

void error_handling(char *message){
	fputs(message,stderr);
	fputc('\n',stderr);
	exit(1);
}
