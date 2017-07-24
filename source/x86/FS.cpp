#include <x86/FS.h>
#include <x86/FAT32.h>
#include <x86/Logger.h>
#include <x86/DOSDisk.h>
#include <x86/BootInfo.h>
#include <x86/IDE.h>
#include <x86/UTF8.h>

static struct MBR mbr;

FileDescriptor openFiles[MAX_KERNEL_OPEN_FILES];//the kernel can open upto 128 files

void initFSModule(){

	for(int i=0;i<MAX_KERNEL_OPEN_FILES;i++){

		openFiles[i].valid=false;
		openFiles[i].pointer=0;
		openFiles[i].fileLength=0;

	}

	debugString("\nDetecting ATA devices: ");
        detectDevices();
        resetDevices();

        readMBR(0,&mbr);

/*
	debugString("\nMBR reading done: ");
	debugLong(mbr.mbrSignature[0]);
	debugLong(mbr.mbrSignature[1]);

	debugString("\nMBR partition0 status: ");debugLong(mbr.partition0[0]);
	debugString("\nMBR partition1 status: ");debugLong(mbr.partition1[0]);
	debugString("\nMBR partition2 status: ");debugLong(mbr.partition2[0]);
	debugString("\nMBR partition3 status: ");debugLong(mbr.partition3[0]);
*/
//        char asciiSource[]="/a_very_long_file_name_best_wishes/dir2/dir3/dir4/dir5/dir6/";
//        UCS2 ucs2FileName[sizeof(asciiSource)+1];

//        fillUCS2String(ucs2FileName,sizeof(asciiSource),asciiSource);
//        ucs2FileName[sizeof(asciiSource)]=0;
//        clearConsole();
//        long clusterNumber= open(0,&mbr,(struct PartitionEntry*)mbr.partition0,ucs2FileName);
//        debugString("\ntarget dir clsuter number: ");debugLong(clusterNumber);

//        debugString("\nlong long size:"); debugLong(sizeof(uint64_t));


	return ;

}



int open(const UTF8* filePath){

	UCS2 ucs2Path[128];

	UCS2decode(ucs2Path,128,filePath);
/*
	debugString("\nFS.open--type");debugLong(reinterpret_cast<const PartitionEntry*>(mbr.partition0)->type);
	debugString("\nFS.open--status");debugLong(reinterpret_cast<const PartitionEntry*>(mbr.partition0)->status);

*/
	unsigned int fileSize;
	long firstCluster= getFileFirstCluster(DRIVE0,&mbr,reinterpret_cast<const PartitionEntry*>(mbr.partition0),ucs2Path,fileSize);

	debugString("\nFirst cluster number: ");debugLong(firstCluster);

	int nextFileIndex=-1;
	for(int i=0;i<MAX_KERNEL_OPEN_FILES;i++){

		if(!openFiles[i].valid){
			nextFileIndex=i;
			break;
		}

	}

	if(nextFileIndex!=-1){

		openFiles[nextFileIndex].valid=true;
		openFiles[nextFileIndex].path=filePath;
		openFiles[nextFileIndex].pointer=0;
		openFiles[nextFileIndex].fileLength=fileSize;

	}


	return nextFileIndex;
}

unsigned int seek(int fd,unsigned int pointer){

	if( openFiles[fd].valid)
		openFiles[fd].pointer= pointer;
	
	return openFiles[fd].pointer;

}


int read(int fd,char* buffer,int bufferLength){

	debugString("\nFile Length before FAT32 read:");debugLong(openFiles[fd].fileLength);
        int bytesRead=FAT32_read(&mbr,fd,buffer,bufferLength);
	debugString("\nFile Length after FAT32 read:");debugLong(openFiles[fd].fileLength);

	if(bytesRead>0)
		openFiles[fd].pointer+=bytesRead;

	return bytesRead;

}






int write(int fd,char* buffer,int bufferLength){

	return -1;
}


int close(int fd){
	
	openFiles[fd].pointer=0;
	openFiles[fd].valid=false;
	
	return -1;
}

unsigned int getFileLength(int fd){

	return openFiles[fd].fileLength;

}
