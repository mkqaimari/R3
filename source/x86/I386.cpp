#include <x86/I386.h>


 unsigned char readPortByte (const unsigned short _port)
{
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

 void writePortByte (const unsigned short _port,const unsigned char _data)
{
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}


 unsigned short readPortShort (const unsigned short _port)
{
    unsigned short rv;
    __asm__ __volatile__ ("inw %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

 void writePortShort (const unsigned short _port,const unsigned short _data)
{
    __asm__ __volatile__ ("outw %1, %0" : : "dN" (_port), "a" (_data));
}

 void halt()
{
	__asm__ __volatile__ ("HLT");
	return ;
}


 void enableInterrupts ()
{
    __asm__ __volatile__ ("STI");
}


 void disableInterrupts ()
{
    __asm__ __volatile__ ("CLI");
}



 short getUnsignedByte(const void* ptr)
{
        
        return *((unsigned char*) ptr);
}

 int getUnsignedShort(const void* ptr)
{       
        return *((unsigned short*)ptr);
}

 long getUnsignedInt(const void* ptr)
{
        return *((unsigned int*)ptr);
}


 void delay15MicroSeconds()
{

        unsigned char systemStatus=readPortByte(0x61);	
	systemStatus&=0x10;
	unsigned char lastSystemStatus=systemStatus;

	while(1){

		if(lastSystemStatus!=systemStatus){//a toggle
			break;	
		}

		lastSystemStatus= systemStatus;
		systemStatus= readPortByte(0x61);
		systemStatus&=0x10;
	
	}

	return ;
}


  
