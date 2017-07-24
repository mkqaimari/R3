#include <x86/Logger.h>
#include <x86/Types.h>
#include <x86/IDE.h>
#include <x86/I386.h>
#include <x86/Console.h>
#include <x86/Timer.h>


static char devicePresent[2]={0};//no devices yet

//forward declaration:

static void detectDevice(unsigned char  deviceNumber);
static void getDeviceDetails(int deviceNumber);
static bool selectDevice(int deviceNumber,bool lbaMode);
//static bool executeNonDataCommand(int command); 


static bool pollNotBusy()
{


   // Poll for not BUSY -or- wait for time out.

   int count;
   for(count=0;count<MAX_COMMAND_EXECUTION_TIME_SECONDS;count++)
   {
      unsigned char status = readPortByte(ALTERNATE_STATUS_REGISTER );       // poll for not busy
      
      if ( ( status & STATUS_BUSY ) == 0 )
         return true;

	kernelSleep(1000);//pause for 1 second
      
   }
   
   return false;
}

void detectDevices()
{

	writePortByte(CONTROL_REGISTER,0);//no interrupts , I love this man
	
	//now check device-0:
	//select device-0

	detectDevice(DEVICE0);
        detectDevice(DEVICE1);

	resetDevices();
	
	if(devicePresent[DEVICE0]!=DEVICE_NOT_PRESENT)
		getDeviceDetails(DEVICE0);
        if(devicePresent[DEVICE1]!=DEVICE_NOT_PRESENT)
		getDeviceDetails(DEVICE1);

	return ;
}

static void getDeviceDetails(int deviceNumber){

	//now, more details about attached devices

        writePortByte(CONTROL_REGISTER,deviceNumber);//select device-0

        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay

        unsigned char sectorCount= readPortByte(SECTOR_COUNT_REGISTER);
        unsigned char sectorNumber= readPortByte(SECTOR_NUMBER_REGISTER);

        if(sectorCount==1 && sectorNumber==1){

                unsigned char statusByte= readPortByte(STATUS_REGISTER);
                unsigned char cylinderHigh= readPortByte(CYLINDER_HIGH_REGISTER);
                unsigned char cylinderLow= readPortByte(CYLINDER_LOW_REGISTER);

                devicePresent[deviceNumber]= DEVICE_ATA_UNKNOWN; 

                if ((cylinderLow== 0x14U ) && ( cylinderHigh == 0xebU ))// DEVICE_PATAPI
                {        
			devicePresent[deviceNumber]= DEVICE_PATAPI; 
			debugString("\nType of device-");debugLong(deviceNumber/16);debugString(" DEVICE_PATAPI");

		}
                if ((cylinderLow == 0x69U ) && ( cylinderHigh == 0x96U ))       // DEVICE_SATAPI
                {
		        devicePresent[deviceNumber]=DEVICE_SATAPI;
                        debugString("\nType of device-");debugLong(deviceNumber/16);debugString(" DEVICE_SATAPI");
		}
                if(statusByte!=0 && ( cylinderLow == 0x00 ) && ( cylinderHigh == 0x00 ))
                {
		       devicePresent[deviceNumber]=DEVICE_PATA;
                       debugString("\nType of device-");debugLong(deviceNumber/16);debugString("DEVICE_PATA");
		}
                if(statusByte!=0 && ( cylinderLow == 0x3cU ) && ( cylinderHigh == 0xc3U ))
                {
		        devicePresent[deviceNumber]=DEVICE_SATA;
                        debugString("\nType of device-");debugLong(deviceNumber/16);debugString("DEVICE_SATA");
		}
        }

	return ;
}

static void detectDevice(unsigned char  deviceNumber)
{

	writePortByte(DEVICE_HEAD_REGISTER,deviceNumber);

        //400us delay, usual stufff
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay


        writePortByte( SECTOR_COUNT_REGISTER, 0x55 );
        writePortByte( SECTOR_NUMBER_REGISTER, 0xaa );
        writePortByte( SECTOR_COUNT_REGISTER, 0xaa );
        writePortByte( SECTOR_NUMBER_REGISTER, 0x55 );
        writePortByte( SECTOR_COUNT_REGISTER, 0x55 );
        writePortByte( SECTOR_NUMBER_REGISTER, 0xaa );

        unsigned char sectorCount = readPortByte( SECTOR_COUNT_REGISTER );
        unsigned char sectorNumber= readPortByte( SECTOR_NUMBER_REGISTER );

        if ( ( sectorCount == 0x55u ) && ( sectorNumber == 0xAAu ) ){
		debugString("\nATA device-");debugLong(deviceNumber/16);debugString(" detected");
		devicePresent[deviceNumber]=DEVICE_ATA_UNKNOWN;
	}else{
                debugString("\nATA device-");debugLong(deviceNumber/16);debugString(" not present");
		devicePresent[deviceNumber]= DEVICE_NOT_PRESENT;
	}

	return ;
} 

