#include <x86/Logger.h>
#include <x86/Types.h>
#include <x86/FAT32.h>
#include <x86/I386.h>
#include <x86/UTF8.h>
#include <x86/Console.h>
#include <x86/IDE.h>
#include <x86/FS.h>




typedef long CHS;

//global vars section:

static char  driverBuffer[DRIVER_BUFFER_SIZE];//
static char  clusterContentBuffer[MAX_DIR_CONTENT_SIZE];
//struct   FAT32BootSector bootSector;


// private function prototypes section:
static long computeVolumeDataFirstSectorNumber(const struct FAT32BootSector* bSector);
static long computeClusterFirstSectorNumber(const struct FAT32BootSector*  bSector,long clusterNumber);
//static long LBA2CHS(const struct FAT32BootSector* bSector,long lba);
//static long CHS2LBA(const struct FAT32BootSector* bSector,long cylinder,long head,long sector);
static long computeVolumeDataFirstSectorNumber(const struct FAT32BootSector*);
static int  isFAT32EOC(long fat32Entry);
static long computeClusterFATEntryOffset(const struct MBR* mbr,const struct FAT32BootSector* bSector,long clusterNumber);
static long computeClusterFATEntrySectorNumber(const struct MBR* mbr,const struct FAT32BootSector* bSector,long clusterNumber);
static void extractUCS2Values(const struct FAT32LongDirectoryEntry* longEntry,UCS2* targetBuffer);

//global constants section:

const UCS2 PATH_SEPARATOR[]	=	{'/',UCS2_LINE_TERMINATOR};
const UCS2 ROOT_DIR_PATH[]	=  	{'/',UCS2_LINE_TERMINATOR};



void loadFAT32BootSector(const int physicalDriveNumber,FAT32BootSector* targetSector,long volumeLBA){


        while(!readDataLBA28(physicalDriveNumber,targetSector,COMMAND_READ_SECTORS,0/*NO_FEATURES*/,volumeLBA));
        return ;
}



#define LONG_ENTRY_COUNT_MASK  0x3F
bool isFAT32LongEntry(const struct FAT32DirectoryEntry* dirEntryPtr){
	return (getUnsignedByte(dirEntryPtr->directoryAttribute) 
		& ATTRIBUTE_LONG_NAME_MASK)
		 == ATTRIBUTE_LONG_NAME;
}

bool  doesNameMatchEntry(const struct FAT32DirectoryEntry* dirEntryPtr,const UCS2* name){

	//Entry is not free, start handling it.
        //is it a long entry or not?
        if(isFAT32LongEntry(dirEntryPtr)){ 
      	    debugString("\nLong entry found");                      
	    //this code assumes that the first long entry is marked 
	    //with FAT32_LAST_LONG_DIRECTORY_ENTRY_FLAG flag.
	    //otherwise, it's your own problem
	    
	    const struct FAT32LongDirectoryEntry* longEntry= (const struct FAT32LongDirectoryEntry* ) dirEntryPtr;
	    //const struct FAT32DirectoryEntry *    shortEntry= dirEntryPtr;
	    //longEntry points to the last long entry, not the first 
	    const int longEntryCount= ((getUnsignedByte(longEntry->entryOrder) & /*0x3F*/ LONG_ENTRY_COUNT_MASK));//role-out the 0x40 flag
	    debugString("\ndoesNameMatchEntry.entryOrder value: ");debugLong(getUnsignedByte(longEntry->entryOrder));
            debugString("\ndoesNameMatchEntry.longEntryCount");debugLong(longEntryCount);
	   	    
	    UCS2 extractBuffer[MAX_FILE_NAME_SIZE];//
	    extractBuffer[MAX_FILE_NAME_SIZE-1]=UCS2_LINE_TERMINATOR;
	    int currentOrder;
	    int ucs2TargetIndex= 0;
	    
	    for(currentOrder=longEntryCount-1;currentOrder>=0;currentOrder--){
	         
		extractUCS2Values(longEntry+currentOrder,extractBuffer+ucs2TargetIndex); 
		ucs2TargetIndex+= FAT32_LONG_ENTRY_NAME_CAPACITY;                                 
	    }
	    
	    //now we have the complete subpath
	    if(UCS2cmp(extractBuffer,name)==0){
		debugString("\nMatch Found");
		return true;
	    }
	    else{
		debugString("\nMismatch");
		return false;
	    }
	   
		
	}else{//it's a short entry
		
		debugString("\nProcessing a short entry: \n");
        	    int i;
                    UCS2 entryFileName[13]; 
		    
		    entryFileName[12]=NULL;

        	    for(i=0;i<12;i++)
                           entryFileName[i]=' ';

                    for(i=0; i<8 && (dirEntryPtr->directoryName[i]!=' ') && (dirEntryPtr->directoryName[i]!=NULL) ;i++){
                           entryFileName[i]=  dirEntryPtr->directoryName[i];//
			   debugLong(dirEntryPtr->directoryName[i]);
	    	    }

                    if(dirEntryPtr->directoryName[8]!=' '){

                           entryFileName[i]='.';
				
			   int j=i;

			   while(j<11){
				if(dirEntryPtr->directoryName[j]==' ' || dirEntryPtr->directoryName[j]==NULL){
					++j;
					continue;			
				}else{

				
					entryFileName[i+1]= dirEntryPtr->directoryName[j];
					++i;
					++j;
				}
			   }

			   entryFileName[i+2]=NULL;
	             }

		     debugString("\nentryFileName: ");debugStringN(reinterpret_cast<const char*>(entryFileName),26);
		     debugString("\nname recieved: ");debugStringN(reinterpret_cast<const char*>(name),26); 


		     debugLong(UCS2ncmp(entryFileName,name,12));
                     return (UCS2ncmp(entryFileName,name,12)==0);//match found
	}

}


