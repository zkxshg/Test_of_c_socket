#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> 
#include <string.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>
#define SERVPORT 3334 
#define MAXDATASIZE 1024
int main() { 

	int sockfd,sendbytes,recvbytes; 
	char buf[MAXDATASIZE];  
	struct hostent* host; 
	struct sockaddr_in serv_addr; 
	/*
	if(argc < 2) {
		//需要用户指定链接的地址 
		fprintf(stderr,"Please enter the server's hostname and your client_ID\n"); 
		exit(1); 
	} 
	*/
	//转换为hostent 
	if((host = gethostbyname("localhost")) == NULL) {	
		perror("gethostbyname"); 
		exit(1); 
	} 
	//创建socket 
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
		perror("socket"); 
		exit(1); 
	} 
	//填充数据 
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(SERVPORT); 
	serv_addr.sin_addr = *((struct in_addr *)host->h_addr); 
	bzero(&(serv_addr.sin_zero),8); 
	//发起对服务器的链接
	if((connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))) == -1) {
		perror("connect"); 
		exit(1); 
	} 
	printf("please enter your client_ID first.\n");
	char rece[MAXDATASIZE];
	while(1){
		printf("please enter your request.\n");
		gets(buf);
		int len = strlen(buf);
		if(strcmp(buf, "0") == 0)
			break;
		if(len > MAXDATASIZE){
			printf("len is longer than MAXDATASIZE!");
			continue;
		}
		if((sendbytes = send(sockfd,buf,MAXDATASIZE,0)) == -1) {
			//发送消息给服务器端 
			printf("sendbytes error");
			perror("send"); 
			exit(1); 
		} 

		
		if((recvbytes = recv(sockfd,rece,MAXDATASIZE,0)) == -1) {
			//接收server的info 
			printf("received error"); 	
			perror("recv"); 
			exit(1); 
		} 
		puts(rece);
	}
	close(sockfd);
}

