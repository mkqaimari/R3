[BITS 32]

;Global constants
KERNEL_STACK_SIZE	EQU	64*1024	;A 64KB stack reserved for the kernel.
					;Yes, too much but who knows? 

KERNEL_DATA_SEGMENT_SELECTOR	EQU	0x10
KERNEL_CODE_SEGMENT_SELECTOR	EQU	0x08

GLOBAL start
GLOBAL flushGDT
GLOBAL loadIDT

GLOBAL isr0
GLOBAL isr1
GLOBAL isr2
GLOBAL isr3
GLOBAL isr4
GLOBAL isr5
GLOBAL isr6
GLOBAL isr7

GLOBAL isr8
GLOBAL isr9
GLOBAL isr10
GLOBAL isr11
GLOBAL isr12
GLOBAL isr13
GLOBAL isr14
GLOBAL isr15

GLOBAL isr16
GLOBAL isr17
GLOBAL isr18
GLOBAL isr19
GLOBAL isr20
GLOBAL isr21
GLOBAL isr22
GLOBAL isr23

GLOBAL isr24
GLOBAL isr25
GLOBAL isr26
GLOBAL isr27
GLOBAL isr28
GLOBAL isr29
GLOBAL isr30
GLOBAL isr31


GLOBAL irq0
GLOBAL irq1
GLOBAL irq2
GLOBAL irq3
GLOBAL irq4
GLOBAL irq5
GLOBAL irq6
GLOBAL irq7

GLOBAL irq8
GLOBAL irq9
GLOBAL irq10
GLOBAL irq11
GLOBAL irq12
GLOBAL irq13
GLOBAL irq14
GLOBAL irq15


start:					;start is the kernel's entry-point.
					;You will find "ENTRY(start)" in the kernel's 
					;linker script

	MOV	ESP, KERNEL_STACK
	JMP	REAL_START


					;The following section is fixed and required
                                        ;by GRUB

					;This part MUST be 4byte aligned, 
					;so we solve that issue using 'ALIGN 4'
ALIGN 4

mboot:
    ; Multiboot macros to make a few lines later more readable
    MULTIBOOT_PAGE_ALIGN	equ 1<<0
    MULTIBOOT_MEMORY_INFO	equ 1<<1
    MULTIBOOT_AOUT_KLUDGE	equ 1<<16
    MULTIBOOT_HEADER_MAGIC	equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS	equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
    
    MULTIBOOT_CHECKSUM	equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
    
    EXTERN code, bss, end

    ; This is the GRUB Multiboot header. A boot signature
    noname1	dd 	MULTIBOOT_HEADER_MAGIC
    noname2	dd 	MULTIBOOT_HEADER_FLAGS
    noname3	dd 	MULTIBOOT_CHECKSUM
    
    ; AOUT kludge - must be physical addresses. Make a note of these:
    ; The linker script fills in the data for these ones!
    
    GLOBAL	noname7, noname8


    noname4	dd 	mboot	;mboot, code, bss, end, start, are all linker
    noname5	dd 	code	;variables, caluclated during linking phase.
    noname6	dd 	bss
    noname7	dd 	end
    noname8	dd 	start


REAL_START:
	EXTERN	kmainCWrapper			;A symbol exported by another object 
					;code file

	PUSH	EBX			;this passes struct BootInfo* to kernel main	
	CALL	kmainCWrapper		
	JMP	REAL_START

;------------------------------------------------------------------------------
;                       Utility Procesdures
;------------------------------------------------------------------------------


flushGDT:
	EXTERN 	gdtPointer
	
	LGDT 	[gdtPointer]
	MOV	AX,KERNEL_DATA_SEGMENT_SELECTOR
	
	MOV	SS,AX
	MOV	DS,AX
	MOV	ES,AX
	MOV	GS,AX
	MOV	FS,AX

	JMP	KERNEL_CODE_SEGMENT_SELECTOR:RETURN_TO_C_CODE

RETURN_TO_C_CODE:

	RET	;AND SO YOU EMPTY THE STACK FROM A CALL FRAME

loadIDT:
	EXTERN 	idtPointer
	
	LIDT	[idtPointer]
	RET	

;------------------------------------------------------------------------------
;			Kernel's IRQs
;------------------------------------------------------------------------------

EXTERN irqHandlerCWrapper

