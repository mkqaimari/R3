#ifndef	IDE_H
#define	IDE_H


#include<x86/Types.h>


//device numbers supported
#define DRIVE0  0x80


#define MAX_COMMAND_EXECUTION_TIME_SECONDS 20L


//device/head register bits

#define DEVICE0		0x00U
#define DEVICE1		0x10U
#define ENABLE_LBA	0x40U

#define CHANNEL0_BASE_ADDRESS0           0x1f0U
#define CHANNEL0_BASE_ADDRESS1           0x3f0U


#define  DATA_REGISTER                               CHANNEL0_BASE_ADDRESS0+0
#define  ERROR_REGISTER                              CHANNEL0_BASE_ADDRESS0+1
#define  FEATURE_REGISTER                            CHANNEL0_BASE_ADDRESS0+1
#define  SECTOR_COUNT_REGISTER                       CHANNEL0_BASE_ADDRESS0+2
#define  SECTOR_NUMBER_REGISTER                      CHANNEL0_BASE_ADDRESS0+3
#define  LBA_LOW				     CHANNEL0_BASE_ADDRESS0+3
#define  LBA_MIDDLE				     CHANNEL0_BASE_ADDRESS0+4
#define  LBA_HIGH				     CHANNEL0_BASE_ADDRESS0+5
#define  CYLINDER_LOW_REGISTER                       CHANNEL0_BASE_ADDRESS0+4

#define  CYLINDER_HIGH_REGISTER                      CHANNEL0_BASE_ADDRESS0+5
#define  DEVICE_HEAD_REGISTER                        CHANNEL0_BASE_ADDRESS0+6
#define  STATUS_REGISTER                             CHANNEL0_BASE_ADDRESS0+7
#define  COMMAND_REGISTER                            CHANNEL0_BASE_ADDRESS0+7
#define  ALTERNATE_STATUS_REGISTER                   CHANNEL0_BASE_ADDRESS1+6
#define  CONTROL_REGISTER                            CHANNEL0_BASE_ADDRESS1+6



//status register and alternate status register share the same bits layout
#define STATUS_BUSY             0x80U
#define STATUS_DATA_READY       0x40U
#define STATUS_DEVICE_FAULT	0x20U
#define STATUS_WRITE_FAULT      0x20U
#define STATUS_SEEK_COMPLETE	0x10U
#define STATUS_SERVICE		0x10U
#define STATUS_DATA_REQUEST     0x08U
#define STATUS_CORRECTED	0x04U
#define STATUS_INDEX		0x02U
#define STATUS_ERROR            0x01U
#define STATUS_ATAPI		0x01U

//device control register bits
#define CONTROL_SOFT_RESET              4
#define CONTROL_ENABLE_INTERRUPT        2


//device types 
#define DEVICE_NOT_PRESENT     0x00U
#define DEVICE_PATA            0x01U
#define DEVICE_SATA            0x02U
#define DEVICE_PATAPI          0x03U
#define DEVICE_SATAPI          0x04U
#define DEVICE_ATA_UNKNOWN     0x09U


//the set of ATAPI commands

#define COMMAND_CFA_ERASE_SECTORS            0xC0
#define COMMAND_CFA_REQUEST_EXT_EEROR_CODE     0x03
#define COMMAND_CFA_TRANSLATE_SECTOR         0x87
#define COMMAND_CFA_WRITE_MULTIPLE_WO_ERASE  0xCD
#define COMMAND_CFA_WRITE_SECTORS_WO_ERASE   0x38
#define COMMAND_CHECK_POWER_MODE1            0xE5
#define COMMAND_CHECK_POWER_MODE2            0x98
#define COMMAND_DEVICE_RESET                 0x08
#define COMMAND_EXECUTE_DEVICE_DIAGNOSTIC    0x90
#define COMMAND_FLUSH_CACHE                  0xE7
#define COMMAND_FLUSH_CACHE_EXT              0xEA
#define COMMAND_FORMAT_TRACK                 0x50
#define COMMAND_IDENTIFY_DEVICE              0xEC
#define COMMAND_IDENTIFY_DEVICE_PACKET       0xA1
#define COMMAND_IDENTIFY_PACKET_DEVICE       0xA1
#define COMMAND_IDLE1                        0xE3
#define COMMAND_IDLE2                        0x97
#define COMMAND_IDLE_IMMEDIATE1              0xE1
#define COMMAND_IDLE_IMMEDIATE2              0x95
#define COMMAND_INITIALIZE_DRIVE_PARAMETERS  0x91
#define COMMAND_INITIALIZE_DEVICE_PARAMETERS 0x91
#define COMMAND_NOP                          0x00
#define COMMAND_PACKET                       0xA0
#define COMMAND_READ_BUFFER                  0xE4
#define COMMAND_READ_DMA                     0xC8
#define COMMAND_READ_DMA_EXT                 0x25
#define COMMAND_READ_DMA_QUEUED              0xC7
#define COMMAND_READ_DMA_QUEUED_EXT          0x26
#define COMMAND_READ_MULTIPLE                0xC4
#define COMMAND_READ_MULTIPLE_EXT            0x29
#define COMMAND_READ_SECTORS                 0x20
#define COMMAND_READ_SECTORS_EXT             0x24
#define COMMAND_READ_VERIFY_SECTORS          0x40
#define COMMAND_READ_VERIFY_SECTORS_EXT      0x42
#define COMMAND_RECALIBRATE                  0x10
#define COMMAND_SEEK                         0x70
#define COMMAND_SET_FEATURES                 0xEF
#define COMMAND_SET_MULTIPLE_MODE            0xC6
#define COMMAND_SLEEP1                       0xE6
#define COMMAND_SLEEP2                       0x99
#define COMMAND_SMART                        0xB0
#define COMMAND_STANDBY1                     0xE2
#define COMMAND_STANDBY2                     0x96
#define COMMAND_STANDBY_IMMEDIATE1           0xE0
#define COMMAND_STANDBY_IMMEDIATE2           0x94
#define COMMAND_WRITE_BUFFER                 0xE8
#define COMMAND_WRITE_DMA                    0xCA
#define COMMAND_WRITE_DMA_EXT                0x35
#define COMMAND_WRITE_DMA_QUEUED             0xCC
#define COMMAND_WRITE_DMA_QUEUED_EXT         0x36
#define COMMAND_WRITE_MULTIPLE               0xC5
#define COMMAND_WRITE_MULTIPLE_EXT           0x39
#define COMMAND_WRITE_SECTORS                0x30
#define COMMAND_WRITE_SECTORS_EXT            0x34
#define COMMAND_WRITE_VERIFY                 0x3C


void resetDevices();
void detectDevices();
/*bool readDataCHS(
		int deviceNumber,
                void* targetBuffer,
                unsigned char command,
                unsigned char features,
                unsigned char sectorCount,
                unsigned char cylinder,
                unsigned char head,
                unsigned char sectorNumber);
*/
bool readDataLBA28(int deviceNumber,
                void* targetBuffer,
                unsigned char command,
                unsigned char features,
/*                unsigned char sectorCount,*/
                unsigned int lba28);

bool executeNonDataCommand(  unsigned char deviceNumber,
                                unsigned char command,
                                unsigned char features,
                                unsigned char sectorCount,
                                unsigned char cylinder,
                                unsigned char head,
                                unsigned char sectorNumber);


#endif