void resetDevices()
{
	
	writePortByte(CONTROL_REGISTER,CONTROL_SOFT_RESET);

	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay

	//busy bit must have been set now by device-0

        writePortByte(CONTROL_REGISTER,0);

        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
	
	int retryCount;


	//now checking device-0
	if(devicePresent[DEVICE1]!=DEVICE_NOT_PRESENT){

	        //ATAPI Devices require 110ms extra delay
        	
		if(devicePresent[DEVICE1]==DEVICE_PATAPI || devicePresent[DEVICE1]==DEVICE_SATAPI)
			kernelSleep(110);

        	for(retryCount=0;retryCount<MAX_COMMAND_EXECUTION_TIME_SECONDS;retryCount++){
		
                        debugString("\nTrying to see if ATA device-1 did reset or not"); 
		        writePortByte(DEVICE_HEAD_REGISTER,DEVICE1);
			
		        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
		        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
		        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
		        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay


		        unsigned char sectorCount = readPortByte( SECTOR_COUNT_REGISTER );
		        unsigned char sectorNumber= readPortByte( SECTOR_NUMBER_REGISTER );

		        if ( ( sectorCount ==0x01 ) && ( sectorNumber == 0x01 ) ){
		        	break;
			}

	                kernelSleep(1000);//20 retries, 20 seconds
        	}

		unsigned char device1Status= readPortByte(STATUS_REGISTER);
	        if((device1Status & STATUS_BUSY )== 0){

        	        debugString("\nATA device-1 reset is done");
	
	        }

	}//if device present


        if(devicePresent[DEVICE0]!=DEVICE_NOT_PRESENT){

                //ATAPI Devices require 110ms extra delay

                if(devicePresent[DEVICE0]==DEVICE_PATAPI || devicePresent[DEVICE0]==DEVICE_SATAPI)
                        kernelSleep(110);


                for(retryCount=0;retryCount<MAX_COMMAND_EXECUTION_TIME_SECONDS;retryCount++){
                        debugString("\nTrying to see if ATA device-0 did reset or not");
                        unsigned char statusByte= readPortByte(STATUS_REGISTER);

                        if((statusByte & STATUS_BUSY) == 0){
                                debugString("\nATA device-0 reset is done");
                                break;
                        }
                        kernelSleep(1000);//20 retries, 20 seconds
                }


        }//if device present



	return ;
}


static bool selectDevice(int deviceNumber,bool lbaMode){

	
	int i;
	for(i=0;i<MAX_COMMAND_EXECUTION_TIME_SECONDS;i++){
	

		unsigned char status= readPortByte(STATUS_REGISTER);
		if((status & STATUS_BUSY) ==0   && (status & STATUS_DATA_REQUEST)==0)
			break;

		else
			kernelSleep(1000);//sleep for a whole second

	}

	if(i==MAX_COMMAND_EXECUTION_TIME_SECONDS)
		return false;

	if(!lbaMode)
                writePortByte( DEVICE_HEAD_REGISTER, ((0xA0U) | (deviceNumber<<4)));
        else
                writePortByte( DEVICE_HEAD_REGISTER, ((0xE0U) | (deviceNumber<<4)));


	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay


	for(i=0;i<MAX_COMMAND_EXECUTION_TIME_SECONDS;i++){

		unsigned char status= readPortByte(STATUS_REGISTER);

                if((status & STATUS_BUSY) ==0   && (status & STATUS_DATA_REQUEST)==0)
			break;

		else
			kernelSleep(1000);
	}

	if(i==MAX_COMMAND_EXECUTION_TIME_SECONDS)
		return false;

	return true;

}//selectDevice


/*
	Before calling this function, plz call selectDevice().

*/
bool executeNonDataCommand(	unsigned char deviceNumber,
				unsigned char command,
				unsigned char features,
				unsigned char sectorCount,
				unsigned char cylinder,
				unsigned char head,
				unsigned char sectorNumber)
{

	if(
		!selectDevice( ((0xAU|deviceNumber)<<4) | (head&0x0FU) ,false)
	)
	{
		return false;
	}


        writePortByte( FEATURE_REGISTER, features );
      	writePortByte( SECTOR_COUNT_REGISTER, sectorCount);
      	writePortByte( SECTOR_NUMBER_REGISTER, sectorNumber  );
      	writePortByte( CYLINDER_LOW_REGISTER, (unsigned char)(cylinder & 0xFF) );
      	writePortByte( CYLINDER_HIGH_REGISTER,(unsigned char)(cylinder >> 8) & 0xFF );
      	writePortByte( DEVICE_HEAD_REGISTER, ((0xAU | deviceNumber)<<4)|(head&0x0FU));

	//After settig up all register, you set the command register

	writePortByte(COMMAND_REGISTER,command);

	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay

	//After the 400us delay, everything is OK now.
	int i;

	for(i=0;i<MAX_COMMAND_EXECUTION_TIME_SECONDS;i++)
	{
	

		unsigned char status= readPortByte(ALTERNATE_STATUS_REGISTER);

		if ( status & ( STATUS_BUSY | STATUS_DEVICE_FAULT | STATUS_DATA_REQUEST | STATUS_ERROR ) )
      			return false;
		
		if(status & STATUS_BUSY)
			kernelSleep(1000);//sleep for a complete 1 second
		else
		{
			readPortByte(STATUS_REGISTER);//clear any pending interrupt
			return true;
		}

	}//

	readPortByte(STATUS_REGISTER);
	
	return false;//you will never get to this line unless you have a command-execution-timeout :)

}

