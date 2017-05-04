//SEE WHY IT WON"T RUN ON NFS1 SERVER, ONLY ON BOARD
//ADD ARG FOR SECOND IP
//NOTE: MUST RUN RTU FIRST
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "status.h"

#define MSG_SIZE 150			// message size

typedef struct {
	int ID;
	double timestamp; 			// Look at lab4 for formatting. Should be a timeval
	int LEDs[2]; 				// = {false, false, false};
	int buttons[3]; 			// = {false, false, false, false, false};
	double voltage; 			//string because it was difficult to do a string to float conversion
	char eventChange[MSG_SIZE]; //If an LED was turned on or off, which one? Same for buttons
} Data;

Data rtu[3]; //stores up to 3 rtu data
status_t event;
char* tokens;

void error(const char *);

int main(int argc, char *argv[]) {
	int sock, n;
	unsigned int length;
	struct sockaddr_in server, from;
	struct hostent *hp;
	char fileName[15];
	char buffer[MSG_SIZE];
	char buffer2[MSG_SIZE];
	FILE *fp;

	strcpy(buffer, "HANDSHAKE");
	if (argc != 3) {
		printf("usage %s hostname port\n", argv[0]);
		exit(1);
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
	if (sock < 0)
		error("socket");

	server.sin_family = AF_INET; // symbol constant for Internet domain
	hp = gethostbyname(argv[1]); // converts hostname input (e.g. 10.3.52.15)
	if (hp == 0)
		error("Unknown host");

	bcopy((char *) hp->h_addr, (char *) &server.sin_addr, hp->h_length);
	server.sin_port = htons(atoi(argv[2])); // port field
	length = sizeof(struct sockaddr_in); // size of structure
	while (1) {

		//printf("Please  enter the message: ");
		bzero(buffer2, MSG_SIZE); // sets all values to zero. memset() could be used

		n = sendto(sock, buffer, strlen(buffer), 0,
				(const struct sockaddr *) &server, length);

		// receive from server
		n = recvfrom(sock, buffer2, MSG_SIZE, 0, (struct sockaddr *) &from,
				&length);
		if (n < 0)
			error("recvfrom");

		//Parser needs to be fprintf(fp, "LED3: %d\n", rtu[1].LEDs[2]);put in a function
		tokens = strtok(buffer2, "|");
		rtu[1].ID = atoi(tokens);

		snprintf(fileName, sizeof(char)*15, "Log%d.txt", rtu[1].ID);

		fp = fopen(fileName, "ab+");	//Name using ID num from RTU?c d

		//Start Parser
		tokens = strtok(NULL, "|");
		sscanf(tokens, "%lf", &rtu[1].timestamp);
		tokens = strtok(NULL, "|");
		rtu[1].LEDs[0] = atoi(tokens);
		tokens = strtok(NULL, "|");
		rtu[1].LEDs[1] = atoi(tokens);
		tokens = strtok(NULL, "|");
		rtu[1].buttons[0] = atoi(tokens);
		tokens = strtok(NULL, "|");
		rtu[1].buttons[1] = atoi(tokens);
		tokens = strtok(NULL, "|");
		rtu[1].buttons[2] = atoi(tokens);
		tokens = strtok(NULL, "|");
		sscanf(tokens, "%lf", &rtu[1].voltage);
		//End parser

		printf("ID: %d\n", rtu[1].ID);
		printf("Time Stamp: %lf\n", rtu[1].timestamp);
		printf("LED1: %d\n", rtu[1].LEDs[0]);
		printf("LED2: %d\n", rtu[1].LEDs[1]);
		printf("Button1: %d\n", rtu[1].buttons[0]);
		printf("Button2: %d\n", rtu[1].buttons[1]);
		printf("Button3: %d\n", rtu[1].buttons[2]);
		printf("Voltage: %.3lf\n\n", rtu[1].voltage);

		fprintf(fp, "ID: %d\n", rtu[1].ID);
		fprintf(fp, "Time Stamp: %lf\n", rtu[1].timestamp);
		fprintf(fp, "LED1: %d\n", rtu[1].LEDs[0]);
		fprintf(fp, "LED2: %d\n", rtu[1].LEDs[1]);
		fprintf(fp, "Button1: %d\n", rtu[1].buttons[0]);
		fprintf(fp, "Button2: %d\n", rtu[1].buttons[1]);
		fprintf(fp, "Button3: %d\n", rtu[1].buttons[2]);
		fprintf(fp, "Voltage: %.3lf\n", rtu[1].voltage);
		fprintf(fp, "\n");

		fclose(fp);
	}
	fclose(fp);
	close(sock); // close socket.
	return 0;
}

//USE FOR SORTING. Put in function? sprintf based on current log files
//system("cat Log1.txt Log2.txt | sort -t'|' -k3 -n";


void error(const char *msg) {
	perror(msg);
	exit(0);
}
