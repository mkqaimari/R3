#ifndef	DOSDISK_H
#define DOSDISK_H

struct MBR {

        char codeArea[446];

        char partition0[16];
        char partition1[16];
        char partition2[16];
        char partition3[16];

        char mbrSignature[2];


}__attribute__((packed));//Master Boot Record


struct PartitionEntry {

        char status;
        char startHeadSectorCylinder[3];

        char type;

        char endHeadSectorCylinder[3];

        char startSectorLBA[4];
        char sectorCount[4];

}__attribute__((packed));


void readMBR(int deviceNumber,struct MBR*);

int getMBRPartitionEntryHead(struct PartitionEntry* entry);
int getMBRPartitionEntryCylinder(struct PartitionEntry* entry);
int getMBRPartitionEntrySector(struct PartitionEntry* entry);


#endif
