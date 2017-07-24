#ifndef FAT32_H
#define FAT32_H

#include <x86/DOSDisk.h>
#include <x86/UTF8.h>


//strcuts section:


struct FAT32BootSector {

	char x86Jump[3];
	char oemName[8];
	
	char bytePerSector[2];
	#define BYTES_PER_SECTOR_512 512
	#define BYTES_PER_SECTOR_1024 1024
	#define BYTES_PER_SECTOR_2048 2048
	#define BYTES_PER_SECTOR_4096 4096
	
	char sectorPerCluster[1];
	#define SECTOR_PER_CLUSTER_1 1
        #define SECTOR_PER_CLUSTER_2 2
        #define SECTOR_PER_CLUSTER_4 4
        #define SECTOR_PER_CLUSTER_8 8
        #define SECTOR_PER_CLUSTER_16 16
        #define SECTOR_PER_CLUSTER_32 32
        #define SECTOR_PER_CLUSTER_64 64
        #define SECTOR_PER_CLUSTER_128 128
	
	#define MAX_CLUSTER_SIZE 32*1024 		//you can't have cluster greater than 32K in size no matter what 

	char reservedSectorCount[2];
	char fatCount[1];				//usually set to 2

	char rootDirEntryCount[2];	
	#define FAT32_ROOTDIR_ENTRY_COUNT	0	//only 0 is valid for FAT32

	char totalSectorCount16[2];			//
	#define FAT32_TOTAL_SECTOR_COUNT16 	0

	char media[1];
	#define MEDIA_NON_REMOVABLE		0xF8	 
	#define	MEDIA_REMOVABLE			0xF0

	char singleFATSize16[2];
	#define FAT32_SINGLE_FAT_SIZE		0

	char sectorPerTrack[2];
	char headCount[2];
	char hiddenSectorCount[4];	

	char totalSectorCount32[4];
	char singleFATSize32[4];
	char extFlags[2];			//Bits 0-3: Zero-based index of the active FAT, only valid if mirroring is disabled.
						//Bits 4-6: Reserved
						//Bit    7: 0 Means mirroring is enabled on all FATs.
						//	    1 Means mirroring is disabled, use the selected FAT in bits 0-3.
						//Bits 8-15: Reserved
	char filesystemVersion[2];
	#define FAT32_MAJOR_REVNUMBER 0
	#define FAT32_MINOR_REVNUMBER 0
	
	char rootClusterNumber[4];
	char filesystemInfoSectorNumber[2];
	char backupBootSectorNumber[2];
	char reserved0[12];
	char driveNumber[1];
	char reserved1[1];
	
	char bootSignature[1];
	#define EXTENDED_BOOT_SIGNATURE	0x29
	
	char volumeId[4];
	char volumeLabel[11];
	char filesystemType[8];

	#define FAT32_FILESYSTEM_TYPE	"FAT32   "

	char padding[422];

}__attribute__((packed));

struct FileSystemInfo {

	char leadingSignature[4];
	#define FILESYSTEM_INFO_LEADING_SIG 		0x41615252
	
	char reserved1[480];

	char structureSignature[4];
	#define FILESYSTEM_INFO_STRUCT_SIG		0x61417272
	
	char freeCount[4];			
	#define FILESYSTEM_INFO_FREE_COUNT_UNKNOWN	0xFFFFFFFF

	char nextFreeCluster[4];
	#define FILESYSTEM_INFO_NO_NEXT_CLUSTER		0xFFFFFFFF

	char reserved2[12];

	char trailSignature[4];
	#define FILESYSTEM_INFO_TRAIL_SIG		0xAA550000


}__attribute__((packed));

static const char invalidFirstCharName[]={' '};
static const char invalidAllCharName[]={0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb,0xc,0xd,0xe,0xf,
					0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,
					0x22,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x5b,0x5c,0x5d,0x7c};

struct FAT32DirectoryEntry {

	char directoryName[11];
	#define DIRECTORY_ENTRY_FREE		0xE5
	#define DIRECTORY_ENTRY_FREE_PLUS_REST	0x0
	#define DIRECTORY_ENTRY_KANJI_DELEGATE  0x05

	

	char directoryAttribute[1];
	
