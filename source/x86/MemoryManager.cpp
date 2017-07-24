#include <x86/Logger.h>
#include <x86/Types.h>
#include <x86/MemoryManager.h>
#include <x86/BootInfo.h>
#include <x86/Thread.h>

//we need a hashtable that holds start addresses as keys and allocation info values
//minimum allowed allocation size should be parameterized.
//maximum addressable memory size should be stored in MemoryManager.h


static uint32_t pagesBitMap[PAGES_BIT_MAP_SIZE];

static Mutex mmLock;


PageTableEntry* kernelPTDirectory; 

static inline void setPageFlag(uint32_t currentAddress){
    
    uint32_t index1= currentAddress >> 17;
    uint32_t index2= (currentAddress >> 12 ) & 0x1F ;
    
    pagesBitMap[index1]= pagesBitMap[index1] |  (1<<index2);

//    debugString("\n @ ");debugLong(index1);debugString(",");debugLong(index2);
    
    return ;    
}

static inline void clearPageFlag(uint32_t currentAddress){
    
    uint32_t index1= currentAddress >> 17; 
    uint32_t index2= (currentAddress >> 12 ) & 0x1F; 

    pagesBitMap[index1]= pagesBitMap[index1] & ~(1<<index2);
    return ;
}


static void setMemoryRegionBitFields(uint32_t baseAddress32,uint32_t length32){

	if(length32==0){
		debugString("\nllength32==0, no memory allocated");
	}

        //baseAddress32= baseAddress32+ (MEMORY_PAGE_SIZE -1);
        baseAddress32-= (baseAddress32%MEMORY_PAGE_SIZE);//reserving a single byte of a page reserves the whole page
	uint32_t currentAddress= baseAddress32;

	setPageFlag(baseAddress32);//at least one page must be set

        while(currentAddress +MEMORY_PAGE_SIZE< (baseAddress32+length32)){

                setPageFlag(currentAddress);
                currentAddress+=MEMORY_PAGE_SIZE;

        }

        debugString("\nSetting Memory Page from : ");debugLong(baseAddress32);
        debugString(" to: ");debugLong(currentAddress-1);


        return ;
}


const struct MemoryAllocationInfo allocateNonPagable(uint32_t byteCount){

	enterCriticalSection(&mmLock);
	//first-fit alogrithm is used here

	uint32_t pageCount= byteCount/MEMORY_PAGE_SIZE;

	if(pageCount==0)
		++pageCount;

	if(byteCount%MEMORY_PAGE_SIZE!=0)
		++pageCount;

	uint32_t pageIndex=0;
	

	while(pageIndex <  /*32 bits, single bit per page */ 32* PAGES_BIT_MAP_SIZE){
		
		uint32_t freePageCount=0;
		uint32_t allocatedStartPageIndex= pageIndex;
		while((pagesBitMap[pageIndex>>5] & (0x01 << (pageIndex & 0x1F))) ==0 && 
			freePageCount < pageCount ){//find the first incident of a free page
		
			++pageIndex;
			++freePageCount;
			debugString("\nFree page count:");debugLong(freePageCount); 
		}

		if(freePageCount==pageCount){
			setMemoryRegionBitFields(allocatedStartPageIndex* MEMORY_PAGE_SIZE,pageCount* MEMORY_PAGE_SIZE);
			debugString("\nallocatedStartPageIndex: ");debugLong(allocatedStartPageIndex);
			struct MemoryAllocationInfo allocationInfo= {true, allocatedStartPageIndex* MEMORY_PAGE_SIZE,pageCount* MEMORY_PAGE_SIZE};
			leaveCriticalSection(&mmLock);
			return allocationInfo;
		}else{
			//debugString("\nUnreachable code executed....");
			++pageIndex;
		}
	}

	struct MemoryAllocationInfo allocationInfo= {false,0,0};
	leaveCriticalSection(&mmLock);
	return allocationInfo;
}


