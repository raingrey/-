#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUFF_SIZE 578
void error_handling(char *message);

int main (int argc,char* argv[]){
//local socket descripter
	int sock;
//server socket address
	struct sockaddr_in server_addr;
	struct sockaddr_in from_addr;
	socklen_t addr_size;
//table to save accepted string
	char message7[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','7',0x08,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbe,0xa6};
	char message6[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','6',0x08,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbe,0xa6};
	char message5[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','5',0x08,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbe,0xa6};
	char message4[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','4',0x08,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbe,0xa6};
	char message3[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','3',0x08,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbe,0xa6};
	char message2[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','2',0x08,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbe,0xa6};
	char message1[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','1',0x08,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbe,0xa6};
	char message0[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','0',0x08,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbe,0xa6};
	char message8[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','8',0x08,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbe,0xa6};
	char message87[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','8',0x07,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbf,0x16};
	char message86[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','8',0x06,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x72,0x8a};
	char message85[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','8',0x05,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x6f,0x26};
	char message84[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','8',0x04,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xeb,0xf3};
	char message83[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','8',0x03,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x8d,0xe5};
	char message82[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','8',0x02,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x40,0x79};
	char message81[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','8',0x01,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x14,0x9c};
	char message9[BUFF_SIZE]={'3','5','6','5','6','6','0','7','0','9','0','3','4','4','9',0x08,0x03,0x14,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0xbe,0xa6};
//length of accepted string
	int str_len=0;
	int IPlen;
	char* IPaddress;
//check argc if the parameter is legal
	if(argc != 3){
		printf("<IP> Address and <PORT> number to connect is needed\n",argv[0]);
		exit(1);
	}
//create client socket, set protocol and connect type
	sock=socket(PF_INET,SOCK_DGRAM,0);
//initialize memory applyed by server_addr
	memset(&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr(argv[1]);
	server_addr.sin_port=htons(atoi(argv[2]));
	while(1){
		sendto(sock,message0,strlen(message0),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up0\n");
		usleep(2000000);
		sendto(sock,message1,strlen(message1),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up1\n");
		usleep(2000000);
		sendto(sock,message2,strlen(message2),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up2\n");
		usleep(2000000);
		sendto(sock,message3,strlen(message3),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up3\n");
		usleep(2000000);
		sendto(sock,message4,strlen(message4),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up4\n");
		usleep(2000000);
		sendto(sock,message5,strlen(message5),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up5\n");
		usleep(2000000);
		sendto(sock,message6,strlen(message6),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up6\n");
		usleep(2000000);
		sendto(sock,message7,strlen(message7),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up7\n");
		usleep(2000000);
		sendto(sock,message8,strlen(message8),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up8n");
		usleep(2000000);
		sendto(sock,message87,strlen(message87),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up87\n");
		usleep(2000000);

		sendto(sock,message86,strlen(message86),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up86\n");
		usleep(2000000);

		sendto(sock,message85,strlen(message85),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up85\n");
		usleep(2000000);
		sendto(sock,message84,strlen(message84),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up84\n");
		usleep(2000000);
		sendto(sock,message83,strlen(message83),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up83\n");
		usleep(2000000);
		sendto(sock,message82,strlen(message82),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up82\n");
		usleep(2000000);
		sendto(sock,message81,strlen(message81),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up81\n");
		usleep(2000000);

		sendto(sock,message9,strlen(message9),0,(struct sockaddr *)&server_addr,sizeof(server_addr));
		addr_size=sizeof(server_addr);
//		str_len=recvfrom(sock,message,BUFF_SIZE,0,(struct sockaddr *)&server_addr,&addr_size);
		printf("Message sent up9\n");
		usleep(2000000);


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
