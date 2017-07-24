#if !defined	IO_H
#define IO_H

#include <x86/Types.h>

class InputStream {

public:

	virtual	int	read(uint8_t byteArray[],int bufferLength)=0;
	virtual	void	close()=0;


};//InputStream

#endif
