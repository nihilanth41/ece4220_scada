#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <pthread.h>
#include <rtai.h>
#include <rtai_lxrt.h>

#include "status.h"

#define MSG_SIZE 150			// message size

char buf1[MSG_SIZE];
char buf2[MSG_SIZE];
int sock, length, n;
int boolval = 1; // for a socket option
socklen_t fromlen;
struct sockaddr_in server;
struct sockaddr_in from;




void error(const char *msg) {
	perror(msg);
	exit(0);
}

static RT_TASK *Task;
RTIME period;
status_t events;

void *readFromKernel(void *arg) {

	int fifo1 = open("/dev/rtf/0", O_RDWR); //fifo to receive tone message from Auxiliary board
	while (1) {
		if (read(fifo1, &events, sizeof(events)) < 0){
			err(1, "readFromKernel (); read()");
		}
		printf("\nRTU_ID: 5\n");
		printf("Button 1: %d\n", events.button_status[0]);
		printf("Button 2: %d\n", events.button_status[1]);
		printf("Button 3: %d\n", events.button_status[2]);
		printf("Time Stamp: %ld.%ld\n", (long)events.event_tv.tv_sec, (long)events.event_tv.tv_usec);
		printf("Events: %d\n", events.event_type);
		printf("Voltage: %d\n", events.line_voltage);
	}
	pthread_exit(0);
}

void *sendToHistorian(void *args) {
	Task = rt_task_init(nam2num("task1"), 0, 512, 256);
	period = start_rt_timer(nano2count(10000000));
	rt_task_make_periodic(Task, rt_get_time(), period);

	while (1) {

		bzero(buf2, MSG_SIZE);
		// receive from client
		n = recvfrom(sock, buf2, MSG_SIZE, 0, (struct sockaddr *) &from,
				&fromlen);

		n = sendto(sock, buf1, MSG_SIZE, 0, (struct sockaddr *) &from, fromlen);
		rt_task_wait_period();

	}

}

int main(int argc, char *argv[]) {

	pthread_t reader, sender;

	if (argc < 2) {
		fprintf(stderr, "ERROR, no port provided\n");
		exit(0);
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
	if (sock < 0)
		error("Opening socket");

	length = sizeof(server); // length of structure
	bzero(&server, length); // sets all values to zero. memset() could be used
	server.sin_family = AF_INET; // symbol constant for Internet domain
	server.sin_addr.s_addr = INADDR_ANY; // IP address of the machine on which
										 // the server is running
	server.sin_port = htons(atoi(argv[1])); // port number

	// binds the socket to the address of the host and the port number
	if (bind(sock, (struct sockaddr *) &server, length) < 0)
		error("binding");

	// set broadcast option
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval))
			< 0) {
		printf("error setting socket options\n");
		exit(-1);
	}

	fromlen = sizeof(struct sockaddr_in); // size of structure

	pthread_create(&reader, NULL, readFromKernel, NULL);
	pthread_create(&sender, NULL, sendToHistorian, NULL);

	printf("Ready\n");

//	sender = rt_task_init(nam2num("task1"), 0, 512, 256);
//	period = start_rt_timer(nano2count(10000000));
//	rt_task_make_periodic(sender, rt_get_time(), period);


//
//   while (1)
//   {
//	   bzero(buf2, MSG_SIZE);
//	   // receive from client
//       n = recvfrom(sock, buf2, MSG_SIZE, 0, (struct sockaddr *)&from, &fromlen);
//
//
//       n = sendto(sock, buf1, MSG_SIZE, 0, (struct sockaddr *)&from, fromlen);
//       rt_task_wait_period();
//
//
//   }

	pthread_join(reader, NULL);
	//pthread_join(sender, NULL);

	return 0;
}
