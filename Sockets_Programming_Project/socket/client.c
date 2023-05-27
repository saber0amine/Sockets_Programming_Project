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
char fanion [] = "10000001";
char adrS [] = "10000111";
char adrD[] = "10000101" ;
char trame[200]="";
char *strrev(char *str)
{
      char *p1, *p2;

      if (! str || ! *str)
            return str;
      for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
      {
            *p1 ^= *p2;
            *p2 ^= *p1;
            *p1 ^= *p2;
      }
      
}
char* crc(char* d){
char g[] = "10010011"; 
    int i,j,keylen,msglen;

    char message[100],msg_temp[30],quot[100],key1[30];
    char key[30];

    strcpy(message,d);

    strcpy(key,g);
    keylen=strlen(key);
    msglen=strlen(message);
    char key2[keylen];
    strcpy(key2,key);
    char rem[keylen-1];

    strcpy(key1,key);
    for (i=0; i<keylen-1; i++)
    {
        message[msglen+i]='0';
    }
    for (i=0; i<keylen; i++)
        msg_temp[i]=message[i];
    for (i=0; i<msglen; i++)
    {
        quot[i]=msg_temp[0];
        if(quot[i]=='0')
            for (j=0; j<keylen; j++)
                key[j]='0';
        else
            for (j=0; j<keylen; j++)
                key[j]=key1[j];
        for (j=keylen-1; j>0; j--)
        {
            if(msg_temp[j]==key[j])
                rem[j-1]='0';
            else
                rem[j-1]='1';
        }
        rem[keylen-1]=message[i+keylen];
        strcpy(msg_temp,rem);
    }
    strcpy(rem,msg_temp);
    char* crc = (char*) malloc(sizeof(rem)+2);
    strcpy(crc,"0");
    strcat(crc,rem);
   	crc[sizeof(rem)+1] = '\0';
    return crc;

}

void  intial_trame(){
	strcpy(trame,"");
	strcat(trame,fanion);
	strcat(trame,adrS);
	strcat(trame,adrD);
}
void func(int sockfd)
{
	char buff[26];
	 char data[25]; 
	 int i ; 
	  for(i=0;i<sizeof(data)-1;i++) {
        data[i] = '0';
    }
    data[i] = '\0';
	int n;
	char resCRC[10];

	
	for (;;) {
		intial_trame();
		bzero(buff, sizeof(buff));
		printf("Enter le message : ");
		n = 0;
		
		strcpy(resCRC,"");
		while ((buff[n++] = getchar()) != '\n')
			;
		
		strcpy(resCRC,crc(buff));
		printf("\nresultas de CRC : ");
		puts(resCRC);
		strrev(buff);
		for (int i = 0,j=0; i < 25; ++i)
		{

			if(buff[i]!='\0' && buff[i]!='\n'){
				data[j++]=buff[i];
			}
		}
    	//puts(data);
		strcat(trame,data);
		strcat(trame,resCRC);
		strcat(trame,"0");
		strcat(trame,fanion);
		write(sockfd, trame, sizeof(trame));
		bzero(trame, sizeof(trame));
		read(sockfd, trame, sizeof(trame));
		printf("From Server : %s", trame);
		if ((strncmp(trame, "exit", 4)) == 0) {
			printf("Client Exit...\n");
			break;
		}
	}

}
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


