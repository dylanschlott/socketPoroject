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
#define FALSE 1
#define MAXCLIENTS 10
struct purchase_request{
	int myid;
	int destinationid;
	int coins; /*Number of coins myId is requesting from destinationId.*/
}purchase_request;

struct clientmsg{
	int userId;
	int coins;
	struct purchase_request req;
	int vectorTime[MAXCLIENTS];
}client;

void
DieWithError(const char *errorMessage) /* External error handling function */
{
        perror(errorMessage);
        exit(1);
}

void
clientMsg(int sockfd,char * sendline,int strlen)
{
	switch(sendline[0]){
	case '1':
	break;
	case '2':
	break;
}
}

void
str_cli(FILE *fp, int sockfd)
{
	ssize_t n;
        char    sendline[ECHOMAX], recvline[ECHOMAX];

        while (fgets(sendline, ECHOMAX, fp) != NULL) {

		clientMsg(sockfd,sendline,strlen(sendline));
                //write(sockfd, sendline, strlen(sendline));
		
	
                if ( (n = read(sockfd, recvline, ECHOMAX)) == 0)
                        DieWithError("str_cli: server terminated prematurely");

		recvline[ n ] = '\0';
                fputs(recvline, stdout);
        }
}
int
proof_of_work(long bigNumber)
{
	int isPrime = TRUE;
	for(int n = 2; n < bigNumber; n++)
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
