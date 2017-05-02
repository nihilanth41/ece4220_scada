#ifndef __STATUS_H
#define __STATUS_H

#define NUM_BUTTONS 3
#define NUM_LED 2;

enum rtu_event { 
	BTN0_FALLING, // 0
	BTN0_RISING,
	BTN1_FALLING,
	BTN1_RISING,
	BTN2_FALLING,
	BTN2_RISING,
	LINE_OVERVOLT,
	LINE_UNDERVOLT,
	LINE_NOPOWER,
	LED1_ON,
	LED1_OFF,
	LED2_ON,
	LED2_OFF // 12
};

typedef struct status {
	int rtu_id; // Filled in by userspace
	int button_status[NUM_BUTTONS];
	int led_status[NUM_LED]; // Filled in by userspace
	int line_voltage;	
	struct timeval event_tv;
	enum rtu_event event_type;
} status_t;

#endif