long getNextCluster(const struct MBR* mbr,const struct FAT32BootSector* bootSector,const long physicalDriveNumber,const long volumeLBA,const long currentCluster){

	
	const long currentClusterLBA= volumeLBA+computeClusterFATEntrySectorNumber(mbr,bootSector,currentCluster);
	const long currentClusterOffset= computeClusterFATEntryOffset(mbr,bootSector,currentCluster);

	debugString("\ngetNextCluster");
	//const CHS  currentClusterCHS= LBA2CHS(&bootSector,currentClusterLBA);

	while(!readDataLBA28(physicalDriveNumber,clusterContentBuffer,COMMAND_READ_SECTORS,0,currentClusterLBA));
	
	long nextCluster=  getUnsignedInt((clusterContentBuffer)+currentClusterOffset);	
	
	return nextCluster;
}

static int getClusterSize(const FAT32BootSector* pBootSector)
{
	const short sectorPerCluster= getUnsignedByte(pBootSector->sectorPerCluster);
        const int bytePerSector= getUnsignedShort(pBootSector->bytePerSector);

        const long clusterSize= bytePerSector* sectorPerCluster;

	return clusterSize;

}

long  readFile(const struct MBR* mbr,const struct FAT32BootSector* bSector,const long physicalDriveNumber,const long volumeLBA ,char* targetBuffer,const long bufferLength,const long startClusterNumberR/*,const long dirStartOffset*/){


	const long clusterSize= getClusterSize(bSector); 
        const short sectorPerCluster= getUnsignedByte(bSector->sectorPerCluster);
        const int bytePerSector= getUnsignedShort(bSector->bytePerSector);


	if(bufferLength%clusterSize!=0){
	
		debugString("\nthe buffer size must be a multiple of the cluster size for this volume");
		return -1;//nothing to read at all :)

	}		

	int i= 0;
	long currentCluster=startClusterNumberR;

	debugString("\nCurrent Cluster Number: ");debugLong(currentCluster);
	
	while(!isFAT32EOC(currentCluster) && i<bufferLength){
	
		debugString("\nanother reading iteration ");debugLong(i);
		if(currentCluster<2){
			
			debugString("\nCluster number can't be less than 2 ");
			return i;	
		}
	
		debugString("\nreadFile.currentClusterNumber");debugLong(currentCluster);
		debugString("\nVolume LBA: ");debugLong(volumeLBA);
		long dataSectorLBA= computeClusterFirstSectorNumber(bSector,currentCluster);
		dataSectorLBA+=(volumeLBA);
		debugString("\nCluster # ");debugLong(currentCluster);debugString(" first sector # ");debugLong(dataSectorLBA);

		for(int sn=0;sn<sectorPerCluster;sn++)
			while(!readDataLBA28(physicalDriveNumber,((char*)targetBuffer)+i+(bytePerSector*sn),COMMAND_READ_SECTORS,0/* NO_FEATURES */,dataSectorLBA+sn));


		//debugStringN(targetBuffer,512);
		i+= (bytePerSector* sectorPerCluster);

		currentCluster= getNextCluster(mbr,bSector,physicalDriveNumber,volumeLBA,currentCluster);
		
		if(isFAT32EOC(currentCluster)){
                        debugString("readFile.nextCluster == EOC ");
			debugString("\n# of bytes read: ");debugLong(i);
			break ; 
		}

		debugString("\nnextCluster for dir contents : ");debugLong(currentCluster);
	}

	return i;
}


