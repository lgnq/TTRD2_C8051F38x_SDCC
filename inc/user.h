#ifndef __USER_H__
#define __USER_H__

#include "main.h"

// SYSCLK frequency in Hz
#define SYSCLK (48000000)

void system_clock_init(uint32_t freq);

void timer_init(uint32_t tick);
void timer_start(void);
void timer_interrupt_enable(void);

void int_enable(void);
void int_disable(void);

void sleep(void);

void mcu_init(void);

void tasks_init(void);

#endif
