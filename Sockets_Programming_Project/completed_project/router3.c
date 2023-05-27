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
#define PORT 8083
#define PORT_CL 8084
#define SA struct sockaddr
#define max 7

int tableRoutage[4][4] ={
	{1,1,8081},{2,7,8082},{4,3,8084},{5,2,8084}
};
//la premiere colonne signifie le numéro du routeur
//la deuxieme colonne signifie le cout de chemin
////la troisieme colonne signifie le numéro du port
/*tableRoutage[0][0]=1; tableRoutage[0][1]=1; tableRoutage[0][2]=8080; 
tableRoutage[1][0]=2; tableRoutage[1][1]=7; tableRoutage[1][2]=8081;
tableRoutage[2][0]=4;  tableRoutage[2][1]=3; tableRoutage[2][2]=8084;
tableRoutage[3][0]=5;  tableRoutage[3][1]=2; tableRoutage[3][2]=8084;*/

char fanion [] = "00000000"; //10000001
char adrS [] = "rrrrrrrr";//10000111
char adrD[] = "SSSSSSSS" ; //10000101
char trame[200]="";

void  intial (){
	strcpy(trame,"");
	strcat(trame,fanion);
	strcat(trame,adrS);
	strcat(trame,adrD);
}



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

// Function designed for chat between client and the router and then between the router and the server.
void func(int connfd,int sockfd)
{
	char buff[MAX];
	char adrD[9]="";
	char adrS[9]="";
	int bufferlen;
	char messageWithCrc[33]="";
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(connfd, buff, sizeof(buff));
		printf("the old frame : %s\n",buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}

		strcpy(messageWithCrc,"");
		bufferlen = strlen(buff); //-1
		// print buffer which contains the client contents
		//printf("The buffer received by router: %s\n",buff);

		//Check if the message received is correct (Recalculate the CRCyn)
		memcpy(messageWithCrc,&buff[24],bufferlen-32);
		if(CRC(messageWithCrc)==false){
			printf("\n------The router have detected an error in the message sent by the client-----\n");
		}
		else{
			//Generate a new Frame by the router
			intial();
			strcat(trame,messageWithCrc);
			strcat(trame,fanion);
			strcpy(buff,trame);
			printf("the new frame : %s\n",buff);
			// and send that buffer to server
			write(sockfd, buff, sizeof(buff));
			bzero(buff, MAX);
			//read from server
			read(sockfd, buff, sizeof(buff));
			printf("From Server : %s\n",buff);
			write(connfd, buff, sizeof(buff));

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

	







	int sockfdr, connfdr;
	struct sockaddr_in servaddrr, clir;

	// socket create and verification
	sockfdr = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfdr == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddrr, sizeof(servaddrr));

	// assign IP, PORT
	servaddrr.sin_family = AF_INET;
	servaddrr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddrr.sin_port = htons(tableRoutage[1][2]);

	// connect the client socket to server socket
	if (connect(sockfdr, (SA*)&servaddrr, sizeof(servaddrr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(connfd,sockfdr);

	// close the socket
	close(sockfd);
	close(sockfdr);
	
}