/*
	int  UCS2Tokenizer(const UCS2* source,const UCS2* token,UCS2* target,int startIndex){
 */

/*
	Function Notes:

	1-The path parameter type was changed from UTF8* to UCS2* to enable predictable path tokenization.

*/



int FAT32_read(const MBR* pMBR,int fd,char* callerBuffer,int bufferLength){


        const PartitionEntry* partition0= reinterpret_cast<const PartitionEntry*>(pMBR->partition0);
	long volumeLBA= getUnsignedInt(partition0->startSectorLBA); // :)
	long startClusterNumberR=-1;
   	

	int pathUTF8Width= UCS2declen(openFiles[fd].path);

	{
		UCS2 pathUCS2[1024];

		UCS2decode(pathUCS2,pathUTF8Width,openFiles[fd].path);

		debugStringN(reinterpret_cast<const char*>(pathUCS2),32);

		unsigned int fileSize;

		startClusterNumberR = getFileFirstCluster(DRIVE0,pMBR,partition0,pathUCS2,fileSize);
	}//preseving stack space.

	debugString("\nstartClusterNumberR  within  FAT32_read : ");debugLong(startClusterNumberR);

	FAT32BootSector bSector;
	loadFAT32BootSector(DRIVE0,&bSector,volumeLBA);

	int clusterSize= getClusterSize(&bSector);
	int positionsSkipped=clusterSize;

	int filePointer= openFiles[fd].pointer;
	
	while(positionsSkipped<filePointer){
		

		startClusterNumberR= 
			getNextCluster(pMBR,&bSector,DEVICE0,volumeLBA,startClusterNumberR);

		if(isFAT32EOC(startClusterNumberR)){
			debugString("\na seeking problem");
			return -1;//something wronge happened
		}
		
		positionsSkipped+=clusterSize;
	}


	
	int bytesRead= (long)  
		readFile(pMBR,&bSector,DRIVE0,volumeLBA ,driverBuffer,sizeof(driverBuffer),startClusterNumberR);

	if(bytesRead<=0){
		return -1;
 	}	

	int relativeBufferPosition= filePointer%sizeof(driverBuffer);
	debugString("\nrelativeBufferPosition:");debugLong(relativeBufferPosition);


	for(unsigned int i=0;i<sizeof(driverBuffer)-relativeBufferPosition;i++){
		driverBuffer[i]= driverBuffer[i+relativeBufferPosition];
	}


	for(int i=0;i<bufferLength;i++){
		callerBuffer[i]= driverBuffer[i];
	}


	return bytesRead > bufferLength? bufferLength: bytesRead;
}





