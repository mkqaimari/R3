#ifndef IDT_H
#define IDT_H

#include <x86/I386.h>

struct IDTPointer {

	unsigned short 	limit;
	unsigned int 	base;



}__attribute__((packed));//not like java ;)

struct IDTEntry {

	unsigned short baseLow;
    	unsigned short segment;        
  	unsigned char always0;     
    	unsigned char flags;       
    	unsigned short baseHigh;

}__attribute__((packed));

void setupIDT();	//setup the 256 entry table and init it with zeros

extern "C" void loadIDT();		//the facade of an assembly proc

void installIDTGate(unsigned char gateNumber,unsigned short segment,unsigned int address,unsigned char flags);

extern "C" void exceptionHandler(struct ISRValues* );

#endif
