#include <x86/LDT.h>
#include <x86/GDT.h>
#include <x86/Thread.h>

static volatile Mutex ldtLock=0;

/*
void installLDTGate(int gateNumber,int baseAddress,int limit,char access,char granularity){

	installGDTGate(gateNumber,baseAddress,limit,access,granularity);
        return ;
}*/
