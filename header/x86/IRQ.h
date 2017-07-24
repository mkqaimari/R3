#ifndef IRQ_H
#define IRQ_H

#include <x86/Types.h>
#include <x86/I386.h>

void remapIRQs();

extern "C" uint32_t irqHandler(IRQValues* irqValues);


#endif
