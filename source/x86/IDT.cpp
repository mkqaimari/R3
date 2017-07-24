#include <x86/Logger.h>
#include <x86/IDT.h>
#include <x86/Console.h>
#include <x86/Thread.h>

static volatile Mutex idtLock=0;
//Module data vars

struct IDTPointer idtPointer;
struct IDTEntry   idt[256];//256 interrupts :), Old memories die hard

void setupIDT(){


	//Setup the idt pointer that the lidt iinstruction will use
	idtPointer.limit= ( sizeof(struct IDTEntry) * 256 ) - 1;//
	idtPointer.base= (unsigned int) idt;


	//Now zero-out the IDT table
	char* idtBytes= (char*) idt;//

	int i;
	for(i=0;i<=idtPointer.limit;i++)
		idtBytes[i]= 0;

	return ;
}

void installIDTGate(unsigned char gateNumber,unsigned short segment,unsigned int address,unsigned char flags){

	enterCriticalSection(&idtLock);

	idt[gateNumber].baseLow = (address     & 0xFFFF);
	idt[gateNumber].baseHigh= (address>>16) & 0xFFFF;
	idt[gateNumber].always0 = 0x0;
	idt[gateNumber].segment = segment;//Values is copied as-is. The called provides the value shifted by 3 bits to the left
	idt[gateNumber].flags= flags;

	leaveCriticalSection(&idtLock);

	return ;
}

extern "C" void exceptionHandlerCWrapper(struct ISRValues* pISRValues){
    
    exceptionHandler(pISRValues);
    return ;
}

void exceptionHandler(struct ISRValues* pISRValues){
	
	//char message[]="\nAn Exception Took Place.\n";
	//debugString(message,sizeof(message));
	
	debugString("CPU Exception #");debugLong(pISRValues->interruptNumber);writeString("\n");
	debugString("CS: ");debugLong(pISRValues->cs);debugString(" EIP: ");debugLong(pISRValues->eip);
	debugString("\n");debugIRQValues((IRQValues*) pISRValues);
		
	for(;;);
		halt();
}
