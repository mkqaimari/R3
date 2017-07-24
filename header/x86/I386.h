#ifndef I386_h 
#define I386_h

#include <x86/Types.h>

//some constants related to 386 descriptor tables.

#define	DESCRIPTOR_TYPE_SYSTEM	(0 << 4)
#define	DESCRIPTOR_TYPE_OTHER	(1 << 4)

#define   DPL_RING_0	(0 << 5)
#define   DPL_RING_3	(3 << 5)

#define	SEGMENT_PRESENT	(1 << 7)
#define	SEGMENT_NOT_PRESENT (0 << 7)

#define	DEFAULT_OPERATION_SIZE_16BIT	(0 << 6)
#define	DEFAULT_OPERATION_SIZE_32BIT	(1 << 6)

#define GRANULARITY_UNIT_BYTE (0 << 7)
#define	GRANULARITY_UNIT_4KB (1 << 7)

#define	SEGMENT_TYPE_CODE (1 << 3)
#define	SEGMENT_TYPE_DATA (0 << 3)


#define DATA_ACCESSED		(1 << 0)

#define DATA_READ_ONLY		(0 << 1)
#define DATA_READ_WRITE		(1 << 1)

#define DATA_EXPAND_UP		(0 << 2)
#define DATA_EXPAND_DOWN	(1 << 2)

#define CODE_ACCESSED			(1 << 0)

#define CODE_READ_ENABLED		(0 << 1)
#define CODE_READ_DISABLED		(1 << 1)

#define CODE_NON_CONFORMANT		(0 << 2)
#define CODE_CONFORMANT			(1 << 2)




struct ISRValues {

	uint32_t gs, fs, es, ds;      /* pushed the segs last */
    	uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    	uint32_t interruptNumber, errorCode;    /* our 'push byte #' and ecodes do this */
    	uint32_t eip, cs;
	uint32_t eflags, /* pushed by the processor automatically */
		 userESP;
	uint32_t  userSS;   /* provided only when switching from a ring to a more prvileged ring */ 

}__attribute__((packed));

typedef struct  ISRValues IRQValues;
typedef	int	Mutex;


extern "C"void isr0();
extern "C"void isr1();
extern "C"void isr2();
extern "C"void isr3();

extern "C"void isr4();
extern "C"void isr5();
extern "C"void isr6();
extern "C"void isr7();

extern "C"void isr8();
extern "C"void isr9();
extern "C"void isr10();
extern "C"void isr11();

extern "C"void isr12();
extern "C"void isr13();
extern "C"void isr14();
extern "C"void isr15();

extern "C"void isr16();
extern "C"void isr17();
extern "C"void isr18();
extern "C"void isr19();

extern "C"void isr20();
extern "C"void isr21();
extern "C"void isr22();
extern "C"void isr23();

extern "C"void isr24();
extern "C"void isr25();
extern "C"void isr26();
extern "C"void isr27();

extern "C"void isr28();
extern "C"void isr29();
extern "C"void isr30();
extern "C"void isr31();

extern "C"void irq0();
extern "C"void irq1();
extern "C"void irq2();
extern "C"void irq3();

extern "C"void irq4();
extern "C"void irq5();
extern "C"void irq6();
extern "C"void irq7();

extern "C"void irq8();
extern "C"void irq9();
extern "C"void irq10();
extern "C"void irq11();

extern "C"void irq12();
extern "C"void irq13();
extern "C"void irq14();
extern "C"void irq15();


 short getUnsignedByte(const void*);
 int getUnsignedShort(const void*);
 long getUnsignedInt(const void*);


 void halt();
 void enableInterrupts ();        
 void disableInterrupts ();



 unsigned char readPortByte(const unsigned short );
 void writePortByte(const unsigned short,const unsigned char);

 unsigned short readPortShort(const unsigned short);
 void writePortShort(const unsigned short,const unsigned short);


 /*void readPortShorts(void* target,const unsigned short portNumber,const unsigned int wordCount);*/

 void delay15MicroSeconds();

extern "C"  int lockMutex(int value,volatile Mutex* pMutex);
extern "C"  void unlockMutex(volatile Mutex* pMutex);
#endif
