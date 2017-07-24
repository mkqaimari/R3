#include <x86/Logger.h>
#include <x86/Timer.h>
#include <x86/GDT.h>
#include <x86/Thread.h>
#include <x86/Types.h>

#define MAX_THREAD_COUNT 128

static Mutex threadsLock=0;

static int threadCount=0;

static int currentThreadIndex=0;

static struct ThreadInfo threads[MAX_THREAD_COUNT];


/**
 * This function isn't thread-safe.
 * The caller (usually the timer handler) must make
 * sure this metod won't get called by multiple threads
 * if any at the same time.
 *
 */

void saveCurrentThreadState(const IRQValues* values){

	if(threadCount==0)
		return ;


     	threads[currentThreadIndex].threadESP= reinterpret_cast<uint32_t>(values);//values->esp-(12*sizeof(uint32_t));//esp before thread state saving 
	
	return ;
}

/**
 * This function has the same rules as saveCurrentThreadState().
 */

const struct ThreadInfo* selectNextThread(){

	if(threadCount==0)
		return NULL;

//	debugString("selectNextThread");
	int nextThreadIndex=currentThreadIndex;

	while(true){

		nextThreadIndex = (nextThreadIndex+1)%MAX_THREAD_COUNT;
		currentThreadIndex= nextThreadIndex;	
//		debugString(".");
		if(threads[nextThreadIndex].valid){
//			debugString("\nnextThreadIndex:");debugLong(nextThreadIndex);	
			return threads+nextThreadIndex;
		}
	}


}

void memzero(uint8_t* targetBytes,uint32_t count){

	for(uint32_t i=0;i<count;i++)
		targetBytes[i]=0;

	return ;
}

	

void initThreadModule(){

	memzero(reinterpret_cast<uint8_t*>(threads),sizeof(threads));
}

//static bool firstCall=true;

static const struct ThreadInfo* registerKernelThreadPrivate(void (*targetFunction)(void),uint8_t* stackStartAddress,uint32_t  stackSizeInBytes);

/**
 * Thread-safe to maintain the integrity of threading structures.
 */

const struct ThreadInfo* registerKernelThread(void (*targetFunction)(void),uint8_t* stackStartAddress,uint32_t  stackSizeInBytes){

	enterCriticalSection(&threadsLock);

	const struct ThreadInfo* pThreadInfo= registerKernelThreadPrivate(targetFunction,stackStartAddress,stackSizeInBytes);

	leaveCriticalSection(&threadsLock);
	return pThreadInfo;
}

static inline const struct ThreadInfo* registerKernelThreadPrivate(void (*targetFunction)(void),uint8_t* stackStartAddress,uint32_t  stackSizeInBytes){


	uint32_t index=0xFFFFFFFF;

	for(uint32_t i=0;i<MAX_THREAD_COUNT;i++){

		if(!threads[i].valid){
			index=i;
			break;
		}

	}

	if(index==0xffffffff){
		debugString("Max number of threads exceeded");
		return NULL;
	}

	debugString("About to  allocate stack space");
//	threads[index].threadStackInfo= allocateNonPagable(stackSizeInBytes);

//	if(!threads[index].threadStackInfo.valid){
		
//		debugString("Unable to allocate thread stack space ");	
//		return NULL;

//	}

        threads[index].valid=true;
        threads[index].processId= KERNEL_THREAD_PROCESS_ID;
	debugString("\na thread registered with eip:");
	debugLong(reinterpret_cast<uint32_t>(targetFunction));
        threads[index].targetFunction= targetFunction;

//	zeroMemory(reinterpret_cast<uint8_t*>(&threads[index].threadState),sizeof(threads[index].threadState));
/*
struct ISRValues {

        unsigned int gs, fs, es, ds;       pushed the segs last 
        unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;   pushed by 'pusha' 
        unsigned int interruptNumber, errorCode;     our 'push byte #' and ecodes do this 
        unsigned int eip, cs, eflags, userESP, userSS;    pushed by the processor automatically 

}__attribute__((packed));
*/
	threads[index].threadStackInfo.startAddress=reinterpret_cast<uint32_t>(stackStartAddress);
	threads[index].threadStackInfo.byteCount=stackSizeInBytes; 


        threads[index].threadESP=threads[index].threadStackInfo.startAddress+threads[index].threadStackInfo.byteCount;
	threads[index].threadESP-=sizeof(IRQValues);
	threads[index].threadESP+= 2*sizeof(uint32_t);//ss:esp 

	IRQValues* newThreadState= reinterpret_cast<IRQValues*>(threads[index].threadESP);

	newThreadState->gs=KERNEL_DATA_SEGMENT_SELECTOR;
	newThreadState->fs=KERNEL_DATA_SEGMENT_SELECTOR;
	newThreadState->es=KERNEL_DATA_SEGMENT_SELECTOR;
	newThreadState->ds=KERNEL_DATA_SEGMENT_SELECTOR;

	newThreadState->ebp=threads[index].threadStackInfo.startAddress+threads[index].threadStackInfo.byteCount - 5*sizeof(uint32_t);
	newThreadState->esp=threads[index].threadStackInfo.startAddress+threads[index].threadStackInfo.byteCount - 5*sizeof(uint32_t);

	newThreadState->eip=reinterpret_cast<uint32_t>(targetFunction);
	newThreadState->cs=KERNEL_CODE_SEGMENT_SELECTOR;
	newThreadState->eflags= 0x200202;
	
	debugString("\nA Thread registered");debugLong(reinterpret_cast<uint32_t>(targetFunction));debugString("index:");debugLong(index);
	++threadCount;	
	return threads+index;	
}

void enterCriticalSection(volatile Mutex* pMutex){

	while(lockMutex(1,pMutex))
		for(int i=0;i<10;i++)
			;//I really don't know how this loop makes 
			//everything workring.	
	return ;
}

void leaveCriticalSection(volatile Mutex* pMutex){
	
	unlockMutex(pMutex);
	return;
}

