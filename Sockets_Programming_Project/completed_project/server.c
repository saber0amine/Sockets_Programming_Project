#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
//&&&&&&&&&
#include <unistd.h>
#include <arpa/inet.h>
//&&&&&&&&&
#define MAX 80
#define PORT 8087 //changed from 8080
#define SA struct sockaddr
 
// Function designed to verify if the message has been received without error
bool CRC(char* msg) {
	int i,j,keylen,msglen;
	bool messgeWithoutError = true;
	char input[100], key[30],temp[30],quot[100],rem[30],key1[30];
	strcpy(input,msg);
	strcpy(key,"10010011");
	keylen=strlen(key);
	msglen=strlen(input); 
	strcpy(key1,key);
	for (i=0;i<keylen-1;i++) {
		input[msglen+i]='0';
	}
	for (i=0;i<keylen;i++)
	 temp[i]=input[i];
	for (i=0;i<msglen;i++) {
		quot[i]=temp[0];
		if(quot[i]=='0')
		 for (j=0;j<keylen;j++)
		 key[j]='0'; else
		 for (j=0;j<keylen;j++)
		 key[j]=key1[j];
		for (j=keylen-1;j>0;j--) {
			if(temp[j]==key[j])
			 rem[j-1]='0'; else
			 rem[j-1]='1';
		}
		rem[keylen-1]=input[i+keylen];
		strcpy(temp,rem);
	}
	strcpy(rem,temp);
    
    char* c= malloc(30);
	for (i=0;i<keylen-1;i++){
	    c[i]=rem[i];
	}
	printf("the remainder is : %s\n",c);
	for ( i = 0; i < strlen(c); i++)
	{
		if(c[i]!='0'){
			messgeWithoutError=false;
			break;
		}
	}

return messgeWithoutError;
}

// Function designed for chat between client and server.
void func(int connfd)
{
	char buff[MAX];
	int n,i,j;
	int bufferlen;
	char messageWithCrc[30]="";
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);
		strcpy(messageWithCrc,"");

		// read the message from client and copy it in buffer
		read(connfd, buff, sizeof(buff));
		//printf("\n thse size of buffer is : %ld\n",strlen(buff));

		printf("trame : %s\n",buff);

		strcpy(messageWithCrc,"");
		bufferlen = strlen(buff)-1; 

		/*for(j=0,i=26 ;i<d;i++,j++)
			*(mm+j)=buff[i];
		printf("the frame without adresses is : %s\n",mm);   

		char messageWithCrc[35];
		for(i=0;i<strlen(mm)-7;i++)
			messageWithCrc[i] = mm[i];*/
		
		memcpy(messageWithCrc,&buff[24],bufferlen-32);

	    printf("the message with crc is : ");
	    puts(messageWithCrc);

	    

		// print buffer which contains the client contents if the message is received without error
		if(CRC(messageWithCrc)==true)
		   printf("From client: %s\t To client : ", buff); 
		else{
			printf("\n------An error has been detected in the message sent by the client-----\n");
			printf("To client : ");
		}
		bzero(buff, MAX);


		n = 0;
		printf("Enter the string : ");
		// copy server message in the buffer
		while ((buff[n++] = getchar()) != '\n')
			;

		// and send that buffer to client
		write(connfd, buff, sizeof(buff));

		// if msg contains "Exit" then server exit and chat ended.
		if (strncmp("exit", buff, 4) == 0) {
			printf("Server Exit...\n");
			break;
		}
	}
}

// Driver function
int main()
{
	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("socket bind failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully binded..\n");

	// Now server is ready to listen and verification
	if ((listen(sockfd, 5)) != 0) {
		printf("Listen failed...\n");
		exit(0);
	}
	else
		printf("Server listening..\n");
	len = sizeof(cli);

	// Accept the data packet from client and verification
	connfd = accept(sockfd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("server accept failed...\n");
		exit(0);
	}
	else
		printf("server accept the client...\n");

	// Function for chatting between client and server
	func(connfd);

	// After chatting close the socket
	close(sockfd);
}

