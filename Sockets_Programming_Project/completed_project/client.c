#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
//&&&&&&&&
#include <unistd.h>
#include <arpa/inet.h>
//&&&&&&&&
#define MAX 80
#define PORT 8081
#define SA struct sockaddr


char fanion [] = "00000000"; //10000001
char adrS [] = "ssssssss";//10000111
char adrD[] = "dddddddd" ; //10000101
char trame[200]="";

void  intial (){
	strcpy(trame,"");
	strcat(trame,fanion);
	//strcat(trame,"/");
	//puts(trame);
	strcat(trame,adrS);
	//strcat(trame,"/");
	//puts(trame);
	strcat(trame,adrD);
	//strcat(trame,"/");
	//puts(trame);
}
char* removeLastchar(char ms[]){ //remove the enter character when the user tap enter
	int i;
	char* b=malloc(1);
	for (int i = 0; i < strlen(ms)-1; i++)
	{
		b[i]=ms[i];
		b=(char*)realloc(b,(strlen(b)+1)*sizeof(char));
	}
	b=(char*)realloc(b,(strlen(b)-1)*sizeof(char));
return b;}
char* CRC(char* );
char* CRC2(char* );
void func(int sockfd)
{
	char buff[MAX];
	int n,i;
	char* resCRC=malloc(10);

	
	for (;;) {
		intial();
		bzero(buff, sizeof(buff));
		printf("Enter the string : ");
		n = 0;
		
		strcpy(resCRC,"");
		while ((buff[n++] = getchar()) != '\n')
			;
		//getting msg without the letter when pressing enter

		char* m=malloc(35);
		for(i=0;i<strlen(buff)-1;i++)
			*(m+i)=buff[i];

		printf("%s \n",buff);
		resCRC=CRC(m);
		strcat(trame,removeLastchar(buff));//strcat(trame,buff);
		
		//strcat(trame,"/");
		
		//strcpy(buff,trame);
		//strcat(buff,resCRC);
		strcat(trame,resCRC);

		//strcat(trame,"/");
		
		strcat(trame,"0"); //ajouter un bit de bourrage au crc pour completer 8 bits car le polynome est de degre 7 

		//strcat(trame,"/");
		
		strcat(trame,fanion);
		strcpy(buff,trame);


		write(sockfd, buff, sizeof(buff));
		bzero(buff, sizeof(buff));
		read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		if ((strncmp(buff, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}

}
char* CRC(char* msg) {
	int i,j,keylen,msglen;
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
    
    char* c= malloc(1);
	for (i=0;i<keylen-1;i++){
	    c[i]=rem[i];
	    c=(char*)realloc(c,(strlen(c)+1)*sizeof(char));
	}
		c=(char*)realloc(c,(strlen(c)-1)*sizeof(char));

return c;
}
//fonction qui retourne le crc a 7 bits
/*char* CRC2(char* msg){
	int i,j;
	char crc[]="10010011";
	int m = strlen(msg);
	int n = strlen(crc);
	char encocded[100]="";
	strcat(encocded,msg);

	for(i=0;i<n-1;i++)
		strcat(encocded,"0");

	for(i=0;i<strlen(encocded)-n;){
		for(j=0;j<n;j++)
			encocded[i+j] = encocded[i+j] == crc[j] ? '0' : '1';
		for(;i<strlen(encocded) && encocded[i]!='1';i++);
	}
	
	char* fcrc = malloc(30);
	for(i=strlen(encocded)-7,j=0;i<strlen(encocded);i++,j++)
		fcrc[j] = encocded[i];
	return fcrc;

}*/
int main()
{
	int sockfd, connfd;
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
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// function for chat
	func(sockfd);

	// close the socket
	close(sockfd);
}

