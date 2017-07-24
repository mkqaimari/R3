#ifndef FS_H
#define FS_H

#include <x86/DOSDisk.h>
#include <x86/UTF8.h>

void initFSModule();


int open(const UTF8* filePath);


unsigned int seek(int fd,unsigned int pointer);


int read(int fd,char* buffer,int bufferLength);


int write(int fd,char* buffer,int bufferLength);


int close(int fd);


unsigned int getFileLength(int fd);

#define MAX_KERNEL_OPEN_FILES                   128
#define BOOT_MEDIA_PHYSICAL_DRIVE_NUMBER        0x80

struct FileDescriptor{

public://cpp style structs

bool            valid;
const UTF8*     path;
unsigned int    pointer;
unsigned int 	fileLength;

};//


extern FileDescriptor openFiles[MAX_KERNEL_OPEN_FILES];//the kernel can open upto 128 files




#endif
