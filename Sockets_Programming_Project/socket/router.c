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
#define PORT 8081
#define SA struct sockaddr
#define G "10010011"
char buff[MAX];

//table de routage 
int table_routage [3][3] = {{3,1,9099},{4,5,8088},{5,3,8070}};
void strev(unsigned char *str)
{
	int i;
	int j;
	unsigned char a;
	unsigned len = strlen((const char *)str);
	for (i = 0, j = len - 1; i < j; i++, j--)
	{
		a = str[i];
		str[i] = str[j];
		str[j] = a;
	}
}


void decapsulation(char msg[], char* crc, char* d) {
//strcpy(msg,"0000000000000001000000101101001000000010100000000");
	int i, j, keylen, msglen, remlen, flag = 0;
	char input[100], key[30], temp[30], quot[100], rem[30], key1[30], retmessage[100];
	int pci = 23; ///fanion+adrSrc+adrst+data
	int rpci = 48; ///fanion+fcs
	char data[25], fcs[9];
	int k;
	printf("Trame: ");
	puts(msg);
///Cut Data
	for (i = 24; i < 48; i++) {
		data[i - 24] = msg[i];
	}
	data[i - 24] = '\0';
	strcpy(d, data);
	printf("Data: ");
	puts(data);
	strcpy(input, data);
	printf("\nMessage Data Received From Client: %s", input);
	strcpy(key, G);
	printf("\nDivisor: %s", key);
	printf("\n");
///Cut FCS
	for (k = 0; k < sizeof(fcs) - 1; k++) {
		fcs[k] = msg[24 + strlen(data) + k];
	}
	fcs[k] = '\0';
	printf("FCS: ");
	puts(fcs);
	for (i = 1; i < strlen(fcs); i++) {
		crc[i - 1] = fcs[i];
	}
	crc[i - 1] = '\0';
}

int client(int port) {

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
	servaddr.sin_port = htons(port);

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
void crc(char* input, char* crc) {
	int i, j, keylen, msglen, remlen, flag = 0;
	char key[30];
//char input[100];
//char crc[30];
	char temp[30], quot[100], rem[30], key1[30], retmessage[100];
	strcpy(key, G);
	strcpy(rem, crc);
	keylen = strlen(key);
	msglen = strlen(input);
	strcpy(key1, key);
	strcat(input, rem);
	for (i = 0; i < keylen; i++)
		temp[i] = input[i];
	for (i = 0; i < msglen; i++) {
		quot[i] = temp[0];
		if (quot[i] == '0')
			for (j = 0; j < keylen; j++)
				key[j] = '0'; else
			for (j = 0; j < keylen; j++)
				key[j] = key1[j];
		for (j = keylen - 1; j > 0; j--)
		{
			if (temp[j] == key[j])
				rem[j - 1] = '0'; else
				rem[j - 1] = '1';
		}
		rem[keylen - 1] = input[i + keylen];
		strcpy(temp, rem);
	}

	strcpy(rem, temp);
	/*printf("\nQuotient is ");
	for (i=0;i<msglen;i++)
	printf("%c",quot[i]);*/
	printf("Remainder or Crc is: ");
	for (i = 0; i < keylen - 1; i++)
		printf("%c", rem[i]);
	printf("\n");
	remlen = strlen(rem);
	printf("Status: Message is received ");
	for (i = 0; i < remlen; i++)
	{
		if (rem[i] != '0')
			flag++;
	}
	if (flag > 0)
// If Remainder is Not Zero
//strcpy(retmessage,"Message Received by Server , Message In Error Please Send Again");
		printf("With ");
	else
// If Remainder is Zero
		printf("Without ");
	printf("Error\n");
}

void funcC(int sockfd)
{

	int n;
	for (;;) {
		write(sockfd, buff, sizeof(buff));
		break;
	}
}



// Function designed for chat between client and server.
void funcS(int connfd)
{

	int n;
	// infinite loop for chat
	for (;;) {
		bzero(buff, MAX);
		char CRC[8], input[25];
		// read the message from client and copy it in buffer
		read(connfd, buff, sizeof(buff));
		// print buffer which contains the client contents
		//printf("Client: ");
		//puts(buff);
		decapsulation(buff, CRC, input);
		//decapsulation("000000000000000100000010ddddddddddddddddddddddddffffffff00000000",CRC);


		//puts(CRC);
		//printf("Buff= ");
		//puts(input);
		crc(input, CRC);

		//puts(CRC);
		break;
		printf("Done funcS\n");
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

	// Chat as a server
	funcS(connfd);

	// After chatting close the socket
	close(sockfd);
	printf("Done\n");
	sockfd = client(table_routage [0][2]);
	funcC(sockfd);
	close(sockfd);
}

