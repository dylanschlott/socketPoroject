#include        <sys/socket.h>  /* for socket() and bind() */
#include        <stdio.h>               /* printf() and fprintf() */
#include        <stdlib.h>              /* for atoi() and exit() */
#include        <arpa/inet.h>   /* for sockaddr_in and inet_ntoa() */
#include        <sys/types.h>
#include        <string.h>
#include        <unistd.h>

#define ECHOMAX 255             /* Longest string to echo */
#define BACKLOG 128
#define TRUE 1
#define FALSE 0
#define MAXCLIENTS 10
#define BUFF 64
#define SUCCESS 1
#define FAILURE 0

//TODO query info
//TODO
struct Miner {
	char userName [64];
	char ipAddress [64];
	char portNumber [64];
	int userId;
	int coins;
};
struct request{
	int requestType;
	char requestArgs[BUFF];
	int userId;
	int status;
	int numMiners;//number active miners
	struct Miner minerInfo;
	struct Miner myMiners[MAXCLIENTS];
	int VectorClock[MAXCLIENTS];	
};

struct Miner myMiner;//This client's miner
struct request myRequest;//this is a general purpose struct for handling client->server requests
struct request serverRequest;//this is a gernal purpose struct for handling server->client requests
char response[BUFF];

void
DieWithError(const char *errorMessage) /* External error handling function */
{
        perror(errorMessage);
        exit(1);
}

void
clientMsgOut(int sockfd,char * sendline,int strlen)
{
	//msg format"REQUEST,ARG1,ARG2,ARGN,"
	char * msg;
	switch(sendline[0]){
	case '1':
	write(sockfd, "querytest\n", 9);
	break;

	case '2':
	write(sockfd, "register\n", 8);
	break;

	default:
	write(sockfd, sendline, strlen);
	}
}
void 
clientMsgIn(char * inputString)
{
	fputs(inputString,stdout);
}
void
str_cli(FILE *fp, int sockfd)
{
	ssize_t n;
        char    sendline[ECHOMAX], recvline[ECHOMAX];
        while (fgets(sendline, ECHOMAX, fp) != NULL) {

		//clientMsgOut(sockfd,sendline,strlen(sendline));
		write(sockfd,&myMiner,sizeof(myMiner));
                //write(sockfd, sendline, strlen(sendline));
		
	
                if ( (n = read(sockfd, recvline, ECHOMAX)) == 0)
                        DieWithError("str_cli: server terminated prematurely");

		recvline[ n ] = '\0';
		//clientMsgIn(recvline);
                //fputs(recvline, stdout);
        }
}
int
proof_of_work(long bigNumber)
{
	int isPrime = TRUE;
	for(int n = 2; n < bigNumber-1; n++)
	{
		if(bigNumber%n==0)
		{
			isPrime = FALSE;
		}
	}
	return isPrime;
}



int
main(int argc, char **argv)
{
	int sockfd;
	char command[BUFF];
	char inputName[BUFF];
	struct sockaddr_in servaddr;
	
	if (argc != 5)
		DieWithError( "usage: tcp-client <Server-IPaddress> <Server-Port> <Coins> <Name>" );
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	strncpy(myMiner.userName,argv[4],sizeof(argv[4]));
	myMiner.coins = atoi(argv[3]);
	strncpy(myMiner.ipAddress,argv[1],sizeof(argv[1]));
	myMiner.userId = -1;
	strncpy(myMiner.portNumber,argv[2],sizeof(argv[2]));
	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	myRequest.requestType = 1;//1 is register
	myRequest.userId = 999;
	myRequest.status = -1;
	myRequest.minerInfo = myMiner;

	//register client to server	
	write(sockfd,&myRequest,sizeof(myRequest));

	//sleep(1);
	ssize_t n;
 	if ( (n = read(sockfd, &serverRequest, ECHOMAX)) == 0)
                        DieWithError("str_cli: server terminated prematurely");

	printf("Return code:%d \nuserID %d\nrequestArgs %s\n", serverRequest.status,serverRequest.userId, serverRequest.requestArgs);

	//str_cli(stdin, sockfd);		/* do it all */

	while(1) {

		fgets(command, BUFF, stdin);
		printf("command was %s\n", command);

		if(strcmp(command, "1\n") == 0) {

			printf("\nEnter user name: ");
			fgets(inputName, BUFF, stdin);

			strcpy(myMiner.userName, inputName);
			strcpy(myMiner.ipAddress, "1.11");
			strcpy(myMiner.portNumber, "1");
			myMiner.userId = -1;
			myMiner.coins = 777;

			myRequest.requestType = 1;
			myRequest.userId = 999;
			myRequest.status = -1;
			myRequest.minerInfo = myMiner;
			write(sockfd, &myRequest, sizeof(myRequest));
		}
		else if (strcmp(command, "2\n") == 0) {

			printf("Requested to print list\n");

			myRequest.requestType = 2;
			write(sockfd, &myRequest, sizeof(myRequest));

			if ((n = read(sockfd, &serverRequest, ECHOMAX)) == 0);
				DieWithError("server terminated prematurely");

			printf("request args was %s", serverRequest.requestArgs);
		}
		else if (strcmp(command, "break") == 0) {

			printf("\nleaving loop");
		}
		else {

			printf("\nInvalid input");
		}
	}

	exit(0);
}
