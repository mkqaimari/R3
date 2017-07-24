#ifndef MEMORY_MANGER_H
#define MEMORY_MANGER_H

#include <x86/BootInfo.h>
#include <x86/Types.h>

#define MEMORY_PAGE_SIZE        	(4*1024)
#define PAGES_BIT_MAP_SIZE		(0x8000)	
#define PAGE_TABLE_DIRECTORY_SIZE	(1024)


extern const uint32_t noname8,noname7;

void initMemoryManager(const struct  BootInfo* bootInfo);

const struct MemoryAllocationInfo allocateNonPagable(uint32_t byteCount);

void freeNonPagable(const struct MemoryAllocationInfo* allocationInfo);

unsigned int getKernelStartAddress();

unsigned int getKernelImageSize();

struct MemoryAllocationInfo {
	bool  valid;
	uint32_t startAddress;
	uint32_t byteCount;
};

struct PageTableEntry {

	uint8_t	present:1;
	uint8_t	writeable:1;
	uint8_t	userPage:1;
	uint8_t	writeTransparent:1;
	uint8_t	cacheDisabled:1;
	uint8_t	acccessed:1;
	uint8_t	dirty:1;
	uint8_t	longPage:1;
	uint8_t	reserved:1;//must be 0
	uint8_t systemFlags:3;
	uint32_t pageFrameAddress:20;
};

extern PageTableEntry* kernelPTDirectory;


#endif
