#include <x86/IRQ.h>
#include <x86/I386.h>
#include <x86/Timer.h>
#include <x86/Console.h>
#include <x86/Logger.h>

#define IRQ0_INT_NUMBER	32
#define IRQ8_INT_NUMBER 47



uint32_t irqHandler(IRQValues* irqValues)
{

//	debugString("q");
//	debugIRQValues(irqValues);
//      kernelSleep(30000);

	//Do whatever you want here:
	if((irqValues->interruptNumber-32)==0){

		uint32_t newESP=handleTimerTick(irqValues);
	        ::writePortByte(0x20, 0x20);//Master PIC          
		return newESP; 
	} 
	
	if(irqValues->interruptNumber >= IRQ0_INT_NUMBER && irqValues->interruptNumber<IRQ8_INT_NUMBER){//send EOI to the first PIC only
	
		//write('M');
		::writePortByte(0x20, 0x20);//Master PIC           
		    
	}else{//send EOI to both PICs
		//write('S');
		::writePortByte(0xA0, 0x20);//Slave PIC
		::writePortByte(0x20, 0x20);//Master PIC		
	}
	
	return 0xFFFFFFFF;
}
 
 extern "C"  uint32_t irqHandlerCWrapper(IRQValues* irqValues)
{
	return irqHandler(irqValues);
}


void remapIRQs()
{   
    ::writePortByte(0x20, 0x11);
    ::writePortByte(0xA0, 0x11);
    ::writePortByte(0x21, 0x20);
    ::writePortByte(0xA1, 0x28);
    ::writePortByte(0x21, 0x04);
    ::writePortByte(0xA1, 0x02);
    ::writePortByte(0x21, 0x01);
    ::writePortByte(0xA1, 0x01);
    ::writePortByte(0x21, 0x0);
    ::writePortByte(0xA1, 0x0);

    return ;
}
