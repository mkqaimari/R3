#include <x86/Console.h>
#include <x86/Thread.h>

static volatile Mutex consoleMutex=0;

static void write(unsigned char);


static char* videoMemory; 	
static int currentPosition; 	
static int screenWidthChar;
static int screenHeightChar;

static void newline();//forward declaration

static void scrollDown(int lines)
{
	
	short* videoWords= (short* ) videoMemory;

	int i;

	for(i=0;i<(screenWidthChar-1)*screenHeightChar;i++){
		videoWords[i]=videoWords[i+screenWidthChar*1];
	}

	for(i=0;i<screenWidthChar*1;i++){
		videoWords[(screenWidthChar-1)*screenHeightChar]=' ';
	}

	currentPosition-=screenWidthChar*1;

	return  ;
}


void initConsole()
{

	videoMemory= (char*) 0xB8000; //a physical not virtual address
	currentPosition=0;    //A logical pointer position


	screenWidthChar= 80;
	screenHeightChar= 25;

	return ;
}

void writeString(const char* string){

	enterCriticalSection(&consoleMutex);
	
	for(int i=0; string[i]!='\0' ;i++)
                write(string[i]);

	leaveCriticalSection(&consoleMutex);
}

void writeStringN(const char* string,int length){

	enterCriticalSection(&consoleMutex);
       
	 int i;
        for(i=0; i<length ;i++)
                write(string[i]);

	leaveCriticalSection(&consoleMutex);
}


static void write(const unsigned char value)
{
    /*
	if(value<0u){
                write('_');
                return ;
        }
    */
	char* targetAddress= videoMemory+ (currentPosition*2);


	if(value=='\n')
		newline();		
	else{
		*targetAddress= value;
		currentPosition+=1;
	}

	if(currentPosition==((screenWidthChar*screenHeightChar))){
		scrollDown(1);
		//currentPosition-=screenWidthChar;
	}

	return ;
}

void clearConsole()
{
	enterCriticalSection(&consoleMutex);

	int i;
	for(i=0;i<screenWidthChar*screenHeightChar;i+=2)
		videoMemory[i]=' ';	//clear char bits	
	
	leaveCriticalSection(&consoleMutex);

	return ;
}

static void newline()
{
//	enterCriticalSection(&consoleMutex);
	
	currentPosition+=(screenWidthChar-currentPosition%screenWidthChar);

//	leaveCriticalSection(&consoleMutex);

	return ;
}

void writeLong(long value){
/*
	if(value<0){
		writeString("Negative Value");
		return ;
	}
*
*/
	
	enterCriticalSection(&consoleMutex);

	if(value==0){
		write('0');
		leaveCriticalSection(&consoleMutex);

		return ;
	}	
	int isNegative=0;
	if(value<0){
		value= -value;//invert the sign
		isNegative=1;
	}		

	char buffer[20]={0};

        long result = 0;

	long modFactor=10;
	long divFactor=1;

	int index=0;

	while(value>=divFactor && index<static_cast<int>(sizeof(buffer))){

		result= (value%modFactor)/divFactor;
		
		divFactor*=10;
		modFactor*=10;
		
		buffer[index++]= (char)(result+48);		
	}

	if(isNegative){
		write('-');
	}
	
	index= sizeof(buffer)-1;
	for(;index>=0;index--){
		if(buffer[index]!='\0')
			write(buffer[index]);
	}
	leaveCriticalSection(&consoleMutex);	
	return ;
}
