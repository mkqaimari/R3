#include <x86/Types.h>
#include <x86/I386.h>
#include <x86/Console.h>
#include <x86/Timer.h>

static bool debugEnabled=false;

void setDebugEnabled(bool v)
{
	debugEnabled=v;
}

bool isDebugEnabled(){

	return debugEnabled;
}

void debugString(const char* str){

        if(debugEnabled)
                writeString(str);
        return;
}

void debugLong(long value){
        if(debugEnabled)
                writeLong(value);
        return ;
}

void debugStringN(const char* str,int length){

        if(debugEnabled)
                writeStringN(str,length);

        return ;
}

/*
struct ISRValues {

        unsigned int gs, fs, es, ds;       pushed the segs last 
        unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;   pushed by 'pusha' 
        unsigned int interruptNumber, errorCode;     our 'push byte #' and ecodes do this 
        unsigned int eip, cs, eflags, userESP, userSS;    pushed by the processor automatically 

}__attribute__((packed));
*/


void debugIRQValues(IRQValues* values){


	debugString("IRQValues :\n");
	debugString("segment registers:\n");

	debugString(" gs :");debugLong(values->gs);debugString("\n");
        debugString(" fs :");debugLong(values->fs);debugString("\n");
        debugString(" es :");debugLong(values->es);debugString("\n");
        debugString(" ds :");debugLong(values->ds);debugString("\n");
        //debugString(" userSS :");debugLong(values->userSS);debugString("\n");
        debugString(" cs :");debugLong(values->cs);debugString("\n");
	
//	debugString("general purpose registers:\n");

//        debugString(" edi :");debugLong(values->edi);debugString("\n");
//       debugString(" esi :");debugLong(values->esi);debugString("\n");
//        debugString(" ebp :");debugLong(values->ebp);debugString("\n");
        debugString("-> esp :");debugLong(values->esp);debugString("\n");
//        debugString(" ebx :");debugLong(values->ebx);debugString("\n");
//        debugString(" edx :");debugLong(values->edx);debugString("\n");
//        debugString(" ecx :");debugLong(values->ecx);debugString("\n");
//        debugString(" eax :");debugLong(values->eax);debugString("\n");
        debugString(" int# :");debugLong(values->interruptNumber);debugString("\n");
        debugString(" error# :");debugLong(values->errorCode);debugString("\n");
        debugString(" eip :");debugLong(values->eip);debugString("\n");
        debugString(" eflags :");debugLong(values->eflags);debugString("\n");
//        debugString("-> userESP :");debugLong(values->userESP);debugString("\n");

//	kernelSleep(4000);

}

extern "C" void debugIRQValuesCWrapper(IRQValues* values){

        debugIRQValues(values);
        return ;
}

