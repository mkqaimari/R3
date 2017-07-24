#ifndef GDT_H
#define GDT_H

#define KERNEL_CODE_SEGMENT_SELECTOR 0x08
#define KERNEL_DATA_SEGMENT_SELECTOR 0x10

struct GDTEntry 
{

	short		limit00_15;
	short		baseAddress00_15;
	char		baseAddress16_23;
	char		access;
	char 		granularity;
	char   		baseAddress24_31;
	
}__attribute__((packed));

struct GDTPointer
{
	short limit;
	int   baseAddress;

}__attribute__((packed));


void installGDT();

void installGDTGate(int gateNumber,int baseAddress,int limit,char access,char granularity);

extern "C" void flushGDT();


#endif
