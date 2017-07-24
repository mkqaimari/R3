#include <x86/Types.h>
#include <x86/Console.h>
#include <x86/Logger.h>
#include <x86/DOSDisk.h>
#include <x86/GDT.h>
#include <x86/IDT.h>
#include <x86/I386.h>
#include <x86/IRQ.h>
#include <x86/Timer.h>
#include <x86/IDE.h>
#include <x86/BootInfo.h>
#include <x86/MemoryManager.h>
#include <x86/Thread.h>
#include <x86/FS.h>
#include <x86/ELFLoader.h>



#define KERNEL_CODE_SEGMENT_SELECTOR 0x08
#define KERNEL_DATA_SEGMENT_SELECTOR 0x10

uint8_t stack3[1024];
uint8_t stack2[1024];
uint8_t stack1[1024];

extern uint32_t KERNEL_STACK_START_ADDRESS;

void mainThreadFinalLoop(){

	

	int x=0;
	while(true){
//		disableInterrupts();
		debugString("\nf ");debugLong(x++);
		halt();
//		enableInterrupts();	
	}

}

void func2();


void func1(){

      	debugString("About to begin func1"); 

//	disableInterrupts();
        registerKernelThread(mainThreadFinalLoop,stack1,1024);
        registerKernelThread(func2,stack3,1024);

//	enableInterrupts();
	
	long x=0;
	while(true){

		debugString("\nfunc1: ");debugLong(x++);
		halt();	
	}

}

void func2(){

//	allocateNonPagable(1024);
	while(true){
	        debugString("\nfunc2");
		halt();
	}

}



