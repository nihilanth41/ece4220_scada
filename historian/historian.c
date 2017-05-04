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
#include <errno.h>
#include "status.h"

#define MSG_SIZE 150			// message size

void error(const char *msg, int errval);
int logEvent(status_t event); 
double adc_to_volts(int adc_val);
void event_to_string(char *buf, int event_type);
void event_to_string(char *buf, int event_type) {
	const char *event_strings[13] = { "Button 0 Falling Edge", 
			   "Button 0 Rising Edge", 
			   "Button 1 Falling Edge",
			   "Button 1 Rising Edge", 
			   "Button 2 Falling Edge",
			   "Button 2 Rising Edge",
			   "Line Overvoltage",
			   "Line Undervoltage",
			   "Line No Power",
			   "LED 1 On",
			   "LED 1 Off",
			   "LED 2 On",
			   "LED 2 Off" };
	sprintf(buf, "%s", event_strings[event_type]);
}


// Convert event struct to string and write to a file.
// Gets filename from rtu_id.
// Params: status_t event 
// Return: 0 for success, nonzero for failure.
int logEvent(status_t event) {
	char filename[MSG_SIZE];
	char log_str[MSG_SIZE];
	char event_str[MSG_SIZE];
	sprintf(filename, "rtu_%d_log.txt", event.rtu_id);
	FILE* fp = fopen(filename, "a+");
	if(NULL == fp) { 
		return -1;
	}
	// rtu_id, time, btn0, btn1, btn2, led1, led2, voltage, event_string
	int rtu_id = event.rtu_id;
	double time = (double)event.event_tv.tv_sec + ((double)event.event_tv.tv_usec*(0.000001));
	int btn0 = event.button_status[0];
	int btn1 = event.button_status[1];
	int btn2 = event.button_status[2];
	int led1 = event.led_status[0];
	int led2 = event.led_status[1];
	double voltage = adc_to_volts(event.line_voltage);
	char *pBuf = (char *)&event_str;
	event_to_string(pBuf, event.event_type);
	pBuf = (char *)&log_str;
	sprintf(pBuf, "%d|%lf|%d|%d|%d|%d|%d|%lf|%s|\n", rtu_id, time, btn0, btn1, btn2, led1, led2, voltage, event_str);
	fprintf(fp, "%s", log_str);
	// debug
	printf("Logging: %s", log_str);

	fclose(fp);
	return 0;
}


int main(int argc, char *argv[]) {
	int sock, n;
	unsigned int length;
	struct sockaddr_in server, from;
	struct hostent *hp;
	char buffer[MSG_SIZE];
	status_t event;
	

	strcpy(buffer, "HANDSHAKE");
	if (argc != 3) {
		fprintf(stderr, "Usage: %s hostname port\n", argv[0]);
		return -1;
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
	if (sock < 0) {
		error("socket()", errno);
	}

	server.sin_family = AF_INET; // symbol constant for Internet domain
	hp = gethostbyname(argv[1]); // converts hostname input (e.g. 10.3.52.15)
	if (hp == 0) {
		error("Unknown host", -1);
	}

	bcopy((char *) hp->h_addr, (char *) &server.sin_addr, hp->h_length);
	server.sin_port = htons(atoi(argv[2])); // port field
	length = sizeof(struct sockaddr_in); // size of structure

	while(1) 
	{

		//printf("Please  enter the message: ");
		bzero((void *)&event, sizeof(event)); // sets all values to zero. memset() could be used

		if( sendto(sock, buffer, strlen(buffer), 0, (const struct sockaddr *) &server, length) < 0 ) {
			error("sendto", errno); 
		}

		// receive from server
		n = recvfrom(sock,&event,sizeof(event), 0, (struct sockaddr *) &from, &length);
		if (n < 0) {
			error("recvfrom", errno);
		}
		else {
			logEvent(event);
		}
		sleep(0);
	}
	close(sock); // close socket.
	return 0;
}

//USE FOR SORTING. Put in function? sprintf based on current log files
//system("cat Log1.txt Log2.txt | sort -t'|' -k3 -n";

double adc_to_volts(int adc_val) {
 const int num_bits = 4095; // 4096-1 (2^12-1)
 const int vref = 5; // 5v
 const double vpd = ( (double)vref / (double)num_bits );
 return (double)adc_val * vpd;
}


void error(const char *msg, int errval) {
	if(NULL == msg) {
		return;
	}
	fprintf(stderr, "Error: %s\n", msg);
	exit(errval);
}
