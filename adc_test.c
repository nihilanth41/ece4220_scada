#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
	unsigned long *BasePtr;
	BasePtr = (unsigned long*)mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0x80840000); 
	// A/D registers MAX197
	// See page 29 TS-7250 manual.
	unsigned long *ADCInit, *ADCComplete, *ADCOptionInstalled;

	return EXIT_SUCCESS;
}