bool executeDataInCommand(
		int deviceNumber,
		void* targetBuffer,
		unsigned char command,
		unsigned char features,
		unsigned char sectorCount,
		unsigned char cylinder,
		unsigned char head,
		unsigned char sectorNumber,
		bool lbaMode)
{
	
	  // these commands transfer only 1 sector
   	if (( command == COMMAND_IDENTIFY_DEVICE )
        ||  ( command == COMMAND_IDENTIFY_DEVICE_PACKET )
        ||  ( command == COMMAND_READ_BUFFER ))
		sectorCount=1;


	//currently driver support 1 sector transfer only
	sectorCount=1;
	debugString("\n reading one sector only");
		 
	if(!selectDevice(deviceNumber,lbaMode)){
                       return false;
	}

	debugString("\ndevice selection done");

	writePortByte( FEATURE_REGISTER, features );
        writePortByte( SECTOR_COUNT_REGISTER, sectorCount);
        writePortByte( SECTOR_NUMBER_REGISTER, sectorNumber  );
        writePortByte( CYLINDER_LOW_REGISTER, (unsigned char)(cylinder & 0xFF) );
        writePortByte( CYLINDER_HIGH_REGISTER,(unsigned char)(cylinder >> 8) & 0xFF );
        
	if(!lbaMode)
		writePortByte( DEVICE_HEAD_REGISTER, ((0xA0U) | (deviceNumber<<4))|(head&0x0FU));
	else
		writePortByte( DEVICE_HEAD_REGISTER, ((0xE0U) | (deviceNumber<<4))|(head&0x0FU));

	writePortByte(COMMAND_REGISTER,command);
		
	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
 	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
 	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
 	readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay	

//	while(true)   single sector read operations only
	{
	
		if(!pollNotBusy()){
			//break;
			readPortByte(STATUS_REGISTER);
			return false;//no operation field
		}
		debugString("\nDevice ready for transfer");
		//no matter what, the following line must be executed
		unsigned char status= readPortByte(STATUS_REGISTER);
		debugLong(status);	
     	  	
		debugString("\nError register: ");debugLong(readPortByte(ERROR_REGISTER));
		if((status&STATUS_DATA_REQUEST)==STATUS_DATA_REQUEST && (status&STATUS_BUSY)==0)
		{
			debugString("\nData tx started");
			//read data using rep  insw
                        for(int i=0;i<256;i++)
                            (reinterpret_cast<uint16_t*>(targetBuffer))[i]=readPortShort(DATA_REGISTER);//hard-coded value is here
		     
		        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
		        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
		        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay
		        readPortByte(ALTERNATE_STATUS_REGISTER);//100-uSec delay       

			status= readPortByte(STATUS_REGISTER);

			if(status & (STATUS_BUSY | STATUS_DEVICE_FAULT | STATUS_ERROR)) 
				return false;//there was an error
			else
				return true;//read operation was successful
		}else{
			return false;
		}
		

		

	}//read loop
	

	return -1;
}


/*
	bool readDataLBA28(
                int deviceNumber,
                void* targetBuffer,
                unsigned char command,
                unsigned char features,
                unsigned char sectorCount,
                unsigned char lba8to23,
                unsigned char lba24to27,
                unsigned char lba0to7);


*/

bool readDataLBA28(int deviceNumber,
                void* targetBuffer,
                unsigned char command,
                unsigned char features,
                /*unsigned char sectorCount,*/
                unsigned int lba28)
{

	return  executeDataInCommand(
                deviceNumber,
                targetBuffer,
                command,
                features,
                1,
                ((lba28 & 0x00FFFF00)>>8),
                ((lba28 & 0x0F000000)>>28),
                 (lba28  & 0x000000FF),
                true);	
}

bool readDataCHS(int deviceNumber,
                void* targetBuffer,
                unsigned char command,
                unsigned char features,
                unsigned char sectorCount,
                unsigned char cylinder,
                unsigned char head,
                unsigned char sectorNumber)
{
        return  executeDataInCommand(
                deviceNumber,
                targetBuffer,
                command,
                features,
                sectorCount,
                cylinder,
                head,
                sectorNumber,
                false);
}

