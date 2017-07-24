#ifndef THREAD_H
#define THREAD_H

#include <x86/I386.h>
#include <x86/IRQ.h>
#include <x86/Types.h>
#include <x86/MemoryManager.h>


#define KERNEL_THREAD_PROCESS_ID 0xffffffff

struct ThreadInfo {

	uint32_t processId;
	void (* targetFunction)(void);
	struct MemoryAllocationInfo threadStackInfo;
	//IRQValues threadState;
	uint32_t threadESP;
	bool 	valid;

}__attribute__((packed));


void initThreadModule();

void saveCurrentThreadState(const IRQValues* values);

void enterCriticalSection(volatile Mutex*);

void leaveCriticalSection(volatile Mutex*);

const struct ThreadInfo* registerKernelThread(void (*targetFunction)(void),uint8_t* stackStartAddress,uint32_t  stackSizeInBytes);

bool beginThread(const ThreadInfo* threadInfo);

bool endThread(const ThreadInfo* threadInfo);

const struct ThreadInfo* selectNextThread();

#endif