	#define ATTRIBUTE_READ_ONLY	0x01
	#define ATTRIBUTE_HIDDEN	0X02
	#define	ATTRIBUTE_SYSTEM	0X04
	#define	ATTRIBUTE_VOLUME_ID	0x08
	#define	ATTRIBUTE_DIRECTORY	0X10
	#define	ATTRIBUTE_ARCHIVE	0x20
	#define	ATTRIBUTE_LONG_NAME	(ATTRIBUTE_READ_ONLY | ATTRIBUTE_HIDDEN | ATTRIBUTE_SYSTEM | ATTRIBUTE_VOLUME_ID)
	#define ATTRIBUTE_LONG_NAME_MASK	(ATTRIBUTE_READ_ONLY | ATTRIBUTE_HIDDEN | ATTRIBUTE_SYSTEM | ATTRIBUTE_VOLUME_ID | ATTRIBUTE_DIRECTORY | ATTRIBUTE_ARCHIVE)
	char ntReserved[1];
	#define NT_RESERVED_FIELD_VALUE	0x0

	char creationTimeTenth[1];	//Valid values are 0-199 inclusive
	char creationTime[2];		
	
	char creationDate[2];
	char lastAccessedDate[2];
	
	char firstClusterHigh[2];
	char writeTime[2];
	char writeDate[2];
	char firstClusterLow[2];
	
	char fileSize[4];
	
	
	
	
}__attribute__((packed));

struct FAT32LongDirectoryEntry {

	char entryOrder[1];	//the sequence of the entry in the list of long name's entries
	#define FAT32_LAST_LONG_DIRECTORY_ENTRY_FLAG	0x40
	char name1[10];
	char directoryAttribute[1];
	char type[1];		//Always 0
	#define FAT32_LONG_DIRECTORY_ENTRY_TYPE	0
	char checksum[1];
	char name2[12];
	char firstClusterNumberLow[2];	//always set to zero
	#define FAT32_LONG_DIRECTORY_ENTRY_FIRST_CLUSTER_LOW	0
	char name3[4];
}__attribute__((packed));

//constants and macros section:
//#define PATH_SEPARATOR                  	'/'
#define DRIVER_BUFFER_SIZE			16*1024    //statically allocated buffer used by the driver
#define MAX_DIR_CONTENT_SIZE                	DRIVER_BUFFER_SIZE
#define MAX_FILE_NAME_SIZE                  	255
//#define MAX_PATH_DEPTH                      	4
//#define MAX_PATH_SIZE                       	(MAX_PATH_DEPTH*MAX_FILE_NAME_SIZE)

#define CYLINDER(chs) 			((chs>>16) & 1024 )
#define HEAD(chs) 			(chs & 0xFF)
#define SECTOR(chs) 			((chs >> 8) & 0xFF)

#define MBR_PARTITION_ENTRY_CYLINDER(entry)             ((((unsigned char)entry[1]) >>6) | ((unsigned char) entry[2]))
#define MBR_PARTITION_ENTRY_HEAD(entry)                 (entry[0])
#define MBR_PARTITION_ENTRY_SECTOR(entry)               (((unsigned char) entry[1]) &0x3F)

#define FAT32_LONG_ENTRY_NAME_CAPACITY                  (13)


#define PARTITION_BOOTABLE_STATUS 0x80
#define PARTITION_NONBOOTABLE_STATUS 0x00

#define FAT32_LBA 0x0C
#define FAT32_CHS 0x0B

#define FAT32_REGION_RESERVED
#define FAT32_REGION_FAT
#define FAT32_REGION_ROOTDIR
#define FAT32_REGION_DATA

#define FAT32_DATA_FIRST_CLUSTER_NUMBER	2

#define FAT32_EOC_MARKER 		0x0FFFFFF8
#define FAT32_BAD_CLUSTER_MARKER	0x0FFFFFF7	
#define FAT32_ZERO_LENGTH_FILE_CLUSTER	0

#define CHS_CYLINDER_HIGH(c)    	(((c>>8)&0x3))
#define CHS_CHYLINDER_LOW(c)    	((c&0xFF))
#define CHS_DRIVE_PLUS_HEAD(d,h)        (0xA0 | (d<<4) | (h & 0x0F) )

//functions section:


long  getFileFirstCluster(const int physicalDriveNumber,const struct MBR*  mbr,const struct  PartitionEntry* pe,const UCS2* path,unsigned int & fileSize);

int FAT32_read(const MBR* pMBR,int fd,char* buffer,int bufferLength);

#endif
