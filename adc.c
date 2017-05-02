#ifndef MODULE 
#define MODULE
#endif

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/time.h>
//#include <rtai.h>
//#include <rtai_sched.h>
//#include <rtai_sem.h>

#include "status.h"

MODULE_LICENSE("GPL");

#define MSG_SIZE 150
#define BUF_SIZE 40

#define BTN1 0
#define BTN2 1
#define BTN3 2
#define LED1 6
#define LED2 7

#define FIFO_WRITE 0

const int hw_irq = 59;

// A/D registers MAX197
// See page 29 TS-7250 manual.
unsigned long *baseptrA = NULL, *baseptrB = NULL;
unsigned long *PBDR, *PBDDR;
//See page 54 of EP9301 Manual
unsigned long *GPIOBIntEn;	 	//Controlling the generation of interrupts by the pins of Port B
unsigned long *GPIOBIntType1;	//Register controlling type, level or edge, of interrupt generated by the pins of port B
unsigned long *GPIOBIntType2;	//Register controlling polarity, high/low or rising/falling, of interrupt generated by Port B
unsigned long *GPIOBEOI;  		//GPIO Port B End of Interrupt Register
unsigned long *GPIOBDB;		 	//GPIO B Debounce Register
unsigned long *IntStsB; 	 	//GPIO Interrupt Status Register. Contains status of Port B interrupts after masking.
unsigned long *ADCInitiate = NULL, *ADCOptInstalled = NULL, *ADCConvComplete = NULL;
volatile uint16_t *ADCResult = NULL;

static void button_handler(int irq_num, void *cookie) {
	rt_disable_irq(irq_num);

	int i=0;	
	status_t st; // Struct to populate with values
	
	// If RawIntSts == 1 then that button was pressed
	for(i=0; i<NUM_BUTTONS; i++)
	{
		// Record raw port status
		st.button_status[i] = (*PBDR & 1 << i) ? 1 : 0; // Assign 1 if not 0
		// Determine which event occurred
		if( (*RawIntStsB & (1 << i)) ) 
		{ // Look for button event 
			// Button<i> event
			if( (*GPIOBIntType2 & (1 << i)) ) { 
				// Rising edge event
				switch(i) {
					case 0: st.event_type = BTN0_RISING; break;
					case 1: st.event_type = BTN1_RISING; break;
					case 2: st.event_type = BTN2_RISING; break;
					default: break;
				}
				// Set interrupt type to falling edge
				*GPIOBIntType2 &= ~(0x07);
			}
			else { // Falling edge event
				switch(i) { 
					case 0: st.event_type = BTN0_FALLING; break;
					case 1: st.event_type = BTN1_FALLING; break;
					case 2: st.event_type = BTN2_FALLING; break;
				}
				// Set interrupt type to rising edge
				*GPIOBIntType2 |= (0x07);
			}
		}
	}
	// Get ADC Value
	// TODO 
	st.line_voltage = 555;	
	// Get event time
	do_gettimeofday(&(st.event_tv));
	// Write to fifo
	int ret = rtf_put(FIFO_WRITE, &st, sizeof(st));
	// Clear EOI register by *setting* the bit.
	*GPIOBEOI |= (0x1F);

	// Re-enable interrupts
	rt_enable_irq(irq_num);
}

int init_module(void) {
	// Attempt to map address space
	baseptrA = (unsigned long *)__ioremap(0x80840000, 4096, 0);
	baseptrB = (unsigned long *)__ioremap(0x800C0000, 4096, 0);//See page 51 of EP3901 Manual
	if( NULL == baseptrA || NULL == baseptrB )  {
		printk("Unable to map memory for base pointers\n");
		return -1;
	}
	// Map registers
	PBDR = (unsigned long *)((char *)baseptrA + 0x04); 
	PBDDR = (unsigned long *)((char *)baseptrA + 0x14);
	GPIOBIntEn = (unsigned long *)((char *)baseptrA + 0xB8);
	GPIOBIntType1 = (unsigned long *)((char *)baseptrA + 0xAC);
	GPIOBIntType2 = (unsigned long *)((char *)baseptrA + 0xB0);
	GPIOBEOI = (unsigned long *)((char *)baseptrA + 0xB4);
	GPIOBDB = (unsigned long *)((char *)baseptrA + 0xC4);
	IntStsB = (unsigned long *)((char *)baseptrA + 0xBC);
        // Setup Port B
	*PBDDR &= ~(1 << BTN1 || 1 << BTN2 || 1 << BTN3); // Set buttons as input
	*PBDR |= (1 << LED1 || 1 << LED2);	       	//indicator LED
	*GPIOBEOI |= 0x07;		//make sure interrupt is cleared
	*GPIOBIntEn = 0x07;		//allow interrupt on buttons
	*GPIOBIntType1 = 0x07;		//portB 0~2 edge sensitive
	*GPIOBIntType2 &= ~(0x07);	//portB 0~2 rising edge sensitive
	*GPIOBDB = 0x07;		//allow button debouncing

	ADCOptInstalled = (unsigned long *) __ioremap(0x22400000, 4096, 0);
	ADCInitiate = (unsigned long *) __ioremap(0x10F00000, 4096, 0);
	ADCConvComplete = (unsigned long *) __ioremap(0x10800000, 4096, 0);
	ADCResult = (uint16_t *)ADCInitiate;
	
	if( !ADCOptInstalled || !ADCInitiate || !ADCConvComplete )
	{ 
		printk("Unable to map memory space for ADC\n");
		return -1;
	}
	else if( ! (*ADCOptInstalled & (0x01)) ) // ADC Installed when LSB is set;
	{
		printk("ADC option not installed\n");
		return -1;
	}
	// Read value of ADC
	//// Sample ADC channel 0, 0-5v range
	//*ADCInitiate = 0x40;
	//// Busy wait
	//// Bit 7 is set when conversion is done
	//while( !(0x80 & *ADCConvComplete) ) { }
	//int result = (int)*ADCResult;

	// Attempt to attach handler
	if(rt_request_irq(hw_irq, button_handler, NULL, 1) < 0)
	{
		printk("Unable to request IRQ\n");
		return -1;
	}
	
	rt_enable_irq(hw_irq);

	if(rtf_create(FIFO_WRITE, sizeof(status_t)) < 0) {
		printk("Unable to create fifo\n");
		return -1;
	}
	
	printk("MODULE INSTALLED\n");
	return 0; 
}
		
void cleanup_module(void) {
	rt_disable_irq(hw_irq);
	rt_release_irq(hw_irq);
	rtf_destroy(FIFO_WRITE);
	printk("MODULE_REMOVED\n");
}

