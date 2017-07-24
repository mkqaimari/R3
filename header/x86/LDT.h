#ifndef LDT_H
#define LDT_H

#include <x86/Types.h>

struct LDTEntry
{

        short           limit00_15;
        short           baseAddress00_15;
        char            baseAddress16_23;
        char            access;
        char            granularity;
        char            baseAddress24_31;

}__attribute__((packed));


extern "C" void loadLDT(uint16_t);

void installLDTGate(int gateNumber,int baseAddress,int limit,char access,char granularity);




#endif
