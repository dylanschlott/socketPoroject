#include	<sys/socket.h> 	/* for socket() and bind() */
#include	<stdio.h>		/* printf() and fprintf() */
#include	<stdlib.h>		/* for atoi() and exit() */
#include	<arpa/inet.h>	/* for sockaddr_in and inet_ntoa() */
#include	<sys/types.h>
#include	<string.h>
#include	<unistd.h>
#include	<string.h>

#define	ECHOMAX	255		/* Longest string to echo */
#define BACKLOG	128
#define BUFF 64

int minerQty = 0;

struct Miner {

	char userName [64];
	char ipAddress [64];
	char portNumber [64];
	int coins;
};
struct request{
	char requestType[BUFF];
	char requestArgs[BUFF];
	int userId;
	int status;
	struct miner minerInfo;
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
EchoString(int sockfd)
{
    ssize_t n;
    char    line[ECHOMAX];
    struct Miner myMiner;

    for ( ; ; ) {
	    if ( (n = read(sockfd, &myMiner, ECHOMAX)) == 0 )
   	    	return; /* connection closed by other end */

//        write(sockfd, line, n );
	printf("%s %s %s \n",myMiner.userName,myMiner.ipAddress,myMiner.portNumber);	
	//printf("%s\n",line);
	//struct test
    }
}

int registerMiner() {

	int success = 0;

	if(success == 0) {

		printf("SUCCESS");
	}	
	else {

		printf("FAILURE");
	}

	return -1;
}

int query() {
	
	return minerQty;	
}



void deRegister(char * name) {

	int success = 0;
	
	if(success == 0) {

		printf("SUCCESS");
	}
	else {

		printf("FAILURE");
	}
}

void save(char * fName) {

	FILE * fp;
	char buffer[11];
	char fileName[20];
	
	strcpy(fileName, fName);
	strcat(fileName, ".txt");

	//printf(fileName);

	fp = fopen(fileName, "w");

	//sprintf(buffer, "%d", minerQty);
	fputs("I went and made a file!", fp);

	fclose(fp);	

}

int
main(int argc, char **argv)
{
    int sock, connfd;                /* Socket */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned int cliAddrLen;         /* Length of incoming message */
    char echoBuffer[ECHOMAX];        /* Buffer for echo string */
    unsigned short echoServPort;     /* Server port */
    int recvMsgSize;                 /* Size of received message */

	char * test = "testFile";

	save(test);

    if (argc != 2)         /* Test for correct number of parameters */
    {
        fprintf(stderr,"Usage: %s <TDP SERVER PORT>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for sending/receiving datagrams */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        DieWithError("server: socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("server: bind() failed");
  
	if (listen(sock, BACKLOG) < 0 )
		DieWithError("server: listen() failed");

	cliAddrLen = sizeof(echoClntAddr);
	connfd = accept( sock, (struct sockaddr *) &echoClntAddr, &cliAddrLen );
	
	EchoString(connfd);
	close(connfd);
}