long  getFileFirstCluster(const int physicalDriveNumber,const struct MBR*  mbr,const struct  PartitionEntry*  pe,const UCS2* path,unsigned int& fileSize){

	if(path[0]!=PATH_SEPARATOR[0]){ 	
		
		debugString("Path Must Start with /");
		return FAT32_EOC_MARKER;
	}
		
	//lookup the root dir:
	//plz change the following lines, they may lead to a huge problem

	long volumeLBA= getUnsignedInt(pe->startSectorLBA); // :)
	
	struct FAT32BootSector bootSector;

	switch(pe->type){

	case FAT32_CHS:
	{
		while(!readDataLBA28(physicalDriveNumber,&bootSector,COMMAND_READ_SECTORS,0/*NO_FEATURES*/,volumeLBA));
	}
	break;


	case FAT32_LBA:
	{	
		 volumeLBA= getUnsignedInt(pe->startSectorLBA);
        	 while(!readDataLBA28(physicalDriveNumber,&bootSector,COMMAND_READ_SECTORS,0/*NO_FEATURES*/,volumeLBA));
	}
	break;


	default:
	
		debugString("Unsupported Partition Type: ");debugLong(pe->type);
		return -1;
	}

        debugString("Done Reading Boot Sector...\n\n");
        debugStringN(bootSector.oemName,8);

	debugString("\nvolumeLBA: ");debugLong(volumeLBA);
	//read this volume boot sector
	
	const long rootClusterNumber= getUnsignedInt(bootSector.rootClusterNumber);

	debugString("\nRoot cluster number: ");debugLong(rootClusterNumber);
	//const int bytePerSector= getUnsignedShort(bootSector.bytePerSector);
	//const int sectorPerCluster= getUnsignedByte(bootSector.sectorPerCluster);
	
	if(UCS2cmp(path,PATH_SEPARATOR)==0){
		debugString("\nRoot Dir Match Found\n");
		return rootClusterNumber;
	}

	//debugString("\nFAT32BootSector size ");debugLong(sizeof(struct FAT32BootSector));
	//long reservedSectorCount= getUnsignedShort(bootSector.reservedSectorCount);
        //debugString("\nreservedSectorCount: ");debugLong(reservedSectorCount);
	//long fatCount= getUnsignedByte( bootSector.fatCount );
	//debugString("\nfatCount: ");debugLong(fatCount);

        //long fatSize= getUnsignedInt(bootSector.singleFATSize32);
        //debugString("\nfatSize: ");debugLong(fatSize);
	//long firstDataSectorNumber= reservedSectorCount + (fatCount*fatSize) + 0;//

        //debugString("\n first data sector (NOLBA)");debugLong(computeVolumeDataFirstSectorNumber(&bootSector));
        //debugString("\n first data sector (+LBA)");debugLong(computeVolumeDataFirstSectorNumber(&bootSector)+volumeLBA);
	
	//long dataCHS= LBA2CHS(&bootSector,computeVolumeDataFirstSectorNumber(&bootSector)+volumeLBA);
	//dataCHS-=12;//
	//readDataCHS(0,driverBuffer,COMMAND_READ_SECTORS,0,1,CYLINDER(dataCHS),HEAD(dataCHS),SECTOR(dataCHS));
	
	//debugStringN(driverBuffer,512);

	//int tokenIndex=0;
	int startIndex=0;
	long currentCluster=rootClusterNumber;
	debugString("\nRoot cluster number : ");debugLong(rootClusterNumber);

        const long clusterSize= getClusterSize(&bootSector); 

	UCS2 currentFileName[MAX_FILE_NAME_SIZE];

PATH_LOOP: //This laabel signifies the advancement in a cluster chain node

	startIndex= UCS2Tokenizer(path,PATH_SEPARATOR,currentFileName,startIndex);
	while(startIndex!=-1){

		debugString("\nYet Another startIndex: ");debugLong(startIndex);

		//debugString("\nProcessing a token");
	    	struct FAT32DirectoryEntry* currentDirEntries= (struct FAT32DirectoryEntry*) driverBuffer;
		debugString("\nCurrent cluster number: ");debugLong(currentCluster);	

		//readFile must be revised.
		readFile(mbr,&bootSector,physicalDriveNumber,volumeLBA ,driverBuffer,clusterSize*2,currentCluster/*,0*/);
		debugString("\nDir Entries :");debugStringN(driverBuffer,512);
		
		int entryIndex;
		int maxEntryCount= clusterSize/sizeof(struct FAT32DirectoryEntry);
		for(entryIndex=0; entryIndex<maxEntryCount ;entryIndex++){
			debugString("\nProcessing an entry");
			debugStringN(driverBuffer+sizeof(FAT32DirectoryEntry)*entryIndex,sizeof(FAT32DirectoryEntry));

			if( !isFAT32LongEntry(currentDirEntries+entryIndex) && 
				getUnsignedByte(currentDirEntries[entryIndex].directoryName)==DIRECTORY_ENTRY_FREE){
				//is emtpy skip
				
				debugString("\nShort-Entry is Free");
				continue;
			}

			if(!isFAT32LongEntry(currentDirEntries+entryIndex) && 
				getUnsignedByte(currentDirEntries[entryIndex].directoryName)==
				DIRECTORY_ENTRY_FREE_PLUS_REST){//this + the rest is empty, target doesn't exist
		
				debugStringN(currentDirEntries[entryIndex].directoryName,16);
				debugString("\nShort-Entry free with rest--file not found");
				return FAT32_EOC_MARKER;
			}
			debugString("\nTrying to match the name with entry");
		
			if(doesNameMatchEntry(currentDirEntries+entryIndex,currentFileName)){

				if(isFAT32LongEntry(currentDirEntries+entryIndex)){
					debugString("\nProcessing long entry");
					struct FAT32LongDirectoryEntry* longEntryPtr=(struct FAT32LongDirectoryEntry*) (currentDirEntries+entryIndex);
					const int longEntryCount= ((getUnsignedByte(longEntryPtr->entryOrder) & /* 0x3F */ LONG_ENTRY_COUNT_MASK));
					struct FAT32DirectoryEntry* shortEntry= currentDirEntries+(entryIndex+longEntryCount);

					fileSize= getUnsignedInt(shortEntry->fileSize);
				
					currentCluster= ((getUnsignedShort(shortEntry->firstClusterHigh)<<16)|(getUnsignedShort(shortEntry->firstClusterLow)));
					entryIndex += (longEntryCount+1);//by pass already processed entries
					
					goto PATH_LOOP;					 
				}else{
					
					//the following code is buggy, and is ready for english chars only .
					
					struct FAT32DirectoryEntry* shortEntry= currentDirEntries+entryIndex;
					currentCluster= ((getUnsignedShort(shortEntry->firstClusterHigh)<<16)|(getUnsignedShort(shortEntry->firstClusterLow)));
				
					fileSize= getUnsignedInt(shortEntry->fileSize); 
					goto PATH_LOOP;
				}	

			}//doesNameMatchEntry?
		}//for loop

		
		debugString("\nFile not found ");
		return FAT32_EOC_MARKER;

	}//path loop

        return currentCluster;//welcome to file descriptors
	
}

