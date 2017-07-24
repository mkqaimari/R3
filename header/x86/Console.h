#ifndef CONSOLE_H
#define CONSOLE_H

void initConsole();

//static void write(const unsigned char value);

void clearConsole();

void writeString(const char* string);

void writeStringN(const char* string,int length);

void writeLong(long value);

#endif