void freeNonPagable(const struct MemoryAllocationInfo* allocationInfo)
{
	enterCriticalSection(&mmLock);
        uint32_t startAddress= allocationInfo->startAddress;
        uint32_t pageIndex= startAddress/MEMORY_PAGE_SIZE;
        
        if(startAddress%MEMORY_PAGE_SIZE!=0)
            ++pageIndex;
        
        uint32_t pageCount= allocationInfo->byteCount / MEMORY_PAGE_SIZE;//already adjusted to page boundry by allocation code

        for(uint32_t i=0;i<pageCount;i++)
            clearPageFlag(pageIndex+i);

	leaveCriticalSection(&mmLock);
	return ;
}

static void clearMemoryRegionBitFields(uint32_t baseAddress,uint32_t length){


	uint32_t baseAddress32= (uint32_t) baseAddress;
	const uint32_t length32= (uint32_t) length;
	uint32_t currentAddress= baseAddress32;

	baseAddress32= baseAddress32+ (MEMORY_PAGE_SIZE	-1);
	baseAddress32-= (baseAddress32%MEMORY_PAGE_SIZE);

	while(currentAddress +MEMORY_PAGE_SIZE< (baseAddress32+length32)){

		uint32_t index1= currentAddress >> 17; 
		uint32_t index2= (currentAddress >> 12) & 0x1F; 

		pagesBitMap[index1]= pagesBitMap[index1] & ~(1<<index2);
		
		currentAddress+=MEMORY_PAGE_SIZE;

	}

	debugString("\nClearing Memory Page from : ");debugLong(baseAddress32);
        debugString(" to: ");debugLong(currentAddress);


	return ;
}


void initMemoryManager(const struct BootInfo* bootInfo)
{

	uint32_t i;
	for(i=0;i<PAGES_BIT_MAP_SIZE;i++)
		pagesBitMap[i]=0xFFFFFFFF;//make memory pages reserved then clear free region later


	if(bootInfo->flags & MEMORY_MAP_VALID){
	
		uint32_t memoryMapSize= bootInfo->memoryMapByteCount;
		struct MemoryMapEntry* currentEntry= (struct MemoryMapEntry*) bootInfo->memoryMapAddress;
	        	
		for(i=0;i<memoryMapSize;){

			uint32_t currentEntrySize= currentEntry->size+4;
			if(currentEntry->type==1){//it's an empty pool	
				
				clearMemoryRegionBitFields(currentEntry->baseAddress,currentEntry->length);					
			}
		
			i+=currentEntrySize;
			currentEntry= (struct MemoryMapEntry*) (((char*)bootInfo->memoryMapAddress)+i);
		}
	
	}

	debugString("\nReserving sensitive memory area");

	debugString("\nKernel start Address: ");debugLong(getKernelStartAddress());
        debugString("\nKernel image size   : ");debugLong(getKernelImageSize());

	int kernelImageSize= getKernelImageSize(); 


	setMemoryRegionBitFields(0,0x100000);//lower 1M of memory shouldn't be used
	setMemoryRegionBitFields(noname8,kernelImageSize+MEMORY_PAGE_SIZE);//reserve kernel image memory

	//allocate kernel-space page table directory
	
	MemoryAllocationInfo allocationInfo= allocateNonPagable(1024*4);

	kernelPTDirectory= reinterpret_cast<PageTableEntry*>(allocationInfo.startAddress);

	debugString("\nPTDir address: ");debugLong(allocationInfo.startAddress);

	//init kernel-space page table directory

	for(uint32_t i=0;i<PAGE_TABLE_DIRECTORY_SIZE;i++)
		reinterpret_cast<uint32_t*>(kernelPTDirectory)[i]= 0;//clear all flags


	return ;
}

unsigned int getKernelStartAddress(){

	return noname8;
}


unsigned int getKernelImageSize(){

	return noname7-noname8 ; 
}