void extractUCS2Values(const struct FAT32LongDirectoryEntry* longEntry,UCS2* targetBuffer)
{
 
    UCS2Copy(targetBuffer,(UCS2*)longEntry->name1,5);
    UCS2Copy(targetBuffer+5,(UCS2*)longEntry->name2,6);
    UCS2Copy(targetBuffer+11,(UCS2*)longEntry->name3,2);

    debugString("\nextractUCS2Values :");
    debugStringN((const char* )targetBuffer,26);
	    
    return ;
}

/*

LBA = ( ( CYL * HPC + HEAD ) * SPT ) + SECT - 1
Where:
 LBA: linear base address of the block
 CYL: value of the cylinder CHS coordinate
 HPC: number of heads per cylinder for the disk
HEAD: value of the head CHS coordinate
 SPT: number of sectors per track for the disk
SECT: value of the sector CHS coordinate

*/
/*
long CHS2LBA(const struct FAT32BootSector* bSector,long cylinder,long head,long sector){

        debugString("\nCHS2LBA for: ");

	debugString(" C ");debugLong(cylinder);
	debugString(" H ");debugLong(head);
	debugString(" S ");debugLong(sector);

	long headCount= getUnsignedShort(bSector->headCount);
	
	long sectorPerTrack= getUnsignedShort(bSector->sectorPerTrack);

	long result= ((cylinder*headCount+head)*sectorPerTrack)+sector-1;

	return result;
}


CHS LBA2CHS(const struct FAT32BootSector* bSector,long lba){

	debugString("\nLBA2CHS for: ");debugLong(lba);
	
	const long headPerCylinder=  getUnsignedShort(bSector->headCount);
	const long sectorPerTrack= getUnsignedShort(bSector->sectorPerTrack);

	long cylinder= lba / (headPerCylinder*sectorPerTrack);
	long temp    = lba % (headPerCylinder*sectorPerTrack);
	long head    = temp / sectorPerTrack;
	long sector  = (temp % sectorPerTrack)+1;
	
	CHS result= head | (sector<<8) | (cylinder<<16);	

	return result;
}
*/
long computeVolumeDataFirstSectorNumber(const struct FAT32BootSector* bSector){

	// FirstDataSector = ReservedSectorCount + (FATCount * FATSize) + RootDirSector(Always Zero for FAT32);
	long reservedSectorCount= getUnsignedShort(bSector->reservedSectorCount);
	long fatCount= getUnsignedByte( bSector->fatCount );
	long fatSize= getUnsignedInt(bSector->singleFATSize32); 
	long firstDataSectorNumber= reservedSectorCount + (fatCount*fatSize) + 0;//

	return firstDataSectorNumber;
}

