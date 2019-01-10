#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <ctype.h>
#include <time.h>
#include <arpa/inet.h>
#define SERVPORT 3334//定义端口号 
#define BACKLOG 20//请求队列中允许的最大请求数 
#define MAXDATASIZE 1024//数据长度 


int main() { 
	struct sockaddr_in server_sockaddr,client_sockaddr;//声明服务器和客户端的socket存储结构
	//socklen_t addr_size;
	pid_t childpid;
	int sin_size,recvbytes,sendbytes; 

	// ====================initial the capability list=========================
	char caplist[6][7];
	FILE *fp;
	fp = fopen ("./caplist.txt", "r");
	rewind(fp);
	fscanf(fp, "%s %s %s %s %s %s", caplist[0], caplist[1], caplist[2], caplist[3], caplist[4], caplist[5]);
	fclose(fp);
	// initial the filename
	char filename[6][30]; 
	FILE *fp2;
	fp2 = fopen ("./filename.txt", "r");
	rewind(fp2);
	fscanf(fp2, "%s %s %s %s %s %s", filename[0], filename[1], filename[2], filename[3], filename[4],filename[5]);
	fclose(fp2);
	// initial the datecreated
	char datecreated[6][30];
	FILE *fp3;
	fp3 = fopen ("./date.txt", "r");
	rewind(fp3);
	fscanf(fp3,"%[^\n]%*c",datecreated[0]);
	fscanf(fp3,"%[^\n]%*c",datecreated[1]);
	fscanf(fp3,"%[^\n]%*c",datecreated[2]);
	fscanf(fp3,"%[^\n]%*c",datecreated[3]);
	fscanf(fp3,"%[^\n]%*c",datecreated[4]);
	fscanf(fp3,"%[^\n]%*c",datecreated[5]);
	fclose(fp3);

	//=================initial the r/w key;0:no_use;>0:reading/writing;
	int rkey[6];
	FILE *fp4;
	fp4 = fopen ("./rkey.txt", "r");
	rewind(fp4);
	fscanf(fp4, "%d %d %d %d %d %d", &rkey[0], &rkey[1], &rkey[2], &rkey[3], &rkey[4], &rkey[5]);
	fclose(fp4);
	int wkey[6];
	FILE *fp5;
	fp5 = fopen ("./wkey.txt", "r");
	rewind(fp5);
	fscanf(fp5, "%d %d %d %d %d %d", &wkey[0], &wkey[1], &wkey[2], &wkey[3], &wkey[4], &wkey[5]);
	fclose(fp5);

	int sockfd,client_fd;//socket描述符 
	char buf[MAXDATASIZE],backin[MAXDATASIZE];//传输的数据
	int group_id = 0,cli_num = 0;// group ID for clients;1:AOS;2:CSE;3:other;client_num:1/2.

	// file information
	char cli_group[6][10] = {"a1 AOS","a2 AOS","c1 CSE","c2 CSE","o1 other","o2 other"};
	int filesize[6];

	//建立socket链接
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) { 
		perror("Socket"); 
		exit(1); 
	} 
	//socketfd:套接字描述符
	printf("Socket success!,sockfd=%d\n",sockfd); 
	
	//=================memset==========
	memset(&server_sockaddr, '\0', sizeof(server_sockaddr));
	
	//以sockaddt_in结构体填充socket信息
	server_sockaddr.sin_family = AF_INET;//IPv4 
	server_sockaddr.sin_port = htons(SERVPORT);//端
	server_sockaddr.sin_addr.s_addr = INADDR_ANY;//本主机的任意IP都可以使用 		
	bzero(&(server_sockaddr.sin_zero),8);//填充0 
	
	if((bind(sockfd,(struct sockaddr *)&server_sockaddr,sizeof(struct sockaddr))) == -1) {
		//bind函数绑定 
		perror("bind"); 
		exit(-1); 
	} 
	printf("bind success!\n"); 

	if(listen(sockfd,BACKLOG) == -1) {
		//监听 
		perror("listen"); 
		exit(1); 
	} 
	printf("listening ... \n"); 

	
	
	while(1){
		//等待客户端链接 
		if((client_fd = accept(sockfd,(struct sockaddr *) &client_sockaddr,&sin_size)) == -1) {	
			perror("accept"); 
			exit(1); 
		} 
		printf("accept success!\n"); 
		//printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
		if((childpid = fork()) == 0){
			close(sockfd);
		while(1){

		// store client's Group	and client_number first
		if(group_id == 0){
			//接收客户端的ID 
			if((recvbytes = recv(client_fd,buf,MAXDATASIZE,0)) == -1) {
				perror("recv"); 
				exit(1); 
			} 
			//client_ID irregular
			if(strlen(buf) > 2){
				char in1[MAXDATASIZE] = "client_ID is too long!\n";	
				send(client_fd,in1,MAXDATASIZE,0);
			}
			else{
				//AOS_group
				if((!strcmp(buf, "a1")) ||(!strcmp(buf, "a2"))){
					char in2[MAXDATASIZE] = "welcome, your group is AOS.\n";
					send(client_fd,in2,MAXDATASIZE,0);
					group_id = 1;
					if(!strcmp(buf, "a1"))
						cli_num = 0;
					else
						cli_num = 1;

				}
				//CSE group
				if((!strcmp(buf, "c1")) ||(!strcmp(buf, "c2"))){
					char in2[MAXDATASIZE] = "welcome, your group is CSE.\n";
					send(client_fd,in2,MAXDATASIZE,0);
					group_id = 2;
					if(!strcmp(buf, "c1"))
						cli_num = 2;
					else
						cli_num = 3;
				}
				//other group
				if((!strcmp(buf, "o1")) ||(!strcmp(buf, "o2"))){
					char in2[MAXDATASIZE] = "welcome, your group is other.\n";
					send(client_fd,in2,MAXDATASIZE,0);
					group_id = 3;
					if(!strcmp(buf, "o1"))
						cli_num = 4;
					else
						cli_num = 5;
				}
				printf("client is %d\n", cli_num);
			}

		}
		else{
		//接收客户端的请求 
		if((recvbytes = recv(client_fd,buf,MAXDATASIZE,0)) == -1) {
			perror("recv"); 
			exit(1); 
		} 

// ====================initial the capability list=========================
	char caplist[6][7];
	FILE *fp;
	fp = fopen ("./caplist.txt", "r");
	rewind(fp);
	fscanf(fp, "%s %s %s %s %s %s", caplist[0], caplist[1], caplist[2], caplist[3], caplist[4], caplist[5]);
	fclose(fp);
	// initial the filename
	char filename[6][30]; 
	FILE *fp2;
	fp2 = fopen ("./filename.txt", "r");
	rewind(fp2);
	fscanf(fp2, "%s %s %s %s %s %s", filename[0], filename[1], filename[2], filename[3], filename[4],filename[5]);
	fclose(fp2);
	// initial the datecreated
	char datecreated[6][30];
	FILE *fp3;
	fp3 = fopen ("./date.txt", "r");
	rewind(fp3);
	fscanf(fp3,"%[^\n]%*c",datecreated[0]);
	fscanf(fp3,"%[^\n]%*c",datecreated[1]);
	fscanf(fp3,"%[^\n]%*c",datecreated[2]);
	fscanf(fp3,"%[^\n]%*c",datecreated[3]);
	fscanf(fp3,"%[^\n]%*c",datecreated[4]);
	fscanf(fp3,"%[^\n]%*c",datecreated[5]);
	fclose(fp3);

	//=================initial the r/w key;0:no_use;>0:reading/writing;
	int rkey[6];
	FILE *fp4;
	fp4 = fopen ("./rkey.txt", "r");
	rewind(fp4);
	fscanf(fp4, "%d %d %d %d %d %d", &rkey[0], &rkey[1], &rkey[2], &rkey[3], &rkey[4], &rkey[5]);
	fclose(fp4);
	int wkey[6];
	FILE *fp5;
	fp5 = fopen ("./wkey.txt", "r");
	rewind(fp5);
	fscanf(fp5, "%d %d %d %d %d %d", &wkey[0], &wkey[1], &wkey[2], &wkey[3], &wkey[4], &wkey[5]);
	fclose(fp5);
//================================================================
		//end the socket
		if(strcmp(buf, "end") == 0){
			char in04[MAXDATASIZE] ="socket has been closed!\n";
			send(client_fd,in04,MAXDATASIZE,0);
			break;
		}
		printf("received a connection : %s\n",buf);
		//Split received message
		char *req[4];
		char *result = strtok(buf, " ");
		printf("result: %s  \n",result);
		req[0] = result;
		int i=1;
		int length = 0;
		while(result != NULL&&(i < 3)){
			result = strtok(NULL, " ");
			req[i] = result;
			length = i;
			i++;
		}
		printf("req: %s %s  \n",req[0], req[1]);
		if(length == 1){
			char in4[MAXDATASIZE] ="Your message is too short!\n";
			send(client_fd,in4,MAXDATASIZE,0);
			continue;
		}
		//====================show all===========================	
		if((strcmp(req[0], "show")==0)&&(strcmp(req[1], "all") == 0)){
		printf("=================show====================\n");
		// calculate the file size
		char infomall[MAXDATASIZE] = "";
		for(int f = 0; f < 6; f++){
			int fID = f;
			if(strcmp(filename[fID], "nameless")==0)
				continue;
			char path02[30] = "./";
			strcat(path02, filename[fID]);
			FILE *fp031 = NULL;
			fp031 = fopen(path02, "r");
			//fseek(fp031,0,SEEK_END);  
    			int fisize = ftell(fp031);
   			fclose(fp031);
			// file_ID = cli_num = i
			char infom[MAXDATASIZE] = " ";
			strcat(infom, caplist[fID]);
			strcat(infom, " ");
			strcat(infom, cli_group[fID]);
			strcat(infom, " ");
			char fsize[20]; 
			sprintf(fsize,"%d",fisize);
			strcat(infom, fsize);
			strcat(infom, " ");
			strcat(infom, datecreated[fID]);
			strcat(infom, " ");
			strcat(infom, filename[fID]);
			strcat(infomall, infom);
			strcat(infomall, "\n");
		}
		send(client_fd,infomall,MAXDATASIZE,0);
		}
		//====================New===========================
		if(strcmp(req[0], "new") == 0){
			// The file existed
			if(caplist[cli_num][0] != '-'){
				char in4[MAXDATASIZE] ="Each client just can own one file!\n";
				send(client_fd,in4,MAXDATASIZE,0);
				continue;
			}
			if(req[2] == NULL){
				char in6[MAXDATASIZE] ="You should specify all the access rights!\n";
				send(client_fd,in6,MAXDATASIZE,0);
				continue;
			}
			if(strlen(req[2]) < 6){
				char in5[MAXDATASIZE] ="You should specify all the access rights!\n";
				send(client_fd,in5,MAXDATASIZE,0);
				continue;
			}
			// Get the file path
			char *name = req[1];
			char path[30];
			char line[] = "./";
			strcpy(path, line);
			strcat(path, req[1]);
			// Save the capability list
			char right[20];
			strcpy(right, req[2]);
			for(int j = 0;j < 6; j++)
				caplist[cli_num][j] = right[j]; //caplist
			// Open the file
			FILE *fp = NULL;
			fp = fopen(path, "w+");
			printf("path: %s\n",path);
			fprintf(fp, "This is file own by client\n"); 
   			fclose(fp);
			// Save the file infomation
			strcpy(filename[cli_num], req[1]); //filename
			printf("filename[cli_num]: %s\n",filename[cli_num]);
			// Get now time
			time_t rawtime;
			struct tm *timeinfo;
			time(&rawtime);	
			strcpy(datecreated[cli_num], ctime(&rawtime)); //datecreated
			datecreated[cli_num][strlen(datecreated[cli_num])-1] = 0;
			// ===========save the informatin in the file======================
			// save the caplist
			FILE * fpn1;
			fpn1 = fopen ("./caplist.txt", "w+");
			fprintf(fpn1,"%s\n%s\n%s\n%s\n%s\n%s",caplist[0], caplist[1], caplist[2], caplist[3], caplist[4], caplist[5]);
			fclose(fpn1);
			// save the filename
			FILE * fpn2;
			fpn2 = fopen ("./filename.txt", "w+");
			fprintf(fpn2,"%s\n%s\n%s\n%s\n%s\n%s",filename[0], filename[1], filename[2], filename[3], filename[4], filename[5]);
			fclose(fpn2);
			//save the datecreated
			FILE * fpn3;
			fpn3 = fopen ("./date.txt", "w+");
			fprintf(fpn3,"%s\n%s\n%s\n%s\n%s\n%s",datecreated[0], datecreated[1], datecreated[2], datecreated[3], datecreated[4], datecreated[5]);
			fclose(fpn3);
			// reply to client
			printf("Information has been recorded!");
			char in7[MAXDATASIZE] ="File has been created!\n";
			send(client_fd,in7,MAXDATASIZE,0);
		}

		//====================change===========================	
		if(strcmp(req[0], "change") == 0){
		printf("=================change====================\n");
			// The file not existed
			if(strcmp(filename[cli_num], "nameless") == 0){
				char in4[MAXDATASIZE] ="You don't own one file!\n";
				send(client_fd,in4,MAXDATASIZE,0);
				continue;
			}
			// The access rights are not specified
			if(strlen(req[2]) < 6){
				char in5[MAXDATASIZE] ="You should specify all the access rights!\n";
				send(client_fd,in5,MAXDATASIZE,0);
			}
			// Update the permisson
			for(int j = 0;j < 6; j++)
				caplist[cli_num][j] = req[2][j];
			printf("caplist[cli_num] is %s\n", caplist[cli_num]);
			// ==============update the informatin in the file ===========
			FILE * fp21;
			fp21 = fopen ("./caplist.txt", "w+");
			fprintf(fp21,"%s\n%s\n%s\n%s\n%s\n%s",caplist[0], caplist[1], caplist[2], caplist[3], caplist[4], caplist[5]);
			fclose(fp21);
			printf("Caplist has been updated!");
			char in5[MAXDATASIZE] ="Permission has been updated!\n";
			send(client_fd,in5,MAXDATASIZE,0);
		}
		//====================information===========================	
		if(strcmp(req[0], "information") == 0){
		printf("=================infor====================\n");
			// Search the file
			int file_ID = -1;
			for(int i = 0; i < 6; i++){
				if(strcmp(filename[i],req[1]) == 0){
				printf("=========get it=========================\n");
					file_ID = i;
					break;
				}
			}
			// The file not existed
			if(file_ID == -1){
				char in6[MAXDATASIZE] ="Can't find the file!\n";
				send(client_fd,in6,MAXDATASIZE,0);
				printf("filename: %s\n", req[1]);
				printf("=========cant get it=========================\n");
				continue;
			}
			// calculate the file size
			char path2[30] = "./";
			strcat(path2, req[1]);
			FILE *fp31 = NULL;
			fp31 = fopen(path2, "r");
			//fseek(fp31,0,SEEK_END);  
    			int fisize = ftell(fp31);
   			fclose(fp31);
			// file_ID = cli_num = i
			char infom[1024] = " ";
			strcat(infom, caplist[file_ID]);
			strcat(infom, " ");
			strcat(infom, cli_group[file_ID]);
			strcat(infom, " ");
			char fsize[20]; 
			sprintf(fsize,"%d",fisize);
			strcat(infom, fsize);
			strcat(infom, " ");
			strcat(infom, datecreated[file_ID]);
			strcat(infom, " ");
			strcat(infom, filename[file_ID]);
			strcat(infom, "\n");
			send(client_fd,infom,MAXDATASIZE,0);
		}

		//====================read===========================	
		if(strcmp(req[0], "read") == 0){
		printf("=================read====================\n");
			// Search the file
			int file_ID = -1;
			for(int i = 0; i < 6; i++){
				if(strcmp(filename[i],req[1]) == 0){
				printf("=========get it=========================\n");
					file_ID = i;
					break;
				}
			}
			// The file not existed
			if(file_ID == -1){
				char in6[MAXDATASIZE] ="Can't find the file!\n";
				send(client_fd,in6,MAXDATASIZE,0);
				printf("filename: %s\n", req[1]);
				printf("=========cant get it=========================\n");
				continue;
			}
			// file_ID = cli_num = i
			// check the caplist
			int ind = (group_id - 1) * 2;
			// The group don't have right
			if(caplist[file_ID][ind] != 'r'){
				char in16[MAXDATASIZE] ="Your group don't get the right to read!\n";
				send(client_fd,in16,MAXDATASIZE,0);
				continue;
			}
			//check if other client is writing 
			if(wkey[file_ID] > 0){
				char in17[MAXDATASIZE] ="Other client is writing!\n";
				send(client_fd,in17,MAXDATASIZE,0);
				continue;
			}
			//start read
			//add the key number
			printf("rkey[file_ID]: %d\n",rkey[file_ID]);
			rkey[file_ID] += 1;
			// ==============update the informatin in the file ===========
			FILE * fp42;
			fp42 = fopen ("./rkey.txt", "w+");
			fprintf(fp42,"%d\n%d\n%d\n%d\n%d\n%d",rkey[0], rkey[1], rkey[2], rkey[3], rkey[4], rkey[5]);
			fclose(fp42);
			//open the file
			char path3[30] = "./";
			strcat(path3, req[1]);
			FILE *fp41 = NULL;
			fp41 = fopen(path3, "r");
   			//start the reading
			int loop = 0;	
			char in18[MAXDATASIZE] ="Send next to read the nextline;send stop to quit.\n";
			send(client_fd,in18,MAXDATASIZE,0);
			while(loop < 100){
			//=================initial the r/w key;0:no_use;>0:reading/writing;
			//int rkey[6];
			FILE *fp4;
			fp4 = fopen ("./rkey.txt", "r");
			rewind(fp4);
			fscanf(fp4, "%d %d %d %d %d %d", &rkey[0], &rkey[1], &rkey[2], &rkey[3], &rkey[4], &rkey[5]);
			fclose(fp4);
			//int wkey[6];
			FILE *fp5;
			fp5 = fopen ("./wkey.txt", "r");
			rewind(fp5);
			fscanf(fp5, "%d %d %d %d %d %d", &wkey[0], &wkey[1], &wkey[2], &wkey[3], &wkey[4], &wkey[5]);
			fclose(fp5);
				recv(client_fd,buf,MAXDATASIZE,0);
				//stop the reading
				if(strcmp(buf, "stop") == 0){
					char in19[MAXDATASIZE] ="reading is end.\n";
					send(client_fd,in19,MAXDATASIZE,0);
					rkey[file_ID] -= 1;
					// ==============update the informatin in the file ===========
					FILE * fp43;
					fp43 = fopen ("./rkey.txt", "w+");
					fprintf(fp43,"%d\n%d\n%d\n%d\n%d\n%d",rkey[0], rkey[1], rkey[2], rkey[3], rkey[4], rkey[5]);
					fclose(fp43);
					break;
				}
				
				//read the nextline
				if(strcmp(buf, "next") == 0){
					char in21[MAXDATASIZE];
					fscanf(fp41,"%[^\n]%*c",in21);
					if(in21 == NULL){
						char in192[MAXDATASIZE] ="reading is end.\n";
						send(client_fd,in192,MAXDATASIZE,0);
						rkey[file_ID] -= 1;
						// ==============update the informatin in the file ===========
						FILE * fp432;
						fp432 = fopen ("./rkey.txt", "w+");
						fprintf(fp432,"%d\n%d\n%d\n%d\n%d\n%d",rkey[0], rkey[1], rkey[2], rkey[3], rkey[4], rkey[5]);
						fclose(fp432);
						break;
					}
					send(client_fd,in21,MAXDATASIZE,0);
				}
				else{
					char in20[MAXDATASIZE] ="Send next to read the nextline;send stop to quit.\n";
					send(client_fd,in20,MAXDATASIZE,0);
				}
				loop += 1;
				
			}
			fclose(fp41);

		}
		//====================write===========================	
		if(strcmp(req[0], "write") == 0){
		printf("=================write====================\n");
			// Search the file
			int file_ID = -1;
			for(int i = 0; i < 6; i++){
				if(strcmp(filename[i],req[1]) == 0){
				printf("=========get it=========================\n");
					file_ID = i;
					break;
				}
			}
			// The file not existed
			if(file_ID == -1){
				char in56[MAXDATASIZE] ="Can't find the file!\n";
				send(client_fd,in56,MAXDATASIZE,0);
				printf("filename: %s\n", req[1]);
				printf("=========cant get it=========================\n");
				continue;
			}
			// file_ID = cli_num = i
			// check the caplist
			int ind = (group_id*2 - 1);
			// The group don't have right
			if(caplist[file_ID][ind] != 'w'){
				char in51[MAXDATASIZE] ="Your group don't get the right to write!\n";
				send(client_fd,in51,MAXDATASIZE,0);
				continue;
			}
			//check if other client is writing 
			if(wkey[file_ID] > 0){
				char in52[MAXDATASIZE] ="Other client is writing!\n";
				send(client_fd,in52,MAXDATASIZE,0);
				continue;
			}
			//check if other client is reading 
			if(rkey[file_ID] > 0){
				char in53[MAXDATASIZE] ="Other client is reading!\n";
				send(client_fd,in53,MAXDATASIZE,0);
				continue;
			}
			// check o/a
			if(req[2] == NULL){
				char in6[MAXDATASIZE] ="Please add o/a!\n";
				send(client_fd,in6,MAXDATASIZE,0);
				continue;
			}
			if(strlen(req[2]) > 1){
				char in54[MAXDATASIZE] ="Please add o/a!\n";
				send(client_fd,in54,MAXDATASIZE,0);
				continue;
			}
			if((strcmp(req[2], "o") != 0)&&(strcmp(req[2], "a") != 0)){
				char in55[MAXDATASIZE] ="Please add o/a!\n";
				send(client_fd,in55,MAXDATASIZE,0);
				continue;
			}
			//start write
			//add the key number
			printf("wkey[file_ID]: %d\n",wkey[file_ID]);
			wkey[file_ID] += 1;
			// ==============update the informatin in the file ===========
			FILE * fp52;
			fp52 = fopen ("./wkey.txt", "w+");
			fprintf(fp52,"%d\n%d\n%d\n%d\n%d\n%d",wkey[0], wkey[1], wkey[2], wkey[3], wkey[4], wkey[5]);
			fclose(fp52);
			//decide add or overwrite
			char oper[4];
			if(strcmp(req[2], "o") == 0)
				strcpy(oper, "w");
			if(strcmp(req[2], "a") == 0)
				strcpy(oper, "a");
			//open the file
			char path4[30] = "./";
			strcat(path4, req[1]);
			FILE *fp51 = NULL;
			fp51 = fopen(path4, oper);
   			//start the reading
			int loop = 0;	
			char in58[MAXDATASIZE] ="Send string to write into the file;or send stop0 to quit.\n";
			send(client_fd,in58,MAXDATASIZE,0);
			while(loop < 100){
			//=================initial the r/w key;0:no_use;>0:reading/writing;
			//int rkey[6];
			FILE *fp4;
			fp4 = fopen ("./rkey.txt", "r");
			rewind(fp4);
			fscanf(fp4, "%d %d %d %d %d %d", &rkey[0], &rkey[1], &rkey[2], &rkey[3], &rkey[4], &rkey[5]);
			fclose(fp4);
			//int wkey[6];
			FILE *fp5;
			fp5 = fopen ("./wkey.txt", "r");
			rewind(fp5);
			fscanf(fp5, "%d %d %d %d %d %d", &wkey[0], &wkey[1], &wkey[2], &wkey[3], &wkey[4], &wkey[5]);
			fclose(fp5);
				recv(client_fd,buf,MAXDATASIZE,0);
				//stop the writing
				if(strcmp(buf, "stop0") == 0){
					char in59[MAXDATASIZE] ="writing is end.\n";
					send(client_fd,in59,MAXDATASIZE,0);
					wkey[file_ID] -= 1;
					// ==============update the informatin in the file ===========
					FILE * fp53;
					fp53 = fopen ("./wkey.txt", "w+");
					fprintf(fp53,"%d\n%d\n%d\n%d\n%d\n%d",wkey[0], wkey[1], wkey[2], wkey[3], wkey[4], wkey[5]);
					fclose(fp53);
					break;
				}
				//write into the file
				fprintf(fp51, "%s", buf); 
				fprintf(fp51, "\n"); 
				char in60[MAXDATASIZE] ="Success writing! Send string to continue; or send stop0 to quit.\n";
				send(client_fd,in60,MAXDATASIZE,0);
				loop += 1;
			}
			fclose(fp51);

		}
		printf("end caplist is %s\n", caplist[0]);
		}
		}
		} 
	}
	//close the socket
	close(client_fd); 
}
