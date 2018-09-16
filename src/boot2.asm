; Second stage of the boot loader

BITS 16

ORG 9000h
	jmp 	Second_Stage

%include "functions_16.asm"
%include "bpb.asm"						; The BIOS Parameter Block (i.e. information about the disk format)
%include "floppy16.asm"					; Routines to access the floppy disk drive
%include "fat12.asm"					; Routines to handle the FAT12 file system
%include "a20.asm"						; Routines to handle the A20 line
%include "memory.asm"

; Where the kernel is to be loaded to in protected mode (we will be paging memory by the time
; we get to accessing this.  Virtual address 0xC0000000 (3GB) will map on to physical address 100000)

%define IMAGE_PMODE_BASE 	0C0000000h

; This is the real mode address where we will initially load the kernel
%define	IMAGE_RMODE_SEG		1000h
%define IMAGE_RMODE_OFFSET	0000h

; This is the above address once we are in 32-bit mode
%define IMAGE_PMODE_LOAD_ADDR 10000h

; kernel name (Must be a 8.3 filename and must be 11 bytes exactly)
ImageName     db "KERNEL  SYS"

; This is where we will store the size of the kernel image in sectors (updated just before jump to kernel to be kernel size in bytes)
ImageSize     			dd 0

BootInfo:
BootInfo_StackTop		dd 0	; Address of top of stack
BootInfo_KernelSize		dd 0	; Size of the kernel in bytes
BootInfo_MemoryMap		dd 0	; Address of memory map
BootInfo_BootDevice		db 0	; Boot device id

;	Start of the second stage of the boot loader
	
Second_Stage:
    mov		[boot_device], dl			; Boot device number is passed in from first stage in DL. Save it to pass to kernel later.
	mov 	si, second_stage_msg		; Output our greeting message
    call 	Console_WriteLine_16
	
	; Enable the A20 line if possible
	
	call	Enable_A20
	
	; Output message saying how A20 line was enabled
	
	push 	dx				
	mov		si, dx						; Calculate index into message table						
	add		si, dx
	mov		si, [si + a20_message_list] ; Get address of message by adding index to base of messages
	call	Console_WriteLine_16
	pop 	dx							; If we were not able to enable the A20 line, then we cannot switch to 
										; protected mode
	cmp		dx, 0
	je		Cannot_Continue
	
	; Store our memory map at 0:1000h (it will be read by the kernel later)
	
	mov		di, 1000h
	mov		dword [BootInfo_MemoryMap], 1000h 
	call	BIOSGetMemoryMap
	
; 	We are now going to load the kernel into memory
	
;	First, load the root directory table
	call	LoadRoot

;	Load kernel.sys.  We have to load it into conventional memory since the BIOS routines
;   can only access conventional memory.  We will relocate it to high memory later
    
	mov		ebx, IMAGE_RMODE_SEG	; BX:BP points to memory address to load the file to
    mov		bp, IMAGE_RMODE_OFFSET
	mov		si, ImageName			; The file to load
	call	LoadFile				; Load the file

	mov		dword [ImageSize], ecx	; Save size of kernel (in sectors)
	cmp		ax, 0					; Test for successful load
	je		Switch_To_Protected_Mode

	mov		si, msgFailure			; Unable to load kernel.sys - print error
	call	Console_Write_16

Cannot_Continue:	
	mov		si, wait_for_key_msg
	call	Console_WriteLine_16
	mov		ah, 0
	int     16h                    	; Wait for key press before continuing
	int     19h                     ; Warm boot computer
	hlt
	
; 	We are now ready to switch to 32-bit protected mode

Switch_To_Protected_Mode:
	lgdt 	[gdt_descriptor]		; Load the global descriptor table
	mov		eax, cr0				; Set the first bit of control register CR0 to switch 
									; into 32-bit protected mode
	or		eax, 1
	mov		cr0, eax
	
	jmp		CODE_SEG:Initialise_PM	; Do a far jump to the 32-bit code.  Doing a far jump 
									;clears the pipeline of any 16-bit instructions.

second_stage_msg  db 'Second stage of boot loader running', 0
a20_msg_one		  db 'A20 line already enabled', 0
a20_msg_two		  db 'A20 line enabled by BIOS', 0
a20_msg_three	  db 'A20 line enabled using keyboard controller', 0
a20_msg_four	  db 'A20 line enabled using Fast A20 gate', 0
no_a20_msg		  db 'Unable to enable A20 line.', 0	
wait_for_key_msg  db 'Press a key to continue', 0
msgFailure 	  	  db 'Unable to find KERNEL.SYS. ', 0

a20_message_list  dw no_a20_msg
				  dw a20_msg_one
				  dw a20_msg_two
				  dw a20_msg_three
				  dw a20_msg_four
boot_device		  db  0

									
; 32-bit code
									
BITS 32

Initialise_PM:						; Start of main 32-bit code
	
;   It is now vital that we make sure that interrupts are turned off until
;   our kernel is running and has setup the interrupt tables.
;   Now we are in protected mode, we have no way of handling interrupts
;   until those tables are setup, so any interrupt would cause a crash.

	cli			
	mov		eax, DATA_SEG			; Update the segment registers to point to our new data segment
	mov		ds, ax
	mov 	ss, ax
	mov		es, ax
	mov		fs, ax
	mov		gs, ax
	
	mov		ebp, 90000h				; Update our stack position so that it is at the top of free space
	mov		dword [BootInfo_StackTop], ebp
	mov		esp, ebp	

	call	EnablePaging			; Setup our temporary page tables and enable paging
	
;	Copy kernel from load address to high memory

  	mov		eax, dword [ImageSize]	; Calculate how many bytes we need to copy
  	movzx	ebx, word [bpbBytesPerSector]
  	mul		ebx
	mov		dword [BootInfo_KernelSize], eax  ; Store kernel size in bytes
  	mov		ebx, 4					; Now divide by 4 to calculate the number of dwords
  	div		ebx
   	cld								; Make sure direction flag is clear (i.e. ESI and EDI get incremented)
   	mov    	esi, IMAGE_PMODE_LOAD_ADDR	; ESI = Where we are copying from
   	mov		edi, IMAGE_PMODE_BASE	; EDI = Where we are copying to
	mov		ecx, eax				; ECX = Number of dwords to copy
   	rep	movsd                   	; Copy kernel to its protected mode address

	; Now we can execute our kernel

	mov		eax, BootInfo			; Load EAX with the address of the bootinfo structure 
	call	IMAGE_PMODE_BASE        

	
	
%include "gdt.asm"	
%include "paging.asm"
	
