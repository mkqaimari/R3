[BITS 32]


LDT_SELECTOR	EQU	0x04
global	loadLDT

loadLDT:

	MOV	AX,[ESP+LDT_SELECTOR] 
	lldt 	ax

	ret;



global	asm_memcpy


asm_memcpy:

	MOV	EDI,[ESP+0X04]	;TO ADDRESS
	MOV	ESI,[ESP+0X08]	;FROM ADDRESS

	MOV	ECX,[ESP+0X0C]	;LENGTH

	CLD	

	REP	MOVSB

	RET


GLOBAL	lockMutex


lockMutex:

	MOV	EAX,[ESP+0X04];VALUE TO STORE IN MUTEX--USUALLY 1 
	MOV	ebp,[ESP+0X08];ADDRESS OF MUTEX
	
	xchg [ebp],eax

	;if eax contains 0, you made it
	;else keep spinning

	ret	;remember that return value is in eax


GLOBAL	unlockMutex

unlockMutex:

	mov	ebp,[esp+0x04];
	mov	eax,0
	xchg 	[ebp], eax	

	ret


	global	enablePaging


enablePaging:

	mov	eax,cr0
	or	eax,0x80000000
	mov	cr0,eax

	ret

	global	disablePaging

disablePaging:

	mov	eax,cr0
	and	eax,0xEFFFFFFF
	mov	cr0,eax

	ret
	
;	void setPTDirectoryPointer(void* ptr);

	global setPTDirectoryPointer

setPTDirectoryPointer:

	mov	eax,cr3
	or	eax,[esp+4]
	mov	cr3,eax
	
	ret




