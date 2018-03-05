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
#define MAXCLIENTS 10

int minerQty = 0;
int registerMiner();
char sendLine[BUFF];
char list[100];

//If someone is trying to contact server, send them 
//finish query: send request struct
//deregister:  

struct Miner {

	char userName [64];
	char ipAddress [64];
	char portNumber [64];
	int userID;
	int coins;
};

struct request{
	int requestType;
	char requestArgs[BUFF];
	int userId;
	int status;
	int numMiners;
	struct Miner minerInfo;
	struct Miner myMiners[MAXCLIENTS];
	int VectorClock[MAXCLIENTS];	
};

struct Miner minerDatabase[MAXCLIENTS];
struct request clientRequest;

//Client requests a miner by name. Request struct sent back with desired miner.
void searchByName(char * inputName, int sockfd) {

	for(int index = 0; index < 10; index++) {

		if(strcmp(minerDatabase[index].userName, inputName)) {

			clientRequest.minerInfo = minerDatabase[index];
			write(sockfd, &minerDatabase[index], sizeof(minerDatabase));	
		}
		else {

			break;
		}
	}
}

char * printList() {

	for(int index = 0; index < 10; index++) {

		if(minerDatabase[index].userName[0] != '\0') {

			if(index == 0) {
			
				strcpy(list, minerDatabase[index].userName);
				strcat(list, "\n");
			}
			else {

				strcat(list, minerDatabase[index].userName);
				strcat(list, "\n");
			}
		}
		else {

			if(index == 0) {

				strcpy(list, "no current miners");
			}
			break;
		}
	}
}

void 
DieWithError(const char *errorMessage) /* External error handling function */
{
	perror(errorMessage);
	exit(1);
}

//NOTE YOU HAVE TO READ IN '2' CONDITIONAL

void
EchoString(int sockfd)
{
	ssize_t n;
	char line[ECHOMAX];
	struct request inRequest;
	int index = 0;
	
	for ( ; ; ) {

		if ((n = read(sockfd, &inRequest, sizeof(inRequest))) == 0 ) {
			
			//connection closed by other end
			return; 
		}

		clientRequest.requestType = inRequest.requestType;
		//strcpy(clientRequest.requestArgs, inRequest.requestArgs);
		clientRequest.userId = inRequest.userId;
		clientRequest.status = inRequest.status;
		clientRequest.minerInfo = inRequest.minerInfo;
		strcpy(clientRequest.requestArgs, "hello");
		//clientRequest.myMiners = inRequest.myMiners;
		//clientRequest.VectorClock = inRequest.VectorClock;	

		if(inRequest.requestType == 1) { //client is trying to register
			
			clientRequest.userId = registerMiner();

			write(sockfd, &clientRequest, sizeof(clientRequest));	
		}
		else if(inRequest.requestType == 2) { //client wants list of miners

			//printList();
			strcpy(clientRequest.requestArgs, "test RequestArgs");

			write(sockfd, &clientRequest, sizeof(clientRequest));	
		}
		else if(inRequest.requestType == 3) { //client is trying to search miner

		}
		else { //client sent invalid requestType

			strcpy(sendLine, "Invalid request\n");
			write(sockfd, &sendLine, sizeof(sendLine));
		}	
	}
}

