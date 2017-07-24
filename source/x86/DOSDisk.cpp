#include <x86/DOSDisk.h>
#include <x86/IDE.h>

static int getPartitionCylinder(char entry[])
{

        int cylinderHigh= (((unsigned char)entry[1]) >>6 );
        //debugString("C HIGH ");writeLong(cylinderHigh);

        int cylinderLow= (unsigned char) entry[2];
        //debugString("C LOW ");writeLong(cylinderLow);
        int result= cylinderLow | (cylinderHigh<<8);
        return result;
}

static int getPartitionSector(char entry[])
{
        int result= (unsigned char) entry[1];
        result&=0x3F;
        return result;
}

static int getPartitionHead(char entry[])
{
	return  (unsigned char) entry[3];
}


int getMBRPartitionEntryHead(struct PartitionEntry* entry)
{
        return getPartitionHead((char*)entry);
}

int getMBRPartitionEntryCylinder(struct PartitionEntry* entry)
{

        return getPartitionCylinder((char*) entry);
}

int getMBRPartitionEntrySector(struct PartitionEntry* entry)
{
        return getPartitionSector((char*)entry);
}


void  readMBR(int deviceNumber,struct MBR* mbr)
{
        while(!readDataLBA28(deviceNumber,mbr,COMMAND_READ_SECTORS,0,0));
        return ;
} 
