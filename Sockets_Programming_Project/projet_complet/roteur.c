#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
//&&&&&&&&&
#include <unistd.h>
#include <arpa/inet.h>
//&&&&&&&&&
#define MAX 80
#define PORT_roteur_server 8088
#define PORT_roteur_client 8080
#define SA struct sockaddr
int connfd ;
int client() {

	int sockfd;
	struct sockaddr_in servaddr;

	// La creation du socket et La Verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");

	// initialisation de servaddr par zero
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT_roteur_client);

	// pour connecter le Client socket avec Router(interface de Server ) socket et Server
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("connection with the server failed...\n");
		return -1;

	}
	else {
		printf("connected to the server..\n");
		return sockfd;
	}


}
char* crc2(char msg[50], char key[50], int connfd)
{
	char rem[50], temp[100], retmsg[2000];
	int msg_len, key_len, i;
	int flag = 0;

	msg_len = strlen(msg);
	key_len = strlen(key);

	msg[msg_len] = '\0';
	rem[key_len] = '\0';

	// Append 0 at the end
	strcpy(temp, msg);
	for (i = 0; i < key_len - 1; i++)
	{
		temp[msg_len + i] = '0';
	}
	// XOR operation
	for (int j = 0; j < msg_len; j++)
	{
		if (temp[j] != '0')
		{
			for (int k = 0; k < key_len; k++)
			{
				rem[k] = temp[j + k] = (temp[j + k] == key[k]) ? '0' : '1';
			}
		}
	}
	// Reduce remainder
	for (int i = 0; i < key_len; i++)
	{
		rem[i] = rem[i + 1];
	}
	printf("\nrem :");
	puts(rem);
	int remlen = strlen(rem);
	for (int i = 0; i < remlen; i++)
	{
		if (rem[i] != '0')
			flag++;
	}
	if (flag > 0)
		// If Remainder is Not Zero
		strcpy(retmsg, "Message Recu :  Message recu avec erreur , renvoyer \n");
	else
		// If Remainder is Zero
		strcpy(retmsg, "Message Recu  : aucun erreur \n");
	write(connfd, retmsg, sizeof(retmsg));

}
char* make_trame_for_server (char* trame) {
	int pci = 23; ///fanion+adrSrc+adrst+data
	int rpci = 48; ///fanion+fcs
	char data[25], fcs[9];
	int j, i, k;
	for (j = 0; j < sizeof(data) - 1; j++)
		data[j] = trame[j + 24];
	data[j] = '\0';
	printf("Trame: ");
	puts(trame);
//puts(data);
///Cut Data
	for (j = 0; j < sizeof(data) && data[j] == '0'; j++);
	char dataNoPadding[sizeof(data) - j - 1];
//printf("J= %d\n",j);
	for (i = j; i <= sizeof(data); i++) {
		dataNoPadding[i - j] = data[i];
	}
//printf("Size of Data with no padding: %d\n",sizeof(dataNoPadding));
	printf("Data: ");
	puts(dataNoPadding);
	char input_crc[50] ;
	strcpy(input_crc , "");

///Cut Padding
	printf("Padding: ");
	for (i = 0; i < j; i++) {
		printf("%c", data[i]);
	}
	printf("\n");
///Cut FCS
	for (k = 0; k < sizeof(fcs) - 1; k++) {
		fcs[k] = trame[24 + strlen(data) + k];
	}
	printf("FCS: ");
	puts(fcs);
	strcat(input_crc, dataNoPadding);
	strcat(input_crc, fcs);
	puts(input_crc);
	crc2(input_crc, "10010011", connfd);
	return input_crc ; 

}
void func_connect_server	 (char* buff_client,int sockfd_client) {
	char fanion [] = "10000001";
char adrS [] = "10011001";
char adrD[] = "10111101" ;
	printf(" \n le message from client to server : %s \n", buff_client);

	char trame[64] = ""; 
	strcpy(buff_client,trame);
	// function for chat
/*
	
	int i ;
	char data[65] = "";
	strcat(data, fanion);
	strcat(data,adrS);
	strcat(data,adrD);







	bzero(trame, sizeof(trame));
	printf("Enter le message : ");



	//strcpy(trame, buff_client);
	puts(trame) ;
	strcpy(trame , make_trame_for_server(trame) );
	strcat(data,trame);
	strcat(data,fanion);
*/
	//puts(data);
	write(sockfd_client, trame, sizeof(trame));
	bzero(trame, sizeof(trame));
	read(sockfd_client, trame, sizeof(trame));
	printf("From Server : %s", trame);
	if ((strncmp(trame, "exit", 4)) == 0) {
		printf("Client Exit...\n");

	}

	// close the socket
	

}
// Function designed for chat between client and server.
void func_conect_client(int connfd , int sockfd_client)
{
	char buff[MAX];

	int n;
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);

		// read the message from client and copy it in buffer
		read(connfd, buff, sizeof(buff));
		//cuting(buff);

		puts(buff);		
		func_connect_server(buff,sockfd_client);

		// print buffer which contains the client contents
		printf("To client : ");
		bzero(buff, MAX);
		n = 0;
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
	int sockfd, len;
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
	servaddr.sin_port = htons(PORT_roteur_server);

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

	
//code to connect to sever 
/*	int sockfd_client;
	struct sockaddr_in servaddr_client, cli_client;

	// socket create and verification
	sockfd_client = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd_client == -1) {
		printf("socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr_client, sizeof(servaddr_client));

	// assign IP, PORT
	servaddr_client.sin_family = AF_INET;
	servaddr_client.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr_client.sin_port = htons(PORT_roteur_client);

	// connect the client socket to server socket
	if (connect(sockfd_client, (SA*)&servaddr_client, sizeof(servaddr_client)) != 0) {
		printf("connection with the server failed...\n");
		exit(0);
	}
	else
		printf("connected to the server..\n");

	// Function for chatting between client and server
	
	func_conect_client(connfd,sockfd_client);*/

	// After chatting close the socket
	close(sockfd);
	close(sockfd_client);
}

