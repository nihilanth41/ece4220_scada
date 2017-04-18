#ifndef MODULE 
#define MODULE
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
//#include <rtai.h>
//#include <rtai_sched.h>
//#include <rtai_sem.h>

MODULE_LICENSE("GPL");

// A/D registers MAX197
// See page 29 TS-7250 manual.


int init_module(void) {
	volatile unsigned char *ADCInitiate = NULL, *ADCOptInstalled = NULL, *ADCConvComplete = NULL;
	volatile uint16_t *ADCResult = NULL;
	// Attempt to map address space
	ADCOptInstalled = (unsigned char *) __ioremap(0x22400000, 4096, 0);
	ADCInitiate = (unsigned char *) __ioremap(0x10F00000, 4096, 0);
	ADCConvComplete = (unsigned char *) __ioremap(0x10800000, 4096, 0);
	ADCResult = (uint16_t *)ADCInitiate;
	
	if( !ADCOptInstalled || !ADCInitiate || !ADCConvComplete )
	{ 
		printk("Unable to map memory space\n");
		return -1;
	}
	else if( ! (*ADCOptInstalled & (0x01)) ) // ADC Installed when LSB is set;
	{
		printk("ADC option not installed\n");
		return -1;
	}
	printk("MODULE INSTALLED\n");
	
	// 5 samples -- just to test
	int i=0;
	for(i=0; i<5; i++)
	{
		// Sample ADC channel 0, 0-5v range
		*ADCInitiate = 0x40;
		// Busy wait
		// Bit 7 is set when conversion is done
		while( !(0x80 & *ADCConvComplete) ) { }
		int result = (int)*ADCResult;
		printk("ADC result: %d\n", result);
	}
	
	return 0; 
}
		
void cleanup_module(void) {
	printk("MODULE_REMOVED\n");
}
