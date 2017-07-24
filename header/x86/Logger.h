#ifndef LOGGER_H
#define	LOGGER_H

#include <x86/Types.h>
#include <x86/I386.h>

void setDebugEnabled(bool v);

bool isDebugEnabled();

void debugString(const char* str);

void debugLong(long value);

void debugStringN(const char* str,int  length);

void debugIRQValues(IRQValues* values);

#endif
