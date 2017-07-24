export GCC_OPTS=-nostdinc -Wall  -nostdlib -fno-builtin -fno-rtti -fno-exceptions -ffast-math -fomit-frame-pointer -I../../header

help:

	echo	"Welcome to R3 Kernel Makefile: ";
	echo	"make build: builds objects files out of .asm and .c code";
	echo	"make clean: removes all .o files";
	echo	"make link: invokes ld with an ld script file to make the kernel's binary image";

all:	all.target
	
 
all.target:  build link mountBootMedium  prepareBootMedium prepareBootImage umountBootMedium clean
	
all.host:   build link mountBootMedium  prepareBootMedium prepareBootImage umountBootMedium clean	

svn.update:
	svn update ../..

build:	


	nasm -f aout  start.asm -o start.o
	nasm -f aout  I386.asm -o I386_asm.o
	
	
	gcc   ${GCC_OPTS} -o Thread.o -c Thread.cpp
	gcc   ${GCC_OPTS} -o main.o -c  main.cpp
	gcc   ${GCC_OPTS} -o Console.o -c Console.cpp
	gcc   ${GCC_OPTS} -o GDT.o  -c GDT.cpp
	gcc   ${GCC_OPTS} -o IDT.o  -c IDT.cpp
	gcc   ${GCC_OPTS} -o I386.o -c  I386.cpp
	gcc   ${GCC_OPTS} -o IRQ.o  -c IRQ.cpp
	gcc   ${GCC_OPTS} -o Timer.o -c  Timer.cpp
	gcc   ${GCC_OPTS} -o IDE.o  -c IDE.cpp
	gcc   ${GCC_OPTS} -o FAT32.o  -c FAT32.cpp
	gcc   ${GCC_OPTS} -o DOSDisk.o  -c DOSDisk.cpp
	gcc   ${GCC_OPTS} -o UTF8.o  -c UTF8.cpp
	gcc   ${GCC_OPTS} -o UCS2.o  -c UCS2.cpp
	gcc   ${GCC_OPTS} -o UCS4.o  -c UCS4.cpp
	gcc   ${GCC_OPTS} -o Logger.o  -c Logger.cpp
	gcc   ${GCC_OPTS} -o MemoryManager.o  -c MemoryManager.cpp
	gcc   ${GCC_OPTS} -o LDT.o  -c LDT.cpp
	gcc   ${GCC_OPTS} -o Runtime.o  -c Runtime.cpp
	gcc   ${GCC_OPTS} -o ELFLoader.o -c ELFLoader.cpp
	gcc   ${GCC_OPTS} -o FS.o -c FS.cpp

clean:
	rm   *.o
link:
	ld   --check-sections -Map AddressMap.map -T LinkerScript.ld -o kernel.bin start.o Runtime.o Thread.o  main.o Console.o  GDT.o IDT.o I386.o IRQ.o Timer.o IDE.o I386_asm.o DOSDisk.o   FAT32.o UTF8.o UCS2.o UCS4.o Logger.o MemoryManager.o LDT.o ELFLoader.o FS.o

link.elf:
	ld	-Map AddressMap.elf.map -T LinkerScript.elf.ld -o kernel.elf *.o

prepareBootMedium:	
	sudo dd if=dev_kernel_grub.img of=/dev/fd0	

prepareBootImage:
	sudo rm   /media/fd0/kernel.bin
	sudo cp   kernel.bin /media/fd0

mountBootMedium:
	sudo mount -t vfat /dev/fd0 /media/fd0

umountBootMedium:

	sudo umount /dev/fd0