int registerMiner() {

	printf("Trying to register");
	int assignedID = -500;
	int index = 0;

	while(minerDatabase[index].userName[0] != '\0') {

		//if the request miner exists in the server's database
		if((strcmp(minerDatabase[index].userName, clientRequest.minerInfo.userName) == 0) && (strcmp(minerDatabase[index].userName, clientRequest.minerInfo.ipAddress) == 0) && 
(strcmp(minerDatabase[index].portNumber, clientRequest.minerInfo.portNumber) ==0) && (minerDatabase[index].coins == clientRequest.minerInfo.coins)) {

				assignedID = index;
				//return assignedID;
				return 420;
		} 

		//No matching miners, but the database isn't full, so add miner
		if(minerDatabase[index].userName[0] == '\0') {

			strcpy(minerDatabase[index].userName, clientRequest.minerInfo.userName);
			strcpy(minerDatabase[index].ipAddress, clientRequest.minerInfo.ipAddress);
			strcpy(minerDatabase[index].portNumber, clientRequest.minerInfo.portNumber);
			minerDatabase[index].userID = clientRequest.minerInfo.userID;
			minerDatabase[index].coins = clientRequest.minerInfo.coins;

			minerQty++;
			assignedID = index;

			//NOTE: is the client supposed to see success, or the server?
			printf("\nSUCCESS\n");
			clientRequest.status = 1;
			//clientRequest.userId = index;
			//return assignedID;
			return 777;
		}

		index++;

		//database is full, can't add the miner (this should avoid segfault)
		if(index == 10) {

			printf("\nFAILURE\n");
			//return assignedID;
			return 888;
		}
	}

	//Adding first miner
	strcpy(minerDatabase[index].userName, clientRequest.minerInfo.userName);
	strcpy(minerDatabase[index].ipAddress, clientRequest.minerInfo.ipAddress);
	strcpy(minerDatabase[index].portNumber, clientRequest.minerInfo.portNumber);
	minerDatabase[index].userID = clientRequest.minerInfo.userID;
	minerDatabase[index].coins = clientRequest.minerInfo.coins;
	clientRequest.status = 1;

	return 1;
}

//NOTE: it says to return the minerQty. Should we add int minerQty to request struct to send back?
void query(int sockfd) {

	clientRequest.numMiners = minerQty;
	write(sockfd, &clientRequest, sizeof(clientRequest));	
}



void deRegister(char * name) {

	//search database for given name
	for(int index = 0; index < 10; index++) {

		//found match
		if(strcmp(name, minerDatabase[index].userName) == 0) {

			//no need to shift next miners back 1 index, just remove the last one
			if(index == 9) {

				strcpy(minerDatabase[index].userName, "notSet");
				strcpy(minerDatabase[index].ipAddress, "notSet");
				strcpy(minerDatabase[index].portNumber, "notSet");
				minerDatabase[index].userID = -1;
				minerDatabase[index].coins = -1;
			}
			else {

				//shift all miners back an index
				for(int index2 = index + 1; index2 < 10; index2++) {

					strcpy(minerDatabase[index].userName, minerDatabase[index2].userName);
					strcpy(minerDatabase[index].ipAddress, minerDatabase[index2].ipAddress);
					strcpy(minerDatabase[index].portNumber, minerDatabase[index2].portNumber);
					minerDatabase[index].userID = minerDatabase[index2].userID;
					minerDatabase[index].coins = minerDatabase[index2].coins;
					
				}
			}

			return; //removes possiblity of finding another w/ same userName
		}
	}
}

void save(char * fName) {

	FILE * fp;
	char buffer[11];
	char fileName[20];
	
	strcpy(fileName, fName);
	strcat(fileName, ".txt");

	fp = fopen(fileName, "w");

	sprintf(buffer, "%d", minerQty);
	fputs(buffer, fp);
	fputs("\n", fp);

	//fputs(printList(), fp);

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

	printf("Starting server...\n");

	//registerMiner("first", "1.1", "1", "111");
	//registerMiner("second", "2.2", "2", "222");
	//registerMiner("last", "3.3", "3", "333");
	
	//printList();

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
	pid_t pid;
	int ret;	
	while(1)
	{
		do connfd = accept( sock, (struct sockaddr *) &echoClntAddr, &cliAddrLen );
		while(connfd==-1);
		if(connfd ==-1)
			DieWithError("something happened");
		if((pid=fork())==-1)
			DieWithError("Could not create child process");
		else if(pid==0)
		{
			EchoString(connfd);
		}
		//parent
		do ret=close(connfd);while(ret==-1);
		if(ret==-1)DieWithError("Server died");
	}
	//cliAddrLen = sizeof(echoClntAddr);
	//connfd = accept( sock, (struct sockaddr *) &echoClntAddr, &cliAddrLen );

	//EchoString(connfd);
	
	//printf("EchoString exited: \n");

	//close(connfd);

	printf("Server exited: \n");
}