long computeClusterFirstSectorNumber(const struct FAT32BootSector*  bSector,long clusterNumber)
{
	debugString("\nSector per cluster: ");debugLong(getUnsignedByte(bSector->sectorPerCluster));
	long result= computeVolumeDataFirstSectorNumber(bSector) + (clusterNumber-2)*(getUnsignedByte(bSector->sectorPerCluster));
	return result;
}


long computeVolumeDataSectorCount(const struct FAT32BootSector* bSector)
{
	//DataSectorCount= TotalSectorCount - (reservedSectorCount+(FATsCount*singleFatSize)+RootDirSectors);
	long totalSectorCount= getUnsignedInt(bSector->totalSectorCount32);
	long reservedSectorCount= getUnsignedShort(bSector->reservedSectorCount);
	long fatCount= getUnsignedByte(bSector->fatCount);
	long singleFatSize= getUnsignedInt(bSector->singleFATSize32);
	
	long dataSectorCount= totalSectorCount - (reservedSectorCount+(fatCount*singleFatSize)+0);

	return dataSectorCount;

}


long computeVolumeDataClusterCount(const struct FAT32BootSector* bSector)
{
	long sectorPerCluster= getUnsignedByte(bSector->sectorPerCluster);
	return computeVolumeDataSectorCount(bSector)/sectorPerCluster;
}

int isFAT32EOC(long fat32Entry)
{

	return fat32Entry >= FAT32_EOC_MARKER;
}

long computeClusterFATEntryOffset(const struct MBR* mbr,const struct FAT32BootSector* bSector,long clusterNumber)
{
        long fatOffset= clusterNumber*4;
        long bytePerSector= getUnsignedShort(bSector->bytePerSector);

        long thisFatEntryOffset= fatOffset % bytePerSector;

	
	return thisFatEntryOffset;
}

long computeClusterFATEntrySectorNumber(const struct MBR* mbr,const struct FAT32BootSector* bSector,long clusterNumber)
{
	long fatOffset= clusterNumber*4;
	long reservedSectorCount= getUnsignedShort(bSector->reservedSectorCount);
	long bytePerSector= getUnsignedShort(bSector->bytePerSector);

	long thisFatSectorNumber= reservedSectorCount + (fatOffset/bytePerSector);

        return thisFatSectorNumber;
}

   

/*

long computeFATTableSectorCount(struct MBR mbr, struct FAT32BootSector bootSector){

	const long rootEntryCount= 0;
	const long bytePerSector= getUnsignedShort(bootSector.bytePerSector);
	const long rootDirSectorCount= ((rootEntryCount*32)+(bytePerSector-1))/bytePerSector;
	const long reservedSectorCount= getUnsignedInt(bootSector.reservedSectorCount);
	const long sectorPerCluster= getUnsignedByte(bootSector.sectorPerCluster);
	const long fatCount= getUnsignedByte(bootSector.fatCount);
	
	const long volumeSectorCount=getUnsigned(bootSector.);
	const long temp1= volumeSectorCount - (reservedSectorCount+rootDirSectorCount);
	const long temp2= ((256*sectorPerCluster)+ fatCount)/2;
	
	const long fatSize= (temp1+ (temp2-1)) /temp2;

	return fatSize;
}
*/
unsigned char computeShortEntryFileNameChecksum(unsigned char * name){

	short nameLength;
	unsigned char sum=0;


	for(nameLength=11;nameLength!=0;nameLength--){
		sum = ((sum & 1)? 0x80 : 0x0 ) + (sum>>1) + *name++;
	}

	return (sum);
}	


