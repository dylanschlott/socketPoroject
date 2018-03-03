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

struct Miner {
	char userName [64];
	char ipAddress [64];
	char portNumber [64];
	int initialCoins;
};
struct request{
	char requestType[BUFF];
	char requestArgs[BUFF];
	struct miner myMiners[MAXCLIENTS];
	int VectorClock[MAXCLIENTS];	
};
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
	struct miner myMiner;
	strncpy(myMiner.userName,"testname",BUFF);
	strncpy(myMiner.ipAddress,"1.2.3.4",BUFF);
	strncpy(myMiner.portNumber,"54321",BUFF);
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
	struct sockaddr_in servaddr;

	if (argc != 3)
		DieWithError( "usage: tcp-client <Server-IPaddress> <Server-Port>" );
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}