irq0:		;First Controller
irq1: 
irq2: 
irq3: 
irq4: 
irq5: 
irq6: 
irq7: 

	CLI	
 	
	PUSH	DWORD	0	;ERROR CODE
	PUSH	DWORD	32	;INTERRUPT NUMBER

	JMP	commonIRQCode

irq8:		;Second Controller
irq9:          ;First Controller
irq10: 
irq11: 
irq12: 
irq13: 
irq14: 
irq15:
	CLI

	PUSH	DWORD	0
	PUSH	DWORD	47

        JMP     commonIRQCode


;------------------------------------------------------------------------------
;                       Kernel's ISRs
;------------------------------------------------------------------------------

EXTERN	exceptionHandlerCWrapper

  isr0:
  isr1:
  isr2:
  isr3:
  isr4:
  isr5:
  isr6:
  isr7:

  isr8:
  isr9:
  isr10:
  isr11:
  isr12:
  isr13:
  isr14:
  isr15:

  isr16:
  isr17:
  isr18:
  isr19:
  isr20:
  isr21:
  isr22:
  isr23:

  isr24:
  isr25:
  isr26:
  isr27:
  isr28:
  isr29:
  isr30:
  isr31:

	CLI		;clear Interrupt Flag.
	MOV  	EAX, 0H	;Just for consistency, if the processor
	PUSH	EAX			;doesn't send us an error code
					;we push 0 as an immediate 32-bits value
	
	MOV	EAX, 0H	;DIVIDE BY ZERO
	PUSH	EAX
	JMP	commonISRCode


commonISRCode:

	PUSHA
	
	PUSH	DS
	PUSH	ES
	PUSH	FS
	PUSH	GS

	MOV	AX,KERNEL_DATA_SEGMENT_SELECTOR
	MOV	DS,AX
	MOV	ES,AX
	MOV	FS,AX
	MOV	GS,AX

	MOV	EAX,ESP		;We're going to call a C function using C calling convintion,
				;Our synthesized ISRValues instance is on stack so just pass
				;a pointer to it
	PUSH	EAX		;The pointer is the only arg the exceptionHandler "C"
				;function sees, and other values on the stack
				;are accessible throw pointer indirection.
	MOV	EAX,exceptionHandlerCWrapper	
	CALL	EAX		;Add to EIP Without push

	POP	EAX		;CALLER MUST CLEAN THE STACK
	
				
	POP	GS		;RESTORE ORIGINAL SEGMENT REGISTERS
	POP	FS
	POP	ES
	POP	DS
	
	POPA			;RESTORE REGISTERS

	ADD	ESP,8

	IRET			;INTERRUPT HANDLER RETURN



commonIRQCode:

;interrupted code state starts here

        PUSHA

        PUSH    DS
        PUSH    ES
        PUSH    FS
        PUSH    GS

        MOV     AX,KERNEL_DATA_SEGMENT_SELECTOR
        MOV     DS,AX
        MOV     ES,AX
        MOV     FS,AX
        MOV     GS,AX

;interrupted code state ends here

        MOV     EAX,ESP         ;We're going to call a C function using C calling convintion,
                                ;Our synthesized ISRValues instance is on stack so just pass
                                ;a pointer to it
        PUSH    EAX             ;The pointer is the only arg the exceptionHandler "C"
                                ;function sees, and other values on the stack
                                ;are accessible throw pointer indirection.
        MOV     EAX,irqHandlerCWrapper
        CALL    EAX             ;Add to EIP Without push

	
	mov	edx,eax		;preserve return value

        POP     EAX             ;CALLER MUST CLEAN THE STACK

	cmp	edx,0xffffffff
	je	pop_state_off_stack


	mov	esp,edx		;use the new stack ptr	

extern debugIRQValuesCWrapper

push	edx

;call debugIRQValuesCWrapper

pop	edx

pop_state_off_stack:

        POP     GS              ;RESTORE ORIGINAL SEGMENT REGISTERS
        POP     FS
        POP     ES
        POP     DS

        POPA                    ;RESTORE REGISTERS
	ADD	ESP,8

        IRET                    ;INTERRUPT HANDLER RETURN



;------------------------------------------------------------------------------
;                       Kernel's Stack
;------------------------------------------------------------------------------

	global KERNEL_STACK

SECTION	.bss				;Uninitialized data section. (A.K.A .bss)
					;This section contains the  stack segment

	RESB	KERNEL_STACK_SIZE	;I guess RESB is only used in .bss section

KERNEL_STACK:
