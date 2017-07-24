#ifndef BOOT_INFO_H
#define BOOT_INFO_H

#include <x86/Types.h>

#define MEMORY_MAP_VALID 0x40

struct BootInfo {

	
        const uint32_t	flags;

        const uint32_t	lowerMemory;
	const uint32_t	upperMemory;
	
	const uint32_t	bootDevice;

        const char*	commandLine;
	
	const uint32_t	moduleCount;
	const void*	moduleTableAddress;

	const char 	symbols[16];

	const uint32_t	memoryMapByteCount;
	const void*	memoryMapAddress;

	const uint32_t	driveTableLength;
	const void*	driveTableAddress;

	const void*	configTable;

	const char* 	bootLoaderName;	
	const void* 	apmTableAddress;




}__attribute__((packed));


struct MemoryMapEntry {

	uint32_t	size;
	uint64_t	baseAddress;
	uint64_t	length;
	uint32_t	type;	

}__attribute__((packed));

#endif
