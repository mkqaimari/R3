#include <x86/ELF.h>
#include <x86/FS.h>
#include <x86/Logger.h>
#include <x86/MemoryManager.h>


//static char clusterBuffer[512];

void parseELF(const UTF8* filePath){

	setDebugEnabled(true);

	MemoryAllocationInfo memoryAllocationInfo= allocateNonPagable(512*8);
	char * clusterBuffer= reinterpret_cast<char*>(memoryAllocationInfo.startAddress);

	debugString("\nAddress of allocated memory: ");debugLong(memoryAllocationInfo.startAddress);
	debugString("\nNumber of bytes allcated: ");debugLong(memoryAllocationInfo.byteCount);


  	int fd = open(filePath);

	if(fd==-1){
                debugString("Unable to open /ELF.BIN\n");
                return ;
         }{
		debugString("\nFile opened with fd = ");debugLong(fd);
	 }

        int bytesRead=read(fd,clusterBuffer,memoryAllocationInfo.byteCount);

         if(bytesRead==-1){

	         debugString("Unable to read Elf32 header\n");
                 close(fd);
		 freeNonPagable(&memoryAllocationInfo);
                 return ;
	 }

   	 Elf32_Ehdr* pELFHeader= reinterpret_cast<Elf32_Ehdr*>(clusterBuffer);

	 debugString("\ne_type:");debugLong(pELFHeader->e_type);
     	 debugString("\ne_machine:");debugLong(pELFHeader->e_machine); 
     	 debugString("\ne_entry:");debugLong(pELFHeader->e_entry); 
 	 debugString("\ne_ehsize: ");debugLong(pELFHeader->e_ehsize);     
     	 debugString("\ne_phoff: ");debugLong(pELFHeader->e_phoff); 
	 debugString("\ne_phnum: ");debugLong(pELFHeader->e_phnum);
	 debugString("\ne_phentsize: ");debugLong(pELFHeader->e_phentsize);
	 debugString("\ne_phnum: ");debugLong(pELFHeader->e_phnum);
	 debugString("\ne_shnum: ");debugLong(pELFHeader->e_shnum);
	 debugString("\ne_shstrndx: ");debugLong(pELFHeader->e_shstrndx); 
	 debugString("\ne_ident[EI_CLASS]: ");debugLong(pELFHeader->e_ident[EI_CLASS]);
     	 debugString("\ne_ident[EI_DATA]: ");debugLong(pELFHeader->e_ident[EI_DATA]); 
	 debugString("\ne_shoff: ");debugLong(pELFHeader->e_shoff);
	 debugString("\nElf_hdr sizeof: ");debugLong(sizeof(Elf32_Ehdr));
	 debugString("\nElf32_Shdr:");debugLong(sizeof(Elf32_Shdr));

	 int shTableOffset= pELFHeader->e_shoff;
	 unsigned int seekto= seek(fd,shTableOffset+sizeof(Elf32_Shdr)*pELFHeader->e_shstrndx);

	 
	 bytesRead= read(fd,clusterBuffer,memoryAllocationInfo.byteCount);
	 if(bytesRead==-1){
	 	debugString("\nSeek To: ");debugLong(seekto);
		debugString("\nUnable to read sections header");
	 }else{
		//debugStringN(clusterBuffer,bytesRead);
	 }


	 Elf32_Shdr* pStringSectionHeader= reinterpret_cast<Elf32_Shdr*>(clusterBuffer);

	 debugString("\nsection type: ");debugLong(pStringSectionHeader->sh_type);
	 debugString("\nsection offset:");debugLong(pStringSectionHeader->sh_offset);
	 debugString("\nsection size:");debugLong(pStringSectionHeader->sh_size);

	 int stringSectionSize= pStringSectionHeader->sh_size;
	 seek(fd,pStringSectionHeader->sh_offset);
	 read(fd,clusterBuffer,memoryAllocationInfo.byteCount);

	 debugStringN(clusterBuffer,(stringSectionSize));

      	 close(fd);
	 freeNonPagable(&memoryAllocationInfo);

         return ;
 }