/*
void fillUCS2String(UCS2* target,int sourceLength,const char* source)
{
	int i;
	for(i=0;i<sourceLength;i++)
		target[i] = (UCS2) source[i];
	


	return ;
}
*/



 int  kmain(struct BootInfo* bootInfo){

	//FAT32SeekTestData4KiB.txt
	setDebugEnabled(true);

	disableInterrupts();

	initConsole();
	initFSModule();
	

	initMemoryManager(bootInfo);
	//initThreadModule();


	debugString("\nR3 -- Kernel Built on " __DATE__ " @ "__TIME__ "\n");

//	parseELF(reinterpret_cast<const UTF8*>("/ELF.BIN"));
// 	debugString("\nEnd of ELF loading");	


	while(true)
		halt();

	debugString("\nstruct BootInfo size: ");debugLong(sizeof(struct BootInfo));
	debugString("\nIndex of mmap_length: ");debugLong(bootInfo->memoryMapByteCount);


	debugString(bootInfo->commandLine);debugString("\n");
	debugLong(bootInfo->memoryMapByteCount);
//	struct MemoryMapEntry* memoryMap= (struct MemoryMapEntry*) bootInfo->memoryMapAddress;

/*
	debugString("\nPrinting Memory Maps provided by GRUB and there availabity for use: \n");
	//int i;
	for(uint32_t i=0;i<bootInfo->memoryMapByteCount;){

		struct MemoryMapEntry* currentEntry= (struct MemoryMapEntry*)(((char*)memoryMap)+i);
			debugString("\n#");debugLong(i);
			debugString(" Size: ");
			debugLong(currentEntry->size);
			debugString(" Start Address: ");debugLong(currentEntry->baseAddress);
			debugString(" Region Length: ");debugLong(currentEntry->length);
			i+=currentEntry->size+4;	
	}

      */
 
	installGDT();	

	//Now install the NULL entry and both data and code segments of the kernel

//void installGDTGate(int gateNumber,int baseAddress,int limit,char access,char granularity){

	installGDTGate(0,0,0,0,0);
	//code-access: SEGMENT_TYPE_CODE | DESCRIPTOR_TYPE_OTHER | DPL_RING_0 | SEGMENT_PRESENT
	installGDTGate(1,0,0xFFFFFFFF,/* 0x9A */ SEGMENT_TYPE_CODE | DESCRIPTOR_TYPE_OTHER | DPL_RING_0 | SEGMENT_PRESENT | CODE_READ_DISABLED | CODE_NON_CONFORMANT, 
	/* 0xCF */ 0x0F | DEFAULT_OPERATION_SIZE_32BIT | GRANULARITY_UNIT_4KB);//code segment with stupid-me hardcoded values
	debugString("\ncode segment access:");	debugLong(SEGMENT_TYPE_CODE | DESCRIPTOR_TYPE_OTHER | DPL_RING_0 | SEGMENT_PRESENT | CODE_READ_DISABLED | CODE_NON_CONFORMANT);
	debugString("\ncode segment gran:"); debugLong(0x0F | DEFAULT_OPERATION_SIZE_32BIT | GRANULARITY_UNIT_4KB);
	
	installGDTGate(2,0,0XFFFFFFFF,/* 0X92 */ SEGMENT_TYPE_DATA | DESCRIPTOR_TYPE_OTHER | DPL_RING_0 | SEGMENT_PRESENT | DATA_READ_WRITE | DATA_EXPAND_UP ,
	/* 0xCF */  0x0F | DEFAULT_OPERATION_SIZE_32BIT | GRANULARITY_UNIT_4KB);//data segment:


	debugString("\ndata segment access:");debugLong(SEGMENT_TYPE_DATA | DESCRIPTOR_TYPE_OTHER | DPL_RING_0 | SEGMENT_PRESENT | DATA_READ_WRITE | DATA_EXPAND_UP );
        debugString("\ndata segment gran:"); debugLong(0x0F | DEFAULT_OPERATION_SIZE_32BIT | GRANULARITY_UNIT_4KB);
	
	//logical --> linear --> physical

	flushGDT();//set segment registers to point to kernel data and code segments then jump into the old/new code segment
		   //using a logical address composed of segment(16bits):offset(32bits) address
	
        debugString("\nGDT is in use\n");

	//place gates installation code here
	setupIDT();
	loadIDT();
	
	installIDTGate(0,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr0,0x8E);
	installIDTGate(1,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr1,0x8E);
	installIDTGate(2,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr2,0x8E);
	installIDTGate(3,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr3,0x8E);

	installIDTGate(4,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr4,0x8E);
	installIDTGate(5,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr5,0x8E);
	installIDTGate(6,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr6,0x8E);
	installIDTGate(7,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr7,0x8E);

	installIDTGate(8,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr8,0x8E);
	installIDTGate(9,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr9,0x8E);
	installIDTGate(10,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr10,0x8E);
	installIDTGate(11,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr11,0x8E);

	installIDTGate(12,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr12,0x8E);
	installIDTGate(13,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr13,0x8E);
	installIDTGate(14,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr14,0x8E);
	installIDTGate(15,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr15,0x8E);

	installIDTGate(16,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr16,0x8E);
	installIDTGate(17,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr17,0x8E);
	installIDTGate(18,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr18,0x8E);
	installIDTGate(19,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr19,0x8E);

	installIDTGate(20,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr20,0x8E);
	installIDTGate(21,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr21,0x8E);
	installIDTGate(22,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr22,0x8E);
	installIDTGate(23,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr23,0x8E);

	installIDTGate(24,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr24,0x8E);
	installIDTGate(25,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr25,0x8E);
	installIDTGate(26,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr26,0x8E);
	installIDTGate(27,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr27,0x8E);

	installIDTGate(28,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr28,0x8E);
	installIDTGate(29,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr29,0x8E);
	installIDTGate(30,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr30,0x8E);
	installIDTGate(31,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)isr31,0x8E);

	remapIRQs();
	
        debugString("IRQs Remapped \n");

	installIDTGate(32,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq0,0x8E);
	installIDTGate(33,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq1,0x8E);
	installIDTGate(34,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq2,0x8E);
	installIDTGate(35,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq3,0x8E);

	installIDTGate(36,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq4,0x8E);
	installIDTGate(37,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq5,0x8E);
	installIDTGate(38,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq6,0x8E);
	installIDTGate(39,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq7,0x8E);

	installIDTGate(40,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq8,0x8E);
	installIDTGate(41,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq9,0x8E);
	installIDTGate(42,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq10,0x8E);
	installIDTGate(43,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq11,0x8E);

	installIDTGate(44,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq12,0x8E);
	installIDTGate(45,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq13,0x8E);
	installIDTGate(46,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq14,0x8E);
	installIDTGate(47,KERNEL_CODE_SEGMENT_SELECTOR,(unsigned int)irq15,0x8E);

	//setupTimer(1000000);

//	debugString("\nfunc1 address");debugLong((uint32_t)func1);

//	registerKernelThread(mainThreadFinalLoop,stack1,1024);

	debugString("Enabling interrupts");	
	enableInterrupts();

/*
	allocateNonPagable(1024);
	allocateNonPagable(1024);
	allocateNonPagable(1024);

	debugString("Three successive allocations performed \n");
*/
	//enableInterrupts();

/*
	UCS2 fileName[]={'/', 'u', '0', '1', '1', '1', '/', 'd', 'i', 'r','/','d','i','r','2', '1', '1', '1', '/',0};
	UCS2 separator[]={'/',0};
	UCS2 targetBuffer[256];
	int startIndex=0;
	debugString("Started Tokenizing");

*/
        
/*
	int  UCS2Tokenizer(const UCS2* source,const UCS2* token,UCS2* target,int startIndex){
*/

/*
	while((startIndex=UCS2Tokenizer(fileName,separator,targetBuffer,startIndex))!=-1){
		debugString("\nResult File Name Length: ");debugLong(UCS2len(targetBuffer));debugString(" startIndex: ");debugLong(startIndex);

		targetBuffer[0]=0;				

	}


	debugString("\nEnded tokenizing");
		
	while(1);
*/	

 //       initMemoryManager(bootInfo);
	
	initFSModule();
	initThreadModule();


	debugString("\nHalt loop started");
	while(1){
		halt();	
	}


}//The kernels main function.
 
 
extern "C" void kmainCWrapper(struct BootInfo* bootInfo){
 
    kmain(bootInfo);
    
}
