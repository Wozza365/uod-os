# Create a bootable image file from our boot code

.DEFAULT_GOAL:=all

IMAGE 		= uodos

# If GCC 7.2 is available, uncomment the two lines below and comment the two lines following
#CC = gcc-7.2.0
#CFLAGS= -ffreestanding -m32 -I./include/ -mgeneral-regs-only 
CC = gcc
CFLAGS= -ffreestanding -m32 -mno-sse -I./include/
OBJS= kernel_main.o console.o print.o draw.o math.o string.o physicalmemorymanager.o virtualmemorymanager.o vm_pde.o vm_pte.o sysapi.o user.o keyboard.o vgamodes.o 
HAL_OBJS = hal/cpu.o hal/hal.o hal/idt.o hal/gdt.o hal/pic.o hal/pit.o hal/exception.o hal/tss.o

.SUFFIXES: .iso .img .bin .asm .sys .o .lib

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

kernelentry.o: kernelentry.asm
	nasm -w+all -f elf -o kernelentry.o kernelentry.asm

hal.lib: $(HAL_OBJS)
	ar -cvq hal.lib $(HAL_OBJS)
	
kernel.sys: kernelentry.o $(OBJS) hal.lib
	ld -o kernel.bin -T linkerscript.txt -mi386pe kernelentry.o $(OBJS) hal.lib
	objcopy -O binary kernel.bin kernel.sys

%.bin: %.asm
	nasm -w+all -f bin -o $@ $<

boot.bin: boot.asm bpb.asm floppy16.asm fat12.asm

boot2.bin: boot2.asm functions_16.asm bpb.asm floppy16.asm fat12.asm a20.asm gdt.asm memory.asm paging.asm

$(IMAGE).img : boot.bin boot2.bin kernel.sys
#	Get the blank floppy disk image
	cp floppy_image/uodos.img $(IMAGE).img
#	Copy our new boot sector over to the floppy image
	dd status=noxfer conv=notrunc if=boot.bin of=$(IMAGE).img
#	Mount floppy image file as Z:
	imdisk -a -t file -f $(IMAGE).img -o rem -m z:
#	Now copy files to z: (we do it this way to avoid problems with cygwin and drive specifiers)
	cmd /c "copy boot2.bin z:BOOT2.BIN"
	cmd /c "copy kernel.sys z:KERNEL.SYS"
#	Unmount the floppy disk image
	imdisk -D -m z:

all: $(IMAGE).img

clean:
	rm -f boot.bin
	rm -f boot2.bin
	rm -f hal/*.o
	rm -f *.o
	rm -f hal.lib
	rm -f kernel.bin
	rm -f kernel.sys
	rm -f $(IMAGE).img
	
	
