#include <x86/GDT.h>
#include <x86/Thread.h>

#define GDT_SIZE 3 

struct GDTEntry gdt[GDT_SIZE];
struct GDTPointer gdtPointer;

static volatile Mutex gdtLock=0;

void installGDT()
{

	gdtPointer.limit= (sizeof(struct GDTEntry)*GDT_SIZE) -1;
	gdtPointer.baseAddress= (unsigned int)  gdt;


	return ;
}


void installGDTGate(int gateNumber,int baseAddress,int limit,char access,char granularity){	

	enterCriticalSection(&gdtLock);

	gdt[gateNumber].baseAddress00_15= baseAddress & 0xFFFF;
	gdt[gateNumber].baseAddress16_23= (baseAddress>>16) & 0xFF;
	gdt[gateNumber].baseAddress24_31= (baseAddress>>24) & 0xFF;

	gdt[gateNumber].limit00_15= limit & 0xFFFF;
	gdt[gateNumber].granularity= ( (limit>>16) & 0xFF);
	gdt[gateNumber].granularity |= (granularity & 0x0F);
	gdt[gateNumber].access= access;

	leaveCriticalSection(&gdtLock);

	return ;
}
