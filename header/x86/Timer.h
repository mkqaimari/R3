#ifndef TIMER_H
#define TIMER_H

#include <x86/I386.h>

#define	CLOCK_RATE      	1193180L
#define	TICKS_PER_SECOND   	10000

void kernelSleep(long millis);

void setupTimer(int frequency);

uint32_t handleTimerTick(IRQValues* irqValues);


#endif
