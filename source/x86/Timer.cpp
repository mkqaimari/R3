#include <x86/Thread.h>
#include <x86/I386.h>
#include <x86/Timer.h>
#include <x86/Logger.h>
#include <x86/IRQ.h>



void setupTimer(int frequency)
{

	int divisor= CLOCK_RATE/frequency;

	//use cannel 0
	//use mode-3 (i.e. square wave mode)
	//raad/write mode LSB+MSB
	//bin, not BCD

	
	writePortByte(0x43,0x36);
	writePortByte(0x40,(char)divisor & 0xFF);
	writePortByte(0x40,(char)divisor>>8);
	
	return ;
}


//This function returns the new ESP

bool saveNextState=false;

uint32_t handleTimerTick(IRQValues* irqValues)
{
	//an atomic operation cause it's an interrupt handler

//	debugString("T");

//	unsigned long _v;
//	__asm__("mov %%ss,%w0":"=r" (_v):"0" (0));

//	debugString("\ncurrent ss:");debugLong(_v);
	
	//debugString("\nirqValues address: ");	debugLong(reinterpret_cast<uint32_t>(irqValues));
//	debugString("IRQValues recieved:\n");
//	debugIRQValues(irqValues);
       //kernelSleep(150000);

	if(saveNextState){
		saveCurrentThreadState(irqValues);
	}

	int newESP=0xFFFFFFFF;

//	saveCurrentThreadState(values);
	//debugString("select Next Thread\n");
	ThreadInfo* threadInfo= (ThreadInfo*) selectNextThread();

	if(threadInfo!=NULL){	
		//*values=threadInfo->threadState;//let the compiler make it
//		debugString("-");
		saveNextState=true;

		newESP= threadInfo->threadESP;//last thread esp
	//	debugString("\nNewESP: ");debugLong(newESP);
	//	debugIRQValues((IRQValues*)newESP);
//		debugString("\nNewESP: ");debugLong(newESP);
		//debugString("\nselected thread eip:");debugLong(threadInfo->threadState.eip);	
	//	debugString("\nSelected Thread ESP:");debugLong(threadInfo->threadState.userESP);	
//		uint8_t* from= reinterpret_cast<uint8_t*>(&(threadInfo->threadState));
//	        uint8_t* to  = reinterpret_cast<uint8_t*>(values);
/*
struct ISRValues {

        unsigned int gs, fs, es, ds;       pushed the segs last 
        unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;   pushed by 'pusha' 
        unsigned int interruptNumber, errorCode;     our 'push byte #' and ecodes do this 
        unsigned int eip, cs, eflags, userESP, userSS;    pushed by the processor automatically 

}__attribute__((packed));
*/
		

//		saveCurrentThreadState(irqValues);

	}
	else{
//		debugString("\nNo thread assigned");
	}
//	debugString("\nAbout to leave handleTimerTick");
//	debugLong(reinterpret_cast<long>(threadInfo));
//      debugString("IRQValues returned:\n");
//      debugIRQValues(irqValues);

//	kernelSleep(15000);

	return newESP;	
}

void kernelSleep(long millis)
{

	if(millis<=0)
		return ;

	long microDelay= (millis* 67);

	while(microDelay--){
		delay15MicroSeconds();
	}

	return ;
}


